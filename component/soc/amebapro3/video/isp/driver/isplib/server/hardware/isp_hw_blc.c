/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <rts_isp.h>
#include <isp_reg_blc_l.h>
#include <isp_reg_virtual.h>
#include <isp_reg_isp_sys.h>
#include "isp_hw_blc.h"

#define MOD_NAME BLC

static uint32_t get_blc_gain(uint32_t offset, uint32_t m)
{
	uint32_t v0;
	uint32_t v1;

	v0 = (offset + m) << 12;
	v1 = 4095 - m - offset;

	return (uint32_t)ceilf((float)v0 / (float)v1);
}

static void isp_hw_offset_gain(FW_EST_MOD_INFO mod_info)
{
	uint32_t blc_offset_gain_r, blc_offset_gain_g1, blc_offset_gain_g2,
			 blc_offset_gain_b;
	uint32_t m;

	//blc_snr_bit = READ_CMOD_REG(mod_info, BLC_SNR_BIT);
	uint32_t sensor_bit_depth =
		READ_VIRT_REG(mod_info, VSYS_SENSOR_BIT_DEPTH);
	sensor_bit_depth =
		sensor_bit_depth > 12 ?
		12 :
		sensor_bit_depth; // max 12 bits for linear mode

	uint16_t blc_offset_r = READ_REG(mod_info, BLC_OFFSET_R);
	uint16_t blc_offset_g1 = READ_REG(mod_info, BLC_OFFSET_G1);
	uint16_t blc_offset_g2 = READ_REG(mod_info, BLC_OFFSET_G2);
	uint16_t blc_offset_b = READ_REG(mod_info, BLC_OFFSET_B);

	m = (1 << (12 - sensor_bit_depth)) - 1;

	blc_offset_gain_r = get_blc_gain(blc_offset_r, m);
	blc_offset_gain_g1 = get_blc_gain(blc_offset_g1, m);
	blc_offset_gain_g2 = get_blc_gain(blc_offset_g2, m);
	blc_offset_gain_b = get_blc_gain(blc_offset_b, m);

	WRITE_REG(mod_info, BLC_OFFSET_GAIN_R, blc_offset_gain_r);
	WRITE_REG(mod_info, BLC_OFFSET_GAIN_G1, blc_offset_gain_g1);
	WRITE_REG(mod_info, BLC_OFFSET_GAIN_G2, blc_offset_gain_g2);
	WRITE_REG(mod_info, BLC_OFFSET_GAIN_B, blc_offset_gain_b);
}

static int isp_hw_blc_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_offset_gain(mod_info);

	return 0;
}

static int isp_hw_blc_estimation(uint32_t isp_id)
{
	int i;
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	for (i = 0; i < 2; i++) {
		info.chan_offset = REG_OFFSET(BLC, i);
		ret = isp_hw_blc_est_dynamic(&info);
		if (ret) {
			return ret;
		}
		isp_write_reg(BLC_REG_LOAD_MASK, BLC_SYNC + REG_OFFSET(BLC, i));
	}

	return RTS_ISP_OK;
}

static int isp_hw_blc_set_vreg(uint32_t isp_id, int bits)
{
	isp_write_vreg(bits, VSYS_SENSOR_BIT_DEPTH);

	return RTS_ISP_OK;
}

static int isp_hw_blc_info_set(uint32_t isp_id,
							   const isp_iq_blc_v1_dyn_item_t *blc)
{
	int i;

	if (!blc) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < 2; i++) {
		isp_write_reg(blc->r[i], BLC_BLC_OFFSET_R + REG_OFFSET(BLC, i));
		isp_write_reg(blc->gr[i],
					  BLC_BLC_OFFSET_G1 + REG_OFFSET(BLC, i));
		isp_write_reg(blc->gb[i],
					  BLC_BLC_OFFSET_G2 + REG_OFFSET(BLC, i));
		isp_write_reg(blc->b[i], BLC_BLC_OFFSET_B + REG_OFFSET(BLC, i));
	}

	return RTS_ISP_OK;
}

static int isp_hw_blc_position_get(uint32_t isp_id, int32_t *blc_pos)
{
	int reg_val;

	reg_val = isp_read_reg(SYS_POS_SEL) & BLC_L_POS_SEL_MASK;
	if (reg_val) {
		*blc_pos = BLC_BEFORE_RNR;
	} else {
		*blc_pos = BLC_AFTER_RNR;
	}

	return RTS_ISP_OK;
}

static const struct isp_hw_blc_ops hw_blc_ops = {
	.est_dynamic = isp_hw_blc_estimation,
	.set_bit_depth = isp_hw_blc_set_vreg,
	.set = isp_hw_blc_info_set,
	.get_position = isp_hw_blc_position_get,
};

const struct isp_hw_blc_ops *isp_hw_blc_get_ops(void)
{
	return &hw_blc_ops;
}
