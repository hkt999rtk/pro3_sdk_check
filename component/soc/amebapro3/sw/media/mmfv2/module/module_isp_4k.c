/*
 * Realtek Semiconductor Corp.
 *
 * Example topology:
 * A: sensor RAW16 -> stream 0
 * B: PPU clockwise 90 -> portrait RAW16
 * C: ISP input -> one ISP pass per injected RAW
 * D: each pass -> stream 1 portrait NV12, stream 2 downscaled NV12 and/or
 *    stream 10 planar RGB, each on its own isp_4k_module context ->
 *    MMF output_ready -> module_encoder (NV12) or the VP-to-AP link (RGB)
 *
 * A-C form one shared source: the first output APPLY starts it and the last
 * output STOP stops it, so outputs may start and stop in any order.
 * No GE fold/unfold or final-image copy. An output buffer stays owned by the
 * scheduler until the MMF linker releases it. A dedicated PPU task owns
 * init/run/deinit; no other NN client may run concurrently with it.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "rts_camera_soc.h"
#include "rts_camera_mem.h"
#include "rtscam_isp_input.h"
#include "rtscam_isp_command.h"
#include "rtsavisp.h"
#include "rts_isp.h"
#include "rts_isp_ctrl.h"
#include "img_process.h"
#include <stdlib.h>
#include "module_isp_4k.h"
#include "avcodec.h"
#include "hal_isp.h"
#include "isp_common.h"
#include "mmf2_mediatime_pro3.h"

static const char *TAG = "ISP_4K";

/* ---- User-configurable settings ---- */
#define PL_RAW_W        ISP_4K_RAW_WIDTH
#define PL_RAW_H        ISP_4K_RAW_HEIGHT
/* DDR virtual-sensor fallback used by input behavior validation. In large-image
 * mode, isp_mod_input uses the physical sensor's mode and timing instead.
 * This value is not a worker/output pacing limit.
 */
#define PL_INPUT_FPS_NUM 1U
#define PL_INPUT_FPS_DEN 5U
/* One timing summary per NV12 output; disable to reduce UART overhead. */
#define PL_TIMING_LOG_EN 0

/* ---- Internal constants (do not modify) ---- */
/* The sensor must output packed RAW16 at PL_RAW_W x PL_RAW_H.
 * PPU requires dimensions divisible by 16 and 64-byte aligned buffers.
 */
#define PL_CAPTURE_W    PL_RAW_W
#define PL_CAPTURE_H    PL_RAW_H
#define PL_INPUT_W      PL_RAW_H
#define PL_INPUT_H      PL_RAW_W

#if (PL_RAW_W % 16) || (PL_RAW_H % 16)
#error "PPU RAW dimensions must be divisible by 16"
#endif

/*
 * Stream-0 dump-tap selection: index into the isp_hw_global output menu
 * (isp_hw_global.c get_output_menu / set_output). Value 1 = "isp in @
 * long" = the pre-crop_l tap, giving the full native sensor raw.
 */
#define PL_OUTPUT_POS_ISP_IN 1

/* RAW is returned after synchronous PPU rotation. Only one ISP input is
 * in flight; NV12 buffers remain leased until the consumer releases them.
 */
#define PL_S0_BUFS 1
#define PL_VI_BUFS 1

#if PL_S0_BUFS != 1 || PL_VI_BUFS != 1
#error "The scheduler tracks one capture buffer and one rotation buffer"
#endif

/* Buffer byte sizes. */
#define PL_STREAM0_BUF_SIZE  ((uint32_t)PL_CAPTURE_W * PL_CAPTURE_H * 2)
#define PL_VERIFY_BUF_SIZE   ((uint32_t)PL_INPUT_W * PL_INPUT_H * 2)

/* An output APPLY that joins a running source waits this long for its first
 * ISP pass before enabling the stream.
 */
#define PL_ATTACH_WAIT_MS 10000U

/* VERIFY completion does not guarantee a delivered NV12 frame. Bound the
 * output wait so a lost/priming frame cannot prevent all later injections.
 */
#define PL_OUTPUT_WAIT_MS 1000U

/* STOP gives up after this and reports -ETIMEDOUT so callers holding a lock
 * do not deadlock. Teardown itself stays unbounded: DMA buffers are never
 * freed while a consumer may still hold them.
 */
#define PL_STOP_WAIT_MS 3000U

/* One context per output stream: main NV12, sub NV12 and planar RGB. */
#define PL_OUT_NUM 3

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static volatile int pl_stop_flag;

static struct rts_video_buffer *pl_s0_bufs[PL_S0_BUFS];
static struct rts_video_buffer *pl_vi_bufs[PL_VI_BUFS];
static void *pl_vi_mem[PL_VI_BUFS];

enum { PL_OUTPUT_DRIVER, PL_OUTPUT_ACQUIRED, PL_OUTPUT_RELEASED };
enum { PL_REQ_NONE, PL_REQ_ATTACH, PL_REQ_DETACH };

static uint32_t pl_frames_out; /* Completed ISP passes. */
static uint32_t pl_frames_timeout;
static uint32_t pl_output_timeouts;

/* Shared source: stream 0, PPU, ISP input and the scheduler task. */
typedef struct isp_4k_src_s {
	/* APPLY waits for startup; the last STOP waits for active to clear. */
	rtos_task_t worker;
	rtos_sema_t startup;

	/* PPU owns its runtime; request/done synchronize buffer access. */
	rtos_task_t ppu_worker;
	rtos_sema_t ppu_request;
	rtos_sema_t ppu_done;
	struct rts_video_buffer *ppu_raw;
	int ppu_pending; /* Scheduler-owned; result published through ppu_done. */
	int ppu_exit;
	int ppu_result;
	uint32_t ppu_ms;
	volatile int active;
	int startup_result;
} isp_4k_src_t;

static isp_4k_src_t pl_src;
static isp_4k_ctx_t pl_outs[PL_OUT_NUM];
static int pl_src_users; /* Applied outputs; control calls are serialized. */
static uint32_t pl_pass_pending; /* Worker-owned: outputs still owing the current pass. */
static uint32_t pl_pass_start_ms; /* Worker-owned: injection time of the current pass. */
/* Worker-owned: outputs attached since the last injection. The first pass
 * after a stream starts only enables it and delivers no frame there; its
 * armed buffer is filled by the next pass, so do not wait for it.
 */
static uint32_t pl_new_outputs;

