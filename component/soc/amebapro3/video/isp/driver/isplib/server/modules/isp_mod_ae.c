/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <rts_isp.h>
#include <rts_isp_ae.h>
#include <isp_os_driver.h>
#include <isp_top.h>
#include <isp_core.h>
#include <isp_mod.h>
#include <isp_algo.h>
#include <isp_notify.h>
#include <isp_statis.h>
#include <isp_v4l2_ctrl.h>
#include <isp_iq.h>
#include <isp_hw_ae.h>
#include "isp_mod_sensor.h"
#include "isp_mod_awb.h"
#include "isp_mod_iq.h"
#include "isp_mod_ae.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_AE
#define ae_algo_ctrl(ae, ...) ae->ae_algo->ctrl(ae->mod.owner_id, ##__VA_ARGS__)

struct isp_mod_ae {
	struct isp_mod mod;
	const struct rts_isp_ae_algo *ae_algo;
	const struct isp_hw_ae_ops *hw_ops;
	struct isp_ae_hw_info hw_info;

	int32_t hdr; /* enum rts_isp_hdr_mode */
	float max_fps;
	float min_fps;
	float max_fps_mode;
	float exposure_step;
	int32_t power_line_freq; /* enum rts_isp_power_line_freq */
	const struct rts_isp_exp_gain_tune_ops *tune_ops;
	int algo_delay;
	unsigned int exp_gain_reported : 1;

	struct rts_isp_ae_exp_gain exp_gain;
	struct rts_isp_ae_exp_gain exp_gain_set;
	int dyn_hdr_changed;
	float dyn_fps;
	float exp_ratio[RTS_ISP_HDR_CHAN_MAX - 1];
	float ratio[RTS_ISP_HDR_CHAN_MAX - 1];
	uint32_t contrast;
	struct rts_isp_ae_apex apex;

	struct {
		struct rts_isp_ae_auto_setting auto_setting;
		int auto_setting_has_set;
	};
	int patch_target_delta;

	struct {
		struct rts_isp_ae_ratio_range ratio_range;
		int ratio_range_has_set;
	};

	const isp_iq_ae_v1_t *iq;
	isp_algo_param_ae_v1_t *algo_param;

	rts_isp_grid_t grid;
	struct rts_isp_ae_roi_areas roi_areas;

	struct {
		int32_t ae_status; /* enum rts_isp_ae_algo_status */
		uint8_t ae_stable_cnt;
	};

	struct {
		struct v4l2_ctrl *auto_gain_ctrl;
		struct v4l2_ctrl *exposure_ctrl;
		struct v4l2_ctrl *gain_ctrl;
	};
	struct v4l2_ctrl *exp_priority_ctrl;
	struct v4l2_ctrl *dyn_fps_ctrl;
	struct v4l2_ctrl *hdr_ctrl;
	struct {
		struct v4l2_ctrl *auto_hdr_ratio_ctrl;
		struct v4l2_ctrl *hdr_ratio_ctrl;
	};
	struct v4l2_ctrl *smart_ir_mode_ctrl;
	struct v4l2_ctrl *smart_ir_manual_level_ctrl;
};

static int ae_algo_set_ctrls(struct isp_mod_ae *ae);

