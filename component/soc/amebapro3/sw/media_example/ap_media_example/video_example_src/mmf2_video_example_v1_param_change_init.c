/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "ameba_soc.h"

#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_video.h"
#include "module_rtsp2.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

static mm_context_t *video_v1_ctx       = NULL;
static mm_context_t *rtsp2_v1_ctx       = NULL;
static mm_siso_t *siso_video_rtsp_v1    = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_H264,
	.width = 1280,
	.height = 720,
	.fps = 30,
	.gop = 30,
	.bps = 500000,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
};

static rtsp2_params_t rtsp2_v1_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.bps = 2000000
		}
	}
};

static void change_parameter(int parm_index)
{
	if (parm_index == 0) {
		video_v1_params.format = VIDEO_HEVC;
		video_v1_params.width = 1920;
		video_v1_params.height = 1080;
		video_v1_params.fps = 15;
		video_v1_params.gop = 15;
		video_v1_params.bps = 512 * 1024;
		video_v1_params.rc_mode = ENC_CBR;
		video_v1_params.min_qp = 35;
		video_v1_params.max_qp = 51;
	} else {
		video_v1_params.format = VIDEO_H264;
		video_v1_params.width = 1280;
		video_v1_params.height = 720;
		video_v1_params.fps = 30;
		video_v1_params.gop = 30;
		video_v1_params.bps = 1 * 1024 * 1024;
		video_v1_params.rc_mode = ENC_VBR;
		video_v1_params.min_qp = 26;
		video_v1_params.max_qp = 48;
	}

	/* update rtsp parameter */
	rtsp2_v1_params.u.v.fps = video_v1_params.fps;
	rtsp2_v1_params.u.v.bps = video_v1_params.bps;
}

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

static void mmf2_video_module_linker_start(void)
{
	/*rtsp parameter setting*/
	rtsp2_v1_params.u.v.codec_id = (video_v1_params.format == VIDEO_HEVC) ? AV_CODEC_ID_H265 : AV_CODEC_ID_H264;
	rtsp2_v1_params.u.v.fps = video_v1_params.fps;
	rtsp2_v1_params.u.v.bps = video_v1_params.bps;

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
	RTK_LOGI(TAG, "VIDEO module opened\n\r");

	rtsp2_v1_ctx = mm_module_open(&rtsp2_module);
	if (rtsp2_v1_ctx) {
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SELECT_STREAM, 0);
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v1_params);
		/* Force an I frame when a client sends RTSP PLAY so the stream starts from
		 * a reference frame instead of waiting for the next GOP. */
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_APPLY, 0);
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_STREAMMING, ON);
	} else {
		RTK_LOGE(TAG, "RTSP2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "RTSP module opened\n\r");

	siso_video_rtsp_v1 = siso_create();
	if (siso_video_rtsp_v1) {
		siso_ctrl(siso_video_rtsp_v1, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		siso_ctrl(siso_video_rtsp_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp2_v1_ctx, 0);
		siso_start(siso_video_rtsp_v1);
	} else {
		RTK_LOGE(TAG, "siso2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v1 started\n\r");

	return;
error:
	return;
}

static void mmf2_video_module_linker_stop(void)
{
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_video_rtsp_v1) {
		siso_stop(siso_video_rtsp_v1);
		siso_delete(siso_video_rtsp_v1);
		siso_video_rtsp_v1 = NULL;
	}
	if (rtsp2_v1_ctx) {
		mm_module_ctrl(rtsp2_v1_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
		mm_module_close(rtsp2_v1_ctx);
		rtsp2_v1_ctx = NULL;
	}
	if (video_v1_ctx) {
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
	}
}

void mmf2_video_example_v1_param_change_init(void)
{
	mmf2_video_module_linker_start();

	/* change parameter test */
	for (int i = 0; i < 30; i++) {

		rtos_time_delay_ms(30 * 1000);
		mmf2_video_module_linker_stop();

		int param_idx = i % 2;
		change_parameter(param_idx);
		mmf2_video_module_linker_start();
	}

	return;
error:

	return;
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1_param_change_init[] = {
	{
		.v_func = mmf2_video_example_v1_param_change_init,
		.v_func_name = "mmf2_video_example_v1_param_change_init",
	}
};
