/******************************************************************************
*
* Copyright(c) 2026 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <string.h>
#include "ameba_soc.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_video.h"
#include "module_uvcd.h"
#include "module_rtsp2.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_osd.h"
#include "video_osd_privacy.h"
#include "video_osd_stream.h"

static const char *TAG = "AP_VOSD_APP";

/*
 * Example topology:
 *   V1 H.264 video -> encoder OSD (privacy + logo + timestamp) -> UVC / RTSP
 *
 * Read mmf2_video_example_v1_osd_start() for the complete setup sequence.
 * Change the settings and polygon table below when adapting this example.
 * VIDEO entries select UVC/RTSP, move the second polygon, or stop the example.
 */

/* ---- User-configurable settings ---- */

/* V1 encoded output; the same settings are used by UVC and RTSP. */
#define STREAM_WIDTH             1280
#define STREAM_HEIGHT            720
#define STREAM_FPS               30
#define STREAM_GOP               30
#define STREAM_BPS               2000000

/* Privacy: opaque RGB fill; the update entry moves polygon 2 left/back. */
#define V1_PRIVACY_ENA           1
#define V1_PRIVACY_FILL_RGB      0x000000U
#define V1_PRIVACY_SHIFT_X       1500U /* Normalized units, not output pixels. */
#define V1_OSD_MEMORY_BUDGET     (8U * 1024U * 1024U) /* Limit, NOT preallocated; allow old/new regions during movement. */

/* Timestamp: bottom-right. System time synchronization is handled elsewhere. */
#define V1_TIMESTAMP_ENA         1
#define V1_TIMESTAMP_FORMAT      VIDEO_OSD_TIMESTAMP_FORMAT_YMD_DASH_24
#define V1_TIMESTAMP_TIME_ZONE   VIDEO_OSD_TIME_LOCAL
#define V1_TIMESTAMP_MAX_CHARS   19 /* Use at least 22 for 12-hour presets. */
#define V1_TIMESTAMP_FONT_SCALE  3 /* 1/2/3/5; select 5 for larger 40-pixel cells. */
#define V1_TIMESTAMP_MARGIN_X    16
#define V1_TIMESTAMP_MARGIN_Y    16

/* Logo: top-left. */
#define V1_LOGO_ENA              1
#define V1_LOGO_MARGIN_X         16
#define V1_LOGO_MARGIN_Y         16

/*
 * Customer polygon data:
 *   (0,0) = top-left; (10000,10000) = bottom-right at ANY stream resolution.
 *   List vertices around the perimeter; do not repeat the first vertex.
 *   Convex and concave polygons are allowed, but edges must not cross.
 *   id identifies the mask; enabled=0 hides only that mask.
 *
 * Both shapes use slanted edges. They also cross the logo/timestamp areas
 * so the demo shows those overlays remaining visible over an opaque mask.
 */
static const video_osd_privacy_polygon_t v1_privacy_polygons[] = {
	/* Polygon 1: seven-sided convex shape near the upper-left corner. */
	{
		.id = 1,
		.enabled = 1,
		.vertex_count = 7,
		.vertices = {
			{   0,    0},
			{1800,  150},
			{3400, 1100},
			{3900, 2800},
			{2700, 4100},
			{ 900, 3300},
			{   0, 1800},
		},
	},
	/* Polygon 2: ten-vertex concave star with five tips and diagonal notches. */
	{
		.id = 2,
		.enabled = 1,
		.vertex_count = 10,
		.vertices = {
			{7500, 3200},
			{8100, 5000},
			{9700, 5300},
			{8500, 6700},
			{9400, 9500},
			{7500, 8500},
			{6200, 9800},
			{6100, 7400},
			{4300, 6500},
			{6300, 5800},
		},
	},
};

/* ---- Internal constants (do not modify) ---- */
#define V1_SINK_UVC              0
#define V1_SINK_RTSP             1
#define OSD_APPLY_TIMEOUT_MS     5000
#define V1_PRIVACY_MOVE_INDEX    1

#if V1_LOGO_ENA
/* Customer logo, ARGB8888 (0xAARRGGBB), resized to half of realtek-logo.png. */
#include "logo_samples/realtek_logo_argb.h"
#define V1_LOGO_WIDTH  REALTEK_LOGO_ARGB_WIDTH
#define V1_LOGO_HEIGHT REALTEK_LOGO_ARGB_HEIGHT
#endif

static mm_context_t *video_v1_ctx   = NULL;
static mm_context_t *video_sink_ctx = NULL; /* uvcd_module or rtsp2_module */
static mm_siso_t *siso_v1_sink      = NULL;

