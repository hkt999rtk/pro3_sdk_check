/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <rts_isp_sensor.h>
#include <rts_isp_patch.h>
#include <isp_os_driver.h>
#include <isp_utils.h>
#include <isp_top.h>
#include <isp_msg_buf.h>
#include <isp_notify.h>
#include <isp_core.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_poll.h>
#include <isp_sensor.h>
#include <isp_v4l2_ctrl.h>
#include <isp_hw_ae.h>
#include <isp_hw_sensor.h>
#include "isp_mod_iq.h"
#include "isp_mod_ae.h"
#include "isp_mod_input.h"
#include "isp_mod_sensor.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_SENSOR

struct isp_mod_sensor {
	struct isp_mod mod;
	const struct rts_isp_sensor_ops *sensor_ops;
	const struct rts_isp_sensor_patch_ops *patch_ops;

	const struct isp_hw_sensor_ops *hw_ops;

	struct rts_isp_sensor_info info;
	struct isp_sensor_mode mode;
	rts_isp_point_t start;
	struct rts_isp_interface_info interface;
	struct rts_isp_i2c_regs init_regs[8];

	float fps;
	float set_fps;
	int fps_change_notifying;

	struct rts_isp_sensor_exp_gain exp_gain;
	uint32_t isp_gain_delay;

	const isp_iq_high_temp_v1_t *iq;
	isp_algo_param_high_temp_v1_t *algo_param;

	struct isp_notify_dynamic_sensor dynamic;
	isp_timer_handle_t timer;
	int gain_is_high : 1;
	int temp_is_high : 1;

	uint32_t flick_dummy;
	uint32_t focus_pos;
	struct v4l2_ctrl *mirror_flip_ctrl;
};

static float precision_fps(float fps)
{
	return roundf(fps * SENSOR_FPS_PRECISION) / SENSOR_FPS_PRECISION;
}

static int check_patch_ops(const struct rts_isp_sensor_patch_ops *patch_ops)
{
	if (!patch_ops) {
		return RTS_ISP_OK;
	}
	if (patch_ops->api_version != PATCH_API_VERSION) {
		return -RTS_ISP_EINVAL;
	}
	if (!patch_ops->iq_change && !patch_ops->dynamic) {
		return -RTS_ISP_EPLUGIN;
	}
	return RTS_ISP_OK;
}

