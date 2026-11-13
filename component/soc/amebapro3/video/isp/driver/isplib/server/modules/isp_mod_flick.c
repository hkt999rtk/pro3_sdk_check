/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 * Copyright (C) 2018 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <rts_isp_other.h>
#include <isp_v4l2_ctrl.h>
#include <isp_core.h>
#include <isp_statis.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_mod.h>
#include <isp_algo.h>
#include <isp_hw_flick.h>
#include "isp_mod_sensor.h"
#include "isp_mod_ae.h"
#include "isp_mod_flick.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_FLICK
#define flick_algo_ctrl(flick, ...)                                 \
	flick->flick_algo->ctrl(flick->mod.owner_id, ##__VA_ARGS__)

struct isp_mod_flick {
	struct isp_mod mod;
	const struct rts_isp_flick_algo *flick_algo;
	const struct isp_hw_flick_ops *hw_ops;
	float ae_exposure;
	uint32_t banding_line;
	struct {
		int32_t ae_status; /* enum rts_isp_ae_algo_status */
		int ae_info_first_set;
	};
	struct v4l2_ctrl *flick_ctrl;

	const isp_iq_flick_v1_t *iq;
	isp_algo_param_flick_v1_t *algo_param;
};

static int mod_flick_set_result(struct isp_mod *mod,
								const struct rts_isp_flick_result *result)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_flick *flick = get_mod(mod, flick);
	uint32_t insert_dummy = result->config.insert_dummy_line;
	int32_t power_line_freq = result->power_line_freq;

	if (result->config.need_update) {
		/* dummy line should be greater than 1/10 banding line */
		if (insert_dummy)
			insert_dummy =
				max(flick->banding_line / 10, insert_dummy);
		ret = isp_mod_sibling_execute(mod, MOD_SENSOR,
									  SENSOR_EXEC_SET_DUMMY,
									  &insert_dummy,
									  sizeof(insert_dummy));
		if (ret) {
			goto out;
		}
	}

	if (result->need_update) {
		ret = isp_mod_sibling_execute(mod, MOD_AE, AE_EXEC_SET_PLFREQ,
									  &power_line_freq,
									  sizeof(power_line_freq));
		if (ret) {
			goto out;
		}
	}

out:
	if (ret) {
		isp_perror(ret, "isp set flick result fail");
	}
	return ret;
}

static int set_ae_info(struct isp_mod *mod)
{
	int ret;
	float ae_exposure;
	int32_t ae_status; /* enum rts_isp_ae_algo_status */
	struct isp_mod_flick *flick = get_mod(mod, flick);

	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_EXPOSURE,
								   &ae_exposure, sizeof(ae_exposure));
	if (ret) {
		goto out;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_STATUS, &ae_status,
								   sizeof(ae_status));
	if (ret) {
		goto out;
	}
	if (flick->ae_info_first_set ||
		float_ne(flick->ae_exposure, ae_exposure)) {
		flick->ae_exposure = ae_exposure;
		if (flick->flick_algo) {
			ret = flick_algo_ctrl(flick, SET_AE_EXPOSURE,
								  flick->ae_exposure);
			if (ret) {
				isp_perror(ret, "set ae exposure fail");
				return ret;
			}
		}
	}
	if (flick->ae_info_first_set || flick->ae_status != ae_status) {
		flick->ae_status = ae_status;
		if (flick->flick_algo) {
			ret = flick_algo_ctrl(flick, SET_AE_STATUS,
								  flick->ae_status);
			if (ret) {
				isp_perror(ret, "set ae status fail");
				return ret;
			}
			flick->ae_info_first_set = 0;
		}
	}
out:
	if (ret) {
		isp_perror(ret, "flick get ae info fail");
	}
	return ret;
}

static int mod_flick_statis_done(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	const struct rts_isp_flick_statis *statis;
	struct isp_mod_flick *flick = get_mod(mod, flick);
	struct rts_isp_flick_result result;

	debug("flick statis notify handing\n");

	statis = &((const struct rts_isp_statis *)data)->flick;
	if (flick->flick_algo && statis->valid) {
		ret = set_ae_info(mod);
		if (ret) {
			goto out;
		}
		isp_memset(&result, 0, sizeof(result));
		ret = flick->flick_algo->run(mod->owner_id, statis, &result);
		if (ret) {
			goto out;
		}
		ret = mod_flick_set_result(mod, &result);
		if (ret) {
			goto out;
		}
	}
out:
	if (ret) {
		isp_perror(ret, "flick statis done handle fail");
	}
	return ret;
}

