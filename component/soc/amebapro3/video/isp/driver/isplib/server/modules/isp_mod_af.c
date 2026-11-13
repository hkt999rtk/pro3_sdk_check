/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_af.h>
#include <isp_core.h>
#include <isp_statis.h>
#include <isp_notify.h>
#include <isp_v4l2_ctrl.h>
#include <isp_iq.h>
#include <isp_mod.h>
#include <isp_algo.h>
#include <isp_hw_af.h>
#include "isp_mod_sensor.h"
#include "isp_mod_ae.h"
#include "isp_mod_af.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_AF
#define af_algo_ctrl(af, ...) \
	af->af_algo->ctrl(af->mod.owner_id, ##__VA_ARGS__)

struct isp_mod_af {
	struct isp_mod mod;
	const struct rts_isp_af_algo *af_algo;
	const struct isp_hw_af_ops *hw_ops;
	struct isp_af_hw_info hw_info;
	int32_t ae_status; /* enum rts_isp_ae_algo_status */
	uint32_t ae_gain;

	const isp_iq_af_v1_t *iq;
	isp_algo_param_af_v1_t *algo_param;
	uint32_t current_position;
	struct {
		struct v4l2_ctrl *auto_focus_ctrl;
		struct v4l2_ctrl *focus_ctrl;
	};
};

static int mod_af_info_statis(struct isp_mod *mod, void *data)
{
	struct isp_mod_af *af = get_mod(mod, af);
	struct isp_mod_af_stat_info *info = data;

	info->info.win = af->hw_info.grid_info.size;
	info->illum_cnt_size = af->hw_info.illum_cnt_size;
	info->filter_size = af->hw_info.filter_size;

	return RTS_ISP_OK;
}

static int mod_af_info_version(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_af *af = get_mod(mod, af);
	const struct rts_isp_af_algo *algo;
	uint16_t *version = data;

	if (af->af_algo) {
		*version = af->af_algo->api_version & AF_VERSION_MASK;
		return RTS_ISP_OK;
	}
	ret = isp_algo_get_af(mod->owner_id, &algo);
	if (ret) {
		return ret;
	}
	*version = algo->api_version & AF_VERSION_MASK;

	return RTS_ISP_OK;
}

static struct isp_mod_action_info af_info_actions[] = {
	ISP_MOD_ACTION(AF_INFO_STATIS, mod_af_info_statis),
	ISP_MOD_ACTION(AF_INFO_VERSION, mod_af_info_version),
};

static int set_ae_status(struct isp_mod *mod)
{
	int ret;
	int32_t ae_status; /* enum rts_isp_ae_algo_status */
	struct isp_mod_af *af = get_mod(mod, af);

	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_STATUS, &ae_status,
								   sizeof(ae_status));
	if (ret) {
		goto out;
	}

	if (af->ae_status != ae_status) {
		af->ae_status = ae_status;
		if (af->af_algo)
			ret = af_algo_ctrl(af, SET_AE_ALGO_STATUS,
							   af->ae_status);
		if (ret) {
			isp_perror(ret, "set ae status fail");
		}
		return ret;
	}
out:
	if (ret) {
		isp_perror(ret, "af get ae status fail");
	}
	return ret;
}

static int mod_af_statis_done(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_af *af = get_mod(mod, af);
	const struct rts_isp_af_statis *statis;
	struct rts_isp_af_result result;

	if (!af->af_algo) {
		return RTS_ISP_OK;
	}

	set_ae_status(mod);
	result.config.need_update = RTS_ISP_FALSE;
	result.result.need_update = RTS_ISP_FALSE;

	statis = &((const struct rts_isp_statis *)data)->af;

	ret = af->af_algo->run(mod->owner_id, statis, &result);
	if (ret) {
		isp_perror(ret, "af algo run fail");
		return ret;
	}
	if (result.result.need_update) {
		ret = isp_mod_sibling_execute(mod, MOD_SENSOR,
									  SENSOR_EXEC_SET_FOCUS,
									  &result.result.position,
									  sizeof(result.result.position));
		if (ret) {
			isp_perror(ret, "set af result fail");
			return ret;
		}
		af->current_position = result.result.position;
	}
	return RTS_ISP_OK;
}

