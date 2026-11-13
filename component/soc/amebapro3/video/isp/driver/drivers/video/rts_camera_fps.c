/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr: removed LOG_MODULE_REGISTER.
 */

#include <string.h>
#include "rts_camera_fps.h"
#include "rts_camera.h"

static void __rtscam_get_sensor_maxfps(struct rtscam_video_fps *fps,
									   struct rts_video_fps *n);
static int __rtscam_check_sensor_fps(struct rtscam_video_fps *fps,
									 struct rts_video_fps *n);
static void __rtscam_update_sensor_fps_dynamic(
	struct rtscam_video_stream *stream,
	struct rts_video_fps fps);

static int __calc_gcd(int a, int b)
{
	int tmp;

	while (b) {
		tmp = a % b;
		a = b;
		b = tmp;
	}
	return a;
}

static int __calc_coprime(int *a, int *b, int snum, int unum)
{
	int m = *a;
	int n = *b;
	int tmp;

	if (m < 0 || n < 0) {
		return 0;
	}
	if (m == 0) {
		return n;
	}
	if (n == 0) {
		return m;
	}

	tmp = snum * unum / __calc_gcd(snum, unum);

	m = m * (tmp / snum);
	n = n * (tmp / unum);

	tmp = __calc_gcd(m, n);

	*a = m / tmp;
	*b = n / tmp;

	return m;
}

static void __normalize_fract(uint32_t *nume, uint32_t *deno)
{
	int gcd = __calc_gcd(*deno, *nume);

	*deno /= gcd;
	*nume /= gcd;
}

static void __rtscam_init_skip_info(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_skip_info *skip = NULL;

	if (stream->streamid >= stream->fps.sensor_fps->streamnum) {
		return;
	}

	skip = &stream->fps.skip_info;

	skip->m = stream->fps.sensor_fps->sensor_fps_actual.denominator;
	skip->n = stream->fps.user_actual.denominator;

	__calc_coprime(&skip->m, &skip->n,
				   stream->fps.sensor_fps->sensor_fps_actual.numerator,
				   stream->fps.user_actual.numerator);

	if (skip->m > 2 * skip->n) {
		skip->flag = 1;
	} else {
		skip->flag = 0;
		skip->n = skip->m - skip->n;
	}
	skip->count = 0;
	skip->index = 0;
}

int rtscam_skip_frame(struct rtscam_video_stream *stream)
{
	struct rtscam_soc_skip_info *skip_info = NULL;
	int skip = 0;

	if (stream->streamid >= stream->fps.sensor_fps->streamnum) {
		return 0;
	}

	skip_info = &stream->fps.skip_info;

	rtsprintk(RTS_TRACE_VIDEO, "%d %d %d %d %d",
			  skip_info->m, skip_info->n, skip_info->flag,
			  skip_info->count, skip_info->index);

	if (skip_info->n == 0) {
		return skip_info->flag;
	}

	if ((skip_info->n - skip_info->count) * skip_info->m >=
		(skip_info->m - skip_info->index) * skip_info->n) {
		skip = 1 - skip_info->flag;
		skip_info->count++;
	} else {
		skip = skip_info->flag;
	}

	skip_info->index++;
	if (skip_info->index % skip_info->m == 0) {
		skip_info->count = 0;
		skip_info->index = 0;
	}

	return skip;
}

static void __update_user_fps_actual(struct rtscam_video_fps *fps,
									 struct rts_video_fps user_fps)
{
	if (!fps) {
		return;
	}

	fps->user_actual.numerator = user_fps.numerator;
	fps->user_actual.denominator = user_fps.denominator;
}

void rtscam_set_user_fps_actual(struct rtscam_video_fps *fps)
{
	if (!fps) {
		return;
	}

	__update_user_fps_actual(fps, fps->sensor_fps->sensor_fps_actual);
}

