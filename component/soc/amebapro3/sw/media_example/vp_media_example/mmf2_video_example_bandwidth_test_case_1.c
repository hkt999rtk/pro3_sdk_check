/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_encoder.h"
#include "module_isp.h"
#include "module_vp2ap.h"
#include "module_array.h"
#include "module_vipnn.h"
#include "avcodec.h"
#include "img_process.h"

#include "video_app_param_config.h"
#include "model_yolo.h"

/* ISP stand alone */
static mm_context_t *isp_v1_ctx         = NULL;
static mm_context_t *vp2ap_v1_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v1     = NULL;

static mm_context_t *isp_v2_ctx         = NULL;
static mm_context_t *vp2ap_v2_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v2     = NULL;

static mm_context_t *isp_v3_ctx         = NULL;
static mm_context_t *vp2ap_v3_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v3     = NULL;

static mm_context_t *isp_v4_ctx         = NULL;
static mm_context_t *vp2ap_v4_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v4     = NULL;

static mm_context_t *isp_v5_ctx         = NULL;
static mm_context_t *vp2ap_v5_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v5     = NULL;

/* ENC stand alone */
static mm_context_t *array_v1_ctx       = NULL;
static mm_context_t *enc_v1_ctx         = NULL;
static mm_context_t *vp2ap_enc_v1_ctx   = NULL;
static mm_siso_t *siso_array_enc_v1     = NULL;
static mm_siso_t *siso_enc_vp2ap_v1     = NULL;

static mm_context_t *array_v2_ctx       = NULL;
static mm_context_t *enc_v2_ctx         = NULL;
static mm_context_t *vp2ap_enc_v2_ctx   = NULL;
static mm_siso_t *siso_array_enc_v2     = NULL;
static mm_siso_t *siso_enc_vp2ap_v2     = NULL;

static mm_context_t *array_v3_ctx       = NULL;
static mm_context_t *enc_v3_ctx         = NULL;
static mm_context_t *vp2ap_enc_v3_ctx   = NULL;
static mm_siso_t *siso_array_enc_v3     = NULL;
static mm_siso_t *siso_enc_vp2ap_v3     = NULL;

/* NN stand alone */
static mm_context_t *array_v5_ctx       = NULL;
static mm_context_t *vipnn_ctx          = NULL;
static mm_siso_t *siso_array_vipnn_v5   = NULL;


static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};
static isp_enc_ch_en_t v_ch_en = {{0}, {0}};

#define V1_ENA  1
#define V2_ENA  1
#define V3_ENA  1
#define V4_ENA  1
#define V5_ENA  1
#define NN_ENA  0

/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m
 * please make sure the choosed model is also selected in amebapro2_fwfs_nn_models.json */
#define NN_MODEL_OBJ    yolov7_tiny
/* RGB video resolution
 * please make sure the resolution is matched to model input size and thereby to avoid SW image resizing */
#define NN_WIDTH	640
#define NN_HEIGHT	480

static vp2ap_params_t vp2ap_v1_params = {
	.stream_id = V1_STREAM_ID
};
static vp2ap_params_t vp2ap_v2_params = {
	.stream_id = V2_STREAM_ID
};
static vp2ap_params_t vp2ap_v3_params = {
	.stream_id = V3_STREAM_ID
};
static vp2ap_params_t vp2ap_v4_params = {
	.stream_id = V4_STREAM_ID
};
static vp2ap_params_t vp2ap_v5_params = {
	.stream_id = V5_STREAM_ID
};

static nn_data_param_t nn_input_params = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static array_params_t array_1_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_NV12,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps    = 30,
		}
	}
};

static array_params_t array_2_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_NV12,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps    = 30,
		}
	}
};

static array_params_t array_3_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_NV12,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps    = 1,
		}
	}
};

static array_params_t rgb_array_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_RGB888,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps    = 10,
		}
	}
};

static void v1_snapshot_cb(void *jpeg_addr, uint32_t jpeg_size)
{
	printf(">> squeeze jpeg size=%d\n\r", jpeg_size);
}

