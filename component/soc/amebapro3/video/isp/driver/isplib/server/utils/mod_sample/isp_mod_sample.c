/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_mod.h>
#include <isp_hw_sample.h>
#include "isp_mod_sample.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_SAMPLE

struct isp_mod_sample {
	struct isp_mod mod;
	const struct isp_hw_sample_ops *hw_ops;
};

// static struct isp_mod_action_info sample_exec_actions[] = {
// };

// static struct isp_mod_action_info sample_info_actions[] = {
// };

// static struct isp_mod_action_info sample_notify_actions[] = {
// };

static int isp_mod_sample_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_sample *sample;

	sample = get_mod(mod, sample);
	sample->hw_ops = isp_hw_sample_get_ops();
	if (sample->hw_ops == NULL) {
		isp_error("get sample hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_sample_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_sample_hardware_init(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_sample_hardware_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_sample_add_ctrl(struct isp_mod *mod, void *phandler)
{
	return RTS_ISP_OK;
}

static struct isp_mod_sample sample;

int isp_register_mod_sample(void)
{
	isp_memset(&sample, 0, sizeof(sample));
	sample.mod.id = MOD_SAMPLE;
	sample.mod.name = "sample";
	sample.mod.init = isp_mod_sample_init;
	sample.mod.cleanup = isp_mod_sample_cleanup;
	sample.mod.hardware_init = isp_mod_sample_hardware_init;
	sample.mod.hardware_cleanup = isp_mod_sample_hardware_cleanup;
	sample.mod.add_ctrl = isp_mod_sample_add_ctrl;
	// sample.mod.exec_actions = sample_exec_actions;
	// sample.mod.exec_actions_num = ARRAY_SIZE(sample_exec_actions);
	// sample.mod.info_actions = sample_info_actions;
	// sample.mod.info_actions_num = ARRAY_SIZE(sample_info_actions);
	// sample.mod.notify_actions = sample_notify_actions;
	// sample.mod.notify_actions_num = ARRAY_SIZE(sample_notify_actions);
	sample.mod.size = sizeof(sample);
	return isp_mod_register(&sample.mod);
}

void isp_unregister_mod_sample(void)
{
	isp_mod_unregister(&sample.mod);
}
