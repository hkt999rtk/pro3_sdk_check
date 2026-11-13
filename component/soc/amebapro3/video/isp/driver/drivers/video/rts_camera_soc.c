/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS:
 * - LOG_MODULE_REGISTER      -> removed
 * - const struct device *dev -> global singleton soc_data
 * - sys_read32/write32       -> readl/writel
 * - k_spin_lock/unlock       -> isp_rtos_spin_lock/unlock (no key)
 * - k_mutex_init             -> isp_rtos_mutex_init
 * - sys_dlist_init           -> INIT_LIST_HEAD
 * - k_fifo_init              -> INIT_LIST_HEAD
 * - DT_INST_REG_ADDR/SIZE    -> CONFIG_RTS_SOC_CAM_BASE/SIZE
 * - RESET_DT_SPEC_GET_BY_IDX -> rtscam_rtos_reset_control_get
 * - reset_line_toggle_dt     -> rtscam_rtos_reset_control_reset
 * - reset_line_deassert_dt   -> rtscam_rtos_reset_control_deassert
 * - IRQ_CONNECT(...)         -> interruptN_handler() weak symbol override
 * - PM_DEVICE_DT_INST_DEFINE -> removed
 * - shell commands           -> excluded from port
 * - atomic_clear_bit         -> bitmask operations
 * - atomic_clear             -> isp_rtos_atomic_set(..., 0)
 */

#include "drivers/irq/irq.h"
#include "rts_camera.h"
#include <errno.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_rtos_spinlock.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_atomic.h>
#include <isp_rtos_iomem.h>
#include <list.h>
#include "rts_camera_soc.h"
#include "rts_camera_soc_reg.h"
#include "rts_video.h"
#include "rts_camera_hw_config.h"
#include "rtscam_isp_input.h"
#include <irq.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* NVIC control — same pattern as other drivers in the project */
#define NVIC_ISER(n) (*((volatile uint32_t *)(0xE000E100 + 4 * (n))))
#define NVIC_ICER(n) (*((volatile uint32_t *)(0xE000E180 + 4 * (n))))
#define NVIC_IPR(n)  (*((volatile uint32_t *)(0xE000E400 + 4 * (n))))
#define __NVIC_PRIO_BITS 3

#ifndef __DSB
#define __DSB() __asm__ __volatile__("dsb 0xF" ::: "memory")
#endif
#ifndef __ISB
#define __ISB() __asm__ __volatile__("isb 0xF" ::: "memory")
#endif

/* Stream index limits (must match rts_camera_zoom.c) */
#define RTSCAM_YUV_MAX_STRM_NUM   4
#define RTSCAM_MAX_STREAM_NUM     (RTSCAM_YUV_MAX_STRM_NUM + 1)
#define RTSCAM_RGB_YONLY_STRM_IDX (RTSCAM_MAX_STREAM_NUM - 1)

/* format type bits */
#define RTSCAM_FORMAT_TYPE_YUV420_SEMIPLANAR (1 << 0)
#define RTSCAM_FORMAT_TYPE_YUV422_SEMIPLANAR (1 << 1)
#define RTSCAM_FORMAT_TYPE_RGB               (1 << 2)
#define RTSCAM_FORMAT_TYPE_Y_ONLY            (1 << 3)
#define RTSCAM_FORMAT_TYPE_YUYV              (1 << 4)
#define RTSCAM_FORMAT_TYPE_YVYU              (1 << 5)
#define RTSCAM_FORMAT_TYPE_128BIT_3PIXEL     (1 << 6)

#define RTS_VIDEOIN_HEIGHT_ALIGN  32
#define RTSCAM_SOC_MIN_W          80
#define RTSCAM_SOC_MIN_H          60

static int __rtscam_soc_s_stream(struct rtscam_video_stream *stream, int enable);

static struct rtscam_video_format_xlate m_rtscam_soc_formats[] = {
	{
		.index   = 0,
		.name    = "SEMIPLANAR YCBCR 4:2:0",
		.fourcc  = V4L2_PIX_FMT_NV12,
		.bpp     = 12,
		.is_yuv  = true,
		.rts_code = RTSCAM_FORMAT_TYPE_YUV420_SEMIPLANAR,
	},
	{
		.index   = 1,
		.name    = "SEMIPLANAR YCBCR 4:2:2",
		.fourcc  = V4L2_PIX_FMT_NV16,
		.bpp     = 16,
		.is_yuv  = true,
		.rts_code = RTSCAM_FORMAT_TYPE_YUV422_SEMIPLANAR,
	},
	{
		.index   = 2,
		.name    = "RGB",
		.fourcc  = V4L2_PIX_FMT_RGB24,
		.bpp     = 24,
		.is_yuv  = false,
		.rts_code = RTSCAM_FORMAT_TYPE_RGB,
	},
	{
		.index   = 3,
		.name    = "YONLY",
		.fourcc  = V4L2_PIX_FMT_Y8I,
		.bpp     = 8,
		.is_yuv  = false,
		.rts_code = RTSCAM_FORMAT_TYPE_Y_ONLY,
	},
	{
		.index   = 4,
		.name    = "INTERLEAVE YUYV 4:2:2",
		.fourcc  = V4L2_PIX_FMT_YUYV,
		.bpp     = 16,
		.is_yuv  = true,
		.rts_code = RTSCAM_FORMAT_TYPE_YUYV,
	},
	{
		.index   = 5,
		.name    = "INTERLEAVE YVYU 4:2:2",
		.fourcc  = V4L2_PIX_FMT_YVYU,
		.bpp     = 16,
		.is_yuv  = true,
		.rts_code = RTSCAM_FORMAT_TYPE_YVYU,
	},
	{
		.index   = 6,
		.name    = "RGB/YUV 128bit 3pixel",
		.fourcc  = V4L2_PIX_FMT_128BIT_3PIXEL,
		.bpp     = 128,
		.is_yuv  = false,
		.rts_code = RTSCAM_FORMAT_TYPE_128BIT_3PIXEL,
	},
};

/*
 * Global singleton — replaces Zephyr device framework.
 * Initialised in rtscam_soc_init(); the DT static init is replaced by
 * run-time assignment using CONFIG_RTS_* macros.
 */
static struct rtscam_soc_dev soc_data;

/* ------------------------------------------------------------------ */
/* Register accessors                                                  */
/* ------------------------------------------------------------------ */

static int rtscam_soc_read_reg(struct rtscam_soc_dev *rsocdev, uint32_t reg)
{
	return readl(rsocdev->base + reg);
}

static void rtscam_soc_write_reg(struct rtscam_soc_dev *rsocdev,
								 uint32_t value, uint32_t reg)
{
	writel(value, rsocdev->base + reg);
}

/* ------------------------------------------------------------------ */
/* Buffer-status helpers                                               */
/* ------------------------------------------------------------------ */

int rtscam_get_video_in_buf_status(int streamid, uint32_t *val)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;

	if (!val) {
		return -EINVAL;
	}

	if (streamid < 0 || streamid >= rsocdev->rvdev.streamnum) {
		rtsprintk(RTS_TRACE_ERROR, "streamid is invalid");
		return -EINVAL;
	}

	*val = rtscam_soc_read_reg(rsocdev,
							   rsocdev->reg.buf_status_base + 4 * streamid);

	return 0;
}

int rtscam_set_video_in_buf_status(int streamid, uint32_t val)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;

	if (streamid < 0 || streamid >= rsocdev->rvdev.streamnum) {
		rtsprintk(RTS_TRACE_ERROR, "streamid is invalid");
		return -EINVAL;
	}

	rtscam_soc_write_reg(rsocdev, val,
						 rsocdev->reg.buf_status_base + 4 * streamid);

	return 0;
}

/* ------------------------------------------------------------------ */
/* Stream access                                                       */
/* ------------------------------------------------------------------ */

struct rtscam_video_stream *rtscam_soc_get_video_stream(unsigned int streamid)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;

	if (streamid == RTSCAM_INPUT_STRM_IDX) {
		return rtscam_isp_input_get_stream();
	}

	if (streamid >= (unsigned int)rsocdev->rvdev.streamnum &&
		streamid != 10) {
		return NULL;
	}

	if (streamid == 10) {
		struct rtscam_video_stream *stream;

		streamid = rsocdev->rvdev.streamnum - 1;
		stream = rsocdev->rvdev.streams + streamid;
		if (stream->rts_code != RTSCAM_FORMAT_TYPE_RGB &&
			stream->rts_code != RTSCAM_FORMAT_TYPE_Y_ONLY) {
			return NULL;
		}
	}

	return rsocdev->rvdev.streams + streamid;
}

/* ------------------------------------------------------------------ */
/* Format helpers                                                      */
/* ------------------------------------------------------------------ */

