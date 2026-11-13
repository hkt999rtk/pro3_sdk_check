/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "ameba_soc.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "mmf2_mimo.h"
#include "module_video.h"
#include "module_rtsp2.h"
#include "module_audio.h"
#include "module_aac.h"
#include "module_mp4.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 HEVC video + AAC audio -> MP4 recording
 *   V2 H.264 video + AAC audio -> RTSP live stream
 *   V5 RGB video -> VIPNN object detection
 *
 * This example shows AV recording, AV streaming, and NN running in parallel.
 * The NN callback only logs results; use the dedicated NN examples for OSD.
 */

/* AI Denoise Option */
#define AINR_ENA     1  /* AI noise reduction, will take effect on each video channel */

/* Audio Option */
#define AUDIO_ENA    1  /* Enable audio for recording and live streaming */

/* NN Option */
#define NN_ENA       1  /* AI NN model detection on V5 RGB channel */


#if NN_ENA
#include "model_yolo.h"
/* model selection: yolov4_tiny, yolov7_tiny, nanodet_plus_m */
#define NN_MODEL_OBJ    yolov4_tiny
/* please make sure the choosed model is also in the file system */
#define NN_MODEL_NAME   "vfs:/yolov4_tiny_asymu8.nb"  //"sd:yolov4_tiny_asymu8.nb"
/* RGB video resolution
 * please make sure the resolution is matched to model input size and thereby to avoid SW image resizing */
#define NN_WIDTH	416
#define NN_HEIGHT	416
#endif

static mm_context_t *video_v1_ctx       = NULL;
static mm_context_t *video_v2_ctx       = NULL;
static mm_context_t *video_v5_ctx       = NULL;
static mm_context_t *audio_ctx          = NULL;
static mm_context_t *aac_ctx            = NULL;
static mm_context_t *mp4_ctx            = NULL;
static mm_context_t *rtsp_ctx           = NULL;
static mm_context_t *vipnn_ctx          = NULL;
static mm_siso_t *siso_audio_aac        = NULL;
static mm_siso_t *siso_rgb_nn           = NULL;
static mm_mimo_t *mimo_mp4_rtsp         = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_HEVC,
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
};

static aac_params_t aac_params = {
	.sample_rate = 16000,
	.channel = 1,
	.trans_type = AAC_TYPE_ADTS, //AAC_TYPE_RAW, AAC_TYPE_ADTS
	.object_type = AAC_AOT_LC,
	.bitrate = 32000,

	.mem_total_size = 10 * 1024,
	.mem_block_size = 128,
	.mem_frame_size = 1024
};

static mp4_params_t mp4_params = {
	.sample_rate = 16000,
	.channel = 1,
	.record_length = 10, //seconds
	.record_type = AUDIO_ENA ? STORAGE_ALL : STORAGE_VIDEO,
	.record_file_num = 3,
	.record_file_name = "AmebaPro_recording",
	.fatfs_buf_size = 224 * 1024, /* 32kb multiple */
	.mp4_audio_format = AUDIO_AAC,//AUDIO_OPUS,//AUDIO_ULAW
	.mp4_audio_duration = 20,//audio duration for PCM, 8KHz/40ms 16KHz/20ms
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps      = 30,
			.bps      = 0, // if unknown
		}
	}
};

static rtsp2_params_t rtsp2_a_params = {
	.type = AVMEDIA_TYPE_AUDIO,
	.u = {
		.a = {
			.codec_id   = AV_CODEC_ID_MP4A_LATM,
			.channel    = 1,
			.samplerate = 16000
		}
	}
};

#if NN_ENA
static video_params_t video_v5_params = {
	.stream_id = V5_STREAM_ID,
	.format = VIDEO_RGB,
	.width = NN_WIDTH,
	.height = NN_HEIGHT,
	.fps = 10,
};