static int v1_sink_mode = V1_SINK_UVC;
static int v1_privacy_moved = 0;
static video_osd_stream_t v1_osd_stream;
static video_osd_privacy_t v1_privacy_osd;
static video_osd_logo_t v1_logo_osd;
static video_osd_timestamp_t v1_timestamp_osd;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format    = VIDEO_H264,
	.width     = STREAM_WIDTH,
	.height    = STREAM_HEIGHT,
	.fps       = STREAM_FPS,
	.gop       = STREAM_GOP,
	.bps       = STREAM_BPS,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
};

static const video_osd_target_t v1_osd_target = {
	.stream_id    = V1_STREAM_ID,
	.codec        = VIDEO_H264,
	.frame_width  = STREAM_WIDTH,
	.frame_height = STREAM_HEIGHT,
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps      = STREAM_FPS,
			.bps      = STREAM_BPS,
		},
	},
};

/* OSD helpers ***************************************************************/
/* This is the application integration point: replace these vertices with the
 * complete scene decoded from the customer's HTTP/MQTT/P2P command. */
static int v1_privacy_apply(int move)
{
	video_osd_privacy_scene_t scene;
	video_osd_privacy_scene_default(&scene);
	scene.enabled = V1_PRIVACY_ENA;
	scene.fill_rgb = V1_PRIVACY_FILL_RGB;
	scene.polygon_count = sizeof(v1_privacy_polygons) / sizeof(v1_privacy_polygons[0]);
	if (scene.polygon_count > VIDEO_OSD_PRIVACY_MAX_POLYGONS) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	memcpy(scene.polygons, v1_privacy_polygons, sizeof(v1_privacy_polygons));
	/* A customer's APP would supply a complete replacement scene here.
	 * The demo instead moves the second polygon, leaving the first unchanged. */
	if (move && scene.polygon_count > V1_PRIVACY_MOVE_INDEX) {
		video_osd_privacy_polygon_t *polygon = &scene.polygons[V1_PRIVACY_MOVE_INDEX];
		if (polygon->vertex_count > VIDEO_OSD_PRIVACY_MAX_VERTICES) {
			return VIDEO_OSD_PRIVACY_EINVAL;
		}
		for (uint32_t i = 0; i < polygon->vertex_count; i++) {
			if (polygon->vertices[i].x < V1_PRIVACY_SHIFT_X) {
				return VIDEO_OSD_PRIVACY_EINVAL;
			}
			polygon->vertices[i].x -= V1_PRIVACY_SHIFT_X;
		}
	}
	uint32_t revision = 0;
	/* The shared sync API retries EBUSY and waits for this exact revision.
	 * Timeout is not cancellation: retain the context and query status/retry. */
	int result = video_osd_privacy_apply_sync(&v1_privacy_osd, &scene, &revision, OSD_APPLY_TIMEOUT_MS);
	if (result) {
		RTK_LOGE(TAG, "privacy update incomplete: %d (accepted revision=%lu)\r\n", result, (unsigned long)revision);
		return result;
	}
	RTK_LOGI(TAG, "privacy revision %lu applied (convex polygon + concave star)\r\n", (unsigned long)revision);
	return 0;
}

#if V1_PRIVACY_ENA
static int v1_privacy_start(void)
{
	video_osd_privacy_config_t config = {
		.target = v1_osd_target,
	};
	int result = video_osd_privacy_init(&v1_privacy_osd, &config);
	if (result != 0) {
		return result;
	}
	return v1_privacy_apply(0);
}
#endif

#if V1_LOGO_ENA
static int v1_logo_start(void)
{
	video_osd_logo_config_t config;
	video_osd_logo_config_default(&config);
	config.target = v1_osd_target;
	config.placement.position = VIDEO_OSD_POSITION_UPPER_LEFT;
	config.placement.margin_x = V1_LOGO_MARGIN_X;
	config.placement.margin_y = V1_LOGO_MARGIN_Y;
	config.argb8888 = realtek_logo_argb;
	config.width = V1_LOGO_WIDTH;
	config.height = V1_LOGO_HEIGHT;
	config.stride_bytes = V1_LOGO_WIDTH * sizeof(realtek_logo_argb[0]);
	return video_osd_logo_show_sync(&v1_logo_osd, &config, OSD_APPLY_TIMEOUT_MS);
}
#endif

#if V1_TIMESTAMP_ENA
static int v1_timestamp_start(void)
{
	video_osd_timestamp_config_t config;
	video_osd_timestamp_config_default(&config);
	config.target = v1_osd_target;
	config.placement.position = VIDEO_OSD_POSITION_LOWER_RIGHT;
	config.placement.margin_x = V1_TIMESTAMP_MARGIN_X;
	config.placement.margin_y = V1_TIMESTAMP_MARGIN_Y;
	config.format = V1_TIMESTAMP_FORMAT;
	config.time_zone = V1_TIMESTAMP_TIME_ZONE;
	config.max_chars = V1_TIMESTAMP_MAX_CHARS;
	config.font_scale = V1_TIMESTAMP_FONT_SCALE;
	return video_osd_timestamp_start_sync(&v1_timestamp_osd, &config, OSD_APPLY_TIMEOUT_MS);
}
#endif

