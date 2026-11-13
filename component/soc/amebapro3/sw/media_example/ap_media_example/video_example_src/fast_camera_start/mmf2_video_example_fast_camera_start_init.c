/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "ameba_soc.h"
#include "mmf2_module.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "mmf2_miso.h"
#include "mmf2_dbg.h"
#include "module_video.h"
#include "module_filesaver.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"
#include "video_fcs_config.h"
#include "video_fcs_params.h"
#include "mmf2_video_example_fast_camera_start_init.h"
#include "module_audio.h"
#include "module_aac.h"
#include "module_mp4.h"
#include "module_rtsp2.h"
#include "mmf2_mediatime_pro3.h"
#include "vfs.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 HEVC video + AAC audio -> MP4 recording
 *   V2 H.264 video -> RTSP (video only)
 *   V3 JPEG video -> snapshot filesaver
 *   V4 NV12 video -> motion/raw-frame path
 *   V5 RGB video -> filesaver (raw frame dump; swap for VIPNN when needed)
 *
 * The example attaches AP modules to channels started by VP FCS and exposes the
 * live V2 stream through RTSP.
 *
 * Audio goes to the MP4 recording only, so it stays on the V1 path:
 *
 *   audio -> AAC --> MISO in1 --+
 *   V1 ------------> MISO in0 --+--> MP4
 *   V2 ------------> SISO -----------> RTSP
 *
 * The V1 path needs a MISO rather than a second linker into MP4 because
 * mp4_muxer_handle() is not re-entrant: its video and audio must be delivered by
 * one task. Every other channel keeps its own SISO.
 *
 * On an FCS boot the two tracks do not start together, and the fix differs by
 * direction. If audio starts first the MP4 muxer stretches the first video sample
 * on its own. If video starts first nothing downstream can invent the missing
 * audio, so fcs_audio_capture_start() arms the audio module to prepend silent
 * frames covering the gap (FCS_AV_SYNC). Use "FCS avsyncinfo" to see which case a
 * given boot hit.
 */

/* ---- User-configurable settings ----------------------------------------- */

#define MP4_ENA      1  /* set to 0 to disable MP4 SD card recording on V1 */
#define AUDIO_ENA    1  /* set to 0 to disable audio/AAC on the V1 MP4 recording */
#define FCS_AV_SYNC  1  /* set to 0 to never back-fill silence when audio starts after video */

/* ---- Internal constants (do not modify) ---------------------------------- */

/* Audio only has a sink when V1 records to MP4; filesaver cannot mux audio. */
#define MP4_AUDIO_ENA   (AUDIO_ENA && FCS_V1_ENA && MP4_ENA)


static mm_context_t *video_v1_ctx       = NULL;
static mm_context_t *video_v2_ctx       = NULL;
static mm_context_t *video_v3_ctx       = NULL;
static mm_context_t *video_v4_ctx       = NULL;
static mm_context_t *video_v5_ctx       = NULL;

static int fcs_started = 0;
static rtos_mutex_t fcs_ap_lock = NULL;

static int fcs_ap_lock_ensure(void)
{
	if (fcs_ap_lock == NULL) {
		rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
		if (fcs_ap_lock == NULL) {
			rtos_mutex_create(&fcs_ap_lock);
		}
		rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
	}
	return fcs_ap_lock ? 0 : -1;
}

static mm_context_t *fs_v1_ctx          = NULL;
static mm_context_t *fs_v3_ctx          = NULL;
static mm_context_t *fs_v4_ctx          = NULL;
static mm_context_t *fs_v5_ctx          = NULL;
static mm_context_t *rtsp_ctx           = NULL;
static mm_context_t *mp4_ctx            = NULL;
static mm_context_t *audio_ctx          = NULL;
static mm_context_t *aac_ctx            = NULL;

/* Start-time bookkeeping reported by the "FCS avsyncinfo" command. Kept in one
 * struct, separate from the module contexts above, so it reads at a glance as
 * report-only state rather than something the pipeline depends on. */
typedef struct fcs_info_global_s {
	uint32_t audio_apply_ms;                    /* media time CMD_AUDIO_APPLY returned */
	uint32_t audio_frame_ms;                     /* duration of one captured audio frame */
	uint32_t avsync_vtime;                       /* video start handed to the audio module, 0 = back-fill not armed */
	uint32_t attach_ms[VIDEO_ISP_VALID_CH];      /* media time CMD_VIDEO_FCS_ATTACH returned, per channel */
} fcs_info_global_t;

static fcs_info_global_t g_fcs_info;

static mm_siso_t *siso_audio_aac        = NULL;
static mm_miso_t *miso_v1               = NULL;
static mm_siso_t *siso_v2               = NULL;
static mm_siso_t *siso_v3               = NULL;
static mm_siso_t *siso_v4               = NULL;
static mm_siso_t *siso_v5               = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = FCS_CODEC_VAL(FCS_V1_CODEC),
	.width = FCS_V1_W,
	.height = FCS_V1_H,
	.fps = FCS_V1_FPS,
	.gop = FCS_V1_GOP,
	.bps = FCS_V1_BPS,
	.rc_mode = FCS_ENC_RCMODE,
	.min_qp = FCS_ENC_MINQP,
	.max_qp = FCS_ENC_MAXQP,
	.qlevel = FCS_V1_QLEVEL,
};

static video_params_t video_v2_params = {
	.stream_id = V2_STREAM_ID,
	.format = FCS_CODEC_VAL(FCS_V2_CODEC),
	.width = FCS_V2_W,
	.height = FCS_V2_H,
	.fps = FCS_V2_FPS,
	.gop = FCS_V2_GOP,
	.bps = FCS_V2_BPS,
	.rc_mode = FCS_ENC_RCMODE,
	.min_qp = FCS_ENC_MINQP,
	.max_qp = FCS_ENC_MAXQP,
	.qlevel = FCS_V2_QLEVEL,
};

