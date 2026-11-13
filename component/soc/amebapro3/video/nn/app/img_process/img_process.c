/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "img_process.h"
#ifdef CONFIG_VMODULE_TS
#include "ts_verify.h"
#endif
#ifdef CONFIG_VMODULE_GE
#include "rts_zoom.h"
#endif

#define LIMIT(x, lower, upper) do{ if(x<lower) x=lower; else if(x>upper) x=upper; } while(0)

/*-------------------------------------------------------------------*/

#define QP	8

#define I2Q(v, q) (int)((v)*(1<<(q)))
#define Q2I(v, q) (int)((v)/(1<<(q)))

#define F2Q(v, q) (int)floor((v)*(1<<(q)))
#define Q2F(v, q) (float)( (float)(v)/(float)(1<<(q)))

#define MULQ(a, b, q)       ((a)*(b))/(1<<(q))
#define DIVQ(a, b, q)       ((a)*(1<<(q)))/(b)

#define IM(im,w,y,x) im[y*w+x]

/*-------------------------------------------------------------------*/

static int img_resize_planar_internal(img_t *im_in, rect_t *roi, img_t *im_out)
{
	int roi_w = roi->xmax - roi->xmin;
	int roi_h = roi->ymax - roi->ymin;

	int offx = roi->xmin;
	int offy = roi->ymin;

	if (im_out->width == 0) {
		im_out->width = roi_w;
	}
	if (im_out->height == 0) {
		im_out->height = roi_h;
	}

	// int scale_w = F2Q((float)im_out->width / (float)roi_w, QP);
	// int scale_h = F2Q((float)im_out->height / (float)roi_h, QP);
	int inv_scale_w = F2Q((float)roi_w / (float)im_out->width, QP);
	int inv_scale_h = F2Q((float)roi_h / (float)im_out->height, QP);

	if (im_out->data == NULL) {
		return -1;
	}

	uint8_t *in[3];
	uint8_t *out[3];

	in[0] = im_in->data;
	in[1] = im_in->data + im_in->width * im_in->height;
	in[2] = im_in->data + im_in->width * im_in->height * 2;

	out[0] = im_out->data;
	out[1] = im_out->data + im_out->width * im_out->height;
	out[2] = im_out->data + im_out->width * im_out->height * 2;

	for (int y = 0; y < (int)im_out->height; y++) {
		for (int x = 0; x < (int)im_out->width; x++) {
			// int in_x = DIVQ(I2Q(x, QP), scale_w, QP) + I2Q(offx, QP);
			// int in_y = DIVQ(I2Q(y, QP), scale_h, QP) + I2Q(offy, QP);
			int in_x = MULQ(I2Q(x, QP), inv_scale_w, QP) + I2Q(offx, QP);
			int in_y = MULQ(I2Q(y, QP), inv_scale_h, QP) + I2Q(offy, QP);

			int top = Q2I(in_y, QP);
			int left = Q2I(in_x, QP);

			for (int p = 0; p < 3; p++) {
				IM(out[p], im_out->width, y, x) = IM(in[p], im_in->width, top, left);
			}
		}
	}

	return 0;
}

int img_resize_planar(img_t *im_in, img_t *im_out)
{
	rect_t roi = {
		.xmin = 0,
		.ymin = 0,
		.xmax = im_in->width,
		.ymax = im_in->height,
	};

	return img_resize_planar_internal(im_in, &roi, im_out);
}

int img_roi_resize_planar(img_t *im_in, rect_t *roi, img_t *im_out)
{
	return img_resize_planar_internal(im_in, roi, im_out);
}

/*-------------------------------------------------------------------*/

// only hp, vp is fixed point
static int bilinearly_interpolate_fix(int top, int bottom, int left, int right, int hp, int vp, uint8_t *input, int pitch)
{
	int top_left = I2Q(IM(input, pitch, top, left), QP);         //input[top][left];
	int top_right = I2Q(IM(input, pitch, top, right), QP);       //input[top][right];
	int bottom_left = I2Q(IM(input, pitch, bottom, left), QP);   //input[bottom][left];
	int bottom_right = I2Q(IM(input, pitch, bottom, right), QP);  //input[bottom][right];

	int horizontal_progress = hp - I2Q(left, QP);
	int vertical_progress = vp - I2Q(top, QP);

	int top_block = top_left + MULQ(horizontal_progress, (top_right - top_left), QP);
	int bottom_block = bottom_left + MULQ(horizontal_progress, (bottom_right - bottom_left), QP);

	return Q2I(top_block + MULQ(vertical_progress, (bottom_block - top_block), QP), QP);
}

