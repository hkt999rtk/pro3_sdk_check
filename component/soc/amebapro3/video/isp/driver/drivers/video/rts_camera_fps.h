/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr: removed Zephyr includes, kept pure-C content.
 */

#ifndef _RTS_CAMERA_FPS_H
#define _RTS_CAMERA_FPS_H

#include "rts_camera_config.h"
#include "rts_video.h"
#include <stddef.h>

#define RTSCAM_SOC_MAX_FPS			300
#define RTSCAM_SOC_STEP_FPS			1
#define RTSCAM_SOC_FPS_CVRT_NUMERATOR		1000

struct rtscam_soc_skip_info {
	int m;
	int n;
	int flag;
	int count;
	int index;
};

struct rtscam_soc_fps_descriptor {
	struct rts_video_fps max;
	struct rts_video_fps min;
	struct rts_video_fps step;
};

/* Forward declarations */
struct rtscam_video_stream;
struct rtscam_frame_size;

struct rtscam_sensor_fps {
	struct rts_video_fps sensor_fps_setting;
	struct rts_video_fps sensor_fps_actual;

	struct rtscam_soc_fps_descriptor desc;

	uint8_t streamnum;
	int *streaming_count;
	struct rtscam_video_stream *streams;

	int flag_max;

	int (*set_fps)(struct rtscam_video_stream *stream,
				   struct rts_video_fps fps);
	int (*set_stream)(struct rtscam_video_stream *stream, int enable);
};

struct rtscam_video_fps {
	struct rts_video_fps user_setting;
	struct rts_video_fps user_actual;

	struct rtscam_soc_skip_info skip_info;

	struct rtscam_sensor_fps *sensor_fps;
};

void rtscam_set_user_fps(struct rtscam_video_fps *fps,
						 uint32_t user_numerator, uint32_t user_denominator);
void rtscam_set_user_fps_actual(struct rtscam_video_fps *fps);
int rtscam_update_sensor_fps(struct rtscam_video_stream *stream,
							 struct rts_video_fps fps, int flag);
void rtscam_adjust_sensor_fps(struct rtscam_video_stream *stream, int enable);
void rtscam_adjust_sensor_fps_dynamic(struct rtscam_video_stream *stream,
									  uint32_t user_numerator, uint32_t user_denominator);
void rtscam_exec_sensor_fps_setting(struct rtscam_video_stream *stream,
									int enable);

int rtscam_change_dynamic_fps(struct rtscam_sensor_fps *sensor_fps,
							  struct rts_video_fps fps);
int rtscam_skip_frame(struct rtscam_video_stream *stream);

int rtscam_init_sensor_fps(struct rtscam_sensor_fps *sensor_fps, int flag_max);
int rtscam_release_sensor_fps(struct rtscam_sensor_fps *sensor_fps);

int rtscam_register_frmival(struct rtscam_video_stream *stream,
							uint32_t fourcc, struct rtscam_frame_size *size);

#endif /* _RTS_CAMERA_FPS_H */
