/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 * Copyright (C) 2019 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <isp_top.h>
#include <isp_mod.h>
#include <isp_iq.h>
#include <isp_notify.h>
#include <isp_hw_ygc.h>
#include "isp_mod_ygc.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_YGC

struct isp_mod_ygc {
	struct isp_mod mod;
	const struct isp_hw_ygc_ops *hw_ops;

	const isp_iq_ygc_v1_t *iq;
	isp_algo_param_ygc_v1_t *algo_param;
	isp_iq_ygc_v1_dyn_item_t dyn_items[3];

	isp_iq_ygc_v1_dyn_item_t current;
	int force_update;
	int patch_mode;
};

static void intp_ygc_v1_item(isp_iq_ygc_v1_dyn_item_t *item, int val, int th0,
							 const isp_iq_ygc_v1_dyn_item_t *item0, int th1,
							 const isp_iq_ygc_v1_dyn_item_t *item1)
{
	item->curve_count = item0->curve_count;
	linear_intp_array(item->curve, val, th0, item0->curve, th1,
					  item1->curve, item->curve_count);
	linear_intp_array(item->offsets, val, th0, item0->offsets, th1,
					  item1->offsets, ARRAY_SIZE(item->offsets));
}

DEFINE_ISP_IQ_INTP_FUNC(ygc, v1, intp_ygc_v1_item)

static int ygc_set_item(struct isp_mod_ygc *ygc)
{
	return ygc->hw_ops->set_ygc(ygc->mod.owner_id, &ygc->current);
}

static int mod_ygc_exec_set_patch_mode(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	int enable = *(int32_t *)data;
	struct isp_mod_ygc *ygc = get_mod(mod, ygc);

	ret = isp_mod_set_patch_notify(mod, enable, RTS_ISP_TRUE);
	if (!ret) {
		ygc->patch_mode = enable;
	}

	return ret;
}

static int mod_ygc_exec_set_v1_dyn(struct isp_mod *mod, void *data)
{
	struct isp_mod_ygc *ygc = get_mod(mod, ygc);
	isp_iq_ygc_v1_dyn_item_t *item = *(void **)data;

	if (!ygc->patch_mode) {
		return -RTS_ISP_EINVAL;
	}
	ygc->current = *item;
	return ygc_set_item(ygc);
}

static struct isp_mod_action_info ygc_exec_actions[] = {
	ISP_MOD_ACTION(YGC_EXEC_SET_PATCH_MODE, mod_ygc_exec_set_patch_mode),
	ISP_MOD_ACTION(YGC_EXEC_SET_V1_DYN, mod_ygc_exec_set_v1_dyn),
};

static int mod_ygc_statis_done(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ygc *ygc = get_mod(mod, ygc);

	if (!ygc->force_update &&
		isp_memcmp_array(ygc->current.curve, ygc->dyn_items[0].curve,
						 ygc->current.curve_count) == 0 &&
		isp_memcmp_array(ygc->current.offsets, ygc->dyn_items[0].offsets,
						 ARRAY_SIZE(ygc->current.offsets)) == 0) {
		return RTS_ISP_OK;
	}
	ygc->force_update = RTS_ISP_FALSE;
	calc_smooth_array(ygc->current.curve, ygc->dyn_items[0].curve, 8, 128,
					  ygc->current.curve_count);
	calc_smooth_array(ygc->current.offsets, ygc->dyn_items[0].offsets, 8,
					  128, ARRAY_SIZE(ygc->current.offsets));
	ret = ygc_set_item(ygc);
	if (ret) {
		return ret;
	}
	/* need to call estimation to set the sync reg */
	return ygc->hw_ops->est_dynamic(mod->owner_id);
}

static int mod_ygc_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_ygc *ygc = get_mod(mod, ygc);
	const isp_iq_ygc_v1_dyn_item_t *item;

	ygc->iq = isp_iq_get_ygc_v1(mod->owner_id, &ygc->algo_param->common);
	if (!ygc->iq) {
		return -RTS_ISP_EINVAL;
	}
	ygc->current.curve_count =
		ygc->iq->dynamic.nest.dyns[0].items[0].curve_count;

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_ygc_v1_dyn_enabled(&ygc->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_ygc_v1_dyn_item_default(&ygc->iq->dynamic.nest,
			&ygc->algo_param->common);
	if (!item) {
		return -RTS_ISP_ENOITEM;
	}
	ygc->dyn_items[0] = ygc->current = *item;

	return ygc_set_item(ygc);
}

static int mod_ygc_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_ygc *ygc = get_mod(mod, ygc);
	const struct isp_notify_dynamic *dyn = data;
	const isp_iq_ygc_v1_dyn_item_t *item;

	if (!isp_iq_ygc_v1_dyn_enabled(&ygc->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	ygc->force_update = RTS_ISP_TRUE;
	item = isp_iq_get_ygc_v1_dyn_item(&ygc->iq->dynamic.nest,
									  &ygc->algo_param->common, dyn,
									  isp_iq_ygc_v1_intp_cb, ygc);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (item != &ygc->dyn_items[0]) {
		ygc->dyn_items[0] = *item;
	}

	return RTS_ISP_OK;
}

static int mod_ygc_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_ygc *ygc = get_mod(mod, ygc);

	return ygc->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info ygc_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_ygc_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_ygc_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_ygc_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_ygc_dynamic_post),
};

static int isp_mod_ygc_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_ygc *ygc;

	ygc = get_mod(mod, ygc);
	ygc->hw_ops = isp_hw_ygc_get_ops();
	if (ygc->hw_ops == NULL) {
		isp_error("get ygc hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	ygc->algo_param = isp_algo_param_get_ygc_v1(mod->owner_id);
	if (!ygc->algo_param) {
		isp_error("get ygc algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_ygc_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static struct isp_mod_ygc ygc;

int isp_register_mod_ygc(void)
{
	isp_memset(&ygc, 0, sizeof(ygc));
	ygc.mod.id = MOD_YGC;
	ygc.mod.name = "ygc";
	ygc.mod.init = isp_mod_ygc_init;
	ygc.mod.cleanup = isp_mod_ygc_cleanup;
	ygc.mod.exec_actions = ygc_exec_actions;
	ygc.mod.exec_actions_num = ARRAY_SIZE(ygc_exec_actions);
	ygc.mod.notify_actions = ygc_notify_actions;
	ygc.mod.notify_actions_num = ARRAY_SIZE(ygc_notify_actions);
	ygc.mod.size = sizeof(ygc);
	return isp_mod_register(&ygc.mod);
}

void isp_unregister_mod_ygc(void)
{
	isp_mod_unregister(&ygc.mod);
}