int img_rotation_fix(img_t *im_in, rect_t *roi, rotate_t *rotate_cfg, img_t *im_out, float (*aligned_c)[2])
{

	int cx = rotate_cfg->cx;    // horizontal
	int cy = rotate_cfg->cy;    // vertical
	float angle = rotate_cfg->angle;

	int roi_w = roi->xmax - roi->xmin;
	int roi_h = roi->ymax - roi->ymin;

	if (im_out->width == 0) {
		im_out->width = roi_w;
	}
	if (im_out->height == 0) {
		im_out->height = roi_h;
	}

	int scale_w = F2Q((float)im_out->width / (float)roi_w, QP);
	int scale_h = F2Q((float)im_out->height / (float)roi_h, QP);
	int scale = scale_w < scale_h ? scale_w : scale_h;

	int inv_scale_w = F2Q((float)roi_w / (float)im_out->width, QP);
	int inv_scale_h = F2Q((float)roi_h / (float)im_out->height, QP);
	int inv_scale = inv_scale_w > inv_scale_h ? inv_scale_w : inv_scale_h;

	int offx, offy;
	if (aligned_c != NULL) {
		offx = cx - aligned_c[0][0] / Q2F(scale, QP);
		offy = cy - aligned_c[0][1] / Q2F(scale, QP);
	} else {
		offx = roi->xmin;
		offy = roi->ymin;
	}

	int sin_val = F2Q((float)sin(angle), QP);
	int cos_val = F2Q((float)cos(angle), QP);

	if (im_out->data == NULL) {
		im_out->data = (uint8_t *)malloc((im_out->width + 8) * (im_out->height + 8) * 3);
		if (!im_out->data) {
			return -1;
		}
	}

	uint8_t *in[3];
	uint8_t *out[3];

	in[0] = im_in->data;
	in[1] = im_in->data + im_in->width * im_in->height;
	in[2] = im_in->data + im_in->width * im_in->height * 2;

	out[0] = im_out->data;
	out[1] = im_out->data + im_out->width * im_out->height;
	out[2] = im_out->data + im_out->width * im_out->height * 2;

	for (int y = 0; y < (int)im_out->height; y++) {
		for (int x = 0; x < (int)im_out->width; x++) {
			// int in_x = DIVQ(I2Q(x, QP), scale, QP) + I2Q(offx, QP);
			// int in_y = DIVQ(I2Q(y, QP), scale, QP) + I2Q(offy, QP);
			int in_x = MULQ(I2Q(x, QP), inv_scale, QP) + I2Q(offx, QP);
			int in_y = MULQ(I2Q(y, QP), inv_scale, QP) + I2Q(offy, QP);

			int vp = MULQ(cos_val, in_y - I2Q(cy, QP), QP) + MULQ(sin_val, in_x - I2Q(cx, QP), QP) + I2Q(cy, QP);
			int hp = MULQ(-sin_val, in_y - I2Q(cy, QP), QP) + MULQ(cos_val, in_x - I2Q(cx, QP), QP) + I2Q(cx, QP);

			int top = Q2I(vp, QP);
			int left = Q2I(hp, QP);

			int bottom = top + 1;
			int right = left + 1;

			if (top >= 0 && bottom < (int)im_in->height && left >= 0 && right < (int)im_in->width) {
				for (int p = 0; p < 3; p++) {
					IM(out[p], im_out->width, y, x) = (uint8_t)bilinearly_interpolate_fix(top, bottom, left, right, hp, vp, in[p], im_in->width);
				}
			}
		}
	}

	return 0;
}

/*-------------------------------------------------------------------*/

