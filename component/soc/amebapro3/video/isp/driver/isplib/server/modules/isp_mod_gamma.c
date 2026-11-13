/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_top.h>
#include <isp_v4l2_ctrl.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_hw_gamma.h>
#include "isp_mod_gamma.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_GAMMA

struct isp_mod_gamma {
	struct isp_mod mod;
	const struct isp_hw_gamma_ops *hw_ops;

	const isp_iq_gamma_v1_t *iq;
	isp_algo_param_gamma_v1_t *algo_param;
	isp_iq_gamma_v1_dyn_item_t dyn_items[3];

	isp_iq_gamma_v1_dyn_item_t current;
	int force_update;
	int patch_mode;

	struct {
		struct v4l2_ctrl *auto_gamma_ctrl;
		struct v4l2_ctrl *gamma_curve_ctrl;
	};
};

static int gamma_set_item(struct isp_mod_gamma *rgbgamma)
{
	if (!rgbgamma->auto_gamma_ctrl->val) {
		return RTS_ISP_OK;
	}

	return rgbgamma->hw_ops->set_gamma(rgbgamma->mod.owner_id,
									   &rgbgamma->current);
}

static int gamma_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int i;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTO_GAMMA:
		for (i = 0; i < rgbgamma->gamma_curve_ctrl->elems; i++)
			rgbgamma->gamma_curve_ctrl->p_new.p_u16[i] =
				rgbgamma->current.curve[i];
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	return RTS_ISP_OK;
}

static int gamma_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int i;
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTO_GAMMA:
		if (ctrl->val) {
			if (ctrl->is_new) {
				ret = gamma_set_item(rgbgamma);
			}
		} else if (rgbgamma->gamma_curve_ctrl->is_new) {
			isp_iq_gamma_v1_dyn_item_t manual_iq;

			manual_iq.curve_count =
				rgbgamma->gamma_curve_ctrl->elems;
			for (i = 0; i < manual_iq.curve_count; i++)
				manual_iq.curve[i] = rgbgamma->gamma_curve_ctrl
									 ->p_new.p_u16[i];
			manual_iq.offsets[0] = rgbgamma->current.offsets[0];
			manual_iq.offsets[1] = rgbgamma->current.offsets[1];
			ret = rgbgamma->hw_ops->set_gamma(mod->owner_id,
											  &manual_iq);
		}
		break;
	default:
		break;
	}
	return ret;
}

static const struct v4l2_ctrl_ops gamma_ctrl_ops = {
	.g_volatile_ctrl = gamma_g_volatile_ctrl,
	.s_ctrl = gamma_s_ctrl,
};

static void intp_gamma_v1_item(isp_iq_gamma_v1_dyn_item_t *item, int val,
							   int th0, const isp_iq_gamma_v1_dyn_item_t *item0,
							   int th1, const isp_iq_gamma_v1_dyn_item_t *item1)
{
	item->curve_count = item0->curve_count;
	linear_intp_array(item->curve, val, th0, item0->curve, th1,
					  item1->curve, item->curve_count);
	linear_intp_array(item->offsets, val, th0, item0->offsets, th1,
					  item1->offsets, ARRAY_SIZE(item->offsets));
}

DEFINE_ISP_IQ_INTP_FUNC(gamma, v1, intp_gamma_v1_item)

static int mod_gamma_exec_set_patch_mode(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	int enable = *(int32_t *)data;
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);

	ret = isp_mod_set_patch_notify(mod, enable, RTS_ISP_TRUE);
	if (!ret) {
		rgbgamma->patch_mode = enable;
	}

	return ret;
}

static int mod_gamma_exec_set_v1_dyn(struct isp_mod *mod, void *data)
{
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);
	isp_iq_gamma_v1_dyn_item_t *item = *(void **)data;

	if (!rgbgamma->patch_mode) {
		return -RTS_ISP_EINVAL;
	}
	rgbgamma->current = *item;
	return gamma_set_item(rgbgamma);
}

static struct isp_mod_action_info gamma_exec_actions[] = {
	ISP_MOD_ACTION(GAMMA_EXEC_SET_PATCH_MODE,
				   mod_gamma_exec_set_patch_mode),
	ISP_MOD_ACTION(GAMMA_EXEC_SET_V1_DYN, mod_gamma_exec_set_v1_dyn),
};

static int mod_gamma_statis_done(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);

	if (!rgbgamma->force_update &&
		isp_memcmp_array(rgbgamma->current.curve,
						 rgbgamma->dyn_items[0].curve,
						 rgbgamma->current.curve_count) == 0 &&
		isp_memcmp_array(rgbgamma->current.offsets,
						 rgbgamma->dyn_items[0].offsets,
						 ARRAY_SIZE(rgbgamma->current.offsets)) == 0) {
		return RTS_ISP_OK;
	}
	rgbgamma->force_update = RTS_ISP_FALSE;
	calc_smooth_array(rgbgamma->current.curve, rgbgamma->dyn_items[0].curve,
					  8, 128, rgbgamma->current.curve_count);
	calc_smooth_array(rgbgamma->current.offsets,
					  rgbgamma->dyn_items[0].offsets, 8, 128,
					  ARRAY_SIZE(rgbgamma->current.offsets));
	ret = gamma_set_item(rgbgamma);
	if (ret) {
		return ret;
	}
	/* need to call estimation to set the sync reg */
	return rgbgamma->hw_ops->est_dynamic(mod->owner_id);
}

