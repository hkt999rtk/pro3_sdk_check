/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * ISP verify/input (TX) stream driver.
 *
 * Feeds one GE-reshaped RAW buffer at a time into the ISP verify hardware
 * path: submit_buffer() sends the phys addr via RTSCAM_ISP_SET_BUFFER,
 * s_stream(1) programs the behavior via RTSCAM_ISP_SET_BEHAVIOR and starts
 * a dedicated stream_thread that serializes submit -> wait-for-verify-done
 * -> rtscam_buffer_done() -> repeat, matching the verify hardware's
 * one-frame-at-a-time model.
 *
 * See openspec/changes/add-isp-input-verify-stream/design.md for the
 * rationale (fixed streamid outside the zoom-derived capture range,
 * bounded drop-newest ready-list, direct exported IRQ wakeup, bounded
 * completion wait with recovery).
 */

#include <errno.h>
#include <string.h>
#include <list.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <isp_rtos_completion.h>
#include <isp_rtos_spinlock.h>
#include <isp_rtos_atomic.h>
#include <rts_camera_isp.h>
#include "rtscam_isp_command.h"
#include "rtscam_isp_pipeline.h"
#include "rtscam_isp_input.h"
#include "../video/rts_camera.h"
#include "../video/rts_camera_soc.h"

/* Driver-side dispatch into the co-located ISP server; single binary, no
 * cross-core hop. Defined in isplib/server/core/isp_client_message.c,
 * forward-declared here the same way rtscam_isp_pipeline.c does for
 * RTSCAM_ISP_MOD_CONTROL actions. */
int isp_client_message_simple(uint32_t isp_id, uint32_t mod_id, uint32_t action,
							  void *data, uint32_t len);

#define RTSCAM_INPUT_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 4)
#define RTSCAM_INPUT_WAIT_TIMEOUT_MS 500
#define RTSCAM_INPUT_STOP_JOIN_MS 50

struct rtscam_isp_input_dev {
	struct rtscam_video_stream stream;
	struct rtscam_video_device icd;

	isp_rtos_completion_t frame_done;
	SemaphoreHandle_t ready_sem;
	int ready_count;
	struct rtscam_video_buffer *inflight;

	uint32_t ddr_mode;
	uint32_t fmt;
	uint32_t behavior_width;
	uint32_t behavior_height;
	uint32_t fps_numerator;
	uint32_t fps_denominator;

	TaskHandle_t task_handle;
	volatile int should_stop;
	unsigned long drop_count;

	int initialized;
};

static struct rtscam_isp_input_dev m_input;

static int rtscam_isp_input_submit_buffer(struct rtscam_video_stream *stream,
		struct rtscam_video_buffer *rbuf)
{
	struct rtscam_isp_input_dev *idev = &m_input;
	struct rtscam_isp_input_buffer_msg msg;
	int ret;

	(void)stream;

	if (!rbuf->buf.phy_addr) {
		rtsprintk(RTS_TRACE_ERROR, "input buffer has no phy_addr");
		return -EINVAL;
	}

	msg.phy_addr = rbuf->buf.phy_addr;
	ret = isp_client_message_simple(RTSCAM_ISPC_PIPELINE_0,
									RTSCAM_ISP_MOD_INPUT,
									RTSCAM_ISP_SET_BUFFER, &msg,
									sizeof(msg));
	if (ret) {
		return ret;
	}

	idev->inflight = rbuf;
	return 0;
}

static int rtscam_isp_input_task_start(struct rtscam_isp_input_dev *idev);
static void rtscam_isp_input_task_stop(struct rtscam_isp_input_dev *idev);

