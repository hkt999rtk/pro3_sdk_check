/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2018 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <pb_encode.h>
#include <pb_decode.h>
#include <rts_isp.h>
#include <rts_isp_tuning.h>
#include <rts_isp_patch.h>
#include <isp_iq_table.pb.h>
#include <isp_define.h>
#include <isp_top.h>
#include <isp_poll.h>
#include <isp_core.h>
#include <isp_iq.h>
#include <isp_mod.h>
#include <isp_msg_buf.h>
#include <isp_notify.h>
#include <isp_v4l2_ctrl.h>
#include <isp_tracker.h>
#include <isp_hw_iq.h>
#include "isp_mod_vctrl.h"
#include "isp_mod_ae.h"
#include "isp_mod_awb.h"
#include "isp_mod_af.h"
#include "isp_mod_sensor.h"
#include "isp_mod_iq.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_IQ

#define DYNAMIC_IQ_TIMER_INTERVAL 100

#define iq_get_addr(addr) \
	(((addr) & ISP_IQ_REG_ADDR_MASK) >> ISP_IQ_REG_ADDR_OFFSET)
#define iq_get_bitoff(addr) \
	(((addr) & ISP_IQ_REG_BITOFF_MASK) >> ISP_IQ_REG_BITOFF_OFFSET)
#define iq_get_bitnum(addr) \
	(((addr) & ISP_IQ_REG_BITNUM_MASK) >> ISP_IQ_REG_BITNUM_OFFSET)

struct isp_mod_iq {
	struct isp_mod mod;
	struct isp_iq *man;
	struct {
		struct v4l2_ctrl *day_night_ctrl;
		struct v4l2_ctrl *iq_ctrl;
		const char *iq_menu[_MAX_ISP_IQ_NUM];
	};
	struct v4l2_ctrl *dyn_iq_ctrl;
	struct v4l2_ctrl *nr_level_ctrl;
	struct v4l2_ctrl *de_level_ctrl;

	const isp_iq_texture_v1_t *iq;
	isp_algo_param_texture_v1_t *algo_param;
	int nest_index;
	isp_algo_param_t *algo_param_all;

	isp_timer_handle_t timer;
	struct isp_notify_dynamic dynamic;
	struct isp_notify_dynamic notified_dynamic;

	const struct isp_hw_iq_ops *hw_ops;

	const struct rts_isp_sensor_patch_ops *patch_ops;
};

static int apply_iq_table(struct isp_mod_iq *iq, uint32_t day_night,
						  uint32_t target_id);

static int mod_iq_info_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	*(struct isp_notify_dynamic **)data = &iq->dynamic;
	return RTS_ISP_OK;
}

static int mod_iq_info_packed_iq(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_mod_iq_packed_iq_info *info = data;

	info->version = iq->man->packed_version;
	info->cus_version = iq->man->cus_version;
	info->timestamp = iq->man->timestamp;

	return RTS_ISP_OK;
}