void rtscam_adjust_sensor_fps_dynamic(struct rtscam_video_stream *stream,
									  uint32_t user_numerator, uint32_t user_denominator)
{
	struct rtscam_video_fps *fps = &stream->fps;
	struct rts_video_fps fps_max;
	struct rts_video_fps fps_set;

	if (!rtscam_is_streaming(stream)) {
		return;
	}

	if (fps->sensor_fps->flag_max) {
		return;
	}

	__rtscam_get_sensor_maxfps(fps, &fps_max);

	if (user_denominator * stream->fps.user_setting.numerator
		== stream->fps.user_setting.denominator * user_numerator) {
		return;
	}

	if ((user_denominator * stream->fps.user_setting.numerator >
		 stream->fps.user_setting.denominator * user_numerator)
		&& (user_denominator * fps_max.numerator <=
			fps_max.denominator * user_numerator)) {
		return;
	}

	if ((user_denominator * stream->fps.user_setting.numerator <
		 stream->fps.user_setting.denominator * user_numerator)
		&& (stream->fps.user_setting.denominator *
			fps_max.numerator <= fps_max.denominator *
			stream->fps.user_setting.numerator)) {
		return;
	}

	if (user_denominator * fps_max.numerator >
		fps_max.denominator * user_numerator) {
		fps_set.denominator = user_denominator;
		fps_set.numerator = user_numerator;
	} else {
		fps_set.denominator = fps_max.denominator;
		fps_set.numerator = fps_max.numerator;
	}

	if (__rtscam_check_sensor_fps(fps, &fps_set) != 0) {
		__rtscam_update_sensor_fps_dynamic(stream, fps_set);
	}
}

void rtscam_set_user_fps(struct rtscam_video_fps *fps,
						 uint32_t user_numerator, uint32_t user_denominator)
{
	struct rtscam_video_stream *stream;

	if (!fps) {
		return;
	}

	stream = container_of(fps, struct rtscam_video_stream, fps);

	__normalize_fract(&user_numerator, &user_denominator);
	rtscam_adjust_sensor_fps_dynamic(stream, user_numerator, user_denominator);

	fps->user_setting.numerator = user_numerator;
	fps->user_setting.denominator = user_denominator;

	if (fps->sensor_fps->sensor_fps_actual.numerator * user_denominator <=
		fps->sensor_fps->sensor_fps_actual.denominator * user_numerator
		&& stream->vin_mode != RTS_VIN_MODE_RING_MEM) {
		__update_user_fps_actual(fps, fps->user_setting);
	} else {
		__update_user_fps_actual(fps, fps->sensor_fps->sensor_fps_actual);
	}

	__rtscam_init_skip_info(stream);
}

static void __set_sensor_fps(struct rtscam_sensor_fps *sensor_fps,
							 struct rts_video_fps fps)
{
	if (!sensor_fps) {
		return;
	}

	sensor_fps->sensor_fps_setting.numerator = fps.numerator;
	sensor_fps->sensor_fps_setting.denominator = fps.denominator;
}

static void __update_sensor_fps_actual(struct rtscam_sensor_fps *sensor_fps,
									   struct rts_video_fps fps)
{
	int i;
	struct rts_video_fps fps_user_setting;
	struct rts_video_fps fps_user_actual;
	struct rtscam_video_stream *stream;
	struct rts_video_fps *fract;

	if (!sensor_fps) {
		return;
	}

	if (sensor_fps->sensor_fps_actual.numerator == fps.numerator
		&& sensor_fps->sensor_fps_actual.denominator == fps.denominator) {
		return;
	}

	sensor_fps->sensor_fps_actual.numerator = fps.numerator;
	sensor_fps->sensor_fps_actual.denominator = fps.denominator;

	for (i = 0; i < sensor_fps->streamnum; i++) {
		stream = sensor_fps->streams + i;

		fract = &stream->fps.user_setting;
		fps_user_setting.denominator = fract->denominator;
		fps_user_setting.numerator = fract->numerator;
		fract = &stream->fps.user_actual;
		fps_user_actual.denominator = fract->denominator;
		fps_user_actual.numerator = fract->numerator;

		if (fps_user_setting.denominator *
			sensor_fps->sensor_fps_actual.numerator >
			sensor_fps->sensor_fps_actual.denominator *
			fps_user_setting.numerator
			|| stream->vin_mode == RTS_VIN_MODE_RING_MEM) {
			__update_user_fps_actual(&stream->fps,
									 sensor_fps->sensor_fps_actual);
		} else if (fps_user_setting.denominator *
				   fps_user_actual.numerator !=
				   fps_user_setting.numerator * fps_user_actual.denominator)
			__update_user_fps_actual(&stream->fps,
									 stream->fps.user_setting);

		__rtscam_init_skip_info(stream);
	}
}

static void __rtscam_get_sensor_maxfps(struct rtscam_video_fps *fps,
									   struct rts_video_fps *n)
{
	int i;
	struct rts_video_fps n_max = {1, 0};
	struct rtscam_video_stream *stream;
	struct rtscam_video_stream *stream_tmp;

