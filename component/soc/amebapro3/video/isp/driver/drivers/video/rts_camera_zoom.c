/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS:
 * - LOG_MODULE_REGISTER     -> removed
 * - sys_read32/write32      -> readl/writel
 * - k_mutex_lock/unlock     -> isp_rtos_mutex_lock/unlock
 * - k_mutex_init            -> isp_rtos_mutex_init
 * - CONTAINER_OF            -> container_of
 * - k_usleep(1000)          -> vTaskDelay(pdMS_TO_TICKS(1))
 * - DT_INST_REG_ADDR(0)     -> CONFIG_RTS_ZOOM_SYS_BASE
 * - reset_line_toggle_dt    -> rtscam_rtos_reset_control_reset
 * - clock_control_*         -> no-op (clk_refcnt tracking only)
 * - DT stream/filter        -> static CONFIG_RTS_ZOOM_* macros
 *
 * rts3921 porting changes vs rts3917:
 * - Sub-stream register spacing: ZOOM_V1_CH_REG_OFFSET/STEP (was 0x80 flat)
 * - Async zoom-down worker task replacing kthread_worker
 * - Sample pre-downscaler for sub-streams
 * - DISABLE_ZOOM = 0x0 (was 0x2)
 * - IVE_REG_BASE = 0x800 (was 0x11000)
 * - Split set_zoom into set_zoom_up (ch0) and set_zoom_down_reg (ch1+)
 */

#include <errno.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_iomem.h>
#include <isp_rtos_completion.h>
#include "rts_camera_zoom.h"
#include "rts_camera_zoom_regs.h"
#include "rts_camera_soc.h"
#include "rts_camera_hw_config.h"
#include "hal_isp.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define BT601_FULL_RANGE 1
#define BT709_FULL_RANGE 3

#define RTSCAM_YUV_MAX_STRM_NUM   4
#define RTSCAM_MAX_STREAM_NUM     (RTSCAM_YUV_MAX_STRM_NUM + 1)
#define RTSCAM_RGB_YONLY_STRM_IDX (RTSCAM_MAX_STREAM_NUM - 1)
#define RTS_ZOOM_RGB_YONLY_STREAM_ID RTSCAM_RGB_YONLY_STRM_IDX

/* format type bits — must match rts_camera_soc.c */
#define RTSCAM_FORMAT_TYPE_YUV420_SEMIPLANAR (1 << 0)
#define RTSCAM_FORMAT_TYPE_YUV422_SEMIPLANAR (1 << 1)
#define RTSCAM_FORMAT_TYPE_RGB               (1 << 2)
#define RTSCAM_FORMAT_TYPE_Y_ONLY            (1 << 3)
#define RTSCAM_FORMAT_TYPE_YUYV              (1 << 4)
#define RTSCAM_FORMAT_TYPE_YVYU              (1 << 5)
#define RTSCAM_FORMAT_TYPE_128BIT_3PIXEL     (1 << 6)

/* Worker runs below ISP IRQ task but above idle */
#define ZOOM_WORKER_TASK_PRIORITY \
	(portPRIVILEGE_BIT | (tskIDLE_PRIORITY + 2))

static inline uint32_t rtscam_zoom_read_reg(struct rtscam_zoom_dev *zoom_dev,
		uint32_t offset)
{
	return readl(zoom_dev->base + offset);
}

static inline void rtscam_zoom_write_reg(struct rtscam_zoom_dev *zoom_dev,
		uint32_t value, uint32_t offset)
{
	writel(value, zoom_dev->base + offset);
}

/* ------------------------------------------------------------------ */
/* Filter tables                                                        */
/* ------------------------------------------------------------------ */

#define ZOOM_FILTER_COEF_NUM 20
static const uint8_t zoom_filter[6][ZOOM_FILTER_COEF_NUM] = {
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x20, 0x40, 0x60, 0xa0, 0xc0, 0xe0, 0xff
	},
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20,
		0x30, 0x40, 0x40, 0x50, 0x60, 0x70, 0x80, 0x80, 0x80, 0x80
	},
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x15, 0x20, 0x35, 0x40,
		0x4b, 0x55, 0x55, 0x55, 0x55, 0x55, 0x56, 0x56, 0x55, 0x56
	},
	{
		0x08, 0x10, 0x18, 0x20, 0x20, 0x28, 0x30, 0x38, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
	},
	{
		0x20, 0x26, 0x2d, 0x33, 0x33, 0x3a, 0x40, 0x46, 0x33, 0x33,
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x33, 0x34
	},
	{
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34
	},
};

static const uint8_t *get_zoom_filter(uint32_t scale)
{
	int i;
	const uint32_t thd[] = { 64 * 16, 128 * 16, 192 * 16, 256 * 16,
							 320 * 16
						   };

	for (i = 0; i < (int)ARRAY_SIZE(thd); i++) {
		if (scale <= thd[i]) {
			break;
		}
	}
	return zoom_filter[i];
}

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static struct rtscam_zoom_dev *
rtscam_zoom_get_zoom_dev(struct rtscam_zoom *zoom)
{
	struct rtscam_zoom(*ptr)[RTS_ZOOM_MAX_STREAM];

	ptr = (struct rtscam_zoom(*)[RTS_ZOOM_MAX_STREAM])(
			  (uintptr_t)zoom - zoom->stream_id * sizeof(struct rtscam_zoom));
	return container_of(ptr, struct rtscam_zoom_dev, zoom);
}

