/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_os_driver.h>
#include <isp_v4l2_ctrl.h>
#include <isp_mod.h>
#include <isp_hw_rgb2yuv.h>
#include "isp_mod_rgb2yuv.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_RGB2YUV

struct isp_mod_rgb2yuv {
	struct isp_mod mod;
	const struct isp_hw_rgb2yuv_ops *hw_ops;

	struct v4l2_ctrl *color_range_ctrl;
};

static int mod_rgb2yuv_exec_convert(struct isp_mod *mod, void *data)
{
	int ret;
	uint32_t color = *(uint32_t *)data;
	struct isp_mod_rgb2yuv *rgb2yuv = get_mod(mod, rgb2yuv);

	ret = rgb2yuv->hw_ops->convert(mod->owner_id, &color);
	if (ret) {
		return ret;
	}
	*(uint32_t *)data = color;
	return RTS_ISP_OK;
}

static struct isp_mod_action_info rgb2yuv_exec_actions[] = {
	ISP_MOD_ACTION(RGB2YUV_EXEC_CONVERT, mod_rgb2yuv_exec_convert),
};
static int rgb2yuv_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_rgb2yuv *rgb2yuv = get_mod(mod, rgb2yuv);

	switch (ctrl->id) {
	case ISP_V4L2_CID_COLOR_RANGE:
		ret = rgb2yuv->hw_ops->set(mod->owner_id, ctrl->val);
		if (ret) {
			break;
		}
		ret = isp_os_driver_notify_color_range(ctrl->val);
		if (ret) {
			rgb2yuv->hw_ops->set(mod->owner_id, ctrl->cur.val);
		}
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set rgb2yuv ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops rgb2yuv_ctrl_ops = {
	.s_ctrl = rgb2yuv_s_ctrl,
};

static int isp_mod_rgb2yuv_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_rgb2yuv *rgb2yuv;

	rgb2yuv = get_mod(mod, rgb2yuv);
	rgb2yuv->hw_ops = isp_hw_rgb2yuv_get_ops();
	if (rgb2yuv->hw_ops == NULL) {
		isp_error("get rgb2yuv hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_rgb2yuv_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_rgb2yuv_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_rgb2yuv *rgb2yuv = get_mod(mod, rgb2yuv);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config color_range_config = {
		.ops = &rgb2yuv_ctrl_ops,
		.id = ISP_V4L2_CID_COLOR_RANGE,
		.name = "Color range",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = ISP_COLOR_RANGE_BT601_FULL_SWING,
		.max = ISP_COLOR_RANGE_BT709_FULL_SWING,
		.step = 2,
		.def = ISP_COLOR_RANGE_BT709_FULL_SWING,
	};

	rgb2yuv->color_range_ctrl =
		v4l2_ctrl_new_custom(handler, &color_range_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add rgb2yuv ctrls fail");
		return handler->error;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_rgb2yuv rgb2yuv;

int isp_register_mod_rgb2yuv(void)
{
	isp_memset(&rgb2yuv, 0, sizeof(rgb2yuv));
	rgb2yuv.mod.id = MOD_RGB2YUV;
	rgb2yuv.mod.name = "rgb2yuv";
	rgb2yuv.mod.init = isp_mod_rgb2yuv_init;
	rgb2yuv.mod.cleanup = isp_mod_rgb2yuv_cleanup;
	rgb2yuv.mod.add_ctrl = isp_mod_rgb2yuv_add_ctrl;
	rgb2yuv.mod.exec_actions = rgb2yuv_exec_actions;
	rgb2yuv.mod.exec_actions_num = ARRAY_SIZE(rgb2yuv_exec_actions);
	rgb2yuv.mod.size = sizeof(rgb2yuv);
	return isp_mod_register(&rgb2yuv.mod);
}

void isp_unregister_mod_rgb2yuv(void)
{
	isp_mod_unregister(&rgb2yuv.mod);
}