static video_params_t video_v3_params = {
	.stream_id = V3_STREAM_ID,
	.format = FCS_CODEC_VAL(FCS_V3_CODEC),
	.width = FCS_V3_W,
	.height = FCS_V3_H,
	.fps = FCS_V3_FPS,
	.gop = FCS_V3_GOP,
	.bps = FCS_V3_BPS,
	.rc_mode = FCS_V3_RCMODE,
	.min_qp = FCS_V3_MINQP,
	.max_qp = FCS_V3_MAXQP,
	.qlevel = FCS_V3_QLEVEL,
};

static video_params_t video_v4_params = {
	.stream_id = V4_STREAM_ID,
	.format = VIDEO_NV12,
	.width = FCS_V4_W,
	.height = FCS_V4_H,
	.fps = FCS_V4_FPS,
};

static video_params_t video_v5_params = {
	.stream_id = V5_STREAM_ID,
	.format = VIDEO_RGB,
	.width = FCS_V5_W,
	.height = FCS_V5_H,
	.fps = FCS_V5_FPS,
};

#if MP4_AUDIO_ENA
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
#endif

static mp4_params_t mp4_v1_params = {
	.sample_rate = 16000,
	.channel = 1,
	.record_length = 10, //seconds
	.record_type = MP4_AUDIO_ENA ? STORAGE_ALL : STORAGE_VIDEO,
	.record_file_num = 1,
	.record_file_name = "AmebaPro_recording_FCS",
	.fatfs_buf_size = 224 * 1024, /* 32kb multiple */
	.mp4_audio_format = AUDIO_AAC,
	.mp4_audio_duration = 20,//audio duration for PCM, 8KHz/40ms 16KHz/20ms
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

static void saver_handler(char *file_name, uint32_t data_addr, uint32_t data_size)
{
	(void)file_name;
	(void)data_addr;
	(void)data_size;
}

/*
 * Overwrite the compile-time video_vX_params with values from PSRAM when a
 * valid fcs_persistent_params_t is present.  Called at the start of each FCS
 * init so that params saved by mmf2_fcs_params_save_current() take effect on
 * the next boot without recompiling.
 */
static void fcs_load_params_from_psram(void)
{
	const fcs_persistent_params_t *p = FCS_PARAMS_ADDR;
	if (!fcs_params_valid(p)) {
		return;
	}

	video_v1_params.format   = p->ch[0].format;
	video_v1_params.width    = p->ch[0].width;
	video_v1_params.height   = p->ch[0].height;
	video_v1_params.fps      = p->ch[0].fps;
	video_v1_params.gop      = p->ch[0].gop;
	video_v1_params.bps      = p->ch[0].bps;
	video_v1_params.rc_mode  = p->ch[0].rc_mode;
	video_v1_params.min_qp   = p->ch[0].min_qp;
	video_v1_params.max_qp   = p->ch[0].max_qp;
	video_v1_params.qlevel   = p->ch[0].qlevel;
	video_v1_params.ainr_en  = p->ainr_ena;

	video_v2_params.format   = p->ch[1].format;
	video_v2_params.width    = p->ch[1].width;
	video_v2_params.height   = p->ch[1].height;
	video_v2_params.fps      = p->ch[1].fps;
	video_v2_params.gop      = p->ch[1].gop;
	video_v2_params.bps      = p->ch[1].bps;
	video_v2_params.rc_mode  = p->ch[1].rc_mode;
	video_v2_params.min_qp   = p->ch[1].min_qp;
	video_v2_params.max_qp   = p->ch[1].max_qp;
	video_v2_params.qlevel   = p->ch[1].qlevel;

	video_v3_params.format   = p->ch[2].format;
	video_v3_params.width    = p->ch[2].width;
	video_v3_params.height   = p->ch[2].height;
	video_v3_params.fps      = p->ch[2].fps;
	video_v3_params.gop      = p->ch[2].gop;
	video_v3_params.bps      = p->ch[2].bps;
	video_v3_params.rc_mode  = p->ch[2].rc_mode;
	video_v3_params.min_qp   = p->ch[2].min_qp;
	video_v3_params.max_qp   = p->ch[2].max_qp;
	video_v3_params.qlevel   = p->ch[2].qlevel;

	video_v4_params.width    = p->ch[3].width;
	video_v4_params.height   = p->ch[3].height;
	video_v4_params.fps      = p->ch[3].fps;

	video_v5_params.width    = p->ch[4].width;
	video_v5_params.height   = p->ch[4].height;
	video_v5_params.fps      = p->ch[4].fps;
}

#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
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

static int fcs_rtsp_params_update(void)
{
	switch (video_v2_params.format) {
	case VIDEO_H264:
		rtsp2_params.u.v.codec_id = AV_CODEC_ID_H264;
		break;
	case VIDEO_HEVC:
		rtsp2_params.u.v.codec_id = AV_CODEC_ID_H265;
		break;
	case VIDEO_JPEG:
		rtsp2_params.u.v.codec_id = AV_CODEC_ID_MJPEG;
		break;
	default:
		RTK_LOGE(TAG, "unsupported V2 RTSP format 0x%lx\r\n", video_v2_params.format);
		return -1;
	}
	rtsp2_params.u.v.fps = video_v2_params.fps;
	rtsp2_params.u.v.bps = video_v2_params.bps;
	return 0;
}
#endif

#if MP4_AUDIO_ENA
/*
 * Start audio capture, arming the FCS AV-sync silence back-fill only when it is
 * actually needed.
 *
 * Two directions are possible on an FCS boot and they need opposite fixes:
 *
 *   audio starts first  - the recording opens with audio that has no video yet.
 *                         The MP4 muxer already handles this by stretching the
 *                         first video sample, so nothing to do here.
 *   video starts first  - the recording would open with video that has no audio.
 *                         Nothing downstream can invent the missing audio, so the
 *                         audio module is asked to prepend silent frames covering
 *                         [video start, audio start) and both tracks then begin at
 *                         the same media time.
 *
 * Which direction applies is only known once video has produced a frame, so V1 is
 * queried here: if it already has one, audio is the later of the two and the
 * back-fill is armed with that frame's timestamp. If it does not, audio is
 * starting first and the back-fill is left disarmed. The audio module repeats the
 * comparison internally (it only fills when its first captured frame is later
 * than the timestamp given), so arming it is safe even if the two race.
 *
 * The timestamp has to be handed over before capture starts. The module injects
 * the back-fill on its first RX interrupt; had real frames already been queued
 * with newer timestamps, the back-dated silent frames would land behind them and
 * the audio queue would stop being monotonic, which breaks the muxer's per-sample
 * delta calculation. CMD_AUDIO_APPLY is therefore the last step here.
 */
static int fcs_audio_capture_start(void)
{
	if (!audio_ctx) {
		return -1;
	}
#if FCS_AV_SYNC
	if (video_v1_ctx) {
		video_first_frame_info_t v1;

		memset(&v1, 0, sizeof(v1));
		if (mm_module_ctrl(video_v1_ctx, CMD_VIDEO_FCS_GET_INFO, (int)&v1) >= 0 && v1.valid) {
			mm_module_ctrl(audio_ctx, CMD_AUDIO_SET_AVSYNC_TIMESTAMP, (int)v1.timestamp);
			g_fcs_info.avsync_vtime = v1.timestamp;
		}
	}
#endif
	if (mm_module_ctrl(audio_ctx, CMD_AUDIO_APPLY, 0) < 0) {
		return -1;
	}
	/* Recorded for the "FCS avsyncinfo" report. */
	g_fcs_info.audio_apply_ms = mm_read_mediatime_ms();
	mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_FRAMESIZE_MS, (int)&g_fcs_info.audio_frame_ms);
	return 0;
}
#endif

