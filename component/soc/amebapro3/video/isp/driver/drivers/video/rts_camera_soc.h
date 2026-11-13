/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS:
 * - reset_dt_spec   -> rtscam_rtos_reset_t
 * - const struct device * removed (global singleton soc_data used instead)
 */

#ifndef _RTS_CAMERA_SOC_H
#define _RTS_CAMERA_SOC_H

#include <reset.h>
#include "rts_camera.h"
#include "rts_camera_fps.h"
#include "rts_camera_zoom.h"
#include <rts_camera_isp.h>
#include "rtscam_isp_pipeline.h"

#define RTSCAM_SOC_HW_SLOT_NUM 2

/* Reserved streamid for the ISP verify/input (TX) stream. Outside the
 * zoom-derived capture range (0..streamnum-1) and the RTSCAM_RGB_YONLY_STRM_IDX
 * alias (10), so it is never counted by __get_valid_streamnum(). The input
 * stream is a separate object (see drivers/isp/rtscam_isp_input.c), not a
 * slot in rvdev.streams[]. */
#define RTSCAM_INPUT_STRM_IDX 20

struct rtscam_soc_icfg {
	int found;
	struct rtscam_region y;
	struct rtscam_region uv;
};

struct rtscam_soc_rgbcfg {
	struct rtscam_region rgb[3];
};

struct rtscam_soc_slot_info {
	uint8_t slot_index;
	uint8_t slot_num;
	struct rtscam_video_buffer *slots[RTSCAM_SOC_HW_SLOT_NUM];
};

struct rtscam_soc_dev {
	uint32_t base;
	uint32_t size;

	rtscam_rtos_reset_t video_reset;   /* replaces reset_dt_spec */
	rtscam_rtos_reset_t video_sysmem;  /* replaces reset_dt_spec */

	int initialized;

	struct rtscam_sensor_fps sensor_fps;
	struct rtscam_zoom_dev zoom_dev;
	struct rtscam_isp_pipeline *isp; /* points to global ISP pipeline (opaque type) */
	struct rtscam_video_device rvdev;
	struct rtscam_zoom_stream_info stream_info;

	struct rtscam_soc_slot_info slot_info[RTSCAM_MAX_STM_COUNT];

	struct rtscam_soc_icfg icfgs[RTSCAM_YUV_MAX_STRM_NUM];
	unsigned int icfg_count;
	struct rtscam_soc_rgbcfg rgbcfg;

	unsigned long drop_frames;
	unsigned long drops[RTSCAM_MAX_STM_COUNT];

	int keep_user_setting;

	uint32_t s0_width;
	uint32_t s0_height;

	int width_step;
	int height_step;
	uint8_t height_align;
	struct {
		uint32_t yuv_axibuf_base;
		uint32_t yuv_base_val_bit;
		uint32_t yuv_size_val_bit;
		uint32_t rgb_axibuf_base;
		uint32_t rgb_axibuf_interval;
		uint32_t rgb_frame_len_r;
		uint32_t rgb_frame_len_g;
		uint32_t rgb_frame_len_b;
		uint32_t y_frame_len_base;
		uint32_t uv_frame_len_base;
		uint32_t yuv_frame_len_interval;
		uint32_t yuv_frame_buf_cnt;
		uint32_t rgb_frame_buf_cnt;
		uint32_t yuv_interleave_select;
		uint32_t isp_nv12_select;
		uint32_t ringbuf_mode;
		uint32_t ringbuf_rowend_delay;
		uint32_t buf_status_base;
	} reg;
};

/* Initialise the soc device (replaces Zephyr rtscam_soc_init() auto-called
 * by device framework). Normally reached via rtscam_video_init(). */
int rtscam_soc_init(void);

/* Event handler: called on ISP FPS-changed or zoom stream-info changed events */
int rtscam_soc_event(int id, void *arg);

/* Returns the zoom_dev from the global singleton — used by the isp layer to
 * update stream dimensions without CONTAINER_OF (isp is a pointer, not embedded). */
struct rtscam_zoom_dev *rtscam_soc_get_zoom_dev(void);

/* PM callbacks (called from rtscam_isp_resume / rtscam_isp_suspend) */
int rtscam_soc_resume(void);
int rtscam_soc_suspend(void);

#endif /* _RTS_CAMERA_SOC_H */