static uint32_t nn_fps = 0;
static uint32_t nn_ts = 0;
static void nn_display_cb(void *p, void *img_param)
{
	if (!p || !img_param)	{
		return;
	}

	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	objdetect_res_t *res = (objdetect_res_t *)&out->res[0];

	int obj_num = out->res_cnt;
	printf("object num = %d\r\n", obj_num);

	if (nn_ts == 0) {
		nn_ts = xTaskGetTickCount();
	}
	nn_fps++;
	if ((xTaskGetTickCount() - nn_ts) > 1000) {
		printf("NN fps=%lu\r\n", nn_fps);
		nn_fps = 0;
		nn_ts = xTaskGetTickCount();
	}
}

#include "ts_verify.h"

extern void ts_convert_if2of(struct ts_info_t ts_info);
extern void ts_check_end(void);
extern void ts_clear_operation_status(void);

static SemaphoreHandle_t ts_lock = NULL;
static int ts_lock_inited = 0;

#define RT_DEV_LOCK_TS  &ts_lock

static void device_mutex_init(SemaphoreHandle_t *lock)
{
	if (!ts_lock_inited) {
		*lock = xSemaphoreCreateMutex();
		ts_lock_inited = 1;
	}
}

static void device_mutex_lock(SemaphoreHandle_t *lock)
{
	device_mutex_init(lock);
	if (xSemaphoreTake(*lock, (TickType_t)10000) != pdTRUE) {
		printf("ts lock timeout\n");
	}
}

static void device_mutex_unlock(SemaphoreHandle_t *lock)
{
	xSemaphoreGive(*lock);
}

static void ts_scaleup_360p_to_1440p_task(void *p)
{
	struct ts_info_t ts_info;
	memset((u8 *)&ts_info, 0, sizeof(struct ts_info_t));

	uint8_t *in_buf = NULL;
	uint8_t *out_buf = NULL;

	in_buf = (uint8_t *)0x80000000;  // don't care the input data
	out_buf = (uint8_t *)p;

	ts_info.in_f = TS_FORMAT_S420;
	ts_info.out_f = TS_FORMAT_S420;
	ts_info.buf = in_buf;
	ts_info.out_buf = out_buf;
	ts_info.in_w = 640;
	ts_info.in_h = 360;
	ts_info.out_w = 2560;
	ts_info.out_h = 1440;
	ts_info.tile_en = 1;
	ts_info.csc_idx = 0;
	ts_info.intp_idx = 0;
	ts_info.wrap_en = 0;
	ts_info.tile_w = 64;
	ts_info.tile_h = 64;
	ts_info.wrap_num = 0;

	device_mutex_lock(RT_DEV_LOCK_TS);
	ts_convert_if2of(ts_info);

	ts_check_end();
	ts_clear_operation_status();
	device_mutex_unlock(RT_DEV_LOCK_TS);

	printf("ts 360p to 1440p run done\r\n");

error:

	vTaskDelete(NULL);
}

/* crop and scale by TS */
static void crop_scaleup_isp_to_enc(void *y_buf, void *uv_buf, uint32_t w, uint32_t h, void *out_yuv_buf)
{
	if (xTaskCreate(ts_scaleup_360p_to_1440p_task, "ts_scaleup", 256, (void *)out_yuv_buf, 3, NULL) != pdPASS) {
		printf("error: fail to create ts_scaleup task, memory not enough\r\n");
	}
}

