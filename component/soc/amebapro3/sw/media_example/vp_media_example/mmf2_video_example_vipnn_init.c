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
#include "module_isp.h"
#include "module_vipnn.h"
#include "module_vp2ap.h"
#include "avcodec.h"

#include "video_app_param_config.h"
#include "model_yolo.h"
#include "model_scrfd.h"

/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m
 * please make sure the choosed model is also selected in amebapro2_fwfs_nn_models.json */
#define NN_MODEL_OBJ    scrfd
/* RGB video resolution
 * please make sure the resolution is matched to model input size and thereby to avoid SW image resizing */
#define NN_WIDTH	576
#define NN_HEIGHT	320

static array_params_t rgb_array_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_RGB888,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps    = 30,
		}
	}
};

static nn_data_param_t nn_input_params = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};
static isp_enc_ch_en_t v_ch_en = {{0}, {0}};

static mm_context_t *array_ctx      = NULL;
static mm_context_t *isp_v1_ctx     = NULL;
static mm_context_t *vipnn_ctx      = NULL;
static mm_context_t *vp2ap_v1_ctx   = NULL;
static mm_siso_t *siso_array_vipnn  = NULL;

#define ISP_ENA     0
#define NN_ENA      1

static uint32_t nn_fps = 0;
static uint32_t nn_ts = 0;
static void nn_display_cb(void *p, void *img_param)
{
	if (!p || !img_param)	{
		return;
	}

	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	facedetect_res_t *face_res = (facedetect_res_t *)&out->res[0];

	int obj_num = out->res_cnt;
	printf("object num = %d\r\n", obj_num);

	nn_fps++;
	if ((xTaskGetTickCount() - nn_ts) > 1000) {
		printf("NN fps=%lu\r\n", nn_fps);
		nn_fps = 0;
		nn_ts = xTaskGetTickCount();
	}
}

void mmf2_video_example_vipnn_init(char *user_cmd)
{
#if ISP_ENA
	char *v_cmd = "--ainr=0 "
				  "--isp0:w=2688,h=1520,fps=15,fmt=1 "
				  "--enc0:w=2688,h=1520,fps=15,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,fmt=2 "
				  "--isp4:w=576,h=320,fps=10,fmt=5 ";
	/* parsing video app params to isp/enc params */
	if (video_app_params_parsing(&v_ch_en, v_cmd, isp_params, enc_params) != 0) {
		printf("error: fail to parse video params\r\n");
		goto error;
	}

	/* ISP NV12 */
	isp_v1_ctx = mm_module_open(&isp_module);
	if (isp_v1_ctx) {
		mm_module_ctrl(isp_v1_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}
	printf("isp_v1_ctx open success\n\r");
#else
	/* ARRAY */
	array_t array;
	array.data_addr = (uint32_t) malloc(NN_WIDTH * NN_HEIGHT * 3);
	array.data_len = (uint32_t) NN_WIDTH * NN_HEIGHT * 3;
	array_ctx = mm_module_open(&array_module);
	if (array_ctx) {
		mm_module_ctrl(array_ctx, CMD_ARRAY_SET_PARAMS, (int)&rgb_array_params);
		mm_module_ctrl(array_ctx, CMD_ARRAY_SET_ARRAY, (int)&array);
		mm_module_ctrl(array_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}
#endif

#if NN_ENA
	/* NN */
	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&NN_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&nn_input_params);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(facedetect_res_t));		// result size
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, 10);		// result max count
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		printf("VIPNN open fail\n\r");
		goto error;
	}
	printf("VIPNN opened\n\r");
#else
	/* VP to AP */
	vp2ap_v1_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v1_ctx) {
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
		goto error;
	}
#endif

	//--------------Link---------------------------
	siso_array_vipnn = siso_create();
	if (siso_array_vipnn) {
#if ISP_ENA
		siso_ctrl(siso_array_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v1_ctx, 0);
#else
		siso_ctrl(siso_array_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)array_ctx, 0);
#endif
#if NN_ENA
		siso_ctrl(siso_array_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
#else
		siso_ctrl(siso_array_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v1_ctx, 0);
#endif
		siso_start(siso_array_vipnn);
	} else {
		printf("siso_array_vipnn open fail\n\r");
		goto error;
	}
	printf("siso_array_vipnn started\n\r");

	return;
error:

	return;
}

static void mmf2_video_example_vipnn_deinit(void)
{
	if (siso_array_vipnn) {
		siso_stop(siso_array_vipnn);
		siso_delete(siso_array_vipnn);
		siso_array_vipnn = NULL;
	}
#if NN_ENA
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_STATUS, (int)VIPNN_PAUSED);
		mm_module_close(vipnn_ctx);
		vipnn_ctx = NULL;
	}
#else
	if (vp2ap_v1_ctx) {
		mm_module_close(vp2ap_v1_ctx);
		vp2ap_v1_ctx = NULL;
	}
#endif
#if ISP_ENA
	if (isp_v1_ctx) {
		mm_module_close(isp_v1_ctx);
		isp_v1_ctx = NULL;
	}
#else
	if (array_ctx) {
		mm_module_ctrl(array_ctx, CMD_ARRAY_STREAMING, 0);
		mm_module_close(array_ctx);
		array_ctx = NULL;
	}
#endif
	printf("mmf2_video_example_vipnn_deinit done\n\r");
}

u32 CmdVipnnExample(IN u16 argc, IN u8 *argv[])
{
	if (_strcmp((const char *)argv[0], "stop") == 0) {
		printf("vipnn example stop\r\n");
		mmf2_video_example_vipnn_deinit();
		// VIPNN stop
	} else {
		printf("Usage: VIPNN stop\r\n");
		return _FALSE;
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE vipnn_example_cmd[] = {
	{
		"VIPNN", CmdVipnnExample
	},
};
