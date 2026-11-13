/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_os_mem.h>
#include <isp_utils.h>
#include <isp_reg_virtual.h>
#include <isp_reg_ccm.h>
#include "isp_hw_ccm.h"

#define CCM_WIN_ROWS 9
#define CCM_WIN_COLS 16
#define CCM_WIN_GAP_H 4
#define CCM_WIN_GAP_V 4

#define MOD_NAME CCM
static void isp_hw_sat_thd_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t sat_thd_diff_offset;
	uint32_t sat_thd_slope;

	sat_thd_diff_offset = READ_REG(mod_info, SAT_THD_DIFF_OFFSET);

	sat_thd_slope =
		(uint32_t)((1048576.0 / (float)(sat_thd_diff_offset)) + 0.5);

	WRITE_REG(mod_info, SAT_THD_SLOPE, sat_thd_slope);
}

static void isp_hw_edge_rate_slope(FW_EST_MOD_INFO mod_info)
{
	uint16_t edge_offset;
	uint32_t edge_rate_slope;

	edge_offset = READ_REG(mod_info, EDGE_OFFSET);

	edge_rate_slope =
		(uint32_t)(((256.0 / (float)(edge_offset)) * 4096.0) + 0.5);

	WRITE_REG(mod_info, EDGE_RATE_SLOPE, edge_rate_slope);
}

static void isp_hw_color_diff_slope_level(FW_EST_MOD_INFO mod_info)
{
	uint16_t color_diff_1_rate_min_level_1_2,
			 color_diff_1_rate_min_level_2_3, color_diff_1_rate_min_level_3;
	uint16_t color_diff_2_rate_min_level_1_2,
			 color_diff_2_rate_min_level_2_3, color_diff_2_rate_min_level_3;
	uint16_t color_diff_1_thd_offset_level_1_2,
			 color_diff_1_thd_offset_level_2_3,
			 color_diff_1_thd_offset_level_3;
	uint16_t color_diff_2_thd_offset_level_1_2,
			 color_diff_2_thd_offset_level_2_3,
			 color_diff_2_thd_offset_level_3;
	uint32_t color_diff_1_slop_levle_1_2, color_diff_1_slop_levle_2_3,
			 color_diff_1_slop_levle_3;
	uint32_t color_diff_2_slop_levle_1_2, color_diff_2_slop_levle_2_3,
			 color_diff_2_slop_levle_3;

	//PFR_color_diff_1_SLOPE_LEVEL_1_2
	color_diff_1_rate_min_level_1_2 =
		READ_REG(mod_info, COLOR_DIFF_1_RATE_MIN_LEVEL_1_2);
	color_diff_1_thd_offset_level_1_2 =
		READ_REG(mod_info, COLOR_DIFF_1_THD_OFFSET_LEVEL_1_2);

	color_diff_1_slop_levle_1_2 =
		(uint32_t)(((float)(256.0 - color_diff_1_rate_min_level_1_2) /
					(float)(color_diff_1_thd_offset_level_1_2) *
					4096.0) +
				   0.5);

	//PFR_color_diff_1_SLOPE_LEVEL_2_3
	color_diff_1_rate_min_level_2_3 =
		READ_REG(mod_info, COLOR_DIFF_1_RATE_MIN_LEVEL_2_3);
	color_diff_1_thd_offset_level_2_3 =
		READ_REG(mod_info, COLOR_DIFF_1_THD_OFFSET_LEVEL_2_3);

	color_diff_1_slop_levle_2_3 =
		(uint32_t)(((float)(256.0 - color_diff_1_rate_min_level_2_3) /
					(float)(color_diff_1_thd_offset_level_2_3) *
					4096.0) +
				   0.5);

	//PFR_color_diff_1_SLOPE_LEVEL_3
	color_diff_1_rate_min_level_3 =
		READ_REG(mod_info, COLOR_DIFF_1_RATE_MIN_LEVEL_3);
	color_diff_1_thd_offset_level_3 =
		READ_REG(mod_info, COLOR_DIFF_1_THD_OFFSET_LEVEL_3);

	color_diff_1_slop_levle_3 =
		(uint32_t)(((float)(256.0 - color_diff_1_rate_min_level_3) /
					(float)(color_diff_1_thd_offset_level_3) * 4096.0) +
				   0.5);

	//PFR_color_diff_2_SLOPE_LEVEL_1_2
	color_diff_2_rate_min_level_1_2 =
		READ_REG(mod_info, COLOR_DIFF_2_RATE_MIN_LEVEL_1_2);
	color_diff_2_thd_offset_level_1_2 =
		READ_REG(mod_info, COLOR_DIFF_2_THD_OFFSET_LEVEL_1_2);

	color_diff_2_slop_levle_1_2 =
		(uint32_t)(((float)(256.0 - color_diff_2_rate_min_level_1_2) /
					(float)(color_diff_2_thd_offset_level_1_2) *
					4096.0) +
				   0.5);

	//PFR_color_diff_2_SLOPE_LEVEL_2_3
	color_diff_2_rate_min_level_2_3 =
		READ_REG(mod_info, COLOR_DIFF_2_RATE_MIN_LEVEL_2_3);
	color_diff_2_thd_offset_level_2_3 =
		READ_REG(mod_info, COLOR_DIFF_2_THD_OFFSET_LEVEL_2_3);

	color_diff_2_slop_levle_2_3 =
		(uint32_t)(((float)(256.0 - color_diff_2_rate_min_level_2_3) /
					(float)(color_diff_2_thd_offset_level_2_3) *
					4096.0) +
				   0.5);

	//PFR_color_diff_2_SLOPE_LEVEL_3
	color_diff_2_rate_min_level_3 =
		READ_REG(mod_info, COLOR_DIFF_2_RATE_MIN_LEVEL_3);
	color_diff_2_thd_offset_level_3 =
		READ_REG(mod_info, COLOR_DIFF_2_THD_OFFSET_LEVEL_3);

	color_diff_2_slop_levle_3 =
		(uint32_t)(((float)(256.0 - color_diff_2_rate_min_level_3) /
					(float)(color_diff_2_thd_offset_level_3) * 4096.0) +
				   0.5);

	WRITE_REG(mod_info, COLOR_DIFF_1_SLOPE_LEVEL_1_2,
			  color_diff_1_slop_levle_1_2);
	WRITE_REG(mod_info, COLOR_DIFF_1_SLOPE_LEVEL_2_3,
			  color_diff_1_slop_levle_2_3);
	WRITE_REG(mod_info, COLOR_DIFF_1_SLOPE_LEVEL_3,
			  color_diff_1_slop_levle_3);

	WRITE_REG(mod_info, COLOR_DIFF_2_SLOPE_LEVEL_1_2,
			  color_diff_2_slop_levle_1_2);
	WRITE_REG(mod_info, COLOR_DIFF_2_SLOPE_LEVEL_2_3,
			  color_diff_2_slop_levle_2_3);
	WRITE_REG(mod_info, COLOR_DIFF_2_SLOPE_LEVEL_3,
			  color_diff_2_slop_levle_3);
}