static int check_algo_version(uint32_t version)
{
	if (algo_magic(version) != AE_API_VERSION_MAGIC ||
		algo_major_version(version) != AE_API_MAJOR_VERSION ||
		algo_minor_version(version) > AE_API_MINOR_VERSION) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_algo(const struct rts_isp_ae_algo *ae_algo)
{
	if (!ae_algo || check_algo_version(ae_algo->api_version)) {
		return -RTS_ISP_EINVAL;
	}
	if (!ae_algo->init || !ae_algo->cleanup) {
		return -RTS_ISP_EINVAL;
	}
	if (!ae_algo->run || !ae_algo->ctrl) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static uint32_t ae_algo_version(struct isp_mod_ae *ae)
{
	return ae->ae_algo->api_version & AE_VERSION_MASK;
}

static int ae_check_exp_gain(struct isp_mod_ae *ae,
							 const struct rts_isp_ae_exp_gain *exp_gain)
{
	int i;

	if (!ae || !exp_gain) {
		return -RTS_ISP_EINVAL;
	}
	for (i = 0; i < hdr_chan_num(ae->hdr); i++) {
		if (!isnormal(exp_gain->exposure[i]) ||
			!isnormal(exp_gain->analog_gain[i]) ||
			!isnormal(exp_gain->digital_gain[i]) ||
			!isnormal(exp_gain->isp_hdr_gain[i]) ||
			exp_gain->exposure[i] < ae->exposure_step ||
			exp_gain->analog_gain[i] < 1.0f ||
			exp_gain->digital_gain[i] < 1.0f ||
			exp_gain->isp_hdr_gain[i] < 1.0f) {
			return -RTS_ISP_EINVAL;
		}
	}
	if (!isnormal(exp_gain->isp_gain) || exp_gain->isp_gain < 1.0f) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static float ae_total_gain(struct isp_mod_ae *ae)
{
	return ae->exp_gain.analog_gain[0] * ae->exp_gain.digital_gain[0] *
		   ae->exp_gain.isp_hdr_gain[0] * ae->exp_gain.isp_gain;
}

static float ae_total_ratio(struct isp_mod_ae *ae)
{
	int i;
	float ratio = 1;

	for (i = 0; i < hdr_chan_num(ae->hdr) - 1; i++) {
		ratio *= ae->exp_ratio[i];
	}
	return ratio;
}

static void ae_set_exp_gain_unused(struct isp_mod_ae *ae,
								   struct rts_isp_ae_exp_gain *exp_gain)
{
	int i;

	for (i = hdr_chan_num(ae->hdr); i < RTS_ISP_HDR_CHAN_MAX; i++) {
		exp_gain->exposure[i] = exp_gain->exposure[i - 1];
		exp_gain->analog_gain[i] = exp_gain->analog_gain[i - 1];
		exp_gain->digital_gain[i] = exp_gain->digital_gain[i - 1];
		exp_gain->isp_hdr_gain[i] = exp_gain->isp_hdr_gain[i - 1];
	}
}

static int ae_tune_exposure_gain(struct isp_mod_ae *ae,
								 struct rts_isp_ae_exp_gain *exp_gain)
{
	int i;
	int ret;
	float min_hdr_gain;
	float gain_extend = 1;
	int num = hdr_chan_num(ae->hdr);
	float min_exposure[RTS_ISP_HDR_CHAN_MAX];
	float max_exposure[RTS_ISP_HDR_CHAN_MAX];
	float gain[RTS_ISP_HDR_CHAN_MAX];

	if (ae_check_exp_gain(ae, exp_gain)) {
		return -RTS_ISP_EINVAL;
	}

	ret = ae->tune_ops->get_exposure_range(ae->mod.owner_id, ae->dyn_fps,
										   ae->exp_ratio, min_exposure,
										   max_exposure);
	if (ret) {
		return ret;
	}

	isp_memcpy_array(gain, exp_gain->analog_gain, num);
	if (exp_gain->exposure[0] > max_exposure[0]) {
		gain_extend = exp_gain->exposure[0] / max_exposure[0];
		exp_gain->exposure[0] = max_exposure[0];
		for (i = 0; i < num; i++) {
			gain[i] *= gain_extend;
		}
	}
	exp_gain->exposure[0] =
		clamp(exp_gain->exposure[0], min_exposure[0], max_exposure[0]);
	for (i = 1; i < num; i++)
		exp_gain->exposure[i] =
			clamp(exp_gain->exposure[i - 1] / ae->exp_ratio[i - 1],
				  min_exposure[i], max_exposure[i]);

	isp_memcpy_array(exp_gain->analog_gain, gain, num);
	ret = ae->tune_ops->get_tuned_again(ae->mod.owner_id,
										exp_gain->analog_gain);
	if (ret) {
		return ret;
	}
	for (i = 0; i < num; i++) {
		exp_gain->analog_gain[i] = max(1.0f, exp_gain->analog_gain[i]);
		exp_gain->digital_gain[i] *= gain[i] / exp_gain->analog_gain[i];
	}
	isp_memcpy_array(gain, exp_gain->digital_gain, num);
	ret = ae->tune_ops->get_tuned_dgain(ae->mod.owner_id,
										exp_gain->digital_gain);
	if (ret) {
		return ret;
	}
	for (i = 0; i < num; i++) {
		exp_gain->digital_gain[i] =
			max(1.0f, exp_gain->digital_gain[i]);
		exp_gain->isp_hdr_gain[i] *=
			gain[i] / exp_gain->digital_gain[i];
	}
	min_hdr_gain = exp_gain->isp_hdr_gain[0];
	for (i = 1; i < num; i++) {
		min_hdr_gain = min(min_hdr_gain, exp_gain->isp_hdr_gain[i]);
	}
	for (i = 0; i < num; i++) {
		exp_gain->isp_hdr_gain[i] /= min_hdr_gain;
		exp_gain->isp_hdr_gain[i] =
			max(1.0f, exp_gain->isp_hdr_gain[i]);
	}
	exp_gain->isp_gain *= min_hdr_gain;
	exp_gain->isp_gain = max(1.0f, exp_gain->isp_gain);

	ae_set_exp_gain_unused(ae, exp_gain);

	return RTS_ISP_OK;
}

static int ae_algo_prepare_info(struct isp_mod_ae *ae,
								struct rts_isp_ae_info *info,
								struct rts_isp_ae_iq_info *iq,
								struct rts_isp_ae_param_info *param)
{
	int i;
	const isp_iq_header_t *iq_header;
	const isp_algo_param_header_t *param_header;

	isp_memset(info, 0, sizeof(*info));
	isp_memset(iq, 0, sizeof(*iq));
	isp_memset(param, 0, sizeof(*param));
	info->stat_info.win = ae->hw_info.ae.grid_info.size;
	info->stat_info.hist_bins = ae->hw_info.ae.hist_bins;
	for (i = 0; i < RTS_ISP_HDR_CHAN_MAX; i++) {
		info->stat_info.hdr[i].win = ae->hw_info.hdr[i].grid_info.size;
		info->stat_info.hdr[i].hist_bins = ae->hw_info.hdr[i].hist_bins;
	}

	info->tune = ae->tune_ops;
	if (ae_algo_version(ae) >= 0x0201) {
		iq_header = isp_iq_get_header(ae->mod.owner_id);
		param_header = isp_iq_get_algo_param_header(ae->mod.owner_id);
		if (!iq_header || !param_header) {
			return -RTS_ISP_EINVAL;
		}

		iq->magic = iq_header->magic;
		iq->version = iq_header->version;
		iq->iq = ae->iq;
		info->iq = iq;
		param->magic = param_header->magic;
		param->version = param_header->version;
		param->algo_param = ae->algo_param;
		info->algo_param = param;
	} else {
		info->iq = ae->iq;
		info->algo_param = ae->algo_param;
	}

	return RTS_ISP_OK;
}

static int ae_algo_set_auto(struct isp_mod_ae *ae, int auto_mode)
{
	if (auto_mode) {
		int ret;

		ret = ae_algo_set_ctrls(ae);
		if (ret) {
			return ret;
		}
		return isp_mod_add_notify(&ae->mod, ISP_NOTIFY_STATIS_DONE);
	} else {
		return isp_mod_del_notify(&ae->mod, ISP_NOTIFY_STATIS_DONE);
	}
}

static int ae_algo_init(struct isp_mod_ae *ae)
{
	int ret;
	struct rts_isp_ae_info info;
	struct rts_isp_ae_iq_info iq_info;
	struct rts_isp_ae_param_info param_info;

	ret = isp_algo_get_ae(ae->mod.owner_id, &ae->ae_algo);
	if (ret) {
		return ret;
	}
	ret = check_algo(ae->ae_algo);
	if (ret) {
		return ret;
	}
	ret = ae_algo_prepare_info(ae, &info, &iq_info, &param_info);
	if (ret) {
		return ret;
	}
	return ae->ae_algo->init(ae->mod.owner_id, &info);
}

static int ae_algo_cleanup(struct isp_mod_ae *ae)
{
	if (ae->ae_algo) {
		ae->ae_algo->cleanup(ae->mod.owner_id);
		ae->ae_algo = NULL;
	}
	return RTS_ISP_OK;
}

static int ae_algo_check_roi(struct isp_mod_ae *ae,
							 const struct rts_isp_ae_roi_areas *areas)
{
	int i;
	const int max = 65535;

	for (i = 0; i < ARRAY_SIZE(areas->roi); i++) {
		const struct rts_isp_ae_roi *roi = &areas->roi[i];

		if (roi->level < RTS_AE_ROI_DISABLE ||
			roi->level > RTS_AE_ROI_LEVEL4) {
			return -RTS_ISP_EINVAL;
		}

		if (roi->rect.x < 0 || roi->rect.x > max || roi->rect.y < 0 ||
			roi->rect.y > max || roi->rect.w < 0 || roi->rect.h < 0 ||
			roi->rect.w > max || roi->rect.h > max) {
			return -RTS_ISP_ERANGE;
		}
	}

	return RTS_ISP_OK;
}

static void ae_algo_set_algo_roi(struct isp_mod_ae *ae,
								 const struct rts_isp_ae_roi *roi,
								 struct rts_isp_ae_algo_roi *algo_roi)
{
	rts_isp_prectf_t rect;
	const float cellw = ae->grid.cell.w;
	const float cellh = ae->grid.cell.h;
	const rts_isp_point_t *start = &ae->grid.start;

	rect.left = clamp((roi->rect.x - start->x) / cellw, 0.0f, 16.0f);
	rect.top = clamp((roi->rect.y - start->y) / cellh, 0.0f, 16.0f);
	rect.right = clamp((roi->rect.x + roi->rect.w - start->x) / cellw, 0.0f,
					   16.0f);
	rect.bottom = clamp((roi->rect.y + roi->rect.h - start->y) / cellh,
						0.0f, 16.0f);

	algo_roi->level = roi->level;
	algo_roi->rect.x = rect.left;
	algo_roi->rect.y = rect.top;
	algo_roi->rect.w = rect.right - rect.left;
	algo_roi->rect.h = rect.bottom - rect.top;
}

static int ae_algo_set_roi(struct isp_mod_ae *ae)
{
	int i;
	struct rts_isp_ae_algo_roi_areas areas;

	if (!ae->ae_algo) {
		return RTS_ISP_OK;
	}

	for (i = 0; i < ARRAY_SIZE(ae->roi_areas.roi); i++) {
		ae_algo_set_algo_roi(ae, &ae->roi_areas.roi[i], &areas.roi[i]);
	}

	return ae_algo_ctrl(ae, SET_ROI_AREAS, &areas);
}

static int ae_algo_set_api_ctrls(struct isp_mod_ae *ae,
								 const struct rts_isp_ae_auto_setting *setting)
{
	int ret;
	float gain_max = setting->gain_max / 16.0f;
	float min_fps = setting->min_fps;
	float max_et = setting->exposure_max;

	if (!ae->auto_setting_has_set) {
		return RTS_ISP_OK;
	}
	ret = ae_algo_ctrl(ae, SET_WIN_WEIGHTS, setting->win_weights);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_TARGET_DELTA, setting->target_delta);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_GAIN_MAX, gain_max);
	if (ret) {
		return ret;
	}
	if (min_fps < ae->min_fps) {
		min_fps = ae->min_fps;
	}
	ret = ae_algo_ctrl(ae, SET_MIN_FPS, min_fps);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_EXPOSURE_MAX, max_et);
	return ret;
}

static int ae_algo_set_ctrls(struct isp_mod_ae *ae)
{
	int ret;
	int ratio;

	if (!ae->ratio_range_has_set) {
		ae->ratio_range.min_ratio_total = AE_MIN_RATIO;
		ae->ratio_range.max_ratio_total = AE_MAX_RATIO;
		ae->ratio_range.min_ratio[0] = ae->ratio_range.min_ratio_total;
		ae->ratio_range.max_ratio[0] = ae->ratio_range.max_ratio_total;
		ae->ratio_range.min_ratio[1] = ae->ratio_range.min_ratio[2] = 1;
		ae->ratio_range.max_ratio[1] = ae->ratio_range.max_ratio[2] = 1;
	}
	ratio = ae->auto_hdr_ratio_ctrl->val ? 0 : ae->hdr_ratio_ctrl->val;
	if (ratio >= 256) {
		ratio /= 256;
	}
	ae->exp_gain.hdr = ae->hdr;

	ret = ae_algo_ctrl(ae, SET_BASE_FPS, ae->max_fps);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_MIN_FPS, ae->min_fps);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_EXP_GAIN, &ae->exp_gain);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_EXPOSURE_STEP, ae->exposure_step);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_POWER_LINE_FREQ, ae->power_line_freq);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_DYNAMIC_FPS_ENABLE,
					   ae->exp_priority_ctrl->cur.val);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_DYNAMIC_HDR_MODE, ae->hdr_ctrl->cur.val);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_HDR_RATIO_RANGE, &ae->ratio_range);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_HDR_RATIO, ratio);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_SORT_MODE, ae->smart_ir_mode_ctrl->cur.val);
	if (ret) {
		return ret;
	}
	ret = ae_algo_ctrl(ae, SET_SORT_MANUAL_LEVEL,
					   ae->smart_ir_manual_level_ctrl->cur.val);
	if (ret) {
		return ret;
	}
	ret = ae_algo_set_roi(ae);
	if (ret) {
		return ret;
	}
	return ae_algo_set_api_ctrls(ae, &ae->auto_setting);
}