	stream = container_of(fps, struct rtscam_video_stream, fps);
	for (i = 0; i < fps->sensor_fps->streamnum; i++) {
		stream_tmp = fps->sensor_fps->streams + i;
		if (stream_tmp->streamid == stream->streamid) {
			continue;
		}

		if (rtscam_is_streaming(stream_tmp)) {
			if (stream_tmp->fps.user_setting.denominator *
				n_max.numerator > n_max.denominator *
				stream_tmp->fps.user_setting.numerator) {
				n_max.numerator =
					stream_tmp->fps.user_setting.numerator;
				n_max.denominator =
					stream_tmp->fps.user_setting.denominator;
			}
		}
	}
	n->numerator = n_max.numerator;
	n->denominator = n_max.denominator;
}

static int __check_fps_valid(struct rts_video_fps *fps,
							 struct rts_video_fps step,
							 struct rts_video_fps min,
							 struct rts_video_fps max)
{
	int m = fps->numerator;
	int n = step.numerator;
	int deno = fps->denominator;
	int tmp;
	int min_com_multi;

	if (m <= 0 || n <= 0) {
		return 0;
	}

	if (deno < 0) {
		return 0;
	}

	if (!deno || !step.denominator) {
		goto exit;
	}

	min_com_multi = m * n / __calc_gcd(m, n);

	m = deno * (min_com_multi / m);
	n = step.denominator * (min_com_multi / n);

	if (m / n * n == m) {
		goto exit;
	}

	deno = m / n * n + n;
	tmp = __calc_gcd(deno, min_com_multi);

	fps->denominator = deno / tmp;
	fps->numerator = min_com_multi / tmp;
exit:
	if (fps->denominator * min.numerator < fps->numerator * min.denominator) {
		memcpy(fps, &min, sizeof(struct rts_video_fps));
	} else if (fps->denominator * max.numerator > fps->numerator * max.denominator) {
		memcpy(fps, &max, sizeof(struct rts_video_fps));
	}

	return 1;
}

static int __rtscam_check_sensor_fps(struct rtscam_video_fps *fps,
									 struct rts_video_fps *n)
{
	int flag = 0;
	struct rtscam_sensor_fps *sensor_fps;

	sensor_fps = fps->sensor_fps;

	flag = __check_fps_valid(n, sensor_fps->desc.step,
							 sensor_fps->desc.min, sensor_fps->desc.max);

	return flag;
}

static int __rtscam_update_sensor_fps(struct rtscam_video_fps *fps,
									  struct rts_video_fps val, int flag)
{
	int i;
	int ret;
	struct rtscam_video_stream *stream_tmp;
	struct rtscam_video_stream *stream =
		container_of(fps, struct rtscam_video_stream, fps);
	struct rtscam_sensor_fps *sensor_fps = fps->sensor_fps;

	if (val.denominator > RTSCAM_SOC_MAX_FPS * val.numerator) {
		rtsprintk(RTS_TRACE_ERROR, "%s:fps is too large", __func__);
		return -EINVAL;
	}

	__normalize_fract(&val.numerator, &val.denominator);
	for (i = 0; i < sensor_fps->streamnum; i++) {
		stream_tmp = sensor_fps->streams + i;

		if (stream_tmp->streamid == stream->streamid && flag) {
			continue;
		}

		if (!rtscam_is_streaming(stream_tmp)) {
			continue;
		}

		sensor_fps->set_stream(stream_tmp, 0);
	}
	ret = sensor_fps->set_fps(stream, val);
	if (ret == 0) {
		__set_sensor_fps(sensor_fps, val);
		__update_sensor_fps_actual(sensor_fps, val);
	} else {
		rtsprintk(RTS_TRACE_ERROR, "%s:set fps fail", __func__);
	}

	for (i = 0; i < sensor_fps->streamnum; i++) {
		stream_tmp = sensor_fps->streams + i;

		if (stream_tmp->streamid == stream->streamid && flag) {
			continue;
		}

		if (!rtscam_is_streaming(stream_tmp)) {
			continue;
		}

		sensor_fps->set_stream(stream_tmp, 1);
	}

	return ret;
}

int rtscam_update_sensor_fps(struct rtscam_video_stream *stream,
							 struct rts_video_fps fps, int flag)
{
	struct rtscam_video_fps *stream_fps = &stream->fps;

	if (stream_fps->sensor_fps->flag_max) {
		return 0;
	}

