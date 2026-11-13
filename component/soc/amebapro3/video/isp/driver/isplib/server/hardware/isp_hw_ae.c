/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_define.h>
#include <isp_os_log.h>
#include <isp_os_time.h>
#include <isp_os_mem.h>
#include <isp_utils.h>
#include <isp_reg_vhdr_axi.h>
#include <isp_reg_vhdrfs.h>
#include <isp_reg_tm.h>
#include <isp_reg_ae_0.h>
#include <isp_reg_virtual.h>
#include "isp_hw_ae.h"
#include "isp_hw_ae_hdr.h"

#define AE_WIN_COLS 16
#define AE_WIN_ROWS 16
#define AE_WIN_SAMPLE_GAP_H 4
#define AE_WIN_SAMPLE_GAP_V 4
#define AE_HIST_BINS 1024
#define AE_HIST_SAMPLE_GAP_H 4
#define AE_HIST_SAMPLE_GAP_V 4
#define AE_HDR_HIST_BINS 256

static void __set_hdr_tm_regs(void)
{
	/* 16x ratio setting */
	isp_write_reg(0x1000 / AE_DEF_RATIO, TM_EXPOSURE_RATIO_INV);
	isp_write_reg(AE_DEF_RATIO * 4, FUS_EXPOSURE_RATIO);
	isp_write_reg(0x100, STATIS_GAIN);
	isp_write_reg(0x1, VHDR_ENABLE);
}

static void __set_linear_tm_regs(void)
{
	/* 1x ratio setting */
	isp_write_reg(0x1000, TM_EXPOSURE_RATIO_INV);
	isp_write_reg(0x4, FUS_EXPOSURE_RATIO);
	isp_write_reg(0x100, STATIS_GAIN);
	isp_write_reg(0x0, VHDR_ENABLE);
}

static int isp_hw_ae_get_hw_info(uint32_t isp_id, struct isp_ae_hw_info *info)
{
	if (!info) {
		return -RTS_ISP_EINVAL;
	}

	isp_memset(info, 0, sizeof(*info));
	info->ae.grid_info.size.cols = AE_WIN_COLS;
	info->ae.grid_info.size.rows = AE_WIN_ROWS;
	info->ae.grid_info.cell_align.w = AE_WIN_SAMPLE_GAP_H;
	info->ae.grid_info.cell_align.h = AE_WIN_SAMPLE_GAP_V;
	info->ae.cell_sample.w = AE_WIN_SAMPLE_GAP_H;
	info->ae.cell_sample.h = AE_WIN_SAMPLE_GAP_V;
	info->ae.hist_bins = AE_HIST_BINS;
	info->ae.hist_sample.w = AE_HIST_SAMPLE_GAP_H;
	info->ae.hist_sample.h = AE_HIST_SAMPLE_GAP_V;
	info->ae.y_mean_size = 2;
	info->ae.hist_bin_size = 4;
	info->hdr[0] = info->ae;
	info->hdr[0].hist_bins = AE_HDR_HIST_BINS;
	info->hdr[1] = info->ae;
	info->hdr[1].hist_bins = AE_HDR_HIST_BINS;

	return RTS_ISP_OK;
}

static int isp_hw_ae_set_statis(uint32_t isp_id, const rts_isp_grid_t *grid)
{
	rts_isp_point_t end;
	int win_sample_num;

	end.x = grid->start.x + grid->cell.w * grid->size.cols;
	end.y = grid->start.y + grid->cell.h * grid->size.rows;
	win_sample_num = grid->cell.w * grid->cell.h / AE_WIN_SAMPLE_GAP_H /
					 AE_WIN_SAMPLE_GAP_V;

	isp_write_reg(grid->start.x | grid->start.y << 16, AE_DYN_WIN_START);
	isp_write_reg(((grid->cell.w / AE_WIN_SAMPLE_GAP_H) |
				   (grid->cell.h / AE_WIN_SAMPLE_GAP_V) << 8 |
				   grid->size.cols << 16 | grid->size.rows << 24),
				  AE_DYN_WIN_SIZE);
	isp_write_reg(win_sample_num, AE_DYN_WIN_CAL_NUM);
	isp_write_reg(grid->start.x | grid->start.y << 16, AE_DYN_HWIN_START);
	isp_write_reg(end.x | end.y << 16, AE_DYN_HWIN_END);
	isp_write_reg(0x1, AE_DYN_CTRL);

	isp_hw_ae_hdr_set_statis(isp_id, win_sample_num, grid, &end);

	return RTS_ISP_OK;
}