/* Enable timing logs with CMD_ISP_4K_LOG_EN. */
static volatile uint8_t pl_diag_log_en;
#define PL_DIAG_LOG(fmt, ...) \
	do { if (pl_diag_log_en) RTK_LOGI(TAG, fmt, ##__VA_ARGS__); } while (0)

/* Capture latency starts when the single RAW slot is armed. */
static uint32_t pl_raw_queued_ms;
/* Setup entry time, for the first-RAW latency log. */
static uint32_t pl_setup_start_ms;

typedef struct {
	uint32_t capture_ms;
	uint32_t raw_dequeue_lag_ms;
	uint32_t rotate_ms;
	uint32_t ready_ms;
} pl_frame_timing_t;

static int pl_hw_initialized;
static struct isp_io pl_io;
static void pl_stop_resources(void);

/* Linker only marks releases; the worker alone requeues ISP buffers. */
static void *isp_release_item(void *p, void *d, int length)
{
	isp_4k_ctx_t *ctx = p;
	mm_queue_item_t *item = d;
	(void)length;

	rtos_critical_enter(RTOS_CRITICAL_SOC);
	for (int i = 0; item && i < ctx->nbufs; i++) {
		if (ctx->output_state[i] == PL_OUTPUT_ACQUIRED &&
			ctx->leased_item[i] == item && ctx->bufs[i] &&
			item->isp.y_addr == ctx->bufs[i]->phy_addr) {
			ctx->leased_item[i] = NULL;
			/* data_addr aliases isp.y_addr in the item union. */
			item->data_addr = 0;
			if (!ctx->is_rgb) {
				item->isp.uv_addr = 0;
			}
			ctx->output_state[i] = PL_OUTPUT_RELEASED;
			break;
		}
	}
	rtos_critical_exit(RTOS_CRITICAL_SOC);
	return NULL;
}

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
/* Outputs (ISP stream 1 / 2 / 10)                                     */
/* ------------------------------------------------------------------ */

static int pl_out_set_stream(isp_4k_ctx_t *ctx, int stream)
{
	/* Validate first: an invalid id must leave the context unchanged */
	if (stream != (int)ISP_4K_MAIN_STREAM_IDX && stream != (int)ISP_4K_SUB_STREAM_IDX &&
		stream != (int)ISP_4K_RGB_STREAM_IDX) {
		return -EINVAL;
	}
	ctx->is_rgb = 0;
	if (stream == (int)ISP_4K_MAIN_STREAM_IDX) {
		ctx->width = ISP_4K_OUTPUT_WIDTH;
		ctx->height = ISP_4K_OUTPUT_HEIGHT;
		ctx->nbufs = ISP_4K_OUTPUT_BUFS;
	} else if (stream == (int)ISP_4K_SUB_STREAM_IDX) {
		ctx->width = ISP_4K_SUB_OUTPUT_WIDTH;
		ctx->height = ISP_4K_SUB_OUTPUT_HEIGHT;
		ctx->nbufs = ISP_4K_SUB_OUTPUT_BUFS;
	} else {
		ctx->width = ISP_4K_RGB_WIDTH;
		ctx->height = ISP_4K_RGB_HEIGHT;
		ctx->nbufs = ISP_4K_RGB_BUFS;
		ctx->is_rgb = 1;
	}
	ctx->stream = (uint32_t)stream;
	return 0;
}

/* Bytes per driver buffer: planar RGB is three full planes, NV12 is 1.5. */
static uint32_t pl_out_buf_size(const isp_4k_ctx_t *ctx)
{
	if (ctx->is_rgb) {
		return (uint32_t)ctx->width * ctx->height * 3;
	}
	return (uint32_t)ctx->width * ctx->height * 3 / 2;
}

/* Count buffers withheld by consumers (or failed requeues), not active DMA.
 * DRIVER includes buffers queued for future output. Released leases are
 * requeued (or only retired when requeue is 0). After close the MMF queues
 * are gone, so a lease that can no longer be returned is retired instead.
 * Stream stop quiesces driver-owned buffers before they are freed.
 */
static int pl_out_recycle(isp_4k_ctx_t *ctx, int requeue)
{
	int pending = 0;

	for (int i = 0; i < ctx->nbufs; i++) {
		int state;
		rtos_critical_enter(RTOS_CRITICAL_SOC);
		state = ctx->output_state[i];
		if (state == PL_OUTPUT_ACQUIRED && ctx->closing) {
			ctx->leased_item[i] = NULL;
			state = PL_OUTPUT_RELEASED;
		}
		rtos_critical_exit(RTOS_CRITICAL_SOC);
		if (state == PL_OUTPUT_RELEASED) {
			if (requeue && rtscam_video_enqueue(ctx->stream, ctx->bufs[i]) != 0) {
				RTK_LOGE(TAG, "s%u requeue failed: slot=%d\r\n", (unsigned)ctx->stream, i);
				ctx->detaching = 1;
				pending++;
				continue;
			}
			rtos_critical_enter(RTOS_CRITICAL_SOC);
			ctx->output_state[i] = PL_OUTPUT_DRIVER;
			rtos_critical_exit(RTOS_CRITICAL_SOC);
		} else if (state == PL_OUTPUT_ACQUIRED) {
			pending++;
		}
	}
	return pending;
}

/* Publish one lease to the MMF linker. RGB goes out as one packed block of
 * three consecutive planes, matching module_isp's AV_CODEC_ID_RGB888 item.
 */
static int pl_out_publish(isp_4k_ctx_t *ctx, struct rts_video_buffer *buf)
{
	mm_context_t *mctx = ctx->parent;
	mm_queue_item_t *item;
	int slot;

	for (slot = 0; slot < ctx->nbufs; slot++) {
		if (ctx->bufs[slot] == buf) {
			break;
		}
	}
	if (slot == ctx->nbufs) {
		return -EINVAL;
	}
	if (rtos_queue_receive(mctx->output_recycle, &item, 0) != SUCCESS) {
		return -EAGAIN;
	}
	if (ctx->is_rgb) {
		item->data_addr = buf->phy_addr;
		item->size = (uint32_t)ctx->width * ctx->height * 3;
		item->type = AV_CODEC_ID_RGB888;
	} else {
		item->isp.y_addr = buf->phy_addr;
		item->isp.uv_addr = buf->phy_addr + ctx->width * ctx->height;
		item->size = buf->bytesused;
		item->type = AV_CODEC_ID_NV12;
	}
	/* Use the shared MMF clock at delivery, not the raw OS-tick epoch. */
	item->timestamp = mm_read_mediatime_ms();
	item->hw_timestamp = item->timestamp;
	rtos_critical_enter(RTOS_CRITICAL_SOC);
	ctx->leased_item[slot] = item;
	ctx->output_state[slot] = PL_OUTPUT_ACQUIRED;
	rtos_critical_exit(RTOS_CRITICAL_SOC);
	if (rtos_queue_send(mctx->output_ready, &item, 0) != SUCCESS) {
		/* Never leased: back to DRIVER, the caller requeues buf itself. */
		rtos_critical_enter(RTOS_CRITICAL_SOC);
		ctx->leased_item[slot] = NULL;
		ctx->output_state[slot] = PL_OUTPUT_DRIVER;
		rtos_critical_exit(RTOS_CRITICAL_SOC);
		item->data_addr = 0;
		item->size = 0;
		rtos_queue_send(mctx->output_recycle, &item, 0);
		return -EAGAIN;
	}
	return 0;
}

static void pl_out_close(isp_4k_ctx_t *ctx)
{
	if (!ctx->opened) {
		return;
	}
	rtscam_video_stream_stop(ctx->stream);
	pl_release_stream_bufs(ctx->stream, ctx->bufs, ctx->nbufs);
	rtscam_video_close(ctx->stream);
	ctx->opened = 0;
	RTK_LOGI(TAG, "s%u stopped: frames=%u dropped=%u\r\n", (unsigned)ctx->stream, (unsigned)ctx->frames_out, (unsigned)ctx->frames_dropped);
}

/* Open, format and fill the stream; pl_out_start() turns it on. */
static int pl_out_config(isp_4k_ctx_t *ctx)
{
	struct rts_video_format format;
	int ret;
	int i;

	ret = rtscam_video_open(ctx->stream);
	if (ret != 0) {
		RTK_LOGE(TAG, "s%u open failed %d\r\n", (unsigned)ctx->stream, ret);
		return ret;
	}
	ctx->opened = 1;
	memset(&format, 0, sizeof(format));
	format.pixelformat = ctx->is_rgb ? V4L2_PIX_FMT_RGB24 : V4L2_PIX_FMT_NV12;
	format.width = ctx->width;
	format.height = ctx->height;
	ret = rtscam_video_set_format(ctx->stream, &format);
	if (ret != 0) {
		RTK_LOGE(TAG, "s%u set_format failed %d\r\n", (unsigned)ctx->stream, ret);
		goto fail;
	}
	/* Allocate all buffers before the first enqueue: the driver latches
	 * the hardware slot count on that call and does not update it later.
	 */
	for (i = 0; i < ctx->nbufs; i++) {
		ctx->bufs[i] = rtscam_video_buffer_alloc(ctx->stream, RTS_MEMORY_KERNEL, pl_out_buf_size(ctx));
		if (!ctx->bufs[i]) {
			RTK_LOGE(TAG, "s%u buffer[%d] alloc failed\r\n", (unsigned)ctx->stream, i);
			ret = -ENOMEM;
			goto fail;
		}
	}
	for (i = 0; i < ctx->nbufs; i++) {
		ret = rtscam_video_enqueue(ctx->stream, ctx->bufs[i]);
		if (ret != 0) {
			RTK_LOGE(TAG, "s%u buffer[%d] enqueue failed %d\r\n", (unsigned)ctx->stream, i, ret);
			goto fail;
		}
	}
	memset(ctx->leased_item, 0, sizeof(ctx->leased_item));
	memset(ctx->output_state, 0, sizeof(ctx->output_state));
	ctx->frames_out = 0;
	ctx->frames_dropped = 0;
	return 0;

fail:
	pl_out_close(ctx);
	return ret;
}

static int pl_out_start(isp_4k_ctx_t *ctx)
{
	int ret = rtscam_video_stream_start(ctx->stream);

	if (ret != 0) {
		RTK_LOGE(TAG, "s%u stream start failed %d\r\n", (unsigned)ctx->stream, ret);
		return ret;
	}
	RTK_LOGI(TAG, "s%u started: %ux%u\r\n", (unsigned)ctx->stream, (unsigned)ctx->width, (unsigned)ctx->height);
	return 0;
}

static void pl_out_complete(isp_4k_ctx_t *ctx, int result)
{
	ctx->result = result;
	ctx->request = PL_REQ_NONE;
	rtos_sema_give(ctx->done);
}

/* Worker-only, once per loop. Zoom enable/disable completes only after an
 * ISP pass, which this worker injects, so the control task opens and closes
 * the stream. DETACH completes once every lease has returned.
 */
static int pl_out_service(isp_4k_ctx_t *ctx)
{
	uint32_t bit = 1U << (ctx - pl_outs);
	struct rts_video_buffer *buf;

	if (!ctx->in_use) {
		return 0;
	}
	if (ctx->request == PL_REQ_ATTACH) {
		ctx->detaching = 0;
		ctx->attached = 1;
		pl_new_outputs |= bit;
		pl_out_complete(ctx, 0);
		return 1;
	}
	if (!ctx->attached) {
		if (ctx->request == PL_REQ_DETACH) {
			pl_out_complete(ctx, 0);
		}
		return 0;
	}
	if (ctx->request == PL_REQ_DETACH) {
		ctx->detaching = 1;
	}
	if (ctx->detaching) {
		mm_context_t *mctx = ctx->parent;
		mm_queue_item_t *item;

		pl_pass_pending &= ~bit;
		while (!ctx->closing && rtos_queue_receive(mctx->output_ready, &item, 0) == SUCCESS) {
			isp_release_item(ctx, item, item->size);
			rtos_queue_send(mctx->output_recycle, &item, 0);
		}
		if (pl_out_recycle(ctx, 0) != 0) {
			return 0;
		}
		ctx->attached = 0;
		ctx->detaching = 0;
		if (ctx->request == PL_REQ_DETACH) {
			pl_out_complete(ctx, 0);
		}
		return 1;
	}
	pl_out_recycle(ctx, 1);
	/* closing is set by isp_destroy() after MMF already deleted the queues.
	 * Keep the frame unleased: publishing would touch freed queues.
	 */
	if (ctx->closing) {
		return 0;
	}
	if (rtscam_video_dequeue(ctx->stream, &buf) != 0) {
		return 0;
	}
	/* Driver timestamps use the same tick epoch, in microseconds. A frame done
	 * before this pass was injected is a late one from a timed-out pass: it is
	 * still published but does not complete the current pass.
	 */
	if ((int32_t)((uint32_t)(buf->timestamp / 1000ULL) - pl_pass_start_ms) >= 0) {
		pl_pass_pending &= ~bit;
	}
	if (pl_out_publish(ctx, buf) == 0) {
		if (++ctx->frames_out == 1) {
			RTK_LOGI(TAG, "first %s ready: %ux%u\r\n", ctx->is_rgb ? "RGB" : "NV12", (unsigned)ctx->width, (unsigned)ctx->height);
		}
	} else {
		ctx->frames_dropped++;
		if (rtscam_video_enqueue(ctx->stream, buf) != 0) {
			RTK_LOGE(TAG, "s%u unpublished requeue failed\r\n", (unsigned)ctx->stream);
			ctx->detaching = 1;
		}
	}
	return 1;
}

/* Attached outputs that must deliver the next pass. */
static uint32_t pl_out_attached_mask(void)
{
	uint32_t mask = 0;

	for (int i = 0; i < PL_OUT_NUM; i++) {
		if (pl_outs[i].attached && !pl_outs[i].detaching) {
			mask |= 1U << i;
		}
	}
	return mask;
}

/* Inject only when every attached output has a driver-owned buffer, so a
 * slow consumer throttles the source instead of losing passes.
 */
static int pl_out_ready(void)
{
	for (int i = 0; i < PL_OUT_NUM; i++) {
		isp_4k_ctx_t *ctx = &pl_outs[i];

		if (ctx->attached && !ctx->detaching && pl_out_recycle(ctx, 1) >= ctx->nbufs) {
			return 0;
		}
	}
	return 1;
}

/* Worker exit: drain leases while the linkers still run and answer any
 * request the worker can no longer serve. Streams stay open until each
 * output's STOP closes them.
 */
static void pl_out_finish(void)
{
	for (int i = 0; i < PL_OUT_NUM; i++) {
		isp_4k_ctx_t *ctx = &pl_outs[i];

		if (ctx->in_use && ctx->attached) {
			ctx->detaching = 1;
			while (!pl_out_service(ctx) && ctx->attached) {
				rtos_time_delay_ms(1);
			}
		}
		ctx->attached = 0;
		ctx->detaching = 0;
		if (ctx->request != PL_REQ_NONE) {
			pl_out_complete(ctx, ctx->request == PL_REQ_ATTACH ? -EIO : 0);
		}
	}
	pl_pass_pending = 0;
}

/* PPU calls are synchronous and task-owned. Keep them off the scheduler
 * so an output IRQ/release can be serviced while PPU is still working.
 * request/done semaphores publish the job and result across tasks.
 */
static void pl_ppu_task(void *arg)
{
	isp_4k_src_t *source = arg;
	int ret = img_nv16_rotate_90_init();

	source->ppu_result = ret;
	rtos_sema_give(source->ppu_done);
	if (ret != 0) {
		rtos_task_delete(NULL);
		return;
	}
	for (;;) {
		img_t src = {0};
		img_t dst = {0};
		uint32_t start_ms;

		rtos_sema_take(source->ppu_request, RTOS_MAX_TIMEOUT);
		if (source->ppu_exit) {
			break;
		}
		src.width = PL_RAW_W;
		src.height = PL_RAW_H;
		/* VIP HOST buffers take CPU pointers; the ISP DMA pool is identity-mapped on VP. */
		src.data = source->ppu_raw->vm_addr;
		dst.width = PL_INPUT_W;
		dst.height = PL_INPUT_H;
		dst.data = pl_vi_bufs[0]->vm_addr;
		start_ms = (uint32_t)rtos_time_get_current_system_time_ms();
		source->ppu_result = img_nv16_rotate_90(&src, &dst);
		source->ppu_ms = (uint32_t)rtos_time_get_current_system_time_ms() - start_ms;
		__DSB();
		rtos_sema_give(source->ppu_done);
	}
	source->ppu_result = img_nv16_rotate_90_deinit();
	rtos_sema_give(source->ppu_done);
	rtos_task_delete(NULL);
}

static int pl_ppu_start(isp_4k_src_t *source)
{
	int ret = -ENOMEM;

	source->ppu_pending = 0;
	source->ppu_exit = 0;
	if (rtos_sema_create_binary(&source->ppu_request) != 0) {
		return ret;
	}
	if (rtos_sema_create_binary(&source->ppu_done) != 0) {
		rtos_sema_delete(source->ppu_request);
		return ret;
	}
	if (rtos_task_create(&source->ppu_worker, "isp_4k_ppu", pl_ppu_task,
						 source, 4096, 1) == 0) {
		rtos_sema_take(source->ppu_done, RTOS_MAX_TIMEOUT);
		ret = source->ppu_result;
		if (ret == 0) {
			return 0;
		}
	}
	rtos_sema_delete(source->ppu_done);
	rtos_sema_delete(source->ppu_request);
	return ret;
}

static void pl_ppu_stop(isp_4k_src_t *source)
{
	/* Never free RAW/VI memory or destroy the runtime during a rotation. */
	if (source->ppu_pending) {
		rtos_sema_take(source->ppu_done, RTOS_MAX_TIMEOUT);
		source->ppu_pending = 0;
	}
	source->ppu_exit = 1;
	rtos_sema_give(source->ppu_request);
	rtos_sema_take(source->ppu_done, RTOS_MAX_TIMEOUT);
	if (source->ppu_result != 0) {
		RTK_LOGE(TAG, "PPU deinit failed (%d)\r\n", source->ppu_result);
	}
	rtos_sema_delete(source->ppu_done);
	rtos_sema_delete(source->ppu_request);
	source->ppu_raw = NULL;
}

/* Snapshot an output stream before teardown erases evidence of a missing frame. */
static void pl_log_output_timeout(uint32_t streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	uint32_t hw_status = 0;
	int ret = rtscam_get_video_in_buf_status(streamid, &hw_status);

	RTK_LOGW(TAG, "s%u timeout: hw_status=0x%08x status_ret=%d\r\n", (unsigned)streamid, (unsigned)hw_status, ret);
	if (!stream) {
		return;
	}
	RTK_LOGI(TAG, "s%u: frames=%u skipped=%u errors=%u overflow=%u abnormal=%u\r\n",
			 (unsigned)streamid, (unsigned)stream->frame_count, (unsigned)stream->skip_count,
			 (unsigned)stream->error_count, (unsigned)stream->overflow_count,
			 (unsigned)stream->abnormal_count);
	RTK_LOGI(TAG, "s%u: streaming=%u delay=%u fps=%u/%u skip(m/n/flag/index)=%d/%d/%d/%d\r\n",
			 (unsigned)streamid, (unsigned)stream->streaming, (unsigned)stream->delay,
			 (unsigned)stream->fps.user_actual.denominator,
			 (unsigned)stream->fps.user_actual.numerator,
			 stream->fps.skip_info.m, stream->fps.skip_info.n,
			 stream->fps.skip_info.flag, stream->fps.skip_info.index);
	for (unsigned i = 0; i < stream->buf_nums && i < RTSCAM_MAX_BUFFER_NUM; i++) {
		if (stream->vbuf[i]) {
			RTK_LOGI(TAG, "s%u buffer[%u]: state=%d bytesused=%u\r\n", (unsigned)streamid, i, stream->vbuf[i]->state, (unsigned)stream->vbuf[i]->buf.bytesused);
		}
	}
}

/* RAW/VI each have one buffer; NV12 buffers are leased independently.
 * Return RAW after PPU finishes, before submitting VERIFY. A queued RAW
 * slot is not an ISP busy indication. Retain completed RAW/rotation under
 * backpressure.
 */
static void pl_loop_task(isp_4k_src_t *source)
{
	struct rts_video_buffer *raw_ready = NULL;
	struct rts_video_buffer *buf;
	pl_frame_timing_t raw_timing = {0}, rotated_timing = {0}, input_timing = {0};
	int raw_queued = 1; /* Setup armed stream 0 once. */
	int rotated_ready = 0;
	int c_inflight = 0;
	int output_pending = 0;
	int timing_pair_valid = 1;
	uint32_t output_wait_start = 0;
	uint32_t isp_start_ms = 0;
	uint32_t isp_input_done_ms = 0;
	uint32_t previous_output_ms = 0;
	uint32_t seq = 0;
	uint32_t raw_frames = 0;

	while (!pl_stop_flag) {
		int did_work = 0;
		int ret;

		for (int i = 0; i < PL_OUT_NUM; i++) {
			did_work |= pl_out_service(&pl_outs[i]);
		}

		/* C releases VI independently of the NV12 output completion. */
		if (c_inflight) {
			ret = rtscam_isp_input_dequeue(&buf);
			if (ret == 0) {
				c_inflight = 0;
				did_work = 1;
				if (buf->bytesused == 0) {
					/* Driver timeout is not proof DMA stopped reading VI.
					 * Stop instead of allowing PPU to overwrite that input.
					 */
					pl_frames_timeout++;
					RTK_LOGW(TAG, "ISP input timeout: seq=%u, stopping\r\n", (unsigned)seq);
					pl_stop_flag = 1;
					break;
				}
				if (seq == 1) {
					/* The first VERIFY input after stream start primes the ISP and
					 * produces no output frame; inject the next RAW instead of
					 * waiting PL_OUTPUT_WAIT_MS for it.
					 */
					pl_pass_pending = 0;
				} else {
					output_pending = 1;
				}
				output_wait_start = (uint32_t)rtos_time_get_current_system_time_ms();
				isp_input_done_ms = output_wait_start - isp_start_ms;
				PL_DIAG_LOG("ISP input done: seq=%u\r\n", (unsigned)seq);
			}
		}

		/* A pass is complete once every output attached at injection has
		 * dequeued its frame (or detached).
		 */
		if (output_pending) {
			if (!pl_pass_pending) {
				uint32_t now = (uint32_t)rtos_time_get_current_system_time_ms();
				uint32_t interval = pl_frames_out ? now - previous_output_ms : 0;
				output_pending = 0;
				did_work = 1;
				pl_frames_out++;
				if (PL_TIMING_LOG_EN || pl_diag_log_en) {
					RTK_LOGI(TAG, "timing latest_input_seq=%u raw_capture_wait=%u ms "
							 "ppu_rotate=%u ms rotated_wait=%u ms isp_raw16_to_nv12=%u ms "
							 "isp_input_done=%u ms nv12_wait=%u ms raw_dequeue_lag=%u ms "
							 "output_interval=%u ms pair_valid=%d\r\n",
							 (unsigned)seq, (unsigned)input_timing.capture_ms,
							 (unsigned)input_timing.rotate_ms,
							 (unsigned)(isp_start_ms - input_timing.ready_ms),
							 (unsigned)(now - isp_start_ms),
							 (unsigned)isp_input_done_ms,
							 (unsigned)(now - output_wait_start),
							 (unsigned)input_timing.raw_dequeue_lag_ms,
							 (unsigned)interval, timing_pair_valid);
				}
				previous_output_ms = now;
			} else if ((uint32_t)rtos_time_get_current_system_time_ms() - output_wait_start >= PL_OUTPUT_WAIT_MS) {
				/* A pass can end without a frame. Output buffers stay armed (driver-owned);
				 * the next injection pushes them out. Never reclaim output DMA memory.
				 */
				if (pl_diag_log_en) {
					for (int i = 0; i < PL_OUT_NUM; i++) {
						if (pl_pass_pending & (1U << i)) {
							pl_log_output_timeout(pl_outs[i].stream);
						}
					}
				}
				pl_output_timeouts++;
				pl_pass_pending = 0;
				timing_pair_valid = 0;
				output_pending = 0;
				did_work = 1;
				RTK_LOGW(TAG, "NV12 wait timeout: seq=%u, retry next RAW\r\n", (unsigned)seq);
			}
		}

		/* B completion frees RAW; VI now holds a frame waiting for C. */
		if (source->ppu_pending && rtos_sema_take(source->ppu_done, 0) == 0) {
			source->ppu_pending = 0;
			source->ppu_raw = NULL;
			did_work = 1;
			if (source->ppu_result != 0) {
				/* Do not reuse either buffer after an unconfirmed PPU failure. */
				RTK_LOGE(TAG, "PPU rotation failed (%d), stopping\r\n", source->ppu_result);
				pl_stop_flag = 1;
			} else {
				rotated_timing.rotate_ms = source->ppu_ms;
				rotated_timing.ready_ms = (uint32_t)rtos_time_get_current_system_time_ms();
				rotated_ready = 1;
				PL_DIAG_LOG("PPU ready: rotate=%u ms\r\n", (unsigned)source->ppu_ms);
			}
		}
		if (pl_stop_flag) {
			break;
		}

		/* A's single slot is never rearmed until PPU has stopped reading it.
		 * Keep a captured frame rather than discarding it while VI is busy.
		 */
		if (raw_queued) {
			ret = rtscam_video_dequeue(0, &buf);
			if (ret == 0) {
				raw_queued = 0;
				raw_ready = buf;
				raw_timing.capture_ms = (uint32_t)rtos_time_get_current_system_time_ms() - pl_raw_queued_ms;
				/* Driver timestamps use microseconds on the same OS-tick epoch
				 * as rtos_time_get_current_system_time_ms().
				 */
				raw_timing.raw_dequeue_lag_ms = rtos_time_get_current_system_time_ms() -
												(uint32_t)(buf->timestamp / 1000ULL);
				did_work = 1;
				if (raw_frames++ == 0) {
					uint32_t now = (uint32_t)rtos_time_get_current_system_time_ms();
					/* SYSTIMER (TIM0) counts from the bootloader. Its clock source can be
					 * switched at runtime, so query the rate as mmf2_mediatime_pro3.c does. */
					uint32_t systick = SYSTIMER_TickGet();
					uint32_t clk_hz = PERI_ClkGet(CLKID_PON_TIMER0);

					RTK_LOGI(TAG, "first RAW ready: %u ms after stream 0 start, %u ms after setup start (t=%u ms)\r\n",
							 (unsigned)raw_timing.capture_ms, (unsigned)(now - pl_setup_start_ms), (unsigned)now);
					RTK_LOGI(TAG, "first RAW SYSTIMER: tick=%u, %u us\r\n", (unsigned)systick,
							 (unsigned)(clk_hz ? (uint64_t)systick * 1000000ULL / clk_hz : 0));
				}
				PL_DIAG_LOG("RAW ready: capture_wait=%u ms\r\n", (unsigned)raw_timing.capture_ms);
			}
		}

		/* B needs only a captured RAW and a free VI buffer, not a free NV12.
		 * It can also run while a previous C waits for its output DMA tail.
		 */
		if (raw_ready && !source->ppu_pending && !rotated_ready && !c_inflight) {
			rotated_timing = raw_timing;
			source->ppu_raw = raw_ready;
			raw_ready = NULL;
			source->ppu_pending = 1;
			rtos_sema_give(source->ppu_request);
			did_work = 1;
		}

		/* Rearm RAW as soon as PPU releases it, BEFORE submitting C.
		 * Waiting for NV12 here delays the next sensor frame.
		 * Never rearm a held or PPU-owned RAW.
		 */
		if (!raw_queued && !raw_ready && !source->ppu_pending) {
			pl_raw_queued_ms = (uint32_t)rtos_time_get_current_system_time_ms();
			ret = rtscam_video_enqueue(0, pl_s0_bufs[0]);
			if (ret != 0) {
				RTK_LOGE(TAG, "RAW enqueue failed (%d), stopping\r\n", ret);
				pl_stop_flag = 1;
				break;
			}
			raw_queued = 1;
			did_work = 1;
		}
		/* Submit only with no ISP transaction pending. pl_out_ready() also reaps
		 * consumer releases, even while PPU is running. After an output timeout
		 * the output slots stay armed and receive the next injection's frame.
		 */
		if (rotated_ready && !c_inflight && !output_pending && pl_out_ready()) {
			pl_vi_bufs[0]->bytesused = PL_VERIFY_BUF_SIZE;
			isp_start_ms = (uint32_t)rtos_time_get_current_system_time_ms();
			pl_pass_start_ms = isp_start_ms;
			pl_pass_pending = pl_out_attached_mask() & ~pl_new_outputs;
			pl_new_outputs = 0;
			ret = rtscam_isp_input_enqueue(pl_vi_bufs[0]);
			if (ret != 0) {
				RTK_LOGE(TAG, "ISP input enqueue failed (%d), stopping\r\n", ret);
				pl_stop_flag = 1;
				break;
			}
			input_timing = rotated_timing;
			rotated_ready = 0;
			c_inflight = 1;
			seq++;
			did_work = 1;
		}

		if (!did_work) {
			rtos_time_delay_ms(1);
		}
	}

	/* Join PPU before releasing its RAW/VI memory. Drain downstream leases
	 * even on an internal error (no external STOP caller is required).
	 */
	pl_ppu_stop(source);
	pl_out_finish();
	pl_stop_resources();
	source->active = 0;
	rtos_task_delete(NULL);
}

/* Keep geometry-dependent ISP modules bypassed: sensor calibration and
 * reference windows have not been transformed into the rotated coordinates.
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

/* Bring up the ISP driver and server once; the server starts asynchronously
 * (sensor probe included) and setup waits for RTS_ISP_RUNNING.
 */
static int pl_hw_init(void)
{
	int ret;

	if (pl_hw_initialized) {
		return 0;
	}
	/* Fail loudly instead of running the 4K pipeline with normal-mode clocks, sensor and pool. */
	if (!ISP_Get4KMode()) {
		RTK_LOGE(TAG, "ISP 4K mode not selected before ISP_Init()\r\n");
		return -EINVAL;
	}
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	/* Fast camera start already brought the ISP up in stage1. */
	extern int video_fcs_isp_stage1_is_started(void);
	if (video_fcs_isp_stage1_is_started()) {
		pl_hw_initialized = 1;
		return 0;
	}
#endif
	/* Common video clocks are initialized by the VP application. */
	ISP_Init();
	ret = isp_initial(&pl_io);
	if (ret != 0) {
		return ret;
	}
	ret = rtscam_video_init();
	if (ret != 0) {
		return ret;
	}
	pl_hw_initialized = 1;
	return 0;
}

/* ------------------------------------------------------------------ */
/* Setup task                                                           */
/* ------------------------------------------------------------------ */

static void pl_setup_task(void *arg)
{
	isp_4k_ctx_t *first = arg;
	isp_4k_src_t *source = first->src;
	struct rts_video_format format;
	int i;
	int ret;
	uint32_t init_start;

	pl_setup_start_ms = (uint32_t)rtos_time_get_current_system_time_ms();
	ret = pl_hw_init();
	if (ret != 0) {
		goto done;
	}
	init_start = rtos_time_get_current_system_time_ms();
	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		if (pl_stop_flag || (uint32_t)(rtos_time_get_current_system_time_ms() - init_start) >= 5000U) {
			ret = -ETIMEDOUT;
			goto done;
		}
		rtos_time_delay_ms(10);
	}

	ret = pl_ppu_start(source);
	if (ret != 0) {
		RTK_LOGE(TAG, "PPU init failed (%d)\r\n", ret);
		goto done;
	}

	ret = rts_isp_set_large_image_mode(ISP0, 1);
	if (ret != 0) {
		RTK_LOGE(TAG, "large image mode failed %d\r\n", ret);
		goto close_ppu;
	}

	ret = pl_set_module_bypass(1);
	if (ret != 0) {
		RTK_LOGE(TAG, "module bypass failed %d\r\n", ret);
		goto disable_mode;
	}

	/* Bring up the input path first -- starting it drives the sensor
	 * into the large-image geometry and forces a SoC video
	 * detach+attach, which would wipe out any capture stream opened
	 * beforehand (mirrors mmf2_video_example_test_vin_large_image.c).
	 */
	ret = rtscam_video_open(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		RTK_LOGE(TAG, "input open failed %d\r\n", ret);
		goto disable_mode;
	}

	for (i = 0; i < PL_VI_BUFS; i++) {
		uint32_t vi_phy = 0;

		/* USERPTR allocates only a descriptor. Allocate its pixel storage from
		 * the ISP DMA pool; PPU writes and ISP reads it without CPU access.
		 */
		pl_vi_mem[i] = rtscam_mem_alloc(PL_VERIFY_BUF_SIZE, &vi_phy,
										RTS_MEM_ATTR_CACHE, "li_verify");
		if (!pl_vi_mem[i]) {
			RTK_LOGE(TAG, "verify-input mem alloc[%d] failed\r\n", i);
			goto fail_verify_bufs;
		}
		pl_vi_bufs[i] = rtscam_video_buffer_alloc(RTSCAM_INPUT_STRM_IDX,
						RTS_MEMORY_USERPTR,
						PL_VERIFY_BUF_SIZE);
		if (!pl_vi_bufs[i]) {
			RTK_LOGE(TAG, "verify-input buffer alloc[%d] failed\r\n", i);
			goto fail_verify_bufs;
		}
		/* RTS_MEMORY_USERPTR: driver allocates no backing memory;
		 * the caller supplies and owns it (PPU -> ISP zero-copy).
		 */
		pl_vi_bufs[i]->vm_addr = (uint8_t *)pl_vi_mem[i];
		pl_vi_bufs[i]->phy_addr = vi_phy;
	}

	/* Even-sized GRBG RAW rotated clockwise 90 degrees becomes BGGR. */
	ret = rtscam_isp_input_set_behavior(1, ISP_INPUT_SBGGR16,
										PL_INPUT_W, PL_INPUT_H,
										PL_INPUT_FPS_NUM, PL_INPUT_FPS_DEN);
	if (ret != 0) {
		RTK_LOGE(TAG, "set_behavior failed %d\r\n", ret);
		goto fail_verify_bufs;
	}

	/* set_behavior() above only stores parameters; s_stream(1) below
	 * dispatches INPUT_EXEC_SET_BEHAVIOR, which drives the sensor
	 * geometry change / force_reset described above.
	 */
	ret = rtscam_video_stream_start(RTSCAM_INPUT_STRM_IDX);
	if (ret != 0) {
		RTK_LOGE(TAG, "input stream start failed %d\r\n", ret);
		goto fail_verify_bufs;
	}

	/* Capture streams are opened only after the input stream is
	 * running, so the force_reset above cannot wipe their open state.
	 */
	ret = rtscam_video_open(0);
	if (ret != 0) {
		RTK_LOGE(TAG, "stream 0 open failed %d\r\n", ret);
		goto fail_input_started;
	}
	memset(&format, 0, sizeof(format));
	/* Stream 0 has no RAW16 fourcc. YUYV supplies the required 16-bit
	 * pitch/size bookkeeping; large-image mode captures Bayer RAW bytes.
	 */
	format.pixelformat = V4L2_PIX_FMT_YUYV;
	format.width = PL_CAPTURE_W;
	format.height = PL_CAPTURE_H;
	ret = rtscam_video_set_format(0, &format);
	if (ret != 0) {
		RTK_LOGE(TAG, "stream 0 set_format failed %d\r\n", ret);
		goto fail_stream0_opened;
	}
	/* Allocate all buffers before the first enqueue: the driver latches
	 * the hardware slot count on that call and does not update it later.
	 */
	for (i = 0; i < PL_S0_BUFS; i++) {
		pl_s0_bufs[i] = rtscam_video_buffer_alloc(0, RTS_MEMORY_KERNEL,
						PL_STREAM0_BUF_SIZE);
		if (!pl_s0_bufs[i]) {
			RTK_LOGE(TAG, "stream 0 buffer[%d] alloc failed\r\n", i);
			goto fail_stream0_configured;
		}
	}
	for (i = 0; i < PL_S0_BUFS; i++) {
		if (rtscam_video_enqueue(0, pl_s0_bufs[i]) != 0) {
			RTK_LOGE(TAG, "stream 0 buffer[%d] enqueue failed\r\n", i);
			goto fail_stream0_configured;
		}
	}

	/* The first output's stream is configured before the first injection. */
	ret = pl_out_config(first);
	if (ret != 0) {
		goto fail_stream0_configured;
	}

	pl_raw_queued_ms = (uint32_t)rtos_time_get_current_system_time_ms();
	ret = rtscam_video_stream_start(0);
	if (ret != 0) {
		RTK_LOGE(TAG, "stream 0 start failed %d\r\n", ret);
		goto fail_out_configured;
	}
	ret = pl_out_start(first);
	if (ret != 0) {
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
			RTK_LOGE(TAG, "set output position failed %d\r\n", ret);
			goto fail_stream0_started;
		}
	}

	first->attached = 1;
	pl_pass_pending = 0;
	pl_new_outputs = 0;
	pl_frames_out = 0;
	pl_frames_timeout = 0;
	pl_output_timeouts = 0;
	source->startup_result = 0;
	rtos_sema_give(source->startup);
	RTK_LOGI(TAG, "started: capture %ux%u, "
			 "rotated %ux%u, first output s%u\r\n",
			 (unsigned)PL_CAPTURE_W, (unsigned)PL_CAPTURE_H,
			 (unsigned)PL_INPUT_W, (unsigned)PL_INPUT_H, (unsigned)first->stream);
	/* Continue scheduling ISP and MMF; PPU has its own task. */
	pl_loop_task(source);
	return;

fail_stream0_started:
	rtscam_video_stream_stop(0);
fail_out_configured:
	pl_out_close(first);
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
close_ppu:
	pl_ppu_stop(source);
done:
	source->startup_result = ret ? ret : -EIO;
	RTK_LOGE(TAG, "setup failed (%d)\r\n", source->startup_result);
	source->active = 0;
	rtos_sema_give(source->startup);
	rtos_task_delete(NULL);
}

