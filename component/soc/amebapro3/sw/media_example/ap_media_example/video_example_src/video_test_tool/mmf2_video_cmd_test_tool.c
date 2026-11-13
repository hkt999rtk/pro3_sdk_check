/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "ameba_soc.h"
#include "atcmd_service.h"
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_filesaver.h"
#include "module_mp4.h"
#include "vfs.h"
#include "module_video.h"
#include "module_vipnn.h"
#include "model_yolo.h"
#include "nn_coord_map/nn_coord_map.h"
#include "video_common_type.h"
#include "os_wrapper.h"

#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0) && !defined(CONFIG_BUILD_ISP_SRAM_TEST)
#define VIDEO_TEST_RTSP_ENABLED 1
#include "module_rtsp2.h"
#else
#define VIDEO_TEST_RTSP_ENABLED 0
#endif
#if defined(CONFIG_USBD_UVC) && (CONFIG_USBD_UVC != 0)
#define VIDEO_TEST_UVC_ENABLED 1
#include "module_uvcd.h"
#include "usbd.h" /* USB OS and device types required by usbd_uvc.h. */
#include "usbd_uvc.h"
#else
#define VIDEO_TEST_UVC_ENABLED 0
#endif

#if defined(CONFIG_WLAN) && (CONFIG_WHC_HOST || CONFIG_WHC_NONE || CONFIG_WHC_DEV_TCPIP_KEEPALIVE) && !defined(CONFIG_BUILD_ISP_SRAM_TEST)
#define VIDEO_TEST_HTTP_ENABLED 1
#include <httpd/httpd.h>
#include "lwip_netconf.h"
#else
#define VIDEO_TEST_HTTP_ENABLED 0
#endif

static const char *TAG = "VID_CMD_TEST";

/*
 * Example topology:
 *   V1 (CH0) video -> filesaver
 *   V2 (CH1) video -> SISO -> statistics/optional MP4 + optional RTSP or UVC
 *   V3 (CH2) video -> filesaver
 *   V4 (CH3) NV12 video -> filesaver
 *   V5 (CH4) RGB video -> SISO -> statistics + optional YOLOv4-tiny
 *
 * Configure every channel with AT+VID* commands before starting the test.
 * AT+VID? prints command usage, value descriptions, and examples.
 * AT+VIDCHEN selects which of the five channel pipelines will run.
 * AT+VIDINFO prints all channel settings and AT+VIDINFO=<ch> prints one.
 * AT+VIDNN=<0|1> selects optional fixed YOLOv4-tiny detection on V5.
 * AT+VIDOUT=<0=none|1=RTSP|2=UVC> selects the optional V2 preview sink.
 * AT+VIDREC=<0|1|2>,<seconds> selects no recording, V1, or V2 MP4 on SD.
 * After AT+VIDTEST=0, AT+VIDHTTP=1 serves the completed file at /video.
 * AT+VIDTEST=1 starts the selected paths and AT+VIDTEST=0 stops them.
 */

/* ---- Internal constants (do not modify) ---------------------------------- */

#define VIDEO_TEST_NN_WIDTH         416
#define VIDEO_TEST_NN_HEIGHT        416
#define VIDEO_TEST_NN_MAX_RESULTS   32

#define VIDEO_TEST_CHANNEL_COUNT    VIDEO_ISP_VALID_CH
#define VIDEO_TEST_QUEUE_LEN        60
#define VIDEO_TEST_MAX_DIMENSION    UINT16_MAX
#define VIDEO_TEST_MAX_FPS          UINT16_MAX
#define VIDEO_TEST_MAX_GOP          UINT16_MAX
#define VIDEO_TEST_MIN_QP           0
#define VIDEO_TEST_MAX_QP           51
#define VIDEO_TEST_MAX_QLEVEL       10
#define VIDEO_TEST_MIN_PROFILE      (-1)
#define VIDEO_TEST_HEVC_MAX_PROFILE 9
#define VIDEO_TEST_H264_MIN_PROFILE 9
#define VIDEO_TEST_H264_MAX_PROFILE 12
#define VIDEO_TEST_MAX_LEVEL        UINT16_MAX
#define VIDEO_TEST_MAX_TIER         1
#define VIDEO_TEST_MAX_CTB_RC       3
#define VIDEO_TEST_MAX_BLOCK_RC     2
#define VIDEO_TEST_MAX_BITRATE_WIN  300
#define VIDEO_TEST_MAX_MOVING_TOL   2000
#define VIDEO_TEST_MIN_MONITOR_FRM  10
#define VIDEO_TEST_MAX_MONITOR_FRM  120
#define VIDEO_TEST_MAX_RC_FPS_NUM   ((1U << 20) - 1U)
#define VIDEO_TEST_MAX_SAR          UINT16_MAX
#define VIDEO_TEST_HEVC_BLOCK_SIZE  64
#define VIDEO_TEST_H264_BLOCK_SIZE  16

enum video_at_error {
	VIDEO_AT_OK = 0,
	VIDEO_AT_ERR_PARAM = 1,
	VIDEO_AT_ERR_STATE = 2,
	VIDEO_AT_ERR_START = 3,
	VIDEO_AT_ERR_DATA = 4,
	VIDEO_AT_ERR_IO = 5,
};

enum video_test_state {
	VIDEO_TEST_STOPPED = 0,
	VIDEO_TEST_STARTING,
	VIDEO_TEST_RUNNING,
	VIDEO_TEST_STOPPING,
};

static mm_context_t *video_ctx[VIDEO_TEST_CHANNEL_COUNT];
static mm_context_t *filesaver_ctx[VIDEO_TEST_CHANNEL_COUNT];
static mm_siso_t *video_filesaver_siso[VIDEO_TEST_CHANNEL_COUNT];
static uint8_t video_applied[VIDEO_TEST_CHANNEL_COUNT];
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
static uint8_t video_channel_enabled[VIDEO_TEST_CHANNEL_COUNT] = {1, 0, 0, 0, 0};
#else
static uint8_t video_channel_enabled[VIDEO_TEST_CHANNEL_COUNT] = {1, 1, 1, 1, 1};
#endif
static volatile enum video_test_state video_test_state = VIDEO_TEST_STOPPED;
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
static int video_comparison_started;
#endif

/* V2 has one consumer task; preview shares each buffer with statistics/MP4. */
enum video_preview_mode {
	VIDEO_PREVIEW_NONE = 0,
	VIDEO_PREVIEW_RTSP,
	VIDEO_PREVIEW_UVC,
};
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
static uint32_t video_ainr_enabled;
#else
static uint32_t video_ainr_enabled = 1; /* Global across V1-V5. */
#endif
static uint32_t video_nn_enabled;
static mm_context_t *video_nn_ctx;
static nn_data_param_t video_nn_input; /* VIPNN retains this pointer until close. */
static uint32_t video_preview_mode;
static mm_context_t *video_preview_ctx;
#if VIDEO_TEST_UVC_ENABLED
static int video_uvc_host_state;
#endif

static const char *video_preview_name(void)
{
	return video_preview_mode == VIDEO_PREVIEW_RTSP ? "RTSP" :
		   video_preview_mode == VIDEO_PREVIEW_UVC ? "UVC" : "NONE";
}

static void video_preview_handle(void *input, void *output);

/* Only one recorder is active: 0=off, 1=V1, 2=V2. */
static uint32_t video_record_source;
static uint32_t video_record_seconds = 10;
static mm_context_t *video_mp4_ctx;
static mm_module_t video_test_sink_module;
static char video_record_path[64];
static char video_expected_path[80];
static unsigned int video_record_id;
static int video_record_ready;
static int video_http_owned;
#if VIDEO_TEST_HTTP_ENABLED
static int video_http_started;
static volatile int video_http_stopping;
#endif

static video_params_t video_params[VIDEO_TEST_CHANNEL_COUNT] = {
	[V1_STREAM_ID] = {
		.stream_id = V1_STREAM_ID,
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
		.format = VIDEO_H264,
		.width = 1920,
		.height = 1080,
#else
		.format = VIDEO_HEVC,
		.width = 2688,
		.height = 1520,
#endif
		.fps = 15,
		.gop = 15,
		.bps = 2000000,
		.rc_mode = ENC_VBR,
		.min_qp = 26,
		.max_qp = 48,
		.qlevel = 5,
	},
	[V2_STREAM_ID] = {
		.stream_id = V2_STREAM_ID,
		.format = VIDEO_H264,
		.width = 1280,
		.height = 720,
		.fps = 30,
		.gop = 30,
		.bps = 500000,
		.rc_mode = ENC_VBR,
		.min_qp = 26,
		.max_qp = 48,
		.qlevel = 5,
	},
	[V3_STREAM_ID] = {
		.stream_id = V3_STREAM_ID,
		.format = VIDEO_JPEG,
		.width = 1920,
		.height = 1080,
		.fps = 1,
		.gop = 1,
		.bps = 1000000,
		.rc_mode = ENC_VBR,
		.min_qp = 26,
		.max_qp = 48,
		.qlevel = 5,
	},
	[V4_STREAM_ID] = {
		.stream_id = V4_STREAM_ID,
		.format = VIDEO_NV12,
		.width = 640,
		.height = 480,
		.fps = 10,
	},
	[V5_STREAM_ID] = {
		.stream_id = V5_STREAM_ID,
		.format = VIDEO_RGB,
		.width = 416,
		.height = 416,
		.fps = 10,
	},
};

typedef struct video_test_stats_s {
	uint32_t buffers;
	uint32_t invalid;
	uint64_t bytes;
	TickType_t last_tick;
} video_test_stats_t;

static video_test_stats_t video_stats[VIDEO_TEST_CHANNEL_COUNT];

/* The sink consumes buffers only; it does not save files or decode bitstreams. */
static void video_count_buffer(int channel, uint32_t data_addr, uint32_t data_size)
{
	rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
	if (!data_addr || !data_size) {
		video_stats[channel].invalid++;
	} else {
		video_stats[channel].buffers++;
		video_stats[channel].bytes += data_size;
		video_stats[channel].last_tick = xTaskGetTickCount();
	}
	rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
}

/* filesaver has no user context argument, so each channel needs its own callback. */
#define VIDEO_TEST_HANDLER(ch) \
	static void filesaver_handler_##ch(char *name, uint32_t addr, uint32_t size) \
	{ \
		(void)name; \
		video_count_buffer(ch, addr, size); \
	}
VIDEO_TEST_HANDLER(0)
VIDEO_TEST_HANDLER(1)
VIDEO_TEST_HANDLER(2)
VIDEO_TEST_HANDLER(3)
VIDEO_TEST_HANDLER(4)
#undef VIDEO_TEST_HANDLER

static const type_handler_t filesaver_handlers[VIDEO_TEST_CHANNEL_COUNT] = {
	filesaver_handler_0, filesaver_handler_1, filesaver_handler_2, filesaver_handler_3, filesaver_handler_4
};

static void video_stats_snapshot(video_test_stats_t *stats)
{
	/* Protect 64-bit byte totals from tearing while the SISO tasks update them. */
	rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
	memcpy(stats, video_stats, sizeof(video_stats));
	rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
}

/* Count and mux the same queue item in one SISO task, preserving its timestamp.
 * A second SISO consuming the same ready queue would steal frames. */
static int video_test_sink_handle(void *p, void *input, void *output)
{
	filesaver_module.handle(p, input, output);
	if (video_mp4_ctx && video_record_source &&
		filesaver_ctx[video_record_source - 1] && p == filesaver_ctx[video_record_source - 1]->priv) {
		mm_queue_item_t *item = input;
		if (item->data_addr && item->size) {
			mp4_module.handle(video_mp4_ctx->priv, input, output);
		}
	}
	if (video_preview_ctx && video_test_state == VIDEO_TEST_RUNNING &&
		filesaver_ctx[V2_STREAM_ID] && p == filesaver_ctx[V2_STREAM_ID]->priv) {
		mm_queue_item_t *item = input;
		if (item->data_addr && item->size) {
			video_preview_handle(input, output);
		}
	}
	if (video_nn_ctx && video_test_state == VIDEO_TEST_RUNNING &&
		filesaver_ctx[V5_STREAM_ID] && p == filesaver_ctx[V5_STREAM_ID]->priv) {
		mm_queue_item_t *item = input;
		if (item->data_addr && item->size) {
			/* Synchronous inference keeps this frame owned until VIPNN returns. */
			vipnn_module.handle(video_nn_ctx->priv, input, NULL);
		}
	}
	return 0;
}

static int video_record_end_cb(void *arg)
{
	(void)arg;
	RTK_LOGI(TAG, "MP4 capture finished; AT+VIDTEST=0 then AT+VIDHTTP=1 to download\r\n");
	return 0;
}

static int video_record_error_cb(void *arg)
{
	(void)arg;
	RTK_LOGE(TAG, "MP4 capture failed; check SD space/write errors and stop the test\r\n");
	return 0;
}

static const char *video_rc_mode_name(uint32_t rc_mode);

/* Reserve both names exclusively. Existing/failed runs keep their numbers on SD.
 * Commands cannot change parameters while starting/running; snapshot before capture. */
static int video_record_save_expected(int channel)
{
	struct stat st;
	FILE *file;
	video_params_t *p = &video_params[channel];
	vpap_shared_enc_rc_ext_t *rc = &p->rc_ext;
	vpap_shared_enc_coding_t *coding = &p->coding;
	int failed = 0;
	char json_line[512];
	for (video_record_id = 1; video_record_id <= 999999; video_record_id++) {
		snprintf(video_record_path, sizeof(video_record_path), "sd:/video_test_v%u_%06u.mp4", (unsigned int)video_record_source, video_record_id);
		snprintf(video_expected_path, sizeof(video_expected_path), "sd:/video_test_v%u_%06u_params.expected.json", (unsigned int)video_record_source, video_record_id);
		if (stat(video_record_path, &st) == 0 || stat(video_expected_path, &st) == 0) {
			continue;
		}
		/* FAT VFS maps wx to FA_CREATE_NEW, so an existing file is never truncated. */
		file = fopen(video_record_path, "wx");
		if (!file) {
			return -1;
		}
		if (fclose(file) != 0) {
			return -1;
		}
		break;
	}
	if (video_record_id > 999999 || !(file = fopen(video_expected_path, "wx"))) {
		return -1;
	}
	/* VFS FILE objects support fwrite, not libc fprintf. */
#define JSON_WRITE(...) do { \
	int n = snprintf(json_line, sizeof(json_line), __VA_ARGS__); \
	if (n < 0 || (size_t)n >= sizeof(json_line) || fwrite(json_line, 1, (size_t)n, file) != (size_t)n) { failed = 1; } \
} while (0)
#define JSON_INT(key, value) JSON_WRITE(",\n  \"%s\": %d", key, (int)(value))
	JSON_WRITE("{\n  \"_note\": \"Requested settings captured before recording; not encoder readback.\",\n"
			   "  \"_record_id\": \"v%u_%06u\",\n  \"_video_file\": \"%s\",\n  \"format\": \"%s\"",
			   (unsigned int)video_record_source, video_record_id, strrchr(video_record_path, '/') + 1,
			   p->format == VIDEO_HEVC ? "HEVC" : "H264");
	JSON_INT("width", p->width);
	JSON_INT("height", p->height);
	JSON_INT("fps", p->fps);
	JSON_INT("gop", p->gop);
	JSON_INT("bps", (rc->flags & VPAP_ENC_RC_EXT_BPS) ? rc->bps : p->bps);
	JSON_WRITE(",\n  \"rc_mode\": \"%s\"", video_rc_mode_name((rc->flags & VPAP_ENC_RC_EXT_RCMODE) ? rc->rcMode : p->rc_mode));
	JSON_INT("min_qp", (rc->flags & VPAP_ENC_RC_EXT_QP_RANGE) ? rc->minQp : p->min_qp);
	JSON_INT("max_qp", (rc->flags & VPAP_ENC_RC_EXT_QP_RANGE) ? rc->maxQp : p->max_qp);
	JSON_INT("ainr", video_ainr_enabled);
	JSON_INT("record_seconds", video_record_seconds);
	JSON_INT("_rc_flags", rc->flags);
	JSON_INT("_coding_flags", coding->flags);
	JSON_WRITE(",\n  \"_enabled_channels\": [");
	for (int i = 0; i < VIDEO_TEST_CHANNEL_COUNT; i++) {
		JSON_WRITE("%s%u", i ? "," : "", (unsigned int)video_channel_enabled[i]);
	}
	JSON_WRITE("]");
	if (p->profile_set) {
		JSON_INT("profile", p->profile);
	}
	if (p->level_set) {
		JSON_INT("level", p->level);
	}
	if (p->tier_set) {
		JSON_INT("tier", p->tier);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_QP_HDR) {
		JSON_INT("qpHdr", rc->qpHdr);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_I_QP_RANGE) {
		JSON_INT("minIQp", rc->minIQp);
		JSON_INT("maxIQp", rc->maxIQp);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_INTRA_QP_DELTA) {
		JSON_INT("intraQpDelta", rc->intraQpDelta);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_FIXED_INTRA_QP) {
		JSON_INT("fixedIntraQp", rc->fixedIntraQp);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_PIC_RC) {
		JSON_INT("pictureRc", rc->pictureRc);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_CTB_RC) {
		JSON_INT("ctbRc", rc->ctbRc);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_BLOCK_RC_SIZE) {
		JSON_INT("blockRCSize", rc->blockRCSize);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_PIC_SKIP) {
		JSON_INT("pictureSkip", rc->pictureSkip);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_HRD) {
		JSON_INT("hrd", rc->hrd);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_CPB) {
		JSON_WRITE(",\n  \"hrdCpbSize\": %u", (unsigned int)rc->hrdCpbSize);
		JSON_INT("cpbMaxRate", rc->cpbMaxRate);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_BITRATE_WINDOW) {
		JSON_INT("bitrateWindow", rc->bitrateWindow);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_MOVING_BITRATE) {
		JSON_INT("tolMovingBitRate", rc->tolMovingBitRate);
		JSON_INT("monitorFrames", rc->monitorFrames);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_FILLER_DATA) {
		JSON_INT("fillerData", rc->fillerData);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_CRF) {
		JSON_INT("crf", rc->crf);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_VBR) {
		JSON_INT("vbr", rc->vbr);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_FRAME_RATE) {
		JSON_INT("frameRateNum", rc->frameRateNum);
		JSON_INT("frameRateDenom", rc->frameRateDenom);
	}
	if (coding->flags & VPAP_ENC_CODING_SLICE_SIZE) {
		JSON_INT("sliceSize", coding->sliceSize);
	}
	if (coding->flags & VPAP_ENC_CODING_SEI) {
		JSON_INT("seiMessages", coding->seiMessages);
	}
	if (coding->flags & VPAP_ENC_CODING_SEI_USER_DATA) {
		JSON_INT("seiUserData", coding->seiUserData);
	}
	if (coding->flags & VPAP_ENC_CODING_FULL_RANGE) {
		JSON_INT("full_range", coding->vuiVideoFullRange);
	}
	if (coding->flags & VPAP_ENC_CODING_DEBLOCK) {
		JSON_INT("disableDeblockingFilter", coding->disableDeblockingFilter);
		JSON_INT("tcOffset", coding->tcOffset);
		JSON_INT("betaOffset", coding->betaOffset);
	}
	if (coding->flags & VPAP_ENC_CODING_SAO) {
		JSON_INT("sao", coding->enableSao);
	}
	if (coding->flags & VPAP_ENC_CODING_CABAC) {
		JSON_INT("cabac", coding->enableCabac);
	}
	if (coding->flags & VPAP_ENC_CODING_CIR) {
		JSON_INT("cirStart", coding->cirStart);
		JSON_INT("cirInterval", coding->cirInterval);
	}
	if (coding->flags & VPAP_ENC_CODING_SAR) {
		JSON_WRITE(",\n  \"sar\": \"%u:%u\"", (unsigned int)coding->sampleAspectRatioWidth, (unsigned int)coding->sampleAspectRatioHeight);
	}
	if (coding->flags & VPAP_ENC_CODING_ROI) {
		JSON_WRITE(",\n  \"roi\": [");
		for (int i = 0; i < VPAP_ENC_MAX_ROI; i++) {
			vpap_shared_enc_roi_t *r = &coding->roi[i];
			JSON_WRITE("%s{\"enable\":%u,\"top\":%u,\"left\":%u,\"bottom\":%u,\"right\":%u,\"delta_qp\":%d,\"qp\":%d}",
					   i ? "," : "", (unsigned int)r->enable, (unsigned int)r->top, (unsigned int)r->left,
					   (unsigned int)r->bottom, (unsigned int)r->right, (int)r->deltaQp, (int)r->qp);
		}
		JSON_WRITE("]");
	}
	JSON_WRITE("\n}\n");
