/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_reg_vhdrfs.h>
#include "isp_hw_fusion.h"

#define MOD_NAME FUS
static void isp_hw_rate_lumin_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t fusion_lumin_thd1, fusion_lumin_thd0;
	uint32_t rate_lumin_slope;

	fusion_lumin_thd0 = READ_REG(mod_info, FUSION_THD0);
	fusion_lumin_thd1 = READ_REG(mod_info, FUSION_THD1);

	rate_lumin_slope =
		(uint32_t)((256.0 /
					(float)(fusion_lumin_thd1 - fusion_lumin_thd0) *
					4096.0) +
				   0.5);

	WRITE_REG(mod_info, FUSION_SLOPE, rate_lumin_slope);
}

static void isp_hw_rate_diff_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t fusion_diff_offset;
	uint32_t rate_diff_slope;

	fusion_diff_offset = READ_REG(mod_info, FUSION_DIFF_OFFSET);

	rate_diff_slope =
		(uint32_t)(256.0 / (float)(fusion_diff_offset) * 4096.0);

	WRITE_REG(mod_info, FUSION_DIFF_SLOPE, rate_diff_slope);
}

static void isp_hw_over_exposed_L_slop(FW_EST_MOD_INFO mod_info)
{
	uint16_t over_exposed_thd1, over_exposed_thd0, over_exposed_rate1_long;
	uint32_t over_exposed_slope;

	over_exposed_thd0 = READ_REG(mod_info, OVER_EXPOSED_THD0_LONG);
	over_exposed_thd1 = READ_REG(mod_info, OVER_EXPOSED_THD1_LONG);
	over_exposed_rate1_long = READ_REG(mod_info, OVER_EXPOSED_RATE1_LONG);

	over_exposed_slope =
		(uint32_t)(((float)(over_exposed_rate1_long) /
					(float)(over_exposed_thd1 - over_exposed_thd0)) *
				   64.0);

	WRITE_REG(mod_info, OVER_EXPOSED_SLOPE_LONG, over_exposed_slope);
}

static void isp_hw_over_exposed_S_slop(FW_EST_MOD_INFO mod_info)
{
	uint16_t over_exposed_thd1, over_exposed_thd0, over_exposed_rate1_short;
	uint32_t over_exposed_slope;

	over_exposed_thd0 = READ_REG(mod_info, OVER_EXPOSED_THD0_SHORT);
	over_exposed_thd1 = READ_REG(mod_info, OVER_EXPOSED_THD1_SHORT);
	over_exposed_rate1_short = READ_REG(mod_info, OVER_EXPOSED_RATE1_SHORT);

	over_exposed_slope =
		(uint32_t)(((float)(over_exposed_rate1_short) /
					(float)(over_exposed_thd1 - over_exposed_thd0)) *
				   64.0);

	WRITE_REG(mod_info, OVER_EXPOSED_SLOPE_SHORT, over_exposed_slope);
}

static void isp_hw_exposure_ratio_inv(FW_EST_MOD_INFO mod_info)
{
	int exposure_ratio = READ_REG(mod_info, EXPOSURE_RATIO);
	int exposure_ratio_inv = (int)(4096.0 * 4.0 / (float)(exposure_ratio));

	WRITE_REG(mod_info, EXPOSURE_RATIO_INV, exposure_ratio_inv);
}

static int isp_hw_fus_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_rate_lumin_slope(mod_info);
	isp_hw_rate_diff_slope(mod_info);
	isp_hw_over_exposed_L_slop(mod_info);
	isp_hw_over_exposed_S_slop(mod_info);
	isp_hw_exposure_ratio_inv(mod_info);

	return 0;
}

static int isp_hw_fus_init(FW_EST_MOD_INFO mod_info)
{
	WRITE_REG(mod_info, OVER_EXPOSED_THD0_LONG, 2048); //FUS
	WRITE_REG(mod_info, OVER_EXPOSED_THD1_LONG, 2560); //FUS
	WRITE_REG(mod_info, OVER_EXPOSED_SLOPE_LONG, 8); //FUS
	WRITE_REG(mod_info, OVER_EXPOSED_THD0_SHORT, 3967); //FUS
	WRITE_REG(mod_info, OVER_EXPOSED_THD1_SHORT, 4095); //FUS
	WRITE_REG(mod_info, OVER_EXPOSED_SLOPE_SHORT, 32); //FUS

	return RTS_ISP_OK;
}

static int isp_hw_fusion_init(uint32_t isp_id)
{
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	return isp_hw_fus_init(&info);
}

static int isp_hw_fusion_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_fus_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(VHDRFS_REG_LOAD_MASK, FUSION_REG_LOAD);

	return 0;
}

static const struct isp_hw_fusion_ops hw_fusion_ops = {
	.init = isp_hw_fusion_init,
	.est_dynamic = isp_hw_fusion_estimation,
};

const struct isp_hw_fusion_ops *isp_hw_fusion_get_ops(void)
{
	return &hw_fusion_ops;
}