/* Worker teardown, after the last MMF lease is returned. Output streams are
 * closed by their own STOP.
 */
static void pl_stop_resources(void)
{
	rtscam_video_stream_stop(0);
	rtscam_video_stream_stop(RTSCAM_INPUT_STRM_IDX);

	pl_release_stream_bufs(0, pl_s0_bufs, PL_S0_BUFS);
	pl_release_verify_bufs();

	rtscam_video_close(0);
	rtscam_video_close(RTSCAM_INPUT_STRM_IDX);

	pl_set_module_bypass(0);
	rts_isp_set_large_image_mode(ISP0, 0);

	RTK_LOGI(TAG, "stopped: passes=%u timeouts=%u output_timeouts=%u c_drop=%u\r\n",
			 (unsigned)pl_frames_out, (unsigned)pl_frames_timeout,
			 (unsigned)pl_output_timeouts, (unsigned)rtscam_isp_input_get_drop_count());
}

/* Post one request to the worker. If the worker exits first, it answers
 * pending requests in pl_out_finish(); the poll only covers a request posted
 * after that final answer. On timeout the request stays posted, so the worker
 * keeps detaching and a retried STOP picks up where this one left off.
 */
static int pl_out_request(isp_4k_ctx_t *ctx, int request, uint32_t timeout_ms)
{
	uint32_t start_ms = (uint32_t)rtos_time_get_current_system_time_ms();

	rtos_sema_take(ctx->done, 0);
	ctx->result = -EIO;
	ctx->request = request;
	while (rtos_sema_take(ctx->done, 100) != 0) {
		if (!ctx->src->active) {
			ctx->request = PL_REQ_NONE;
			ctx->attached = 0;
			return request == PL_REQ_ATTACH ? -EIO : 0;
		}
		if (timeout_ms != RTOS_MAX_TIMEOUT &&
			(uint32_t)rtos_time_get_current_system_time_ms() - start_ms >= timeout_ms) {
			RTK_LOGW(TAG, "s%u stop timeout: lease not returned\r\n", (unsigned)ctx->stream);
			return -ETIMEDOUT;
		}
	}
	return ctx->result;
}