static int rtscam_zoom_set_zoom_filter(struct rtscam_zoom *zoom, int stream_id,
									   uint32_t scale)
{
	unsigned long i;
	uint32_t offset;
	const uint8_t *filter = get_zoom_filter(scale);

	(void)stream_id;

	for (i = 0, offset = ZOOM_CH0_COEF0 + zoom->base;
		 i < ZOOM_FILTER_COEF_NUM; i += 4, offset += 4) {
		uint32_t filter_value;

		filter_value = (filter[i] | filter[i + 1] << 8 |
						filter[i + 2] << 16 | filter[i + 3] << 24);
		writel(filter_value, offset);
	}

	return 0;
}

/*
 * Compute timeout ticks from current ISP FPS (3× frame period).
 * Falls back to 300 ms if ISP info is unavailable.
 */
static uint32_t rtscam_zoom_get_timeout_ticks(struct rtscam_zoom_dev *zoom_dev)
{
	struct rtscam_soc_dev *soc =
		container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);
	struct rtscam_isp_frame_info info = {0};
	uint32_t timeout_ms = 300;

	rtscam_isp_get_frame_info(soc->isp, &info);
	if (info.fps_max.denominator && info.fps_max.numerator)
		timeout_ms = 3000u * info.fps_max.numerator /
					 info.fps_max.denominator;

	return pdMS_TO_TICKS(timeout_ms);
}

/*
 * Poll ZOOM_SYS_CONTROL bit 3 until the hardware trigger is idle.
 * Returns non-zero on success (trigger clear), 0 on timeout.
 */
static int rtscam_zoom_wait_trig(struct rtscam_zoom_dev *zoom_dev)
{
	struct rtscam_soc_dev *soc =
		container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);
	struct rtscam_isp_frame_info info = {0};
	uint32_t frame_ms = 33; /* default ~30fps */
	int cnt = 5;

	rtscam_isp_get_frame_info(soc->isp, &info);
	if (info.fps_max.denominator && info.fps_max.numerator)
		frame_ms = 1000u * info.fps_max.numerator /
				   info.fps_max.denominator;

	while (cnt) {
		if (!((readl(ZOOM_SYS_CONTROL + zoom_dev->base) >> 3) & 1)) {
			break;
		}
		vTaskDelay(pdMS_TO_TICKS(frame_ms));
		cnt--;
	}
	return cnt;
}

/* ------------------------------------------------------------------ */
/* Sample pre-downscaler (ch1+ only)                                   */
/* ------------------------------------------------------------------ */

static int rtscam_zoom_calc_sample(struct rtscam_zoom_down_data *data)
{
	int sw = data->src_w / data->dest_w;
	int sh = data->src_h / data->dest_h;
	int s  = MIN(sw, sh);

	if (s <= 2) {
		return 1;
	}
	if (s <= 4) {
		return 2;
	}
	return 4;
}

/* bits [2:1] of ZOOM_SYS_CONTROL: 0=1×, 1=2×, 2=4× */
static void rtscam_zoom_set_sample_reg(struct rtscam_zoom_dev *zoom_dev,
									   int sample)
{
	uint32_t v = readl(ZOOM_SYS_CONTROL + zoom_dev->base) & ~0x6u;

	if (sample == 2) {
		v |= (1u << 1);
	} else if (sample >= 4) {
		v |= (2u << 1);
	}
	writel(v, ZOOM_SYS_CONTROL + zoom_dev->base);
}

/* ------------------------------------------------------------------ */
/* Register programming                                                 */
/* ------------------------------------------------------------------ */

/* ch0: zoom-up (or pan) — commits with ZOOM_CH0_REG_LOAD */
static int rtscam_zoom_set_zoom_up(struct rtscam_zoom *zoom, int stream_id)
{
	uint16_t src_w, src_h;
	uint16_t dest_w, dest_h;
	uint32_t scale_h, scale_v;
	uint32_t crop_start_x, crop_start_y;
	uint32_t ch_offset = zoom->base;
	struct rtscam_zoom_dev *zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	struct rtscam_soc_dev *soc =
		container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);
	struct rtscam_isp_frame_info isp_info = {0};

	rtscam_isp_get_frame_info(soc->isp, &isp_info);

	crop_start_x = zoom->crop_info.start_x;
	crop_start_y = zoom->crop_info.start_y;
	src_w = zoom->crop_info.width;
	src_h = zoom->crop_info.height;

	if (src_w && src_h) {
		if (src_w + crop_start_x > isp_info.width ||
			src_h + crop_start_y > isp_info.height) {
			return -EINVAL;
		}
	} else {
		src_w = isp_info.width;
		src_h = isp_info.height;
		crop_start_x = 0;
		crop_start_y = 0;
	}

	dest_w = zoom->current_fmt.width;
	dest_h = zoom->current_fmt.height;
	if (!dest_w || !dest_h) {
		return -EINVAL;
	}

	scale_h = ((uint32_t)src_w << 10) / dest_w;
	scale_v = ((uint32_t)src_h << 10) / dest_h;
	if (scale_h > 0x1ffff || scale_v > 0x1ffff) {
		return -EINVAL;
	}

	writel((crop_start_y << 16 | crop_start_x),
		   ZOOM_CH0_CROP_START + ch_offset);
	writel(((uint32_t)dest_w | (uint32_t)dest_h << 16),
		   ZOOM_CH0_OUTPUT_SIZE + ch_offset);
	writel(scale_h, ZOOM_CH0_STEP_H + ch_offset);
	writel(scale_v, ZOOM_CH0_STEP_V + ch_offset);
	rtscam_zoom_set_zoom_filter(zoom, stream_id, MIN(scale_h, scale_v));
	writel(LOAD_ZOOM_PARAM, ZOOM_CH0_REG_LOAD + ch_offset);

	return 0;
}

