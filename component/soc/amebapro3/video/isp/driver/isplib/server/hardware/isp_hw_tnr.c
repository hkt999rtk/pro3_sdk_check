/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_os_time.h>
#include <isp_utils.h>
#include <isp_reg_tnr.h>
#include <isp_reg_virtual.h>
#include "isp_hw_tnr.h"

#define MOD_NAME TNR

#define REG_MASK_SYM // symmetry
//#define REG_MD_WT_SHARED
//#define REG_SNR_WT_SHARED
//#define REG_GE_WT_SHARED

static void diff_slope_reg_from_th_n_level(int diff_th[5], int diff_level[5],
		int diff_slope[4],
		int bit_fract_alpha_weight_slope)
{
	for (int i = 0; i < 4; i++)
		diff_slope[i] =
			(int)((float)(diff_level[i] - diff_level[i + 1]) /
				  (float)(diff_th[i + 1] - diff_th[i]) *
				  (float)(1
						  << bit_fract_alpha_weight_slope) +
				  0.5);
}

static void reg3917_detail3917_edgeslope(int detail_th[4],
		int detail_edgeness[4],
		int detail_edgeslope[3],
		int bit_fract_alpha_weight_slope)
{
	for (int i = 0; i < 3; i++) {
		detail_edgeslope[i] =
			(int)((float)((detail_edgeness[i + 1] -
						   detail_edgeness[i]) *
						  (1 << bit_fract_alpha_weight_slope)) /
				  (float)(detail_th[i + 1] - detail_th[i]) +
				  0.5);
	}
}

static void reg3917_detail3917_adjustslope(int detail_edgeness[4],
		int detail_adjust[4],
		int detail_adjustslope[3],
		int bit_fract_alpha_weight_slope)
{
	for (int i = 0; i < 3; i++) {
		detail_adjustslope[i] =
			(int)((float)((detail_adjust[i + 1] - detail_adjust[i]) *
						  (1 << bit_fract_alpha_weight_slope)) /
				  (float)(detail_edgeness[i + 1] -
						  detail_edgeness[i]) +
				  0.5);
	}
}

static void reg3917_mask_coef_slope(int th_num, int coef_num, int th[5],
									int coef[5][2], int slope[4][2],
									int bit_fract_mask_coef_slope)
{
	int sign = 0;
	float slope_temp = 0.0;
	for (int i = 0; i < /*4*/ (th_num - 1); i++) {
		for (int j = 0; j < coef_num; j++) {
			slope_temp = (float)(coef[i + 1][j] - coef[i][j]) /
						 (float)(th[i + 1] - th[i]);

			if (fabsf(slope_temp) > 0) {
				sign = slope_temp / fabsf(slope_temp);
				slope[i][j] =
					sign *
					(int)(fabsf(slope_temp) *
						  (float)(1
								  << bit_fract_mask_coef_slope) +
						  0.5);
			} else {
				slope[i][j] = 0;
			}
		}
	}
}

static int ceiling(float x)
{
	int out;

	if (x >= 0.0) {
		out = ((x - (float)((int)(x))) > 0.0) ? (int)(x + 1) : (int)(x);
	} else {
		out = ((x - (float)((int)(x))) < 0.0) ? (int)(x) : (int)(x);
	}

	return out;
}

