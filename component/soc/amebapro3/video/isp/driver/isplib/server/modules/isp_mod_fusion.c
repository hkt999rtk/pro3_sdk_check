/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_fusion.h>
#include "isp_mod_fusion.h"

struct isp_mod_fusion {
	struct isp_mod mod;
	const struct isp_hw_fusion_ops *hw_ops;
};

static int mod_fusion_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_fusion *fusion = get_mod(mod, fusion);

	return fusion->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info fusion_notify_actions[] = {
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_fusion_dynamic_post),
};

static int isp_mod_fusion_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_fusion *fusion;

	fusion = get_mod(mod, fusion);
	fusion->hw_ops = isp_hw_fusion_get_ops();
	if (fusion->hw_ops == NULL) {
		isp_error("get fusion hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_fusion_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_fusion_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_fusion *fusion = get_mod(mod, fusion);

	return fusion->hw_ops->init(mod->owner_id);
}

static struct isp_mod_fusion fusion;

int isp_register_mod_fusion(void)
{
	isp_memset(&fusion, 0, sizeof(fusion));
	fusion.mod.id = MOD_FUSION;
	fusion.mod.name = "fusion";
	fusion.mod.init = isp_mod_fusion_init;
	fusion.mod.cleanup = isp_mod_fusion_cleanup;
	fusion.mod.hardware_init = isp_mod_fusion_hardware_init;
	fusion.mod.notify_actions = fusion_notify_actions;
	fusion.mod.notify_actions_num = ARRAY_SIZE(fusion_notify_actions);
	fusion.mod.size = sizeof(fusion);
	return isp_mod_register(&fusion.mod);
}

void isp_unregister_mod_fusion(void)
{
	isp_mod_unregister(&fusion.mod);
}
