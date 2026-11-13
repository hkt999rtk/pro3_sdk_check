/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_os_driver.h>
#include <isp_mod.h>
#include <isp_v4l2_ctrl.h>
#include <isp_notify.h>
#include <isp_hw_global.h>
#include "isp_mod_sensor.h"
#include "isp_mod_global.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_GLOBAL

struct isp_mod_global {
	struct isp_mod mod;
	int32_t snr_type; /* enum rts_isp_sensor_type */
	int32_t snr_inf; /* enum rts_isp_interface */
	int32_t snr_hdr; /* enum rts_isp_hdr_mode */
	struct rts_isp_bypass bypass;

	const struct isp_hw_global_ops *hw_ops;
	uint32_t isp_clk;
	int is_fpga;

	struct v4l2_ctrl *output_pos_ctrl;
	struct v4l2_ctrl *output_chn_ctrl;
	struct v4l2_ctrl *output_mode_ctrl;

	int started : 1;
	unsigned int wdr_enable : 1;
	unsigned int ldc_enable : 1;
};

uint32_t get_min_isp_clk(const struct isp_sensor_mode *mode, int snr_type,
						 int inf_type)
{
	int is_raw = snr_type == RAW_SENSOR;
	int is_mipi = inf_type == SNR_INTERFACE_MIPI;

	if (is_mipi && is_raw) {
		uint32_t isp_hts;
		uint32_t clk1 = 0;
		uint32_t clk2 = 0;
		const uint32_t RX_BUF_SIZE = 2048;

		/*
		 * |<----                sensor_hts             ---->|
		 * +-----------snr_width----------+------dummy-------+ @pclk
		 * +--------snr_width - SZ--------+-----SZ + 32------+ @isp_clk
		 * |<----                 isp_hts               ---->|
		 *
		 * isp_hts / isp_clk <= sensor_hts / pclk
		 * (snr_width - SZ) / isp_clk <= snr_width / pclk
		 */
		isp_hts = mode->size.snr.w + 32;
		clk1 = ceilf((float)isp_hts / mode->hts * mode->pclk);
		if (mode->size.snr.w > RX_BUF_SIZE)
			clk2 = ceilf((float)(mode->size.snr.w - RX_BUF_SIZE) /
						 mode->size.snr.w * mode->pclk);
		return max(clk1, clk2);
	} else if (!is_mipi && !is_raw) {
		return 2 * mode->pclk;
	} else {
		return mode->pclk;
	}
}

static int global_calc_speed_ctrl(struct isp_mod_global *global,
								  const struct isp_sensor_mode *mode,
								  struct isp_speed_ctrl *speed)
{
	int speed_mult;
	int speed_div;
	int final_speed_mult = 1;
	int final_speed_div = 1;
	int final_last_break;
	float clk_ratio;
	float final_ratio = 1;
	float ratio;
	const int MAX_MULT = 7;
	const int MAX_DIV = 31;

	if (!global || !mode || !speed) {
		return -RTS_ISP_EINVAL;
	}
	if (!global->isp_clk) {
		return -RTS_ISP_ENOTREADY;
	}

	clk_ratio = (float)mode->pclk / global->isp_clk;
	for (speed_mult = 1; speed_mult <= MAX_MULT; speed_mult++) {
		for (speed_div = MAX_DIV; speed_div > 0; speed_div--) {
			ratio = (float)speed_mult / speed_div;
			if (ratio > final_ratio) {
				break;
			}
			if (ratio >= clk_ratio) {
				final_ratio = ratio;
				final_speed_mult = speed_mult;
				final_speed_div = speed_div;
				continue;
			}
		}
	}
	final_last_break = mode->hts / clk_ratio - mode->size.isp.w *
					   final_speed_div /
					   final_speed_mult;
	final_last_break = min(0x3ff, final_last_break / 8);

	speed->speed_mult = final_speed_mult;
	speed->speed_div = final_speed_div;
	speed->last_break = final_last_break;

	return RTS_ISP_OK;
}

static int global_set_func_enable(struct isp_mod *mod)
{
	struct rts_isp_bypass bypass;
	struct isp_mod_global *global = get_mod(mod, global);

	bypass = global->bypass;
	bypass.bypass_wdr = global->bypass.bypass_wdr || !global->wdr_enable;
	bypass.bypass_ldc = global->bypass.bypass_ldc || !global->ldc_enable;

	return global->hw_ops->set_func_enable(mod->owner_id, global->snr_type,
										   global->snr_hdr, bypass);
}

static int mod_global_info_is_fpga(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	*(int *)data = global->is_fpga;
	return RTS_ISP_OK;
}