static int rtscam_soc_is_yuv(uint8_t rts_code)
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(m_rtscam_soc_formats); i++) {
		if (m_rtscam_soc_formats[i].rts_code == rts_code) {
			return m_rtscam_soc_formats[i].is_yuv;
		}
	}

	return 0;
}

static int rtscam_soc_is_rgb(uint8_t rts_code)
{
	return rts_code == RTSCAM_FORMAT_TYPE_RGB;
}

static int rtscam_soc_is_yonly(uint8_t rts_code)
{
	return rts_code == RTSCAM_FORMAT_TYPE_Y_ONLY;
}

static int rtscam_soc_is_128bit3pixel(uint8_t rts_code)
{
	return rts_code == RTSCAM_FORMAT_TYPE_128BIT_3PIXEL;
}

/* ------------------------------------------------------------------ */
/* Per-stream counters                                                 */
/* ------------------------------------------------------------------ */

static void rtscam_soc_dec_drops(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	rsocdev->drops[stream->streamid]--;
}

static int rtscam_soc_get_drops(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	return rsocdev->drops[stream->streamid];
}

static uint8_t rtscam_soc_get_stream_reg_index(
	struct rtscam_video_stream *stream)
{
	uint8_t idx = stream->streamid;

	if (stream->rts_code == RTSCAM_FORMAT_TYPE_RGB ||
		stream->rts_code == RTSCAM_FORMAT_TYPE_Y_ONLY) {
		idx = RTSCAM_RGB_YONLY_STRM_IDX;
	}

	return idx;
}

static struct rtscam_video_stream *rtscam_soc_get_stream_from_reg_index(
	struct rtscam_soc_dev *rsocdev, int index)
{
	if (index == RTSCAM_RGB_YONLY_STRM_IDX) {
		index = rsocdev->rvdev.streamnum - 1;
	}

	if (index < 0 || index >= rsocdev->rvdev.streamnum) {
		return NULL;
	}

	if (rsocdev->rvdev.streamnum == 0) {
		return NULL;
	}

	return rsocdev->rvdev.streams + index;
}

static void rtscam_soc_inc_overflow(struct rtscam_soc_dev *rsocdev, int i)
{
	struct rtscam_video_stream *stream;

	stream = rtscam_soc_get_stream_from_reg_index(rsocdev, i);
	if (stream) {
		stream->overflow_count++;
	}
}

static void rtscam_soc_inc_abnormals(struct rtscam_soc_dev *rsocdev, int i)
{
	struct rtscam_video_stream *stream;

	stream = rtscam_soc_get_stream_from_reg_index(rsocdev, i);
	if (stream) {
		stream->abnormal_count++;
	}
}

static void rtscam_soc_inc_frames(struct rtscam_video_stream *stream)
{
	if (stream) {
		stream->frame_count++;
	}
}

static void rtscam_soc_inc_skips(struct rtscam_video_stream *stream)
{
	if (stream) {
		stream->skip_count++;
	}
}

static void rtscam_soc_inc_errors(struct rtscam_video_stream *stream)
{
	if (stream) {
		stream->error_count++;
	}
}

/* ------------------------------------------------------------------ */
/* Frame processing                                                    */
/* ------------------------------------------------------------------ */

static struct rtscam_soc_slot_info *rtscam_soc_get_skip_info(
	struct rtscam_soc_dev *rsocdev, int streamid)
{
	return &rsocdev->slot_info[streamid];
}

static int rtscam_soc_process_frame(struct rtscam_soc_dev *rsocdev,
									struct rtscam_video_stream *stream, int frameid)
{
	struct rtscam_soc_slot_info *info;
	struct rtscam_video_buffer *rbuf;
	uint32_t reg;
	uint32_t status;
	uint32_t mask;
	uint8_t idx = rtscam_soc_get_stream_reg_index(stream);
	uint32_t bytesused;
	int skip;

	info = rtscam_soc_get_skip_info(rsocdev, stream->streamid);

	if (frameid < 0 || frameid >= (int)info->slot_num) {
		return -EINVAL;
	}

	reg = RTS_REG_YUV_FRAME_BUFFER_STATUS_BASE + 4 * idx;
	status = rtscam_soc_read_reg(rsocdev, reg);

	if (rtscam_soc_is_yuv(stream->rts_code) ||
		rtscam_soc_is_128bit3pixel(stream->rts_code)) {
		mask = 0x20 << (8 * frameid);
	} else {
		mask = 0x10 << (8 * frameid);
	}
	if (!(mask & status)) {
		return 0;
	}

	rbuf = info->slots[frameid];
	info->slots[frameid] = NULL;

	if (rtscam_soc_is_yuv(stream->rts_code) ||
		rtscam_soc_is_128bit3pixel(stream->rts_code)) {
		mask = 1 << (8 * frameid + 4);
	} else {
		mask = 1 << (8 * frameid + 3);
	}
	if (mask & status) {
		rtscam_submit_buffer(stream, rbuf);
		rtscam_soc_inc_errors(stream);
		rtsprintk(RTS_TRACE_WARNING,
				  "frame error:stream<%d> frame<%d>: 0x%08x",
				  stream->streamid, frameid, status);
		return -EINVAL;
	}

	if (rbuf == NULL) {
		rtsprintk(RTS_TRACE_WARNING, "frameid[%d] is invalid", frameid);
		return -EINVAL;
	}

	if (rtscam_soc_get_drops(stream)) {
		rtscam_soc_dec_drops(stream);
		rtscam_soc_inc_skips(stream);
		rtscam_submit_buffer(stream, rbuf);
		rtsprintk(RTS_TRACE_VIDEO, "[%d]--", stream->streamid);
		return 0;
	}

	skip = rtscam_skip_frame(stream);
	rtsprintk(RTS_TRACE_VIDEO, "frameid=%d skip=%d", frameid, skip);
	if (skip) {
		rtscam_soc_inc_skips(stream);
		rtscam_submit_buffer(stream, rbuf);
		return 0;
	}

	bytesused = stream->sizeimage;
	rtscam_soc_inc_frames(stream);
	rtscam_buffer_done(stream, rbuf, bytesused);
	rtscam_submit_buffer(stream, NULL);

	return 0;
}

/* ------------------------------------------------------------------ */
/* ISR processing                                                      */
/* ------------------------------------------------------------------ */

static int rts_soc_process_rgb_yonly_irq(struct rtscam_soc_dev *rsocdev)
{
	const uint32_t reg = RTS_REG_INT_FLAG_RGB_YONLY_HOST;
	uint32_t status;
	uint32_t mask;
	int i;
	struct rtscam_video_stream *stream = NULL;

	status = rtscam_soc_read_reg(rsocdev, reg);
	if (!status) {
		return 0;
	}

	mask = 0xffffff07;
	if (status & mask) {
		rtscam_soc_write_reg(rsocdev, mask, reg);
		return 1;
	}

	mask = 0xe0;
	if (status & mask) {
		rtsprintk(RTS_TRACE_ERROR,
				  "rgb/yonly ddr len overflow: 0x%08x", status);
		rtscam_soc_write_reg(rsocdev, mask, reg);
		rtscam_soc_inc_abnormals(rsocdev, RTSCAM_RGB_YONLY_STRM_IDX);
		return 1;
	}

	mask = 0x8;
	if (status & mask) {
		rtsprintk(RTS_TRACE_WARNING,
				  "rgb/yonly overflow: 0x%08x", status);
		rtscam_soc_write_reg(rsocdev, mask, reg);
		rtscam_soc_inc_overflow(rsocdev, RTSCAM_RGB_YONLY_STRM_IDX);

		stream = rtscam_soc_get_stream_from_reg_index(rsocdev,
				 RTSCAM_RGB_YONLY_STRM_IDX);
		isp_rtos_spin_lock(&stream->lock);
		rtscam_submit_buffer(stream, NULL);
		isp_rtos_spin_unlock(&stream->lock);
		return 1;
	}

	mask = 1 << 4;
	if (status & mask) {
		int cnt;

		rtscam_soc_write_reg(rsocdev, mask, reg);

		stream = rtscam_soc_get_stream_from_reg_index(rsocdev,
				 RTSCAM_RGB_YONLY_STRM_IDX);
		if (stream == NULL) {
			rtsprintk(RTS_TRACE_WARNING, "no stream found");
			return 1;
		}

		isp_rtos_spin_lock(&stream->lock);
		mask = 0x3;
		i = mask & rtscam_soc_read_reg(rsocdev,
									   RTS_REG_RGB_R_CUR_FRAME_INDEX_OFFSET);
		for (cnt = 0; cnt < RTSCAM_SOC_HW_SLOT_NUM; cnt++) {
			rtscam_soc_process_frame(rsocdev, stream, i++);
			i %= RTSCAM_SOC_HW_SLOT_NUM;
		}
		isp_rtos_spin_unlock(&stream->lock);

		return 1;
	}

	return 0;
}

