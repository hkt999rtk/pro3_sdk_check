/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS:
 * - k_calloc/k_free -> pvPortMalloc+memset / vPortFree
 * - LOG_MODULE_REGISTER -> removed
 */

#include <string.h>
#include <errno.h>
#include <freertos/FreeRTOS.h>
#include "rts_camera.h"
#include "rts_camera_fps.h"

/*
 * V4L2_PIX_FMT_128BIT_3PIXEL packs 3 pixels into 128 bits, not 128 bits
 * per pixel — the generic height*((width*bpp)>>3) formula below would
 * overshoot by 3x for it. Matches the Linux rts_camera driver's
 * rtscam_queue_setup() special case (this port has no separate vb2
 * queue_setup step; stream->sizeimage plays that role directly).
 */
static uint32_t __rtscam_calc_sizeimage(uint32_t width, uint32_t height,
										uint32_t fourcc, uint8_t bpp)
{
	if (fourcc == V4L2_PIX_FMT_128BIT_3PIXEL) {
		return (width * height + 2) / 3 * (bpp >> 3);
	}

	return height * ((width * bpp) >> 3);
}

struct rtscam_video_format *find_format_by_fourcc(
	struct rtscam_video_stream *stream, uint32_t fourcc)
{
	struct rtscam_video_format *format = stream->user_formats;

	while (format) {
		if (fourcc == format->fourcc) {
			return format;
		}
		format = format->next;
	}

	return NULL;
}

int rtscam_register_format(struct rtscam_video_stream *stream,
						   struct rtscam_video_format_xlate *pfmt)
{
	struct rtscam_video_format *format = NULL;
	struct rtscam_video_format *p = NULL;

	if (find_format_by_fourcc(stream, pfmt->fourcc)) {
		rtsprintk(RTS_TRACE_ERROR, "%c%c%c%c has been registered",
				  v4l2pixfmtstr(pfmt->fourcc));
		return -EEXIST;
	}

	format = pvPortMalloc(sizeof(*format));
	if (!format) {
		return -ENOMEM;
	}
	memset(format, 0, sizeof(*format));

	format->index = pfmt->index;
	format->fourcc = pfmt->fourcc;
	format->bpp = pfmt->bpp;
	format->is_yuv = pfmt->is_yuv;
	format->rts_code = pfmt->rts_code;
	memcpy((void *)format->name, (void *)pfmt->name, sizeof(format->name));
	format->next = NULL;

	if (!stream->user_format) {
		stream->rts_code = pfmt->rts_code;
		stream->user_format = pfmt->fourcc;
	}

	if (!stream->user_formats) {
		stream->user_formats = format;
		return 0;
	}
	p = stream->user_formats;
	while (p->next) {
		p = p->next;
	}
	p->next = format;

	return 0;
}

int rtscam_register_frame_stepwise(struct rtscam_video_stream *stream,
								   uint32_t fourcc,
								   struct rtscam_frame_size *max,
								   struct rtscam_frame_size *min,
								   struct rtscam_frame_size *step)
{
	struct rtscam_video_format *format;

	format = find_format_by_fourcc(stream, fourcc);
	if (!format) {
		rtsprintk(RTS_TRACE_ERROR,
				  "please register format(%c%c%c%c) first",
				  v4l2pixfmtstr(fourcc));
		return -EINVAL;
	}

	if (stream->user_format == fourcc &&
		(stream->user_width != max->width ||
		 stream->user_height != max->height)) {
		stream->user_width = max->width;
		stream->user_height = max->height;
		stream->bytesperline = (stream->user_width * format->bpp) >> 3;
		stream->sizeimage = __rtscam_calc_sizeimage(stream->user_width,
							stream->user_height, fourcc, format->bpp);
	}

	format->max.width = max->width;
	format->max.height = max->height;
	format->min.width = min->width;
	format->min.height = min->height;
	format->step.width = step->width;
	format->step.height = step->height;
	format->initialized = 1;

	return 0;
}

int rtscam_clr_format(struct rtscam_video_stream *stream, int keep)
{
	struct rtscam_video_format *cur = stream->user_formats;
	struct rtscam_video_format *next = NULL;

	while (cur) {
		next = cur->next;

		if (cur->initialized) {
			rtscam_clr_frmival(&cur->frmival);
			cur->initialized = 0;
		}
		vPortFree(cur);
		cur = next;
	}
	stream->user_formats = NULL;

	if (!keep) {
		stream->rts_code = 0;
		stream->user_format = 0;
		stream->user_width = 0;
		stream->user_height = 0;

		rtscam_set_user_fps(&stream->fps, 1, 1);
	}

	return 0;
}

static int rtscam_check_frmival(struct rtscam_video_frmival *frmival)
{
	if (!frmival) {
		return -EINVAL;
	}

	if (!frmival->initialized) {
		return -EINVAL;
	}

	if (frmival->max.numerator == 0 || frmival->max.denominator == 0 ||
		frmival->min.numerator == 0 || frmival->min.denominator == 0 ||
		frmival->step.numerator == 0 || frmival->step.denominator == 0) {
		return -EINVAL;
	}

	return 0;
}

