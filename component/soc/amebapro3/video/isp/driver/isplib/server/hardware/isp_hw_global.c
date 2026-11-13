/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_reg_isp_sys.h>
#include "isp_hw_global.h"

static int hw_global_get_bypass_bitmap(uint32_t isp_id,
									   struct rts_isp_bypass bypass)
{
	int i;
	uint32_t value = 0;
	static const uint8_t bit_mapping[] = {
		BLC_L_EN_OFFSET,	BLC_S_EN_OFFSET,     DPC_L_EN_OFFSET,
		DPC_S_EN_OFFSET,	TNR_EN_OFFSET,	     DPC_S_EN_OFFSET,
		NLSC_EN_OFFSET,		MLSC_EN_OFFSET,	     AWB_GAIN_EN_OFFSET,
		AE_GAIN_L_EN_OFFSET,	AE_GAIN_S_EN_OFFSET, AE_GAIN_EN_OFFSET,
		CCM_EN_OFFSET,		GAMMA_EN_OFFSET,     HDR_EN_OFFSET,
		YGC_EN_OFFSET,		UVT_EN_OFFSET,	     DITHER_EN_OFFSET,
		LDC_EN_PRE_OFFSET,	EEH_EN_OFFSET,	     LCE_EN_OFFSET,
		SPE_EN_OFFSET,		YGAMMA_EN_OFFSET,    HUE_EN_OFFSET,
		MOT_INFO_SYS_EN_OFFSET, CAC_EN_OFFSET,
	};

	for (i = 0; i < ARRAY_SIZE(bit_mapping); i++)
		if ((1 << i) & bypass.all) {
			isp_set_bit(value, bit_mapping[i]);
		}
	return value;
}

static int isp_hw_global_set_func_enable(uint32_t isp_id, int32_t type,
		int32_t hdr,
		struct rts_isp_bypass bypass)
{
	uint32_t control_value;
	uint32_t clk_ctrl;
	uint32_t bypass_bits;

