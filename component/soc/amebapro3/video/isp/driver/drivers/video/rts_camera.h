/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS bare-metal:
 * - k_spinlock  -> isp_rtos_spinlock_t
 * - k_mutex     -> isp_rtos_mutex_t
 * - k_fifo      -> struct list_head (fifo_out) + spinlock
 * - k_poll_signal -> removed (no user-space)
 * - sys_dlist_t  -> struct list_head
 * - sys_dnode_t  -> struct list_head
 * - k_work_delayable -> TimerHandle_t
 * - atomic_t    -> typedef int atomic_t (same)
 * - LOG_*       -> rtsprintk via printf
 */

#ifndef _RTS_CAMERA_H
#define _RTS_CAMERA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <isp_rtos_spinlock.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_atomic.h>
#include <list.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include "rts_video.h"
#include "rts_camera_fps.h"

#define RTSCAM_VIDEO_SUPPORT_MAX_STREAM_NUM 5

#define rtscam_call_video_op(icd, op, args...) \
	(((icd)->ops->op) ? ((icd)->ops->op(args)) : -EINVAL)

#define v4l2pixfmtstr(x) \
	(unsigned char)((x) & 0xff), (unsigned char)(((x) >> 8) & 0xff), \
	(unsigned char)(((x) >> 16) & 0xff), (unsigned char)(((x) >> 24) & 0xff)

struct rtscam_frame_size {
	uint32_t width;
	uint32_t height;
};

struct rtscam_video_frmival {
	uint8_t initialized;
	struct rts_video_fps max;
	struct rts_video_fps min;
	struct rts_video_fps step;
};

struct rtscam_video_format {
	uint8_t index;
	char name[32];
	uint32_t fourcc;
	uint8_t bpp;
	bool is_yuv;
	uint32_t rts_code;

	uint8_t initialized;

	struct rtscam_frame_size max;
	struct rtscam_frame_size min;
	struct rtscam_frame_size step;
	struct rtscam_video_frmival frmival;

	struct rtscam_video_format *next;
};

struct rtscam_video_format_xlate {
	uint8_t index;
	const char name[32];
	uint32_t fourcc;
	uint8_t bpp;
	bool is_yuv;
	uint32_t rts_code;
};

enum {
	RTS_BUF_STATE_IDLE = 0,
	RTS_BUF_STATE_READY,
	RTS_BUF_STATE_HW,
	RTS_BUF_STATE_DELAYED,
	RTS_BUF_STATE_DONE,
	RTS_BUF_STATE_DEQUEUED,
	RTS_BUF_STATE_QUEUED,
	RTS_BUF_STATE_RESERVED
};

struct rtscam_video_buffer {
	uint32_t reserved;
	struct rts_video_buffer buf;
	struct list_head node;  /* replaces sys_dnode_t */
	int index;
	TimerHandle_t delay_timer; /* replaces k_work_delayable */
	int state;
};

enum rtscam_state {
	RTSCAM_STATE_PASSIVE = 0,
	RTSCAM_STATE_ACTIVE  = 1
};

struct rtscam_fh {
	enum rtscam_state state;
	struct rtscam_video_stream *stream;
};

#define RTSCAM_MAX_BUFFER_NUM 16
struct rtscam_video_stream {
	struct rtscam_video_device *icd;
	struct rtscam_fh *handle;
	uint8_t streamid;
	uint8_t capture_init;
	unsigned int buf_nums;
	struct rtscam_video_buffer *vbuf[RTSCAM_MAX_BUFFER_NUM];
	atomic_t active;                /* typedef int atomic_t */
	isp_rtos_spinlock_t lock;       /* replaces k_spinlock */
	isp_rtos_mutex_t stream_lock;   /* replaces k_mutex */
	struct list_head fifo_out;      /* replaces k_fifo: done-buffer list */
	struct list_head capture;       /* replaces sys_dlist_t */

	struct rtscam_video_format *user_formats;
	struct rtscam_video_fps fps;