/* First output: setup opens its stream before the first injection. */
static int pl_src_start(isp_4k_ctx_t *ctx)
{
	isp_4k_src_t *source = ctx->src;

	rtos_sema_take(source->startup, 0);
	pl_stop_flag = 0;
	source->active = 1;
	source->startup_result = -EIO;
	if (rtos_task_create(&source->worker, "isp_4k", pl_setup_task, ctx, 4096, 1) != 0) {
		source->active = 0;
		return -ENOMEM;
	}
	rtos_sema_take(source->startup, RTOS_MAX_TIMEOUT);
	return source->startup_result;
}

/* Later output: open the stream here, not in the worker, because zoom enable
 * waits for an ISP pass. The first pass can take seconds after the source
 * starts, so wait for it.
 */
static int pl_out_attach(isp_4k_ctx_t *ctx)
{
	int ret;

	for (uint32_t ms = 0; !pl_frames_out; ms += 10) {
		if (ms >= PL_ATTACH_WAIT_MS || !ctx->src->active || pl_stop_flag) {
			RTK_LOGW(TAG, "s%u attach: no ISP pass yet\r\n", (unsigned)ctx->stream);
			return -ETIMEDOUT;
		}
		rtos_time_delay_ms(10);
	}
	ret = pl_out_config(ctx);
	if (ret == 0) {
		ret = pl_out_start(ctx);
	}
	if (ret == 0) {
		ret = pl_out_request(ctx, PL_REQ_ATTACH, RTOS_MAX_TIMEOUT);
	}
	if (ret != 0) {
		pl_out_close(ctx);
	}
	return ret;
}