void *img_dma_copy(uint8_t *dst, uint8_t *src, uint32_t size)
{
	//TODO: implement it by pro3 API
	memcpy(dst, src, size);

	return (void *)dst;
}

/*-------------------------------------------------------------------*/
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../image/3rdparty/stb/stb_image_resize.h"

void img_stb_resize_planar(img_t *img_in, img_t *img_out)
{
	uint8_t *in[3];
	uint8_t *out[3];

	in[0] = img_in->data;
	in[1] = img_in->data + img_in->width * img_in->height;
	in[2] = img_in->data + img_in->width * img_in->height * 2;

	out[0] = img_out->data;
	out[1] = img_out->data + img_out->width * img_out->height;
	out[2] = img_out->data + img_out->width * img_out->height * 2;

	for (int i = 0; i < 3; i++) {
		stbir_resize_uint8(in[i], img_in->width, img_in->height, 0,
						   out[i], img_out->width, img_out->height, 0, 1);
	}
}

/*-------------------------------------------------------------------*/

img_t *img_create_image(int w, int h)
{
	img_t *new_img = (img_t *)malloc(sizeof(img_t) + w * h * 3);
	if (new_img == NULL) {
		printf("[%s] fail to allocate memory\r\n", __func__);
		return NULL;
	}
	new_img->width = w;
	new_img->height = h;
	new_img->data = (uint8_t *)new_img + sizeof(img_t);
	return new_img;
}

void img_free_image(img_t *im)
{
	if (im) {
		free(im);
	}
}

/*-------------------------------------------------------------------*/

void img_fill_image(img_t *im, uint8_t val)
{
	for (int i = 0; i < (int)(im->width * im->height * 3); i++) {
		im->data[i] = val;
	}
}

static void set_pixel(img_t *im, int x, int y, int c, uint8_t val)
{
	if (x < 0 || y < 0 || c < 0 || x >= (int)im->width || y >= (int)im->height || c >= 3) {
		return;
	}
	assert(x < (int)im->width && y < (int)im->height && c < 3);
	im->data[im->width * im->height * c + y * im->width + x] = val;
}

static uint8_t get_pixel(img_t *im, int x, int y, int c)
{
	assert(x < (int)im->width && y < (int)im->height && c < 3);
	return im->data[c * im->height * im->width + y * im->width + x];
}

static void img_embed_image(img_t *im_src, img_t *im_dst, int dx, int dy)
{
	for (int k = 0; k < 3; k++) {
		for (int y = 0; y < (int)im_src->height; y++) {
			for (int x = 0; x < (int)im_src->width; x++) {
				float val = get_pixel(im_src, x, y, k);
				set_pixel(im_dst, dx + x, dy + y, k, val);
			}
		}
	}
}

void img_scaled_into_letterbox(img_t *p_im_in, img_t *p_im_out)
{
	img_t *im_in = p_im_in;
	img_t *im_out = p_im_out;

	int dst_w = im_out->width;
	int dst_h = im_out->height;
	int new_w, new_h;
	if (((float)dst_w / im_in->width) < ((float)dst_h / im_in->height)) {
		new_w = dst_w;
		new_h = (im_in->height * dst_w) / im_in->width;
	} else {
		new_h = dst_h;
		new_w = (im_in->width * dst_h) / im_in->height;
	}

	img_t *resized_im = img_create_image(new_w, new_h);
	if (resized_im == NULL) {
		printf("[%s] fail to create image\r\n", __func__);
		return;
	}
	img_stb_resize_planar(im_in, resized_im);

	img_fill_image(im_out, 0);
	img_embed_image(resized_im, im_out, (dst_w - new_w) / 2, (dst_h - new_h) / 2);

	img_free_image(resized_im);
}

/*-------------------------------------------------------------------*/