#undef JSON_INT
#undef JSON_WRITE
	if (ferror(file)) {
		failed = 1;
	}
	if (fclose(file) != 0) {
		failed = 1;
	}
	if (failed) {
		RTK_LOGE(TAG, "parameter snapshot write failed; recording cancelled\r\n");
		return -1;
	}
	RTK_LOGI(TAG, "Parameters: %s\r\n", video_expected_path);
	return 0;
}

static int video_record_prepare(void)
{
	mp4_params_t params = {0};
	int channel = (int)video_record_source - 1;
	int status = 0;

	if (!video_record_source) {
		return 0;
	}
	video_record_ready = 0;
	video_record_path[0] = '\0';
	video_mp4_ctx = mm_module_open(&mp4_module);
	if (!video_mp4_ctx) {
		return -1;
	}
	params.width = video_params[channel].width;
	params.height = video_params[channel].height;
	params.fps = video_params[channel].fps;
	/* This controls muxer keyframe-index allocation, not the encoder GOP.
	 * Reserve an entry per frame so forced I-frames are also indexed. */
	params.gop = 1;
	params.record_length = video_record_seconds;
	params.record_type = STORAGE_VIDEO;
	params.record_file_num = 1;
	params.fatfs_buf_size = 224 * 1024;
	params.use_self_file_name = 1;
	if (video_record_save_expected(channel) != 0) {
		return -1;
	}
	snprintf(params.record_file_name, sizeof(params.record_file_name), "%s", video_record_path);
	if (mm_module_ctrl(video_mp4_ctx, CMD_MP4_SET_PARAMS, (int)&params) != 0 ||
		mm_module_ctrl(video_mp4_ctx, CMD_MP4_LOOP_MODE, 0) != 0 ||
		mm_module_ctrl(video_mp4_ctx, CMD_MP4_SET_END_CB, (int)video_record_end_cb) != 0 ||
		mm_module_ctrl(video_mp4_ctx, CMD_MP4_SET_ERROR_CB, (int)video_record_error_cb) != 0 ||
		mm_module_ctrl(video_mp4_ctx, CMD_MP4_START, 1) != 0) {
		return -1;
	}
	mm_module_ctrl(video_mp4_ctx, CMD_MP4_GET_STATUS, (int)&status);
	RTK_LOGI(TAG, "MP4 V%u -> %s (%u nominal seconds; paired parameter snapshot saved)\r\n",
			 (unsigned int)video_record_source, video_record_path, (unsigned int)video_record_seconds);
	return status ? 0 : -1;
}

/* Call only after all SISO tasks have stopped, so muxer finalization cannot race
 * with mp4_handle(). Keep incomplete recordings unavailable to HTTP. */
static int video_record_close(int rollback)
{
	pmp4_context mux;
	int result = 0;

	if (!video_mp4_ctx) {
		return 0;
	}
	mux = ((mp4_ctx_t *)video_mp4_ctx->priv)->mp4_muxer;
	if (mp4_is_recording(mux)) {
		if (mux->root.video_len < 2) {
			/* The muxer timestamp table reads sample[n-2] when writing moov.
			 * Abort zero/one-frame clips through its cleanup path instead. */
			mux->storage_state = STORAGE_ERROR;
			mp4_muxer_handle(mux, NULL, 0, 0, 0);
		} else {
			mm_module_ctrl(video_mp4_ctx, CMD_MP4_STOP_IMMEDIATELY, 0);
		}
	}
	video_record_ready = !rollback && mux->status == WRITE_MP4_DONE && mux->root.video_len >= 2;
	if (!video_record_ready) {
		RTK_LOGE(TAG, "MP4 incomplete or failed; download is disabled\r\n");
		result = -1;
	} else {
		RTK_LOGI(TAG, "MP4 finalized: %s, samples=%u; use AT+VIDHTTP=1\r\n",
				 video_record_path, (unsigned int)mux->root.video_len);
	}
	mm_module_close(video_mp4_ctx);
	video_mp4_ctx = NULL;
	return result;
}

static const char *video_format_name(uint32_t format)
{
	switch (format) {
	case VIDEO_HEVC:
		return "HEVC";
	case VIDEO_H264:
		return "H264";
	case VIDEO_JPEG:
		return "JPEG";
	case VIDEO_NV12:
		return "NV12";
	case VIDEO_RGB:
		return "RGB";
	default:
		return "UNKNOWN";
	}
}

static const char *video_rc_mode_name(uint32_t rc_mode)
{
	switch (rc_mode) {
	case ENC_CVBR:
		return "CVBR";
	case ENC_CBR:
		return "CBR";
	case ENC_VBR:
		return "VBR";
	case ENC_ABR:
		return "ABR";
	case ENC_CRF:
		return "CRF";
	case ENC_CQP:
		return "CQP";
	default:
		return "UNKNOWN";
	}
}

static const char *video_stream_name(int channel)
{
	static const char *const stream_names[VIDEO_TEST_CHANNEL_COUNT] = {"V1", "V2", "V3", "V4", "V5"};

	if (channel < V1_STREAM_ID || channel > V5_STREAM_ID) {
		return "UNKNOWN";
	}
	return stream_names[channel];
}

static const char *video_test_state_name(void)
{
	switch (video_test_state) {
	case VIDEO_TEST_STOPPED:
		return "STOPPED";
	case VIDEO_TEST_STARTING:
		return "STARTING";
	case VIDEO_TEST_RUNNING:
		return "RUNNING";
	case VIDEO_TEST_STOPPING:
		return "STOPPING";
	default:
		return "UNKNOWN";
	}
}

static int video_channel_has_encoder(int channel)
{
	return channel >= V1_STREAM_ID && channel <= V3_STREAM_ID;
}

static int video_format_is_supported(int channel, uint32_t format)
{
	if (video_channel_has_encoder(channel)) {
		return format == VIDEO_HEVC || format == VIDEO_H264 || format == VIDEO_JPEG || format == VIDEO_NV12;
	}
	if (channel == V4_STREAM_ID) {
		return format == VIDEO_NV12;
	}
	if (channel == V5_STREAM_ID) {
		return format == VIDEO_RGB;
	}
	return 0;
}

static int video_channel_supports_advanced_encoder(int channel)
{
	return video_channel_has_encoder(channel) &&
		   (video_params[channel].format == VIDEO_HEVC || video_params[channel].format == VIDEO_H264);
}

static int video_format_uses_encoder(uint32_t format)
{
	return format == VIDEO_HEVC || format == VIDEO_H264 || format == VIDEO_JPEG;
}

static int video_profile_is_supported(uint32_t format, int32_t profile)
{
	if (profile == VIDEO_TEST_MIN_PROFILE) {
		return 1;
	}
	if (format == VIDEO_HEVC) {
		return profile >= 0 && profile <= VIDEO_TEST_HEVC_MAX_PROFILE;
	}
	if (format == VIDEO_H264) {
		return profile >= VIDEO_TEST_H264_MIN_PROFILE && profile <= VIDEO_TEST_H264_MAX_PROFILE;
	}
	return 0;
}

static int video_level_is_supported(uint32_t format, uint32_t level)
{
	if (level == VIDEO_TEST_MAX_LEVEL) {
		return 1;
	}

	if (format == VIDEO_HEVC) {
		switch (level) {
		case 30:
		case 60:
		case 63:
		case 90:
		case 93:
		case 120:
		case 123:
		case 150:
		case 153:
		case 156:
		case 180:
		case 183:
		case 186:
			return 1;
		default:
			return 0;
		}
	}

	if (format == VIDEO_H264) {
		switch (level) {
		case 10:
		case 11:
		case 12:
		case 13:
		case 20:
		case 21:
		case 22:
		case 30:
		case 31:
		case 32:
		case 40:
		case 41:
		case 42:
		case 50:
		case 51:
		case 52:
		case 60:
		case 61:
		case 62:
		case 99:
			return 1;
		default:
			return 0;
		}
	}

	return 0;
}

static int video_parse_u32(const char *text, uint32_t min, uint32_t max, uint32_t *value)
{
	char *end = NULL;
	unsigned long parsed;

	/* strtoul accepts a minus sign and wraps on the 32-bit target. */
	if (!text || text[0] < '0' || text[0] > '9' || !value) {
		return -1;
	}

	errno = 0;
	parsed = strtoul(text, &end, 0);
	if (errno == ERANGE || end == text || *end != '\0' || parsed < min || parsed > max) {
		return -1;
	}

	*value = (uint32_t)parsed;
	return 0;
}

static int video_parse_i32(const char *text, int32_t min, int32_t max, int32_t *value)
{
	char *end = NULL;
	long parsed;

	if (!text || !text[0] || !value) {
		return -1;
	}

	errno = 0;
	parsed = strtol(text, &end, 0);
	if (errno == ERANGE || end == text || *end != '\0' || parsed < min || parsed > max) {
		return -1;
	}

	*value = (int32_t)parsed;
	return 0;
}

static uint32_t video_encoder_block_size(int channel)
{
	return video_params[channel].format == VIDEO_HEVC ? VIDEO_TEST_HEVC_BLOCK_SIZE : VIDEO_TEST_H264_BLOCK_SIZE;
}

static uint32_t video_encoder_block_columns(int channel)
{
	uint32_t block_size = video_encoder_block_size(channel);

	return (video_params[channel].width + block_size - 1U) / block_size;
}

static uint32_t video_encoder_block_rows(int channel)
{
	uint32_t block_size = video_encoder_block_size(channel);

	return (video_params[channel].height + block_size - 1U) / block_size;
}

static int video_parse_channel(const char *text, int *channel)
{
	uint32_t parsed;

	if (video_parse_u32(text, V1_STREAM_ID, V5_STREAM_ID, &parsed) != 0) {
		return -1;
	}
	*channel = (int)parsed;
	return 0;
}

static int video_parse_format(const char *text, uint32_t *format)
{
	uint32_t parsed;

	if (!text || !format) {
		return -1;
	}

	if (_stricmp(text, "HEVC") == 0) {
		*format = VIDEO_HEVC;
	} else if (_stricmp(text, "H264") == 0) {
		*format = VIDEO_H264;
	} else if (_stricmp(text, "JPEG") == 0) {
		*format = VIDEO_JPEG;
	} else if (_stricmp(text, "NV12") == 0) {
		*format = VIDEO_NV12;
	} else if (_stricmp(text, "RGB") == 0) {
		*format = VIDEO_RGB;
	} else {
		if (video_parse_u32(text, 0, UINT32_MAX, &parsed) != 0) {
			return -1;
		}
		*format = parsed;
	}
	return 0;
}

static const char *video_channel_path_state(int channel)
{
	if (!video_channel_enabled[channel]) {
		return "DISABLED";
	}
	if (video_test_state == VIDEO_TEST_STOPPING) {
		return "STOPPING";
	}
	if (video_applied[channel] && video_filesaver_siso[channel] && filesaver_ctx[channel]) {
		return "ACTIVE";
	}
	if (video_ctx[channel] || filesaver_ctx[channel] || video_filesaver_siso[channel]) {
		return "STARTING";
	}
	if (video_test_state == VIDEO_TEST_RUNNING) {
		return "ERROR";
	}
	if (video_test_state == VIDEO_TEST_STARTING) {
		return "PENDING";
	}
	return "INACTIVE";
}

static int video_enabled_channel_count(void)
{
	int channel;
	int count = 0;

	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (video_channel_enabled[channel]) {
			count++;
		}
	}
	return count;
}

