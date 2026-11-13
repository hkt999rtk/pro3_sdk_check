/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 * Copyright (C) 2019 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_top.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_v4l2_ctrl.h>
#include <isp_hw_ygamma.h>
#include "isp_mod_ygamma.h"

#define GAMMA_CTRL_MIN 100
#define GAMMA_CTRL_DEF 300
#define GAMMA_CTRL_MAX 500

#define ISP_DEBUG_TYPE ISP_DEBUG_GAMMA

struct isp_mod_ygamma {
	struct isp_mod mod;
	const struct isp_hw_ygamma_ops *hw_ops;

	const isp_iq_ygamma_v1_t *iq;
	isp_algo_param_ygamma_v1_t *algo_param;
	isp_iq_ygamma_v1_dyn_item_t dyn_items[3];

	isp_iq_ygamma_v1_dyn_item_t ygamma_set;

	struct v4l2_ctrl *gamma_ctrl;
	struct {
		struct v4l2_ctrl *auto_ygamma_ctrl;
		struct v4l2_ctrl *ygamma_curve_ctrl;
	};

	int patch_mode;
};

static void intp_ygamma_v1_item(isp_iq_ygamma_v1_dyn_item_t *item, int val,
								int th0,
								const isp_iq_ygamma_v1_dyn_item_t *item0,
								int th1,
								const isp_iq_ygamma_v1_dyn_item_t *item1)
{
	item->curve_count = item0->curve_count;
	linear_intp_array(item->curve, val, th0, item0->curve, th1,
					  item1->curve, item->curve_count);
}

DEFINE_ISP_IQ_INTP_FUNC(ygamma, v1, intp_ygamma_v1_item)

static int ygamma_set_item(struct isp_mod_ygamma *ygamma)
{
	if (!ygamma->auto_ygamma_ctrl->val) {
		return RTS_ISP_OK;
	}

	return ygamma->hw_ops->set_ygamma(ygamma->mod.owner_id,
									  &ygamma->ygamma_set);
}

static int ygamma_set_gamma(struct isp_mod *mod, int value)
{
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);
	int def = ygamma->gamma_ctrl->default_value;
	int max = ygamma->gamma_ctrl->maximum;
	int min = ygamma->gamma_ctrl->minimum;

	/* clang-format off */
	static const isp_iq_ygamma_v1_dyn_item_t ygamma_max = {
		.curve_count = 16,
		.curve = {
			0, 8, 16, 45, 61, 73, 84, 93, 102, 118, 133, 146, 159,
			82, 204, 224,
		},
	};
	static const isp_iq_ygamma_v1_dyn_item_t ygamma_min = {
		.curve_count = 16,
		.curve = {
			0, 8, 16, 17, 19, 22, 26, 31, 36, 47, 61, 76, 93, 131,
			175, 224,
		}
	};
	/* clang-format on */

	if (value >= def)
		intp_ygamma_v1_item(&ygamma->ygamma_set, value, def,
							&ygamma->dyn_items[0], max, &ygamma_max);
	else
		intp_ygamma_v1_item(&ygamma->ygamma_set, value, def,
							&ygamma->dyn_items[0], min, &ygamma_min);

	return ygamma_set_item(ygamma);
}

static int ygamma_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int i;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTO_YGAMMA:
		for (i = 0; i < ygamma->ygamma_curve_ctrl->dims[0]; i++)
			ygamma->ygamma_curve_ctrl->p_new.p_u8[i] =
				ygamma->ygamma_set.curve[i];
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	return RTS_ISP_OK;
}

static int ygamma_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);

	switch (ctrl->id) {
	case ISP_V4L2_CID_GAMMA:
		ret = ygamma_set_gamma(mod, ctrl->val);
		break;
	case ISP_V4L2_CID_AUTO_YGAMMA:
		if (ctrl->val) {
			if (ctrl->is_new) {
				ret = ygamma_set_gamma(mod, ctrl->val);
			}
		} else if (ygamma->ygamma_curve_ctrl->is_new) {
			int i;
			isp_iq_ygamma_v1_dyn_item_t item;

			item.curve_count = ygamma->ygamma_curve_ctrl->dims[0];
			for (i = 0; i < item.curve_count; i++)
				item.curve[i] =
					ygamma->ygamma_curve_ctrl->p_new.p_u8[i];
			ret = ygamma->hw_ops->set_ygamma(mod->owner_id, &item);
		}
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set ygamma ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops ygamma_ctrl_ops = {
	.g_volatile_ctrl = ygamma_g_volatile_ctrl,
	.s_ctrl = ygamma_s_ctrl,
};

static int mod_ygamma_exec_set_patch_mode(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	int enable = *(int32_t *)data;
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);

	ret = isp_mod_set_patch_notify(mod, enable, RTS_ISP_FALSE);
	if (!ret) {
		ygamma->patch_mode = enable;
	}

	return ret;
}

static int mod_ygamma_exec_set_v1_dyn(struct isp_mod *mod, void *data)
{
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);
	isp_iq_ygamma_v1_dyn_item_t *item = *(void **)data;

	if (!ygamma->patch_mode) {
		return -RTS_ISP_EINVAL;
	}
	ygamma->ygamma_set = *item;
	return ygamma_set_item(ygamma);
}

