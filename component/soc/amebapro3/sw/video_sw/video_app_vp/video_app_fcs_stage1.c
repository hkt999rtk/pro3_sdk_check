/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

/*
 * video_app_fcs_stage1.c
 *
 * Two-stage fast-camera-start (FCS) ISP initialisation.
 *
 * Stage 1: start ISP HW and begin sensor streaming as early as possible,
 *           before the full stage-2 (BF) firmware has been copied in.
 *           Runs as a FreeRTOS task created at the end of main().
 *
 * Stage 2: once bf_main_task signals fcs_isp_sema, hand the live isp_io
 *           handle over to module_isp via the video_fcs_isp_stage1_get_io() /
 *           video_fcs_isp_stage1_is_started() interface so that the mmfv2 ISP
 *           module can attach without re-init.
 *
 * Semaphore ownership:
 *   video_fcs_isp_stage1_sema_init()  — called once from main()
 *   video_fcs_isp_stage1_sema_give()  — called from bf_main_task() after stage2 data/bss init
 *   video_fcs_isp_stage1_sema_take()  — called inside video_fcs_isp_stage1_task, blocks until
 *                           stage2 is ready, then spawns video_fcs_stream_start_task
 */

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "log.h"

#include "hal_isp.h"
#include "isp_stream.h"

#include "module_isp.h"
#include "video_common_type.h"
#include "ipc_video.h"
#include "video_app_main.h"
#include "video_app_param_config.h"
#include "video_fcs_params.h"
#include "video_app_fcs_stage1.h"

/*
 * Enable first-frame timing measurement via GPIO PE13 and a RTK_LOGS printout.
 * When defined (default), the first frame delivered by module_isp triggers a
 * GPIO low edge and prints the system-time timestamp — useful for boot-time
 * profiling on the bench.  Undefine or set to 0 to strip the measurement code
 * from a production build.
 */

static const char *TAG = "FCS_S1";

/* --------------------------------------------------------------------------
 * ISP semaphore — synchronises stage1 task with bf_main_task
 * -------------------------------------------------------------------------- */
static rtos_sema_t fcs_isp_sema;
static int fcs_isp_sema_valid;

int video_fcs_isp_stage1_sema_init(void)
{
	if (rtos_sema_create(&fcs_isp_sema, 0, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "error: create fcs_isp_sema failed\r\n");
		fcs_isp_sema_valid = 0;
		return -1;
	}
	fcs_isp_sema_valid = 1;
	return 0;
}

void video_fcs_isp_stage1_sema_give(void)
{
	if (fcs_isp_sema_valid) {
		rtos_sema_give(fcs_isp_sema);
	}
}

void video_fcs_isp_stage1_sema_take(void)
{
	if (fcs_isp_sema_valid) {
		rtos_sema_take(fcs_isp_sema, RTOS_MAX_TIMEOUT);
	}
}

/* --------------------------------------------------------------------------
 * Stage1 ISP state — shared with module_isp via weak-override interface
 * -------------------------------------------------------------------------- */
static struct isp_io fcs_isp_io;
static int          fcs_isp_started;
static stream_params_t fcs_strm_param;

/* These three functions override the __weak stubs in module_isp.c so that
 * isp_driver_open() can attach to the already-running stage1 ISP without
 * calling ISP_Init() / isp_initial() a second time. */

int video_fcs_isp_stage1_is_started(void)
{
	return fcs_isp_started;
}

struct isp_io *video_fcs_isp_stage1_get_io(void)
{
	return fcs_isp_started ? &fcs_isp_io : NULL;
}

int video_fcs_isp_stage1_get_stream_params(stream_params_t *param)
{
	if (!param || !fcs_isp_started) {
		return -1;
	}
	memcpy(param, &fcs_strm_param, sizeof(*param));
	return 0;
}

/* --------------------------------------------------------------------------
 * Stage1 ISP frame callback
 * -------------------------------------------------------------------------- */
static int fcs_isp_frame_cb(int stream_id, int buf_id, uint32_t phy_addr)
{
	/* Stage1 uses user-release mode — buffers must be explicitly freed. */
	(void)phy_addr;
	isp_buf_release(stream_id, (void *)NULL, buf_id);
	return 0;
}

/* --------------------------------------------------------------------------
 * video_fcs_isp_stage1_start
 * -------------------------------------------------------------------------- */
