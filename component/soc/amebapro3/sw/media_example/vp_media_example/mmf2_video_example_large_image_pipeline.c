/*
 * Realtek Semiconductor Corp.
 *
 * Large-image production orchestrator: application-layer A->B->C->D
 * pipeline built entirely on existing, stable driver primitives already
 * validated by the large-image-mode, isp-input-stream, and
 * large-image-mode-tnr-test capabilities:
 *
 *   A  capture RAW           rtscam_video_* (stream 0)
 *   B  fold RAW (GE)         ge_raw_reshape()
 *   C  ISP verify/inject     rtscam_isp_input_*
 *   D  unfold NV12 (GE)      ge_yuv_reshape()
 *
 * See openspec/changes/2026-08-25-add-large-image-pipeline/{proposal,
 * design}.md for the rationale. This file implements the single
 * orchestration task that owns the one GE context and drives B and D in
 * statement order (structural GE mutual exclusion), the zero-copy B->C
 * (RTS_MEMORY_USERPTR) and C_out==D_in hold-until-D handoffs, one-in-flight
 * C with drop-newest back-pressure, C-timeout skip-D recovery, and the
 * single final_result buffer with an explicit consumer release handshake.
 *
 * Kept separate from mmf2_video_example_test_vin_large_image.c, which is
 * the mode-verification *test* (file-copy injection, no GE fold/unfold)
 * and stays intact/unmodified.
 *
 * NOTE on geometry (revised; supersedes decision D12 in
 * openspec/changes/2026-08-25-add-large-image-pipeline/design.md):
 * stream-0 captures the sensor's full native raw (2568x1448, pre-crop_l)
 * via the "isp in @ long" dump tap, instead of an enlarged crop_l window.
 * The GC4023 crop_l mode-0 stays at its original 2560x1440; the fold's
 * per-half crop window (1288x1444 = dst 1280x1440 + overlap 8x4) is taken
 * directly out of the larger 2568x1448 source, so it still has the
 * y_overlap headroom it needs without requiring crop_l to grow.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ameba_soc.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "rts_camera_soc.h"
#include "rts_camera_mem.h"
#include "rtscam_isp_input.h"
#include "rtscam_isp_command.h"
#include "rtsavisp.h"
#include "rts_isp.h"
#include "rts_isp_ctrl.h"
#include "ge_reshape_api.h"
#include "ge_sys_api.h"
#include "hal_ge.h"
#include "vfs.h"

/* ------------------------------------------------------------------ */
/* Geometry -- derived symbolically from the clean target dimensions    */
/* (see file header note above).                                        */
/* ------------------------------------------------------------------ */

#define PL_X_OVERLAP    8U
#define PL_Y_OVERLAP    4U

/* Clean, full-resolution output (== per-half fold geometry). */
#define PL_FINAL_W      2560U
#define PL_FINAL_H      1440U
#define PL_HALF_W       (PL_FINAL_W / 2)   /* 1280 clean per-half width  */
#define PL_HALF_H       PL_FINAL_H         /* 1440 clean per-half height */

/*
 * Stream-0 capture = the sensor's full native raw array, taken pre-crop_l
 * via the "isp in" dump tap. This is strictly larger than the fold's
 * per-half crop window (1288x1444) in both dimensions, so the fold's
 * y_overlap headroom comes from the source's natural slack instead of
 * requiring crop_l to be enlarged.
 */
#define PL_CAPTURE_W    2568U /* GC4023 full sensor array width */
#define PL_CAPTURE_H    1448U /* GC4023 full sensor array height */

#define PL_FOLD_W       (PL_HALF_W + PL_X_OVERLAP)       /* 1288 */
#define PL_FOLD_H       ((PL_HALF_H + PL_Y_OVERLAP) * 2) /* 2888 */
#define PL_FOLD_START_X 0U
#define PL_FOLD_START_Y (PL_Y_OVERLAP / 2)               /* 2 */

/*
 * Stream-0 dump-tap selection: index into the isp_hw_global output menu
 * (isp_hw_global.c get_output_menu / set_output). Value 1 = "isp in @
 * long" = the pre-crop_l tap, giving the full native sensor raw.
 */
#define PL_OUTPUT_POS_ISP_IN 1

/* Buffer counts (D6). */
#define PL_S0_BUFS 2
#define PL_VI_BUFS 2
#define PL_S1_BUFS 2

/* Buffer byte sizes, derived symbolically (tasks.md 2.1). */
#define PL_STREAM0_BUF_SIZE  ((uint32_t)PL_CAPTURE_W * PL_CAPTURE_H * 2)
#define PL_VERIFY_BUF_SIZE   ((uint32_t)PL_FOLD_W * PL_FOLD_H * 2)
#define PL_STREAM1_BUF_SIZE  ((uint32_t)PL_FOLD_W * PL_FOLD_H * 3 / 2)
#define PL_FINAL_BUF_SIZE    ((uint32_t)PL_FINAL_W * PL_FINAL_H * 3 / 2)