static int prv_rtsp_play_cb(void *unused)
{
	(void)unused;
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	}
	return 0;
}

void mmf2_video_example_v1_osd_stop(void);

/* Start ********************************************************************/
static void mmf2_video_example_v1_osd_start(int sink_mode)
{
	if (video_v1_ctx) {
		RTK_LOGE(TAG, "stop the current V1 OSD example first\r\n");
		return;
	}
	v1_sink_mode = sink_mode;
	video_output_gate_t output_gate;

	/* V1 ******************************************************************/
	/* V1 is the user-facing H.264 stream. Block delivery BEFORE APPLY:
	 * the encoder must run to accept OSD, but startup frames are not masked. */
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		output_gate.blocked = 1;
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_OUTPUT_GATE, (int)&output_gate);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video V1 open failed\r\n");
		goto error;
	}

	/* Shared OSD manager **************************************************/
	/* Independent of privacy: logo-only, clock-only and NN-only users can use
	 * this same manager. No full-frame canvas is allocated at initialization. */
	video_osd_stream_config_t stream_osd_config = {
		.target = v1_osd_target,
		.memory_budget = V1_OSD_MEMORY_BUDGET,
	};
	if (video_osd_stream_init(&v1_osd_stream, &stream_osd_config) != 0) {
		RTK_LOGE(TAG, "V1 OSD manager init failed\r\n");
		goto error;
	}

	/* Privacy OSD *********************************************************/
#if V1_PRIVACY_ENA
	if (v1_privacy_start() != 0) {
		RTK_LOGE(TAG, "V1 privacy OSD start failed\r\n");
		goto error;
	}
#endif

	/* Logo OSD ************************************************************/
#if V1_LOGO_ENA
	/* Customer assets use uint32_t 0xAARRGGBB and an explicit row stride. */
	if (v1_logo_start() != 0) {
		RTK_LOGE(TAG, "V1 logo OSD start failed\r\n");
		goto error;
	}
#endif

	/* Timestamp OSD *******************************************************/
#if V1_TIMESTAMP_ENA
	if (v1_timestamp_start() != 0) {
		RTK_LOGE(TAG, "V1 timestamp OSD start failed\r\n");
		goto error;
	}
#endif
	/* Individual sync starts are complete. Obtain a common applied timestamp
	 * for the output gate, also covering any intervening clock/NN update. */
	video_osd_stream_status_t osd_status;
	if (video_osd_stream_wait_applied(&v1_osd_stream, 0, OSD_APPLY_TIMEOUT_MS, &osd_status) != 0) {
		goto error;
	}

	RTK_LOGI(TAG, "OSD memory: manager=%lu source=%lu peak=%lu bytes; bitmap=%lu ARGB=%lu indices=%lu\r\n",
			 (unsigned long)osd_status.allocated_bytes, (unsigned long)osd_status.source_bytes,
			 (unsigned long)osd_status.peak_bytes, (unsigned long)osd_status.bitmap_regions,
			 (unsigned long)osd_status.argb_regions, (unsigned long)osd_status.index_count);

	/* UVC / RTSP **********************************************************/
	/* Select exactly one consumer for the same encoded V1 output. */
	if (v1_sink_mode == V1_SINK_UVC) {
		video_sink_ctx = mm_module_open(&uvcd_module);
		if (!video_sink_ctx) {
			RTK_LOGE(TAG, "UVC module open failed\r\n");
			goto error;
		}
	} else {
		video_sink_ctx = mm_module_open(&rtsp2_module);
		if (video_sink_ctx) {
			mm_module_ctrl(video_sink_ctx, CMD_RTSP2_SELECT_STREAM, 0);
			mm_module_ctrl(video_sink_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v_params);
			/* Force an I-frame on PLAY so a new client starts from a reference frame. */
			mm_module_ctrl(video_sink_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
			mm_module_ctrl(video_sink_ctx, CMD_RTSP2_SET_APPLY, 0);
			mm_module_ctrl(video_sink_ctx, CMD_RTSP2_SET_STREAMMING, ON);
		} else {
			RTK_LOGE(TAG, "RTSP module open failed\r\n");
			goto error;
		}
	}

	/* SISO V1 -> selected sink *********************************************/
	/* One producer (V1 H.264) feeds one consumer (UVC or RTSP). */
	siso_v1_sink = siso_create();
	if (siso_v1_sink) {
		siso_ctrl(siso_v1_sink, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		siso_ctrl(siso_v1_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)video_sink_ctx, 0);
		siso_start(siso_v1_sink);
	} else {
		RTK_LOGE(TAG, "siso V1 sink create failed\r\n");
		goto error;
	}

	/* Allow masked output *************************************************/
	/* Drop any delayed IPC frame older than the confirmed OSD scene.
	 * Do not remove this gate when copying the example into an application. */
	output_gate.blocked = 0;
	output_gate.filter_timestamp = osd_status.applied_revision != 0;
	output_gate.minimum_timestamp = osd_status.applied_timestamp;
	if (mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_OUTPUT_GATE, (int)&output_gate) < 0) {
		goto error;
	}
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	RTK_LOGI(TAG, "V1 logo + timestamp + polygon privacy (%s) started\r\n",
			 v1_sink_mode == V1_SINK_UVC ? "UVC" : "RTSP");
	return;

