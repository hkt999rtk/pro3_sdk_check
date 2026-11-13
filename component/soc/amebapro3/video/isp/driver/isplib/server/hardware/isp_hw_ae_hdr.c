/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2024 Sally Peng <sally_peng@realsil.com.cn>
 */

#include <isp_reg_ae_2.h>
#include "isp_hw_ae_hdr.h"

#define AE_WIN_SAMPLE_GAP_H 4
#define AE_WIN_SAMPLE_GAP_V 4

void isp_hw_ae_hdr_set_statis(uint32_t isp_id, int win_sample_num,
							  const rts_isp_grid_t *grid,
							  const rts_isp_point_t *end)
{
	int i;

	for (i = 0; i < 2; i++) {
		isp_write_reg(grid->start.x | grid->start.y << 16,
					  AE_WIN_START + AE_REG_OFFSET(i));
		isp_write_reg(((grid->cell.w / AE_WIN_SAMPLE_GAP_H) |
					   (grid->cell.h / AE_WIN_SAMPLE_GAP_V) << 8 |
					   grid->size.cols << 16 | grid->size.rows << 24),
					  AE_WIN_SIZE + AE_REG_OFFSET(i));
		isp_write_reg(win_sample_num,
					  AE_WIN_CAL_NUM + AE_REG_OFFSET(i));
		isp_write_reg(grid->start.x | grid->start.y << 16,
					  AE_HWIN_START + AE_REG_OFFSET(i));
		isp_write_reg(end->x | end->y << 16,
					  AE_HWIN_END + AE_REG_OFFSET(i));
		isp_write_reg(0x01, AE_CTRL + AE_REG_OFFSET(i));
	}
}

void isp_hw_ae_hdr_get_sync_regs(uint32_t isp_id, uint32_t hdr_gain[2], int i,
								 struct rts_isp_sync_reg *reg)
{
	set_sync_isp(&reg[i++], AE_GAIN_DATA, hdr_gain[0]);
	set_sync_isp(&reg[i++], AE_GAIN_DATA + AE_REG_OFFSET(1), hdr_gain[1]);
}
