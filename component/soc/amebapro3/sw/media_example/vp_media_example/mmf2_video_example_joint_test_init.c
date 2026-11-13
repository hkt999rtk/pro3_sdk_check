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
#include "module_encoder.h"
#include "module_isp.h"
#include "module_vp2ap.h"
#include "module_array.h"
#include "module_vipnn.h"
#include "avcodec.h"
#include "img_process.h"

#include "video_app_param_config.h"
#include "model_yolo.h"

static mm_context_t *isp_v1_ctx         = NULL;
static mm_context_t *enc_v1_ctx         = NULL;
static mm_context_t *vp2ap_v1_ctx       = NULL;
static mm_siso_t *siso_isp_enc_v1       = NULL;
static mm_siso_t *siso_enc_vp2ap_v1     = NULL;

static mm_context_t *isp_v2_ctx         = NULL;
static mm_context_t *enc_v2_ctx         = NULL;
static mm_context_t *vp2ap_v2_ctx       = NULL;
static mm_siso_t *siso_isp_enc_v2       = NULL;
static mm_siso_t *siso_enc_vp2ap_v2     = NULL;

static mm_context_t *isp_v3_ctx         = NULL;
static mm_context_t *enc_v3_ctx         = NULL;
static mm_context_t *vp2ap_v3_ctx       = NULL;
static mm_siso_t *siso_isp_enc_v3       = NULL;
static mm_siso_t *siso_enc_vp2ap_v3     = NULL;

static mm_context_t *isp_v4_ctx         = NULL;
static mm_context_t *vp2ap_v4_ctx       = NULL;
static mm_siso_t *siso_isp_vp2ap_v4     = NULL;

static mm_context_t *isp_v5_ctx         = NULL;
static mm_context_t *vipnn_ctx          = NULL;
static mm_context_t *vp2ap_v5_ctx       = NULL;
static mm_siso_t *siso_isp_vipnn        = NULL;

static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};
static isp_enc_ch_en_t v_ch_en = {{0}, {0}};

#define NN_ENA  0
#define DDR_TRAFFIC_ENA  0

/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m
 * please make sure the choosed model is also selected in amebapro2_fwfs_nn_models.json */
#define NN_MODEL_OBJ    yolov7_tiny
/* RGB video resolution
 * please make sure the resolution is matched to model input size and thereby to avoid SW image resizing */
#define NN_WIDTH	640
#define NN_HEIGHT	480

static vp2ap_params_t vp2ap_v1_params = {
	.stream_id = V1_STREAM_ID,
	.send_ap_off = 1
};
static vp2ap_params_t vp2ap_v2_params = {
	.stream_id = V2_STREAM_ID,
	.send_ap_off = 1
};
static vp2ap_params_t vp2ap_v3_params = {
	.stream_id = V3_STREAM_ID,
	.send_ap_off = 1
};
static vp2ap_params_t vp2ap_v4_params = {
	.stream_id = V4_STREAM_ID,
	.send_ap_off = 1
};
static vp2ap_params_t vp2ap_v5_params = {
	.stream_id = V5_STREAM_ID,
	.send_ap_off = 1
};

static nn_data_param_t nn_input_params = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

#define SLICE_WRAP_NUM  8

static isp_remap_param_t isp_remap_v1_param = {
	.feature_bit = REMAP_COMPRESS,
	.enc_type = ISP_REMAP_ENC_HEVC,  //0: H264, 1: H265
	.wrap_num = SLICE_WRAP_NUM,
	.burst_id = 0,  //should be 0 (64-aligned) if RTT enabled, 2 (256-aligned) if compression enabled
	.fixp_hts_ratio_2 = 4,
	.encDec400LumTableBase = {0},
	.encDec400ChrTableBase = {0},
};

static enc_remap_param_t enc_remap_v1_param = {
	.slice_wrap_num = SLICE_WRAP_NUM,
};

static void v1_snapshot_cb(void *jpeg_addr, uint32_t jpeg_size)
{
	printf(">> squeeze jpeg size=%d\n\r", jpeg_size);
}

void cpu_ddr_traffic_task(void *p)
{
	int data_size = 1024 * 1024;
	while (1) {
		uint8_t *src = (uint8_t *)malloc(data_size);
		uint8_t *dst = (uint8_t *)malloc(data_size);
		if (src == NULL || dst == NULL) {
			printf("error: failed to allocate memory\r\n");
			goto cleanUp;
		}
		memset(src, 0xAA, data_size);
		memcpy(dst, src, data_size);

		free(src);
		free(dst);

		vTaskDelay(500);
	}
cleanUp:
	vTaskDelete(NULL);
}