void img_nv12_resize_nearest(img_t *im_in, img_t *im_out)
{
	int sw = im_in->width;
	int sh = im_in->height;
	int dw = im_out->width;
	int dh = im_out->height;
	uint32_t srcy, srcx, src_index;
	uint32_t xrIntFloat_16 = (sw << 16) / dw + 1; //better than float division
	uint32_t yrIntFloat_16 = (sh << 16) / dh + 1;
	uint8_t *dst_uv = im_out->yuv.uv_data;
	if (!dst_uv) {
		dst_uv = im_out->data + dh * dw; //memory start pointer of dest uv
	}
	uint8_t *src_uv = im_in->data + sh * sw; //memory start pointer of source uv
	uint8_t *dst_uv_yScanline = NULL;
	uint8_t *src_uv_yScanline = NULL;
	uint8_t *dst_y_slice = im_out->data; //memory start pointer of dest y
	uint8_t *src_y_slice;
	uint8_t *sp;
	uint8_t *dp;

	for (int y = 0; y < (dh & ~7); ++y) { //'dh & ~7' is to generate faster assembly code
		srcy = (y * yrIntFloat_16) >> 16;
		src_y_slice = im_in->data + srcy * sw;

		if ((y & 1) == 0) {
			dst_uv_yScanline = dst_uv + (y / 2) * dw;
			src_uv_yScanline = src_uv + (srcy / 2) * sw;
		}

		for (int x = 0; x < (dw & ~7); ++x) {
			srcx = (x * xrIntFloat_16) >> 16;
			dst_y_slice[x] = src_y_slice[srcx];

			if ((y & 1) == 0) { //y is even
				if ((x & 1) == 0) { //x is even
					src_index = (srcx / 2) * 2;

					sp = dst_uv_yScanline + x;
					dp = src_uv_yScanline + src_index;
					*sp = *dp;
					++sp;
					++dp;
					*sp = *dp;
				}
			}
		}
		dst_y_slice += dw;
	}
}

void img_nv12_resize_bilinear(img_t *im_in, img_t *im_out)
{
	uint8_t *src = (uint8_t *)im_in->data;
	uint8_t *dst = (uint8_t *)im_out->data;

	int x, y;
	int ox, oy;
	int tmpx, tmpy;
	int xratio = (im_in->width << 8) / im_out->width;
	int yratio = (im_in->height << 8) / im_out->height;
	uint8_t *dst_y = dst;
	uint8_t *dst_uv = im_out->yuv.uv_data;
	if (!dst_uv) {
		dst_uv = dst + im_out->height * im_out->width;
	}
	uint8_t *src_y = src;
	uint8_t *src_uv = src + im_in->height * im_in->width;
	uint8_t y_plane_color[2][2];
	int offsetY;
	int y_final;

	uint8_t *dst_uv_yScanline;
	uint8_t *src_uv_yScanline;
	unsigned long int src_index;
	uint8_t *sp;
	uint8_t *dp;

	tmpy = 0;
	for (int j = 0; j < (int)(im_out->height & ~7); ++j) {

		oy = tmpy >> 8;
		y = tmpy & 0xFF;
		tmpx = 0;

		if ((j & 1) == 0) {
			dst_uv_yScanline = dst_uv + (j / 2) * im_out->width;
			src_uv_yScanline = src_uv + (oy / 2) * im_in->width;
		}

		for (int i = 0; i < (int)(im_out->width & ~7); ++i) {

			ox = tmpx >> 8;
			x = tmpx & 0xFF;

			offsetY = oy * im_in->width;
			//Y use bilinear
			y_plane_color[0][0] = src_y[offsetY + ox];
			y_plane_color[1][0] = src_y[offsetY + ox + 1];
			y_plane_color[0][1] = src_y[offsetY + im_in->width + ox];
			y_plane_color[1][1] = src_y[offsetY + im_in->width + ox + 1];

			y_final = (0x100 - x) * (0x100 - y) * y_plane_color[0][0]
					  + x * (0x100 - y) * y_plane_color[1][0]
					  + (0x100 - x) * y * y_plane_color[0][1]
					  + x * y * y_plane_color[1][1];
			y_final = y_final >> 16;
			LIMIT(y_final, 0, 255);
			dst_y[ j * im_out->width + i] = (uint8_t)y_final; //set Y in dest array

			//UV use nearest
			if ((j & 1) == 0) { //y is even
				if ((i & 1) == 0) { //x is even
					src_index = (ox / 2) * 2;
					sp = dst_uv_yScanline + i;
					dp = src_uv_yScanline + src_index;
					*sp = *dp;
					++sp;
					++dp;
					*sp = *dp;
				}
			}
			tmpx += xratio;
		}
		tmpy += yratio;
	}
}

