/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_dpc_rnr.h>
#include "isp_mod_dpc_rnr.h"

struct isp_mod_dpc_rnr {
	struct isp_mod mod;
	const struct isp_hw_dpc_rnr_ops *hw_ops;
};

static int mod_dpc_rnr_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_dpc_rnr *dpc_rnr = get_mod(mod, dpc_rnr);

	return dpc_rnr->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info dpc_rnr_notify_actions[] = {
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_dpc_rnr_dynamic_post),
};

static int isp_mod_dpc_rnr_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_dpc_rnr *dpc_rnr;

	dpc_rnr = get_mod(mod, dpc_rnr);
	dpc_rnr->hw_ops = isp_hw_dpc_rnr_get_ops();
	if (dpc_rnr->hw_ops == NULL) {
		isp_error("get dpc_rnr hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_dpc_rnr_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static struct isp_mod_dpc_rnr dpc_rnr;

int isp_register_mod_dpc_rnr(void)
{
	isp_memset(&dpc_rnr, 0, sizeof(dpc_rnr));
	dpc_rnr.mod.id = MOD_DPC_RNR;
	dpc_rnr.mod.name = "dpc_rnr";
	dpc_rnr.mod.init = isp_mod_dpc_rnr_init;
	dpc_rnr.mod.cleanup = isp_mod_dpc_rnr_cleanup;
	dpc_rnr.mod.notify_actions = dpc_rnr_notify_actions;
	dpc_rnr.mod.notify_actions_num = ARRAY_SIZE(dpc_rnr_notify_actions);
	dpc_rnr.mod.size = sizeof(dpc_rnr);
	return isp_mod_register(&dpc_rnr.mod);
}

void isp_unregister_mod_dpc_rnr(void)
{
	isp_mod_unregister(&dpc_rnr.mod);
}
