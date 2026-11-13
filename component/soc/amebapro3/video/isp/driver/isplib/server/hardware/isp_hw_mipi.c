/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_driver.h>
#include <isp_utils.h>
#include <isp_reg_mipi.h>
#include <isp_reg_y2y.h>
#include "isp_hw_interface.h"

static int
get_mipi_data_type(int type, int bit_depth,
				   const struct rts_isp_sensor_type_config type_config)
{
	if (type == YUV_SENSOR) {
		if (type_config.yuv.type == SNR_YUV422) {
			return 0x1E;
		} else if (type_config.yuv.type == SNR_YUV420_LEGACY) {
			return 0x1A;
		} else if (type_config.yuv.type == SNR_YUV420) {
			return 0x18;
		} else {
			return 0x1C;
		}
	} else if (type == RAW_SENSOR) {
		if (bit_depth == 8) {
			return 0x2A;
		} else if (bit_depth == 10) {
			return 0x2B;
		} else if (bit_depth == 12) {
			return 0x2C;
		} else {
			return 0x2E;    // RAW16
		}
	}
	return -RTS_ISP_EINVAL;
}

static int
get_mipi_data_fmt(int type, int bit_depth,
				  const struct rts_isp_sensor_type_config type_config)
{
	if (type == YUV_SENSOR) {
		if (type_config.yuv.type == SNR_YUV422) {
			return 0x02 | type_config.yuv.order.yuv422 << 8;
		} else if (type_config.yuv.type == SNR_YUV420_LEGACY) {
			return 0x5;
		} else {
			return 0x6;
		}
	} else if (type == RAW_SENSOR) {
		if (bit_depth == 8) {
			return 0x00;
		} else if (bit_depth == 10) {
			return 0x01;
		} else if (bit_depth == 12) {
			return 0x04;
		} else {
			return 0x02;    // RAW16
		}
	}
	return -RTS_ISP_EINVAL;
}

static int get_mipi_rx_pos(uint32_t pclk, uint32_t mipi_clk, int lanes,
						   int bit_num)
{
	return mipi_clk * 12 / (pclk * bit_num / isp_get_val_weight(lanes) / 4);
}

static int write_aphy_reg(uint16_t addr, uint16_t data)
{
	struct isp_i2c_info_reg i2c;

	i2c.info.addr_len = 2;
	i2c.info.data_len = 1;
	i2c.info.i2c_id = 0x69;
	i2c.reg.addr = addr;
	i2c.reg.data = data;

	return isp_os_driver_i2c_write_reg(&i2c);
}

static int mipi_set_enable(int enable)
{
	return write_aphy_reg(0x0410, !enable);
}

static int mipi_start_extern_aphy(void)
{
	return mipi_set_enable(RTS_ISP_TRUE);
}

static int mipi_stop_extern_aphy(void)
{
	return mipi_set_enable(RTS_ISP_FALSE);
}

static int mipi_config_hdr(uint32_t isp_id,
						   const struct isp_interface_info *info)
{
	switch ((int)info->info->mipi.hdr) {
	case MIPI_HDR_NONE:
	case MIPI_HDR_VC:
		isp_write_reg(0, MIPI_DPHY_INPUT_MODE);
		break;
	case MIPI_HDR_DUMMY:
		isp_write_reg(1, MIPI_DPHY_INPUT_MODE);
		isp_write_reg(info->info->mipi.max_short_exp, MIPI_DPHY_MAX_SRT_EXP);
		isp_write_reg(info->size.h, MIPI_DPHY_STAGGER_FRM_HEIGHT);
		break;
	case MIPI_HDR_NODUMMY:
		isp_write_reg(2, MIPI_DPHY_INPUT_MODE);
		isp_write_reg(info->info->mipi.max_short_exp, MIPI_DPHY_MAX_SRT_EXP);
		isp_write_reg(info->size.h, MIPI_DPHY_STAGGER_FRM_HEIGHT);
		break;
	case MIPI_HDR_FID:
		isp_write_reg(3, MIPI_DPHY_INPUT_MODE);
		break;
	}
	return RTS_ISP_OK;
}

