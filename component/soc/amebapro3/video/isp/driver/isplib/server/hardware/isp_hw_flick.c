/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_reg_flick.h>
#include "isp_hw_flick.h"

static int isp_hw_flick_set_statis(uint32_t isp_id, const rts_isp_rect_t *rect)
{
	int i;
	uint32_t step_h, step_v;
	uint32_t sample_bits_h, sample_num_h;

	for (i = 10; i > 6; i--) {
		if (rect->w / 2 > (1 << i)) {
			break;
		}
	}
	sample_bits_h = i;
	sample_num_h = 1 << i;
	step_h = rect->w * 16 / sample_num_h;
	step_v = rect->h * 16 / 256;
	for (i = 0; i < 2; i++) {
		isp_write_reg((step_h << FLICK_H_DEC_OFFSET |
					   step_v << FLICK_V_DEC_OFFSET),
					  FLICK_SAMPLE_STEP + FLICK_REG_OFFSET(i));
		isp_write_reg((rounddown(rect->x, 8) << FLICK_START_H_OFFSET |
					   rounddown(rect->y, 8) << FLICK_START_V_OFFSET),
					  FLICK_SAMPLE_START + FLICK_REG_OFFSET(i));
		isp_write_reg_mask(6 << FLICK_SHIFT_BITS_OFFSET,
						   FLICK_FFT_RESULT_CONTRL +
						   FLICK_REG_OFFSET(i),
						   FLICK_SHIFT_BITS_MASK);
		isp_write_reg(sample_bits_h,
					  FLICK_SAMPLE_BITS_H + FLICK_REG_OFFSET(i));
		isp_write_reg(0x10, FLICK_CTRL + FLICK_REG_OFFSET(i));
	}
	return RTS_ISP_OK;
}

static const struct isp_hw_flick_ops hw_flick_ops = {
	.set_statis = isp_hw_flick_set_statis,
};

const struct isp_hw_flick_ops *isp_hw_flick_get_ops(void)
{
	return &hw_flick_ops;
}