static int rtscam_soc_process_isp_irq(struct rtscam_soc_dev *rsocdev)
{
	const uint32_t reg_isp = RTS_REG_INT_FLAG_ISP_HOST;
	uint32_t status;
	uint32_t mask;
	int i, cnt;
	struct rtscam_video_stream *stream = NULL;

	status = rtscam_soc_read_reg(rsocdev, reg_isp);

	if (!status) {
		return 0;
	}

	stream = rtscam_soc_get_stream_from_reg_index(rsocdev, 0);
	if (stream->vin_mode == RTS_VIN_MODE_RING_MEM) {
		mask = 0xff00333f;
	} else {
		mask = 0xff003333;
	}

	if (status & mask) {
		rtscam_soc_write_reg(rsocdev, mask, reg_isp);
		return 1;
	}

	for (i = 0; i < RTSCAM_YUV_MAX_STRM_NUM; i++) {
		mask = 0x3 << (16 + 2 * i);
		if (status & mask) {
			rtsprintk(RTS_TRACE_ERROR,
					  "ddr len overflow: 0x%08x", status);
			rtscam_soc_write_reg(rsocdev, mask, reg_isp);
			rtscam_soc_inc_abnormals(rsocdev, i);
			return 1;
		}

		mask = 0x4 << (4 * i);
		if (status & mask) {
			rtsprintk(RTS_TRACE_WARNING,
					  "isp overflow: 0x%08x", status);
			rtscam_soc_write_reg(rsocdev, mask, reg_isp);
			rtscam_soc_inc_overflow(rsocdev, i);

			stream = rtscam_soc_get_stream_from_reg_index(
						 rsocdev, i);

			isp_rtos_spin_lock(&stream->lock);
			rtscam_submit_buffer(stream, NULL);
			isp_rtos_spin_unlock(&stream->lock);
			return 1;
		}
	}

	for (i = 0; i < RTSCAM_MAX_STREAM_NUM; i++) {
		mask = 1 << (4 * i + 3);
		if (status & mask) {
			rtscam_soc_write_reg(rsocdev, mask, reg_isp);
			break;
		}
	}

	stream = rtscam_soc_get_stream_from_reg_index(rsocdev, i);
	if (stream == NULL) {
		rtsprintk(RTS_TRACE_WARNING, "no stream found");
		return 1;
	}

	isp_rtos_spin_lock(&stream->lock);
	mask = 0x3;
	i = mask & rtscam_soc_read_reg(rsocdev,
								   RTS_REG_YUV_Y_CUR_FRAME_INDEX_OFFSET_BASE +
								   i * RTS_REG_YUV_CUR_FRAME_INDEX_OFFSET_INTERVAL);
	for (cnt = 0; cnt < RTSCAM_SOC_HW_SLOT_NUM; cnt++) {
		rtscam_soc_process_frame(rsocdev, stream, i++);
		i %= RTSCAM_SOC_HW_SLOT_NUM;
	}
	isp_rtos_spin_unlock(&stream->lock);

	return 1;
}

/*
 * ISR entry point.
 *
 * TODO: rename to interruptN_handler() where N matches CONFIG_RTS_SOC_IRQ,
 *       and add a weak declaration for it in apps/start.c g_pfnVectors[].
 */
void rtscam_soc_isr(void)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;

	if (rts_soc_process_rgb_yonly_irq(rsocdev)) {
		return;
	}

	rtscam_soc_process_isp_irq(rsocdev);
}

/* ------------------------------------------------------------------ */
/* Buffer submission                                                   */
/* ------------------------------------------------------------------ */
#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))

static int __submit_buffer(struct rtscam_soc_dev *rsocdev,
						   struct rtscam_video_stream *stream,
						   uint32_t phy_addr, int nr)
{
	uint32_t reg_addr;
	uint32_t reg_addr_uv;
	uint32_t reg_status;
	uint32_t status;

	if (!rsocdev || !stream) {
		return -EINVAL;
	}

	if (stream->vin_mode == RTS_VIN_MODE_RING_MEM && !stream->streamid) {
		uint32_t len_y = 0, len_uv = 0;
		uint32_t height = stream->ring_buf_height;

		if (phy_addr != 0) {
			len_y = stream->user_width * height;
			len_uv = stream->bytesperline * height - len_y;
		}

		if (stream->rts_code != RTSCAM_FORMAT_TYPE_YUYV &&
			stream->rts_code != RTSCAM_FORMAT_TYPE_YVYU) {
			rtscam_soc_write_reg(rsocdev,
								 phy_addr + len_y, RTS_REG_RINGBUF_ADDRUV);
			rtscam_soc_write_reg(rsocdev,
								 len_uv, RTS_REG_RINGBUF_LENUV);
		} else {
			len_y += len_uv;
		}

		rtscam_soc_write_reg(rsocdev, phy_addr, RTS_REG_RINGBUF_ADDRY);
		rtscam_soc_write_reg(rsocdev, len_y, RTS_REG_RINGBUF_LENY);

		return 0;
	}

	if (rtscam_soc_is_yuv(stream->rts_code)) {
		uint32_t idx = 4 * nr;
		uint32_t offset = stream->streamid *
						  RTS_REG_YUV_FRAME_START_ADDRESS_INTERVAL;
		uint32_t size = stream->user_width * stream->user_height;
		uint32_t uv_size;
		uint32_t align_w = 4096;
		uint32_t phy_addr_y = phy_addr;
		uint32_t phy_addr_uv = phy_addr + size;
		uint32_t slot_num = rtscam_soc_read_reg(rsocdev, RTS_REG_YUV_FRAME_BUFFER_COUNT);

		if (phy_addr == 0) {
			phy_addr_y = 0;
			phy_addr_uv = 0;
		}

		if (stream->ar_feature == 2 && stream->ar_align_w != 0) {
			size = stream->ar_align_w * stream->user_height;
			uv_size = stream->ar_align_w * stream->user_height / 2;
			size = ALIGN_UP(size, align_w);
			uv_size = ALIGN_UP(uv_size, align_w);
			phy_addr_y = ALIGN_UP(phy_addr_y, align_w);
			phy_addr_uv = phy_addr_y + size;
			phy_addr_uv = ALIGN_UP(phy_addr_uv, align_w);
		} else if (stream->ar_feature == 1 && stream->ar_align_w != 0) {
			size = stream->ar_align_w * stream->user_height;
			uv_size = stream->ar_align_w * stream->user_height / 2;
			size = ALIGN_UP(size, align_w);
			uv_size = ALIGN_UP(uv_size, align_w);
			phy_addr_y = ALIGN_UP(phy_addr_y, align_w);
			phy_addr_uv = phy_addr_y + (slot_num - nr) * size + nr * uv_size;
			phy_addr_uv = ALIGN_UP(phy_addr_uv, align_w);
		}
		reg_addr =
			RTS_REG_YUV_FRAME_Y_START_ADDRESS_BASE + offset + idx;
		reg_addr_uv =
			RTS_REG_YUV_FRAME_UV_START_ADDRESS_BASE + offset + idx;
		reg_status = RTS_REG_YUV_FRAME_BUFFER_STATUS_BASE +
					 stream->streamid * 4;
		status = 0x30 << (8 * nr);

		rtscam_soc_write_reg(rsocdev, phy_addr_y, reg_addr);
		if (stream->rts_code != RTSCAM_FORMAT_TYPE_YUYV &&
			stream->rts_code != RTSCAM_FORMAT_TYPE_YVYU) {
			rtscam_soc_write_reg(rsocdev, phy_addr_uv, reg_addr_uv);
		}
	} else if (rtscam_soc_is_128bit3pixel(stream->rts_code)) {
		uint32_t idx = 4 * nr;
		uint32_t offset = stream->streamid *
						  RTS_REG_YUV_FRAME_START_ADDRESS_INTERVAL;

		reg_addr =
			RTS_REG_YUV_FRAME_Y_START_ADDRESS_BASE + offset + idx;
		reg_status = RTS_REG_YUV_FRAME_BUFFER_STATUS_BASE +
					 stream->streamid * 4;
		status = 0x30 << (8 * nr);

		rtscam_soc_write_reg(rsocdev, phy_addr, reg_addr);
	} else if (rtscam_soc_is_rgb(stream->rts_code)) {
		uint32_t offset = stream->user_width * stream->user_height;

		if (phy_addr == 0) {
			offset = 0;
		}

		reg_status = RTS_REG_RGB_YONLY_FRAME_BUFFER_STATUS;
		status = 0x18 << (8 * nr);

		reg_addr = RTS_REG_RGB_R_FRAME_START_ADDRESS_BASE + 4 * nr;
		rtscam_soc_write_reg(rsocdev, phy_addr, reg_addr);

		phy_addr += offset;
		reg_addr = RTS_REG_RGB_G_FRAME_START_ADDRESS_BASE + 4 * nr;
		rtscam_soc_write_reg(rsocdev, phy_addr, reg_addr);

		phy_addr += offset;
		reg_addr = RTS_REG_RGB_B_FRAME_START_ADDRESS_BASE + 4 * nr;
		rtscam_soc_write_reg(rsocdev, phy_addr, reg_addr);
	} else if (rtscam_soc_is_yonly(stream->rts_code)) {
		reg_status = RTS_REG_RGB_YONLY_FRAME_BUFFER_STATUS;
		status = 0x18 << (8 * nr);

		reg_addr = RTS_REG_RGB_R_FRAME_START_ADDRESS_BASE + 4 * nr;
		rtscam_soc_write_reg(rsocdev, phy_addr, reg_addr);
	} else {
		rtsprintk(RTS_TRACE_BUF,
				  "invalid stream format (%d)", stream->rts_code);
		return -EINVAL;
	}

	rtsprintk(RTS_TRACE_DEBUG,
			  "clear frame status, reg=0x%08x val=0x%08x",
			  reg_status, status);
	rtscam_soc_write_reg(rsocdev, status, reg_status);
	status = rtscam_soc_read_reg(rsocdev, reg_status);

	return 0;
}