static void mipi_set_yuv420(const struct isp_interface_info *info)
{
	if (info->info->type == YUV_SENSOR &&
		info->info->type_config.yuv.type != SNR_YUV422) {
		isp_write_reg(Y2Y_EN_MASK, Y2Y_CTRL_EN);
		isp_write_reg((info->size.h << FRAME_HEIGHT_OFFSET |
					   info->size.w << FRAME_WIDTH_OFFSET),
					  Y2Y_FRAME_SIZE);
		if (info->info->type_config.yuv.type == SNR_YUV420_LEGACY) {
			isp_write_reg(0, Y2Y_MODE);
		} else {
			isp_write_reg(YUV420_MODE_MASK, Y2Y_MODE);
		}
	}
}

static void isp_hw_mipi_soft_reset(void)
{
}

static void isp_hw_mipi_soft_reset_pre(void)
{
	isp_set_reg_bit(SRST_OFFSET, MIPI_DPHY_LANE_CTRL);
	isp_clr_reg_bit(SRST_OFFSET, MIPI_DPHY_LANE_CTRL);

	isp_write_reg(1, MIPI_DPHY_ECC_CNT_L);
	isp_write_reg(1, MIPI_DPHY_CRC_CNT_L);
	isp_write_reg(1, MIPI_DPHY_MERGE_OVERFLOW_CNT);

	/* write MIPI_START_TRIG to soft reset mipi */
	isp_write_reg(1, MIPI_START_TRIG);
}

static void isp_hw_mipi_set_dphy_lane_en(uint8_t lanes)
{
	if (lanes) {
		isp_write_reg(lanes | CLK_LANE_EN_MASK, MIPI_DPHY_LANE_EN);
	} else {
		isp_write_reg(0, MIPI_DPHY_LANE_EN);
	}
}

static int mipi_init(uint32_t isp_id, const struct isp_interface_info *info)
{
	uint8_t pixel_bits;
	int mipi_data_fmt;
	int mipi_data_type;
	const struct rts_isp_mipi_info *mipi;

	if (!info || info->info->interface != SNR_INTERFACE_MIPI) {
		return -RTS_ISP_EINVAL;
	}

	mipi_data_fmt = get_mipi_data_fmt(info->info->type,
									  info->info->bit_depth,
									  info->info->type_config);
	mipi_data_type = get_mipi_data_type(info->info->type,
										info->info->bit_depth,
										info->info->type_config);
	if (mipi_data_fmt < 0 || mipi_data_type < 0) {
		return -RTS_ISP_EINVAL;
	}

	pixel_bits = info->info->bit_depth;
	if (info->info->type == YUV_SENSOR) {
		pixel_bits *= 2;
	}

	mipi = &info->info->mipi;
	if (mipi->lanes == 0) {
		return -RTS_ISP_EINVAL;
	}

	isp_write_reg_mask(0x2E, MIPI_APHY_REG0, R50_CAL_RG_MASK);
	isp_write_reg_mask(0x0, MIPI_APHY_REG1, EN_R50_AUTO_CAL_RG_MASK);
	isp_write_reg((mipi->data_skew << SDATA0_SKW_RG_OFFSET |
				   mipi->data_skew << SDATA1_SKW_RG_OFFSET),
				  MIPI_APHY_REG7);
	isp_write_reg((mipi->data_skew << SDATA2_SKW_RG_OFFSET |
				   mipi->data_skew << SDATA3_SKW_RG_OFFSET),
				  MIPI_APHY_REG8);
	isp_write_reg((mipi->clk_skew << SCLK0_SKW_RG_OFFSET |
				   mipi->clk_skew << SCLK1_SKW_RG_OFFSET),
				  MIPI_APHY_REG9);
	isp_write_reg((mipi->clk_skew << SCLK2_SKW_RG_OFFSET |
				   mipi->clk_skew << SCLK3_SKW_RG_OFFSET),
				  MIPI_APHY_REG10);
	isp_write_reg(1, MIPI_APHY_REG15);
	isp_write_reg(PIX_DIN_MIPI_SEL_MASK | CRC16_EN_MASK | ECC_EN_MASK,
				  MIPI_DPHY_CFG0);
	isp_write_reg(0, MIPI_DPHY_DIVSEL);
	isp_write_reg(0, MIPI_DPHY_HSCLK_SEL);
	isp_write_reg((mipi->hs_term << HL_INTERVAL_0_OFFSET |
				   mipi->hs_term << HL_INTERVAL_1_OFFSET),
				  MIPI_DPHY_HSTERM0);
	isp_write_reg((mipi->hs_term << HL_INTERVAL_2_OFFSET |
				   mipi->hs_term << HL_INTERVAL_3_OFFSET),
				  MIPI_DPHY_HSTERM1);
	isp_write_reg((mipi_data_fmt << DEC_FORMAT0_OFFSET |
				   (mipi_data_type | 0 << 6) << DEC_ID0_OFFSET),
				  MIPI_DPHY_VC0_DATA_FORMAT);
	isp_write_reg((mipi_data_fmt << DEC_FORMAT1_OFFSET |
				   (mipi_data_type | 1 << 6) << DEC_ID1_OFFSET),
				  MIPI_DPHY_VC1_DATA_FORMAT);
	isp_write_reg(get_mipi_rx_pos(info->pixel_clk, info->interface_clk,
								  mipi->lanes, pixel_bits),
				  MIPI_DPHY_RXPOS_LANE);
	if (info->info->type == RAW_SENSOR) {
		isp_set_reg_bit(TRANS_MODE_SEL_OFFSET, MIPI_DPHY_LANE_CTRL);
	} else {
		isp_clr_reg_bit(TRANS_MODE_SEL_OFFSET, MIPI_DPHY_LANE_CTRL);
	}
	mipi_set_yuv420(info);

	return mipi_config_hdr(isp_id, info);
}