static int ae_algo_update(struct isp_mod_ae *ae)
{
	int ret;

	ret = ae_algo_cleanup(ae);
	if (ret) {
		return ret;
	}
	ret = ae_algo_init(ae);
	if (ret) {
		return ret;
	}
	return ae_algo_set_ctrls(ae);
}

static int ae_algo_need_delay(struct isp_mod_ae *ae)
{
	if (ae->algo_delay) {
		ae->algo_delay--;
		return RTS_ISP_TRUE;
	}
	return RTS_ISP_FALSE;
}

static int mod_ae_info_statis(struct isp_mod *mod, void *data)
{
	int i;
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct isp_mod_ae_stat_info *info = data;

	info->info.win = ae->hw_info.ae.grid_info.size;
	info->info.hist_bins = ae->hw_info.ae.hist_bins;
	info->win_mean_size = ae->hw_info.ae.y_mean_size;
	info->hist_bin_size = ae->hw_info.ae.hist_bin_size;

	for (i = 0; i < RTS_ISP_HDR_CHAN_MAX; i++) {
		info->info.hdr[i].win = ae->hw_info.hdr[i].grid_info.size;
		info->info.hdr[i].hist_bins = ae->hw_info.hdr[i].hist_bins;
		info->hdr[i].win_mean_size = ae->hw_info.hdr[i].y_mean_size;
		info->hdr[i].hist_bin_size = ae->hw_info.hdr[i].hist_bin_size;
	}

	return RTS_ISP_OK;
}

static int mod_ae_info_exposure(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	*(float *)data = ae->exp_gain.exposure[0];

	return RTS_ISP_OK;
}

static int mod_ae_info_status(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	*(int32_t *)data = ae->ae_status;

	return RTS_ISP_OK;
}

static int mod_ae_info_gain(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	*(float *)data = ae_total_gain(ae);

	return RTS_ISP_OK;
}

static int mod_ae_info_version(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	uint16_t *version = data;

	*version = ae_algo_version(ae);

	return RTS_ISP_OK;
}

static int mod_ae_info_target(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	*(uint16_t *)data = ae->iq->y_mean_target;

	return RTS_ISP_OK;
}

static struct isp_mod_action_info ae_info_actions[] = {
	ISP_MOD_ACTION(AE_INFO_STATIS, mod_ae_info_statis),
	ISP_MOD_ACTION(AE_INFO_EXPOSURE, mod_ae_info_exposure),
	ISP_MOD_ACTION(AE_INFO_STATUS, mod_ae_info_status),
	ISP_MOD_ACTION(AE_INFO_GAIN, mod_ae_info_gain),
	ISP_MOD_ACTION(AE_INFO_VERSION, mod_ae_info_version),
	ISP_MOD_ACTION(AE_INFO_TARGET, mod_ae_info_target),
};

static int ae_notify_exp_gain_change(struct isp_mod_ae *ae)
{
	int i;
	struct isp_notify_dynamic_ae dyn;

	dyn.mode = ae->exp_gain.hdr;
	dyn.num = hdr_chan_num(dyn.mode);
	isp_memcpy_array(dyn.exposure, ae->exp_gain.exposure, dyn.num);
	isp_memcpy_array(dyn.analog_gain, ae->exp_gain.analog_gain, dyn.num);
	isp_memcpy_array(dyn.digital_gain, ae->exp_gain.digital_gain, dyn.num);
	isp_memcpy_array(dyn.isp_hdr_gain, ae->exp_gain.isp_hdr_gain, dyn.num);
	dyn.isp_gain = ae->exp_gain.isp_gain;

	for (i = 0; i < dyn.num; i++) {
		dyn.sensor_gain[i] = dyn.analog_gain[i] * dyn.digital_gain[i];
		dyn.total_gain[i] = (dyn.sensor_gain[i] * dyn.isp_hdr_gain[i] *
							 dyn.isp_gain);
		dyn.iq_sensor_gain[i] = dyn.sensor_gain[i] * 16;
		dyn.iq_total_gain[i] = dyn.total_gain[i] * 16;
		dyn.iq_exp_gain[i] = dyn.exposure[i] * dyn.total_gain[i] / 100;
		if (i)
			dyn.iq_ratio[i] =
				(dyn.exposure[i - 1] * dyn.total_gain[i - 1]) /
				(dyn.exposure[i] * dyn.total_gain[i]);
	}
	dyn.iq_ratio[0] =
		((dyn.exposure[0] * dyn.total_gain[0]) /
		 (dyn.exposure[dyn.num - 1] * dyn.total_gain[dyn.num - 1]));

	for (i = dyn.num; i < RTS_ISP_HDR_CHAN_MAX; i++) {
		dyn.exposure[i] = dyn.exposure[i - 1];
		dyn.analog_gain[i] = dyn.analog_gain[i - 1];
		dyn.digital_gain[i] = dyn.digital_gain[i - 1];
		dyn.isp_hdr_gain[i] = dyn.isp_hdr_gain[i - 1];
		dyn.sensor_gain[i] = dyn.sensor_gain[i - 1];
		dyn.total_gain[i] = dyn.total_gain[i - 1];
		dyn.iq_sensor_gain[i] = dyn.iq_sensor_gain[i - 1];
		dyn.iq_total_gain[i] = dyn.iq_total_gain[i - 1];
		dyn.iq_exp_gain[i] = dyn.iq_exp_gain[i - 1];
		dyn.iq_ratio[i] = 1;
	}

	dyn.contrast = ae->contrast;
	dyn.iq_bv = ae->apex.bv * 16;

	return isp_mod_sibling_execute(&ae->mod, MOD_IQ, IQ_EXEC_SET_DYNAMIC_AE,
								   &dyn, sizeof(dyn));
}

static int ae_set_fine_exp_gain(struct isp_mod_ae *ae,
								const struct rts_isp_ae_exp_gain *exp_gain,
								float fps)
{
	int ret;
	struct isp_sensor_exp_gain snr_exp_gain;

	if (ae_check_exp_gain(ae, exp_gain) || fps < ae->min_fps ||
		fps > ae->max_fps) {
		return -RTS_ISP_EINVAL;
	}

	/* get isp sync regs callback will use it */
	ae->exp_gain_set = *exp_gain;

	/* we trust exp_gain here */
	snr_exp_gain.exp_gain = exp_gain;
	snr_exp_gain.fps = fps;

	ret = isp_mod_sibling_execute(&ae->mod, MOD_SENSOR,
								  SENSOR_EXEC_SET_EXP_GAIN, &snr_exp_gain,
								  sizeof(snr_exp_gain));
	if (ret) {
		return ret;
	}

	if (float_ne(ae->dyn_fps, fps)) {
		isp_os_driver_notify_dynamic_fps(fps);
	}

	if (exp_gain->hdr != ae->exp_gain.hdr) {
		ae->dyn_hdr_changed = RTS_ISP_TRUE;
	}

	ae->exp_gain = *exp_gain;
	ae->dyn_fps = fps;

	ae_notify_exp_gain_change(ae);

	return RTS_ISP_OK;
}

static int ae_clone_iq_to_setting(struct isp_mod_ae *ae)
{
	struct rts_isp_ae_auto_setting *setting = &ae->auto_setting;
	int win_num = (ae->hw_info.ae.grid_info.size.cols *
				   ae->hw_info.ae.grid_info.size.rows);

	if (ae->auto_setting_has_set) {
		return RTS_ISP_OK;
	}

	isp_assert(win_num == ARRAY_SIZE(ae->iq->win_weight));
	isp_assert(ARRAY_SIZE(setting->win_weights) >=
			   ARRAY_SIZE(ae->iq->win_weight));
	if (!setting->window_num) {
		set_const_var(setting->window_num, win_num);
	}
	isp_memcpy(setting->win_weights, ae->iq->win_weight,
			   sizeof(ae->iq->win_weight));
	setting->target_delta = ae->patch_target_delta;
	setting->gain_max = ae->iq->total_gain_max;
	setting->exposure_max = 1e6f / setting->min_fps;

	return RTS_ISP_OK;
}