static int rtscam_soc_submit_buffer(struct rtscam_video_stream *stream,
									struct rtscam_video_buffer *rbuf)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct rtscam_soc_slot_info *info;
	int i;
	int nr = -1;
	uint32_t phy_addr;

	info = &rsocdev->slot_info[stream->streamid];

	for (i = 0; i < (int)info->slot_num; i++) {
		if (info->slots[i] == rbuf) {
			rtsprintk(RTS_TRACE_BUF, "buf in slot already");
			return -EINVAL;
		}
	}

	for (i = 0; i < (int)info->slot_num; i++) {
		nr = (i + info->slot_index) % info->slot_num;
		if (info->slots[nr] == NULL) {
			info->slot_index = (nr + 1) % info->slot_num;
			break;
		}
	}

	if (i == (int)info->slot_num) {
		rtsprintk(RTS_TRACE_BUF, "there is no free slot now");
		return -EINVAL;
	}

	phy_addr = rbuf->buf.phy_addr;
	if (!phy_addr) {
		rtsprintk(RTS_TRACE_BUF, "invalid phy addr");
		return -EINVAL;
	}

	info->slots[nr] = rbuf;

	return __submit_buffer(rsocdev, stream, phy_addr, nr);
}

/* ------------------------------------------------------------------ */
/* Interrupt enable/disable                                            */
/* ------------------------------------------------------------------ */

static void rtscam_soc_enable_interrupt(struct rtscam_soc_dev *rsocdev,
										int enable)
{
	uint32_t int_en;

	if (enable) {
		int_en = 0xffcccc;
	} else {
		int_en = 0;
	}

	rtscam_soc_write_reg(rsocdev, int_en, RTS_REG_INT_EN_ISP_TO_HOST);
	rtscam_soc_write_reg(rsocdev,
						 0xffffffff, RTS_REG_INT_FLAG_ISP_HOST);

	if (enable) {
		int_en = 0xf8;
	}

	rtscam_soc_write_reg(rsocdev,
						 int_en, RTS_REG_INT_EN_RGB_YONLY_TO_HOST);
	rtscam_soc_write_reg(rsocdev,
						 0xffffffff, RTS_REG_INT_FLAG_RGB_YONLY_HOST);
}

static void __isp_control(struct rtscam_soc_dev *rsocdev, uint8_t idx,
						  int enable)
{
	uint32_t reg;

	if (idx > RTSCAM_RGB_YONLY_STRM_IDX) {
		return;
	}

	if (idx == RTSCAM_RGB_YONLY_STRM_IDX) {
		reg = RTS_REG_ISP_RGB_YONLY_STREAM_EN;
	} else {
		reg = RTS_REG_ISP_STREAM_EN_BASE + 4 * idx;
	}

	rtscam_soc_write_reg(rsocdev, enable ? 1 : 0, reg);
}

/* ------------------------------------------------------------------ */
/* Buffer pool config                                                  */
/* ------------------------------------------------------------------ */

static void rtscam_soc_config_rgb_buffer(struct rtscam_soc_dev *rsocdev)
{
	struct rtscam_soc_rgbcfg *rgbcfg;
	uint32_t reg, addr, size, value;
	int i;

	if (!rsocdev) {
		return;
	}

	rgbcfg = &rsocdev->rgbcfg;
	for (i = 0; i < 3; i++) {
		reg = rsocdev->reg.rgb_axibuf_base +
			  i * rsocdev->reg.rgb_axibuf_interval;
		addr = rgbcfg->rgb[i].base;
		size = rgbcfg->rgb[i].size;
		value = addr + (size << 16);
		rtscam_soc_write_reg(rsocdev, value, reg);
	}
}

static void rtscam_soc_config_isp_buffer(struct rtscam_soc_dev *rsocdev)
{
	uint8_t idx = 0;
	struct rtscam_video_stream *stream;
	struct rtscam_soc_icfg *icfg;
	int i;
	uint32_t reg, value, base_bit, size_bit;

	if (!rsocdev) {
		return;
	}

	for (i = 0; i < rsocdev->rvdev.streamnum; i++) {
		stream = rsocdev->rvdev.streams + i;
		icfg = &rsocdev->icfgs[i];
		idx = rtscam_soc_get_stream_reg_index(stream);

		reg = rsocdev->reg.yuv_axibuf_base + 8 * idx;
		base_bit = rsocdev->reg.yuv_base_val_bit;
		size_bit = rsocdev->reg.yuv_size_val_bit;
		value = (icfg->y.base << base_bit) + (icfg->y.size << size_bit);
		rtscam_soc_write_reg(rsocdev, value, reg);

		reg = reg + 4;
		value = (icfg->uv.base << base_bit) +
				(icfg->uv.size << size_bit);
		rtscam_soc_write_reg(rsocdev, value, reg);
	}
}

/* ------------------------------------------------------------------ */
/* Video stream init/release                                           */
/* ------------------------------------------------------------------ */

static int rtscam_soc_init_video_stream(struct rtscam_soc_dev *rsocdev,
										struct rtscam_video_stream *stream,
										struct rtscam_zoom_stream_fmt *s_fmt)
{
	struct rtscam_video_format_xlate *fmt = NULL;
	int i;
	struct rtscam_frame_size max;
	struct rtscam_frame_size min = {RTSCAM_SOC_MIN_W, RTSCAM_SOC_MIN_H};
	struct rtscam_frame_size step;
	int ret;

	step.width = rsocdev->width_step;
	step.height = rsocdev->height_step;
	for (i = 0; i < (int)ARRAY_SIZE(m_rtscam_soc_formats); i++) {
		fmt = &m_rtscam_soc_formats[i];
		if (!(fmt->rts_code & s_fmt->fmt)) {
			continue;
		}

		ret = rtscam_register_format(stream, fmt);
		if (ret) {
			goto error;
		}

		max.width = s_fmt->width;
		max.height = s_fmt->height;
		if (stream->streamid == 0) {
			max.width = rsocdev->s0_width ? rsocdev->s0_width : s_fmt->width;
			max.height = rsocdev->s0_height ? rsocdev->s0_height : s_fmt->height;
		}
		ret = rtscam_register_frame_stepwise(stream, fmt->fourcc,
											 &max, &min, &step);
		if (ret) {
			goto error;
		}

		ret = rtscam_register_frmival(stream, fmt->fourcc, &max);
		if (ret) {
			goto error;
		}
	}
	return 0;
error:
	rtscam_clr_format(stream, 0);
	return ret;
}

static void rtscam_soc_release_video_stream(struct rtscam_soc_dev *rsocdev,
		struct rtscam_video_stream *stream)
{
	rtscam_clr_format(stream, rsocdev->keep_user_setting);
}

static void __reset_streaminfo(struct rtscam_video_stream *stream)
{
	isp_rtos_spin_lock(&stream->lock);
	stream->overflow_count = 0;
	stream->frame_count = 0;
	stream->skip_count = 0;
	stream->error_count = 0;
	stream->abnormal_count = 0;
	isp_rtos_spin_unlock(&stream->lock);
}

static int rtscam_soc_set_fps(struct rtscam_video_stream *stream,
							  struct rts_video_fps fps)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct v4l2_fract isp_fps = { 0 };

	isp_fps.numerator = fps.numerator;
	isp_fps.denominator = fps.denominator;

	return rtscam_isp_soc_set_fps(rsocdev->isp, isp_fps);
}

