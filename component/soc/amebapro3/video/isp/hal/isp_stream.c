/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include "diag.h"
#include <FreeRTOS.h>
#include <task.h>
#include "ameba_soc.h"
#include "rts_camera.h"
#include "rts_video.h"
#include "isp_stream.h"

/* ------------------------------------------------------------------ */
/* Stream context (full definition — internal to this TU)             */
/* ------------------------------------------------------------------ */

struct isp_stream {
	uint32_t  ch;
	uint32_t  buf_count;

	struct rts_video_buffer *bufs[ISP_STREAM_MAX_BUFS];

	void *recycler_task;
	int   active;

	/* Optional frame-ready callback invoked from the recycler task before
	 * the buffer is re-enqueued.  NULL disables it.  Survives isp_stream_init().
	 * @param phy_addr  Physical address of the Y plane (UV follows at +luma_size).
	 * Return 0 to re-enqueue; non-zero to take ownership of the buffer. */
	int (*s_isp_ch_handle)(int stream_id, int buf_id, uint32_t phy_addr);

	/* user_release mode: recycler never auto-enqueues after the callback.
	 * User must call isp_buf_release() / isp_stream_release_buf() instead. */
	int      user_release;

	/* Watchdog: if a held buffer is not released within this period the
	 * recycler force-enqueues it.  0 = disabled (default). */
	uint32_t release_timeout_ms;

	/* Per-slot tracking for held buffers in user_release mode. */
	struct {
		struct rts_video_buffer *buf;
		uint32_t handoff_tick;   /* xTaskGetTickCount() at handoff */
	} held_bufs[ISP_STREAM_MAX_BUFS];
};

/* ------------------------------------------------------------------ */
/* Buffer recycler task                                                */
/* ------------------------------------------------------------------ */

static void isp_stream_recycler(void *arg)
{
	isp_stream_t *ctx = (isp_stream_t *)arg;
	struct rts_video_buffer *buf;
	int ret;
	uint32_t i;

	while (ctx->active) {
		/* Watchdog: check every iteration for buffers held too long. */
		if (ctx->release_timeout_ms) {
			uint32_t now = xTaskGetTickCount();
			for (i = 0; i < ISP_STREAM_MAX_BUFS; i++) {
				if (!ctx->held_bufs[i].buf) {
					continue;
				}
				uint32_t held_ms =
					(now - ctx->held_bufs[i].handoff_tick) *
					portTICK_PERIOD_MS;
				if (held_ms > ctx->release_timeout_ms) {
					rtscam_video_enqueue(ctx->ch,
										 ctx->held_bufs[i].buf);
					ctx->held_bufs[i].buf = NULL;
				}
			}
		}

		if (rtscam_video_dequeue(ctx->ch, &buf) == 0 && buf) {
			if (ctx->s_isp_ch_handle) {
				ret = ctx->s_isp_ch_handle((int)ctx->ch,
										   buf->index,
										   buf->phy_addr);
				if (ctx->user_release) {
					/* user owns the buffer; track it for watchdog */
					ctx->held_bufs[buf->index].buf = buf;
					ctx->held_bufs[buf->index].handoff_tick =
						xTaskGetTickCount();
					continue;
				}
				if (ret != 0) {
					continue;    /* callback took ownership */
				}
			}
			rtscam_video_enqueue(ctx->ch, buf);
		} else {
			vTaskDelay(pdMS_TO_TICKS(1));
		}
	}

	vTaskDelete(NULL);
}

/* ------------------------------------------------------------------ */
/* Internal helpers (static)                                           */
/* ------------------------------------------------------------------ */
struct rtscam_video_buffer *rbuf[4];

