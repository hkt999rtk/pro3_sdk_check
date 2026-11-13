/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS bare-metal:
 * - k_fifo_put/get       -> list_add_tail / list_first_entry under spinlock
 * - k_work_delayable     -> TimerHandle_t (one-shot timer per buffer)
 * - k_poll_signal        -> removed (no user-space on MCU)
 * - k_spin_lock/unlock   -> isp_rtos_spin_lock/unlock
 * - k_mutex_lock/unlock  -> isp_rtos_mutex_lock/unlock
 * - k_calloc/k_free      -> pvPortMalloc+memset / vPortFree
 * - sys_dlist_*          -> list_* (from list.h)
 * - k_ticks_to_us        -> pdTICKS_TO_MS * 1000 (coarse timestamp)
 * - const struct device *dev -> uses global soc_data via rtscam_soc_get_video_stream()
 */

#include <string.h>
#include <errno.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include "rts_camera.h"
#include "rts_camera_hw_config.h"
#include "hal_isp.h"
#include "rts_camera_mem.h"
#include "rts_camera_soc.h"
#include "rts_isp_sensor.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

/* Forward declaration — defined in "Stream start / stop" section below */
static int rtscam_video_queue_enable(struct rtscam_video_stream *stream,
									 int enable);

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static int rtscam_set_buffer_state(struct rtscam_video_buffer *buffer, int state)
{
	if (!buffer) {
		return -EINVAL;
	}

	if (state < RTS_BUF_STATE_IDLE || state >= RTS_BUF_STATE_RESERVED) {
		return -EINVAL;
	}

	buffer->state = state;
	return 0;
}

static void __rtscam_buffer_done(struct rtscam_video_stream *stream,
								 struct rtscam_video_buffer *buffer)
{
	if (!buffer || !stream) {
		return;
	}

	rtscam_set_buffer_state(buffer, RTS_BUF_STATE_DONE);
	list_add_tail(&buffer->node, &stream->fifo_out);
}

/* Delay timer callback — fires in timer task context */
static void rtscam_buffer_timer_cb(TimerHandle_t xTimer)
{
	struct rtscam_video_buffer *rbuf =
		(struct rtscam_video_buffer *)pvTimerGetTimerID(xTimer);
	struct rtscam_video_stream *stream;

	if (!rbuf) {
		return;
	}

	stream = (struct rtscam_video_stream *)rbuf->buf.driver_data;
	if (!stream) {
		return;
	}

	isp_rtos_spin_lock(&stream->lock);
	__rtscam_buffer_done(stream, rbuf);
	isp_rtos_spin_unlock(&stream->lock);
}

static void rtscam_delay_buffer(struct rtscam_video_buffer *rbuf,
								unsigned long delay_ms)
{
	if (!rbuf->delay_timer) {
		rbuf->delay_timer = xTimerCreate("rtscam_buf",
										 pdMS_TO_TICKS(delay_ms),
										 pdFALSE,
										 rbuf,
										 rtscam_buffer_timer_cb);
	} else {
		xTimerChangePeriod(rbuf->delay_timer,
						   pdMS_TO_TICKS(delay_ms),
						   pdMS_TO_TICKS(10));
	}
	rtscam_set_buffer_state(rbuf, RTS_BUF_STATE_DELAYED);
	xTimerStart(rbuf->delay_timer, pdMS_TO_TICKS(10));
}

static void rtscam_clr_delay_buffer(struct rtscam_video_stream *stream)
{
	struct rtscam_video_buffer *buffer;
	int i;

	if (!stream) {
		return;
	}

	for (i = 0; i < (int)stream->buf_nums; i++) {
		buffer = stream->vbuf[i];
		if (!buffer) {
			continue;
		}

		if (buffer->state == RTS_BUF_STATE_DELAYED) {
			if (buffer->delay_timer) {
				xTimerStop(buffer->delay_timer, pdMS_TO_TICKS(10));
			}
			rtscam_set_buffer_state(buffer, RTS_BUF_STATE_IDLE);
		}
	}
}

static uint64_t rtscam_get_timestamp(void)
{
	return (uint64_t)xTaskGetTickCount() * portTICK_PERIOD_MS * 1000ULL;
}

/* ------------------------------------------------------------------ */
/* Public buffer API                                                   */
/* ------------------------------------------------------------------ */

void rtscam_buffer_done(struct rtscam_video_stream *stream,
						struct rtscam_video_buffer *rbuf,
						unsigned long bytesused)
{
	if (!stream || !rbuf) {
		return;
	}

	rbuf->buf.timestamp = rtscam_get_timestamp();
	rbuf->buf.bytesused = bytesused;

	if (stream->delay) {
		rtscam_delay_buffer(rbuf, stream->delay);
	} else {
		__rtscam_buffer_done(stream, rbuf);
	}
}

int rtscam_push_back_ready_buffer(struct rtscam_video_stream *stream,
								  struct rtscam_video_buffer *buf)
{
	struct rtscam_video_buffer *rbuf;

	if (!stream || !buf) {
		return -EINVAL;
	}

	list_for_each_entry(rbuf, &stream->capture, node) {
		if (rbuf == buf) {
			return 0;
		}
	}

	rtscam_set_buffer_state(buf, RTS_BUF_STATE_READY);
	list_add_tail(&buf->node, &stream->capture);

	return 0;
}