static void video_print_advanced_params(int channel)
{
	video_params_t *params = &video_params[channel];
	vpap_shared_enc_rc_ext_t *rc = &params->rc_ext;
	vpap_shared_enc_coding_t *coding = &params->coding;

	if (!params->profile_set && !params->level_set && !params->tier_set && rc->flags == 0 && coding->flags == 0) {
		RTK_LOGI(TAG, "   Advanced   : defaults (no explicit extras)\r\n");
		return;
	}

	RTK_LOGI(TAG, "   Extra flags: profile=%u level=%u tier=%u rc=0x%x coding=0x%x\r\n",
			 (unsigned int)params->profile_set, (unsigned int)params->level_set, (unsigned int)params->tier_set,
			 (unsigned int)rc->flags, (unsigned int)coding->flags);
	if (params->profile_set || params->level_set || params->tier_set) {
		RTK_LOGI(TAG, "   Profile    : profile=%d level=%u tier=%u\r\n",
				 (int)params->profile, (unsigned int)params->level, (unsigned int)params->tier);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_RCMODE) {
		RTK_LOGI(TAG, "   RC mode ext: %u\r\n", (unsigned int)rc->rcMode);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_BPS) {
		RTK_LOGI(TAG, "   BPS ext    : %u bps\r\n", (unsigned int)rc->bps);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_QP_HDR) {
		RTK_LOGI(TAG, "   QP header  : %d\r\n", (int)rc->qpHdr);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_QP_RANGE) {
		RTK_LOGI(TAG, "   QP ext     : %u-%u\r\n", (unsigned int)rc->minQp, (unsigned int)rc->maxQp);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_I_QP_RANGE) {
		RTK_LOGI(TAG, "   I-QP range : %u-%u\r\n", (unsigned int)rc->minIQp, (unsigned int)rc->maxIQp);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_INTRA_QP_DELTA) {
		RTK_LOGI(TAG, "   I-QP delta : %d\r\n", (int)rc->intraQpDelta);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_FIXED_INTRA_QP) {
		RTK_LOGI(TAG, "   Fixed I-QP : %u\r\n", (unsigned int)rc->fixedIntraQp);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_PIC_RC) {
		RTK_LOGI(TAG, "   Picture RC : %s\r\n", rc->pictureRc ? "ON" : "OFF");
	}
	if (rc->flags & VPAP_ENC_RC_EXT_CTB_RC) {
		RTK_LOGI(TAG, "   CTB RC     : %u\r\n", (unsigned int)rc->ctbRc);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_BLOCK_RC_SIZE) {
		RTK_LOGI(TAG, "   RC block   : %u (0=64,1=32,2=16)\r\n", (unsigned int)rc->blockRCSize);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_PIC_SKIP) {
		RTK_LOGI(TAG, "   Pic skip   : %s\r\n", rc->pictureSkip ? "ON" : "OFF");
	}
	if (rc->flags & VPAP_ENC_RC_EXT_HRD) {
		RTK_LOGI(TAG, "   HRD        : %s\r\n", rc->hrd ? "ON" : "OFF");
	}
	if (rc->flags & VPAP_ENC_RC_EXT_CPB) {
		RTK_LOGI(TAG, "   CPB        : size=%u max_rate=%u\r\n", (unsigned int)rc->hrdCpbSize, (unsigned int)rc->cpbMaxRate);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_BITRATE_WINDOW) {
		RTK_LOGI(TAG, "   BPS window : %u frames\r\n", (unsigned int)rc->bitrateWindow);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_MOVING_BITRATE) {
		RTK_LOGI(TAG, "   Moving BPS : tolerance=%d%% monitor=%d frames\r\n", (int)rc->tolMovingBitRate, (int)rc->monitorFrames);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_FILLER_DATA) {
		RTK_LOGI(TAG, "   Filler data: %s\r\n", rc->fillerData ? "ON" : "OFF");
	}
	if (rc->flags & VPAP_ENC_RC_EXT_CRF) {
		RTK_LOGI(TAG, "   CRF        : %d\r\n", (int)rc->crf);
	}
	if (rc->flags & VPAP_ENC_RC_EXT_VBR) {
		RTK_LOGI(TAG, "   VBR ext    : %s\r\n", rc->vbr ? "ON" : "OFF");
	}
	if (rc->flags & VPAP_ENC_RC_EXT_FRAME_RATE) {
		RTK_LOGI(TAG, "   RC FPS     : %u/%u\r\n", (unsigned int)rc->frameRateNum, (unsigned int)rc->frameRateDenom);
	}
	if (coding->flags & VPAP_ENC_CODING_SLICE_SIZE) {
		RTK_LOGI(TAG, "   Slice rows : %u\r\n", (unsigned int)coding->sliceSize);
	}
	if (coding->flags & VPAP_ENC_CODING_SEI) {
		RTK_LOGI(TAG, "   Driver SEI : %s\r\n", coding->seiMessages ? "ON" : "OFF");
	}
	if (coding->flags & VPAP_ENC_CODING_SEI_USER_DATA) {
		RTK_LOGI(TAG, "   User SEI   : %s\r\n", coding->seiUserData ? "ON" : "OFF");
	}
	if (coding->flags & VPAP_ENC_CODING_FULL_RANGE) {
		RTK_LOGI(TAG, "   Full range : %s\r\n", coding->vuiVideoFullRange ? "ON" : "OFF");
	}
	if (coding->flags & VPAP_ENC_CODING_DEBLOCK) {
		RTK_LOGI(TAG, "   Deblock    : disable=%u tc=%d beta=%d\r\n",
				 (unsigned int)coding->disableDeblockingFilter, (int)coding->tcOffset, (int)coding->betaOffset);
	}
	if (coding->flags & VPAP_ENC_CODING_SAO) {
		RTK_LOGI(TAG, "   HEVC SAO   : %s\r\n", coding->enableSao ? "ON" : "OFF");
	}
	if (coding->flags & VPAP_ENC_CODING_CABAC) {
		RTK_LOGI(TAG, "   H264 CABAC : %s\r\n", coding->enableCabac ? "ON" : "OFF");
	}
	if (coding->flags & VPAP_ENC_CODING_SAR) {
		RTK_LOGI(TAG, "   SAR        : %u:%u\r\n",
				 (unsigned int)coding->sampleAspectRatioWidth, (unsigned int)coding->sampleAspectRatioHeight);
	}
	if (coding->flags & VPAP_ENC_CODING_CIR) {
		RTK_LOGI(TAG, "   CIR        : start=%u interval=%u\r\n", (unsigned int)coding->cirStart, (unsigned int)coding->cirInterval);
	}
	if (coding->flags & VPAP_ENC_CODING_ROI) {
		int roi_index;

		for (roi_index = 0; roi_index < VPAP_ENC_MAX_ROI; roi_index++) {
			vpap_shared_enc_roi_t *roi = &coding->roi[roi_index];

			RTK_LOGI(TAG, "   ROI%d       : en=%u t=%u l=%u b=%u r=%u dqp=%d qp=%d\r\n",
					 roi_index, (unsigned int)roi->enable, (unsigned int)roi->top, (unsigned int)roi->left,
					 (unsigned int)roi->bottom, (unsigned int)roi->right, (int)roi->deltaQp, (int)roi->qp);
		}
	}
}

static void video_print_all_params(void)
{
	int channel;

	RTK_LOGI(TAG, "========================== VIDEO TEST CONFIGURATION ==========================\r\n");
	RTK_LOGI(TAG, " Test state : %s\r\n", video_test_state_name());
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
	RTK_LOGI(TAG, " ISP handoff: %s (AT+VIDPIPE)\r\n", video_params[0].isp_slice_en ? "SRAM SLICE" : "FRAME");
#endif
	RTK_LOGI(TAG, " Selected   : %d/%d channels\r\n", video_enabled_channel_count(), VIDEO_TEST_CHANNEL_COUNT);
	RTK_LOGI(TAG, " AINR       : %s\r\n", video_ainr_enabled ? "ON" : "OFF");
	RTK_LOGI(TAG, " MP4        : source=%u (0=off,1=V1,2=V2), seconds=%u, ready=%d, http=%d\r\n",
			 (unsigned int)video_record_source, (unsigned int)video_record_seconds, video_record_ready, video_http_owned);
	RTK_LOGI(TAG, " Queue      : %d items per channel\r\n", VIDEO_TEST_QUEUE_LEN);
	RTK_LOGI(TAG, " V5 NN      : %s (YOLOv4-tiny)\r\n", video_nn_enabled ? "ON" : "OFF");
	RTK_LOGI(TAG, " Pipeline   : video_module -> SISO -> statistics/MP4; V2 preview=%s\r\n", video_preview_name());
	RTK_LOGI(TAG, "------------------------------------------------------------------------------\r\n");
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		video_params_t *params = &video_params[channel];

		RTK_LOGI(TAG, " [CH%d / %s]\r\n", channel, video_stream_name(channel));
		RTK_LOGI(TAG, "   Enabled    : %s\r\n", video_channel_enabled[channel] ? "YES" : "NO");
		RTK_LOGI(TAG, "   Format     : %s\r\n", video_format_name(params->format));
		RTK_LOGI(TAG, "   Resolution : %u x %u\r\n", (unsigned int)params->width, (unsigned int)params->height);
		RTK_LOGI(TAG, "   Frame rate : %u fps\r\n", (unsigned int)params->fps);
		if (video_format_uses_encoder(params->format)) {
			RTK_LOGI(TAG, "   GOP        : %u frames\r\n", (unsigned int)params->gop);
			RTK_LOGI(TAG, "   Bitrate    : %u bps\r\n", (unsigned int)params->bps);
			RTK_LOGI(TAG, "   RC / QP    : %s / %u-%u\r\n", video_rc_mode_name(params->rc_mode),
					 (unsigned int)params->min_qp, (unsigned int)params->max_qp);
			RTK_LOGI(TAG, "   JPEG QLvl  : %u\r\n", (unsigned int)params->qlevel);
		} else if (video_channel_has_encoder(channel)) {
			RTK_LOGI(TAG, "   Encoder    : not active for current raw format\r\n");
		} else {
			RTK_LOGI(TAG, "   Encoder    : N/A (raw video channel)\r\n");
		}
		if (video_channel_has_encoder(channel)) {
			video_print_advanced_params(channel);
		}
		RTK_LOGI(TAG, "   Path state : %s\r\n", video_channel_path_state(channel));
		RTK_LOGI(TAG, "------------------------------------------------------------------------------\r\n");
	}
	RTK_LOGI(TAG, " Format support: CH0-2=HEVC/H264/JPEG/NV12, CH3=NV12 only, CH4=RGB only\r\n");
	RTK_LOGI(TAG, "==============================================================================\r\n");
}

static void video_print_channel_params(int channel)
{
	video_params_t *params = &video_params[channel];

	RTK_LOGI(TAG, "========================== VIDEO CHANNEL %d (%s) ==========================\r\n", channel, video_stream_name(channel));
	RTK_LOGI(TAG, " Test state       : %s\r\n", video_test_state_name());
	RTK_LOGI(TAG, " Enabled          : %s\r\n", video_channel_enabled[channel] ? "YES" : "NO");
	RTK_LOGI(TAG, " Path state       : %s\r\n", video_channel_path_state(channel));
	RTK_LOGI(TAG, " Pipeline         : video_module -> SISO -> statistics/MP4; preview=%s\r\n",
			 channel == V2_STREAM_ID ? video_preview_name() : "NONE");
	RTK_LOGI(TAG, " Format           : %s (%u)\r\n", video_format_name(params->format), (unsigned int)params->format);
	RTK_LOGI(TAG, " Resolution       : %u x %u\r\n", (unsigned int)params->width, (unsigned int)params->height);
	RTK_LOGI(TAG, " Frame rate       : %u fps\r\n", (unsigned int)params->fps);
	if (video_format_uses_encoder(params->format)) {
		RTK_LOGI(TAG, " Encoder settings : enabled for CH0-2\r\n");
		RTK_LOGI(TAG, " GOP              : %u frames\r\n", (unsigned int)params->gop);
		RTK_LOGI(TAG, " Bitrate          : %u bps\r\n", (unsigned int)params->bps);
		RTK_LOGI(TAG, " RC mode          : %s (%u)\r\n", video_rc_mode_name(params->rc_mode), (unsigned int)params->rc_mode);
		RTK_LOGI(TAG, " QP range         : %u - %u\r\n", (unsigned int)params->min_qp, (unsigned int)params->max_qp);
		RTK_LOGI(TAG, " JPEG qlevel      : %u\r\n", (unsigned int)params->qlevel);
	} else if (video_channel_has_encoder(channel)) {
		RTK_LOGI(TAG, " Encoder settings : not active for current raw format\r\n");
	} else {
		RTK_LOGI(TAG, " Encoder settings : not applicable to this raw channel\r\n");
	}
	if (video_channel_has_encoder(channel)) {
		video_print_advanced_params(channel);
	}
	RTK_LOGI(TAG, " AINR (global)    : %s\r\n", video_ainr_enabled ? "ON" : "OFF");
	RTK_LOGI(TAG, "=========================================================================\r\n");
}

static int video_require_stopped(void)
{
	if (video_test_state != VIDEO_TEST_STOPPED) {
		RTK_LOGI(TAG, "video test state is %s; stop it before changing parameters\r\n", video_test_state_name());
		return -1;
	}
	return 0;
}

static int video_prepare_advanced_command(u16 argc, u16 expected_argc, char **argv, int *channel)
{
	if (argc != expected_argc || video_require_stopped() != 0 || video_parse_channel(argv[1], channel) != 0 ||
		!video_channel_supports_advanced_encoder(*channel)) {
		return -1;
	}
	return 0;
}

static int video_validate_dimensions(uint32_t format, uint32_t width, uint32_t height)
{
	/* All supported formats except RGB use an NV12 ISP source. */
	if (format != VIDEO_RGB && ((width | height) & 1U)) {
		RTK_LOGE(TAG, "NV12 source width and height must be even\r\n");
		return -1;
	}
	/* Reject arithmetic overflow before downstream frame-buffer sizing. Actual
	 * sensor/channel limits are checked by the ISP, not by these storage limits. */
	if (!width || !height || width > VIDEO_TEST_MAX_DIMENSION || height > VIDEO_TEST_MAX_DIMENSION ||
		(uint64_t)width * height * 3U > INT_MAX) {
		RTK_LOGE(TAG, "invalid or overflowing frame dimensions\r\n");
		return -1;
	}
	return 0;
}

static int video_validate_advanced_params(int channel)
{
	video_params_t *params = &video_params[channel];
	vpap_shared_enc_rc_ext_t *rc = &params->rc_ext;
	vpap_shared_enc_coding_t *coding = &params->coding;
	uint32_t block_columns;
	uint32_t block_rows;
	uint32_t block_count;

	if (!params->profile_set && !params->level_set && !params->tier_set && rc->flags == 0 && coding->flags == 0) {
		return 0;
	}
	if (!video_channel_supports_advanced_encoder(channel)) {
		RTK_LOGE(TAG, "CH%d has advanced encoder settings but format %s is not H264/HEVC; use AT+VIDXCLR or change format\r\n",
				 channel, video_format_name(params->format));
		return -1;
	}
	if (params->profile_set && !video_profile_is_supported(params->format, params->profile)) {
		RTK_LOGE(TAG, "CH%d profile %d is invalid for %s\r\n", channel, (int)params->profile, video_format_name(params->format));
		return -1;
	}
	if (params->level_set && !video_level_is_supported(params->format, params->level)) {
		RTK_LOGE(TAG, "CH%d level %u is invalid for %s\r\n", channel, (unsigned int)params->level, video_format_name(params->format));
		return -1;
	}
	if (params->tier_set && (params->tier > VIDEO_TEST_MAX_TIER || (params->format == VIDEO_H264 && params->tier != 0))) {
		RTK_LOGE(TAG, "CH%d tier %u is invalid for %s\r\n", channel, (unsigned int)params->tier, video_format_name(params->format));
		return -1;
	}
	if ((rc->flags & VPAP_ENC_RC_EXT_HRD) && rc->hrd && (rc->flags & VPAP_ENC_RC_EXT_VBR) && rc->vbr) {
		RTK_LOGE(TAG, "CH%d HRD and VBR extras cannot both be enabled\r\n", channel);
		return -1;
	}
	if ((rc->flags & VPAP_ENC_RC_EXT_FILLER_DATA) && rc->fillerData &&
		(!(rc->flags & VPAP_ENC_RC_EXT_CPB) || rc->hrdCpbSize == 0 ||
		 !(rc->flags & VPAP_ENC_RC_EXT_PIC_RC) || rc->pictureRc == 0)) {
		RTK_LOGE(TAG, "CH%d filler data requires nonzero CPB size and picture RC enabled\r\n", channel);
		return -1;
	}
	if ((rc->flags & VPAP_ENC_RC_EXT_CRF) && rc->crf >= 0 &&
		(rc->flags & VPAP_ENC_RC_EXT_CTB_RC) && rc->ctbRc >= 2) {
		RTK_LOGE(TAG, "CH%d CRF cannot be used with CTB RC mode %u\r\n", channel, (unsigned int)rc->ctbRc);
		return -1;
	}
	if ((rc->flags & VPAP_ENC_RC_EXT_FRAME_RATE) &&
		(rc->frameRateNum == 0 || rc->frameRateNum > VIDEO_TEST_MAX_RC_FPS_NUM || rc->frameRateDenom == 0)) {
		RTK_LOGE(TAG, "CH%d RC frame rate must have a valid nonzero numerator and denominator\r\n", channel);
		return -1;
	}
	if ((coding->flags & VPAP_ENC_CODING_SAO) && params->format != VIDEO_HEVC) {
		RTK_LOGE(TAG, "CH%d SAO is supported only with HEVC\r\n", channel);
		return -1;
	}
	if ((coding->flags & VPAP_ENC_CODING_CABAC) && params->format != VIDEO_H264) {
		RTK_LOGE(TAG, "CH%d CABAC is supported only with H264\r\n", channel);
		return -1;
	}
	if ((coding->flags & VPAP_ENC_CODING_CABAC) && coding->enableCabac && params->profile_set &&
		params->profile == VIDEO_TEST_H264_MIN_PROFILE) {
		RTK_LOGE(TAG, "CH%d H264 baseline profile cannot enable CABAC\r\n", channel);
		return -1;
	}
	if ((coding->flags & VPAP_ENC_CODING_SAR) &&
		((coding->sampleAspectRatioWidth == 0) != (coding->sampleAspectRatioHeight == 0))) {
		RTK_LOGE(TAG, "CH%d SAR width and height must both be zero or both be nonzero\r\n", channel);
		return -1;
	}

	block_columns = video_encoder_block_columns(channel);
	block_rows = video_encoder_block_rows(channel);
	block_count = block_columns * block_rows;
	if ((coding->flags & VPAP_ENC_CODING_SLICE_SIZE) && coding->sliceSize > block_rows) {
		RTK_LOGE(TAG, "CH%d slice size %u exceeds the current %u block rows\r\n",
				 channel, (unsigned int)coding->sliceSize, (unsigned int)block_rows);
		return -1;
	}
	if ((coding->flags & VPAP_ENC_CODING_CIR) &&
		(coding->cirStart > block_count || coding->cirInterval > block_count)) {
		RTK_LOGE(TAG, "CH%d CIR values exceed the current %u encoder blocks\r\n", channel, (unsigned int)block_count);
		return -1;
	}
	if (coding->flags & VPAP_ENC_CODING_ROI) {
		int roi_index;

		for (roi_index = 0; roi_index < VPAP_ENC_MAX_ROI; roi_index++) {
			vpap_shared_enc_roi_t *roi = &coding->roi[roi_index];

			if (roi->left > roi->right || roi->top > roi->bottom ||
				roi->right >= block_columns || roi->bottom >= block_rows ||
				roi->deltaQp < -VIDEO_TEST_MAX_QP || roi->deltaQp > VIDEO_TEST_MAX_QP ||
				roi->qp < -1 || roi->qp > VIDEO_TEST_MAX_QP) {
				RTK_LOGE(TAG, "CH%d ROI%d is invalid for the current %ux%u block grid\r\n",
						 channel, roi_index, (unsigned int)block_columns, (unsigned int)block_rows);
				return -1;
			}
		}
	}
	return 0;
}