	uint32_t rts_code;
	uint32_t user_format;
	uint32_t user_width;
	uint32_t user_height;
	uint32_t user_height_align;

	uint32_t bytesperline;
	uint32_t sizeimage;

	unsigned long sequence;
	unsigned long frame_count;
	unsigned long skip_count;
	unsigned long overflow_count;
	unsigned long error_count;
	unsigned long abnormal_count;

	unsigned long delay;
	unsigned long delta;

	int fov_status;

	uint8_t vin_mode;
	uint32_t ring_buf_height;
	uint8_t mode_initialized;

	uint8_t streaming;

	uint32_t ar_feature; //0:disable, 1:frame mode compress, 2: slice mode
	uint32_t ar_align_w;
};

static inline bool rtscam_is_streaming(struct rtscam_video_stream *stream)
{
	return stream->streaming;
}

struct rtscam_video_ops {
	int (*init_capture_buffers)(struct rtscam_video_stream *stream);
	int (*submit_buffer)(struct rtscam_video_stream *stream,
						 struct rtscam_video_buffer *buffer);
	int (*s_stream)(struct rtscam_video_stream *stream, int enable);

	int (*set_selection)(struct rtscam_video_stream *stream,
						 struct rts_video_selection *selection);
	int (*get_selection)(struct rtscam_video_stream *stream,
						 struct rts_video_selection *selection);
	int (*set_limit_color_range)(struct rtscam_video_stream *stream,
								 int *limit_enable);
	int (*get_limit_color_range)(struct rtscam_video_stream *stream,
								 int *limit_enable);
};

struct rtscam_video_device {
	unsigned streamnum;
	struct rtscam_video_stream streams[RTSCAM_VIDEO_SUPPORT_MAX_STREAM_NUM];
	struct rtscam_video_ops *ops;

	isp_rtos_mutex_t dev_lock;   /* replaces k_mutex */

	int streaming_count;

	void *priv;
	int initialized;
};

struct rtscam_region {
	uint32_t base;
	uint32_t size;
};

#define RTS_TRACE_ERROR   (1 << 0)
#define RTS_TRACE_PROBE   (1 << 1)
#define RTS_TRACE_CTRL    (1 << 2)
#define RTS_TRACE_IOCTL   (1 << 3)
#define RTS_TRACE_BUF     (1 << 4)
#define RTS_TRACE_VIDEO   (1 << 5)
#define RTS_TRACE_STATUS  (1 << 6)
#define RTS_TRACE_DEBUG   (1 << 7)
#define RTS_TRACE_INFO    (1 << 8)
#define RTS_TRACE_NOTICE  (1 << 9)
#define RTS_TRACE_WARNING (1 << 10)
#define RTS_TRACE_REG     (1 << 11)
#define RTS_TRACE_MEMINFO (1 << 12)
#define RTS_TRACE_DEV     (1 << 13)
#define RTS_TRACE_COMMAND (1 << 14)
#define RTS_TRACE_POWER   (1 << 15)
#define RTS_TRACE_PTS     (1 << 16)
#define RTS_TRACE_FPS     (1 << 17)
#define RTS_TRACE_WQ      (1 << 18)
#define RTS_TRACE_MD      (1 << 19)
#define RTS_TRACE_MCU     (1 << 20)

#define RTS_TRACE_DEFAULT (RTS_TRACE_ERROR | RTS_TRACE_INFO | RTS_TRACE_MCU)
// #define RTS_TRACE_DEFAULT (RTS_TRACE_ERROR | RTS_TRACE_INFO | RTS_TRACE_MCU | RTS_TRACE_WARNING | RTS_TRACE_NOTICE | RTS_TRACE_DEBUG)