/*
 * Verify/input cadence fed to rtscam_isp_input_set_behavior(). This feeds
 * a real HW-timing check (input->vsnr_mode.mode.fps in isp_mod_input.c) as
 * well as the simulated-sensor rate, not just bookkeeping. No production
 * cadence was specified for this migration; the folded geometry here
 * (1288x2888) is within a few percent of the already-validated
 * large-image-mode-tnr-test geometry (1280x2880), so this mirrors that
 * capability's own validated fps convention rather than guessing a new
 * value with unproven HW-timing margin.
 */
#define PL_INPUT_FPS_NUM 1U
#define PL_INPUT_FPS_DEN 5U

/*
 * Poll interval for the default background consumer (pl_consumer_task,
 * below): it exists purely to keep the D-stage release handshake cycling
 * when no real downstream consumer has been wired up (D9). Kept well
 * under the shutdown drain delay in
 * mmf2_video_example_large_image_pipeline_stop() (currently 50 ms) so
 * that delay reliably covers both this task and pl_loop_task observing
 * pl_stop_flag before the sema/buffer they share are torn down.
 */
#define PL_CONSUMER_POLL_MS 10U

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static volatile int pl_running;
static volatile int pl_stop_flag;
static rtos_task_t pl_setup_task_handle;
static rtos_task_t pl_loop_task_handle;
static rtos_task_t pl_consumer_task_handle;

static ge_drv_ctx_t *pl_ge_ctx;

static struct rts_video_buffer *pl_s0_bufs[PL_S0_BUFS];
static struct rts_video_buffer *pl_s1_bufs[PL_S1_BUFS];
static struct rts_video_buffer *pl_vi_bufs[PL_VI_BUFS];
static void *pl_vi_mem[PL_VI_BUFS];

static void *pl_final_mem;
static void *pl_final_buf;
static rtos_sema_t pl_final_sema;

static unsigned long pl_frames_out;
static unsigned long pl_frames_dropped;
static unsigned long pl_frames_timeout;

/* Per-step diagnostic log (step A/B/C/D timing, final_result cadence) --
 * off by default, toggle via CLI "video vin large_image_pipeline log_en
 * <0|1>". Errors/timeouts/cadence warnings always print regardless. */
