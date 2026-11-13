/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 * Copyright (C) 2019 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <rts_isp_other.h>
#include <isp_v4l2_ctrl.h>
#include <isp_iq.h>
#include <isp_core.h>
#include <isp_statis.h>
#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_algo.h>
#include <isp_hw_wdr.h>
#include "isp_mod_ae.h"
#include "isp_mod_wdr.h"
#include "isp_mod_global.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_WDR
#define wdr_algo_ctrl(wdr, ...) \
	wdr->wdr_algo->ctrl(wdr->mod.owner_id, ##__VA_ARGS__)

struct isp_mod_wdr {
	struct isp_mod mod;
	const struct rts_isp_wdr_algo *wdr_algo;
	const struct isp_hw_wdr_ops *hw_ops;
	struct v4l2_ctrl *wdr_mode_ctrl;
	struct v4l2_ctrl *wdr_level_ctrl;
	struct isp_mod *mod_ae;
	const isp_iq_wdr_v1_t *iq;
	isp_algo_param_wdr_v1_t *algo_param;
};

static int wdr_set_level(struct isp_mod_wdr *wdr, int value)
{
	int ret, val_def;
	int level = value;

	val_def = wdr->hw_ops->get_iq_level(wdr->mod.owner_id);
	if (level < WDR_LEVEL_CTRL_DEF)
		level = linear_intp(level, WDR_LEVEL_CTRL_MIN,
							WDR_LEVEL_CTRL_MIN, WDR_LEVEL_CTRL_DEF,
							val_def);
	else
		level = linear_intp(level, WDR_LEVEL_CTRL_DEF, val_def,
							WDR_LEVEL_CTRL_MAX, WDR_LEVEL_CTRL_MAX);

	if (wdr->wdr_mode_ctrl->val == ISP_WDR_INNER_AUTO) {
		ret = wdr_algo_ctrl(wdr, SET_WDR_LEVEL_MANUAL, level);
	} else {
		level = linear_intp(level, 0, 0, 100, 120);
		ret = wdr->hw_ops->set_level(wdr->mod.owner_id, level, 0);
	}
	return ret;
}

static int mod_wdr_set_result(struct isp_mod *mod,
							  const struct rts_isp_wdr_result *result)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	if (result->need_update) {
		ret = wdr->hw_ops->set_step(mod->owner_id, result->step);
		if (ret) {
			goto err;
		}
		ret = wdr->hw_ops->set_level(mod->owner_id, result->level, 1);
	}
err:
	if (ret) {
		isp_perror(ret, "isp set wdr result fail");
	}
	return ret;
}

static int mod_wdr_statis_done(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	debug("wdr statis notify handing\n");

	if (wdr->wdr_algo) {
		struct rts_isp_wdr_result result;
		struct rts_isp_wdr_statis statis;

		statis.ae_statis = &((const struct rts_isp_statis *)data)->ae;
		result.need_update = RTS_ISP_FALSE;
		ret = wdr->wdr_algo->run(mod->owner_id, &statis, &result);
		if (ret) {
			goto out;
		}
		ret = mod_wdr_set_result(mod, &result);
		if (ret) {
			goto out;
		}
	}
out:
	if (ret) {
		isp_perror(ret, "wdr statis done handle fail");
	}
	return ret;
}

static int mod_wdr_cleanup_algo(struct isp_mod *mod)
{
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	if (wdr->wdr_algo) {
		wdr->wdr_algo->cleanup(mod->owner_id);
		wdr->wdr_algo = NULL;
	}
	return RTS_ISP_OK;
}