static int mod_iq_info_plugins_version(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct rts_isp_plugins_version *version = data;

	isp_memset(version, 0, sizeof(*version));
	ret = isp_mod_sibling_get_info(mod, MOD_AE, AE_INFO_VERSION,
								   &version->ae_api_version,
								   sizeof(version->ae_api_version));
	if (ret) {
		return ret;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_AWB, AWB_INFO_VERSION,
								   &version->awb_api_version,
								   sizeof(version->awb_api_version));
	if (ret) {
		return ret;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_AF, AF_INFO_VERSION,
								   &version->af_api_version,
								   sizeof(version->af_api_version));
	if (ret) {
		return ret;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_VERSION,
								   &version->sensor_api_version,
								   sizeof(version->sensor_api_version));
	if (ret) {
		return ret;
	}
	if (iq->man && iq->man->iq_table) {
		version->iq_pack_version = iq->man->packed_version;
		version->iq_proto_version = iq->man->iq_table->header.version;
		version->iq_customer_version = iq->man->cus_version;
	} else {
		version->iq_pack_version = 0;
		version->iq_proto_version = 0;
		version->iq_customer_version = 0;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_action_info iq_info_actions[] = {
	ISP_MOD_ACTION(IQ_INFO_DYNAMIC, mod_iq_info_dynamic),
	ISP_MOD_ACTION(IQ_INFO_PACKED_IQ, mod_iq_info_packed_iq),
	ISP_MOD_ACTION(IQ_INFO_PLUGINS_VERSION, mod_iq_info_plugins_version),
};

static void iq_reset_notified_value(struct isp_mod_iq *iq)
{
	isp_memset(&iq->notified_dynamic, 0, sizeof(iq->notified_dynamic));
}

static int check_texture_v1_common_iq(const isp_iq_texture_v1_common_t *common)
{
	int i;

	for (i = 0; i < common->items_count; i++) {
		if (common->items[i].offsets_count == 0) {
			return -RTS_ISP_EINVAL;
		} else if (common->items[i].offsets_count == 1) {
			if (common->items[i].values_count == 0) {
				return -RTS_ISP_EINVAL;
			}
		} else {
			if (common->items[i].offsets_count !=
				common->items[i].values_count) {
				return -RTS_ISP_EINVAL;
			}
		}
	}
	return RTS_ISP_OK;
}

static int
check_texture_v1_dynamic_iq(const isp_iq_texture_v1_dynamic_t *dynamic)
{
	int i;
	int j;
	int k;
	const isp_iq_texture_v1_dyn_nest_t *nest;
	const isp_iq_texture_v1_dyn_t *dyn;
	const isp_iq_texture_v1_dyn_item_t *item;

	for (i = 0; i < dynamic->nests_count; i++) {
		nest = &dynamic->nests[i];
		for (j = 0; j < nest->dyns_count; j++) {
			dyn = &nest->dyns[j];
			if (nest->offsets_count == 0) {
				return -RTS_ISP_EINVAL;
			} else if (nest->offsets_count == 1) {
				if (dyn->items[0].values_count == 0) {
					return -RTS_ISP_EINVAL;
				}
				for (k = 1; k < dyn->items_count; k++) {
					item = &dyn->items[k];
					if (item->values_count !=
						dyn->items[0].values_count) {
						return -RTS_ISP_ERANGE;
					}
				}
			} else {
				for (k = 0; k < dyn->items_count; k++) {
					item = &dyn->items[k];
					if (item->values_count !=
						nest->offsets_count) {
						return -RTS_ISP_ERANGE;
					}
				}
			}
		}
	}
	return RTS_ISP_OK;
}

static void iq_dynamic_notify_callback(int ret, void *priv)
{
	struct isp_mod_iq *iq = priv;

	if (ret) {
		isp_perror(ret, "iq notify dynamic fail");
	}

	iq->notified_dynamic = iq->dynamic;
}

static int iq_dynamic_notify(struct isp_mod_iq *iq, uint32_t target_id)
{
	int ret = RTS_ISP_OK;

	if (!iq->dyn_iq_ctrl->val) {
		return RTS_ISP_OK;
	}

	if (!isp_memcmp(&iq->dynamic, &iq->notified_dynamic,
					sizeof(iq->dynamic))) {
		return RTS_ISP_OK;
	}

	time_tracker_begin("dyn_notify");
	if (target_id) {
		ret = isp_core_notify_mod_sched(
				  iq->mod.owner, target_id, ISP_NOTIFY_DYNAMIC,
				  &iq->dynamic, sizeof(iq->dynamic), NULL, NULL);
	} else {
		ret = isp_core_notify_sched(iq->mod.owner, ISP_NOTIFY_DYNAMIC,
									&iq->dynamic, sizeof(iq->dynamic),
									iq_dynamic_notify_callback, iq);
	}
	time_tracker_end("dyn_notify");
	if (ret) {
		isp_perror(ret, "iq notify dynamic fail");
	}
	return ret;
}

static int iq_timer_callback(isp_timer_handle_t *timer, void *data)
{
	struct isp_mod *mod = data;
	struct isp_mod_iq *iq = get_mod(mod, iq);
	int ret = RTS_ISP_OK;

	if (iq->dyn_iq_ctrl->cur.val) {
		isp_top_lock_internal();
		ret = iq_dynamic_notify(iq, 0);
		isp_top_unlock_internal();
	}
	return ret;
}

static uint32_t iq_read_reg_mask(struct isp_mod_iq *iq, uint32_t offset,
								 uint32_t mask)
{
	return iq->hw_ops->read_reg_mask(iq->mod.owner_id, offset, mask);
}

static void iq_write_reg_mask(struct isp_mod_iq *iq, uint32_t value,
							  uint32_t offset, uint32_t mask)
{
	iq->hw_ops->write_reg_mask(iq->mod.owner_id, value, offset, mask);
}

static void iq_write_texture_reg(struct isp_mod_iq *iq, uint32_t value,
								 uint32_t offset)
{
	if (offset & ISP_IQ_REG_DESC_MASK) {
		uint32_t addr = iq_get_addr(offset);
		uint32_t bitnum = iq_get_bitnum(offset);
		uint32_t bitoff = iq_get_bitoff(offset);

		iq_write_reg_mask(iq, value << bitoff, addr,
						  ((1 << bitnum) - 1) << bitoff);
	} else {
		iq_write_reg_mask(iq, value, offset, 0);
	}
}

static int iq_process_texture_common(struct isp_mod_iq *iq)
{
	int i;
	int j;
	const isp_iq_texture_v1_common_t *common;

	common = &iq->iq->common;
	for (i = 0; i < common->items_count; i++) {
		const isp_iq_texture_v1_common_item_t *item = &common->items[i];
		const int cont_mode = item->offsets_count == 1;

		if (cont_mode) {
			uint32_t base = item->offsets[0] & ISP_IQ_REG_ADDR_MASK;
			uint32_t mask = item->offsets[0] & ISP_IQ_REG_DESC_MASK;

			for (j = 0; j < item->values_count; j++)
				iq_write_texture_reg(iq, item->values[j],
									 (base + j * 4) | mask);
		} else {
			for (j = 0; j < item->values_count; j++)
				iq_write_texture_reg(iq, item->values[j],
									 item->offsets[j]);
		}
	}

	return RTS_ISP_OK;
}

static void
process_texture_dyn_nest_default(const isp_iq_texture_v1_dyn_nest_t *nest,
								 isp_algo_param_common_t *common,
								 struct isp_mod_iq *iq)
{
	uint32_t base;
	uint32_t mask;
	isp_iq_texture_v1_dyn_t *dyn;
	isp_iq_texture_v1_dyn_item_t *item;
	const int cont_mode = nest->offsets_count == 1;

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_texture_v1_dyn_enabled(nest)) {
		return;
	}

	if (cont_mode) {
		base = nest->offsets[0] & ISP_IQ_REG_ADDR_MASK;
		mask = nest->offsets[0] & ISP_IQ_REG_DESC_MASK;
	}
	dyn = &nest->dyns[nest->bounds.def_index];
	item = &dyn->items[dyn->bounds.def_index];
	for (int i = 0; i < item->values_count; i++) {
		uint32_t offset;

		if (cont_mode) {
			offset = (base + 4 * i) | mask;
		} else {
			offset = nest->offsets[i];
		}
		iq_write_texture_reg(iq, item->values[i], offset);
	}
}

static int iq_process_texture_dynamic_default(struct isp_mod_iq *iq)
{
	int i;
	const isp_iq_texture_v1_dynamic_t *iq_dynamic;

	iq_dynamic = &iq->iq->dynamic;
	for (i = 0; i < iq_dynamic->nests_count; i++)
		process_texture_dyn_nest_default(&iq_dynamic->nests[i],
										 &iq->algo_param->nests[i], iq);

	return RTS_ISP_OK;
}

static const isp_iq_texture_v1_dyn_item_t *
isp_iq_texture_v1_intp_cb(int dyn_vals[3], int thds[6],
						  const isp_iq_texture_v1_dyn_item_t *items[4],
						  int changed[3], void *priv)
{
	uint32_t base;
	uint32_t mask;
	int values[3];
	struct isp_mod_iq *iq = priv;
	const isp_iq_texture_v1_dyn_nest_t *nest =
		&iq->iq->dynamic.nests[iq->nest_index];
	const int cont_mode = nest->offsets_count == 1;

	if (!changed[0] && !changed[1] && !changed[2]) {
		return NULL;
	}

	if (cont_mode) {
		base = nest->offsets[0] & ISP_IQ_REG_ADDR_MASK;
		mask = nest->offsets[0] & ISP_IQ_REG_DESC_MASK;
	}
	for (int i = 0; i < nest->dyns[0].items[0].values_count; i++) {
		uint32_t offset;

		if (cont_mode) {
			offset = (base + 4 * i) | mask;
		} else {
			offset = nest->offsets[i];
		}

		if (items[0] && items[1])
			values[1] = linear_intp(dyn_vals[1], thds[2],
									items[0]->values[i], thds[3],
									items[1]->values[i]);
		else {
			values[1] = items[0]->values[i];
		}

		if (items[2] && items[3])
			values[2] = linear_intp(dyn_vals[2], thds[4],
									items[2]->values[i], thds[5],
									items[3]->values[i]);
		else if (items[2]) {
			values[2] = items[2]->values[i];
		} else {
			values[2] = 0;
		}

		if (items[0] && items[2])
			values[0] = linear_intp(dyn_vals[0], thds[0], values[1],
									thds[1], values[2]);
		else {
			values[0] = values[1];
		}

		iq_write_texture_reg(iq, values[0], offset);
	}
	return NULL;
}

static void process_texture_dyn_nest(const isp_iq_texture_v1_dyn_nest_t *nest,
									 const struct isp_notify_dynamic *dynamic,
									 isp_algo_param_common_t *common,
									 struct isp_mod_iq *iq)
{
	if (!isp_iq_texture_v1_dyn_enabled(nest)) {
		return;
	}
	isp_iq_get_texture_v1_dyn_item(nest, common, dynamic,
								   isp_iq_texture_v1_intp_cb, iq);
}

static int iq_process_texture_dynamic(struct isp_mod_iq *iq,
									  const struct isp_notify_dynamic *dynamic)
{
	int i;
	const isp_iq_texture_v1_dynamic_t *iq_dynamic;

	iq_dynamic = &iq->iq->dynamic;
	for (i = 0; i < iq_dynamic->nests_count; i++) {
		iq->nest_index = i;
		process_texture_dyn_nest(&iq_dynamic->nests[i], dynamic,
								 &iq->algo_param->nests[i], iq);
	}

	return RTS_ISP_OK;
}

static int iq_apply_callback(void *priv)
{
	struct isp_mod_iq *iq = priv;

	return apply_iq_table(iq, iq->day_night_ctrl->cur.val, 0);
}

static int iq_reload_packed_iq(struct isp_mod_iq *iq)
{
	int i;
	int ret;
	struct isp_iq_ctrl_info info;

	ret = isp_iq_reload(iq->man, iq_apply_callback, iq);
	if (ret) {
		return ret;
	}

	ret = isp_iq_get_ctrl_info(iq->man, &info);
	if (ret) {
		return ret;
	}
	iq->iq_ctrl->val = info.id;
	iq->iq_ctrl->cur.val = info.id;
	iq->iq_ctrl->menu_skip_mask = ~info.entry_bits &
								  ((1 << _MAX_ISP_IQ_NUM) - 1);
	for (i = 0; i < _MAX_ISP_IQ_NUM; i++) {
		iq->iq_menu[i] = info.names[i];
	}

	return RTS_ISP_OK;
}

static int iq_sensor_update(struct isp_mod_iq *iq)
{
	return isp_mod_sibling_get_info(&iq->mod, MOD_SENSOR,
									SENSOR_INFO_PATCH_OPS, &iq->patch_ops,
									sizeof(iq->patch_ops));
}

static int mod_iq_start_pre(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	ret = iq_dynamic_notify(iq, 0);
	if (ret) {
		return ret;
	}
	return isp_timer_start(&iq->timer, DYNAMIC_IQ_TIMER_INTERVAL,
						   DYNAMIC_IQ_TIMER_INTERVAL);
}

static int mod_iq_stop_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	iq_reset_notified_value(iq);
	return isp_timer_stop(&iq->timer);
}

