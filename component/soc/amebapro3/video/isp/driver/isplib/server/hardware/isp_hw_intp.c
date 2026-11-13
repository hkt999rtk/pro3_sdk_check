/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Kelsey Chen <kelsey_chen@realsil.com.cn>
 */

#include <isp_os_driver.h>
#include <isp_utils.h>
#include <isp_reg_virtual.h>
#include <isp_reg_intp.h>
#include "isp_hw_intp.h"

#define MOD_NAME INTP
static void isp_hw_edge_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t y_blend_rate_0, y_blend_rate_1, intp_edge_thd_offset;
	uint32_t edge_slope;

	y_blend_rate_0 = READ_REG(mod_info, Y_BLEND_RATE_0);
	y_blend_rate_1 = READ_REG(mod_info, Y_BLEND_RATE_1);
	intp_edge_thd_offset = READ_REG(mod_info, EDGE_THD0_OFFSET);

	edge_slope =
		(uint32_t)(((float)((y_blend_rate_1 - y_blend_rate_0) * 256.0) /
					(float)(intp_edge_thd_offset)) +
				   0.5);

	WRITE_REG(mod_info, EDGE_SLOPE, edge_slope);
}

static void isp_hw_moire_rate(FW_EST_MOD_INFO mod_info, int moire_sum_r,
							  int moire_num_r, int moire_sum_g, int moire_num_g,
							  int moire_sum_b, int moire_num_b)
{
	int moire_r_rate, moire_b_rate;
	float r_ratio, g_ratio, b_ratio;

	if (moire_num_r != 0)
		r_ratio = moire_sum_r /
				  moire_num_r; // read from 0x1c3c and 0x1c48
	else {
		r_ratio = 0;
	}
	if (moire_num_g != 0)
		g_ratio = moire_sum_g /
				  moire_num_g; // read from 0x1c40 and 0x1c4c
	else {
		g_ratio = 0;
	}
	if (moire_num_b != 0)
		b_ratio = moire_sum_b /
				  moire_num_b; // read from 0x1c44 and 0x1c50
	else {
		b_ratio = 0;
	}
	if (g_ratio != 0) {
		moire_r_rate =
			(int)128 * r_ratio / g_ratio; // write to 0x1c34 [23:16]
		moire_b_rate =
			(int)128 * b_ratio / g_ratio; // write to 0x1c34 [15:8]
	} else {
		moire_r_rate = 0;
		moire_b_rate = 0;
	}

	WRITE_REG(mod_info, MOIRE_R_RATE, moire_r_rate);
	WRITE_REG(mod_info, MOIRE_B_RATE, moire_b_rate);
}

static void isp_hw_mi_slope(FW_EST_MOD_INFO mod_info)
{
	int mi_edge_thd0_offset_max =
		READ_REG(mod_info, MI_EDGE_THD0_OFFSET_MAX);
	int mi_edge_thd0_offset_min =
		READ_REG(mod_info, MI_EDGE_THD0_OFFSET_MIN);
	int mi_edge_thd0_thd1 = READ_REG(mod_info, MI_EDGE_THD0_THD1);
	int mi_edge_thd0_thd0 = READ_REG(mod_info, MI_EDGE_THD0_THD0);
	int mi_edge_thd0_slope =
		(mi_edge_thd0_thd1 == mi_edge_thd0_thd0) ?
		0 :
		(int)((float)(mi_edge_thd0_offset_max -
					  mi_edge_thd0_offset_min) /
			  (float)(mi_edge_thd0_thd1 -
					  mi_edge_thd0_thd0) *
			  64.0 +
			  0.5);
	WRITE_REG(mod_info, MI_EDGE_THD0_SLOPE, mi_edge_thd0_slope);

	int mi_change_ratio_rate_max =
		READ_REG(mod_info, MI_CHANGE_RATIO_RATE_MAX);
	int mi_change_ratio_rate_min =
		READ_REG(mod_info, MI_CHANGE_RATIO_RATE_MIN);
	int mi_change_ratio_thd1 = READ_REG(mod_info, MI_CHANGE_RATIO_THD1);
	int mi_change_ratio_thd0 = READ_REG(mod_info, MI_CHANGE_RATIO_THD0);
	int mi_change_ratio_slope =
		(mi_change_ratio_thd1 == mi_change_ratio_thd0) ?
		0 :
		(int)((float)(mi_change_ratio_rate_max -
					  mi_change_ratio_rate_min) /
			  (float)(mi_change_ratio_thd1 -
					  mi_change_ratio_thd0) *
			  64.0 +
			  0.5);
	WRITE_REG(mod_info, MI_CHANGE_RATIO_SLOPE, mi_change_ratio_slope);

	int mi_enhance_ratio_rate_max =
		READ_REG(mod_info, MI_ENHANCE_RATIO_RATE_MAX);
	int mi_enhance_ratio_rate_min =
		READ_REG(mod_info, MI_ENHANCE_RATIO_RATE_MIN);
	int mi_enhance_ratio_thd1 = READ_REG(mod_info, MI_ENHANCE_RATIO_THD1);
	int mi_enhance_ratio_thd0 = READ_REG(mod_info, MI_ENHANCE_RATIO_THD0);
	int mi_enhance_ratio_slope =
		(mi_enhance_ratio_thd1 == mi_enhance_ratio_thd0) ?
		0 :
		(int)((float)(mi_enhance_ratio_rate_max -
					  mi_enhance_ratio_rate_min) /
			  (float)(mi_enhance_ratio_thd1 -
					  mi_enhance_ratio_thd0) *
			  64.0 +
			  0.5);
	WRITE_REG(mod_info, MI_ENHANCE_RATIO_SLOPE, mi_enhance_ratio_slope);
}

static int isp_hw_intp_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_edge_slope(mod_info);
	isp_hw_mi_slope(mod_info);

	return 0;
}

static int isp_hw_intp_est_per_frame(FW_EST_MOD_INFO mod_info)
{
	isp_hw_moire_rate(mod_info, READ_REG(mod_info, MOIRE_SUM_R),
					  READ_REG(mod_info, MOIRE_NUM_R),
					  READ_REG(mod_info, MOIRE_SUM_G),
					  READ_REG(mod_info, MOIRE_NUM_G),
					  READ_REG(mod_info, MOIRE_SUM_B),
					  READ_REG(mod_info, MOIRE_NUM_B));

	return 0;
}

static int isp_hw_intp_init(uint32_t isp_id)
{
	isp_write_reg(1, INTP_MI_EN);

	return RTS_ISP_OK;
}

static int isp_hw_intp_update_moire(uint32_t isp_id)
{
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	return isp_hw_intp_est_per_frame(&info);
}

static int isp_hw_intp_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_intp_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(INTP_REG_LOAD_MASK, INTP_SYNC);

	return RTS_ISP_OK;
}

static const struct isp_hw_intp_ops hw_intp_ops = {
	.init = isp_hw_intp_init,
	.est_per_frame = isp_hw_intp_update_moire,
	.est_dynamic = isp_hw_intp_estimation,
};

const struct isp_hw_intp_ops *isp_hw_intp_get_ops(void)
{
	return &hw_intp_ops;
}
