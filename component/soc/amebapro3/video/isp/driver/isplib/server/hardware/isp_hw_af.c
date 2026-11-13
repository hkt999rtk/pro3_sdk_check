/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_reg_af1.h>
#include "isp_hw_af.h"

#define AF_WIN_COLS 16
#define AF_WIN_ROWS 16
#define AF_WIN_ALIGN_H 2
#define AF_WIN_ALIGN_V 2
#define AF_WIN_SAMPLE_GAP_H 2
#define AF_WIN_SAMPLE_GAP_V 2

static int isp_hw_af_get_hw_info(uint32_t isp_id, struct isp_af_hw_info *info)
{
	if (!info) {
		return -RTS_ISP_EINVAL;
	}

	info->grid_info.size.cols = AF_WIN_COLS;
	info->grid_info.size.rows = AF_WIN_ROWS;
	info->grid_info.cell_align.w = AF_WIN_SAMPLE_GAP_H;
	info->grid_info.cell_align.h = AF_WIN_SAMPLE_GAP_V;
	info->cell_sample.w = AF_WIN_SAMPLE_GAP_H;
	info->cell_sample.h = AF_WIN_SAMPLE_GAP_V;
	info->illum_cnt_size = 2;
	info->filter_size = 4;

	return RTS_ISP_OK;
}

static int isp_hw_af_set_statis(uint32_t isp_id, const rts_isp_grid_t *grid)
{
	int i;

	for (i = 0; i < 2; i++) {
		isp_write_reg((grid->start.x << AF_START_X_OFFSET |
					   grid->start.y << AF_START_Y_OFFSET),
					  ISP_AF_WIN_START + AF_REG_OFFSET(i));
		isp_write_reg(
			((grid->cell.w / AF_WIN_ALIGN_H) << WIN_WIDTH_OFFSET |
			 (grid->cell.h / AF_WIN_ALIGN_V) << WIN_HEIGHT_OFFSET |
			 grid->size.rows << WIN_NUM_H_OFFSET |
			 grid->size.cols << WIN_NUM_V_OFFSET),
			ISP_AF_WIN_SIZE + AF_REG_OFFSET(i));
		isp_write_reg(0x1, ISP_AF_MODE + AF_REG_OFFSET(i));
	}

	return RTS_ISP_OK;
}

static int isp_hw_af_set_iir(uint32_t isp_id)
{
	int i;
	uint32_t iir_shift0, iir_shift1;
	uint32_t iir_para0, iir_para1, iir_para2;

	int iir_para[7] = { 404, -72, 465, -431, 81, -432, 200 };
	int iir_shift[5] = { 4, 2, 1, 0, 1 };

	iir_shift0 = iir_shift[0] << H1_IIR_SHIFT_0_OFFSET |
				 iir_shift[1] << H1_IIR_SHIFT_1_OFFSET |
				 iir_shift[2] << H1_IIR_SHIFT_2_OFFSET |
				 iir_shift[3] << H1_IIR_SHIFT_3_OFFSET;
	iir_shift1 = iir_shift[4];
	iir_para0 = (iir_para[1] & 0x3ff) << H1_IIR_PARA_1_OFFSET |
				(iir_para[2] & 0x3ff) << H1_IIR_PARA_2_OFFSET |
				(iir_para[3] & 0x3ff) << H1_IIR_PARA_3_OFFSET;
	iir_para1 = (iir_para[4] & 0x3ff) << H1_IIR_PARA_4_OFFSET |
				(iir_para[5] & 0x3ff) << H1_IIR_PARA_5_OFFSET |
				(iir_para[6] & 0x3ff) << H1_IIR_PARA_6_OFFSET;
	iir_para2 = iir_para[0] & 0xffff;
	for (i = 0; i < 2; i++) {
		isp_write_reg(iir_shift0,
					  ISP_AF_H1_IIR_SHIFT_0 + AF_REG_OFFSET(i));
		isp_write_reg(iir_shift1,
					  ISP_AF_H1_IIR_SHIFT_1 + AF_REG_OFFSET(i));
		isp_write_reg(iir_shift0,
					  ISP_AF_H2_IIR_SHIFT_0 + AF_REG_OFFSET(i));
		isp_write_reg(iir_shift1,
					  ISP_AF_H2_IIR_SHIFT_1 + AF_REG_OFFSET(i));
		isp_write_reg(iir_para0,
					  ISP_AF_H1_IIR_PARA_0 + AF_REG_OFFSET(i));
		isp_write_reg(iir_para1,
					  ISP_AF_H1_IIR_PARA_1 + AF_REG_OFFSET(i));
		isp_write_reg(iir_para2,
					  ISP_AF_H1_IIR_PARA_2 + AF_REG_OFFSET(i));
		isp_write_reg(iir_para0,
					  ISP_AF_H2_IIR_PARA_0 + AF_REG_OFFSET(i));
		isp_write_reg(iir_para1,
					  ISP_AF_H2_IIR_PARA_1 + AF_REG_OFFSET(i));
		isp_write_reg(iir_para2,
					  ISP_AF_H2_IIR_PARA_2 + AF_REG_OFFSET(i));
	}

	return RTS_ISP_OK;
}