static int mod_iq_mode_change(struct isp_mod *mod, void *data)
{
	const struct isp_sensor_mode *mode = data;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	iq->dynamic.ae.mode = iq->notified_dynamic.ae.mode = mode->hdr;

	return isp_iq_set_mode(iq->man, mode->hdr);
}

static int mod_iq_iq_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	iq->iq = isp_iq_get_texture_v1(mod->owner_id, &iq->algo_param->common);
	if (!iq->iq) {
		return -RTS_ISP_ENOITEM;
	}
	if (check_texture_v1_common_iq(&iq->iq->common) ||
		check_texture_v1_dynamic_iq(&iq->iq->dynamic)) {
		return -RTS_ISP_EINVAL;
	}
	if (iq->iq->dynamic.nests_count > ARRAY_SIZE(iq->algo_param->nests)) {
		return -RTS_ISP_ERANGE;
	}
	iq->algo_param->nests_count = iq->iq->dynamic.nests_count;

	ret = iq_process_texture_common(iq);
	if (ret) {
		return ret;
	}
	return iq_process_texture_dynamic_default(iq);
}

static int mod_iq_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	return iq_process_texture_dynamic(iq, data);
}

static int mod_iq_dynamic_patch(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	if (iq->patch_ops) {
		return iq->patch_ops->dynamic(iq->mod.owner_id, &iq->dynamic);
	}

	return RTS_ISP_OK;
}

