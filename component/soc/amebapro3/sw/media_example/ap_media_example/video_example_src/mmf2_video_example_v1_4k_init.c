/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ameba_soc.h"
#include "mmf2_link.h"
#include "mmf2_siso.h"
#include "module_video.h"
#include "module_rtsp2.h"
#include "module_uvcd.h"
#include "module_mp4.h"
#include "module_filesaver.h"
#include "mmf2_mediatime_pro3.h"
#include "avcodec.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

/*
 * Example topology:
 *   V1 H.264 video -> RTSP/UVC
 *
 * Use this as the smallest live-view example. Change video_v1_params for the
 * camera stream and rtsp2_v_params when the RTSP metadata must match it.
 *
 * UVC requires CONFIG_USBD_UVC and a matching USB descriptor before use.
 * The shared defaults advertise 1920x1080 at 30/15/10 fps; this example does
 * not update them automatically. For this 2160x3840 H.264 stream at 20 fps:
 * - Update USBD_UVC_FRAME_WIDTH/HEIGHT in component/usb/device/uvc/usbd_video.h
 * - Update the H.264 frame descriptor, frame info table and probe/commit
 *   defaults in component/usb/device/uvc/usbd_uvc_descriptor.c to match;
 *   20 fps uses a frame interval of 500000 in 100 ns units
 * These shared settings also affect other UVC examples.
 */

#if (!defined(CONFIG_LWIP_LAYER) || (CONFIG_LWIP_LAYER == 0))
#error "please enable CONFIG_LWIP_LAYER before using RTSP"
#endif

/* AI Denoise Option */
#define AINR_ENA    0   /* AI noise reduction on V1; set 1 to enable */

/* ---- Internal constants (do not modify) ---------------------------------- */
#define V1_SINK_RTSP    0
#define V1_SINK_UVC     1

static mm_context_t *video_v1_ctx  = NULL;
static mm_context_t *rtsp_ctx      = NULL;
static mm_context_t *uvcd_ctx      = NULL;
static mm_siso_t    *siso_v1_sink  = NULL;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format    = VIDEO_H264,
	.width     = 2160,
	.height    = 3840,
	.fps       = 20,
	.gop       = 20,
	.bps       = 2000000,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
	.ainr_en   = AINR_ENA,
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