static volatile uint8_t pl_diag_log_en;
#define PL_DIAG_LOG(fmt, ...) \
	do { if (pl_diag_log_en) DiagPrintf(fmt, ##__VA_ARGS__); } while (0)

/* Per-frame stage timestamps (single in-flight C, D3), used to log each
 * step's own duration plus end-to-end latency at final_result time. */
static TickType_t pl_t_frame_start;
static TickType_t pl_t_c_start;

/* Sensor cadence vs. delivery cadence tracking: pl_raw_interval is the
 * most recent RAW-to-RAW capture gap (the sensor's own frame period);
 * pl_t_final_prev is the tick of the previous final_result, used to
 * detect if delivery ever falls behind that period. */
static TickType_t pl_t_raw_prev;
static TickType_t pl_raw_interval;
static TickType_t pl_t_final_prev;

/*
 * Capture is requested by the console command but serviced by
 * pl_consumer_task, which already runs with a 4096-word stack (needed
 * for the FatFs -> SD call chain) and already owns the
 * get_output()/release_output() handshake -- avoids adding a second
 * reader of pl_final_buf.
 */
static volatile int pl_capture_request;
static char pl_capture_suffix[32];

extern void mmf2_video_example_test_vin_ensure_hw(void);

/* Forward decl: full definitions (with API doc comments) are below.
 * pl_consumer_task, defined further up in this file, calls these just
 * like any other downstream consumer would, to exercise the same D9
 * release handshake.
 */
int mmf2_video_example_large_image_pipeline_get_output(
	struct rts_video_buffer *out);
void mmf2_video_example_large_image_pipeline_release_output(void);

/* ------------------------------------------------------------------ */
/* Teardown helpers                                                     */
/* ------------------------------------------------------------------ */

static void pl_release_stream_bufs(unsigned int streamid,
								   struct rts_video_buffer **bufs, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		if (bufs[i]) {
			rtscam_video_buffer_release(streamid, bufs[i]);
			bufs[i] = NULL;
		}
	}
}

static void pl_release_verify_bufs(void)
{
	int i;

	for (i = 0; i < PL_VI_BUFS; i++) {
		if (pl_vi_bufs[i]) {
			rtscam_video_buffer_release(RTSCAM_INPUT_STRM_IDX,
										pl_vi_bufs[i]);
			pl_vi_bufs[i] = NULL;
		}
		if (pl_vi_mem[i]) {
			rtscam_mem_free(pl_vi_mem[i]);
			pl_vi_mem[i] = NULL;
		}
	}
}

/* ------------------------------------------------------------------ */
/* Orchestration loop (single task, owns the GE context)                */
/* ------------------------------------------------------------------ */

static void pl_loop_task(void *arg)
{
	struct rts_video_buffer *vi_buf;
	struct rts_video_buffer *s1_buf;
	struct rts_video_buffer *raw_buf;
	int c_inflight = 0;
	int output_pending = 0;
	int vi_idx = 0;
	uint32_t seq = 0;
	int did_work;
	int ret;

	(void)arg;

	while (!pl_stop_flag) {
		did_work = 0;

		/* Step 1: reap C's input completion (frame_done), the wait
		 * gate that fires on both success and the driver's ~500 ms
		 * timeout (D4/D5). This frees the verify-input buffer for
		 * the next fold regardless of outcome.
		 */
		if (c_inflight) {
			ret = rtscam_isp_input_dequeue(&vi_buf);
			if (ret == 0) {
				TickType_t t_c_done = xTaskGetTickCount();

				c_inflight = 0;
				did_work = 1;
				if (vi_buf->bytesused == 0) {
					pl_frames_timeout++;
					DiagPrintf("[large_image_pipeline] step C: timeout "
							   "seq=%u dur=%lu ticks\r\n", (unsigned)seq,
							   (unsigned long)(t_c_done - pl_t_c_start));
				} else {
					output_pending = 1;
					PL_DIAG_LOG("[large_image_pipeline] step C: done "
								"seq=%u dur=%lu ticks\r\n", (unsigned)seq,
								(unsigned long)(t_c_done - pl_t_c_start));
				}
			}
			/* else -EAGAIN: not done yet, fall through. */
		}

		/* Step 1b: once C's output is known valid, pick up the
		 * stream-1 buffer it produced (may arrive on a later
		 * iteration than frame_done -- keep polling without
		 * re-checking frame_done, D5).
		 */
		if (output_pending) {
			ret = rtscam_video_dequeue(1, &s1_buf);
			if (ret == 0) {
				TickType_t t_d_start = xTaskGetTickCount();
				TickType_t t_d_done;

				output_pending = 0;
				did_work = 1;

				PL_DIAG_LOG("[large_image_pipeline] step C: s1 dequeued "
							"seq=%u tick=%lu\r\n", (unsigned)seq,
							(unsigned long)t_d_start);

				/* D9: do not overwrite final_result until the
				 * consumer has released the previous frame.
				 * This is the single documented place where a
				 * slow/absent consumer stalls this task --
				 * accepted by design (D9/D3): capture-side
				 * drops follow once stream-0/1 pools fill.
				 */
				if (rtos_sema_take(pl_final_sema, 0xFFFFFFFF) == 0) {
					ret = ge_yuv_reshape(pl_ge_ctx,
										 (void *)(uintptr_t)s1_buf->phy_addr,
										 pl_final_buf,
										 PL_FOLD_W, PL_FOLD_H,
										 PL_FINAL_W, PL_FINAL_H,
										 PL_X_OVERLAP, PL_Y_OVERLAP);
					t_d_done = xTaskGetTickCount();
					pl_frames_out++;
					if (ret != 0) {
						DiagPrintf("[large_image_pipeline] "
								   "ge_yuv_reshape failed (%d)\r\n",
								   ret);
					} else {
						TickType_t frame_interval = pl_t_final_prev ?
													(t_d_done - pl_t_final_prev) : 0;

						PL_DIAG_LOG("[large_image_pipeline] step D: done "
									"seq=%u dur=%lu ticks\r\n", (unsigned)seq,
									(unsigned long)(t_d_done - t_d_start));
						PL_DIAG_LOG("[large_image_pipeline] final_result "
									"#%lu ready: %ux%u NV12 @ 0x%08x "
									"(%u bytes), seq=%u, total=%lu ticks, "
									"interval=%lu ticks (sensor=%lu)\r\n",
									pl_frames_out,
									(unsigned)PL_FINAL_W,
									(unsigned)PL_FINAL_H,
									(unsigned)(uintptr_t)pl_final_buf,
									(unsigned)PL_FINAL_BUF_SIZE,
									(unsigned)seq,
									(unsigned long)(t_d_done - pl_t_frame_start),
									(unsigned long)frame_interval,
									(unsigned long)pl_raw_interval);
						if (frame_interval > pl_raw_interval) {
							DiagPrintf("[large_image_pipeline] "
									   "WARNING: delivery fell behind "
									   "sensor cadence (interval=%lu > "
									   "sensor=%lu)\r\n",
									   (unsigned long)frame_interval,
									   (unsigned long)pl_raw_interval);
						}
						pl_t_final_prev = t_d_done;
					}
				} else {
					pl_frames_dropped++;
				}
				rtscam_video_enqueue(1, s1_buf);
			}
			/* else -EAGAIN: retry next iteration. */
		}

		/* Step 2: pull a newly captured frame, if any. */
		ret = rtscam_video_dequeue(0, &raw_buf);
		if (ret == 0) {
			TickType_t t_raw = xTaskGetTickCount();
			TickType_t t_b_done;
			TickType_t raw_interval =
				pl_t_raw_prev ? (t_raw - pl_t_raw_prev) : 0;

			pl_t_raw_prev = t_raw;
			did_work = 1;
			PL_DIAG_LOG("[large_image_pipeline] step A: RAW frame "
						"tick=%lu interval=%lu ticks\r\n",
						(unsigned long)t_raw, (unsigned long)raw_interval);

			if (c_inflight) {
				/* D3: C is busy -- drop the newest frame
				 * unprocessed rather than fold-then-drop.
				 */
				rtscam_video_enqueue(0, raw_buf);
				pl_frames_dropped++;
			} else {
				ret = ge_raw_reshape(pl_ge_ctx,
									 (void *)(uintptr_t)raw_buf->phy_addr,
									 (void *)(uintptr_t)pl_vi_bufs[vi_idx]->phy_addr,
									 PL_CAPTURE_W, PL_CAPTURE_H,
									 PL_FOLD_START_X, PL_FOLD_START_Y,
									 PL_HALF_W, PL_HALF_H,
									 PL_X_OVERLAP, PL_Y_OVERLAP);
				t_b_done = xTaskGetTickCount();
				if (ret != 0) {
					DiagPrintf("[large_image_pipeline] "
							   "ge_raw_reshape failed (%d)\r\n", ret);
				} else {
					PL_DIAG_LOG("[large_image_pipeline] step B: done "
								"seq=%u dur=%lu ticks\r\n",
								(unsigned)(seq + 1),
								(unsigned long)(t_b_done - t_raw));
				}
				/* B has read raw_buf; return the stream-0
				 * slot immediately (legacy reuse condition).
				 */
				rtscam_video_enqueue(0, raw_buf);

				pl_vi_bufs[vi_idx]->bytesused = PL_VERIFY_BUF_SIZE;
				if (rtscam_isp_input_enqueue(pl_vi_bufs[vi_idx]) == 0) {
					c_inflight = 1;
					seq++;
					vi_idx = 1 - vi_idx;
					pl_t_frame_start = t_raw;
					pl_t_c_start = xTaskGetTickCount();
					pl_raw_interval = raw_interval;
				} else {
					/* Defensive -ENOSPC: should not happen
					 * given the one-in-flight discipline
					 * above; treat as a drop.
					 */
					pl_frames_dropped++;
				}
			}
		}

		if (!did_work) {
			vTaskDelay(pdMS_TO_TICKS(1));
		}
	}

	pl_loop_task_handle = NULL;
	vTaskDelete(NULL);
}

static void pl_save_to_sd(void *data, uint32_t size, const char *filename)
{
	char filepath[64];
	FILE *fp;
	int ret;

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] VFS-FAT init fail\r\n");
		return;
	}
	snprintf(filepath, sizeof(filepath), "fat:%s", filename);
	fp = fopen(filepath, "wb");
	if (!fp) {
		DiagPrintf("[large_image_pipeline] fopen(%s) failed\r\n", filepath);
		vfs_target_unregister("fat");
		return;
	}
	ret = fwrite(data, 1, size, fp);
	if (ret != (int)size) {
		DiagPrintf("[large_image_pipeline] fwrite() failed, wrote %d of "
				   "%u bytes\r\n", ret, size);
	} else {
		DiagPrintf("[large_image_pipeline] fwrite() succeeded, wrote %u "
				   "bytes to %s\r\n", size, filepath);
	}
	fclose(fp);
	vfs_target_unregister("fat");
}