static int pl_out_apply(isp_4k_ctx_t *ctx)
{
	mm_context_t *mctx = ctx->parent;
	mm_queue_item_t *item;
	int dynamic;
	int ret;

	if (ctx->applied) {
		return -EBUSY;
	}
	for (int i = 0; i < PL_OUT_NUM; i++) {
		if (pl_outs[i].applied && pl_outs[i].stream == ctx->stream) {
			return -EBUSY;
		}
	}
	if (!mctx->output_ready || !mctx->output_recycle) {
		return -EINVAL;
	}
	if (rtos_queue_receive(mctx->output_recycle, &item, 0) != SUCCESS) {
		return -EINVAL;
	}
	dynamic = (item->flag & MMQI_FLAG_DYNAMIC) != 0;
	rtos_queue_send(mctx->output_recycle, &item, 0);
	if (!dynamic) {
		return -EINVAL;
	}
	if (ctx->src->active) {
		ret = pl_stop_flag ? -ENODEV : pl_out_attach(ctx);
	} else if (pl_src_users) {
		/* The source stopped on an error; stop the other output first. */
		ret = -EIO;
	} else {
		ret = pl_src_start(ctx);
	}
	if (ret == 0) {
		ctx->applied = 1;
		pl_src_users++;
	}
	return ret;
}

