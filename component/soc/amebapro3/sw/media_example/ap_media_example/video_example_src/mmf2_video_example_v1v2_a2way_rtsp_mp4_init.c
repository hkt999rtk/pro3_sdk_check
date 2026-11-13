/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include "ameba_soc.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "mmf2_simo.h"
#include "mmf2_miso.h"
#include "module_video.h"
#include "module_rtsp2.h"
#include "module_rtp.h"
#include "module_audio.h"
#include "module_aac.h"
#include "module_g711.h"
#include "module_mp4.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 HEVC + AAC audio -> MP4 recording (always on)
 *   V2 H.264 + G711 audio -> RTSP streaming + 2-way RTP audio (default on)
 *
 * Shell command: V2RTSP 1/0  to open/close the V2 RTSP + 2-way audio path
 *
 * Permanent modules (init once, never closed):
 *   audio_ctx, aac_ctx, g711e_ctx, video_v1_ctx, mp4_ctx
 *   simo_audio_aac_g711e (OUTPUT0=aac, OUTPUT1=g711e; OUTPUT1 paused when V2 is off)
 *   miso_v1_mp4
 *
 * V2 path modules (closed/reopened by V2RTSP command):
 *   video_v2_ctx, rtsp_ctx, miso_v2_rtsp
 *   rtp_ctx, g711d_ctx, siso_rtp_g711d, siso_g711d_audio
 */

/* AI Denoise Option */
#define AINR_ENA     1  /* AI noise reduction, will take effect on each video channel */

/* permanent modules */
static mm_context_t *video_v1_ctx         = NULL;
static mm_context_t *audio_ctx            = NULL;
static mm_context_t *aac_ctx              = NULL;
static mm_context_t *g711e_ctx            = NULL;
static mm_context_t *mp4_ctx              = NULL;
static mm_simo_t    *simo_audio_aac_g711e = NULL;
static mm_miso_t    *miso_v1_mp4          = NULL;

/* V2 path modules */
static mm_context_t *video_v2_ctx         = NULL;
static mm_context_t *rtsp_ctx             = NULL;
static mm_context_t *rtp_ctx              = NULL;
static mm_context_t *g711d_ctx            = NULL;
static mm_miso_t    *miso_v2_rtsp         = NULL;
static mm_siso_t    *siso_rtp_g711d       = NULL;
static mm_siso_t    *siso_g711d_audio     = NULL;

/* 0 = v2 rtsp + 2-way audio closed, 1 = running */
static int v2_rtsp_running = 0;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_HEVC,
	.width = 2560,
	.height = 1440,
	.fps = 20,
	.gop = 20,
	.bps = 1000000,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
	.ainr_en = AINR_ENA,
};

static video_params_t video_v2_params = {
	.stream_id = V2_STREAM_ID,
	.format = VIDEO_H264,
	.width = 2560,
	.height = 1440,
	.fps = 20,
	.gop = 20,
	.bps = 1000000,
	.rc_mode = ENC_VBR,
	.min_qp = 36,
	.max_qp = 51,
};

static aac_params_t aac_params = {
	.sample_rate = 8000,
	.channel = 1,
	.trans_type = AAC_TYPE_ADTS, //AAC_TYPE_RAW, AAC_TYPE_ADTS
	.object_type = AAC_AOT_LC,
	.bitrate = 32000,

	.mem_total_size = 10 * 1024,
	.mem_block_size = 128,
	.mem_frame_size = 1024
};

static g711_params_t g711e_params = {
	.codec_id = AV_CODEC_ID_PCMU,
	.buf_len = 2048,
	.mode     = G711_ENCODE
};

static g711_params_t g711d_params = {
	.codec_id = AV_CODEC_ID_PCMU,
	.buf_len = 2048,
	.mode     = G711_DECODE
};

static mp4_params_t mp4_params = {
	.sample_rate = 8000,
	.channel = 1,
	.record_length = 10, //seconds
	.record_type = STORAGE_ALL,
	.record_file_num = 1,
	.record_file_name = "AmebaPro_recording",
	.fatfs_buf_size = 224 * 1024, /* 32kb multiple */
	.mp4_audio_format = AUDIO_AAC,//AUDIO_OPUS,//AUDIO_ULAW
	.mp4_audio_duration = 40,//audio duration for PCM, 8KHz/40ms 16KHz/20ms
};