static void mmf2_video_example_fast_camera_start_init(void)
{
	if (fcs_ap_lock_ensure() < 0) {
		RTK_LOGE(TAG, "FCS AP lock create failed\r\n");
		return;
	}
	rtos_mutex_take(fcs_ap_lock, portMAX_DELAY);
	if (fcs_started) {
		rtos_mutex_give(fcs_ap_lock);
		return;
	}
	// DBG_MMF_INFO_MSG_OFF(0xffffffff);
	fcs_load_params_from_psram();

#if MP4_AUDIO_ENA
	/* Audio + AAC ***********************************************/
	/* Created before any video channel so that audio is ready to run as early as
	 * possible: this is a fast-camera-start example, so the goal is to keep the
	 * audio-to-video start delta small. Capture itself is started later by
	 * fcs_audio_capture_start(), once V1 exists to compare against. */
	audio_ctx = mm_module_open(&audio_module);
	if (audio_ctx) {
		audio_params_t audio_params;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_PARAMS, (int)&audio_params);
		audio_params.sample_rate = ASR_16KHZ;
		/* Lets the module back-fill silence if it starts after video. Harmless
		 * when unused: the back-fill also needs a non-zero avsync timestamp. */
		audio_params.avsync_en = FCS_AV_SYNC;
		mm_module_ctrl(audio_ctx, CMD_AUDIO_SET_PARAMS, (int)&audio_params);
		/* Deep enough to hold the audio captured while MP4 opens its file on the
		 * SD card, and it also caps the silence back-fill: the module takes the
		 * silent frames from this pool, so at 20 ms per frame 200 items can cover
		 * a gap of about 4 s. */
		mm_module_ctrl(audio_ctx, MM_CMD_SET_QUEUE_LEN, 200);
		mm_module_ctrl(audio_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_STATIC);
	} else {
		RTK_LOGE(TAG, "audio open fail\n\r");
		goto error;
	}

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

	/* SISO carries raw PCM into the AAC encoder. The encoded output becomes the
	 * audio input of miso_v1 below. */
	siso_audio_aac = siso_create();
	if (siso_audio_aac) {
		siso_ctrl(siso_audio_aac, MMIC_CMD_ADD_INPUT, (uint32_t)audio_ctx, 0);
		siso_ctrl(siso_audio_aac, MMIC_CMD_ADD_OUTPUT, (uint32_t)aac_ctx, 0);
		siso_ctrl(siso_audio_aac, MMIC_CMD_SET_STACKSIZE, 44 * 1024, 0);
		siso_ctrl(siso_audio_aac, MMIC_CMD_SET_TASKNANE, (uint32_t)"aud->aac", 0);
		if (siso_start(siso_audio_aac) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_audio_aac open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_audio_aac started\n\r");
#endif

#if FCS_V1_ENA
	/* V1 ********************************************************/
	/* VP already created the FCS channel. AP opens video_module, prepares its
	 * queue, then CMD_VIDEO_FCS_ATTACH starts the deferred VP2AP path. */
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
		mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(video_v1_ctx, CMD_VIDEO_FCS_ATTACH, 0) < 0) {
			RTK_LOGE(TAG, "video v1 FCS attach fail\n\r");
			goto error;
		}
		g_fcs_info.attach_ms[V1_STREAM_ID] = mm_read_mediatime_ms();
	} else {
		RTK_LOGE(TAG, "video v1 open fail\n\r");
		goto error;
	}
#if MP4_AUDIO_ENA
	/* Start audio capture now that V1 is attached, so the AV-sync back-fill can
	 * be armed if video already got there first. */
	if (fcs_audio_capture_start() < 0) {
		RTK_LOGE(TAG, "audio capture start fail\n\r");
		goto error;
	}