/*
 * Default background consumer (D9 fix): drains final_result via the same
 * public get_output()/release_output() handshake any real downstream
 * consumer would use, so pl_loop_task's D-stage is never left blocked on
 * pl_final_sema when nothing else has been wired up. Deliberately
 * lightweight -- log-and-drop, not real processing. pl_frames_out is
 * reused as-is for the log's frame-number field; only logged when it
 * changes so the tick delta between prints reflects the real frame
 * interval instead of the poll interval.
 */
static void pl_consumer_task(void *arg)
{
	struct rts_video_buffer out;
	unsigned long last_logged = 0;
	int have_logged = 0;
	int ret;

	(void)arg;

	while (!pl_stop_flag) {
		ret = mmf2_video_example_large_image_pipeline_get_output(&out);
		if (ret == 0) {
			if (!have_logged || pl_frames_out != last_logged) {
				PL_DIAG_LOG("[large_image_pipeline] consumer: frame #%lu "
							"%ux%u NV12 (%u bytes) tick=%lu\r\n",
							pl_frames_out,
							(unsigned)PL_FINAL_W, (unsigned)PL_FINAL_H,
							(unsigned)out.bytesused,
							(unsigned long)xTaskGetTickCount());
				last_logged = pl_frames_out;
				have_logged = 1;
			}
			if (pl_capture_request) {
				char filename[64];

				if (pl_capture_suffix[0]) {
					snprintf(filename, sizeof(filename),
							 "li_result_%lu_%s.nv12", pl_frames_out,
							 pl_capture_suffix);
				} else {
					snprintf(filename, sizeof(filename),
							 "li_result_%lu.nv12", pl_frames_out);
				}
				pl_save_to_sd(out.vm_addr, out.bytesused, filename);
				pl_capture_request = 0;
			}
			mmf2_video_example_large_image_pipeline_release_output();
		}
		vTaskDelay(pdMS_TO_TICKS(PL_CONSUMER_POLL_MS));
	}

	pl_consumer_task_handle = NULL;
	vTaskDelete(NULL);
}

