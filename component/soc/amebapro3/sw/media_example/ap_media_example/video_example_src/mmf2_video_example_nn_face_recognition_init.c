/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <string.h>
#include "ameba_soc.h"
#include "avcodec.h"
#include "mmf2_dbg.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_facerecog.h"
#include "module_rtsp2.h"
#include "module_uvcd.h"
#include "module_video.h"
#include "module_vipnn.h"
#include "nn_osd/nn_osd.h"
#include "video_api.h"
#include "video_common_type.h"

#include "model_mobilefacenet.h"
#include "model_scrfd.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 H.264 video -> runtime-selected sink (UVC or RTSP) with recognition OSD
 *   V5 RGB video -> SCRFD face detection -> MobileFaceNet -> facerecog module
 *
 * VIDEO list exposes separate UVC and RTSP entries at runtime. Face feature
 * registration and threshold changes are controlled by the FREG/FR* shell
 * commands near the bottom of this file.
 *
 * Customer configurable example settings
 *
 * The defines below are intended to be the first place to edit when adapting
 * this example. Keep the model input size in sync with the model binary shape.
 */

/* ---- User-configurable settings ----------------------------------------- */

/* Encoded output stream. Change these to match the desired output resolution,
 * frame rate, GOP length, and target bitrate. */
#define STREAM_WIDTH    1920
#define STREAM_HEIGHT   1080
#define STREAM_FPS      30
#define STREAM_GOP      30
#define STREAM_BPS      2000000

/* RGB stream for face detection NN input. These values must match the SCRFD
 * model input shape selected below. */
#define NN_WIDTH        576
#define NN_HEIGHT       320
#define NN_FPS          10

/* Face detection and face recognition models. Replace the model object and
 * vfs:/ path together when using a different NBG model. */
#define FACEDET_MODEL_OBJ       scrfd
#define FACENET_MODEL_OBJ       mbfacenet_fwfs
#define FACEDET_MODEL_NAME      "vfs:/scrfd_500m_bnkps_shape576x320.nb"
#define FACENET_MODEL_NAME      "vfs:/mobilefacenet_pcqsymi8.nb"

/* Set to 1 to clear stale OSD boxes automatically when draw callbacks stop. */
#define FACE_RECOG_OSD_WATCHDOG       1

/* ---- Internal constants (do not modify) ---------------------------------- */

#define FACE_RECOG_SINK_RTSP          0
#define FACE_RECOG_SINK_UVC           1
#define FACE_RECOG_OSD_KNOWN_LAYER    0   /* index into facerecog_osd_layer[] */
#define FACE_RECOG_OSD_UNKNOWN_LAYER  1

static mm_context_t *video_v1_ctx        = NULL;
static mm_context_t *rtsp_ctx            = NULL;
static mm_context_t *uvcd_ctx            = NULL;
static mm_context_t *video_rgb_ctx       = NULL;
static mm_context_t *facedet_ctx         = NULL;
static mm_context_t *facenet_ctx         = NULL;
static mm_context_t *facerecog_ctx       = NULL;

static mm_siso_t *siso_video_sink        = NULL;
static mm_siso_t *siso_rgb_facedet       = NULL;
static mm_siso_t *siso_facedet_facenet   = NULL;
static mm_siso_t *siso_facenet_facerecog = NULL;

static nn_osd_group_t face_recognition_osd;
static const nn_osd_layer_config_t face_recognition_osd_layer[] = {
	{
		.stream_id = V1_STREAM_ID,
		.codec = VIDEO_H264,
		.alpha = 128, /* OSD alpha */
		.y = 0,       /* green Y */
		.u = 0,       /* green U */
		.v = 0,       /* green V */
	},
	{
		.stream_id = V1_STREAM_ID,
		.codec = VIDEO_H264,
		.alpha = 128, /* OSD alpha */
		.y = 76,      /* red Y */
		.u = 85,      /* red U */
		.v = 255,     /* red V */
	},
};

static char *facedet_model_file_name = FACEDET_MODEL_NAME;
static char *facenet_model_file_name = FACENET_MODEL_NAME;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_H264,
	.width = STREAM_WIDTH,
	.height = STREAM_HEIGHT,
	.fps = STREAM_FPS,
	.gop = STREAM_GOP,
	.bps = STREAM_BPS,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
	.qlevel = 5,
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps = STREAM_FPS,
			.bps = 0,
		}
	}
};

