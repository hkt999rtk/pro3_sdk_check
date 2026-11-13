/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <string.h>
#include "ameba_soc.h"
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_isp_4k.h"
#include "module_encoder.h"
#include "module_uvcd.h"

#include "video_app_param_config.h"

/* Example topology:
 * V1: ISP 4K (2160x3840 NV12) -> H.264 encoder -> UVC
 * Requires VP UVC support (CONFIG_SUPPORT_UVC_TUNING_FOR_VP) and
 * CONFIG_USBD_UVC in the VP image; CONFIG_4K_DEV alone does not enable it.
 */

/* ---- User-configurable settings ---- */
#define STREAM_WIDTH   2160
#define STREAM_HEIGHT  3840
#define STREAM_FPS     20
#define STREAM_GOP     20
#define STREAM_BPS     2000000

/* ---- Internal constants (do not modify) ---- */
#if ((STREAM_WIDTH != ISP_4K_OUTPUT_WIDTH) || (STREAM_HEIGHT != ISP_4K_OUTPUT_HEIGHT))
#error "V1 encoder dimensions must match ISP 4K NV12 output"
#endif

static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};

static mm_context_t *isp_v1_ctx   = NULL;
static mm_context_t *enc_v1_ctx     = NULL;
static mm_context_t *uvcd_v1_ctx   = NULL;
static mm_siso_t *siso_isp_enc_v1 = NULL;
static mm_siso_t *siso_enc_uvcd_v1 = NULL;

static void mmf2_video_enc_param_setup(void)
{
	memset(enc_params, 0, sizeof(enc_params));
	enc_params[V1_STREAM_ID].stream_id = V1_STREAM_ID;
	enc_params[V1_STREAM_ID].width = STREAM_WIDTH;
	enc_params[V1_STREAM_ID].height = STREAM_HEIGHT;
	enc_params[V1_STREAM_ID].fps = STREAM_FPS;
	enc_params[V1_STREAM_ID].gop = STREAM_GOP;
	enc_params[V1_STREAM_ID].bps = STREAM_BPS;
	enc_params[V1_STREAM_ID].rcMode = ENC_VBR;
	enc_params[V1_STREAM_ID].minQp = 26;
	enc_params[V1_STREAM_ID].maxQp = 48;
	enc_params[V1_STREAM_ID].codec_fmt = VIDEO_H264;
	/* ISP 4K lends full, contiguous NV12 frames without remap or slices. */
	enc_params[V1_STREAM_ID].decompEnable = 0;
	enc_params[V1_STREAM_ID].lowLatencyEnable = 0;
	enc_params[V1_STREAM_ID].cs.cropScaleEnable = 0;

	video_enc_buf_pool_setup(enc_params);
	return;
}

