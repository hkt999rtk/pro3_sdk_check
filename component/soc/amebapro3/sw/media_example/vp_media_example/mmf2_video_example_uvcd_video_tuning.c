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
#include "module_vp2ap.h"
#include "module_vipnn.h"
#include "video_app_param_config.h"
#include "video_common_type.h"
#include "avcodec.h"
#include "img_process.h"
#include "model_yolo.h"
#include "usb_os.h"
#include "usbd_video.h"


static isp_params_t isp_params[VIDEO_ISP_VALID_CH] = {0};
static enc_parameter_t enc_params[VIDEO_ENC_VALID_CH] = {0};
static isp_enc_ch_en_t v_ch_en = {{0}, {0}};

//static mm_siso_t *siso_enc_uvc   	 = NULL;
static mm_siso_t *siso_isp_h264      = NULL;
static mm_siso_t *siso_h264_uvc   	 = NULL;
static mm_siso_t *siso_isp_hevc      = NULL;
static mm_siso_t *siso_hevc_uvc   	 = NULL;
static mm_siso_t *siso_isp_enc       = NULL;
static mm_siso_t *siso_isp_uvc       = NULL;
static mm_siso_t *siso_isp_jpeg      = NULL;
static mm_siso_t *siso_jpeg_uvc   	 = NULL;
static mm_context_t *isp_ctx         = NULL;
static mm_context_t *enc_h264_ctx    = NULL;
static mm_context_t *enc_hevc_ctx    = NULL;
static mm_context_t *enc_jpeg_ctx    = NULL;
static mm_context_t *uvcd_ctx        = NULL;

#define MJPEG_ENABLE //It need to check the memory size.