static rtsp2_params_t rtsp2_v_params = {
	.type = AVMEDIA_TYPE_VIDEO,
	.u = {
		.v = {
			.codec_id = AV_CODEC_ID_H264,
			.fps      = 20,
			.bps      = 0, // if unknown
		}
	}
};

static rtsp2_params_t rtsp2_a_params = {
	.type = AVMEDIA_TYPE_AUDIO,
	.u = {
		.a = {
			.codec_id   = AV_CODEC_ID_PCMU,
			.channel    = 1,
			.samplerate = 8000
		}
	}
};

static rtp_params_t rtp_g711d_params = {
	.valid_pt = 0xFFFFFFFF,
	.port = 16384,
	.frame_size = 1500,
	.cache_depth = 6
};

/* Close V2 RTSP + 2-way audio path without affecting V1. */
static void v2_rtsp_close(void)
{
	/* stop v2 video source first */
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}

	/* stop and delete miso_v2_rtsp linker */
	if (miso_v2_rtsp) {
		miso_stop(miso_v2_rtsp);
		miso_delete(miso_v2_rtsp);
		miso_v2_rtsp = NULL;
	}

	/* pause simo OUTPUT1 (aud->g711e) so OUTPUT0 (aud->aac) keeps running */
	simo_pause(simo_audio_aac_g711e, MM_OUTPUT1);

	/* close RTSP */
	if (rtsp_ctx) {
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
		mm_module_close(rtsp_ctx);
		rtsp_ctx = NULL;
	}

	/* close video v2 */
	if (video_v2_ctx) {
		mm_module_close(video_v2_ctx);
		video_v2_ctx = NULL;
	}

	/* close RTP receive path */
	if (rtp_ctx) {
		mm_module_ctrl(rtp_ctx, CMD_RTP_STREAMING, 0);
	}
	if (siso_rtp_g711d) {
		siso_stop(siso_rtp_g711d);
		siso_delete(siso_rtp_g711d);
		siso_rtp_g711d = NULL;
	}
	if (siso_g711d_audio) {
		siso_stop(siso_g711d_audio);
		siso_delete(siso_g711d_audio);
		siso_g711d_audio = NULL;
	}
	if (rtp_ctx) {
		mm_module_close(rtp_ctx);
		rtp_ctx = NULL;
	}
	if (g711d_ctx) {
		mm_module_close(g711d_ctx);
		g711d_ctx = NULL;
	}

	v2_rtsp_running = 0;
	RTK_LOGI(TAG, "v2 rtsp 2-way audio off\n\r");
}

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

/* Open V2 RTSP + 2-way audio path.
 * Prerequisite: g711e_ctx is open and simo OUTPUT1 (aud->g711e) is paused. */
