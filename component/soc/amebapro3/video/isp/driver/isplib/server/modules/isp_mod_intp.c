/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Kelsey Chen <kelsey_chen@realsil.com.cn>
 */

#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_hw_intp.h>
#include "isp_mod_sensor.h"
#include "isp_mod_intp.h"

struct isp_mod_intp {
	struct isp_mod mod;
	const struct isp_hw_intp_ops *hw_ops;
};

static int mod_intp_statis_done(struct isp_mod *mod, void *data)
{
	struct isp_mod_intp *intp = get_mod(mod, intp);

	return intp->hw_ops->est_per_frame(mod->owner_id);
}

static int mod_intp_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_intp *intp = get_mod(mod, intp);

	return intp->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info intp_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_intp_statis_done),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_intp_dynamic_post),
};

static int isp_mod_intp_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_intp *intp;

	intp = get_mod(mod, intp);
	intp->hw_ops = isp_hw_intp_get_ops();
	if (intp->hw_ops == NULL) {
		isp_error("get intp hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_intp_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_intp_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_intp *intp = get_mod(mod, intp);

	return intp->hw_ops->init(mod->owner_id);
}

static struct isp_mod_intp intp;

int isp_register_mod_intp(void)
{
	isp_memset(&intp, 0, sizeof(intp));
	intp.mod.id = MOD_INTP;
	intp.mod.name = "intp";
	intp.mod.init = isp_mod_intp_init;
	intp.mod.cleanup = isp_mod_intp_cleanup;
	intp.mod.hardware_init = isp_mod_intp_hardware_init;
	intp.mod.notify_actions = intp_notify_actions;
	intp.mod.notify_actions_num = ARRAY_SIZE(intp_notify_actions);
	intp.mod.size = sizeof(intp);
	return isp_mod_register(&intp.mod);
}

void isp_unregister_mod_intp(void)
{
	isp_mod_unregister(&intp.mod);
}