static int check_mode(const struct rts_isp_sensor_mode *mode)
{
	if (mode->hdr != RTS_ISP_HDR_NONE &&
		mode->hdr != RTS_ISP_HDR_LINE_2TO1 &&
		mode->hdr != RTS_ISP_HDR_DCG_2TO1 &&
		mode->hdr != RTS_ISP_HDR_DCG_COMBINE &&
		mode->hdr != RTS_ISP_HDR_DCG_COMBINE_TM) {
		return -RTS_ISP_EINVAL;
	}
	if (mode->size.w < 64 || mode->size.h < 64 || mode->size.w % 2 ||
		mode->size.h % 2 || mode->fps < 0.1) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_modes(const struct rts_isp_sensor_modes *modes)
{
	int i;

	if (!modes->num || modes->num > ARRAY_SIZE(modes->mode)) {
		return -RTS_ISP_EINVAL;
	}
	for (i = 0; i < modes->num; i++)
		if (check_mode(&modes->mode[i])) {
			return -RTS_ISP_EINVAL;
		}
	return RTS_ISP_OK;
}

static int check_mode_compatible(const struct rts_isp_sensor_mode *mode,
								 const struct rts_isp_sensor_modes *modes)
{
	int i;

	for (i = 0; i < modes->num; i++)
		if (mode->hdr == modes->mode[i].hdr &&
			mode->size.w <= modes->mode[i].size.w &&
			mode->size.h <= modes->mode[i].size.h &&
			float_eq(mode->fps, modes->mode[i].fps)) {
			break;
		}
	if (i == modes->num) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_info(const struct rts_isp_sensor_info *info)
{
	if (check_modes(&info->modes)) {
		return -RTS_ISP_EINVAL;
	}
	if ((info->i2c.addr_len != 1 && info->i2c.addr_len != 2) ||
		(info->i2c.data_len != 1 && info->i2c.data_len != 2)) {
		return -RTS_ISP_EINVAL;
	}
	if (info->power_down.num > ARRAY_SIZE(info->power_down.items) ||
		info->power_down.num > ARRAY_SIZE(info->power_down.items)) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_init_info(const struct rts_isp_sensor_init_info *info,
						   const struct rts_isp_sensor_mode *mode)
{
	if (info->start.x + mode->size.w > info->size.w ||
		info->start.y + mode->size.h > info->size.h) {
		return -RTS_ISP_ERANGE;
	}
	if (fabs(info->hts * info->min_vts * mode->fps - info->pclk) >
		0.01 * info->pclk) {
		return -RTS_ISP_ERANGE;
	}
	if (!info->pclk || !info->hts || !info->min_vts ||
		info->max_vts < info->min_vts) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int refine_fps(struct rts_isp_sensor_modes *modes)
{
	int i;

	if (!modes) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < modes->num; i++) {
		modes->mode[i].fps = precision_fps(modes->mode[i].fps);
	}
	return RTS_ISP_OK;
}

static int sensor_mirror_flip_enable(struct isp_mod_sensor *sensor)
{
	return sensor_minor_version(sensor->sensor_ops->api_version) >= 1 &&
		   sensor->sensor_ops->get_mirror_flip;
}

static int sensor_get_exposure_range(struct isp_mod_sensor *sensor,
									 uint32_t vts,
									 float ratio[RTS_ISP_HDR_CHAN_MAX - 1],
									 float min_exposure[RTS_ISP_HDR_CHAN_MAX],
									 float max_exposure[RTS_ISP_HDR_CHAN_MAX])
{
	int i;
	int max_short_exp_cnt;
	int cur_max_exp_cnt;
	int min_delay_cnt;
	int remain_cnt;
	float real_ratio = 1;
	float ratio_sum = real_ratio;
	const int gap = 4;
	const int num = hdr_chan_num(sensor->mode.hdr);
	const float step = 1e6 * sensor->mode.hts / num / sensor->mode.pclk;

	for (i = num - 2; i >= 0; i--) {
		if (ratio[i] < 1) {
			return -RTS_ISP_EINVAL;
		}
		real_ratio = real_ratio * ratio[i];
		ratio_sum += real_ratio;
	}

	/* calc min exposure */
	min_exposure[num - 1] = sensor->mode.exposure_step;
	for (i = num - 2; i >= 0; i--) {
		min_exposure[i] = min_exposure[i + 1] * ratio[i];
	}

	/* calc max exposure */
	min_delay_cnt = sensor->hw_ops->min_exp_delay(sensor->mod.owner_id);
	remain_cnt = (vts - gap) * num;
	max_short_exp_cnt =
		min((uint32_t)(remain_cnt / ratio_sum),
			roundup_div(sensor->mode.min_vts, AE_MIN_RATIO) - gap);
	do {
		cur_max_exp_cnt = max_short_exp_cnt;
		for (i = num - 1; i > 0; i--) {
			if (min_exposure[i] > cur_max_exp_cnt * step) {
				return -RTS_ISP_EINVAL;
			}
			remain_cnt -= max(min_delay_cnt, cur_max_exp_cnt);
			cur_max_exp_cnt = cur_max_exp_cnt * ratio[i - 1];
		}
		if (cur_max_exp_cnt <= remain_cnt) {
			break;
		}
	} while (--max_short_exp_cnt);
	if (max_short_exp_cnt == 0) {
		return -RTS_ISP_EINVAL;
	}
	max_exposure[0] = step * cur_max_exp_cnt;
	for (i = 1; i < num; i++) {
		max_exposure[i] = max_exposure[i - 1] / ratio[i - 1];
	}

	return RTS_ISP_OK;
}

static int get_exposure_range(int32_t isp_id, float fps,
							  float ratio[RTS_ISP_HDR_CHAN_MAX - 1],
							  float min_exposure[RTS_ISP_HDR_CHAN_MAX],
							  float max_exposure[RTS_ISP_HDR_CHAN_MAX])
{
	uint32_t vts;
	struct isp_mod *mod;
	struct isp_mod_sensor *sensor;

	mod = isp_top_get_mod(isp_id, MOD_SENSOR);
	if (!mod) {
		return -RTS_ISP_EINVAL;
	}

	sensor = get_mod(mod, sensor);

	if (fps < sensor->mode.min_fps || fps > sensor->mode.max_fps ||
		(hdr_chan_num(sensor->mode.hdr) > 1 && !ratio) || !min_exposure ||
		!max_exposure) {
		return -RTS_ISP_EINVAL;
	}

	vts = sensor->mode.min_vts * sensor->mode.max_fps / fps;

	if (sensor->sensor_ops->get_exposure_range)
		return sensor->sensor_ops->get_exposure_range(
				   sensor->mod.owner_id, vts, ratio, min_exposure,
				   max_exposure);

	if (sensor->mode.hdr == RTS_ISP_HDR_LINE_2TO1)
		return sensor_get_exposure_range(sensor, vts, ratio,
										 min_exposure, max_exposure);
	/* can set 40ms exposure at 25 fps by a little fps reducing */
	min_exposure[0] = sensor->mode.exposure_step;
	max_exposure[0] = 1e6f / fps;
	return RTS_ISP_OK;
}

static int get_tuned_again(uint32_t isp_id, float again[RTS_ISP_HDR_CHAN_MAX])
{
	struct isp_mod *mod;

	mod = isp_top_get_mod(isp_id, MOD_SENSOR);
	if (!mod || !again) {
		return -RTS_ISP_EINVAL;
	}
	return get_mod(mod, sensor)->sensor_ops->get_tuned_again(isp_id, again);
}

static int get_tuned_dgain(uint32_t isp_id, float dgain[RTS_ISP_HDR_CHAN_MAX])
{
	struct isp_mod *mod;

	mod = isp_top_get_mod(isp_id, MOD_SENSOR);
	if (!mod || !dgain) {
		return -RTS_ISP_EINVAL;
	}
	return get_mod(mod, sensor)->sensor_ops->get_tuned_dgain(isp_id, dgain);
}

static const struct rts_isp_exp_gain_tune_ops exp_gain_tune_ops = {
	.get_exposure_range = get_exposure_range,
	.get_tuned_again = get_tuned_again,
	.get_tuned_dgain = get_tuned_dgain,
};

static const char *get_hdr_str(int32_t hdr)
{
	switch (hdr) {
	case RTS_ISP_HDR_NONE:
		return "linear";
	case RTS_ISP_HDR_LINE_2TO1:
		return "hdr line 2to1";
	case RTS_ISP_HDR_DCG_COMBINE:
		return "hdr dcg combine";
	case RTS_ISP_HDR_DCG_COMBINE_TM:
		return "hdr dcg combine & compress";
	case RTS_ISP_HDR_DCG_2TO1:
		return "hdr dcg 2to1";
	default:
		return "unknown mode";
	}
}

static int sensor_get_snr_ops(struct isp_mod_sensor *sensor,
							  const struct rts_isp_sensor_ops **ops)
{
	return isp_mod_sibling_get_info(&sensor->mod, MOD_INPUT,
									INPUT_INFO_SNR_OPS, ops, sizeof(*ops));
}

static int sensor_init_plugin(struct isp_mod_sensor *sensor)
{
	int ret;

	ret = sensor_get_snr_ops(sensor, &sensor->sensor_ops);
	if (ret) {
		goto err;
	}
	if (!sensor->sensor_ops) {
		ret = -RTS_ISP_ENOTREADY;
		goto err;
	}
	if (sensor->sensor_ops->get_patch_ops) {
		sensor->patch_ops = sensor->sensor_ops->get_patch_ops();
	}
	ret = check_patch_ops(sensor->patch_ops);
	if (ret) {
		goto err;
	}
	if (sensor->sensor_ops->init) {
		ret = sensor->sensor_ops->init(sensor->mod.owner_id);
		if (ret) {
			goto err;
		}
	}
	if (sensor->patch_ops && sensor->patch_ops->init) {
		ret = sensor->patch_ops->init(sensor->mod.owner_id);
		if (ret) {
			if (sensor->sensor_ops && sensor->sensor_ops->cleanup)
				sensor->sensor_ops->cleanup(
					sensor->mod.owner_id);
			goto err;
		}
	}
	return RTS_ISP_OK;
err:
	sensor->sensor_ops = NULL;
	sensor->patch_ops = NULL;
	return ret;
}

static void sensor_cleanup_plugin(struct isp_mod_sensor *sensor)
{
	if (sensor->sensor_ops && sensor->sensor_ops->cleanup) {
		sensor->sensor_ops->cleanup(sensor->mod.owner_id);
	}
	if (sensor->patch_ops && sensor->patch_ops->cleanup) {
		sensor->patch_ops->cleanup(sensor->mod.owner_id);
	}

	sensor->sensor_ops = NULL;
	sensor->patch_ops = NULL;
}

static int sensor_get_info(struct isp_mod_sensor *sensor)
{
	int ret;

	isp_memset(&sensor->info, 0, sizeof(sensor->info));
	ret = sensor->sensor_ops->get_info(sensor->mod.owner_id, &sensor->info);
	if (ret) {
		return ret;
	}
	ret = check_info(&sensor->info);
	if (ret) {
		return ret;
	}
	return refine_fps(&sensor->info.modes);
}

static int sensor_set_mode(struct isp_mod_sensor *sensor,
						   const struct rts_isp_sensor_init_info *init_info,
						   const struct rts_isp_sensor_mode *mode)
{
	sensor->mode.hdr = mode->hdr;
	sensor->mode.size.snr = init_info->size;
	sensor->mode.size.isp = mode->size;
	sensor->mode.max_fps = mode->fps;
	sensor->mode.min_fps = (sensor->mode.max_fps * init_info->min_vts /
							init_info->max_vts);
	sensor->mode.hts = init_info->hts;
	sensor->mode.min_vts = init_info->min_vts;
	sensor->mode.max_vts = init_info->max_vts;
	sensor->mode.pclk = init_info->pclk;
	if (float_ne(init_info->exposure_step, 0)) {
		sensor->mode.exposure_step = init_info->exposure_step;
	} else
		sensor->mode.exposure_step =
			(1e6 * sensor->mode.hts /
			 hdr_chan_num(sensor->mode.hdr) / init_info->pclk);
	if (sensor->sensor_ops->api_version >= 0x00010002) {
		int ret;

		if (sensor->mode.hdr == RTS_ISP_HDR_DCG_COMBINE) {
			ret = sensor->hw_ops->set_comp(sensor->mod.owner_id,
										   RTS_ISP_TRUE);
			if (ret) {
				return ret;
			}
			return sensor->hw_ops->set_decomp(sensor->mod.owner_id,
											  RTS_ISP_TRUE, NULL);
		} else if (sensor->mode.hdr == RTS_ISP_HDR_DCG_COMBINE_TM) {
			ret = sensor->hw_ops->set_comp(sensor->mod.owner_id,
										   RTS_ISP_FALSE);
			if (ret) {
				return ret;
			}
			return sensor->hw_ops->set_decomp(
					   sensor->mod.owner_id, RTS_ISP_TRUE,
					   &init_info->comp_table);
		} else {
			ret = sensor->hw_ops->set_comp(sensor->mod.owner_id,
										   RTS_ISP_FALSE);
			if (ret) {
				return ret;
			}
			return sensor->hw_ops->set_decomp(sensor->mod.owner_id,
											  RTS_ISP_FALSE, NULL);
		}
	}

	return RTS_ISP_OK;
}

static int sensor_change_mode(struct isp_mod_sensor *sensor,
							  const struct rts_isp_sensor_mode *mode)
{
	int ret;
	struct rts_isp_sensor_init_info init_info;

	isp_memset(&init_info, 0, sizeof(init_info));
	ret = sensor->sensor_ops->get_init_info(sensor->mod.owner_id, mode,
											&init_info);
	if (ret) {
		return ret;
	}
	ret = check_init_info(&init_info, mode);
	if (ret) {
		return ret;
	}

	ret = sensor_set_mode(sensor, &init_info, mode);
	if (ret) {
		return ret;
	}
	sensor->start = init_info.start;
	sensor->interface = init_info.interface;
	isp_assert(sizeof(sensor->init_regs) == sizeof(init_info.sensor_regs));
	isp_memcpy(sensor->init_regs, init_info.sensor_regs,
			   sizeof(sensor->init_regs));
	isp_info("change sensor mode => %dx%d@%.3ffps - '%s'\n", mode->size.w,
			 mode->size.h, mode->fps, get_hdr_str(mode->hdr));

	return RTS_ISP_OK;
}

static int sensor_load_plugin(struct isp_mod_sensor *sensor)
{
	int ret;

	ret = sensor_init_plugin(sensor);
	if (ret) {
		return ret;
	}
	ret = sensor_get_info(sensor);
	if (ret) {
		return ret;
	}
	return sensor_change_mode(sensor, &sensor->info.modes.mode[0]);
}

static int sensor_set_power(struct isp_mod_sensor *sensor, int enable)
{
	if (enable) {
		return isp_os_driver_set_power(&sensor->info.power_up);
	} else {
		return isp_os_driver_set_power(&sensor->info.power_down);
	}
}

static int sensor_initialize(struct isp_mod_sensor *sensor)
{
	int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(sensor->init_regs); i++) {
		if (!sensor->init_regs[i].num) {
			break;
		}
		ret = rts_isp_write_i2c_regs(&sensor->info.i2c,
									 &sensor->init_regs[i]);
		if (ret) {
			return ret;
		}
	}
	return RTS_ISP_OK;
}

#define MAX_SNR_SYNC_NUM 32
#define MAX_ISP_SYNC_NUM 16

static int alloc_sync_regs(struct rts_isp_sync_regs *regs,
						   uint32_t total_sync_num)
{
	if (!regs) {
		return -RTS_ISP_EINVAL;
	}

	regs->reg = isp_malloc(total_sync_num * sizeof(*regs->reg));
	if (!regs->reg) {
		return -RTS_ISP_ENOMEM;
	}
	regs->split_index = 0;

	return RTS_ISP_OK;
}

static void free_sync_regs(struct rts_isp_sync_regs *regs)
{
	if (!regs) {
		return;
	}
	isp_free(regs->reg);
}

static void fill_i2c_sync_regs_info(struct rts_isp_sync_regs *regs,
									struct rts_isp_i2c_info *i2c_info)
{
	int i;

	for (i = 0; i < regs->num; i++) {
		if (regs->reg[i].type == RTS_ISP_SYNC_TYPE_I2C) {
			regs->reg[i].i2c.i2c_id = i2c_info->i2c_id;
			regs->reg[i].i2c.addr_len = i2c_info->addr_len;
			regs->reg[i].i2c.data_len = i2c_info->data_len;
		}
	}
}

static int sensor_fill_sensor_exp_gain(struct isp_mod_sensor *sensor,
									   struct rts_isp_sensor_exp_gain *exp_gain,
									   struct rts_isp_i2c_info *i2c_info,
									   struct rts_isp_sync_regs *regs)
{
	int ret;
	const struct rts_isp_sensor_ops *ops = sensor->sensor_ops;

	regs->num = 0;
	/* sensor exp gain func may change sensor->isp_gain_delay */
	sensor->isp_gain_delay = 1;
	ret = ops->get_exposure_gain_info(sensor->mod.owner_id, exp_gain, regs);
	if (ret) {
		isp_perror(ret, "get sensor exposure gain info fail");
		return ret;
	}
	if (regs->num > MAX_SNR_SYNC_NUM) {
		ret = -RTS_ISP_ERANGE;
		isp_perror(ret, "sensor sync regs number > %d",
				   MAX_SNR_SYNC_NUM);
		return ret;
	}
	if (sensor->isp_gain_delay != 1) {
		ret = isp_mod_sibling_get_info(&sensor->mod, MOD_AE,
									   AE_EXEC_SET_RESULT_DELAY,
									   &sensor->isp_gain_delay,
									   sizeof(sensor->isp_gain_delay));
		if (ret) {
			return ret;
		}
	}
	fill_i2c_sync_regs_info(regs, i2c_info);

	return RTS_ISP_OK;
}

static int sensor_fill_normal_sensor_exp_gain(struct isp_mod_sensor *sensor,
		struct rts_isp_sync_regs *regs)
{
	int ret;

	ret = sensor_fill_sensor_exp_gain(sensor, &sensor->exp_gain,
									  &sensor->info.i2c, regs);
	if (ret) {
		return ret;
	}
	regs->split_index = regs->num;

	return RTS_ISP_OK;
}

static int sensor_fill_isp_gain(struct isp_mod_sensor *sensor,
								struct rts_isp_sync_regs *regs)
{
	int ret;
	struct isp_mod_ae_sync_regs sync_regs;

	sync_regs.reg = &regs->reg[regs->num];
	sync_regs.num = 0;
	sync_regs.delay = sensor->isp_gain_delay;

	ret = isp_mod_sibling_execute(&sensor->mod, MOD_AE,
								  AE_EXEC_FILL_SYNC_REGS, &sync_regs,
								  sizeof(sync_regs));
	if (ret) {
		return ret;
	}
	if (sync_regs.num > MAX_ISP_SYNC_NUM) {
		ret = -RTS_ISP_ERANGE;
		isp_perror(ret, "isp sync regs number > %d", MAX_ISP_SYNC_NUM);
		return ret;
	}
	regs->num += sync_regs.num;

	return RTS_ISP_OK;
}

static int sensor_set_exp_gain(struct isp_mod_sensor *sensor)
{
	int ret;
	int total_sync_num;
	struct rts_isp_sync_regs regs;

	total_sync_num = MAX_SNR_SYNC_NUM + MAX_ISP_SYNC_NUM;
	ret = alloc_sync_regs(&regs, total_sync_num);
	if (ret) {
		return ret;
	}

	ret = sensor_fill_normal_sensor_exp_gain(sensor, &regs);
	if (ret) {
		goto out;
	}
	ret = sensor_fill_isp_gain(sensor, &regs);
	if (ret) {
		goto out;
	}

	ret = isp_os_driver_write_sync_regs(&regs);
out:
	free_sync_regs(&regs);
	return ret;
}

static int sensor_set_mirror_flip(struct isp_mod_sensor *sensor)
{
	int ret;
	struct rts_isp_sync_regs regs;
	struct rts_isp_mirror_flip mf_info;

	mf_info.mirror = sensor->mirror_flip_ctrl->val & 1;
	mf_info.flip = (sensor->mirror_flip_ctrl->val >> 1) & 1;

	ret = alloc_sync_regs(&regs, MAX_SNR_SYNC_NUM);
	if (ret) {
		return ret;
	}

	ret = sensor->sensor_ops->get_mirror_flip(sensor->mod.owner_id,
			&mf_info, &regs);
	if (ret) {
		isp_perror(ret, "set sensor mirror or flip fail");
		goto out;
	}
	if (regs.num > MAX_SNR_SYNC_NUM) {
		ret = -RTS_ISP_ERANGE;
		isp_perror(ret, "sensor sync regs number > %d",
				   MAX_SNR_SYNC_NUM);
		goto out;
	}
	fill_i2c_sync_regs_info(&regs, &sensor->info.i2c);

	ret = isp_os_driver_write_sync_regs(&regs);
	if (ret) {
		goto out;
	}
	ret = isp_core_notify(sensor->mod.owner, ISP_NOTIFY_MIRROR_FLIP_CHANGE,
						  &mf_info, sizeof(mf_info));
out:
	free_sync_regs(&regs);
	return ret;
}

static int sensor_calc_vts(struct isp_mod_sensor *sensor, float fps)
{
	uint32_t vts;

	vts = roundf(sensor->mode.min_vts * sensor->mode.max_fps / fps);
	if (sensor->mode.hdr == RTS_ISP_HDR_NONE) {
		uint32_t exp_lines;

		/* can set 40ms exposure at 25 fps by a little fps reducing */
		exp_lines = roundf(sensor->exp_gain.exposure[0] /
						   (float)(1e6 / sensor->mode.max_fps) *
						   sensor->mode.min_vts);
		if (vts < exp_lines) {
			return -RTS_ISP_ERANGE;
		}
		vts = max(exp_lines + 4, vts);
	}
	sensor->exp_gain.vts = clamp(vts + sensor->flick_dummy,
								 sensor->mode.min_vts,
								 sensor->mode.max_vts);
	return RTS_ISP_OK;
}

static int sensor_set_exp_gain_fps(struct isp_mod_sensor *sensor, float fps)
{
	int ret;

	if (float_eq(sensor->set_fps, 0) || float_eq(fps, 0)) {
		return RTS_ISP_OK;
	}

	ret = sensor_calc_vts(sensor, fps);
	if (ret) {
		return ret;
	}
	sensor->fps = fps;

	if (sensor->fps_change_notifying) {
		return RTS_ISP_OK;
	}

	return sensor_set_exp_gain(sensor);
}

static int sensor_timer_callback(isp_timer_handle_t *timer, void *data)
{
	int ret;
	struct isp_mod *mod = data;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	const struct isp_notify_dynamic *dyn;
	uint32_t gain_th;

	isp_top_lock_internal();
	ret = isp_mod_sibling_get_info(mod, MOD_IQ, IQ_INFO_DYNAMIC, &dyn,
								   sizeof(dyn));
	if (ret) {
		isp_top_unlock_internal();
		return ret;
	}
	gain_th = sensor->gain_is_high ? sensor->iq->gain.th1 :
			  sensor->iq->gain.th0;
	if (dyn->ae.iq_total_gain[0] > gain_th) {
		int temp;
		int temp_th;

		sensor->gain_is_high = RTS_ISP_TRUE;
		isp_os_driver_sync_lock(RTS_ISP_TRUE);
		temp = sensor->sensor_ops->get_temperature(mod->owner_id);
		isp_os_driver_sync_lock(RTS_ISP_FALSE);
		temp = max(temp, 0);
		sensor->algo_param->sensor_temperature = temp;
		temp_th = sensor->temp_is_high ? sensor->iq->temp.th1 :
				  sensor->iq->temp.th0;
		if (temp > temp_th) {
			sensor->temp_is_high = RTS_ISP_TRUE;
			sensor->dynamic.temperature = temp;
		} else {
			sensor->temp_is_high = RTS_ISP_FALSE;
		}
	} else {
		sensor->gain_is_high = RTS_ISP_FALSE;
	}
	sensor->dynamic.high_temp_en = sensor->gain_is_high &&
								   sensor->temp_is_high;
	sensor->algo_param->high_temp_en = sensor->dynamic.high_temp_en;

	ret = isp_mod_sibling_execute(mod, MOD_IQ, IQ_EXEC_SET_DYNAMIC_SENSOR,
								  &sensor->dynamic,
								  sizeof(sensor->dynamic));
	isp_top_unlock_internal();
	return ret;
}

static int sensor_high_temp_start(struct isp_mod_sensor *sensor, int start)
{
	int ret;

	sensor->gain_is_high = RTS_ISP_FALSE;
	sensor->temp_is_high = RTS_ISP_FALSE;

	if (start && sensor->iq->enable &&
		sensor->sensor_ops->get_temperature) {
		ret = isp_timer_start(&sensor->timer, 0,
							  max_t(uint32_t, 5000,
									sensor->iq->interval));
	} else {
		ret = isp_timer_stop(&sensor->timer);
		if (ret) {
			return ret;
		}
		sensor->dynamic.high_temp_en = RTS_ISP_FALSE;
		sensor->dynamic.temperature = 0;
		ret = isp_mod_sibling_execute(&sensor->mod, MOD_IQ,
									  IQ_EXEC_SET_DYNAMIC_SENSOR,
									  &sensor->dynamic,
									  sizeof(sensor->dynamic));
	}
	return ret;
}

static int sensor_set_focus(struct isp_mod_sensor *sensor)
{
	int ret;

	if (float_eq(sensor->set_fps, 0)) {
		return RTS_ISP_OK;
	}

	if (sensor->sensor_ops->set_focus) {
		if (sensor->focus_pos > sensor->info.focus.max ||
			sensor->focus_pos < sensor->info.focus.min) {
			return -RTS_ISP_EINVAL;
		}
		isp_os_driver_sync_lock(RTS_ISP_TRUE);
		ret = sensor->sensor_ops->set_focus(sensor->mod.owner_id,
											sensor->focus_pos);
		isp_os_driver_sync_lock(RTS_ISP_FALSE);
		return ret;
	}
	return RTS_ISP_OK;
}

static int sensor_start(struct isp_mod_sensor *sensor)
{
	int ret;
	int early = isp_sensor_take_early_started(sensor->mod.owner_id);

	isp_info("start sensor %s%s\n", sensor->sensor_ops->name, early ? " (early started)" : "");

	/* An early-started sensor is already powered and streaming; a power
	 * cycle or the init table (soft reset first) would restart it.
	 */
	if (!early) {
		ret = sensor_set_power(sensor, RTS_ISP_TRUE);
		if (ret) {
			goto err;
		}
		ret = sensor_initialize(sensor);
		if (ret) {
			goto err;
		}
	}
	if (sensor->sensor_ops->start && rts_isp_get_remap_mode(ISP0) == 0) {
		ret = sensor->sensor_ops->start(sensor->mod.owner_id);
		if (ret) {
			goto err;
		}
	}

	sensor->fps = sensor->mode.max_fps;
	sensor->exp_gain.vts = sensor->mode.min_vts;

	if (sensor->patch_ops && sensor->patch_ops->preview_start) {
		ret = sensor->patch_ops->preview_start(sensor->mod.owner_id);
		if (ret) {
			goto err_sensor_start;
		}
	}
	ret = sensor_high_temp_start(sensor, RTS_ISP_TRUE);
	if (ret) {
		goto err_patch_start;
	}

	return RTS_ISP_OK;

err_patch_start:
	if (sensor->patch_ops && sensor->patch_ops->preview_stop) {
		sensor->patch_ops->preview_stop(sensor->mod.owner_id);
	}
err_sensor_start:
	if (sensor->sensor_ops->stop) {
		sensor->sensor_ops->stop(sensor->mod.owner_id);
	}
err:
	sensor_high_temp_start(sensor, RTS_ISP_FALSE);
	sensor_set_power(sensor, RTS_ISP_FALSE);
	isp_perror(ret, "start sensor fail");

	return ret;
}

static int sensor_stop(struct isp_mod_sensor *sensor)
{
	isp_info("stop sensor\n");

	sensor->fps = 0;
	sensor->set_fps = 0;
	sensor->exp_gain.vts = sensor->mode.min_vts;

	sensor_high_temp_start(sensor, RTS_ISP_FALSE);
	if (sensor->patch_ops && sensor->patch_ops->preview_stop) {
		sensor->patch_ops->preview_stop(sensor->mod.owner_id);
	}
	if (sensor->sensor_ops->stop) {
		sensor->sensor_ops->stop(sensor->mod.owner_id);
	}
	sensor_set_power(sensor, RTS_ISP_FALSE);

	return RTS_ISP_OK;
}

static int sensor_change_fps(struct isp_mod_sensor *sensor, float fps)
{
	int ret;

	if (float_eq(fps, 0)) {
		return RTS_ISP_OK;
	}

	isp_info("change sensor fps to %.3f\n", fps);
	/*
	 * first send fps change, so ae can change exposure to good range,
	 * use fps_change_notifying to prevent ae set exp gain to sensor
	 */
	sensor->set_fps = fps;
	sensor->fps_change_notifying = RTS_ISP_TRUE;
	ret = isp_core_notify(sensor->mod.owner, ISP_NOTIFY_FPS_CHANGE, &fps,
						  sizeof(fps));
	sensor->fps_change_notifying = RTS_ISP_FALSE;
	if (ret) {
		return ret;
	}
	ret = sensor_set_focus(sensor);
	if (ret) {
		return ret;
	}
	if (sensor->mirror_flip_ctrl->val) {
		ret = sensor_set_mirror_flip(sensor);
		if (ret) {
			return ret;
		}
	}
	/* use sensor->fps for keeping dynamic fps */
	return sensor_set_exp_gain_fps(sensor, sensor->fps);
}

static int mod_sensor_exec_set_fps(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	float fps = *(float *)data;

	if (fps < 0 || fps > sensor->mode.max_fps) {
		return -RTS_ISP_ERANGE;
	}

	if (float_eq(fps, sensor->set_fps)) {
		return RTS_ISP_OK;
	}

	if (float_eq(sensor->set_fps, 0)) {
		ret = sensor_start(sensor);
	} else if (float_eq(fps, 0)) {
		ret = sensor_stop(sensor);
	}
	if (ret) {
		return ret;
	}

	return sensor_change_fps(sensor, fps);
}

static int mod_sensor_exec_set_exp_gain(struct isp_mod *mod, void *data)
{
	float max_fps;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	struct isp_sensor_exp_gain *snr_exp_gain = data;
	const struct rts_isp_ae_exp_gain *exp_gain;

	if (float_eq(sensor->set_fps, 0)) {
		max_fps = sensor->mode.max_fps;
	} else {
		max_fps = sensor->set_fps;
	}

	if (!snr_exp_gain->exp_gain || snr_exp_gain->fps > max_fps ||
		snr_exp_gain->fps < sensor->mode.min_fps) {
		return -RTS_ISP_ERANGE;
	}

	/* exposure, gain and fps has been verified at mod ae, we trust it */
	exp_gain = snr_exp_gain->exp_gain;
	isp_memcpy(sensor->exp_gain.exposure, exp_gain->exposure,
			   sizeof(sensor->exp_gain.exposure));
	isp_memcpy(sensor->exp_gain.analog_gain, exp_gain->analog_gain,
			   sizeof(sensor->exp_gain.analog_gain));
	isp_memcpy(sensor->exp_gain.digital_gain, exp_gain->digital_gain,
			   sizeof(sensor->exp_gain.digital_gain));

	return sensor_set_exp_gain_fps(sensor, snr_exp_gain->fps);
}

static int mod_sensor_exec_read_sen_reg(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	struct rts_isp_i2c_reg *reg = data;

	if (float_eq(sensor->set_fps, 0)) {
		return -RTS_ISP_ENOTREADY;
	}
	return rts_isp_read_i2c_reg(&sensor->info.i2c, reg);
}

static int mod_sensor_exec_write_sen_reg(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	struct rts_isp_i2c_reg *reg = data;

	if (float_eq(sensor->set_fps, 0)) {
		return -RTS_ISP_ENOTREADY;
	}
	return rts_isp_write_i2c_reg(&sensor->info.i2c, reg);
}

static int mod_sensor_exec_set_dummy(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	sensor->flick_dummy = *(uint32_t *)data;

	return sensor_set_exp_gain_fps(sensor, sensor->fps);
}

static int mod_sensor_exec_set_focus(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	int ret;

	sensor->focus_pos = *(uint32_t *)data;

	ret = sensor_set_focus(sensor);
	if (ret) {
		isp_perror(ret, "sensor set focus fail");
	}
	return ret;
}

static int mod_sensor_exec_enum_modes(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	*(struct rts_isp_sensor_modes *)data = sensor->info.modes;

	return RTS_ISP_OK;
}

static int mod_sensor_exec_get_mode(struct isp_mod *mod, void *data)
{
	struct rts_isp_sensor_mode *mode = data;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	mode->hdr = sensor->mode.hdr;
	mode->size = sensor->mode.size.isp;
	mode->fps = sensor->mode.max_fps;

	return RTS_ISP_OK;
}

static int mod_sensor_exec_set_mode(struct isp_mod *mod, void *data)
{
	int ret;
	const struct rts_isp_sensor_mode *mode = data;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	const struct rts_isp_sensor_modes *modes = &sensor->info.modes;

	if (sensor->set_fps) {
		return -RTS_ISP_EBUSY;
	}

	if (check_mode(mode)) {
		return -RTS_ISP_EINVAL;
	}
	if (check_mode_compatible(mode, modes)) {
		return -RTS_ISP_EINVAL;
	}

	if (mode->hdr == sensor->mode.hdr &&
		mode->size.w == sensor->mode.size.isp.w &&
		mode->size.h == sensor->mode.size.isp.h &&
		float_eq(mode->fps, sensor->mode.max_fps)) {
		return RTS_ISP_OK;
	}

	/* An early-started sensor still runs the probe-time table: drop the
	 * flag so the next start powers up and writes the new mode's table.
	 */
	isp_sensor_take_early_started(sensor->mod.owner_id);
	ret = sensor_change_mode(sensor, mode);
	if (ret) {
		return ret;
	}
	return isp_core_notify(mod->owner, ISP_NOTIFY_MODE_CHANGE,
						   &sensor->mode, sizeof(sensor->mode));
}

static int mod_sensor_exec_i2c_transfer(struct isp_mod *mod, void *data)
{
	int ret;
	void *buf;
	void *end;
	void *i2c_buf;
	struct isp_msg_buf *msg = *(void **)data;
	struct isp_mod_sensor_i2c_msg *buf_msg;
	uint32_t msg_len = sizeof(msg->hdr);

	buf = msg->data;
	end = buf + isp_msg_data_capacity(&msg->hdr);

	ret = get_buf_ptr(buf_msg, buf, end);
	if (ret) {
		goto out;
	}
	ret = get_buf_sized_ptr(i2c_buf, buf_msg->len, buf, end);
	if (ret) {
		goto out;
	}

	if (buf_msg->is_read)
		ret = isp_os_driver_i2c_read(buf_msg->i2c_id, i2c_buf,
									 buf_msg->len);
	else
		ret = isp_os_driver_i2c_write(buf_msg->i2c_id, i2c_buf,
									  buf_msg->len);
	if (ret) {
		goto out;
	}
	if (buf_msg->is_read) {
		msg_len = msg->hdr.capacity;
	}
out:
	isp_msg_to_resp(msg, ret, msg_len);
	return RTS_ISP_OK;
}

static int mod_sensor_exec_set_isp_gain_delay(struct isp_mod *mod, void *data)
{
	uint32_t delay = *(uint32_t *)data;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	if (delay > 3) {
		return -RTS_ISP_EINVAL;
	}

	sensor->isp_gain_delay = delay;

	return 0;
}

static struct isp_mod_action_info sensor_exec_actions[] = {
	ISP_MOD_ACTION(SENSOR_EXEC_SET_FPS, mod_sensor_exec_set_fps),
	ISP_MOD_ACTION(SENSOR_EXEC_SET_EXP_GAIN, mod_sensor_exec_set_exp_gain),
	ISP_MOD_ACTION(SENSOR_EXEC_READ_SEN_REG, mod_sensor_exec_read_sen_reg),
	ISP_MOD_ACTION(SENSOR_EXEC_WRITE_SEN_REG,
				   mod_sensor_exec_write_sen_reg),
	ISP_MOD_ACTION(SENSOR_EXEC_SET_DUMMY, mod_sensor_exec_set_dummy),
	ISP_MOD_ACTION(SENSOR_EXEC_SET_FOCUS, mod_sensor_exec_set_focus),
	ISP_MOD_ACTION(SENSOR_EXEC_ENUM_MODES, mod_sensor_exec_enum_modes),
	ISP_MOD_ACTION(SENSOR_EXEC_GET_MODE, mod_sensor_exec_get_mode),
	ISP_MOD_ACTION(SENSOR_EXEC_SET_MODE, mod_sensor_exec_set_mode),
	ISP_MOD_ACTION(SENSOR_EXEC_I2C_TRANSFER, mod_sensor_exec_i2c_transfer),
	ISP_MOD_ACTION(SENSOR_EXEC_SET_ISP_GAIN_DELAY,
				   mod_sensor_exec_set_isp_gain_delay),
};

static int mod_sensor_info_crop(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	rts_isp_point_t *start = data;

	*start = sensor->start;

	return RTS_ISP_OK;
}

static int mod_sensor_info_mode(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	const struct isp_sensor_mode **mode = data;

	*mode = &sensor->mode;

	return RTS_ISP_OK;
}

static int mod_sensor_info_interface(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	const struct rts_isp_interface_info **info = data;

	*info = &sensor->interface;

	return RTS_ISP_OK;
}

static int mod_sensor_info_bound_name(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	isp_strlcpy(data, sensor->sensor_ops->name,
				ISP_AID_SIZE(SENSOR_INFO_BOUND_NAME));
	return RTS_ISP_OK;
}

static int mod_sensor_info_vcm(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	struct rts_isp_focus_info *value = data;

	*value = sensor->info.focus;
	return RTS_ISP_OK;
}

static int mod_sensor_info_patch_ops(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	*(const void **)data = sensor->patch_ops;

	return RTS_ISP_OK;
}

static int mod_sensor_info_version(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	const struct rts_isp_sensor_ops *ops;
	uint16_t *version = data;

	if (sensor->sensor_ops) {
		*version = sensor->sensor_ops->api_version &
				   SENSOR_VERSION_MASK;
		return RTS_ISP_OK;
	}

	ret = sensor_get_snr_ops(sensor, &ops);
	if (ret) {
		return ret;
	}
	*version = ops->api_version & SENSOR_VERSION_MASK;

	return RTS_ISP_OK;
}

static int mod_sensor_info_tune_ops(struct isp_mod *mod, void *data)
{
	const struct rts_isp_exp_gain_tune_ops **ops = data;

	*ops = &exp_gain_tune_ops;

	return RTS_ISP_OK;
}

static int mod_sensor_info_front_size(struct isp_mod *mod, void *data)
{
	struct isp_front_size *size = data;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	size->bit_depth = sensor->interface.bit_depth;
	size->snr = sensor->mode.size.snr;
	size->isp = sensor->mode.size.isp;

	return RTS_ISP_OK;
}

static struct isp_mod_action_info sensor_info_actions[] = {
	ISP_MOD_ACTION(SENSOR_INFO_CROP, mod_sensor_info_crop),
	ISP_MOD_ACTION(SENSOR_INFO_MODE, mod_sensor_info_mode),
	ISP_MOD_ACTION(SENSOR_INFO_INTERFACE, mod_sensor_info_interface),
	ISP_MOD_ACTION(SENSOR_INFO_BOUND_NAME, mod_sensor_info_bound_name),
	ISP_MOD_ACTION(SENSOR_INFO_VCM, mod_sensor_info_vcm),
	ISP_MOD_ACTION(SENSOR_INFO_PATCH_OPS, mod_sensor_info_patch_ops),
	ISP_MOD_ACTION(SENSOR_INFO_VERSION, mod_sensor_info_version),
	ISP_MOD_ACTION(SENSOR_INFO_TUNE_OPS, mod_sensor_info_tune_ops),
	ISP_MOD_ACTION(SENSOR_INFO_FRONT_SIZE, mod_sensor_info_front_size),
};

static int mod_sensor_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	sensor->iq = isp_iq_get_high_temp_v1(mod->owner_id,
										 &sensor->algo_param->common);
	if (!sensor->iq) {
		return -RTS_ISP_ENOITEM;
	}

	if (sensor->patch_ops) {
		int ret;
		int id = isp_iq_current_table(mod->owner_id);
		int night = isp_iq_is_night(mod->owner_id);

		ret = sensor->patch_ops->iq_change(mod->owner_id, id, night);
		if (ret) {
			return ret;
		}
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING) {
		return sensor_high_temp_start(sensor, RTS_ISP_TRUE);
	}

	return RTS_ISP_OK;
}

static int mod_sensor_stop_pre(struct isp_mod *mod, void *data)
{
	return isp_os_driver_clear_sync_regs();
}

static int mod_sensor_plugin_update(struct isp_mod *mod, void *data)
{
	int ret;
	uint32_t notify_data = *(uint32_t *)data;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	if (notify_data != RTS_ISP_PLUG_TYPE_SENSOR) {
		return RTS_ISP_OK;
	}

	sensor_cleanup_plugin(sensor);
	ret = sensor_load_plugin(sensor);
	if (ret) {
		goto err;
	}
	ret = isp_core_notify_sched(mod->owner, ISP_NOTIFY_MODE_CHANGE,
								&sensor->mode, sizeof(sensor->mode), NULL,
								NULL);
	if (ret) {
		goto err;
	}
	if (sensor_mirror_flip_enable(sensor)) {
		v4l2_ctrl_modify_range(sensor->mirror_flip_ctrl, 0, 3, 0, 0);
	} else {
		v4l2_ctrl_modify_range(sensor->mirror_flip_ctrl, 0, 0, 0, 0);
	}

	return RTS_ISP_OK;
err:
	sensor_cleanup_plugin(sensor);
	return ret;
}

static struct isp_mod_action_info sensor_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_sensor_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STOP_PRE, mod_sensor_stop_pre),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_PLUGIN_UPDATE,
						   ISP_PLUGIN_UPDATE_SENSOR,
						   mod_sensor_plugin_update),
};

