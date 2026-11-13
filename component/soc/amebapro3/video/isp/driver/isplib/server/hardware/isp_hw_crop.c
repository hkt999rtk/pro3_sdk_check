/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_reg_crop_l.h>
#include "isp_hw_crop.h"

static int isp_hw_crop_set_crop(uint32_t isp_id, rts_isp_rect_t *rect)
{
	int i;

	if (!rect) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < 2; i++) {
		isp_write_reg(rect->start.x,
					  CROP_START_X + REG_OFFSET(CROP, i));
		isp_write_reg(rect->start.y,
					  CROP_START_Y + REG_OFFSET(CROP, i));
		isp_write_reg(rect->size.w,
					  CROP_FRAME_WIDTH + REG_OFFSET(CROP, i));
		isp_write_reg(rect->size.h,
					  CROP_FRAME_HEIGHT + REG_OFFSET(CROP, i));
		isp_write_reg(CROP_REG_LOAD_MASK,
					  CROP_SYNC + REG_OFFSET(CROP, i));
	}

	return RTS_ISP_OK;
}

static const struct isp_hw_crop_ops hw_crop_ops = {
	.set_crop = isp_hw_crop_set_crop,
};

const struct isp_hw_crop_ops *isp_hw_crop_get_ops(void)
{
	return &hw_crop_ops;
}