/*
 * ch1+: program all enabled sub-stream registers (called from worker task).
 * All sub-streams are programmed atomically; caller sets ZOOM_SYS_CONTROL
 * bit 3 to trigger the atomic load.
 */
static int rtscam_zoom_set_zoom_down_reg(struct rtscam_zoom_dev *zoom_dev,
		int sample)
{
	int i;

	for (i = 0; i < (int)zoom_dev->stream_num - 1; i++) {
		struct rtscam_zoom *zoom = &zoom_dev->zoom[i + 1];
		struct rtscam_zoom_down_data *data = &zoom_dev->data[i];
		uint32_t base = zoom->base;

		if (data->enable) {
			uint32_t scale_h, scale_v, x, y;
			int s = (sample < 1) ? 1 : sample;

			x = data->x / (uint32_t)s;
			y = data->y / (uint32_t)s;
			scale_h = ((uint32_t)(data->src_w / s) << 10) /
					  data->dest_w;
			scale_v = ((uint32_t)(data->src_h / s) << 10) /
					  data->dest_h;

			if (scale_h > 0x1ffff || scale_v > 0x1ffff ||
				scale_h < 0x400   || scale_v < 0x400) {
				rtsprintk(RTS_TRACE_ERROR,
						  "zoom ch%d scale out of range "
						  "scale_h=0x%x scale_v=0x%x",
						  i + 1, scale_h, scale_v);
				return -EINVAL;
			}

			writel((y << 16 | x), ZOOM_CH0_CROP_START + base);
			writel(((uint32_t)data->dest_w |
					(uint32_t)data->dest_h << 16),
				   ZOOM_CH0_OUTPUT_SIZE + base);
			writel(scale_h, ZOOM_CH0_STEP_H + base);
			writel(scale_v, ZOOM_CH0_STEP_V + base);
			rtscam_zoom_set_zoom_filter(zoom, i + 1,
										MIN(scale_h, scale_v));
		}
		writel((uint32_t)data->enable, ZOOM_CH0_CTRL + base);
	}
	return 0;
}

/* ------------------------------------------------------------------ */
/* Worker task (replaces Linux kthread_worker)                          */
/* ------------------------------------------------------------------ */

static void rtscam_zoom_queue_work(struct rtscam_zoom_dev *zoom_dev,
								   uint8_t op)
{
	zoom_dev->worker_op = op;
	xTaskNotifyGive(zoom_dev->zoom_worker_handle);
}

/* Port of rtscam_zoom_down_work */
static void rtscam_zoom_do_down_work(struct rtscam_zoom_dev *zoom_dev)
{
	int i;
	uint8_t ids;
	int sample = 5; /* sentinel: no enabled sub-streams yet */
	int ret;

	if (!rtscam_zoom_wait_trig(zoom_dev)) {
		rtsprintk(RTS_TRACE_ERROR, "zoom wait_trig timeout");
		/* still drain pending_ids so callers don't hang */
		isp_rtos_mutex_lock(&zoom_dev->down_lock);
		ids = zoom_dev->pending_ids;
		zoom_dev->pending_ids = 0;
		isp_rtos_mutex_unlock(&zoom_dev->down_lock);
		goto complete;
	}

	isp_rtos_mutex_lock(&zoom_dev->down_lock);

	/* capture ids early so they are always completed on any exit */
	ids = zoom_dev->pending_ids;
	zoom_dev->pending_ids = 0;

	for (i = 0; i < (int)zoom_dev->stream_num - 1; i++) {
		if (!zoom_dev->data[i].enable) {
			continue;
		}
		int s = rtscam_zoom_calc_sample(&zoom_dev->data[i]);

		if (s < sample) {
			sample = s;
		}
	}

	ret = rtscam_zoom_set_zoom_down_reg(zoom_dev, sample);
	if (ret) {
		isp_rtos_mutex_unlock(&zoom_dev->down_lock);
		rtsprintk(RTS_TRACE_ERROR,
				  "zoom set_zoom_down_reg fail ret=%d", ret);
		goto complete;
	}

	if (sample != 5) {
		rtscam_zoom_set_sample_reg(zoom_dev, sample);
		iomem_set_bit(3, ZOOM_SYS_CONTROL + zoom_dev->base);
	}

	isp_rtos_mutex_unlock(&zoom_dev->down_lock);

complete:
	for (i = 0; i < (int)zoom_dev->stream_num - 1; i++) {
		if (ids & (1u << i)) {
			isp_rtos_complete(&zoom_dev->trig_completion[i]);
		}
	}
}

/* Port of rtscam_zoom_disable_work */
static void rtscam_zoom_do_disable_work(struct rtscam_zoom_dev *zoom_dev)
{
	if (!rtscam_zoom_wait_trig(zoom_dev)) {
		rtsprintk(RTS_TRACE_ERROR, "wait stream disable timeout");
	}
	isp_rtos_complete(&zoom_dev->disable_completion);
}

static void rtscam_zoom_worker_task(void *arg)
{
	struct rtscam_zoom_dev *zoom_dev = (struct rtscam_zoom_dev *)arg;

	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (zoom_dev->worker_op == ZOOM_WORK_DOWN) {
			rtscam_zoom_do_down_work(zoom_dev);
		} else {
			rtscam_zoom_do_disable_work(zoom_dev);
		}
	}
}