void img_nv12_resize_stb(img_t *im_in, img_t *im_out)
{
	uint8_t *dst_uv = im_out->yuv.uv_data;
	if (!dst_uv) {
		dst_uv = im_out->data + im_out->width * im_out->height;
	}
	stbir_resize_uint8(im_in->data, im_in->width, im_in->height, 0, im_out->data, im_out->width, im_out->height, 0, 1);
	stbir_resize_uint8(im_in->data + im_in->width * im_in->height, im_in->width / 2, im_in->height / 2, 0, dst_uv, im_out->width / 2,
					   im_out->height / 2, 0, 2);
}

/*-------------------------------------------------------------------*/

void img_nv12_to_rgb(img_t *nv12_in, img_t *rgb_out)
{
	const int nv_start = nv12_in->width * nv12_in->height ;
	u32  i, j, index = 0, rgb_index = 0;
	u8 y, u, v;
	int r, g, b, nv_index = 0;

	for (i = 0; i < nv12_in->height ; i++) {
		for (j = 0; j < nv12_in->width; j ++) {
			nv_index = i / 2  * nv12_in->width + j - j % 2;

			y = nv12_in->data[rgb_index];
			u = nv12_in->data[nv_start + nv_index];
			v = nv12_in->data[nv_start + nv_index + 1];

			r = y + (140 * (v - 128)) / 100; //r
			g = y - (34 * (u - 128)) / 100 - (71 * (v - 128)) / 100; //g
			b = y + (177 * (u - 128)) / 100; //b

			LIMIT(r, 0, 255);
			LIMIT(g, 0, 255);
			LIMIT(b, 0, 255);

			index = rgb_index % nv12_in->width + i * nv12_in->width;
			rgb_out->data[index * 3 + 0] = r;
			rgb_out->data[index * 3 + 1] = g;
			rgb_out->data[index * 3 + 2] = b;
			rgb_index++;
		}
	}
}

/*-------------------------------------------------------------------*/

static void draw_box(img_t *im_in, int x1, int y1, int x2, int y2)
{
	LIMIT(x1, 0, (int)im_in->width - 1);
	LIMIT(x2, 0, (int)im_in->width - 1);
	LIMIT(y1, 0, (int)im_in->height - 1);
	LIMIT(y2, 0, (int)im_in->height - 1);

	for (int i = x1; i <= x2; ++i) {
		im_in->data[i + y1 * im_in->width] = 255;
		im_in->data[i + y2 * im_in->width] = 255;
	}
	for (int i = y1; i <= y2; ++i) {
		im_in->data[x1 + i * im_in->width] = 255;
		im_in->data[x2 + i * im_in->width] = 255;
	}
}

static void draw_filled_box(img_t *im_in, int x1, int y1, int x2, int y2)
{
	LIMIT(x1, 0, (int)im_in->width - 1);
	LIMIT(x2, 0, (int)im_in->width - 1);
	LIMIT(y1, 0, (int)im_in->height - 1);
	LIMIT(y2, 0, (int)im_in->height - 1);

	int w = x2 - x1;
	for (int i = y1; i <= y2; ++i) {
		memset(&im_in->data[x1 + i * im_in->width], 255, w);
	}
}

void img_nv12_draw_box(img_t *im_in, int x1, int y1, int x2, int y2, int w)
{
	if (w >= 0) {
		for (int i = 0; i < w; ++i) {
			draw_box(im_in, x1 + i, y1 + i, x2 - i, y2 - i);
		}
	} else {
		draw_filled_box(im_in, x1, y1, x2, y2);
	}
}

/*-------------------------------------------------------------------*/

#ifdef CONFIG_VMODULE_TS
extern void ts_convert_if2of(struct ts_info_t ts_info);
extern void ts_check_end(void);
extern void ts_clear_operation_status(void);
#endif

