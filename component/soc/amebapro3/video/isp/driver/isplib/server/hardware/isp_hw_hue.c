/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_utils.h>
#include <isp_reg_hue.h>
#include "isp_hw_hue.h"

static int isp_hw_set_hue(uint32_t isp_id, int hue)
{
	float angle;
	uint8_t hue_val[4];

	angle = linear_intp_float(hue, -50, -3.14, 50, 3.14);
	hue_val[0] = (int)(cosf(angle) * 64);
	hue_val[1] = (int)(sinf(angle) * 64);
	hue_val[2] = hue_val[1];
	hue_val[3] = hue_val[0];

	isp_write_reg((hue_val[0] << HUE_COS_X_OFFSET |
				   hue_val[1] << HUE_SIN_Y_OFFSET |
				   hue_val[2] << HUE_SIN_X_OFFSET |
				   hue_val[3] << HUE_COS_Y_OFFSET),
				  HUE_PARAM);

	return RTS_ISP_OK;
}

static const struct isp_hw_hue_ops hw_hue_ops = {
	.set_hue = isp_hw_set_hue,
};

const struct isp_hw_hue_ops *isp_hw_hue_get_ops(void)
{
	return &hw_hue_ops;
}
