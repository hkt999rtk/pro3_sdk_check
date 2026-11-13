/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_reg_sample.h>
#include "isp_hw_sample.h"

static const struct isp_hw_sample_ops hw_sample_ops = {};

const struct isp_hw_sample_ops *isp_hw_sample_get_ops(void)
{
	return &hw_sample_ops;
}