/* ------------------------------------------------------------------ */
/* Stream enable / disable                                              */
/* ------------------------------------------------------------------ */

static int rtscam_zoom_enable_zoom(struct rtscam_zoom *zoom, int stream_id)
{
	int ret;
	uint32_t tmp;
	struct rtscam_zoom_dev *zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	struct rtscam_soc_dev *soc =
		container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);

	/* Set image mode register (shared, addressed via zoom_dev) */
	if (zoom->current_fmt.fmt == RTSCAM_FORMAT_TYPE_YUV420_SEMIPLANAR) {
		tmp = rtscam_zoom_read_reg(zoom_dev, ZOOM_SYS_IMAGE_MODE_SEL);
		rtscam_zoom_write_reg(zoom_dev, tmp & ~(1u << stream_id),
							  ZOOM_SYS_IMAGE_MODE_SEL);
	} else if ((zoom->current_fmt.fmt ==
				RTSCAM_FORMAT_TYPE_YUV422_SEMIPLANAR) ||
			   (zoom->current_fmt.fmt == RTSCAM_FORMAT_TYPE_YUYV) ||
			   (zoom->current_fmt.fmt == RTSCAM_FORMAT_TYPE_YVYU) ||
			   (zoom->current_fmt.fmt ==
				RTSCAM_FORMAT_TYPE_128BIT_3PIXEL)) {
		tmp = rtscam_zoom_read_reg(zoom_dev, ZOOM_SYS_IMAGE_MODE_SEL);
		rtscam_zoom_write_reg(zoom_dev, tmp | (1u << stream_id),
							  ZOOM_SYS_IMAGE_MODE_SEL);
	} else if (zoom->current_fmt.fmt == RTSCAM_FORMAT_TYPE_RGB) {
		rtscam_zoom_write_reg(zoom_dev, 1, ZOOM_SYS_IMAGE_AI_CHAN_SEL);
	} else if (zoom->current_fmt.fmt == RTSCAM_FORMAT_TYPE_Y_ONLY) {
		rtscam_zoom_write_reg(zoom_dev, 0, ZOOM_SYS_IMAGE_AI_CHAN_SEL);
	} else {
		return -EINVAL;
	}

	if (stream_id == 0) {
		/* ch0: zoom-up, synchronous */
		ret = rtscam_zoom_set_zoom_up(zoom, stream_id);
		if (ret) {
			return ret;
		}
		writel(ENABLE_ZOOM, ZOOM_CH0_CTRL + zoom->base);
	} else {
		/* ch1+: zoom-down via worker task */
		struct rtscam_zoom_down_data *data =
				&zoom_dev->data[stream_id - 1];
		struct rtscam_isp_frame_info isp_info = {0};
		uint32_t timeout_ticks;

		rtscam_isp_get_frame_info(soc->isp, &isp_info);

		if (!isp_info.width || !isp_info.height) {
			rtsprintk(RTS_TRACE_ERROR,
					  "zoom ch%d ISP frame info not ready",
					  stream_id);
			return -EAGAIN;
		}

		isp_rtos_mutex_lock(&zoom_dev->down_lock);
		data->enable = ENABLE_ZOOM;
		data->dest_w = zoom->current_fmt.width;
		data->dest_h = zoom->current_fmt.height;
		if (zoom->crop_info.width && zoom->crop_info.height) {
			data->x     = zoom->crop_info.start_x;
			data->y     = zoom->crop_info.start_y;
			data->src_w = zoom->crop_info.width;
			data->src_h = zoom->crop_info.height;
		} else {
			data->x     = 0;
			data->y     = 0;
			data->src_w = isp_info.width;
			data->src_h = isp_info.height;
		}
		zoom_dev->pending_ids |= (1u << (stream_id - 1));
		rtscam_zoom_queue_work(zoom_dev, ZOOM_WORK_DOWN);
		isp_rtos_mutex_unlock(&zoom_dev->down_lock);

		timeout_ticks = rtscam_zoom_get_timeout_ticks(zoom_dev);
		ret = isp_rtos_wait_for_completion_timeout(
				  &zoom_dev->trig_completion[stream_id - 1],
				  timeout_ticks);
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "zoom ch%d enable timeout", stream_id);
			return ret;
		}
	}

	return ret;
}

static int rtscam_zoom_disable_zoom(struct rtscam_zoom *zoom, int stream_id)
{
	int ret = 0;
	struct rtscam_zoom_dev *zoom_dev = rtscam_zoom_get_zoom_dev(zoom);

	if (rtscam_isp_zoom_get_aov_mode()) {
		return 0;
	}

	if (stream_id == 0) {
		/* ch0: directly clear enable bit */
		writel(DISABLE_ZOOM, ZOOM_CH0_CTRL + zoom->base);
	} else {
		/* ch1+: route through worker for atomic trigger */
		isp_rtos_mutex_lock(&zoom_dev->down_lock);
		zoom_dev->data[stream_id - 1].enable = DISABLE_ZOOM;
		zoom_dev->pending_ids |= (1u << (stream_id - 1));
		rtscam_zoom_queue_work(zoom_dev, ZOOM_WORK_DOWN);
		isp_rtos_mutex_unlock(&zoom_dev->down_lock);

		ret = isp_rtos_wait_for_completion_timeout(
				  &zoom_dev->trig_completion[stream_id - 1],
				  rtscam_zoom_get_timeout_ticks(zoom_dev));
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "zoom down disable timeout ch%d", stream_id);
			return ret;
		}
	}

	return ret;
}

