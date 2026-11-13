/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_reg_raw_stats.h>
#include "isp_hw_rawstat.h"

#define RAWSTAT_WIN_COLS 16
#define RAWSTAT_WIN_ROWS 16
#define RAWSTAT_WIN_ALIGN_H 2
#define RAWSTAT_WIN_ALIGN_V 2
#define RAWSTAT_WIN_SAMPLE_GAP_H 2
#define RAWSTAT_WIN_SAMPLE_GAP_V 2

static int isp_hw_rawstat_get_hw_info(uint32_t isp_id,
									  struct isp_rawstat_hw_info *info)
{
	if (!info) {
		return -RTS_ISP_EINVAL;
	}

	info->grid_info.size.cols = RAWSTAT_WIN_COLS;
	info->grid_info.size.rows = RAWSTAT_WIN_ROWS;
	info->grid_info.cell_align.w = RAWSTAT_WIN_ALIGN_H;
	info->grid_info.cell_align.h = RAWSTAT_WIN_ALIGN_V;
	info->cell_sample.w = RAWSTAT_WIN_SAMPLE_GAP_H;
	info->cell_sample.h = RAWSTAT_WIN_SAMPLE_GAP_V;
	info->rgby_size = 2;
	info->rgbg_size = 2;
	info->pixels_size = 2;

	return RTS_ISP_OK;
}

static int isp_hw_rawstat_set_statis(uint32_t isp_id,
									 const rts_isp_grid_t *grid)
{
	if (!grid) {
		return -RTS_ISP_EINVAL;
	}

	isp_write_reg((grid->start.x << RAW_START_X_OFFSET |
				   grid->start.y << RAW_START_Y_OFFSET),
				  RAW_WIN_START);
	isp_write_reg(
		((grid->cell.w / RAWSTAT_WIN_ALIGN_H) << WIN_WIDTH_OFFSET |
		 (grid->cell.h / RAWSTAT_WIN_ALIGN_V) << WIN_HEIGHT_OFFSET),
		RAW_WIN_SIZE);
	isp_write_reg(0x22000565, RAW_BRIGHT_COEF);
	isp_write_reg(0x118, RAW_STATIS_CTRL);

	return RTS_ISP_OK;
}

static const struct isp_hw_rawstat_ops hw_rawstat_ops = {
	.get_hw_info = isp_hw_rawstat_get_hw_info,
	.set_statis = isp_hw_rawstat_set_statis,
};

const struct isp_hw_rawstat_ops *isp_hw_rawstat_get_ops(void)
{
	return &hw_rawstat_ops;
}