static GDMA_InitTypeDef GDMA_InitStructVF;
static uint32_t GDMAIndex = 1;
static volatile int dma_done = 0;
static uint32_t write_data_irq(void *Data)
{
	PGDMA_InitTypeDef GDMA_InitStruct = (PGDMA_InitTypeDef) Data;
	u8 IsrTypeMap = 0;

	//Clear Pending ISR
	IsrTypeMap = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	dma_done = 1;
	return 0;
}

static int dma_copy(uint32_t dst, uint32_t src, uint32_t length)
{
	static PGDMA_InitTypeDef GDMA_InitStruct;
	static u8  GdmaChannel = 0;

	DCache_CleanInvalidate((uint32_t) src, length);

	GDMA_InitStruct = (PGDMA_InitTypeDef) &GDMA_InitStructVF;
	GDMA_StructInit(GDMA_InitStruct);
	GDMA_InitStruct->GDMA_Index = GDMAIndex;
	GDMA_InitStruct->GDMA_DIR = TTFCMemToMem;

	if (((length & 0x03) == 0) && (((uint32_t)(src) & 0x03) == 0)) {
		/*	4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = length >> 2;
	} else {
		printf("write_data_dma: Aligment Err: src=%p,  length=%u\n", src, length);
		return -1;
	}

	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = IncType;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)src;
	GDMA_InitStruct->GDMA_DstAddr = (u32)dst;

	GDMA_InitStruct->GDMA_IsrType = (TransferType | ErrType);
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_ReloadDst = 0;

	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChannel = GDMA_ChnlAlloc(GDMAIndex, (IRQ_FUN)write_data_irq, (u32)GDMA_InitStruct, INT_PRI_MIDDLE);	// TODO
	if (GdmaChannel == 0xFF) {
		// No Available DMA channel
		printf("error: No Available DMA channel\n");
		return -1;
	}
	GDMA_InitStruct->GDMA_ChNum = GdmaChannel;
	GDMA_Init(GDMAIndex, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMAIndex, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	while (dma_done == 0) {
		vTaskDelay(1);
	}
	dma_done = 0;

	//printf("nb_write_data_dma done\n");

	return 0;
}

void dma_ddr_traffic_task(void *p)
{
	int data_size = 1024 * 1024;

	while (1) {
		uint8_t *src = (uint8_t *)malloc(data_size);
		uint8_t *dst = (uint8_t *)malloc(data_size);
		if (src == NULL || dst == NULL) {
			printf("error: failed to allocate memory\r\n");
			goto cleanUp;
		}
		memset(src, 0xAA, data_size);
		dma_copy(dst, src, data_size);

		free(src);
		free(dst);

		vTaskDelay(500);
	}
cleanUp:
	vTaskDelete(NULL);
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

/* crop and scale by TS */
static void crop_scaleup_isp_to_enc(void *y_buf, void *uv_buf, uint32_t w, uint32_t h,  void *out_yuv_buf)
{
	img_t im_in = {
		.width = w,
		.height = h,
		.data = y_buf
	};
	rect_t crop_roi = {
		.xmin = 0,
		.ymin = 0,
		.xmax = 1280,
		.ymax = 720
	};
	img_t im_out = {
		.width = w,
		.height = h,
		.data = out_yuv_buf
	};
	img_crop_then_scale(&im_in, &crop_roi, &im_out);
}

static void init_isp_module_task(void *p)
{
	/* ISP module init */
	isp_v1_ctx = mm_module_open(&isp_module);
	if (isp_v1_ctx) {
		mm_module_ctrl(isp_v1_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		/* isp remap setting */
		if (isp_params[V1_STREAM_ID].slice_mode_en || isp_params[V1_STREAM_ID].compress_en) {
			isp_remap_v1_param.enc_type = (enc_params[V1_STREAM_ID].codec_fmt == VIDEO_HEVC) ? ISP_REMAP_ENC_HEVC : ISP_REMAP_ENC_H264;
			if (isp_params[V1_STREAM_ID].slice_mode_en && isp_params[V1_STREAM_ID].compress_en) {
				isp_remap_v1_param.feature_bit = (REMAP_SLICE | REMAP_COMPRESS);
				isp_remap_v1_param.burst_id = 2;  //compression must need 256-aligned
				printf("video app set slice mode with compression\n\r");
			} else if (isp_params[V1_STREAM_ID].slice_mode_en) {
				isp_remap_v1_param.feature_bit = (REMAP_SLICE | REMAP_RTT);
				isp_remap_v1_param.burst_id = 0;  //RTT must need 64-aligned
				printf("video app set slice mode with RTT\n\r");
			} else if (isp_params[V1_STREAM_ID].compress_en) {
				isp_remap_v1_param.feature_bit = REMAP_COMPRESS;
				isp_remap_v1_param.burst_id = 2;  //compression must need 256-aligned
				printf("video app set frame mode compression\n\r");
			}
			mm_module_ctrl(isp_v1_ctx, CMD_ISP_SET_REMAP, (int)&isp_remap_v1_param);
		}
		//mm_module_ctrl(isp_v1_ctx, CMD_ISP_APPLY, 0);	// start channel 0
	} else {
		printf("isp_v1_ctx open fail\n\r");
		goto error;
	}
	printf("isp_v1_ctx open success\n\r");

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

	/* ISP module enable streaming channel */
	if (v_ch_en.isp[V1_STREAM_ID]) {
		mm_module_ctrl(isp_v1_ctx, CMD_ISP_APPLY, 0);	// enable channel 0
	}
	if (v_ch_en.isp[V2_STREAM_ID]) {
		mm_module_ctrl(isp_v2_ctx, CMD_ISP_APPLY, 1);	// enable channel 1
	}
	if (v_ch_en.isp[V3_STREAM_ID]) {
		mm_module_ctrl(isp_v3_ctx, CMD_ISP_APPLY, 2);	// enable channel 2
	}
	if (v_ch_en.isp[V4_STREAM_ID]) {
		mm_module_ctrl(isp_v4_ctx, CMD_ISP_APPLY, 3);	// enable channel 3
	}
	if (v_ch_en.isp[V5_STREAM_ID]) {
		mm_module_ctrl(isp_v5_ctx, CMD_ISP_APPLY, 4);	// enable channel 4
	}

	/* TODO: should be modified after isp control sw being ready
	   isp control sw should be able to use API control each channel on/off */
	//extern void isp_start_streaming(void);
	//isp_start_streaming();

error:

cleanUp:

	vTaskDelete(NULL);
}

static void init_enc_module_task(void *p)
{
	if (v_ch_en.enc[V1_STREAM_ID]) {
		/* ENC module init */
		enc_v1_ctx = mm_module_open(&enc_module);
		if (enc_v1_ctx) {
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V1_STREAM_ID]);
			//mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PREPROCESS_CB, (int)crop_scaleup_isp_to_enc);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_JPEG_SNAPSHOT_CB, (int)v1_snapshot_cb);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_JPEG_DO_SNAPSHOT, 3);  //do a snapshot at 3rd frame
			mm_module_ctrl(enc_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);//V2_ENC_QUEUE_LEN);
			mm_module_ctrl(enc_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_INIT_MEM_POOL, 0);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_ISP_REMAP, (int)&isp_remap_v1_param);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_ENC_REMAP, (int)&enc_remap_v1_param);
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_APPLY, 0);
		} else {
			printf("ENC open fail\n\r");
			goto error;
		}
		printf("enc_v1_ctx open success\n\r");
	}

	if (v_ch_en.enc[V2_STREAM_ID]) {
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
	}

	if (v_ch_en.enc[V3_STREAM_ID]) {
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
	}