/* Detach while the source still injects passes, so the zoom disable in
 * pl_out_close() can complete; the last output then stops the source.
 * STOP must run while the MMF queues still exist. On timeout nothing is
 * freed: the worker keeps waiting for the lease, and the caller must keep
 * linkers/modules alive and may retry STOP later.
 */
static int pl_out_stop(isp_4k_ctx_t *ctx, uint32_t timeout_ms)
{
	uint32_t start_ms;
	int ret;

	if (ctx->attached) {
		ret = pl_out_request(ctx, PL_REQ_DETACH, timeout_ms);
		if (ret != 0) {
			return ret;
		}
	}
	pl_out_close(ctx);
	if (!ctx->applied) {
		return 0;
	}
	if (pl_src_users == 1) {
		start_ms = (uint32_t)rtos_time_get_current_system_time_ms();
		pl_stop_flag = 1;
		while (ctx->src->active) {
			if (timeout_ms != RTOS_MAX_TIMEOUT &&
				(uint32_t)rtos_time_get_current_system_time_ms() - start_ms >= timeout_ms) {
				RTK_LOGW(TAG, "stop timeout: NV12 lease or PPU job not returned\r\n");
				return -ETIMEDOUT;
			}
			rtos_time_delay_ms(1);
		}
	}
	ctx->applied = 0;
	pl_src_users--;
	return 0;
}