static int rtscam_zoom_enable_stream(struct rtscam_zoom *zoom, int stream_id)
{
	int ret;
	struct rtscam_zoom_dev *zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	struct rtscam_soc_dev *soc =
		container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);

	if (zoom_dev->streaming & (1UL << stream_id)) {
		return -EBUSY;
	}

	ret = rtscam_isp_zoom_set_stream_on(soc->isp);
	if (ret) {
		return ret;
	}

	ret = rtscam_zoom_enable_zoom(zoom, stream_id);
	if (ret) {
		rtscam_isp_zoom_set_stream_off(soc->isp);
		return ret;
	}

	zoom_dev->streaming |= (1UL << stream_id);
	return 0;
}

static int rtscam_zoom_disable_stream(struct rtscam_zoom *zoom, int stream_id)
{
	int ret;
	struct rtscam_zoom_dev *zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	struct rtscam_soc_dev *soc =
		container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);

	zoom_dev->streaming &= ~(1UL << stream_id);

	if (!zoom_dev->streaming) {
		/* Last stream off: wait for HW trigger to drain */
		rtscam_zoom_queue_work(zoom_dev, ZOOM_WORK_DISABLE);
		isp_rtos_wait_for_completion_timeout(
			&zoom_dev->disable_completion,
			rtscam_zoom_get_timeout_ticks(zoom_dev));
	}

	ret = rtscam_zoom_disable_zoom(zoom, stream_id);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "disable zoom at %d fail", stream_id);
		return ret;
	}
	return rtscam_isp_zoom_set_stream_off(soc->isp);
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int rtscam_zoom_soc_set_stream(struct rtscam_zoom *zoom, int enable)
{
	int ret;
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	if (enable) {
		zoom->set_clock(zoom, 1);
		ret = rtscam_zoom_enable_stream(zoom, zoom->stream_id);
		if (ret) {
			zoom->set_clock(zoom, 0);
		}
	} else {
		ret = rtscam_zoom_disable_stream(zoom, zoom->stream_id);
		if (!ret) {
			zoom->set_clock(zoom, 0);
		}
	}
	isp_rtos_mutex_unlock(&zoom_dev->lock);
	return ret;
}

int rtscam_zoom_soc_set_fmt(struct rtscam_zoom *zoom, uint32_t rts_code,
							uint32_t w, uint32_t h)
{
	int ret = 0;
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	if (!w || w > zoom->stream_fmt.width || !h ||
		h > zoom->stream_fmt.height ||
		(rts_code & zoom->stream_fmt.fmt) == 0) {
		ret = -EINVAL;
		goto out;
	}

	zoom->current_fmt.fmt   = rts_code;
	zoom->current_fmt.width = w;
	zoom->current_fmt.height = h;
out:
	isp_rtos_mutex_unlock(&zoom_dev->lock);
	return ret;
}

int rtscam_zoom_soc_set_ive_ctrl(struct rtscam_zoom *zoom,
								 struct rtscam_soc_ive_ctrl *ctrl)
{
	uint32_t val_norm_mean;
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom || !ctrl) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	val_norm_mean = (ctrl->normal_mean.r & 0xff) |
					((ctrl->normal_mean.g & 0xff) << 8) |
					((ctrl->normal_mean.b & 0xff) << 16);

	zoom->set_clock(zoom, 1);

	rtscam_zoom_write_reg(zoom_dev, val_norm_mean, IVE_NORM_MEAN);
	rtscam_zoom_write_reg(zoom_dev, ctrl->normal_scale & 0xffff,
						  IVE_NORM_SCALE);
	rtscam_zoom_write_reg(zoom_dev, ctrl->quant_len & 0x1f, IVE_QUANT_LEN);
	rtscam_zoom_write_reg(zoom_dev, ctrl->asym_inv_scale & 0x3ffff,
						  IVE_ASYM_INV_SCALE);
	rtscam_zoom_write_reg(zoom_dev, ctrl->asym_zero_point & 0xff,
						  IVE_ASYM_ZERO_POINT);
	rtscam_zoom_write_reg(zoom_dev, ctrl->enable, IVE_ENABLE);

	zoom->set_clock(zoom, 0);
	isp_rtos_mutex_unlock(&zoom_dev->lock);

	return 0;
}

int rtscam_zoom_soc_get_ive_ctrl(struct rtscam_zoom *zoom,
								 struct rtscam_soc_ive_ctrl *ctrl)
{
	uint32_t val_norm_mean;
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom || !ctrl) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	zoom->set_clock(zoom, 1);
	ctrl->enable = rtscam_zoom_read_reg(zoom_dev, IVE_ENABLE);
	val_norm_mean = rtscam_zoom_read_reg(zoom_dev, IVE_NORM_MEAN);
	ctrl->normal_mean.r = val_norm_mean & 0xff;
	ctrl->normal_mean.g = (val_norm_mean >> 8) & 0xff;
	ctrl->normal_mean.b = (val_norm_mean >> 16) & 0xff;
	ctrl->normal_scale = rtscam_zoom_read_reg(zoom_dev, IVE_NORM_SCALE) &
						 0xffff;
	ctrl->quant_len = rtscam_zoom_read_reg(zoom_dev, IVE_QUANT_LEN) & 0x1f;
	ctrl->asym_inv_scale =
		rtscam_zoom_read_reg(zoom_dev, IVE_ASYM_INV_SCALE) & 0x3ffff;
	ctrl->asym_zero_point =
		rtscam_zoom_read_reg(zoom_dev, IVE_ASYM_ZERO_POINT) & 0xff;
	zoom->set_clock(zoom, 0);
	isp_rtos_mutex_unlock(&zoom_dev->lock);

	return 0;
}

