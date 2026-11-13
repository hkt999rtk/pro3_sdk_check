/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2024 Sally Peng <sally_peng@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_fbc_tnr.h>
#include "isp_mod_fbc_tnr.h"

struct isp_mod_fbc_tnr {
	struct isp_mod mod;
	const struct isp_hw_fbc_tnr_ops *hw_ops;
	uint32_t phy_addr;
};

static void fbc_tnr_release_memory(struct isp_mod_fbc_tnr *fbc_tnr)
{
	if (fbc_tnr->phy_addr) {
		isp_os_driver_mem_free(fbc_tnr->phy_addr);
		fbc_tnr->phy_addr = 0;
	}
}

static int mod_fbc_tnr_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_fbc_tnr *fbc_tnr = get_mod(mod, fbc_tnr);

	return fbc_tnr->hw_ops->estimation(mod->owner_id);
}

static int mod_fbc_tnr_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	uint32_t length;
	const struct isp_sensor_mode *mode = data;
	struct isp_mod_fbc_tnr *fbc_tnr = get_mod(mod, fbc_tnr);

	fbc_tnr_release_memory(fbc_tnr);
	length = ALIGN(mode->size.isp.w, 16) * mode->size.isp.h;
	ret = isp_os_driver_mem_alloc(&fbc_tnr->phy_addr, length, "ISP 3dnr");
	if (ret) {
		goto out;
	}
	ret = fbc_tnr->hw_ops->set_ddr(mod->owner_id, fbc_tnr->phy_addr,
								   length);
	if (ret) {
		goto out;
	}

out:
	if (ret) {
		fbc_tnr_release_memory(fbc_tnr);
	}
	return ret;
}

static struct isp_mod_action_info fbc_tnr_notify_actions[] = {
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_fbc_tnr_dynamic_post),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_fbc_tnr_mode_change),
};

static int isp_mod_fbc_tnr_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_fbc_tnr *fbc_tnr;

	fbc_tnr = get_mod(mod, fbc_tnr);
	fbc_tnr->hw_ops = isp_hw_fbc_tnr_get_ops();
	if (fbc_tnr->hw_ops == NULL) {
		isp_error("get fbc_tnr hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_fbc_tnr_cleanup(struct isp_mod *mod)
{
	struct isp_mod_fbc_tnr *fbc_tnr = get_mod(mod, fbc_tnr);

	fbc_tnr_release_memory(fbc_tnr);

	return RTS_ISP_OK;
}

static struct isp_mod_fbc_tnr fbc_tnr;

int isp_register_mod_fbc_tnr(void)
{
	isp_memset(&fbc_tnr, 0, sizeof(fbc_tnr));
	fbc_tnr.mod.id = MOD_FBC_TNR;
	fbc_tnr.mod.name = "fbc_tnr";
	fbc_tnr.mod.init = isp_mod_fbc_tnr_init;
	fbc_tnr.mod.cleanup = isp_mod_fbc_tnr_cleanup;
	fbc_tnr.mod.notify_actions = fbc_tnr_notify_actions;
	fbc_tnr.mod.notify_actions_num = ARRAY_SIZE(fbc_tnr_notify_actions);
	fbc_tnr.mod.size = sizeof(fbc_tnr);
	return isp_mod_register(&fbc_tnr.mod);
}

void isp_unregister_mod_fbc_tnr(void)
{
	isp_mod_unregister(&fbc_tnr.mod);
}
