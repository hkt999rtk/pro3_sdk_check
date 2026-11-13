/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2018 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <isp_top.h>
#include <isp_utils.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_mod.h>
#include <isp_v4l2_ctrl.h>
#include <isp_hw_spe.h>
#include "isp_mod_spe.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_SPE

struct isp_mod_spe {
	struct isp_mod mod;
	const struct isp_hw_spe_ops *hw_ops;

	const isp_iq_spe_v1_t *iq;
	isp_algo_param_spe_v1_t *algo_param;
	isp_iq_spe_v1_dyn_item_t dyn_items[3];

	struct v4l2_ctrl *brightness_ctrl;
	struct v4l2_ctrl *contrast_ctrl;
	struct v4l2_ctrl *saturation_ctrl;
	struct v4l2_ctrl *gray_mode_ctrl;
	struct v4l2_ctrl *y_gain_ctrl;
};

static int spe_set_brightness(struct isp_mod *mod, int value)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);

	return spe->hw_ops->set_brightness(mod->owner_id, value);
}

static int spe_set_contrast(struct isp_mod *mod, int value)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);

	return spe->hw_ops->set_contrast(mod->owner_id, value);
}

static int spe_set_saturation(struct isp_mod *mod, int value)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);

	return spe->hw_ops->set_saturation(mod->owner_id, value);
}

static int spe_set_gray_mode(struct isp_mod *mod, int enable)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);

	return spe->hw_ops->set_gray_mode(mod->owner_id, enable);
}

static int spe_set_y_gain(struct isp_mod *mod, int value)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);

	return spe->hw_ops->set_y_gain(mod->owner_id, value);
}

static int spe_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;

	switch (ctrl->id) {
	case ISP_V4L2_CID_BRIGHTNESS:
		ret = spe_set_brightness(mod, ctrl->val);
		break;
	case ISP_V4L2_CID_CONTRAST:
		ret = spe_set_contrast(mod, ctrl->val);
		break;
	case ISP_V4L2_CID_SATURATION:
		ret = spe_set_saturation(mod, ctrl->val);
		break;
	case ISP_V4L2_CID_GRAY:
		ret = spe_set_gray_mode(mod, ctrl->val);
		break;
	case ISP_V4L2_CID_Y_GAIN:
		ret = spe_set_y_gain(mod, ctrl->val);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set spe ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops spe_ctrl_ops = {
	.s_ctrl = spe_s_ctrl,
};

static int isp_mod_spe_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_spe *spe;

	spe = get_mod(mod, spe);
	spe->hw_ops = isp_hw_spe_get_ops();
	if (spe->hw_ops == NULL) {
		isp_error("get spe hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	spe->algo_param = isp_algo_param_get_spe_v1(mod->owner_id);
	if (!spe->algo_param) {
		isp_error("get spe algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_spe_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_spe_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);

	return spe->hw_ops->init_vreg(mod->owner_id);
}

static void intp_spe_v1_item(isp_iq_spe_v1_dyn_item_t *item, int val, int th0,
							 const isp_iq_spe_v1_dyn_item_t *item0, int th1,
							 const isp_iq_spe_v1_dyn_item_t *item1)
{
	item->u = linear_intp(val, th0, item0->u, th1, item1->u);
	item->v = linear_intp(val, th0, item0->v, th1, item1->v);
}

DEFINE_ISP_IQ_INTP_FUNC(spe, v1, intp_spe_v1_item)

static int mod_spe_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);
	const isp_iq_spe_v1_dyn_item_t *item;

	spe->iq = isp_iq_get_spe_v1(mod->owner_id, &spe->algo_param->common);
	if (!spe->iq) {
		return -RTS_ISP_EINVAL;
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_spe_v1_dyn_enabled(&spe->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_spe_v1_dyn_item_default(&spe->iq->dynamic.nest,
			&spe->algo_param->common);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	return spe->hw_ops->set_uv_offset(mod->owner_id, item);
}

static int mod_spe_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);
	const isp_iq_spe_v1_dyn_item_t *item;
	const struct isp_notify_dynamic *dyn = data;

	if (!isp_iq_spe_v1_dyn_enabled(&spe->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_spe_v1_dyn_item(&spe->iq->dynamic.nest,
									  &spe->algo_param->common, dyn,
									  isp_iq_spe_v1_intp_cb, spe);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	return spe->hw_ops->set_uv_offset(mod->owner_id, item);
}

static int mod_spe_dynamic_post(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_spe *spe = get_mod(mod, spe);

	ret = spe_set_brightness(mod, spe->brightness_ctrl->cur.val);
	if (ret) {
		return ret;
	}
	ret = spe_set_saturation(mod, spe->saturation_ctrl->cur.val);
	if (ret) {
		return ret;
	}
	return spe_set_contrast(mod, spe->contrast_ctrl->cur.val);
}

static struct isp_mod_action_info spe_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_spe_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_spe_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_spe_dynamic_post),
};

static int isp_mod_spe_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_spe *spe = get_mod(mod, spe);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config gray_config = {
		.ops = &spe_ctrl_ops,
		.id = ISP_V4L2_CID_GRAY,
		.name = "Gray Mode",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
	};
	const struct v4l2_ctrl_config ygain_config = {
		.ops = &spe_ctrl_ops,
		.id = ISP_V4L2_CID_Y_GAIN,
		.name = "y gain",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = YGAIN_CTRL_MIN,
		.max = YGAIN_CTRL_MAX,
		.step = 1,
		.def = YGAIN_CTRL_DEF,
	};

	spe->brightness_ctrl = v4l2_ctrl_new_std(
							   handler, &spe_ctrl_ops, ISP_V4L2_CID_BRIGHTNESS,
							   BRIGHT_CTRL_MIN, BRIGHT_CTRL_MAX, 1, BRIGHT_CTRL_DEF, mod);
	spe->contrast_ctrl =
		v4l2_ctrl_new_std(handler, &spe_ctrl_ops, ISP_V4L2_CID_CONTRAST,
						  CONTRAST_CTRL_MIN, CONTRAST_CTRL_MAX, 1,
						  CONTRAST_CTRL_DEF, mod);
	spe->saturation_ctrl = v4l2_ctrl_new_std(handler, &spe_ctrl_ops,
						   ISP_V4L2_CID_SATURATION,
						   SATURATION_CTRL_MIN,
						   SATURATION_CTRL_MAX, 1,
						   SATURATION_CTRL_DEF, mod);
	spe->gray_mode_ctrl = v4l2_ctrl_new_custom(handler, &gray_config, mod);
	spe->y_gain_ctrl = v4l2_ctrl_new_custom(handler, &ygain_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add spe ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_spe spe;

int isp_register_mod_spe(void)
{
	isp_memset(&spe, 0, sizeof(spe));
	spe.mod.id = MOD_SPE;
	spe.mod.name = "spe";
	spe.mod.init = isp_mod_spe_init;
	spe.mod.cleanup = isp_mod_spe_cleanup;
	spe.mod.hardware_init = isp_mod_spe_hardware_init;
	spe.mod.add_ctrl = isp_mod_spe_add_ctrl;
	spe.mod.notify_actions = spe_notify_actions;
	spe.mod.notify_actions_num = ARRAY_SIZE(spe_notify_actions);
	spe.mod.size = sizeof(spe);
	return isp_mod_register(&spe.mod);
}

void isp_unregister_mod_spe(void)
{
	isp_mod_unregister(&spe.mod);
}