error:
	RTK_LOGE(TAG, "V1 OSD startup failed\r\n");
	mmf2_video_example_v1_osd_stop();
}

void mmf2_video_example_v1_osd_uvc_init(void)
{
	mmf2_video_example_v1_osd_start(V1_SINK_UVC);
}

void mmf2_video_example_v1_osd_rtsp_init(void)
{
	mmf2_video_example_v1_osd_start(V1_SINK_RTSP);
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1_osd_init[] = {
	{
		.v_func      = mmf2_video_example_v1_osd_uvc_init,
		.v_func_name = "mmf2_video_example_v1_osd_uvc_init",
	},
	{
		.v_func      = mmf2_video_example_v1_osd_rtsp_init,
		.v_func_name = "mmf2_video_example_v1_osd_rtsp_init",
	}
};


void mmf2_video_example_v1_osd_update(void)
{
	if (!video_v1_ctx || !V1_PRIVACY_ENA) {
		RTK_LOGE(TAG, "start V1 OSD first\r\n");
		return;
	}
	if (!v1_privacy_apply(!v1_privacy_moved)) {
		v1_privacy_moved = !v1_privacy_moved;
	}
}

/* Stop *********************************************************************/
/* Reverse the startup order: stop delivery, remove OSD, then stop V1. */
void mmf2_video_example_v1_osd_stop(void)
{
	/* Close downstream delivery first. Keep V1 running until the last OSD
	 * release is acknowledged; retained contexts allow a later stop retry. */
	video_output_gate_t gate = {.blocked = 1};
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_OUTPUT_GATE, (int)&gate);
	}
	if (siso_v1_sink) {
		siso_stop(siso_v1_sink);
		siso_delete(siso_v1_sink);
		siso_v1_sink = NULL;
	}
	if (video_sink_ctx) {
		if (v1_sink_mode == V1_SINK_RTSP) {
			mm_module_ctrl(video_sink_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
		}
		mm_module_close(video_sink_ctx);
		video_sink_ctx = NULL;
	}
	/* Each sync removal returns 0 only after safe retirement. Already-removed
	 * clients also return 0, so the same stop entry can be retried after timeout.
	 * Keep the encoder running if ANY step fails; never inspect ctx.priv. */
	int result = video_osd_timestamp_stop_sync(&v1_timestamp_osd, OSD_APPLY_TIMEOUT_MS);
	if (!result) {
		result = video_osd_logo_hide_sync(&v1_logo_osd, OSD_APPLY_TIMEOUT_MS);
	}
	if (!result) {
		result = video_osd_privacy_deinit_sync(&v1_privacy_osd, OSD_APPLY_TIMEOUT_MS);
	}
	if (!result) {
		result = video_osd_stream_deinit_sync(&v1_osd_stream, OSD_APPLY_TIMEOUT_MS);
	}
	if (result) {
		RTK_LOGE(TAG, "OSD cleanup incomplete (%d); output blocked, retry the stop entry\r\n", result);
		return;
	}
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
	}
	RTK_LOGI(TAG, "V1 OSD stopped\r\n");
}

u32 CmdApVideoOSDTest(IN u16 argc, IN u8 *argv[])
{
	if (argc != 1 || !argv || !argv[0]) {
		RTK_LOGI(TAG, "Usage: VAPP_OSD update|stop\r\n");
		return _FALSE;
	}
	if (_strcmp((const char *)argv[0], "update") == 0) {
		RTK_LOGI(TAG, "AP update OSD test example\r\n");
		mmf2_video_example_v1_osd_update();
		// VAPP_OSD update
	} else if (_strcmp((const char *)argv[0], "stop") == 0) {
		RTK_LOGI(TAG, "AP stop OSD test example\r\n");
		mmf2_video_example_v1_osd_stop();
		// VAPP_OSD stop
	} else {
		RTK_LOGI(TAG, "Usage: VAPP_OSD update|stop\r\n");
		return _FALSE;
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   video_osd_test_cmd[] = {
	{
		"VAPP_OSD", CmdApVideoOSDTest
	},
};