static int mod_iq_plugin_update(struct isp_mod *mod, void *data)
{
	uint32_t notify_data = *(uint32_t *)data;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	if (notify_data != RTS_ISP_PLUG_TYPE_IQ &&
		notify_data != RTS_ISP_PLUG_TYPE_SENSOR) {
		return RTS_ISP_OK;
	}
	if (notify_data == RTS_ISP_PLUG_TYPE_SENSOR) {
		return iq_sensor_update(iq);
	}
	return iq_reload_packed_iq(iq);
}

static struct isp_mod_action_info iq_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_START_PRE, mod_iq_start_pre),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STOP_POST, mod_iq_stop_post),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MODE_CHANGE, mod_iq_mode_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_iq_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_iq_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_PATCH_PRIORITY,
						   mod_iq_dynamic_patch),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_PLUGIN_UPDATE, mod_iq_plugin_update),
};

static inline void set_ptr_value(uint32_t *ptr, uint32_t value)
{
	if (ptr) {
		*ptr = value;
	}
}

static int iq_get_type_info(struct isp_mod *mod, int32_t type,
							const pb_msgdesc_t **fields, void **iq_ptr,
							uint32_t *target_id, uint32_t *size)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_iq *man = iq->man;

	switch (type) {
	case RTS_ISP_TUNING_IQ_TABLE_ALL:
		*fields = isp_iq_table_t_fields;
		*iq_ptr = man->iq_table;
		set_ptr_value(target_id, 0);
		set_ptr_value(size, sizeof(*man->iq_table));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_BLC:
		*fields = isp_iq_blc_entries_t_fields;
		*iq_ptr = &man->iq_table->blc;
		set_ptr_value(target_id, MOD_BLC);
		set_ptr_value(size, sizeof(man->iq_table->blc));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_NLSC:
		*fields = isp_iq_nlsc_entries_t_fields;
		*iq_ptr = &man->iq_table->nlsc;
		set_ptr_value(target_id, MOD_NLSC);
		set_ptr_value(size, sizeof(man->iq_table->nlsc));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_MLSC:
		*fields = isp_iq_mlsc_entries_t_fields;
		*iq_ptr = &man->iq_table->mlsc;
		set_ptr_value(target_id, MOD_MLSC);
		set_ptr_value(size, sizeof(man->iq_table->mlsc));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_CCM:
		*fields = isp_iq_ccm_entries_t_fields;
		*iq_ptr = &man->iq_table->ccm;
		set_ptr_value(target_id, MOD_CCM);
		set_ptr_value(size, sizeof(man->iq_table->ccm));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_GAMMA:
		*fields = isp_iq_gamma_entries_t_fields;
		*iq_ptr = &man->iq_table->gamma;
		set_ptr_value(target_id, MOD_GAMMA);
		set_ptr_value(size, sizeof(man->iq_table->gamma));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_YGC:
		*fields = isp_iq_ygc_entries_t_fields;
		*iq_ptr = &man->iq_table->ygc;
		set_ptr_value(target_id, MOD_YGC);
		set_ptr_value(size, sizeof(man->iq_table->ygc));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_UVTUNE:
		*fields = isp_iq_uvtune_entries_t_fields;
		*iq_ptr = &man->iq_table->uvtune;
		set_ptr_value(target_id, MOD_UVTUNE);
		set_ptr_value(size, sizeof(man->iq_table->uvtune));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_SPE:
		*fields = isp_iq_spe_entries_t_fields;
		*iq_ptr = &man->iq_table->spe;
		set_ptr_value(target_id, MOD_SPE);
		set_ptr_value(size, sizeof(man->iq_table->spe));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_YGAMMA:
		*fields = isp_iq_ygamma_entries_t_fields;
		*iq_ptr = &man->iq_table->ygamma;
		set_ptr_value(target_id, MOD_YGAMMA);
		set_ptr_value(size, sizeof(man->iq_table->ygamma));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_EEH:
		*fields = isp_iq_eeh_entries_t_fields;
		*iq_ptr = &man->iq_table->eeh;
		set_ptr_value(target_id, MOD_EEH);
		set_ptr_value(size, sizeof(man->iq_table->eeh));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_TEXTURE:
		*fields = isp_iq_texture_entries_t_fields;
		*iq_ptr = &man->iq_table->texture;
		set_ptr_value(target_id, MOD_IQ);
		set_ptr_value(size, sizeof(man->iq_table->texture));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_AE:
		*fields = isp_iq_ae_entries_t_fields;
		*iq_ptr = &man->iq_table->ae;
		set_ptr_value(target_id, MOD_AE);
		set_ptr_value(size, sizeof(man->iq_table->ae));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_AWB:
		*fields = isp_iq_awb_entries_t_fields;
		*iq_ptr = &man->iq_table->awb;
		set_ptr_value(target_id, MOD_AWB);
		set_ptr_value(size, sizeof(man->iq_table->awb));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_AF:
		*fields = isp_iq_af_entries_t_fields;
		*iq_ptr = &man->iq_table->af;
		set_ptr_value(target_id, MOD_AF);
		set_ptr_value(size, sizeof(man->iq_table->af));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_FLICK:
		*fields = isp_iq_flick_entries_t_fields;
		*iq_ptr = &man->iq_table->flick;
		set_ptr_value(target_id, MOD_FLICK);
		set_ptr_value(size, sizeof(man->iq_table->flick));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_WDR:
		*fields = isp_iq_wdr_entries_t_fields;
		*iq_ptr = &man->iq_table->wdr;
		set_ptr_value(target_id, MOD_WDR);
		set_ptr_value(size, sizeof(man->iq_table->wdr));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_DAYNIGHT:
		*fields = isp_iq_daynight_entries_t_fields;
		*iq_ptr = &man->iq_table->daynight;
		set_ptr_value(target_id, MOD_RAWSTAT);
		set_ptr_value(size, sizeof(man->iq_table->daynight));
		break;
	case RTS_ISP_TUNING_IQ_TABLE_HIGH_TEMP:
		*fields = isp_iq_high_temp_entries_t_fields;
		*iq_ptr = &man->iq_table->high_temp;
		set_ptr_value(target_id, MOD_SENSOR);
		set_ptr_value(size, sizeof(man->iq_table->high_temp));
		break;
	default:
		return -RTS_ISP_ENOITEM;
	}
	return RTS_ISP_OK;
}