static nn_data_param_t nn_input_params = {
	.img = {
		.width = NN_WIDTH,
		.height = NN_HEIGHT,
	},
	.codec_type = AV_CODEC_ID_RGB888
};

static char *nn_model_file_name = NN_MODEL_NAME;

#define LIMIT(x, lower, upper) if(x<lower) x=lower; else if(x>upper) x=upper;
static uint32_t nn_fps = 0;
static uint32_t nn_ts = 0;
static void nn_display_cb(void *p, void *img_param)
{
	int i = 0;
	vipnn_out_buf_t *out = (vipnn_out_buf_t *)p;
	objdetect_res_t *res = (objdetect_res_t *)&out->res[0];

	int obj_num = out->res_cnt;
	RTK_LOGD(TAG, "object num = %d\r\n", obj_num);

	nn_fps++;
	if ((rtos_time_get_current_system_time_ms() - nn_ts) > 1000) {
		RTK_LOGD(TAG, "NN fps %d\n\r", nn_fps);
		nn_fps = 0;
		nn_ts = rtos_time_get_current_system_time_ms();
	}
}
#endif

/* Called from the RTSP service task on every RTSP PLAY request. Forcing an I frame
 * here removes the up-to-one-GOP wait before the client gets a reference frame.
 * The rtsp2 module drops frames until that I frame actually arrives, so the first
 * frame the client receives is always an I frame. V2 is the stream fed to RTSP. */
static int prv_rtsp_play_cb(void *p)
{
	(void)p;
	RTK_LOGI(TAG, "RTSP play: force I frame on ch%d\r\n", V2_STREAM_ID);
	mm_module_ctrl(video_v2_ctx, CMD_VIDEO_FORCE_IFRAME, 0);
	return 0;
}

