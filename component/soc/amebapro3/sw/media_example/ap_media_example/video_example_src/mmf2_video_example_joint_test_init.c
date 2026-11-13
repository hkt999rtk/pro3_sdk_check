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
#include "mmf2_dbg.h"
#include "module_video.h"
#include "module_filesaver.h"
#include "module_vipnn.h"
#include "module_uvcd.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"
#include "module_mp4.h"
#include "module_rtsp2.h"
#include "vfs.h"
#include "nn_osd/nn_osd.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "sensor.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 HEVC video -> MP4/filesaver recording
 *   V2 H.264 video -> runtime-selected live sink (UVC or RTSP)
 *   V3 JPEG video -> snapshot filesaver
 *   V4 NV12 video -> motion/raw-frame path
 *   V5 RGB video -> optional VIPNN object detection and OSD
 *
 * This is the broad integration test. VIDEO list exposes separate UVC and RTSP
 * entries at runtime while the example code keeps all channel parameters in the
 * blocks below.
 */

/* ---- User-configurable settings ----------------------------------------- */

/* Video channel enable */
#define V1_ENA       1  /* V1(CH0) for video recording */
#define V2_ENA       1  /* V2(CH1) for live streaming */
#define V3_ENA       1  /* V3(CH2) for snapshot */
#define V4_ENA       1  /* V4(CH3) for motion detection */
#define V5_ENA       1  /* V5(CH4) for NN AI detection/recognition */

#define AINR_ENA     1  /* set to 0 to disable AI noise reduction */
#define MP4_ENA      1  /* set to 0 to disable MP4 SD card recording on V1 */
#define VAPP_ROI_ENA 1  /* set to 0 to restore the original encoder headers/bitstream and disable runtime ROI */

#define NN_ENA       0  /* set to 1 to enable NN model detection on V5 RGB channel */
#define NN_OSD_ENA   1  /* set to 0 to disable NN detection OSD overlay on streaming channel */

#if NN_ENA
#include "model_yolo.h"
/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m */
#define NN_MODEL_OBJ    yolov4_tiny
#define NN_MODEL_NAME   "vfs:/yolov4_tiny_asymu8.nb"
/* must match the model input shape; set equal to V5_RGB_WIDTH/HEIGHT if the model has no in-graph resize */
#define NN_WIDTH        416
#define NN_HEIGHT       416
#endif

/* V5 RGB output size; set equal to NN_WIDTH/HEIGHT when using a model without in-graph resize */
#define V5_RGB_WIDTH    416
#define V5_RGB_HEIGHT   416

/* ---- Internal constants (do not modify) ---------------------------------- */

#define V2_SINK_RTSP    0
#define V2_SINK_UVC     1
#define NN_OSD_LAYER    0
#define VAPP_ROI_COUNT  2  /* encoder hardware supports two ROIs */
#define VAPP_H264_MB_SIZE  16
#define VAPP_HEVC_CTB_SIZE 64

static mm_context_t *video_v1_ctx       = NULL;
static mm_context_t *video_v2_ctx       = NULL;
static mm_context_t *video_v3_ctx       = NULL;
static mm_context_t *video_v4_ctx       = NULL;
static mm_context_t *video_v5_ctx       = NULL;

static int v2_sink_mode = V2_SINK_UVC;

static mm_context_t *fs_v1_ctx          = NULL;
static mm_context_t *fs_v3_ctx          = NULL;
static mm_context_t *fs_v4_ctx          = NULL;
static mm_context_t *fs_v5_ctx          = NULL;
static mm_context_t *vipnn_ctx          = NULL;
static mm_context_t *uvcd_ctx           = NULL;
static mm_context_t *rtsp_ctx           = NULL;
static mm_context_t *mp4_ctx            = NULL;

static mm_siso_t *siso_v1               = NULL;
static mm_siso_t *siso_v2               = NULL;
static mm_siso_t *siso_v3               = NULL;
static mm_siso_t *siso_v4               = NULL;
static mm_siso_t *siso_v5               = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_HEVC,  //VIDEO_HEVC_JPEG
	.width = 2688,
	.height = 1520,
	.fps = 15,
	.gop = 15,
	.bps = 2000000,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
	.qlevel = 5,
	.ainr_en = AINR_ENA,
#if VAPP_ROI_ENA
	/* Enable ROI0 with zero QP offset to enable HEVC PPS CU QP delta support.
	 * Inclusive (0,0)-(0,0) covers one CTB; VAPP roi updates or disables it. */
	.coding = {
		.flags = VPAP_ENC_CODING_ROI,
		.roi = {{.enable = 1, .top = 0, .left = 0, .bottom = 0, .right = 0, .deltaQp = 0, .qp = -1}},
	},
#endif
};