static int isp_control(void *p, int cmd, int arg)
{
	isp_4k_ctx_t *ctx = p;

	switch (cmd) {
	case CMD_ISP_4K_SET_STREAM:
		return ctx->applied ? -EBUSY : pl_out_set_stream(ctx, arg);
	case CMD_ISP_4K_APPLY:
		return pl_out_apply(ctx);
	case CMD_ISP_4K_STREAM_STOP:
		return pl_out_stop(ctx, PL_STOP_WAIT_MS);
	case CMD_ISP_4K_LOG_EN:
		pl_diag_log_en = arg != 0;
		return 0;
	default:
		return -EINVAL;
	}
}

static void *isp_create(void *parent)
{
	isp_4k_ctx_t *ctx = NULL;
	rtos_sema_t done;

	for (int i = 0; i < PL_OUT_NUM; i++) {
		if (!pl_outs[i].in_use) {
			ctx = &pl_outs[i];
			break;
		}
	}
	if (!ctx) {
		return NULL;
	}
	/* Semaphores live as long as the static contexts. */
	if (!pl_src.startup && rtos_sema_create_binary(&pl_src.startup) != 0) {
		return NULL;
	}
	done = ctx->done;
	if (!done && rtos_sema_create_binary(&done) != 0) {
		return NULL;
	}
	memset(ctx, 0, sizeof(*ctx));
	ctx->done = done;
	ctx->parent = parent;
	ctx->src = &pl_src; /* After the memset: every output shares the one source. */
	pl_out_set_stream(ctx, ISP_4K_MAIN_STREAM_IDX);
	ctx->in_use = 1;
	return ctx;
}