static int isp_hw_ae_get_sync_regs(uint32_t isp_id, uint32_t delay,
								   struct rts_isp_sync_reg *reg, uint32_t *num,
								   const struct rts_isp_ae_exp_gain *exp_gain)
{
	int i = 0;
	float ratiof;
	uint32_t fusion_enable;
	uint32_t hdr_gain[2];
	uint32_t ratio;
	uint32_t ratio_inv;
	uint32_t statis_gain;
	uint32_t isp_gain;

	if (!reg || !exp_gain || delay > 2) {
		return -RTS_ISP_EINVAL;
	}

	isp_gain = exp_gain->isp_gain * 2048;
	if (isp_gain > ((1 << AE_GAIN_BITS) - 1) || isp_gain < 2048) {
		isp_error("isp_gain out of range\n");
		return -RTS_ISP_EINVAL;
	}

	if (exp_gain->hdr == RTS_ISP_HDR_LINE_2TO1) {
		fusion_enable = 1;
		hdr_gain[0] = exp_gain->isp_hdr_gain[0] * 2048;
		hdr_gain[1] = exp_gain->isp_hdr_gain[1] * 2048;
		ratiof = ((exp_gain->exposure[0] * exp_gain->analog_gain[0] *
				   exp_gain->digital_gain[0] *
				   exp_gain->isp_hdr_gain[0]) /
				  (exp_gain->exposure[1] * exp_gain->analog_gain[1] *
				   exp_gain->digital_gain[1] *
				   exp_gain->isp_hdr_gain[1]));
		ratio = ratiof * 4;
		statis_gain =
			isp_upper_power_of_2(ratio >> 2) * 4.0 / ratio * 0x100;
	} else {
		fusion_enable = 0;
		hdr_gain[0] = exp_gain->isp_hdr_gain[0] * 2048;
		hdr_gain[1] = 2048;
		if (exp_gain->hdr == RTS_ISP_HDR_DCG_COMBINE ||
			exp_gain->hdr == RTS_ISP_HDR_DCG_COMBINE_TM) {
			ratiof = 16.f;
		} else {
			ratiof = 1.0f;
		}
		ratio = 4;
		statis_gain = 0x100;
	}
	ratio_inv = 4096 / ratiof;

	set_sync_info(&reg[i++], delay, RTS_ISP_INT_DATA_START);
	set_sync_isp(&reg[i++], AE_DYN_GAIN, isp_gain);
	isp_hw_ae_hdr_get_sync_regs(isp_id, hdr_gain, i, reg);
	set_sync_isp(&reg[i++], FUS_EXPOSURE_RATIO, ratio);
	set_sync_isp(&reg[i++], STATIS_GAIN, statis_gain);
	set_sync_isp(&reg[i++], VHDR_ENABLE, fusion_enable);
	set_sync_isp(&reg[i++], TM_EXPOSURE_RATIO_INV, ratio_inv);
	*num = i;

	return RTS_ISP_OK;
}

static int isp_hw_ae_enable_hdr(uint32_t isp_id, int enable)
{
	if (enable) {
		__set_hdr_tm_regs();
	} else {
		__set_linear_tm_regs();
	}

	return RTS_ISP_OK;
}

static int isp_hw_ae_enable_hdr_axi(uint32_t isp_id, int enable)
{
	if (enable) {
		isp_write_reg(STREAM_EN_VHDR_MASK, VHDR_STREAM_EN);
	} else {
		int count = 10;

		isp_set_reg_bit(VHDR_STOP_OFFSET, VHDR_STREAM_EN);
		while (isp_read_reg(VHDR_AXI_FSM_STATE) && count--) {
			isp_os_usleep(1000);
		}
		if (count < 0) {
			return -RTS_ISP_EBUSY;
		}
	}

	return RTS_ISP_OK;
}

static const struct isp_hw_ae_ops hw_ae_ops = {
	.get_hw_info = isp_hw_ae_get_hw_info,
	.set_statis = isp_hw_ae_set_statis,
	.get_sync_regs = isp_hw_ae_get_sync_regs,
	.enable_hdr = isp_hw_ae_enable_hdr,
	.enable_hdr_axi = isp_hw_ae_enable_hdr_axi,
};

const struct isp_hw_ae_ops *isp_hw_ae_get_ops(void)
{
	return &hw_ae_ops;
}
