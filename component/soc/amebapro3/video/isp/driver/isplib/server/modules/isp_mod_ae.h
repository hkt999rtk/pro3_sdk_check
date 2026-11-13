#ifndef _ISP_MOD_AE_H_INC_
#define _ISP_MOD_AE_H_INC_

#include <rts_isp.h>
#include <rts_isp_ae.h>
#include <rts_isp_other.h>
#include <isp_utils_common.h>

struct isp_mod_ae_hdr_stat_info {
	uint8_t win_mean_size;
	uint8_t hist_bin_size;
};

struct isp_mod_ae_stat_info {
	struct rts_isp_ae_stat_info info;
	uint8_t win_mean_size;
	uint8_t hist_bin_size;
	struct isp_mod_ae_hdr_stat_info hdr[RTS_ISP_HDR_CHAN_MAX];
};

enum isp_mod_ae_get_info_type {
	AE_INFO_STATIS =
		isp_action_r(INFO_TYPE, 0, struct isp_mod_ae_stat_info),
	AE_INFO_EXPOSURE = isp_action_r(INFO_TYPE, 1, float),
	AE_INFO_STATUS = isp_action_r(INFO_TYPE, 2, int32_t),
	AE_INFO_GAIN = isp_action_r(INFO_TYPE, 3, float),
	AE_INFO_VERSION = isp_action_r(INFO_TYPE, 4, uint16_t),
	AE_INFO_TARGET = isp_action_r(INFO_TYPE, 5, uint16_t),
};

struct isp_mod_ae_sync_regs {
	struct rts_isp_sync_reg *reg;
	uint32_t num;
	uint32_t delay;
};

enum isp_mod_ae_exec_type {
	AE_EXEC_SET_PLFREQ = isp_action_w(EXEC_TYPE, 0, int32_t),
	AE_EXEC_GET_SETTING =
		isp_action_wr(EXEC_TYPE, 1, struct rts_isp_ae_setting),
	AE_EXEC_SET_SETTING =
		isp_action_wr(EXEC_TYPE, 2, struct rts_isp_ae_setting),
	AE_EXEC_SET_TARGET_DELTA = isp_action_wr(EXEC_TYPE, 3, int),
	AE_EXEC_GET_ROI_AREAS =
		isp_action_r(EXEC_TYPE, 4, struct rts_isp_ae_roi_areas),
	AE_EXEC_SET_ROI_AREAS =
		isp_action_w(EXEC_TYPE, 5, struct rts_isp_ae_roi_areas),
	AE_EXEC_REPORT_EXP_GAIN =
		isp_action_wr(EXEC_TYPE, 6, struct rts_isp_exp_gain_report),
	AE_EXEC_SET_RATIO_RANGE =
		isp_action_w(EXEC_TYPE, 7, struct rts_isp_ratio_range),
	AE_EXEC_FILL_SYNC_REGS =
		isp_action_wr(EXEC_TYPE, 8, struct isp_mod_ae_sync_regs),
	AE_EXEC_SET_RESULT_DELAY = isp_action_w(EXEC_TYPE, 9, uint32_t),
};

#endif /* _ISP_MOD_AE_H_INC_ */