static int v2_rtsp_open(void)
{
	/* V2 ********************************************************/
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

	/* RTSP ******************************************************/
	rtsp_ctx = mm_module_open(&rtsp2_module);
	if (rtsp_ctx) {
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 0);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_v_params);
		/* Force an I frame when a client sends RTSP PLAY so the stream starts from
		 * a reference frame instead of waiting for the next GOP. */
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);

		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 1);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_a_params);
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0);

		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, ON);
	} else {
		RTK_LOGE(TAG, "RTSP2 open fail\n\r");
		goto error;
	}

	/* MISO V2+G711E -> RTSP Linker ******************************/
	miso_v2_rtsp = miso_create();
	if (miso_v2_rtsp) {
		miso_ctrl(miso_v2_rtsp, MMIC_CMD_ADD_INPUT0, (uint32_t)video_v2_ctx, 0);
		miso_ctrl(miso_v2_rtsp, MMIC_CMD_ADD_INPUT1, (uint32_t)g711e_ctx, 0);
		miso_ctrl(miso_v2_rtsp, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		miso_ctrl(miso_v2_rtsp, MMIC_CMD_SET_TASKNANE, (uint32_t)"v2+g711->rtsp", 0);
		miso_start(miso_v2_rtsp);
	} else {
		RTK_LOGE(TAG, "miso_v2_rtsp open fail\n\r");
		goto error;
	}

	/* resume simo OUTPUT1 (aud->g711e) now that g711e has a consumer */
	simo_resume(simo_audio_aac_g711e);

	/* RTP ******************************************************/
	rtp_ctx = mm_module_open(&rtp_module);
	if (rtp_ctx) {
		mm_module_ctrl(rtp_ctx, CMD_RTP_SET_PARAMS, (int)&rtp_g711d_params);
		mm_module_ctrl(rtp_ctx, MM_CMD_SET_QUEUE_LEN, 16);
		mm_module_ctrl(rtp_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(rtp_ctx, CMD_RTP_APPLY, 0);
		mm_module_ctrl(rtp_ctx, CMD_RTP_STREAMING, 1);
	} else {
		RTK_LOGE(TAG, "RTP open fail\n\r");
		goto error;
	}

	/* G711D *******************************************************/
	g711d_ctx = mm_module_open(&g711_module);
	if (g711d_ctx) {
		mm_module_ctrl(g711d_ctx, CMD_G711_SET_PARAMS, (int)&g711d_params);
		mm_module_ctrl(g711d_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(g711d_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(g711d_ctx, CMD_G711_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "G711D open fail\n\r");
		goto error;
	}

	/* SISO RTP -> G711D ******************************************/
	siso_rtp_g711d = siso_create();
	if (siso_rtp_g711d) {
		siso_ctrl(siso_rtp_g711d, MMIC_CMD_ADD_INPUT, (uint32_t)rtp_ctx, 0);
		siso_ctrl(siso_rtp_g711d, MMIC_CMD_ADD_OUTPUT, (uint32_t)g711d_ctx, 0);
		siso_ctrl(siso_rtp_g711d, MMIC_CMD_SET_TASKNANE, (uint32_t)"rtp->g711d", 0);
		siso_start(siso_rtp_g711d);
	} else {
		RTK_LOGE(TAG, "siso_rtp_g711d open fail\n\r");
		goto error;
	}

	/* SISO G711D -> Audio ****************************************/
	siso_g711d_audio = siso_create();
	if (siso_g711d_audio) {
		siso_ctrl(siso_g711d_audio, MMIC_CMD_ADD_INPUT, (uint32_t)g711d_ctx, 0);
		siso_ctrl(siso_g711d_audio, MMIC_CMD_ADD_OUTPUT, (uint32_t)audio_ctx, 0);
		siso_ctrl(siso_g711d_audio, MMIC_CMD_SET_TASKNANE, (uint32_t)"g711d->aud", 0);
		siso_start(siso_g711d_audio);
	} else {
		RTK_LOGE(TAG, "siso_g711d_audio open fail\n\r");
		goto error;
	}

	v2_rtsp_running = 1;
	RTK_LOGI(TAG, "v2 rtsp 2-way audio on\n\r");
	return 0;

error:
	v2_rtsp_close();
	return -1;
}

void mmf2_video_example_v1v2_a2way_rtsp_mp4_init(void)
{
	/* V1 ********************************************************/
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

	/* Audio *****************************************************/
	audio_ctx = mm_module_open(&audio_module);
	if (audio_ctx) {
		/* set to 8KHz */
		audio_params_t audio_params;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_PARAMS, (int)&audio_params);
		audio_params.sample_rate = ASR_8KHZ;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_SET_PARAMS, (int)&audio_params);
		/* enable audio RX AEC, AGC, NS. (Ameba --> peer) */
		RX_cfg_t audio_rx_cfg;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_RXASP_PARAM, (int)&audio_rx_cfg);
		audio_rx_cfg.aec_cfg.AEC_EN = 1;
		audio_rx_cfg.agc_cfg.AGC_EN = 1;
		audio_rx_cfg.ns_cfg.NS_EN = 1;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_SET_RXASP_PARAM, (int)&audio_rx_cfg);
		/* enable audio TX AGC, NS (peer --> Ameba) */
		TX_cfg_t audio_tx_cfg;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_TXASP_PARAM, (int)&audio_tx_cfg);
		audio_tx_cfg.agc_cfg.AGC_EN = 1;
		audio_tx_cfg.ns_cfg.NS_EN = 1;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_SET_TXASP_PARAM, (int)&audio_tx_cfg);
		/* Set up run time AEC, AGC, NS in TX/RX */
		mm_module_ctrl(audio_ctx, CMD_AUDIO_RUN_AEC, 1);
		mm_module_ctrl(audio_ctx, CMD_AUDIO_RUN_AGC, AUDIO_TX_MASK | AUDIO_RX_MASK);
		mm_module_ctrl(audio_ctx, CMD_AUDIO_RUN_NS, AUDIO_TX_MASK | AUDIO_RX_MASK);

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

	/* G711E (permanent; OUTPUT1 of simo, paused when V2 is off) */
	g711e_ctx = mm_module_open(&g711_module);
	if (g711e_ctx) {
		mm_module_ctrl(g711e_ctx, CMD_G711_SET_PARAMS, (int)&g711e_params);
		mm_module_ctrl(g711e_ctx, MM_CMD_SET_QUEUE_LEN, 6);
		mm_module_ctrl(g711e_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
		mm_module_ctrl(g711e_ctx, CMD_G711_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "G711E open fail\n\r");
		goto error;
	}

	/* SIMO Audio Linker (OUTPUT0: aac->mp4; OUTPUT1: g711e->rtsp) */
	simo_audio_aac_g711e = simo_create();
	if (simo_audio_aac_g711e) {
		simo_ctrl(simo_audio_aac_g711e, MMIC_CMD_ADD_INPUT, (uint32_t)audio_ctx, 0);
		simo_ctrl(simo_audio_aac_g711e, MMIC_CMD_ADD_OUTPUT0, (uint32_t)aac_ctx, 0);
		simo_ctrl(simo_audio_aac_g711e, MMIC_CMD_ADD_OUTPUT1, (uint32_t)g711e_ctx, 0);
		simo_ctrl(simo_audio_aac_g711e, MMIC_CMD_SET_STACKSIZE, 44 * 1024, 0);
		simo_ctrl(simo_audio_aac_g711e, MMIC_CMD_SET_TASKNANE0, (uint32_t)"aud->aac", 0);
		simo_ctrl(simo_audio_aac_g711e, MMIC_CMD_SET_TASKNANE1, (uint32_t)"aud->g711e", 0);
		simo_start(simo_audio_aac_g711e);
	} else {
		RTK_LOGE(TAG, "simo open fail\n\r");
		goto error;
	}

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

	/* MISO V1+AAC -> MP4 Linker *********************************/
	miso_v1_mp4 = miso_create();
	if (miso_v1_mp4) {
		miso_ctrl(miso_v1_mp4, MMIC_CMD_ADD_INPUT0, (uint32_t)video_v1_ctx, 0);
		miso_ctrl(miso_v1_mp4, MMIC_CMD_ADD_INPUT1, (uint32_t)aac_ctx, 0);
		miso_ctrl(miso_v1_mp4, MMIC_CMD_ADD_OUTPUT, (uint32_t)mp4_ctx, 0);
		miso_ctrl(miso_v1_mp4, MMIC_CMD_SET_TASKNANE, (uint32_t)"v1+aac->mp4", 0);
		miso_start(miso_v1_mp4);
	} else {
		RTK_LOGE(TAG, "miso_v1_mp4 open fail\n\r");
		goto error;
	}

	/* V2 RTSP + 2-way audio (default on) ************************/
	if (v2_rtsp_open() != 0) {
		goto error;
	}

	return;
error:
	return;
}