static void init_isp_module_task(void *p)
{
#if V1_ENA
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
#endif
#if V2_ENA
	/* ISP module init */
	isp_v2_ctx = mm_module_open(&isp_module);
	if (isp_v2_ctx) {
		mm_module_ctrl(isp_v2_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V2_STREAM_ID]);
		mm_module_ctrl(isp_v2_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		//mm_module_ctrl(isp_v2_ctx, CMD_ISP_APPLY, 1);	// start channel 1
	} else {
		printf("ISP open fail\n\r");
		goto error;
	}
	printf("isp_v2_ctx open success\n\r");
#endif
#if V3_ENA
	/* ISP module init */
	isp_v3_ctx = mm_module_open(&isp_module);
	if (isp_v3_ctx) {
		mm_module_ctrl(isp_v3_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V3_STREAM_ID]);
		mm_module_ctrl(isp_v3_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		//mm_module_ctrl(isp_v3_ctx, CMD_ISP_APPLY, 2);	// start channel 2
	} else {
		printf("ISP open fail\n\r");
		goto error;
	}
	printf("isp_v3_ctx open success\n\r");
#endif
#if V4_ENA
	/* ISP module init */
	isp_v4_ctx = mm_module_open(&isp_module);
	if (isp_v4_ctx) {
		mm_module_ctrl(isp_v4_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V4_STREAM_ID]);
		mm_module_ctrl(isp_v4_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v4_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		//mm_module_ctrl(isp_v4_ctx, CMD_ISP_APPLY, 3);	// start channel 3
	} else {
		printf("ISP open fail\n\r");
		goto error;
	}
	printf("isp_v4_ctx open success\n\r");
#endif
#if V5_ENA
	/* ISP module init */
	isp_v5_ctx = mm_module_open(&isp_module);
	if (isp_v5_ctx) {
		mm_module_ctrl(isp_v5_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V5_STREAM_ID]);
		mm_module_ctrl(isp_v5_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v5_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		//mm_module_ctrl(isp_v5_ctx, CMD_ISP_APPLY, 3);	// start channel 3
	} else {
		printf("ISP open fail\n\r");
		goto error;
	}
	printf("isp_v5_ctx open success\n\r");
#endif

#if V1_ENA
	mm_module_ctrl(isp_v1_ctx, CMD_ISP_APPLY, 0);	// start channel 0
#endif
#if V2_ENA
	mm_module_ctrl(isp_v2_ctx, CMD_ISP_APPLY, 1);	// start channel 1
#endif
#if V3_ENA
	mm_module_ctrl(isp_v3_ctx, CMD_ISP_APPLY, 2);	// start channel 2
#endif
#if V4_ENA
	mm_module_ctrl(isp_v4_ctx, CMD_ISP_APPLY, 3);	// start channel 3
#endif
#if V5_ENA
	mm_module_ctrl(isp_v5_ctx, CMD_ISP_APPLY, 4);	// start channel 4
#endif

error:

	vTaskDelete(NULL);
}