static struct rtscam_video_buffer *rtscam_pop_ready_buffer(
	struct rtscam_video_stream *stream)
{
	struct rtscam_video_buffer *buf = NULL;

	if (!stream || list_empty(&stream->capture)) {
		return NULL;
	}

	buf = list_first_entry(&stream->capture, struct rtscam_video_buffer, node);
	list_del(&buf->node);
	INIT_LIST_HEAD(&buf->node);

	return buf;
}

int rtscam_submit_buffer(struct rtscam_video_stream *stream,
						 struct rtscam_video_buffer *buf)
{
	struct rtscam_video_buffer *rbuf = NULL;
	int ret;

	if (buf) {
		rtscam_push_back_ready_buffer(stream, buf);
	}

	rbuf = rtscam_pop_ready_buffer(stream);
	if (!rbuf) {
		return -EINVAL;
	}

	ret = rtscam_call_video_op(stream->icd, submit_buffer, stream, rbuf);
	if (ret) {
		rtscam_push_back_ready_buffer(stream, rbuf);
		return ret;
	}

	rtscam_set_buffer_state(rbuf, RTS_BUF_STATE_HW);
	return 0;
}

/* ------------------------------------------------------------------ */
/* Privilege / handle management                                       */
/* ------------------------------------------------------------------ */

static int rtscam_acquire_privileges(struct rtscam_video_stream *stream)
{
	if (!stream || !stream->handle) {
		return -EINVAL;
	}

	if (rtscam_is_streaming(stream)) {
		return -EBUSY;
	}

	if (stream->handle->state == RTSCAM_STATE_ACTIVE) {
		return 0;
	}

	isp_rtos_atomic_inc(&stream->active);
	if (isp_rtos_atomic_read(&stream->active) != 1) {
		isp_rtos_atomic_dec(&stream->active);
		return -EBUSY;
	}

	stream->handle->state = RTSCAM_STATE_ACTIVE;
	return 0;
}

static void rtscam_dismiss_privileges(struct rtscam_video_stream *stream)
{
	if (!stream || !stream->handle) {
		return;
	}

	if (stream->handle->state == RTSCAM_STATE_ACTIVE) {
		isp_rtos_atomic_dec(&stream->active);
	}

	stream->handle->state = RTSCAM_STATE_PASSIVE;
}

static int rtscam_has_privileges(struct rtscam_video_stream *stream)
{
	if (!stream || !stream->handle) {
		return 0;
	}
	return stream->handle->state == RTSCAM_STATE_ACTIVE;
}

/* ------------------------------------------------------------------ */
/* Open / enable / close                                               */
/* ------------------------------------------------------------------ */

int rtscam_video_open(unsigned int streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_fh *handle = NULL;

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	if (stream->handle) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -EBUSY;
	}

	handle = pvPortMalloc(sizeof(*handle));
	if (!handle) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -ENOMEM;
	}
	memset(handle, 0, sizeof(*handle));

	handle->stream = stream;
	handle->state = RTSCAM_STATE_PASSIVE;
	stream->handle = handle;
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;
}

static int rtscam_video_enable(struct rtscam_video_stream *stream, int enable)
{
	int ret = 0;

	if (enable) {
		if (rtscam_is_streaming(stream)) {
			return -EINVAL;
		}

		if (stream->vin_mode != RTS_VIN_MODE_NO_MEM) {
			if (!stream->buf_nums) {
				rtsprintk(RTS_TRACE_ERROR,
						  "stream<%d> no buffers allocated (buf_nums=0)",
						  stream->streamid);
				return -EINVAL;
			}
		}

		ret = rtscam_video_queue_enable(stream, 1);
		if (ret) {
			return ret;
		}
		ret = rtscam_call_video_op(stream->icd, s_stream, stream, 1);
		if (ret) {
			rtscam_video_queue_enable(stream, 0);
		} else {
			stream->icd->streaming_count++;
		}
	} else {
		if (!rtscam_is_streaming(stream)) {
			return 0;
		}

		ret = rtscam_call_video_op(stream->icd, s_stream, stream, 0);
		if (!ret) {
			ret = rtscam_video_queue_enable(stream, 0);
		}

		if (!ret) {
			stream->icd->streaming_count--;
		}
	}

	return ret;
}

int rtscam_video_close(unsigned int streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_video_device *icd;
	struct rtscam_fh *handle;

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	icd = stream->icd;
	handle = stream->handle;

	if (rtscam_has_privileges(stream)) {
		isp_rtos_mutex_lock(&icd->dev_lock);
		rtscam_video_enable(stream, 0);
		stream->mode_initialized = 0;
		isp_rtos_mutex_unlock(&icd->dev_lock);
	}
	rtscam_dismiss_privileges(stream);
	rtscam_clr_delay_buffer(stream);

	vPortFree(handle);
	stream->handle = NULL;
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;
}

/* ------------------------------------------------------------------ */
/* Selection ops (internal helpers)                                    */
/* ------------------------------------------------------------------ */
static int rtscam_video_set_selection_op(struct rtscam_video_stream *stream,
		struct rts_video_selection *selection)
{
	return rtscam_call_video_op(stream->icd, set_selection, stream,
								selection);
}

static int rtscam_video_get_selection_op(struct rtscam_video_stream *stream,
		struct rts_video_selection *selection)
{
	return rtscam_call_video_op(stream->icd, get_selection, stream,
								selection);
}

