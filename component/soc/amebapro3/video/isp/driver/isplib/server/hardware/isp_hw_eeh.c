/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Kelsey Chen <kelsey_chen@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_reg_virtual.h>
#include <isp_reg_eeh.h>
#include <isp_reg_isp_sys.h>
#include "isp_hw_eeh.h"

#define MOD_NAME EEH

#define LUMA_CNT 5
#define HUE_CNT 8

static int isp_hw_eeh_init(uint32_t isp_id)
{
	isp_write_reg(1, EEH_MI_EN);

	return RTS_ISP_OK;
}

static int isp_hw_eeh_init_vreg(uint32_t isp_id)
{
	isp_write_vreg(0xCC, VEEH_M_SUPPRESS_UP);
	isp_write_vreg(0xCC, VEEH_M_SUPPRESS_LOW);
	isp_write_vreg(0xA0, VEEH_MID_SH_RATE0_W);
	isp_write_vreg(0xA0, VEEH_MID_SH_RATE1_W);
	isp_write_vreg(0x100, VEEH_MID_SH_RATE2_W);
	isp_write_vreg(0x100, VEEH_MID_SH_RATE3_W);
	isp_write_vreg(0xA0, VEEH_MID_HIGH_SH_RATE0_W);
	isp_write_vreg(0xA0, VEEH_MID_HIGH_SH_RATE1_W);
	isp_write_vreg(0x100, VEEH_MID_HIGH_SH_RATE2_W);
	isp_write_vreg(0x100, VEEH_MID_HIGH_SH_RATE3_W);
	isp_write_vreg(0xA0, VEEH_HIGH_SH_RATE0_W);
	isp_write_vreg(0xA0, VEEH_HIGH_SH_RATE1_W);
	isp_write_vreg(0x100, VEEH_HIGH_SH_RATE2_W);
	isp_write_vreg(0x100, VEEH_HIGH_SH_RATE3_W);
	isp_write_vreg(0xA0, VEEH_MID_SH_RATE0_B);
	isp_write_vreg(0xA0, VEEH_MID_SH_RATE1_B);
	isp_write_vreg(0x100, VEEH_MID_SH_RATE2_B);
	isp_write_vreg(0x100, VEEH_MID_SH_RATE3_B);
	isp_write_vreg(0xA0, VEEH_MID_HIGH_SH_RATE0_B);
	isp_write_vreg(0xA0, VEEH_MID_HIGH_SH_RATE1_B);
	isp_write_vreg(0x100, VEEH_MID_HIGH_SH_RATE2_B);
	isp_write_vreg(0x100, VEEH_MID_HIGH_SH_RATE3_B);
	isp_write_vreg(0xA0, VEEH_HIGH_SH_RATE0_B);
	isp_write_vreg(0xA0, VEEH_HIGH_SH_RATE1_B);
	isp_write_vreg(0x100, VEEH_HIGH_SH_RATE2_B);
	isp_write_vreg(0x100, VEEH_HIGH_SH_RATE3_B);
	isp_write_vreg(SHARPNESS_CTRL_DEF, VSHARPNESS);

	return RTS_ISP_OK;
}

static int isp_hw_eeh_est(uint32_t isp_id)
{
	isp_write_reg(1, EEH_REG_SYNC);

	return RTS_ISP_OK;
}