static void uvtune3917_supr_assign_rep_vec_offline(int pipe_rep_vec[16][2],
		int uvs_mode)
{
	float pipe_dir_vec[16][2] = { { -1.0, 0.0 },
		{ -2.0 / sqrt(5.0), 1.0 / sqrt(5.0) },
		{ -1.0 / sqrt(2.0), 1.0 / sqrt(2.0) },
		{ -1.0 / sqrt(5.0), 2.0 / sqrt(5.0) },
		{ 0.0, 1.0 },
		{ 1.0 / sqrt(5.0), 2.0 / sqrt(5.0) },
		{ 1.0 / sqrt(2.0), 1.0 / sqrt(2.0) },
		{ 2.0 / sqrt(5.0), 1.0 / sqrt(5.0) },
		{ 1.0, 0.0 },
		{ 2.0 / sqrt(5.0), -1.0 / sqrt(5.0) },
		{ 1.0 / sqrt(2.0), -1.0 / sqrt(2.0) },
		{ 1.0 / sqrt(5.0), -2.0 / sqrt(5.0) },
		{ 0.0, -1.0 },
		{ -1.0 / sqrt(5.0), -2.0 / sqrt(5.0) },
		{ -1.0 / sqrt(2.0), -1.0 / sqrt(2.0) },
		{ -2.0 / sqrt(5.0), -1.0 / sqrt(5.0) }
	};

	if (uvs_mode == 0) { // L1 norm
		for (int i = 0; i < 4; i++) {
			pipe_rep_vec[0 + i][0] = -256;
			pipe_rep_vec[0 + i][1] = 256;
			pipe_rep_vec[4 + i][0] = 256;
			pipe_rep_vec[4 + i][1] = 256;
			pipe_rep_vec[8 + i][0] = 256;
			pipe_rep_vec[8 + i][1] = -256;
			pipe_rep_vec[12 + i][0] = -256;
			pipe_rep_vec[12 + i][1] = -256;
		}
	} else if (uvs_mode == 1) { // L2 norm
		float cos_a = pipe_dir_vec[0][0] * pipe_dir_vec[1][0] +
					  pipe_dir_vec[0][1] * pipe_dir_vec[1][1];
		float cos_b = pipe_dir_vec[1][0] * pipe_dir_vec[2][0] +
					  pipe_dir_vec[1][1] * pipe_dir_vec[2][1];

		float cos_c = sqrt((1.0 + max(cos_a, cos_b)) / 2.0);
		float sin_c = sqrt((1.0 - max(cos_a, cos_b)) / 2.0);
		float vec[2];

		for (int i = 0; i < 16; i++) {
			if (i % 2 == 0) {
				vec[0] = pipe_dir_vec[i + 1][0] * cos_c -
						 pipe_dir_vec[i + 1][1] * sin_c;
				vec[1] = pipe_dir_vec[i + 1][1] * cos_c +
						 pipe_dir_vec[i + 1][0] * sin_c;
			} else {
				vec[0] = pipe_dir_vec[i][0] * cos_c +
						 pipe_dir_vec[i][1] * sin_c;
				vec[1] = pipe_dir_vec[i][1] * cos_c -
						 pipe_dir_vec[i][0] * sin_c;
			}

			for (int j = 0; j < 2; j++) {
				if (vec[j] >= 0.0)
					pipe_rep_vec[i][j] =
						(int)(256.0 * vec[j] + 0.5);
				else
					pipe_rep_vec[i][j] =
						-1 *
						(int)(-256.0 * vec[j] + 0.5);
			}
		}
	} else if (uvs_mode == 2) { // L infinity norm
		for (int i = 0; i < 4; i++) {
			pipe_rep_vec[2 + i][0] = 0;
			pipe_rep_vec[2 + i][1] = 256;
			pipe_rep_vec[6 + i][0] = 256;
			pipe_rep_vec[6 + i][1] = 0;
			pipe_rep_vec[10 + i][0] = 0;
			pipe_rep_vec[10 + i][1] = -256;
			pipe_rep_vec[(14 + i) % 16][0] = -256;
			pipe_rep_vec[(14 + i) % 16][1] = 0;
		}
	}
}