/* ------------------------------------------------------------------ */
/* Format (internal helpers + public API)                              */
/* ------------------------------------------------------------------ */

static int rtscam_check_fmt_by_crop_info(struct rtscam_video_stream *stream,
		uint32_t width, uint32_t height)
{
	struct rts_video_selection selection;
	int ret;

	ret = rtscam_video_get_selection_op(stream, &selection);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "get selection fail");
		return ret;
	}

	if (!stream->streamid) {
		if (width > 5 * selection.width / 2 ||
			height > 5 * selection.height / 2) {
			ret = -EINVAL;
		}
	} else {
		if (width > selection.width || height > selection.height) {
			ret = -EINVAL;
		}
	}

	if (ret)
		rtsprintk(RTS_TRACE_ERROR,
				  "invalid resolution(%ux%u) for crop(%ux%u)",
				  width, height, selection.width, selection.height);
	return ret;
}

static int rtscam_check_crop_info_by_fmt(struct rtscam_video_stream *stream,
		struct rts_video_selection *selection)
{
	int ret = 0;

	if (!stream->streamid) {
		if (stream->user_width > 5 * selection->width / 2 ||
			stream->user_height > 5 * selection->height / 2) {
			ret = -EINVAL;
		}
	} else {
		if (stream->user_width > selection->width ||
			stream->user_height > selection->height) {
			ret = -EINVAL;
		}
	}

	if (ret)
		rtsprintk(RTS_TRACE_ERROR,
				  "invalid crop(%ux%u) for resolution(%ux%u)",
				  selection->width, selection->height,
				  stream->user_width, stream->user_height);
	return ret;
}

static int rtscam_video_try_fmt(struct rtscam_video_stream *stream,
								struct rts_video_format *fmt)
{
	int ret;

	ret = rtscam_try_user_format(stream, fmt->pixelformat,
								 fmt->width, fmt->height);
	if (ret)
		rtsprintk(RTS_TRACE_ERROR,
				  "format (0x%08x) %ux%u unsupported",
				  fmt->pixelformat, fmt->width, fmt->height);
	return ret;
}

static int rtscam_video_set_fmt(struct rtscam_video_stream *stream,
								struct rts_video_format *fmt)
{
	int ret;

	ret = rtscam_video_try_fmt(stream, fmt);
	if (ret) {
		return ret;
	}

	if (rtscam_is_streaming(stream)) {
		return -EBUSY;
	}

	ret = rtscam_check_fmt_by_crop_info(stream, fmt->width, fmt->height);
	if (ret) {
		return ret;
	}

	return rtscam_set_user_format(stream, fmt->pixelformat,
								  fmt->width, fmt->height);
}

int rtscam_video_set_format(unsigned int streamid, struct rts_video_format *fmt)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int ret;

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	if (stream->buf_nums > 0) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -EBUSY;
	}

	ret = rtscam_acquire_privileges(stream);
	if (ret < 0) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return ret;
	}

	ret = rtscam_video_set_fmt(stream, fmt);
	if (ret) {
		rtscam_dismiss_privileges(stream);
	}
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return ret;
}

static int rtscam_video_get_fmt(struct rtscam_video_stream *stream,
								struct rts_video_format *fmt)
{
	fmt->pixelformat = stream->user_format;
	fmt->width       = stream->user_width;
	fmt->height      = stream->user_height;
	return 0;
}

int rtscam_video_get_format(unsigned int streamid, struct rts_video_format *fmt)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	if (!stream || !fmt) {
		return -EINVAL;
	}

	return rtscam_video_get_fmt(stream, fmt);
}

/* ------------------------------------------------------------------ */
/* Format enumeration                                                  */
/* ------------------------------------------------------------------ */

static int rtscam_video_enum_fmt(struct rtscam_video_stream *stream,
								 int i, uint32_t *pixelformat)
{
	struct rtscam_video_format *format;
	int index = 0;

	if (i < 0) {
		i = 0;
	}

	format = stream->user_formats;
	while (format) {
		if (index == i) {
			break;
		}
		format = format->next;
		index++;
	}

	if (!format) {
		return -EINVAL;
	}

	*pixelformat = format->fourcc;
	return 0;
}

static int rtscam_video_enum_framesizes(struct rtscam_video_stream *stream,
										uint32_t pixelformat,
										struct rtscam_frame_size *max,
										struct rtscam_frame_size *min,
										struct rtscam_frame_size *step)
{
	struct rtscam_video_format *format;

	format = find_format_by_fourcc(stream, pixelformat);
	if (!format || !format->initialized) {
		return -EINVAL;
	}

	max->width  = format->max.width;
	max->height = format->max.height;
	min->width  = format->min.width;
	min->height = format->min.height;
	step->width  = format->step.width;
	step->height = format->step.height;

	return 0;
}

static int rtscam_video_enum_frameintervals(struct rtscam_video_stream *stream,
		uint32_t pixelformat,
		uint32_t width, uint32_t height,
		struct rts_video_fps *max,
		struct rts_video_fps *min,
		struct rts_video_fps *step)
{
	struct rtscam_video_frmival *frmival;

	frmival = rtscam_get_video_frmival(stream, pixelformat, width, height);
	if (!frmival || !frmival->initialized) {
		return -EINVAL;
	}

	*max  = frmival->max;
	*min  = frmival->min;
	*step = frmival->step;

	return 0;
}

/* ------------------------------------------------------------------ */
/* Buffer capture init / memory check / queue_buffer                  */
/* ------------------------------------------------------------------ */