/*
 * Modules not supported by the fold/verify/unfold geometry (their
 * reference windows don't survive the RAW split-and-restack): NLSC,
 * MLSC, CCM, LCE, LDC, WDR/HDR, motion-info, and geometric CAC. Set via
 * the ISP's public bypass API (rts_isp_set_bypass(), backed by
 * isp_hw_global_set_func_enable()'s bypass_bits/control_value) instead
 * of hardcoding a stripped-down control_value in isp_hw_global.c --
 * that hack disabled these modules for every ISP session, not just this
 * pipeline.
 */
static int pl_set_module_bypass(int enable)
{
	struct rts_isp_bypass bypass;

	memset(&bypass, 0, sizeof(bypass));
	if (enable) {
		bypass.bypass_nlsc = 1;
		bypass.bypass_mlsc = 1;
		bypass.bypass_ccm = 1;
		bypass.bypass_lce = 1;
		bypass.bypass_ldc = 1;
		bypass.bypass_wdr = 1;
		bypass.bypass_motion_info = 1;
		bypass.bypass_gcac = 1;
	}
	return rts_isp_set_bypass(ISP0, &bypass);
}

/* ------------------------------------------------------------------ */
/* Setup task                                                           */
/* ------------------------------------------------------------------ */

static void pl_setup_task(void *arg)
{
	struct rts_video_format format;
	int i;
	int ret;

	(void)arg;

	mmf2_video_example_test_vin_ensure_hw();
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	hal_ge_init();
	pl_ge_ctx = ge_open();
	if (!pl_ge_ctx || pl_ge_ctx == (ge_drv_ctx_t *)EINVAL) {
		DiagPrintf("[large_image_pipeline] ge_open failed\r\n");
		pl_ge_ctx = NULL;
		goto done;
	}
	ge_ctrl(pl_ge_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	ret = rts_isp_set_large_image_mode(ISP0, 1);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] large image mode failed "
				   "%d\r\n", ret);
		goto close_ge;
	}

	ret = pl_set_module_bypass(1);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] module bypass failed "
				   "%d\r\n", ret);
		goto disable_mode;
	}

	/* Bring up the input path first -- starting it drives the sensor
	 * into the large-image geometry and forces a SoC video
	 * detach+attach, which would wipe out any capture stream opened
	 * beforehand (mirrors mmf2_video_example_test_vin_large_image.c).
	 */
	ret = rtscam_video_open(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] input open failed %d\r\n",
				   ret);
		goto disable_mode;
	}

	for (i = 0; i < PL_VI_BUFS; i++) {
		uint32_t vi_phy = 0;

		/* Allocate the USERPTR backing from the physically-contiguous
		 * ISP DMA pool (rtscam_mem_alloc), NOT the FreeRTOS task heap:
		 * one verify-input buffer is ~7.4 MB and the leftover task heap
		 * is far too small (the 48 MB ISP pool is itself carved from the
		 * heap at init). The GE fold writes it and the ISP reads it, both
		 * via DMA; it is never touched by the CPU.
		 */
		pl_vi_mem[i] = rtscam_mem_alloc(PL_VERIFY_BUF_SIZE, &vi_phy,
										RTS_MEM_ATTR_CACHE, "li_verify");
		if (!pl_vi_mem[i]) {
			DiagPrintf("[large_image_pipeline] verify-input mem "
					   "alloc[%d] failed\r\n", i);
			goto fail_verify_bufs;
		}
		pl_vi_bufs[i] = rtscam_video_buffer_alloc(RTSCAM_INPUT_STRM_IDX,
						RTS_MEMORY_USERPTR,
						PL_VERIFY_BUF_SIZE);
		if (!pl_vi_bufs[i]) {
			DiagPrintf("[large_image_pipeline] verify-input buffer "
					   "alloc[%d] failed\r\n", i);
			goto fail_verify_bufs;
		}
		/* RTS_MEMORY_USERPTR: driver allocates no backing memory;
		 * the caller supplies and owns it (D7/B->C zero-copy).
		 */
		pl_vi_bufs[i]->vm_addr = (uint8_t *)pl_vi_mem[i];
		pl_vi_bufs[i]->phy_addr = vi_phy;
	}

	ret = rtscam_isp_input_set_behavior(1, ISP_INPUT_SGRBG16,
										PL_FOLD_W, PL_FOLD_H,
										PL_INPUT_FPS_NUM, PL_INPUT_FPS_DEN);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] set_behavior failed %d\r\n",
				   ret);
		goto fail_verify_bufs;
	}

	/* set_behavior() above only stores parameters; s_stream(1) below
	 * dispatches INPUT_EXEC_SET_BEHAVIOR, which drives the sensor
	 * geometry change / force_reset described above.
	 */
	ret = rtscam_video_stream_start(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] input stream start failed "
				   "%d\r\n", ret);
		goto fail_verify_bufs;
	}

	/* Capture streams are opened only after the input stream is
	 * running, so the force_reset above cannot wipe their open state.
	 */
	ret = rtscam_video_open(0);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] stream 0 open failed %d\r\n",
				   ret);
		goto fail_input_started;
	}
	memset(&format, 0, sizeof(format));
	/* No RAW16 Bayer fourcc is registered on stream 0 in this codebase
	 * (only NV12/NV16/YUYV/128BIT_3PIXEL, see
	 * rts_camera_soc.c:m_rtscam_soc_formats + CONFIG_RTS_ZOOM_S0_FMT).
	 * V4L2_PIX_FMT_YUYV is the only registered stream-0 format whose
	 * bpp (16) matches RAW16's 2 bytes/pixel, so it produces the exact
	 * sizeimage/pitch RAW16 needs; the bytes captured in large image
	 * mode are RAW regardless of this tag (see the large-image-mode
	 * capability's own purpose statement). This is the same choice
	 * mmf2_video_example_test_vin_large_image.c makes, reused here for
	 * its buffer-geometry bookkeeping only, not for its file-injection
	 * test logic.
	 */
	format.pixelformat = V4L2_PIX_FMT_YUYV;
	format.width = PL_CAPTURE_W;
	format.height = PL_CAPTURE_H;
	ret = rtscam_video_set_format(0, &format);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] stream 0 set_format failed "
				   "%d\r\n", ret);
		goto fail_stream0_opened;
	}
	/*
	 * Allocate ALL of this stream's buffers before enqueuing any of
	 * them: rtscam_video_enqueue() latches stream->buf_nums into the
	 * hardware's per-stream slot count on its first-ever call
	 * (rtscam_init_capture_buffer() -> rtscam_soc_init_capture_buffers(),
	 * gated by stream->capture_init and never re-run). Interleaving
	 * alloc+enqueue per buffer would fire that latch after only the
	 * first buffer is allocated, permanently configuring a single HW
	 * ring slot instead of PL_S0_BUFS.
	 */
	for (i = 0; i < PL_S0_BUFS; i++) {
		pl_s0_bufs[i] = rtscam_video_buffer_alloc(0, RTS_MEMORY_KERNEL,
						PL_STREAM0_BUF_SIZE);
		if (!pl_s0_bufs[i]) {
			DiagPrintf("[large_image_pipeline] stream 0 buffer[%d] "
					   "alloc failed\r\n", i);
			goto fail_stream0_configured;
		}
	}
	for (i = 0; i < PL_S0_BUFS; i++) {
		if (rtscam_video_enqueue(0, pl_s0_bufs[i]) != 0) {
			DiagPrintf("[large_image_pipeline] stream 0 buffer[%d] "
					   "enqueue failed\r\n", i);
			goto fail_stream0_configured;
		}
	}

	ret = rtscam_video_open(1);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] stream 1 open failed %d\r\n",
				   ret);
		goto fail_stream0_configured;
	}
	memset(&format, 0, sizeof(format));
	format.pixelformat = V4L2_PIX_FMT_NV12;
	format.width = PL_FOLD_W;
	format.height = PL_FOLD_H;
	ret = rtscam_video_set_format(1, &format);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] stream 1 set_format failed "
				   "%d\r\n", ret);
		goto fail_stream1_configured;
	}
	/* Allocate-then-enqueue, split as for stream 0 above. */
	for (i = 0; i < PL_S1_BUFS; i++) {
		pl_s1_bufs[i] = rtscam_video_buffer_alloc(1, RTS_MEMORY_KERNEL,
						PL_STREAM1_BUF_SIZE);
		if (!pl_s1_bufs[i]) {
			DiagPrintf("[large_image_pipeline] stream 1 buffer[%d] "
					   "alloc failed\r\n", i);
			goto fail_stream1_configured;
		}
	}
	for (i = 0; i < PL_S1_BUFS; i++) {
		if (rtscam_video_enqueue(1, pl_s1_bufs[i]) != 0) {
			DiagPrintf("[large_image_pipeline] stream 1 buffer[%d] "
					   "enqueue failed\r\n", i);
			goto fail_stream1_configured;
		}
	}

	ret = rtscam_video_stream_start(0);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] stream 0 start failed %d\r\n",
				   ret);
		goto fail_stream1_configured;
	}
	ret = rtscam_video_stream_start(1);
	if (ret != 0) {
		DiagPrintf("[large_image_pipeline] stream 1 start failed %d\r\n",
				   ret);
		goto fail_stream0_started;
	}

	/* Select the stream-0 dump tap = "isp in @ long" (pre-crop_l), giving
	 * the full native sensor raw. Use the proper ISP control (routes to
	 * isp_hw_global_set_output -> SYS_IMAGE_SEL1) instead of poking
	 * register 0x4c directly.
	 */
	{
		struct isp_v4l2_control octrl;

		memset(&octrl, 0, sizeof(octrl));
		octrl.id = ISP_V4L2_CID_OUTPUT_POSITION;
		octrl.value = PL_OUTPUT_POS_ISP_IN;
		ret = rts_isp_v4l2_s_ctrl(ISP0, &octrl);
		if (ret != 0) {
			DiagPrintf("[large_image_pipeline] set output position "
					   "failed %d\r\n", ret);
			goto fail_stream1_started;
		}
	}

	/* final_result from the FreeRTOS heap (NOT the ISP DMA pool): it is the
	 * one CPU-touched buffer (the consumer reads it after DCache_Invalidate,
	 * D10), and keeping it off the pool leaves the ~44.5 MB DMA working set
	 * within the 48 MB pool. The AP heap is 64-byte aligned
	 * (portBYTE_ALIGNMENT), which satisfies GE's 16-byte dst alignment.
	 */
	pl_final_mem = rtos_mem_zmalloc(PL_FINAL_BUF_SIZE);
	if (!pl_final_mem) {
		DiagPrintf("[large_image_pipeline] final_result alloc failed\r\n");
		goto fail_stream1_started;
	}
	pl_final_buf = pl_final_mem;
	/* Flush the zero-fill so a late eviction of these (CPU-dirtied) cache
	 * lines cannot clobber the GE DMA output before the consumer reads it.
	 */
	DCache_CleanInvalidate((uint32_t)(uintptr_t)pl_final_buf, PL_FINAL_BUF_SIZE);

	if (rtos_sema_create_binary(&pl_final_sema) != 0) {
		DiagPrintf("[large_image_pipeline] final sema create failed\r\n");
		goto fail_final_alloc;
	}
	/* Starts "available" so the very first D is never blocked (D9). */
	rtos_sema_give(pl_final_sema);

	pl_frames_out = 0;
	pl_frames_dropped = 0;
	pl_frames_timeout = 0;
	pl_stop_flag = 0;
	pl_running = 1;
	if (rtos_task_create(&pl_loop_task_handle, "li_pipeline",
						 pl_loop_task, NULL, 4096, 1) != 0) {
		DiagPrintf("[large_image_pipeline] loop task create failed\r\n");
		pl_running = 0;
		goto fail_sema_created;
	}

	/*
	 * Default background consumer (D9 fix): spawned unconditionally --
	 * there is no opt-out API -- so a missing real consumer can never
	 * stall pl_loop_task on pl_final_sema past the first frame.
	 */
	if (rtos_task_create(&pl_consumer_task_handle, "li_consumer",
						 pl_consumer_task, NULL, 4096, 1) != 0) {
		DiagPrintf("[large_image_pipeline] consumer task create "
				   "failed\r\n");
		pl_consumer_task_handle = NULL;
		pl_running = 0;
		/*
		 * pl_loop_task is already running. Unblock and signal it the
		 * same way mmf2_video_example_large_image_pipeline_stop()
		 * does, and give it the same drain delay, before falling
		 * through to the cleanup chain below that deletes
		 * pl_final_sema and frees pl_final_buf -- both of which
		 * pl_loop_task still touches until it observes pl_stop_flag.
		 */
		pl_stop_flag = 1;
		rtos_sema_give(pl_final_sema);
		vTaskDelay(pdMS_TO_TICKS(50));
		goto fail_sema_created;
	}

	DiagPrintf("[large_image_pipeline] started: capture %ux%u, "
			   "fold %ux%u, final %ux%u\r\n",
			   (unsigned)PL_CAPTURE_W, (unsigned)PL_CAPTURE_H,
			   (unsigned)PL_FOLD_W, (unsigned)PL_FOLD_H,
			   (unsigned)PL_FINAL_W, (unsigned)PL_FINAL_H);
	goto done;