static void isp_hw_pipe_rep_vec(FW_EST_MOD_INFO mod_info)
{
	int pipe_rep_vec[16][2];

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 2; j++)
			pipe_rep_vec[i][j] = READ_REG_ARRAY_I(
									 mod_info, PIPE_REP_VEC, i * 2 + j);

	int uvs_mode = 1; // 0: L1 norm, 1: L2 norm, 2: L infinity norm

	uvtune3917_supr_assign_rep_vec_offline(pipe_rep_vec, uvs_mode);

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 2; j++)
			WRITE_REG_ARRAY_I(mod_info, PIPE_REP_VEC, i * 2 + j,
							  pipe_rep_vec[i][j]);
}

static void isp_hw_chroma_thd_slope(FW_EST_MOD_INFO mod_info)
{
	int ccm_blend_rate_min = READ_REG(mod_info, CCM_BLEND_RATE_MIN);
	int chroma_thd_offset = READ_REG(mod_info, CHROMA_THD_OFFSET);
	int chroma_thd_slope =
		(int)((float)(256 - ccm_blend_rate_min) * 4096.0 /
			  (float)(chroma_thd_offset) +
			  0.5);
	WRITE_REG(mod_info, CHROMA_THD_SLOPE, chroma_thd_slope);
}

static void isp_hw_dark_edge_thd_slope(FW_EST_MOD_INFO mod_info)
{
	int chroma_thd_inc_by_dark_edge =
		READ_REG(mod_info, CHROMA_THD_INC_BY_DARK_EDGE);
	int dark_edge_thd_offset = READ_REG(mod_info, DARK_EDGE_THD_OFFSET);
	int dark_edge_thd_slope =
		(int)((float)(chroma_thd_inc_by_dark_edge) * 4096.0 /
			  (float)(dark_edge_thd_offset) +
			  0.5);
	WRITE_REG(mod_info, DARK_EDGE_THD_SLOPE, dark_edge_thd_slope);
}

