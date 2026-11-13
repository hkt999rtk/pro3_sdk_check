/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "ameba_soc.h"
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_array.h"
#include "module_encoder.h"
#include "module_vp2ap.h"
#include "avcodec.h"

#include "video_app_param_config.h"

/* NV12 video resolution */
#define VIDEO_V1_WIDTH  2160
#define VIDEO_V1_HEIGHT 3840
#define VIDEO_V1_FPS    20

#define VIDEO_V2_WIDTH  1440
#define VIDEO_V2_HEIGHT 2560
#define VIDEO_V2_FPS    20

/* video channel enable option */
#define V1_ENA	1
#define V2_ENA	0

static array_params_t array_v1_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_NV12,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps = VIDEO_V1_FPS,
		}
	}
};

static array_params_t array_v2_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_NV12,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps = VIDEO_V2_FPS,
		}
	}
};

static vp2ap_params_t vp2ap_v1_params = {
	.stream_id = V1_STREAM_ID,
	.send_ap_off = 1
};

static vp2ap_params_t vp2ap_v2_params = {
	.stream_id = V2_STREAM_ID,
	.send_ap_off = 1
};

static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};

static mm_context_t *array_v1_ctx   = NULL;
static mm_context_t *array_v2_ctx   = NULL;
static mm_context_t *enc_v1_ctx     = NULL;
static mm_context_t *enc_v2_ctx     = NULL;
static mm_context_t *vp2ap_v1_ctx   = NULL;
static mm_context_t *vp2ap_v2_ctx   = NULL;
static mm_siso_t *siso_array_enc_v1 = NULL;
static mm_siso_t *siso_array_enc_v2 = NULL;
static mm_siso_t *siso_enc_vp2ap_v1 = NULL;
static mm_siso_t *siso_enc_vp2ap_v2 = NULL;

static void mmf2_video_enc_param_setup(void)
{
	default_video_param(isp_params, enc_params);
#if V1_ENA
	enc_params[V1_STREAM_ID].width = VIDEO_V1_WIDTH;
	enc_params[V1_STREAM_ID].height = VIDEO_V1_HEIGHT;
	enc_params[V1_STREAM_ID].fps = VIDEO_V1_FPS;
	enc_params[V1_STREAM_ID].gop = VIDEO_V1_FPS;
	enc_params[V1_STREAM_ID].bps = 2000000;
	enc_params[V1_STREAM_ID].rcMode = ENC_VBR;
	enc_params[V1_STREAM_ID].minQp = 26;
	enc_params[V1_STREAM_ID].maxQp = 48;
	enc_params[V1_STREAM_ID].codec_fmt = VIDEO_H264;
#endif

#if V2_ENA
	enc_params[V2_STREAM_ID].width = VIDEO_V2_WIDTH;
	enc_params[V2_STREAM_ID].height = VIDEO_V2_HEIGHT;
	enc_params[V2_STREAM_ID].fps = VIDEO_V2_FPS;
	enc_params[V2_STREAM_ID].gop = VIDEO_V2_FPS;
	enc_params[V2_STREAM_ID].bps = 2000000;
	enc_params[V2_STREAM_ID].rcMode = ENC_VBR;
	enc_params[V2_STREAM_ID].minQp = 26;
	enc_params[V2_STREAM_ID].maxQp = 48;
	enc_params[V2_STREAM_ID].codec_fmt = VIDEO_H264;
#endif
	video_enc_buf_pool_setup(enc_params);
	return;
}

