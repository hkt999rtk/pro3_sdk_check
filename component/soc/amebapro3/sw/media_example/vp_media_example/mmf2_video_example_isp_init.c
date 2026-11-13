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
#include "module_isp.h"
#include "module_vp2ap.h"
#include "avcodec.h"

#include "video_app_param_config.h"

static mm_context_t *isp_v1_ctx         = NULL;
static mm_context_t *vp2ap_v1_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v1     = NULL;

static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};
static isp_enc_ch_en_t v_ch_en = {{0}, {0}};

static vp2ap_params_t vp2ap_v1_params = {
	.stream_id = V1_STREAM_ID,
	.send_ap_off = 1
};

void mmf2_video_example_isp_init(char *user_cmd)
{
	DBG_MMF_INFO_MSG_OFF(0xffff);

	if (video_app_params_parsing(&v_ch_en, user_cmd, isp_params, enc_params) != 0) {
		printf("error: fail to parse video params\r\n");
		goto error;
	}

	/* ISP module init */
	isp_v1_ctx = mm_module_open(&isp_module);
	if (isp_v1_ctx) {
		mm_module_ctrl(isp_v1_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		//mm_module_ctrl(isp_v1_ctx, CMD_ISP_APPLY, 0);	// start channel 0
	} else {
		printf("isp_v1_ctx open fail\n\r");
		goto error;
	}
	printf("isp_v1_ctx open success\n\r");


	/* VP to AP */
	vp2ap_v1_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v1_ctx) {
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v1_params);
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap_v1_ctx open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	while (isp_v1_ctx == NULL || vp2ap_v1_ctx == NULL) {;}
	siso_isp_vp2ap_v1 = siso_create();
	if (siso_isp_vp2ap_v1) {
		siso_ctrl(siso_isp_vp2ap_v1, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v1_ctx, 0);
		siso_ctrl(siso_isp_vp2ap_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v1_ctx, 0);
		siso_start(siso_isp_vp2ap_v1);
	} else {
		printf("siso_isp_vp2ap_v1 open fail\n\r");
		goto error;
	}

	/* ISP module enable streaming channel */
	mm_module_ctrl(isp_v1_ctx, CMD_ISP_APPLY, 0);	// enable channel 0

	/* TODO: should be modified after isp control sw being ready
	   isp control sw should be able to use API control each channel on/off */
	extern void isp_start_streaming(void);
	isp_start_streaming();

error:
	return;
}