static int rtscam_register_fps_info(struct rtscam_soc_dev *rsocdev,
									struct rtscam_video_stream *streams,
									int streamnum)
{
	struct rtscam_sensor_fps *sensor_fps;
	struct rtscam_video_stream *stream;
	struct v4l2_fract fps_min, fps_max;
	int i, ret;
	struct rtscam_soc_fps_descriptor *desc;

	sensor_fps = &rsocdev->sensor_fps;
	desc = &sensor_fps->desc;

	ret = rtscam_isp_soc_get_fps(rsocdev->isp, &fps_min, &fps_max);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "get fps fail: %d", ret);
		return ret;
	}

	desc->max.numerator = fps_max.numerator;
	desc->max.denominator = fps_max.denominator;
	desc->min.numerator = fps_min.numerator;
	desc->min.denominator = fps_min.denominator;
	desc->step.denominator = 1;
	desc->step.numerator = 1000;

	sensor_fps->set_stream = __rtscam_soc_s_stream;
	sensor_fps->set_fps = rtscam_soc_set_fps;
	rtscam_init_sensor_fps(&rsocdev->sensor_fps, 0);
	sensor_fps->streamnum = streamnum;
	sensor_fps->streaming_count = &rsocdev->rvdev.streaming_count;
	sensor_fps->streams = streams;
	for (i = 0; i < streamnum; i++) {
		stream = streams + i;
		stream->fps.sensor_fps = sensor_fps;
	}

	return 0;
}

static int __get_valid_streamnum(struct rtscam_soc_dev *rsocdev)
{
	int i, j, num = 0;
	int ret;

	if (!rsocdev) {
		return 0;
	}

	ret = rtscam_zoom_soc_get_stream_info(&rsocdev->zoom_dev,
										  &rsocdev->stream_info);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "get stream info fail: %d", ret);
		return 0;
	}

	for (i = 0; i < (int)rsocdev->stream_info.stream_num; i++) {
		uint32_t fmt = rsocdev->stream_info.fmt[i].fmt;

		for (j = 0; j < (int)ARRAY_SIZE(m_rtscam_soc_formats); j++) {
			struct rtscam_video_format_xlate *mfmt;

			mfmt = &m_rtscam_soc_formats[j];
			if (mfmt->rts_code & fmt) {
				num++;
				break;
			}
		}
	}

	if (num != (int)rsocdev->stream_info.stream_num) {
		rtsprintk(RTS_TRACE_ERROR,
				  "stream num %d is not same as zoom %d",
				  num, rsocdev->stream_info.stream_num);
		return 0;
	}

	return num;
}

static int rtscam_soc_config_hw_slot_num(struct rtscam_video_stream *stream,
		int count)
{
	uint32_t reg;
	uint32_t value;
	uint8_t idx = rtscam_soc_get_stream_reg_index(stream);
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	if (count < 0 || count >= 8) {
		return -EINVAL;
	}

	if (idx == RTSCAM_RGB_YONLY_STRM_IDX) {
		idx = 0;
		reg = rsocdev->reg.rgb_frame_buf_cnt;
	} else {
		reg = rsocdev->reg.yuv_frame_buf_cnt;
	}
	if (reg == 0) {
		return -EINVAL;
	}

	value = rtscam_soc_read_reg(rsocdev, reg);

	/* Clear 4-bit field at offset 4*idx, then set new count */
	value &= ~(0xfU << (4 * idx));
	value |= ((uint32_t)count << (4 * idx));

	rtscam_soc_write_reg(rsocdev, value, reg);

	return 0;
}

int rtscam_soc_get_hw_slot_num(uint32_t streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	uint32_t reg;
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	uint32_t value;

	if (streamid == RTSCAM_RGB_YONLY_STRM_IDX) {
		reg = rsocdev->reg.rgb_frame_buf_cnt;
	} else {
		reg = rsocdev->reg.yuv_frame_buf_cnt;
	}

	value = rtscam_soc_read_reg(rsocdev, reg);

	if (streamid < RTSCAM_YUV_MAX_STRM_NUM) {
		value = (value >> (4 * streamid)) & 0x7;
	} else {
		value = value & 0x3;
	}

	return 0;
}

int rtscam_soc_get_isp_remap_buf(uint32_t streamid, uint32_t buf_id, uint32_t *y_buf, uint32_t *uv_buf)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	uint32_t reg_addr;
	uint32_t reg_addr_uv;
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	if (rtscam_soc_is_yuv(stream->rts_code)) {
		uint32_t idx = 4 * buf_id;
		uint32_t offset = streamid * RTS_REG_YUV_FRAME_START_ADDRESS_INTERVAL;

		reg_addr = RTS_REG_YUV_FRAME_Y_START_ADDRESS_BASE + offset + idx;
		reg_addr_uv = RTS_REG_YUV_FRAME_UV_START_ADDRESS_BASE + offset + idx;
		*y_buf = rtscam_soc_read_reg(rsocdev, reg_addr);
		*uv_buf = rtscam_soc_read_reg(rsocdev, reg_addr_uv);
	} else {
		*y_buf = 0;
		*uv_buf = 0;
	}
	return 0;
}

static int rtscam_soc_init_capture_buffers(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct rtscam_soc_slot_info *info;
	int i;

	info = &rsocdev->slot_info[stream->streamid];
	info->slot_index = 0;
	for (i = 0; i < RTSCAM_SOC_HW_SLOT_NUM; i++) {
		info->slots[i] = NULL;
	}

	if (stream->buf_nums < RTSCAM_SOC_HW_SLOT_NUM) {
		info->slot_num = stream->buf_nums;
	} else {
		info->slot_num = RTSCAM_SOC_HW_SLOT_NUM;
	}

	rtscam_soc_config_hw_slot_num(stream, info->slot_num);

	return 0;
}

/* ------------------------------------------------------------------ */
/* Selection (crop)                                                    */
/* ------------------------------------------------------------------ */

static int rtscam_soc_get_selection(struct rtscam_video_stream *stream,
									struct rts_video_selection *selection)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct rtscam_subdev_crop_info crop = { 0 };
	int ret;

	if (!rsocdev || !rsocdev->initialized) {
		return -EINVAL;
	}

	ret = rtscam_zoom_soc_get_crop(
			  rsocdev->zoom_dev.zoom + stream->streamid, &crop);
	if (ret) {
		return ret;
	}

	selection->left = crop.start_x;
	selection->top = crop.start_y;
	selection->width = crop.width;
	selection->height = crop.height;

	return 0;
}

static int rtscam_soc_set_selection(struct rtscam_video_stream *stream,
									struct rts_video_selection *selection)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct rtscam_subdev_crop_info crop;

	if (!rsocdev || !rsocdev->initialized) {
		return -EINVAL;
	}

	crop.mode = selection->mode;
	crop.start_x = selection->left;
	crop.start_y = selection->top;
	crop.width = selection->width;
	crop.height = selection->height;

	return rtscam_zoom_soc_set_crop(
			   rsocdev->zoom_dev.zoom + stream->streamid, &crop);
}

static int rtscam_soc_get_limit_color_range(struct rtscam_video_stream *stream,
		int *limit_enable)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	if (!rsocdev || !rsocdev->initialized) {
		return -EINVAL;
	}

	return rtscam_zoom_soc_get_limit_color_range(
			   rsocdev->zoom_dev.zoom + stream->streamid, limit_enable);
}

static int rtscam_soc_set_limit_color_range(struct rtscam_video_stream *stream,
		int *limit_enable)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	if (!rsocdev || !rsocdev->initialized) {
		return -EINVAL;
	}

	return rtscam_zoom_soc_set_limit_color_range(
			   rsocdev->zoom_dev.zoom + stream->streamid, limit_enable);
}

/* ------------------------------------------------------------------ */
/* Format and frame-length programming                                 */
/* ------------------------------------------------------------------ */

uint32_t g_pix_cnt_thd = 0xffff;

static int rtscam_soc_set_format(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = NULL;
	uint32_t tmp;

	if (!stream || !stream->icd || !stream->icd->priv) {
		return -EINVAL;
	}

	rsocdev = stream->icd->priv;

	if (stream->streamid && stream->vin_mode == RTS_VIN_MODE_RING_MEM) {
		rtsprintk(RTS_TRACE_ERROR,
				  "stream %d not support for vin ringbuf",
				  stream->streamid);
		return -EINVAL;
	}

	if (!stream->streamid) {
		if (stream->vin_mode == RTS_VIN_MODE_RING_MEM) {
			rtscam_soc_write_reg(rsocdev, 0x1,
								 rsocdev->reg.ringbuf_mode);
			rtscam_soc_write_reg(rsocdev, 1,
								 rsocdev->reg.ringbuf_rowend_delay);
		} else {
			rtscam_soc_write_reg(rsocdev, 0,
								 rsocdev->reg.ringbuf_mode);
		}
	}

	if (!stream->streamid && rsocdev->reg.yuv_interleave_select) {
		if (stream->rts_code == RTSCAM_FORMAT_TYPE_YUYV ||
			rtscam_soc_is_128bit3pixel(stream->rts_code) ||
			stream->rts_code == RTSCAM_FORMAT_TYPE_YVYU)
			rtscam_soc_write_reg(rsocdev, 0x01,
								 rsocdev->reg.yuv_interleave_select);
		else
			rtscam_soc_write_reg(rsocdev, 0,
								 rsocdev->reg.yuv_interleave_select);
	}

	if (rsocdev->reg.isp_nv12_select) {
		tmp = rtscam_soc_read_reg(rsocdev,
								  rsocdev->reg.isp_nv12_select);
		if (stream->rts_code == RTSCAM_FORMAT_TYPE_YVYU)
			rtscam_soc_write_reg(rsocdev,
								 tmp & ~(1 << stream->streamid),
								 rsocdev->reg.isp_nv12_select);
		else
			rtscam_soc_write_reg(rsocdev,
								 tmp | (1 << stream->streamid),
								 rsocdev->reg.isp_nv12_select);
	}

	g_pix_cnt_thd = stream->user_width * stream->user_height / 16 * 3 / 4;

	return rtscam_zoom_soc_set_fmt(
			   rsocdev->zoom_dev.zoom + stream->streamid,
			   stream->rts_code, stream->user_width,
			   stream->user_height);
}