static int video_nn_validate(uint32_t enable)
{
	video_params_t *v = &video_params[V5_STREAM_ID];
	if (enable && (!video_channel_enabled[V5_STREAM_ID] || v->format != VIDEO_RGB ||
				   v->width != VIDEO_TEST_NN_WIDTH || v->height != VIDEO_TEST_NN_HEIGHT)) {
		RTK_LOGE(TAG, "VIDNN requires enabled CH4/V5 RGB 416x416\r\n");
		return -1;
	}
	return 0;
}

static void nn_display_cb(void *p, void *img_param)
{
	if (!p || !img_param) {
		return;
	}
	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	objdetect_res_t *res = (objdetect_res_t *)&out->res[0];
	nn_data_param_t *im = (nn_data_param_t *)img_param;
	nn_coord_map_t map;
	/* Log boxes in V5 input pixels; this callback does not allocate OSD layers. */
	nn_coord_map_init(&map, im->img.width, im->img.height, im, NN_COORD_MAP_SCALE_STRETCH);
	for (int i = 0; i < out->res_cnt && i < VIDEO_TEST_NN_MAX_RESULTS; i++) {
		int class_id = (int)res[i].result[0];
		int xmin, ymin, xmax, ymax;
		nn_coord_map_bbox(&map, res[i].result[2], res[i].result[3],
						  res[i].result[4], res[i].result[5], &xmin, &ymin, &xmax, &ymax);
		RTK_LOGI(TAG, "%d,c%d:%d %d %d %d\n\r", i, class_id, xmin, ymin, xmax, ymax);
	}
}

static int video_nn_open(void)
{
	if (!video_nn_enabled) {
		return 0;
	}
	memset(&video_nn_input, 0, sizeof(video_nn_input));
	video_nn_input.img.width = video_params[V5_STREAM_ID].width;
	video_nn_input.img.height = video_params[V5_STREAM_ID].height;
	video_nn_input.img.model_width = VIDEO_TEST_NN_WIDTH;
	video_nn_input.img.model_height = VIDEO_TEST_NN_HEIGHT;
	video_nn_input.codec_type = AV_CODEC_ID_RGB888;
	video_nn_ctx = mm_module_open(&vipnn_module);
	if (!video_nn_ctx ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_MODEL, (int)&yolov4_tiny) != 0 ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_MODEL_FILE_NAME, (int)"vfs:/yolov4_tiny_asymu8.nb") != 0 ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&video_nn_input) != 0 ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb) != 0 ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(objdetect_res_t)) != 0 ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, VIDEO_TEST_NN_MAX_RESULTS) != 0 ||
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_APPLY, 0) != 0) {
		RTK_LOGE(TAG, "V5 YOLOv4-tiny setup failed; check model file and NN resources\r\n");
		return -1;
	}
	RTK_LOGI(TAG, "V5 YOLOv4-tiny ready: RGB 416x416, bounding-box logs only\r\n");
	return 0;
}

static int video_preview_validate(uint32_t mode)
{
	video_params_t *params = &video_params[V2_STREAM_ID];
	if (mode == VIDEO_PREVIEW_NONE) {
		return 0;
	}
	if (!video_channel_enabled[V2_STREAM_ID]) {
		RTK_LOGE(TAG, "VIDOUT requires CH1/V2 enabled by AT+VIDCHEN\r\n");
		return -1;
	}
	if ((params->format == VIDEO_H264 || params->format == VIDEO_HEVC) &&
		(params->coding.flags & VPAP_ENC_CODING_SLICE_SIZE) && params->coding.sliceSize) {
		RTK_LOGE(TAG, "V2 preview requires complete frames; disable slice splitting\r\n");
		return -1;
	}
	if (mode == VIDEO_PREVIEW_RTSP) {
#if VIDEO_TEST_RTSP_ENABLED
		if (params->format == VIDEO_H264 || params->format == VIDEO_HEVC) {
			return 0;
		}
		RTK_LOGE(TAG, "V2 RTSP supports H264/HEVC only\r\n");
#else
		RTK_LOGE(TAG, "RTSP unavailable: enable CONFIG_LWIP_LAYER\r\n");
#endif
	} else if (mode == VIDEO_PREVIEW_UVC) {
#if VIDEO_TEST_UVC_ENABLED
		return 0;
#else
		RTK_LOGE(TAG, "UVC unavailable: enable CONFIG_USBD_UVC\r\n");
#endif
	}
	return -1;
}

#if VIDEO_TEST_RTSP_ENABLED
static int video_rtsp_play(void *arg)
{
	(void)arg;
	/* The RTSP task is joined before the video module is closed. */
	if (video_test_state == VIDEO_TEST_RUNNING && video_ctx[V2_STREAM_ID]) {
		mm_module_ctrl(video_ctx[V2_STREAM_ID], CMD_VIDEO_FORCE_IFRAME, 0);
	}
	return 0;
}
#endif

static int video_preview_open(void)
{
	if (video_preview_mode == VIDEO_PREVIEW_NONE) {
		return 0;
	}
#if VIDEO_TEST_RTSP_ENABLED
	if (video_preview_mode == VIDEO_PREVIEW_RTSP) {
		video_params_t *v = &video_params[V2_STREAM_ID];
		rtsp2_params_t params = {0};
		params.type = AVMEDIA_TYPE_VIDEO;
		params.u.v.codec_id = v->format == VIDEO_HEVC ? AV_CODEC_ID_H265 : AV_CODEC_ID_H264;
		params.u.v.fps = v->fps;
		params.u.v.bps = v->bps;
		/* Match joint test: leave the RTSP URL filter unset for player compatibility. */
		video_preview_ctx = mm_module_open(&rtsp2_module);
		if (!video_preview_ctx ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SELECT_STREAM, 0) != 0 ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_PARAMS, (int)&params) != 0 ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_PORT, 554) != 0 ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_BLOCK_TYPE, RTSP2_NON_BLOCK_TYPE) != 0 ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_START_CB, (int)video_rtsp_play) != 0 ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_APPLY, 0) != 0 ||
			mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_STREAMMING, ON) != 0) {
			return -1;
		}
		RTK_LOGI(TAG, "V2 RTSP: rtsp://<device-ip>:554 (no URL path filter; connect network first)\r\n");
		return 0;
	}
#endif
#if VIDEO_TEST_UVC_ENABLED
	if (video_preview_mode == VIDEO_PREVIEW_UVC) {
		video_uvc_host_state = 0;
		video_preview_ctx = mm_module_open(&uvcd_module);
		if (!video_preview_ctx) {
			return -1;
		}
		RTK_LOGI(TAG, "V2 UVC: forwarding V2 as configured; no descriptor/host format checks\r\n");
		return 0;
	}
#endif
	RTK_LOGE(TAG, "V2 preview mode %u unavailable (RTSP enabled=%d, UVC enabled=%d)\r\n",
			 (unsigned int)video_preview_mode, VIDEO_TEST_RTSP_ENABLED, VIDEO_TEST_UVC_ENABLED);
	return -1;
}

static void video_preview_handle(void *input, void *output)
{
	(void)input;
	(void)output;
#if VIDEO_TEST_RTSP_ENABLED
	if (video_preview_mode == VIDEO_PREVIEW_RTSP) {
		rtsp2_module.handle(video_preview_ctx->priv, input, output);
	}
#endif
#if VIDEO_TEST_UVC_ENABLED
	if (video_preview_mode == VIDEO_PREVIEW_UVC) {
		video_params_t *v = &video_params[V2_STREAM_ID];
		if (!usbd_uvc_get_status()) {
			video_uvc_host_state = 0;
			return;
		}
		if (video_uvc_host_state != 1) {
			video_uvc_host_state = 1;
			if (v->format == VIDEO_H264 || v->format == VIDEO_HEVC) {
				mm_module_ctrl(video_ctx[V2_STREAM_ID], CMD_VIDEO_FORCE_IFRAME, 0);
				return;
			}
		}
		uvcd_module.handle(video_preview_ctx->priv, input, output);
	}
#endif
}

/* Follow the joint-test example: stop video and delete SISO before closing the sink. */
static void video_preview_close(void)
{
	if (!video_preview_ctx) {
		return;
	}
#if VIDEO_TEST_RTSP_ENABLED
	if (video_preview_mode == VIDEO_PREVIEW_RTSP) {
		mm_module_ctrl(video_preview_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
	}
#endif
#if VIDEO_TEST_UVC_ENABLED
	if (video_preview_mode == VIDEO_PREVIEW_UVC) {
		mm_module_ctrl(video_preview_ctx, CMD_UVCD_STOP, 0);
	}
#endif
	mm_module_close(video_preview_ctx);
	video_preview_ctx = NULL;
	RTK_LOGI(TAG, "V2 %s preview module closed\r\n", video_preview_name());
}

static int video_validate_selected_params(void)
{
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
	if (!video_channel_enabled[0] || video_enabled_channel_count() != 1 ||
		video_ainr_enabled || video_nn_enabled || video_preview_mode || video_record_source ||
		video_params[0].format != VIDEO_H264 || video_params[0].width != 1920 ||
		video_params[0].height != 1080 || video_params[0].fps != 15 || video_params[0].coding.sliceSize) {
		RTK_LOGE(TAG, "Comparison requires CH0 only, H264 1920x1080@15, AINR/NN/preview/recording/NAL splitting off\r\n");
		return -1;
	}
	RTK_LOGI(TAG, "ISP->H264: %s\r\n", video_params[0].isp_slice_en ?
		"SRAM slice, 16 lines x 4 slots; Y+UV 184320 bytes" : "frame handoff");
#endif
	int channel;
	int ret = 0;

	if (video_preview_validate(video_preview_mode) != 0 || video_nn_validate(video_nn_enabled) != 0) {
		return -1;
	}
	if (!video_enabled_channel_count()) {
		RTK_LOGE(TAG, "no channel selected; use AT+VIDCHEN\r\n");
		return -1;
	}
	if (video_record_source) {
		channel = (int)video_record_source - 1;
		uint64_t samples = (uint64_t)video_params[channel].fps * video_record_seconds;
		if (!video_channel_enabled[channel] || !video_channel_supports_advanced_encoder(channel) ||
			samples < 2 || samples > 7200 ||
			((video_params[channel].coding.flags & VPAP_ENC_CODING_SLICE_SIZE) && video_params[channel].coding.sliceSize)) {
			RTK_LOGE(TAG, "MP4 requires enabled V1/V2 H264/HEVC, 2-7200 samples, and slice splitting disabled\r\n");
			return -1;
		}
	}
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (video_channel_enabled[channel] &&
			(video_validate_dimensions(video_params[channel].format, video_params[channel].width, video_params[channel].height) != 0 ||
			 video_validate_advanced_params(channel) != 0)) {
			RTK_LOGE(TAG, "CH%d configuration validation failed\r\n", channel);
			ret = -1;
		}
	}
	return ret;
}

static void video_at_finish(int error)
{
	/* Keep the AT protocol terminator untagged and routed through the AT transport. */
	if (error == VIDEO_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error);
	}
}

static int video_test_deinit(int rollback)
{
	int channel;
	int record_result;

	video_test_state = VIDEO_TEST_STOPPING;
	RTK_LOGI(TAG, "[VIDEO TEST][%s] teardown started\r\n", rollback ? "ROLLBACK" : "STOP");
	RTK_LOGI(TAG, "[VIDEO TEST][STOP][1/3] stopping video producers...\r\n");

	/* Resource checks release only started channels and also cover a partial-start rollback. */
	/* Stop all video producers before deleting their SISO paths. */
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (video_ctx[channel] && video_applied[channel]) {
			RTK_LOGI(TAG, "[VIDEO TEST][STOP][1/3][CH%d] stopping %s video stream...\r\n", channel, video_stream_name(channel));
			mm_module_ctrl(video_ctx[channel], CMD_VIDEO_STREAM_STOP, 0);
			video_applied[channel] = 0;
			RTK_LOGI(TAG, "[VIDEO TEST][STOP][1/3][CH%d] video stream stopped\r\n", channel);
		}
	}

	RTK_LOGI(TAG, "[VIDEO TEST][STOP][2/3] stopping and deleting SISO paths...\r\n");
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (video_filesaver_siso[channel]) {
			RTK_LOGI(TAG, "[VIDEO TEST][STOP][2/3][CH%d] deleting video -> filesaver path...\r\n", channel);
			siso_stop(video_filesaver_siso[channel]);
			siso_delete(video_filesaver_siso[channel]);
			video_filesaver_siso[channel] = NULL;
			RTK_LOGI(TAG, "[VIDEO TEST][STOP][2/3][CH%d] SISO path deleted\r\n", channel);
		}
	}

	/* V5 consumer is joined before releasing its inference buffers/model. */
	if (video_nn_ctx) {
		vipnn_status_t status;
		mm_module_ctrl(video_nn_ctx, CMD_VIPNN_GET_STATUS, (int)&status);
		if (status == VIPNN_APPLIED) {
			mm_module_ctrl(video_nn_ctx, CMD_VIPNN_SET_STATUS, (int)VIPNN_PAUSED);
		}
		mm_module_close(video_nn_ctx);
		video_nn_ctx = NULL;
	}
	video_preview_close();
	record_result = video_record_close(rollback);

	RTK_LOGI(TAG, "[VIDEO TEST][STOP][3/3] closing filesaver and video modules...\r\n");
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (filesaver_ctx[channel]) {
			mm_module_close(filesaver_ctx[channel]);
			filesaver_ctx[channel] = NULL;
			RTK_LOGI(TAG, "[VIDEO TEST][STOP][3/3][CH%d] filesaver module closed\r\n", channel);
		}
		if (video_ctx[channel]) {
			mm_module_close(video_ctx[channel]);
			video_ctx[channel] = NULL;
			RTK_LOGI(TAG, "[VIDEO TEST][STOP][3/3][CH%d] video module closed\r\n", channel);
		}
	}

	video_test_state = VIDEO_TEST_STOPPED;
	RTK_LOGI(TAG, "[VIDEO TEST][%s][DONE] all active channel close operations completed\r\n", rollback ? "ROLLBACK" : "STOP");
	RTK_LOGI(TAG, "video command test stopped\r\n");
	return record_result;
}

