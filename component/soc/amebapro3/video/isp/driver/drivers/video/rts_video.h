/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Stripped-down rts_video.h for FreeRTOS bare-metal (no Zephyr device model).
 */

#ifndef _RTS_VIDEO_H_
#define _RTS_VIDEO_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RTS_VIDEOIN_HEIGHT_ALIGN_RTS3917 32

enum {
	RTS_VIN_MODE_NO_MEM,
	RTS_VIN_MODE_RING_MEM,
	RTS_VIN_MODE_NORMAL
};

/*  Four-character-code (FOURCC) */
#define v4l2_fourcc(a, b, c, d) \
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define V4L2_PIX_FMT_128BIT_3PIXEL  v4l2_fourcc('R', '1', '2', '8')
#define V4L2_PIX_FMT_NV12    v4l2_fourcc('N', 'V', '1', '2')
#define V4L2_PIX_FMT_NV21    v4l2_fourcc('N', 'V', '2', '1')
#define V4L2_PIX_FMT_NV16    v4l2_fourcc('N', 'V', '1', '6')
#define V4L2_PIX_FMT_NV61    v4l2_fourcc('N', 'V', '6', '1')
#define V4L2_PIX_FMT_NV24    v4l2_fourcc('N', 'V', '2', '4')
#define V4L2_PIX_FMT_NV42    v4l2_fourcc('N', 'V', '4', '2')
#define V4L2_PIX_FMT_YUYV    v4l2_fourcc('Y', 'U', 'Y', 'V')
#define V4L2_PIX_FMT_YVYU    v4l2_fourcc('Y', 'V', 'Y', 'U')
#define V4L2_PIX_FMT_RGB24   v4l2_fourcc('R', 'G', 'B', '3')
#define V4L2_PIX_FMT_Y8I     v4l2_fourcc('Y', '8', 'I', ' ')
#define V4L2_PIX_FMT_YUV420M v4l2_fourcc('Y', 'M', '1', '2')
#define V4L2_PIX_FMT_MJPEG   v4l2_fourcc('M', 'J', 'P', 'G')
#define V4L2_PIX_FMT_H264    v4l2_fourcc('H', '2', '6', '4')
#define V4L2_PIX_FMT_BGR24   v4l2_fourcc('B', 'G', 'R', '3')

struct rts_video_format {
	uint32_t pixelformat;
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
};

struct rts_video_fps {
	uint32_t numerator;
	uint32_t denominator;
};

struct rts_video_selection {
	uint8_t mode;
	uint32_t left;
	uint32_t top;
	uint32_t width;
	uint32_t height;
};

struct rts_video_format_cap {
	uint32_t pixelformat;
	uint32_t width_min;
	uint32_t width_max;
	uint32_t height_min;
	uint32_t height_max;
	uint16_t width_step;
	uint16_t height_step;
	struct rts_video_fps fps_max;
	struct rts_video_fps fps_min;
	struct rts_video_fps fps_step;
};

struct rts_video_caps {
	const struct rts_video_format_cap *format_caps;
	uint8_t min_vbuf_count;
};

enum {
	RTS_MEMORY_KERNEL = 1,
	RTS_MEMORY_USERPTR,
	RTS_MEMORY_RESERVED,
};

struct rts_video_buffer {
	void *driver_data;
	uint8_t *vm_addr;
	uint32_t phy_addr;
	uint32_t size;
	uint32_t bytesused;
	uint64_t timestamp;
	uint32_t memory;
	int index;
};

enum rts_video_signal_result {
	RTS_VIDEO_BUF_DONE = 1,
	RTS_VIDEO_BUF_ABORTED,
	RTS_VIDEO_BUF_ERROR,
};

#endif /* _RTS_VIDEO_H_ */