static int check_wdr(const struct rts_isp_wdr_algo *wdr_algo)
{
	if (!wdr_algo) {
		return -RTS_ISP_EINVAL;
	}
	if (!wdr_algo->init || !wdr_algo->cleanup) {
		return -RTS_ISP_EINVAL;
	}
	if (!wdr_algo->run || !wdr_algo->ctrl) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int mod_wdr_update_wdr_info(struct isp_mod *mod,
								   struct rts_isp_wdr_info *info,
								   struct rts_isp_wdr_info_ex *info_ex)
{
	int ret;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);
	struct isp_mod_ae_stat_info ae_info;
	const isp_iq_header_t *iq_header;
	const isp_algo_param_header_t *param_header;

	ret = isp_mod_get_info(wdr->mod_ae, AE_INFO_STATIS, &ae_info,
						   sizeof(ae_info));
	if (ret) {
		return ret;
	}

	isp_memset(info, 0, sizeof(*info));
	isp_memset(info_ex, 0, sizeof(*info_ex));
	info->stat_info = ae_info.info;
	info->iq = wdr->iq;
	info->algo_param = wdr->algo_param;

	iq_header = isp_iq_get_header(wdr->mod.owner_id);
	param_header = isp_iq_get_algo_param_header(wdr->mod.owner_id);
	if (!iq_header || !param_header) {
		return -RTS_ISP_EINVAL;
	}

	info_ex->iq_info.magic = iq_header->magic;
	info_ex->iq_info.version = iq_header->version;
	info_ex->iq_info.iq = wdr->iq;
	info_ex->param_info.magic = param_header->magic;
	info_ex->param_info.version = param_header->version;
	info_ex->param_info.algo_param = wdr->algo_param;

	info->resv[0] = ((uintptr_t)info_ex) & 0xff;
	info->resv[1] = (((uintptr_t)info_ex) & 0xff00) >> 8;
	info->resv[2] = (((uintptr_t)info_ex) & 0xff0000) >> 16;
	info->resv[3] = (((uintptr_t)info_ex) & 0xff000000) >> 24;

	return RTS_ISP_OK;
}

static int mod_wdr_init_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);
	struct rts_isp_wdr_info info;
	struct rts_isp_wdr_info_ex info_ex;
	uint16_t ae_target;
	int night = isp_iq_is_night(mod->owner_id);

	ret = mod_wdr_update_wdr_info(mod, &info, &info_ex);
	if (ret) {
		return ret;
	}
	ret = wdr->wdr_algo->init(mod->owner_id, &info);
	if (ret) {
		return ret;
	}
	ret = wdr_algo_ctrl(wdr, SET_DAY_NIGHT, night);
	if (ret) {
		return ret;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_TARGET, &ae_target,
								   sizeof(ae_target));
	if (ret) {
		return ret;
	}

	return wdr_algo_ctrl(wdr, SET_AE_TARGET, ae_target);
}

static int mod_wdr_update_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	mod_wdr_cleanup_algo(mod);
	ret = isp_algo_get_wdr(mod->owner_id, &wdr->wdr_algo);
	if (ret) {
		goto err;
	}
	if (!wdr->wdr_algo) {
		ret = -RTS_ISP_ENOITEM;
		goto err;
	}
	ret = check_wdr(wdr->wdr_algo);
	if (ret) {
		goto err;
	}
	ret = mod_wdr_init_algo(mod);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	isp_perror(ret, "update wdr algo fail");
	mod_wdr_cleanup_algo(mod);
	return ret;
}

static int mod_wdr_plugin_update(struct isp_mod *mod, void *data)
{
	uint32_t notify_data;

	notify_data = *(uint32_t *)data;
	if (notify_data != RTS_ISP_PLUG_TYPE_ALGO_OTHER) {
		return RTS_ISP_OK;
	}

	return mod_wdr_update_algo(mod);
}

static int mod_wdr_iq_change(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	wdr->iq = isp_iq_get_wdr_v1(mod->owner_id, &wdr->algo_param->common);
	if (!wdr->iq) {
		isp_error("get wdr iq fail\n");
		return -RTS_ISP_ENOITEM;
	}

	if (wdr->wdr_mode_ctrl->flags & ISP_V4L2_CTRL_FLAG_READ_ONLY) {
		wdr->wdr_mode_ctrl->flags &= ~ISP_V4L2_CTRL_FLAG_READ_ONLY;
		v4l2_ctrl_s_ctrl(wdr->wdr_mode_ctrl, wdr->iq->mode);
	}
	if (wdr->wdr_algo) {
		ret = mod_wdr_update_algo(mod);
		if (ret) {
			return ret;
		}
	}

	return RTS_ISP_OK;
}

static int mod_wdr_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	return wdr_set_level(wdr, wdr->wdr_level_ctrl->cur.val);
}

static struct isp_mod_action_info wdr_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_wdr_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_PLUGIN_UPDATE, mod_wdr_plugin_update),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_wdr_iq_change),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_wdr_dynamic_post),
};