static int ae_clone_mode_to_setting(struct isp_mod_ae *ae)
{
	struct rts_isp_ae_auto_setting *setting = &ae->auto_setting;

	if (!ae->auto_setting_has_set) {
		setting->min_fps = ae->min_fps;
	}

	return RTS_ISP_OK;
}

static int ae_set_auto_setting(struct isp_mod_ae *ae,
							   const struct rts_isp_ae_auto_setting *setting)
{
	struct rts_isp_ae_auto_setting *current = &ae->auto_setting;
	struct isp_ae_hw_info *info = &ae->hw_info;
	int win_num =
		info->ae.grid_info.size.cols * info->ae.grid_info.size.rows;
	int ret = RTS_ISP_OK;

	if (setting->window_num != win_num) {
		return -RTS_ISP_EINVAL;
	}

	ae->auto_setting_has_set = RTS_ISP_TRUE;
	ret = ae_algo_set_api_ctrls(ae, setting);
	if (ret) {
		isp_perror(ret, "set ae setting ctrl fail");
		return ret;
	}

	isp_memcpy(current->win_weights, setting->win_weights,
			   win_num * sizeof(current->win_weights[0]));
	current->target_delta = setting->target_delta;
	current->gain_max = setting->gain_max;
	current->min_fps = setting->min_fps;
	current->exposure_max = setting->exposure_max;

	return RTS_ISP_OK;
}

static int
ae_set_manual_setting(struct isp_mod_ae *ae,
					  const struct rts_isp_ae_manual_setting *setting)
{
	int i;
	int ret;
	uint16_t exposure;
	uint16_t gain;
	uint16_t analog_gain;
	uint16_t digital_gain;
	uint16_t isp_gain;
	struct rts_isp_ae_exp_gain exp_gain = ae->exp_gain;

	exposure = v4l2_ctrl_g_ctrl(ae->exposure_ctrl);
	gain = v4l2_ctrl_g_ctrl(ae->gain_ctrl);
	isp_gain = ae->exp_gain.isp_gain * 256;
	analog_gain = ae->exp_gain.analog_gain[0] * 256;
	digital_gain = ae->exp_gain.digital_gain[0] * 256;

	/* TODO: ae setting API support hdr */
	if (setting->exposure_time != exposure) {
		exp_gain.exposure[0] = setting->exposure_time;
		for (i = 0; i < hdr_chan_num(ae->hdr) - 1; i++)
			exp_gain.exposure[i + 1] =
				exp_gain.exposure[i] / ae->exp_ratio[i];
	}
	if (setting->gain.analog_gain != analog_gain ||
		setting->gain.digital_gain != digital_gain ||
		setting->gain.isp_gain != isp_gain) {
		for (i = 0; i < hdr_chan_num(ae->hdr); i++) {
			exp_gain.analog_gain[i] =
				setting->gain.analog_gain / 256.0f;
			exp_gain.digital_gain[i] =
				setting->gain.digital_gain / 256.0f;
		}
		exp_gain.isp_gain = setting->gain.isp_gain / 256.0f;
	} else if (setting->total_gain != gain) {
		for (i = 0; i < hdr_chan_num(ae->hdr); i++) {
			exp_gain.analog_gain[i] = setting->total_gain / 256.0f;
			exp_gain.digital_gain[i] = 1.0f;
		}
		exp_gain.isp_gain = 1.0f;
	}

	ret = ae_tune_exposure_gain(ae, &exp_gain);
	if (ret) {
		return ret;
	}
	ret = ae_set_fine_exp_gain(ae, &exp_gain, ae->dyn_fps);
	if (ret) {
		return ret;
	}

	ae->exposure_ctrl->cur.val = ae->exp_gain.exposure[0];
	ae->gain_ctrl->cur.val = ae_total_gain(ae) * 256;

	return RTS_ISP_OK;
}

static int ae_ctrl_modify_range(struct isp_mod_ae *ae)
{
	int ret;
	float min_exposure[RTS_ISP_HDR_CHAN_MAX];
	float max_exposure[RTS_ISP_HDR_CHAN_MAX];

	ret = ae->tune_ops->get_exposure_range(ae->mod.owner_id, ae->dyn_fps,
										   ae->exp_ratio, min_exposure,
										   max_exposure);
	if (ret) {
		return ret;
	}
	ret = v4l2_ctrl_modify_range(ae->exposure_ctrl, ceilf(min_exposure[0]),
								 floorf(max_exposure[0]), 1,
								 clamp(ae->iq->def_exposure,
									   ceilf(min_exposure[0]),
									   floorf(max_exposure[0])));
	if (ret) {
		return ret;
	}
	return v4l2_ctrl_modify_range(ae->dyn_fps_ctrl,
								  ae->min_fps * SENSOR_FPS_PRECISION,
								  ae->max_fps * SENSOR_FPS_PRECISION, 1,
								  ae->max_fps * SENSOR_FPS_PRECISION);
}

static int mod_ae_exec_set_plfreq(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	ae->power_line_freq = *(int32_t *)data;

	ret = ae_algo_ctrl(ae, SET_POWER_LINE_FREQ, ae->power_line_freq);
	if (ret) {
		isp_perror(ret, "set power line freq fail");
	}
	return ret;
}

static int mod_ae_exec_get_setting(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct rts_isp_ae_setting *setting = data;
	struct rts_isp_ae_auto_setting *auto_setting = &ae->auto_setting;

	isp_assert(auto_setting->window_num);
	isp_assert(auto_setting->window_num == setting->_auto.window_num);

	setting->mode = v4l2_ctrl_g_ctrl(ae->auto_gain_ctrl);

	isp_memcpy(setting->_auto.win_weights, auto_setting->win_weights,
			   auto_setting->window_num *
			   sizeof(auto_setting->win_weights[0]));
	setting->_auto.gain_max = auto_setting->gain_max;
	setting->_auto.min_fps = auto_setting->min_fps;
	setting->_auto.exposure_max = auto_setting->exposure_max;
	if (ae->auto_setting_has_set) {
		setting->_auto.target_delta = auto_setting->target_delta;
	} else {
		setting->_auto.target_delta = ae->patch_target_delta;
	}

	setting->_manual.exposure_time = v4l2_ctrl_g_ctrl(ae->exposure_ctrl);
	setting->_manual.total_gain = v4l2_ctrl_g_ctrl(ae->gain_ctrl);
	setting->_manual.gain.analog_gain = ae->exp_gain.analog_gain[0] * 256;
	setting->_manual.gain.digital_gain = ae->exp_gain.digital_gain[0] * 256;
	setting->_manual.gain.isp_gain = ae->exp_gain.isp_gain * 256;

	return RTS_ISP_OK;
}

static int mod_ae_exec_set_setting(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct rts_isp_ae_setting *setting = data;

	if (setting->mode != RTS_ISP_AE_AUTO_MODE &&
		setting->mode != RTS_ISP_AE_MANUAL_MODE) {
		return -RTS_ISP_EINVAL;
	}

	ret = v4l2_ctrl_s_ctrl(ae->auto_gain_ctrl, setting->mode);
	if (ret) {
		return ret;
	}
	if (setting->mode == RTS_ISP_AE_AUTO_MODE) {
		ret = ae_set_auto_setting(ae, &setting->_auto);
	} else {
		ret = ae_set_manual_setting(ae, &setting->_manual);
	}
	if (ret) {
		return ret;
	}
	return mod_ae_exec_get_setting(mod, setting);
}

static int mod_ae_exec_set_target_delta(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	ae->patch_target_delta = *(int *)data;
	if (!ae->auto_setting_has_set) {
		ae_algo_ctrl(ae, SET_TARGET_DELTA, ae->patch_target_delta);
	}
	return RTS_ISP_OK;
}

static int mod_ae_exec_get_roi_areas(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	*(struct rts_isp_ae_roi_areas *)data = ae->roi_areas;

	return RTS_ISP_OK;
}

static int mod_ae_exec_set_roi_areas(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	const struct rts_isp_ae_roi_areas *areas = data;

	if (ae_algo_check_roi(ae, areas)) {
		return -RTS_ISP_EINVAL;
	}

	ae->roi_areas = *areas;

	return ae_algo_set_roi(ae);
}

