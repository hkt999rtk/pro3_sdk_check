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
#include "nn_mosaic/nn_mosaic.h"
#include "nn_osd/nn_osd.h"
#include "video_api.h"
#include "video_common_type.h"

#include "model_scrfd.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 H.264 video -> runtime-selected sink (UVC or RTSP) with green NN OSD
 *   V5 RGB video -> VIPNN SCRFD face detection
 *
 * VIDEO list exposes separate UVC and RTSP entries at runtime. Set
 * FACEDET_USE_ARRAY_INPUT to 1 only when a test wants array_module to feed NN
 * instead of the live V5 RGB stream.
 *
 * V1 is the encoded stream sent to RTSP or UVC and is also the OSD coordinate
 * space. V5 is the default RGB stream used for NN input. Set
 * FACEDET_V1_SINK_ENABLE to 0 to drop the V1 path entirely and run the NN
 * path only.
 */

/* ---- User-configurable settings ----------------------------------------- */

#define FACEDET_STREAM_WIDTH       1920
#define FACEDET_STREAM_HEIGHT      1080
#define FACEDET_STREAM_FPS         30
#define FACEDET_STREAM_GOP         30
#define FACEDET_STREAM_BPS         2000000

#define FACEDET_NN_WIDTH           576
#define FACEDET_NN_HEIGHT          320
#define FACEDET_NN_FPS             30

#define FACEDET_MODEL_OBJ          scrfd
#define FACEDET_MODEL_NAME         "vfs:/scrfd_500m_bnkps_shape576x320.nb"

/* Set to 1 to feed VIPNN from array_module instead of video_module V5. */
#ifndef FACEDET_USE_ARRAY_INPUT
#define FACEDET_USE_ARRAY_INPUT    0
#endif

/* Set to 0 to disable the V1 H.264 video -> sink path and run the NN
 * detection path only. */
#define FACEDET_V1_SINK_ENABLE     1

#define FACEDET_OSD_ENABLE         0  /* set to 1 to draw NN boxes, labels, and landmarks */
#define FACEDET_MOSAIC_ENABLE      1  /* set to 0 to compile out all mosaic code */
#define FACEDET_MOSAIC_DEFAULT_EN  1  /* runtime default; toggled by CmdFaceDetectionMosaic */
#define FACEDET_MOSAIC_SIZE        NN_MOSAIC_SIZE_16x16  /* mosaic block size */

/* ---- Internal constants (do not modify) ---------------------------------- */

#define FACEDET_SINK_RTSP          0
#define FACEDET_SINK_UVC           1
#define FACEDET_MOSAIC_CODEC_ALIGN 16   /* H.264 macroblock size; not a tunable parameter */
#define FACEDET_OSD_LAYERS         1    /* OSD bitmap layers allocated when FACEDET_OSD_ENABLE=1; deducted from mosaic pool */

/* OSD and mosaic both draw onto the V1 stream, so force them off when V1 is disabled. */
#if !FACEDET_V1_SINK_ENABLE
#undef FACEDET_OSD_ENABLE
#define FACEDET_OSD_ENABLE        0
#undef FACEDET_MOSAIC_ENABLE
#define FACEDET_MOSAIC_ENABLE     0
#endif

#if FACEDET_USE_ARRAY_INPUT
#include "module_array.h"
#endif

#if FACEDET_V1_SINK_ENABLE
static mm_context_t *video_v1_ctx     = NULL;
#endif
static mm_context_t *video_rgb_ctx    = NULL;
static mm_context_t *array_ctx        = NULL;
static mm_context_t *vipnn_ctx        = NULL;
#if FACEDET_V1_SINK_ENABLE
static mm_context_t *uvcd_ctx         = NULL;
static mm_context_t *rtsp_ctx         = NULL;

static mm_siso_t *siso_video_sink     = NULL;
#endif
static mm_siso_t *siso_rgb_vipnn      = NULL;