static void isp_hw_ccm_update_cac(FW_EST_MOD_INFO mod_info)
{
	float thd0;
	float thd1;
	float log10_thd0;
	float log10_thd1;
	int sat_thd1_offset;
	int sat_thd_diff_offset;
	int sat_thd_slope;
#define log10_4096 3.61236f /* log10f(4096.0) */

	thd0 = clamp(isp_read_vreg(VCAC_SAT_THD0) / 1024.0f, 0.01f, 0.99f);
	thd1 = clamp(isp_read_vreg(VCAC_SAT_THD1) / 1024.0f, thd0 + 0.01f,
				 1.0f);
	log10_thd0 = log10f(thd0);
	log10_thd1 = log10f(thd1);

	sat_thd1_offset = (4095.0f / log10_4096) * -log10_thd1 + 0.5f;
	sat_thd_diff_offset =
		((4095.0f / log10_4096) * (log10_thd1 - log10_thd0) + 0.5f);
	sat_thd_slope = (float)(1 << 20) / sat_thd_diff_offset + 0.5f;

	WRITE_REG(mod_info, SAT_THD1_OFFSET, sat_thd1_offset);
	WRITE_REG(mod_info, SAT_THD_DIFF_OFFSET, sat_thd_diff_offset);
	WRITE_REG(mod_info, SAT_THD_SLOPE, sat_thd_slope);
}

static int isp_hw_ccm_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	isp_hw_sat_thd_slope(mod_info);
	isp_hw_edge_rate_slope(mod_info);
	isp_hw_color_diff_slope_level(mod_info);
	isp_hw_pipe_rep_vec(mod_info);
	isp_hw_chroma_thd_slope(mod_info);
	isp_hw_dark_edge_thd_slope(mod_info);
	isp_hw_ccm_update_cac(mod_info);

	return 0;
}

static int isp_hw_ccm_init_vreg(uint32_t isp_id)
{
	isp_write_vreg(0x232, VCAC_SAT_THD0);
	isp_write_vreg(0x333, VCAC_SAT_THD1);

	return RTS_ISP_OK;
}

static int isp_hw_ccm_get_hw_info(uint32_t isp_id, struct isp_ccm_hw_info *info)
{
	if (!info) {
		return -RTS_ISP_EINVAL;
	}

	isp_memset(info, 0, sizeof(*info));
	info->grid_info.size.cols = CCM_WIN_COLS;
	info->grid_info.size.rows = CCM_WIN_ROWS;
	info->grid_info.cell_align.w = CCM_WIN_GAP_H;
	info->grid_info.cell_align.h = CCM_WIN_GAP_V;

	return RTS_ISP_OK;
}

static int isp_hw_ccm_set_ccm(uint32_t isp_id,
							  const isp_iq_ccm_v1_dyn_item_t *ccm)
{
	int i;
	uint32_t offset;

	if (!ccm) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0, offset = CCM_R_AFFECT_R; i < 9; i++, offset += 4) {
		isp_write_reg(ccm->ccm[i], offset);
	}

	return RTS_ISP_OK;
}

static int isp_hw_ccm_set_size(uint32_t isp_id, const rts_isp_grid_t *grid)
{
	int win_height, win_width, win_cal_num;
	int ae_start_x, ae_start_y, win_end_x, win_end_y;

	win_height = grid->cell.w / CCM_WIN_GAP_H;
	win_width = grid->cell.h / CCM_WIN_GAP_V;
	win_cal_num = win_height * win_width;
	ae_start_x = grid->start.x;
	ae_start_y = grid->start.y;
	win_end_x = grid->start.x + grid->cell.w * grid->size.cols;
	win_end_y = grid->start.y + grid->cell.h * grid->size.rows;

	isp_write_reg(CCM_WIN_ROWS, CCM_WIN_NUM_V);
	isp_write_reg(CCM_WIN_COLS, CCM_WIN_NUM_H);
	isp_write_reg(win_height, CCM_WIN_HEIGHT);
	isp_write_reg(win_width, CCM_WIN_WIDTH);
	isp_write_reg(win_cal_num, CCM_WIN_CAL_NUM);
	isp_write_reg(ae_start_x, CCM_AE_START_X);
	isp_write_reg(ae_start_y, CCM_AE_START_Y);
	isp_write_reg(win_end_x, CCM_WIN_END_X);
	isp_write_reg(win_end_y, CCM_WIN_END_Y);

	return RTS_ISP_OK;
}

static int isp_hw_ccm_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_ccm_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(CCM_REG_LOAD_MASK, CCM_SYNC);

	return RTS_ISP_OK;
}

static const struct isp_hw_ccm_ops hw_ccm_ops = {
	.init_vreg = isp_hw_ccm_init_vreg,
	.get_hw_info = isp_hw_ccm_get_hw_info,
	.set_ccm = isp_hw_ccm_set_ccm,
	.set_size = isp_hw_ccm_set_size,
	.est_dynamic = isp_hw_ccm_estimation,
};

const struct isp_hw_ccm_ops *isp_hw_ccm_get_ops(void)
{
	return &hw_ccm_ops;
}