static int mod_ae_exec_report_exp_gain(struct isp_mod *mod, void *data)
{
	int i, num, ret;
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct rts_isp_exp_gain_report *report = data;
	struct rts_isp_ae_exp_gain exp_gain;

	if (v4l2_ctrl_g_ctrl(ae->auto_gain_ctrl)) {
		num = hdr_chan_num(ae->hdr);
		exp_gain.hdr = ae->hdr;
		exp_gain.isp_gain = ae->exp_gain.isp_gain;

		for (i = 0; i < num; i++) {
			exp_gain.analog_gain[i] = report->analog_gain[i];
			exp_gain.digital_gain[i] = report->digital_gain[i];
			exp_gain.exposure[i] = report->exposure[i];
			exp_gain.isp_hdr_gain[i] = ae->exp_gain.isp_hdr_gain[i];
		}
		/* TODO: support hdr ae with no delay */
		if (ae->hdr == RTS_ISP_HDR_NONE) {
			ae->exp_gain_reported = 1;
		} else {
			ae->exp_gain_reported = 0;
		}

		ret = ae_tune_exposure_gain(ae, &exp_gain);
		if (ret) {
			return ret;
		}
		ret = ae_set_fine_exp_gain(ae, &exp_gain, ae->dyn_fps);
		if (ret) {
			return ret;
		}
		return ae_algo_ctrl(ae, SET_EXP_GAIN, &exp_gain);
	}

	return RTS_ISP_OK;
}

static int ae_clip_ratio_range(struct rts_isp_ae_ratio_range *range)
{
	int i;
	int ratio;
	int chn_min_total = 256, chn_max_total = 256;

	range->min_ratio_total = clamp(range->min_ratio_total,
								   AE_MIN_RATIO * 256, AE_MAX_RATIO * 256);
	range->max_ratio_total = clamp(range->max_ratio_total,
								   AE_MIN_RATIO * 256, AE_MAX_RATIO * 256);

	chn_min_total = 256;
	chn_max_total = 256;
	for (i = 0; i < RTS_ISP_HDR_CHAN_MAX - 1; i++) {
		chn_min_total = chn_min_total * range->min_ratio[i] / 256;
		chn_max_total = chn_max_total * range->max_ratio[i] / 256;
	}
	if (chn_min_total > chn_max_total ||
		range->max_ratio_total < chn_min_total ||
		range->min_ratio_total > chn_max_total) {
		return -RTS_ISP_EINVAL;
	}

	if (chn_min_total < range->min_ratio_total) {
		ratio = range->min_ratio_total;
		for (i = 0; i < RTS_ISP_HDR_CHAN_MAX - 1; i++) {
			range->min_ratio[i] = clamp(ratio, range->min_ratio[i],
										range->max_ratio[i]);
			ratio = ratio * 256 / range->min_ratio[i];
		}
	}
	if (chn_max_total > range->max_ratio_total) {
		ratio = range->max_ratio_total;
		for (i = 0; i < RTS_ISP_HDR_CHAN_MAX - 1; i++) {
			range->max_ratio[i] = clamp(ratio, range->min_ratio[i],
										range->max_ratio[i]);
			ratio = ratio * 256 / range->max_ratio[i];
		}
	}

	return RTS_ISP_OK;
}

static int ae_check_ratio_range(struct isp_mod_ae *ae,
								struct rts_isp_ratio_range *sensor_range)
{
	int i;
	struct rts_isp_ae_ratio_range *range = &ae->ratio_range;

	if (sensor_range->min_ratio_total > sensor_range->max_ratio_total) {
		return -RTS_ISP_EINVAL;
	}
	range->min_ratio_total = (int)(sensor_range->min_ratio_total * 256);
	range->max_ratio_total = (int)(sensor_range->max_ratio_total * 256);
	for (i = 0; i < RTS_ISP_HDR_CHAN_MAX - 1; i++) {
		range->min_ratio[i] = (int)(sensor_range->min_ratio[i] * 256);
		range->max_ratio[i] = (int)(sensor_range->max_ratio[i] * 256);
	}
	if (ae_clip_ratio_range(range)) {
		return -RTS_ISP_EINVAL;
	}

	if (ae_algo_version(ae) < 0x0201) {
		/*
		 * before 2.1 ratio does not have 8 bit precision,
		 * do this for abi compatibility
		 */
		range->min_ratio_total /= 256;
		range->max_ratio_total /= 256;
		for (i = 0; i < RTS_ISP_HDR_CHAN_MAX - 1; i++) {
			range->min_ratio[i] /= 256;
			range->max_ratio[i] /= 256;
		}
	}

	return RTS_ISP_OK;
}

static int mod_ae_exec_set_ratio_range(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct rts_isp_ratio_range *range = (struct rts_isp_ratio_range *)data;

	if (ae_check_ratio_range(ae, range)) {
		isp_error("set ratio range error\n");
		return -RTS_ISP_EINVAL;
	}

	ae->ratio_range_has_set = 1;
	if (ae->ae_algo) {
		return ae_algo_ctrl(ae, SET_HDR_RATIO_RANGE, &ae->ratio_range);
	}
	return RTS_ISP_OK;
}

static int mod_ae_exec_fill_sync_regs(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct isp_mod_ae_sync_regs *regs = data;

	return ae->hw_ops->get_sync_regs(mod->owner_id, regs->delay, regs->reg,
									 &regs->num, &ae->exp_gain_set);
}

static int mod_ae_exec_set_result_delay(struct isp_mod *mod, void *data)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	uint32_t delay = *(uint32_t *)data;

	ae->apex.delay = delay + 1;
	if (ae->apex.delay >= 5) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_action_info ae_exec_actions[] = {
	ISP_MOD_ACTION(AE_EXEC_SET_PLFREQ, mod_ae_exec_set_plfreq),
	ISP_MOD_ACTION(AE_EXEC_GET_SETTING, mod_ae_exec_get_setting),
	ISP_MOD_ACTION(AE_EXEC_SET_SETTING, mod_ae_exec_set_setting),
	ISP_MOD_ACTION(AE_EXEC_SET_TARGET_DELTA, mod_ae_exec_set_target_delta),
	ISP_MOD_ACTION(AE_EXEC_GET_ROI_AREAS, mod_ae_exec_get_roi_areas),
	ISP_MOD_ACTION(AE_EXEC_SET_ROI_AREAS, mod_ae_exec_set_roi_areas),
	ISP_MOD_ACTION(AE_EXEC_REPORT_EXP_GAIN, mod_ae_exec_report_exp_gain),
	ISP_MOD_ACTION(AE_EXEC_SET_RATIO_RANGE, mod_ae_exec_set_ratio_range),
	ISP_MOD_ACTION(AE_EXEC_FILL_SYNC_REGS, mod_ae_exec_fill_sync_regs),
	ISP_MOD_ACTION(AE_EXEC_SET_RESULT_DELAY, mod_ae_exec_set_result_delay),
};

static int ae_set_result(struct isp_mod_ae *ae,
						 struct rts_isp_ae_result *result)
{
	int i;
	int ret = RTS_ISP_OK;
	float ratio;
	struct rts_isp_ae_exp_gain *exp_gain;
	static uint16_t ratio_last;

	if (!ae->exp_priority_ctrl->val) {
		result->dyn_fps = ae->max_fps;
	}

	exp_gain = &result->exp_gain;
	ret = ae_set_fine_exp_gain(ae, exp_gain, result->dyn_fps);
	if (ret) {
		goto out;
	}
	ratio = 1.f;
	for (i = 0; i < ARRAY_SIZE(ae->exp_ratio); i++)
		ae->exp_ratio[i] =
			exp_gain->exposure[i] / exp_gain->exposure[i + 1];
	for (i = 0; i < hdr_chan_num(ae->hdr) - 1; i++) {
		ratio *= ae->exp_ratio[i];
	}