int img_crop_then_scale(img_t *img_in, rect_t *roi, img_t *img_out)
{
#ifdef CONFIG_VMODULE_TS
	struct ts_info_t ts_info;
	memset((u8 *)&ts_info, 0, sizeof(struct ts_info_t));

	u8 *in_buf = img_in->data;
	u8 *out_buf = img_out->data;

	ts_info.in_f = TS_FORMAT_S420;
	ts_info.out_f = TS_FORMAT_S420;
	ts_info.buf = in_buf;
	ts_info.out_buf = out_buf;
	ts_info.in_w = roi->xmax - roi->xmin;
	ts_info.in_h = roi->ymax - roi->ymin;
	ts_info.in_frame_w = img_in->width;
	ts_info.in_frame_h = img_in->height;
	ts_info.in_ROI_left = roi->xmin;
	ts_info.in_ROI_top = roi->ymin;
	ts_info.out_w = img_out->width;
	ts_info.out_h = img_out->height;
	ts_info.out_frame_w = img_out->width;
	ts_info.out_frame_h = img_out->height;
	ts_info.out_ROI_left = 0;
	ts_info.out_ROI_top = 0;
	ts_info.in_stride_negative = 0;
	ts_info.out_stride_negative = 0;

	ts_convert_if2of(ts_info);

	ts_check_end();
	ts_clear_operation_status();

	return 0;
#else
	(void)img_in;
	(void)roi;
	(void)img_out;
	return -1;
#endif
}

/* choose crop and scale by which HW (GE/TS) */
#define img_rgb_planner_to_interleaved_by_GE    img_rgb_planner_to_interleaved

int img_rgb_planner_to_interleaved_by_TS(img_t *img_in, img_t *img_out)
{
#ifdef CONFIG_VMODULE_TS
	struct ts_info_t ts_info;
	memset((u8 *)&ts_info, 0, sizeof(struct ts_info_t));

	u8 *in_buf = img_in->data;
	u8 *out_buf = img_out->data;

	ts_info.in_f = TS_FORMAT_P_RGB888;
	ts_info.out_f = TS_FORMAT_RGB888;
	ts_info.buf = in_buf;
	ts_info.out_buf = out_buf;
	ts_info.in_w = img_in->width;
	ts_info.in_h = img_in->height;
	ts_info.out_w = img_out->width;
	ts_info.out_h = img_out->height;
	ts_info.lpf_en = 0;
	ts_info.tile_en = 0;
	ts_info.start_0 = 0;
	ts_info.scale_order = 0;
	ts_info.wrap_en = 0;
	ts_info.tile_w = 0;
	ts_info.tile_h = 0;
	ts_info.wrap_num = 0;

	ts_convert_if2of(ts_info);

	ts_check_end();
	ts_clear_operation_status();

	return 0;
#else
	(void)img_in;
	(void)img_out;
	return -1;
#endif
}

int img_rgb_planner_to_interleaved_by_GE(img_t *img_in, img_t *img_out)
{
#ifdef CONFIG_VMODULE_GE
	int ret = 0;

	uint32_t half_en_flag = 0;
	uint32_t quart_en_flag = 0;
	uint32_t dst_img_fmt = GE_FMT_RGB888_INT;
	uint32_t dst_img_order_ctrl = GE_RGB_ORDER;
	uint32_t roi_x = 0;
	uint32_t roi_y = 0;
	uint32_t crop_w = img_in->width;
	uint32_t crop_h = img_in->height;
	uint32_t dst_w = img_out->width;
	uint32_t dst_h = img_out->height;
	uint32_t full_limit_flag = 1;
	uint32_t offset = 0;
	uint32_t img_w = img_in->width;
	uint32_t img_h = img_in->height;
	uint32_t src_img_fmt = GE_FMT_RGB888_PLA;
	uint32_t src_img_order_ctrl = GE_RGB_ORDER;
	uint32_t input_addr = (uint32_t)img_in->data;
	uint32_t output_addr = (uint32_t)img_out->data;

	extern int run_zoom_proc(uint32_t src_addr, uint32_t dst_addr,
							 uint32_t img_w, uint32_t img_h,
							 uint32_t roi_x, uint32_t roi_y,
							 uint32_t half_en_flag, uint32_t quart_en_flag,
							 uint32_t src_img_fmt, uint32_t src_img_order_ctrl,
							 uint32_t dst_img_fmt, uint32_t dst_img_order_ctrl,
							 uint32_t crop_w, uint32_t crop_h, uint32_t dst_w, uint32_t dst_h,
							 uint32_t full_limit_flag, uint32_t offset);
	ret = run_zoom_proc(input_addr, output_addr,
						img_w, img_h, roi_x, roi_y, half_en_flag, quart_en_flag,
						src_img_fmt, src_img_order_ctrl, dst_img_fmt, dst_img_order_ctrl,
						crop_w, crop_h, dst_w, dst_h,
						full_limit_flag, offset);
	return ret;
#else
	(void)img_in;
	(void)img_out;
	return -1;
#endif
}

