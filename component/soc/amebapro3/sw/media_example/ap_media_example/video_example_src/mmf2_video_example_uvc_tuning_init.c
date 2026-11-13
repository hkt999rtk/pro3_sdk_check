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
#include "module_video.h"
#include "module_uvcd.h"
#include "video_api.h"
#include "video_common_type.h"

static const char *TAG = "AP_VAPP";

#if defined(CONFIG_USBD_UVC) && (CONFIG_USBD_UVC != 0)

#include "usbd.h"
#include "usbd_uvc.h"
#include "usb_os.h"

/*
 * AP-core UVC tuning example: a single V1 video_module channel whose
 * format is switched at runtime (NV16/NV12/H264/H265/MJPEG) to follow
 * whatever format the UVC host selects, mirroring
 * vp_media_example/mmf2_video_example_uvcd_video_tuning.c.
 *
 * VP's reference reconfigures the ISP to ISP_FORMAT_YUV422_SEMIPLANAR for
 * a host-selected UVC YUY2 request - that is 4:2:2 semi-planar, i.e. what
 * AP core's video_params_t.format calls VIDEO_NV16 (see the isp_fmt
 * mapping in module_video.c:video_cmd_gen()), not packed YUY2. This
 * example follows that same mapping for consistency with VP.
 *
 * All formats reuse V1_STREAM_ID: switching format stops and recreates the
 * V1 video_module channel with new video_params_t rather than trying to
 * reconfigure a running channel in place, matching the pattern already
 * proven in mmf2_video_example_v1_param_change_init.c.
 */

#define UVC_TUNING_TASK_STACK_SIZE   (1024 * 2)
#define UVC_TUNING_TASK_PRIORITY     2

/* Gap between closing the old V1 channel and re-opening it with a new
 * format. CMD_VIDEO_STREAM_STOP's 50ms wait (module_video.c) is only for
 * the frame-recycle IPC handshake with VP; VP's own async stop task
 * (video_app_stream_stop() -> video_channel_deinit() in video_app_main.c)
 * can still be tearing the channel down after that. A back-to-back
 * CMD_VIDEO_APPLY reinitializing the same channel races that in-flight
 * VP-side teardown and can tear the just-reopened channel back down, so
 * this settle delay is inserted between stop and reopen. */
#define UVC_TUNING_SETTLE_MS         500

/* Sentinel: no V1 channel has been opened for any UVC format yet. */
#define UVC_TUNING_FORMAT_NONE       0xFFFFFFFFU

static mm_context_t *video_v1_ctx = NULL;
static mm_context_t *uvcd_ctx     = NULL;
static mm_siso_t *siso_v1_uvc     = NULL;
static rtos_task_t uvc_tuning_task;
static uint32_t current_uvc_format = UVC_TUNING_FORMAT_NONE;
static uint32_t current_uvc_width  = 0;
static uint32_t current_uvc_height = 0;

static video_params_t video_v1_params = {
	.stream_id = V1_STREAM_ID,
	.format = VIDEO_H264,
	.width = USBD_UVC_FRAME_WIDTH,
	.height = USBD_UVC_FRAME_HEIGHT,
	.fps = 30,
	.gop = 30,
	.bps = 2000000,
	.rc_mode = ENC_VBR,
	.min_qp = 26,
	.max_qp = 48,
};

/* Fill video_v1_params for the UVC format/resolution the host just
 * negotiated. video_v1_params is reused across every switch, so start
 * from a clean slate each time instead of leaving stale fields from the
 * previous format (e.g. qlevel from MJPEG bleeding into a later H264
 * config). Width/height come from the host-negotiated
 * uvc_format_ptr->width/height rather than the USBD_UVC_*_FRAME_*
 * macros: each format currently advertises only one frame size in
 * usbd_uvc_descriptor.c, which happens to equal those macros, but if a
 * format ever advertises more than one size the host's actual pick must
 * still be honored.
 * Returns 0 if the format is supported, -1 otherwise. */
