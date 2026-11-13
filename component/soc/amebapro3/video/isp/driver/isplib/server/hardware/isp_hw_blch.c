/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2024 Kelsey Chen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <rts_isp.h>
#include <isp_reg_virtual.h>
#include <isp_reg_isp_sys.h>
#include <isp_reg_blch.h>
#include "isp_hw_blc.h"

#define MOD_NAME BLCH

static void isp_hw_offset_gain(FW_EST_MOD_INFO mod_info)
{
	// =====================================================================
	// 12bits mode process (output with 12bits clip)
	// =====================================================================
	// input: raw 10to12-bit
	//				(3 + offset) x 2^16
	//		 gain = ceil( ----------------------- )
	//				(1023 x 4) - offset
	//
	// input: raw 12
	//				offset x 2^16
	//		 gain = ceil( ----------------- )
	//				4095 - offset
	// =====================================================================
	// 16bits mode process (output with 16bits clip)
	// =====================================================================
	// input: raw 14to16
	//				(3 + offset) x 2^16
	//		  gain = ceil( --------------------- )
	//				(16383x4) - offset
	//
	// input: raw 16
	//				 offset x 2^16
	//		  gain = ceil( ------------------ )
	//				 65535 - offset
	// =====================================================================

	const uint32_t gain_lsb = 16;

	int blch_mode = READ_REG(
						mod_info, BLCH_MODE); /* 0: 12bits process, 1: 16bits process*/
	uint32_t sensor_bit_depth =
		READ_VIRT_REG(mod_info, VSYS_SENSOR_BIT_DEPTH);

	uint32_t pipeline_dynrange_max = 0;
	uint32_t m = 0;
	uint16_t blch_offset_r = 0, blch_offset_g1 = 0, blch_offset_g2 = 0,
			 blch_offset_b = 0;
	uint32_t blch_offset_gain_r = 0, blch_offset_gain_g1 = 0,
			 blch_offset_gain_g2 = 0, blch_offset_gain_b = 0;

	if (blch_mode) {
		/*16bits process*/
		pipeline_dynrange_max = 65535;
		sensor_bit_depth =
			sensor_bit_depth <= 12 ?
			14 :
			sensor_bit_depth; // 14bits and 16bits only
		m = (1 << (16 - sensor_bit_depth)) - 1;
	} else {
		/*12bits process*/
		pipeline_dynrange_max = 4095;
		sensor_bit_depth = sensor_bit_depth > 12 ?
						   12 :
						   sensor_bit_depth; // max 12 bits
		m = (1 << (12 - sensor_bit_depth)) - 1;
	}

	blch_offset_r = READ_REG(mod_info, BLCH_OFFSET_R);
	blch_offset_g1 = READ_REG(mod_info, BLCH_OFFSET_G1);
	blch_offset_g2 = READ_REG(mod_info, BLCH_OFFSET_G2);
	blch_offset_b = READ_REG(mod_info, BLCH_OFFSET_B);

	blch_offset_gain_r = (uint32_t)ceilf(
							 (float)((m + blch_offset_r) * (1 << gain_lsb)) /
							 (float)(pipeline_dynrange_max - m - blch_offset_r));
	blch_offset_gain_g1 = (uint32_t)ceilf(
							  (float)((m + blch_offset_g1) * (1 << gain_lsb)) /
							  (float)(pipeline_dynrange_max - m - blch_offset_g1));
	blch_offset_gain_g2 = (uint32_t)ceilf(
							  (float)((m + blch_offset_g2) * (1 << gain_lsb)) /
							  (float)(pipeline_dynrange_max - m - blch_offset_g2));
	blch_offset_gain_b = (uint32_t)ceilf(
							 (float)((m + blch_offset_b) * (1 << gain_lsb)) /
							 (float)(pipeline_dynrange_max - m - blch_offset_b));

	WRITE_REG(mod_info, BLCH_OFFSET_GAIN_R, blch_offset_gain_r);
	WRITE_REG(mod_info, BLCH_OFFSET_GAIN_G1, blch_offset_gain_g1);
	WRITE_REG(mod_info, BLCH_OFFSET_GAIN_G2, blch_offset_gain_g2);
	WRITE_REG(mod_info, BLCH_OFFSET_GAIN_B, blch_offset_gain_b);
}

static int isp_hw_blch_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_offset_gain(mod_info);

	return 0;
}

static int isp_hw_blch_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_blch_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(BLCH_REG_LOAD_MASK, BLCH_BLCH_SYNC);

	return RTS_ISP_OK;
}

static int isp_hw_blch_set_vreg(uint32_t isp_id, int bits)
{
	isp_write_vreg(bits, VSYS_SENSOR_BIT_DEPTH);

	return RTS_ISP_OK;
}

static int isp_hw_blch_info_set(uint32_t isp_id,
								const isp_iq_blc_v1_dyn_item_t *blc)
{
	if (!blc) {
		return -RTS_ISP_EINVAL;
	}

	isp_write_reg(blc->r[0], BLCH_BLCH_OFFSET_R);
	isp_write_reg(blc->gr[0], BLCH_BLCH_OFFSET_G1);
	isp_write_reg(blc->gb[0], BLCH_BLCH_OFFSET_G2);
	isp_write_reg(blc->b[0], BLCH_BLCH_OFFSET_B);

	return RTS_ISP_OK;
}

static int isp_hw_blch_get_position(uint32_t isp_id, int32_t *blc_pos)
{
	*blc_pos = BLC_AFTER_DECOMP;

	return RTS_ISP_OK;
}

static const struct isp_hw_blc_ops hw_blch_ops = {
	.est_dynamic = isp_hw_blch_estimation,
	.set_bit_depth = isp_hw_blch_set_vreg,
	.set = isp_hw_blch_info_set,
	.get_position = isp_hw_blch_get_position,
};

const struct isp_hw_blc_ops *isp_hw_blch_get_ops(void)
{
	return &hw_blch_ops;
}