/*
 * Shell command: V2RTSP <en>
 *   V2RTSP 1 : open V2 H.264+G711 RTSP and 2-way audio
 *   V2RTSP 0 : close V2 H.264+G711 RTSP and 2-way audio
 */
u32 CmdV2RtspCtrl(u16 argc, u8 *argv[])
{
	if (argc < 1) {
		RTK_LOGI(TAG, "V2RTSP <0|1>  current=%d\n\r", v2_rtsp_running);
		return _TRUE;
	}

	int en = atoi((char *)argv[0]);

	if (en && !v2_rtsp_running) {
		if (v2_rtsp_open() != 0) {
			RTK_LOGE(TAG, "v2 rtsp open failed\n\r");
		}
	} else if (!en && v2_rtsp_running) {
		v2_rtsp_close();
	} else {
		RTK_LOGI(TAG, "v2 rtsp 2-way audio already %s\n\r", v2_rtsp_running ? "on" : "off");
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE v1v2_a2way_rtsp_mp4_cmd[] = {
	{"V2RTSP", CmdV2RtspCtrl},
};

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1v2_a2way_rtsp_mp4_init[] = {
	{
		.v_func = mmf2_video_example_v1v2_a2way_rtsp_mp4_init,
		.v_func_name = "mmf2_video_example_v1v2_a2way_rtsp_mp4_init",
	}
};