int rtscam_zoom_soc_set_crop(struct rtscam_zoom *zoom,
							 struct rtscam_subdev_crop_info *crop)
{
	struct rtscam_soc_dev *soc;
	struct rtscam_zoom_dev *zoom_dev;
	int sid;

	if (!zoom || !crop) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	soc = container_of(zoom_dev, struct rtscam_soc_dev, zoom_dev);

	{
		struct rtscam_isp_frame_info isp_info = {0};

		rtscam_isp_get_frame_info(soc->isp, &isp_info);
		if (isp_info.width && isp_info.height) {
			if (crop->start_x + crop->width > isp_info.width ||
				crop->start_y + crop->height > isp_info.height) {
				return -EINVAL;
			}
		}
	}

	if (!memcmp(&zoom->crop_info, crop, sizeof(*crop))) {
		return 0;
	}

	isp_rtos_mutex_lock(&zoom_dev->lock);
	memcpy(&zoom->crop_info, crop, sizeof(*crop));
	sid = zoom->stream_id;

	if (crop->mode) {
		if (sid == 0) {
			if (rtscam_zoom_set_zoom_up(zoom, sid)) {
				rtsprintk(RTS_TRACE_ERROR, "invalid crop info");
				isp_rtos_mutex_unlock(&zoom_dev->lock);
				return -EINVAL;
			}
		} else {
			/* Update data[] source crop and queue worker */
			isp_rtos_mutex_lock(&zoom_dev->down_lock);
			zoom_dev->data[sid - 1].x     = crop->start_x;
			zoom_dev->data[sid - 1].y     = crop->start_y;
			zoom_dev->data[sid - 1].src_w = crop->width;
			zoom_dev->data[sid - 1].src_h = crop->height;
			zoom_dev->pending_ids |= (1u << (sid - 1));
			rtscam_zoom_queue_work(zoom_dev, ZOOM_WORK_DOWN);
			isp_rtos_mutex_unlock(&zoom_dev->down_lock);

			isp_rtos_wait_for_completion_timeout(
				&zoom_dev->trig_completion[sid - 1],
				rtscam_zoom_get_timeout_ticks(zoom_dev));
		}
	}
	isp_rtos_mutex_unlock(&zoom_dev->lock);
	return 0;
}

int rtscam_zoom_soc_get_crop(struct rtscam_zoom *zoom,
							 struct rtscam_subdev_crop_info *crop)
{
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom || !crop) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	memcpy(crop, &zoom->crop_info, sizeof(*crop));
	isp_rtos_mutex_unlock(&zoom_dev->lock);
	return 0;
}

int rtscam_zoom_soc_get_stream_info(struct rtscam_zoom_dev *zoom_dev,
									struct rtscam_zoom_stream_info *stream_info)
{
	int i;

	if (!zoom_dev || !stream_info) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&zoom_dev->lock);
	stream_info->stream_num = zoom_dev->stream_num;
	for (i = 0; i < (int)stream_info->stream_num; i++) {
		stream_info->fmt[i] = zoom_dev->zoom[i].stream_fmt;
	}
	isp_rtos_mutex_unlock(&zoom_dev->lock);
	return 0;
}

int rtscam_zoom_soc_set_limit_color_range(struct rtscam_zoom *zoom,
		int *limit_enable)
{
	uint32_t val;
	int fmt;
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom || !limit_enable) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	zoom->set_clock(zoom, 1);
	fmt = zoom->current_fmt.fmt;
	if (fmt == RTSCAM_FORMAT_TYPE_RGB || fmt == RTSCAM_FORMAT_TYPE_Y_ONLY) {
		zoom->set_clock(zoom, 0);
		isp_rtos_mutex_unlock(&zoom_dev->lock);
		return -ERANGE;
	}
	val = rtscam_zoom_read_reg(zoom_dev, ZOOM_SYS_YUV_TRAN_CTRL);
	val &= ~(LIMIT_MASK << zoom->stream_id);
	if (*limit_enable) {
		val |= LIMIT_MASK << zoom->stream_id;
	}
	rtscam_zoom_write_reg(zoom_dev, val, ZOOM_SYS_YUV_TRAN_CTRL);
	zoom->set_clock(zoom, 0);
	isp_rtos_mutex_unlock(&zoom_dev->lock);

	return 0;
}

int rtscam_zoom_soc_get_limit_color_range(struct rtscam_zoom *zoom,
		int *limit_enable)
{
	uint32_t val;
	int fmt;
	struct rtscam_zoom_dev *zoom_dev;

	if (!zoom || !limit_enable) {
		return -EINVAL;
	}

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	isp_rtos_mutex_lock(&zoom_dev->lock);
	zoom->set_clock(zoom, 1);
	fmt = zoom->current_fmt.fmt;
	if (fmt == RTSCAM_FORMAT_TYPE_RGB || fmt == RTSCAM_FORMAT_TYPE_Y_ONLY) {
		zoom->set_clock(zoom, 0);
		isp_rtos_mutex_unlock(&zoom_dev->lock);
		return -ERANGE;
	}
	val = rtscam_zoom_read_reg(zoom_dev, ZOOM_SYS_YUV_TRAN_CTRL);
	*limit_enable = !!(val & (1u << zoom->stream_id));
	zoom->set_clock(zoom, 0);
	isp_rtos_mutex_unlock(&zoom_dev->lock);

	return 0;
}

