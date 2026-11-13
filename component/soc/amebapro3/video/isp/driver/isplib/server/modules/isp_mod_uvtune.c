/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_driver.h>
#include <isp_top.h>
#include <isp_iq.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_hw_uvtune.h>
#include "isp_mod_uvtune.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_UVTUNE

struct isp_mod_uvtune {
	struct isp_mod mod;
	const struct isp_hw_uvtune_ops *hw_ops;

	const isp_iq_uvtune_v1_t *iq;
	isp_algo_param_uvtune_v1_t *algo_param;
	isp_iq_uvtune_v1_dyn_item_t dyn_items[3];

	int reinit_count;
};

static int uvtune_check_iq(struct isp_mod_uvtune *uvtune)
{
	int i;
	int j;
	const isp_iq_uvtune_v1_dyn_nest_t *nest;

	if (!uvtune->iq) {
		return -RTS_ISP_EINVAL;
	}

	nest = &uvtune->iq->dynamic.nest;
	if (nest->indexes_count > 9 * 16) {
		return -RTS_ISP_ERANGE;
	}
	for (i = 0; i < nest->indexes_count; i++) {
		if (nest->indexes[i].y_index > 8 ||
			nest->indexes[i].uv_index > 15) {
			return -RTS_ISP_EINVAL;
		}
	}
	for (i = 0; i < nest->dyns_count; i++) {
		for (j = 0; j < nest->dyns[i].items_count; j++) {
			if (nest->dyns[i].items[j].values_count !=
				nest->indexes_count) {
				return -RTS_ISP_EINVAL;
			}
		}
	}
	return RTS_ISP_OK;
}

static void uvtune_release_dyn_item(struct isp_mod_uvtune *uvtune)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(uvtune->dyn_items); i++) {
		uvtune->dyn_items[i].values_count = 0;
		isp_free(uvtune->dyn_items[i].values);
	}
}

static int uvtune_realloc_dyn_item(struct isp_mod_uvtune *uvtune)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(uvtune->dyn_items); i++) {
		uvtune->dyn_items[i].values_count =
			uvtune->iq->dynamic.nest.dyns[0].items[0].values_count;
		if (uvtune->dyn_items[i].values_count) {
			uvtune->dyn_items[i].values = isp_realloc(
											  uvtune->dyn_items[i].values,
											  uvtune->dyn_items[i].values_count *
											  sizeof(isp_iq_uvtune_v1_value_t));
			if (!uvtune->dyn_items[i].values) {
				goto err;
			}
		} else {
			isp_free(uvtune->dyn_items[i].values);
		}
	}

	return RTS_ISP_OK;
err:
	uvtune_release_dyn_item(uvtune);
	return -RTS_ISP_ENOMEM;
}

static int uvtune_reinit(struct isp_mod_uvtune *uvtune)
{
	if (isp_top_get_status() == RTS_ISP_STREAMING) {
		uvtune->hw_ops->wait_sync(uvtune->mod.owner_id);
		uvtune->reinit_count--;
	} else if (uvtune->reinit_count == 2) {
		uvtune->reinit_count--;
	}
	return uvtune->hw_ops->init(uvtune->mod.owner_id);
}

static void intp_uvtune_v1_item(isp_iq_uvtune_v1_dyn_item_t *item, int val,
								int th0,
								const isp_iq_uvtune_v1_dyn_item_t *item0,
								int th1,
								const isp_iq_uvtune_v1_dyn_item_t *item1)
{
	int i;

	for (i = 0; i < item->values_count; i++) {
		linear_intp_array(item->values[i].mats, val, th0,
						  item0->values[i].mats, th1,
						  item1->values[i].mats,
						  ARRAY_SIZE(item->values[i].mats));
		linear_intp_array(item->values[i].thds, val, th0,
						  item0->values[i].thds, th1,
						  item1->values[i].thds,
						  ARRAY_SIZE(item->values[i].thds));
	}
	linear_intp_array(item->suppress, val, th0, item0->suppress, th1,
					  item1->suppress, ARRAY_SIZE(item->suppress));
}

DEFINE_ISP_IQ_INTP_FUNC(uvtune, v1, intp_uvtune_v1_item)

static int mod_uvtune_iq_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_uvtune *uvtune = get_mod(mod, uvtune);
	const isp_iq_uvtune_v1_dyn_item_t *item;

	uvtune->iq = isp_iq_get_uvtune_v1(mod->owner_id,
									  &uvtune->algo_param->common);
	ret = uvtune_check_iq(uvtune);
	if (ret) {
		return ret;
	}
	ret = uvtune_realloc_dyn_item(uvtune);
	if (ret) {
		return ret;
	}

	uvtune->reinit_count = 2;

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_uvtune_v1_dyn_enabled(&uvtune->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_uvtune_v1_dyn_item_default(
			   &uvtune->iq->dynamic.nest, &uvtune->algo_param->common);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	ret = uvtune_reinit(uvtune);
	if (ret) {
		return ret;
	}
	return uvtune->hw_ops->set_uvtune(
			   mod->owner_id, uvtune->iq->dynamic.nest.indexes, item);
}

static int mod_uvtune_dynamic(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_uvtune *uvtune = get_mod(mod, uvtune);
	const isp_iq_uvtune_v1_dyn_item_t *item;
	const struct isp_notify_dynamic *dyn = data;

	if (!isp_iq_uvtune_v1_dyn_enabled(&uvtune->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_uvtune_v1_dyn_item(&uvtune->iq->dynamic.nest,
										 &uvtune->algo_param->common, dyn,
										 isp_iq_uvtune_v1_intp_cb, uvtune);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	if (uvtune->reinit_count) {
		ret = uvtune_reinit(uvtune);
		if (ret) {
			return ret;
		}
	}
	return uvtune->hw_ops->set_uvtune(
			   mod->owner_id, uvtune->iq->dynamic.nest.indexes, item);
}

static int mod_uvtune_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_uvtune *uvtune = get_mod(mod, uvtune);

	return uvtune->hw_ops->est_dynamic(mod->owner_id);
}

static struct isp_mod_action_info uvtune_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_uvtune_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_uvtune_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_uvtune_dynamic_post),
};

static int isp_mod_uvtune_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_uvtune *uvtune;

	uvtune = get_mod(mod, uvtune);
	uvtune->hw_ops = isp_hw_uvtune_get_ops();
	if (uvtune->hw_ops == NULL) {
		isp_error("get uvtune hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	uvtune->algo_param = isp_algo_param_get_uvtune_v1(mod->owner_id);
	if (!uvtune->algo_param) {
		isp_error("get uvtune algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_uvtune_cleanup(struct isp_mod *mod)
{
	struct isp_mod_uvtune *uvtune = get_mod(mod, uvtune);

	uvtune_release_dyn_item(uvtune);

	return RTS_ISP_OK;
}

static struct isp_mod_uvtune uvtune;

int isp_register_mod_uvtune(void)
{
	isp_memset(&uvtune, 0, sizeof(uvtune));
	uvtune.mod.id = MOD_UVTUNE;
	uvtune.mod.name = "uvtune";
	uvtune.mod.init = isp_mod_uvtune_init;
	uvtune.mod.cleanup = isp_mod_uvtune_cleanup;
	uvtune.mod.notify_actions = uvtune_notify_actions;
	uvtune.mod.notify_actions_num = ARRAY_SIZE(uvtune_notify_actions);
	uvtune.mod.size = sizeof(uvtune);
	return isp_mod_register(&uvtune.mod);
}

void isp_unregister_mod_uvtune(void)
{
	isp_mod_unregister(&uvtune.mod);
}
