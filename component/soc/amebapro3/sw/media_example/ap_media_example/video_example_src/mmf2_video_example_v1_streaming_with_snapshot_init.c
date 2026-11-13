/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ameba_soc.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_video.h"
#include "module_rtsp2.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 -> H.264 -> RTSP
 *      -> JPEG on demand -> AP destination buffer (VAPP_SNAPSHOT [path])
 * Both outputs use the same V1 resolution.
 */

#if (!defined(CONFIG_LWIP_LAYER) || (CONFIG_LWIP_LAYER == 0))
#error "please enable CONFIG_LWIP_LAYER before using RTSP"
#endif

/* ---- User-configurable settings ---- */
#define STREAM_WIDTH  1920
#define STREAM_HEIGHT 1080
#define STREAM_FPS    30
#define STREAM_GOP    30
#define STREAM_BPS    2000000
#define SNAPSHOT_CAPACITY (STREAM_WIDTH * STREAM_HEIGHT * 6U / 5U) /* increase if the JPEG does not fit */
#define AINR_ENA    0   /* AI noise reduction on V1; set 1 to enable */

/* ---- Internal constants (do not modify) ---- */
#define STREAM_QUEUE_LEN 60

static mm_context_t *video_v1_ctx  = NULL;
static mm_context_t *rtsp_ctx      = NULL;
static mm_siso_t    *siso_v1_rtsp  = NULL;

static int snapshot_ready;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format    = VIDEO_H264_JPEG,
	.width     = STREAM_WIDTH,
	.height    = STREAM_HEIGHT,
	.fps       = STREAM_FPS,
	.gop       = STREAM_GOP,
	.bps       = STREAM_BPS,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
	.qlevel = 5,
	.ainr_en   = AINR_ENA,
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps      = STREAM_FPS,
			.bps      = 0, // if unknown
		}
	}
};

/* Start RTSP playback with an I-frame when a client sends PLAY. */
static int prv_rtsp_play_cb(void *p)
{
	(void)p;
	RTK_LOGI(TAG, "RTSP play: force I frame on ch%d\r\n", V1_STREAM_ID);
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	return 0;
}

void mmf2_video_example_v1_streaming_with_snapshot_init(void)
{
	if (video_v1_ctx) {
		RTK_LOGW(TAG, "V1 snapshot example already started\r\n");
		return;
	}
	/* V1 supplies both the H.264 live stream and on-demand JPEGs. */
	video_v1_ctx = mm_module_open(&video_module);
	if (!video_v1_ctx) {
		RTK_LOGE(TAG, "video v1 open failed\r\n");
		return;
	}
	if (mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params) < 0 ||
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, STREAM_QUEUE_LEN) < 0 ||
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0 ||
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0) < 0) {
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
		RTK_LOGE(TAG, "V1 setup failed\r\n");
		return;
	}

	rtsp_ctx = mm_module_open(&rtsp2_module);
	if (!rtsp_ctx) {
		goto error;
	}
	mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 0);
	mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v_params);
	mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
	mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);
	mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, ON);

	/* SISO carries only H.264 from V1 to RTSP. */
	siso_v1_rtsp = siso_create();
	if (!siso_v1_rtsp) {
		goto error;
	}
	siso_ctrl(siso_v1_rtsp, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
	siso_ctrl(siso_v1_rtsp, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
	if (siso_start(siso_v1_rtsp) < 0) {
		goto error;
	}
	snapshot_ready = 1;
	RTK_LOGI(TAG, "siso_v1_rtsp started\n\r");

	return;
error:
	RTK_LOGE(TAG, "V1 RTSP setup failed\r\n");
	if (siso_v1_rtsp) {
		siso_delete(siso_v1_rtsp);
		siso_v1_rtsp = NULL;
	}
	if (rtsp_ctx) {
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
		mm_module_close(rtsp_ctx);
		rtsp_ctx = NULL;
	}
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
	mm_module_close(video_v1_ctx);
	video_v1_ctx = NULL;
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1_streaming_with_snapshot_init[] = {
	{
		.v_func      = mmf2_video_example_v1_streaming_with_snapshot_init,
		.v_func_name = "mmf2_video_example_v1_streaming_with_snapshot_init",
	}
};

/* Optional path is on an already mounted filesystem. With no path the
 * command just captures and reports the encoded size. */
static u32 cmd_snapshot(u16 argc, u8 *argv[])
{
	if (!snapshot_ready || argc > 1) {
		RTK_LOGE(TAG, "Start mmf2_video_example_v1_streaming_with_snapshot_init using VIDEO list and VIDEO run <index>.\r\n"
				 "  VAPP_SNAPSHOT                 Capture one JPEG and print its size.\r\n"
				 "  VAPP_SNAPSHOT sd:/shot01.jpg  Save one JPEG to a new file on the mounted SD card.\r\n");
		return FALSE;
	}
	video_snapshot_t shot = {.dst = malloc(SNAPSHOT_CAPACITY), .capacity = SNAPSHOT_CAPACITY};
	if (!shot.dst) {
		RTK_LOGE(TAG, "Cannot allocate snapshot destination\r\n");
		return FALSE;
	}
	int ret = mm_module_ctrl(video_v1_ctx, CMD_VIDEO_GET_SNAPSHOT, (int)&shot);
	if (ret == VIDEO_SNAPSHOT_NOSPACE) {
		RTK_LOGE(TAG, "Snapshot needs %lu bytes\r\n", (unsigned long)shot.jpeg_len);
	} else if (ret != 0) {
		RTK_LOGE(TAG, "Snapshot failed (%d)\r\n", ret);
	} else {
		RTK_LOGI(TAG, "Captured %ux%u JPEG, %lu bytes\r\n", STREAM_WIDTH, STREAM_HEIGHT, (unsigned long)shot.jpeg_len);
		if (argc == 1) {
			FILE *file = fopen((const char *)argv[0], "wx");
			if (!file) {
				ret = -1;
			} else {
				size_t written = fwrite(shot.dst, 1, shot.jpeg_len, file);
				if (fclose(file) != 0 || written != shot.jpeg_len) {
					ret = -1;
				}
			}
			if (ret != 0) {
				RTK_LOGE(TAG, "Could not save JPEG to %s\r\n", argv[0]);
			}
		}
	}
	free(shot.dst);
	return ret == 0 ? TRUE : FALSE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_snapshot_cmd[] = {
	{"VAPP_SNAPSHOT", cmd_snapshot},
};
