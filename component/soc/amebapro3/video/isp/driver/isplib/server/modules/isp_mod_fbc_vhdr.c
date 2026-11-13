/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2024 Sally Peng <sally_peng@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_hw_fbc_vhdr.h>
#include "isp_mod_fbc_vhdr.h"

struct isp_mod_fbc_vhdr {
	struct isp_mod mod;
	const struct isp_hw_fbc_vhdr_ops *hw_ops;
	uint32_t phy_addr;
};

static void fbc_vhdr_release_memory(struct isp_mod_fbc_vhdr *fbc_vhdr)
{
	if (fbc_vhdr->phy_addr) {
		isp_os_driver_mem_free(fbc_vhdr->phy_addr);
		fbc_vhdr->phy_addr = 0;
	}
}

static int mod_fbc_vhdr_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_fbc_vhdr *fbc_vhdr = get_mod(mod, fbc_vhdr);

	return fbc_vhdr->hw_ops->estimation(mod->owner_id);
}

static int mod_fbc_vhdr_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	uint32_t length = 0;
	uint32_t frame_length = 0;
	uint32_t ae_min_ratio = 8;
	const struct isp_sensor_mode *mode = data;
	struct isp_mod_fbc_vhdr *fbc_vhdr = get_mod(mod, fbc_vhdr);

	fbc_vhdr_release_memory(fbc_vhdr);
	if (mode->hdr == RTS_ISP_HDR_LINE_2TO1 ||
		mode->hdr == RTS_ISP_HDR_DCG_2TO1) {
		const rts_isp_size_t *size = &mode->size.isp;

		frame_length = ALIGN(size->w, 16) * size->h;
		length = size->w * roundup_div(mode->min_vts, ae_min_ratio);
		length = ALIGN(length, 4096);
		ret = isp_os_driver_mem_alloc(&fbc_vhdr->phy_addr, length,
									  "HDR");
		if (ret) {
			goto out;
		}
	}
	ret = fbc_vhdr->hw_ops->set_ddr(mod->owner_id, fbc_vhdr->phy_addr,
									length, frame_length);
	if (ret) {
		goto out;
	}
out:
	if (ret) {
		fbc_vhdr_release_memory(fbc_vhdr);
	}
	return ret;
}

static struct isp_mod_action_info fbc_vhdr_notify_actions[] = {
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_fbc_vhdr_dynamic_post),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_fbc_vhdr_mode_change),
};

static int isp_mod_fbc_vhdr_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_fbc_vhdr *fbc_vhdr;

	fbc_vhdr = get_mod(mod, fbc_vhdr);
	fbc_vhdr->hw_ops = isp_hw_fbc_vhdr_get_ops();
	if (fbc_vhdr->hw_ops == NULL) {
		isp_error("get fbc_vhdr hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_fbc_vhdr_cleanup(struct isp_mod *mod)
{
	struct isp_mod_fbc_vhdr *fbc_vhdr = get_mod(mod, fbc_vhdr);

	fbc_vhdr_release_memory(fbc_vhdr);

	return RTS_ISP_OK;
}

static struct isp_mod_fbc_vhdr fbc_vhdr;

int isp_register_mod_fbc_vhdr(void)
{
	isp_memset(&fbc_vhdr, 0, sizeof(fbc_vhdr));
	fbc_vhdr.mod.id = MOD_FBC_VHDR;
	fbc_vhdr.mod.name = "fbc_vhdr";
	fbc_vhdr.mod.init = isp_mod_fbc_vhdr_init;
	fbc_vhdr.mod.cleanup = isp_mod_fbc_vhdr_cleanup;
	fbc_vhdr.mod.notify_actions = fbc_vhdr_notify_actions;
	fbc_vhdr.mod.notify_actions_num = ARRAY_SIZE(fbc_vhdr_notify_actions);
	fbc_vhdr.mod.size = sizeof(fbc_vhdr);
	return isp_mod_register(&fbc_vhdr.mod);
}

void isp_unregister_mod_fbc_vhdr(void)
{
	isp_mod_unregister(&fbc_vhdr.mod);
}