	if ((uint16_t)floorf(ratio) != ratio_last) {
		ratio_last = floorf(ratio);
		ret = isp_mod_sibling_execute(&ae->mod, MOD_AWB,
									  AWB_EXEC_SET_AE_RATIO,
									  &ratio_last, sizeof(ratio_last));
	}

out:
	if (ret) {
		isp_perror(ret, "isp set result fail");
		isp_error("need_update: %d\n", result->need_update);
		isp_error("mode: %d\n", result->exp_gain.hdr);
		isp_error("exposure: [%.2f, %.2f, %.2f, %.2f]\n",
				  result->exp_gain.exposure[0],
				  result->exp_gain.exposure[1],
				  result->exp_gain.exposure[2],
				  result->exp_gain.exposure[3]);
		isp_error("analog_gain: [%.6f, %.6f, %.6f, %.6f]\n",
				  result->exp_gain.analog_gain[0],
				  result->exp_gain.analog_gain[1],
				  result->exp_gain.analog_gain[2],
				  result->exp_gain.analog_gain[3]);
		isp_error("digital_gain: [%.6f, %.6f, %.6f, %.6f]\n",
				  result->exp_gain.digital_gain[0],
				  result->exp_gain.digital_gain[1],
				  result->exp_gain.digital_gain[2],
				  result->exp_gain.digital_gain[3]);
		isp_error("isp_hdr_gain: [%.6f, %.6f, %.6f, %.6f]\n",
				  result->exp_gain.isp_hdr_gain[0],
				  result->exp_gain.isp_hdr_gain[1],
				  result->exp_gain.isp_hdr_gain[2],
				  result->exp_gain.isp_hdr_gain[3]);
		isp_error("isp_gain: %.2f\n", result->exp_gain.isp_gain);
		isp_error("dyn_fps: %0.f\n", result->dyn_fps);
		isp_error("ae->exposure_step: %.2f\n", ae->exposure_step);
		isp_error("ae->min_fps: %.2f, ae->max_fps: %.2f\n", ae->min_fps,
				  ae->max_fps);
	}
	return ret;
}

static int ae_sensor_update(struct isp_mod_ae *ae)
{
	int ret;

	ret = isp_mod_sibling_get_info(&ae->mod, MOD_SENSOR,
								   SENSOR_INFO_TUNE_OPS, &ae->tune_ops,
								   sizeof(ae->tune_ops));
	if (ret) {
		return ret;
	}
	if (!ae->ae_algo) {
		return RTS_ISP_OK;
	}
	return ae_algo_update(ae);
}

static int ae_get_contrast(struct isp_mod_ae *ae)
{
	if ((ae->ae_algo->api_version & AE_VERSION_MASK) < 0x00000202) {
		ae->contrast = 0;
		return RTS_ISP_OK;
	}
	return ae_algo_ctrl(ae, GET_CONTRAST_VALUE, &ae->contrast);
}

static int ae_get_apex_bv(struct isp_mod_ae *ae)
{
	if ((ae->ae_algo->api_version & AE_VERSION_MASK) < 0x00000203) {
		ae->apex.bv = 0;
		return RTS_ISP_OK;
	}
	return ae_algo_ctrl(ae, GET_APEX_BV, &ae->apex);
}

static int ae_get_ctrl_dynamic_values(struct isp_mod_ae *ae)
{
	int ret;

	ret = ae_get_contrast(ae);
	if (ret) {
		return ret;
	}
	return ae_get_apex_bv(ae);
}

static int mod_ae_statis_done(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ae *ae = get_mod(mod, ae);
	const struct rts_isp_ae_statis *statis;
	struct rts_isp_ae_result result;

	debug("ae statis notify handling\n");
	if (ae_algo_need_delay(ae) || !ae->ae_algo) {
		return RTS_ISP_OK;
	}

	result.need_update = RTS_ISP_FALSE;
	statis = &((const struct rts_isp_statis *)data)->ae;

	ret = ae->ae_algo->run(mod->owner_id, statis, &result);
	if (ret) {
		isp_perror(ret, "ae algo run fail");
		return ret;
	}
	if (result.need_update) {
		if (result.exp_gain.hdr != ae->hdr &&
			result.exp_gain.hdr != RTS_ISP_HDR_NONE) {
			ret = -RTS_ISP_ERANGE;
			isp_perror(ret, "hdr mode range error");
			return ret;
		}
		ae->ae_status = AE_UNSTABLE;
		ae->ae_stable_cnt = 0;
		ret = ae_get_ctrl_dynamic_values(ae);
		if (ret) {
			isp_perror(ret, "get ctrl dynamic fail");
			return ret;
		}
		/* we trust algo result, so do not tune it here */
		ae_set_exp_gain_unused(ae, &result.exp_gain);
		ret = ae_set_result(ae, &result);
		if (ret) {
			isp_perror(ret, "set ae result fail");
			return ret;
		}
	} else {
		if (ae->ae_stable_cnt < 4) {
			ae->ae_stable_cnt++;
		} else {
			ae->ae_status = AE_STABLE;
		}
	}

	return RTS_ISP_OK;
}

static int ae_adjust_exposure_ctrl(struct isp_mod_ae *ae)
{
	struct v4l2_ctrl *ctrl = ae->exposure_ctrl;
	float min_exposure[RTS_ISP_HDR_CHAN_MAX];
	float max_exposure[RTS_ISP_HDR_CHAN_MAX];
	uint32_t min;
	uint32_t max;
	uint32_t def;
	uint32_t step = 1;
	int ret;

	ret = ae->tune_ops->get_exposure_range(ae->mod.owner_id, ae->dyn_fps,
										   ae->exp_ratio, min_exposure,
										   max_exposure);
	if (ret) {
		return ret;
	}

	min = ceilf(min_exposure[0]);
	max = floorf(max_exposure[0]);
	def = min;

	if (ctrl->minimum != min || ctrl->maximum != max ||
		ctrl->step != step || ctrl->default_value != def) {
		ctrl->minimum = min;
		ctrl->maximum = max;
		ctrl->step = step;
		ctrl->default_value = def;
	}
	ctrl->cur.val = ctrl->val = clamp((uint32_t)ctrl->val, min, max);

	return RTS_ISP_OK;
}

static int mod_ae_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	const struct isp_sensor_mode *mode = data;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	ae->hdr = mode->hdr;
	ae->min_fps = mode->min_fps;
	if (ae->max_fps_mode == 0) {
		ae->max_fps_mode = ae->max_fps = ae->dyn_fps = mode->max_fps;
	} else {
		ae->max_fps_mode = mode->max_fps;
		ae->dyn_fps = clamp(ae->dyn_fps, mode->min_fps, mode->max_fps);
	}
	ae->exposure_step = mode->exposure_step;

	ae->exp_gain.hdr = ae->hdr;

	ae->ratio[0] = ae->exp_ratio[0] = AE_DEF_RATIO;
	ae->ratio[1] = ae->exp_ratio[1] = 1;
	ae->ratio[2] = ae->exp_ratio[2] = 1;

	/*
	 * Since the exposure ctrl is automatically managed by
	 * v4l2_ctrl_auto_cluster, the value cannot be manually modified by
	 * v4l2_ctrl_modify_range, so modify the exposure ctrl directly.
	 */

	ret = ae_adjust_exposure_ctrl(ae);
	if (ret) {
		return ret;
	}

	/* we change exposure ctrl here and refine exp gain at fps change */

	return ae_clone_mode_to_setting(ae);
}

static int mod_ae_fps_change(struct isp_mod *mod, void *data)
{
	int ret;
	float max_fps_pre;
	struct isp_mod_ae *ae = get_mod(mod, ae);
	float fps = *(float *)data;

	if (ae->exp_gain_reported) {
		ae->algo_delay = 0;
	} else {
		ae->algo_delay = 2;
	}
	max_fps_pre = ae->max_fps;
	ae->max_fps = fps;
	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		ae->dyn_fps < max_fps_pre) {
		ae->dyn_fps = clamp(ae->dyn_fps, ae->min_fps, ae->max_fps);
	} else {
		ae->dyn_fps = fps;
	}
	ret = ae_ctrl_modify_range(ae);
	if (ret) {
		return ret;
	}
	ret = ae_tune_exposure_gain(ae, &ae->exp_gain);
	if (ret) {
		return ret;
	}
	ret = ae_set_fine_exp_gain(ae, &ae->exp_gain, ae->dyn_fps);
	if (ret) {
		return ret;
	}
	return ae_algo_set_ctrls(ae);
}

static int mod_ae_plugin_update(struct isp_mod *mod, void *data)
{
	uint32_t notify_data;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	notify_data = *(uint32_t *)data;
	if (notify_data != RTS_ISP_PLUG_TYPE_ALGO_AE &&
		notify_data != RTS_ISP_PLUG_TYPE_SENSOR) {
		return RTS_ISP_OK;
	}

	if (notify_data == RTS_ISP_PLUG_TYPE_SENSOR) {
		return ae_sensor_update(ae);
	}

	return ae_algo_update(ae);
}

