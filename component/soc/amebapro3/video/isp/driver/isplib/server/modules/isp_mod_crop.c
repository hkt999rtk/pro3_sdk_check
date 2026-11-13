/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_core.h>
#include <isp_v4l2_ctrl.h>
#include <isp_hw_crop.h>
#include "isp_mod_sensor.h"
#include "isp_mod_input.h"
#include "isp_mod_crop.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_CROP

struct isp_mod_crop {
	struct isp_mod mod;

	const struct isp_sensor_mode *mode;

	struct {
		struct v4l2_ctrl *crop_x_ctrl;
		struct v4l2_ctrl *crop_y_ctrl;
	};

	const struct isp_hw_crop_ops *hw_ops;
	struct rts_isp_mirror_flip mf_info;
};

static void crop_adjust_start_based_on_mirror_flip(struct isp_mod_crop *crop,
		rts_isp_point_t *start)
{
	if (crop->mf_info.mirror)
		start->x = crop->mode->size.snr.w - crop->mode->size.isp.w -
				   start->x;
	if (crop->mf_info.flip)
		start->y = crop->mode->size.snr.h - crop->mode->size.isp.h -
				   start->y;
}

static int crop_set_crop(struct isp_mod_crop *crop)
{
	rts_isp_rect_t rect;

	rect.size = crop->mode->size.isp;
	rect.start.x = crop->crop_x_ctrl->val;
	rect.start.y = crop->crop_y_ctrl->val;
	crop_adjust_start_based_on_mirror_flip(crop, &rect.start);

	return crop->hw_ops->set_crop(crop->mod.owner_id, &rect);
}

static int crop_get_ctrl_range(struct isp_mod_crop *crop, rts_isp_point_t *min,
							   rts_isp_point_t *max, rts_isp_point_t *def)
{
	int ret;
	int remain;
	rts_isp_point_t start;
	const struct isp_sensor_mode *mode = crop->mode;

	ret = isp_mod_sibling_get_info(&crop->mod, MOD_SENSOR, SENSOR_INFO_CROP,
								   &start, sizeof(start));
	if (ret) {
		return ret;
	}

	min->x = start.x & 1;
	remain = max_t(int, mode->size.snr.w - mode->size.isp.w - min->x, 0);
	max->x = min->x + ALIGN_DOWN(remain, 2);
	def->x = start.x;

	min->y = start.y & 1;
	remain = max_t(int, mode->size.snr.h - mode->size.isp.h - min->y, 0);
	max->y = min->y + ALIGN_DOWN(remain, 2);
	def->y = start.y;

	return RTS_ISP_OK;
}

static int crop_modify_ctrl_range(struct isp_mod_crop *crop,
								  const rts_isp_point_t *min,
								  const rts_isp_point_t *max,
								  const rts_isp_point_t *def)
{
	int ret;

	v4l2_ctrl_modify_range(crop->crop_x_ctrl, min->x, max->x, 2, def->x);
	v4l2_ctrl_modify_range(crop->crop_y_ctrl, min->y, max->y, 2, def->y);

	ret = v4l2_ctrl_s_ctrl(crop->crop_x_ctrl, def->x);
	if (ret) {
		return ret;
	}
	return v4l2_ctrl_s_ctrl(crop->crop_y_ctrl, def->y);
}

static int mod_crop_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	rts_isp_point_t min;
	rts_isp_point_t max;
	rts_isp_point_t def;
	const struct isp_sensor_mode *mode = data;
	struct isp_mod_crop *crop = get_mod(mod, crop);

	crop->mode = mode;

	ret = crop_get_ctrl_range(crop, &min, &max, &def);
	if (ret) {
		return ret;
	}
	return crop_modify_ctrl_range(crop, &min, &max, &def);
}

static int mod_crop_mirror_flip_change(struct isp_mod *mod, void *data)
{
	int ret;
	int mode;
	struct isp_mod_crop *crop = get_mod(mod, crop);

	ret = isp_mod_sibling_get_info(mod, MOD_INPUT,
								   INPUT_INFO_LARGE_IMAGE_MODE, &mode,
								   sizeof(mode));
	if (ret) {
		return ret;
	}
	if (mode) {
		return RTS_ISP_OK;
	}

	crop->mf_info = *(struct rts_isp_mirror_flip *)data;
	return crop_set_crop(crop);
}

static struct isp_mod_action_info crop_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_crop_mode_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MIRROR_FLIP_CHANGE,
					  mod_crop_mirror_flip_change),
};

static void crop_notify_callback(int ret, void *priv)
{
	isp_free(priv);
}

static int crop_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_crop *crop = get_mod(mod, crop);
	rts_isp_point_t *offset;

	switch (ctrl->id) {
	case ISP_V4L2_CID_SNR_CROP_X:
	case ISP_V4L2_CID_SNR_CROP_Y:
		ret = crop_set_crop(crop);
		if (ret) {
			return ret;
		}
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	if (!isp_calloc_item(offset)) {
		return -RTS_ISP_ENOMEM;
	}
	offset->x = crop->crop_x_ctrl->val - crop->crop_x_ctrl->default_value;
	offset->y = crop->crop_y_ctrl->val - crop->crop_y_ctrl->default_value;

	return isp_core_notify_sched(mod->owner, ISP_NOTIFY_CROP_CHANGE, offset,
								 sizeof(*offset), crop_notify_callback,
								 offset);
}

static const struct v4l2_ctrl_ops crop_ctrl_ops = {
	.s_ctrl = crop_s_ctrl,
};

static int isp_mod_crop_init(struct isp_mod *mod)
{
	struct isp_mod_crop *crop;

	crop = get_mod(mod, crop);
	crop->hw_ops = isp_hw_crop_get_ops();
	if (crop->hw_ops == NULL) {
		isp_error("get crop hw ops fail\n");
		return -RTS_ISP_ENOOPS;
	}

	return RTS_ISP_OK;
}

static int isp_mod_crop_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_crop_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_crop *crop = get_mod(mod, crop);
	struct v4l2_ctrl_handler *handler = phandler;
	struct v4l2_ctrl_config crop_x_config = {
		.ops = &crop_ctrl_ops,
		.id = ISP_V4L2_CID_SNR_CROP_X,
		.name = "Sensor horizontal crop",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = 0,
		.max = 0,
		.step = 2,
		.def = 0,
	};
	struct v4l2_ctrl_config crop_y_config = {
		.ops = &crop_ctrl_ops,
		.id = ISP_V4L2_CID_SNR_CROP_Y,
		.name = "Sensor vertical crop",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = 0,
		.max = 0,
		.step = 2,
		.def = 0,
	};

	crop->crop_x_ctrl = v4l2_ctrl_new_custom(handler, &crop_x_config, mod);
	crop->crop_y_ctrl = v4l2_ctrl_new_custom(handler, &crop_y_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add crop ctrls fail");
		return handler->error;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_crop crop;

int isp_register_mod_crop(void)
{
	isp_memset(&crop, 0, sizeof(crop));
	crop.mod.id = MOD_CROP;
	crop.mod.name = "crop";
	crop.mod.init = isp_mod_crop_init;
	crop.mod.cleanup = isp_mod_crop_cleanup;
	crop.mod.add_ctrl = isp_mod_crop_add_ctrl;
	crop.mod.notify_actions = crop_notify_actions;
	crop.mod.notify_actions_num = ARRAY_SIZE(crop_notify_actions);
	crop.mod.size = sizeof(crop);
	return isp_mod_register(&crop.mod);
}

void isp_unregister_mod_crop(void)
{
	isp_mod_unregister(&crop.mod);
}
