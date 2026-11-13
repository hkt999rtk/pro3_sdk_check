/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "mmf2_link.h"
#include "mmf2_mimo.h"
#include "module_video.h"
#include "module_rtsp2.h"
#include "module_mp4.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 HEVC video -> MP4 recording
 *   V2 H.264 video -> RTSP live stream
 *
 * MIMO keeps the recording and live-view paths independent. Update V1 params
 * for the recorded file and V2/RTSP params for the live stream.
 */

#if (!defined(CONFIG_LWIP_LAYER) || (CONFIG_LWIP_LAYER == 0))
#error "please enable CONFIG_LWIP_LAYER before using RTSP"
#endif

#define AINR_ENA    0   /* AI noise reduction; set 1 to enable */

static mm_context_t *video_v1_ctx          = NULL;
static mm_context_t *video_v2_ctx          = NULL;
static mm_context_t *mp4_ctx               = NULL;
static mm_context_t *rtsp_ctx              = NULL;
static mm_mimo_t    *mimo_v1v2_mp4_rtsp    = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format    = VIDEO_HEVC,
	.width     = 2688,
	.height    = 1520,
	.fps       = 15,
	.gop       = 15,
	.bps       = 2000000,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
	.qlevel    = 5,
	.ainr_en   = AINR_ENA,
};

static video_params_t video_v2_params = {
	.stream_id = V2_STREAM_ID,
	.format    = VIDEO_H264,
	.width     = 1280,
	.height    = 720,
	.fps       = 30,
	.gop       = 30,
	.bps       = 500000,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
};

static mp4_params_t mp4_params = {
	.record_length    = 10,
	.record_type      = STORAGE_VIDEO,
	.record_file_num  = 3,
	.record_file_name = "AmebaPro_recording",
	.fatfs_buf_size   = 224 * 1024, /* 32kb multiple */
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps      = 30,
			.bps      = 0, // if unknown
		}
	}
};

/* Called from the RTSP service task on every RTSP PLAY request. Forcing an I frame
 * here removes the up-to-one-GOP wait before the client gets a reference frame.
 * The rtsp2 module drops frames until that I frame actually arrives, so the first
 * frame the client receives is always an I frame. V2 is the stream fed to RTSP. */
static int prv_rtsp_play_cb(void *p)
{
	(void)p;
	RTK_LOGI(TAG, "RTSP play: force I frame on ch%d\r\n", V2_STREAM_ID);
	mm_module_ctrl(video_v2_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	return 0;
}

void mmf2_video_example_v1v2_mp4_rtsp_init(void)
{
	/* V1 ********************************************************/
	/* V1 is the high-resolution recording stream. CMD_VIDEO_APPLY starts
	 * the stream after params and queue settings are ready. */
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v1 open fail\n\r");
		goto error;
	}

	/* V2 ********************************************************/
	/* V2 is the live-view stream, tuned independently from V1 so RTSP can
	 * use a lower bitrate/resolution than the recording path. */
	video_v2_ctx = mm_module_open(&video_module);
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v2_params);
		mm_module_ctrl(video_v2_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v2 open fail\n\r");
		goto error;
	}

	/* MP4 *******************************************************/
	mp4_params.fps    = video_v1_params.fps;
	mp4_params.gop    = video_v1_params.gop;
	mp4_params.width  = video_v1_params.width;
	mp4_params.height = video_v1_params.height;
	mp4_ctx = mm_module_open(&mp4_module);
	if (mp4_ctx) {
		mm_module_ctrl(mp4_ctx, CMD_MP4_SET_PARAMS, (int)&mp4_params);
		mm_module_ctrl(mp4_ctx, CMD_MP4_LOOP_MODE, 0);
		mm_module_ctrl(mp4_ctx, CMD_MP4_START, mp4_params.record_file_num);
	} else {
		RTK_LOGE(TAG, "mp4 open fail\n\r");
		goto error;
	}

	/* RTSP ******************************************************/
	rtsp_ctx = mm_module_open(&rtsp2_module);
	if (rtsp_ctx) {
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 0);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v_params);
		/* Force an I frame when a client sends RTSP PLAY so the stream starts from
		 * a reference frame instead of waiting for the next GOP. */
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, ON);
	} else {
		RTK_LOGE(TAG, "RTSP2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "RTSP module opened\n\r");

	/* MIMO V1->MP4, V2->RTSP ************************************/
	/* MIMO routes multiple inputs to multiple outputs. Dependency flags keep
	 * MP4 consuming V1 and RTSP consuming V2. */
	mimo_v1v2_mp4_rtsp = mimo_create();
	if (mimo_v1v2_mp4_rtsp) {
		mimo_ctrl(mimo_v1v2_mp4_rtsp, MMIC_CMD_ADD_INPUT0, (uint32_t)video_v1_ctx, 0);
		mimo_ctrl(mimo_v1v2_mp4_rtsp, MMIC_CMD_ADD_INPUT1, (uint32_t)video_v2_ctx, 0);
		mimo_ctrl(mimo_v1v2_mp4_rtsp, MMIC_CMD_ADD_OUTPUT0, (uint32_t)mp4_ctx,  MMIC_DEP_INPUT0);
		mimo_ctrl(mimo_v1v2_mp4_rtsp, MMIC_CMD_ADD_OUTPUT1, (uint32_t)rtsp_ctx, MMIC_DEP_INPUT1);
		mimo_start(mimo_v1v2_mp4_rtsp);
	} else {
		RTK_LOGE(TAG, "mimo open fail\n\r");
		goto error;
	}

	return;
error:
	return;
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1v2_mp4_rtsp_init[] = {
	{
		.v_func      = mmf2_video_example_v1v2_mp4_rtsp_init,
		.v_func_name = "mmf2_video_example_v1v2_mp4_rtsp_init",
	}
};