	return __rtscam_update_sensor_fps(stream_fps, fps, flag);
}

static void __rtscam_update_sensor_fps_dynamic(
	struct rtscam_video_stream *stream,
	struct rts_video_fps fps)
{
	struct rtscam_video_fps *stream_fps = &stream->fps;
	struct rtscam_sensor_fps *sensor_fps = stream_fps->sensor_fps;
	int ret;

	if (fps.denominator > RTSCAM_SOC_MAX_FPS * fps.numerator) {
		rtsprintk(RTS_TRACE_ERROR, "%s:frmival is too large", __func__);
		return;
	}

	__normalize_fract(&fps.numerator, &fps.denominator);
	ret = sensor_fps->set_fps(stream, fps);
	if (ret == 0) {
		__set_sensor_fps(sensor_fps, fps);
		__update_sensor_fps_actual(sensor_fps, fps);
		rtsprintk(RTS_TRACE_FPS, "dynamic set fps as %d/%d ok",
				  fps.denominator, fps.numerator);
	} else {
		rtsprintk(RTS_TRACE_ERROR, "dynamic set sensor fps fail");
	}
}

void rtscam_adjust_sensor_fps(struct rtscam_video_stream *stream, int enable)
{
	struct rts_video_fps fps_max;
	struct rts_video_fps fps_cur;
	struct rts_video_fps fps_set;
	struct rtscam_video_fps *fps = &stream->fps;
	struct rtscam_sensor_fps *sensor_fps;
	int ret = 0;

	sensor_fps = fps->sensor_fps;
	if (stream->streamid >= sensor_fps->streamnum) {
		return;
	}

	fps_cur.denominator = fps->user_setting.denominator;
	fps_cur.numerator = fps->user_setting.numerator;
	__rtscam_get_sensor_maxfps(fps, &fps_max);

	if (fps_cur.denominator * fps_max.numerator >
		fps_cur.numerator * fps_max.denominator) {
		if (enable) {
			fps_set.denominator = fps_cur.denominator;
			fps_set.numerator = fps_cur.numerator;
		} else {
			fps_set.denominator = fps_max.denominator;
			fps_set.numerator = fps_max.numerator;
		}
	} else {
		if (sensor_fps->sensor_fps_setting.denominator !=
			sensor_fps->sensor_fps_actual.denominator ||
			sensor_fps->sensor_fps_setting.numerator !=
			sensor_fps->sensor_fps_actual.numerator) {
			fps_set.denominator =
				sensor_fps->sensor_fps_setting.denominator;
			fps_set.numerator =
				sensor_fps->sensor_fps_setting.numerator;
		}
		goto exit;
	}

	if (!enable && rtscam_is_streaming(stream) &&
		*fps->sensor_fps->streaming_count == 1) {
		goto exit;
	}

	ret = __rtscam_check_sensor_fps(fps, &fps_set);
exit:
	if (ret != 0) {
		__rtscam_update_sensor_fps_dynamic(stream, fps_set);
	}

	if (enable) {
		__rtscam_init_skip_info(stream);
	}
}

void rtscam_exec_sensor_fps_setting(struct rtscam_video_stream *stream,
									int enable)
{
	struct rtscam_sensor_fps *sensor_fps = stream->fps.sensor_fps;

	if (!stream->icd->streaming_count) {
		sensor_fps->set_fps(stream, sensor_fps->sensor_fps_setting);
		__update_sensor_fps_actual(sensor_fps,
								   sensor_fps->sensor_fps_setting);
	}

	if (enable) {
		__rtscam_init_skip_info(stream);
	}
}

int rtscam_change_dynamic_fps(struct rtscam_sensor_fps *sensor_fps,
							  struct rts_video_fps fps)
{
	if (!sensor_fps) {
		return -EINVAL;
	}

	if (fps.denominator <= 0 || fps.numerator <= 0) {
		rtsprintk(RTS_TRACE_ERROR, "invalid dynamic fps : %d/%d",
				  fps.denominator, fps.numerator);
		return -EINVAL;
	}

	__normalize_fract(&fps.numerator, &fps.denominator);
	__update_sensor_fps_actual(sensor_fps, fps);

	rtsprintk(RTS_TRACE_FPS, "dynamic fps change to: %d/%d",
			  sensor_fps->sensor_fps_actual.denominator,
			  sensor_fps->sensor_fps_actual.numerator);

	return 0;
}

