/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_reg_virtual.h>
#include <isp_reg_lce.h>
#include "isp_hw_lce.h"

#define MOD_NAME LCE
#define BITS_SCALE 17

static void isp_hw_lce_info(FW_EST_MOD_INFO mod_info, int w, int h)
{
	int scale_unit = 1 << BITS_SCALE;
	//int bin_shift = 2;

	int bk_w = READ_REG(mod_info, BK_W);
	int bk_h = READ_REG(mod_info, BK_H);
	int bk_ygap = 0; //READ_REG(mod_info, BK_YGAP); //cmodel_only
	int clip_val = READ_VIRT_REG(mod_info, VLCE_CLIP_VAL); //8~64

	int bk_sx;
	int bk_sy;
	int bk_ex;
	int bk_ey;
	int bk_xsize; // bk_xsize = (w-bk_sx)/bk_w;
	int bk_ysize; // bk_ysize = (h-bk_sy)/bk_h;
	int x_step;
	int y_step;
	int dx0_val;
	int dy0_val;
	int h2c_factor;
	int his_clip;

	float his_clip_log2;
	int h2c_bits_shift;
	int nor_gfactor;

	bk_xsize = w / bk_w;
	bk_ysize = h / bk_h;
	bk_ysize -= bk_ygap;

	bk_sx = (w - bk_xsize * bk_w) / 2;
	bk_sy = (h - (bk_ysize + bk_ygap) * bk_h) / 2;
	bk_ex = bk_sx + bk_w * bk_xsize - 1;
	bk_ey = bk_sy + bk_h * (bk_ysize + bk_ygap) - 1;

	x_step = scale_unit / bk_xsize;
	y_step = scale_unit / (bk_ysize + bk_ygap);
	dx0_val = (scale_unit / 2 - bk_sx * x_step) >> 8;
	dy0_val = (scale_unit / 2 - bk_sy * y_step) >> 8;

	// 512<(4096<<h2c_bits_shift)/(bk_xsize*bk_ysize)<1024
	//==>8*2^h2c_bits_shift > bk_xsize*bk_ysize;
	//==>h2c_bits_shift = ceil(log2(bk_xsize*bk_ysize))-3
	his_clip_log2 = log2(bk_xsize * bk_ysize);
	h2c_bits_shift = ceil(his_clip_log2) - 3; //6~7
	h2c_factor = (4096 << h2c_bits_shift) /
				 (bk_xsize * bk_ysize); //512~1023    u8.(4+h2c_bits_shift)
	his_clip =
		((bk_xsize * bk_ysize * clip_val) * 4) / 2048; //clip_val=8~64
	nor_gfactor = 1024 / (bk_w * bk_h); //21~113

	//update reg
	WRITE_REG(mod_info, BK_SX, bk_sx);
	WRITE_REG(mod_info, BK_SY, bk_sy);
	WRITE_REG(mod_info, BK_EX, bk_ex);
	WRITE_REG(mod_info, BK_EY, bk_ey);
	WRITE_REG(mod_info, BK_XSIZE, bk_xsize);
	WRITE_REG(mod_info, BK_YSIZE, bk_ysize);
	WRITE_REG(mod_info, X_STEP, x_step);
	WRITE_REG(mod_info, Y_STEP, y_step);
	WRITE_REG(mod_info, DX0_VAL, dx0_val);
	WRITE_REG(mod_info, DY0_VAL, dy0_val);
	WRITE_REG(mod_info, H2C_BITS_SHIFT, h2c_bits_shift);
	WRITE_REG(mod_info, H2C_FACTOR, h2c_factor);
	WRITE_REG(mod_info, HIS_CLIP, his_clip);
	WRITE_REG(mod_info, NOR_GFACTOR, nor_gfactor);
}

static int isp_hw_lce_init_vreg(uint32_t isp_id)
{
	isp_write_vreg(12, VLCE_CLIP_VAL);

	return RTS_ISP_OK;
}

static int isp_hw_lce_set_size(uint32_t isp_id, const rts_isp_rect_t *rect)
{
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	isp_hw_lce_info(&info, rect->size.w, rect->size.h);
	isp_write_reg(LCE_REG_LOAD_MASK, LCE_SYNC);

	return RTS_ISP_OK;
}

static const struct isp_hw_lce_ops hw_lce_ops = {
	.init_vreg = isp_hw_lce_init_vreg,
	.set_size = isp_hw_lce_set_size,
};

const struct isp_hw_lce_ops *isp_hw_lce_get_ops(void)
{
	return &hw_lce_ops;
}