fail_sema_created:
	rtos_sema_delete(pl_final_sema);
	pl_final_sema = NULL;
fail_final_alloc:
	rtos_mem_free(pl_final_mem);
	pl_final_mem = NULL;
	pl_final_buf = NULL;
fail_stream1_started:
	rtscam_video_stream_stop(1);
fail_stream0_started:
	rtscam_video_stream_stop(0);
fail_stream1_configured:
	pl_release_stream_bufs(1, pl_s1_bufs, PL_S1_BUFS);
	rtscam_video_close(1);
fail_stream0_configured:
	pl_release_stream_bufs(0, pl_s0_bufs, PL_S0_BUFS);
fail_stream0_opened:
	rtscam_video_close(0);
fail_input_started:
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);
fail_verify_bufs:
	pl_release_verify_bufs();
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);
disable_mode:
	pl_set_module_bypass(0);
	rts_isp_set_large_image_mode(ISP0, 0);
close_ge:
	if (pl_ge_ctx) {
		ge_close(pl_ge_ctx);
		pl_ge_ctx = NULL;
	}
done:
	pl_setup_task_handle = NULL;
	vTaskDelete(NULL);
}

/* ------------------------------------------------------------------ */
/* Public entry points                                                  */
/* ------------------------------------------------------------------ */