static int isp_hw_af_set_statis_config(uint32_t isp_id)
{
	int i;
	uint32_t ldg_low_reg, ldg_high_reg, coring_reg;

	ldg_low_reg = 1 << H1_LDG_EN_OFFSET | 32 << H1_LDG_SLOPE1_OFFSET |
				  15 << H1_LDG_LOW_GAIN_OFFSET | 20 << H1_LDG_LOW_TH_OFFSET;
	ldg_high_reg = 120 << H1_LDG_HIGH_TH_OFFSET |
				   20 << H1_LDG_HIGH_GAIN_OFFSET |
				   96 << H1_LDG_SLOPE2_OFFSET;
	coring_reg = 32 << H1_CORING_SLOPE_OFFSET |
				 1024 << H1_CORING_HIGH_TH_OFFSET |
				 30 << H1_CORING_LOW_TH_OFFSET;

	for (i = 0; i < 2; i++) {
		isp_write_reg(0, ISP_AF_EN + AF_REG_OFFSET(i));
		isp_write_reg((1 << BLC_EN_OFFSET | 65 << BLC_GR_OFFSET_OFFSET |
					   65 << BLC_GB_OFFSET_OFFSET),
					  ISP_AF_BLC + AF_REG_OFFSET(i));
		isp_write_reg(ldg_low_reg, ISP_AF_H1_LDG_0 + AF_REG_OFFSET(i));
		isp_write_reg(ldg_high_reg, ISP_AF_H1_LDG_1 + AF_REG_OFFSET(i));
		isp_write_reg(coring_reg, ISP_AF_H1_CORING + AF_REG_OFFSET(i));
		isp_write_reg(ldg_low_reg, ISP_AF_V1_LDG_0 + AF_REG_OFFSET(i));
		isp_write_reg(ldg_high_reg, ISP_AF_V1_LDG_1 + AF_REG_OFFSET(i));
		isp_write_reg(coring_reg, ISP_AF_V1_CORING + AF_REG_OFFSET(i));
		isp_write_reg(ldg_low_reg, ISP_AF_H2_LDG_0 + AF_REG_OFFSET(i));
		isp_write_reg(ldg_high_reg, ISP_AF_H2_LDG_1 + AF_REG_OFFSET(i));
		isp_write_reg(coring_reg, ISP_AF_H2_CORING + AF_REG_OFFSET(i));
		isp_write_reg(ldg_low_reg, ISP_AF_V2_LDG_0 + AF_REG_OFFSET(i));
		isp_write_reg(ldg_high_reg, ISP_AF_V2_LDG_1 + AF_REG_OFFSET(i));
		isp_write_reg(coring_reg, ISP_AF_V2_CORING + AF_REG_OFFSET(i));
		isp_write_reg(0x400, ISP_AF_CONFIG_0 + AF_REG_OFFSET(i));
		isp_write_reg((128 << BLEND_RATIO1_OFFSET |
					   128 << BLEND_RATIO2_OFFSET),
					  ISP_AF_BLEND_RATIO + AF_REG_OFFSET(i));
		isp_write_reg((0 << FV_SHIFTE1_0_OFFSET |
					   0 << FV_SHIFTE1_1_OFFSET),
					  ISP_AF_SHIFTER1 + AF_REG_OFFSET(i));
		isp_write_reg((0 << FV_SHIFTE2_0_OFFSET |
					   0 << FV_SHIFTE2_1_OFFSET),
					  ISP_AF_SHIFTER2 + AF_REG_OFFSET(i));
	}

	return isp_hw_af_set_iir(isp_id);
}

static const struct isp_hw_af_ops hw_af_ops = {
	.get_hw_info = isp_hw_af_get_hw_info,
	.set_statis = isp_hw_af_set_statis,
	.set_statis_config = isp_hw_af_set_statis_config,
};

const struct isp_hw_af_ops *isp_hw_af_get_ops(void)
{
	return &hw_af_ops;
}