void mmf2_video_example_array_encoder_init(char *user_cmd)
{
	(void)user_cmd;

	mmf2_video_enc_param_setup();

#if V1_ENA
	/* ARRAY */
	array_t array_v1;
	array_v1.data_len = (uint32_t) VIDEO_V1_WIDTH * VIDEO_V1_HEIGHT * 1.5;
	array_v1.data_addr = (uint32_t) malloc(array_v1.data_len);
	if (array_v1.data_addr == NULL) {
		printf("error: fail to alloc memory for array_v1 module input \n\r");
		goto error;
	}
	memset(array_v1.data_addr, 0, array_v1.data_len);
	DCache_CleanInvalidate((u32)array_v1.data_addr, array_v1.data_len);
	array_v1_ctx = mm_module_open(&array_module);
	if (array_v1_ctx) {
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_SET_PARAMS, (int)&array_v1_params);
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_SET_ARRAY, (int)&array_v1);
		mm_module_ctrl(array_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	/* ENC module init */
	enc_v1_ctx = mm_module_open(&enc_module);
	if (enc_v1_ctx) {
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V1_STREAM_ID]);
		mm_module_ctrl(enc_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
		mm_module_ctrl(enc_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
	printf("enc_v1_ctx open success\n\r");

	vp2ap_v1_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v1_ctx) {
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v1_params);
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
		goto error;
	}

	//--------------Link---------------------------
	siso_array_enc_v1 = siso_create();
	if (siso_array_enc_v1) {
		siso_ctrl(siso_array_enc_v1, MMIC_CMD_ADD_INPUT, (uint32_t)array_v1_ctx, 0);
		siso_ctrl(siso_array_enc_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v1_ctx, 0);
		siso_start(siso_array_enc_v1);
	} else {
		printf("siso_array_enc_v1 open fail\n\r");
		goto error;
	}
	printf("siso_array_enc_v1 started\n\r");

	siso_enc_vp2ap_v1 = siso_create();
	if (siso_enc_vp2ap_v1) {
		siso_ctrl(siso_enc_vp2ap_v1, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v1_ctx, 0);
		siso_ctrl(siso_enc_vp2ap_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v1_ctx, 0);
		siso_start(siso_enc_vp2ap_v1);
	} else {
		printf("siso_enc_vp2ap_v1 open fail\n\r");
		goto error;
	}
	printf("siso_enc_vp2ap_v1 start success\n\r");
#endif

#if V2_ENA
	/* ARRAY */
	array_t array_v2;
	array_v2.data_len = (uint32_t) VIDEO_V2_WIDTH * VIDEO_V2_HEIGHT * 1.5;
	array_v2.data_addr = (uint32_t) malloc(array_v2.data_len);
	if (array_v2.data_addr == NULL) {
		printf("error: fail to alloc memory for array_v2 module input \n\r");
		goto error;
	}
	memset(array_v2.data_addr, 0, array_v2.data_len);
	DCache_CleanInvalidate((u32)array_v2.data_addr, array_v2.data_len);
	array_v2_ctx = mm_module_open(&array_module);
	if (array_v2_ctx) {
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_SET_PARAMS, (int)&array_v2_params);
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_SET_ARRAY, (int)&array_v2);
		mm_module_ctrl(array_v2_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	/* ENC module init */
	enc_v2_ctx = mm_module_open(&enc_module);
	if (enc_v2_ctx) {
		mm_module_ctrl(enc_v2_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V2_STREAM_ID]);
		mm_module_ctrl(enc_v2_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
		mm_module_ctrl(enc_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_v2_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_v2_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
	printf("enc_v2_ctx open success\n\r");

	vp2ap_v2_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v2_ctx) {
		mm_module_ctrl(vp2ap_v2_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v2_params);
		mm_module_ctrl(vp2ap_v2_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
		goto error;
	}

	//--------------Link---------------------------
	siso_array_enc_v2 = siso_create();
	if (siso_array_enc_v2) {
		siso_ctrl(siso_array_enc_v2, MMIC_CMD_ADD_INPUT, (uint32_t)array_v2_ctx, 0);
		siso_ctrl(siso_array_enc_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v2_ctx, 0);
		siso_start(siso_array_enc_v2);
	} else {
		printf("siso_array_enc_v2 open fail\n\r");
		goto error;
	}
	printf("siso_array_enc_v2 started\n\r");

	siso_enc_vp2ap_v2 = siso_create();
	if (siso_enc_vp2ap_v2) {
		siso_ctrl(siso_enc_vp2ap_v2, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v2_ctx, 0);
		siso_ctrl(siso_enc_vp2ap_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v2_ctx, 0);
		siso_start(siso_enc_vp2ap_v2);
	} else {
		printf("siso_enc_vp2ap_v2 open fail\n\r");
		goto error;
	}
	printf("siso_enc_vp2ap_v2 start success\n\r");
#endif

	return;
error:

	return;
}