static int rtscam_video_queue_buffer(struct rtscam_video_stream *stream,
									 struct rts_video_buffer *buf)
{
	struct rtscam_video_buffer *buffer =
		container_of(buf, struct rtscam_video_buffer, buf);

	isp_rtos_spin_lock(&stream->lock);
	buffer->index = buf->index;
	rtscam_submit_buffer(stream, buffer);
	isp_rtos_spin_unlock(&stream->lock);
	rtscam_set_buffer_state(buffer, RTS_BUF_STATE_QUEUED);

	return 0;
}

static void rtscam_init_capture_buffer(struct rtscam_video_stream *stream)
{
	isp_rtos_spin_lock(&stream->lock);
	INIT_LIST_HEAD(&stream->capture);
	INIT_LIST_HEAD(&stream->fifo_out);
	rtscam_call_video_op(stream->icd, init_capture_buffers, stream);
	stream->capture_init = 1;
	isp_rtos_spin_unlock(&stream->lock);
}

static int rtscam_check_memory(uint32_t memory)
{
	if (memory < RTS_MEMORY_KERNEL || memory >= RTS_MEMORY_RESERVED) {
		rtsprintk(RTS_TRACE_ERROR, "invalid memory type %d", memory);
		return -EINVAL;
	}
	return 0;
}

/* ------------------------------------------------------------------ */
/* Enqueue / dequeue                                                   */
/* ------------------------------------------------------------------ */

int rtscam_video_enqueue(unsigned int streamid, struct rts_video_buffer *buf)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int ret = 0;

	if (!stream || !buf) {
		return -EINVAL;
	}

	ret = rtscam_check_memory(buf->memory);
	if (ret) {
		return ret;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	if (!rtscam_has_privileges(stream)) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -EBUSY;
	}

	if (!stream->buf_nums) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -EINVAL;
	}

	if (!stream->capture_init) {
		rtscam_init_capture_buffer(stream);
	}

	rtscam_video_queue_buffer(stream, buf);

	isp_rtos_mutex_unlock(&stream->stream_lock);
	return 0;
}

/*
 * Non-blocking dequeue: returns 0 with *buf set if a done buffer is available,
 * -EAGAIN if the queue is empty.
 */
int rtscam_video_dequeue(unsigned int streamid, struct rts_video_buffer **buf)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_video_buffer *buffer = NULL;

	if (!stream || !buf) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	isp_rtos_spin_lock(&stream->lock);

	if (!list_empty(&stream->fifo_out)) {
		buffer = list_first_entry(&stream->fifo_out,
								  struct rtscam_video_buffer, node);
		list_del(&buffer->node);
		INIT_LIST_HEAD(&buffer->node);
	}

	isp_rtos_spin_unlock(&stream->lock);

	if (!buffer) {
		*buf = NULL;
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -EAGAIN;
	}

	*buf = &buffer->buf;
	rtscam_set_buffer_state(buffer, RTS_BUF_STATE_DEQUEUED);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;
}

/* ------------------------------------------------------------------ */
/* Buffer alloc / release                                              */
/* ------------------------------------------------------------------ */

struct rts_video_buffer *rtscam_video_buffer_alloc(unsigned int streamid,
		unsigned int flag,
		size_t size)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_video_buffer *rbuf = NULL;
	uint32_t memory = flag & 0xFFFF;
	int ret;

	if (!stream) {
		return NULL;
	}

	if (rtscam_check_memory(memory)) {
		return NULL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);

	if (stream->vin_mode == RTS_VIN_MODE_NORMAL
		&& (size < stream->sizeimage || !stream->sizeimage)) {
		rtsprintk(RTS_TRACE_ERROR, "size %u doesn't match sizeimage %u",
				  (unsigned)size, stream->sizeimage);
		goto out;
	}

	ret = rtscam_acquire_privileges(stream);
	if (ret) {
		goto out;
	}

	rbuf = pvPortMalloc(sizeof(*rbuf));
	if (!rbuf) {
		rtsprintk(RTS_TRACE_ERROR, "pvPortMalloc fail");
		goto out1;
	}
	memset(rbuf, 0, sizeof(*rbuf));

	if (memory == RTS_MEMORY_KERNEL) {
		rbuf->buf.vm_addr = rtscam_mem_alloc(size,
											 &rbuf->buf.phy_addr, flag & 0xFFFF0000, "ISP Frame");
		if (!rbuf->buf.vm_addr) {
			rtsprintk(RTS_TRACE_ERROR, "alloc isp buffer fail");
			goto out1;
		}
	}

	rbuf->buf.memory = memory;
	INIT_LIST_HEAD(&rbuf->node);
	rbuf->buf.size = (uint32_t)size;
	rbuf->buf.driver_data = stream;

	rtscam_set_buffer_state(rbuf, RTS_BUF_STATE_IDLE);
	stream->buf_nums++;
	if (stream->buf_nums > RTSCAM_MAX_BUFFER_NUM) {
		rtsprintk(RTS_TRACE_ERROR, "isp buffer outnumber %d",
				  RTSCAM_MAX_BUFFER_NUM);
		goto out2;
	}
	stream->vbuf[stream->buf_nums - 1] = rbuf;
	rbuf->buf.index = stream->buf_nums - 1;

	isp_rtos_mutex_unlock(&stream->stream_lock);
	return &rbuf->buf;