int rtscam_zoom_set_color_range(struct rtscam_zoom_dev *zoom_dev, int range)
{
	uint32_t yr, yg, yb;
	uint32_t ur, ug, ub;
	uint32_t vr, vg, vb;
	uint32_t y_offset;

	if (!zoom_dev) {
		return -EINVAL;
	}
	if (range == BT601_FULL_RANGE) {
		yr = 256;
		ur = 0;
		vr = 359;
		yg = 256;
		ug = 88;
		vg = 183;
		yb = 256;
		ub = 454;
		vb = 0;
		y_offset = 0;
	} else if (range == BT709_FULL_RANGE) {
		yr = 256;
		ur = 0;
		vr = 403;
		yg = 256;
		ug = 48;
		vg = 120;
		yb = 256;
		ub = 475;
		vb = 0;
		y_offset = 0;
	} else {
		return -EINVAL;
	}

	rtscam_zoom_write_reg(zoom_dev, yg << 16 | yr, YUV2RGB0);
	rtscam_zoom_write_reg(zoom_dev, ur << 16 | yb, YUV2RGB1);
	rtscam_zoom_write_reg(zoom_dev, ub << 16 | ug, YUV2RGB2);
	rtscam_zoom_write_reg(zoom_dev, vg << 16 | vr, YUV2RGB3);
	rtscam_zoom_write_reg(zoom_dev, vb, YUV2RGB4);
	rtscam_zoom_write_reg(zoom_dev, y_offset, YUV2RGB_YOFFSET);
	return 0;
}

/* ------------------------------------------------------------------ */
/* Clock control stub                                                   */
/* ------------------------------------------------------------------ */

static int rtscam_zoom_dev_set_clock(struct rtscam_zoom_dev *zoom_dev,
									 int enable)
{
	if (enable) {
		zoom_dev->clk_refcnt++;
	} else if (zoom_dev->clk_refcnt > 0) {
		zoom_dev->clk_refcnt--;
	}
	return 0;
}

static int rtscam_zoom_set_clock(struct rtscam_zoom *zoom, int enable)
{
	struct rtscam_zoom_dev *zoom_dev;

	zoom_dev = rtscam_zoom_get_zoom_dev(zoom);
	return rtscam_zoom_dev_set_clock(zoom_dev, enable);
}

/* ------------------------------------------------------------------ */
/* Initialization                                                       */
/* ------------------------------------------------------------------ */

static int rtscam_zoom_init_streams(struct rtscam_zoom_dev *zoom_dev)
{
	const int mode_4k = ISP_Get4KMode();
	const struct rtscam_zoom_stream_fmt stream_fmts[] = {
#if defined(CONFIG_RTS_ZOOM_S0_FMT)
		{
			.fmt = CONFIG_RTS_ZOOM_S0_FMT,
			.width = mode_4k ? CONFIG_RTS_ZOOM_S0_WIDTH_4K : CONFIG_RTS_ZOOM_S0_WIDTH,
			.height = mode_4k ? CONFIG_RTS_ZOOM_S0_HEIGHT_4K : CONFIG_RTS_ZOOM_S0_HEIGHT
		},
#endif
#if defined(CONFIG_RTS_ZOOM_S1_FMT)
		{
			.fmt = CONFIG_RTS_ZOOM_S1_FMT,
			.width = CONFIG_RTS_ZOOM_S1_WIDTH,
			.height = mode_4k ? CONFIG_RTS_ZOOM_S1_HEIGHT_4K : CONFIG_RTS_ZOOM_S1_HEIGHT
		},
#endif
#if defined(CONFIG_RTS_ZOOM_S2_FMT)
		{
			.fmt = CONFIG_RTS_ZOOM_S2_FMT,
			.width = CONFIG_RTS_ZOOM_S2_WIDTH,
			.height = mode_4k ? CONFIG_RTS_ZOOM_S2_HEIGHT_4K : CONFIG_RTS_ZOOM_S2_HEIGHT
		},
#endif
#if defined(CONFIG_RTS_ZOOM_S3_FMT)
		{
			.fmt = CONFIG_RTS_ZOOM_S3_FMT,
			.width = CONFIG_RTS_ZOOM_S3_WIDTH,
			.height = mode_4k ? CONFIG_RTS_ZOOM_S3_HEIGHT_4K : CONFIG_RTS_ZOOM_S3_HEIGHT
		},
#endif
#if defined(CONFIG_RTS_ZOOM_S4_FMT)
		{
			.fmt = CONFIG_RTS_ZOOM_S4_FMT,
			.width = CONFIG_RTS_ZOOM_S4_WIDTH,
			.height = CONFIG_RTS_ZOOM_S4_HEIGHT
		},
#endif
	};
	int cnt = (int)ARRAY_SIZE(stream_fmts);
	int i;

	if (cnt == 0 || cnt > RTS_ZOOM_MAX_STREAM) {
		rtsprintk(RTS_TRACE_ERROR,
				  "invalid zoom stream count %d (max %d)",
				  cnt, RTS_ZOOM_MAX_STREAM);
		return -EINVAL;
	}

	for (i = 0; i < cnt; i++) {
		zoom_dev->zoom[i].stream_id = i;
		/*
		 * rts3921 V1 register layout:
		 *   ch0: zoom_dev->base (no extra offset)
		 *   ch1: zoom_dev->base + ZOOM_V1_CH_REG_OFFSET
		 *   chN: zoom_dev->base + ZOOM_V1_CH_REG_OFFSET + (N-1)*ZOOM_V1_CH_REG_STEP
		 * ZOOM_CH0_CTRL etc. are then added on top of this base.
		 */
		if (i == 0) {
			zoom_dev->zoom[i].base = zoom_dev->base;
		} else
			zoom_dev->zoom[i].base = zoom_dev->base +
									 ZOOM_V1_CH_REG_OFFSET +
									 (i - 1) * ZOOM_V1_CH_REG_STEP;
		zoom_dev->zoom[i].dt_stream_fmt = stream_fmts[i];
		zoom_dev->zoom[i].set_clock = rtscam_zoom_set_clock;
		zoom_dev->zoom[i].zoom_in_enable = (i == 0);
	}

	/* RGB/YONLY stream must be last; remap to its fixed channel base */
	for (i = 0; i < cnt; i++) {
		uint32_t fmt = stream_fmts[i].fmt;

		if (!(fmt & RTSCAM_FORMAT_TYPE_RGB) &&
			!(fmt & RTSCAM_FORMAT_TYPE_Y_ONLY)) {
			continue;
		}
		if (i != cnt - 1) {
			rtsprintk(RTS_TRACE_ERROR,
					  "RGB/YONLY stream must be last");
			return -EINVAL;
		}
		zoom_dev->zoom[i].base = zoom_dev->base +
								 ZOOM_V1_CH_REG_OFFSET +
								 (RTS_ZOOM_RGB_YONLY_STREAM_ID - 1) *
								 ZOOM_V1_CH_REG_STEP;
	}

	for (i = 0; i < cnt; i++) {
		zoom_dev->zoom[i].stream_fmt = stream_fmts[i];
		rtsprintk(RTS_TRACE_INFO,
				  "zoom stream %d: %ux%u fmt=0x%x base=0x%08x", i,
				  stream_fmts[i].width, stream_fmts[i].height,
				  stream_fmts[i].fmt, zoom_dev->zoom[i].base);
	}

	zoom_dev->stream_num = cnt;
	return 0;
}

