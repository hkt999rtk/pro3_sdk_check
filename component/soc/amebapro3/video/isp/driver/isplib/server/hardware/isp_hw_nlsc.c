/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_reg_nlsc.h>
#include "isp_hw_nlsc.h"

#define NLSC_CURVE_COUNT 40

#define MOD_NAME NLSC

#define clip(x, a, b) \
	((x) < (b) ? (b) : ((x) > (a) ? (a) : (x))) // clip(x, upper, lower)

typedef struct {
	int x;
	int y;
} ISP_POINT;

static int isp_hw_nlsc_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	const int MAX_EFFRNG_DST = (1 << 13) - 1;
	const int MIN_EFFRNG_DST = 0;
	const int MAX_EFFRNG_ERR = (1 << 15) - 1;
	const int MIN_EFFRNG_ERR = -1 * (1 << 15);

	ISP_POINT center_r;
	ISP_POINT center_g;
	ISP_POINT center_b;

	center_r.x = READ_REG(mod_info, CENTER_R_X);
	center_r.y = READ_REG(mod_info, CENTER_R_Y);
	center_g.x = READ_REG(mod_info, CENTER_G_X);
	center_g.y = READ_REG(mod_info, CENTER_G_Y);
	center_b.x = READ_REG(mod_info, CENTER_B_X);
	center_b.y = READ_REG(mod_info, CENTER_B_Y);

	// From FW estimation
	uint32_t g_square, b_square, r_square;
	uint16_t g_sqr, b_sqr, r_sqr;

	g_square = (center_g.x) * (center_g.x) + (center_g.y) * (center_g.y);
	g_sqr = (uint16_t)sqrt(g_square);
	b_square = (center_b.x) * (center_b.x) + (center_b.y) * (center_b.y);
	b_sqr = (uint16_t)sqrt(b_square);
	r_square = (center_r.x) * (center_r.x) + (center_r.y) * (center_r.y);
	r_sqr = (uint16_t)sqrt(r_square);

	// for general resolution
	int is_err_g = clip((int)g_sqr * (int)g_sqr - (int)g_square,
						MAX_EFFRNG_ERR, MIN_EFFRNG_ERR);
	int is_err_b = clip((int)b_sqr * (int)b_sqr - (int)b_square,
						MAX_EFFRNG_ERR, MIN_EFFRNG_ERR);
	int is_err_r = clip((int)r_sqr * (int)r_sqr - (int)r_square,
						MAX_EFFRNG_ERR, MIN_EFFRNG_ERR);

	int is_dst_g = clip(g_sqr, MAX_EFFRNG_DST, MIN_EFFRNG_DST);
	int is_dst_b = clip(b_sqr, MAX_EFFRNG_DST, MIN_EFFRNG_DST);
	int is_dst_r = clip(r_sqr, MAX_EFFRNG_DST, MIN_EFFRNG_DST);

	WRITE_REG(mod_info, IS_ERR_G, is_err_g);
	WRITE_REG(mod_info, IS_ERR_B, is_err_b);
	WRITE_REG(mod_info, IS_ERR_R, is_err_r);
	WRITE_REG(mod_info, IS_DST_G, is_dst_g);
	WRITE_REG(mod_info, IS_DST_B, is_dst_b);
	WRITE_REG(mod_info, IS_DST_R, is_dst_r);

	return 0;
}

static int isp_hw_nlsc_center_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_nlsc_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(1, NLSC_CENTER_RGB_CTL);

	return RTS_ISP_OK;
}

static inline int get_step_reg_value(uint32_t step)
{
	switch (step) {
	case 32:
		return 0;
	case 64:
		return 1;
	case 128:
		return 2;
	default:
		return 0;
	}
}

static int hw_set_nlsc_curve(uint32_t isp_id,
							 const isp_iq_nlsc_v1_common_item_t *nlsc)
{
	int i;

	if (nlsc->r_curve_count != NLSC_CURVE_COUNT ||
		nlsc->g_curve_count != NLSC_CURVE_COUNT ||
		nlsc->b_curve_count != NLSC_CURVE_COUNT) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < nlsc->r_curve_count; i++) {
		isp_write_reg(nlsc->r_curve[i], NLSC_CIRCLE_R_GAIN_0 + i * 4);
		isp_write_reg(nlsc->g_curve[i], NLSC_CIRCLE_G_GAIN_0 + i * 4);
		isp_write_reg(nlsc->b_curve[i], NLSC_CIRCLE_B_GAIN_0 + i * 4);
	}
	isp_write_reg(get_step_reg_value(nlsc->step), NLSC_STEP);
	isp_write_reg(1, NLSC_ADJRATE_R_EN);
	isp_write_reg(1, NLSC_ADJRATE_G_EN);
	isp_write_reg(1, NLSC_ADJRATE_B_EN);

	return RTS_ISP_OK;
}

static int isp_hw_nlsc_set_nlsc(uint32_t isp_id,
								const isp_iq_nlsc_v1_common_item_t *nlsc)
{
	if (!nlsc) {
		return -RTS_ISP_EINVAL;
	}
	return hw_set_nlsc_curve(isp_id, nlsc);
}

static int isp_hw_nlsc_set_center(uint32_t isp_id,
								  const struct isp_hw_nlsc_center *center)
{
	isp_write_reg(center->r.x, NLSC_CENTER_R_X);
	isp_write_reg(center->r.y, NLSC_CENTER_R_Y);
	isp_write_reg(center->g.x, NLSC_CENTER_G_X);
	isp_write_reg(center->g.y, NLSC_CENTER_G_Y);
	isp_write_reg(center->b.x, NLSC_CENTER_B_X);
	isp_write_reg(center->b.y, NLSC_CENTER_B_Y);

	return isp_hw_nlsc_center_estimation(isp_id);
}

static int isp_hw_nlsc_set_adj(uint32_t isp_id,
							   const isp_iq_nlsc_v1_dyn_item_t *adj)
{
	if (!adj) {
		return -RTS_ISP_EINVAL;
	}
	isp_write_reg(1, NLSC_ADJ_STEP_R);
	isp_write_reg(1, NLSC_ADJ_STEP_G);
	isp_write_reg(1, NLSC_ADJ_STEP_B);
	isp_write_reg(adj->r, NLSC_ADJRATE_R);
	isp_write_reg(adj->g, NLSC_ADJRATE_G);
	isp_write_reg(adj->b, NLSC_ADJRATE_B);
	isp_write_reg(NLSC_ADJ_TRIG_MASK | 1 << NLSC_ADJ_MODE_OFFSET,
				  NLSC_NLSC_ADJ_MODE);
	return RTS_ISP_OK;
}

static const struct isp_hw_nlsc_ops hw_nlsc_ops = {
	.set_nlsc = isp_hw_nlsc_set_nlsc,
	.set_center = isp_hw_nlsc_set_center,
	.set_adj = isp_hw_nlsc_set_adj,
};

const struct isp_hw_nlsc_ops *isp_hw_nlsc_get_ops(void)
{
	return &hw_nlsc_ops;
}