static int isp_stream_alloc_buf(isp_stream_t *ctx, uint32_t pixfmt,
								uint32_t width, uint32_t height,
								uint32_t slot_num,
								uint32_t ar_feature, uint32_t ar_align_w)
{
	uint32_t i;
	size_t frame_size;
	u32 align_w = 4096;
	u32 tmp_buf_fr_com_len;
	u32 tmp_buf_slice_len;
	int ret;

	if (!ctx || slot_num == 0 || slot_num > ISP_STREAM_MAX_BUFS) {
		return -1;
	}

	if (pixfmt == V4L2_PIX_FMT_RGB24) {
		frame_size = (size_t)width * height * 3;
	} else if (pixfmt == V4L2_PIX_FMT_NV16) {
		frame_size = (size_t)width * height * 4 / 2;
	} else if (pixfmt == V4L2_PIX_FMT_NV12) {
		if (ar_feature == 2 && ar_align_w != 0) {
			tmp_buf_slice_len = (ar_align_w * height + align_w + ar_align_w * height / 2 + align_w);
			frame_size = tmp_buf_slice_len;
		} else if (ar_feature == 1 && ar_align_w != 0) {
			tmp_buf_fr_com_len = (ar_align_w * height + align_w + ar_align_w * height / 2 + align_w);
			frame_size = tmp_buf_fr_com_len;
		} else {
			frame_size = (size_t)width * height * 3 / 2;
		}
	} else {
		frame_size = 0;
	}

	frame_size = (frame_size + 15) & ~15U;

	DiagPrintf("[isp_stream] ch%u alloc %u buf(s) size=%u\r\n",
			   ctx->ch, slot_num, (unsigned)frame_size);

	if (ar_feature == 2 && ar_align_w != 0) {
		ret = rtscam_video_slice_buffer_alloc(ctx->ch, RTS_MEMORY_KERNEL,
											  frame_size, ar_align_w, slot_num, rbuf);
		if (ret) {
			DiagPrintf("[isp_stream] buffer alloc fail\r\n");
			goto err_cleanup;
		}
		for (i = 0; i < slot_num; i++) {
			ctx->bufs[i] = &rbuf[i]->buf;
			ctx->buf_count++;

			DiagPrintf("[isp_stream] ch%u buf[%u] phy=0x%08x\r\n",
					   ctx->ch, i, rbuf[i]->buf.phy_addr);
		}
	} else if (ar_feature == 1 && ar_align_w != 0) {
		ret = rtscam_video_fr_compress_buffer_alloc(ctx->ch, RTS_MEMORY_KERNEL,
				frame_size * slot_num, ar_align_w, slot_num, rbuf);
		if (ret) {
			DiagPrintf("[isp_stream] buffer alloc fail\r\n");
			goto err_cleanup;
		}
		for (i = 0; i < slot_num; i++) {
			ctx->bufs[i] = &rbuf[i]->buf;
			ctx->buf_count++;

			DiagPrintf("[isp_stream] ch%u buf[%u] phy=0x%08x\r\n",
					   ctx->ch, i, rbuf[i]->buf.phy_addr);
		}

	} else {
		for (i = 0; i < slot_num; i++) {
			struct rts_video_buffer *buf;

			buf = rtscam_video_buffer_alloc(ctx->ch, RTS_MEMORY_KERNEL,
											frame_size);
			if (!buf) {
				DiagPrintf("[isp_stream] buf_alloc[%u], frame_size:%u fail\r\n", i, frame_size);
				DiagPrintf("[isp_stream] Free heap = %u bytes\n", (unsigned int)xPortGetFreeHeapSize());
				goto err_cleanup;
			}
			ctx->bufs[i] = buf;
			ctx->buf_count++;

			DiagPrintf("[isp_stream] ch%u buf[%u] phy=0x%08x\r\n",
					   ctx->ch, i, buf->phy_addr);
		}
	}

	/* Enqueue after all buffers are allocated so that buf_nums is fully
	 * populated when rtscam_init_capture_buffers fires on the first enqueue,
	 * ensuring slot_num = min(buf_nums, RTSCAM_SOC_HW_SLOT_NUM) = 2. */
	for (i = 0; i < slot_num; i++) {
		if (rtscam_video_enqueue(ctx->ch, ctx->bufs[i]) != 0) {
			DiagPrintf("[isp_stream] enqueue buf[%u] fail\r\n", i);
			goto err_cleanup;
		}
	}
	return 0;

err_cleanup:
	for (i = 0; i < ctx->buf_count; i++) {
		if (ctx->bufs[i]) {
			rtscam_video_buffer_release(ctx->ch, ctx->bufs[i]);
			ctx->bufs[i] = NULL;
		}
	}
	ctx->buf_count = 0;
	return -1;
}

int isp_get_buf_idx(int stream_id, void *buf_addr)
{
	int ret = -1;
	for (int j = 0; j < 2; j++) {
		void *curr_buf = (void *)rbuf[j]->buf.phy_addr;
		if (buf_addr == curr_buf) {
			ret = j;
		}
	}
	return ret;
}

