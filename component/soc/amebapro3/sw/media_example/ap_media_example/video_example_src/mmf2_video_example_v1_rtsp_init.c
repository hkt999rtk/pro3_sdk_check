/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
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
 *   V1 H.264 video -> RTSP
 *
 * Use this as the smallest live-view example. Change video_v1_params for the
 * camera stream and rtsp2_v_params when the RTSP metadata must match it.
 */

#if (!defined(CONFIG_LWIP_LAYER) || (CONFIG_LWIP_LAYER == 0))
#error "please enable CONFIG_LWIP_LAYER before using RTSP"
#endif

/* AI Denoise Option */
#define AINR_ENA    0   /* AI noise reduction on V1; set 1 to enable */

static mm_context_t *video_v1_ctx  = NULL;
static mm_context_t *rtsp_ctx      = NULL;
static mm_siso_t    *siso_v1_rtsp  = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format    = VIDEO_H264,
	.width     = 1920,
	.height    = 1080,
	.fps       = 30,
	.gop       = 30,
	.bps       = 2000000,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
	.ainr_en   = AINR_ENA,
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
 * frame the client receives is always an I frame. */
static int prv_rtsp_play_cb(void *p)
{
	(void)p;
	RTK_LOGI(TAG, "RTSP play: force I frame on ch%d\r\n", V1_STREAM_ID);
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	return 0;
}

void mmf2_video_example_v1_rtsp_init(void)
{
	/* V1 ********************************************************/
	/* video_module owns one hardware video stream. Set stream params and
	 * queue policy first; CMD_VIDEO_APPLY starts frame production. */
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

	/* SISO V1 -> RTSP *******************************************/
	/* SISO links one producer to one consumer. Frames from V1 are delivered
	 * to RTSP after siso_start(). */
	siso_v1_rtsp = siso_create();
	if (siso_v1_rtsp) {
		siso_ctrl(siso_v1_rtsp, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		siso_ctrl(siso_v1_rtsp, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx,     0);
		siso_start(siso_v1_rtsp);
	} else {
		RTK_LOGE(TAG, "siso_v1_rtsp create fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v1_rtsp started\n\r");

	return;
error:
	return;
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1_rtsp_init[] = {
	{
		.v_func      = mmf2_video_example_v1_rtsp_init,
		.v_func_name = "mmf2_video_example_v1_rtsp_init",
	}
};
