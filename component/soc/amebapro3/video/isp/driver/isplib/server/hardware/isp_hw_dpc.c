/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_reg_dpc_l.h>
#include <isp_reg_virtual.h>
#include "isp_hw_dpc.h"

#define MOD_NAME DPC
static void isp_hw_bl_gain_init(FW_EST_MOD_INFO mod_info)
{
	uint32_t sensor_bit_depth =
		READ_VIRT_REG(mod_info, VSYS_SENSOR_BIT_DEPTH);
	sensor_bit_depth =
		sensor_bit_depth > 12 ?
		12 :
		sensor_bit_depth; // max 12 bits for linear mode
	uint32_t m = (1 << (12 - sensor_bit_depth)) - 1;

	uint16_t BL_OFFSET_12BIT_CH;
	uint32_t BL_OFFSET_GAIN_12BIT_CH;

	for (int i = 0; i < 4; i++) {
		BL_OFFSET_12BIT_CH =
			READ_REG_ARRAY_I(mod_info, BL_OFFSET_12BIT_CH, i);

		BL_OFFSET_GAIN_12BIT_CH = (uint32_t)ceilf(
									  (float)((BL_OFFSET_12BIT_CH + m) << 12) /
									  (float)(4095 - m - BL_OFFSET_12BIT_CH));

		WRITE_REG_ARRAY_I(mod_info, BL_OFFSET_GAIN_12BIT_CH, i,
						  BL_OFFSET_GAIN_12BIT_CH);
	}
}

static int isp_hw_dpc_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_bl_gain_init(mod_info);

	return 0;
}

static int isp_hw_dpc_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_dpc_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(REG_LOAD_MASK, DPC_REGLOAD);

	return 0;
}

static const struct isp_hw_dpc_ops hw_dpc_ops = {
	.est_dynamic = isp_hw_dpc_estimation,
};

const struct isp_hw_dpc_ops *isp_hw_dpc_get_ops(void)
{
	return &hw_dpc_ops;
}