static int isp_stream_start_recycler(isp_stream_t *ctx)
{
	if (!ctx) {
		return -1;
	}

	ctx->active = 1;
	xTaskCreate(isp_stream_recycler, "isp_recycle",
				configMINIMAL_STACK_SIZE * 4, ctx,
				(portPRIVILEGE_BIT | ((configMAX_PRIORITIES / 2) - 3)),
				(TaskHandle_t *)&ctx->recycler_task);
	return 0;
}

static void isp_stream_destroy(isp_stream_t *ctx)
{
	struct rts_video_buffer *buf;
	uint32_t i = 0;

	if (!ctx || !ctx->buf_count) {
		return;
	}

	ctx->active = 0;
	if (ctx->recycler_task) {
		vTaskDelay(pdMS_TO_TICKS(20));
		ctx->recycler_task = NULL;
	}

	/* Return any buffers held in user_release mode so the drain can find them. */
	for (i = 0; i < ISP_STREAM_MAX_BUFS; i++) {
		if (ctx->held_bufs[i].buf) {
			rtscam_video_enqueue(ctx->ch, ctx->held_bufs[i].buf);
			ctx->held_bufs[i].buf = NULL;
		}
	}

	while (rtscam_video_dequeue(ctx->ch, &buf) == 0 && buf)
		;

	for (i = 0; i < ctx->buf_count; i++) {
		if (ctx->bufs[i]) {
			rtscam_video_buffer_release(ctx->ch, ctx->bufs[i]);
			ctx->bufs[i] = NULL;
		}
	}

	DiagPrintf("[isp_stream] ch%u released %u buf(s)\r\n",
			   ctx->ch, ctx->buf_count);

	ctx->buf_count = 0;
}

/* ------------------------------------------------------------------ */
/* Per-channel context array (owned here)                             */
/* ------------------------------------------------------------------ */

static isp_stream_t g_stream_ctx[STREAM_CNT_MAX];