void mmf2_video_example_uvcd_video_tuning(char *user_cmd)
{
	(void)user_cmd;

#ifdef UVC_SINGLE_STREAM
	printf("ERROR: UVC_SINGLE_STREAM enabled, tuning not supported\n");
	return;
#endif

	usbd_uvc_format_t *uvc_format_ptr = NULL;
	usbd_uvc_format_t *uvc_format_local = NULL;

	uvc_format_local = (usbd_uvc_format_t *)malloc(sizeof(usbd_uvc_format_t *));
	memset(uvc_format_local, 0, sizeof(usbd_uvc_format_t *));

	//usb_os_sema_create(&uvc_format_ptr->uvcd_change_sema);

	char *v_cmd = "--ainr=0 "
				  /* ch0 */
				  "--isp0:w=2688,h=1520,fps=30,fmt=1 "
				  "--enc0:w=2688,h=1520,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,fmt=2 "
				  "--enc1:w=2688,h=1520,fps=30,gop=30,bps=2000000,rcmode=3,minqp=26,maxqp=48,fmt=1 "
				  "--enc2:w=2688,h=1520,fps=1,gop=1,bps=1000000,rcmode=3,minqp=26,maxqp=48,qlevel=5,fmt=4 ";

	if (video_app_params_parsing(&v_ch_en, v_cmd, isp_params, enc_params) != 0) {
		printf("error: fail to parse video params\r\n");
		goto error;
	}

	uvc_format_local->format = -1;

	printf("foramr %d height %d width %d fps %d\r\n", uvc_format_local->format, uvc_format_local->height, uvc_format_local->width, uvc_format_local->fps);

	/* UVCD */
	uvcd_ctx = mm_module_open(&uvcd_module);
	if (uvcd_ctx) {

	} else {
		printf("uvc open fail\n\r");
		goto error;
	}

	uvc_format_ptr = usbd_uvc_get_format();

	/* ISP */
	isp_ctx = mm_module_open(&isp_module);
	if (isp_ctx) {
		mm_module_ctrl(isp_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
		mm_module_ctrl(isp_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(isp_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(isp_ctx, CMD_ISP_APPLY, 0);	// start channel 0
	} else {
		printf("ARRAY open fail\n\r");
		goto error;
	}

	/* ENC H264*/
	enc_h264_ctx = mm_module_open(&enc_module);
	if (enc_h264_ctx) {
		mm_module_ctrl(enc_h264_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V1_STREAM_ID]);
		mm_module_ctrl(enc_h264_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(enc_h264_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_h264_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_h264_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}

	/* ENC HEVC*/
	enc_hevc_ctx = mm_module_open(&enc_module);
	if (enc_hevc_ctx) {
		mm_module_ctrl(enc_hevc_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V2_STREAM_ID]);
		mm_module_ctrl(enc_hevc_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(enc_hevc_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_hevc_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_hevc_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
#ifdef MJPEG_ENABLE
	/* ENC JPEG*/
	enc_jpeg_ctx = mm_module_open(&enc_module);
	enc_params[V3_STREAM_ID].width = USBD_UVC_JPEG_FRAME_WIDTH;
	enc_params[V3_STREAM_ID].height = USBD_UVC_JPEG_FRAME_HEIGHT;
	if (enc_jpeg_ctx) {
		mm_module_ctrl(enc_jpeg_ctx, CMD_ENC_SET_PARAMS, (int)&enc_params[V3_STREAM_ID]);
		mm_module_ctrl(enc_jpeg_ctx, MM_CMD_SET_QUEUE_LEN, 1);
		mm_module_ctrl(enc_jpeg_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(enc_jpeg_ctx, CMD_ENC_INIT_MEM_POOL, 0);
		mm_module_ctrl(enc_jpeg_ctx, CMD_ENC_APPLY, 0);
	} else {
		printf("ENC open fail\n\r");
		goto error;
	}
#endif
	//--------------Link ISP H264---------------------------
	siso_isp_h264 = siso_create();
	if (siso_isp_h264) {
		siso_ctrl(siso_isp_h264, MMIC_CMD_ADD_INPUT, (uint32_t)isp_ctx, 0);
		siso_ctrl(siso_isp_h264, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_h264_ctx, 0);
		//siso_start(siso_isp_h264);
	} else {
		printf("siso_isp_enc open fail\n\r");
		goto error;
	}
	printf("siso_isp_h264 started\n\r");

	//--------------Link H264 UVC---------------------------
	siso_h264_uvc = siso_create();
	if (siso_h264_uvc) {
		siso_ctrl(siso_h264_uvc, MMIC_CMD_ADD_INPUT, (uint32_t)enc_h264_ctx, 0);
		siso_ctrl(siso_h264_uvc, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		//siso_start(siso_h264_uvc);
	} else {
		printf("siso_enc_h264 open fail\n\r");
		goto error;
	}

	//--------------Link ISP HEVC---------------------------
	siso_isp_hevc = siso_create();
	if (siso_isp_hevc) {
		siso_ctrl(siso_isp_hevc, MMIC_CMD_ADD_INPUT, (uint32_t)isp_ctx, 0);
		siso_ctrl(siso_isp_hevc, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_hevc_ctx, 0);
		//siso_start(siso_isp_hevc);
	} else {
		printf("siso_isp_enc open fail\n\r");
		goto error;
	}
	printf("siso_isp_hevc started\n\r");

	//--------------Link HEVC UVC---------------------------
	siso_hevc_uvc = siso_create();
	if (siso_hevc_uvc) {
		siso_ctrl(siso_hevc_uvc, MMIC_CMD_ADD_INPUT, (uint32_t)enc_hevc_ctx, 0);
		siso_ctrl(siso_hevc_uvc, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		//siso_start(siso_hevc_uvc);
	} else {
		printf("siso_enc_h264 open fail\n\r");
		goto error;
	}

	//--------------Link ISP UVC---------------------------
	siso_isp_uvc = siso_create();
	if (siso_isp_uvc) {
		siso_ctrl(siso_isp_uvc, MMIC_CMD_ADD_INPUT, (uint32_t)isp_ctx, 0);
		siso_ctrl(siso_isp_uvc, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		//siso_start(siso_isp_uvc);
	} else {
		printf("siso_isp_uvc open fail\n\r");
		goto error;
	}
	printf("siso_isp_uvc started\n\r");

	//--------------Link ISP JPEG---------------------------
	siso_isp_jpeg = siso_create();
	if (siso_isp_jpeg) {
		siso_ctrl(siso_isp_jpeg, MMIC_CMD_ADD_INPUT, (uint32_t)isp_ctx, 0);
		siso_ctrl(siso_isp_jpeg, MMIC_CMD_ADD_OUTPUT, (uint32_t)enc_jpeg_ctx, 0);
		//siso_start(siso_isp_h264);
	} else {
		printf("siso_isp_enc open fail\n\r");
		goto error;
	}
	printf("siso_isp_h264 started\n\r");

	//--------------Link JPEG UVC---------------------------
	siso_jpeg_uvc = siso_create();
	if (siso_jpeg_uvc) {
		siso_ctrl(siso_jpeg_uvc, MMIC_CMD_ADD_INPUT, (uint32_t)enc_jpeg_ctx, 0);
		siso_ctrl(siso_jpeg_uvc, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		//siso_start(siso_h264_uvc);
	} else {
		printf("siso_enc_jpeg open fail\n\r");
		goto error;
	}

	while (1) {
		printf("usb_os_sema_take\r\n");
		usb_os_sema_take(uvc_format_ptr->uvcd_change_sema, RTOS_MAX_DELAY);

		printf("f:%d h:%d s:%d w:%d\r\n", uvc_format_ptr->format, uvc_format_ptr->height, uvc_format_ptr->state, uvc_format_ptr->width);

		if (uvc_format_local->format != uvc_format_ptr->format) {
			if (uvc_format_ptr->format == USBD_UVC_FORMAT_TYPE_YUY2) {
				mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_STOP, 0);
				vTaskDelay(500);
				siso_pause(siso_isp_uvc);

				siso_pause(siso_isp_h264);
				siso_pause(siso_h264_uvc);

				siso_pause(siso_isp_hevc);
				siso_pause(siso_hevc_uvc);
#ifdef MJPEG_ENABLE
				siso_pause(siso_isp_jpeg);
				siso_pause(siso_jpeg_uvc);
#endif
				vTaskDelay(500);
				isp_params[V1_STREAM_ID].height = USBD_UVC_FRAME_HEIGHT;
				isp_params[V1_STREAM_ID].width  = USBD_UVC_FRAME_WIDTH;
				isp_params[V1_STREAM_ID].format = ISP_FORMAT_YUV422_SEMIPLANAR;
				printf("after parameters %d\r\n", isp_params[V1_STREAM_ID].format);
				mm_module_ctrl(isp_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
				mm_module_ctrl(isp_ctx, CMD_ISP_APPLY, 0);
				siso_resume(siso_isp_uvc);
			} else if (uvc_format_ptr->format == USBD_UVC_FORMAT_TYPE_NV12) {
				mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_STOP, 0);
				vTaskDelay(500);
				siso_pause(siso_isp_uvc);

				siso_pause(siso_isp_h264);
				siso_pause(siso_h264_uvc);

				siso_pause(siso_isp_hevc);
				siso_pause(siso_hevc_uvc);
#ifdef MJPEG_ENABLE
				siso_pause(siso_isp_jpeg);
				siso_pause(siso_jpeg_uvc);
#endif
				vTaskDelay(500);
				isp_params[V1_STREAM_ID].height = USBD_UVC_FRAME_HEIGHT;
				isp_params[V1_STREAM_ID].width  = USBD_UVC_FRAME_WIDTH;
				isp_params[V1_STREAM_ID].format = ISP_FORMAT_YUV420_SEMIPLANAR;
				printf("after parameters %d\r\n", isp_params[V1_STREAM_ID].format);
				mm_module_ctrl(isp_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
				mm_module_ctrl(isp_ctx, CMD_ISP_APPLY, 0);
				siso_resume(siso_isp_uvc);
			} else if (uvc_format_ptr->format == USBD_UVC_FORMAT_TYPE_H264) {
				mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_STOP, 0);
				vTaskDelay(500);
				siso_pause(siso_isp_uvc);

				siso_pause(siso_isp_h264);
				siso_pause(siso_h264_uvc);

				siso_pause(siso_isp_hevc);
				siso_pause(siso_hevc_uvc);
#ifdef MJPEG_ENABLE
				siso_pause(siso_isp_jpeg);
				siso_pause(siso_jpeg_uvc);
#endif
				vTaskDelay(500);
				isp_params[V1_STREAM_ID].height = USBD_UVC_FRAME_HEIGHT;
				isp_params[V1_STREAM_ID].width  = USBD_UVC_FRAME_WIDTH;
				isp_params[V1_STREAM_ID].format = ISP_FORMAT_YUV420_SEMIPLANAR;
				printf("after parameters %d\r\n", isp_params[V1_STREAM_ID].format);
				mm_module_ctrl(isp_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
				mm_module_ctrl(isp_ctx, CMD_ISP_APPLY, 0);
				siso_resume(siso_isp_h264);
				siso_resume(siso_h264_uvc);
			} else if (uvc_format_ptr->format == USBD_UVC_FORMAT_TYPE_H265) {
				mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_STOP, 0);
				vTaskDelay(500);
				siso_pause(siso_isp_uvc);

				siso_pause(siso_isp_h264);
				siso_pause(siso_h264_uvc);

				siso_pause(siso_isp_hevc);
				siso_pause(siso_hevc_uvc);
#ifdef MJPEG_ENABLE
				siso_pause(siso_isp_jpeg);
				siso_pause(siso_jpeg_uvc);
#endif
				vTaskDelay(500);
				isp_params[V1_STREAM_ID].height = USBD_UVC_FRAME_HEIGHT;
				isp_params[V1_STREAM_ID].width  = USBD_UVC_FRAME_WIDTH;
				isp_params[V1_STREAM_ID].format = ISP_FORMAT_YUV420_SEMIPLANAR;
				printf("after parameters %d\r\n", isp_params[V1_STREAM_ID].format);
				mm_module_ctrl(isp_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
				mm_module_ctrl(isp_ctx, CMD_ISP_APPLY, 0);
				siso_resume(siso_isp_hevc);
				siso_resume(siso_hevc_uvc);
			} else if (uvc_format_ptr->format == USBD_UVC_FORMAT_TYPE_MJPEG) {
#ifdef MJPEG_ENABLE
				mm_module_ctrl(isp_ctx, CMD_ISP_STREAM_STOP, 0);
				vTaskDelay(500);
				siso_pause(siso_isp_uvc);

				siso_pause(siso_isp_h264);
				siso_pause(siso_h264_uvc);

				siso_pause(siso_isp_hevc);
				siso_pause(siso_hevc_uvc);

				siso_pause(siso_isp_jpeg);
				siso_pause(siso_jpeg_uvc);

				vTaskDelay(500);
				isp_params[V1_STREAM_ID].height = USBD_UVC_JPEG_FRAME_HEIGHT;
				isp_params[V1_STREAM_ID].width  = USBD_UVC_JPEG_FRAME_WIDTH;
				isp_params[V1_STREAM_ID].format = ISP_FORMAT_YUV420_SEMIPLANAR;
				//printf("after parameters %d\r\n",isp_params[V1_STREAM_ID].format);
				mm_module_ctrl(isp_ctx, CMD_ISP_SET_PARAMS, (int)&isp_params[V1_STREAM_ID]);
				mm_module_ctrl(isp_ctx, CMD_ISP_APPLY, 0);
				siso_resume(siso_isp_jpeg);
				siso_resume(siso_jpeg_uvc);
#endif
			}
		}


	}

	return;
error:
	return;
}