void mmf2_video_example_4k_test_init(char *user_cmd)
{
	(void)user_cmd;

#if !defined(CONFIG_USBD_UVC) || !CONFIG_USBD_UVC || !defined(CONFIG_SUPPORT_UVC_TUNING) || !CONFIG_SUPPORT_UVC_TUNING
	/* Check the sink before allocating the encoder pool or opening modules. */
	printf("V1 UVC requires CONFIG_USBD_UVC and CONFIG_SUPPORT_UVC_TUNING in VP\n\r");
	return;
#endif

	if (isp_v1_ctx) {
		printf("ISP 4K encoder example already initialized\n\r");
		return;
	}
	mmf2_video_enc_param_setup();

	/* Prepare the hardware-owned NV12 queue; start ISP only after linking. */
	isp_v1_ctx = mm_module_open(&isp_4k_module);
	if (!isp_v1_ctx) {
		printf("ISP 4K open fail\n\r");
		goto error;
	}
	if (mm_module_ctrl(isp_v1_ctx, MM_CMD_SET_QUEUE_LEN, 2) < 0 ||
		mm_module_ctrl(isp_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0) {
		printf("ISP 4K queue init fail\n\r");
		goto error;
	}

	/* ENC module init */
	enc_v1_ctx = mm_module_open(&enc_module);
	if (enc_v1_ctx) {
		if (mm_module_ctrl(enc_v1_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V1_STREAM_ID]) < 0 ||
			mm_module_ctrl(enc_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6) < 0 ||
			mm_module_ctrl(enc_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0 ||
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_INIT_MEM_POOL, 0) < 0 ||
			mm_module_ctrl(enc_v1_ctx, CMD_ENC_APPLY, 0) < 0) {
			printf("ENC V1 init fail\n\r");
			goto error;
		}
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
	printf("enc_v1_ctx open success\n\r");

	/* Like uvcd_video_init: open initializes USB; handle sends H.264 frames.
	* The shared USB descriptor is configured in usbd_video.h, independently
	* of encoder dimensions. Match its H.264 size/intervals when testing.
	*/
#if defined(CONFIG_USBD_UVC) && CONFIG_USBD_UVC && defined(CONFIG_SUPPORT_UVC_TUNING) && CONFIG_SUPPORT_UVC_TUNING
	uvcd_v1_ctx = mm_module_open(&uvcd_module);
	if (!uvcd_v1_ctx) {
		printf("UVC V1 open fail\n\r");
		goto error;
	}
#endif

	/* V1 ISP NV12 -> encoder; linker releases the ISP lease. */
	siso_isp_enc_v1 = siso_create();
	if (siso_isp_enc_v1) {
		siso_ctrl(siso_isp_enc_v1, MMIC_CMD_ADD_INPUT, (uint32_t)isp_v1_ctx, 0);
		siso_ctrl(siso_isp_enc_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_v1_ctx, 0);
		if (siso_start(siso_isp_enc_v1) < 0) {
			printf("siso_isp_enc_v1 start fail\n\r");
			goto error;
		}
	} else {
		printf("siso_isp_enc_v1 open fail\n\r");
		goto error;
	}
	printf("siso_isp_enc_v1 started\n\r");

	/* Encoded V1 H.264 bitstream -> UVC. */
	siso_enc_uvcd_v1 = siso_create();
	if (siso_enc_uvcd_v1) {
		siso_ctrl(siso_enc_uvcd_v1, MMIC_CMD_ADD_INPUT, (uint32_t)enc_v1_ctx, 0);
		siso_ctrl(siso_enc_uvcd_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_v1_ctx, 0);
		if (siso_start(siso_enc_uvcd_v1) < 0) {
			printf("siso_enc_uvcd_v1 start fail\n\r");
			goto error;
		}
	} else {
		printf("siso_enc_uvcd_v1 open fail\n\r");
		goto error;
	}
	printf("siso_enc_uvcd_v1 start success\n\r");

	/* Downstream is ready before ISP publishes its first NV12 lease. */
	if (mm_module_ctrl(isp_v1_ctx, CMD_ISP_4K_APPLY, 0) != 0) {
		printf("ISP 4K apply fail\n\r");
		goto error;
	}
	printf("ISP 4K -> encoder V1 -> UVC started: %dx%d NV12\n\r", STREAM_WIDTH, STREAM_HEIGHT);
	return;
error:
	/* Keep linkers alive until ISP has received the last encoder release. */
	if (isp_v1_ctx && mm_module_ctrl(isp_v1_ctx, CMD_ISP_4K_STREAM_STOP, 0) < 0) {
		/* A lease is still outstanding: closing now would free DMA buffers
		 * and queues under the worker. Leave everything allocated.
		 */
		printf("ISP 4K stop timeout, keep modules allocated\n\r");
		return;
	}
	if (siso_isp_enc_v1) {
		siso_isp_enc_v1 = siso_delete(siso_isp_enc_v1);
	}
	if (siso_enc_uvcd_v1) {
		siso_enc_uvcd_v1 = siso_delete(siso_enc_uvcd_v1);
	}
	if (isp_v1_ctx) {
		isp_v1_ctx = mm_module_close(isp_v1_ctx);
	}
	if (enc_v1_ctx) {
		enc_v1_ctx = mm_module_close(enc_v1_ctx);
	}
	if (uvcd_v1_ctx) {
		uvcd_v1_ctx = mm_module_close(uvcd_v1_ctx);
	}
	return;
}

