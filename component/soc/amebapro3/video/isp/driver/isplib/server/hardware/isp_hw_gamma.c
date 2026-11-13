/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_utils.h>
#include <isp_reg_gamma.h>
#include "isp_hw_gamma.h"

#define MOD_NAME GMA

static void isp_hw_gamma_in_r_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t gamma_in_r_offset_black, gamma_in_r_offset_white;
	uint32_t gamma_in_r_slope;

	gamma_in_r_offset_black = READ_REG(mod_info, GAMMA_IN_R_OFFSET_BLACK);
	gamma_in_r_offset_white = READ_REG(mod_info, GAMMA_IN_R_OFFSET_WHITE);

	gamma_in_r_slope = (uint32_t)(ceilf(
									  (float)(gamma_in_r_offset_black + gamma_in_r_offset_white) *
									  4096.0 /
									  (float)(4095 - gamma_in_r_offset_black -
											  gamma_in_r_offset_white)));

	WRITE_REG(mod_info, GAMMA_IN_R_SLOPE, gamma_in_r_slope);
}

static void isp_hw_gamma_in_g_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t gamma_in_g_offset_black, gamma_in_g_offset_white;
	uint32_t gamma_in_g_slope;

	gamma_in_g_offset_black = READ_REG(mod_info, GAMMA_IN_G_OFFSET_BLACK);
	gamma_in_g_offset_white = READ_REG(mod_info, GAMMA_IN_G_OFFSET_WHITE);

	gamma_in_g_slope = (uint32_t)(ceilf(
									  (float)(gamma_in_g_offset_black + gamma_in_g_offset_white) *
									  4096.0 /
									  (float)(4095 - gamma_in_g_offset_black -
											  gamma_in_g_offset_white)));

	WRITE_REG(mod_info, GAMMA_IN_G_SLOPE, gamma_in_g_slope);
}

static void isp_hw_gamma_in_b_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t gamma_in_b_offset_black, gamma_in_b_offset_white;
	uint32_t gamma_in_b_slope;

	gamma_in_b_offset_black = READ_REG(mod_info, GAMMA_IN_B_OFFSET_BLACK);
	gamma_in_b_offset_white = READ_REG(mod_info, GAMMA_IN_B_OFFSET_WHITE);

	gamma_in_b_slope = (uint32_t)(ceilf(
									  (float)(gamma_in_b_offset_black + gamma_in_b_offset_white) *
									  4096.0 /
									  (float)(4095 - gamma_in_b_offset_black -
											  gamma_in_b_offset_white)));

	WRITE_REG(mod_info, GAMMA_IN_B_SLOPE, gamma_in_b_slope);
}

static int isp_hw_gamma_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_gamma_in_r_slope(mod_info);
	isp_hw_gamma_in_g_slope(mod_info);
	isp_hw_gamma_in_b_slope(mod_info);
	return 0;
}

static int isp_hw_gamma_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_gamma_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(GAMMA_PLOAD_DLY_MASK, GAMMA_SYNC);

	return RTS_ISP_OK;
}

static int isp_hw_set_gamma(uint32_t isp_id,
							const isp_iq_gamma_v1_dyn_item_t *gamma)
{
	int i;

	if (!gamma || gamma->curve_count != 35) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < gamma->curve_count; i++) {
		isp_write_reg(gamma->curve[i], GMA_GAMMA_P_0 + i * 4);
	}

	isp_write_reg(gamma->offsets[0], GMA_GAMMA_IN_R_OFFSET_BLACK);
	isp_write_reg(gamma->offsets[0], GMA_GAMMA_IN_G_OFFSET_BLACK);
	isp_write_reg(gamma->offsets[0], GMA_GAMMA_IN_B_OFFSET_BLACK);
	isp_write_reg(gamma->offsets[1], GMA_GAMMA_IN_R_OFFSET_WHITE);
	isp_write_reg(gamma->offsets[1], GMA_GAMMA_IN_G_OFFSET_WHITE);
	isp_write_reg(gamma->offsets[1], GMA_GAMMA_IN_B_OFFSET_WHITE);
	isp_write_reg(GAMMA_LEVEL_ADJ_EN_MASK, GMA_GAMMA_LEVEL_ADJ_EN);

	return RTS_ISP_OK;
}

static const struct isp_hw_gamma_ops hw_gamma_ops = {
	.est_dynamic = isp_hw_gamma_estimation,
	.set_gamma = isp_hw_set_gamma,
};

const struct isp_hw_gamma_ops *isp_hw_gamma_get_ops(void)
{
	return &hw_gamma_ops;
}
