/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "ameba_soc.h"
#include "avcodec.h"
#include "mmf2_dbg.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_rtsp2.h"
#include "module_uvcd.h"
#include "module_video.h"
#include "module_vipnn.h"
#include "nn_osd/nn_osd.h"
#include "video_api.h"
#include "video_common_type.h"

#include "model_yolo.h"
#include "model_yolov9.h"
#include "nn_utils/class_name.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 H.264 video -> runtime-selected sink (UVC or RTSP) with object OSD
 *   V5 RGB video -> VIPNN object detection
 *
 * VIDEO list exposes separate UVC and RTSP entries at runtime. Set
 * OBJDET_USE_ARRAY_INPUT to 1 only when a test wants array_module to feed NN
 * instead of the live V5 RGB stream.
 *
 * V1 is the encoded stream sent to RTSP or UVC and is also the OSD coordinate
 * space. V5 is the default RGB stream used for NN input.
 */

/* ---- User-configurable settings ----------------------------------------- */

#define OBJDET_STREAM_WIDTH       1920
#define OBJDET_STREAM_HEIGHT      1080
#define OBJDET_STREAM_FPS         30
#define OBJDET_STREAM_GOP         30
#define OBJDET_STREAM_BPS         2000000

#define OBJDET_NN_WIDTH           416
#define OBJDET_NN_HEIGHT          416
#define OBJDET_NN_FPS             10

/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m, yolov9_tiny */
#define OBJDET_MODEL_OBJ          yolov4_tiny
#define OBJDET_MODEL_NAME         "vfs:/yolov4_tiny_asymu8.nb"

/* Set to 1 to feed VIPNN from array_module instead of video_module V5. */
#ifndef OBJDET_USE_ARRAY_INPUT
#define OBJDET_USE_ARRAY_INPUT    0
#endif

/* ---- Internal constants (do not modify) ---------------------------------- */

#define OBJDET_SINK_RTSP          0
#define OBJDET_SINK_UVC           1
#define OBJDET_OSD_LAYER          0

#if OBJDET_USE_ARRAY_INPUT
#include "module_array.h"
#endif

static mm_context_t *video_v1_ctx     = NULL;
static mm_context_t *video_rgb_ctx    = NULL;
static mm_context_t *array_ctx        = NULL;
static mm_context_t *vipnn_ctx        = NULL;
static mm_context_t *uvcd_ctx         = NULL;
static mm_context_t *rtsp_ctx         = NULL;

static mm_siso_t *siso_video_sink     = NULL;
static mm_siso_t *siso_rgb_vipnn      = NULL;

static nn_osd_group_t objdet_osd;
static const nn_osd_layer_config_t objdet_osd_layer[] = {
	{
		.stream_id = V1_STREAM_ID,
		.codec = VIDEO_H264,
		.alpha = 128,
		.y = 0,
		.u = 0,
		.v = 0,
	},
};

static char *nn_model_file_name = OBJDET_MODEL_NAME;
static int objdet_sink_mode = OBJDET_SINK_UVC;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_H264,
	.width = OBJDET_STREAM_WIDTH,
	.height = OBJDET_STREAM_HEIGHT,
	.fps = OBJDET_STREAM_FPS,
	.gop = OBJDET_STREAM_GOP,
	.bps = OBJDET_STREAM_BPS,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
	.qlevel = 5,
};

static video_params_t video_rgb_params = {
	.stream_id = V5_STREAM_ID,
	.format = VIDEO_RGB,
	.width = OBJDET_NN_WIDTH,
	.height = OBJDET_NN_HEIGHT,
	.fps = OBJDET_NN_FPS,
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps = OBJDET_STREAM_FPS,
			.bps = 0,
		}
	}
};

#if OBJDET_USE_ARRAY_INPUT
static array_params_t rgb_array_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_RGB888,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps = OBJDET_NN_FPS,
		}
	}
};
#endif

