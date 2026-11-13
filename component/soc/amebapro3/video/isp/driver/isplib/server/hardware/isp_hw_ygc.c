/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 * Copyright (C) 2019 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <math.h>
#include <isp_reg_ygc.h>
#include "isp_hw_ygc.h"

#define MOD_NAME YGC

static void isp_hw_contrast_in_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t contrast_in_offset_black, contrast_in_offset_white;
	uint32_t contrast_in_slope;

	contrast_in_offset_black = READ_REG(mod_info, CONTRAST_IN_OFFSET_BLACK);
	contrast_in_offset_white = READ_REG(mod_info, CONTRAST_IN_OFFSET_WHITE);

	contrast_in_slope = (uint32_t)(ceilf(
									   (float)(contrast_in_offset_black + contrast_in_offset_white) *
									   4096.0 /
									   (float)(4095 - contrast_in_offset_black -
											   contrast_in_offset_white)));

	WRITE_REG(mod_info, CONTRAST_IN_SLOPE, contrast_in_slope);
}

static int isp_hw_ygc_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_contrast_in_slope(mod_info);
	return 0;
}

static int isp_hw_ygc_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_ygc_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(YGC_REG_LOAD_MASK, YGC_REG_LOAD);

	return RTS_ISP_OK;
}

static int isp_hw_set_ygc(uint32_t isp_id, const isp_iq_ygc_v1_dyn_item_t *ygc)
{
	int i;
	uint32_t offset;

	if (!ygc || ygc->curve_count != 63) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0, offset = YGC_CONTRAST_CURVE_VALS_0;
		 i < ygc->curve_count - 1; i++, offset += 4) {
		isp_write_reg(ygc->curve[i], offset);
	}

	isp_write_reg(ygc->offsets[0], YGC_CONTRAST_IN_OFFSET_BLACK);
	isp_write_reg(ygc->offsets[1], YGC_CONTRAST_IN_OFFSET_WHITE);
	isp_write_reg(CONTRAST_LEVEL_ADJ_EN_MASK, YGC_CONTRAST_LEVEL_ADJ_EN);

	return RTS_ISP_OK;
}

static const struct isp_hw_ygc_ops hw_ygc_ops = {
	.est_dynamic = isp_hw_ygc_estimation,
	.set_ygc = isp_hw_set_ygc,
};

const struct isp_hw_ygc_ops *isp_hw_ygc_get_ops(void)
{
	return &hw_ygc_ops;
}