static void mmf2_video_example_v1_4k_init(int sink_mode)
{
	if (video_v1_ctx) {
		RTK_LOGW(TAG, "V1 4K example already initialized\n\r");
		return;
	}
#if !defined(CONFIG_USBD_UVC) || (CONFIG_USBD_UVC == 0)
	/* Reject an unavailable sink before starting the video hardware. */
	if (sink_mode == V1_SINK_UVC) {
		RTK_LOGE(TAG, "UVC sink requires CONFIG_USBD_UVC\n\r");
		return;
	}
#endif
	/* V1 ********************************************************/
	/* video_module owns one hardware video stream. Set stream params and
	 * queue policy first; CMD_VIDEO_APPLY starts frame production. */
	video_v1_ctx = mm_module_open(&video_module);
	if (video_v1_ctx) {
		/* Stop at the first failed step; do not apply a partially configured stream. */
		if (mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params) < 0 ||
			mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 60) < 0 ||
			mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0 ||
			mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0) < 0) {
			RTK_LOGE(TAG, "video v1 setup fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "video v1 open fail\n\r");
		goto error;
	}

	RTK_LOGI(TAG, "sink mode: %s\n\r", sink_mode == V1_SINK_UVC ? "uvc" : "rtsp");
	if (sink_mode == V1_SINK_UVC) {
#if defined(CONFIG_USBD_UVC) && (CONFIG_USBD_UVC != 0)
		/* UVC ******************************************************/
		uvcd_ctx = mm_module_open(&uvcd_module);
		if (!uvcd_ctx) {
			RTK_LOGE(TAG, "uvc open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "UVC module opened\n\r");
#endif
	} else {
		/* RTSP ******************************************************/
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
			RTK_LOGE(TAG, "RTSP2 open fail\n\r");
			goto error;
		}
		RTK_LOGI(TAG, "RTSP module opened\n\r");
	}

	/* SISO V1 -> RTSP/UVC *******************************************/
	siso_v1_sink = siso_create();
	if (siso_v1_sink) {
		siso_ctrl(siso_v1_sink, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
		if (sink_mode == V1_SINK_UVC) {
			siso_ctrl(siso_v1_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
		} else {
			siso_ctrl(siso_v1_sink, MMIC_CMD_ADD_OUTPUT, (uint32_t)rtsp_ctx, 0);
		}
		if (siso_start(siso_v1_sink) < 0) {
			RTK_LOGE(TAG, "siso_v1_sink start fail\n\r");
			goto error;
		}
	} else {
		RTK_LOGE(TAG, "siso_v1_sink create fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "siso_v1_sink started\n\r");

	return;
error:
	/* Detach the linker before freeing either endpoint. */
	if (siso_v1_sink) {
		siso_v1_sink = siso_delete(siso_v1_sink);
	}
	/* Close RTSP before video so PLAY callbacks cannot use a freed context. */
	if (rtsp_ctx) {
		mm_module_ctrl(rtsp_ctx, CMD_RTSP2_SET_STREAMMING, OFF);
		rtsp_ctx = mm_module_close(rtsp_ctx);
	}
	if (uvcd_ctx) {
		uvcd_ctx = mm_module_close(uvcd_ctx);
	}
	if (video_v1_ctx) {
		/* video_destroy() alone does not stop VP or unregister frame callbacks. */
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
		video_v1_ctx = mm_module_close(video_v1_ctx);
	}
	return;
}

void mmf2_video_example_v1_4k_rtsp_init(void)
{
	mmf2_video_example_v1_4k_init(V1_SINK_RTSP);
}

void mmf2_video_example_v1_4k_uvc_init(void)
{
	mmf2_video_example_v1_4k_init(V1_SINK_UVC);
}

/*
 * Multi-channel example on the same 4K ISP pass:
 *   V1 H.264 2160x3840 video -> MP4 recording
 *   V2 1440x2560 video -> H.264 to UVC + JPEG on demand (VAPP_4K_SNAPSHOT [path])
 *   V5 planar RGB 576x320 tap -> filesaver (frame counter)
 *
 * Channels start in that order. The first APPLY starts the 4K source and a
 * later channel joins a running source, so only V1 gets the very first frame;
 * MP4 is on V1 and is linked before V1 is applied so the recording starts
 * from that frame.
 *
 * V1, V2 and V5 are parallel outputs of one ISP pass. V2 is downscaled
 * by ISP stream 2, so its size must match ISP_4K_SUB_OUTPUT_WIDTH/HEIGHT on VP
 * (1440x2560). V5 is the same pass's zoom AI channel, sized by
 * ISP_4K_RGB_WIDTH/HEIGHT on VP (576x320).
 *
 * Every source uses its own SISO. Do not combine V1 and V2 into one MIMO:
 * mmf2_mimo.c only releases a source item when the *output* module's
 * output_type is not MM_TYPE_NONE, and uvcd / mp4 / filesaver are all
 * MM_TYPE_NONE. With a MIMO the VP's vp2ap then waits out its full 1 s release
 * timeout on every frame and all channels drop to ~1 fps. SISO releases from
 * its input module instead, which module_video does support.
 */
/* V5 planar RGB tap, matches ISP_4K_RGB_WIDTH/HEIGHT on VP. */
#define V5_RGB_WIDTH  576
#define V5_RGB_HEIGHT 320

static int v2_snapshot_ready;
static mm_context_t *video_v2_ctx  = NULL;
static mm_context_t *video_v5_ctx  = NULL;
static mm_context_t *mp4_ctx       = NULL;
static mm_context_t *fs_v5_ctx     = NULL;
static mm_siso_t    *siso_v2_uvc   = NULL;
static mm_siso_t    *siso_v5_fs    = NULL;

static video_params_t video_v2_params = {
	.stream_id = V2_STREAM_ID,
	.format    = VIDEO_H264_JPEG,
	.width     = 1440,
	.height    = 2560,
	.fps       = 20,
	.gop       = 20,
	.bps       = 2000000,
	.rc_mode   = ENC_VBR,
	.min_qp    = 26,
	.max_qp    = 48,
	.qlevel = 5,
};

/* Planar RGB tap for a filesaver / NN sink. Size must match
 * ISP_4K_RGB_WIDTH/HEIGHT on VP; the AI channel adds no ISP pass of its own.
 * fps is not a throttle here: RGB follows the shared ISP pass rate.
 */
static video_params_t video_v5_params = {
	.stream_id = V5_STREAM_ID,
	.format    = VIDEO_RGB,
	.width     = V5_RGB_WIDTH,
	.height    = V5_RGB_HEIGHT,
	.fps       = 5,
};

static mp4_params_t mp4_params = {
	.record_length    = 10,
	.record_type      = STORAGE_VIDEO,
	.record_file_num  = 3,
	.record_file_name = "AmebaPro_4k",
	.fatfs_buf_size   = 224 * 1024, /* 32kb multiple */
};

/* Filesaver has no user context argument, so the counters are file-scope.
 * RGB frames are zero-copy ISP buffers: only count them here, never keep the
 * address after the callback returns. No SD I/O either -- this runs on the
 * siso pumper task, so a slow write would stall the linker and back up the
 * whole ISP pass, including the concurrent V1/V2 encoders and MP4 recording.
 */
static uint32_t v5_rgb_frames;
static uint32_t v5_rgb_last_ms;
static uint32_t v5_rgb_fps;

static void v5_rgb_saver(char *name, uint32_t addr, uint32_t size)
{
	uint32_t now = mm_read_mediatime_ms();
	uint32_t elapsed;

	(void)name;
	(void)addr;
	(void)size;

	v5_rgb_frames++;
	if (v5_rgb_last_ms == 0) {
		v5_rgb_last_ms = now;
		return;
	}
	elapsed = now - v5_rgb_last_ms;
	if (elapsed >= 5000) {
		uint32_t fps = v5_rgb_frames * 1000 / elapsed;

		if (fps != v5_rgb_fps) {
			RTK_LOGI(TAG, "V5 RGB %ux%u: FPS=%u\r\n", V5_RGB_WIDTH, V5_RGB_HEIGHT, (unsigned)fps);
			v5_rgb_fps = fps;
		}
		v5_rgb_frames = 0;
		v5_rgb_last_ms = now;
	}
}

static mm_siso_t *prv_siso_start(mm_context_t *in, mm_context_t *out)
{
	mm_siso_t *siso = siso_create();

	if (!siso) {
		return NULL;
	}
	siso_ctrl(siso, MMIC_CMD_ADD_INPUT, (uint32_t)in, 0);
	siso_ctrl(siso, MMIC_CMD_ADD_OUTPUT, (uint32_t)out, 0);
	if (siso_start(siso) < 0) {
		siso_delete(siso);
		return NULL;
	}
	return siso;
}

/* Set stream params and queue policy, link the channel to its sink, then
 * CMD_VIDEO_APPLY starts frame production, so the sink sees the channel's
 * first frame. Stop at the first failed step; do not apply a partially
 * configured stream. On failure nothing is left open and *siso is NULL. */
static mm_context_t *prv_video_start(video_params_t *params, mm_context_t *sink, mm_siso_t **siso)
{
	mm_context_t *ctx = mm_module_open(&video_module);

	*siso = NULL;
	if (!ctx) {
		RTK_LOGE(TAG, "video ch%u open fail\n\r", (unsigned)params->stream_id);
		return NULL;
	}
	if (mm_module_ctrl(ctx, CMD_VIDEO_SET_PARAMS, (int)params) < 0 ||
		mm_module_ctrl(ctx, MM_CMD_SET_QUEUE_LEN, 60) < 0 ||
		mm_module_ctrl(ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC) < 0) {
		RTK_LOGE(TAG, "video ch%u setup fail\n\r", (unsigned)params->stream_id);
		mm_module_close(ctx);
		return NULL;
	}
	*siso = prv_siso_start(ctx, sink);
	if (!*siso) {
		RTK_LOGE(TAG, "video ch%u siso start fail\n\r", (unsigned)params->stream_id);
		mm_module_close(ctx);
		return NULL;
	}
	if (mm_module_ctrl(ctx, CMD_VIDEO_APPLY, 0) < 0) {
		RTK_LOGE(TAG, "video ch%u apply fail\n\r", (unsigned)params->stream_id);
		/* Detach the linker first; video_destroy() alone does not stop VP or
		 * unregister frame callbacks. */
		*siso = siso_delete(*siso);
		mm_module_ctrl(ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(ctx);
		return NULL;
	}
	return ctx;
}

/* Example topology: V1 H.264 2160x3840 video -> MP4 recording only. */
void mmf2_video_example_v1_4k_mp4_init(void)
{
	if (video_v1_ctx || video_v2_ctx || video_v5_ctx) {
		RTK_LOGW(TAG, "4K example already initialized\n\r");
		return;
	}

	/* Match MP4 metadata to the user-facing V1 recording stream. */
	mp4_params.fps    = video_v1_params.fps;
	mp4_params.gop    = video_v1_params.gop;
	mp4_params.width  = video_v1_params.width;
	mp4_params.height = video_v1_params.height;
	mp4_ctx = mm_module_open(&mp4_module);
	if (!mp4_ctx) {
		RTK_LOGE(TAG, "mp4 open fail\n\r");
		return;
	}
	mm_module_ctrl(mp4_ctx, CMD_MP4_SET_PARAMS, (int)&mp4_params);
	/* Set the 4K display orientation before START writes the file header. */
	mm_module_ctrl(mp4_ctx, CMD_MP4_SET_ROTATION, MP4_ROTATION_270);
	mm_module_ctrl(mp4_ctx, CMD_MP4_LOOP_MODE, 0);
	mm_module_ctrl(mp4_ctx, CMD_MP4_START, mp4_params.record_file_num);

	/* SISO links V1 to MP4 before APPLY so recording receives the first frame. */
	video_v1_ctx = prv_video_start(&video_v1_params, mp4_ctx, &siso_v1_sink);
	if (!video_v1_ctx) {
		/* prv_video_start has already detached SISO and closed video on failure. */
		mm_module_ctrl(mp4_ctx, CMD_MP4_STOP_IMMEDIATELY, 0);
		mp4_ctx = mm_module_close(mp4_ctx);
		return;
	}
	RTK_LOGI(TAG, "V1->MP4 started\n\r");
}

static void prv_4k_mp4_uvc_init(int enable_rgb)
{
#if !defined(CONFIG_USBD_UVC) || (CONFIG_USBD_UVC == 0)
	(void)enable_rgb;
	/* Reject an unavailable sink before starting the video hardware. */
	RTK_LOGE(TAG, "UVC sink requires CONFIG_USBD_UVC\n\r");
#else
	if (video_v1_ctx || video_v2_ctx || video_v5_ctx) {
		RTK_LOGW(TAG, "4K example already initialized\n\r");
		return;
	}
	RTK_LOGI(TAG, "sink mode: %s\n\r", enable_rgb ? "mp4_uvc_rgb" : "mp4_uvc");

	/* MP4 *******************************************************/
	mp4_params.fps    = video_v1_params.fps;
	mp4_params.gop    = video_v1_params.gop;
	mp4_params.width  = video_v1_params.width;
	mp4_params.height = video_v1_params.height;
	mp4_ctx = mm_module_open(&mp4_module);
	if (!mp4_ctx) {
		RTK_LOGE(TAG, "mp4 open fail\n\r");
		goto error;
	}
	mm_module_ctrl(mp4_ctx, CMD_MP4_SET_PARAMS, (int)&mp4_params);
	/* The 4K pipeline hands over a portrait frame whose content is still
	 * lying on its side: the PPU rotation and the final display orientation
	 * differ by 90 degrees counter-clockwise. Record that in the tkhd matrix
	 * so players turn it; the bitstream itself is left untouched. Must be set
	 * before CMD_MP4_START, which writes the header when it opens the first file. */
	mm_module_ctrl(mp4_ctx, CMD_MP4_SET_ROTATION, MP4_ROTATION_270);
	mm_module_ctrl(mp4_ctx, CMD_MP4_LOOP_MODE, 0);
	mm_module_ctrl(mp4_ctx, CMD_MP4_START, mp4_params.record_file_num);
	RTK_LOGI(TAG, "MP4 module opened\n\r");

	/* V1 -> MP4 *************************************************/
	video_v1_ctx = prv_video_start(&video_v1_params, mp4_ctx, &siso_v1_sink);
	if (!video_v1_ctx) {
		goto error;
	}

	/* UVC *******************************************************/
	uvcd_ctx = mm_module_open(&uvcd_module);
	if (!uvcd_ctx) {
		RTK_LOGE(TAG, "uvc open fail\n\r");
		goto error;
	}
	RTK_LOGI(TAG, "UVC module opened\n\r");

	/* V2 -> UVC *************************************************/
	video_v2_ctx = prv_video_start(&video_v2_params, uvcd_ctx, &siso_v2_uvc);
	if (!video_v2_ctx) {
		goto error;
	}

	/* The two-channel example stops here; only the three-channel entry opens V5. */
	if (!enable_rgb) {
		v2_snapshot_ready = 1;
		RTK_LOGI(TAG, "V1->MP4, V2->UVC started\n\r");
		return;
	}

	/* FILESAVER *************************************************/
	fs_v5_ctx = mm_module_open(&filesaver_module);
	if (!fs_v5_ctx) {
		RTK_LOGE(TAG, "fs_v5 open fail\n\r");
		goto error;
	}
	mm_module_ctrl(fs_v5_ctx, CMD_FILESAVER_SET_TYPE_HANDLER, (int)v5_rgb_saver);
	mm_module_ctrl(fs_v5_ctx, CMD_FILESAVER_APPLY, 0);

	/* V5 -> FILESAVER *******************************************/
	/* Planar RGB tap on the same 4K ISP pass. The VP side binds it to
	 * ISP_4K_RGB_STREAM_IDX, so size and format must match there. */
	video_v5_ctx = prv_video_start(&video_v5_params, fs_v5_ctx, &siso_v5_fs);
	if (!video_v5_ctx) {
		goto error;
	}
	v2_snapshot_ready = 1;
	RTK_LOGI(TAG, "V1->MP4, V2->UVC, V5 RGB %ux%u->filesaver started\n\r", V5_RGB_WIDTH, V5_RGB_HEIGHT);
	return;

error:
	/* Detach the linkers before freeing either endpoint. */
	if (siso_v1_sink) {
		siso_v1_sink = siso_delete(siso_v1_sink);
	}
	if (siso_v2_uvc) {
		siso_v2_uvc = siso_delete(siso_v2_uvc);
	}
	if (siso_v5_fs) {
		siso_v5_fs = siso_delete(siso_v5_fs);
	}
	if (uvcd_ctx) {
		uvcd_ctx = mm_module_close(uvcd_ctx);
	}
	if (mp4_ctx) {
		mm_module_ctrl(mp4_ctx, CMD_MP4_STOP_IMMEDIATELY, 0);
		mp4_ctx = mm_module_close(mp4_ctx);
	}
	if (fs_v5_ctx) {
		fs_v5_ctx = mm_module_close(fs_v5_ctx);
	}
	/* video_destroy() alone does not stop VP or unregister frame callbacks. */
	if (video_v5_ctx) {
		mm_module_ctrl(video_v5_ctx, CMD_VIDEO_STREAM_STOP, 0);
		video_v5_ctx = mm_module_close(video_v5_ctx);
	}
	if (video_v2_ctx) {
		mm_module_ctrl(video_v2_ctx, CMD_VIDEO_STREAM_STOP, 0);
		video_v2_ctx = mm_module_close(video_v2_ctx);
	}
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
		video_v1_ctx = mm_module_close(video_v1_ctx);
	}
#endif
}

/* Example topology: V1 H.264 2160x3840 -> MP4, V2 H.264 1440x2560 -> UVC. */
void mmf2_video_example_v1v2_4k_mp4_uvc_init(void)
{
	prv_4k_mp4_uvc_init(0);
}

/* Example topology: V1 -> MP4, V2 -> UVC, V5 planar RGB -> filesaver. */
void mmf2_video_example_v1v2v5_4k_mp4_uvc_rgb_init(void)
{
	prv_4k_mp4_uvc_init(1);
}

/* Optional path is on an already mounted filesystem. With no path the
 * command just captures and reports the encoded size. */
static u32 cmd_v2_snapshot(u16 argc, u8 *argv[])
{
	if (!v2_snapshot_ready || argc > 1) {
		RTK_LOGE(TAG, "Start mmf2_video_example_v1v2_4k_mp4_uvc_init or mmf2_video_example_v1v2v5_4k_mp4_uvc_rgb_init using VIDEO list and VIDEO run <index>.\r\n"
				 "  VAPP_4K_SNAPSHOT                 Capture one JPEG and print its size.\r\n"
				 "  VAPP_4K_SNAPSHOT sd:/v2.jpg  Save one JPEG to a new file on the mounted SD card.\r\n");
		return FALSE;
	}
	uint32_t capacity = video_v2_params.width * video_v2_params.height * 0.6;
	video_snapshot_t shot = {.dst = malloc(capacity), .capacity = capacity};
	if (!shot.dst) {
		RTK_LOGE(TAG, "Cannot allocate snapshot destination\r\n");
		return FALSE;
	}
	int ret = mm_module_ctrl(video_v2_ctx, CMD_VIDEO_GET_SNAPSHOT, (int)&shot);
	if (ret == VIDEO_SNAPSHOT_NOSPACE) {
		RTK_LOGE(TAG, "Snapshot needs %lu bytes\r\n", (unsigned long)shot.jpeg_len);
	} else if (ret != 0) {
		RTK_LOGE(TAG, "Snapshot failed (%d)\r\n", ret);
	} else {
		RTK_LOGI(TAG, "Captured %ux%u JPEG, %lu bytes\r\n", (unsigned)video_v2_params.width, (unsigned)video_v2_params.height, (unsigned long)shot.jpeg_len);
		if (argc == 1) {
			FILE *file = fopen((const char *)argv[0], "wx");
			if (!file) {
				ret = -1;
			} else {
				size_t written = fwrite(shot.dst, 1, shot.jpeg_len, file);
				if (fclose(file) != 0 || written != shot.jpeg_len) {
					ret = -1;
				}
			}
			if (ret != 0) {
				RTK_LOGE(TAG, "Could not save JPEG to %s\r\n", argv[0]);
			}
		}
	}
	free(shot.dst);
	return ret == 0 ? TRUE : FALSE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_4k_snapshot_cmd[] = {
	{"VAPP_4K_SNAPSHOT", cmd_v2_snapshot},
};

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_example_v1_4k_init[] = {
	{
		.v_func      = mmf2_video_example_v1_4k_rtsp_init,
		.v_func_name = "mmf2_video_example_v1_4k_rtsp_init",
	},
	{
		.v_func      = mmf2_video_example_v1_4k_uvc_init,
		.v_func_name = "mmf2_video_example_v1_4k_uvc_init",
	},
	{
		.v_func      = mmf2_video_example_v1_4k_mp4_init,
		.v_func_name = "mmf2_video_example_v1_4k_mp4_init",
	},
	{
		.v_func      = mmf2_video_example_v1v2_4k_mp4_uvc_init,
		.v_func_name = "mmf2_video_example_v1v2_4k_mp4_uvc_init",
	},
	{
		.v_func      = mmf2_video_example_v1v2v5_4k_mp4_uvc_rgb_init,
		.v_func_name = "mmf2_video_example_v1v2v5_4k_mp4_uvc_rgb_init",
	}
};