static int sensor_set_mirror_flip_ctrl(struct isp_mod_sensor *sensor)
{
	if (float_eq(sensor->set_fps, 0)) {
		return RTS_ISP_OK;
	}

	return sensor_set_mirror_flip(sensor);
}

static int sensor_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	switch (ctrl->id) {
	case ISP_V4L2_CID_MIRROR_FLIP:
		ret = sensor_set_mirror_flip_ctrl(sensor);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set sensor ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops sensor_ctrl_ops = {
	.s_ctrl = sensor_s_ctrl,
};

static int isp_mod_sensor_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_sensor *sensor;

	sensor = get_mod(mod, sensor);
	sensor->hw_ops = isp_hw_sensor_get_ops();
	if (sensor->hw_ops == NULL) {
		isp_error("get sensor hw ops fail\n");
		return -RTS_ISP_ENOOPS;
	}

	ret = isp_timer_init(&sensor->timer, isp_top_get_poll(),
						 sensor_timer_callback, mod);
	if (ret) {
		goto err;
	}
	sensor->algo_param = isp_algo_param_get_high_temp_v1(mod->owner_id);
	if (!sensor->algo_param) {
		isp_error("get high_temp algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;

err:
	isp_timer_cleanup(&sensor->timer);
	return ret;
}

static int isp_mod_sensor_cleanup(struct isp_mod *mod)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);

	isp_timer_cleanup(&sensor->timer);
	sensor_cleanup_plugin(sensor);

	return RTS_ISP_OK;
}

