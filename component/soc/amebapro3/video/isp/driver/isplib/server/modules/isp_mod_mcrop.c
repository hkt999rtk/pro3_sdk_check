/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_driver.h>
#include <isp_mod.h>
#include <isp_core.h>
#include <isp_notify.h>
#include <isp_hw_mcrop.h>
#include "isp_mod_sensor.h"
#include "isp_mod_mcrop.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_MCROP

struct isp_mod_mcrop {
	struct isp_mod mod;
	rts_isp_rect_t rect;
	const struct isp_hw_mcrop_ops *hw_ops;
};

static int mod_mcrop_info_mcrop_size(struct isp_mod *mod, void *data)
{
	struct isp_mod_mcrop *mcrop = get_mod(mod, mcrop);
	rts_isp_size_t *size = data;

	if (!mcrop->rect.w) {
		return -RTS_ISP_ENOTREADY;
	}

	*size = mcrop->rect.size;

	return RTS_ISP_OK;
}

static struct isp_mod_action_info mcrop_info_actions[] = {
	ISP_MOD_ACTION(MCROP_INFO_SIZE, mod_mcrop_info_mcrop_size),
};

static int mod_mcrop_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_frame_info info;
	struct isp_mod_mcrop *mcrop = get_mod(mod, mcrop);
	const struct isp_sensor_mode *mode = data;

	mcrop->rect.start.x = mcrop->rect.start.y = 0;
	mcrop->rect.size = mode->size.isp;
	ret = mcrop->hw_ops->set_crop(mod->owner_id, &mcrop->rect);
	if (ret) {
		return ret;
	}
	ret = isp_core_notify_sched(mod->owner, ISP_NOTIFY_MCROP_CHANGE,
								&mcrop->rect, sizeof(mcrop->rect), NULL,
								NULL);
	if (ret) {
		return ret;
	}

	info.width = mcrop->rect.w;
	info.height = mcrop->rect.h;
	info.fps_min.numerator = SENSOR_FPS_PRECISION;
	info.fps_min.denominator = SENSOR_FPS_PRECISION * mode->min_fps;
	info.fps_max.numerator = SENSOR_FPS_PRECISION;
	info.fps_max.denominator = SENSOR_FPS_PRECISION * mode->max_fps;

	return isp_os_driver_set_info(&info);
}

static struct isp_mod_action_info mcrop_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_mcrop_mode_change),
};

static int isp_mod_mcrop_init(struct isp_mod *mod)
{
	struct isp_mod_mcrop *mcrop;

	mcrop = get_mod(mod, mcrop);
	mcrop->hw_ops = isp_hw_mcrop_get_ops();
	if (mcrop->hw_ops == NULL) {
		isp_error("get mcrop hw ops fail\n");
		return -RTS_ISP_ENOOPS;
	}

	return RTS_ISP_OK;
}

static int isp_mod_mcrop_cleanup(struct isp_mod *mod)
{
	struct isp_frame_info info;

	info.width = 0;
	info.height = 0;
	info.fps_min.numerator = 1;
	info.fps_min.denominator = 30;
	info.fps_max.numerator = 1;
	info.fps_max.denominator = 30;

	return isp_os_driver_set_info(&info);
}

static struct isp_mod_mcrop mcrop;

int isp_register_mod_mcrop(void)
{
	isp_memset(&mcrop, 0, sizeof(mcrop));
	mcrop.mod.id = MOD_MCROP;
	mcrop.mod.name = "mcrop";
	mcrop.mod.init = isp_mod_mcrop_init;
	mcrop.mod.cleanup = isp_mod_mcrop_cleanup;
	mcrop.mod.info_actions = mcrop_info_actions;
	mcrop.mod.info_actions_num = ARRAY_SIZE(mcrop_info_actions);
	mcrop.mod.notify_actions = mcrop_notify_actions;
	mcrop.mod.notify_actions_num = ARRAY_SIZE(mcrop_notify_actions);
	mcrop.mod.size = sizeof(mcrop);
	return isp_mod_register(&mcrop.mod);
}

void isp_unregister_mod_mcrop(void)
{
	isp_mod_unregister(&mcrop.mod);
}
