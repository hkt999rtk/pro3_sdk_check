/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "mmf2_dbg.h"
#include "module_array.h"
#include "module_uvcd.h"
#include "avcodec.h"
#include "sample_h264.h"
#include "video_app_param_config.h"
#include "module_encoder.h"
#include "module_isp.h"
#include "module_array.h"
#include "video_app_param_config.h"
#include "video_common_type.h"

static array_params_t h264_array_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_H264,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps    = 25,
			.h264_nal_size = 4,
		}
	}
};

static mm_context_t *array_ctx      = NULL;
static mm_context_t *uvcd_ctx       = NULL;
static mm_siso_t *siso_array_uvcd   = NULL;

void mmf2_video_example_uvcd_array_init(char *user_cmd)
{
	(void)user_cmd;
	DBG_MMF_INFO_MSG_OFF(0xffff);
	/* ARRAY */
	array_t array;

	array.data_addr = (uint32_t) h264_sample;
	array.data_len = (uint32_t) h264_sample_len;

	array_ctx = mm_module_open(&array_module);
	if (array_ctx) {
		mm_module_ctrl(array_ctx, CMD_ARRAY_SET_PARAMS, (int)&h264_array_params);
		mm_module_ctrl(array_ctx, CMD_ARRAY_SET_ARRAY, (int)&array);
		mm_module_ctrl(array_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	uvcd_ctx = mm_module_open(&uvcd_module);
	if (uvcd_ctx) {

	} else {
		printf("uvc open fail\n\r");
		goto error;
	}


	//--------------Link---------------------------
	siso_array_uvcd = siso_create();
	if (siso_array_uvcd) {
		siso_ctrl(siso_array_uvcd, MMIC_CMD_ADD_INPUT, (uint32_t)array_ctx, 0);
		siso_ctrl(siso_array_uvcd, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		siso_start(siso_array_uvcd);
	} else {
		printf("siso_array_uvcd open fail\n\r");
		goto error;
	}
	printf("siso_array_uvcd started\n\r");

	return;
error:

	return;
}