static void init_enc_module_task(void *p)
{
#if V1_ENA
	/* ENC module init */
	enc_v1_ctx = mm_module_open(&enc_module);
	if (enc_v1_ctx) {
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V1_STREAM_ID]);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PREPROCESS_CB, (int)crop_scaleup_isp_to_enc);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_JPEG_SNAPSHOT_CB, (int)v1_snapshot_cb);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_JPEG_DO_SNAPSHOT, 2);  //do a snapshot at 2nd frame
		mm_module_ctrl(enc_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
		mm_module_ctrl(enc_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_v1_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
	printf("enc_v1_ctx open success\n\r");
#endif

#if V2_ENA
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
#endif

#if V3_ENA
	/* ENC module init */
	enc_v3_ctx = mm_module_open(&enc_module);
	if (enc_v3_ctx) {
		mm_module_ctrl(enc_v3_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V3_STREAM_ID]);
		mm_module_ctrl(enc_v3_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
		mm_module_ctrl(enc_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_v3_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_v3_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
	printf("enc_v3_ctx open success\n\r");
#endif

error:

	vTaskDelete(NULL);
}

static void init_nn_module_task(void *p)
{
#if NN_ENA
	/* ARRAY */
	array_t array;
	array.data_addr = (uint32_t) malloc(NN_WIDTH * NN_HEIGHT * 3);
	array.data_len = (uint32_t) NN_WIDTH * NN_HEIGHT * 3;
	array_ctx_v5 = mm_module_open(&array_module);
	if (array_ctx_v5) {
		mm_module_ctrl(array_ctx_v5, CMD_ARRAY_SET_PARAMS, (int)&rgb_array_params);
		mm_module_ctrl(array_ctx_v5, CMD_ARRAY_SET_ARRAY, (int)&array);
		mm_module_ctrl(array_ctx_v5, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_ctx_v5, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_ctx_v5, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_ctx_v5, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	// VIPNN
	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&NN_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&nn_input_params);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(objdetect_res_t));		// result size
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, 10);		// result max count
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		printf("VIPNN open fail\n\r");
		goto error;
	}
	printf("VIPNN opened\n\r");

	siso_array_vipnn_v5 = siso_create();
	if (siso_array_vipnn_v5) {
		siso_ctrl(siso_array_vipnn_v5, MMIC_CMD_ADD_INPUT, (uint32_t)array_ctx_v5, 0);
		siso_ctrl(siso_array_vipnn_v5, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
		siso_start(siso_array_vipnn_v5);
	} else {
		printf("siso_array_vipnn_v5 open fail\n\r");
		goto error;
	}
	printf("siso_array_vipnn_v5 started\n\r");
#endif

error:

	vTaskDelete(NULL);
}

void mmf2_video_example_bandwidth_test_case_1(char *user_cmd)
{
	/* parsing video app params to isp/enc params */
	if (video_app_params_parsing(&v_ch_en, user_cmd, isp_params, enc_params) != 0) {
		printf("error: fail to parse video params\r\n");
		goto error;
	}

	/* init ISP module in task */
	if (xTaskCreate(init_isp_module_task, "isp_init_task", 2048, (void *)NULL, 2, NULL) != pdPASS) {
		printf("error: fail to create isp init task, memory not enough\r\n");
		goto error;
	}

	/* init ENC module in task */
	if (xTaskCreate(init_enc_module_task, "enc_init_task", 2048, (void *)NULL, 2, NULL) != pdPASS) {
		printf("error: fail to create enc init task, memory not enough\r\n");
		goto error;
	}

	/* init NN module in task */
	if (xTaskCreate(init_nn_module_task, "nn_init_task", 2048, (void *)NULL, 2, NULL) != pdPASS) {
		printf("error: fail to create nn init task, memory not enough\r\n");
		goto error;
	}

	/*------------------------------------------------------------------------------------------*/
	/* ISP stand alone                                                                          */
	/*------------------------------------------------------------------------------------------*/
#if V1_ENA
	/* VP to AP */
	vp2ap_v1_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v1_ctx) {
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v1_params);
		mm_module_ctrl(vp2ap_v1_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
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
	printf("siso_isp_vp2ap_v1 start success\n\r");
#endif

#if V2_ENA
	/* VP to AP */
	vp2ap_v2_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v2_ctx) {
		mm_module_ctrl(vp2ap_v2_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v2_params);
		mm_module_ctrl(vp2ap_v2_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap_v2_ctx open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	while (isp_v2_ctx == NULL || vp2ap_v2_ctx == NULL) {;}
	siso_isp_vp2ap_v2 = siso_create();
	if (siso_isp_vp2ap_v2) {
		siso_ctrl(siso_isp_vp2ap_v2, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v2_ctx, 0);
		siso_ctrl(siso_isp_vp2ap_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v2_ctx, 0);
		siso_start(siso_isp_vp2ap_v2);
	} else {
		printf("siso_isp_vp2ap_v2 open fail\n\r");
		goto error;
	}
	printf("siso_isp_vp2ap_v2 start success\n\r");
#endif

#if V3_ENA
	/* VP to AP */
	vp2ap_v3_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v3_ctx) {
		mm_module_ctrl(vp2ap_v3_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v3_params);
		mm_module_ctrl(vp2ap_v3_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap_v3_ctx open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	while (isp_v3_ctx == NULL || vp2ap_v3_ctx == NULL) {;}
	siso_isp_vp2ap_v3 = siso_create();
	if (siso_isp_vp2ap_v3) {
		siso_ctrl(siso_isp_vp2ap_v3, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v3_ctx, 0);
		siso_ctrl(siso_isp_vp2ap_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v3_ctx, 0);
		siso_start(siso_isp_vp2ap_v3);
	} else {
		printf("siso_isp_vp2ap_v3 open fail\n\r");
		goto error;
	}
	printf("siso_isp_vp2ap_v3 start success\n\r");
#endif

#if V4_ENA
	/* VP to AP */
	vp2ap_v4_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v4_ctx) {
		mm_module_ctrl(vp2ap_v4_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v4_params);
		mm_module_ctrl(vp2ap_v4_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap_v4_ctx open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	while (isp_v4_ctx == NULL || vp2ap_v4_ctx == NULL) {;}
	siso_isp_vp2ap_v4 = siso_create();
	if (siso_isp_vp2ap_v4) {
		siso_ctrl(siso_isp_vp2ap_v4, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v4_ctx, 0);
		siso_ctrl(siso_isp_vp2ap_v4, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v4_ctx, 0);
		siso_start(siso_isp_vp2ap_v4);
	} else {
		printf("siso_isp_vp2ap_v4 open fail\n\r");
		goto error;
	}
#endif

#if V5_ENA
	/* VP to AP */
	vp2ap_v5_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_v5_ctx) {
		mm_module_ctrl(vp2ap_v5_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v5_params);
		mm_module_ctrl(vp2ap_v5_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap_v5_ctx open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	while (isp_v5_ctx == NULL || vp2ap_v5_ctx == NULL) {;}
	siso_isp_vp2ap_v5 = siso_create();
	if (siso_isp_vp2ap_v5) {
		siso_ctrl(siso_isp_vp2ap_v5, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v5_ctx, 0);
		siso_ctrl(siso_isp_vp2ap_v5, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v5_ctx, 0);
		siso_start(siso_isp_vp2ap_v5);
	} else {
		printf("siso_isp_vp2ap_v5 open fail\n\r");
		goto error;
	}
#endif


	/*------------------------------------------------------------------------------------------*/
	/* ENC stand alone                                                                          */
	/*------------------------------------------------------------------------------------------*/
#if V1_ENA
	/* ENC1 ARRAY */
	array_t array_enc_v1;
	array_enc_v1.data_addr = (uint32_t)0x80000000;  // malloc(2560 * 1440 * 3 / 2);
	array_enc_v1.data_len = (uint32_t)enc_params[V1_STREAM_ID].width * enc_params[V1_STREAM_ID].height * 3 / 2;
	array_1_params.u.v.fps = enc_params[V1_STREAM_ID].fps;
	array_v1_ctx = mm_module_open(&array_module);
	if (array_v1_ctx) {
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_SET_PARAMS, (int)&array_1_params);
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_SET_ARRAY, (int)&array_enc_v1);
		mm_module_ctrl(array_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_v1_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	vp2ap_enc_v1_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_enc_v1_ctx) {
		mm_module_ctrl(vp2ap_enc_v1_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v1_params);
		mm_module_ctrl(vp2ap_enc_v1_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
		goto error;
	}

	siso_array_enc_v1 = siso_create();
	if (siso_array_enc_v1) {
		siso_ctrl(siso_array_enc_v1, MMIC_CMD_ADD_INPUT, (uint32_t)array_v1_ctx, 0);
		siso_ctrl(siso_array_enc_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v1_ctx, 0);
		siso_ctrl(siso_array_enc_v1, MMIC_CMD_SET_TASKPRIORITY, 3, 0);
		siso_start(siso_array_enc_v1);
	} else {
		printf("siso_array_enc_v1 open fail\n\r");
		goto error;
	}

	siso_enc_vp2ap_v1 = siso_create();
	if (siso_enc_vp2ap_v1) {
		siso_ctrl(siso_enc_vp2ap_v1, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v1_ctx, 0);
		siso_ctrl(siso_enc_vp2ap_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_enc_v1_ctx, 0);
		siso_start(siso_enc_vp2ap_v1);
	} else {
		printf("siso_enc_vp2ap_v1 open fail\n\r");
		goto error;
	}
#endif

#if V2_ENA
	/* ENC2 ARRAY */
	array_t array_enc_v2;
	array_enc_v2.data_addr = (uint32_t)(array_enc_v1.data_addr + array_enc_v1.data_len); //malloc(1280 * 720 * 3 / 2);
	array_enc_v2.data_len = (uint32_t) enc_params[V2_STREAM_ID].width * enc_params[V2_STREAM_ID].height * 3 / 2;
	array_2_params.u.v.fps = enc_params[V2_STREAM_ID].fps;
	array_v2_ctx = mm_module_open(&array_module);
	if (array_v2_ctx) {
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_SET_PARAMS, (int)&array_2_params);
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_SET_ARRAY, (int)&array_enc_v2);
		mm_module_ctrl(array_v2_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_v2_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	vp2ap_enc_v2_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_enc_v2_ctx) {
		mm_module_ctrl(vp2ap_enc_v2_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v2_params);
		mm_module_ctrl(vp2ap_enc_v2_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
		goto error;
	}

	siso_array_enc_v2 = siso_create();
	if (siso_array_enc_v2) {
		siso_ctrl(siso_array_enc_v2, MMIC_CMD_ADD_INPUT, (uint32_t)array_v2_ctx, 0);
		siso_ctrl(siso_array_enc_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v2_ctx, 0);
		siso_ctrl(siso_array_enc_v2, MMIC_CMD_SET_TASKPRIORITY, 3, 0);
		siso_start(siso_array_enc_v2);
	} else {
		printf("siso_array_enc_v2 open fail\n\r");
		goto error;
	}

	siso_enc_vp2ap_v2 = siso_create();
	if (siso_enc_vp2ap_v2) {
		siso_ctrl(siso_enc_vp2ap_v2, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v2_ctx, 0);
		siso_ctrl(siso_enc_vp2ap_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_enc_v2_ctx, 0);
		siso_start(siso_enc_vp2ap_v2);
	} else {
		printf("siso_enc_vp2ap_v2 open fail\n\r");
		goto error;
	}
#endif

#if V3_ENA
	/* ENC3 ARRAY */
	array_t array_enc_v3;
	array_enc_v3.data_addr = (uint32_t)(array_enc_v2.data_addr + array_enc_v2.data_len); // malloc(1920 * 1080 * 3 / 2);
	array_enc_v3.data_len = (uint32_t)enc_params[V3_STREAM_ID].width * enc_params[V3_STREAM_ID].height * 3 / 2;
	array_3_params.u.v.fps = enc_params[V3_STREAM_ID].fps;
	array_v3_ctx = mm_module_open(&array_module);
	if (array_v3_ctx) {
		mm_module_ctrl(array_v3_ctx, CMD_ARRAY_SET_PARAMS, (int)&array_3_params);
		mm_module_ctrl(array_v3_ctx, CMD_ARRAY_SET_ARRAY, (int)&array_enc_v3);
		mm_module_ctrl(array_v3_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_v3_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_v3_ctx, CMD_ARRAY_STREAMING, 1);	// streamming on
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	vp2ap_enc_v3_ctx = mm_module_open(&vp2ap_module);
	if (vp2ap_enc_v3_ctx) {
		mm_module_ctrl(vp2ap_enc_v3_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v3_params);
		mm_module_ctrl(vp2ap_enc_v3_ctx, CMD_VP2AP_APPLY, 0);
	} else {
		printf("vp2ap open fail\n\r");
		goto error;
	}

	/* SISO linker create */
	siso_array_enc_v3 = siso_create();
	if (siso_array_enc_v3) {
		siso_ctrl(siso_array_enc_v3, MMIC_CMD_ADD_INPUT, (uint32_t)array_v3_ctx, 0);
		siso_ctrl(siso_array_enc_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v3_ctx, 0);
		siso_ctrl(siso_array_enc_v3, MMIC_CMD_SET_TASKPRIORITY, 3, 0);
		siso_start(siso_array_enc_v3);
	} else {
		printf("siso_array_enc_v3 open fail\n\r");
		goto error;
	}

	siso_enc_vp2ap_v3 = siso_create();
	if (siso_enc_vp2ap_v3) {
		siso_ctrl(siso_enc_vp2ap_v3, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v3_ctx, 0);
		siso_ctrl(siso_enc_vp2ap_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_enc_v3_ctx, 0);
		siso_start(siso_enc_vp2ap_v3);
	} else {
		printf("siso_enc_vp2ap_v3 open fail\n\r");
		goto error;
	}
#endif

error:
	return;
}
