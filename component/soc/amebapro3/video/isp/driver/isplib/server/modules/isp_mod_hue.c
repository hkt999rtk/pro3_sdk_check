/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_mod.h>
#include <isp_v4l2_ctrl.h>
#include <isp_hw_hue.h>
#include "isp_mod_hue.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_HUE

struct isp_mod_hue {
	struct isp_mod mod;
	const struct isp_hw_hue_ops *hw_ops;
	struct v4l2_ctrl *hue_ctrl;
};

static int hue_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_hue *hue = get_mod(mod, hue);

	switch (ctrl->id) {
	case ISP_V4L2_CID_HUE:
		ret = hue->hw_ops->set_hue(hue->mod.owner_id, ctrl->val);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set hue ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops hue_ctrl_ops = {
	.s_ctrl = hue_s_ctrl,
};

static int isp_mod_hue_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_hue *hue;

	hue = get_mod(mod, hue);
	hue->hw_ops = isp_hw_hue_get_ops();
	if (hue->hw_ops == NULL) {
		isp_error("get hue hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_hue_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_hue_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_hue *hue = get_mod(mod, hue);
	struct v4l2_ctrl_handler *handler = phandler;

	hue->hue_ctrl = v4l2_ctrl_new_std(handler, &hue_ctrl_ops,
									  ISP_V4L2_CID_HUE, -50, 50, 1, 0, mod);
	if (handler->error) {
		isp_perror(handler->error, "add hue ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_hue hue;

int isp_register_mod_hue(void)
{
	isp_memset(&hue, 0, sizeof(hue));
	hue.mod.id = MOD_HUE;
	hue.mod.name = "hue";
	hue.mod.init = isp_mod_hue_init;
	hue.mod.cleanup = isp_mod_hue_cleanup;
	hue.mod.add_ctrl = isp_mod_hue_add_ctrl;

	hue.mod.size = sizeof(hue);
	return isp_mod_register(&hue.mod);
}

void isp_unregister_mod_hue(void)
{
	isp_mod_unregister(&hue.mod);
}
