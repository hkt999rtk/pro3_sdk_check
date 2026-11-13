/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_os_driver.h>
#include <isp_utils.h>
#include <isp_top.h>
#include <isp_poll.h>
#include <isp_core.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_v4l2_ctrl.h>
#include <isp_tracker.h>
#include "isp_mod_sensor.h"
#include "isp_mod_interface.h"
#include "isp_mod_global.h"
#include "isp_mod_control.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_CONTROL

#define MAX_STREAM_NUM 8

#define WATCHDOG_PRECISION 100

struct isp_mod_control {
	struct isp_mod mod;
	struct v4l2_ctrl_handler *ctrl_handler;
	float current_fps;

	isp_timer_handle_t timer;
	uint32_t watchdog_timeout;
	int timeout_cnt;
};

static int mod_control_info_top_status(struct isp_mod *mod, void *data)
{
	*(int *)data = isp_top_get_status();
	return RTS_ISP_OK;
}

static struct isp_mod_action_info control_info_actions[] = {
	ISP_MOD_ACTION(CONTROL_INFO_TOP_STATUS, mod_control_info_top_status),
};

static int stop_isp_pipeline(struct isp_mod *mod);

static int set_sensor_fps(struct isp_mod *mod, float fps)
{
	return isp_mod_sibling_execute(mod, MOD_SENSOR, SENSOR_EXEC_SET_FPS,
								   &fps, sizeof(fps));
}

static int start_isp_pipeline(struct isp_mod *mod)
{
	int ret;

	isp_info("start isp pipeline\n");

	ret = isp_mod_sibling_execute(mod, MOD_GLOBAL, GLOBAL_EXEC_START, NULL,
								  0);
	if (ret) {
		goto err;
	}
	ret = isp_mod_sibling_execute(mod, MOD_INTERFACE, INTERFACE_EXEC_START,
								  NULL, 0);
	if (ret) {
		goto err;
	}
	return RTS_ISP_OK;
err:
	isp_perror(ret, "start isp pipeline fail");
	stop_isp_pipeline(mod);
	return ret;
}

static int stop_isp_pipeline(struct isp_mod *mod)
{
	int ret;

	isp_info("stop isp pipeline\n");

	ret = isp_mod_sibling_execute(mod, MOD_INTERFACE, INTERFACE_EXEC_STOP,
								  NULL, 0);
	if (ret) {
		goto err;
	}
	ret = isp_mod_sibling_execute(mod, MOD_GLOBAL, GLOBAL_EXEC_STOP, NULL,
								  0);
	if (ret) {
		goto err;
	}
	return RTS_ISP_OK;
err:
	isp_perror(ret, "stop isp pipeline fail");
	return ret;
}

static int control_enable_timer(struct isp_mod_control *control, int enable)
{
	if (enable && control->watchdog_timeout)
		return isp_timer_start(&control->timer, WATCHDOG_PRECISION,
							   WATCHDOG_PRECISION);
	else {
		return isp_timer_stop(&control->timer);
	}
}

static int mod_control_start_isp(struct isp_mod *mod, float fps)
{
	int ret;
	struct isp_mod_control *control = get_mod(mod, control);

	ret = isp_v4l2_ctrl_handler_setup(control->ctrl_handler);
	if (ret) {
		goto err;
	}
	ret = isp_core_notify(mod->owner, ISP_NOTIFY_START_PRE, NULL, 0);
	if (ret) {
		goto err;
	}
	ret = start_isp_pipeline(mod);
	if (ret) {
		goto err;
	}
	ret = set_sensor_fps(mod, fps);
	if (ret) {
		goto err;
	}
	ret = isp_core_notify(mod->owner, ISP_NOTIFY_START_POST, NULL, 0);
	if (ret) {
		goto err;
	}

	isp_top_set_status(RTS_ISP_STREAMING);

	return RTS_ISP_OK;
err:
	set_sensor_fps(mod, 0);
	stop_isp_pipeline(mod);
	isp_perror(ret, "fail to start isp pipeline");
	return ret;
}

static int mod_control_stop_isp(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_control *control = get_mod(mod, control);

	ret = isp_core_notify(mod->owner, ISP_NOTIFY_STOP_PRE, NULL, 0);
	if (ret) {
		goto err;
	}
	ret = set_sensor_fps(mod, 0);
	if (ret) {
		goto err;
	}
	ret = stop_isp_pipeline(mod);
	if (ret) {
		goto err;
	}
	ret = isp_core_notify(mod->owner, ISP_NOTIFY_STOP_POST, NULL, 0);
	if (ret) {
		goto err;
	}
	isp_v4l2_ctrl_handler_backup_auto(control->ctrl_handler);

	isp_top_set_status(RTS_ISP_RUNNING);

	return RTS_ISP_OK;
err:
	isp_perror(ret, "stop isp pipeline");
	return ret;
}

static int mod_control_change_fps(struct isp_mod *mod, float fps)
{
	int ret;

	ret = set_sensor_fps(mod, fps);
	if (ret) {
		isp_perror(ret, "change sensor fps fail");
		return ret;
	}

	return RTS_ISP_OK;
}

static int mod_control_set_fps(struct isp_mod *mod, float fps)
{
	int ret;
	struct isp_mod_control *control = get_mod(mod, control);

	if (float_eq(fps, control->current_fps)) {
		return RTS_ISP_OK;
	}

	if (float_eq(control->current_fps, 0)) {
		ret = mod_control_start_isp(mod, fps);
	} else if (float_eq(fps, 0)) {
		ret = mod_control_stop_isp(mod);
	} else {
		ret = mod_control_change_fps(mod, fps);
	}
	if (!ret) {
		control->current_fps = fps;
	}
	return ret;
}