static struct isp_mod_action_info ygamma_exec_actions[] = {
	ISP_MOD_ACTION(YGAMMA_EXEC_SET_PATCH_MODE,
				   mod_ygamma_exec_set_patch_mode),
	ISP_MOD_ACTION(YGAMMA_EXEC_SET_V1_DYN, mod_ygamma_exec_set_v1_dyn),
};

static int mod_ygamma_iq_change(struct isp_mod *mod, void *data)
{
	uint32_t curve_count;
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);
	const isp_iq_ygamma_v1_dyn_item_t *item;

	ygamma->iq = isp_iq_get_ygamma_v1(mod->owner_id,
									  &ygamma->algo_param->common);
	if (!ygamma->iq) {
		return -RTS_ISP_EINVAL;
	}
	curve_count = ygamma->iq->dynamic.nest.dyns[0].items[0].curve_count;
	if (curve_count > YGAMMA_ARRAY_SIZE_MAX) {
		return -RTS_ISP_EINVAL;
	}
	ygamma->ygamma_set.curve_count = curve_count;

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_ygamma_v1_dyn_enabled(&ygamma->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_ygamma_v1_dyn_item_default(
			   &ygamma->iq->dynamic.nest, &ygamma->algo_param->common);
	ygamma->dyn_items[0] = ygamma->ygamma_set = *item;

	return ygamma_set_gamma(mod, ygamma->gamma_ctrl->cur.val);
}

static int mod_ygamma_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);
	struct isp_notify_dynamic *dyn = data;
	const isp_iq_ygamma_v1_dyn_item_t *item;

	if (!isp_iq_ygamma_v1_dyn_enabled(&ygamma->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_ygamma_v1_dyn_item(&ygamma->iq->dynamic.nest,
										 &ygamma->algo_param->common, dyn,
										 isp_iq_ygamma_v1_intp_cb, ygamma);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (item != &ygamma->dyn_items[0]) {
		ygamma->dyn_items[0] = *item;
	}

	return ygamma_set_gamma(mod, ygamma->gamma_ctrl->cur.val);
}

static struct isp_mod_action_info ygamma_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_ygamma_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_ygamma_dynamic),
};

static int isp_mod_ygamma_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_ygamma *ygamma;

	ygamma = get_mod(mod, ygamma);
	ygamma->hw_ops = isp_hw_ygamma_get_ops();
	if (ygamma->hw_ops == NULL) {
		isp_error("get ygamma hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	ygamma->algo_param = isp_algo_param_get_ygamma_v1(mod->owner_id);
	if (!ygamma->algo_param) {
		isp_error("get ygamma algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_ygamma_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_ygamma_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_ygamma *ygamma = get_mod(mod, ygamma);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config auto_ygamma_config = {
		.ops = &ygamma_ctrl_ops,
		.id = ISP_V4L2_CID_AUTO_YGAMMA,
		.name = "Auto Ygamma",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 1,
	};
	const struct v4l2_ctrl_config ygamma_curve_config = {
		.ops = &ygamma_ctrl_ops,
		.id = ISP_V4L2_CID_YGAMMA_CURVE,
		.name = "Ygamma Curve",
		.type = ISP_V4L2_CTRL_TYPE_U8,
		.min = 0,
		.max = 255,
		.step = 1,
		.def = 0,
		.dims = { YGAMMA_ARRAY_SIZE_MAX },
	};

	ygamma->gamma_ctrl = v4l2_ctrl_new_std(handler, &ygamma_ctrl_ops,
										   ISP_V4L2_CID_GAMMA,
										   GAMMA_CTRL_MIN, GAMMA_CTRL_MAX,
										   1, GAMMA_CTRL_DEF, mod);
	ygamma->auto_ygamma_ctrl =
		v4l2_ctrl_new_custom(handler, &auto_ygamma_config, mod);
	ygamma->ygamma_curve_ctrl =
		v4l2_ctrl_new_custom(handler, &ygamma_curve_config, mod);
	v4l2_ctrl_auto_cluster(2, &ygamma->auto_ygamma_ctrl, 0, RTS_ISP_TRUE);
	if (handler->error) {
		isp_perror(handler->error, "add ygamma ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_ygamma ygamma;

int isp_register_mod_ygamma(void)
{
	isp_memset(&ygamma, 0, sizeof(ygamma));
	ygamma.mod.id = MOD_YGAMMA;
	ygamma.mod.name = "ygamma";
	ygamma.mod.init = isp_mod_ygamma_init;
	ygamma.mod.cleanup = isp_mod_ygamma_cleanup;
	ygamma.mod.add_ctrl = isp_mod_ygamma_add_ctrl;
	ygamma.mod.exec_actions = ygamma_exec_actions;
	ygamma.mod.exec_actions_num = ARRAY_SIZE(ygamma_exec_actions);
	ygamma.mod.notify_actions = ygamma_notify_actions;
	ygamma.mod.notify_actions_num = ARRAY_SIZE(ygamma_notify_actions);
	ygamma.mod.size = sizeof(ygamma);
	return isp_mod_register(&ygamma.mod);
}

void isp_unregister_mod_ygamma(void)
{
	isp_mod_unregister(&ygamma.mod);
}