#define RTSCAM_DEFAULT_BURST_LEN_Y0 128
int rtscam_check_pixel_cnt(void)
{
	uint32_t val;

	val = rtscam_soc_read_reg(&soc_data,
							  RTS_REG_YUV_Y_CUR_FRAME_INDEX_OFFSET_BASE);
	val = (val >> 2) & 0xfffff;

	if (val < RTSCAM_DEFAULT_BURST_LEN_Y0 || val > g_pix_cnt_thd) {
		return 0;
	} else {
		return 1;
	}
}

static void rtscam_soc_set_frame_len(struct rtscam_video_stream *stream)
{
	uint32_t len, len_rgb, len_y, len_uv;
	uint32_t rts_code;
	struct rtscam_soc_dev *rsocdev = NULL;

	if (!stream || !stream->icd || !stream->icd->priv) {
		return;
	}

	rts_code = stream->rts_code;
	rsocdev = stream->icd->priv;
	if (!rsocdev->reg.rgb_frame_len_r ||
		!rsocdev->reg.rgb_frame_len_g ||
		!rsocdev->reg.rgb_frame_len_b) {
		return;
	}

	if (!rsocdev->reg.y_frame_len_base ||
		!rsocdev->reg.uv_frame_len_base ||
		!rsocdev->reg.yuv_frame_len_interval) {
		return;
	}

	if ((stream->ar_feature == 1 || stream->ar_feature == 2) && stream->ar_align_w != 0) {
		len = stream->ar_align_w * stream->user_height;
	} else {
		len = stream->user_width * stream->user_height;
	}

	len_rgb = (len % 16) ? (len + 16) : len;
	if (rts_code == RTSCAM_FORMAT_TYPE_Y_ONLY) {
		rtscam_soc_write_reg(rsocdev, len_rgb,
							 rsocdev->reg.rgb_frame_len_r);
		return;
	} else if (rts_code == RTSCAM_FORMAT_TYPE_RGB) {
		rtscam_soc_write_reg(rsocdev, len_rgb,
							 rsocdev->reg.rgb_frame_len_r);
		rtscam_soc_write_reg(rsocdev, len_rgb,
							 rsocdev->reg.rgb_frame_len_g);
		rtscam_soc_write_reg(rsocdev, len_rgb,
							 rsocdev->reg.rgb_frame_len_b);
		return;
	}

	switch (rts_code) {
	case RTSCAM_FORMAT_TYPE_YUYV:
	case RTSCAM_FORMAT_TYPE_YVYU:
		len_y = len << 1;
		len_uv = 0;
		break;
	case RTSCAM_FORMAT_TYPE_YUV420_SEMIPLANAR:
		len_y = len;
		len_uv = len >> 1;
		break;
	case RTSCAM_FORMAT_TYPE_YUV422_SEMIPLANAR:
		len_y = len;
		len_uv = len;
		break;
	case RTSCAM_FORMAT_TYPE_128BIT_3PIXEL:
		len_y = (len + 2) / 3 * 128 / 8;
		len_uv = 0;
		break;
	default:
		rtsprintk(RTS_TRACE_ERROR,
				  "set frame len fail, invalid fmt %d", rts_code);
		return;
	}

	len_y = (len_y % 16) ? (len_y + 16) : len_y;
	len_uv = (len_uv % 16) ? (len_uv + 16) : len_uv;

	rtscam_soc_write_reg(rsocdev, len_y,
						 rsocdev->reg.y_frame_len_base +
						 stream->streamid *
						 rsocdev->reg.yuv_frame_len_interval);
	rtscam_soc_write_reg(rsocdev, len_uv,
						 rsocdev->reg.uv_frame_len_base +
						 stream->streamid *
						 rsocdev->reg.yuv_frame_len_interval);
}

/* ------------------------------------------------------------------ */
/* Stream reset / start / stop                                         */
/* ------------------------------------------------------------------ */

static void __reset_isp_reg(struct rtscam_soc_dev *rsocdev, uint8_t idx)
{
	uint32_t reg;

	if (idx == RTSCAM_RGB_YONLY_STRM_IDX) {
		reg = RTS_REG_ISP_RGB_YONLY_STREAM_RST;
	} else {
		reg = RTS_REG_ISP_STREAM_RST_BASE + 4 * idx;
	}

	rtscam_soc_write_reg(rsocdev, 1, reg);
	rtscam_soc_write_reg(rsocdev, 0, reg);
}

static void rtscam_soc_reset_stream(struct rtscam_video_stream *stream)
{
	int idx;
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	idx = rtscam_soc_get_stream_reg_index(stream);
	__reset_isp_reg(rsocdev, idx);
}

static void __fill_stream_slots(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct rtscam_soc_slot_info *info;
	int i;

	isp_rtos_spin_lock(&stream->lock);
	info = &rsocdev->slot_info[stream->streamid];

	for (i = 0; i < (int)info->slot_num; i++) {
		if (info->slots[i]) {
			continue;
		}

		rtscam_submit_buffer(stream, NULL);
	}
	isp_rtos_spin_unlock(&stream->lock);
}

static void __clear_stream_slots(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;
	struct rtscam_soc_slot_info *info;
	int i;

	isp_rtos_spin_lock(&stream->lock);
	info = &rsocdev->slot_info[stream->streamid];

	for (i = 0; i < (int)info->slot_num; i++) {
		struct rtscam_video_buffer *buffer = info->slots[i];

		__submit_buffer(rsocdev, stream, 0, i);

		if (!buffer) {
			continue;
		}

		info->slots[i] = NULL;
		rtscam_push_back_ready_buffer(stream, buffer);
	}
	info->slot_index = 0;
	isp_rtos_spin_unlock(&stream->lock);
}

static int rtscam_soc_start_preview(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	return rtscam_zoom_soc_set_stream(
			   rsocdev->zoom_dev.zoom + stream->streamid, 1);
}

static int rtscam_soc_stop_preview(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_dev *rsocdev = stream->icd->priv;

	return rtscam_zoom_soc_set_stream(
			   rsocdev->zoom_dev.zoom + stream->streamid, 0);
}

static void __calc_stream_delta(struct rtscam_video_stream *stream)
{
	stream->delta = 3;
}

static void __clear_stream_delta(struct rtscam_video_stream *stream)
{
	stream->delta = 0;
}

static int rtscam_soc_init_stream_delay(struct rtscam_soc_dev *rsocdev)
{
	int i;
	unsigned long delay = 0;

	for (i = 0; i < rsocdev->rvdev.streamnum; i++) {
		struct rtscam_video_stream *stream = rsocdev->rvdev.streams + i;

		stream->delay = delay;

		if (stream->delay > 30) {
			stream->delay = 30;
		}

		delay += stream->delta;
	}

	return 0;
}

static int rtscam_soc_s_stream(struct rtscam_video_stream *stream, int enable)
{
	int ret = 0;

	if (!stream || !stream->icd || !stream->icd->priv) {
		return -EINVAL;
	}

	if (stream->fps.sensor_fps->flag_max) {
		rtscam_exec_sensor_fps_setting(stream, enable);
		return __rtscam_soc_s_stream(stream, enable);
	}

	if (enable) {
		rtscam_adjust_sensor_fps(stream, enable);
		ret = __rtscam_soc_s_stream(stream, enable);
	} else {
		ret = __rtscam_soc_s_stream(stream, enable);
		rtscam_adjust_sensor_fps(stream, enable);
	}

	return ret;
}