static video_params_t video_v2_params = {
	.stream_id = V2_STREAM_ID,
	.format = VIDEO_H264,
	.width = 1280,
	.height = 720,
	.fps = 30,
	.gop = 30,
	.bps = 500000,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
#if VAPP_ROI_ENA
	/* Initialize ROI before H.264 stream start for runtime updates.
	 * Inclusive (0,0)-(0,0) covers one MB with zero QP offset. */
	.coding = {
		.flags = VPAP_ENC_CODING_ROI,
		.roi = {{.enable = 1, .top = 0, .left = 0, .bottom = 0, .right = 0, .deltaQp = 0, .qp = -1}},
	},
#endif
};

static video_params_t video_v3_params = {
	.stream_id = V3_STREAM_ID,
	.format = VIDEO_JPEG,
	.width = 1920,
	.height = 1080,
	.fps = 1,
	.qlevel = 5,
};

static video_params_t video_v4_params = {
	.stream_id = V4_STREAM_ID,
	.format = VIDEO_NV12,
	.width = 640,
	.height = 480,
	.fps = 10,
};

static video_params_t video_v5_params = {
	.stream_id = V5_STREAM_ID,
	.format = VIDEO_RGB,
	.width = V5_RGB_WIDTH,
	.height = V5_RGB_HEIGHT,
	.fps = 10,
};

static mp4_params_t mp4_v1_params = {
	.sample_rate = 8000,
	.channel = 1,
	.record_length = 10, //seconds
	.record_type = STORAGE_VIDEO,
	.record_file_num = 1,
	.record_file_name = "AmebaPro_recording",
	.fatfs_buf_size = 224 * 1024, /* 32kb multiple */
	.mp4_audio_format = AUDIO_AAC,//AUDIO_OPUS,//AUDIO_ULAW
	.mp4_audio_duration = 40,//audio duration 40ms for PCM
};

#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
static rtsp2_params_t rtsp2_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps      = 30,
			.bps      = 0, // if unknown
		}
	}
};
#endif