static int rtscam_zoom_init_hardware(struct rtscam_zoom_dev *zoom_dev)
{
	rtscam_zoom_dev_set_clock(zoom_dev, 1);
	rtscam_zoom_write_reg(zoom_dev, 0x1,   ZOOM_SYS_CONTROL);
	rtscam_zoom_write_reg(zoom_dev, 0x101, ZOOM_SYS_SPEED_CTRL);
	rtscam_zoom_write_reg(zoom_dev, 0x4,   ZOOM_SYS_LAST_BREAK);
	rtscam_zoom_write_reg(zoom_dev, 2, ZOOM_CH0_UP_LINE_SPACE);
	rtscam_zoom_dev_set_clock(zoom_dev, 0);
	return 0;
}

int rtscam_zoom_init(struct rtscam_zoom_dev *zoom_dev)
{
	int ret;
	int i;
	TaskHandle_t handle;

	if (!zoom_dev) {
		return -EINVAL;
	}

	zoom_dev->base = CONFIG_RTS_ZOOM_SYS_BASE;
	isp_rtos_mutex_init(&zoom_dev->lock);
	isp_rtos_mutex_init(&zoom_dev->down_lock);

	for (i = 0; i < RTS_ZOOM_MAX_STREAM - 1; i++) {
		isp_rtos_init_completion(&zoom_dev->trig_completion[i]);
	}
	isp_rtos_init_completion(&zoom_dev->disable_completion);

	zoom_dev->streaming   = 0;
	zoom_dev->pending_ids = 0;
	zoom_dev->worker_op   = ZOOM_WORK_DOWN;

	/* "zoom_reset" must be registered in drivers/reset/reset.c */
	zoom_dev->reset = rtscam_rtos_reset_control_get("zoom_reset");
	if (zoom_dev->reset) {
		ret = rtscam_rtos_reset_control_reset(zoom_dev->reset);
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "zoom reset fail (ret %d)", ret);
			return ret;
		}
	}

	zoom_dev->clk_refcnt = 0;

	ret = rtscam_zoom_init_streams(zoom_dev);
	if (ret) {
		return ret;
	}

	ret = rtscam_zoom_init_hardware(zoom_dev);
	if (ret) {
		return ret;
	}

	handle = xTaskCreateStatic(
				 rtscam_zoom_worker_task, "zoom_worker",
				 ZOOM_WORKER_STACK_DEPTH, zoom_dev,
				 ZOOM_WORKER_TASK_PRIORITY,
				 zoom_dev->zoom_worker_stack,
				 &zoom_dev->zoom_worker_task_buf);
	if (handle != (TaskHandle_t)&zoom_dev->zoom_worker_task_buf) {
		rtsprintk(RTS_TRACE_ERROR, "zoom worker task create fail");
		return -ENOMEM;
	}
	zoom_dev->zoom_worker_handle = handle;

	return 0;
}

int rtscam_zoom_release(struct rtscam_zoom_dev *zoom_dev)
{
	if (!zoom_dev) {
		return -EINVAL;
	}
	return 0;
}

int rtscam_zoom_resume(struct rtscam_zoom_dev *zoom_dev)
{
	int ret;

	if (zoom_dev->reset) {
		ret = rtscam_rtos_reset_control_reset(zoom_dev->reset);
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "zoom reset fail (ret %d)", ret);
			return ret;
		}
	}

	return rtscam_zoom_init_hardware(zoom_dev);
}