static int rtscam_isp_input_s_stream(struct rtscam_video_stream *stream, int enable)
{
	struct rtscam_isp_input_dev *idev = &m_input;
	struct rtscam_isp_input_behavior behavior;
	struct rtscam_video_buffer *buf, *tmp;
	int ret;

	if (enable) {
		memset(&behavior, 0, sizeof(behavior));
		behavior.ddr_mode = idev->ddr_mode;
		behavior.fmt = idev->fmt;
		behavior.width = idev->behavior_width;
		behavior.height = idev->behavior_height;
		behavior.fival.numerator = idev->fps_numerator;
		behavior.fival.denominator = idev->fps_denominator;

		ret = isp_client_message_simple(RTSCAM_ISPC_PIPELINE_0,
										RTSCAM_ISP_MOD_INPUT,
										RTSCAM_ISP_SET_BEHAVIOR,
										&behavior, sizeof(behavior));
		if (ret) {
			return ret;
		}

		return rtscam_isp_input_task_start(idev);
	}

	rtscam_isp_input_task_stop(idev);

	isp_rtos_spin_lock(&stream->lock);
	list_for_each_entry_safe(buf, tmp, &stream->capture, node) {
		list_del(&buf->node);
		INIT_LIST_HEAD(&buf->node);
		rtscam_buffer_done(stream, buf, 0);
	}
	idev->ready_count = 0;
	if (idev->inflight) {
		rtscam_buffer_done(stream, idev->inflight, 0);
		idev->inflight = NULL;
	}
	isp_rtos_spin_unlock(&stream->lock);

	return 0;
}

static struct rtscam_video_ops rtscam_isp_input_ops = {
	.submit_buffer = rtscam_isp_input_submit_buffer,
	.s_stream      = rtscam_isp_input_s_stream,
};

static void rtscam_isp_input_task(void *arg)
{
	struct rtscam_isp_input_dev *idev = &m_input;
	struct rtscam_video_stream *stream = &idev->stream;
	struct rtscam_video_buffer *rbuf;
	int ret;

	(void)arg;

	while (!idev->should_stop) {
		if (xSemaphoreTake(idev->ready_sem, pdMS_TO_TICKS(200)) != pdTRUE) {
			continue;
		}
		if (idev->should_stop) {
			break;
		}

		isp_rtos_spin_lock(&stream->lock);
		ret = rtscam_submit_buffer(stream, NULL);
		if (!ret) {
			idev->ready_count--;
		}
		isp_rtos_spin_unlock(&stream->lock);

		if (ret) {
			/* spurious wake (e.g. from task_stop); nothing to do */
			continue;
		}

		ret = isp_rtos_wait_for_completion_timeout(
				  &idev->frame_done,
				  pdMS_TO_TICKS(RTSCAM_INPUT_WAIT_TIMEOUT_MS));

		rbuf = idev->inflight;
		idev->inflight = NULL;
		if (!rbuf) {
			continue;
		}

		if (ret) {
			rtsprintk(RTS_TRACE_ERROR,
					  "input verify-done timeout, recovering buffer");
			isp_rtos_spin_lock(&stream->lock);
			rtscam_buffer_done(stream, rbuf, 0);
			isp_rtos_spin_unlock(&stream->lock);
			continue;
		}

		isp_rtos_spin_lock(&stream->lock);
		rtscam_buffer_done(stream, rbuf, rbuf->buf.size);
		isp_rtos_spin_unlock(&stream->lock);
	}

	idev->task_handle = NULL;
	vTaskDelete(NULL);
}

static int rtscam_isp_input_task_start(struct rtscam_isp_input_dev *idev)
{
	BaseType_t ret;

	if (idev->task_handle) {
		return 0;
	}

	idev->should_stop = 0;
	idev->ready_count = 0;
	idev->inflight = NULL;
	isp_rtos_reinit_completion(&idev->frame_done);
	xSemaphoreTake(idev->ready_sem, 0);

	ret = xTaskCreate(rtscam_isp_input_task, "isp_input",
					  RTSCAM_INPUT_TASK_STACK_SIZE, NULL,
					  (portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)),
					  &idev->task_handle);
	if (ret != pdPASS) {
		idev->task_handle = NULL;
		return -ENOMEM;
	}

	return 0;
}

static void rtscam_isp_input_task_stop(struct rtscam_isp_input_dev *idev)
{
	if (!idev->task_handle) {
		return;
	}

	idev->should_stop = 1;
	isp_rtos_complete(&idev->frame_done);
	xSemaphoreGive(idev->ready_sem);

	/* Task self-deletes at the top of its loop once should_stop is seen;
	 * give it a moment rather than reaching in to force-delete it. */
	vTaskDelay(pdMS_TO_TICKS(RTSCAM_INPUT_STOP_JOIN_MS));
}