#include "ppu/rotate/pro3/img_ppu_rotate.h"


typedef struct {
	uint16_t *src, *dst;
} rotate_test_buffers_t;

int img_nv16_rotate_90_deinit(void)
{
	return img_ppu_rotate_deinit();
}

int img_nv16_rotate_90_init(void)
{
	int status = img_ppu_rotate_init();
	if (status) {
		printf("init failed=%d\n", status);
	}
	return status;
}

#define ROT_VERIFY 0

#if ROT_VERIFY
static unsigned verify(const uint16_t *src, const uint16_t *dst, unsigned width, unsigned height)
{
	unsigned x, y, errors = 0;
	unsigned sum = 0;
	//rotate_test_acquire_bytes(dst, width * height * 2u);
	for (y = 0; y < height; y += 16) for (x = 0; x < width; x += 16) {
			uint16_t want = src[(size_t)y * width + x], got = dst[(size_t)x * height + height - 1 - y];
			sum += want;
			if (want != got) {
				if (!errors) {
					printf("first_mismatch src=(%u,%u) expected=%04x actual=%04x\n", x, y, want, got);
				}
				errors++;
			}
		}
	printf("pixels=%u mismatches=%u sum=%d\n", width * height, errors, sum);
	return errors;
}
#endif

int img_nv16_rotate_90(img_t *img_in, img_t *img_out)
{
	if (!img_in || !img_out || !img_in->data || !img_out->data ||
		!img_in->width || !img_in->height ||
		(img_in->width % ROTATE_SIZE_ALIGNMENT) || (img_in->height % ROTATE_SIZE_ALIGNMENT) ||
		img_out->width != img_in->height || img_out->height != img_in->width) {
		return ROTATE_BAD_IMAGE;
	}
	if (((uintptr_t)img_in->data % ROTATE_ALIGNMENT) || ((uintptr_t)img_out->data % ROTATE_ALIGNMENT)) {
		return ROTATE_BAD_ALIGNMENT;
	}
	unsigned width = img_in->width, height = img_in->height;
	uint32_t bytes = width * height * 2u;
	img_rotate_image_t src = {NULL, width, height, width * 2u, bytes};
	img_rotate_image_t dst = {NULL, height, width, height * 2u, bytes};

	src.data = (uint16_t *)img_in->data;
	dst.data = (uint16_t *)img_out->data;

#if !ROT_VERIFY
	return img_ppu_rotate_u16(&src, &dst);
#else
	int rc = 1, status;
	u32 time_start = rtos_time_get_current_system_time_ms();
	status = img_ppu_rotate_u16(&src, &dst);
	printf("[%s][L:%d] perf:%d(ms) status=%d\r\n", __func__, __LINE__, rtos_time_get_current_system_time_ms() - time_start, status);
	//DCache_Invalidate((uint32_t)src.data, bytes);
	printf("[%s][L:%d] perf:%d(ms) status=%d\r\n", __func__, __LINE__, rtos_time_get_current_system_time_ms() - time_start, status);

	if (verify(src.data, dst.data, width, height)) {
		goto done;
	}
	rc = 0;
done:
	if ((status = img_ppu_rotate_deinit()) != 0) {
		printf("deinit failed=%d\n", status);
		rc = 1;
	}
	printf("PPU_ROTATE %s\n", rc ? "FAIL" : "PASS");
	return rc;
#endif
}