static int apply_iq_table(struct isp_mod_iq *iq, uint32_t day_night,
						  uint32_t target_id)
{
	int ret;

	if (target_id) {
		struct isp_notify_dynamic backup;

		ret = isp_core_notify_mod_sched(iq->mod.owner, target_id,
										ISP_NOTIFY_IQ_CHANGE, NULL, 0,
										NULL, NULL);
		if (ret) {
			goto out;
		}
		backup = iq->notified_dynamic;
		iq_reset_notified_value(iq);
		ret = iq_dynamic_notify(iq, target_id);
		iq->notified_dynamic = backup;
	} else {
		ret = isp_core_notify_sched(iq->mod.owner, ISP_NOTIFY_IQ_CHANGE,
									NULL, 0, NULL, NULL);
		if (ret) {
			goto out;
		}
		iq_reset_notified_value(iq);
		ret = iq_dynamic_notify(iq, 0);
	}
out:
	if (ret) {
		isp_perror(ret, "iq change notify fail");
	}
	return ret;
}

static int iq_get_prepare(struct isp_mod *mod, int32_t type,
						  const pb_msgdesc_t **fields, void **iq_ptr)
{
	int ret;
	void *ptr;

	ret = iq_get_type_info(mod, type, fields, &ptr, NULL, NULL);
	if (ret) {
		return ret;
	}
	*iq_ptr = ptr;

	return RTS_ISP_OK;
}

static int iq_set_prepare(struct isp_mod *mod, int32_t type,
						  const pb_msgdesc_t **fields, void **iq_ptr,
						  uint32_t *target_id)
{
	int ret;
	void *ptr;
	uint32_t size;

	ret = iq_get_type_info(mod, type, fields, &ptr, target_id, &size);
	if (ret) {
		return ret;
	}
	pb_release(*fields, ptr);
	isp_memset(ptr, 0, size);
	*iq_ptr = ptr;

	return RTS_ISP_OK;
}

static void iq_set_unprepare(struct isp_mod *mod, int32_t type, int retval)
{
	int ret;
	void *ptr;
	uint32_t mod_id;
	uint32_t size;
	const pb_msgdesc_t *fields;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	if (!retval) {
		return;
	}
	ret = iq_get_type_info(mod, type, &fields, &ptr, &mod_id, &size);
	if (ret) {
		return;
	}
	isp_error("set iq fail\n");
	pb_release(fields, ptr);
	apply_iq_table(iq, iq->day_night_ctrl->cur.val, mod_id);
}

static int mod_iq_exec_set_dynamic_ae(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_notify_dynamic_ae *dyn_ae;

	dyn_ae = data;

	if (iq->dynamic.ae.mode != dyn_ae->mode) {
		int ret;

		ret = isp_iq_set_mode(iq->man, dyn_ae->mode);
		if (ret) {
			return ret;
		}
		ret = isp_core_notify_sched(mod->owner, ISP_NOTIFY_IQ_CHANGE,
									NULL, 0, NULL, NULL);
		if (ret) {
			return ret;
		}
	}
	iq->dynamic.ae = *dyn_ae;

	return RTS_ISP_OK;
}

static int mod_iq_exec_set_dynamic_awb(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	iq->dynamic.awb = *(struct isp_notify_dynamic_awb *)data;

	return RTS_ISP_OK;
}

static int mod_iq_exec_set_dynamic_snr(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_notify_dynamic_sensor *dyn_snr;

	dyn_snr = data;

	if (iq->dynamic.sensor.high_temp_en != dyn_snr->high_temp_en) {
		int ret;

		ret = isp_iq_set_high_temp(iq->man, dyn_snr->high_temp_en);
		if (ret) {
			return ret;
		}
		ret = isp_core_notify_sched(mod->owner, ISP_NOTIFY_IQ_CHANGE,
									NULL, 0, NULL, NULL);
		if (ret) {
			return ret;
		}
	}
	iq->dynamic.sensor = *dyn_snr;

	return RTS_ISP_OK;
}