static int mod_control_exec_set_fps(struct isp_mod *mod, void *data)
{
	float fps;
	struct isp_preview_info *info = data;

	if (info->fps.numerator) {
		fps = (float)info->fps.denominator / info->fps.numerator;
	} else {
		fps = 0.0f;
	}

	return mod_control_set_fps(mod, fps);
}

static int mod_control_exec_set_track(struct isp_mod *mod, void *data)
{
	int on = *(int *)data;

	if (on) {
		time_tracker_init();
	} else {
		time_tracker_done();
	}

	return RTS_ISP_OK;
}

static int mod_control_exec_set_watchdog(struct isp_mod *mod, void *data)
{
	int timeout = *(uint32_t *)data;
	struct isp_mod_control *control = get_mod(mod, control);

	if (timeout && timeout < 500) {
		return -RTS_ISP_ERANGE;
	}

	isp_info("set stream watchdog timeout to %d\n", timeout);
	control->watchdog_timeout = timeout;
	control_enable_timer(control, control->current_fps);

	return RTS_ISP_OK;
}

static int mod_control_info_get_count(struct isp_mod *mod, void *data)
{
	struct isp_count *count = data;

	return isp_os_driver_get_count(count);
}

static struct isp_mod_action_info control_exec_actions[] = {
	ISP_MOD_ACTION(CONTROL_EXEC_SET_FPS, mod_control_exec_set_fps),
	ISP_MOD_ACTION(CONTROL_EXEC_SET_TRACK, mod_control_exec_set_track),
	ISP_MOD_ACTION(CONTROL_EXEC_SET_WATCHDOG,
				   mod_control_exec_set_watchdog),
	ISP_MOD_ACTION(CONTROL_EXEC_GET_COUNT, mod_control_info_get_count),
};

static int mod_control_statis_done(struct isp_mod *mod, void *data)
{
	struct isp_mod_control *control = get_mod(mod, control);

	control->timeout_cnt = 0;

	return RTS_ISP_OK;
}

static int mod_control_start_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_control *control = get_mod(mod, control);

	control->timeout_cnt = 0;

	return control_enable_timer(control, RTS_ISP_TRUE);
}

static int mod_control_stop_pre(struct isp_mod *mod, void *data)
{
	struct isp_mod_control *control = get_mod(mod, control);

	/* do not stop timer for multiple retrying */
	if (control->timeout_cnt >
		control->watchdog_timeout / WATCHDOG_PRECISION) {
		return RTS_ISP_OK;
	}

	return control_enable_timer(control, RTS_ISP_FALSE);
}

static struct isp_mod_action_info control_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_control_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_START_POST, mod_control_start_post),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STOP_PRE, mod_control_stop_pre),
};

static int watchdog_timer_callback(isp_timer_handle_t *timer, void *data)
{
	struct isp_mod *mod = data;
	struct isp_mod_control *control = get_mod(mod, control);
	float fps = control->current_fps;
	int num = control->watchdog_timeout / WATCHDOG_PRECISION;
	int num_4frame = ceilf(1000.0 / fps * 4 / WATCHDOG_PRECISION);

	if (control->timeout_cnt > num &&
		((control->timeout_cnt - num - 1) % num_4frame == 0)) {
		isp_warning("stream watchdog timedout, restart stream\n");
		isp_top_lock_internal();
		mod_control_set_fps(mod, 0);
		mod_control_set_fps(mod, fps);
		isp_os_driver_notify_dynamic_fps(fps);
		control->current_fps = fps;
		isp_top_unlock_internal();
	}
	control->timeout_cnt++;

	return RTS_ISP_OK;
}

static int isp_mod_control_init(struct isp_mod *mod)
{
	struct isp_mod_control *control = get_mod(mod, control);

	control->watchdog_timeout = 0;
	return isp_timer_init(&control->timer, isp_top_get_poll(),
						  watchdog_timer_callback, mod);
}

static int isp_mod_control_cleanup(struct isp_mod *mod)
{
	struct isp_mod_control *control = get_mod(mod, control);

	isp_timer_cleanup(&control->timer);

	return RTS_ISP_OK;
}

static int isp_mod_control_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_control *control = get_mod(mod, control);

	control->ctrl_handler = phandler;
	return RTS_ISP_OK;
}

static struct isp_mod_control control;

int isp_register_mod_control(void)
{
	isp_memset(&control, 0, sizeof(control));
	control.mod.id = MOD_CONTROL;
	control.mod.name = "control";
	control.mod.init = isp_mod_control_init;
	control.mod.cleanup = isp_mod_control_cleanup;
	control.mod.add_ctrl = isp_mod_control_add_ctrl;
	control.mod.info_actions = control_info_actions;
	control.mod.info_actions_num = ARRAY_SIZE(control_info_actions);
	control.mod.exec_actions = control_exec_actions;
	control.mod.exec_actions_num = ARRAY_SIZE(control_exec_actions);
	control.mod.notify_actions = control_notify_actions;
	control.mod.notify_actions_num = ARRAY_SIZE(control_notify_actions);
	control.mod.virtual = RTS_ISP_TRUE;
	control.mod.size = sizeof(control);
	return isp_mod_register(&control.mod);
}

void isp_unregister_mod_control(void)
{
	isp_mod_unregister(&control.mod);
}