static int video_test_start_channel(int channel)
{
	int ret;

	RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][1/3] opening and configuring %s video module...\r\n", channel, video_stream_name(channel));
	video_ctx[channel] = mm_module_open(&video_module);
	if (!video_ctx[channel]) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] video module open failed\r\n", channel);
		RTK_LOGE(TAG, "video ch%d open failed\r\n", channel);
		return -1;
	}

	/* VIDAINR is global; every selected channel carries the same requested value. */
	video_params[channel].ainr_en = video_ainr_enabled;
	ret = mm_module_ctrl(video_ctx[channel], CMD_VIDEO_SET_PARAMS, (int)&video_params[channel]);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] video parameter setup failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "video ch%d set params failed: %d\r\n", channel, ret);
		return -1;
	}
	ret = mm_module_ctrl(video_ctx[channel], MM_CMD_SET_QUEUE_LEN, VIDEO_TEST_QUEUE_LEN);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] video queue creation failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "video ch%d queue creation failed: %d\r\n", channel, ret);
		return -1;
	}
	ret = mm_module_ctrl(video_ctx[channel], MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] video queue initialization failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "video ch%d queue initialization failed: %d\r\n", channel, ret);
		return -1;
	}

	RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][2/3] opening filesaver module...\r\n", channel);
	filesaver_ctx[channel] = mm_module_open(&video_test_sink_module);
	if (!filesaver_ctx[channel]) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] filesaver module open failed\r\n", channel);
		RTK_LOGE(TAG, "filesaver ch%d open failed\r\n", channel);
		return -1;
	}
	ret = mm_module_ctrl(filesaver_ctx[channel], CMD_FILESAVER_SET_TYPE_HANDLER, (int)filesaver_handlers[channel]);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] filesaver handler setup failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "filesaver ch%d handler setup failed: %d\r\n", channel, ret);
		return -1;
	}
	ret = mm_module_ctrl(filesaver_ctx[channel], CMD_FILESAVER_APPLY, 0);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] filesaver apply failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "filesaver ch%d apply failed: %d\r\n", channel, ret);
		return -1;
	}
	RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][2/3] filesaver module ready\r\n", channel);

	if (channel == V2_STREAM_ID && video_preview_open() != 0) {
		RTK_LOGE(TAG, "V2 preview open failed; rolling back\r\n");
		return -1;
	}
	if (channel == V5_STREAM_ID && video_nn_open() != 0) {
		return -1;
	}
	/* One consumer owns each buffer through statistics, MP4, preview and NN. */
	RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][3/3] creating video -> filesaver SISO path...\r\n", channel);
	video_filesaver_siso[channel] = siso_create();
	if (!video_filesaver_siso[channel]) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] SISO path creation failed\r\n", channel);
		RTK_LOGE(TAG, "siso ch%d create failed\r\n", channel);
		return -1;
	}
	siso_ctrl(video_filesaver_siso[channel], MMIC_CMD_ADD_INPUT, (uint32_t)video_ctx[channel], 0);
	siso_ctrl(video_filesaver_siso[channel], MMIC_CMD_ADD_OUTPUT, (uint32_t)filesaver_ctx[channel], 0);
	ret = siso_start(video_filesaver_siso[channel]);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] SISO path start failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "siso ch%d start failed: %d\r\n", channel, ret);
		video_filesaver_siso[channel] = siso_delete(video_filesaver_siso[channel]);
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][CLEANUP] unstarted SISO path deleted\r\n", channel);
		return -1;
	}

	/* Start the producer only after its consumer can drain and recycle buffers. */
	ret = mm_module_ctrl(video_ctx[channel], CMD_VIDEO_APPLY, 0);
	if (ret != 0) {
		RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][FAILED] video apply failed (%d)\r\n", channel, ret);
		RTK_LOGE(TAG, "video ch%d apply failed: %d\r\n", channel, ret);
		return -1;
	}
	video_applied[channel] = 1;
	RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][3/3] video module applied\r\n", channel);

	RTK_LOGI(TAG, "[VIDEO TEST][START][CH%d][DONE] %s %ux%u @ %u fps is active\r\n",
			 channel, video_format_name(video_params[channel].format),
			 (unsigned int)video_params[channel].width, (unsigned int)video_params[channel].height,
			 (unsigned int)video_params[channel].fps);
	RTK_LOGI(TAG, "ch%d %s %ux%u@%u -> filesaver started\r\n",
			 channel, video_format_name(video_params[channel].format),
			 (unsigned int)video_params[channel].width, (unsigned int)video_params[channel].height,
			 (unsigned int)video_params[channel].fps);
	return 0;
}

static int mmf2_video_cmd_test(void)
{
	int channel;
	int enabled_count;
	int active_index = 0;

	if (video_test_state != VIDEO_TEST_STOPPED) {
		return -1;
	}
	enabled_count = video_enabled_channel_count();
	if (enabled_count == 0) {
		RTK_LOGE(TAG, "[VIDEO TEST][START][FAILED] no channel is enabled; use AT+VIDCHEN first\r\n");
		return -1;
	}
	if (video_validate_selected_params() != 0) {
		return -1;
	}

	if (video_http_owned) {
		RTK_LOGE(TAG, "stop downloads with AT+VIDHTTP=0 before starting a test\r\n");
		return -1;
	}
	/* shortcut: remap restart is not validated; reboot before each comparison run. */
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
	if (video_comparison_started) {
		RTK_LOGE(TAG, "Reboot before another comparison run\r\n");
		return -1;
	}
	video_comparison_started = 1;
#endif
	video_test_sink_module = filesaver_module;
	video_test_sink_module.handle = video_test_sink_handle;
	if (video_record_prepare() != 0) {
		video_record_close(1);
		return -1;
	}
	memset(video_stats, 0, sizeof(video_stats));
	video_test_state = VIDEO_TEST_STARTING;
	RTK_LOGI(TAG, "[VIDEO TEST][START] start request accepted\r\n");
	RTK_LOGI(TAG, "[VIDEO TEST][START][PRECHECK] configuration selected for this run:\r\n");
	video_print_all_params();
	RTK_LOGI(TAG, "[VIDEO TEST][START] initializing %d selected channels...\r\n", enabled_count);

	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (!video_channel_enabled[channel]) {
			RTK_LOGI(TAG, "[VIDEO TEST][START][SKIP] CH%d (%s) is disabled\r\n", channel, video_stream_name(channel));
			continue;
		}
		active_index++;
		RTK_LOGI(TAG, "[VIDEO TEST][START] selected channel %d/%d: CH%d (%s)\r\n",
				 active_index, enabled_count, channel, video_stream_name(channel));
		if (video_test_start_channel(channel) != 0) {
			RTK_LOGI(TAG, "[VIDEO TEST][START][FAILED] CH%d failed; rolling back all initialized channels\r\n", channel);
			RTK_LOGE(TAG, "video command test start failed at ch%d\r\n", channel);
			video_test_deinit(1);
			return -1;
		}
	}

	video_test_state = VIDEO_TEST_RUNNING;
	RTK_LOGI(TAG, "[VIDEO TEST][START][DONE] all %d selected channels are running\r\n", enabled_count);
	RTK_LOGI(TAG, "[VIDEO TEST][START][DONE] use AT+VIDINFO or AT+VIDINFO=<ch> to inspect parameters and path state\r\n");
	RTK_LOGI(TAG, "video command test started\r\n");
	RTK_LOGI(TAG, "Pipeline setup only; use AT+VIDCHECK=3000 to verify buffer delivery.\r\n");
	return 0;
}