static void isp_hw_eeh_set_sharpness(FW_EST_MOD_INFO mod_info, int sharpness)
{
	const int val_def = READ_VIRT_REG(mod_info, VSHARPNESS);
	float suppress_up =
		READ_VIRT_REG(mod_info, VEEH_M_SUPPRESS_UP) / 1024.0f;
	float suppress_low =
		READ_VIRT_REG(mod_info, VEEH_M_SUPPRESS_LOW) / 1024.0f;
	int rate_mid_w[4] = { READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE0_W),
						  READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE1_W),
						  READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE2_W),
						  READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE3_W)
						};
	int rate_mid_high_w[4] = {
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE0_W),
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE1_W),
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE2_W),
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE3_W)
	};
	int rate_high_w[4] = { READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE0_W),
						   READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE1_W),
						   READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE2_W),
						   READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE3_W)
						 };

	int rate_mid_b[4] = { READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE0_B),
						  READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE1_B),
						  READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE2_B),
						  READ_VIRT_REG(mod_info, VEEH_MID_SH_RATE3_B)
						};
	int rate_mid_high_b[4] = {
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE0_B),
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE1_B),
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE2_B),
		READ_VIRT_REG(mod_info, VEEH_MID_HIGH_SH_RATE3_B)
	};
	int rate_high_b[4] = { READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE0_B),
						   READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE1_B),
						   READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE2_B),
						   READ_VIRT_REG(mod_info, VEEH_HIGH_SH_RATE3_B)
						 };

	uint32_t m_suppress_up, m_suppress_low;
	uint32_t rate_result;
	uint8_t i;

	if (sharpness < SHARPNESS_CTRL_DEF)
		sharpness = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
								SHARPNESS_CTRL_MIN, SHARPNESS_CTRL_DEF,
								val_def);
	else
		sharpness = linear_intp(sharpness, SHARPNESS_CTRL_DEF, val_def,
								SHARPNESS_CTRL_MAX, SHARPNESS_CTRL_MAX);

	if (sharpness < 50) {
		suppress_up = linear_intp_float(sharpness, SHARPNESS_CTRL_MIN,
										1, SHARPNESS_CTRL_DEF,
										suppress_up);
		suppress_low = linear_intp_float(sharpness, SHARPNESS_CTRL_MIN,
										 1, SHARPNESS_CTRL_DEF,
										 suppress_low);

		for (i = 0; i < 4; i++) {
			/* Set white sharp rate */
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
									  0, SHARPNESS_CTRL_DEF,
									  rate_mid_w[i]);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_SHARP_RATE_W, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
									  0, SHARPNESS_CTRL_DEF,
									  rate_mid_high_w[i]);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_HIGH_SHARP_RATE_W, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
									  0, SHARPNESS_CTRL_DEF,
									  rate_high_w[i]);
			WRITE_REG_ARRAY_I(mod_info, Y_HIGH_SHARP_RATE_W, i,
							  rate_result);

			/* Set black sharp rate */
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
									  0, SHARPNESS_CTRL_DEF,
									  rate_mid_b[i]);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_SHARP_RATE_B, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
									  0, SHARPNESS_CTRL_DEF,
									  rate_mid_high_b[i]);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_HIGH_SHARP_RATE_B, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_MIN,
									  0, SHARPNESS_CTRL_DEF,
									  rate_high_b[i]);
			WRITE_REG_ARRAY_I(mod_info, Y_HIGH_SHARP_RATE_B, i,
							  rate_result);
		}
	} else {
		suppress_up =
			linear_intp_float(sharpness, 50, suppress_up, 100, 0);
		suppress_low =
			linear_intp_float(sharpness, 50, suppress_low, 100, 0);

		for (i = 0; i < 4; i++) {
			/* Set white sharp rate */
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_DEF,
									  rate_mid_w[i],
									  SHARPNESS_CTRL_MAX, 0x200);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_SHARP_RATE_W, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_DEF,
									  rate_mid_w[i],
									  SHARPNESS_CTRL_MAX, 0x200);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_HIGH_SHARP_RATE_W, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_DEF,
									  rate_mid_w[i],
									  SHARPNESS_CTRL_MAX, 0x200);
			WRITE_REG_ARRAY_I(mod_info, Y_HIGH_SHARP_RATE_W, i,
							  rate_result);

			/* Set black sharp rate */
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_DEF,
									  rate_mid_b[i],
									  SHARPNESS_CTRL_MAX, 0x200);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_SHARP_RATE_B, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_DEF,
									  rate_mid_high_b[i],
									  SHARPNESS_CTRL_MAX, 0x200);
			WRITE_REG_ARRAY_I(mod_info, Y_MID_HIGH_SHARP_RATE_B, i,
							  rate_result);
			rate_result = linear_intp(sharpness, SHARPNESS_CTRL_DEF,
									  rate_high_b[i],
									  SHARPNESS_CTRL_MAX, 0x200);
			WRITE_REG_ARRAY_I(mod_info, Y_HIGH_SHARP_RATE_B, i,
							  rate_result);
		}
	}

	m_suppress_up = clamp_t(uint16_t, ((suppress_up * 64.0) + 0.5), 0, 63);
	m_suppress_low =
		clamp_t(uint16_t, ((suppress_low * 64.0) + 0.5), 0, 63);

	WRITE_REG(mod_info, Y_M_SUPPRESS_UPPER, m_suppress_up);
	WRITE_REG(mod_info, Y_M_SUPPRESS_LOWER, m_suppress_low);
}

static int isp_hw_eeh_set_sharpness_value(uint32_t isp_id, int sharpness)
{
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;
	isp_hw_eeh_set_sharpness(&info, sharpness);

	return RTS_ISP_OK;
}