static int wdr_s_ctrl(struct v4l2_ctrl *ctrl)
{
	uint32_t wdr_enable;
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	switch (ctrl->id) {
	case ISP_V4L2_CID_WDR_MODE:
		if (ctrl->val == ISP_WDR_INNER_AUTO) {
			mod_wdr_update_algo(mod);
			if (!wdr->wdr_algo) {
				return -RTS_ISP_ENOTREADY;
			}
			ret = isp_mod_add_notify(mod, ISP_NOTIFY_STATIS_DONE);
			if (ret) {
				break;
			}
		} else {
			mod_wdr_cleanup_algo(mod);
			ret = isp_mod_del_notify(mod, ISP_NOTIFY_STATIS_DONE);
			if (ret) {
				break;
			}
		}
		if (ctrl->val == ISP_WDR_INNER_DISABLE) {
			wdr_enable = 0;
			ret = wdr->hw_ops->set_enable(mod->owner_id, 0);
			if (ret) {
				break;
			}
		} else {
			wdr_enable = 1;
			ret = wdr->hw_ops->set_enable(mod->owner_id, 1);
			if (ret) {
				break;
			}
			ret = wdr_set_level(wdr, wdr->wdr_level_ctrl->cur.val);
			if (ret) {
				break;
			}
		}
		ret = isp_mod_sibling_execute(mod, MOD_GLOBAL,
									  GLOBAL_EXEC_SET_WDR_ENABLE,
									  &wdr_enable, sizeof(wdr_enable));
		break;
	case ISP_V4L2_CID_WDR_LEVEL:
		ret = wdr_set_level(wdr, ctrl->val);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	};
	if (ret) {
		isp_perror(ret, "set wdr ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops wdr_ctrl_ops = {
	.s_ctrl = wdr_s_ctrl,
};

static int isp_mod_wdr_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_wdr *wdr;

	wdr = get_mod(mod, wdr);
	wdr->hw_ops = isp_hw_wdr_get_ops();
	if (!wdr->hw_ops) {
		isp_error("get wdr hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	wdr->mod_ae = isp_mod_get_sibling(mod, MOD_AE);
	if (!wdr->mod_ae) {
		isp_error("get wdr mod ae fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	wdr->algo_param = isp_algo_param_get_wdr_v1(mod->owner_id);
	if (!wdr->algo_param) {
		isp_error("get wdr algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_wdr_cleanup(struct isp_mod *mod)
{
	mod_wdr_cleanup_algo(mod);

	return RTS_ISP_OK;
}

static int isp_mod_wdr_hardware_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);

	ret = wdr->hw_ops->init_vreg(mod->owner_id);
	if (ret) {
		return ret;
	}
	ret = wdr->hw_ops->set_config(mod->owner_id);
	return ret;
}

static int isp_mod_wdr_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_wdr *wdr = get_mod(mod, wdr);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config wdr_mode_config = {
		.ops = &wdr_ctrl_ops,
		.id = ISP_V4L2_CID_WDR_MODE,
		.name = "WDR Mode",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = ISP_WDR_INNER_DISABLE,
		.max = ISP_WDR_INNER_AUTO,
		.step = 1,
		.def = ISP_WDR_INNER_DISABLE,
	};
	const struct v4l2_ctrl_config wdr_level_config = {
		.ops = &wdr_ctrl_ops,
		.id = ISP_V4L2_CID_WDR_LEVEL,
		.name = "WDR Level",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = 0,
		.max = 100,
		.step = 1,
		.def = 50,
	};

	wdr->wdr_mode_ctrl =
		v4l2_ctrl_new_custom(handler, &wdr_mode_config, mod);
	wdr->wdr_level_ctrl =
		v4l2_ctrl_new_custom(handler, &wdr_level_config, mod);
	wdr->wdr_mode_ctrl->flags |= ISP_V4L2_CTRL_FLAG_READ_ONLY;
	if (handler->error) {
		isp_perror(handler->error, "add wdr ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_wdr wdr;

int isp_register_mod_wdr(void)
{
	isp_memset(&wdr, 0, sizeof(wdr));
	wdr.mod.id = MOD_WDR;
	wdr.mod.name = "wdr";
	wdr.mod.init = isp_mod_wdr_init;
	wdr.mod.cleanup = isp_mod_wdr_cleanup;
	wdr.mod.hardware_init = isp_mod_wdr_hardware_init;
	wdr.mod.add_ctrl = isp_mod_wdr_add_ctrl;
	wdr.mod.notify_actions = wdr_notify_actions;
	wdr.mod.notify_actions_num = ARRAY_SIZE(wdr_notify_actions);
	wdr.mod.size = sizeof(wdr);
	return isp_mod_register(&wdr.mod);
}

void isp_unregister_mod_wdr(void)
{
	isp_mod_unregister(&wdr.mod);
}