static int mod_iq_exec_get_algo_param(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);
	isp_algo_param_t *algo_param_all = iq->algo_param_all;
	struct isp_msg_buf *msg = *(void **)data;
	struct isp_mod_iq_tuning_buffer *algo_param = msg->data;
	pb_ostream_t stream;
	const pb_msgdesc_t *fields;
	const void *src_struct;
	void *buf;
	uint32_t buf_max_len;

	algo_param->len = 0;
	buf = algo_param + 1;
	buf_max_len = isp_msg_data_capacity(&msg->hdr) - sizeof(*algo_param);

	stream = pb_ostream_from_buffer(buf, buf_max_len);
	switch (algo_param->type) {
	case RTS_ISP_TUNING_PARAM_ALL:
		fields = isp_algo_param_t_fields;
		src_struct = algo_param_all;
		break;
	case RTS_ISP_TUNING_PARAM_BLC:
		fields = isp_algo_param_blc_t_fields;
		src_struct = &algo_param_all->blc;
		break;
	case RTS_ISP_TUNING_PARAM_NLSC:
		fields = isp_algo_param_nlsc_t_fields;
		src_struct = &algo_param_all->nlsc;
		break;
	case RTS_ISP_TUNING_PARAM_MLSC:
		fields = isp_algo_param_mlsc_t_fields;
		src_struct = &algo_param_all->mlsc;
		break;
	case RTS_ISP_TUNING_PARAM_CCM:
		fields = isp_algo_param_ccm_t_fields;
		src_struct = &algo_param_all->ccm;
		break;
	case RTS_ISP_TUNING_PARAM_GAMMA:
		fields = isp_algo_param_gamma_t_fields;
		src_struct = &algo_param_all->gamma;
		break;
	case RTS_ISP_TUNING_PARAM_YGC:
		fields = isp_algo_param_ygc_t_fields;
		src_struct = &algo_param_all->ygc;
		break;
	case RTS_ISP_TUNING_PARAM_UVTUNE:
		fields = isp_algo_param_uvtune_t_fields;
		src_struct = &algo_param_all->uvtune;
		break;
	case RTS_ISP_TUNING_PARAM_SPE:
		fields = isp_algo_param_spe_t_fields;
		src_struct = &algo_param_all->spe;
		break;
	case RTS_ISP_TUNING_PARAM_YGAMMA:
		fields = isp_algo_param_ygamma_t_fields;
		src_struct = &algo_param_all->ygamma;
		break;
	case RTS_ISP_TUNING_PARAM_EEH:
		fields = isp_algo_param_eeh_t_fields;
		src_struct = &algo_param_all->eeh;
		break;
	case RTS_ISP_TUNING_PARAM_TEXTURE:
		fields = isp_algo_param_texture_t_fields;
		src_struct = &algo_param_all->texture;
		break;
	case RTS_ISP_TUNING_PARAM_AE:
		fields = isp_algo_param_ae_t_fields;
		src_struct = &algo_param_all->ae;
		break;
	case RTS_ISP_TUNING_PARAM_AWB:
		fields = isp_algo_param_awb_t_fields;
		src_struct = &algo_param_all->awb;
		break;
	case RTS_ISP_TUNING_PARAM_AF:
		fields = isp_algo_param_af_t_fields;
		src_struct = &algo_param_all->af;
		break;
	case RTS_ISP_TUNING_PARAM_FLICK:
		fields = isp_algo_param_flick_t_fields;
		src_struct = &algo_param_all->flick;
		break;
	case RTS_ISP_TUNING_PARAM_WDR:
		fields = isp_algo_param_wdr_t_fields;
		src_struct = &algo_param_all->wdr;
		break;
	case RTS_ISP_TUNING_PARAM_DAYNIGHT:
		fields = isp_algo_param_daynight_t_fields;
		src_struct = &algo_param_all->daynight;
		break;
	case RTS_ISP_TUNING_PARAM_HIGH_TEMP:
		fields = isp_algo_param_high_temp_t_fields;
		src_struct = &algo_param_all->high_temp;
		break;
	default:
		ret = -RTS_ISP_EINVAL;
		goto out;
	}

	if (!pb_encode(&stream, fields, src_struct)) {
		isp_error("encode param error: %s\n", PB_GET_ERROR(&stream));
		ret = -RTS_ISP_EPROTOBUF;
		goto out;
	}
	algo_param->len = stream.bytes_written;
	ret = RTS_ISP_OK;
out:
	isp_msg_to_resp(msg, ret,
					sizeof(msg->hdr) + sizeof(*algo_param) +
					algo_param->len);
	return RTS_ISP_OK;
}

static int mod_iq_exec_set_algo_param(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);
	isp_algo_param_t *algo_param_all = iq->algo_param_all;
	struct isp_msg_buf *msg = *(void **)data;
	struct isp_mod_iq_tuning_buffer *algo_param = msg->data;
	pb_istream_t stream;
	const pb_msgdesc_t *fields;
	void *dst_struct;
	void *buf;

	if (sizeof(*algo_param) + algo_param->len >
		isp_msg_data_len(&msg->hdr)) {
		ret = -RTS_ISP_ERANGE;
		goto out;
	}
	buf = algo_param + 1;
	stream = pb_istream_from_buffer(buf, algo_param->len);
	switch (algo_param->type) {
	case RTS_ISP_TUNING_PARAM_ALL:
		fields = isp_algo_param_t_fields;
		dst_struct = algo_param_all;
		break;
	case RTS_ISP_TUNING_PARAM_AE:
		fields = isp_algo_param_ae_t_fields;
		dst_struct = &algo_param_all->ae;
		break;
	case RTS_ISP_TUNING_PARAM_AWB:
		fields = isp_algo_param_awb_t_fields;
		dst_struct = &algo_param_all->awb;
		break;
	case RTS_ISP_TUNING_PARAM_AF:
		fields = isp_algo_param_af_t_fields;
		dst_struct = &algo_param_all->af;
		break;
	case RTS_ISP_TUNING_PARAM_FLICK:
		fields = isp_algo_param_flick_t_fields;
		dst_struct = &algo_param_all->flick;
		break;
	case RTS_ISP_TUNING_PARAM_WDR:
		fields = isp_algo_param_wdr_t_fields;
		dst_struct = &algo_param_all->wdr;
		break;
	default:
		ret = -RTS_ISP_EINVAL;
		goto out;
	}

	if (!pb_decode_noinit(&stream, fields, dst_struct)) {
		isp_error("decode param error: %s\n", PB_GET_ERROR(&stream));
		ret = -RTS_ISP_EPROTOBUF;
		goto out;
	}
	ret = RTS_ISP_OK;
out:
	isp_msg_to_resp(msg, ret, sizeof(msg->hdr));
	return RTS_ISP_OK;
}

static int mod_iq_exec_get_iq_table(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_msg_buf *msg = *(void **)data;
	struct isp_mod_iq_tuning_buffer *iq_table = msg->data;
	pb_ostream_t stream;
	const pb_msgdesc_t *fields = NULL;
	void *src_struct = NULL;
	void *buf;
	uint32_t buf_max_len;

	iq_table->len = 0;
	ret = iq_get_prepare(mod, iq_table->type, &fields, &src_struct);
	if (ret) {
		goto out;
	}

	buf = iq_table + 1;
	buf_max_len = isp_msg_data_capacity(&msg->hdr) - sizeof(*iq_table);
	stream = pb_ostream_from_buffer(buf, buf_max_len);
	if (!pb_encode(&stream, fields, src_struct)) {
		isp_error("encode iq error: %s\n", PB_GET_ERROR(&stream));
		ret = -RTS_ISP_EPROTOBUF;
		goto out;
	}
	iq_table->len = stream.bytes_written;
out:
	isp_msg_to_resp(msg, ret,
					sizeof(msg->hdr) + sizeof(*iq_table) + iq_table->len);
	return RTS_ISP_OK;
}

