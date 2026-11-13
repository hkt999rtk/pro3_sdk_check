/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_lce.h>
#include "isp_mod_lce.h"

struct isp_mod_lce {
	struct isp_mod mod;
	const struct isp_hw_lce_ops *hw_ops;
};

static int mod_lce_mcrop_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_lce *lce = get_mod(mod, lce);
	const rts_isp_rect_t *rect = data;

	return lce->hw_ops->set_size(mod->owner_id, rect);
}

static struct isp_mod_action_info lce_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_lce_mcrop_change),
};

static int isp_mod_lce_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_lce *lce;

	lce = get_mod(mod, lce);
	lce->hw_ops = isp_hw_lce_get_ops();
	if (lce->hw_ops == NULL) {
		isp_error("get lce hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_lce_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_lce_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_lce *lce = get_mod(mod, lce);

	return lce->hw_ops->init_vreg(mod->owner_id);
}

static struct isp_mod_lce lce;

int isp_register_mod_lce(void)
{
	isp_memset(&lce, 0, sizeof(lce));
	lce.mod.id = MOD_LCE;
	lce.mod.name = "lce";
	lce.mod.init = isp_mod_lce_init;
	lce.mod.cleanup = isp_mod_lce_cleanup;
	lce.mod.hardware_init = isp_mod_lce_hardware_init;
	lce.mod.notify_actions = lce_notify_actions;
	lce.mod.notify_actions_num = ARRAY_SIZE(lce_notify_actions);
	lce.mod.size = sizeof(lce);
	return isp_mod_register(&lce.mod);
}

void isp_unregister_mod_lce(void)
{
	isp_mod_unregister(&lce.mod);
}