static int mod_global_info_isp_clk(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	if (!global->started) {
		return -RTS_ISP_ENOTREADY;
	}

	*(uint32_t *)data = global->isp_clk;
	return RTS_ISP_OK;
}

static struct isp_mod_action_info global_info_actions[] = {
	ISP_MOD_ACTION(GLOBAL_INFO_IS_FPGA, mod_global_info_is_fpga),
	ISP_MOD_ACTION(GLOBAL_INFO_ISP_CLK, mod_global_info_isp_clk),
};

static int mod_global_exec_start(struct isp_mod *mod, void *data)
{
	int ret;
	uint32_t isp_clk_needed;
	struct isp_mod_global *global = get_mod(mod, global);
	struct isp_speed_ctrl speed_ctrl;
	const struct isp_sensor_mode *mode;

	if (global->started) {
		return RTS_ISP_OK;
	}

	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_MODE, &mode,
								   sizeof(mode));
	if (ret) {
		goto err;
	}
	isp_clk_needed =
		get_min_isp_clk(mode, global->snr_type, global->snr_inf);
	global->isp_clk = isp_clk_needed;
	printf("isp_clk_needed: %u\n\r", isp_clk_needed);
	ret = isp_os_driver_set_clk(CLK_ISP, &global->isp_clk);
	if (ret) {
		return ret;
	}
	if (global->isp_clk < isp_clk_needed) {
		global->isp_clk = 0;
		isp_os_driver_set_clk(CLK_ISP, &global->isp_clk);
		return -RTS_ISP_ERANGE;
	}

	ret = global_calc_speed_ctrl(global, mode, &speed_ctrl);
	if (ret) {
		goto err;
	}
	ret = global->hw_ops->set_speed_ctrl(mod->owner_id, &speed_ctrl);
	if (ret) {
		goto err;
	}

	ret = global->hw_ops->start_transfer(mod->owner_id);
	if (ret) {
		goto err;
	}
	ret = global->hw_ops->enable_data(mod->owner_id, RTS_ISP_TRUE);
	if (ret) {
		goto err;
	}
	global->started = RTS_ISP_TRUE;
	return RTS_ISP_OK;
err:
	global->isp_clk = 0;
	isp_os_driver_set_clk(CLK_ISP, &global->isp_clk);
	return ret;
}

static int mod_global_exec_stop(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	if (!global->started) {
		return RTS_ISP_OK;
	}

	global->hw_ops->enable_data(mod->owner_id, RTS_ISP_FALSE);
	global->hw_ops->stop_transfer(mod->owner_id);
	global->isp_clk = 0;
	isp_os_driver_set_clk(CLK_ISP, &global->isp_clk);

	global->started = RTS_ISP_FALSE;

	return RTS_ISP_OK;
}

static int mod_global_exec_select_interface(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);
	int32_t inf = *(int32_t *)data;

	return global->hw_ops->select_interface(mod->owner_id, inf);
}

static int mod_global_exec_get_bypass(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	*(struct rts_isp_bypass *)data = global->bypass;

	return RTS_ISP_OK;
}

static int mod_global_exec_set_bypass(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	global->bypass = *(struct rts_isp_bypass *)data;

	return global_set_func_enable(mod);
}

static int mod_global_exec_set_wdr_enable(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	global->wdr_enable = *(uint32_t *)data;

	return global_set_func_enable(mod);
}

static int mod_global_exec_set_ldc_enable(struct isp_mod *mod, void *data)
{
	struct isp_mod_global *global = get_mod(mod, global);

	global->ldc_enable = *(uint32_t *)data;

	return global_set_func_enable(mod);
}

static int mod_global_exec_set_bayer_pattern(struct isp_mod *mod, void *data)
{
	int *bayer = data;
	struct isp_mod_global *global = get_mod(mod, global);

	return global->hw_ops->set_bayer(mod->owner_id, *bayer);
}

static int mod_global_exec_drop_frames(struct isp_mod *mod, void *data)
{
	int *frames = data;
	struct isp_mod_global *global = get_mod(mod, global);

	return global->hw_ops->delay_data(mod->owner_id, *frames);
}