static void isp_hw_bl(FW_EST_MOD_INFO mod_info)
{
	int bit_fract_bl_offset_gain = 12;

	uint32_t sensor_bit_mode =
		READ_VIRT_REG(mod_info, VSYS_SENSOR_BIT_DEPTH);
	sensor_bit_mode =
		sensor_bit_mode > 12 ?
		12 :
		sensor_bit_mode; // max 12 bits for linear mode
	// int sensor_bit_mode = READ_REG(mod_info, sensor_bit_mode);

	int bl_offset_12bit_ch, bl_offset_gain_12bit_ch;

	for (int ch = 0; ch < 4; ch++) {
		bl_offset_12bit_ch =
			READ_REG_ARRAY_I(mod_info, BL_OFFSET_12BIT_CH, ch);

		if (/*sensor_valid_bit_depth == 8*/ sensor_bit_mode == 8)
			bl_offset_gain_12bit_ch = ceiling(
										  (float)((15 + bl_offset_12bit_ch) *
												  (1 << bit_fract_bl_offset_gain)) /
										  (float)(4080 - bl_offset_12bit_ch));
		else if (/*sensor_valid_bit_depth == 10*/ sensor_bit_mode == 10)
			bl_offset_gain_12bit_ch = ceiling(
										  (float)((3 + bl_offset_12bit_ch) *
												  (1 << bit_fract_bl_offset_gain)) /
										  (float)(4092 - bl_offset_12bit_ch));
		else /*if (sensor_valid_bit_depth == 12 || sensor_bit_mode == 2)*/
			bl_offset_gain_12bit_ch = ceiling(
										  (float)((0 + bl_offset_12bit_ch) *
												  (1 << bit_fract_bl_offset_gain)) /
										  (float)(4095 - bl_offset_12bit_ch));

		WRITE_REG_ARRAY_I(mod_info, BL_OFFSET_GAIN_12BIT_CH, ch,
						  bl_offset_gain_12bit_ch);
	}
}

static void isp_hw_ge_loc(FW_EST_MOD_INFO mod_info)
{
	int bit_fract_ge_loc_rate = 12;

	int ge_loc_th = READ_REG(mod_info, GE_LOC_TH);
	int ge_loc_d0 = READ_REG(mod_info, GE_LOC_D0);
	int ge_loc_d1 = READ_REG(mod_info, GE_LOC_D1);

	int ge_loc_rate =
		(int)((float)ge_loc_th / (float)(ge_loc_d1 - ge_loc_d0) *
			  (float)(1 << bit_fract_ge_loc_rate) +
			  0.5);

	WRITE_REG(mod_info, GE_LOC_RATE, ge_loc_rate);
}