static int __rtscam_soc_s_stream(struct rtscam_video_stream *stream, int enable)
{
	int ret = 0;
	uint8_t idx;
	struct rtscam_soc_dev *rsocdev = NULL;

	rtsprintk(RTS_TRACE_DEBUG, "set stream %s", enable ? "on" : "off");

	if (!stream || !stream->icd || !stream->icd->priv) {
		return -EINVAL;
	}

	rsocdev = stream->icd->priv;
	idx = rtscam_soc_get_stream_reg_index(stream);

	if (enable) {
		stream->sequence = 0;
		__reset_streaminfo(stream);
		rsocdev->drops[stream->streamid] = rsocdev->drop_frames;
		rtscam_soc_set_format(stream);
		rtscam_soc_set_frame_len(stream);
		rtscam_soc_reset_stream(stream);
		if (stream->vin_mode != RTS_VIN_MODE_NO_MEM) {
			__fill_stream_slots(stream);
			__isp_control(rsocdev, idx, 1);
		}

		ret = rtscam_soc_start_preview(stream);
		if (ret) {
			__isp_control(rsocdev, idx, 0);
		}
		__calc_stream_delta(stream);
	} else {
		rtscam_soc_stop_preview(stream);
		__isp_control(rsocdev, idx, 0);
		__clear_stream_delta(stream);
		__clear_stream_slots(stream);
	}
	rtscam_soc_init_stream_delay(rsocdev);

	return ret;
}

static struct rtscam_video_ops rtscam_soc_ops = {
	.init_capture_buffers  = rtscam_soc_init_capture_buffers,
	.submit_buffer         = rtscam_soc_submit_buffer,
	.s_stream              = rtscam_soc_s_stream,
	.set_selection         = rtscam_soc_set_selection,
	.get_selection         = rtscam_soc_get_selection,
	.set_limit_color_range = rtscam_soc_set_limit_color_range,
	.get_limit_color_range = rtscam_soc_get_limit_color_range,
};

/* ------------------------------------------------------------------ */
/* Video device init/release                                           */
/* ------------------------------------------------------------------ */

static void rtscam_soc_release_video_device(struct rtscam_soc_dev *rsocdev)
{
	struct rtscam_video_device *icd = &rsocdev->rvdev;
	int i;

	for (i = 0; i < icd->streamnum; i++) {
		struct rtscam_video_stream *stream = icd->streams + i;

		rtscam_soc_release_video_stream(rsocdev, stream);
		stream->icd = NULL;
	}

	icd->streamnum = 0;
	icd->ops = NULL;
	icd->priv = NULL;
	icd->initialized = 0;

	rtscam_release_sensor_fps(&rsocdev->sensor_fps);
}

static int rtscam_soc_init_video_device(struct rtscam_soc_dev *rsocdev)
{
	struct rtscam_video_device *icd = &rsocdev->rvdev;
	int streamnum, i;
	struct rtscam_video_stream *streams = icd->streams;
	int ret;

	streamnum = __get_valid_streamnum(rsocdev);

	if (!streamnum) {
		rtsprintk(RTS_TRACE_ERROR, "invalid stream num");
		return -EINVAL;
	}

	if (streamnum > RTSCAM_VIDEO_SUPPORT_MAX_STREAM_NUM) {
		rtsprintk(RTS_TRACE_ERROR,
				  "streamnum(%d) is so big, max support %d",
				  streamnum, RTSCAM_VIDEO_SUPPORT_MAX_STREAM_NUM);
		return -EINVAL;
	}

	for (i = 0; i < streamnum; i++) {
		struct rtscam_video_stream *stream = streams + i;

		isp_rtos_mutex_init(&stream->stream_lock);
		isp_rtos_spin_lock_init(&stream->lock);  /* must init before any enqueue */
		INIT_LIST_HEAD(&stream->capture);
		INIT_LIST_HEAD(&stream->fifo_out);
	}

	ret = rtscam_register_fps_info(rsocdev, streams, streamnum);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "register fps info fail");
		rtscam_release_sensor_fps(&rsocdev->sensor_fps);
		return ret;
	}

	for (i = 0; i < streamnum; i++) {
		struct rtscam_video_stream *stream = streams + i;
		struct rtscam_zoom_stream_fmt *fmt =
				&rsocdev->stream_info.fmt[i];

		stream->streamid = i;
		stream->user_height_align = rsocdev->height_align;
		stream->vin_mode = RTS_VIN_MODE_NORMAL;
		ret = rtscam_soc_init_video_stream(rsocdev, stream, fmt);
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR, "init stream<%d> fail", i);
			goto out;
		}

		if (rtscam_soc_is_rgb(stream->rts_code)) {
			stream->fov_status = 1;
		} else {
			stream->fov_status = 0;
		}
		stream->icd = icd;
		stream->handle = NULL;
		isp_rtos_atomic_set(&stream->active, 0);
	}

	icd->streamnum = streamnum;
	icd->ops = &rtscam_soc_ops;
	icd->priv = rsocdev;
	icd->initialized = 1;
	icd->streaming_count = 0;

	/* Reserved TX (input) stream: fixed RTSCAM_INPUT_STRM_IDX, own object
	 * and ops table, deliberately outside the streamnum loop above so it
	 * is never counted by __get_valid_streamnum() and unaffected by
	 * capture-stream attach/detach cycles. Idempotent — a no-op on the
	 * refresh cycles triggered by zoom/fps changes. */
	ret = rtscam_isp_input_init();
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "init input stream fail: %d", ret);
		goto out;
	}

	return 0;
out:
	rtscam_soc_release_video_device(rsocdev);

	return ret;
}

/* ------------------------------------------------------------------ */
/* Attach / detach / refresh                                           */
/* ------------------------------------------------------------------ */

static int rtscam_soc_attach(struct rtscam_soc_dev *rsocdev)
{
	int ret = 0;

	if (!rsocdev) {
		return -EINVAL;
	}

	if (rsocdev->initialized) {
		return 0;
	}

	if (!rsocdev->zoom_dev.stream_num) {
		rtsprintk(RTS_TRACE_ERROR, "invalid zoom stream num");
		ret = -EINVAL;
		goto out;
	}

	ret = rtscam_soc_init_video_device(rsocdev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "init video device fail: %d", ret);
		goto out;
	}

	rtscam_soc_config_isp_buffer(rsocdev);
	rtscam_soc_config_rgb_buffer(rsocdev);

	rsocdev->initialized = 1;
out:
	return ret;
}

static int rtscam_soc_detach(struct rtscam_soc_dev *rsocdev)
{
	if (!rsocdev) {
		return -EINVAL;
	}

	if (!rsocdev->initialized) {
		return 0;
	}

	rtscam_soc_release_video_device(rsocdev);
	rsocdev->initialized = 0;

	return 0;
}

static int rtscam_soc_refresh(struct rtscam_soc_dev *rsocdev, int id, int force_reset)
{
	int ret;

	if (!rsocdev->initialized) {
		return 0;
	}

	if (!force_reset) {
		rsocdev->s0_width = 0;
		rsocdev->s0_height = 0;
	}

	ret = rtscam_soc_detach(rsocdev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "soc detach fail: %d", ret);
		return ret;
	}
	ret = rtscam_soc_attach(rsocdev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "soc attach fail: %d", ret);
		return ret;
	}

	if (!force_reset) {
		rsocdev->s0_width = rsocdev->zoom_dev.zoom[0].stream_fmt.width;
		rsocdev->s0_height = rsocdev->zoom_dev.zoom[0].stream_fmt.height;
	}

	return 0;
}

/* ------------------------------------------------------------------ */
/* Public event handler                                                */
/* ------------------------------------------------------------------ */