static nn_data_param_t nn_input_params = {
	.img = {
		.width = OBJDET_NN_WIDTH,
		.height = OBJDET_NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static void objdet_init_osd(void)
{
	/* One bitmap OSD layer maps to one hardware OSD index. Object detection
	 * uses one green layer for all boxes and labels. */
	if (nn_osd_group_init(&objdet_osd, OBJDET_STREAM_WIDTH, OBJDET_STREAM_HEIGHT,
						  objdet_osd_layer, sizeof(objdet_osd_layer) / sizeof(objdet_osd_layer[0]),
						  0) != 0) {
		RTK_LOGE(TAG, "error: fail to init OSD group\r\n");
	}
}

static const char *objdet_class_name(int class_id)
{
	if (class_id >= 0 && class_id < 80) {
		return coco_name_get_by_id(class_id);
	}

	return "obj";
}

static void nn_display_cb(void *p, void *img_param)
{
	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	nn_data_param_t *im = (nn_data_param_t *)img_param;

	if (!p || !img_param) {
		return;
	}

	objdetect_res_t *res = (objdetect_res_t *)&out->res[0];
	int obj_num = out->res_cnt;
	nn_coord_map_t map;

	if (nn_osd_group_lock(&objdet_osd) != 0) {
		return;
	}

	/* Redraw the whole detection overlay on each callback. Coordinates from
	 * post-processing are normalized, then mapped to V1 stream pixels. */
	nn_osd_group_clear(&objdet_osd);
	nn_coord_map_init(&map, OBJDET_STREAM_WIDTH, OBJDET_STREAM_HEIGHT, im, NN_COORD_MAP_SCALE_STRETCH);

	RTK_LOGD(TAG, "object num = %d\r\n", obj_num);
	for (int i = 0; i < obj_num; i++) {
		int class_id = (int)res[i].result[0];
		if (class_id == -1) {
			continue;
		}

		int xmin, ymin, xmax, ymax;
		nn_coord_map_bbox(&map,
						  res[i].result[2], res[i].result[3],
						  res[i].result[4], res[i].result[5],
						  &xmin, &ymin, &xmax, &ymax);

		nn_osd_canvas_t *canvas = nn_osd_group_canvas(&objdet_osd, OBJDET_OSD_LAYER);
		if (canvas) {
			nn_osd_draw_rect(canvas, xmin, ymin, xmax, ymax, 10);
			nn_osd_draw_stringf(canvas, xmin, ymin - 24, 3, "%s %d",
								objdet_class_name(class_id), (int)(res[i].result[1] * 100));
		}

		RTK_LOGD(TAG, "object[%d] c%d %d %d %d %d\r\n", i, class_id, xmin, ymin, xmax, ymax);
	}

	nn_osd_group_flush(&objdet_osd);
	nn_osd_group_unlock(&objdet_osd);
}

#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
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
#endif

static int objdet_open_sink(void)
{
	RTK_LOGI(TAG, "object detection sink mode: %s\r\n", objdet_sink_mode == OBJDET_SINK_UVC ? "uvc" : "rtsp");

	if (objdet_sink_mode == OBJDET_SINK_UVC) {
#if defined(CONFIG_USBD_UVC) && (CONFIG_USBD_UVC != 0)
		uvcd_ctx = mm_module_open(&uvcd_module);
		if (!uvcd_ctx) {
			RTK_LOGE(TAG, "uvc open fail\r\n");
			return -1;
		}
		RTK_LOGI(TAG, "UVC module opened\r\n");
		return 0;
#else
		RTK_LOGE(TAG, "UVC not supported: CONFIG_USBD_UVC not enabled\r\n");
		return -1;
#endif
	}

#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
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
		RTK_LOGE(TAG, "RTSP2 open fail\r\n");
		return -1;
	}
	RTK_LOGI(TAG, "RTSP module opened\r\n");
	return 0;
#else
	RTK_LOGE(TAG, "RTSP not supported: CONFIG_LWIP_LAYER not enabled\r\n");
	return -1;
#endif
}

static int objdet_open_nn_input(void)
{
#if OBJDET_USE_ARRAY_INPUT
	array_t array;

	array.data_addr = (uint32_t)malloc(OBJDET_NN_WIDTH * OBJDET_NN_HEIGHT * 3);
	array.data_len = (uint32_t)OBJDET_NN_WIDTH * OBJDET_NN_HEIGHT * 3;
	if (!array.data_addr) {
		RTK_LOGE(TAG, "array buffer malloc fail\r\n");
		return -1;
	}

	array_ctx = mm_module_open(&array_module);
	if (array_ctx) {
		mm_module_ctrl(array_ctx, CMD_ARRAY_SET_PARAMS, (int)&rgb_array_params);
		mm_module_ctrl(array_ctx, CMD_ARRAY_SET_ARRAY, (int)&array);
		mm_module_ctrl(array_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(array_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(array_ctx, CMD_ARRAY_APPLY, 0);
		mm_module_ctrl(array_ctx, CMD_ARRAY_STREAMING, 1);
	} else {
		RTK_LOGE(TAG, "ARRAY open fail\r\n");
		return -1;
	}
	RTK_LOGI(TAG, "ARRAY NN input opened\r\n");
#else
	/* Default NN input path: V5 RGB frames come from video_module. Use the
	 * array path only for synthetic/input-buffer tests. */
	video_rgb_ctx = mm_module_open(&video_module);
	if (video_rgb_ctx) {
		mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_rgb_params);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video rgb open fail\r\n");
		return -1;
	}
	RTK_LOGI(TAG, "video rgb NN input opened\r\n");
#endif

	return 0;
}

static void mmf2_video_example_nn_object_detection_init(int sink_mode)
{
	objdet_sink_mode = sink_mode;
	objdet_init_osd();

	/* V1 is the encoded stream shown to the user. OSD is drawn in this stream's
	 * coordinate space before frames reach UVC or RTSP. */
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v1 open fail\r\n");
		goto error;
	}

	if (objdet_open_sink() != 0) {
		goto error;
	}

	/* Link the encoded V1 stream to exactly one runtime-selected sink. */
	siso_video_sink = siso_create();
	if (siso_video_sink) {
		siso_ctrl(siso_video_sink, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		if (objdet_sink_mode == OBJDET_SINK_UVC) {
			siso_ctrl(siso_video_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		} else {
			siso_ctrl(siso_video_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		}
		siso_start(siso_video_sink);
	} else {
		RTK_LOGE(TAG, "siso_video_sink create fail\r\n");
		goto error;
	}

	if (objdet_open_nn_input() != 0) {
		goto error;
	}

	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&OBJDET_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL_FILE_NAME, (int)nn_model_file_name);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&nn_input_params);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(objdetect_res_t));
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, 32);
		mm_module_ctrl(vipnn_ctx, MM_CMD_SET_QUEUE_LEN, 1);
		mm_module_ctrl(vipnn_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "VIPNN open fail\r\n");
		goto error;
	}
	RTK_LOGI(TAG, "VIPNN opened\r\n");

	/* Link NN input frames to VIPNN. The input side is selected at compile
	 * time by OBJDET_USE_ARRAY_INPUT. */
	siso_rgb_vipnn = siso_create();
	if (siso_rgb_vipnn) {
#if OBJDET_USE_ARRAY_INPUT
		siso_ctrl(siso_rgb_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)array_ctx, 0);