out2:
	if (rbuf->buf.vm_addr) {
		rtscam_mem_free(rbuf->buf.vm_addr);
	}
	stream->buf_nums--;
out1:
	vPortFree(rbuf);
	rbuf = NULL;
out:
	isp_rtos_mutex_unlock(&stream->stream_lock);
	return NULL;
}

#define ALIGN_UP(addr, align) \
    (((uintptr_t)(addr) + ((align) - 1)) & ~((uintptr_t)((align) - 1)))

int rtscam_video_fr_compress_buffer_alloc(unsigned int streamid,
		unsigned int flag,
		size_t size, uint32_t ar_align_w, uint32_t slot_num, struct rtscam_video_buffer **rbuf)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	uint32_t memory = flag & 0xFFFF;
	int ret;
	uint32_t i;
	uint32_t y_length;
	uint32_t align_w = 4096;

	if (!stream || !rbuf) {
		return -1;
	}

	y_length = ar_align_w * stream->user_height;

	if (rtscam_check_memory(memory)) {
		return -1;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);

	if (stream->vin_mode == RTS_VIN_MODE_NORMAL
		&& (size < stream->sizeimage || !stream->sizeimage)) {
		rtsprintk(RTS_TRACE_ERROR,
				  "size %u doesn't match sizeimage %u",
				  (unsigned)size, stream->sizeimage);
		goto out;
	}

	ret = rtscam_acquire_privileges(stream);
	if (ret) {
		goto out;
	}

	for (i = 0; i < slot_num; i++) {
		rbuf[i] = pvPortMalloc(sizeof(*rbuf[i]));
		if (!rbuf[i]) {
			rtsprintk(RTS_TRACE_ERROR, "pvPortMalloc fail");
			goto out1;
		}

		memset(rbuf[i], 0, sizeof(*rbuf[i]));
	}

	if (memory == RTS_MEMORY_KERNEL) {
		rbuf[0]->buf.vm_addr =
			rtscam_mem_alloc(size,
							 &rbuf[0]->buf.phy_addr,
							 flag & 0xFFFF0000,
							 "ISP Frame");

		if (!rbuf[0]->buf.vm_addr) {
			rtsprintk(RTS_TRACE_ERROR, "alloc isp buffer fail");
			goto out1;
		}

		for (i = 1; i < slot_num; i++) {
			rbuf[i]->buf.phy_addr =
				rbuf[i - 1]->buf.phy_addr + y_length;
			rbuf[i]->buf.phy_addr = ALIGN_UP(rbuf[i]->buf.phy_addr, align_w);
			rbuf[i]->buf.vm_addr =
				rbuf[i - 1]->buf.vm_addr + y_length;
			rbuf[i]->buf.vm_addr = ALIGN_UP(rbuf[i]->buf.vm_addr, align_w);
		}
	}

	for (i = 0; i < slot_num; i++) {
		rbuf[i]->buf.memory = memory;

		INIT_LIST_HEAD(&rbuf[i]->node);

		rbuf[i]->buf.size = (uint32_t)size;
		rbuf[i]->buf.driver_data = stream;

		rtscam_set_buffer_state(rbuf[i], RTS_BUF_STATE_IDLE);

		stream->buf_nums++;

		if (stream->buf_nums > RTSCAM_MAX_BUFFER_NUM) {
			rtsprintk(RTS_TRACE_ERROR,
					  "isp buffer outnumber %d",
					  RTSCAM_MAX_BUFFER_NUM);
			goto out2;
		}

		stream->vbuf[stream->buf_nums - 1] = rbuf[i];
		rbuf[i]->buf.index = stream->buf_nums - 1;
	}

	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;

out2:
	if (memory == RTS_MEMORY_KERNEL && rbuf[0]->buf.vm_addr) {
		rtscam_mem_free(rbuf[0]->buf.vm_addr);
	}

	stream->buf_nums -= i;

out1:
	for (i = 0; i < slot_num; i++) {
		if (rbuf[i]) {
			vPortFree(rbuf[i]);
			rbuf[i] = NULL;
		}
	}

out:
	isp_rtos_mutex_unlock(&stream->stream_lock);
	return -1;
}

