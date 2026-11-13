/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include "isp_hw_interface.h"

static int isp_hw_vinf_start_transfer(uint32_t isp_id,
									  const struct isp_interface_info *info)
{
	return RTS_ISP_OK;
}

static int isp_hw_vinf_stop_transfer(uint32_t isp_id,
									 const struct isp_interface_info *info)
{
	return RTS_ISP_OK;
}

static const struct isp_hw_interface_ops hw_vinf_ops = {
	.start_transfer = isp_hw_vinf_start_transfer,
	.stop_transfer = isp_hw_vinf_stop_transfer,
};

const struct isp_hw_interface_ops *isp_hw_vinf_get_ops(void)
{
	return &hw_vinf_ops;
}
