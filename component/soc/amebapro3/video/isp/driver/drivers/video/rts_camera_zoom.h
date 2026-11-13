/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS:
 * - k_mutex            -> isp_rtos_mutex_t
 * - reset_dt_spec      -> rtscam_rtos_reset_t (from reset.h)
 * - clock_control_*    -> replaced with clock driver abstraction
 * - mem_addr_t         -> uint32_t
 */

#ifndef _RTS_CAMERA_ZOOM_H_
#define _RTS_CAMERA_ZOOM_H_

#include <stdint.h>
#include <stdbool.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_completion.h>
#include <reset.h>
#include <rts_camera_isp.h>
#include "rtscam_isp_pipeline.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define ZOOM_WORKER_STACK_DEPTH 1024

/* work types for the zoom worker task */
#define ZOOM_WORK_DOWN    0  /* enable/update sub-stream zoom-down */
#define ZOOM_WORK_DISABLE 1  /* wait for HW trigger drain before ISP stop */

#define RTSCAM_MAX_STM_COUNT 5
#define RTS_ZOOM_MAX_STREAM  RTSCAM_MAX_STM_COUNT

struct rtscam_soc_ive_ctrl {
	int enable;
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} normal_mean;
	uint16_t normal_scale;
	int8_t quant_len;
	uint32_t asym_inv_scale;
	uint8_t asym_zero_point;
};

struct rtscam_subdev_crop_info {
	int mode;
	uint32_t start_x;
	uint32_t start_y;
	uint32_t width;
	uint32_t height;
};

struct rtscam_zoom_stream_fmt {
	uint32_t fmt;
	uint16_t width;
	uint16_t height;
};

/* Per sub-stream zoom-down parameters (shared to zoom_dev for worker task) */
struct rtscam_zoom_down_data {
	int      enable;        /* ENABLE_ZOOM / DISABLE_ZOOM */
	uint32_t x, y;
	uint16_t dest_w, dest_h;
	uint16_t src_w,  src_h;
};

struct rtscam_zoom {
	int stream_id;
	uint32_t base;        /* absolute register base (was mem_addr_t) */
	bool zoom_in_enable;

	struct rtscam_zoom_stream_fmt dt_stream_fmt;
	struct rtscam_zoom_stream_fmt stream_fmt;

	/* current_fps removed: FPS is global, managed by rts_camera_fps.c */
	struct rtscam_zoom_stream_fmt current_fmt;
	struct rtscam_subdev_crop_info crop_info;
	int (*set_clock)(struct rtscam_zoom *zoom, int enable);
};

struct rtscam_zoom_dev {
	uint32_t stream_num;
	isp_rtos_mutex_t lock;   /* replaces k_mutex */

	uint32_t base;           /* zoom sys register base */
	rtscam_rtos_reset_t reset; /* replaces reset_dt_spec */
	int clk_refcnt;

	struct rtscam_zoom zoom[RTS_ZOOM_MAX_STREAM];

	/* async zoom-down mechanism (ported from Linux kthread_worker) */
	unsigned long              streaming;   /* bitmask of active streams */
	uint8_t                    pending_ids; /* bitmask of streams awaiting worker */
	isp_rtos_mutex_t           down_lock;   /* guards pending_ids + data[] */
	struct rtscam_zoom_down_data data[RTS_ZOOM_MAX_STREAM - 1];
	isp_rtos_completion_t      trig_completion[RTS_ZOOM_MAX_STREAM - 1];
	isp_rtos_completion_t      disable_completion;
	uint8_t                    worker_op;
	TaskHandle_t               zoom_worker_handle;
	StaticTask_t               zoom_worker_task_buf;
	StackType_t                zoom_worker_stack[ZOOM_WORKER_STACK_DEPTH];
};

struct rtscam_zoom_stream_info {
	uint32_t stream_num;
	struct rtscam_zoom_stream_fmt fmt[RTS_ZOOM_MAX_STREAM];
};

/* API for soc */
int rtscam_zoom_init(struct rtscam_zoom_dev *zoom_dev);
int rtscam_zoom_release(struct rtscam_zoom_dev *zoom_dev);
int rtscam_zoom_soc_set_stream(struct rtscam_zoom *zoom, int enable);
int rtscam_zoom_soc_set_fmt(struct rtscam_zoom *zoom, uint32_t rts_code,
							uint32_t w, uint32_t h);
int rtscam_zoom_soc_set_ive_ctrl(struct rtscam_zoom *zoom,
								 struct rtscam_soc_ive_ctrl *ctrl);
int rtscam_zoom_soc_get_ive_ctrl(struct rtscam_zoom *zoom,
								 struct rtscam_soc_ive_ctrl *ctrl);
int rtscam_zoom_soc_set_crop(struct rtscam_zoom *zoom,
							 struct rtscam_subdev_crop_info *crop);
int rtscam_zoom_soc_get_crop(struct rtscam_zoom *zoom,
							 struct rtscam_subdev_crop_info *crop);
int rtscam_zoom_resume(struct rtscam_zoom_dev *zoom_dev);

/* For soc event STREAM_INFO_CHANGE or get initial stream info */
int rtscam_zoom_soc_get_stream_info(struct rtscam_zoom_dev *zoom_dev,
									struct rtscam_zoom_stream_info *stream_info);

/* API for color range */
int rtscam_zoom_soc_set_limit_color_range(struct rtscam_zoom *zoom,
		int *limit_enable);
int rtscam_zoom_soc_get_limit_color_range(struct rtscam_zoom *zoom,
		int *limit_enable);

/* API for isp */
int rtscam_zoom_set_color_range(struct rtscam_zoom_dev *zoom_dev, int range);

#endif /* _RTS_CAMERA_ZOOM_H_ */