isp_stream_t *isp_stream_get_ctx(uint32_t ch)
{
	if (ch < 0 || ch >= STREAM_CNT_MAX) {
		return NULL;
	}
	return &g_stream_ctx[ch];
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

static int g_video_inited;

static int ensure_video_init(void)
{
	if (!g_video_inited) {
		int ret = rtscam_video_init();

		if (ret) {
			return ret;
		}
		g_video_inited = 1;
	}

	return 0;
}

int isp_stream_init(isp_stream_t *ctx, uint32_t ch)
{
	int (*saved_handler)(int, int, uint32_t);

	if (!ctx) {
		return -1;
	}

	saved_handler = ctx->s_isp_ch_handle;
	memset(ctx, 0, sizeof(*ctx));
	ctx->s_isp_ch_handle = saved_handler;
	ctx->ch = ch;
	return 0;
}

void isp_stream_dump_caps_by_channel(int ch)
{
	struct rts_video_caps caps = {0};
	int i = 0;

	if (ensure_video_init()) {
		return;
	}

	if (rtscam_video_get_caps(ch, &caps)) {
		DiagPrintf("[isp_stream] ch%u: get_caps fail (non-fatal)\r\n",
				   ch);
		return;
	}

	DiagPrintf("- Capabilities:\n");
	while (caps.format_caps[i].pixelformat) {
		const struct rts_video_format_cap *fcap = &caps.format_caps[i];
		DiagPrintf("  %c%c%c%c width [%u; %u; %u] height [%u; %u; %u] fps [%u/%u; %u/%u; %u/%u]\n",
				   (char)fcap->pixelformat,
				   (char)(fcap->pixelformat >> 8),
				   (char)(fcap->pixelformat >> 16),
				   (char)(fcap->pixelformat >> 24),
				   fcap->width_min, fcap->width_max, fcap->width_step,
				   fcap->height_min, fcap->height_max, fcap->height_step,
				   fcap->fps_min.denominator, fcap->fps_min.numerator,
				   fcap->fps_max.denominator, fcap->fps_max.numerator,
				   fcap->fps_step.denominator, fcap->fps_step.numerator);
		i++;
	}
}

int isp_stream_open(isp_stream_t *ctx, uint32_t pixfmt,
					uint32_t width, uint32_t height,
					uint32_t fps, uint32_t slot_num,
					uint32_t ar_feature, uint32_t ar_align_w)
{
	struct rts_video_format fmt = {
		.pixelformat = pixfmt,
		.width       = width,
		.height      = height,
	};
	struct rts_video_fps vfps = {
		.numerator   = 1,
		.denominator = fps,
	};

	int ret;

	if (!ctx) {
		return -EINVAL;
	}

	ret = ensure_video_init();
	if (ret) {
		return ret;
	}

	ret = rtscam_video_open(ctx->ch);
	if (ret) {
		DiagPrintf("[isp_stream] open ch%u fail: %d\r\n", ctx->ch, ret);
		return ret;
	}

	ret = rtscam_video_set_format(ctx->ch, &fmt);
	if (ret) {
		DiagPrintf("[isp_stream] set_format ch%u fail: %d\r\n", ctx->ch, ret);
		goto err_close;
	}

	if (fps) {
		ret = rtscam_video_set_fps(ctx->ch, &vfps);
		if (ret)
			DiagPrintf("[isp_stream] set_fps ch%u fail: %d (non-fatal)\r\n",
					   ctx->ch, ret);
	}

	if (!slot_num) {
		slot_num = 2;
	}

	if (ar_feature != 0 && ar_align_w != 0) {
		rtscam_video_set_remap_info(ctx->ch, ar_feature, ar_align_w);
	}

	ret = isp_stream_alloc_buf(ctx, pixfmt, width, height, slot_num, ar_feature, ar_align_w);
	if (ret) {
		DiagPrintf("[isp_stream] alloc_buf ch%u fail: %d\r\n", ctx->ch, ret);
		goto err_close;
	}

	DiagPrintf("[isp_stream] ch%u calling stream_start\r\n", ctx->ch);
	ret = rtscam_video_stream_start(ctx->ch);
	if (ret) {
		DiagPrintf("[isp_stream] stream_start ch%u fail: %d\r\n", ctx->ch, ret);
		goto err_destroy;
	}

	ret = isp_stream_start_recycler(ctx);
	if (ret) {
		DiagPrintf("[isp_stream] start_recycler ch%u fail: %d\r\n", ctx->ch, ret);
		rtscam_video_stream_stop(ctx->ch);
		goto err_destroy;
	}

	return 0;

err_destroy:
	isp_stream_destroy(ctx);
err_close:
	rtscam_video_close(ctx->ch);
	return ret;
}

int isp_stream_close(isp_stream_t *ctx)
{
	int ret;

	if (!ctx) {
		return -EINVAL;
	}

	ret = rtscam_video_stream_stop(ctx->ch);
	if (ret) {
		DiagPrintf("[isp_stream] stream_stop ch%u fail: %d\r\n", ctx->ch, ret);
	}

	isp_stream_destroy(ctx);

	rtscam_video_close(ctx->ch);
	return ret;
}

void isp_stream_register_handler(isp_stream_t *ctx,
								 int (*handler)(int stream_id, int buf_id,
										 uint32_t phy_addr))
{
	if (!ctx) {
		return;
	}
	ctx->s_isp_ch_handle = handler;
}

void isp_stream_recycle(isp_stream_t *ctx, int buf_id)
{
	if (!ctx || buf_id < 0 || (uint32_t)buf_id >= ctx->buf_count) {
		return;
	}
	ctx->held_bufs[buf_id].buf = NULL;
	rtscam_video_enqueue(ctx->ch, ctx->bufs[buf_id]);
}

void isp_stream_release_buf(isp_stream_t *ctx, void *buf_addr, int buf_id)
{
	struct rts_video_buffer *buf = NULL;
	uint32_t i;

	if (!ctx) {
		return;
	}

	if (buf_id >= 0 && (uint32_t)buf_id < ctx->buf_count) {
		/* Direct index lookup */
		buf = ctx->bufs[buf_id];
		ctx->held_bufs[buf_id].buf = NULL;
	} else if (buf_id == -1 && buf_addr) {
		/* Scan by physical address */
		uint32_t phy = (uint32_t)(uintptr_t)buf_addr;
		for (i = 0; i < ctx->buf_count; i++) {
			if (ctx->bufs[i] && ctx->bufs[i]->phy_addr == phy) {
				buf = ctx->bufs[i];
				ctx->held_bufs[i].buf = NULL;
				break;
			}
		}
	}

	if (buf) {
		rtscam_video_enqueue(ctx->ch, buf);
	}
}

void isp_stream_set_user_release(isp_stream_t *ctx, int enable)
{
	if (!ctx) {
		return;
	}
	ctx->user_release = enable;
}

void isp_stream_set_release_timeout(isp_stream_t *ctx, uint32_t timeout_ms)
{
	if (!ctx) {
		return;
	}
	ctx->release_timeout_ms = timeout_ms;
}