static int mod_af_cleanup_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_af *af = get_mod(mod, af);

	if (af->af_algo) {
		af->af_algo->cleanup(mod->owner_id);
		af->af_algo = NULL;
	}
	ret = isp_mod_del_notify(mod, ISP_NOTIFY_STATIS_DONE);
	if (ret) {
		return ret;
	}
	ret = isp_mod_del_notify(mod, ISP_NOTIFY_DYNAMIC);

	return ret;
}

static int check_af_algo_version(uint32_t version)
{
	if (algo_magic(version) != AF_API_VERSION_MAGIC ||
		algo_major_version(version) != AF_API_MAJOR_VERSION ||
		algo_minor_version(version) > AF_API_MINOR_VERSION) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_af(const struct rts_isp_af_algo *af_algo)
{
	if (!af_algo || check_af_algo_version(af_algo->api_version)) {
		return -RTS_ISP_EINVAL;
	}
	if (!af_algo->init || !af_algo->cleanup) {
		return -RTS_ISP_EINVAL;
	}
	if (!af_algo->run || !af_algo->ctrl) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int af_modify_focus_ctrl_range(struct isp_mod_af *af)
{
	int ret;
	uint32_t def;
	struct rts_isp_focus_info value;

	if (!af->iq) {
		return RTS_ISP_OK;
	}

	ret = isp_mod_sibling_get_info(&af->mod, MOD_SENSOR, SENSOR_INFO_VCM,
								   &value, sizeof(value));
	if (ret) {
		return ret;
	}
	def = clamp(af->iq->def_position, value.min, value.max);
	v4l2_ctrl_modify_range(af->focus_ctrl, value.min, value.max, 1, def);

	/* first iq load, update ctrl to match iq table */
	if (af->auto_focus_ctrl->flags & ISP_V4L2_CTRL_FLAG_READ_ONLY) {
		af->auto_focus_ctrl->flags &= ~ISP_V4L2_CTRL_FLAG_READ_ONLY;
		/* set auto at first, because auto may change manual ctrl*/
		v4l2_ctrl_s_ctrl(af->auto_focus_ctrl, af->iq->enable);
		af->focus_ctrl->val = af->focus_ctrl->cur.val = def;
	}

	return RTS_ISP_OK;
}

static int af_sensor_update(struct isp_mod_af *af)
{
	return af_modify_focus_ctrl_range(af);
}

static int mod_af_update_af_info(struct isp_mod *mod,
								 struct rts_isp_af_info *info,
								 struct rts_isp_af_iq_info *iq,
								 struct rts_isp_af_param_info *param)
{
	const isp_iq_header_t *iq_header;
	const isp_algo_param_header_t *param_header;
	struct isp_mod_af *af = get_mod(mod, af);

	isp_memset(info, 0, sizeof(*info));
	isp_memset(iq, 0, sizeof(*iq));
	isp_memset(param, 0, sizeof(*param));
	info->stat_info.win = af->hw_info.grid_info.size;

	if ((af->af_algo->api_version & AF_VERSION_MASK) >= 0x00000101) {
		iq_header = isp_iq_get_header(af->mod.owner_id);
		param_header = isp_iq_get_algo_param_header(af->mod.owner_id);
		if (!iq_header || !param_header) {
			return -RTS_ISP_EINVAL;
		}

		iq->magic = iq_header->magic;
		iq->version = iq_header->version;
		iq->iq = af->iq;
		info->iq = iq;
		param->magic = param_header->magic;
		param->version = param_header->version;
		param->algo_param = af->algo_param;
		info->algo_param = param;
	} else {
		info->iq = af->iq;
		info->algo_param = af->algo_param;
	}

	return RTS_ISP_OK;
}

static int mod_af_init_algo(struct isp_mod *mod)
{
	int ret;
	float ae_gain_float;
	struct rts_isp_af_info info;
	struct rts_isp_af_iq_info iq_info;
	struct rts_isp_af_param_info param_info;
	struct isp_mod_af *af = get_mod(mod, af);

	ret = mod_af_update_af_info(mod, &info, &iq_info, &param_info);
	if (ret) {
		return ret;
	}
	ret = af->af_algo->init(mod->owner_id, &info);
	if (ret) {
		return ret;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_GAIN,
								   &ae_gain_float, sizeof(ae_gain_float));
	if (ret) {
		return ret;
	}
	af->ae_gain = ae_gain_float * 16;
	ret = af_algo_ctrl(af, SET_AE_GAIN, af->ae_gain);
	if (ret) {
		return ret;
	}
	set_ae_status(mod);
	ret = af_algo_ctrl(af, SET_AE_ALGO_STATUS, af->ae_status);
	return ret;
}

static int mod_af_update_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_af *af = get_mod(mod, af);

	mod_af_cleanup_algo(mod);
	ret = isp_algo_get_af(mod->owner_id, &af->af_algo);
	if (ret) {
		goto out;
	}
	ret = check_af(af->af_algo);
	if (ret) {
		goto out;
	}
	ret = mod_af_init_algo(mod);
	if (ret) {
		goto out;
	}
	ret = isp_mod_add_notify(mod, ISP_NOTIFY_STATIS_DONE);
	if (ret) {
		goto out;
	}
	ret = isp_mod_add_notify(mod, ISP_NOTIFY_DYNAMIC);
out:
	if (ret) {
		isp_perror(ret, "af update algo fail");
	}
	return ret;
}

static int mod_af_plugin_update(struct isp_mod *mod, void *data)
{
	uint32_t notify_data;
	struct isp_mod_af *af = get_mod(mod, af);

	notify_data = *(uint32_t *)data;
	if (notify_data != RTS_ISP_PLUG_TYPE_ALGO_AF &&
		notify_data != RTS_ISP_PLUG_TYPE_SENSOR) {
		return RTS_ISP_OK;
	}
	if (notify_data == RTS_ISP_PLUG_TYPE_SENSOR) {
		return af_sensor_update(af);
	}
	return mod_af_update_algo(mod);
}

static int mod_af_iq_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_af *af = get_mod(mod, af);

	af->iq = isp_iq_get_af_v1(mod->owner_id, &af->algo_param->common);
	if (!af->iq) {
		isp_error("get af iq fail\n");
		return -RTS_ISP_ENOITEM;
	}
	ret = af_modify_focus_ctrl_range(af);
	if (ret) {
		return ret;
	}
	if (!af->af_algo) {
		return RTS_ISP_OK;
	}
	return mod_af_update_algo(mod);
}