int rtscam_check_stream_format(struct rtscam_video_stream *stream)
{
	struct rtscam_video_format *format = NULL;
	int ret;

	if (!stream) {
		return -EINVAL;
	}

	if (!stream->user_formats) {
		return -EINVAL;
	}

	format = stream->user_formats;

	while (format) {
		if (format->fourcc == 0 || !format->initialized) {
			return -EINVAL;
		}

		if (!format->max.width || !format->max.height ||
			!format->min.width || !format->min.height ||
			!format->step.width || !format->step.height) {
			return -EINVAL;
		}

		ret = rtscam_check_frmival(&format->frmival);
		if (ret) {
			return ret;
		}

		format = format->next;
	}

	return 0;
}

static int rtscam_check_user_frmival(struct rtscam_video_frmival *frmival,
									 uint32_t user_numerator,
									 uint32_t user_denominator)
{
	int valid = 1;

	if (user_denominator * frmival->max.numerator >
		user_numerator * frmival->max.denominator) {
		valid = 0;
	}
	if ((user_denominator * frmival->step.numerator) %
		(user_numerator * frmival->step.denominator)) {
		valid = 0;
	}
	if (!valid) {
		return -EINVAL;
	}

	return 0;
}

int rtscam_check_user_format(struct rtscam_video_stream *stream,
							 uint32_t user_format, uint32_t user_width,
							 uint32_t user_height, uint32_t user_numerator,
							 uint32_t user_denominator)
{
	struct rtscam_video_format *format = NULL;
	struct rtscam_video_frmival *frmival = NULL;
	int ret;

	if (!user_format || !user_width || !user_height ||
		!user_numerator || !user_denominator) {
		return -EINVAL;
	}

	format = find_format_by_fourcc(stream, user_format);
	if (!format) {
		rtsprintk(RTS_TRACE_ERROR, "invalid user format(%c%c%c%c)",
				  v4l2pixfmtstr(user_format));
		return -EINVAL;
	}

	frmival = rtscam_get_video_frmival(stream, user_format,
									   user_width, user_height);
	if (!frmival) {
		rtsprintk(RTS_TRACE_ERROR, "invalid user format");
		return -EINVAL;
	}

	ret = rtscam_check_user_frmival(frmival, user_numerator, user_denominator);
	if (ret) {
		rtsprintk(RTS_TRACE_ERROR, "invalid user fps");
		return ret;
	}

	return 0;
}

int rtscam_try_user_format(struct rtscam_video_stream *stream,
						   uint32_t user_format, uint32_t user_width,
						   uint32_t user_height)
{
	struct rtscam_video_frmival *frmival = NULL;

	if (!user_format || !user_width || !user_height) {
		return -EINVAL;
	}

	frmival = rtscam_get_video_frmival(stream, user_format,
									   user_width, user_height);
	if (!frmival) {
		rtsprintk(RTS_TRACE_ERROR, "invalid user format");
		return -EINVAL;
	}

	return 0;
}

int rtscam_set_user_format(struct rtscam_video_stream *stream,
						   uint32_t user_format, uint32_t user_width,
						   uint32_t user_height)
{
	struct rtscam_video_format *format = NULL;
	struct rtscam_video_frmival *frmival = NULL;
	int ret = 0;

	if (!user_format || !user_width || !user_height) {
		return -EINVAL;
	}

	format = find_format_by_fourcc(stream, user_format);
	if (!format) {
		rtsprintk(RTS_TRACE_ERROR, "invalid user format(%c%c%c%c)",
				  v4l2pixfmtstr(user_format));
		return -EINVAL;
	}

	frmival = rtscam_get_video_frmival(stream, user_format,
									   user_width, user_height);
	if (!frmival) {
		rtsprintk(RTS_TRACE_ERROR, "invalid user format");
		return -EINVAL;
	}

	ret = rtscam_check_user_frmival(frmival,
									stream->fps.user_setting.numerator,
									stream->fps.user_setting.denominator);
	if (!ret) {
		goto exit;
	}

	rtscam_set_user_fps(&stream->fps,
						frmival->max.numerator, frmival->max.denominator);

exit:
	stream->rts_code = format->rts_code;
	stream->user_format = user_format;
	stream->user_width = user_width;
	stream->user_height = user_height;
	stream->bytesperline = (stream->user_width * format->bpp) >> 3;
	stream->sizeimage = __rtscam_calc_sizeimage(stream->user_width,
						stream->user_height, user_format, format->bpp);

	return 0;
}

int rtscam_set_user_frmival(struct rtscam_video_stream *stream,
							uint32_t user_numerator, uint32_t user_denominator)
{
	int ret;

	ret = rtscam_check_user_format(stream, stream->user_format,
								   stream->user_width, stream->user_height,
								   user_numerator, user_denominator);
	if (ret) {
		return ret;
	}

	rtscam_set_user_fps(&stream->fps, user_numerator, user_denominator);
	return 0;
}

int rtscam_get_max_frame_size(struct rtscam_video_format *format,
							  struct rtscam_frame_size *max)
{
	if (!format || !max) {
		return -EINVAL;
	}

	if (!format->initialized) {
		return -EINVAL;
	}

	max->width = format->max.width;
	max->height = format->max.height;

	return 0;
}
