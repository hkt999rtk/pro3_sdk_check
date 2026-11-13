/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_top.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_hw_blc.h>
#include "isp_mod_sensor.h"
#include "isp_mod_blc.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_BLC

struct isp_mod_blc {
	struct isp_mod mod;
	const struct isp_hw_blc_ops *hw_ops;

	int32_t snr_hdr; /* enum rts_isp_hdr_mode */

	const isp_iq_blc_v1_t *iq;
	isp_algo_param_blc_v1_t *algo_param;
	isp_iq_blc_v1_dyn_item_t dyn_items[3];

	isp_iq_blc_v1_dyn_item_t current;
	int force_update;

	int patch_mode;
};

static int isp_mod_blc_init(struct isp_mod *mod)
{
	struct isp_mod_blc *blc;

	blc = get_mod(mod, blc);
	blc->algo_param = isp_algo_param_get_blc_v1(mod->owner_id);
	if (!blc->algo_param) {
		isp_error("get blc algo param fail\n");
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

static int isp_mod_blc_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static void calc_smooth_blc_item(isp_iq_blc_v1_dyn_item_t *cur,
								 const isp_iq_blc_v1_dyn_item_t *target)
{
	int i;
	const int max = 16;
	const int div = 8;

	for (i = 0; i < ARRAY_SIZE(cur->r); i++) {
		cur->r[i] = calc_smooth(cur->r[i], target->r[i], div, max);
		cur->gr[i] = calc_smooth(cur->gr[i], target->gr[i], div, max);
		cur->gb[i] = calc_smooth(cur->gb[i], target->gb[i], div, max);
		cur->b[i] = calc_smooth(cur->b[i], target->b[i], div, max);
	}
}

static int blc_set_item(struct isp_mod_blc *blc)
{
	return blc->hw_ops->set(blc->mod.owner_id, &blc->current);
}

static void intp_blc_v1_item(isp_iq_blc_v1_dyn_item_t *item, int val, int th0,
							 const isp_iq_blc_v1_dyn_item_t *item0, int th1,
							 const isp_iq_blc_v1_dyn_item_t *item1)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(item->r); i++) {
		item->r[i] =
			linear_intp(val, th0, item0->r[i], th1, item1->r[i]);
		item->gr[i] =
			linear_intp(val, th0, item0->gr[i], th1, item1->gr[i]);
		item->gb[i] =
			linear_intp(val, th0, item0->gb[i], th1, item1->gb[i]);
		item->b[i] =
			linear_intp(val, th0, item0->b[i], th1, item1->b[i]);
	}
}

DEFINE_ISP_IQ_INTP_FUNC(blc, v1, intp_blc_v1_item)

static int mod_blc_exec_set_patch_mode(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	int enable = *(int32_t *)data;
	struct isp_mod_blc *blc = get_mod(mod, blc);

	ret = isp_mod_set_patch_notify(mod, enable, RTS_ISP_TRUE);
	if (!ret) {
		blc->patch_mode = enable;
	}

	return ret;
}

static int mod_blc_exec_set_v1_dyn(struct isp_mod *mod, void *data)
{
	struct isp_mod_blc *blc = get_mod(mod, blc);
	isp_iq_blc_v1_dyn_item_t *item = *(void **)data;

	if (!blc->patch_mode) {
		return -RTS_ISP_EINVAL;
	}
	blc->current = *item;
	return blc_set_item(blc);
}

static struct isp_mod_action_info blc_exec_actions[] = {
	ISP_MOD_ACTION(BLC_EXEC_SET_PATCH_MODE, mod_blc_exec_set_patch_mode),
	ISP_MOD_ACTION(BLC_EXEC_SET_V1_DYN, mod_blc_exec_set_v1_dyn),
};