int rtscam_video_slice_buffer_alloc(unsigned int streamid,
									unsigned int flag,
									size_t size, uint32_t ar_align_w, uint32_t slot_num, struct rtscam_video_buffer **rbuf)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	uint32_t memory = flag & 0xFFFF;
	int ret;
	uint32_t i;
	uint32_t y_length;
	uint32_t align_w = 4096;

	if (!stream || !rbuf) {
		return -1;
	}

	y_length = ar_align_w * stream->user_height;

	if (rtscam_check_memory(memory)) {
		return -1;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);

	if (stream->vin_mode == RTS_VIN_MODE_NORMAL
		&& (size < stream->sizeimage || !stream->sizeimage)) {
		rtsprintk(RTS_TRACE_ERROR,
				  "size %u doesn't match sizeimage %u",
				  (unsigned)size, stream->sizeimage);
		goto out;
	}

	ret = rtscam_acquire_privileges(stream);
	if (ret) {
		goto out;
	}

	for (i = 0; i < slot_num; i++) {
		rbuf[i] = pvPortMalloc(sizeof(*rbuf[i]));
		if (!rbuf[i]) {
			rtsprintk(RTS_TRACE_ERROR, "pvPortMalloc fail");
			goto out1;
		}

		memset(rbuf[i], 0, sizeof(*rbuf[i]));
	}

	if (memory == RTS_MEMORY_KERNEL) {
		rbuf[0]->buf.vm_addr =
			rtscam_mem_alloc(size,
							 &rbuf[0]->buf.phy_addr,
							 flag & 0xFFFF0000,
							 "ISP Frame");

		if (!rbuf[0]->buf.vm_addr) {
			rtsprintk(RTS_TRACE_ERROR, "alloc isp buffer fail");
			goto out1;
		}

		for (i = 1; i < slot_num; i++) {
			rbuf[i]->buf.phy_addr = rbuf[i - 1]->buf.phy_addr;
			rbuf[i]->buf.phy_addr = ALIGN_UP(rbuf[i]->buf.phy_addr, align_w);

			rbuf[i]->buf.vm_addr = rbuf[i - 1]->buf.vm_addr;
			rbuf[i]->buf.vm_addr = ALIGN_UP(rbuf[i]->buf.vm_addr, align_w);
		}
	}

	for (i = 0; i < slot_num; i++) {
		rbuf[i]->buf.memory = memory;

		INIT_LIST_HEAD(&rbuf[i]->node);

		rbuf[i]->buf.size = (uint32_t)size;
		rbuf[i]->buf.driver_data = stream;

		rtscam_set_buffer_state(rbuf[i], RTS_BUF_STATE_IDLE);

		stream->buf_nums++;

		if (stream->buf_nums > RTSCAM_MAX_BUFFER_NUM) {
			rtsprintk(RTS_TRACE_ERROR,
					  "isp buffer outnumber %d",
					  RTSCAM_MAX_BUFFER_NUM);
			goto out2;
		}

		stream->vbuf[stream->buf_nums - 1] = rbuf[i];
		rbuf[i]->buf.index = stream->buf_nums - 1;
	}

	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;

out2:
	if (memory == RTS_MEMORY_KERNEL && rbuf[0]->buf.vm_addr) {
		rtscam_mem_free(rbuf[0]->buf.vm_addr);
	}

	stream->buf_nums -= i;

out1:
	for (i = 0; i < slot_num; i++) {
		if (rbuf[i]) {
			vPortFree(rbuf[i]);
			rbuf[i] = NULL;
		}
	}

out:
	isp_rtos_mutex_unlock(&stream->stream_lock);
	return -1;
}

int rtscam_video_buffer_release(unsigned int streamid,
								struct rts_video_buffer *buf)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_video_buffer *rbuf;
	int i;

	if (!stream || !buf) {
		return -EINVAL;
	}

	rbuf = container_of(buf, struct rtscam_video_buffer, buf);

	isp_rtos_mutex_lock(&stream->stream_lock);

	if (buf->memory == RTS_MEMORY_KERNEL && buf->vm_addr) {
		rtscam_mem_free(buf->vm_addr);
	}

	if (rbuf->delay_timer) {
		xTimerDelete(rbuf->delay_timer, pdMS_TO_TICKS(10));
		rbuf->delay_timer = NULL;
	}

	for (i = 0; i < (int)stream->buf_nums; i++) {
		if (stream->vbuf[i] == rbuf) {
			stream->vbuf[i] = NULL;
			break;
		}
	}
	stream->buf_nums--;

	if (!stream->buf_nums) {
		rtscam_dismiss_privileges(stream);
	}

	vPortFree(rbuf);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;
}

/* ------------------------------------------------------------------ */
/* Flush                                                               */
/* ------------------------------------------------------------------ */

int rtscam_video_flush(unsigned int streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	if (!stream) {
		return -EINVAL;
	}

	/* Hardware buffer drain handled by stop_streaming path */
	return 0;
}

/* ------------------------------------------------------------------ */
/* Stream start / stop helpers                                         */
/* ------------------------------------------------------------------ */

static int rtscam_video_start_streaming(struct rtscam_video_stream *stream)
{
	/* Hardware stream-on is driven by s_stream op in rtscam_video_enable */
	return 0;
}

static void rtscam_clr_ready_buffer(struct rtscam_video_stream *stream)
{
	struct rtscam_video_buffer *buf, *tmp;

	if (!stream) {
		return;
	}

	list_for_each_entry_safe(buf, tmp, &stream->capture, node) {
		list_del(&buf->node);
		INIT_LIST_HEAD(&buf->node);
		rtscam_set_buffer_state(buf, RTS_BUF_STATE_IDLE);
	}
	INIT_LIST_HEAD(&stream->capture);
}

static void rtscam_return_buffers(struct rtscam_video_stream *stream)
{
	struct rtscam_video_buffer *buf, *tmp;

	if (list_empty(&stream->fifo_out)) {
		return;
	}

	list_for_each_entry_safe(buf, tmp, &stream->fifo_out, node) {
		list_del(&buf->node);
		INIT_LIST_HEAD(&buf->node);
		rtscam_set_buffer_state(buf, RTS_BUF_STATE_DONE);
	}
	INIT_LIST_HEAD(&stream->fifo_out);
}

static int rtscam_video_stop_streaming(struct rtscam_video_stream *stream)
{
	rtscam_clr_delay_buffer(stream);

	isp_rtos_spin_lock(&stream->lock);
	rtscam_clr_ready_buffer(stream);
	rtscam_return_buffers(stream);
	isp_rtos_spin_unlock(&stream->lock);

	return 0;
}