static int uvc_tuning_fill_params(usbd_uvc_format_t *uvc_format_ptr)
{
	memset(&video_v1_params, 0, sizeof(video_v1_params));
	video_v1_params.stream_id = V1_STREAM_ID;
	video_v1_params.width = uvc_format_ptr->width;
	video_v1_params.height = uvc_format_ptr->height;

	switch (uvc_format_ptr->format) {
	case USBD_UVC_FORMAT_TYPE_H264:
		video_v1_params.format = VIDEO_H264;
		video_v1_params.fps = 30;
		video_v1_params.gop = 30;
		video_v1_params.bps = 2000000;
		video_v1_params.rc_mode = ENC_VBR;
		video_v1_params.min_qp = 26;
		video_v1_params.max_qp = 48;
		break;
	case USBD_UVC_FORMAT_TYPE_H265:
		video_v1_params.format = VIDEO_HEVC;
		video_v1_params.fps = 30;
		video_v1_params.gop = 30;
		video_v1_params.bps = 2000000;
		video_v1_params.rc_mode = ENC_VBR;
		video_v1_params.min_qp = 26;
		video_v1_params.max_qp = 48;
		break;
	case USBD_UVC_FORMAT_TYPE_MJPEG:
		video_v1_params.format = VIDEO_JPEG;
		video_v1_params.fps = 10;
		video_v1_params.qlevel = 5;
		break;
	case USBD_UVC_FORMAT_TYPE_NV12:
		video_v1_params.format = VIDEO_NV12;
		video_v1_params.fps = 10;
		break;
	case USBD_UVC_FORMAT_TYPE_YUY2:
		/* AP-side equivalent of VP's ISP_FORMAT_YUV422_SEMIPLANAR, see the
		 * file header comment. */
		video_v1_params.format = VIDEO_NV16;
		video_v1_params.fps = 10;
		break;
	default:
		RTK_LOGW(TAG, "Unknown UVC format %d, keep current\r\n", (int)uvc_format_ptr->format);
		return -1;
	}
	return 0;
}

static int uvc_tuning_video_start(void)
{
	video_v1_ctx = mm_module_open(&video_module);
	if (!video_v1_ctx) {
		RTK_LOGE(TAG, "video v1 open fail\r\n");
		return -1;
	}
	mm_module_ctrl(video_v1_ctx, CMD_VIDEO_SET_PARAMS, (int)&video_v1_params);
	mm_module_ctrl(video_v1_ctx, MM_CMD_SET_QUEUE_LEN, 6);
	mm_module_ctrl(video_v1_ctx, MM_CMD_INIT_QUEUE_ITEMS, MMQI_FLAG_DYNAMIC);
	if (mm_module_ctrl(video_v1_ctx, CMD_VIDEO_APPLY, 0) != 0) {
		RTK_LOGE(TAG, "video v1 apply fail\r\n");
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
		return -1;
	}

	siso_v1_uvc = siso_create();
	if (!siso_v1_uvc) {
		RTK_LOGE(TAG, "siso_v1_uvc open fail\r\n");
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
		return -1;
	}
	siso_ctrl(siso_v1_uvc, MMIC_CMD_ADD_INPUT, (uint32_t)video_v1_ctx, 0);
	siso_ctrl(siso_v1_uvc, MMIC_CMD_ADD_OUTPUT, (uint32_t)uvcd_ctx, 0);
	if (siso_start(siso_v1_uvc) != 0) {
		RTK_LOGE(TAG, "siso_v1_uvc start fail\r\n");
		siso_delete(siso_v1_uvc);
		siso_v1_uvc = NULL;
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
		return -1;
	}

	RTK_LOGI(TAG, "video v1 -> uvc started, format=%d %dx%d\r\n",
			 (int)video_v1_params.format, (int)video_v1_params.width, (int)video_v1_params.height);
	return 0;
}