static int mod_blc_mode_change(struct isp_mod *mod, void *data)
{
	int ret;
	const struct rts_isp_interface_info *info;
	struct isp_mod_blc *blc = get_mod(mod, blc);
	const struct isp_sensor_mode *mode = data;

	blc->snr_hdr = mode->hdr;
	if (blc->snr_hdr == RTS_ISP_HDR_DCG_COMBINE ||
		blc->snr_hdr == RTS_ISP_HDR_DCG_COMBINE_TM) {
		blc->hw_ops = isp_hw_blch_get_ops();
	} else {
		blc->hw_ops = isp_hw_blc_get_ops();
	}
	if (blc->hw_ops == NULL) {
		isp_error("get blc hw ops fail\n");
		return -RTS_ISP_ENOOPS;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_INTERFACE,
								   &info, sizeof(info));
	if (ret) {
		return ret;
	}

	return blc->hw_ops->set_bit_depth(mod->owner_id, info->bit_depth);
}

static int mod_blc_statis_done(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_blc *blc = get_mod(mod, blc);

	if (!blc->force_update && isp_memcmp(&blc->current, &blc->dyn_items[0],
										 sizeof(blc->current)) == 0) {
		return RTS_ISP_OK;
	}
	blc->force_update = RTS_ISP_FALSE;
	calc_smooth_blc_item(&blc->current, &blc->dyn_items[0]);

	ret =  blc_set_item(blc);
	if (ret) {
		return ret;
	}
	/* need to call estimation to set the sync reg */
	return blc->hw_ops->est_dynamic(mod->owner_id);
}

static int mod_blc_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_blc *blc = get_mod(mod, blc);
	const isp_iq_blc_v1_dyn_item_t *item;

	blc->iq = isp_iq_get_blc_v1(mod->owner_id, &blc->algo_param->common);
	if (!blc->iq) {
		return -RTS_ISP_EINVAL;
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_blc_v1_dyn_enabled(&blc->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_blc_v1_dyn_item_default(&blc->iq->dynamic.nest,
			&blc->algo_param->common);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	blc->dyn_items[0] = blc->current = *item;

	return blc_set_item(blc);
}

static int mod_blc_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_blc *blc = get_mod(mod, blc);
	struct isp_notify_dynamic *dyn = data;
	const isp_iq_blc_v1_dyn_item_t *item;

	if (!isp_iq_blc_v1_dyn_enabled(&blc->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	blc->force_update = RTS_ISP_TRUE;

	item = isp_iq_get_blc_v1_dyn_item(&blc->iq->dynamic.nest,
									  &blc->algo_param->common, dyn,
									  isp_iq_blc_v1_intp_cb, blc);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (item != blc->dyn_items) {
		blc->dyn_items[0] = *item;
	}

	return RTS_ISP_OK;
}

static int mod_blc_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_blc *blc = get_mod(mod, blc);

	return blc->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info blc_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_blc_mode_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_blc_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_blc_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_blc_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_blc_dynamic_post),
};

static int mod_blc_info_position(struct isp_mod *mod, void *data)
{
	struct isp_mod_blc *blc = get_mod(mod, blc);
	int32_t *position = data;

	return blc->hw_ops->get_position(mod->owner_id, position);
}

static struct isp_mod_action_info blc_info_actions[] = {
	ISP_MOD_ACTION(BLC_INFO_POSITION, mod_blc_info_position),
};

static struct isp_mod_blc blc;

int isp_register_mod_blc(void)
{
	isp_memset(&blc, 0, sizeof(blc));
	blc.mod.id = MOD_BLC;
	blc.mod.name = "blc";
	blc.mod.init = isp_mod_blc_init;
	blc.mod.cleanup = isp_mod_blc_cleanup;
	blc.mod.exec_actions = blc_exec_actions;
	blc.mod.exec_actions_num = ARRAY_SIZE(blc_exec_actions);
	blc.mod.notify_actions = blc_notify_actions;
	blc.mod.notify_actions_num = ARRAY_SIZE(blc_notify_actions);
	blc.mod.info_actions = blc_info_actions;
	blc.mod.info_actions_num = ARRAY_SIZE(blc_info_actions);
	blc.mod.size = sizeof(blc);
	return isp_mod_register(&blc.mod);
}

void isp_unregister_mod_blc(void)
{
	isp_mod_unregister(&blc.mod);
}