static int mod_gamma_iq_change(struct isp_mod *mod, void *data)
{
	uint32_t curve_count;
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);
	const isp_iq_gamma_v1_dyn_item_t *item;

	rgbgamma->iq = isp_iq_get_gamma_v1(mod->owner_id,
									   &rgbgamma->algo_param->common);
	if (!rgbgamma->iq) {
		return -RTS_ISP_ENOITEM;
	}
	curve_count = rgbgamma->iq->dynamic.nest.dyns[0].items[0].curve_count;
	if (curve_count > GAMMA_ARRAY_SIZE_MAX) {
		return -RTS_ISP_EINVAL;
	}
	rgbgamma->current.curve_count = curve_count;

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_gamma_v1_dyn_enabled(&rgbgamma->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_gamma_v1_dyn_item_default(
			   &rgbgamma->iq->dynamic.nest, &rgbgamma->algo_param->common);
	if (!item) {
		return -RTS_ISP_ENOITEM;
	}
	rgbgamma->dyn_items[0] = rgbgamma->current = *item;

	return gamma_set_item(rgbgamma);
}

static int mod_gamma_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);
	const struct isp_notify_dynamic *dyn = data;
	const isp_iq_gamma_v1_dyn_item_t *item;

	if (!isp_iq_gamma_v1_dyn_enabled(&rgbgamma->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	rgbgamma->force_update = RTS_ISP_TRUE;

	item = isp_iq_get_gamma_v1_dyn_item(&rgbgamma->iq->dynamic.nest,
										&rgbgamma->algo_param->common, dyn,
										isp_iq_gamma_v1_intp_cb, rgbgamma);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (item != &rgbgamma->dyn_items[0]) {
		rgbgamma->dyn_items[0] = *item;
	}

	return RTS_ISP_OK;
}

static int mod_gamma_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_gamma *gamma = get_mod(mod, gamma);

	return gamma->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info gamma_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_gamma_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_gamma_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_gamma_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_gamma_dynamic_post),
};

static int isp_mod_gamma_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_gamma *rgbgamma;

	rgbgamma = get_mod(mod, gamma);
	rgbgamma->hw_ops = isp_hw_gamma_get_ops();
	if (rgbgamma->hw_ops == NULL) {
		isp_error("get rgbgamma hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	rgbgamma->algo_param = isp_algo_param_get_gamma_v1(mod->owner_id);
	if (!rgbgamma->algo_param) {
		isp_error("get gamma algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_gamma_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_gamma_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_gamma *rgbgamma = get_mod(mod, gamma);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config auto_gamma_config = {
		.ops = &gamma_ctrl_ops,
		.id = ISP_V4L2_CID_AUTO_GAMMA,
		.name = "Auto RGB rgbgamma",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 1,
	};
	const struct v4l2_ctrl_config gamma_curve_config = {
		.ops = &gamma_ctrl_ops,
		.id = ISP_V4L2_CID_GAMMA_CURVE,
		.name = "RGB rgbgamma curve",
		.type = ISP_V4L2_CTRL_TYPE_U16,
		.min = 0,
		.max = 4095,
		.step = 1,
		.def = 0,
		.dims = { GAMMA_ARRAY_SIZE_MAX }
	};
	rgbgamma->auto_gamma_ctrl =
		v4l2_ctrl_new_custom(handler, &auto_gamma_config, mod);
	rgbgamma->gamma_curve_ctrl =
		v4l2_ctrl_new_custom(handler, &gamma_curve_config, mod);
	v4l2_ctrl_auto_cluster(2, &rgbgamma->auto_gamma_ctrl, 0, RTS_ISP_TRUE);
	if (handler->error) {
		isp_perror(handler->error, "add rgbgamma ctrls fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_gamma rgbgamma;

int isp_register_mod_gamma(void)
{
	isp_memset(&rgbgamma, 0, sizeof(rgbgamma));
	rgbgamma.mod.id = MOD_GAMMA;
	rgbgamma.mod.name = "rgbgamma";
	rgbgamma.mod.init = isp_mod_gamma_init;
	rgbgamma.mod.cleanup = isp_mod_gamma_cleanup;
	rgbgamma.mod.add_ctrl = isp_mod_gamma_add_ctrl;
	rgbgamma.mod.exec_actions = gamma_exec_actions;
	rgbgamma.mod.exec_actions_num = ARRAY_SIZE(gamma_exec_actions);
	rgbgamma.mod.notify_actions = gamma_notify_actions;
	rgbgamma.mod.notify_actions_num = ARRAY_SIZE(gamma_notify_actions);
	rgbgamma.mod.size = sizeof(rgbgamma);
	return isp_mod_register(&rgbgamma.mod);
}

void isp_unregister_mod_gamma(void)
{
	isp_mod_unregister(&rgbgamma.mod);
}