void mmf2_video_example_large_image_pipeline_set_log_en(int enable)
{
	pl_diag_log_en = enable ? 1 : 0;
	DiagPrintf("[large_image_pipeline] diag log %s\r\n",
			   pl_diag_log_en ? "enabled" : "disabled");
}

/*
 * Request that the next final_result frame be saved to SD as
 * li_result_<seq>[_<suffix>].nv12. Serviced by pl_consumer_task (D9's
 * default consumer), which owns the get_output()/release_output()
 * handshake and has the stack headroom for the FatFs call chain.
 */
void mmf2_video_example_large_image_pipeline_capture(const char *suffix)
{
	if (!pl_running) {
		DiagPrintf("[large_image_pipeline] nothing running\r\n");
		return;
	}
	if (pl_capture_request) {
		DiagPrintf("[large_image_pipeline] capture already pending\r\n");
		return;
	}
	if (suffix && suffix[0]) {
		snprintf(pl_capture_suffix, sizeof(pl_capture_suffix), "%s", suffix);
	} else {
		pl_capture_suffix[0] = '\0';
	}
	pl_capture_request = 1;
	DiagPrintf("[large_image_pipeline] capture requested%s%s\r\n",
			   pl_capture_suffix[0] ? " suffix=" : "", pl_capture_suffix);
}

