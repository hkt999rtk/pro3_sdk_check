/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_reg_isp_sys.h>
#include <isp_reg_rgb2yuv.h>
#include "isp_hw_rgb2yuv.h"

#define COEF_MULT 4096

struct csc_coef {
	int yr;
	int yg;
	int yb;
	int ur;
	int ug;
	int ub;
	int vr;
	int vg;
	int vb;
	int y_offset;
};

static struct csc_coef coef[] = {
	[ISP_COLOR_RANGE_BT601_STUDIO_SWING] = {
		.yr = 1053,
		.yg = 2064,
		.yb = 401,
		.ur = 606,
		.ug = 1192,
		.ub = 1798,
		.vr = 1798,
		.vg = 1507,
		.vb = 291,
		.y_offset = 256,
	},
	[ISP_COLOR_RANGE_BT601_FULL_SWING] = {
		.yr = 1225,
		.yg = 2404,
		.yb = 467,
		.ur = 692,
		.ug = 1356,
		.ub = 2048,
		.vr = 2048,
		.vg = 1716,
		.vb = 332,
		.y_offset = 0,
	},
	[ISP_COLOR_RANGE_BT709_STUDIO_SWING] = {
		.yr = 750,
		.yg = 2515,
		.yb = 254,
		.ur = 410,
		.ug = 1388,
		.ub = 1798,
		.vr = 1798,
		.vg = 1634,
		.vb = 164,
		.y_offset = 256,
	},
	[ISP_COLOR_RANGE_BT709_FULL_SWING] = {
		.yr = 872,
		.yg = 2929,
		.yb = 295,
		.ur = 471,
		.ug = 1577,
		.ub = 2048,
		.vr = 2048,
		.vg = 1860,
		.vb = 188,
		.y_offset = 0,
	},
};

static struct csc_coef *csc = &coef[ISP_COLOR_RANGE_BT709_FULL_SWING];

static int isp_hw_rgb2yuv_set(uint32_t isp_id, int color_range)
{
	if (color_range < ISP_COLOR_RANGE_BT601_STUDIO_SWING ||
		color_range > ISP_COLOR_RANGE_BT709_FULL_SWING) {
		return -RTS_ISP_EINVAL;
	}

	csc = &coef[color_range];

	isp_write_reg((csc->yr << YBT709_YR_OFFSET |
				   csc->yg << YBT709_YG_OFFSET),
				  RGB2YUV0);
	isp_write_reg((csc->yb << YBT709_YB_OFFSET |
				   csc->ur << YBT709_UR_OFFSET),
				  RGB2YUV1);
	isp_write_reg((csc->ug << YBT709_UG_OFFSET |
				   csc->ub << YBT709_UB_OFFSET),
				  RGB2YUV2);
	isp_write_reg((csc->vr << YBT709_VR_OFFSET |
				   csc->vg << YBT709_VG_OFFSET),
				  RGB2YUV3);
	isp_write_reg((csc->vb << YBT709_VB_OFFSET), RGB2YUV4);
	isp_write_reg(csc->y_offset, RGB2YUV_YOFFSET);
	if (csc->y_offset) {
		isp_write_reg_mask(YBT709_MASK, SYS_CONTROL2,
						   YMODE_MASK | YBT709_MASK);
	} else {
		isp_write_reg_mask(YMODE_MASK | YBT709_MASK, SYS_CONTROL2,
						   YMODE_MASK | YBT709_MASK);
	}

	return RTS_ISP_OK;
}

static int isp_hw_rgb2yuv_convert(uint32_t isp_id, uint32_t *rgb2yuv)
{
	int r, g, b;
	int y, u, v;

	if (!rgb2yuv) {
		return -RTS_ISP_EINVAL;
	}

	r = *rgb2yuv & 0xff;
	g = (*rgb2yuv >> 8) & 0xff;
	b = (*rgb2yuv >> 16) & 0xff;

	y = csc->y_offset / 4;
	y += (csc->yr * r + csc->yg * g + csc->yb * b) / COEF_MULT;
	u = (-csc->ur * r - csc->ug * g + csc->ub * b) / COEF_MULT + 128;
	v = (csc->vr * r - csc->vg * g - csc->vb * b) / COEF_MULT + 128;

	*rgb2yuv = y | u << 8 | v << 16;

	return RTS_ISP_OK;
}

static const struct isp_hw_rgb2yuv_ops hw_rgb2yuv_ops = {
	.set = isp_hw_rgb2yuv_set,
	.convert = isp_hw_rgb2yuv_convert,
};

const struct isp_hw_rgb2yuv_ops *isp_hw_rgb2yuv_get_ops(void)
{
	return &hw_rgb2yuv_ops;
}