static struct isp_mod_action_info global_exec_actions[] = {
	ISP_MOD_ACTION(GLOBAL_EXEC_START, mod_global_exec_start),
	ISP_MOD_ACTION(GLOBAL_EXEC_STOP, mod_global_exec_stop),
	ISP_MOD_ACTION(GLOBAL_EXEC_SELECT_INTERFACE,
				   mod_global_exec_select_interface),
	ISP_MOD_ACTION(GLOBAL_EXEC_GET_BYPASS, mod_global_exec_get_bypass),
	ISP_MOD_ACTION(GLOBAL_EXEC_SET_BYPASS, mod_global_exec_set_bypass),
	ISP_MOD_ACTION(GLOBAL_EXEC_SET_WDR_ENABLE,
				   mod_global_exec_set_wdr_enable),
	ISP_MOD_ACTION(GLOBAL_EXEC_SET_LDC_ENABLE,
				   mod_global_exec_set_ldc_enable),
	ISP_MOD_ACTION(GLOBAL_EXEC_SET_BAYER_PATTERN,
				   mod_global_exec_set_bayer_pattern),
	ISP_MOD_ACTION(GLOBAL_EXEC_DROP_FRAMES, mod_global_exec_drop_frames),
};

static int mod_global_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	int32_t line_cnt;
	struct isp_mod_global *global = get_mod(mod, global);
	const struct rts_isp_interface_info *info;
	const struct isp_sensor_mode *mode = data;

	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_INTERFACE,
								   &info, sizeof(info));
	if (ret) {
		goto err;
	}
	global->snr_type = info->type;
	global->snr_inf = info->interface;
	global->snr_hdr = mode->hdr;

	ret = global_set_func_enable(mod);
	if (ret) {
		goto err;
	}
	ret = global->hw_ops->set_isp_path(mod->owner_id, info->type);
	if (ret) {
		goto err;
	}
	/* reserved 5 ms or half line_cnts for check frame end nearing */
	line_cnt = mode->max_fps * mode->min_vts / 1000.0f * 5;
	line_cnt = max(mode->size.snr.h - line_cnt, mode->size.snr.h / 2);
	ret = global->hw_ops->set_line_cnt_int(mod->owner_id, line_cnt);
	if (ret) {
		goto err;
	}
	if (info->type == RAW_SENSOR) {
		ret = global->hw_ops->set_bayer(mod->owner_id,
										info->type_config.raw.bayer);
		if (ret) {
			goto err;
		}
	}
	ret = global->hw_ops->set_tnr_pos(mod->owner_id, mode->hdr);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	isp_perror(ret, "global mode change");
	return ret;
}

static struct isp_mod_action_info global_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_global_mode_change),
};

static int global_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_global *global = get_mod(mod, global);

	switch (ctrl->id) {
	case ISP_V4L2_CID_OUTPUT_POSITION:
		return global->hw_ops->set_output(mod->owner_id, ctrl->val);
	default:
		return -RTS_ISP_ECTRL;
	}
}

static const struct v4l2_ctrl_ops global_ctrl_ops = {
	.s_ctrl = global_s_ctrl,
};

static int isp_mod_global_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_global *global;

	global = get_mod(mod, global);
	global->hw_ops = isp_hw_global_get_ops();
	if (global->hw_ops == NULL) {
		isp_error("get global hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	global->is_fpga = isp_os_driver_is_fpga();

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_global_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_global_add_ctrl(struct isp_mod *mod, void *phandler)
{
	int ret;
	struct isp_mod_global *global = get_mod(mod, global);
	struct v4l2_ctrl_handler *handler = phandler;
	struct isp_output_menu menu;
	struct v4l2_ctrl_config output_pos_config = {
		.ops = &global_ctrl_ops,
		.id = ISP_V4L2_CID_OUTPUT_POSITION,
		.name = "Raw output position menu",
		.type = ISP_V4L2_CTRL_TYPE_MENU,
	};

	ret = global->hw_ops->get_output_menu(mod->owner_id, &menu);
	if (ret) {
		return ret;
	}
	output_pos_config.max = menu.num - 1,
	output_pos_config.qmenu = menu.menu,
	global->output_pos_ctrl =
		v4l2_ctrl_new_custom(handler, &output_pos_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add iq ctrls fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_global global;

int isp_register_mod_global(void)
{
	isp_memset(&global, 0, sizeof(global));
	global.mod.id = MOD_GLOBAL;
	global.mod.name = "global";
	global.mod.init = isp_mod_global_init;
	global.mod.cleanup = isp_mod_global_cleanup;
	global.mod.add_ctrl = isp_mod_global_add_ctrl;
	global.mod.info_actions = global_info_actions;
	global.mod.info_actions_num = ARRAY_SIZE(global_info_actions);
	global.mod.exec_actions = global_exec_actions;
	global.mod.exec_actions_num = ARRAY_SIZE(global_exec_actions);
	global.mod.notify_actions = global_notify_actions;
	global.mod.notify_actions_num = ARRAY_SIZE(global_notify_actions);
	global.mod.size = sizeof(global);
	return isp_mod_register(&global.mod);
}

void isp_unregister_mod_global(void)
{
	isp_mod_unregister(&global.mod);
}
