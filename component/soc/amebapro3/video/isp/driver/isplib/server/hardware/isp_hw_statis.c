/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_define.h>
#include <isp_os_time.h>
#include <isp_reg_statis.h>
#include "isp_hw_statis.h"

static int isp_hw_statis_enable(uint32_t isp_id, int enable, int hdr_mode)
{
	if (enable) {
		if (hdr_mode == RTS_ISP_HDR_LINE_2TO1 ||
			hdr_mode == RTS_ISP_HDR_DCG_2TO1)
			isp_write_reg((RAW_STATS_EN_MASK | AF0_STATS_EN_MASK |
						   AF1_STATS_EN_MASK | AE_0_STATS_EN_MASK |
						   AE_1_STATS_EN_MASK | AE_2_STATS_EN_MASK |
						   AWB_STATS_EN_MASK | Y_STATS_EN_MASK),
						  DMA_STATS_EN);
		else
			isp_write_reg((RAW_STATS_EN_MASK | AF1_STATS_EN_MASK |
						   AE_0_STATS_EN_MASK | AE_2_STATS_EN_MASK |
						   AWB_STATS_EN_MASK | Y_STATS_EN_MASK),
						  DMA_STATS_EN);
		isp_write_reg(AEAWBAF_EN_MASK, DMA_CTRL0);
		isp_write_reg(STATS_TRIG_MASK, DMA_STATS_TRIG);
	} else {
		int count = 10;

		isp_write_reg(STATS_STOP_MASK, DMA_STATS_TRIG);
		isp_set_reg_bit(STREAM_STOP_IN_AEAWBAF_OFFSET, DMA_CTRL0);
		while (isp_read_reg(DMA_AXI_STATE) && count--) {
			isp_os_usleep(1000);
		}
		if (count < 0) {
			return -RTS_ISP_EBUSY;
		}
	}
	return RTS_ISP_OK;
}

static int isp_hw_statis_set_cfg(uint32_t isp_id, uint32_t base, uint32_t size)
{
	isp_write_reg(base << 16 | size, DMA_BUFF_SIZE);
	return RTS_ISP_OK;
}

static const struct isp_hw_statis_ops hw_statis_ops = {
	.enable = isp_hw_statis_enable,
	.set_cfg = isp_hw_statis_set_cfg,
};

const struct isp_hw_statis_ops *isp_hw_statis_get_ops(void)
{
	return &hw_statis_ops;
}