static int rtscam_video_queue_enable(struct rtscam_video_stream *stream,
									 int enable)
{
	int ret = 0;

	if (stream->vin_mode == RTS_VIN_MODE_NO_MEM) {
		goto out;
	}

	if (enable) {
		ret = rtscam_video_start_streaming(stream);
	} else {
		ret = rtscam_video_stop_streaming(stream);
	}

out:
	if (!ret) {
		stream->streaming = (uint8_t)enable;
	}

	return ret;
}

int rtscam_video_stream_start(unsigned int streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_video_device *icd;
	int ret;

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	icd = stream->icd;

	if (!rtscam_has_privileges(stream)) {
		isp_rtos_mutex_unlock(&stream->stream_lock);
		return -EBUSY;
	}

	isp_rtos_mutex_lock(&icd->dev_lock);
	ret = rtscam_video_enable(stream, 1);
	isp_rtos_mutex_unlock(&icd->dev_lock);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return ret;
}

void rtscam_video_set_remap_info(unsigned int streamid, uint32_t ar_feature, uint32_t ar_align_w)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	stream->ar_feature = ar_feature;
	stream->ar_align_w = ar_align_w;
}

int rtscam_video_stream_stop(unsigned int streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	struct rtscam_video_device *icd;
	int ret;

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	icd = stream->icd;

	isp_rtos_mutex_lock(&icd->dev_lock);
	ret = rtscam_video_enable(stream, 0);
	isp_rtos_mutex_unlock(&icd->dev_lock);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return ret;
}

/* ------------------------------------------------------------------ */
/* FPS                                                                 */
/* ------------------------------------------------------------------ */

int rtscam_video_set_fps(unsigned int streamid, struct rts_video_fps *fps)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int ret;

	if (!stream || !fps) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	ret = rtscam_set_user_frmival(stream, fps->numerator, fps->denominator);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return ret;
}

int rtscam_video_get_fps(unsigned int streamid, struct rts_video_fps *fps)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	if (!stream || !fps) {
		return -EINVAL;
	}

	if (rtscam_is_streaming(stream)) {
		fps->numerator   = stream->fps.user_actual.numerator;
		fps->denominator = stream->fps.user_actual.denominator;
	} else {
		fps->numerator   = stream->fps.user_setting.numerator;
		fps->denominator = stream->fps.user_setting.denominator;
	}

	return 0;
}

/* ------------------------------------------------------------------ */
/* Color range                                                         */
/* ------------------------------------------------------------------ */

int rtscam_video_get_limit_color_range(unsigned int streamid, int *limit_enable)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	if (!stream || !limit_enable) {
		return -EINVAL;
	}

	return rtscam_call_video_op(stream->icd, get_limit_color_range,
								stream, limit_enable);
}

int rtscam_video_set_limit_color_range(unsigned int streamid, int *limit_enable)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	if (!stream || !limit_enable) {
		return -EINVAL;
	}

	return rtscam_call_video_op(stream->icd, set_limit_color_range,
								stream, limit_enable);
}

/* ------------------------------------------------------------------ */
/* Selection / crop                                                    */
/* ------------------------------------------------------------------ */

int rtscam_video_get_selection(unsigned int streamid,
							   struct rts_video_selection *selection)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int ret;

	if (!stream || !selection) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	ret = rtscam_video_get_selection_op(stream, selection);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return ret;
}

int rtscam_video_set_selection(unsigned int streamid,
							   struct rts_video_selection *selection)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int ret;

	if (!stream || !selection) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);

	if (rtscam_is_streaming(stream)) {
		ret = rtscam_check_crop_info_by_fmt(stream, selection);
		if (ret) {
			isp_rtos_mutex_unlock(&stream->stream_lock);
			return -EINVAL;
		}
		selection->mode = 1; /* dynamic crop while streaming */
		ret = rtscam_video_set_selection_op(stream, selection);
	} else {
		ret = rtscam_acquire_privileges(stream);
		if (ret) {
			isp_rtos_mutex_unlock(&stream->stream_lock);
			return ret;
		}
		selection->mode = 0; /* static crop before streaming */
		ret = rtscam_video_set_selection_op(stream, selection);
		if (ret) {
			rtscam_dismiss_privileges(stream);
		}
	}

	isp_rtos_mutex_unlock(&stream->stream_lock);
	return ret;
}

/* ------------------------------------------------------------------ */
/* Capabilities                                                        */
/* ------------------------------------------------------------------ */

static struct rts_video_format_cap g_fmt_caps[8];

int rtscam_video_get_caps(unsigned int streamid, struct rts_video_caps *caps)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int i, ret;

	if (!stream || !caps) {
		return -EINVAL;
	}

	for (i = 0; i < (int)ARRAY_SIZE(g_fmt_caps); i++) {
		struct rtscam_frame_size max = {0}, min = {0}, step = {0};
		struct rts_video_fps fps_max = {0}, fps_min = {0}, fps_step = {0};

		ret = rtscam_video_enum_fmt(stream, i,
									&g_fmt_caps[i].pixelformat);
		if (ret) {
			break;
		}

		ret = rtscam_video_enum_framesizes(stream,
										   g_fmt_caps[i].pixelformat,
										   &max, &min, &step);
		if (ret) {
			break;
		}

		g_fmt_caps[i].width_max  = max.width;
		g_fmt_caps[i].width_min  = min.width;
		g_fmt_caps[i].width_step = step.width;
		g_fmt_caps[i].height_max  = max.height;
		g_fmt_caps[i].height_min  = min.height;
		g_fmt_caps[i].height_step = step.height;

		ret = rtscam_video_enum_frameintervals(stream,
											   g_fmt_caps[i].pixelformat,
											   g_fmt_caps[i].width_max,
											   g_fmt_caps[i].height_max,
											   &fps_max, &fps_min,
											   &fps_step);
		if (ret) {
			break;
		}

		g_fmt_caps[i].fps_max  = fps_max;
		g_fmt_caps[i].fps_min  = fps_min;
		g_fmt_caps[i].fps_step = fps_step;
	}

	caps->format_caps    = g_fmt_caps;
	caps->min_vbuf_count = 0;

	return i ? 0 : ret;
}