static int mod_ae_iq_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	if (ae->dyn_hdr_changed) {
		ae->dyn_hdr_changed = RTS_ISP_FALSE;
		return RTS_ISP_OK;
	}
	ae->iq = isp_iq_get_ae_v1(mod->owner_id, &ae->algo_param->common);
	if (!ae->iq) {
		isp_error("get ae iq fail\n");
		return -RTS_ISP_ENOITEM;
	}

	/* first iq load, update ctrl to match iq table */
	if (ae->auto_gain_ctrl->flags & ISP_V4L2_CTRL_FLAG_READ_ONLY) {
		ae->auto_gain_ctrl->flags &= ~ISP_V4L2_CTRL_FLAG_READ_ONLY;
		/* set auto at first, because auto may change manual ctrl*/
		v4l2_ctrl_s_ctrl(ae->auto_gain_ctrl, ae->iq->enable);
		ae->exposure_ctrl->val = ae->iq->def_exposure;
		ae->exposure_ctrl->cur.val = ae->iq->def_exposure;
		ae->gain_ctrl->val = ae->iq->def_gain * 256;
		ae->gain_ctrl->cur.val = ae->iq->def_gain * 256;
		ae->smart_ir_mode_ctrl->val = ae->iq->sort_mode;
		ae->smart_ir_mode_ctrl->cur.val = ae->iq->sort_mode;
		ae->smart_ir_manual_level_ctrl->val = ae->iq->manual_level;
		ae->smart_ir_manual_level_ctrl->cur.val = ae->iq->manual_level;
	}
	ret = ae_algo_update(ae);
	if (ret) {
		return ret;
	}
	return ae_clone_iq_to_setting(ae);
}

static int mod_ae_mcrop_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ae *ae = get_mod(mod, ae);
	const rts_isp_rect_t *rect = data;

	ret = rts_isp_get_grid_from_rect(&ae->grid, rect,
									 &ae->hw_info.ae.grid_info);
	if (ret) {
		return ret;
	}
	return ae->hw_ops->set_statis(mod->owner_id, &ae->grid);
}

static int mod_ae_start_pre(struct isp_mod *mod, void *data)
{
	int ret;
	int enable = 0;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	if (ae->hdr == RTS_ISP_HDR_LINE_2TO1 || ae->hdr == RTS_ISP_HDR_DCG_2TO1) {
		enable = 1;
	}
	ret = ae->hw_ops->enable_hdr_axi(mod->owner_id, enable);
	if (ret) {
		return ret;
	}
	return ae->hw_ops->enable_hdr(mod->owner_id, enable);
}

static int mod_ae_stop_pre(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	ae->exp_gain_reported = 0;
	ret = ae->hw_ops->enable_hdr(mod->owner_id, RTS_ISP_FALSE);
	if (ret) {
		return ret;
	}
	return ae->hw_ops->enable_hdr_axi(mod->owner_id, RTS_ISP_FALSE);
}

static struct isp_mod_action_info ae_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_ae_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_PLUGIN_UPDATE, mod_ae_plugin_update),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_ae_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_ae_mode_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_FPS_CHANGE, mod_ae_fps_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_ae_mcrop_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_START_PRE, mod_ae_start_pre),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STOP_PRE, mod_ae_stop_pre),
};

static int ae_set_exp_gain_ctrl(struct isp_mod_ae *ae, int dyn_hdr)
{
	int i;
	int ret;
	struct rts_isp_ae_exp_gain exp_gain;

	exp_gain.hdr = dyn_hdr;
	exp_gain.exposure[0] = ae->exposure_ctrl->val;
	for (i = 0; i < hdr_chan_num(ae->hdr) - 1; i++)
		exp_gain.exposure[i + 1] =
			exp_gain.exposure[i] / ae->exp_ratio[i];
	for (i = 0; i < hdr_chan_num(ae->hdr); i++) {
		exp_gain.analog_gain[i] = ae->gain_ctrl->val / 256.0f;
		exp_gain.digital_gain[i] = 1.0f;
		exp_gain.isp_hdr_gain[i] = 1.0f;
	}
	exp_gain.isp_gain = 1.0f;
	ret = ae_tune_exposure_gain(ae, &exp_gain);
	if (ret) {
		return ret;
	}
	return ae_set_fine_exp_gain(ae, &exp_gain, ae->dyn_fps);
}

static int ae_set_hdr_ctrl(struct isp_mod_ae *ae)
{
	int hdr;

	if (ae->hdr != RTS_ISP_HDR_LINE_2TO1) {
		return RTS_ISP_OK;
	}

	if (ae->auto_gain_ctrl->val)
		return ae_algo_ctrl(ae, SET_DYNAMIC_HDR_MODE,
							ae->hdr_ctrl->val);

	hdr = ae->hdr_ctrl->val == 0 ? RTS_ISP_HDR_NONE : ae->hdr;
	return ae_set_exp_gain_ctrl(ae, hdr);
}

static int ae_set_hdr_ratio_ctrl(struct isp_mod_ae *ae)
{
	int i;
	int ret;
	float ratio;
	int back_exposure;
	uint16_t ratio_int;
	float back_ratio[RTS_ISP_HDR_CHAN_MAX - 1];
	float back_exp_ratio[RTS_ISP_HDR_CHAN_MAX - 1];

	if (ae->hdr != RTS_ISP_HDR_LINE_2TO1 || hdr_chan_num(ae->hdr) == 1) {
		return RTS_ISP_OK;
	}

	if (ae->auto_gain_ctrl->val) {
		int val;

		if (ae->auto_hdr_ratio_ctrl->val) {
			val = 0;
		} else {
			if (ae->hdr_ratio_ctrl->val < 256) {
				ae->hdr_ratio_ctrl->val *= 256;
			}
			val = ae->hdr_ratio_ctrl->val;
			val /= 256;
		}

		return ae_algo_ctrl(ae, SET_HDR_RATIO, val);
	}

	if (!ae->hdr_ratio_ctrl->is_new) {
		return RTS_ISP_OK;
	}

	back_exposure = ae->exposure_ctrl->val;
	isp_memcpy_array(back_ratio, ae->ratio, ARRAY_SIZE(back_ratio));
	isp_memcpy_array(back_exp_ratio, ae->exp_ratio,
					 ARRAY_SIZE(back_exp_ratio));
	if (ae->hdr_ratio_ctrl->val < 256) {
		ae->hdr_ratio_ctrl->val *= 256;
	}
	ratio = pow(ae->hdr_ratio_ctrl->val / 256.f,
				1.0 / (hdr_chan_num(ae->hdr) - 1));
	for (i = 0; i < hdr_chan_num(ae->hdr) - 1; i++) {
		ae->ratio[i] = ae->exp_ratio[i] = ratio;
	}
	for (i = hdr_chan_num(ae->hdr) - 1; i < RTS_ISP_HDR_CHAN_MAX - 1; i++) {
		ae->ratio[i] = ae->exp_ratio[i] = 1.0;
	}
	ret = ae_ctrl_modify_range(ae);
	if (!ret) {
		ret = ae_set_exp_gain_ctrl(ae, ae->exp_gain.hdr);
	}
	if (ret) {
		isp_memcpy_array(ae->ratio, back_ratio, ARRAY_SIZE(ae->ratio));
		isp_memcpy_array(ae->exp_ratio, back_exp_ratio,
						 ARRAY_SIZE(ae->exp_ratio));
		ae_ctrl_modify_range(ae);
		ae->exposure_ctrl->val = back_exposure;
		ae_set_exp_gain_ctrl(ae, ae->exp_gain.hdr);
	}
	ratio = 1.f;
	for (i = 0; i < hdr_chan_num(ae->hdr) - 1; i++) {
		ratio *= ae->exp_ratio[i];
	}
	ratio_int = (uint16_t)ratio;
	return isp_mod_sibling_execute(&ae->mod, MOD_AWB, AWB_EXEC_SET_AE_RATIO,
								   &ratio_int, sizeof(ratio_int));
}

static int ae_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTOGAIN:
		if (float_ne(ae->exp_gain.exposure[0], 0)) {
			ae->exposure_ctrl->val =
				ceilf(ae->exp_gain.exposure[0]);
			ae->gain_ctrl->val = ae_total_gain(ae) * 256;
		} else {
			ae->exposure_ctrl->val =
				ae->exposure_ctrl->default_value;
			ae->gain_ctrl->val = ae->gain_ctrl->default_value;
		}
		break;
	case ISP_V4L2_CID_DYNAMIC_FPS:
		ae->dyn_fps_ctrl->val = ae->dyn_fps * SENSOR_FPS_PRECISION;
		break;
	case ISP_V4L2_CID_AUTO_HDR_RATIO:
		ae->hdr_ratio_ctrl->val = 256 * ae_total_ratio(ae);
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	return RTS_ISP_OK;
}