static void isp_hw_snr_thd(FW_EST_MOD_INFO mod_info)
{
	int bit_fract_alpha_weight_slope = 10;

	int snr_ch_r_feat_lp_g_th[5];
	int snr_ch_r_feat_lp_g_wt[5];
	int snr_ch_r_feat_lp_g_slp[4];

	int snr_ch_g_feat_lp_g_th[5];
	int snr_ch_g_feat_lp_g_wt[5];
	int snr_ch_g_feat_lp_g_slp[4];

	int snr_ch_b_feat_lp_g_th[5];
	int snr_ch_b_feat_lp_g_wt[5];
	int snr_ch_b_feat_lp_g_slp[4];

	int snr_ch_r_feat_hp_g_th[5];
	int snr_ch_r_feat_hp_g_wt[5];
	int snr_ch_r_feat_hp_g_slp[4];

	int snr_ch_g_feat_hp_g_th[5];
	int snr_ch_g_feat_hp_g_wt[5];
	int snr_ch_g_feat_hp_g_slp[4];

	int snr_ch_b_feat_hp_g_th[5];
	int snr_ch_b_feat_hp_g_wt[5];
	int snr_ch_b_feat_hp_g_slp[4];

	for (int i = 0; i < 5; i++) {
		snr_ch_r_feat_lp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_R_FEAT_LP_G_TH, i);
		snr_ch_r_feat_lp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_R_FEAT_LP_G_WT, i);

		snr_ch_g_feat_lp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_G_FEAT_LP_G_TH, i);
		snr_ch_g_feat_lp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_G_FEAT_LP_G_WT, i);

		snr_ch_b_feat_lp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_B_FEAT_LP_G_TH, i);
		snr_ch_b_feat_lp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_B_FEAT_LP_G_WT, i);

		snr_ch_r_feat_hp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_R_FEAT_HP_G_TH, i);
		snr_ch_r_feat_hp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_R_FEAT_HP_G_WT, i);

		snr_ch_g_feat_hp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_G_FEAT_HP_G_TH, i);
		snr_ch_g_feat_hp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_G_FEAT_HP_G_WT, i);

		snr_ch_b_feat_hp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_B_FEAT_HP_G_TH, i);
		snr_ch_b_feat_hp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, SNR_CH_B_FEAT_HP_G_WT, i);
	}

	diff_slope_reg_from_th_n_level(snr_ch_r_feat_lp_g_th,
								   snr_ch_r_feat_lp_g_wt,
								   snr_ch_r_feat_lp_g_slp,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(snr_ch_g_feat_lp_g_th,
								   snr_ch_g_feat_lp_g_wt,
								   snr_ch_g_feat_lp_g_slp,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(snr_ch_b_feat_lp_g_th,
								   snr_ch_b_feat_lp_g_wt,
								   snr_ch_b_feat_lp_g_slp,
								   bit_fract_alpha_weight_slope);

	diff_slope_reg_from_th_n_level(snr_ch_r_feat_hp_g_th,
								   snr_ch_r_feat_hp_g_wt,
								   snr_ch_r_feat_hp_g_slp,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(snr_ch_g_feat_hp_g_th,
								   snr_ch_g_feat_hp_g_wt,
								   snr_ch_g_feat_hp_g_slp,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(snr_ch_b_feat_hp_g_th,
								   snr_ch_b_feat_hp_g_wt,
								   snr_ch_b_feat_hp_g_slp,
								   bit_fract_alpha_weight_slope);

	for (int i = 0; i < 4; i++) {
		WRITE_REG_ARRAY_I(mod_info, SNR_CH_R_FEAT_LP_G_SLP, i,
						  snr_ch_r_feat_lp_g_slp[i]);
		WRITE_REG_ARRAY_I(mod_info, SNR_CH_G_FEAT_LP_G_SLP, i,
						  snr_ch_g_feat_lp_g_slp[i]);
		WRITE_REG_ARRAY_I(mod_info, SNR_CH_B_FEAT_LP_G_SLP, i,
						  snr_ch_b_feat_lp_g_slp[i]);

		WRITE_REG_ARRAY_I(mod_info, SNR_CH_R_FEAT_HP_G_SLP, i,
						  snr_ch_r_feat_hp_g_slp[i]);
		WRITE_REG_ARRAY_I(mod_info, SNR_CH_G_FEAT_HP_G_SLP, i,
						  snr_ch_g_feat_hp_g_slp[i]);
		WRITE_REG_ARRAY_I(mod_info, SNR_CH_B_FEAT_HP_G_SLP, i,
						  snr_ch_b_feat_hp_g_slp[i]);
	}
}

static void isp_hw_ge_thd(FW_EST_MOD_INFO mod_info)
{
	int bit_fract_alpha_weight_slope = 10;

	int gecur_feat_lp_g_th[5];
	int gecur_feat_lp_g_wt[5];
	int gecur_feat_lp_g_slp[4];

	int gecur_feat_hp_g_th[5];
	int gecur_feat_hp_g_wt[5];
	int gecur_feat_hp_g_slp[4];

	for (int i = 0; i < 5; i++) {
		gecur_feat_lp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, GECUR_FEAT_LP_G_TH, i);
		gecur_feat_lp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, GECUR_FEAT_LP_G_WT, i);

		gecur_feat_hp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, GECUR_FEAT_HP_G_TH, i);
		gecur_feat_hp_g_wt[i] =
			READ_REG_ARRAY_I(mod_info, GECUR_FEAT_HP_G_WT, i);
	}

	diff_slope_reg_from_th_n_level(gecur_feat_lp_g_th, gecur_feat_lp_g_wt,
								   gecur_feat_lp_g_slp,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(gecur_feat_hp_g_th, gecur_feat_hp_g_wt,
								   gecur_feat_hp_g_slp,
								   bit_fract_alpha_weight_slope);

	for (int i = 0; i < 4; i++) {
		WRITE_REG_ARRAY_I(mod_info, GECUR_FEAT_LP_G_SLP, i,
						  gecur_feat_lp_g_slp[i]);
		WRITE_REG_ARRAY_I(mod_info, GECUR_FEAT_HP_G_SLP, i,
						  gecur_feat_hp_g_slp[i]);
	}
}

static void isp_hw_md_thd(FW_EST_MOD_INFO mod_info)
{
	int bit_fract_alpha_weight_slope = 10;

	int md_diff_lp_scope_rb_th[5];
	int md_diff_lp_scope_rb_weight[5];
	int md_diff_lp_scope_rb_slope[4];
	int md_diff_lp_scope_g_th[5];
	int md_diff_lp_scope_g_weight[5];
	int md_diff_lp_scope_g_slope[4];
	int md_diff_lp_scope_gbgr_th[5];
	int md_diff_lp_scope_gbgr_weight[5];
	int md_diff_lp_scope_gbgr_slope[4];

	int md_diff_hp_scope_rb_th[5];
	int md_diff_hp_scope_rb_weight[5];
	int md_diff_hp_scope_rb_slope[4];
	int md_diff_hp_scope_g_th[5];
	int md_diff_hp_scope_g_weight[5];
	int md_diff_hp_scope_g_slope[4];
	int md_diff_hp_scope_gbgr_th[5];
	int md_diff_hp_scope_gbgr_weight[5];
	int md_diff_hp_scope_gbgr_slope[4];

	for (int i = 0; i < 5; i++) {
		md_diff_lp_scope_rb_th[i] =
			READ_REG_ARRAY_I(mod_info, MD_DIFF_LP_SCOPE_RB_TH, i);
		md_diff_lp_scope_rb_weight[i] = READ_REG_ARRAY_I(
											mod_info, MD_DIFF_LP_SCOPE_RB_WEIGHT, i);

		md_diff_lp_scope_g_th[i] =
			READ_REG_ARRAY_I(mod_info, MD_DIFF_LP_SCOPE_G_TH, i);
		md_diff_lp_scope_g_weight[i] = READ_REG_ARRAY_I(
										   mod_info, MD_DIFF_LP_SCOPE_G_WEIGHT, i);

		md_diff_lp_scope_gbgr_th[i] =
			READ_REG_ARRAY_I(mod_info, MD_DIFF_LP_SCOPE_GBGR_TH, i);
		md_diff_lp_scope_gbgr_weight[i] = READ_REG_ARRAY_I(
											  mod_info, MD_DIFF_LP_SCOPE_GBGR_WEIGHT, i);

		md_diff_hp_scope_rb_th[i] =
			READ_REG_ARRAY_I(mod_info, MD_DIFF_HP_SCOPE_RB_TH, i);
		md_diff_hp_scope_rb_weight[i] = READ_REG_ARRAY_I(
											mod_info, MD_DIFF_HP_SCOPE_RB_WEIGHT, i);

		md_diff_hp_scope_g_th[i] =
			READ_REG_ARRAY_I(mod_info, MD_DIFF_HP_SCOPE_G_TH, i);
		md_diff_hp_scope_g_weight[i] = READ_REG_ARRAY_I(
										   mod_info, MD_DIFF_HP_SCOPE_G_WEIGHT, i);

		md_diff_hp_scope_gbgr_th[i] =
			READ_REG_ARRAY_I(mod_info, MD_DIFF_HP_SCOPE_GBGR_TH, i);
		md_diff_hp_scope_gbgr_weight[i] = READ_REG_ARRAY_I(
											  mod_info, MD_DIFF_HP_SCOPE_GBGR_WEIGHT, i);
	}

	diff_slope_reg_from_th_n_level(md_diff_lp_scope_rb_th,
								   md_diff_lp_scope_rb_weight,
								   md_diff_lp_scope_rb_slope,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(md_diff_lp_scope_g_th,
								   md_diff_lp_scope_g_weight,
								   md_diff_lp_scope_g_slope,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(md_diff_lp_scope_gbgr_th,
								   md_diff_lp_scope_gbgr_weight,
								   md_diff_lp_scope_gbgr_slope,
								   bit_fract_alpha_weight_slope);

	diff_slope_reg_from_th_n_level(md_diff_hp_scope_rb_th,
								   md_diff_hp_scope_rb_weight,
								   md_diff_hp_scope_rb_slope,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(md_diff_hp_scope_g_th,
								   md_diff_hp_scope_g_weight,
								   md_diff_hp_scope_g_slope,
								   bit_fract_alpha_weight_slope);
	diff_slope_reg_from_th_n_level(md_diff_hp_scope_gbgr_th,
								   md_diff_hp_scope_gbgr_weight,
								   md_diff_hp_scope_gbgr_slope,
								   bit_fract_alpha_weight_slope);

	for (int i = 0; i < 4; i++) {
		WRITE_REG_ARRAY_I(mod_info, MD_DIFF_LP_SCOPE_RB_SLOPE, i,
						  md_diff_lp_scope_rb_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, MD_DIFF_LP_SCOPE_G_SLOPE, i,
						  md_diff_lp_scope_g_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, MD_DIFF_LP_SCOPE_GBGR_SLOPE, i,
						  md_diff_lp_scope_gbgr_slope[i]);

		WRITE_REG_ARRAY_I(mod_info, MD_DIFF_HP_SCOPE_RB_SLOPE, i,
						  md_diff_hp_scope_rb_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, MD_DIFF_HP_SCOPE_G_SLOPE, i,
						  md_diff_hp_scope_g_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, MD_DIFF_HP_SCOPE_GBGR_SLOPE, i,
						  md_diff_hp_scope_gbgr_slope[i]);
	}
}

static void isp_hw_md_lpf(FW_EST_MOD_INFO mod_info)
{
	int mot_confidence_rate0 =
		READ_REG(mod_info, MD_MOT_PRCS_MOT_CONFIDENCE_RATE0);
	int mot_confidence_rate1 =
		READ_REG(mod_info, MD_MOT_PRCS_MOT_CONFIDENCE_RATE1);
	int mot_confidence_thd0 =
		READ_REG(mod_info, MD_MOT_PRCS_MOT_CONFIDENCE_THD0);
	int mot_confidence_thd1 =
		READ_REG(mod_info, MD_MOT_PRCS_MOT_CONFIDENCE_THD1);

	int mean_partial_rate0 =
		READ_REG(mod_info, MD_MOT_PRCS_MEAN_PARTIAL_RATE0);
	int mean_partial_rate1 =
		READ_REG(mod_info, MD_MOT_PRCS_MEAN_PARTIAL_RATE1);
	int mean_lft_rgt_diff_thd0 =
		READ_REG(mod_info, MD_MOT_PRCS_MEAN_LFT_RGT_DIFF_THD0);
	int mean_lft_rgt_diff_thd1 =
		READ_REG(mod_info, MD_MOT_PRCS_MEAN_LFT_RGT_DIFF_THD1);

	int mot_confidence_rate_slope =
		(int)((float)(mot_confidence_rate0 - mot_confidence_rate1) /
			  (float)(mot_confidence_thd1 - mot_confidence_thd0) *
			  (float)(1 << 10));
	int mean_partial_rate_slope =
		(int)((float)(mean_partial_rate1 - mean_partial_rate0) /
			  (float)(mean_lft_rgt_diff_thd1 - mean_lft_rgt_diff_thd0) *
			  (float)(1 << 10));

	WRITE_REG(mod_info, MD_MOT_PRCS_MOT_CONFIDENCE_RATE_SLOPE,
			  mot_confidence_rate_slope);
	WRITE_REG(mod_info, MD_MOT_PRCS_MEAN_PARTIAL_RATE_SLOPE,
			  mean_partial_rate_slope);
}

static void isp_hw_detail(FW_EST_MOD_INFO mod_info)
{
	int bit_fract_thedge_slope = 11;
	int bit_fract_edgeadjust_slope = 8;

	int detail_edgeness[4];
	int detail_nr_adjust[4];
	int detail_nr_edgeadjust_slope[3];
	int detail_ge_adjust_pre[4];
	int detail_ge_edgeadjust_slope_pre[3];
	int detail_ge_adjust_cur[4];
	int detail_ge_edgeadjust_slope_cur[3];

	int detail_hp_rb_th[4];
	int detail_hp_rb_thedge_slope[3];
	int detail_hp_g_th[4];
	int detail_hp_g_thedge_slope[3];
	int detail_hp_gbgr_th[4];
	int detail_hp_gbgr_thedge_slope[3];
	int detail_mp_rb_th[4];
	int detail_mp_rb_thedge_slope[3];
	int detail_mp_g_th[4];
	int detail_mp_g_thedge_slope[3];

	for (int i = 0; i < 4; i++) {
		detail_edgeness[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_EDGENESS, i);
		detail_nr_adjust[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_NR_ADJUST, i);
		detail_ge_adjust_pre[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_GE_ADJUST_PRE, i);
		detail_ge_adjust_cur[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_GE_ADJUST_CUR, i);

		detail_hp_rb_th[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_HP_RB_TH, i);
		detail_hp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_HP_G_TH, i);
		detail_hp_gbgr_th[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_HP_GBGR_TH, i);
		detail_mp_rb_th[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_MP_RB_TH, i);
		detail_mp_g_th[i] =
			READ_REG_ARRAY_I(mod_info, DETAIL_MP_G_TH, i);
	}

	reg3917_detail3917_edgeslope(detail_hp_rb_th, detail_edgeness,
								 detail_hp_rb_thedge_slope,
								 bit_fract_thedge_slope);
	reg3917_detail3917_edgeslope(detail_hp_g_th, detail_edgeness,
								 detail_hp_g_thedge_slope,
								 bit_fract_thedge_slope);
	reg3917_detail3917_edgeslope(detail_hp_gbgr_th, detail_edgeness,
								 detail_hp_gbgr_thedge_slope,
								 bit_fract_thedge_slope);
	reg3917_detail3917_edgeslope(detail_mp_rb_th, detail_edgeness,
								 detail_mp_rb_thedge_slope,
								 bit_fract_thedge_slope);
	reg3917_detail3917_edgeslope(detail_mp_g_th, detail_edgeness,
								 detail_mp_g_thedge_slope,
								 bit_fract_thedge_slope);

	reg3917_detail3917_adjustslope(detail_edgeness, detail_nr_adjust,
								   detail_nr_edgeadjust_slope,
								   bit_fract_edgeadjust_slope);
	reg3917_detail3917_adjustslope(detail_edgeness, detail_ge_adjust_pre,
								   detail_ge_edgeadjust_slope_pre,
								   bit_fract_edgeadjust_slope);
	reg3917_detail3917_adjustslope(detail_edgeness, detail_ge_adjust_cur,
								   detail_ge_edgeadjust_slope_cur,
								   bit_fract_edgeadjust_slope);

	for (int i = 0; i < 3; i++) {
		WRITE_REG_ARRAY_I(mod_info, DETAIL_NR_EDGEADJUST_SLOPE, i,
						  detail_nr_edgeadjust_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, DETAIL_GE_EDGEADJUST_SLOPE_PRE, i,
						  detail_ge_edgeadjust_slope_pre[i]);
		WRITE_REG_ARRAY_I(mod_info, DETAIL_GE_EDGEADJUST_SLOPE_CUR, i,
						  detail_ge_edgeadjust_slope_cur[i]);

		WRITE_REG_ARRAY_I(mod_info, DETAIL_HP_RB_THEDGE_SLOPE, i,
						  detail_hp_rb_thedge_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, DETAIL_HP_G_THEDGE_SLOPE, i,
						  detail_hp_g_thedge_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, DETAIL_HP_GBGR_THEDGE_SLOPE, i,
						  detail_hp_gbgr_thedge_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, DETAIL_MP_RB_THEDGE_SLOPE, i,
						  detail_mp_rb_thedge_slope[i]);
		WRITE_REG_ARRAY_I(mod_info, DETAIL_MP_G_THEDGE_SLOPE, i,
						  detail_mp_g_thedge_slope[i]);
	}
}

static void isp_hw_mask_sym(FW_EST_MOD_INFO mod_info)
{
	int patch_snr_len = 5;
	int mask_coef_len = (patch_snr_len - 1) / 2;

	int bit_fract_mask_coef_slope = 12;

	int mask_static_level_thd[5];

	int mask_nr_tex_small_coef_sym[5][2];
	int mask_nr_tex_small_slope_sym[4][2];

	int mask_nr_clr_small_coef_sym[5][2];
	int mask_nr_clr_small_slope_sym[4][2];

	int mask_ge_small_coef_sym[5][2];
	int mask_ge_small_slope_sym[4][2];

	for (int rowIdx = 0; rowIdx < 5; rowIdx++) {
		mask_static_level_thd[rowIdx] = READ_REG_ARRAY_I(
											mod_info, MASK_STATIC_LEVEL_THD, rowIdx);

		for (int colIdx = 0; colIdx < 2; colIdx++) {
			mask_nr_tex_small_coef_sym[rowIdx][colIdx] =
				READ_REG_ARRAY_I(mod_info,
								 MASK_NR_TEX_SMALL_COEF_SYM,
								 rowIdx * 2 + colIdx);

			mask_nr_clr_small_coef_sym[rowIdx][colIdx] =
				READ_REG_ARRAY_I(mod_info,
								 MASK_NR_CLR_SMALL_COEF_SYM,
								 rowIdx * 2 + colIdx);

			mask_ge_small_coef_sym[rowIdx][colIdx] =
				READ_REG_ARRAY_I(mod_info,
								 MASK_GE_SMALL_COEF_SYM,
								 rowIdx * 2 + colIdx);
		}
	}

	reg3917_mask_coef_slope(
		/*th_num*/ 5, /*coef_num*/ mask_coef_len, mask_static_level_thd,
		mask_nr_tex_small_coef_sym, mask_nr_tex_small_slope_sym,
		bit_fract_mask_coef_slope); // -> mask_ld_large_slope_x

	reg3917_mask_coef_slope(
		/*th_num*/ 5, /*coef_num*/ mask_coef_len, mask_static_level_thd,
		mask_nr_clr_small_coef_sym, mask_nr_clr_small_slope_sym,
		bit_fract_mask_coef_slope); // -> mask_ld_large_slope_x

	reg3917_mask_coef_slope(
		/*th_num*/ 5, /*coef_num*/ mask_coef_len, mask_static_level_thd,
		mask_ge_small_coef_sym, mask_ge_small_slope_sym,
		bit_fract_mask_coef_slope); // -> mask_ld_large_slope_x

	for (int rowIdx = 0; rowIdx < 4; rowIdx++) {
		for (int colIdx = 0; colIdx < 2; colIdx++) {
			WRITE_REG_ARRAY_I(
				mod_info, MASK_NR_TEX_SMALL_SLOPE_SYM,
				rowIdx * 2 + colIdx,
				mask_nr_tex_small_slope_sym[rowIdx][colIdx]);

			WRITE_REG_ARRAY_I(
				mod_info, MASK_NR_CLR_SMALL_SLOPE_SYM,
				rowIdx * 2 + colIdx,
				mask_nr_clr_small_slope_sym[rowIdx][colIdx]);

			WRITE_REG_ARRAY_I(
				mod_info, MASK_GE_SMALL_SLOPE_SYM,
				rowIdx * 2 + colIdx,
				mask_ge_small_slope_sym[rowIdx][colIdx]);
		}
	}
}

static void isp_hw_mi(FW_EST_MOD_INFO mod_info)
{
	const int bit_fract_alpha = 8; // precision 1.8
	const int bit_fract_mi_alphamax_ctrl_slp = 14;

	int mi_alphamax_ctrl_thd = READ_REG(mod_info, MI_ALPHAMAX_CTRL_THD);

	int mi_alphamax_ctrl_slp =
		(int)((float)(1 << bit_fract_mi_alphamax_ctrl_slp) /
			  (float)((1 << bit_fract_alpha) -
					  mi_alphamax_ctrl_thd) +
			  0.5);

	WRITE_REG(mod_info, MI_ALPHAMAX_CTRL_SLP, mi_alphamax_ctrl_slp);
}

static int isp_hw_tnr_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	/* -------------- black level -------------- */
	isp_hw_bl(mod_info);

	/* -------------- ge loc -------------- */
	isp_hw_ge_loc(mod_info);

	/* -------------- snr/ge thd -------------- */
	//isp_hw_snr_ge_thd(mod_info); // ge + snr

#ifdef REG_SNR_WT_SHARED
	isp_hw_snr_thd_wt_shared(mod_info); // tnr 3.0
#else
	isp_hw_snr_thd(mod_info); // tnr 2.0
#endif

#ifdef REG_GE_WT_SHARED
	isp_hw_ge_thd_wt_shared(mod_info); // tnr 3.0
#else
	isp_hw_ge_thd(mod_info); // tnr 2.0
#endif

	/* -------------- md thd -------------- */
#ifdef REG_MD_WT_SHARED
	isp_hw_md_thd_wt_shared(mod_info); // tnr 3.0
#else
	isp_hw_md_thd(mod_info); // tnr 2.0
#endif

	/* -------------- md lpf -------------- */
	isp_hw_md_lpf(mod_info);

	/* -------------- detail -------------- */
	isp_hw_detail(mod_info);

	/* -------------- mask -------------- */
#ifdef REG_MASK_SYM
	isp_hw_mask_sym(mod_info); // tnr 3.0, mask symmetry
#else
	isp_hw_mask(mod_info); // tnr 2.0, mask_x, mask_y
#endif

	/* -------------- motion info -------------- */
	isp_hw_mi(mod_info);

	return 0;
}

static int isp_hw_tnr_enable(uint32_t isp_id, int enable)
{
	if (enable) {
		isp_write_reg(2, TNR_RAW3DNR_MODE);
	} else {
		isp_write_reg(0, TNR_RAW3DNR_MODE);
	}
	return RTS_ISP_OK;
}

static int isp_hw_tnr_enable_axi(uint32_t isp_id, int enable)
{
	if (enable) {
		isp_write_reg(0x7, TNR_AXI_CLR);
		isp_write_reg(TPNR_EN_MASK, TNR_STREAM_EN);
	} else {
		int count = 10;

		isp_set_reg_bit(TPNR_STOP_OFFSET, TNR_STREAM_EN);
		while (isp_read_reg(TNR_AXI_STATE) && count--) {
			isp_os_usleep(1000);
		}
		if (count < 0) {
			return -RTS_ISP_EBUSY;
		}
	}

	return RTS_ISP_OK;
}

static int isp_hw_tnr_set_snr_depth(uint32_t isp_id, int32_t bit_depth)
{
	uint32_t val;

	switch (bit_depth) {
	case 8:
		val = 0;
		break;
	case 10:
		val = 1;
		break;
	case 12:
		val = 2;
		break;
	default:
		val = 0;
		break;
	}
	isp_write_reg(val, TNR_SENSOR_BIT_MODE);
	return RTS_ISP_OK;
}

static int isp_hw_tnr_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_tnr_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(TPNR_REG_LOAD_MASK, TNR_RAW3DNR_CTRL);

	return RTS_ISP_OK;
}

static const struct isp_hw_tnr_ops hw_tnr_ops = {
	.enable = isp_hw_tnr_enable,
	.enable_axi = isp_hw_tnr_enable_axi,
	.set_snr_depth = isp_hw_tnr_set_snr_depth,
	.est_dynamic = isp_hw_tnr_estimation,
};

const struct isp_hw_tnr_ops *isp_hw_tnr_get_ops(void)
{
	return &hw_tnr_ops;
}