static int isp_hw_eeh_position_get(uint32_t isp_id)
{
	return isp_read_reg(SYS_POS_SEL) & EEH_POS_SEL_MASK;
}

static inline void hw_eeh_intp_point(isp_iq_eeh_v1_point_t *pts,
									 const isp_iq_eeh_v1_point_t *pts0,
									 const isp_iq_eeh_v1_point_t *pts1)
{
	pts->ratio = (pts0->ratio + pts1->ratio) / 2;
	pts->thd0 = (pts0->thd0 + pts1->thd0) / 2;
	pts->thd1 = (pts0->thd1 + pts1->thd1) / 2;
}

static inline void hw_eeh_write_point(uint32_t isp_id, uint32_t offset,
									  isp_iq_eeh_v1_point_t *pts)
{
	int32_t delta;
	uint32_t slope;

	offset *= 4;
	delta = pts->thd0 >= pts->thd1 ? 1 : pts->thd1 - pts->thd0;
	slope = (pts->ratio * 256 - 256 * 256 + delta - 1) / delta;

	isp_write_reg(pts->thd0, EEH_EDGE_MAG_UV_RATIO_THD0_0 + offset);
	isp_write_reg(pts->thd1, EEH_EDGE_MAG_UV_RATIO_THD1_0 + offset);
	isp_write_reg(slope, EEH_EDGE_MAG_UV_RATIO_SLOPE0_0 + offset);
}

static int isp_hw_eeh_set_dyn_item(uint32_t isp_id,
								   const isp_iq_eeh_v1_dyn_item_t *item)
{
	int luma_idx;
	int hue_idx;
	uint32_t offset;
	const isp_iq_eeh_v1_point_t(*ptsp)[HUE_CNT];
	isp_iq_eeh_v1_point_t pts;
	isp_iq_eeh_v1_point_t pts0;
	isp_iq_eeh_v1_point_t pts1;

	if (item->pts_count != LUMA_CNT * HUE_CNT) {
		return -RTS_ISP_EINVAL;
	}

	ptsp = (void *)item->pts;

	/*
	 * c h c h c h c h c h c h c h c h
	 * v d v d v d v d v d v d v d v d
	 * c h c h c h c h c h c h c h c h
	 * v d v d v d v d v d v d v d v d
	 * c h c h c h c h c h c h c h c h
	 * v d v d v d v d v d v d v d v d
	 * c h c h c h c h c h c h c h c h
	 * v d v d v d v d v d v d v d v d
	 * c h c h c h c h c h c h c h c h
	 */
	for (luma_idx = 0; luma_idx < LUMA_CNT; luma_idx++) {
		for (hue_idx = 0; hue_idx < HUE_CNT; hue_idx++) {
			offset = luma_idx * 2 * HUE_CNT * 2 + hue_idx * 2;
			/* c */
			pts = ptsp[luma_idx][hue_idx];
			hw_eeh_write_point(isp_id, offset, &pts);
			/* h */
			hw_eeh_intp_point(
				&pts0, &ptsp[luma_idx][hue_idx],
				&ptsp[luma_idx][(hue_idx + 1) % HUE_CNT]);
			hw_eeh_write_point(isp_id, offset + 1, &pts0);

			if (luma_idx == LUMA_CNT - 1) {
				continue;
			}

			/* v */
			hw_eeh_intp_point(&pts, &ptsp[luma_idx][hue_idx],
							  &ptsp[luma_idx + 1][hue_idx]);
			hw_eeh_write_point(isp_id, offset + HUE_CNT * 2, &pts);
			/* d */
			hw_eeh_intp_point(
				&pts1, &ptsp[luma_idx + 1][hue_idx],
				&ptsp[luma_idx + 1][(hue_idx + 1) % HUE_CNT]);
			hw_eeh_intp_point(&pts, &pts0, &pts1);
			hw_eeh_write_point(isp_id, offset + HUE_CNT * 2 + 1,
							   &pts);
		}
	}

	return 0;
}

static const struct isp_hw_eeh_ops hw_eeh_ops = {
	.init = isp_hw_eeh_init,
	.init_vreg = isp_hw_eeh_init_vreg,
	.estimation = isp_hw_eeh_est,
	.set_sharpness = isp_hw_eeh_set_sharpness_value,
	.get_position = isp_hw_eeh_position_get,
	.set_dyn_item = isp_hw_eeh_set_dyn_item,
};

const struct isp_hw_eeh_ops *isp_hw_eeh_get_ops(void)
{
	return &hw_eeh_ops;
}