#if NN_ENA
static nn_data_param_t nn_input_params = {
	.img = {
		/* input RGB size (including preprocess node) */
		.width = V5_RGB_WIDTH,
		.height = V5_RGB_HEIGHT,
		/* logical model input size after preprocess node */
		.model_width = NN_WIDTH,
		.model_height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static char *nn_model_file_name = NN_MODEL_NAME;

static int desired_class_list[] = {0}; //represent the objects to be detected, please refer to yolov9/data/coco.yaml for the category of class id
static int check_in_list(int class_indx)
{
	for (int i = 0; i < (sizeof(desired_class_list) / sizeof(int)); i++) {
		if (class_indx == desired_class_list[i]) {
			return class_indx;
		}
	}
	return -1;
}


static uint32_t nn_fps = 0;
static uint32_t nn_ts = 0;
static nn_osd_group_t osd_group;
static const nn_osd_layer_config_t osd_layer[] = {
	{
		.stream_id = V2_STREAM_ID,
		.codec = VIDEO_H264,
		.alpha = 128, /* OSD alpha */
		.y = 0,       /* green Y */
		.u = 0,       /* green U */
		.v = 0,       /* green V */
	},
};
static void nn_display_cb(void *p, void *img_param)
{
	int i = 0;
	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	objdetect_res_t *res = (objdetect_res_t *)&out->res[0];

	int obj_num = out->res_cnt;

	nn_data_param_t *im = (nn_data_param_t *)img_param;

	if (!p || !img_param) {
		return;
	}

	int im_w = video_v2_params.width;
	int im_h = video_v2_params.height;
#if NN_OSD_ENA
	/* One nn_osd layer owns one hardware OSD index. This example uses one
	 * green bitmap layer for all detection boxes and labels. */
	if (nn_osd_group_init(&osd_group, im_w, im_h, osd_layer, sizeof(osd_layer) / sizeof(osd_layer[0]), 0) != 0) {
		RTK_LOGE(TAG, "error: fail to init OSD group\r\n");
		return;
	}
	if (nn_osd_group_lock(&osd_group) != 0) {
		return;
	}
	nn_osd_group_clear(&osd_group);
#endif
	nn_coord_map_t map;
	nn_coord_map_init(&map, im_w, im_h, im, NN_COORD_MAP_SCALE_STRETCH);

	RTK_LOGD(TAG, "object num = %d\r\n", obj_num);
	if (obj_num > 0) {
		for (i = 0; i < obj_num; i++) {
			int obj_class = (int)res[i].result[0];
			int class_id = obj_class; //check_in_list(obj_class); //show class in desired_class_list
			if (class_id != -1) {
				int xmin, ymin, xmax, ymax;
				nn_coord_map_bbox(&map, res[i].result[2], res[i].result[3],
								  res[i].result[4], res[i].result[5],
								  &xmin, &ymin, &xmax, &ymax);
				RTK_LOGD(TAG, "%d,c%d:%d %d %d %d\n\r", i, class_id, xmin, ymin, xmax, ymax);
#if NN_OSD_ENA
				nn_osd_canvas_t *canvas = nn_osd_group_canvas(&osd_group, NN_OSD_LAYER);
				if (canvas) {
					nn_osd_draw_rect(canvas, xmin, ymin, xmax, ymax, 10);
					nn_osd_draw_stringf(canvas, xmin, ymin - 24, 3 /*scale*/, "%s %d", coco_name_get_by_id(class_id), (int)(res[i].result[1] * 100));
				}
#endif
			}
		}
	}
#if NN_OSD_ENA
	nn_osd_group_flush(&osd_group);
	nn_osd_group_unlock(&osd_group);
#endif
	nn_fps++;
	if ((rtos_time_get_current_system_time_ms() - nn_ts) > 1000) {
		RTK_LOGD(TAG, "NN fps %d\n\r", nn_fps);
		nn_fps = 0;
		nn_ts = rtos_time_get_current_system_time_ms();
	}
}
#endif

static void saver_handler(char *file_name, uint32_t data_addr, uint32_t data_size)
{
	(void)file_name;
	//RTK_LOGI(TAG, ">>saver_handler[addr:0x%08x][size:%ld]\r\n", data_addr, data_size);
}

#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
/* Poll until WiFi STA is connected. Prints a hint on first check if not yet connected. */
static void prv_wifi_ensure_connected(void)
{
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;
	wifi_get_join_status(&join_status);
	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		return;
	}

	RTK_LOGI(TAG, "WiFi not connected. To connect, run:\r\n");
	RTK_LOGI(TAG, "  AT+WLCONN=ssid,<your_ssid>,pw,<your_password>\r\n");
	RTK_LOGI(TAG, "Waiting for WiFi connection...\r\n");
	do {
		rtos_time_delay_ms(50);
		wifi_get_join_status(&join_status);
	} while (join_status != RTW_JOINSTATUS_SUCCESS);

	unsigned char *ip;
	do {
		rtos_time_delay_ms(50);
		ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
	} while (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0);

	RTK_LOGI(TAG, "WiFi connected. IP: %d.%d.%d.%d\r\n", ip[0], ip[1], ip[2], ip[3]);
}

/* Called from the RTSP service task on every RTSP PLAY request. Forcing an I frame
 * here removes the up-to-one-GOP wait before the client gets a reference frame.
 * The rtsp2 module drops frames until that I frame actually arrives, so the first
 * frame the client receives is always an I frame. */
static int prv_rtsp_play_cb(void *p)
{
	(void)p;
	RTK_LOGI(TAG, "RTSP play: force I frame on ch%d\r\n", V2_STREAM_ID);
	mm_module_ctrl(video_v2_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	return 0;
}
#endif

/* Clamp video stream params to what the attached sensor can actually deliver.
 * Sensor dim/fps are queried once and shared by all streams, since every
 * stream uses the same sensor. GOP is an encoder property (P-frame interval),
 * not a sensor capability, so it is not clamped here. */
static void fix_params(video_params_t *video_params,
					   uint32_t sensor_w, uint32_t sensor_h, uint32_t sensor_fps)
{
	if (video_params->width > sensor_w) {
		video_params->width = sensor_w;
	}
	if (video_params->height > sensor_h) {
		video_params->height = sensor_h;
	}
	if (video_params->fps > sensor_fps) {
		video_params->fps = sensor_fps;
	}
	RTK_LOGD(TAG, "[%s][L:%d] w:%d  h:%d  fps:%d  gop:%d (sensor %dx%d@%d)\r\n", __func__, __LINE__,
			 video_params->width, video_params->height, video_params->fps, video_params->gop,
			 sensor_w, sensor_h, sensor_fps);
}

/* Validates the sensor capability reported over IPC before clamping. On IPC
 * failure the streams are left at their configured values (no clamping), which
 * mirrors the pre-clamp behavior instead of silently truncating. */
static int video_fix_stream_params(video_params_t *params[], int count)
{
	uint32_t sensor_w = 0, sensor_h = 0, sensor_fps = 0;
	struct sensor_info_t sensor_info;
	int i;

	video_get_sensor_info((uint32_t *)&sensor_info);
	sensor_fps = sensor_info.fps;
	sensor_w   = sensor_info.width;
	sensor_h   = sensor_info.height;
	if (sensor_w == 0 || sensor_w > 8192 || sensor_h == 0 || sensor_h > 8192) {
		RTK_LOGE(TAG, "video params check fail sensor_w:%d sensor_h:%d\r\n", sensor_w, sensor_h);
		return -1;
	}
	if (sensor_fps == 0 || sensor_fps > 120) {
		RTK_LOGE(TAG, "video params check fail sensor_fps:%d\r\n", sensor_fps);
		return -1;
	}

	for (i = 0; i < count; i++) {
		fix_params(params[i], sensor_w, sensor_h, sensor_fps);
	}
	return 0;
}

static void mmf2_video_example_joint_test_init(int sink_mode)
{
	// DBG_MMF_INFO_MSG_OFF(0xffffffff);
	video_params_t *video_params_list[] = {
		&video_v1_params, &video_v2_params, &video_v3_params,
		&video_v4_params, &video_v5_params,
	};
	if (video_fix_stream_params(video_params_list, 5)) {
		RTK_LOGW(TAG, "sensor query failed, streams keep configured params\r\n");
	}
	//video_set_sensor_iq(SENSOR_OS04A10, 0);

	v2_sink_mode = sink_mode;
#if V2_ENA
	if (v2_sink_mode == V2_SINK_RTSP) {
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
		prv_wifi_ensure_connected();
#endif
	}
#endif

#if V1_ENA
	/* V1 ********************************************************/
	/* Each video stream follows the same pattern: open video_module, set
	 * video_params_t, prepare the queue, then CMD_VIDEO_APPLY starts it. */
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
#if MP4_ENA
	mp4_v1_params.fps = video_v1_params.fps;
	mp4_v1_params.gop = video_v1_params.gop;
	mp4_v1_params.width = video_v1_params.width;
	mp4_v1_params.height = video_v1_params.height;
	/* MP4 */
	mp4_ctx = mm_module_open(&mp4_module);
	if (mp4_ctx) {
		mm_module_ctrl(mp4_ctx, CMD_MP4_SET_PARAMS, (int)&mp4_v1_params);
		mm_module_ctrl(mp4_ctx, CMD_MP4_LOOP_MODE, 0);
		mm_module_ctrl(mp4_ctx, CMD_MP4_START, mp4_v1_params.record_file_num);
	} else {
		RTK_LOGE(TAG, "mp4 open fail\n\r");
		goto error;
	}
#else
	fs_v1_ctx = mm_module_open(&filesaver_module);
	if (fs_v1_ctx) {
		mm_module_ctrl(fs_v1_ctx, CMD_FILESAVER_SET_TYPE_HANDLER, (int)saver_handler);
		mm_module_ctrl(fs_v1_ctx, CMD_FILESAVER_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "fs_v1_ctx open fail\n\r");
		goto error;
	}
#endif
	/* Link V1 to its selected recorder. SISO has one input and one output in
	 * this path: video -> MP4, or video -> filesaver. */
	siso_v1 = siso_create();
	if (siso_v1) {
		siso_ctrl(siso_v1, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
#if MP4_ENA
		siso_ctrl(siso_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)mp4_ctx, 0);
#else
		siso_ctrl(siso_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)fs_v1_ctx, 0);
#endif
		siso_start(siso_v1);
	} else {
		RTK_LOGE(TAG, "siso_v1 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v1 started\n\r");
#endif

#if V2_ENA
	/* V2 ********************************************************/
	/* V2 is the live stream shown to the user. Its sink is selected by the
	 * VIDEO app table entry before this common init path opens modules. */
	video_v2_ctx = mm_module_open(&video_module);
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v2_params);
		mm_module_ctrl(video_v2_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "V2 sink mode: %s\n\r", v2_sink_mode == V2_SINK_UVC ? "uvc" : "rtsp");
	if (v2_sink_mode == V2_SINK_UVC) {
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
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_params);
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
	/* Link V2 to the sink selected above. Only one output module is attached
	 * for each run. */
	siso_v2 = siso_create();
	if (siso_v2) {
		siso_ctrl(siso_v2, MMIC_CMD_ADD_INPUT, (uint32_t)video_v2_ctx, 0);
		if (v2_sink_mode == V2_SINK_UVC) {
			siso_ctrl(siso_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		} else {
			siso_ctrl(siso_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		}
		siso_start(siso_v2);
	} else {
		RTK_LOGE(TAG, "siso_v2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v2 started\n\r");
#endif

#if V3_ENA
	/* V3 ********************************************************/
	/* V3 is a low-FPS JPEG snapshot stream saved by filesaver. */
	video_v3_ctx = mm_module_open(&video_module);
	if (video_v3_ctx) {
		mm_module_ctrl(video_v3_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v3_params);
		mm_module_ctrl(video_v3_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v3_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v3 open fail\n\r");
		goto error;
	}
	fs_v3_ctx = mm_module_open(&filesaver_module);
	if (fs_v3_ctx) {
		mm_module_ctrl(fs_v3_ctx, CMD_FILESAVER_SET_TYPE_HANDLER, (int)saver_handler);
		mm_module_ctrl(fs_v3_ctx, CMD_FILESAVER_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "fs_v3_ctx open fail\n\r");
		goto error;
	}
	siso_v3 = siso_create();
	if (siso_v3) {
		siso_ctrl(siso_v3, MMIC_CMD_ADD_INPUT, (uint32_t)video_v3_ctx, 0);
		siso_ctrl(siso_v3, MMIC_CMD_ADD_OUTPUT, (uint32_t)fs_v3_ctx, 0);
		siso_start(siso_v3);
	} else {
		RTK_LOGE(TAG, "siso_v3 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v3 started\n\r");
#endif

#if V4_ENA
	/* V4 ********************************************************/
	/* V4 outputs raw NV12 frames for motion/debug style consumers. */
	video_v4_ctx = mm_module_open(&video_module);
	if (video_v4_ctx) {
		mm_module_ctrl(video_v4_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v4_params);
		mm_module_ctrl(video_v4_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v4_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v4_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v4 open fail\n\r");
		goto error;
	}
	fs_v4_ctx = mm_module_open(&filesaver_module);
	if (fs_v4_ctx) {
		mm_module_ctrl(fs_v4_ctx, CMD_FILESAVER_SET_TYPE_HANDLER, (int)saver_handler);
		mm_module_ctrl(fs_v4_ctx, CMD_FILESAVER_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "fs_v4_ctx open fail\n\r");
		goto error;
	}
	siso_v4 = siso_create();
	if (siso_v4) {
		siso_ctrl(siso_v4, MMIC_CMD_ADD_INPUT, (uint32_t)video_v4_ctx, 0);
		siso_ctrl(siso_v4, MMIC_CMD_ADD_OUTPUT, (uint32_t)fs_v4_ctx, 0);
		siso_start(siso_v4);
	} else {
		RTK_LOGE(TAG, "siso_v4 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v4 started\n\r");
#endif

#if V5_ENA
	/* V5 ********************************************************/
	/* V5 is an RGB stream. It feeds VIPNN when NN_ENA is set, otherwise it can
	 * be saved to inspect the exact RGB frames. */
	video_v5_ctx = mm_module_open(&video_module);
	if (video_v5_ctx) {
		mm_module_ctrl(video_v5_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v5_params);
		mm_module_ctrl(video_v5_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v5_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(video_v5_ctx, CMD_VIDEO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "video v5 open fail\n\r");
		goto error;
	}
#if NN_ENA
	vipnn_ctx = mm_module_open(&vipnn_module);
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL, (int)&NN_MODEL_OBJ);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_MODEL_FILE_NAME, (int)nn_model_file_name);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_IN_PARAMS, (int)&nn_input_params);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_DISPPOST, (int)nn_display_cb);
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_SIZE, sizeof(objdetect_res_t));		// result size
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_RES_MAX_CNT, 32);		// result max count
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "VIPNN open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "VIPNN opened\n\r");
#else
	fs_v5_ctx = mm_module_open(&filesaver_module);
	if (fs_v5_ctx) {
		mm_module_ctrl(fs_v5_ctx, CMD_FILESAVER_SET_TYPE_HANDLER, (int)saver_handler);
		mm_module_ctrl(fs_v5_ctx, CMD_FILESAVER_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "fs_v5_ctx open fail\n\r");
		goto error;
	}
#endif
	/* Link V5 either to VIPNN or filesaver depending on NN_ENA. */
	siso_v5 = siso_create();
	if (siso_v5) {
		siso_ctrl(siso_v5, MMIC_CMD_ADD_INPUT, (uint32_t)video_v5_ctx, 0);
#if NN_ENA
		siso_ctrl(siso_v5, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
#else
		siso_ctrl(siso_v5, MMIC_CMD_ADD_OUTPUT, (uint32_t)fs_v5_ctx, 0);
#endif
		siso_start(siso_v5);
	} else {
		RTK_LOGE(TAG, "siso_v5 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v5 started\n\r");
#endif

	return;
error:
	return;
}

void mmf2_video_example_joint_test_rtsp_init(void)
{
	mmf2_video_example_joint_test_init(V2_SINK_RTSP);
}

void mmf2_video_example_joint_test_uvc_init(void)
{
	mmf2_video_example_joint_test_init(V2_SINK_UVC);
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_joint_test_init[] = {
	{
		.v_func = mmf2_video_example_joint_test_uvc_init,
		.v_func_name = "mmf2_video_example_joint_test_uvc_init",
	},
	{
		.v_func = mmf2_video_example_joint_test_rtsp_init,
		.v_func_name = "mmf2_video_example_joint_test_rtsp_init",
	}
};

static void mmf2_video_example_joint_test_deinit(void)
{
	/* Step 1: send stream stop to each video channel */
#if V1_ENA
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_v1) {
		siso_stop(siso_v1);
		siso_delete(siso_v1);
		siso_v1 = NULL;
	}
#if MP4_ENA
	if (mp4_ctx) {
		mm_module_ctrl(mp4_ctx, CMD_MP4_STOP_IMMEDIATELY, 0);
		mm_module_close(mp4_ctx);
		mp4_ctx = NULL;
	}
#else
	if (fs_v1_ctx) {
		mm_module_close(fs_v1_ctx);
		fs_v1_ctx = NULL;
	}
#endif
	if (video_v1_ctx) {
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
	}
#endif

#if V2_ENA
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_v2) {
		siso_stop(siso_v2);
		siso_delete(siso_v2);
		siso_v2 = NULL;
	}
	if (v2_sink_mode == V2_SINK_UVC) {
		if (uvcd_ctx) {
			mm_module_ctrl(uvcd_ctx, CMD_UVCD_STOP, 0);
			mm_module_close(uvcd_ctx);
			uvcd_ctx = NULL;
		}
	} else {
		if (rtsp_ctx) {
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
			mm_module_close(rtsp_ctx);
			rtsp_ctx = NULL;
		}
	}
	if (video_v2_ctx) {
		mm_module_close(video_v2_ctx);
		video_v2_ctx = NULL;
	}
#endif

#if V3_ENA
	if (video_v3_ctx) {
		mm_module_ctrl(video_v3_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_v3) {
		siso_stop(siso_v3);
		siso_delete(siso_v3);
		siso_v3 = NULL;
	}
	if (fs_v3_ctx) {
		mm_module_close(fs_v3_ctx);
		fs_v3_ctx = NULL;
	}
	if (video_v3_ctx) {
		mm_module_close(video_v3_ctx);
		video_v3_ctx = NULL;
	}
#endif

#if V4_ENA
	if (video_v4_ctx) {
		mm_module_ctrl(video_v4_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_v4) {
		siso_stop(siso_v4);
		siso_delete(siso_v4);
		siso_v4 = NULL;
	}
	if (fs_v4_ctx) {
		mm_module_close(fs_v4_ctx);
		fs_v4_ctx = NULL;
	}
	if (video_v4_ctx) {
		mm_module_close(video_v4_ctx);
		video_v4_ctx = NULL;
	}
#endif
#if V5_ENA
	if (video_v5_ctx) {
		mm_module_ctrl(video_v5_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_v5) {
		siso_stop(siso_v5);
		siso_delete(siso_v5);
		siso_v5 = NULL;
	}
#if NN_ENA
	if (vipnn_ctx) {
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_STATUS, (int)VIPNN_PAUSED);
		mm_module_close(vipnn_ctx);
		vipnn_ctx = NULL;
	}
#else
	if (fs_v5_ctx) {
		mm_module_close(fs_v5_ctx);
		fs_v5_ctx = NULL;
	}
#endif
	if (video_v5_ctx) {
		mm_module_close(video_v5_ctx);
		video_v5_ctx = NULL;
	}
#if NN_ENA && NN_OSD_ENA
	if (nn_osd_group_lock(&osd_group) == 0) {
		nn_osd_group_clear_locked(&osd_group);
		nn_osd_group_unlock(&osd_group);
	}
#endif
#endif

	RTK_LOGI(TAG, "joint test deinit done\n\r");
}

static int vapp_video_ctrl(int ch, int cmd, int arg1, int arg2)
{
	mm_context_t *video_ctx;
	switch (ch) {
	case V1_STREAM_ID:
		video_ctx = video_v1_ctx;
		break;
	case V2_STREAM_ID:
		video_ctx = video_v2_ctx;
		break;
	case V3_STREAM_ID:
		video_ctx = video_v3_ctx;
		break;
	case V4_STREAM_ID:
		video_ctx = video_v4_ctx;
		break;
	case V5_STREAM_ID:
		video_ctx = video_v5_ctx;
		break;
	default:
		video_ctx = NULL;
		break;
	}

	if (!video_ctx) {
		RTK_LOGE(TAG, "video ctx for ch%d is NULL\r\n", ch);
		return -1;
	}

	if (cmd == CMD_VIDEO_SET_QP) {
		video_qp_range_t qp = {
			.min_qp = arg1,
			.max_qp = arg2,
		};
		return mm_module_ctrl(video_ctx, CMD_VIDEO_SET_QP, (int)&qp);
	}

	return mm_module_ctrl(video_ctx, cmd, arg1);
}

static u32 vapp_roi_cmd(u16 argc, u8 *argv[])
{
#if !VAPP_ROI_ENA
	(void)argc;
	(void)argv;
	RTK_LOGE(TAG, "runtime ROI requires VAPP_ROI_ENA=1 before encoder start\r\n");
	return _FALSE;
#else
	long values[9] = {0};
	video_params_t params;
	video_roi_t roi;
	uint32_t block_size, rows, cols;
	int ch;

	if (argc != 4 && argc != 10) {
		goto usage;
	}
	values[8] = -1;
	for (int i = 1; i < argc; i++) {
		char *end;
		const char *arg = (const char *)argv[i];
		values[i - 1] = strtol(arg, &end, 10);
		if (end == arg || *end != '\0') {
			RTK_LOGE(TAG, "ROI argument %d must be a decimal integer\r\n", i);
			return _FALSE;
		}
	}
	/* Validate command ranges before narrowing; these also reject strtol overflow. */
	if (values[0] < V1_STREAM_ID || values[0] > V5_STREAM_ID ||
		values[1] < 0 || values[1] >= VAPP_ROI_COUNT || values[2] < 0 || values[2] > 1 || (values[2] && argc != 10) ||
		values[7] < -51 || values[7] > 51 || values[8] < -1 || values[8] > 51) {
		goto usage;
	}
	ch = (int)values[0];
	if (vapp_video_ctrl(ch, CMD_VIDEO_GET_PARAMS, (int)&params, 0) != 0) {
		return _FALSE;
	}
	/* format is a bitmask; also accept encoded+JPEG formats. */
	if (!(params.format & (VIDEO_H264 | VIDEO_HEVC))) {
		RTK_LOGE(TAG, "ch%d ROI requires H.264/HEVC, not JPEG/raw video\r\n", ch);
		return _FALSE;
	}
	/* The driver takes inclusive MB/CTB indices, not pixel coordinates. */
	block_size = (params.format & VIDEO_H264) ? VAPP_H264_MB_SIZE : VAPP_HEVC_CTB_SIZE;
	cols = (params.width + block_size - 1) / block_size;
	rows = (params.height + block_size - 1) / block_size;
	if (values[3] < 0 || values[4] < 0 || values[3] > values[5] || values[4] > values[6] || values[5] >= rows || values[6] >= cols) {
		RTK_LOGE(TAG, "ch%d ROI: top/bottom 0..%u, left/right 0..%u (inclusive, %ux%u pixels/block)\r\n",
				 ch, rows - 1, cols - 1, block_size, block_size);
		return _FALSE;
	}
	roi.idx = (int)values[1];
	roi.enable = (uint32_t)values[2];
	roi.top = (uint32_t)values[3];
	roi.left = (uint32_t)values[4];
	roi.bottom = (uint32_t)values[5];
	roi.right = (uint32_t)values[6];
	/* video_roi_t uses snake_case; video_set_roi() maps this to the shared deltaQp field. */
	roi.delta_qp = (int32_t)values[7];
	roi.qp = (int32_t)values[8];
	/* qp >= 0 selects absolute QP; qp == -1 selects delta_qp. The driver may
	 * clamp absolute QP to the channel QP range (adjust it with VAPP qp). */
	if (vapp_video_ctrl(ch, CMD_VIDEO_ROI, (int)&roi, 0) != 0) {
		return _FALSE;
	}
	/* video_set_roi() returns void: this log confirms submission, not HW success. */
	RTK_LOGI(TAG, "AP ROI request ch%d idx%d enable%u top%u left%u bottom%u right%u delta_qp%d qp%d\r\n",
			 ch, roi.idx, roi.enable, roi.top, roi.left, roi.bottom, roi.right, roi.delta_qp, roi.qp);
	return _TRUE;

usage:
	RTK_LOGI(TAG, "VAPP roi <ch> <idx> <enable> <top> <left> <bottom> <right> <delta_qp> <qp>\r\n");
	RTK_LOGI(TAG, "VAPP roi <ch> <idx> 0: disable; idx 0..%d, enable 0/1, delta_qp -51..51, qp -1..51\r\n", VAPP_ROI_COUNT - 1);
	RTK_LOGI(TAG, "Coordinates: inclusive blocks (H.264 16x16, HEVC 64x64); qp=-1 uses delta_qp\r\n");
	RTK_LOGI(TAG, "Start joint test first: CH0=V1 HEVC, CH1=V2 H.264; check VP logs for encoder errors\r\n");
	// VAPP roi 1 0 1 10 20 29 59 0 28   (CH1 center, absolute QP 28)
	// VAPP roi 1 0 1 10 20 29 59 0 46   (same ROI, absolute QP 46)
	// VAPP roi 1 0 1 10 20 29 59 -8 -1  (same ROI, delta QP -8)
	// VAPP roi 1 1 1 0 0 9 19 0 40     (CH1 second ROI, top-left)
	// VAPP roi 0 0 1 6 10 17 31 0 28   (CH0 HEVC center, absolute QP 28)
	// VAPP roi 1 0 0                    (disable CH1 ROI0 only)
	return _FALSE;
#endif
}

u32 CmdApVideoAppTest(IN u16 argc, IN u8 *argv[])
{
	if (argc == 0) {
		return vapp_roi_cmd(argc, argv);
	}
	if (_strcmp((const char *)argv[0], "bps") == 0) {
		int ch = atoi(argv[1]);
		int bps = atoi(argv[2]);
		RTK_LOGI(TAG, "AP set ch%d bps to %d\r\n", ch, bps);
		if (vapp_video_ctrl(ch, CMD_VIDEO_BITRATE, bps, 0) != 0) {
			return _FALSE;
		}
		// VAPP bps 1 4000000
		// VAPP bps 1 2000000
		// VAPP bps 1 256000
	} else if (_strcmp((const char *)argv[0], "rcmode") == 0) {
		int ch = atoi(argv[1]);
		int mode = atoi(argv[2]);
		// VAPP rcmode 1 0   (0=CVBR 1=CBR 2=VBR 3=ABR 4=CRF 5=CQP)
		RTK_LOGI(TAG, "AP set ch%d rc mode to %d\r\n", ch, mode);
		if (vapp_video_ctrl(ch, CMD_VIDEO_RCMODE, mode, 0) != 0) {
			return _FALSE;
		}
	} else if (_strcmp((const char *)argv[0], "qp") == 0) {
		int ch = atoi(argv[1]);
		int minQp = atoi(argv[2]);
		int maxQp = atoi(argv[3]);
		// VAPP qp 1 26 28
		// VAPP qp 1 45 48
		// VAPP qp 1 1 51
		RTK_LOGI(TAG, "AP set ch%d qp to [%d,%d]\r\n", ch, minQp, maxQp);
		if (vapp_video_ctrl(ch, CMD_VIDEO_SET_QP, minQp, maxQp) != 0) {
			return _FALSE;
		}
	} else if (_strcmp((const char *)argv[0], "roi") == 0) {
		return vapp_roi_cmd(argc, argv);
	} else if (_strcmp((const char *)argv[0], "sei") == 0) {
		int ch = atoi(argv[1]);
		int enable = atoi(argv[2]);
		RTK_LOGI(TAG, "AP set ch%d SEI user data %s\r\n", ch, enable ? "ON" : "OFF");
		if (vapp_video_ctrl(ch, CMD_VIDEO_SEI_USER_DATA, enable, 0) != 0) {
			return _FALSE;
		}
		// VAPP sei 1 1
		// VAPP sei 1 0
	} else if (_strcmp((const char *)argv[0], "gop") == 0) {
		int ch = atoi(argv[1]);
		int gop = atoi(argv[2]);
		// VAPP gop 1 60
		RTK_LOGI(TAG, "AP set ch%d gop to %d\r\n", ch, gop);
		if (vapp_video_ctrl(ch, CMD_VIDEO_GOP, gop, 0) != 0) {
			return _FALSE;
		}
	} else if (_strcmp((const char *)argv[0], "fps") == 0) {
		int ch = atoi(argv[1]);
		int fps = atoi(argv[2]);
		// VAPP fps 0 15
		// VAPP fps 1 15
		RTK_LOGI(TAG, "AP set ch%d fps to %d\r\n", ch, fps);
		if (vapp_video_ctrl(ch, CMD_VIDEO_FPS, fps, 0) != 0) {
			return _FALSE;
		}
	} else if (_strcmp((const char *)argv[0], "i") == 0) {
		int ch = atoi(argv[1]);
		RTK_LOGI(TAG, "AP force ch%d I frame\r\n", ch);
		if (vapp_video_ctrl(ch, CMD_VIDEO_FORCE_IFRAME, 0, 0) != 0) {
			return _FALSE;
		}
		//VAPP i 1
	} else if (_strcmp((const char *)argv[0], "ainr") == 0) {
		int ainr_en = atoi(argv[1]);
		RTK_LOGI(TAG, "AP set ainr %s\r\n", ainr_en ? "ON" : "OFF");
		if (vapp_video_ctrl(V1_STREAM_ID, CMD_VIDEO_AINR, ainr_en, 0) != 0) {
			return _FALSE;
		}
		//VAPP ainr 1
		//VAPP ainr 0
	} else if (_strcmp((const char *)argv[0], "nn") == 0) {
		if (!vipnn_ctx) {
			RTK_LOGE(TAG, "vipnn_ctx not opened yet\r\n");
			return _FALSE;
		}
		vipnn_status_t nn_cur_status;
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_GET_STATUS, (int)&nn_cur_status);
		int nn_run = atoi(argv[1]);
		RTK_LOGI(TAG, "AP set nn %s\r\n", nn_run ? "run" : "paused");
		vipnn_status_t nn_next_status = nn_run ? VIPNN_APPLIED : VIPNN_PAUSED;
		if (nn_next_status == VIPNN_PAUSED && nn_cur_status != VIPNN_APPLIED) {
			RTK_LOGE(TAG, "nn model not applied yet\r\n");
			return _FALSE;
		}
		mm_module_ctrl(vipnn_ctx, CMD_VIPNN_SET_STATUS, (int)nn_next_status);
#if NN_ENA && NN_OSD_ENA
		vTaskDelay(200);
		if (nn_osd_group_lock(&osd_group) == 0) {
			nn_osd_group_clear_locked(&osd_group);
			nn_osd_group_unlock(&osd_group);
		}
#endif
		//VAPP nn 1
		//VAPP nn 0
	} else if (_strcmp((const char *)argv[0], "stop") == 0) {
		RTK_LOGI(TAG, "AP stop joint test example\r\n");
		mmf2_video_example_joint_test_deinit();
		//VAPP stop
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   video_param_change_test_cmd[] = {
	{
		"VAPP", CmdApVideoAppTest
	},
};