static int isp_hw_mipi_start_transfer(uint32_t isp_id,
									  const struct isp_interface_info *info)
{
	int ret;

	ret = mipi_init(isp_id, info);
	if (ret) {
		return ret;
	}
	if (info->is_fpga) {
		ret = mipi_start_extern_aphy();
		if (ret) {
			return ret;
		}
	}

	isp_write_reg(0, MIPI_APHY_REG27);
	isp_hw_mipi_soft_reset_pre();
	isp_hw_mipi_set_dphy_lane_en(info->info->mipi.lanes);
	isp_write_reg(info->info->mipi.lanes, MIPI_DPHY_PWDB);

	return RTS_ISP_OK;
}

static int isp_hw_mipi_stop_transfer(uint32_t isp_id,
									 const struct isp_interface_info *info)
{
	isp_write_reg(0, MIPI_DPHY_PWDB);
	isp_hw_mipi_set_dphy_lane_en(info->info->mipi.lanes);
	isp_write_reg(1, MIPI_APHY_REG27);

	isp_hw_mipi_soft_reset();

	if (info->is_fpga) {
		int ret;

		ret = mipi_stop_extern_aphy();
		if (ret) {
			return ret;
		}
	}

	return RTS_ISP_OK;
}

static int isp_hw_mipi_set_hdr_delay(uint32_t isp_id, uint32_t short_exp_delay)
{
	isp_write_reg(short_exp_delay / 2, MIPI_DPHY_MAX_SRT_EXP);

	return RTS_ISP_OK;
}

static const struct isp_hw_interface_ops hw_mipi_ops = {
	.start_transfer = isp_hw_mipi_start_transfer,
	.stop_transfer = isp_hw_mipi_stop_transfer,
	.set_hdr_delay = isp_hw_mipi_set_hdr_delay,
};

const struct isp_hw_interface_ops *isp_hw_mipi_get_ops(void)
{
	return &hw_mipi_ops;
}