#if FACEDET_OSD_ENABLE
static nn_osd_group_t facedet_osd;
static const nn_osd_layer_config_t facedet_osd_layer[] = {
	{
		.stream_id = V1_STREAM_ID,
		.codec = VIDEO_H264,
		.alpha = 128,
		.y = 0,
		.u = 0,
		.v = 0,
	},
};
#endif

static char *nn_model_file_name = FACEDET_MODEL_NAME;
#if FACEDET_V1_SINK_ENABLE
static int facedet_sink_mode = FACEDET_SINK_UVC;
#endif
#if FACEDET_MOSAIC_ENABLE
static volatile int facedet_mosaic_enable = FACEDET_MOSAIC_DEFAULT_EN;
static nn_mosaic_group_t facedet_mosaic_grp;
#endif

#if FACEDET_V1_SINK_ENABLE
static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_H264,
	.width = FACEDET_STREAM_WIDTH,
	.height = FACEDET_STREAM_HEIGHT,
	.fps = FACEDET_STREAM_FPS,
	.gop = FACEDET_STREAM_GOP,
	.bps = FACEDET_STREAM_BPS,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
	.qlevel = 5,
};
#endif

static video_params_t video_rgb_params = {
	.stream_id = V5_STREAM_ID,
	.format = VIDEO_RGB,
	.width = FACEDET_NN_WIDTH,
	.height = FACEDET_NN_HEIGHT,
	.fps = FACEDET_NN_FPS,
};

#if FACEDET_V1_SINK_ENABLE
static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps = FACEDET_STREAM_FPS,
			.bps = 0,
		}
	}
};
#endif

#if FACEDET_USE_ARRAY_INPUT
static array_params_t rgb_array_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.codec_id = AV_CODEC_ID_RGB888,
	.mode = ARRAY_MODE_LOOP,
	.u = {
		.v = {
			.fps = FACEDET_NN_FPS,
		}
	}
};
#endif