int rtscam_soc_event(int id, void *arg)
{
	int ret = 0;
	int force_reset = arg ? *(int *)arg : 0;

	switch (id) {
	case RTSCAM_ISP_EVT_FPS_CHANGED:
	case RTSCAM_ZOOM_EVT_STREAM_INFO_CHANGED:
		ret = rtscam_soc_refresh(&soc_data, id, force_reset);
		if (ret)
			rtsprintk(RTS_TRACE_ERROR,
					  "soc refresh fail: %d", ret);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

struct rtscam_zoom_dev *rtscam_soc_get_zoom_dev(void)
{
	return &soc_data.zoom_dev;
}

/* ------------------------------------------------------------------ */
/* Buffer config — replaces DT parsing                                 */
/* ------------------------------------------------------------------ */

static int rtscam_soc_init_buf_info(struct rtscam_soc_dev *rsocdev)
{
	int i = 0;

#if defined(CONFIG_RTS_YUV_S0_Y_BASE)
	rsocdev->icfgs[0].y.base  = CONFIG_RTS_YUV_S0_Y_BASE;
	rsocdev->icfgs[0].y.size  = CONFIG_RTS_YUV_S0_Y_SIZE;
	rsocdev->icfgs[0].uv.base = CONFIG_RTS_YUV_S0_UV_BASE;
	rsocdev->icfgs[0].uv.size = CONFIG_RTS_YUV_S0_UV_SIZE;
	rsocdev->icfgs[0].found = 1;
	i++;
#endif
#if defined(CONFIG_RTS_YUV_S1_Y_BASE)
	rsocdev->icfgs[1].y.base  = CONFIG_RTS_YUV_S1_Y_BASE;
	rsocdev->icfgs[1].y.size  = CONFIG_RTS_YUV_S1_Y_SIZE;
	rsocdev->icfgs[1].uv.base = CONFIG_RTS_YUV_S1_UV_BASE;
	rsocdev->icfgs[1].uv.size = CONFIG_RTS_YUV_S1_UV_SIZE;
	rsocdev->icfgs[1].found = 1;
	i++;
#endif
#if defined(CONFIG_RTS_YUV_S2_Y_BASE)
	rsocdev->icfgs[2].y.base  = CONFIG_RTS_YUV_S2_Y_BASE;
	rsocdev->icfgs[2].y.size  = CONFIG_RTS_YUV_S2_Y_SIZE;
	rsocdev->icfgs[2].uv.base = CONFIG_RTS_YUV_S2_UV_BASE;
	rsocdev->icfgs[2].uv.size = CONFIG_RTS_YUV_S2_UV_SIZE;
	rsocdev->icfgs[2].found = 1;
	i++;
#endif
#if defined(CONFIG_RTS_YUV_S3_Y_BASE)
	rsocdev->icfgs[3].y.base  = CONFIG_RTS_YUV_S3_Y_BASE;
	rsocdev->icfgs[3].y.size  = CONFIG_RTS_YUV_S3_Y_SIZE;
	rsocdev->icfgs[3].uv.base = CONFIG_RTS_YUV_S3_UV_BASE;
	rsocdev->icfgs[3].uv.size = CONFIG_RTS_YUV_S3_UV_SIZE;
	rsocdev->icfgs[3].found = 1;
	i++;
#endif

	rsocdev->icfg_count = i;
	if (i == 0) {
		rtsprintk(RTS_TRACE_ERROR, "no isp buffer config defined");
		return -EINVAL;
	}

#if defined(CONFIG_RTS_RGB_R_BASE)
	rsocdev->rgbcfg.rgb[0].base = CONFIG_RTS_RGB_R_BASE;
	rsocdev->rgbcfg.rgb[0].size = CONFIG_RTS_RGB_R_SIZE;
	rsocdev->rgbcfg.rgb[1].base = CONFIG_RTS_RGB_G_BASE;
	rsocdev->rgbcfg.rgb[1].size = CONFIG_RTS_RGB_G_SIZE;
	rsocdev->rgbcfg.rgb[2].base = CONFIG_RTS_RGB_B_BASE;
	rsocdev->rgbcfg.rgb[2].size = CONFIG_RTS_RGB_B_SIZE;
#endif

	return 0;
}

/* ------------------------------------------------------------------ */
/* Resume / suspend (called by ISP driver on AOV events)               */
/* ------------------------------------------------------------------ */

int rtscam_soc_resume(void)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;
	int ret;

	ret = rtscam_zoom_resume(&rsocdev->zoom_dev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "zoom reset fail: %d", ret);
		return ret;
	}

	rtscam_isp_resume(rsocdev->isp);

	if (rsocdev->video_reset) {
		ret = rtscam_rtos_reset_control_reset(rsocdev->video_reset);
		if (ret)
			rtsprintk(RTS_TRACE_ERROR,
					  "video reset fail: %d", ret);
	}

	if (rsocdev->video_sysmem) {
		ret = rtscam_rtos_reset_control_deassert(rsocdev->video_sysmem);
		if (ret)
			rtsprintk(RTS_TRACE_ERROR,
					  "video sysmem fail: %d", ret);
	}

	rtscam_soc_config_isp_buffer(rsocdev);
	rtscam_soc_config_rgb_buffer(rsocdev);
	rtscam_soc_enable_interrupt(rsocdev, 1);

	isp_irq_enable(ISP_IRQ_FRAME_DONE);

	return 0;
}

int rtscam_soc_suspend(void)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;

	rtscam_soc_enable_interrupt(rsocdev, 0);
	isp_irq_disable(ISP_IRQ_FRAME_DONE);

	rtscam_isp_suspend(rsocdev->isp);

	return 0;
}

/* ------------------------------------------------------------------ */
/* Driver init (replaces Zephyr rtscam_soc_init device callback)      */
/* ------------------------------------------------------------------ */

int rtscam_soc_init(void)
{
	struct rtscam_soc_dev *rsocdev = &soc_data;
	int ret = 0;

	if (rsocdev->initialized) {
		return 0;
	}

	memset(rsocdev, 0, sizeof(*rsocdev));

	rsocdev->base = CONFIG_RTS_SOC_CAM_BASE;
	rsocdev->size = CONFIG_RTS_SOC_CAM_SIZE;

	rsocdev->width_step  = 4;
	rsocdev->height_step = 4;
	rsocdev->height_align = RTS_VIDEOIN_HEIGHT_ALIGN;

	rsocdev->reg.yuv_axibuf_base     = RTS_REG_YUV_AXI_BUF_CONFIG_BASE;
	rsocdev->reg.yuv_base_val_bit    = 4;
	rsocdev->reg.yuv_size_val_bit    = 20;
	rsocdev->reg.rgb_axibuf_base     = RTS_REG_RGB_R_AXI_BUF_CONFIG;
	rsocdev->reg.rgb_axibuf_interval = RTS_REG_RGB_AXI_BUF_CONFIG_INTERVAL;
	rsocdev->reg.rgb_frame_len_r     = RTS_REG_RGB_FRAME_LEN_R;
	rsocdev->reg.rgb_frame_len_g     = RTS_REG_RGB_FRAME_LEN_G;
	rsocdev->reg.rgb_frame_len_b     = RTS_REG_RGB_FRAME_LEN_B;
	rsocdev->reg.y_frame_len_base    = RTS_REG_YUV_FRAME_LEN_Y_BASE;
	rsocdev->reg.uv_frame_len_base   = RTS_REG_YUV_FRAME_LEN_UV_BASE;
	rsocdev->reg.yuv_frame_len_interval = RTS_REG_YUV_FRAME_LEN_INTERVAL;
	rsocdev->reg.yuv_frame_buf_cnt   = RTS_REG_YUV_FRAME_BUFFER_COUNT;
	rsocdev->reg.rgb_frame_buf_cnt   = RTS_REG_RGB_YONLY_FRAME_BUFFER_COUNT;
	rsocdev->reg.yuv_interleave_select = RTS_REG_YUV_INTERLEAVE_SELECT;
	rsocdev->reg.isp_nv12_select     = RTS_REG_ISP_NV12_SELECT;
	rsocdev->reg.ringbuf_mode        = RTS_REG_RINGBUF_MODE;
	rsocdev->reg.ringbuf_rowend_delay = RTS_REG_RINGBUF_ROWEND_DELAY;
	rsocdev->reg.buf_status_base     = RTS_REG_YUV_FRAME_BUFFER_STATUS_BASE;

	isp_irq_register(ISP_IRQ_FRAME_DONE, rtscam_soc_isr, INT_PRI_HIGH);
	isp_irq_enable(ISP_IRQ_FRAME_DONE);

	rsocdev->video_reset  =
		rtscam_rtos_reset_control_get("video_reset");   /* TODO: add to reset.c */
	rsocdev->video_sysmem =
		rtscam_rtos_reset_control_get("video_sysmem");  /* TODO: add to reset.c */

	if (rsocdev->video_reset) {
		ret = rtscam_rtos_reset_control_reset(rsocdev->video_reset);
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "video reset fail: %d", ret);
			goto out;
		}
	}

	if (rsocdev->video_sysmem) {
		ret = rtscam_rtos_reset_control_deassert(rsocdev->video_sysmem);
		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "video sysmem fail: %d", ret);
			goto out;
		}
	}

	ret = rtscam_soc_init_buf_info(rsocdev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR,
				  "init buf info fail: %d", ret);
		goto out;
	}

	isp_rtos_mutex_init(&rsocdev->rvdev.dev_lock);

	ret = rtscam_zoom_init(&rsocdev->zoom_dev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "init zoom dev fail: %d", ret);
		goto out;
	}

	/* ISP pipeline was already probed by driver_init(); just get the pointer */
	rsocdev->isp = rtscam_isp_get_pipeline(0);
	if (!rsocdev->isp) {
		rtsprintk(RTS_TRACE_ERROR, "isp pipeline not probed yet");
		ret = -ENODEV;
		goto out1;
	}

	ret = rtscam_soc_attach(rsocdev);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "soc attach fail: %d", ret);
		goto out1;
	}

	rtscam_soc_enable_interrupt(rsocdev, 1);

	return 0;
out1:
	rtscam_zoom_release(&rsocdev->zoom_dev);
	rtscam_soc_detach(rsocdev);
out:
	return ret;
}