void mmf2_video_example_av_rtsp_mp4_nn_init(void)
{

#if NN_ENA
	/* V5 ********************************************************/
	/* V5 is an RGB-only stream sized for the NN model. It is not sent to
	 * RTSP/MP4; it exists only to feed VIPNN. */
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

	/* NN ********************************************************/
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

	/* SISO NN Linker ********************************************/
	/* This SISO starts the NN path: RGB frames from V5 are delivered to
	 * vipnn_module, then nn_display_cb() receives post-process results. */
	siso_rgb_nn = siso_create();
	if (siso_rgb_nn) {
		siso_ctrl(siso_rgb_nn, MMIC_CMD_ADD_INPUT, (uint32_t)video_v5_ctx, 0);
		siso_ctrl(siso_rgb_nn, MMIC_CMD_ADD_OUTPUT, (uint32_t)vipnn_ctx, 0);
		siso_start(siso_rgb_nn);
	} else {
		RTK_LOGE(TAG, "siso_rgb_nn open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_rgb_nn started\n\r");
#endif

	/* V1 ********************************************************/
	/* V1 is the high-resolution encoded stream used by the MP4 recorder. */
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

	/* V2 ********************************************************/
	/* V2 is the lower-bitrate encoded stream used by the RTSP live view. */
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

#if AUDIO_ENA
	/* Audio *****************************************************/
	audio_ctx = mm_module_open(&audio_module);
	if (audio_ctx) {
		audio_params_t audio_params;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_PARAMS, (int)&audio_params);
		audio_params.sample_rate = ASR_16KHZ;
		audio_params.use_mic_type = USE_AUDIO_AMIC;
		audio_params.mic_gain = MIC_20DB;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_SET_PARAMS, (int)&audio_params);
		mm_module_ctrl(audio_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(audio_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(audio_ctx, CMD_AUDIO_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "audio open fail\n\r");
		goto error;
	}

	/* AAC *******************************************************/
	aac_ctx = mm_module_open(&aac_module);
	if (aac_ctx) {
		mm_module_ctrl(aac_ctx, CMD_AAC_SET_PARAMS, (int)&aac_params);
		mm_module_ctrl(aac_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(aac_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		mm_module_ctrl(aac_ctx, CMD_AAC_INIT_MEM_POOL, 0);
		mm_module_ctrl(aac_ctx, CMD_AAC_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "AAC open fail\n\r");
		goto error;
	}

	/* SISO Audio Linker *****************************************/
	/* Audio is encoded by AAC first. The MIMO below combines AAC with the
	 * selected video stream for MP4 and RTSP outputs. */
	siso_audio_aac = siso_create();
	if (siso_audio_aac) {
		siso_ctrl(siso_audio_aac, MMIC_CMD_ADD_INPUT, (uint32_t)audio_ctx, 0);
		siso_ctrl(siso_audio_aac, MMIC_CMD_ADD_OUTPUT, (uint32_t)aac_ctx, 0);
		siso_ctrl(siso_audio_aac, MMIC_CMD_SET_STACKSIZE, 44 * 1024, 0);
		siso_start(siso_audio_aac);
	} else {
		RTK_LOGE(TAG, "siso1 open fail\n\r");
		goto error;
	}
#endif

	/* MP4 *******************************************************/
	mp4_params.fps = video_v1_params.fps;
	mp4_params.gop = video_v1_params.gop;
	mp4_params.width = video_v1_params.width;
	mp4_params.height = video_v1_params.height;
	mp4_ctx = mm_module_open(&mp4_module);
	if (mp4_ctx) {
		mm_module_ctrl(mp4_ctx, CMD_MP4_SET_PARAMS, (int)&mp4_params);
		mm_module_ctrl(mp4_ctx, CMD_MP4_LOOP_MODE, 0);
		mm_module_ctrl(mp4_ctx, CMD_MP4_START, mp4_params.record_file_num);
	} else {
		RTK_LOGE(TAG, "mp4 open fail\n\r");
		goto error;
	}

	/* RTSP ******************************************************/
	rtsp_ctx = mm_module_open(&rtsp2_module);
	if (rtsp_ctx) {
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 0);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v_params);
		/* Force an I frame when a client sends RTSP PLAY so the stream starts from
		 * a reference frame instead of waiting for the next GOP. */
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);
#if AUDIO_ENA
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 1);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_a_params);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);
#endif
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, ON);
	} else {
		RTK_LOGE(TAG, "RTSP2 open fail\n\r");
		goto error;
	}
	printf("RTSP module opened\n\r");

	/* MIMO V/A Linker *******************************************/
	/* MIMO combines independent video/audio inputs. Output dependency flags
	 * describe which inputs each muxer/streamer must wait for. */
	mimo_mp4_rtsp = mimo_create();
	if (mimo_mp4_rtsp) {
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_INPUT0, (uint32_t)video_v1_ctx, 0);
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_INPUT1, (uint32_t)video_v2_ctx, 0);
#if AUDIO_ENA
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_INPUT2, (uint32_t)aac_ctx, 0);
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_OUTPUT0, (uint32_t)mp4_ctx, MMIC_DEP_INPUT0 | MMIC_DEP_INPUT2);
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_OUTPUT1, (uint32_t)rtsp_ctx, MMIC_DEP_INPUT1 | MMIC_DEP_INPUT2);
#else
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_OUTPUT0, (uint32_t)mp4_ctx, MMIC_DEP_INPUT0);
		mimo_ctrl(mimo_mp4_rtsp, MMIC_CMD_ADD_OUTPUT1, (uint32_t)rtsp_ctx, MMIC_DEP_INPUT1);
#endif
		mimo_start(mimo_mp4_rtsp);
	} else {
		RTK_LOGE(TAG, "mimo open fail\n\r");
		goto error;
	}

	return;
error:
	return;
}

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_av_rtsp_mp4_nn_init[] = {
	{
		.v_func = mmf2_video_example_av_rtsp_mp4_nn_init,
		.v_func_name = "mmf2_video_example_av_rtsp_mp4_nn_init",
	}
};