void fVIDHELP(u16 argc, char **argv)
{
	int error = VIDEO_AT_ERR_PARAM;

	(void)argv;
	if (argc != 1) {
		RTK_LOGI(TAG, "usage: AT+VID?\r\n");
		goto end;
	}

	RTK_LOGI(TAG, "========================== VIDEO COMMAND TEST HELP ==========================\r\n");
	RTK_LOGI(TAG, "All parameter changes are allowed only while the test is stopped.\r\n");
	RTK_LOGI(TAG, "Channel map: CH0=V1, CH1=V2, CH2=V3, CH3=V4, CH4=V5\r\n");
	RTK_LOGI(TAG, "Formats    : CH0-2=HEVC/H264/JPEG/NV12, CH3=NV12 only, CH4=RGB only\r\n");
	RTK_LOGI(TAG, "Pipeline   : video_module -> SISO -> filesaver_module (selected channels)\r\n");
	RTK_LOGI(TAG, "------------------------------------------------------------------------------\r\n");
	RTK_LOGI(TAG, "AT+VID?\r\n");
	RTK_LOGI(TAG, "  Show this help. Example: AT+VID?\r\n");
	RTK_LOGI(TAG, "AT+VIDCHEN=<ch0>,<ch1>,<ch2>,<ch3>,<ch4>\r\n");
	RTK_LOGI(TAG, "  Select channel pipelines (0=disabled, 1=enabled). Example: AT+VIDCHEN=1,0,0,1,0\r\n");
	RTK_LOGI(TAG, "AT+VIDFMT=<ch>,<HEVC|H264|JPEG|NV12|RGB>\r\n");
	RTK_LOGI(TAG, "  Set channel format. Example: AT+VIDFMT=0,H264\r\n");
	RTK_LOGI(TAG, "AT+VIDRES=<ch>,<width>,<height>\r\n");
	RTK_LOGI(TAG, "  Set channel resolution. Example: AT+VIDRES=0,1920,1080\r\n");
	RTK_LOGI(TAG, "AT+VIDFPS=<ch>,<fps>\r\n");
	RTK_LOGI(TAG, "  Set channel frame rate. Example: AT+VIDFPS=0,30\r\n");
	RTK_LOGI(TAG, "AT+VIDGOP=<ch0-2>,<gop>\r\n");
	RTK_LOGI(TAG, "  Set encoder GOP. Example: AT+VIDGOP=0,30\r\n");
	RTK_LOGI(TAG, "AT+VIDBPS=<ch0-2>,<bps>\r\n");
	RTK_LOGI(TAG, "  Set encoder bitrate. Example: AT+VIDBPS=0,4000000\r\n");
	RTK_LOGI(TAG, "AT+VIDRCM=<ch0-2>,<mode>\r\n");
	RTK_LOGI(TAG, "  RC mode: 0=CVBR, 1=CBR, 2=VBR, 3=ABR, 4=CRF, 5=CQP\r\n");
	RTK_LOGI(TAG, "  Example: AT+VIDRCM=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDQP=<ch0-2>,<min_qp>,<max_qp>\r\n");
	RTK_LOGI(TAG, "  Set QP range (0-51, min<=max). Example: AT+VIDQP=0,20,40\r\n");
	RTK_LOGI(TAG, "AT+VIDQLVL=<ch0-2>,<qlevel>\r\n");
	RTK_LOGI(TAG, "  Set JPEG quality level (0-10). Example: AT+VIDQLVL=2,5\r\n");
	RTK_LOGI(TAG, "AT+VIDAINR=<0|1>\r\n");
	RTK_LOGI(TAG, "  Disable/enable global AINR. Example: AT+VIDAINR=1\r\n");
	RTK_LOGI(TAG, "  Global AINR is applied when any selected channel starts; CH0/V1 is not required.\r\n");
	RTK_LOGI(TAG, "---------------- Advanced H264/HEVC init-time rate control ------------------\r\n");
	RTK_LOGI(TAG, "The following advanced commands require CH0-2 with current format H264 or HEVC.\r\n");
	RTK_LOGI(TAG, "AT+VIDPROF=<ch>,<profile>,<level>,<tier>\r\n");
	RTK_LOGI(TAG, "  Profile: -1=adaptive, HEVC=0-9, H264=9-12; level: 65535=auto.\r\n");
	RTK_LOGI(TAG, "  HEVC levels: 30,60,63,90,93,120,123,150,153,156,180,183,186; tier: 0-1.\r\n");
	RTK_LOGI(TAG, "  H264 levels: 10,11,12,13,20,21,22,30,31,32,40,41,42,50,51,52,60,61,62,99; tier: 0.\r\n");
	RTK_LOGI(TAG, "  Example: AT+VIDPROF=0,-1,65535,0\r\n");
	RTK_LOGI(TAG, "AT+VIDQPHDR=<ch>,<qp_hdr>\r\n");
	RTK_LOGI(TAG, "  Initial QP (-1=auto, 0-51). Example: AT+VIDQPHDR=0,-1\r\n");
	RTK_LOGI(TAG, "AT+VIDIQP=<ch>,<min_i_qp>,<max_i_qp>\r\n");
	RTK_LOGI(TAG, "  I-frame QP range. Example: AT+VIDIQP=0,20,42\r\n");
	RTK_LOGI(TAG, "AT+VIDIQPD=<ch>,<delta>\r\n");
	RTK_LOGI(TAG, "  Intra QP delta (-51..51). Example: AT+VIDIQPD=0,-3\r\n");
	RTK_LOGI(TAG, "AT+VIDFIQP=<ch>,<fixed_i_qp>\r\n");
	RTK_LOGI(TAG, "  Fixed intra QP (0=off, 1-51). Example: AT+VIDFIQP=0,26\r\n");
	RTK_LOGI(TAG, "AT+VIDPICRC=<ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  Picture-level RC. Example: AT+VIDPICRC=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDCTBRC=<ch>,<mode>\r\n");
	RTK_LOGI(TAG, "  CTB RC: 0=off, 1=subjective, 2=precise, 3=mixed. Example: AT+VIDCTBRC=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDBLKRC=<ch>,<size>\r\n");
	RTK_LOGI(TAG, "  RC block: 0=64x64, 1=32x32, 2=16x16. Example: AT+VIDBLKRC=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDPSKIP=<ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  Allow picture skipping. Example: AT+VIDPSKIP=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDHRD=<ch>,<0|1>,<cpb_size>,<cpb_max_rate>\r\n");
	RTK_LOGI(TAG, "  HRD/CPB control. Example: AT+VIDHRD=0,1,4000000,4000000\r\n");
	RTK_LOGI(TAG, "AT+VIDBRWIN=<ch>,<frames>\r\n");
	RTK_LOGI(TAG, "  Bitrate window (1-300). Example: AT+VIDBRWIN=0,30\r\n");
	RTK_LOGI(TAG, "AT+VIDMVBR=<ch>,<tolerance_percent>,<monitor_frames>\r\n");
	RTK_LOGI(TAG, "  Moving bitrate monitor. Example: AT+VIDMVBR=0,200,30\r\n");
	RTK_LOGI(TAG, "AT+VIDFILL=<ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  Filler data; requires PICRC=1 and nonzero CPB. Example: AT+VIDFILL=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDCRF=<ch>,<crf>\r\n");
	RTK_LOGI(TAG, "  CRF (-1=off, 0-51). Example: AT+VIDCRF=0,28\r\n");
	RTK_LOGI(TAG, "AT+VIDVBR=<ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  VBR extra; conflicts with HRD. Example: AT+VIDVBR=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDRCFPS=<ch>,<numerator>,<denominator>\r\n");
	RTK_LOGI(TAG, "  Rational RC frame rate. Example: AT+VIDRCFPS=0,30000,1001\r\n");
	RTK_LOGI(TAG, "------------------- Advanced H264/HEVC coding control -----------------------\r\n");
	RTK_LOGI(TAG, "AT+VIDSLICE=<ch>,<block_rows>\r\n");
	RTK_LOGI(TAG, "  Slice splitting (0=off). Example: AT+VIDSLICE=0,4\r\n");
	RTK_LOGI(TAG, "AT+VIDSEI=<ch>,<driver_sei>,<user_data_sei>\r\n");
	RTK_LOGI(TAG, "  Configure both SEI sources. Example: AT+VIDSEI=0,1,1\r\n");
	RTK_LOGI(TAG, "AT+VIDFULLR=<ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  VUI limited/full range. Example: AT+VIDFULLR=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDDBLK=<ch>,<disable>,<tc_offset>,<beta_offset>\r\n");
	RTK_LOGI(TAG, "  Deblock control; offsets -6..6. Example: AT+VIDDBLK=0,0,0,0\r\n");
	RTK_LOGI(TAG, "AT+VIDSAO=<HEVC ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  HEVC sample adaptive offset. Example: AT+VIDSAO=0,1\r\n");
	RTK_LOGI(TAG, "AT+VIDCABAC=<H264 ch>,<0|1>\r\n");
	RTK_LOGI(TAG, "  H264 entropy coding. Example: AT+VIDCABAC=1,1\r\n");
	RTK_LOGI(TAG, "AT+VIDSAR=<ch>,<width>,<height>\r\n");
	RTK_LOGI(TAG, "  Sample aspect ratio; 0,0=unspecified. Example: AT+VIDSAR=0,1,1\r\n");
	RTK_LOGI(TAG, "AT+VIDCIR=<ch>,<start>,<interval>\r\n");
	RTK_LOGI(TAG, "  Cyclic intra refresh in encoder blocks. Example: AT+VIDCIR=0,0,10\r\n");
	RTK_LOGI(TAG, "AT+VIDROI=<ch>,<idx>,<en>,<top>,<left>,<bottom>,<right>,<delta_qp>,<qp>\r\n");
	RTK_LOGI(TAG, "  ROI 0-7 in CTB/MB coordinates; qp=-1 uses delta. Example: AT+VIDROI=0,0,1,0,0,3,3,-5,-1\r\n");
	RTK_LOGI(TAG, "AT+VIDXCLR=<ch>,<PROFILE|RC|CODING|ALL>\r\n");
	RTK_LOGI(TAG, "  Clear explicitly configured extras. Example: AT+VIDXCLR=0,ALL\r\n");
	RTK_LOGI(TAG, "------------------------------------------------------------------------------\r\n");
	RTK_LOGI(TAG, "AT+VIDINFO[=<ch0-4|ALL>]\r\n");
	RTK_LOGI(TAG, "  Show one/all channel settings. Examples: AT+VIDINFO=0, AT+VIDINFO=ALL\r\n");
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
	RTK_LOGI(TAG, "AT+VIDPIPE=<0=frame|1=SRAM slice>; reboot between runs; default frame\r\n");
#endif
	RTK_LOGI(TAG, "AT+VIDTEST=<0|1>\r\n");
	RTK_LOGI(TAG, "  Stop/start selected channels. Examples: AT+VIDTEST=1, AT+VIDTEST=0\r\n");
	RTK_LOGI(TAG, "------------------------------------------------------------------------------\r\n");
	RTK_LOGI(TAG, "AT+VIDNN[=<0|1>]: query/select CH4/V5 YOLOv4-tiny; stopped only; default OFF.\r\n");
	RTK_LOGI(TAG, "  Requires CH4 enabled, RGB 416x416, vfs:/yolov4_tiny_asymu8.nb; logs V5 pixel boxes, no OSD.\r\n");
	RTK_LOGI(TAG, "AT+VIDOUT[=<0=none|1=RTSP|2=UVC>]: query/select CH1/V2 preview; stopped only.\r\n");
	RTK_LOGI(TAG, "  Default none keeps statistics/MP4. Set V2 params and enable CH1 before selecting preview.\r\n");
	RTK_LOGI(TAG, "  RTSP: CONFIG_LWIP_LAYER, H264/HEVC, network ready; rtsp://<device-ip>:554 (no URL path filter).\r\n");
	RTK_LOGI(TAG, "  UVC: CONFIG_USBD_UVC; use current V2 settings without descriptor/host format checks.\r\n");
	RTK_LOGI(TAG, "  Disable encoded slice splitting. VIDTEST=0 releases preview; selection persists for restart.\r\n");
	RTK_LOGI(TAG, "AT+VIDREC=0: disable recording (default); AT+VIDREC=<1=V1|2=V2>,<1-60 seconds>.\r\n");
	RTK_LOGI(TAG, "  One H264/HEVC channel, video only, 2-7200 samples, no slice splitting; SD card required.\r\n");
	RTK_LOGI(TAG, "  Replaces sd:/video_test_v1.mp4 or video_test_v2.mp4; duration is fps*seconds samples.\r\n");
	RTK_LOGI(TAG, "AT+VIDHTTP=<0|1>: after VIDTEST=0, start HTTP and browse http://<device-ip>/video.\r\n");
	RTK_LOGI(TAG, "  Stop HTTP before the next test; keep SD inserted until HTTP is stopped.\r\n");
	RTK_LOGI(TAG, "AT+VIDVALID: validate selected parameters without starting hardware (sensor limits still require hardware).\r\n");
	RTK_LOGI(TAG, "AT+VIDSTAT[=<ch0-4>]: buffer counts, KiB, invalid buffers, and last-buffer age; retained after stop.\r\n");
	RTK_LOGI(TAG, "AT+VIDCHECK=<100-10000 ms>: check new buffers on every enabled channel; blocks AT for the window.\r\n");
	RTK_LOGI(TAG, "AT+VIDIFR=<ch0-2>: request an I-frame on an active H264/HEVC channel.\r\n");
	RTK_LOGI(TAG, "The sink counts buffers; VIDREC optionally muxes V1/V2 into MP4 on SD (no bitstream validation).\r\n");
	RTK_LOGI(TAG, "Errors: 1=parameter, 2=state, 3=start, 4=no data or invalid buffers during VIDCHECK, 5=MP4/SD/HTTP error.\r\n");
	RTK_LOGI(TAG, "Suggested flow: AT+VIDCHEN -> configure -> AT+VIDVALID -> AT+VIDTEST=1 -> AT+VIDCHECK=3000 -> AT+VIDTEST=0\r\n");
	RTK_LOGI(TAG, "=============================================================================\r\n");
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDCHEN(u16 argc, char **argv)
{
	uint32_t requested[VIDEO_TEST_CHANNEL_COUNT];
	int channel;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != VIDEO_TEST_CHANNEL_COUNT + 1 || video_require_stopped() != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDCHEN=<ch0>,<ch1>,<ch2>,<ch3>,<ch4> (0=disabled, 1=enabled)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		if (video_parse_u32(argv[channel + 1], 0, 1, &requested[channel]) != 0) {
			RTK_LOGI(TAG, "usage: AT+VIDCHEN=<ch0>,<ch1>,<ch2>,<ch3>,<ch4> (0=disabled, 1=enabled)\r\n");
			goto end;
		}
	}

	/* Commit the complete selection only after all five values are valid. */
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		video_channel_enabled[channel] = (uint8_t)requested[channel];
	}
	RTK_LOGI(TAG, "+VIDCHEN:%u,%u,%u,%u,%u\r\n",
			 (unsigned int)video_channel_enabled[V1_STREAM_ID], (unsigned int)video_channel_enabled[V2_STREAM_ID],
			 (unsigned int)video_channel_enabled[V3_STREAM_ID], (unsigned int)video_channel_enabled[V4_STREAM_ID],
			 (unsigned int)video_channel_enabled[V5_STREAM_ID]);
	RTK_LOGI(TAG, "selected channels: %d/%d\r\n", video_enabled_channel_count(), VIDEO_TEST_CHANNEL_COUNT);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDFMT(u16 argc, char **argv)
{
	int channel;
	uint32_t format;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		video_parse_format(argv[2], &format) != 0 || !video_format_is_supported(channel, format)) {
		RTK_LOGI(TAG, "usage: AT+VIDFMT=<ch>,<HEVC|H264|JPEG|NV12|RGB>\r\n"
				 "ch0-2: HEVC/H264/JPEG/NV12, ch3: NV12, ch4: RGB\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	if (video_validate_dimensions(format, video_params[channel].width, video_params[channel].height) != 0) {
		goto end;
	}
	video_params[channel].format = format;
	RTK_LOGI(TAG, "+VIDFMT:%d,%s\r\n", channel, video_format_name(format));
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDRES(u16 argc, char **argv)
{
	int channel;
	uint32_t width;
	uint32_t height;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 4 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		video_parse_u32(argv[2], 1, VIDEO_TEST_MAX_DIMENSION, &width) != 0 ||
		video_parse_u32(argv[3], 1, VIDEO_TEST_MAX_DIMENSION, &height) != 0 ||
		video_validate_dimensions(video_params[channel].format, width, height) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDRES=<ch>,<width>,<height>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].width = width;
	video_params[channel].height = height;
	RTK_LOGI(TAG, "+VIDRES:%d,%u,%u\r\n", channel, (unsigned int)width, (unsigned int)height);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDFPS(u16 argc, char **argv)
{
	int channel;
	uint32_t fps;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		video_parse_u32(argv[2], 1, VIDEO_TEST_MAX_FPS, &fps) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDFPS=<ch>,<fps>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].fps = fps;
	RTK_LOGI(TAG, "+VIDFPS:%d,%u\r\n", channel, (unsigned int)fps);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDGOP(u16 argc, char **argv)
{
	int channel;
	uint32_t gop;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		!video_channel_has_encoder(channel) || video_parse_u32(argv[2], 1, VIDEO_TEST_MAX_GOP, &gop) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDGOP=<ch0-2>,<gop>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].gop = gop;
	RTK_LOGI(TAG, "+VIDGOP:%d,%u\r\n", channel, (unsigned int)gop);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDBPS(u16 argc, char **argv)
{
	int channel;
	uint32_t bps;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		!video_channel_has_encoder(channel) || video_parse_u32(argv[2], 1, INT_MAX, &bps) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDBPS=<ch0-2>,<bps>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].bps = bps;
	RTK_LOGI(TAG, "+VIDBPS:%d,%u\r\n", channel, (unsigned int)bps);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDRCM(u16 argc, char **argv)
{
	int channel;
	uint32_t rc_mode;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		!video_channel_has_encoder(channel) || video_parse_u32(argv[2], ENC_CVBR, ENC_CQP, &rc_mode) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDRCM=<ch0-2>,<mode>\r\n"
				 "mode: 0=CVBR, 1=CBR, 2=VBR, 3=ABR, 4=CRF, 5=CQP\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_mode = rc_mode;
	RTK_LOGI(TAG, "+VIDRCM:%d,%u\r\n", channel, (unsigned int)rc_mode);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDQP(u16 argc, char **argv)
{
	int channel;
	uint32_t min_qp;
	uint32_t max_qp;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 4 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		!video_channel_has_encoder(channel) || video_parse_u32(argv[2], VIDEO_TEST_MIN_QP, VIDEO_TEST_MAX_QP, &min_qp) != 0 ||
		video_parse_u32(argv[3], VIDEO_TEST_MIN_QP, VIDEO_TEST_MAX_QP, &max_qp) != 0 || min_qp > max_qp) {
		RTK_LOGI(TAG, "usage: AT+VIDQP=<ch0-2>,<min_qp>,<max_qp> (0-51)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].min_qp = min_qp;
	video_params[channel].max_qp = max_qp;
	RTK_LOGI(TAG, "+VIDQP:%d,%u,%u\r\n", channel, (unsigned int)min_qp, (unsigned int)max_qp);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDQLVL(u16 argc, char **argv)
{
	int channel;
	uint32_t qlevel;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		!video_channel_has_encoder(channel) || video_parse_u32(argv[2], 0, VIDEO_TEST_MAX_QLEVEL, &qlevel) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDQLVL=<ch0-2>,<qlevel> (0-10)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].qlevel = qlevel;
	RTK_LOGI(TAG, "+VIDQLVL:%d,%u\r\n", channel, (unsigned int)qlevel);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDAINR(u16 argc, char **argv)
{
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 2 || video_require_stopped() != 0 || video_parse_u32(argv[1], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDAINR=<0|1>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	/* Copy this global selection into each channel when it is opened. */
	video_ainr_enabled = enable;
	RTK_LOGI(TAG, "+VIDAINR:%u\r\n", (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDPROF(u16 argc, char **argv)
{
	int channel;
	int32_t profile;
	uint32_t level;
	uint32_t tier;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 5, argv, &channel) != 0 ||
		video_parse_i32(argv[2], VIDEO_TEST_MIN_PROFILE, VIDEO_TEST_H264_MAX_PROFILE, &profile) != 0 ||
		!video_profile_is_supported(video_params[channel].format, profile) ||
		video_parse_u32(argv[3], 0, VIDEO_TEST_MAX_LEVEL, &level) != 0 ||
		!video_level_is_supported(video_params[channel].format, level) ||
		video_parse_u32(argv[4], 0, VIDEO_TEST_MAX_TIER, &tier) != 0 ||
		(video_params[channel].format == VIDEO_H264 && tier != 0)) {
		RTK_LOGI(TAG, "usage: AT+VIDPROF=<ch0-2>,<profile>,<level>,<tier>\r\n");
		RTK_LOGI(TAG, "profile: -1=adaptive, HEVC=0-9, H264=9-12; level: valid codec enum or 65535=auto; tier: HEVC 0-1, H264 0\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params_set_profile_level(&video_params[channel], profile, level);
	video_params_set_tier(&video_params[channel], tier);
	RTK_LOGI(TAG, "+VIDPROF:%d,%d,%u,%u\r\n", channel, (int)profile, (unsigned int)level, (unsigned int)tier);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDQPHDR(u16 argc, char **argv)
{
	int channel;
	int32_t qp_hdr;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_i32(argv[2], -1, VIDEO_TEST_MAX_QP, &qp_hdr) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDQPHDR=<ch0-2>,<qp_hdr> (-1=auto, 0-51=fixed initial QP)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params_set_qp_hdr(&video_params[channel], qp_hdr);
	RTK_LOGI(TAG, "+VIDQPHDR:%d,%d\r\n", channel, (int)qp_hdr);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDIQP(u16 argc, char **argv)
{
	int channel;
	uint32_t min_i_qp;
	uint32_t max_i_qp;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 4, argv, &channel) != 0 ||
		video_parse_u32(argv[2], VIDEO_TEST_MIN_QP, VIDEO_TEST_MAX_QP, &min_i_qp) != 0 ||
		video_parse_u32(argv[3], VIDEO_TEST_MIN_QP, VIDEO_TEST_MAX_QP, &max_i_qp) != 0 || min_i_qp > max_i_qp) {
		RTK_LOGI(TAG, "usage: AT+VIDIQP=<ch0-2>,<min_i_qp>,<max_i_qp> (0-51)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_I_QP_RANGE;
	video_params[channel].rc_ext.minIQp = min_i_qp;
	video_params[channel].rc_ext.maxIQp = max_i_qp;
	RTK_LOGI(TAG, "+VIDIQP:%d,%u,%u\r\n", channel, (unsigned int)min_i_qp, (unsigned int)max_i_qp);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDIQPD(u16 argc, char **argv)
{
	int channel;
	int32_t delta;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_i32(argv[2], -VIDEO_TEST_MAX_QP, VIDEO_TEST_MAX_QP, &delta) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDIQPD=<ch0-2>,<intra_qp_delta> (-51 to 51)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_INTRA_QP_DELTA;
	video_params[channel].rc_ext.intraQpDelta = delta;
	RTK_LOGI(TAG, "+VIDIQPD:%d,%d\r\n", channel, (int)delta);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDFIQP(u16 argc, char **argv)
{
	int channel;
	uint32_t fixed_i_qp;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_u32(argv[2], VIDEO_TEST_MIN_QP, VIDEO_TEST_MAX_QP, &fixed_i_qp) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDFIQP=<ch0-2>,<fixed_intra_qp> (0=disabled, 1-51=fixed QP)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_FIXED_INTRA_QP;
	video_params[channel].rc_ext.fixedIntraQp = fixed_i_qp;
	RTK_LOGI(TAG, "+VIDFIQP:%d,%u\r\n", channel, (unsigned int)fixed_i_qp);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDPICRC(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDPICRC=<ch0-2>,<0|1>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_PIC_RC;
	video_params[channel].rc_ext.pictureRc = enable;
	RTK_LOGI(TAG, "+VIDPICRC:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDCTBRC(u16 argc, char **argv)
{
	int channel;
	uint32_t mode;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_u32(argv[2], 0, VIDEO_TEST_MAX_CTB_RC, &mode) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDCTBRC=<ch0-2>,<mode> (0=off, 1=subjective, 2=precise, 3=mixed)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_CTB_RC;
	video_params[channel].rc_ext.ctbRc = mode;
	RTK_LOGI(TAG, "+VIDCTBRC:%d,%u\r\n", channel, (unsigned int)mode);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDBLKRC(u16 argc, char **argv)
{
	int channel;
	uint32_t size;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_u32(argv[2], 0, VIDEO_TEST_MAX_BLOCK_RC, &size) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDBLKRC=<ch0-2>,<size> (0=64x64, 1=32x32, 2=16x16)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_BLOCK_RC_SIZE;
	video_params[channel].rc_ext.blockRCSize = size;
	RTK_LOGI(TAG, "+VIDBLKRC:%d,%u\r\n", channel, (unsigned int)size);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDPSKIP(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDPSKIP=<ch0-2>,<0|1>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_PIC_SKIP;
	video_params[channel].rc_ext.pictureSkip = enable;
	RTK_LOGI(TAG, "+VIDPSKIP:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDHRD(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	uint32_t cpb_size;
	uint32_t cpb_max_rate;
	int error = VIDEO_AT_ERR_PARAM;

	/* The command bridge uses signed decimal CPB values. Preserve only the
	 * documented UINT32_MAX (-1) sentinel outside the positive int range. */
	if (video_prepare_advanced_command(argc, 5, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &enable) != 0 ||
		video_parse_u32(argv[3], 0, UINT32_MAX, &cpb_size) != 0 ||
		video_parse_u32(argv[4], 0, INT_MAX, &cpb_max_rate) != 0 ||
		(cpb_size > INT_MAX && cpb_size != UINT32_MAX)) {
		RTK_LOGI(TAG, "usage: AT+VIDHRD=<ch0-2>,<0|1>,<cpb_size>,<cpb_max_rate>\r\n");
		RTK_LOGI(TAG, "cpb_size: 0=default, 1-2147483647, 0xffffffff=bitrate; cpb_max_rate: 0=auto, 1-2147483647\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_HRD | VPAP_ENC_RC_EXT_CPB;
	video_params[channel].rc_ext.hrd = enable;
	video_params[channel].rc_ext.hrdCpbSize = cpb_size;
	video_params[channel].rc_ext.cpbMaxRate = cpb_max_rate;
	RTK_LOGI(TAG, "+VIDHRD:%d,%u,%u,%u\r\n", channel, (unsigned int)enable,
			 (unsigned int)cpb_size, (unsigned int)cpb_max_rate);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDBRWIN(u16 argc, char **argv)
{
	int channel;
	uint32_t frames;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_u32(argv[2], 1, VIDEO_TEST_MAX_BITRATE_WIN, &frames) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDBRWIN=<ch0-2>,<frames> (1-300)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_BITRATE_WINDOW;
	video_params[channel].rc_ext.bitrateWindow = frames;
	RTK_LOGI(TAG, "+VIDBRWIN:%d,%u\r\n", channel, (unsigned int)frames);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDMVBR(u16 argc, char **argv)
{
	int channel;
	int32_t tolerance;
	int32_t monitor_frames;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 4, argv, &channel) != 0 ||
		video_parse_i32(argv[2], 0, VIDEO_TEST_MAX_MOVING_TOL, &tolerance) != 0 ||
		video_parse_i32(argv[3], VIDEO_TEST_MIN_MONITOR_FRM, VIDEO_TEST_MAX_MONITOR_FRM, &monitor_frames) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDMVBR=<ch0-2>,<tolerance_percent>,<monitor_frames>\r\n");
		RTK_LOGI(TAG, "tolerance: 0-2000, monitor_frames: 10-120\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_MOVING_BITRATE;
	video_params[channel].rc_ext.tolMovingBitRate = tolerance;
	video_params[channel].rc_ext.monitorFrames = monitor_frames;
	RTK_LOGI(TAG, "+VIDMVBR:%d,%d,%d\r\n", channel, (int)tolerance, (int)monitor_frames);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDFILL(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDFILL=<ch0-2>,<0|1>\r\n");
		RTK_LOGI(TAG, "enabling filler requires VIDPICRC=1 and a nonzero VIDHRD CPB size\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_FILLER_DATA;
	video_params[channel].rc_ext.fillerData = enable;
	RTK_LOGI(TAG, "+VIDFILL:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDCRF(u16 argc, char **argv)
{
	int channel;
	int32_t crf;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_i32(argv[2], -1, VIDEO_TEST_MAX_QP, &crf) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDCRF=<ch0-2>,<crf> (-1=disabled, 0-51=enabled)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_CRF;
	video_params[channel].rc_ext.crf = crf;
	RTK_LOGI(TAG, "+VIDCRF:%d,%d\r\n", channel, (int)crf);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDVBR(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDVBR=<ch0-2>,<0|1> (cannot enable with HRD)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_VBR;
	video_params[channel].rc_ext.vbr = enable;
	RTK_LOGI(TAG, "+VIDVBR:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDRCFPS(u16 argc, char **argv)
{
	int channel;
	uint32_t numerator;
	uint32_t denominator;
	int error = VIDEO_AT_ERR_PARAM;

	/* The encoder stores frameRateDenom in its signed outRateDenom field. */
	if (video_prepare_advanced_command(argc, 4, argv, &channel) != 0 ||
		video_parse_u32(argv[2], 1, VIDEO_TEST_MAX_RC_FPS_NUM, &numerator) != 0 ||
		video_parse_u32(argv[3], 1, INT_MAX, &denominator) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDRCFPS=<ch0-2>,<numerator>,<denominator>\r\n");
		RTK_LOGI(TAG, "numerator: 1-%u, denominator: 1-2147483647\r\n", (unsigned int)VIDEO_TEST_MAX_RC_FPS_NUM);
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].rc_ext.flags |= VPAP_ENC_RC_EXT_FRAME_RATE;
	video_params[channel].rc_ext.frameRateNum = numerator;
	video_params[channel].rc_ext.frameRateDenom = denominator;
	RTK_LOGI(TAG, "+VIDRCFPS:%d,%u,%u\r\n", channel, (unsigned int)numerator, (unsigned int)denominator);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDSLICE(u16 argc, char **argv)
{
	int channel;
	uint32_t rows;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 ||
		video_parse_u32(argv[2], 0, video_encoder_block_rows(channel), &rows) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDSLICE=<ch0-2>,<block_rows> (0=disabled)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].coding.flags |= VPAP_ENC_CODING_SLICE_SIZE;
	video_params[channel].coding.sliceSize = rows;
	RTK_LOGI(TAG, "+VIDSLICE:%d,%u\r\n", channel, (unsigned int)rows);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDSEI(u16 argc, char **argv)
{
	int channel;
	uint32_t driver_sei;
	uint32_t user_sei;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 4, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &driver_sei) != 0 ||
		video_parse_u32(argv[3], 0, 1, &user_sei) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDSEI=<ch0-2>,<driver_sei>,<user_data_sei> (0|1)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params_set_driver_sei(&video_params[channel], driver_sei);
	video_params_set_sei_user_data(&video_params[channel], user_sei);
	RTK_LOGI(TAG, "+VIDSEI:%d,%u,%u\r\n", channel, (unsigned int)driver_sei, (unsigned int)user_sei);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDFULLR(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDFULLR=<ch0-2>,<0|1> (0=limited VUI range, 1=full VUI range)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].coding.flags |= VPAP_ENC_CODING_FULL_RANGE;
	video_params[channel].coding.vuiVideoFullRange = enable;
	RTK_LOGI(TAG, "+VIDFULLR:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDDBLK(u16 argc, char **argv)
{
	int channel;
	uint32_t disable;
	int32_t tc_offset;
	int32_t beta_offset;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 5, argv, &channel) != 0 || video_parse_u32(argv[2], 0, 1, &disable) != 0 ||
		video_parse_i32(argv[3], -6, 6, &tc_offset) != 0 || video_parse_i32(argv[4], -6, 6, &beta_offset) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDDBLK=<ch0-2>,<disable>,<tc_offset>,<beta_offset>\r\n");
		RTK_LOGI(TAG, "disable: 0=deblock on, 1=deblock off; offsets: -6 to 6\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].coding.flags |= VPAP_ENC_CODING_DEBLOCK;
	video_params[channel].coding.disableDeblockingFilter = disable;
	video_params[channel].coding.tcOffset = tc_offset;
	video_params[channel].coding.betaOffset = beta_offset;
	RTK_LOGI(TAG, "+VIDDBLK:%d,%u,%d,%d\r\n", channel, (unsigned int)disable, (int)tc_offset, (int)beta_offset);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDSAO(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_params[channel].format != VIDEO_HEVC ||
		video_parse_u32(argv[2], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDSAO=<HEVC ch0-2>,<0|1>\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params[channel].coding.flags |= VPAP_ENC_CODING_SAO;
	video_params[channel].coding.enableSao = enable;
	RTK_LOGI(TAG, "+VIDSAO:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDCABAC(u16 argc, char **argv)
{
	int channel;
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 3, argv, &channel) != 0 || video_params[channel].format != VIDEO_H264 ||
		video_parse_u32(argv[2], 0, 1, &enable) != 0 ||
		(enable && video_params[channel].profile_set && video_params[channel].profile == VIDEO_TEST_H264_MIN_PROFILE)) {
		RTK_LOGI(TAG, "usage: AT+VIDCABAC=<H264 ch0-2>,<0|1> (baseline profile cannot enable CABAC)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params_set_cabac(&video_params[channel], enable);
	RTK_LOGI(TAG, "+VIDCABAC:%d,%u\r\n", channel, (unsigned int)enable);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDSAR(u16 argc, char **argv)
{
	int channel;
	uint32_t width;
	uint32_t height;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 4, argv, &channel) != 0 ||
		video_parse_u32(argv[2], 0, VIDEO_TEST_MAX_SAR, &width) != 0 ||
		video_parse_u32(argv[3], 0, VIDEO_TEST_MAX_SAR, &height) != 0 || ((width == 0) != (height == 0))) {
		RTK_LOGI(TAG, "usage: AT+VIDSAR=<ch0-2>,<width>,<height> (0,0=unspecified; otherwise 1-65535)\r\n");
		if (video_test_state != VIDEO_TEST_STOPPED) {
			error = VIDEO_AT_ERR_STATE;
		}
		goto end;
	}

	video_params_set_sar(&video_params[channel], width, height);
	RTK_LOGI(TAG, "+VIDSAR:%d,%u,%u\r\n", channel, (unsigned int)width, (unsigned int)height);
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDCIR(u16 argc, char **argv)
{
	int channel;
	uint32_t start;
	uint32_t interval;
	uint32_t max_blocks;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 4, argv, &channel) != 0) {
		goto invalid;
	}
	max_blocks = video_encoder_block_columns(channel) * video_encoder_block_rows(channel);
	if (video_parse_u32(argv[2], 0, max_blocks, &start) != 0 || video_parse_u32(argv[3], 0, max_blocks, &interval) != 0) {
		goto invalid;
	}

	video_params[channel].coding.flags |= VPAP_ENC_CODING_CIR;
	video_params[channel].coding.cirStart = start;
	video_params[channel].coding.cirInterval = interval;
	RTK_LOGI(TAG, "+VIDCIR:%d,%u,%u\r\n", channel, (unsigned int)start, (unsigned int)interval);
	error = VIDEO_AT_OK;
	goto end;

invalid:
	RTK_LOGI(TAG, "usage: AT+VIDCIR=<ch0-2>,<start>,<interval> (0-current encoder block count)\r\n");
	if (video_test_state != VIDEO_TEST_STOPPED) {
		error = VIDEO_AT_ERR_STATE;
	}
end:
	video_at_finish(error);
}

void fVIDROI(u16 argc, char **argv)
{
	int channel;
	uint32_t index;
	uint32_t enable;
	uint32_t top;
	uint32_t left;
	uint32_t bottom;
	uint32_t right;
	int32_t delta_qp;
	int32_t qp;
	uint32_t block_columns;
	uint32_t block_rows;
	vpap_shared_enc_roi_t *roi;
	int error = VIDEO_AT_ERR_PARAM;

	if (video_prepare_advanced_command(argc, 10, argv, &channel) != 0) {
		goto invalid;
	}
	block_columns = video_encoder_block_columns(channel);
	block_rows = video_encoder_block_rows(channel);
	if (video_parse_u32(argv[2], 0, VPAP_ENC_MAX_ROI - 1, &index) != 0 || video_parse_u32(argv[3], 0, 1, &enable) != 0 ||
		video_parse_u32(argv[4], 0, block_rows - 1, &top) != 0 || video_parse_u32(argv[5], 0, block_columns - 1, &left) != 0 ||
		video_parse_u32(argv[6], 0, block_rows - 1, &bottom) != 0 || video_parse_u32(argv[7], 0, block_columns - 1, &right) != 0 ||
		video_parse_i32(argv[8], -VIDEO_TEST_MAX_QP, VIDEO_TEST_MAX_QP, &delta_qp) != 0 ||
		video_parse_i32(argv[9], -1, VIDEO_TEST_MAX_QP, &qp) != 0 || top > bottom || left > right) {
		goto invalid;
	}

	roi = &video_params[channel].coding.roi[index];
	video_params[channel].coding.flags |= VPAP_ENC_CODING_ROI;
	roi->enable = enable;
	roi->top = top;
	roi->left = left;
	roi->bottom = bottom;
	roi->right = right;
	roi->deltaQp = delta_qp;
	roi->qp = qp;
	RTK_LOGI(TAG, "+VIDROI:%d,%u,%u,%u,%u,%u,%u,%d,%d\r\n", channel, (unsigned int)index, (unsigned int)enable,
			 (unsigned int)top, (unsigned int)left, (unsigned int)bottom, (unsigned int)right, (int)delta_qp, (int)qp);
	error = VIDEO_AT_OK;
	goto end;

invalid:
	RTK_LOGI(TAG, "usage: AT+VIDROI=<ch0-2>,<idx0-7>,<en>,<top>,<left>,<bottom>,<right>,<delta_qp>,<qp>\r\n");
	RTK_LOGI(TAG, "coordinates use the current HEVC CTB or H264 MB grid; delta_qp=-51..51, qp=-1 (use delta) or 0..51 (absolute)\r\n");
	if (video_test_state != VIDEO_TEST_STOPPED) {
		error = VIDEO_AT_ERR_STATE;
	}
end:
	video_at_finish(error);
}

void fVIDXCLR(u16 argc, char **argv)
{
	int channel;
	int clear_profile;
	int clear_rc;
	int clear_coding;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 3 || video_require_stopped() != 0 || video_parse_channel(argv[1], &channel) != 0 ||
		!video_channel_has_encoder(channel)) {
		goto invalid;
	}
	clear_profile = _stricmp(argv[2], "PROFILE") == 0 || _stricmp(argv[2], "ALL") == 0;
	clear_rc = _stricmp(argv[2], "RC") == 0 || _stricmp(argv[2], "ALL") == 0;
	clear_coding = _stricmp(argv[2], "CODING") == 0 || _stricmp(argv[2], "ALL") == 0;
	if (!clear_profile && !clear_rc && !clear_coding) {
		goto invalid;
	}

	if (clear_profile) {
		video_params[channel].profile = 0;
		video_params[channel].level = 0;
		video_params[channel].tier = 0;
		video_params[channel].profile_set = 0;
		video_params[channel].level_set = 0;
		video_params[channel].tier_set = 0;
	}
	if (clear_rc) {
		video_params[channel].rc_ext = (vpap_shared_enc_rc_ext_t) {
			0
		};
	}
	if (clear_coding) {
		video_params[channel].coding = (vpap_shared_enc_coding_t) {
			0
		};
	}
	RTK_LOGI(TAG, "+VIDXCLR:%d,%s\r\n", channel, argv[2]);
	error = VIDEO_AT_OK;
	goto end;

invalid:
	RTK_LOGI(TAG, "usage: AT+VIDXCLR=<ch0-2>,<PROFILE|RC|CODING|ALL>\r\n");
	if (video_test_state != VIDEO_TEST_STOPPED) {
		error = VIDEO_AT_ERR_STATE;
	}
end:
	video_at_finish(error);
}

void fVIDINFO(u16 argc, char **argv)
{
	int channel;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc == 1 || (argc == 2 && _stricmp(argv[1], "ALL") == 0)) {
		video_print_all_params();
		error = VIDEO_AT_OK;
	} else if (argc == 2 && video_parse_channel(argv[1], &channel) == 0) {
		video_print_channel_params(channel);
		error = VIDEO_AT_OK;
	} else {
		RTK_LOGI(TAG, "usage: AT+VIDINFO or AT+VIDINFO=<ch0-4|ALL>\r\n");
	}

	video_at_finish(error);
}

#ifdef CONFIG_BUILD_ISP_SRAM_TEST
void fVIDPIPE(u16 argc, char **argv)
{
	uint32_t mode;
	if (argc == 1) {
		RTK_LOGI(TAG, "+VIDPIPE:%u,%s\r\n", (unsigned int)video_params[0].isp_slice_en,
			video_params[0].isp_slice_en ? "SRAM_SLICE" : "FRAME");
		video_at_finish(VIDEO_AT_OK);
		return;
	}
	if (argc != 2 || video_parse_u32(argv[1], 0, 1, &mode) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDPIPE=<0=frame|1=SRAM slice>, before first start in this boot\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	if (video_require_stopped() != 0 || video_comparison_started) {
		RTK_LOGI(TAG, "Reboot, then select AT+VIDPIPE before starting another run\r\n");
		video_at_finish(VIDEO_AT_ERR_STATE);
		return;
	}
	video_params[0].isp_slice_en = mode;
	RTK_LOGI(TAG, "+VIDPIPE:%u,%s (start with AT+VIDTEST=1)\r\n", (unsigned int)mode,
		mode ? "SRAM_SLICE" : "FRAME");
	video_at_finish(VIDEO_AT_OK);
}
#endif

void fVIDTEST(u16 argc, char **argv)
{
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 2 || video_parse_u32(argv[1], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDTEST=<0|1>\r\n");
		goto end;
	}

	if (enable) {
		if (video_test_state != VIDEO_TEST_STOPPED) {
			RTK_LOGI(TAG, "video test cannot start because current state is %s\r\n", video_test_state_name());
			error = VIDEO_AT_ERR_STATE;
			goto end;
		}
		if (mmf2_video_cmd_test() != 0) {
			error = VIDEO_AT_ERR_START;
			goto end;
		}
	} else if (video_test_state == VIDEO_TEST_RUNNING) {
		if (video_test_deinit(0) != 0) {
			error = VIDEO_AT_ERR_IO;
			goto end;
		}
	} else if (video_test_state == VIDEO_TEST_STOPPED) {
		RTK_LOGI(TAG, "video test is already stopped\r\n");
	} else {
		RTK_LOGI(TAG, "video test cannot stop because current state is %s\r\n", video_test_state_name());
		error = VIDEO_AT_ERR_STATE;
		goto end;
	}

	RTK_LOGI(TAG, "+VIDTEST:%u,%s\r\n", (unsigned int)enable, video_test_state_name());
	error = VIDEO_AT_OK;

end:
	video_at_finish(error);
}

void fVIDNN(u16 argc, char **argv)
{
	uint32_t enable;
	if (argc == 1) {
		RTK_LOGI(TAG, "+VIDNN:%u (CH4/V5 YOLOv4-tiny)\r\n", (unsigned int)video_nn_enabled);
		video_at_finish(VIDEO_AT_OK);
		return;
	}
	if (argc != 2 || video_parse_u32(argv[1], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDNN=<0|1> (CH4/V5, stopped only)\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	if (video_require_stopped() != 0) {
		video_at_finish(VIDEO_AT_ERR_STATE);
		return;
	}
	if (video_nn_validate(enable) != 0) {
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	video_nn_enabled = enable;
	RTK_LOGI(TAG, "+VIDNN:%u (applied on next AT+VIDTEST=1)\r\n", (unsigned int)enable);
	video_at_finish(VIDEO_AT_OK);
}

void fVIDOUT(u16 argc, char **argv)
{
	uint32_t mode;
	if (argc == 1) {
		RTK_LOGI(TAG, "+VIDOUT:%u,%s (CH1/V2 only)\r\n", (unsigned int)video_preview_mode, video_preview_name());
		video_at_finish(VIDEO_AT_OK);
		return;
	}
	if (argc != 2 || video_parse_u32(argv[1], 0, 2, &mode) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDOUT=<0=none|1=RTSP|2=UVC> (CH1/V2, stopped only)\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	if (video_require_stopped() != 0) {
		video_at_finish(VIDEO_AT_ERR_STATE);
		return;
	}
	if (video_preview_validate(mode) != 0) {
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	video_preview_mode = mode;
	RTK_LOGI(TAG, "+VIDOUT:%u,%s (applied on next AT+VIDTEST=1)\r\n", (unsigned int)mode, video_preview_name());
	video_at_finish(VIDEO_AT_OK);
}

void fVIDREC(u16 argc, char **argv)
{
	uint32_t source;
	uint32_t seconds = video_record_seconds;

	if ((argc != 2 && argc != 3) || video_parse_u32(argv[1], 0, 2, &source) != 0 ||
		(argc == 3 && video_parse_u32(argv[2], 1, 60, &seconds) != 0) || (source && argc != 3)) {
		RTK_LOGI(TAG, "usage: AT+VIDREC=0 or AT+VIDREC=<1=V1|2=V2>,<1-60 seconds>\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	if (video_require_stopped() != 0) {
		video_at_finish(VIDEO_AT_ERR_STATE);
		return;
	}
	video_record_source = source;
	video_record_seconds = seconds;
	RTK_LOGI(TAG, "+VIDREC:%u,%u (new numbered MP4 and params.expected.json on SD)\r\n",
			 (unsigned int)source, (unsigned int)seconds);
	video_at_finish(VIDEO_AT_OK);
}

#if VIDEO_TEST_HTTP_ENABLED
static void video_http_home(struct httpd_conn *conn)
{
	static const char body[] = "<!doctype html><html><body><h1>Video test recording</h1>"
							   "<p>Latest finalized recording and matching settings (download both):</p>"
							   "<a href=\"/video/record.mp4\" download>Download MP4</a><br>"
							   "<a href=\"/video/params.expected.json\" download>Download parameters JSON</a>"
							   "<p>Stop HTTP with AT+VIDHTTP=0 before the next test.</p></body></html>";
	if (!httpd_request_is_method(conn, "GET")) {
		httpd_response_method_not_allowed(conn, NULL);
	} else {
		httpd_response_write_header_start(conn, "200 OK", "text/html", sizeof(body) - 1);
		httpd_response_write_header(conn, "Connection", "close");
		httpd_response_write_header(conn, "Cache-Control", "no-store");
		httpd_response_write_header_finish(conn);
		httpd_response_write_data(conn, (uint8_t *)body, sizeof(body) - 1);
	}
	httpd_conn_close(conn);
}

static void video_http_send_file(struct httpd_conn *conn, const char *path, const char *content_type)
{
	FILE *file = NULL;
	uint8_t *buffer = NULL;
	long length;
	size_t remaining;
	char disposition[96];
	int timeout = 3000;

	if (!httpd_request_is_method(conn, "GET")) {
		httpd_response_method_not_allowed(conn, NULL);
		goto end;
	}
	/* Serve only our finalized path, never a filename supplied by the client. */
	file = video_record_ready ? fopen(path, "rb") : NULL;
	if (!file) {
		httpd_response_not_found(conn, NULL);
		goto end;
	}
	if (fseek(file, 0, SEEK_END) != 0 || (length = ftell(file)) <= 0 || fseek(file, 0, SEEK_SET) != 0) {
		httpd_response_internal_server_error(conn, NULL);
		goto end;
	}
	buffer = malloc(4096);
	if (!buffer) {
		httpd_response_internal_server_error(conn, NULL);
		goto end;
	}
	setsockopt(conn->sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	snprintf(disposition, sizeof(disposition), "attachment; filename=\"%s\"", strrchr(path, '/') + 1);
	if (httpd_response_write_header_start(conn, "200 OK", content_type, (size_t)length) < 0 ||
		httpd_response_write_header(conn, "Content-Disposition", disposition) < 0 ||
		httpd_response_write_header(conn, "Cache-Control", "no-store") < 0 ||
		httpd_response_write_header(conn, "Connection", "close") < 0 ||
		httpd_response_write_header_finish(conn) < 0) {
		goto end;
	}
	remaining = (size_t)length;
	while (remaining && !video_http_stopping) {
		size_t wanted = remaining > 4096 ? 4096 : remaining;
		size_t count = fread(buffer, 1, wanted, file);
		if (count != wanted || httpd_response_write_data(conn, buffer, count) != (int)count) {
			RTK_LOGW(TAG, "File download interrupted or SD read failed\r\n");
			break;
		}
		remaining -= count;
	}
end:
	free(buffer);
	if (file) {
		fclose(file);
	}
	httpd_conn_close(conn);
}

static void video_http_download(struct httpd_conn *conn)
{
	video_http_send_file(conn, video_record_path, "video/mp4");
}

static void video_http_expected(struct httpd_conn *conn)
{
	video_http_send_file(conn, video_expected_path, "application/json");
}

#endif

void fVIDHTTP(u16 argc, char **argv)
{
	uint32_t enable;
	int error = VIDEO_AT_ERR_PARAM;

	if (argc != 2 || video_parse_u32(argv[1], 0, 1, &enable) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDHTTP=<0|1> (stop/start MP4 download server on port 80)\r\n");
		goto end;
	}
#if VIDEO_TEST_HTTP_ENABLED
	if (!enable) {
		if (video_http_owned) {
			/* Observe startup before requesting shutdown; otherwise a late task
			 * could start and exit between polls without ever being observed. */
			for (int i = 0; i < 100 && !video_http_started; i++) {
				video_http_started = httpd_is_running();
				if (!video_http_started) {
					vTaskDelay(pdMS_TO_TICKS(10));
				}
			}
			if (!video_http_started) {
				error = VIDEO_AT_ERR_STATE;
				goto end;
			}
			/* httpd_stop is asynchronous. Keep SD mounted until its task and
			 * the single-threaded download callback have both exited. */
			video_http_stopping = 1;
			httpd_stop();
			for (int i = 0; i < 100; i++) {
				if (httpd_is_running()) {
					video_http_started = 1;
				} else if (video_http_started) {
					break;
				}
				vTaskDelay(pdMS_TO_TICKS(50));
			}
			if (!video_http_started || httpd_is_running()) {
				RTK_LOGW(TAG, "HTTP still stopping; retry AT+VIDHTTP=0\r\n");
				error = VIDEO_AT_ERR_STATE;
				goto end;
			}
			video_http_owned = 0;
			video_http_started = 0;
		}
		error = VIDEO_AT_OK;
		goto end;
	}
	if (video_http_owned || httpd_is_running() || video_test_state != VIDEO_TEST_STOPPED) {
		RTK_LOGE(TAG, "stop the test and any existing HTTP server first\r\n");
		error = VIDEO_AT_ERR_STATE;
		goto end;
	}
	if (!video_record_ready) {
		RTK_LOGE(TAG, "no finalized MP4; record and stop with AT+VIDTEST=0 first\r\n");
		error = VIDEO_AT_ERR_IO;
		goto end;
	}
	video_http_stopping = 0;
	if (httpd_reg_page_callback("/video", video_http_home) != 0 ||
		httpd_reg_page_callback("/video/record.mp4", video_http_download) != 0 ||
		httpd_reg_page_callback("/video/params.expected.json", video_http_expected) != 0 ||
		httpd_start(80, 2, 4096, HTTPD_THREAD_SINGLE, HTTPD_SECURE_NONE) != 0) {
		httpd_clear_page_callbacks();
		error = VIDEO_AT_ERR_IO;
		goto end;
	}
	video_http_owned = 1;
	for (int i = 0; i < 100 && !httpd_is_running(); i++) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	video_http_started = httpd_is_running();
	if (!video_http_started) {
		RTK_LOGW(TAG, "HTTP startup pending; keep SD inserted and use AT+VIDHTTP=0 to stop\r\n");
		error = VIDEO_AT_ERR_STATE;
		goto end;
	}
	{
		uint8_t *ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
		RTK_LOGI(TAG, "MP4: %s; PC download: http://<device-ip>/video\r\n", video_record_path);
		if (ip && (ip[0] || ip[1] || ip[2] || ip[3])) {
			RTK_LOGI(TAG, "Wi-Fi URL: http://%u.%u.%u.%u/video\r\n", ip[0], ip[1], ip[2], ip[3]);
		} else {
			RTK_LOGI(TAG, "Connect Wi-Fi using AT+WLCONN=ssid,<ssid>,pw,<password> first\r\n");
		}
	}
	error = VIDEO_AT_OK;
#else
	(void)enable;
	RTK_LOGE(TAG, "HTTP requires the WLAN network/httpd configuration\r\n");
	error = VIDEO_AT_ERR_STATE;
#endif
end:
	video_at_finish(error);
}

void fVIDVALID(u16 argc, char **argv)
{
	(void)argv;
	if (argc != 1) {
		RTK_LOGI(TAG, "usage: AT+VIDVALID\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	video_at_finish(video_validate_selected_params() == 0 ? VIDEO_AT_OK : VIDEO_AT_ERR_PARAM);
}

void fVIDSTAT(u16 argc, char **argv)
{
	video_test_stats_t stats[VIDEO_TEST_CHANNEL_COUNT];
	int first = V1_STREAM_ID;
	int last = V5_STREAM_ID;
	int channel;
	TickType_t now;

	if (argc == 2 && video_parse_channel(argv[1], &first) == 0) {
		last = first;
	} else if (argc != 1) {
		RTK_LOGI(TAG, "usage: AT+VIDSTAT[=<ch0-4>]\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	video_stats_snapshot(stats);
	now = xTaskGetTickCount();
	RTK_LOGI(TAG, "Counters since last start; buffers are not necessarily complete frames (e.g. slices).\r\n");
	for (channel = first; channel <= last; channel++) {
		RTK_LOGI(TAG, "+VIDSTAT:%d,buffers=%u,kib=%u,invalid=%u,age_ms=%u\r\n", channel,
				 (unsigned int)stats[channel].buffers, (unsigned int)(stats[channel].bytes / 1024U),
				 (unsigned int)stats[channel].invalid,
				 stats[channel].buffers ? (unsigned int)((uint64_t)(TickType_t)(now - stats[channel].last_tick) * 1000U / configTICK_RATE_HZ) : 0U);
	}
	video_at_finish(VIDEO_AT_OK);
}

void fVIDCHECK(u16 argc, char **argv)
{
	video_test_stats_t before[VIDEO_TEST_CHANNEL_COUNT];
	video_test_stats_t after[VIDEO_TEST_CHANNEL_COUNT];
	uint32_t duration_ms;
	TickType_t start;
	TickType_t elapsed;
	int channel;
	int error = VIDEO_AT_OK;

	if (argc != 2 || video_parse_u32(argv[1], 100, 10000, &duration_ms) != 0) {
		RTK_LOGI(TAG, "usage: AT+VIDCHECK=<100-10000 ms>; use several frame periods\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	if (video_test_state != VIDEO_TEST_RUNNING) {
		video_at_finish(VIDEO_AT_ERR_STATE);
		return;
	}
	/* AT commands are dispatched serially; callbacks only update the counters. */
	video_stats_snapshot(before);
	start = xTaskGetTickCount();
	vTaskDelay(pdMS_TO_TICKS(duration_ms) + 1);
	video_stats_snapshot(after);
	elapsed = xTaskGetTickCount() - start;
	if (!elapsed) {
		video_at_finish(VIDEO_AT_ERR_DATA);
		return;
	}
	for (channel = V1_STREAM_ID; channel <= V5_STREAM_ID; channel++) {
		uint32_t buffers = after[channel].buffers - before[channel].buffers;
		uint32_t invalid = after[channel].invalid - before[channel].invalid;
		uint64_t bytes = after[channel].bytes - before[channel].bytes;
		int pass = buffers != 0 && invalid == 0;

		if (!video_channel_enabled[channel]) {
			continue;
		}
		RTK_LOGI(TAG, "+VIDCHECK:%d,%s,buffers=%u,invalid=%u,buffers_per_s=%u,kbps=%u\r\n",
				 channel, pass ? "PASS" : "FAIL", (unsigned int)buffers, (unsigned int)invalid,
				 (unsigned int)((uint64_t)buffers * configTICK_RATE_HZ / elapsed),
				 (unsigned int)(bytes * 8U * configTICK_RATE_HZ / elapsed / 1000U));
		if (!pass) {
			error = VIDEO_AT_ERR_DATA;
		}
	}
	RTK_LOGI(TAG, "PASS checks delivery only, not target FPS, image quality, or bitstream validity.\r\n");
	video_at_finish(error);
}

void fVIDIFR(u16 argc, char **argv)
{
	int channel;

	if (argc != 2 || video_parse_channel(argv[1], &channel) != 0 || !video_channel_supports_advanced_encoder(channel)) {
		RTK_LOGI(TAG, "usage: AT+VIDIFR=<H264/HEVC ch0-2>\r\n");
		video_at_finish(VIDEO_AT_ERR_PARAM);
		return;
	}
	if (video_test_state != VIDEO_TEST_RUNNING || !video_applied[channel] || !video_ctx[channel]) {
		video_at_finish(VIDEO_AT_ERR_STATE);
		return;
	}
	/* The video API returns no encoder acknowledgement; this is a request only. */
	mm_module_ctrl(video_ctx[channel], CMD_VIDEO_FORCE_IFRAME, 0);
	RTK_LOGI(TAG, "+VIDIFR:%d,REQUESTED\r\n", channel);
	video_at_finish(VIDEO_AT_OK);
}

ATCMD_TABLE_DATA_SECTION
const log_item_t at_video_cmd_test_items[] = {
	{"+VID?", fVIDHELP},         /* Show command usage and examples. */
	{"+VIDCHEN", fVIDCHEN},     /* Select channel pipelines. */
	{"+VIDFMT", fVIDFMT},       /* Set video channel format. */
	{"+VIDRES", fVIDRES},       /* Set video channel resolution. */
	{"+VIDFPS", fVIDFPS},       /* Set video channel frame rate. */
	{"+VIDGOP", fVIDGOP},       /* Set video channel GOP. */
	{"+VIDBPS", fVIDBPS},       /* Set video channel bit rate. */
	{"+VIDRCM", fVIDRCM},       /* Set video channel RC mode. */
	{"+VIDQP", fVIDQP},         /* Set video channel QP range. */
	{"+VIDQLVL", fVIDQLVL},     /* Set video channel JPEG qlevel. */
	{"+VIDAINR", fVIDAINR},     /* Set video AINR on/off. */
	{"+VIDPROF", fVIDPROF},     /* Set profile, level, and tier. */
	{"+VIDQPHDR", fVIDQPHDR},   /* Set initial encoder QP. */
	{"+VIDIQP", fVIDIQP},       /* Set I-frame QP range. */
	{"+VIDIQPD", fVIDIQPD},     /* Set intra QP delta. */
	{"+VIDFIQP", fVIDFIQP},     /* Set fixed intra QP. */
	{"+VIDPICRC", fVIDPICRC},   /* Set picture-level RC. */
	{"+VIDCTBRC", fVIDCTBRC},   /* Set CTB RC mode. */
	{"+VIDBLKRC", fVIDBLKRC},   /* Set RC block size. */
	{"+VIDPSKIP", fVIDPSKIP},   /* Set picture skipping. */
	{"+VIDHRD", fVIDHRD},       /* Set HRD and CPB. */
	{"+VIDBRWIN", fVIDBRWIN},   /* Set bitrate window. */
	{"+VIDMVBR", fVIDMVBR},     /* Set moving bitrate monitor. */
	{"+VIDFILL", fVIDFILL},     /* Set filler data. */
	{"+VIDCRF", fVIDCRF},       /* Set CRF. */
	{"+VIDVBR", fVIDVBR},       /* Set VBR extra. */
	{"+VIDRCFPS", fVIDRCFPS},   /* Set rational RC frame rate. */
	{"+VIDSLICE", fVIDSLICE},   /* Set slice size. */
	{"+VIDSEI", fVIDSEI},       /* Set driver and user SEI. */
	{"+VIDFULLR", fVIDFULLR},   /* Set VUI full range. */
	{"+VIDDBLK", fVIDDBLK},     /* Set deblocking control. */
	{"+VIDSAO", fVIDSAO},       /* Set HEVC SAO. */
	{"+VIDCABAC", fVIDCABAC},   /* Set H264 CABAC. */
	{"+VIDSAR", fVIDSAR},       /* Set sample aspect ratio. */
	{"+VIDCIR", fVIDCIR},       /* Set cyclic intra refresh. */
	{"+VIDROI", fVIDROI},       /* Set encoder ROI. */
	{"+VIDXCLR", fVIDXCLR},     /* Clear advanced parameters. */
	{"+VIDINFO", fVIDINFO},     /* Show one or all channel settings. */
#ifdef CONFIG_BUILD_ISP_SRAM_TEST
	{"+VIDPIPE", fVIDPIPE},     /* Select frame/SRAM handoff before the first run. */
#endif
	{"+VIDTEST", fVIDTEST},     /* Start/stop the video test. */
	{"+VIDNN", fVIDNN},         /* Select optional V5 YOLOv4-tiny inference. */
	{"+VIDOUT", fVIDOUT},       /* Select V2 preview: none, RTSP or UVC. */
	{"+VIDREC", fVIDREC},       /* Select V1/V2 MP4 recording on SD. */
	{"+VIDHTTP", fVIDHTTP},     /* Serve the finalized recording over HTTP. */
	{"+VIDVALID", fVIDVALID},   /* Validate selected settings without starting hardware. */
	{"+VIDSTAT", fVIDSTAT},     /* Inspect delivered buffer counters. */
	{"+VIDCHECK", fVIDCHECK},   /* Check delivery during a bounded observation window. */
	{"+VIDIFR", fVIDIFR},       /* Request a runtime I-frame. */
};