#else
		siso_ctrl(siso_rgb_vipnn, MMIC_CMD_ADD_INPUT, (uint32_t)video_rgb_ctx, 0);
#endif
		siso_ctrl(siso_rgb_vipnn, MMIC_CMD_SET_STACKSIZE, (uint32_t)1024 * 64, 0);
		siso_ctrl(siso_rgb_vipnn, MMIC_CMD_SET_TASKPRIORITY, 3, 0);
		siso_ctrl(siso_rgb_vipnn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
		siso_start(siso_rgb_vipnn);
	} else {
		RTK_LOGE(TAG, "siso_rgb_vipnn create fail\r\n");
		goto error;
	}

	RTK_LOGI(TAG, "object detection %s started\r\n", objdet_sink_mode == OBJDET_SINK_UVC ? "uvc" : "rtsp");
	return;

error:
	return;
}

void mmf2_video_example_nn_object_detection_rtsp_init(void)
{
	mmf2_video_example_nn_object_detection_init(OBJDET_SINK_RTSP);
}

void mmf2_video_example_nn_object_detection_uvc_init(void)
{
	mmf2_video_example_nn_object_detection_init(OBJDET_SINK_UVC);
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_nn_object_detection_init[] = {
	{
		.v_func = mmf2_video_example_nn_object_detection_uvc_init,
		.v_func_name = "mmf2_video_example_nn_object_detection_uvc_init",
	},
	{
		.v_func = mmf2_video_example_nn_object_detection_rtsp_init,
		.v_func_name = "mmf2_video_example_nn_object_detection_rtsp_init",
	}
};