static int mod_flick_cleanup_algo(struct isp_mod *mod)
{
	struct isp_mod_flick *flick = get_mod(mod, flick);
	uint32_t insert_dummy = 0;

	if (flick->flick_algo) {
		flick->flick_algo->cleanup(mod->owner_id);
		flick->flick_algo = NULL;
		isp_mod_sibling_execute(mod, MOD_SENSOR, SENSOR_EXEC_SET_DUMMY,
								&insert_dummy, sizeof(insert_dummy));
	}
	return RTS_ISP_OK;
}

static int check_flick(const struct rts_isp_flick_algo *flick_algo)
{
	if (!flick_algo) {
		return -RTS_ISP_EINVAL;
	}
	if (!flick_algo->init || !flick_algo->cleanup) {
		return -RTS_ISP_EINVAL;
	}
	if (!flick_algo->run || !flick_algo->ctrl) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int mod_flick_update_flick_info(struct isp_mod *mod,
									   struct rts_isp_flick_info *info,
									   struct rts_isp_flick_info_ex *info_ex)
{
	int ret;
	struct isp_mod_flick *flick = get_mod(mod, flick);
	struct isp_mod_ae_stat_info ae_info;
	const isp_iq_header_t *iq_header;
	const isp_algo_param_header_t *param_header;

	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_STATIS, &ae_info,
								   sizeof(ae_info));
	if (ret) {
		return ret;
	}

	isp_memset(info, 0, sizeof(*info));
	isp_memset(info_ex, 0, sizeof(*info_ex));
	info->stat_info = ae_info.info;
	info->cur_power_line_freq = flick->flick_ctrl->cur.val;
	info->iq = flick->iq;
	info->algo_param = flick->algo_param;

	iq_header = isp_iq_get_header(flick->mod.owner_id);
	param_header = isp_iq_get_algo_param_header(flick->mod.owner_id);
	if (!iq_header || !param_header) {
		return -RTS_ISP_EINVAL;
	}

	info_ex->iq_info.magic = iq_header->magic;
	info_ex->iq_info.version = iq_header->version;
	info_ex->iq_info.iq = flick->iq;
	info_ex->param_info.magic = param_header->magic;
	info_ex->param_info.version = param_header->version;
	info_ex->param_info.algo_param = flick->algo_param;

	info->resv[0] = ((uintptr_t)info_ex) & 0xff;
	info->resv[1] = (((uintptr_t)info_ex) & 0xff00) >> 8;
	info->resv[2] = (((uintptr_t)info_ex) & 0xff0000) >> 16;
	info->resv[3] = (((uintptr_t)info_ex) & 0xff000000) >> 24;

	return RTS_ISP_OK;
}

static int mod_flick_init_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_flick *flick = get_mod(mod, flick);
	struct rts_isp_flick_info info;
	struct rts_isp_flick_info_ex info_ex;

	ret = mod_flick_update_flick_info(mod, &info, &info_ex);
	if (ret) {
		return ret;
	}
	ret = flick->flick_algo->init(mod->owner_id, &info);
	if (ret) {
		return ret;
	}
	flick->ae_info_first_set = 1;

	return ret;
}

static int mod_flick_update_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_flick *flick = get_mod(mod, flick);

	mod_flick_cleanup_algo(mod);
	ret = isp_algo_get_flick(mod->owner_id, &flick->flick_algo);
	if (ret) {
		goto err;
	}
	if (!flick->flick_algo) {
		ret = -RTS_ISP_ENOITEM;
		goto err;
	}
	ret = check_flick(flick->flick_algo);
	if (ret) {
		goto err;
	}
	ret = mod_flick_init_algo(mod);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	isp_perror(ret, "update flick algo fail");
	mod_flick_cleanup_algo(mod);
	return ret;
}

static int mod_flick_plugin_update(struct isp_mod *mod, void *data)
{
	uint32_t notify_data;

	notify_data = *(uint32_t *)data;
	if (notify_data != RTS_ISP_PLUG_TYPE_ALGO_OTHER) {
		return RTS_ISP_OK;
	}

	return mod_flick_update_algo(mod);
}