#define rtsprintk(flag, fmt, ...) \
	do { \
		if (RTS_TRACE_DEFAULT & (int)(flag)) { \
			printf("[RTSCAM] " fmt "\n", ##__VA_ARGS__); \
		} \
	} while (0)

/* Buffer / format management */
int rtscam_register_format(struct rtscam_video_stream *stream,
						   struct rtscam_video_format_xlate *pfmt);
int rtscam_register_frame_stepwise(struct rtscam_video_stream *stream,
								   uint32_t fourcc,
								   struct rtscam_frame_size *max,
								   struct rtscam_frame_size *min,
								   struct rtscam_frame_size *step);
int rtscam_clr_format(struct rtscam_video_stream *stream, int keep);

int rtscam_push_back_ready_buffer(struct rtscam_video_stream *stream,
								  struct rtscam_video_buffer *buf);
int rtscam_submit_buffer(struct rtscam_video_stream *stream,
						 struct rtscam_video_buffer *buf);
void rtscam_buffer_done(struct rtscam_video_stream *stream,
						struct rtscam_video_buffer *rbuf,
						unsigned long bytesused);
struct rtscam_video_format *find_format_by_fourcc(
	struct rtscam_video_stream *stream, uint32_t fourcc);

int rtscam_check_stream_format(struct rtscam_video_stream *stream);
int rtscam_check_user_format(struct rtscam_video_stream *stream,
							 uint32_t user_format, uint32_t user_width,
							 uint32_t user_height, uint32_t user_numerator,
							 uint32_t user_denominator);
int rtscam_try_user_format(struct rtscam_video_stream *stream,
						   uint32_t user_format, uint32_t user_width, uint32_t user_height);
int rtscam_set_user_format(struct rtscam_video_stream *stream,
						   uint32_t user_format, uint32_t user_width, uint32_t user_height);
int rtscam_set_user_frmival(struct rtscam_video_stream *stream,
							uint32_t user_numerator, uint32_t user_denominator);
int rtscam_get_max_frame_size(struct rtscam_video_format *format,
							  struct rtscam_frame_size *max);
struct rtscam_video_frmival *rtscam_get_video_frmival(
	struct rtscam_video_stream *stream, uint32_t fourcc,
	uint32_t width, uint32_t height);
int rtscam_clr_frmival(struct rtscam_video_frmival *frmival);

#define RTSCAM_ALIGN(x, a) (((x) + (a) - 1) & (~((a) - 1)))

/* SOC helper */
struct rtscam_video_stream *rtscam_soc_get_video_stream(unsigned int streamid);
int rtscam_get_video_in_buf_status(int streamid, uint32_t *val);
int rtscam_set_video_in_buf_status(int streamid, uint32_t val);
int rtscam_check_pixel_cnt(void);

/* Video stream management API (replaces rts_video_* Zephyr inline functions) */
int rtscam_video_init(void);
int rtscam_video_open(unsigned int streamid);
int rtscam_video_close(unsigned int streamid);
int rtscam_video_set_format(unsigned int streamid, struct rts_video_format *fmt);
int rtscam_video_get_format(unsigned int streamid, struct rts_video_format *fmt);
int rtscam_video_stream_start(unsigned int streamid);
int rtscam_video_stream_stop(unsigned int streamid);
struct rts_video_buffer *rtscam_video_buffer_alloc(unsigned int streamid,
		unsigned int flag,
		size_t size);
int rtscam_video_buffer_release(unsigned int streamid,
								struct rts_video_buffer *buf);
int rtscam_video_enqueue(unsigned int streamid, struct rts_video_buffer *buf);
int rtscam_video_dequeue(unsigned int streamid, struct rts_video_buffer **buf);
int rtscam_video_set_fps(unsigned int streamid, struct rts_video_fps *fps);
int rtscam_video_get_fps(unsigned int streamid, struct rts_video_fps *fps);
int rtscam_video_set_selection(unsigned int streamid,
							   struct rts_video_selection *selection);
int rtscam_video_get_selection(unsigned int streamid,
							   struct rts_video_selection *selection);
int rtscam_video_get_caps(unsigned int streamid, struct rts_video_caps *caps);
int rtscam_video_set_vin_mode(unsigned int streamid, unsigned int mode);

#endif /* _RTS_CAMERA_H */