static int mod_iq_exec_set_iq_table(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_msg_buf *msg = *(void **)data;
	struct isp_mod_iq_tuning_buffer *iq_table = msg->data;
	pb_istream_t stream;
	const pb_msgdesc_t *fields = NULL;
	void *dst_struct = NULL;
	uint32_t mod_id = 0;
	void *buf;

	if (sizeof(*iq_table) + iq_table->len > isp_msg_data_len(&msg->hdr)) {
		ret = -RTS_ISP_ERANGE;
		goto out;
	}
	ret = iq_set_prepare(mod, iq_table->type, &fields, &dst_struct,
						 &mod_id);
	if (ret) {
		goto out;
	}

	buf = iq_table + 1;
	stream = pb_istream_from_buffer(buf, iq_table->len);
	if (!pb_decode(&stream, fields, dst_struct)) {
		isp_error("decode iq error: %s\n", PB_GET_ERROR(&stream));
		ret = -RTS_ISP_EPROTOBUF;
		goto unprepare_out;
	}
	ret = apply_iq_table(iq, iq->day_night_ctrl->cur.val, mod_id);
unprepare_out:
	iq_set_unprepare(mod, iq_table->type, ret);
out:
	isp_msg_to_resp(msg, ret, sizeof(msg->hdr));
	return RTS_ISP_OK;
}

static int mod_iq_read_write_regs(struct isp_mod *mod, void *data)
{
	int i;
	int ret;
	void *buf;
	void *end;
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_msg_buf *msg = *(void **)data;
	uint32_t msg_len = sizeof(msg->hdr);
	struct rts_isp_tuning_reg *regs;
	struct isp_mod_iq_tuning_regs *buf_tuning_regs;

	buf = msg->data;
	end = buf + isp_msg_data_len(&msg->hdr);

	ret = get_buf_ptr(buf_tuning_regs, buf, end);
	if (ret) {
		goto out;
	}
	if (!buf_tuning_regs->num) {
		ret = -RTS_ISP_EINVAL;
		goto out;
	}
	ret = get_buf_array_ptr(regs, buf_tuning_regs->num, buf, end);
	if (ret) {
		goto out;
	}
	if (msg->hdr.action == IQ_EXEC_READ_REGS) {
		for (i = 0; i < buf_tuning_regs->num; i++)
			regs[i].value = iq_read_reg_mask(iq, regs[i].offset,
											 regs[i].mask);
		msg_len = msg->hdr.msg_len;
	} else {
		for (i = 0; i < buf_tuning_regs->num; i++)
			iq_write_reg_mask(iq, regs[i].value, regs[i].offset,
							  regs[i].mask);
		ret = isp_core_notify_priority(mod->owner,
									   ISP_DYNAMIC_POST_PRIORITY,
									   ISP_NOTIFY_DYNAMIC, &iq->dynamic,
									   sizeof(iq->dynamic));
		msg_len = sizeof(msg->hdr);
	}
out:
	isp_msg_to_resp(msg, ret, msg_len);
	return RTS_ISP_OK;
}

static int mod_iq_exec_read_regs(struct isp_mod *mod, void *data)
{
	return mod_iq_read_write_regs(mod, data);
}

static int mod_iq_exec_write_regs(struct isp_mod *mod, void *data)
{
	return mod_iq_read_write_regs(mod, data);
}

static int mod_iq_exec_reload_packed_iq(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	return iq_reload_packed_iq(iq);
}

static int mod_iq_exec_get_scene(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	*(int32_t *)data = iq->man->scene;

	return RTS_ISP_OK;
}

static int mod_iq_exec_set_scene(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	ret = isp_iq_set_scene(iq->man, *(int32_t *)data);
	if (ret) {
		return ret;
	}
	return apply_iq_table(iq, iq->day_night_ctrl->cur.val, 0);
}

static int mod_iq_exec_apply_iq(struct isp_mod *mod, void *data)
{
	uint32_t mod_id = *(uint32_t *)data;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	return apply_iq_table(iq, iq->day_night_ctrl->cur.val, mod_id);
}

static int mod_iq_exec_read_write_reg(struct isp_mod *mod, void *data)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct isp_mod_iq_read_write_reg *reg = data;

	if (reg->type == ISP_MOD_IQ_READ_REG) {
		reg->value = iq_read_reg_mask(iq, reg->offset, reg->mask);
	} else {
		iq_write_reg_mask(iq, reg->value, reg->offset, reg->mask);
	}

	return RTS_ISP_OK;
}

static struct isp_mod_action_info iq_exec_actions[] = {
	ISP_MOD_ACTION(IQ_EXEC_SET_DYNAMIC_AE, mod_iq_exec_set_dynamic_ae),
	ISP_MOD_ACTION(IQ_EXEC_SET_DYNAMIC_AWB, mod_iq_exec_set_dynamic_awb),
	ISP_MOD_ACTION(IQ_EXEC_GET_ALGO_PARAM, mod_iq_exec_get_algo_param),
	ISP_MOD_ACTION(IQ_EXEC_SET_ALGO_PARAM, mod_iq_exec_set_algo_param),
	ISP_MOD_ACTION(IQ_EXEC_GET_IQ_TABLE, mod_iq_exec_get_iq_table),
	ISP_MOD_ACTION(IQ_EXEC_SET_IQ_TABLE, mod_iq_exec_set_iq_table),
	ISP_MOD_ACTION(IQ_EXEC_SET_DYNAMIC_SENSOR, mod_iq_exec_set_dynamic_snr),
	ISP_MOD_ACTION(IQ_EXEC_READ_REGS, mod_iq_exec_read_regs),
	ISP_MOD_ACTION(IQ_EXEC_WRITE_REGS, mod_iq_exec_write_regs),
	ISP_MOD_ACTION(IQ_EXEC_RELOAD_PACKED_IQ, mod_iq_exec_reload_packed_iq),
	ISP_MOD_ACTION(IQ_EXEC_GET_SCENE, mod_iq_exec_get_scene),
	ISP_MOD_ACTION(IQ_EXEC_SET_SCENE, mod_iq_exec_set_scene),
	ISP_MOD_ACTION(IQ_EXEC_APPLY_IQ, mod_iq_exec_apply_iq),
	ISP_MOD_ACTION(IQ_EXEC_READ_WRITE_REG, mod_iq_exec_read_write_reg),
};

