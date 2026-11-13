/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_ltm.h>
#include "isp_mod_ltm.h"

struct isp_mod_ltm {
	struct isp_mod mod;
	const struct isp_hw_ltm_ops *hw_ops;
};

static int mod_ltm_mcrop_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_ltm *ltm = get_mod(mod, ltm);
	const rts_isp_rect_t *rect = data;

	return ltm->hw_ops->set_size(mod->owner_id, rect);
}

static int mod_ltm_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_ltm *ltm = get_mod(mod, ltm);

	return ltm->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info ltm_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_ltm_mcrop_change),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_ltm_dynamic_post),
};

static int isp_mod_ltm_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_ltm *ltm;

	ltm = get_mod(mod, ltm);
	ltm->hw_ops = isp_hw_ltm_get_ops();
	if (ltm->hw_ops == NULL) {
		isp_error("get ltm hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_ltm_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_ltm_hardware_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_ltm *ltm = get_mod(mod, ltm);

	ret = ltm->hw_ops->init_vreg(mod->owner_id);
	if (ret) {
		return ret;
	}
	return ltm->hw_ops->init_ltm(mod->owner_id);
}

static struct isp_mod_ltm ltm;

int isp_register_mod_ltm(void)
{
	isp_memset(&ltm, 0, sizeof(ltm));
	ltm.mod.id = MOD_LTM;
	ltm.mod.name = "ltm";
	ltm.mod.init = isp_mod_ltm_init;
	ltm.mod.cleanup = isp_mod_ltm_cleanup;
	ltm.mod.hardware_init = isp_mod_ltm_hardware_init;
	ltm.mod.notify_actions = ltm_notify_actions;
	ltm.mod.notify_actions_num = ARRAY_SIZE(ltm_notify_actions);
	ltm.mod.size = sizeof(ltm);
	return isp_mod_register(&ltm.mod);
}

void isp_unregister_mod_ltm(void)
{
	isp_mod_unregister(&ltm.mod);
}