static int mod_af_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_af *af = get_mod(mod, af);
	const struct isp_notify_dynamic *dyn = data;

	af->ae_gain = dyn->ae.iq_total_gain[0];
	if (af->af_algo) {
		return af_algo_ctrl(af, SET_AE_GAIN, af->ae_gain);
	}
	return RTS_ISP_OK;
}

static int mod_af_mcrop_change(struct isp_mod *mod, void *data)
{
	int ret;
	rts_isp_grid_t grid;
	struct isp_mod_af *af = get_mod(mod, af);
	const rts_isp_rect_t *rect = data;

	ret = rts_isp_get_grid_from_rect(&grid, rect, &af->hw_info.grid_info);
	if (ret) {
		return ret;
	}
	return af->hw_ops->set_statis(mod->owner_id, &grid);
}

static struct isp_mod_action_info af_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_af_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_PLUGIN_UPDATE, mod_af_plugin_update),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_af_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_af_dynamic),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_af_mcrop_change),
};

static int af_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_af *af = get_mod(mod, af);

	switch (ctrl->id) {
	case ISP_V4L2_CID_FOCUS_AUTO:
		af->focus_ctrl->val = af->current_position;
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	return RTS_ISP_OK;
}

static int af_set_auto_focus_mode(struct isp_mod *mod, int auto_mode)
{
	if (auto_mode) {
		return mod_af_update_algo(mod);
	} else {
		return mod_af_cleanup_algo(mod);
	}
}