error:

	vTaskDelete(NULL);
}

static void init_nn_module_task(void *p)
{
	if (v_ch_en.isp[V5_STREAM_ID]) {
#if NN_ENA
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
#else
		/* VP to AP */
		vp2ap_v5_ctx = mm_module_open(&vp2ap_module);
		if (vp2ap_v5_ctx) {
			mm_module_ctrl(vp2ap_v5_ctx, CMD_VP2AP_SET_PARAMS, (int)&vp2ap_v5_params);
			mm_module_ctrl(vp2ap_v5_ctx, CMD_VP2AP_APPLY, 0);
		} else {
			printf("vp2ap_v5_ctx open fail\n\r");
			goto error;
		}
#endif
		siso_isp_vipnn = siso_create();
		if (siso_isp_vipnn) {
			siso_ctrl(siso_isp_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v5_ctx, 0);
#if NN_ENA
			siso_ctrl(siso_isp_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
#else
			siso_ctrl(siso_isp_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v5_ctx, 0);
#endif
			siso_start(siso_isp_vipnn);
		} else {
			printf("siso_isp_vipnn open fail\n\r");
			goto error;
		}
		printf("siso_isp_vipnn started\n\r");
	}

error:

	vTaskDelete(NULL);
}

void mmf2_video_example_joint_test_init(char *user_cmd)
{
	DBG_MMF_INFO_MSG_OFF(0xffff);

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
	if (xTaskCreate(init_enc_module_task, "enc_init_task", 2048, (void *)NULL, 3, NULL) != pdPASS) {
		printf("error: fail to create enc init task, memory not enough\r\n");
		goto error;
	}
	int enc_v1_prio = 8;
	int enc_v2_prio = 7;
	int enc_v3_prio = 6;
	if (isp_params[V1_STREAM_ID].slice_mode_en) {
		enc_v1_prio = 7;
		enc_v2_prio = 8;
	}

	/* init NN module in task */
	if (xTaskCreate(init_nn_module_task, "nn_init_task", 2048, (void *)NULL, 2, NULL) != pdPASS) {
		printf("error: fail to create nn init task, memory not enough\r\n");
		goto error;
	}

	if (v_ch_en.isp[V1_STREAM_ID] && v_ch_en.enc[V1_STREAM_ID]) {
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
		while (isp_v1_ctx == NULL || enc_v1_ctx == NULL) {;}
		siso_isp_enc_v1 = siso_create();
		if (siso_isp_enc_v1) {
			siso_ctrl(siso_isp_enc_v1, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v1_ctx, 0);
			siso_ctrl(siso_isp_enc_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v1_ctx, 0);
			siso_ctrl(siso_isp_enc_v1, MMIC_CMD_SET_TASKPRIORITY, enc_v1_prio, 0);
			siso_start(siso_isp_enc_v1);
		} else {
			printf("siso_isp_enc_v1 open fail\n\r");
			goto error;
		}
		printf("siso_isp_enc_v1 start success\n\r");

		/* SISO linker create */
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
	}

	if (v_ch_en.isp[V2_STREAM_ID] && v_ch_en.enc[V2_STREAM_ID]) {
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
		while (isp_v2_ctx == NULL || enc_v2_ctx == NULL) {;}
		siso_isp_enc_v2 = siso_create();
		if (siso_isp_enc_v2) {
			siso_ctrl(siso_isp_enc_v2, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v2_ctx, 0);
			siso_ctrl(siso_isp_enc_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v2_ctx, 0);
			siso_ctrl(siso_isp_enc_v2, MMIC_CMD_SET_TASKPRIORITY, enc_v2_prio, 0);
			siso_start(siso_isp_enc_v2);
		} else {
			printf("siso_isp_enc_v2 open fail\n\r");
			goto error;
		}
		printf("siso_isp_enc_v2 start success\n\r");

		/* SISO linker create */
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
	}

	if (v_ch_en.isp[V3_STREAM_ID] && v_ch_en.enc[V3_STREAM_ID]) {
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
		while (isp_v3_ctx == NULL || enc_v3_ctx == NULL) {;}
		siso_isp_enc_v3 = siso_create();
		if (siso_isp_enc_v3) {
			siso_ctrl(siso_isp_enc_v3, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v3_ctx, 0);
			siso_ctrl(siso_isp_enc_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v3_ctx, 0);
			siso_ctrl(siso_isp_enc_v3, MMIC_CMD_SET_TASKPRIORITY, enc_v3_prio, 0);
			siso_start(siso_isp_enc_v3);
		} else {
			printf("siso_isp_enc_v3 open fail\n\r");
			goto error;
		}
		printf("siso_isp_enc_v3 start success\n\r");

		/* SISO linker create */
		siso_enc_vp2ap_v3 = siso_create();
		if (siso_enc_vp2ap_v3) {
			siso_ctrl(siso_enc_vp2ap_v3, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v3_ctx, 0);
			siso_ctrl(siso_enc_vp2ap_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)vp2ap_v3_ctx, 0);
			siso_start(siso_enc_vp2ap_v3);
		} else {
			printf("siso_enc_vp2ap_v3 open fail\n\r");
			goto error;
		}
	}

	if (v_ch_en.isp[V4_STREAM_ID]) {
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
	}

	/* start a task to generate some ddr traffic */
#if DDR_TRAFFIC_ENA
	xTaskCreate(cpu_ddr_traffic_task, "cpu_traffic", 128, (void *)NULL, 5, NULL);
	xTaskCreate(dma_ddr_traffic_task, "dma_traffic", 128, (void *)NULL, 5, NULL);
#endif


error:
	return;
}