#endif
#if MP4_ENA
	mp4_v1_params.fps = video_v1_params.fps;
	mp4_v1_params.gop = video_v1_params.gop;
	mp4_v1_params.width = video_v1_params.width;
	mp4_v1_params.height = video_v1_params.height;
	if (g_fcs_info.audio_frame_ms) {
		mp4_v1_params.mp4_audio_duration = g_fcs_info.audio_frame_ms;
	}
	/* MP4 */
	mp4_ctx = mm_module_open(&mp4_module);
	if (mp4_ctx) {
		mm_module_ctrl(mp4_ctx, CMD_MP4_SET_PARAMS, (int)&mp4_v1_params);
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
		/* The 4K portrait frame content is still lying on its side; record the
		 * display rotation in the tkhd matrix before CMD_MP4_START writes it. */
		mm_module_ctrl(mp4_ctx, CMD_MP4_SET_ROTATION, MP4_ROTATION_270);
#endif
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
	/* Link V1 to its selected recorder. MISO input0 is always the V1 video; when
	 * audio is enabled input1 adds the AAC stream, so MP4 receives both tracks
	 * from this one task. */
	miso_v1 = miso_create();
	if (miso_v1) {
		miso_ctrl(miso_v1, MMIC_CMD_ADD_INPUT0, (uint32_t)video_v1_ctx, 0);
#if MP4_AUDIO_ENA
		miso_ctrl(miso_v1, MMIC_CMD_ADD_INPUT1, (uint32_t)aac_ctx, 0);
#endif
#if MP4_ENA
		miso_ctrl(miso_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)mp4_ctx, 0);
		miso_ctrl(miso_v1, MMIC_CMD_SET_TASKNANE, (uint32_t)"v1+aac->mp4", 0);
#else
		miso_ctrl(miso_v1, MMIC_CMD_ADD_OUTPUT, (uint32_t)fs_v1_ctx, 0);
		miso_ctrl(miso_v1, MMIC_CMD_SET_TASKNANE, (uint32_t)"v1->fs", 0);
#endif
		if (miso_start(miso_v1) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "miso_v1 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "miso_v1 started\n\r");
#endif

#if FCS_V2_ENA
	/* V2 ********************************************************/
	/* V2 is the RTSP live stream shown to the user. */
	video_v2_ctx = mm_module_open(&video_module);
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v2_params);
		mm_module_ctrl(video_v2_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v2_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(video_v2_ctx, CMD_VIDEO_FCS_ATTACH, 0) < 0) {
			RTK_LOGE(TAG, "video v2 FCS attach fail\n\r");
			goto error;
		}
		g_fcs_info.attach_ms[V2_STREAM_ID] = mm_read_mediatime_ms();
	} else {
		RTK_LOGE(TAG, "video v2 open fail\n\r");
		goto error;
	}
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
	if (fcs_rtsp_params_update() < 0) {
		goto error;
	}
	rtsp_ctx = mm_module_open(&rtsp2_module);
	if (rtsp_ctx) {
		/* CMD_RTSP2_SET_START_CB forces an I frame when a client sends RTSP PLAY so
		 * the stream starts from a reference frame instead of waiting for the next GOP. */
		if (mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SELECT_STREAM, 0) < 0 ||
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_PARAMS, (int)&rtsp2_params) < 0 ||
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_START_CB, (int)prv_rtsp_play_cb) < 0 ||
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_APPLY, 0) < 0 ||
			mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, ON) < 0) {
			RTK_LOGE(TAG, "RTSP2 apply fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "RTSP2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "RTSP module opened\n\r");
#else
	RTK_LOGE(TAG, "RTSP not supported: CONFIG_LWIP_LAYER not enabled\n\r");
	goto error;
#endif
	/* Link V2 to the RTSP sink. Video only, so a plain SISO is enough. */
	siso_v2 = siso_create();
	if (siso_v2) {
		siso_ctrl(siso_v2, MMIC_CMD_ADD_INPUT, (uint32_t)video_v2_ctx, 0);
		siso_ctrl(siso_v2, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		siso_ctrl(siso_v2, MMIC_CMD_SET_TASKNANE, (uint32_t)"v2->rtsp", 0);
		if (siso_start(siso_v2) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_v2 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v2 started\n\r");
#endif

#if FCS_V3_ENA
	/* V3 ********************************************************/
	/* V3 is a low-FPS JPEG snapshot stream saved by filesaver. */
	video_v3_ctx = mm_module_open(&video_module);
	if (video_v3_ctx) {
		mm_module_ctrl(video_v3_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v3_params);
		mm_module_ctrl(video_v3_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v3_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(video_v3_ctx, CMD_VIDEO_FCS_ATTACH, 0) < 0) {
			RTK_LOGE(TAG, "video v3 FCS attach fail\n\r");
			goto error;
		}
		g_fcs_info.attach_ms[V3_STREAM_ID] = mm_read_mediatime_ms();
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
		siso_ctrl(siso_v3, MMIC_CMD_SET_TASKNANE, (uint32_t)"v3->fs", 0);
		if (siso_start(siso_v3) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_v3 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v3 started\n\r");
#endif

#if FCS_V4_ENA
	/* V4 ********************************************************/
	/* V4 outputs raw NV12 frames for motion/debug style consumers. */
	video_v4_ctx = mm_module_open(&video_module);
	if (video_v4_ctx) {
		mm_module_ctrl(video_v4_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v4_params);
		mm_module_ctrl(video_v4_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v4_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(video_v4_ctx, CMD_VIDEO_FCS_ATTACH, 0) < 0) {
			RTK_LOGE(TAG, "video v4 FCS attach fail\n\r");
			goto error;
		}
		g_fcs_info.attach_ms[V4_STREAM_ID] = mm_read_mediatime_ms();
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
		siso_ctrl(siso_v4, MMIC_CMD_SET_TASKNANE, (uint32_t)"v4->fs", 0);
		if (siso_start(siso_v4) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_v4 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v4 started\n\r");
#endif

#if FCS_V5_ENA
	/* V5 ********************************************************/
	/* V5 is an RGB stream */
	video_v5_ctx = mm_module_open(&video_module);
	if (video_v5_ctx) {
		mm_module_ctrl(video_v5_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v5_params);
		mm_module_ctrl(video_v5_ctx, MM_CMD_SET_QUEUE_LEN, 60);
		mm_module_ctrl(video_v5_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
		if (mm_module_ctrl(video_v5_ctx, CMD_VIDEO_FCS_ATTACH, 0) < 0) {
			RTK_LOGE(TAG, "video v5 FCS attach fail\n\r");
			goto error;
		}
		g_fcs_info.attach_ms[V5_STREAM_ID] = mm_read_mediatime_ms();
	} else {
		RTK_LOGE(TAG, "video v5 open fail\n\r");
		goto error;
	}

	fs_v5_ctx = mm_module_open(&filesaver_module);
	if (fs_v5_ctx) {
		mm_module_ctrl(fs_v5_ctx, CMD_FILESAVER_SET_TYPE_HANDLER, (int)saver_handler);
		mm_module_ctrl(fs_v5_ctx, CMD_FILESAVER_APPLY, 0);
	} else {
		RTK_LOGE(TAG, "fs_v5_ctx open fail\n\r");
		goto error;
	}

	/* Link V5 filesaver depending */
	siso_v5 = siso_create();
	if (siso_v5) {
		siso_ctrl(siso_v5, MMIC_CMD_ADD_INPUT, (uint32_t)video_v5_ctx, 0);
		siso_ctrl(siso_v5, MMIC_CMD_ADD_OUTPUT, (uint32_t)fs_v5_ctx, 0);
		siso_ctrl(siso_v5, MMIC_CMD_SET_TASKNANE, (uint32_t)"v5->fs", 0);
		if (siso_start(siso_v5) < 0) {
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_v5 open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v5 started\n\r");

#endif
	fcs_started = 1;
	rtos_mutex_give(fcs_ap_lock);
	return;
error:
	fcs_started = 0;
#if FCS_V5_ENA
	if (siso_v5) {
		siso_stop(siso_v5);
		siso_delete(siso_v5);
		siso_v5 = NULL;
	}
	if (fs_v5_ctx) {
		mm_module_close(fs_v5_ctx);
		fs_v5_ctx = NULL;
	}
	if (video_v5_ctx) {
		mm_module_ctrl(video_v5_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v5_ctx);
		video_v5_ctx = NULL;
	}
#endif
#if FCS_V4_ENA
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
		mm_module_ctrl(video_v4_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v4_ctx);
		video_v4_ctx = NULL;
	}
#endif
#if FCS_V3_ENA
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
		mm_module_ctrl(video_v3_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v3_ctx);
		video_v3_ctx = NULL;
	}
#endif
#if FCS_V2_ENA
	if (siso_v2) {
		siso_stop(siso_v2);
		siso_delete(siso_v2);
		siso_v2 = NULL;
	}
	if (rtsp_ctx) {
		mm_module_close(rtsp_ctx);
		rtsp_ctx = NULL;
	}
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v2_ctx);
		video_v2_ctx = NULL;
	}
#endif
#if FCS_V1_ENA
	if (miso_v1) {
		miso_stop(miso_v1);
		miso_delete(miso_v1);
		miso_v1 = NULL;
	}
#if MP4_ENA
	if (mp4_ctx) {
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
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
	}
#endif
#if MP4_AUDIO_ENA
	if (siso_audio_aac) {
		siso_stop(siso_audio_aac);
		siso_delete(siso_audio_aac);
		siso_audio_aac = NULL;
	}
	if (aac_ctx) {
		mm_module_close(aac_ctx);
		aac_ctx = NULL;
	}
	if (audio_ctx) {
		mm_module_close(audio_ctx);
		audio_ctx = NULL;
	}
#endif
	rtos_mutex_give(fcs_ap_lock);
}

void mmf2_video_example_fast_camera_start_rtsp_init(void)
{
	mmf2_video_example_fast_camera_start_init();
}

static mm_context_t *fcs_video_ctx_get(int ch)
{
	switch (ch) {
	case V1_STREAM_ID:
		return video_v1_ctx;
	case V2_STREAM_ID:
		return video_v2_ctx;
	case V3_STREAM_ID:
		return video_v3_ctx;
	case V4_STREAM_ID:
		return video_v4_ctx;
	case V5_STREAM_ID:
		return video_v5_ctx;
	default:
		return NULL;
	}
}

static mm_siso_t *fcs_video_siso_get(int ch)
{
	switch (ch) {
	case V2_STREAM_ID:
		return siso_v2;
	case V3_STREAM_ID:
		return siso_v3;
	case V4_STREAM_ID:
		return siso_v4;
	case V5_STREAM_ID:
		return siso_v5;
	default:
		return NULL;
	}
}

/* Pause the linker feeding channel `ch`. Returns 0 once it reached pause state.
 * V1 uses a MISO because it may also carry audio; MM_OUTPUT blocks that whole
 * linker, matching what siso_pause() does for the other channels. */
static int fcs_channel_linker_pause(int ch)
{
	mm_siso_t *siso;

	if (ch == V1_STREAM_ID) {
		if (!miso_v1 || !miso_v1->task) {
			return -1;
		}
		miso_pause(miso_v1, MM_OUTPUT);
		return (miso_v1->status == MMIC_STAT_PAUSE) ? 0 : -1;
	}

	siso = fcs_video_siso_get(ch);
	if (!siso || !siso->task) {
		return -1;
	}
	siso_pause(siso);
	return (siso->status == MMIC_STAT_PAUSE) ? 0 : -1;
}

/* Resume the linker paused by fcs_channel_linker_pause(). Returns 0 on success. */
static int fcs_channel_linker_resume(int ch)
{
	mm_siso_t *siso;

	if (ch == V1_STREAM_ID) {
		if (!miso_v1 || !miso_v1->task) {
			return -1;
		}
		miso_resume(miso_v1);
		return (miso_v1->status == MMIC_STAT_RUN) ? 0 : -1;
	}

	siso = fcs_video_siso_get(ch);
	if (!siso || !siso->task) {
		return -1;
	}
	siso_resume(siso);
	return (siso->status == MMIC_STAT_RUN) ? 0 : -1;
}

static video_params_t *fcs_video_params_get(int ch)
{
	switch (ch) {
	case V1_STREAM_ID:
		return &video_v1_params;
	case V2_STREAM_ID:
		return &video_v2_params;
	case V3_STREAM_ID:
		return &video_v3_params;
	case V4_STREAM_ID:
		return &video_v4_params;
	case V5_STREAM_ID:
		return &video_v5_params;
	default:
		return NULL;
	}
}

/* Fully close a VP channel, apply new AP parameters, and rebuild the VP path.
 * The AP video module and its downstream linker remain allocated. */
int mmf2_fcs_video_channel_reopen(int ch, const video_params_t *new_params)
{
	mm_context_t *video_ctx;
	video_params_t *current_params;
	int ret = -1;

	if (fcs_ap_lock_ensure() < 0) {
		return -1;
	}
	rtos_mutex_take(fcs_ap_lock, portMAX_DELAY);
	video_ctx = fcs_video_ctx_get(ch);
	current_params = fcs_video_params_get(ch);
	if (!video_ctx || !current_params || !new_params || new_params->stream_id != (uint32_t)ch) {
		goto exit;
	}
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER != 0)
	if (ch == V2_STREAM_ID && rtsp_ctx &&
		new_params->format != VIDEO_H264 && new_params->format != VIDEO_HEVC && new_params->format != VIDEO_JPEG) {
		goto exit;
	}
#endif

	if (fcs_channel_linker_pause(ch) < 0) {
		RTK_LOGE(TAG, "FCS ch%d linker pause fail\r\n", ch);
		goto exit;
	}
	if (mm_module_ctrl(video_ctx, CMD_VIDEO_STREAM_STOP, 0) < 0) {
		fcs_channel_linker_resume(ch);
		goto exit;
	}

	if (mm_module_ctrl(video_ctx, CMD_VIDEO_SET_PARAMS, (int)new_params) < 0 ||
		mm_module_ctrl(video_ctx, CMD_VIDEO_APPLY, 0) < 0) {
		goto exit;
	}
	memcpy(current_params, new_params, sizeof(video_params_t));

	if (fcs_channel_linker_resume(ch) < 0) {
		goto stop_channel;
	}
	ret = 0;
	goto exit;

stop_channel:
	mm_module_ctrl(video_ctx, CMD_VIDEO_STREAM_STOP, 0);
exit:
	rtos_mutex_give(fcs_ap_lock);
	return ret;
}

static void fcs_reopen_test_usage(void)
{
	RTK_LOGI(TAG, "usage:\r\n");
	RTK_LOGI(TAG, "  FCS show <ch>\r\n");
	RTK_LOGI(TAG, "  FCS reopen <ch> <width> <height> [bps]\r\n");
	RTK_LOGI(TAG, "  FCS save\r\n");
	RTK_LOGI(TAG, "  FCS avsyncinfo\r\n");
	RTK_LOGI(TAG, "example: FCS reopen 1 1280 720 300000\r\n");
}

/*
 * "FCS avsyncinfo" - report when each stream actually started, so the audio/video
 * start offset of an FCS boot can be inspected without a debugger.
 *
 * All times are media time in ms from the one clock shared by VP and AP
 * (mm_read_mediatime_ms(), backed by SYSTIMER), so video and audio timestamps
 * are directly comparable.
 *
 * Video, per channel:
 *   first_frame  media time VP stamped when it encoded the first frame it sent to
 *                AP. For FCS this can predate the AP attach, because VP starts
 *                the sensor long before AP is up.
 *   arrived      media time that frame reached the AP pipeline. The bracketed
 *                value is (arrived - first_frame), i.e. VP->AP transfer latency
 *                plus any time the frame sat waiting on VP.
 *   attach       media time CMD_VIDEO_FCS_ATTACH returned for that channel.
 *
 * Audio:
 *   first_frame  media time of the first captured PCM frame, back-dated to its
 *                first sample.
 *   apply        media time CMD_AUDIO_APPLY returned, i.e. when capture started.
 *   frame_len    duration of one captured audio frame. This is the granularity of
 *                the audio timestamps, so an offset smaller than this is within
 *                one frame and cannot be resolved any finer.
 *
 * The last line is the one that matters: it states which of audio / V1 video
 * started first and by how much. The MP4 muxer already compensates whichever
 * direction it sees by stretching the first sample of the later-starting track,
 * so a small offset needs no action.
 */
static void fcs_avsync_info_show(void)
{
	static const int ch_list[] = {V1_STREAM_ID, V2_STREAM_ID, V3_STREAM_ID, V4_STREAM_ID, V5_STREAM_ID};
	video_first_frame_info_t info;
	video_first_frame_info_t v1_info;
	uint32_t a_data_ts = 0;
	uint32_t a_dummy_ts = 0;
	unsigned int i;

	memset(&v1_info, 0, sizeof(v1_info));

	RTK_LOGI(TAG, "---- FCS av start info (media time in ms) ----\r\n");
	RTK_LOGI(TAG, "now=%lu\r\n", (unsigned long)mm_read_mediatime_ms());

	for (i = 0; i < sizeof(ch_list) / sizeof(ch_list[0]); i++) {
		int ch = ch_list[i];
		mm_context_t *vctx = fcs_video_ctx_get(ch);

		if (!vctx) {
			RTK_LOGI(TAG, "V%d: disabled\r\n", ch + 1);
			continue;
		}
		memset(&info, 0, sizeof(info));
		if (mm_module_ctrl(vctx, CMD_VIDEO_FCS_GET_INFO, (int)&info) < 0) {
			RTK_LOGI(TAG, "V%d: query fail\r\n", ch + 1);
			continue;
		}
		if (!info.valid) {
			RTK_LOGI(TAG, "V%d: no frame yet, attach=%lu\r\n",
					 ch + 1, (unsigned long)g_fcs_info.attach_ms[ch]);
			continue;
		}
		if (ch == V1_STREAM_ID) {
			v1_info = info;
		}
		RTK_LOGI(TAG, "V%d: first_frame=%lu  arrived=%lu (+%d)  attach=%lu\r\n",
				 ch + 1, (unsigned long)info.timestamp, (unsigned long)info.arrive_ms,
				 (int)(info.arrive_ms - info.timestamp), (unsigned long)g_fcs_info.attach_ms[ch]);
	}

	if (audio_ctx) {
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_FIRST_DATA_TS, (int)&a_data_ts);
		mm_module_ctrl(audio_ctx, CMD_AUDIO_GET_FIRST_DUMMY_TS, (int)&a_dummy_ts);
		RTK_LOGI(TAG, "A : first_frame=%lu  apply=%lu  frame_len=%lu ms per frame\r\n",
				 (unsigned long)a_data_ts, (unsigned long)g_fcs_info.audio_apply_ms,
				 (unsigned long)g_fcs_info.audio_frame_ms);
		if (a_dummy_ts) {
			/* Back-fill ran: silence covers [dummy_ts, first_frame). */
			RTK_LOGI(TAG, "    silence back-fill: %lu -> %lu (%d ms)\r\n",
					 (unsigned long)a_dummy_ts, (unsigned long)a_data_ts,
					 (int)(a_data_ts - a_dummy_ts));
		} else if (g_fcs_info.avsync_vtime) {
			RTK_LOGI(TAG, "    back-fill armed at %lu but not needed\r\n",
					 (unsigned long)g_fcs_info.avsync_vtime);
		} else {
			RTK_LOGI(TAG, "    back-fill not armed (audio started before video)\r\n");
		}
	} else {
		RTK_LOGI(TAG, "A : disabled\r\n");
	}

	/* Bottom line: who started first, and by how much. */
	if (!audio_ctx || !a_data_ts) {
		RTK_LOGI(TAG, "=> audio not started, no offset to report\r\n");
	} else if (!v1_info.valid) {
		RTK_LOGI(TAG, "=> V1 video not started, no offset to report\r\n");
	} else {
		int offset = (int)(a_data_ts - v1_info.timestamp);
		if (offset < 0) {
			RTK_LOGI(TAG, "=> audio started %d ms before V1 video\r\n", -offset);
		} else if (offset > 0) {
			RTK_LOGI(TAG, "=> V1 video started %d ms before audio\r\n", offset);
		} else {
			RTK_LOGI(TAG, "=> audio and V1 video started together\r\n");
		}
	}
	RTK_LOGI(TAG, "---------------------------------------------\r\n");
}

static void fcs_reopen_test_params_show(int ch, const video_params_t *params)
{
	RTK_LOGI(TAG, "FCS ch%d fmt=0x%lx %lux%lu@%lu gop=%lu bps=%lu rc=%lu qp=[%lu,%lu]\r\n",
			 ch, (unsigned long)params->format,
			 (unsigned long)params->width, (unsigned long)params->height,
			 (unsigned long)params->fps, (unsigned long)params->gop,
			 (unsigned long)params->bps, (unsigned long)params->rc_mode,
			 (unsigned long)params->min_qp, (unsigned long)params->max_qp);
}

u32 CmdFcsVideoReopenTest(IN u16 argc, IN u8 *argv[])
{
	video_params_t *current_params;
	video_params_t new_params;
	int ch;
	int width;
	int height;
	int bps;

	if (argc < 1) {
		fcs_reopen_test_usage();
		return _FALSE;
	}

	if (_strcmp((const char *)argv[0], "show") == 0) {
		if (argc != 2) {
			fcs_reopen_test_usage();
			return _FALSE;
		}
		ch = atoi((const char *)argv[1]);
		current_params = fcs_video_params_get(ch);
		if (!current_params) {
			RTK_LOGE(TAG, "invalid FCS channel %d\r\n", ch);
			return _FALSE;
		}
		fcs_reopen_test_params_show(ch, current_params);
		return _TRUE;
	}
	if (_strcmp((const char *)argv[0], "avsyncinfo") == 0) {
		if (argc != 1) {
			fcs_reopen_test_usage();
			return _FALSE;
		}
		fcs_avsync_info_show();
		return _TRUE;
	}
	if (_strcmp((const char *)argv[0], "save") == 0) {
		if (argc != 1) {
			fcs_reopen_test_usage();
			return _FALSE;
		}
		if (mmf2_fcs_params_save_current() < 0) {
			RTK_LOGE(TAG, "save current FCS params failed\r\n");
			return _FALSE;
		}
		RTK_LOGI(TAG, "all current FCS channel params saved for the next boot\r\n");
		return _TRUE;
	}

	if (_strcmp((const char *)argv[0], "reopen") != 0 || (argc != 4 && argc != 5)) {
		fcs_reopen_test_usage();
		return _FALSE;
	}

	ch = atoi((const char *)argv[1]);
	width = atoi((const char *)argv[2]);
	height = atoi((const char *)argv[3]);
	bps = (argc == 5) ? atoi((const char *)argv[4]) : -1;
	current_params = fcs_video_params_get(ch);
	if (!current_params) {
		RTK_LOGE(TAG, "invalid FCS channel %d\r\n", ch);
		return _FALSE;
	}
	if (width <= 0 || height <= 0 || (argc == 5 && bps < 0)) {
		RTK_LOGE(TAG, "invalid reopen params: width/height must be positive and bps must be non-negative\r\n");
		return _FALSE;
	}

	memcpy(&new_params, current_params, sizeof(new_params));
	new_params.width = (uint32_t)width;
	new_params.height = (uint32_t)height;
	if (bps >= 0) {
		new_params.bps = (uint32_t)bps;
	}

	RTK_LOGI(TAG, "testing FCS channel %d full close and reopen\r\n", ch);
	fcs_reopen_test_params_show(ch, &new_params);
	if (mmf2_fcs_video_channel_reopen(ch, &new_params) < 0) {
		RTK_LOGE(TAG, "FCS channel %d reopen failed\r\n", ch);
		return _FALSE;
	}
	RTK_LOGI(TAG, "FCS channel %d reopen succeeded\r\n", ch);
	return _TRUE;
}

/*
 * Save the current video_params_t values to PSRAM so they take effect on the
 * next FCS boot.  Call this after adjusting any video_vX_params fields at
 * runtime (e.g. after calling video_set_bitrate() or similar API, mirror the
 * change back into the local video_vX_params and then call this function).
 *
 * This function only persists the parameters used by the next FCS boot.
 */
int mmf2_fcs_params_save_current(void)
{
	fcs_persistent_params_t *p = FCS_PARAMS_ADDR;

	if (fcs_ap_lock_ensure() < 0) {
		return -1;
	}
	rtos_mutex_take(fcs_ap_lock, portMAX_DELAY);
	if (!fcs_params_valid(p)) {
		fcs_params_init_defaults(p);
	}
	p->channel_enable_mask = FCS_CHANNEL_ENABLE_MASK;

	p->ch[0].format  = video_v1_params.format;
	p->ch[0].width   = video_v1_params.width;
	p->ch[0].height  = video_v1_params.height;
	p->ch[0].fps     = video_v1_params.fps;
	p->ch[0].gop     = video_v1_params.gop;
	p->ch[0].bps     = video_v1_params.bps;
	p->ch[0].rc_mode = video_v1_params.rc_mode;
	p->ch[0].min_qp  = video_v1_params.min_qp;
	p->ch[0].max_qp  = video_v1_params.max_qp;
	p->ch[0].qlevel  = video_v1_params.qlevel;
	p->ainr_ena      = video_v1_params.ainr_en;

	p->ch[1].format  = video_v2_params.format;
	p->ch[1].width   = video_v2_params.width;
	p->ch[1].height  = video_v2_params.height;
	p->ch[1].fps     = video_v2_params.fps;
	p->ch[1].gop     = video_v2_params.gop;
	p->ch[1].bps     = video_v2_params.bps;
	p->ch[1].rc_mode = video_v2_params.rc_mode;
	p->ch[1].min_qp  = video_v2_params.min_qp;
	p->ch[1].max_qp  = video_v2_params.max_qp;
	p->ch[1].qlevel  = video_v2_params.qlevel;

	p->ch[2].format  = video_v3_params.format;
	p->ch[2].width   = video_v3_params.width;
	p->ch[2].height  = video_v3_params.height;
	p->ch[2].fps     = video_v3_params.fps;
	p->ch[2].gop     = video_v3_params.gop;
	p->ch[2].bps     = video_v3_params.bps;
	p->ch[2].rc_mode = video_v3_params.rc_mode;
	p->ch[2].min_qp  = video_v3_params.min_qp;
	p->ch[2].max_qp  = video_v3_params.max_qp;
	p->ch[2].qlevel  = video_v3_params.qlevel;

	p->ch[3].format  = VIDEO_NV12;
	p->ch[3].width   = video_v4_params.width;
	p->ch[3].height  = video_v4_params.height;
	p->ch[3].fps     = video_v4_params.fps;
	p->ch[3].gop     = 0;
	p->ch[3].bps     = 0;
	p->ch[3].rc_mode = 0;
	p->ch[3].min_qp  = 0;
	p->ch[3].max_qp  = 0;
	p->ch[3].qlevel  = 0;

	p->ch[4].format  = VIDEO_RGB;
	p->ch[4].width   = video_v5_params.width;
	p->ch[4].height  = video_v5_params.height;
	p->ch[4].fps     = video_v5_params.fps;
	p->ch[4].gop     = 0;
	p->ch[4].bps     = 0;
	p->ch[4].rc_mode = 0;
	p->ch[4].min_qp  = 0;
	p->ch[4].max_qp  = 0;
	p->ch[4].qlevel  = 0;

	p->crc = fcs_crc_compute(p);

	/* PSRAM RET is mapped write-back cacheable on AP; flush so the freshly
	 * written params reach physical PSRAM before sleep and are visible to VP
	 * on the next boot. */
	DCache_Clean((u32)p, sizeof(fcs_persistent_params_t));
	rtos_mutex_give(fcs_ap_lock);
	return 0;
}

/*
 * FCS runtime commands:
 *   FCS show 1                        show current params of a channel
 *   FCS reopen 1 1280 720 300000      close and reopen a channel with new params
 *   FCS save                          persist current params for the next boot
 *   FCS avsyncinfo                    report when each video channel and audio
 *                                     actually started, and their offset
 */
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE fcs_video_reopen_test_cmd[] = {
	{
		"FCS", CmdFcsVideoReopenTest
	},
};

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_fast_camera_start_init[] = {
	{
		.v_func = mmf2_video_example_fast_camera_start_rtsp_init,
		.v_func_name = "mmf2_video_example_fast_camera_start_rtsp_init",
	},
};