static nn_data_param_t nn_input_params = {
	.img = {
		.width = FACEDET_NN_WIDTH,
		.height = FACEDET_NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static int facedet_init_overlay(void)
{
#if FACEDET_OSD_ENABLE
	/* One bitmap OSD layer maps to one hardware OSD index. Face detection uses
	 * one green layer so boxes, labels, and landmarks share the same bitmap. */
	if (nn_osd_group_init(&facedet_osd, FACEDET_STREAM_WIDTH, FACEDET_STREAM_HEIGHT,
						  facedet_osd_layer, sizeof(facedet_osd_layer) / sizeof(facedet_osd_layer[0]),
						  0) != 0) {
		RTK_LOGE(TAG, "error: fail to init OSD group\r\n");
	}
#endif

#if FACEDET_MOSAIC_ENABLE
	nn_mosaic_config_t mosaic_cfg = {
		.stream_id    = V1_STREAM_ID,
		.stream_width  = FACEDET_STREAM_WIDTH,
		.stream_height = FACEDET_STREAM_HEIGHT,
		.max_regions  = NN_MOSAIC_MAX_REGIONS - (FACEDET_OSD_ENABLE ? FACEDET_OSD_LAYERS : 0),
		.mosaic_size  = FACEDET_MOSAIC_SIZE,
		.codec_align  = FACEDET_MOSAIC_CODEC_ALIGN,
	};
	if (nn_mosaic_group_init(&facedet_mosaic_grp, &mosaic_cfg) != 0) {
		RTK_LOGE(TAG, "mosaic group init fail\r\n");
		return -1;
	}
#endif
	return 0;
}


static void nn_display_cb(void *p, void *img_param)
{
	vipnn_out_buf_t *out;
	nn_data_param_t *im;
	facedetect_res_t *face_res;
	int obj_num;
#if FACEDET_OSD_ENABLE || FACEDET_MOSAIC_ENABLE
	nn_coord_map_t map;
#endif
#if FACEDET_OSD_ENABLE
	int osd_ready = 0;
#endif
#if FACEDET_MOSAIC_ENABLE
	int mosaic_count = 0;
#endif

	if (!p || !img_param) {
		return;
	}

	out = (vipnn_out_buf_t *)p;
	im = (nn_data_param_t *)img_param;
	face_res = (facedetect_res_t *)&out->res[0];
	obj_num = out->res_cnt;

#if FACEDET_OSD_ENABLE
	if (nn_osd_group_lock(&facedet_osd) == 0) {
		nn_osd_group_clear(&facedet_osd);
		osd_ready = 1;
	}
#endif

	/* Map normalized NN coordinates to stream pixels; used by both OSD and mosaic. */
#if FACEDET_OSD_ENABLE || FACEDET_MOSAIC_ENABLE
	nn_coord_map_init(&map, FACEDET_STREAM_WIDTH, FACEDET_STREAM_HEIGHT, im, NN_COORD_MAP_SCALE_LETTERBOX);
#endif

	RTK_LOGD(TAG, "face num = %d\r\n", obj_num);
	for (int i = 0; i < obj_num; i++) {
		int obj_class = (int)face_res[i].result[0];
		if (obj_class == -1) {
			continue;
		}

#if FACEDET_OSD_ENABLE || FACEDET_MOSAIC_ENABLE
		int xmin, ymin, xmax, ymax;
		nn_coord_map_bbox(&map,
						  face_res[i].result[2], face_res[i].result[3],
						  face_res[i].result[4], face_res[i].result[5],
						  &xmin, &ymin, &xmax, &ymax);
#endif

#if FACEDET_MOSAIC_ENABLE
		if (facedet_mosaic_enable &&
			mosaic_count < facedet_mosaic_grp.max_regions &&
			nn_mosaic_group_set_bbox(&facedet_mosaic_grp, mosaic_count, xmin, ymin, xmax, ymax) == 0) {
			mosaic_count++;
		}
#endif

#if FACEDET_OSD_ENABLE
		nn_osd_canvas_t *canvas = osd_ready ? nn_osd_group_canvas(&facedet_osd, 0) : NULL;
		if (canvas) {
			nn_osd_draw_rect(canvas, xmin, ymin, xmax, ymax, 10);
			nn_osd_draw_stringf(canvas, xmin, ymin - 24, 3, "face %d", (int)(face_res[i].result[1] * 100));

			for (int j = 0; j < 5; j++) {
				int x, y;
				nn_coord_map_point(&map,
								   face_res[i].landmark.pos[j].x,
								   face_res[i].landmark.pos[j].y,
								   &x, &y);
				nn_osd_draw_point(canvas, x, y, 5);
			}
		}
#endif

#if FACEDET_OSD_ENABLE || FACEDET_MOSAIC_ENABLE
		RTK_LOGD(TAG, "face[%d] %d %d %d %d\r\n", i, xmin, ymin, xmax, ymax);
#endif
	}

#if FACEDET_MOSAIC_ENABLE
	if (!facedet_mosaic_enable) {
		mosaic_count = 0;
	}
	nn_mosaic_group_commit(&facedet_mosaic_grp, mosaic_count);
#endif

#if FACEDET_OSD_ENABLE
	if (osd_ready) {
		nn_osd_group_flush(&facedet_osd);
		nn_osd_group_unlock(&facedet_osd);
	}
#endif
}

#if FACEDET_V1_SINK_ENABLE
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

static int facedet_open_sink(void)
{
	RTK_LOGI(TAG, "face detection sink mode: %s\r\n", facedet_sink_mode == FACEDET_SINK_UVC ? "uvc" : "rtsp");

	if (facedet_sink_mode == FACEDET_SINK_UVC) {
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
#endif /* FACEDET_V1_SINK_ENABLE */

static int facedet_open_nn_input(void)
{
#if FACEDET_USE_ARRAY_INPUT
	array_t array;

	array.data_addr = (uint32_t)malloc(FACEDET_NN_WIDTH * FACEDET_NN_HEIGHT * 3);
	array.data_len = (uint32_t)FACEDET_NN_WIDTH * FACEDET_NN_HEIGHT * 3;
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

static void mmf2_video_example_nn_face_detection_init(int sink_mode)
{
	DBG_MMF_INFO_MSG_OFF(0xffffffff);
#if FACEDET_V1_SINK_ENABLE
	facedet_sink_mode = sink_mode;
#else
	(void)sink_mode;
#endif
	if (facedet_init_overlay() != 0) {
		goto error;
	}

#if FACEDET_V1_SINK_ENABLE
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

	if (facedet_open_sink() != 0) {
		goto error;
	}

	/* Link the encoded V1 stream to exactly one runtime-selected sink. */
	siso_video_sink = siso_create();
	if (siso_video_sink) {
		siso_ctrl(siso_video_sink, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		if (facedet_sink_mode == FACEDET_SINK_UVC) {
			siso_ctrl(siso_video_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		} else {
			siso_ctrl(siso_video_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		}
		siso_start(siso_video_sink);
	} else {
		RTK_LOGE(TAG, "siso_video_sink create fail\r\n");
		goto error;
	}
#endif

	if (facedet_open_nn_input() != 0) {
		goto error;
	}

	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&FACEDET_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL_FILE_NAME, (int)nn_model_file_name);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&nn_input_params);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(facedetect_res_t));
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, MAX_FACE_DETECT_NUM);
		mm_module_ctrl(vipnn_ctx, MM_CMD_SET_QUEUE_LEN, 1);
		mm_module_ctrl(vipnn_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "VIPNN open fail\r\n");
		goto error;
	}
	RTK_LOGI(TAG, "VIPNN opened\r\n");

	/* Link NN input frames to VIPNN. The input side is selected at compile
	 * time by FACEDET_USE_ARRAY_INPUT. */
	siso_rgb_vipnn = siso_create();
	if (siso_rgb_vipnn) {
#if FACEDET_USE_ARRAY_INPUT
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

#if FACEDET_V1_SINK_ENABLE
	RTK_LOGI(TAG, "face detection %s started\r\n", facedet_sink_mode == FACEDET_SINK_UVC ? "uvc" : "rtsp");
#else
	RTK_LOGI(TAG, "face detection nn-only started\r\n");
#endif
	return;

error:
	return;
}

void mmf2_video_example_nn_face_detection_rtsp_init(void)
{
	mmf2_video_example_nn_face_detection_init(FACEDET_SINK_RTSP);
}

void mmf2_video_example_nn_face_detection_uvc_init(void)
{
	mmf2_video_example_nn_face_detection_init(FACEDET_SINK_UVC);
}

#if FACEDET_MOSAIC_ENABLE
u32 CmdFaceDetectionMosaic(IN u16 argc, IN u8 *argv[])
{
	int enable;

	if (argc < 1) {
		RTK_LOGE(TAG, "usage: FDMOS <0|1>\r\n");
		return _FALSE;
	}

	enable = atoi((const char *)argv[0]) ? 1 : 0;
	facedet_mosaic_enable = enable;
	if (!enable && video_v1_ctx) {
		nn_mosaic_group_disable_all(&facedet_mosaic_grp);
	}

	RTK_LOGI(TAG, "face detection mosaic %s\r\n", enable ? "enabled" : "disabled");
	return _TRUE;
}
#endif

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_nn_face_detection_init[] = {
	{
		.v_func = mmf2_video_example_nn_face_detection_uvc_init,
		.v_func_name = "mmf2_video_example_nn_face_detection_uvc_init",
	},

	{
		.v_func = mmf2_video_example_nn_face_detection_rtsp_init,
		.v_func_name = "mmf2_video_example_nn_face_detection_rtsp_init",
	}
};

#if FACEDET_MOSAIC_ENABLE
/*
 * Face detection shell command:
 * FDMOS <0|1>   Disable/enable privacy mosaic on detected face boxes.
 */
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE face_detection_cmd[] = {
	{
		"FDMOS", CmdFaceDetectionMosaic
	},
};
#endif