struct rtscam_video_frmival *rtscam_get_video_frmival(
	struct rtscam_video_stream *stream, uint32_t fourcc,
	uint32_t width, uint32_t height)
{
	struct rtscam_video_format *format;
	struct rtscam_video_frmival *frmival = NULL;

	format = find_format_by_fourcc(stream, fourcc);

	if (format == NULL) {
		rtsprintk(RTS_TRACE_ERROR,
				  "please register format(%c%c%c%c) first",
				  v4l2pixfmtstr(fourcc));
		return NULL;
	}

	if (width < format->min.width || width > format->max.width) {
		rtsprintk(RTS_TRACE_ERROR,
				  "the range is not satisfied: width(%d), format->min.width(%d), format->max.width(%d)",
				  width, format->min.width, format->max.width);
		return NULL;
	}
	if ((width - format->min.width) % format->step.width != 0) {
		rtsprintk(RTS_TRACE_ERROR,
				  "the multiples is not satisfied: (width(%d) - format->min.width(%d)) mod (format->step.width(%d))",
				  width, format->min.width, format->step.width);
		return NULL;
	}
	if (height < format->min.height || height > format->max.height) {
		rtsprintk(RTS_TRACE_ERROR,
				  "the range is not satisfied: height(%d), format->min.height(%d), format->max.height(%d)",
				  height, format->min.height, format->max.height);
		return NULL;
	}
	if ((height - format->min.height) % format->step.height != 0) {
		rtsprintk(RTS_TRACE_ERROR,
				  "the multiples is not satisfied: (height(%d) - format->min.height(%d)) mod (format->step.height(%d))",
				  height, format->min.height, format->step.height);
		return NULL;
	}
	frmival = &format->frmival;

	return frmival;
}

int rtscam_register_frmival(struct rtscam_video_stream *stream,
							uint32_t fourcc, struct rtscam_frame_size *size)
{
	struct rtscam_video_frmival *frmival = NULL;
	struct rts_video_fps fps = stream->fps.sensor_fps->sensor_fps_setting;

	if (fps.denominator > fps.numerator * RTSCAM_SOC_MAX_FPS) {
		fps.denominator = RTSCAM_SOC_MAX_FPS;
		fps.numerator = 1;
	}

	frmival = rtscam_get_video_frmival(stream, fourcc,
									   size->width, size->height);
	if (frmival == NULL) {
		return -EINVAL;
	}

	if (frmival->initialized) {
		return -EINVAL;
	}

	if (stream->user_format == fourcc &&
		stream->user_width == size->width &&
		stream->user_height == size->height) {
		rtscam_set_user_fps(&stream->fps, fps.numerator, fps.denominator);
	}

	frmival->max = stream->fps.sensor_fps->desc.max;
	frmival->min = stream->fps.sensor_fps->desc.min;
	frmival->step = stream->fps.sensor_fps->desc.step;

	frmival->initialized = 1;

	return 0;
}

int rtscam_clr_frmival(struct rtscam_video_frmival *frmival)
{
	if (!frmival->initialized) {
		return 0;
	}

	memset(&frmival->max, 0, sizeof(frmival->max));
	memset(&frmival->min, 0, sizeof(frmival->min));
	memset(&frmival->step, 0, sizeof(frmival->step));

	frmival->initialized = 0;

	return 0;
}

int rtscam_release_sensor_fps(struct rtscam_sensor_fps *sensor_fps)
{
	return 0;
}

int rtscam_init_sensor_fps(struct rtscam_sensor_fps *sensor_fps, int flag_max)
{
	struct rts_video_fps fps = {1, 0};
	struct rtscam_soc_fps_descriptor *desc = &sensor_fps->desc;

	sensor_fps->flag_max = flag_max;
	fps.denominator = desc->max.denominator;
	fps.numerator = desc->max.numerator;

	/* Sensor frame info not yet received (async init) — use safe default 30fps.
	 * rtscam_soc_event(RTSCAM_ISP_EVT_FPS_CHANGED) will re-init with real values. */
	if (!fps.numerator || !fps.denominator) {
		fps.numerator = 1;
		fps.denominator = 30;
	}

	if (fps.denominator > RTSCAM_SOC_MAX_FPS * fps.numerator) {
		fps.denominator = RTSCAM_SOC_MAX_FPS;
		fps.numerator = 1;
	}
	__normalize_fract(&fps.numerator, &fps.denominator);
	__set_sensor_fps(sensor_fps, fps);

	return 0;
}