int video_fcs_isp_stage1_start(int ch)
{
	if (fcs_isp_started) {
		return 0;
	}

	if (!video_app_fcs_isp_ch_enabled(ch)) {
		RTK_LOGE(TAG, "error: invalid or disabled channel %d\r\n", ch);
		return -1;
	}

	memset(&fcs_strm_param, 0, sizeof(fcs_strm_param));
	if (video_app_fcs_get_isp_params(ch, &fcs_strm_param) != 0) {
		RTK_LOGE(TAG, "error: failed to get isp params for ch%d\r\n", ch);
		return -1;
	}

	ISP_Init();
	extern int rts_camera_init(void);
	int cam_ret = rts_camera_init();
	if (cam_ret) {
		RTK_LOGE(TAG, "error: rts_camera_init fail, ret = %d\r\n", cam_ret);
		return -1;
	}

	if (isp_initial(&fcs_isp_io) != 0) {
		RTK_LOGE(TAG, "error: isp_initial failed\r\n");
		return -1;
	}

	isp_set_user_release();
	isp_ch_handle_register(fcs_isp_frame_cb);

	if (ISP_StreamOnByChannel(&fcs_strm_param) < 0) {
		RTK_LOGE(TAG, "error: ISP_StreamOnByChannel failed\r\n");
		return -1;
	}

	fcs_isp_started = 1;
	RTK_LOGI(TAG, "[FCS S1] ch%d streaming started\r\n", ch);
	return 0;
}

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
/* --------------------------------------------------------------------------
 * video_fcs_isp_4k_stage1_start
 *
 * 4K stage1: same ISP bring-up as the ISP 4K module's own (ISP driver and
 * server; the server probes and early-starts the sensor asynchronously), but
 * run before the BF image is loaded. No stream is started: the module, which
 * lives in the BF image, sets up its streams later and skips this bring-up.
 * -------------------------------------------------------------------------- */
int video_fcs_isp_4k_stage1_start(void)
{
	extern int rtscam_video_init(void);

	if (fcs_isp_started) {
		return 0;
	}
	ISP_Init();
	if (isp_initial(&fcs_isp_io) != 0) {
		RTK_LOGE(TAG, "error: isp_initial failed\r\n");
		return -1;
	}
	if (rtscam_video_init() != 0) {
		RTK_LOGE(TAG, "error: rtscam_video_init failed\r\n");
		return -1;
	}
	fcs_isp_started = 1;
	RTK_LOGI(TAG, "[FCS S1] 4K ISP started\r\n");
	return 0;
}
#endif

/* --------------------------------------------------------------------------
 * video_fcs_isp_stage1_task
 *
 * 1. Start stage1 ISP HW streaming.
 * 2. Wait on fcs_isp_sema (given by bf_main_task when stage2 is loaded).
 * 3. Spawn video_fcs_stream_start_task to complete stage2 mmfv2 pipeline setup.
 *
 * CONFIG_4K_DEV: the ISP 4K module (BF image) sets up its streams itself, so
 * step 1 only brings up the ISP driver and server, sensor included, and step 3
 * runs in this task (8 KB stack); V1, V2 and V5 then start and the module skips
 * its own ISP bring-up. The VP-to-AP links stay deferred until AP attaches
 * each channel.
 * -------------------------------------------------------------------------- */

/* video_fcs_stream_start_task is declared in video_app_fcs_stage1.h and
 * defined in video_app_main.c. */

void video_fcs_isp_stage1_task(void *param)
{
	int ch;
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	if (video_fcs_isp_4k_stage1_start() != 0) {
		for (ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
			if (video_app_fcs_isp_ch_enabled(ch)) {
				video_app_fcs_set_ch_state(ch, VIDEO_CH_ERROR);
			}
		}
		video_app_fcs_set_boot_state(VIDEO_FCS_BOOT_ERROR);
		rtos_task_delete(NULL);
		return;
	}

	video_fcs_isp_stage1_sema_take();
	/* Deletes this task when done. */
	video_fcs_stream_start_task(param);
#else
	int start_ch;
	(void)param;

	start_ch = video_app_fcs_get_start_ch();

	if (video_fcs_isp_stage1_start(start_ch) != 0) {
		RTK_LOGE(TAG, "error: stage1 start failed on ch%d\r\n", start_ch);
		video_app_fcs_set_ch_state(start_ch, VIDEO_CH_ERROR);
		video_app_fcs_set_boot_state(VIDEO_FCS_BOOT_ERROR);
		vTaskDelete(NULL);
		return;
	}

	/* Block until bf_main_task signals that stage2 data/bss is initialised. */
	video_fcs_isp_stage1_sema_take();

	/* Hand off to stage2 pipeline setup task. */
	if (xTaskCreate(video_fcs_stream_start_task, "video_fcs_start",
					2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		RTK_LOGE(TAG, "error: fail to create video_fcs_start task\r\n");
		for (ch = 0; ch < VIDEO_ISP_VALID_CH; ch++) {
			if (video_app_fcs_isp_ch_enabled(ch)) {
				video_app_fcs_set_ch_state(ch, VIDEO_CH_ERROR);
			}
		}
		video_app_fcs_set_boot_state(VIDEO_FCS_BOOT_ERROR);
	}

	vTaskDelete(NULL);
#endif
}
