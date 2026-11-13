/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2018 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_reg_ldc_x.h>
#include "isp_hw_ldc.h"

static int isp_hw_ldc_init(uint32_t isp_id)
{
	int i;
	static const uint32_t ldc_params[] = {
		0x0010fbb9, 0x0ffe99fe, 0x000020bd, 0x007ffd07, 0x00098794,
		0x0007c905, 0x059f09ff, 0x0063ee01, 0x001f98c1
	};

	for (i = 0; i < ARRAY_SIZE(ldc_params); i++) {
		isp_write_reg(ldc_params[i], LDC_SCALECOFE0 + i * 4);
	}
	isp_write_reg(LDC_REG_LOAD_MASK, LDC_SYNC);

	return RTS_ISP_OK;
}

static int isp_hw_ldc_set_sync(uint32_t isp_id)
{
	isp_write_reg(LDC_REG_LOAD_MASK, LDC_SYNC);
	return RTS_ISP_OK;
}

static const struct isp_hw_ldc_ops hw_ldc_ops = {
	.init = isp_hw_ldc_init,
	.set_sync = isp_hw_ldc_set_sync,
};

const struct isp_hw_ldc_ops *isp_hw_ldc_get_ops(void)
{
	return &hw_ldc_ops;
}