static struct isp_mod_sensor sensor;

static int isp_mod_sensor_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_sensor *sensor = get_mod(mod, sensor);
	struct v4l2_ctrl_handler *handler = phandler;
	static const char *const mirror_flip_menu[] = {
		"default",
		"mirror",
		"flip",
		"mirror and flip",
	};
	struct v4l2_ctrl_config snr_mirror_flip_config = {
		.ops = &sensor_ctrl_ops,
		.id = ISP_V4L2_CID_MIRROR_FLIP,
		.name = "Set mirror and flip",
		.type = ISP_V4L2_CTRL_TYPE_MENU,
		.max = 0,
		.qmenu = mirror_flip_menu,
		.def = 0,
	};

	sensor->mirror_flip_ctrl =
		v4l2_ctrl_new_custom(handler, &snr_mirror_flip_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add sensor ctrls fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

int isp_register_mod_sensor(void)
{
	isp_memset(&sensor, 0, sizeof(sensor));
	sensor.mod.id = MOD_SENSOR;
	sensor.mod.name = "sensor";
	sensor.mod.init = isp_mod_sensor_init;
	sensor.mod.cleanup = isp_mod_sensor_cleanup;
	sensor.mod.add_ctrl = isp_mod_sensor_add_ctrl;
	sensor.mod.exec_actions = sensor_exec_actions;
	sensor.mod.exec_actions_num = ARRAY_SIZE(sensor_exec_actions);
	sensor.mod.info_actions = sensor_info_actions;
	sensor.mod.info_actions_num = ARRAY_SIZE(sensor_info_actions);
	sensor.mod.notify_actions = sensor_notify_actions;
	sensor.mod.notify_actions_num = ARRAY_SIZE(sensor_notify_actions);
	sensor.mod.size = sizeof(sensor);
	return isp_mod_register(&sensor.mod);
}

void isp_unregister_mod_sensor(void)
{
	isp_mod_unregister(&sensor.mod);
}