/* ------------------------------------------------------------------ */
/* VIN mode                                                            */
/* ------------------------------------------------------------------ */

int rtscam_video_set_vin_mode(unsigned int streamid, unsigned int mode)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	uint8_t vin_mode = (uint8_t)(mode & 0xFF);

	if (!stream) {
		return -EINVAL;
	}

	if (stream->mode_initialized) {
		return -EINVAL;
	}

	if (vin_mode > RTS_VIN_MODE_NORMAL) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	stream->vin_mode = vin_mode;
	if (stream->vin_mode == RTS_VIN_MODE_RING_MEM) {
		stream->ring_buf_height = (uint32_t)(mode >> 8);
		rtscam_set_user_fps_actual(&stream->fps);
	}
	rtscam_acquire_privileges(stream);
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;
}

/* ------------------------------------------------------------------ */
/* FOV mode                                                            */
/* ------------------------------------------------------------------ */

int rtscam_video_set_fov_mode(unsigned int streamid, unsigned int mode)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	stream->fov_status = mode ? 1 : 0;
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return 0;
}

int rtscam_video_get_fov_mode(unsigned int streamid)
{
	struct rtscam_video_stream *stream = rtscam_soc_get_video_stream(streamid);
	int val;

	if (!stream) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&stream->stream_lock);
	val = stream->fov_status;
	isp_rtos_mutex_unlock(&stream->stream_lock);

	return val;
}

/* ------------------------------------------------------------------ */
/* Signal (no-op: FreeRTOS has no k_poll equivalent)                  */
/* ------------------------------------------------------------------ */

int rtscam_video_set_signal(unsigned int streamid, void *signal)
{
	/* k_poll_signal not available on FreeRTOS bare-metal — no-op */
	(void)streamid;
	(void)signal;
	return 0;
}

/* ------------------------------------------------------------------ */
/* Init (AmebaPro3 only — no Zephyr device framework equivalent)      */
/* ------------------------------------------------------------------ */

enum rtscam_video_init_state {
	RTSCAM_VIDEO_INIT_UNINITIALIZED = 0,
	RTSCAM_VIDEO_INIT_IN_PROGRESS,
	RTSCAM_VIDEO_INIT_DONE,
};

int rtscam_video_init(void)
{
	static volatile int init_state = RTSCAM_VIDEO_INIT_UNINITIALIZED;
	static void *mem_base;
	int wait_ms = 0;
	int ret;

	for (;;) {
		taskENTER_CRITICAL();
		if (init_state == RTSCAM_VIDEO_INIT_DONE) {
			taskEXIT_CRITICAL();
			return 0;
		}
		if (init_state == RTSCAM_VIDEO_INIT_UNINITIALIZED) {
			init_state = RTSCAM_VIDEO_INIT_IN_PROGRESS;
			taskEXIT_CRITICAL();
			break;
		}
		taskEXIT_CRITICAL();
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	while (!rtscam_isp_get_pipeline(0)) {
		if (wait_ms >= 10000) {
			rtsprintk(RTS_TRACE_ERROR,
					  "rtscam_video_init: timeout waiting for ISP pipeline");
			ret = -ENODEV;
			goto err_reset_state;
		}
		vTaskDelay(pdMS_TO_TICKS(10));
		wait_ms += 10;
	}

	const uint32_t mem_size = ISP_Get4KMode() ? CONFIG_RTS_MEM_SIZE_4K : CONFIG_RTS_MEM_SIZE;
	mem_base = pvPortMalloc(mem_size);
	if (!mem_base) {
		rtsprintk(RTS_TRACE_ERROR,
				  "rtscam_video_init: pvPortMalloc(0x%x) failed",
				  (unsigned)mem_size);
		ret = -ENOMEM;
		goto err_reset_state;
	}

	ret = rtscam_mem_init((uint32_t)mem_base, mem_size);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "rtscam_mem_init failed: %d", ret);
		goto err_free_mem;
	}

	ret = rtscam_soc_init();
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "rtscam_soc_init failed: %d", ret);
		goto err_cleanup_mem;
	}

	taskENTER_CRITICAL();
	init_state = RTSCAM_VIDEO_INIT_DONE;
	taskEXIT_CRITICAL();

	return 0;

err_cleanup_mem:
	rtscam_mem_cleanup();
err_free_mem:
	vPortFree(mem_base);
	mem_base = NULL;
err_reset_state:
	taskENTER_CRITICAL();
	init_state = RTSCAM_VIDEO_INIT_UNINITIALIZED;
	taskEXIT_CRITICAL();
	return ret;
}