static int iq_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret = RTS_ISP_OK;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_iq *iq = get_mod(mod, iq);

	if (ctrl->val == ctrl->cur.val) { /* called by ctrl setup */
		return RTS_ISP_OK;
	}
	switch (ctrl->id) {
	case ISP_V4L2_CID_DAY_NIGHT:
		ret = isp_iq_change_day_night(iq->man, ctrl->val);
		if (ret) {
			return ret;
		}
		ret = apply_iq_table(iq, iq->day_night_ctrl->val, 0);
		if (ret) {
			isp_error("change day night fail, roll back to old\n");
			isp_iq_change_day_night(iq->man, ctrl->cur.val);
			apply_iq_table(iq, iq->day_night_ctrl->val, 0);
		}
		break;
	case ISP_V4L2_CID_IQ_TABLE:
		ret = isp_iq_change_table(iq->man, ctrl->val);
		ret = apply_iq_table(iq, iq->day_night_ctrl->val, 0);
		if (ret) {
			isp_error("change iq table fail, roll back to old\n");
			isp_iq_change_table(iq->man, ctrl->cur.val);
			apply_iq_table(iq, iq->day_night_ctrl->val, 0);
		}
		break;
	case ISP_V4L2_CID_DYNAMIC_IQ:
		if (!iq->dyn_iq_ctrl->val) {
			break;
		}
		iq_reset_notified_value(iq);
		ret = iq_dynamic_notify(iq, 0);
		break;
	default:
		ret = -RTS_ISP_ECTRL;
	}

	return ret;
}

static const struct v4l2_ctrl_ops iq_ctrl_ops = {
	.s_ctrl = iq_s_ctrl,
};

static int isp_mod_iq_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_iq *iq;

	iq = get_mod(mod, iq);
	iq->man = isp_core_get_iq(mod->owner);
	if (!iq->man) {
		ret = -RTS_ISP_ENOTREADY;
		goto err;
	}
	iq->hw_ops = isp_hw_iq_get_ops();
	if (!iq->hw_ops) {
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	ret = isp_mod_sibling_get_info(mod, MOD_SENSOR, SENSOR_INFO_PATCH_OPS,
								   &iq->patch_ops, sizeof(iq->patch_ops));
	if (ret) {
		goto err;
	}
	iq->algo_param_all = isp_iq_get_algo_param(mod->owner_id);
	if (!iq->algo_param_all) {
		isp_error("get algo param all fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	iq->algo_param = isp_algo_param_get_texture_v1(mod->owner_id);
	if (!iq->algo_param) {
		isp_error("get texture algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	iq->dynamic.pre = &iq->notified_dynamic;
	iq->notified_dynamic.pre = &iq->notified_dynamic;
	ret = isp_timer_init(&iq->timer, isp_top_get_poll(), iq_timer_callback,
						 mod);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	isp_perror(ret, "mod iq init fail");
	return ret;
}

static int isp_mod_iq_cleanup(struct isp_mod *mod)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);

	isp_timer_cleanup(&iq->timer);
	return RTS_ISP_OK;
}

static int isp_mod_iq_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_iq *iq = get_mod(mod, iq);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config dn_config = {
		.ops = &iq_ctrl_ops,
		.id = ISP_V4L2_CID_DAY_NIGHT,
		.name = "Night mode",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
	};
	int i;

	for (i = 0; i < _MAX_ISP_IQ_NUM; i++) {
		iq->iq_menu[i] = "None";
	}

	struct v4l2_ctrl_config iq_config = {
		.ops = &iq_ctrl_ops,
		.id = ISP_V4L2_CID_IQ_TABLE,
		.name = "IQ table menu",
		.type = ISP_V4L2_CTRL_TYPE_MENU,
		.max = ARRAY_SIZE(iq->iq_menu) - 1,
		.qmenu = iq->iq_menu,
		.def = 0,
	};
	const struct v4l2_ctrl_config dyn_iq_config = {
		.ops = &iq_ctrl_ops,
		.id = ISP_V4L2_CID_DYNAMIC_IQ,
		.name = "IQ dynamic global enable",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 1,
	};

	iq->day_night_ctrl = v4l2_ctrl_new_custom(handler, &dn_config, mod);
	iq->iq_ctrl = v4l2_ctrl_new_custom(handler, &iq_config, mod);
	iq->dyn_iq_ctrl = v4l2_ctrl_new_custom(handler, &dyn_iq_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add iq ctrls fail");
		return handler->error;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_iq iq;

int isp_register_mod_iq(void)
{
	isp_memset(&iq, 0, sizeof(iq));
	iq.mod.id = MOD_IQ;
	iq.mod.name = "iq";
	iq.mod.init = isp_mod_iq_init;
	iq.mod.cleanup = isp_mod_iq_cleanup;
	iq.mod.add_ctrl = isp_mod_iq_add_ctrl;
	iq.mod.info_actions = iq_info_actions;
	iq.mod.info_actions_num = ARRAY_SIZE(iq_info_actions);
	iq.mod.notify_actions = iq_notify_actions;
	iq.mod.notify_actions_num = ARRAY_SIZE(iq_notify_actions);
	iq.mod.exec_actions = iq_exec_actions;
	iq.mod.exec_actions_num = ARRAY_SIZE(iq_exec_actions);
	iq.mod.size = sizeof(iq);
	return isp_mod_register(&iq.mod);
}

void isp_unregister_mod_iq(void)
{
	isp_mod_unregister(&iq.mod);
}
