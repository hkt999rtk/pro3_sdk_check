/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_reg.h>
#include <isp_reg_base.h>
#include <isp_reg_virtual.h>
#include "isp_hw_iq.h"

static uint32_t isp_hw_iq_read_reg_mask(uint32_t isp_id, uint32_t offset,
										uint32_t mask)
{
	if (mask) {
		if (offset < VIRTUAL_REG_BASE) {
			return isp_read_reg(offset) & mask;
		} else {
			return isp_read_vreg(offset) & mask;
		}
	} else {
		if (offset < VIRTUAL_REG_BASE) {
			return isp_read_reg(offset);
		} else {
			return isp_read_vreg(offset);
		}
	}
}

static void isp_hw_iq_write_reg_mask(uint32_t isp_id, uint32_t value,
									 uint32_t offset, uint32_t mask)
{
	if (mask) {
		if (offset < VIRTUAL_REG_BASE) {
			isp_write_reg_mask(value, offset, mask);
		} else {
			isp_write_vreg_mask(value, offset, mask);
		}
	} else {
		if (offset < VIRTUAL_REG_BASE) {
			isp_write_reg(value, offset);
		} else {
			isp_write_vreg(value, offset);
		}
	}
}

static const struct isp_hw_iq_ops hw_iq_ops = {
	.read_reg_mask = isp_hw_iq_read_reg_mask,
	.write_reg_mask = isp_hw_iq_write_reg_mask,
};

const struct isp_hw_iq_ops *isp_hw_iq_get_ops(void)
{
	return &hw_iq_ops;
}
