/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_reg_ygamma.h>
#include "isp_hw_ygamma.h"

static int isp_hw_set_ygamma(uint32_t isp_id,
							 const isp_iq_ygamma_v1_dyn_item_t *item)
{
	int i;
	uint32_t offset;

	if (!item || item->curve_count != 16) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0, offset = YGAMMA_PARAM0; i < item->curve_count;
		 i += 4, offset += 4) {
		uint32_t ygamma_val = item->curve[i] | item->curve[i + 1] << 8 |
							  item->curve[i + 2] << 16 |
							  item->curve[i + 3] << 24;
		isp_write_reg(ygamma_val, offset);
	}
	isp_write_reg(YGAMMA_PLOAD_DLY_MASK, YGAMMA_SYNC);

	return RTS_ISP_OK;
}

static const struct isp_hw_ygamma_ops hw_ygamma_ops = {
	.set_ygamma = isp_hw_set_ygamma,
};

const struct isp_hw_ygamma_ops *isp_hw_ygamma_get_ops(void)
{
	return &hw_ygamma_ops;
}