static video_params_t video_rgb_params = {
	.stream_id = V5_STREAM_ID,
	.format = VIDEO_RGB,
	.width = NN_WIDTH,
	.height = NN_HEIGHT,
	.fps = NN_FPS,
};

static nn_data_param_t facedet_nn_input = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static nn_data_param_t facenet_nn_input = {
	.codec_type = AV_CODEC_ID_NN_RAW
};

static int face_recognition_sink_mode = FACE_RECOG_SINK_UVC;

static int face_recognition_is_unknown(const char *name)
{
	return name == NULL || strcmp(name, "unknown") == 0;
}

static void face_recognition_init_osd(void)
{
	/* Each bitmap OSD layer owns one hardware OSD index. Two layers are used
	 * here because known and unknown faces intentionally use different colors. */
	if (nn_osd_group_init(&face_recognition_osd, STREAM_WIDTH, STREAM_HEIGHT,
						  face_recognition_osd_layer, sizeof(face_recognition_osd_layer) / sizeof(face_recognition_osd_layer[0]),
						  FACE_RECOG_OSD_WATCHDOG) != 0) {
		RTK_LOGE(TAG, "error: fail to init OSD\r\n");
	}
}

static void face_recognition_draw_object(void *p, void *img_param)
{
	(void)img_param;
	frc_draw_t *fdraw = (frc_draw_t *)p;

	if (!fdraw) {
		return;
	}

	/* The OSD group owns one bitmap canvas per color. Lock it while updating
	 * canvases so the watchdog or other update paths cannot touch the same
	 * buffers at the same time. */
	if (nn_osd_group_lock(&face_recognition_osd) != 0) {
		return;
	}

	/* Draw each recognition callback as a complete frame. Clearing first also
	 * removes faces that existed in the previous callback but not this one. */
	nn_osd_group_clear(&face_recognition_osd);

	if (fdraw->pic_width <= 0 || fdraw->pic_height <= 0) {
		/* Invalid image size means there is nothing meaningful to map, so send
		 * empty canvases to disable all registered OSD layers. */
		nn_osd_group_clear_locked(&face_recognition_osd);
		nn_osd_group_unlock(&face_recognition_osd);
		return;
	}

	nn_data_param_t im = {
		.img = {
			.width = fdraw->pic_width,
			.height = fdraw->pic_height,
		},
	};
	nn_coord_map_t map;
	/* facerecog reports boxes in NN input coordinates. Map them back to the
	 * encoded output stream so boxes line up with the RTSP/UVC image. */
	nn_coord_map_init(&map, STREAM_WIDTH, STREAM_HEIGHT, &im, NN_COORD_MAP_SCALE_STRETCH);

	for (int i = 0; i < fdraw->obj_cnt; i++) {
		frc_bbox_t *bbox = &fdraw->bbox[i];
		const char *name = fdraw->obj_name[i] ? fdraw->obj_name[i] : "unknown";
		nn_osd_canvas_t *canvas = nn_osd_group_canvas(&face_recognition_osd,
								  face_recognition_is_unknown(name) ? FACE_RECOG_OSD_UNKNOWN_LAYER : FACE_RECOG_OSD_KNOWN_LAYER);
		int xmin, ymin, xmax, ymax;

		if (!canvas) {
			continue;
		}

		nn_coord_map_bbox(&map, bbox->xmin, bbox->ymin, bbox->xmax, bbox->ymax, &xmin, &ymin, &xmax, &ymax);
		/* Known faces are drawn on the green layer; unknown faces are drawn on
		 * the red layer. Bitmap OSD supports one color per OSD index. */
		nn_osd_draw_rect(canvas, xmin, ymin, xmax, ymax, 10);
		nn_osd_draw_stringf(canvas, xmin, ymin - 24, 3, "%s", name);

		RTK_LOGD(TAG, "face[%d] %s %d %d %d %d\r\n", i, name, xmin, ymin, xmax, ymax);
	}

	/* Flush all color layers, then kick the optional watchdog. If enabled and
	 * callbacks stop after faces leave the scene, stale boxes are cleared. */
	nn_osd_group_flush(&face_recognition_osd);
	nn_osd_group_kick(&face_recognition_osd, fdraw->obj_cnt > 0);

	nn_osd_group_unlock(&face_recognition_osd);
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

static void mmf2_video_example_nn_face_recognition_init(int sink_mode)
{
	// DBG_MMF_DEBUG_MSG_ON(_MMF_DBG_NN_);
	face_recognition_sink_mode = sink_mode;
	face_recognition_init_osd();

	/* V1: encoded video output stream.
	 * This stream carries H.264 to RTSP or UVC and is also where OSD is drawn. */
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v1 open fail\n\r");
		goto error;
	}

	/* Select the output sink. The encoded V1 stream is connected to exactly one
	 * sink in this example: UVC by default, or RTSP via the RTSP init entry. */
	RTK_LOGI(TAG, "face recognition sink mode: %s\n\r", face_recognition_sink_mode == FACE_RECOG_SINK_UVC ? "uvc" : "rtsp");
	if (face_recognition_sink_mode == FACE_RECOG_SINK_UVC) {
#if defined(CONFIG_USBD_UVC) && (CONFIG_USBD_UVC != 0)
		uvcd_ctx = mm_module_open(&uvcd_module);
		if (!uvcd_ctx) {
			RTK_LOGE(TAG, "uvc open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "UVC module opened\n\r");
#else
		RTK_LOGE(TAG, "UVC not supported: CONFIG_USBD_UVC not enabled\n\r");
		goto error;
#endif
	} else {
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
		rtsp_ctx = mm_module_open(&rtsp2_module);
		if (rtsp_ctx) {
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 0);
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v_params);
			/* Force an I frame when a client sends RTSP PLAY so the stream starts
			 * from a reference frame instead of waiting for the next GOP. */
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, ON);
		} else {
			RTK_LOGE(TAG, "RTSP2 open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "RTSP module opened\n\r");
#else
		RTK_LOGE(TAG, "RTSP not supported: CONFIG_LWIP_LAYER not enabled\n\r");
		goto error;
#endif
	}

	/* Link V1 encoded output to the selected sink. This path is independent of
	 * the NN path below, so streaming continues even if NN runs at lower FPS. */
	/* SISO connects one producer to one consumer in this path: V1 to either
	 * UVC or RTSP, chosen by the wrapper init function. */
	siso_video_sink = siso_create();
	if (siso_video_sink) {
		siso_ctrl(siso_video_sink, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		if (face_recognition_sink_mode == FACE_RECOG_SINK_UVC) {
			siso_ctrl(siso_video_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		} else {
			siso_ctrl(siso_video_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		}
		siso_start(siso_video_sink);
	} else {
		RTK_LOGE(TAG, "siso_video_sink create fail\n\r");
		goto error;
	}

	/* V5: RGB frames for NN input. Keep this resolution/FPS aligned with the
	 * face detection model and performance budget. */
	/* This RGB stream is not exposed to the user; it exists only to feed the
	 * cascaded NN pipeline. */
	video_rgb_ctx = mm_module_open(&video_module);
	if (video_rgb_ctx) {
		mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_rgb_params);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_SET_QUEUE_LEN, 2);
		mm_module_ctrl(video_rgb_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_rgb_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video rgb open fail\n\r");
		goto error;
	}

	/* First NN stage: SCRFD face detection. It starts a grouped result sequence
	 * that is consumed by the cascaded MobileFaceNet stage. */
	facedet_ctx = mm_module_open(&vipnn_module);
	if (facedet_ctx) {
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_SET_MODEL, (int)&FACEDET_MODEL_OBJ);
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_SET_MODEL_FILE_NAME, (int)facedet_model_file_name);
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&facedet_nn_input);
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_SET_OUTPUT, 1);
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(facedetect_res_t));
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_SET_RES_MAX_CNT, MAX_FACE_DETECT_NUM);
		mm_module_ctrl(facedet_ctx, MM_CMD_SET_DATAGROUP, MM_GROUP_START);
		mm_module_ctrl(facedet_ctx, MM_CMD_SET_QUEUE_LEN, 1);
		mm_module_ctrl(facedet_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(facedet_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "facedet VIPNN open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "facedet VIPNN opened\n\r");

	/* Second NN stage: MobileFaceNet runs once for each detected face ROI and
	 * outputs one feature vector per face. */
	facenet_ctx = mm_module_open(&vipnn_module);
	if (facenet_ctx) {
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_MODEL, (int)&FACENET_MODEL_OBJ);
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_MODEL_FILE_NAME, (int)facenet_model_file_name);
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&facenet_nn_input);
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_CASCADE, VIPNN_CMODE_ALL_ROI);
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_OUTPUT, 1);
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(face_feature_res_t));
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_SET_RES_MAX_CNT, MAX_FACE_DETECT_NUM);
		mm_module_ctrl(facenet_ctx, MM_CMD_SET_DATAGROUP, MM_GROUP_END);
		mm_module_ctrl(facenet_ctx, MM_CMD_SET_QUEUE_LEN, 1);
		mm_module_ctrl(facenet_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(facenet_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "facenet VIPNN open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "facenet VIPNN opened\n\r");

	/* Face recognition stage: compare MobileFaceNet features with registered
	 * features, then call face_recognition_draw_object() for OSD drawing. */
	facerecog_ctx = mm_module_open(&facerecog_module);
	if (facerecog_ctx) {
		mm_module_ctrl(facerecog_ctx, CMD_FRC_SET_THRES100, 99);
		mm_module_ctrl(facerecog_ctx, CMD_FRC_SET_OSD_DRAW, (int)face_recognition_draw_object);
	} else {
		RTK_LOGE(TAG, "FACERECOG open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "FACERECOG opened\n\r");

	/* Build the NN/recognition pipeline from downstream to upstream so queues
	 * are ready before RGB frames start flowing. */
	/* The following SISO links form a cascade:
	 *   facedet VIPNN -> facenet VIPNN -> facerecog module. */
	siso_facenet_facerecog = siso_create();
	if (siso_facenet_facerecog) {
		siso_ctrl(siso_facenet_facerecog, MMIC_CMD_ADD_INPUT, (uint32_t)facenet_ctx, 0);
		siso_ctrl(siso_facenet_facerecog, MMIC_CMD_ADD_OUTPUT, (uint32_t)facerecog_ctx, 0);
		siso_start(siso_facenet_facerecog);
	} else {
		RTK_LOGE(TAG, "siso_facenet_facerecog create fail\n\r");
		goto error;
	}

	/* Feed face detection results into MobileFaceNet cascade. */
	siso_facedet_facenet = siso_create();
	if (siso_facedet_facenet) {
		siso_ctrl(siso_facedet_facenet, MMIC_CMD_ADD_INPUT, (uint32_t)facedet_ctx, 0);
		siso_ctrl(siso_facedet_facenet, MMIC_CMD_ADD_OUTPUT, (uint32_t)facenet_ctx, 0);
		siso_start(siso_facedet_facenet);
	} else {
		RTK_LOGE(TAG, "siso_facedet_facenet create fail\n\r");
		goto error;
	}

	/* Start the RGB-to-face-detection link last. Once this starts, the full
	 * NN pipeline begins processing frames. */
	siso_rgb_facedet = siso_create();
	if (siso_rgb_facedet) {
		siso_ctrl(siso_rgb_facedet, MMIC_CMD_ADD_INPUT, (uint32_t)video_rgb_ctx, 0);
		siso_ctrl(siso_rgb_facedet, MMIC_CMD_SET_STACKSIZE, (uint32_t)1024 * 64, 0);
		siso_ctrl(siso_rgb_facedet, MMIC_CMD_SET_TASKPRIORITY, 3, 0);
		siso_ctrl(siso_rgb_facedet, MMIC_CMD_ADD_OUTPUT, (uint32_t)facedet_ctx, 0);
		siso_start(siso_rgb_facedet);
	} else {
		RTK_LOGE(TAG, "siso_rgb_facedet create fail\n\r");
		goto error;
	}

	RTK_LOGI(TAG, "face recognition %s started\n\r", face_recognition_sink_mode == FACE_RECOG_SINK_UVC ? "uvc" : "rtsp");
	return;

error:
	return;
}

void mmf2_video_example_nn_face_recognition_rtsp_init(void)
{
	mmf2_video_example_nn_face_recognition_init(FACE_RECOG_SINK_RTSP);
}

void mmf2_video_example_nn_face_recognition_uvc_init(void)
{
	mmf2_video_example_nn_face_recognition_init(FACE_RECOG_SINK_UVC);
}

u32 CmdFaceRecognitionRegister(IN u16 argc, IN u8 *argv[])
{
	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	if (argc < 1) {
		RTK_LOGE(TAG, "usage: FREG <name>\r\n");
		return _FALSE;
	}

	RTK_LOGI(TAG, "enter face register mode: %s\r\n", (char *)argv[0]);
	mm_module_ctrl(facerecog_ctx, CMD_FRC_REGISTER_MODE, (int)argv[0]);

	return _TRUE;
}

u32 CmdFaceRecognitionMode(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	mm_module_ctrl(facerecog_ctx, CMD_FRC_RECOGNITION_MODE, 0);
	RTK_LOGI(TAG, "face recognition mode enabled (FRRM cancels a pending FREG request)\r\n");

	return _TRUE;
}

u32 CmdFaceRecognitionLoad(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	RTK_LOGI(TAG, "load face recognition features\r\n");
	if (mm_module_ctrl(facerecog_ctx, CMD_FRC_LOAD_FEATURES, 0) < 0) {
		RTK_LOGE(TAG, "load face recognition features failed\r\n");
		return _FALSE;
	}

	return _TRUE;
}

u32 CmdFaceRecognitionSave(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	RTK_LOGI(TAG, "save face recognition features\r\n");
	if (mm_module_ctrl(facerecog_ctx, CMD_FRC_SAVE_FEATURES, 0) < 0) {
		RTK_LOGE(TAG, "save face recognition features failed\r\n");
		return _FALSE;
	}

	return _TRUE;
}

u32 CmdFaceRecognitionReset(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	RTK_LOGI(TAG, "reset face recognition features\r\n");
	mm_module_ctrl(facerecog_ctx, CMD_FRC_RESET_FEATURES, 0);

	return _TRUE;
}

u32 CmdFaceRecognitionList(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	RTK_LOGI(TAG, "list face recognition features\r\n");
	mm_module_ctrl(facerecog_ctx, CMD_FRC_LIST_FEATURES, 0);

	return _TRUE;
}

u32 CmdFaceRecognitionScore(IN u16 argc, IN u8 *argv[])
{
	int score;

	if (!facerecog_ctx) {
		RTK_LOGE(TAG, "facerecog_ctx not opened yet\r\n");
		return _FALSE;
	}

	if (argc < 1) {
		RTK_LOGE(TAG, "usage: FRSC <score>\r\n");
		return _FALSE;
	}

	score = strtol((char *)argv[0], NULL, 0);
	RTK_LOGI(TAG, "set face recognition score threshold to %.2f\r\n", (float)score / 100.0f);
	mm_module_ctrl(facerecog_ctx, CMD_FRC_SET_THRES100, score);

	return _TRUE;
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_nn_face_recognition_init[] = {
	{
		.v_func = mmf2_video_example_nn_face_recognition_uvc_init,
		.v_func_name = "mmf2_video_example_nn_face_recognition_uvc_init",
	},
	{
		.v_func = mmf2_video_example_nn_face_recognition_rtsp_init,
		.v_func_name = "mmf2_video_example_nn_face_recognition_rtsp_init",
	}
};

/*
 * Face recognition shell commands:
 * FREG <name>       Register the next single detected face with the given name.
 * FRRM              Cancel pending registration and return to recognition mode.
 * FRFL              Load registered features from vfs:/face_feature.bin.
 * FRFS              Save registered features to vfs:/face_feature.bin.
 * FRFR              Reset registered features in RAM.
 * FRLS              List registered features in RAM.
 * FRSC <score>      Set recognition threshold to score / 100.
 */
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE face_recognition_cmd[] = {
	{
		"FREG", CmdFaceRecognitionRegister
	},
	{
		"FRRM", CmdFaceRecognitionMode
	},
	{
		"FRFL", CmdFaceRecognitionLoad
	},
	{
		"FRFS", CmdFaceRecognitionSave
	},
	{
		"FRFR", CmdFaceRecognitionReset
	},
	{
		"FRLS", CmdFaceRecognitionList
	},
	{
		"FRSC", CmdFaceRecognitionScore
	},
};