void mmf2_video_example_large_image_pipeline_start(void)
{
	if (pl_running || pl_setup_task_handle) {
		DiagPrintf("[large_image_pipeline] already running\r\n");
		return;
	}
	if (rtos_task_create(&pl_setup_task_handle, "li_pipe_setup",
						 pl_setup_task, NULL, 4096, 1) != 0) {
		DiagPrintf("[large_image_pipeline] setup task create failed\r\n");
		pl_setup_task_handle = NULL;
	}
}

void mmf2_video_example_large_image_pipeline_stop(void)
{
	if (!pl_running) {
		DiagPrintf("[large_image_pipeline] nothing running\r\n");
		return;
	}

	/*
	 * The loop task may currently be blocked indefinitely in
	 * rtos_sema_take(pl_final_sema, ...) waiting for a consumer that
	 * has never been wired up to call the release function below
	 * (D9's accepted stall risk, now covered by the default
	 * pl_consumer_task started alongside pl_loop_task in
	 * pl_setup_task). Give the semaphore first so the loop task is
	 * guaranteed to observe pl_stop_flag on its next check instead of
	 * blocking forever past teardown; give() on an already-available
	 * binary semaphore is a harmless no-op.
	 */
	if (pl_final_sema) {
		rtos_sema_give(pl_final_sema);
	}
	pl_stop_flag = 1;
	/*
	 * 50 ms must cover every task that still touches pl_final_sema /
	 * pl_final_buf noticing pl_stop_flag and exiting before they are
	 * deleted/freed below: pl_loop_task's worst case is one iteration
	 * (unblocked by the give() above, or its own 1 ms idle poll), and
	 * pl_consumer_task polls every PL_CONSUMER_POLL_MS == 10 ms. Both
	 * are well under this delay, so it holds with comfortable margin.
	 */
	vTaskDelay(pdMS_TO_TICKS(50));

	rtscam_video_stream_stop(1);
	rtscam_video_stream_stop(0);
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);

	pl_release_stream_bufs(1, pl_s1_bufs, PL_S1_BUFS);
	pl_release_stream_bufs(0, pl_s0_bufs, PL_S0_BUFS);
	pl_release_verify_bufs();

	rtscam_video_close(1);
	rtscam_video_close(0);
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);

	pl_set_module_bypass(0);
	rts_isp_set_large_image_mode(ISP0, 0);

	if (pl_ge_ctx) {
		ge_close(pl_ge_ctx);
		pl_ge_ctx = NULL;
	}
	if (pl_final_sema) {
		rtos_sema_delete(pl_final_sema);
		pl_final_sema = NULL;
	}
	if (pl_final_mem) {
		rtos_mem_free(pl_final_mem);
		pl_final_mem = NULL;
		pl_final_buf = NULL;
	}

	DiagPrintf("[large_image_pipeline] stopped: frames=%lu dropped=%lu "
			   "timeouts=%lu c_drop=%lu\r\n",
			   pl_frames_out, pl_frames_dropped, pl_frames_timeout,
			   rtscam_isp_input_get_drop_count());

	pl_running = 0;
	pl_loop_task_handle = NULL;
	pl_consumer_task_handle = NULL;
}

/*
 * Best-effort snapshot of final_result for a downstream consumer.
 * DCache_Invalidate happens here (at the CPU-touch boundary, D10), not in
 * the orchestrator loop, so it runs exactly once per consumer read rather
 * than once per D. Does not take/give pl_final_sema: that semaphore
 * governs when the orchestrator may write the *next* D, not read access.
 * Call mmf2_video_example_large_image_pipeline_release_output() once done
 * reading so the orchestrator can proceed past the next D.
 */
int mmf2_video_example_large_image_pipeline_get_output(struct rts_video_buffer *out)
{
	if (!out) {
		return -EINVAL;
	}
	if (!pl_running || !pl_final_buf || pl_frames_out == 0) {
		return -ENODATA;
	}

	DCache_Invalidate((uint32_t)(uintptr_t)pl_final_buf, PL_FINAL_BUF_SIZE);

	memset(out, 0, sizeof(*out));
	out->vm_addr = (uint8_t *)pl_final_buf;
	out->phy_addr = (uint32_t)(uintptr_t)pl_final_buf;
	out->size = PL_FINAL_BUF_SIZE;
	out->bytesused = PL_FINAL_BUF_SIZE;

	return 0;
}

void mmf2_video_example_large_image_pipeline_release_output(void)
{
	if (pl_final_sema) {
		rtos_sema_give(pl_final_sema);
	}
}
