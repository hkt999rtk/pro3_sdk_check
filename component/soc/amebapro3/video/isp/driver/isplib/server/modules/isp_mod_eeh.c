/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Kelsey Chen <kelsey_chen@realsil.com.cn>
 */

#include <isp_v4l2_ctrl.h>
#include <isp_mod.h>
#include <isp_top.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_hw_eeh.h>
#include "isp_mod_eeh.h"

struct isp_mod_eeh {
	struct isp_mod mod;
	const struct isp_hw_eeh_ops *hw_ops;

	const isp_iq_eeh_v1_t *iq;
	isp_algo_param_eeh_v1_t *algo_param;
	isp_iq_eeh_v1_dyn_item_t dyn_items[3];

	struct v4l2_ctrl *sharpness_ctrl;
};

static void intp_eeh_v1_item(isp_iq_eeh_v1_dyn_item_t *item, int val, int th0,
							 const isp_iq_eeh_v1_dyn_item_t *item0, int th1,
							 const isp_iq_eeh_v1_dyn_item_t *item1)
{
	int i;

	item->pts_count = item0->pts_count;
	for (i = 0; i < item->pts_count; i++) {
		item->pts[i].ratio = linear_intp(val, th0, item0->pts[i].ratio,
										 th1, item1->pts[i].ratio);
		item->pts[i].thd0 = linear_intp(val, th0, item0->pts[i].thd0,
										th1, item1->pts[i].thd0);
		item->pts[i].thd1 = linear_intp(val, th0, item0->pts[i].thd1,
										th1, item1->pts[i].thd1);
	}
}

DEFINE_ISP_IQ_INTP_FUNC(eeh, v1, intp_eeh_v1_item)

static int eeh_set_dyn_item(struct isp_mod_eeh *eeh)
{
	return eeh->hw_ops->set_dyn_item(eeh->mod.owner_id, &eeh->dyn_items[0]);
}

static int mod_eeh_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);
	const isp_iq_eeh_v1_dyn_item_t *item;

	eeh->iq = isp_iq_get_eeh_v1(mod->owner_id, &eeh->algo_param->common);
	if (!eeh->iq) {
		return -RTS_ISP_EINVAL;
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_eeh_v1_dyn_enabled(&eeh->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_eeh_v1_dyn_item_default(&eeh->iq->dynamic.nest,
			&eeh->algo_param->common);
	eeh->dyn_items[0] = *item;

	return eeh_set_dyn_item(eeh);
}

static int mod_eeh_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);
	struct isp_notify_dynamic *dyn = data;
	const isp_iq_eeh_v1_dyn_item_t *item;

	if (!isp_iq_eeh_v1_dyn_enabled(&eeh->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_eeh_v1_dyn_item(&eeh->iq->dynamic.nest,
									  &eeh->algo_param->common, dyn,
									  isp_iq_eeh_v1_intp_cb, eeh);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (item != &eeh->dyn_items[0]) {
		eeh->dyn_items[0] = *item;
	}

	return eeh_set_dyn_item(eeh);
}

static int mod_eeh_dynamic_post(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);

	ret = eeh->hw_ops->estimation(mod->owner_id);
	if (ret) {
		return ret;
	}

	return eeh->hw_ops->set_sharpness(mod->owner_id,
									  eeh->sharpness_ctrl->cur.val);
}

static struct isp_mod_action_info eeh_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_eeh_iq_change),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_IQ_CHANGE, ISP_DYNAMIC_PRIORITY,
						   mod_eeh_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_eeh_dynamic_post),
};

static int mod_eeh_info_position(struct isp_mod *mod, void *data)
{
	int reg_val;
	int32_t *position = data;
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);

	reg_val = eeh->hw_ops->get_position(mod->owner_id);
	if (reg_val) {
		*position = EEH_BEFORE_LCE;
	} else {
		*position = EEH_AFTER_LCE;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_action_info eeh_info_actions[] = {
	ISP_MOD_ACTION(EEH_INFO_POSITION, mod_eeh_info_position),
};

static int eeh_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);

	switch (ctrl->id) {
	case ISP_V4L2_CID_SHARPNESS:
		ret = eeh->hw_ops->set_sharpness(mod->owner_id, ctrl->val);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set eeh ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops eeh_ctrl_ops = {
	.s_ctrl = eeh_s_ctrl,
};

static int isp_mod_eeh_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_eeh *eeh;

	eeh = get_mod(mod, eeh);
	eeh->hw_ops = isp_hw_eeh_get_ops();
	if (eeh->hw_ops == NULL) {
		isp_error("get eeh hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	eeh->algo_param = isp_algo_param_get_eeh_v1(mod->owner_id);
	if (!eeh->algo_param) {
		isp_error("get eeh algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_eeh_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_eeh_hardware_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);

	ret = eeh->hw_ops->init_vreg(mod->owner_id);
	if (ret) {
		return ret;
	}
	return eeh->hw_ops->init(mod->owner_id);
}

static int isp_mod_eeh_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_eeh *eeh = get_mod(mod, eeh);
	struct v4l2_ctrl_handler *handler = phandler;

	eeh->sharpness_ctrl = v4l2_ctrl_new_std(handler, &eeh_ctrl_ops,
											ISP_V4L2_CID_SHARPNESS,
											SHARPNESS_CTRL_MIN,
											SHARPNESS_CTRL_MAX, 1,
											SHARPNESS_CTRL_DEF, mod);
	return RTS_ISP_OK;
}

static struct isp_mod_eeh eeh;

int isp_register_mod_eeh(void)
{
	isp_memset(&eeh, 0, sizeof(eeh));
	eeh.mod.id = MOD_EEH;
	eeh.mod.name = "eeh";
	eeh.mod.init = isp_mod_eeh_init;
	eeh.mod.cleanup = isp_mod_eeh_cleanup;
	eeh.mod.hardware_init = isp_mod_eeh_hardware_init;
	eeh.mod.add_ctrl = isp_mod_eeh_add_ctrl;
	eeh.mod.notify_actions = eeh_notify_actions;
	eeh.mod.notify_actions_num = ARRAY_SIZE(eeh_notify_actions);
	eeh.mod.info_actions = eeh_info_actions;
	eeh.mod.info_actions_num = ARRAY_SIZE(eeh_info_actions);
	eeh.mod.size = sizeof(eeh);
	return isp_mod_register(&eeh.mod);
}

void isp_unregister_mod_eeh(void)
{
	isp_mod_unregister(&eeh.mod);
}
