/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_dpc.h>
#include "isp_mod_dpc.h"

struct isp_mod_dpc {
	struct isp_mod mod;
	const struct isp_hw_dpc_ops *hw_ops;
};

static int mod_dpc_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_dpc *dpc = get_mod(mod, dpc);

	return dpc->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info dpc_notify_actions[] = {
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_dpc_dynamic_post),
};

static int isp_mod_dpc_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_dpc *dpc;

	dpc = get_mod(mod, dpc);
	dpc->hw_ops = isp_hw_dpc_get_ops();
	if (dpc->hw_ops == NULL) {
		isp_error("get dpc hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_dpc_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static struct isp_mod_dpc dpc;

int isp_register_mod_dpc(void)
{
	isp_memset(&dpc, 0, sizeof(dpc));
	dpc.mod.id = MOD_DPC;
	dpc.mod.name = "dpc";
	dpc.mod.init = isp_mod_dpc_init;
	dpc.mod.cleanup = isp_mod_dpc_cleanup;
	dpc.mod.notify_actions = dpc_notify_actions;
	dpc.mod.notify_actions_num = ARRAY_SIZE(dpc_notify_actions);
	dpc.mod.size = sizeof(dpc);
	return isp_mod_register(&dpc.mod);
}

void isp_unregister_mod_dpc(void)
{
	isp_mod_unregister(&dpc.mod);
}