int rtscam_isp_input_set_behavior(uint32_t ddr_mode, uint32_t fmt,
								  uint32_t width, uint32_t height,
								  uint32_t fps_numerator,
								  uint32_t fps_denominator)
{
	struct rtscam_isp_input_dev *idev = &m_input;

	if (!idev->initialized) {
		return -EINVAL;
	}
	if (rtscam_is_streaming(&idev->stream)) {
		return -EBUSY;
	}

	idev->ddr_mode = ddr_mode;
	idev->fmt = fmt;
	idev->behavior_width = width;
	idev->behavior_height = height;
	idev->fps_numerator = fps_numerator;
	idev->fps_denominator = fps_denominator;

	return 0;
}

int rtscam_isp_input_enqueue(struct rts_video_buffer *buf)
{
	struct rtscam_isp_input_dev *idev = &m_input;
	struct rtscam_video_stream *stream = &idev->stream;
	struct rtscam_video_buffer *rbuf;
	int drop = 0;

	if (!idev->initialized || !buf) {
		return -EINVAL;
	}

	rbuf = container_of(buf, struct rtscam_video_buffer, buf);

	isp_rtos_spin_lock(&stream->lock);
	if (idev->ready_count >= RTSCAM_INPUT_MAX_READY) {
		drop = 1;
	} else {
		rtscam_push_back_ready_buffer(stream, rbuf);
		idev->ready_count++;
	}
	isp_rtos_spin_unlock(&stream->lock);

	if (drop) {
		idev->drop_count++;
		rtsprintk(RTS_TRACE_ERROR,
				  "input ready list full (%d), dropping newest buffer",
				  RTSCAM_INPUT_MAX_READY);
		return -ENOSPC;
	}

	xSemaphoreGive(idev->ready_sem);
	return 0;
}

int rtscam_isp_input_dequeue(struct rts_video_buffer **buf)
{
	return rtscam_video_dequeue(RTSCAM_INPUT_STRM_IDX, buf);
}

unsigned long rtscam_isp_input_get_drop_count(void)
{
	return m_input.drop_count;
}

void rtscam_isp_input_frame_done(void)
{
	if (!m_input.initialized) {
		return;
	}
	isp_rtos_complete(&m_input.frame_done);
}

struct rtscam_video_stream *rtscam_isp_input_get_stream(void)
{
	if (!m_input.initialized) {
		return NULL;
	}
	return &m_input.stream;
}

int rtscam_isp_input_init(void)
{
	struct rtscam_isp_input_dev *idev = &m_input;
	struct rtscam_video_stream *stream = &idev->stream;
	int ret;

	if (idev->initialized) {
		return 0;
	}

	memset(idev, 0, sizeof(*idev));

	ret = isp_rtos_spin_lock_init(&stream->lock);
	if (ret) {
		return ret;
	}
	ret = isp_rtos_mutex_init(&stream->stream_lock);
	if (ret) {
		return ret;
	}
	INIT_LIST_HEAD(&stream->capture);
	INIT_LIST_HEAD(&stream->fifo_out);

	stream->streamid = RTSCAM_INPUT_STRM_IDX;
	stream->vin_mode = RTS_VIN_MODE_NO_MEM;
	stream->icd = &idev->icd;

	ret = isp_rtos_mutex_init(&idev->icd.dev_lock);
	if (ret) {
		return ret;
	}
	idev->icd.ops = &rtscam_isp_input_ops;
	idev->icd.priv = idev;
	idev->icd.streamnum = 1;
	idev->icd.initialized = 1;

	ret = isp_rtos_init_completion(&idev->frame_done);
	if (ret) {
		return ret;
	}

	idev->ready_sem = xSemaphoreCreateCounting(RTSCAM_INPUT_MAX_READY, 0);
	if (!idev->ready_sem) {
		return -ENOMEM;
	}

	idev->initialized = 1;
	return 0;
}