static void *isp_destroy(void *p)
{
	isp_4k_ctx_t *ctx = p;
	int ret;

	/* MMF deletes queues BEFORE destroy: retire leases without touching them.
	 * Normal shutdown issues CMD_ISP_4K_STREAM_STOP before close; without it
	 * the worker may already have used the deleted queues.
	 */
	if (ctx->applied) {
		RTK_LOGW(TAG, "s%u closed without STREAM_STOP\r\n", (unsigned)ctx->stream);
	}
	ctx->closing = 1;
	/* Bounded so mm_module_close() cannot hang on a lease that will never
	 * return (e.g. its linker was already deleted). On timeout the context
	 * stays in_use: isp_create() would memset it while the worker may still
	 * read it, so burning the slot is safer than handing it out again.
	 */
	ret = pl_out_stop(ctx, PL_STOP_WAIT_MS);
	if (ret != 0) {
		RTK_LOGE(TAG, "s%u destroy timed out (%d): context left in use; "
				 "a later open must not reuse it\r\n", (unsigned)ctx->stream, ret);
		return NULL;
	}
	ctx->in_use = 0;
	return NULL;
}

/* Pixel storage belongs to ISP; static MMF item allocation is unsupported. */
static void *isp_new_item(void *p)
{
	(void)p;
	return NULL;
}

static void *isp_del_item(void *p, void *d)
{
	(void)p;
	(void)d;
	/* Keep this callback: MM_CMD_CLEAR_QUEUE_ITEMS gates vrelease_item
	 * on del_item being non-NULL. Pixel memory still belongs to ISP.
	 */
	return NULL;
}

mm_module_t isp_4k_module = {
	.create = isp_create,
	.destroy = isp_destroy,
	.control = isp_control,
	.handle = NULL,
	.new_item = isp_new_item,
	.del_item = isp_del_item,
	.rsz_item = NULL,
	.vrelease_item = isp_release_item,
	.output_type = MM_TYPE_VDSP,
	.module_type = MM_TYPE_VSRC,
	.name = "ISP_4K"
};