static int ae_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_ae *ae = get_mod(mod, ae);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTOGAIN:
		if (ae->auto_gain_ctrl->is_new) {
			ret = ae_algo_set_auto(ae, ae->auto_gain_ctrl->val);
			if (ret) {
				break;
			}
		}
		if (ae->exposure_ctrl->is_new || ae->gain_ctrl->is_new) {
			ret = ae_set_exp_gain_ctrl(ae, ae->exp_gain.hdr);
		}
		break;
	case ISP_V4L2_CID_EXPOSURE_AUTO_PRIORITY:
		if (ae->exp_priority_ctrl->is_new)
			ret = ae_algo_ctrl(ae, SET_DYNAMIC_FPS_ENABLE,
							   ae->exp_priority_ctrl->val);
		break;
	case ISP_V4L2_CID_WIDE_DYNAMIC_RANGE:
		ret = ae_set_hdr_ctrl(ae);
		break;
	case ISP_V4L2_CID_AUTO_HDR_RATIO:
		ret = ae_set_hdr_ratio_ctrl(ae);
		break;
	case ISP_V4L2_CID_SMART_IR_MODE:
		ret = ae_algo_ctrl(ae, SET_SORT_MODE, ctrl->val);
		break;
	case ISP_V4L2_CID_SMART_IR_MANUAL_LEVEL:
		ret = ae_algo_ctrl(ae, SET_SORT_MANUAL_LEVEL, ctrl->val);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set ae ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops ae_ctrl_ops = {
	.g_volatile_ctrl = ae_g_volatile_ctrl,
	.s_ctrl = ae_s_ctrl,
};

static int isp_mod_ae_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_ae *ae;

	ae = get_mod(mod, ae);
	ae->hw_ops = isp_hw_ae_get_ops();
	if (ae->hw_ops == NULL) {
		isp_error("get ae hw ops fail\n");
		return -RTS_ISP_ENOOPS;
	}
	ret = ae->hw_ops->get_hw_info(mod->owner_id, &ae->hw_info);
	if (ret) {
		return ret;
	}
	ae->algo_param = isp_algo_param_get_ae_v1(mod->owner_id);
	if (!ae->algo_param) {
		isp_error("get ae algo param fail\n");
		return -RTS_ISP_EINVAL;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_TUNE_OPS,
								   &ae->tune_ops, sizeof(ae->tune_ops));
	if (ret) {
		return ret;
	}
	ae->power_line_freq = POWER_LINE_50HZ;
	ae->apex.delay = 2;

	return RTS_ISP_OK;
}

static int isp_mod_ae_cleanup(struct isp_mod *mod)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);

	ae_algo_set_auto(ae, RTS_ISP_FALSE);
	ae_algo_cleanup(ae);
	return RTS_ISP_OK;
}

static int isp_mod_ae_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_ae *ae = get_mod(mod, ae);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config dyn_fps_config = {
		.ops = &ae_ctrl_ops,
		.id = ISP_V4L2_CID_DYNAMIC_FPS,
		.name = "Dynamic fps",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.step = 1,
		.flags = ISP_V4L2_CTRL_FLAG_READ_ONLY |
		ISP_V4L2_CTRL_FLAG_VOLATILE,
	};
	struct v4l2_ctrl_config smart_ir_mode_config = {
		.ops = &ae_ctrl_ops,
		.id = ISP_V4L2_CID_SMART_IR_MODE,
		.name = "Smart IR Mode",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = SORT_DISABLE,
		.max = SORT_MANUAL,
		.step = 1,
		.def = SORT_DISABLE,
	};
	struct v4l2_ctrl_config smart_ir_manual_level_config = {
		.ops = &ae_ctrl_ops,
		.id = ISP_V4L2_CID_SMART_IR_MANUAL_LEVEL,
		.name = "Smart IR Manual Level",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = 0,
		.max = 100,
		.step = 1,
		.def = 0,
	};
	static const char *const hdr_menu[] = {
		"HDR off",
		"HDR on",
		"HDR automatic",
	};
	struct v4l2_ctrl_config hdr_config = {
		.ops = &ae_ctrl_ops,
		.id = ISP_V4L2_CID_WIDE_DYNAMIC_RANGE,
		.name = "High Dynamic Range",
		.type = ISP_V4L2_CTRL_TYPE_MENU,
		.max = ARRAY_SIZE(hdr_menu) - 1,
		.qmenu = hdr_menu,
		.def = ARRAY_SIZE(hdr_menu) - 2,
	};
	struct v4l2_ctrl_config auto_hdr_ratio_config = {
		.ops = &ae_ctrl_ops,
		.id = ISP_V4L2_CID_AUTO_HDR_RATIO,
		.name = "HDR exposure ratio, Automatic",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 1,
	};
	struct v4l2_ctrl_config hdr_ratio_config = {
		.ops = &ae_ctrl_ops,
		.id = ISP_V4L2_CID_HDR_RATIO,
		.name = "HDR exposure ratio",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = AE_MIN_RATIO,
		.max = AE_MAX_RATIO * 256,
		.step = 1,
		.def = AE_DEF_RATIO * 256,
	};

	ae->auto_gain_ctrl = v4l2_ctrl_new_std(
							 handler, &ae_ctrl_ops, ISP_V4L2_CID_AUTOGAIN, 0, 1, 1, 1, mod);
	ae->exposure_ctrl = v4l2_ctrl_new_std(handler, &ae_ctrl_ops,
										  ISP_V4L2_CID_EXPOSURE, 100,
										  100000, 1, 10000, mod);
	ae->gain_ctrl = v4l2_ctrl_new_std(handler, &ae_ctrl_ops,
									  ISP_V4L2_CID_GAIN, 256, 128 * 256, 1,
									  256, mod);
	v4l2_ctrl_auto_cluster(3, &ae->auto_gain_ctrl, 0, RTS_ISP_TRUE);
	ae->auto_gain_ctrl->flags |= ISP_V4L2_CTRL_FLAG_READ_ONLY;
	ae->exp_priority_ctrl = v4l2_ctrl_new_std(
								handler, &ae_ctrl_ops, ISP_V4L2_CID_EXPOSURE_AUTO_PRIORITY, 0,
								1, 1, 1, mod);
	ae->dyn_fps_ctrl = v4l2_ctrl_new_custom(handler, &dyn_fps_config, mod);
	ae->hdr_ctrl = v4l2_ctrl_new_custom(handler, &hdr_config, mod);
	ae->auto_hdr_ratio_ctrl =
		v4l2_ctrl_new_custom(handler, &auto_hdr_ratio_config, mod);
	ae->hdr_ratio_ctrl =
		v4l2_ctrl_new_custom(handler, &hdr_ratio_config, mod);
	v4l2_ctrl_auto_cluster(2, &ae->auto_hdr_ratio_ctrl, 0, RTS_ISP_TRUE);
	ae->smart_ir_mode_ctrl =
		v4l2_ctrl_new_custom(handler, &smart_ir_mode_config, mod);
	ae->smart_ir_manual_level_ctrl = v4l2_ctrl_new_custom(
										 handler, &smart_ir_manual_level_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add ae ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_ae ae;

int isp_register_mod_ae(void)
{
	isp_memset(&ae, 0, sizeof(ae));
	ae.mod.id = MOD_AE;
	ae.mod.name = "ae";
	ae.mod.init = isp_mod_ae_init;
	ae.mod.cleanup = isp_mod_ae_cleanup;
	ae.mod.add_ctrl = isp_mod_ae_add_ctrl;
	ae.mod.info_actions = ae_info_actions;
	ae.mod.info_actions_num = ARRAY_SIZE(ae_info_actions);
	ae.mod.exec_actions = ae_exec_actions;
	ae.mod.exec_actions_num = ARRAY_SIZE(ae_exec_actions);
	ae.mod.notify_actions = ae_notify_actions;
	ae.mod.notify_actions_num = ARRAY_SIZE(ae_notify_actions);
	ae.mod.size = sizeof(ae);
	return isp_mod_register(&ae.mod);
}

void isp_unregister_mod_ae(void)
{
	isp_mod_unregister(&ae.mod);
}