	if (isp_id >= _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	if (type == RAW_SENSOR) {
		control_value =
			(DPC_L_EN_MASK | DPC_S_EN_MASK | TNR_EN_MASK |
			 AE_GAIN_L_EN_MASK | AE_GAIN_S_EN_MASK | NLSC_EN_MASK |
			 MLSC_EN_MASK | AWB_GAIN_EN_MASK | AE_GAIN_EN_MASK |
			 TM_EN_MASK | CCM_EN_MASK | GAMMA_EN_MASK |
			 YGC_EN_MASK | UVT_EN_MASK | DITHER_EN_MASK |
			 EEH_EN_MASK | LCE_EN_MASK | SPE_EN_MASK | HUE_EN_MASK |
			 YGAMMA_EN_MASK | LDC_EN_PRE_MASK | HDR_EN_MASK |
			 MOT_INFO_SYS_EN_MASK | CAC_EN_MASK);
		if (hdr == RTS_ISP_HDR_DCG_COMBINE ||
			hdr == RTS_ISP_HDR_DCG_COMBINE_TM) {
			control_value |= BLCH_EN_MASK;
		} else {
			control_value |= (BLC_L_EN_MASK | BLC_S_EN_MASK);
		}
		clk_ctrl = (MTD_FUNC_CLK_ON_MASK | HDR_FUNC_CLK_ON_MASK |
					UVT_FUNC_CLK_ON_MASK | LDC_FUNC_CLK_ON_MASK);
	} else {
		control_value =
			(CCM_EN_MASK | GAMMA_EN_MASK | YGC_EN_MASK |
			 UVT_EN_MASK | DITHER_EN_MASK | EEH_EN_MASK |
			 LCE_EN_MASK | SPE_EN_MASK | HUE_EN_MASK |
			 YGAMMA_EN_MASK | LDC_EN_PRE_MASK | HDR_EN_MASK);
		clk_ctrl = (DUMMY_CLK_ON_MASK | MTD_FUNC_CLK_ON_MASK |
					HDR_FUNC_CLK_ON_MASK | UVT_FUNC_CLK_ON_MASK |
					LDC_FUNC_CLK_ON_MASK | YUVSIN_FUNC_CLK_ON_MASK |
					YUV2RGB_FUNC_CLK_ON_MASK);
	}

	bypass_bits = hw_global_get_bypass_bitmap(isp_id, bypass);

	isp_write_reg(~bypass_bits & control_value, SYS_CONTROL1);
	isp_write_reg(clk_ctrl, SYS_ISP_CLK_CTRL);

	return RTS_ISP_OK;
}

static int isp_hw_global_set_isp_path(uint32_t isp_id, int32_t type)
{
	if (type == RAW_SENSOR) {
		isp_write_reg(0, SYS_PATH_SEL);
	} else
		isp_write_reg(IMAGE_SOURCE_MASK | PATH_B_SEL_MASK,
					  SYS_PATH_SEL);
	return RTS_ISP_OK;
}

static int isp_hw_global_enable_data(uint32_t isp_id, int enable)
{
	if (enable) {
		isp_set_reg_bit(ISP_DATA_EN_OFFSET, SYS_DATA_DELAY);
	} else {
		isp_clr_reg_bit(ISP_DATA_EN_OFFSET, SYS_DATA_DELAY);
	}
	return RTS_ISP_OK;
}

static int isp_hw_global_delay_data(uint32_t isp_id, int delay)
{
	if (delay < 0 || delay > ((1 << ISP_DATA_NUM_BITS) - 1)) {
		return -RTS_ISP_EINVAL;
	}
	isp_write_reg(delay, SYS_DATA_DELAY);
	isp_write_reg(ISP_DATA_TRIG_MASK, SYS_DATA_DELAY_TRIG);
	return RTS_ISP_OK;
}

static int isp_hw_global_select_interface(uint32_t isp_id, int32_t inf)
{
	switch (inf) {
	case SNR_INTERFACE_DVP:
		isp_write_reg(0, SYS_INPUT_CTRL);
		break;
	case SNR_INTERFACE_MIPI:
		isp_write_reg(1, SYS_INPUT_CTRL);
		break;
	case SNR_INTERFACE_NONE:
		isp_write_reg(2, SYS_INPUT_CTRL);
		break;
	default:
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int isp_hw_global_start_transfer(uint32_t isp_id)
{
	isp_write_reg(0xffffffff, SYS_INT_CLR0);
	isp_write_reg((DATA_START_OUT_INT_CLR_MASK | FRAME_END_INT_EN_MASK),
				  SYS_INT_EN0);
	isp_write_reg(0xffffffff, SYS_INT_CLR1);
	isp_write_reg(STATIS_DONE_INT_EN_MASK, SYS_INT_EN1);

	isp_write_reg(ISP_START_MASK, SYS_CONTROL0);

	return RTS_ISP_OK;
}

static int isp_hw_global_stop_transfer(uint32_t isp_id)
{
	isp_write_reg(ISP_STOP_MASK, SYS_CONTROL0);

	isp_write_reg(0x0, SYS_INT_EN0);
	isp_write_reg(0xffffffff, SYS_INT_CLR0);
	isp_write_reg(0x0, SYS_INT_EN1);
	isp_write_reg(0xffffffff, SYS_INT_CLR1);

	return RTS_ISP_OK;
}

static int isp_hw_global_set_speed_ctrl(uint32_t isp_id,
										struct isp_speed_ctrl *speed)
{
	const int speed_ctrl = speed->speed_mult << 5 | speed->speed_div;

	isp_write_reg((speed_ctrl << RAW_SPEED_DIV_OFFSET |
				   speed_ctrl << RGB_SPEED_DIV_OFFSET |
				   speed_ctrl << YUV_SPEED_DIV_OFFSET),
				  SYS_SPEED_CTRL);
	isp_write_reg((speed->last_break << RAW_LAST_BREAK_OFFSET |
				   speed->last_break << RGB_LAST_BREAK_OFFSET |
				   speed->last_break << YUV_LAST_BREAK_OFFSET),
				  SYS_LAST_BREAK);

	return RTS_ISP_OK;
}

static int isp_hw_global_get_output_menu(uint32_t isp_id,
		struct isp_output_menu *menu)
{
	static const char *const menu_str[] = {
		"disable raw output",
		"isp in @ long",
		"isp in @ short",
		"crop out @ long",
		"crop out @ short",
		"tpnr out",
		"fusion in @ short",
		"fusion in @ long",
		"fusion in all",
		"fusion out",
		"nlsc in",
		"awb gain out",
		"tm in",
		"gtm out",
		"tm out",
		"intp in",
		"rgb out",
		"yuv out",
		"lce out",
		"eeh out @ yuv422",
		"eeh out @ raw32",
		"mask out",
	};

	menu->num = ARRAY_SIZE(menu_str);
	menu->menu = menu_str;

	return RTS_ISP_OK;
}

static int isp_hw_global_set_output(uint32_t isp_id, uint32_t output)
{
	if (output) {
		isp_write_reg(IMG_SEL_EN_MASK | (output - 1), SYS_IMAGE_SEL1);
	} else {
		isp_write_reg(0, SYS_IMAGE_SEL1);
	}

	return RTS_ISP_OK;
}

static int isp_hw_global_set_line_cnt_int(uint32_t isp_id, uint32_t line_cnt)
{
	isp_write_reg(line_cnt, SYS_LCNT_INT_THD);
	return RTS_ISP_OK;
}

static int isp_hw_global_set_bayer(uint32_t isp_id, int32_t bayer)
{
	isp_write_reg(bayer, ISP_SYS_BAYER_PATTERN);
	return RTS_ISP_OK;
}

static int isp_hw_global_set_tnr_pos(uint32_t isp_id, int32_t hdr)
{
	if (hdr == RTS_ISP_HDR_LINE_2TO1 || hdr == RTS_ISP_HDR_DCG_2TO1) {
		isp_write_reg_mask(1 << TPNR_POS_SEL_OFFSET, SYS_POS_SEL,
						   TPNR_POS_SEL_MASK);
		isp_write_reg_mask(1 << TPNR_DCOMP_EN_OFFSET, SYS_TPNR_COMP,
						   TPNR_DCOMP_EN_MASK);
		isp_write_reg_mask(1 << TPNR_COMP_EN_OFFSET, SYS_TPNR_COMP,
						   TPNR_COMP_EN_MASK);
	} else {
		isp_write_reg_mask(0 << TPNR_POS_SEL_OFFSET, SYS_POS_SEL,
						   TPNR_POS_SEL_MASK);
		isp_write_reg_mask(0 << TPNR_DCOMP_EN_OFFSET, SYS_TPNR_COMP,
						   TPNR_DCOMP_EN_MASK);
		isp_write_reg_mask(0 << TPNR_COMP_EN_OFFSET, SYS_TPNR_COMP,
						   TPNR_COMP_EN_MASK);
	}

	return RTS_ISP_OK;
}

static const struct isp_hw_global_ops hw_global_ops = {
	.set_func_enable = isp_hw_global_set_func_enable,
	.set_isp_path = isp_hw_global_set_isp_path,
	.enable_data = isp_hw_global_enable_data,
	.delay_data = isp_hw_global_delay_data,
	.select_interface = isp_hw_global_select_interface,
	.start_transfer = isp_hw_global_start_transfer,
	.stop_transfer = isp_hw_global_stop_transfer,
	.set_speed_ctrl = isp_hw_global_set_speed_ctrl,
	.get_output_menu = isp_hw_global_get_output_menu,
	.set_output = isp_hw_global_set_output,
	.set_line_cnt_int = isp_hw_global_set_line_cnt_int,
	.set_bayer = isp_hw_global_set_bayer,
	.set_tnr_pos = isp_hw_global_set_tnr_pos,
};

const struct isp_hw_global_ops *isp_hw_global_get_ops(void)
{
	return &hw_global_ops;
}