static int mod_flick_mcrop_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_flick *flick = get_mod(mod, flick);
	const struct isp_sensor_mode *mode;
	const rts_isp_rect_t *isp_rect = data;
	rts_isp_rect_t rect;

	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_MODE, &mode,
								   sizeof(mode));
	if (ret) {
		goto err;
	}
	rect.x = isp_rect->x;
	rect.y = isp_rect->y;
	rect.w = isp_rect->w;
	/*
	 * Flicker statis contains up to 9 banding_nums
	 * flick_rect_h <= 9 * banding_lines
	 * banding_lines = min(banding_time_50Hz, banding_time_60Hz) / row_time
	 * row_time = hts / pclk;
	 * row_time_hdr = row_time * 2
	 */
	flick->banding_line = mode->pclk / (mode->hts * 120);
	rect.h = (uint64_t)mode->pclk * 9 / (mode->hts * 120);
	rect.h = min(rect.h, isp_rect->h);
	ret = flick->hw_ops->set_statis(mod->owner_id, &rect);
	if (ret) {
		goto err;
	}
	return RTS_ISP_OK;
err:
	isp_perror(ret, "set flick mcrop change fail\n");
	return ret;
}

static int mod_flick_iq_change(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_flick *flick = get_mod(mod, flick);

	flick->iq =
		isp_iq_get_flick_v1(mod->owner_id, &flick->algo_param->common);
	if (!flick->iq) {
		isp_error("get flick iq fail\n");
		ret = -RTS_ISP_EINVAL;
		return ret;
	}
	if (flick->flick_algo) {
		ret = mod_flick_update_algo(mod);
	}

	return ret;
}

static struct isp_mod_action_info flick_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_flick_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_PLUGIN_UPDATE, mod_flick_plugin_update),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_flick_mcrop_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_flick_iq_change),
};

static int flick_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_flick *flick = get_mod(mod, flick);

	if (ctrl->id != ISP_V4L2_CID_POWER_LINE_FREQUENCY) {
		return -RTS_ISP_ECTRL;
	}

	if (ctrl->val == ISP_V4L2_CID_POWER_LINE_FREQUENCY_AUTO) {
		mod_flick_update_algo(mod);
		if (!flick->flick_algo) {
			return -RTS_ISP_ENOTREADY;
		}
		ret = isp_mod_add_notify(mod, ISP_NOTIFY_STATIS_DONE);
		if (ret) {
			goto out;
		}
	} else {
		mod_flick_cleanup_algo(mod);
		ret = isp_mod_del_notify(mod, ISP_NOTIFY_STATIS_DONE);
		if (ret) {
			goto out;
		}
		ret = isp_mod_sibling_execute(mod, MOD_AE, AE_EXEC_SET_PLFREQ,
									  &ctrl->val, sizeof(ctrl->val));
		if (ret) {
			goto out;
		}
	}

out:
	if (ret) {
		isp_perror(ret, "set flick ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops flick_ctrl_ops = {
	.s_ctrl = flick_s_ctrl,
};

static int isp_mod_flick_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_flick *flick;

	flick = get_mod(mod, flick);

	flick->hw_ops = isp_hw_flick_get_ops();
	if (!flick->hw_ops) {
		isp_error("get flick hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	flick->algo_param = isp_algo_param_get_flick_v1(mod->owner_id);
	if (!flick->algo_param) {
		isp_error("get flick algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		return ret;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_flick_cleanup(struct isp_mod *mod)
{
	mod_flick_cleanup_algo(mod);

	return RTS_ISP_OK;
}

static int isp_mod_flick_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_flick *flick = get_mod(mod, flick);
	struct v4l2_ctrl_handler *handler = phandler;

	flick->flick_ctrl = v4l2_ctrl_new_std_menu(
							handler, &flick_ctrl_ops, ISP_V4L2_CID_POWER_LINE_FREQUENCY,
							ISP_V4L2_CID_POWER_LINE_FREQUENCY_AUTO, 0,
							ISP_V4L2_CID_POWER_LINE_FREQUENCY_50HZ, mod);
	if (handler->error) {
		isp_perror(handler->error, "add flick ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_flick flick;

int isp_register_mod_flick(void)
{
	isp_memset(&flick, 0, sizeof(flick));
	flick.mod.id = MOD_FLICK;
	flick.mod.name = "flick";
	flick.mod.init = isp_mod_flick_init;
	flick.mod.cleanup = isp_mod_flick_cleanup;
	flick.mod.add_ctrl = isp_mod_flick_add_ctrl;
	flick.mod.notify_actions = flick_notify_actions;
	flick.mod.notify_actions_num = ARRAY_SIZE(flick_notify_actions);
	flick.mod.size = sizeof(flick);
	return isp_mod_register(&flick.mod);
}

void isp_unregister_mod_flick(void)
{
	isp_mod_unregister(&flick.mod);
}