static void uvc_tuning_video_stop(void)
{
	if (video_v1_ctx) {
		mm_module_ctrl(video_v1_ctx, CMD_VIDEO_STREAM_STOP, 0);
	}
	if (siso_v1_uvc) {
		siso_stop(siso_v1_uvc);
		siso_delete(siso_v1_uvc);
		siso_v1_uvc = NULL;
	}
	if (video_v1_ctx) {
		mm_module_close(video_v1_ctx);
		video_v1_ctx = NULL;
	}
}

/* Blocks on the UVC format-change semaphore (given by module_uvcd.c's
 * uvcd_change_format_resolution() whenever the host commits a new UVC
 * streaming format) and restarts the V1 channel with matching params. */
static void uvc_tuning_task_entry(void *param)
{
	(void)param;
	usbd_uvc_format_t *uvc_format_ptr = usbd_uvc_get_format();

	while (1) {
		usb_os_sema_take(uvc_format_ptr->uvcd_change_sema, RTOS_MAX_DELAY);
		RTK_LOGI(TAG, "UVC host requested format=%d %dx%d\r\n",
				 (int)uvc_format_ptr->format, (int)uvc_format_ptr->width, (int)uvc_format_ptr->height);

		if (uvc_format_ptr->format == current_uvc_format &&
			uvc_format_ptr->width == current_uvc_width &&
			uvc_format_ptr->height == current_uvc_height) {
			RTK_LOGI(TAG, "UVC format unchanged, skip restart\r\n");
			continue;
		}
		if (uvc_tuning_fill_params(uvc_format_ptr) != 0) {
			continue;
		}
		if (video_v1_ctx) {
			uvc_tuning_video_stop();
			rtos_time_delay_ms(UVC_TUNING_SETTLE_MS);
		}
		if (uvc_tuning_video_start() == 0) {
			current_uvc_format = uvc_format_ptr->format;
			current_uvc_width  = uvc_format_ptr->width;
			current_uvc_height = uvc_format_ptr->height;
		} else {
			RTK_LOGE(TAG, "failed to restart video v1 for new UVC format\r\n");
			/* video_v1_ctx is NULL again (uvc_tuning_video_start() cleans up
			 * on every failure path), so nothing matches the stale cached
			 * format/resolution anymore - reset it to keep the cache from
			 * claiming a channel that doesn't exist. */
			current_uvc_format = UVC_TUNING_FORMAT_NONE;
			current_uvc_width  = 0;
			current_uvc_height = 0;
		}
	}
}

void mmf2_video_example_uvc_tuning_init(void)
{
	uvcd_ctx = mm_module_open(&uvcd_module);
	if (!uvcd_ctx) {
		RTK_LOGE(TAG, "uvc open fail\r\n");
		return;
	}
	RTK_LOGI(TAG, "UVC module opened\r\n");

	/* Do not pre-open a default V1 channel here: the first UVC
	 * format-change event (fired on the host's first STREAMON) creates
	 * it via uvc_tuning_task_entry(). Opening one here would just have
	 * that first event immediately tear it down and reopen it for the
	 * host's negotiated format - the exact close-then-reopen race
	 * UVC_TUNING_SETTLE_MS above exists to avoid. */

	if (rtos_task_create(&uvc_tuning_task, "uvc_tuning_task", uvc_tuning_task_entry, NULL,
						 UVC_TUNING_TASK_STACK_SIZE, UVC_TUNING_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "create uvc_tuning_task fail\r\n");
	}
}

#else /* !CONFIG_USBD_UVC */

void mmf2_video_example_uvc_tuning_init(void)
{
	RTK_LOGE(TAG, "UVC not supported: CONFIG_USBD_UVC not enabled\r\n");
}

#endif /* CONFIG_USBD_UVC */

MEDIA_APP_TABLE_SECTION
const VIDEO_APP_TABLE vapp_mmf2_video_uvc_tuning_init[] = {
	{
		.v_func = mmf2_video_example_uvc_tuning_init,
		.v_func_name = "mmf2_video_example_uvc_tuning_init",
	}
};