static int af_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_af *af = get_mod(mod, af);

	switch (ctrl->id) {
	case ISP_V4L2_CID_FOCUS_AUTO:
		if (af->auto_focus_ctrl->is_new) {
			ret = af_set_auto_focus_mode(mod,
										 af->auto_focus_ctrl->val);
			if (ret) {
				break;
			}
		}
		if (af->focus_ctrl->is_new) {
			ret = isp_mod_sibling_execute(
					  mod, MOD_SENSOR, SENSOR_EXEC_SET_FOCUS,
					  &af->focus_ctrl->val,
					  sizeof(af->focus_ctrl->val));
		}
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set af ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops af_ctrl_ops = {
	.g_volatile_ctrl = af_g_volatile_ctrl,
	.s_ctrl = af_s_ctrl,
};

static int isp_mod_af_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_af *af;

	af = get_mod(mod, af);
	af->hw_ops = isp_hw_af_get_ops();
	if (af->hw_ops == NULL) {
		isp_error("get af hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	af->algo_param = isp_algo_param_get_af_v1(mod->owner_id);
	if (!af->algo_param) {
		isp_error("get af algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	ret = af->hw_ops->get_hw_info(mod->owner_id, &af->hw_info);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_af_cleanup(struct isp_mod *mod)
{
	struct isp_mod_af *af = get_mod(mod, af);

	if (af->af_algo) {
		af->af_algo->cleanup(mod->owner_id);
	}
	return RTS_ISP_OK;
}

static int isp_mod_af_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_af *af;

	af = get_mod(mod, af);

	return af->hw_ops->set_statis_config(mod->owner_id);
}

static int isp_mod_af_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_af *af = get_mod(mod, af);
	struct v4l2_ctrl_handler *handler = phandler;

	af->auto_focus_ctrl = v4l2_ctrl_new_std(handler, &af_ctrl_ops,
											ISP_V4L2_CID_FOCUS_AUTO, 0, 1,
											1, 1, mod);
	af->focus_ctrl = v4l2_ctrl_new_std(handler, &af_ctrl_ops,
									   ISP_V4L2_CID_FOCUS_ABSOLUTE, 0, 0, 1,
									   0, mod);
	v4l2_ctrl_auto_cluster(2, &af->auto_focus_ctrl, 0, RTS_ISP_TRUE);
	af->auto_focus_ctrl->flags |= ISP_V4L2_CTRL_FLAG_READ_ONLY;
	if (handler->error) {
		isp_perror(handler->error, "add af ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_af af;

int isp_register_mod_af(void)
{
	isp_memset(&af, 0, sizeof(af));
	af.mod.id = MOD_AF;
	af.mod.name = "af";
	af.mod.init = isp_mod_af_init;
	af.mod.cleanup = isp_mod_af_cleanup;
	af.mod.hardware_init = isp_mod_af_hardware_init;
	af.mod.add_ctrl = isp_mod_af_add_ctrl;
	af.mod.info_actions = af_info_actions;
	af.mod.info_actions_num = ARRAY_SIZE(af_info_actions);
	af.mod.notify_actions = af_notify_actions;
	af.mod.notify_actions_num = ARRAY_SIZE(af_notify_actions);
	af.mod.size = sizeof(af);
	return isp_mod_register(&af.mod);
}

void isp_unregister_mod_af(void)
{
	isp_mod_unregister(&af.mod);
}
