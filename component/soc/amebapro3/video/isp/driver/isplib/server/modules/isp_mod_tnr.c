/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_notify.h>
#include <isp_v4l2_ctrl.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_tnr.h>
#include "isp_mod_sensor.h"
#include "isp_mod_iq.h"
#include "isp_mod_tnr.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_TNR

struct isp_mod_tnr {
	struct isp_mod mod;
	const struct isp_hw_tnr_ops *hw_ops;
	struct v4l2_ctrl *tnr_ctrl;
};

static int mod_tnr_start_pre(struct isp_mod *mod, void *data)
{
	struct isp_mod_tnr *tnr = get_mod(mod, tnr);

	return tnr->hw_ops->enable_axi(mod->owner_id, RTS_ISP_TRUE);
}

static int mod_tnr_stop_pre(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_tnr *tnr = get_mod(mod, tnr);

	ret = tnr->hw_ops->enable(mod->owner_id, RTS_ISP_FALSE);
	if (ret) {
		return ret;
	}
	return tnr->hw_ops->enable_axi(mod->owner_id, RTS_ISP_FALSE);
}

static int mod_tnr_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	const struct rts_isp_interface_info *info;
	struct isp_mod_tnr *tnr = get_mod(mod, tnr);

	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_INTERFACE,
								   &info, sizeof(info));
	if (ret) {
		return ret;
	}
	return tnr->hw_ops->set_snr_depth(mod->owner_id, info->bit_depth);
}

static int mod_tnr_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_tnr *tnr = get_mod(mod, tnr);

	return tnr->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info tnr_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_START_PRE, mod_tnr_start_pre),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STOP_PRE, mod_tnr_stop_pre),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_tnr_mode_change),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_tnr_dynamic_post),
};

static int tnr_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_tnr *tnr = get_mod(mod, tnr);

	if (ctrl->id != ISP_V4L2_CID_TEMPORAL_DENOISE) {
		return -RTS_ISP_ECTRL;
	}
	return tnr->hw_ops->enable(mod->owner_id, ctrl->val);
}

static const struct v4l2_ctrl_ops tnr_ctrl_ops = {
	.s_ctrl = tnr_s_ctrl,
};

static int isp_mod_tnr_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_tnr *tnr;

	tnr = get_mod(mod, tnr);
	tnr->hw_ops = isp_hw_tnr_get_ops();
	if (tnr->hw_ops == NULL) {
		isp_error("get tnr hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_tnr_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_tnr_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_tnr *tnr = get_mod(mod, tnr);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config tnr_config = {
		.ops = &tnr_ctrl_ops,
		.id = ISP_V4L2_CID_TEMPORAL_DENOISE,
		.name = "Temporal denoise",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 1,
	};

	tnr->tnr_ctrl = v4l2_ctrl_new_custom(handler, &tnr_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add tnr ctrls fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_tnr tnr;

int isp_register_mod_tnr(void)
{
	isp_memset(&tnr, 0, sizeof(tnr));
	tnr.mod.id = MOD_TNR;
	tnr.mod.name = "tnr";
	tnr.mod.init = isp_mod_tnr_init;
	tnr.mod.cleanup = isp_mod_tnr_cleanup;
	tnr.mod.notify_actions = tnr_notify_actions;
	tnr.mod.notify_actions_num = ARRAY_SIZE(tnr_notify_actions);
	tnr.mod.add_ctrl = isp_mod_tnr_add_ctrl;
	tnr.mod.size = sizeof(tnr);
	return isp_mod_register(&tnr.mod);
}

void isp_unregister_mod_tnr(void)
{
	isp_mod_unregister(&tnr.mod);
}
