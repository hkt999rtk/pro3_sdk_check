/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_driver.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_hw_interface.h>
#include "isp_mod_sensor.h"
#include "isp_mod_global.h"
#include "isp_mod_interface.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_INTERFACE

struct isp_mod_interface {
	struct isp_mod mod;
	struct isp_interface_info info;
	const struct isp_hw_interface_ops *hw_ops;
	int started : 1;
};

static int interface_sel_state(struct isp_mod_interface *interface, int32_t inf)
{
	int ret;

	ret = isp_os_driver_sel_pin_state(inf);
	if (ret) {
		return ret;
	}
	return isp_mod_sibling_execute(&interface->mod, MOD_GLOBAL,
								   GLOBAL_EXEC_SELECT_INTERFACE, &inf,
								   sizeof(inf));
}

static int interface_set_clk(struct isp_mod_interface *interface, uint32_t clk)
{
	int ret;

	if (interface->info.info->interface != SNR_INTERFACE_MIPI) {
		return RTS_ISP_OK;
	}

	interface->info.interface_clk = interface->info.pixel_clk;
	ret = isp_os_driver_set_clk(CLK_INTERFACE,
								&interface->info.interface_clk);
	if (ret) {
		return ret;
	}
	if (interface->info.interface_clk < interface->info.pixel_clk) {
		return -RTS_ISP_ERANGE;
	}
	return RTS_ISP_OK;
}

static int mod_interface_exec_start(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_interface *interface = get_mod(mod, interface);

	if (interface->started) {
		return RTS_ISP_OK;
	}

	ret = interface_set_clk(interface, interface->info.pixel_clk);
	if (ret) {
		goto err;
	}
	ret = interface_sel_state(interface, interface->info.info->interface);
	if (ret) {
		return ret;
	}
	ret = interface->hw_ops->start_transfer(mod->owner_id,
												&interface->info);
	if (ret) {
		goto err;
	}

	interface->started = RTS_ISP_TRUE;
	return RTS_ISP_OK;
err:
	interface_set_clk(interface, 0);
	interface_sel_state(interface, SNR_INTERFACE_NONE);
	isp_perror(ret, "interface start fail");
	return ret;
}

static int mod_interface_exec_stop(struct isp_mod *mod, void *data)
{
	struct isp_mod_interface *interface = get_mod(mod, interface);

	if (!interface->started) {
		return RTS_ISP_OK;
	}

	interface->hw_ops->stop_transfer(mod->owner_id, &interface->info);
	interface_set_clk(interface, 0);
	interface_sel_state(interface, SNR_INTERFACE_NONE);

	interface->started = RTS_ISP_FALSE;
	return RTS_ISP_OK;
}

static int mod_interface_exec_short_exp_delay(struct isp_mod *mod, void *data)
{
	struct isp_mod_interface *interface = get_mod(mod, interface);
	struct isp_interface_novc_delay *novc_delay = data;

	return interface->hw_ops->set_hdr_delay(mod->owner_id,
												novc_delay->channel[0]);
}

static struct isp_mod_action_info interface_exec_actions[] = {
	ISP_MOD_ACTION(INTERFACE_EXEC_START, mod_interface_exec_start),
	ISP_MOD_ACTION(INTERFACE_EXEC_STOP, mod_interface_exec_stop),
	ISP_MOD_ACTION(INTERFACE_EXEC_SHORT_EXP_DELAY,
				   mod_interface_exec_short_exp_delay),
};

static int mod_interface_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	const struct isp_sensor_mode *mode = data;
	struct isp_mod_interface *interface = get_mod(mod, interface);
	struct isp_interface_info *info = &interface->info;

	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_INTERFACE,
								   &info->info, sizeof(info->info));
	if (ret) {
		return ret;
	}
	info->size = mode->size.snr;
	info->pixel_clk = mode->pclk;
	ret = isp_mod_sibling_get_info(mod, MOD_GLOBAL, GLOBAL_INFO_IS_FPGA,
								   &info->is_fpga, sizeof(info->is_fpga));
	if (ret) {
		return ret;
	}

	if (info->info->interface == SNR_INTERFACE_DVP) {
		interface->hw_ops = isp_hw_dvp_get_ops();
	} else if (info->info->interface == SNR_INTERFACE_MIPI) {
		interface->hw_ops = isp_hw_mipi_get_ops();
	} else if (info->info->interface == SNR_INTERFACE_NONE) {
		interface->hw_ops = isp_hw_vinf_get_ops();
	}
	if (interface->hw_ops == NULL) {
		isp_error("get interface hw ops fail\n");
		return -RTS_ISP_ENOOPS;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_action_info interface_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_interface_mode_change),
};

static int isp_mod_interface_init(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_interface_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static struct isp_mod_interface interface;

int isp_register_mod_interface(void)
{
	isp_memset(&interface, 0, sizeof(interface));
	interface.mod.id = MOD_INTERFACE;
	interface.mod.name = "interface";
	interface.mod.init = isp_mod_interface_init;
	interface.mod.cleanup = isp_mod_interface_cleanup;
	interface.mod.exec_actions = interface_exec_actions;
	interface.mod.exec_actions_num = ARRAY_SIZE(interface_exec_actions);
	interface.mod.notify_actions = interface_notify_actions;
	interface.mod.notify_actions_num = ARRAY_SIZE(interface_notify_actions);
	interface.mod.size = sizeof(interface);
	return isp_mod_register(&interface.mod);
}

void isp_unregister_mod_interface(void)
{
	isp_mod_unregister(&interface.mod);
}
