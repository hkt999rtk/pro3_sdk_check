/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_awb.h>
#include <isp_v4l2_ctrl.h>
#include <isp_statis.h>
#include <isp_core.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_algo.h>
#include <isp_hw_awb.h>
#include "isp_mod_iq.h"
#include "isp_mod_awb.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_AWB
#define awb_algo_ctrl(awb, ...) \
	awb->awb_algo->ctrl(awb->mod.owner_id, ##__VA_ARGS__)

struct isp_mod_awb {
	struct isp_mod mod;
	const struct rts_isp_awb_algo *awb_algo;
	const struct isp_hw_awb_ops *hw_ops;

	struct isp_awb_hw_info hw_info;
	uint16_t pixels_per_cell;
	uint32_t fine_pixels;

	struct rts_isp_awb_gain gain_cur;
	struct rts_isp_awb_gain gain_tar;
	uint32_t force_run;
	uint32_t color_temp;
	uint32_t day_night;

	int32_t mode; /* enum rts_isp_awb_mode */
	struct {
		uint16_t r_gain_adj;
		uint16_t b_gain_adj;
	} _auto_setting;
	uint32_t auto_setting_has_set;
	struct rts_isp_awb_rb_gain_adj gain_adj;
	struct rts_isp_awb_weight_table weight_table;

	struct {
		struct v4l2_ctrl *awb_ctrl;
		struct v4l2_ctrl *red_ctrl;
		struct v4l2_ctrl *green_ctrl;
		struct v4l2_ctrl *blue_ctrl;
		struct v4l2_ctrl *color_temp_ctrl;
	};

	struct isp_mod *mod_iq;
	const isp_iq_awb_v1_t *iq;
	isp_algo_param_awb_v1_t *algo_param;
	isp_iq_awb_v1_dyn_item_t dyn_items[3];
};

static int check_awb_algo_version(uint32_t version)
{
	if (algo_magic(version) != AWB_API_VERSION_MAGIC ||
		algo_major_version(version) != AWB_API_MAJOR_VERSION ||
		algo_minor_version(version) > AWB_API_MINOR_VERSION) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_awb(const struct rts_isp_awb_algo *awb_algo)
{
	if (!awb_algo || check_awb_algo_version(awb_algo->api_version)) {
		return -RTS_ISP_EINVAL;
	}
	if (!awb_algo->init || !awb_algo->cleanup) {
		return -RTS_ISP_EINVAL;
	}
	if (!awb_algo->run || !awb_algo->ctrl) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_awb_iq(const isp_iq_awb_v1_t *iq)
{
	int i, j;
	int ct_block_num;
	const isp_iq_awb_v1_dyn_t *dyn;
	const isp_iq_awb_v1_dyn_item_t *item;

	if (!iq) {
		return -RTS_ISP_EINVAL;
	}

	if (iq->def_r_gain < 256 || iq->def_r_gain > 2047 ||
		iq->def_g_gain < 256 || iq->def_g_gain > 2047 ||
		iq->def_b_gain < 256 || iq->def_b_gain > 2047) {
		return -RTS_ISP_ERANGE;
	}

	ct_block_num = iq->ct_block_num;
	for (i = 0; i < iq->dynamic.nest.dyns_count; i++) {
		dyn = &iq->dynamic.nest.dyns[i];
		for (j = 0; j < dyn->items_count; j++) {
			item = &dyn->items[j];
			if (item->normal_weight.w_table_count != ct_block_num ||
				item->normal_weight.w_table_count >
				AWB_WEIGHT_TABLE_COUNT_MAX) {
				return -RTS_ISP_EINVAL;
			}
			if (item->normal_weight.g_table_count != ct_block_num ||
				item->normal_weight.g_table_count >
				AWB_WEIGHT_TABLE_COUNT_MAX) {
				return -RTS_ISP_EINVAL;
			}
		}
	}

	return RTS_ISP_OK;
}

static int awb_calc_pixels(struct isp_mod_awb *awb, const rts_isp_grid_t *grid)
{
	awb->pixels_per_cell = grid->cell.w * grid->cell.h /
						   awb->hw_info.cell_sample.w /
						   awb->hw_info.cell_sample.h;
	awb->fine_pixels =
		awb->pixels_per_cell * grid->size.cols * grid->size.rows;
	return RTS_ISP_OK;
}

static int mod_awb_info_statis(struct isp_mod *mod, void *data)
{
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct isp_mod_awb_stat_info *info = data;

	info->info.win = awb->hw_info.grid_info.size;
	info->info.pixels_per_cell = awb->pixels_per_cell;
	info->info.fine_pixels = awb->fine_pixels;
	info->rgby_size = awb->hw_info.rgby_size;
	info->rgbg_size = awb->hw_info.rgbg_size;
	info->pixels_size = awb->hw_info.pixels_size;

	return RTS_ISP_OK;
}

static int mod_awb_info_version(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_awb *awb = get_mod(mod, awb);
	const struct rts_isp_awb_algo *algo;
	uint16_t *version = data;

	if (awb->awb_algo) {
		*version = awb->awb_algo->api_version & AWB_VERSION_MASK;
		return RTS_ISP_OK;
	}
	ret = isp_algo_get_awb(mod->owner_id, &algo);
	if (ret) {
		return ret;
	}
	*version = algo->api_version & AWB_VERSION_MASK;

	return RTS_ISP_OK;
}

static struct isp_mod_action_info awb_info_actions[] = {
	ISP_MOD_ACTION(AWB_INFO_STATIS, mod_awb_info_statis),
	ISP_MOD_ACTION(AWB_INFO_VERSION, mod_awb_info_version),
};

static int mod_awb_exec_get_setting(struct isp_mod *mod, void *data)
{
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct rts_isp_awb_setting *setting = data;

	setting->mode = awb->mode;
	setting->_temperature.temperature =
		v4l2_ctrl_g_ctrl(awb->color_temp_ctrl);
	setting->_auto.r_gain_adj = awb->_auto_setting.r_gain_adj;
	setting->_auto.b_gain_adj = awb->_auto_setting.b_gain_adj;
	setting->_component.r_gain = v4l2_ctrl_g_ctrl(awb->red_ctrl);
	setting->_component.g_gain = v4l2_ctrl_g_ctrl(awb->green_ctrl);
	setting->_component.b_gain = v4l2_ctrl_g_ctrl(awb->blue_ctrl);

	return RTS_ISP_OK;
}

static int mod_awb_exec_set_setting(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct rts_isp_awb_setting *setting = data;

	if (setting->mode < RTS_ISP_AWB_TEMPERATURE_MODE ||
		setting->mode > RTS_ISP_AWB_COMPONENT_MODE) {
		ret = -RTS_ISP_EINVAL;
		goto out;
	}

	ret = v4l2_ctrl_s_ctrl(awb->awb_ctrl,
						   setting->mode == RTS_ISP_AWB_AUTO_MODE);
	if (ret) {
		goto out;
	}
	awb->mode = setting->mode;

	switch (setting->mode) {
	case RTS_ISP_AWB_TEMPERATURE_MODE:
		ret = v4l2_ctrl_s_ctrl(awb->color_temp_ctrl,
							   setting->_temperature.temperature);
		if (ret) {
			goto out;
		}
		break;
	case RTS_ISP_AWB_AUTO_MODE:
		if (awb->awb_algo) {
			awb->gain_adj.r_gain_adj = setting->_auto.r_gain_adj;
			awb->gain_adj.b_gain_adj = setting->_auto.b_gain_adj;
			ret = awb_algo_ctrl(awb, SET_GAIN_ADJ, awb->gain_adj);
			if (ret) {
				goto out;
			}
		}
		awb->_auto_setting.r_gain_adj = setting->_auto.r_gain_adj;
		awb->_auto_setting.b_gain_adj = setting->_auto.b_gain_adj;
		awb->auto_setting_has_set = RTS_ISP_TRUE;
		break;
	case RTS_ISP_AWB_COMPONENT_MODE:
		ret = v4l2_ctrl_s_ctrl(awb->red_ctrl,
							   setting->_component.r_gain);
		if (ret) {
			goto out;
		}
		ret = v4l2_ctrl_s_ctrl(awb->green_ctrl,
							   setting->_component.g_gain);
		if (ret) {
			goto out;
		}
		ret = v4l2_ctrl_s_ctrl(awb->blue_ctrl,
							   setting->_component.b_gain);
		if (ret) {
			goto out;
		}
		break;
	}

	ret = mod_awb_exec_get_setting(mod, setting);
out:
	if (ret) {
		isp_perror(ret, "set awb ctrl fail");
	}
	return ret;
}

static int mod_awb_exec_set_ae_ratio(struct isp_mod *mod, void *data)
{
	uint32_t ratio = *(uint16_t *)data;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	if (awb->awb_algo) {
		return awb_algo_ctrl(awb, SET_AE_RATIO, ratio);
	}

	return RTS_ISP_OK;
}

static struct isp_mod_action_info awb_exec_actions[] = {
	ISP_MOD_ACTION(AWB_EXEC_GET_SETTING, mod_awb_exec_get_setting),
	ISP_MOD_ACTION(AWB_EXEC_SET_SETTING, mod_awb_exec_set_setting),
	ISP_MOD_ACTION(AWB_EXEC_SET_AE_RATIO, mod_awb_exec_set_ae_ratio),
};

static int mod_awb_set_gain(struct isp_mod *mod)
{
	const int div = 8, max = 16;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	if (!memcmp(&awb->gain_cur, &awb->gain_tar, sizeof(awb->gain_cur))) {
		return RTS_ISP_OK;
	}

	if (awb->force_run) {
		awb->force_run--;
		awb->gain_cur = awb->gain_tar;
	} else {
		awb->gain_cur.r_gain = calc_smooth(awb->gain_cur.r_gain,
										   awb->gain_tar.r_gain, div, max);
		awb->gain_cur.gr_gain = calc_smooth(awb->gain_cur.gr_gain,
											awb->gain_tar.gr_gain, div, max);
		awb->gain_cur.gb_gain = calc_smooth(awb->gain_cur.gb_gain,
											awb->gain_tar.gb_gain, div, max);
		awb->gain_cur.b_gain = calc_smooth(awb->gain_cur.b_gain,
										   awb->gain_tar.b_gain, div, max);
	}

	return awb->hw_ops->set_final_gain(mod->owner_id, &awb->gain_cur);
}

static int mod_awb_set_result_dynamic(struct isp_mod *mod,
									  const struct rts_isp_awb_gain_result *result)
{
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct isp_notify_dynamic_awb dyn_awb;

	awb->color_temp = result->color_temperature;
	awb->gain_tar = result->gain;

	dyn_awb.color_temp = awb->color_temp;
	dyn_awb.r_gain = result->gain.r_gain / 256.0f;
	dyn_awb.g_gain = (result->gain.gr_gain + result->gain.gb_gain) / 512.0f;
	dyn_awb.b_gain = result->gain.b_gain / 256.0f;

	return isp_mod_execute(awb->mod_iq, IQ_EXEC_SET_DYNAMIC_AWB,
						   &dyn_awb, sizeof(dyn_awb));
}

static void clear_awb_result_need_update_flag(struct rts_isp_awb_result *result)
{
	result->result.need_update = RTS_ISP_FALSE;
	result->config.rough_gain.need_update = RTS_ISP_FALSE;
	result->config.rough_limit.need_update = RTS_ISP_FALSE;
	result->config.fine_limit.need_update = RTS_ISP_FALSE;
	result->config.illums.need_update = RTS_ISP_FALSE;
}

static int mod_awb_statis_done(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	debug("awb statis done handling\n");
	if (awb->day_night == RTS_ISP_NIGHT && !awb->iq->enable) {
		return RTS_ISP_OK;
	}

	if (awb->awb_algo) {
		const struct rts_isp_awb_statis *statis;
		struct rts_isp_awb_result result;

		clear_awb_result_need_update_flag(&result);
		statis = &((const struct rts_isp_statis *)data)->awb;

		ret = awb->awb_algo->run(mod->owner_id, statis, &result);
		if (ret) {
			goto out;
		}
		if (result.result.need_update) {
			ret = mod_awb_set_result_dynamic(mod, &result.result);
			if (ret) {
				return ret;
			}
		}
		ret = mod_awb_set_gain(mod);
		if (ret) {
			return ret;
		}

		ret = awb->hw_ops->set_statis_config(mod->owner_id,
											 &result.config);
	}
out:
	if (ret) {
		isp_perror(ret, "awb statis done handle fail");
	}
	return ret;
}

static int mod_awb_cleanup_algo(struct isp_mod *mod)
{
	struct isp_mod_awb *awb = get_mod(mod, awb);

	if (awb->awb_algo) {
		awb->awb_algo->cleanup(mod->owner_id);
		awb->awb_algo = NULL;
	}
	awb->force_run = 0;
	return RTS_ISP_OK;
}

static int mod_awb_update_awb_info(struct isp_mod *mod,
								   struct rts_isp_awb_info *info,
								   struct rts_isp_awb_iq_info *iq,
								   struct rts_isp_awb_param_info *param)
{
	const isp_iq_header_t *iq_header;
	const isp_algo_param_header_t *param_header;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	isp_memset(info, 0, sizeof(*info));
	isp_memset(iq, 0, sizeof(*iq));
	isp_memset(param, 0, sizeof(*param));
	info->stat_info.win = awb->hw_info.grid_info.size;
	info->stat_info.pixels_per_cell = awb->pixels_per_cell;
	info->stat_info.fine_pixels = awb->fine_pixels;

	if ((awb->awb_algo->api_version & AWB_VERSION_MASK) >= 0x00000101) {
		iq_header = isp_iq_get_header(awb->mod.owner_id);
		param_header = isp_iq_get_algo_param_header(awb->mod.owner_id);
		if (!iq_header || !param_header) {
			return -RTS_ISP_EINVAL;
		}

		iq->magic = iq_header->magic;
		iq->version = iq_header->version;
		iq->iq = awb->iq;
		info->iq = iq;
		param->magic = param_header->magic;
		param->version = param_header->version;
		param->algo_param = awb->algo_param;
		info->algo_param = param;
	} else {
		info->iq = awb->iq;
		info->algo_param = awb->algo_param;
	}

	return RTS_ISP_OK;
}

static int mod_awb_init_algo(struct isp_mod *mod)
{
	int ret = RTS_ISP_OK;
	struct rts_isp_awb_info info;
	struct rts_isp_awb_iq_info iq_info;
	struct rts_isp_awb_param_info param_info;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	awb->algo_param = isp_algo_param_get_awb_v1(mod->owner_id);
	if (!awb->algo_param) {
		ret = -RTS_ISP_EINVAL;
		isp_perror(ret, "get awb algo param fail");
		return ret;
	}

	ret = mod_awb_update_awb_info(mod, &info, &iq_info, &param_info);
	if (ret) {
		return ret;
	}
	awb->force_run++;

	return awb->awb_algo->init(mod->owner_id, &info);
}

static int mod_awb_update_algo(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	mod_awb_cleanup_algo(mod);
	ret = isp_algo_get_awb(mod->owner_id, &awb->awb_algo);
	if (ret) {
		goto err;
	}
	ret = check_awb(awb->awb_algo);
	if (ret) {
		goto err;
	}
	ret = mod_awb_init_algo(mod);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;
err:
	isp_perror(ret, "update awb algo fail");
	mod_awb_cleanup_algo(mod);
	return ret;
}

static int mod_awb_plugin_update(struct isp_mod *mod, void *data)
{
	uint32_t notify_data;

	notify_data = *(uint32_t *)data;
	if (notify_data != RTS_ISP_PLUG_TYPE_ALGO_AWB) {
		return RTS_ISP_OK;
	}
	return mod_awb_update_algo(mod);
}

static int awb_set_default_dyn_item(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_awb *awb = get_mod(mod, awb);
	const isp_iq_awb_v1_dyn_item_t *item;

	if (awb->awb_algo) {
		item = isp_iq_get_awb_v1_dyn_item_default(
				   &awb->iq->dynamic.nest, &awb->algo_param->common);
		if (!item) {
			return -RTS_ISP_EINVAL;
		}
		awb->weight_table.w_table_count =
			item->normal_weight.w_table_count;
		awb->weight_table.g_table_count =
			item->normal_weight.g_table_count;
		isp_memcpy_array(awb->weight_table.w_table,
						 item->normal_weight.w_table,
						 item->normal_weight.w_table_count);
		isp_memcpy_array(awb->weight_table.g_table,
						 item->normal_weight.g_table,
						 item->normal_weight.g_table_count);
		ret = awb_algo_ctrl(awb, SET_WEIGHT_TABLE, &awb->weight_table);
		if (ret) {
			return ret;
		}

		if (awb->auto_setting_has_set) {
			awb->gain_adj.r_gain_adj =
				awb->_auto_setting.r_gain_adj;
			awb->gain_adj.b_gain_adj =
				awb->_auto_setting.b_gain_adj;
		} else {
			awb->gain_adj.r_gain_adj = item->adj.r_gain;
			awb->gain_adj.b_gain_adj = item->adj.b_gain;
		}
		ret = awb_algo_ctrl(awb, SET_GAIN_ADJ, awb->gain_adj);
		if (ret) {
			return ret;
		}
	}
	return RTS_ISP_OK;
}

static int mod_awb_iq_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	awb->iq = isp_iq_get_awb_v1(mod->owner_id, &awb->algo_param->common);
	if (check_awb_iq(awb->iq)) {
		isp_error("get awb iq fail\n");
		return -RTS_ISP_ENOITEM;
	}
	ret = mod_awb_update_algo(mod);
	if (ret) {
		return ret;
	}

	awb->day_night = isp_iq_is_night(mod->owner_id);
	if (awb->day_night == RTS_ISP_NIGHT && !awb->iq->enable) {
		struct rts_isp_awb_gain awb_gain;

		awb_gain.r_gain = 256;
		awb_gain.gr_gain = 256;
		awb_gain.gb_gain = 256;
		awb_gain.b_gain = 256;
		return awb->hw_ops->set_final_gain(mod->owner_id, &awb_gain);
	}

	/* first iq load, update ctrl to match iq table */
	if (awb->awb_ctrl->flags & ISP_V4L2_CTRL_FLAG_READ_ONLY) {
		awb->awb_ctrl->flags &= ~ISP_V4L2_CTRL_FLAG_READ_ONLY;
		/* set auto at first, because auto may change manual ctrl*/
		v4l2_ctrl_s_ctrl(awb->awb_ctrl, awb->iq->enable);
		awb->red_ctrl->val = awb->iq->def_r_gain;
		awb->blue_ctrl->val = awb->iq->def_b_gain;
		awb->green_ctrl->val = awb->iq->def_g_gain;
		awb->red_ctrl->cur.val = awb->iq->def_r_gain;
		awb->blue_ctrl->cur.val = awb->iq->def_b_gain;
		awb->green_ctrl->cur.val = awb->iq->def_g_gain;
	}

	return awb_set_default_dyn_item(mod);
}

static void intp_awb_v1_item(isp_iq_awb_v1_dyn_item_t *item, int val, int th0,
							 const isp_iq_awb_v1_dyn_item_t *item0, int th1,
							 const isp_iq_awb_v1_dyn_item_t *item1)
{
	item->adj.r_gain = linear_intp_bound(val, th0, item0->adj.r_gain, th1,
										 item1->adj.r_gain);
	item->adj.b_gain = linear_intp_bound(val, th0, item0->adj.b_gain, th1,
										 item1->adj.b_gain);

	item->normal_weight.w_table_count = item0->normal_weight.w_table_count;
	item->normal_weight.g_table_count = item0->normal_weight.g_table_count;
	linear_intp_array(item->normal_weight.w_table, val,
					  th0, item0->normal_weight.w_table,
					  th1, item1->normal_weight.w_table,
					  item->normal_weight.w_table_count);
	linear_intp_array(item->normal_weight.g_table, val,
					  th0, item0->normal_weight.g_table,
					  th1, item1->normal_weight.g_table,
					  item->normal_weight.g_table_count);
}

DEFINE_ISP_IQ_INTP_FUNC(awb, v1, intp_awb_v1_item)

static int mod_awb_dynamic(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct rts_isp_awb_weight_table table;
	const isp_iq_awb_v1_dyn_item_t *item;
	const struct isp_notify_dynamic *dyn = data;

	if (!awb->awb_algo) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_awb_v1_dyn_item(&awb->iq->dynamic.nest,
									  &awb->algo_param->common, dyn,
									  isp_iq_awb_v1_intp_cb, awb);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	isp_memset(&table, 0, sizeof(table));
	table.w_table_count = item->normal_weight.w_table_count;
	table.g_table_count = item->normal_weight.g_table_count;
	isp_memcpy_array(table.w_table, item->normal_weight.w_table,
					 item->normal_weight.w_table_count);
	isp_memcpy_array(table.g_table, item->normal_weight.g_table,
					 item->normal_weight.g_table_count);

	if (isp_memcmp(&table, &awb->weight_table, sizeof(table))) {
		ret = awb_algo_ctrl(awb, SET_WEIGHT_TABLE, &table);
		if (ret) {
			return ret;
		}
		awb->weight_table = table;
	}

	if (awb->auto_setting_has_set) {
		return RTS_ISP_OK;
	}

	if (awb->gain_adj.r_gain_adj != item->adj.r_gain ||
		awb->gain_adj.b_gain_adj != item->adj.b_gain) {
		awb->gain_adj.r_gain_adj = item->adj.r_gain;
		awb->gain_adj.b_gain_adj = item->adj.b_gain;
		return awb_algo_ctrl(awb, SET_GAIN_ADJ, awb->gain_adj);
	} else {
		return RTS_ISP_OK;
	}
}

static int mod_awb_mcrop_change(struct isp_mod *mod, void *data)
{
	int ret;
	rts_isp_grid_t grid;
	struct isp_mod_awb *awb = get_mod(mod, awb);
	const rts_isp_rect_t *rect = data;

	ret = rts_isp_get_grid_from_rect(&grid, rect, &awb->hw_info.grid_info);
	if (ret) {
		return ret;
	}
	ret = awb_calc_pixels(awb, &grid);
	if (ret) {
		return ret;
	}
	return awb->hw_ops->set_statis(mod->owner_id, &grid);
}

static struct isp_mod_action_info awb_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_awb_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_PLUGIN_UPDATE, mod_awb_plugin_update),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_awb_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_awb_dynamic),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_awb_mcrop_change),
};

static int isp_mod_awb_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_awb *awb;

	awb = get_mod(mod, awb);

	awb->hw_ops = isp_hw_awb_get_ops();
	if (!awb->hw_ops) {
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	awb->algo_param = isp_algo_param_get_awb_v1(mod->owner_id);
	if (!awb->algo_param) {
		isp_error("get awb algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	ret = awb->hw_ops->get_hw_info(mod->owner_id, &awb->hw_info);
	if (ret) {
		goto err;
	}
	awb->mod_iq = isp_mod_get_sibling(mod, MOD_IQ);
	if (!awb->mod_iq) {
		goto err;
	}
	awb->mode = RTS_ISP_AWB_AUTO_MODE;
	awb->_auto_setting.r_gain_adj = 256;
	awb->_auto_setting.b_gain_adj = 256;
	awb->gain_cur.r_gain = awb->gain_tar.r_gain = 256;
	awb->gain_cur.gr_gain = awb->gain_tar.gr_gain = 256;
	awb->gain_cur.gb_gain = awb->gain_tar.gb_gain = 256;
	awb->gain_cur.b_gain = awb->gain_tar.b_gain = 256;

	return RTS_ISP_OK;

err:
	return ret;
}

static int isp_mod_awb_cleanup(struct isp_mod *mod)
{
	mod_awb_cleanup_algo(mod);

	return RTS_ISP_OK;
}

static int isp_mod_awb_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_awb *awb = get_mod(mod, awb);

	return awb->hw_ops->init(mod->owner_id);
}

static int awb_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_awb *awb = get_mod(mod, awb);

	if (ctrl->id != ISP_V4L2_CID_AUTO_WHITE_BALANCE) {
		return -RTS_ISP_ECTRL;
	}

	if (awb->color_temp) {
		awb->color_temp_ctrl->val = awb->color_temp;
		awb->red_ctrl->val = awb->gain_tar.r_gain;
		awb->green_ctrl->val = (awb->gain_tar.gr_gain +
								awb->gain_tar.gb_gain) / 2;
		awb->blue_ctrl->val = awb->gain_tar.b_gain;
	} else {
		awb->color_temp_ctrl->val = awb->color_temp_ctrl->default_value;
		awb->red_ctrl->val = awb->red_ctrl->default_value;
		awb->green_ctrl->val = awb->green_ctrl->default_value;
		awb->blue_ctrl->val = awb->blue_ctrl->default_value;
	}

	return RTS_ISP_OK;
}

static int awb_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int ret;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct rts_isp_awb_gain_result result;

	if (ctrl->id != ISP_V4L2_CID_AUTO_WHITE_BALANCE) {
		return -RTS_ISP_ECTRL;
	}

	if (awb->day_night == RTS_ISP_NIGHT && !awb->iq->enable) {
		return RTS_ISP_OK;
	}

	if (awb->awb_ctrl->is_new) {
		if (awb->awb_ctrl->val) {
			ret = isp_mod_add_notify(mod, ISP_NOTIFY_STATIS_DONE);
		} else {
			ret = isp_mod_del_notify(mod, ISP_NOTIFY_STATIS_DONE);
		}
		if (ret) {
			return ret;
		}
	}

	if (awb->red_ctrl->is_new || awb->green_ctrl->is_new ||
		awb->blue_ctrl->is_new) {
		result.gain.r_gain = awb->red_ctrl->val;
		result.gain.gr_gain = awb->green_ctrl->val;
		result.gain.gb_gain = awb->green_ctrl->val;
		result.gain.b_gain = awb->blue_ctrl->val;
		ret = awb_algo_ctrl(awb, GAIN_TO_COLOR_TEMP, &result);
		if (ret) {
			isp_perror(ret, "get color temp from gain fail");
			return ret;
		}
		result.color_temperature =
			clamp_t(uint32_t, result.color_temperature,
					awb->color_temp_ctrl->minimum,
					awb->color_temp_ctrl->maximum);

		awb->force_run++;
		ret = mod_awb_set_result_dynamic(mod, &result);
		if (ret) {
			return ret;
		}
		ret = mod_awb_set_gain(mod);
		if (ret) {
			return ret;
		}

		awb->color_temp_ctrl->val = result.color_temperature;
		awb->color_temp_ctrl->cur.val = result.color_temperature;
	} else if (awb->color_temp_ctrl->is_new) {
		result.color_temperature = awb->color_temp_ctrl->val;
		ret = awb_algo_ctrl(awb, COLOR_TEMP_TO_GAIN, &result);
		if (ret) {
			isp_perror(ret, "get gain from color temp fail");
			return ret;
		}

		awb->force_run++;
		ret = mod_awb_set_result_dynamic(mod, &result);
		if (ret) {
			return ret;
		}
		ret = mod_awb_set_gain(mod);
		if (ret) {
			return ret;
		}

		awb->red_ctrl->val = result.gain.r_gain;
		awb->green_ctrl->val =
			(result.gain.gr_gain + result.gain.gb_gain) / 2;
		awb->blue_ctrl->val = result.gain.b_gain;
		awb->red_ctrl->cur.val = result.gain.r_gain;
		awb->green_ctrl->cur.val =
			(result.gain.gr_gain + result.gain.gb_gain) / 2;
		awb->blue_ctrl->cur.val = result.gain.b_gain;
	}

	return RTS_ISP_OK;
}

static const struct v4l2_ctrl_ops awb_ctrl_ops = {
	.g_volatile_ctrl = awb_g_volatile_ctrl,
	.s_ctrl = awb_s_ctrl,
};

static int isp_mod_awb_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_awb *awb = get_mod(mod, awb);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config green_config = {
		.ops = &awb_ctrl_ops,
		.id = ISP_V4L2_CID_GREEN_BALANCE,
		.name = "Green Balance",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = 256,
		.max = 2047,
		.step = 1,
		.def = 256,
		.flags = ISP_V4L2_CTRL_FLAG_SLIDER,
	};

	awb->awb_ctrl = v4l2_ctrl_new_std(handler, &awb_ctrl_ops,
									  ISP_V4L2_CID_AUTO_WHITE_BALANCE, 0, 1,
									  1, 1, mod);
	awb->red_ctrl = v4l2_ctrl_new_std(handler, &awb_ctrl_ops,
									  ISP_V4L2_CID_RED_BALANCE, 256, 2047,
									  1, 256, mod);
	awb->green_ctrl = v4l2_ctrl_new_custom(handler, &green_config, mod);
	awb->blue_ctrl = v4l2_ctrl_new_std(handler, &awb_ctrl_ops,
									   ISP_V4L2_CID_BLUE_BALANCE, 256, 2047,
									   1, 256, mod);
	awb->color_temp_ctrl = v4l2_ctrl_new_std(
							   handler, &awb_ctrl_ops, ISP_V4L2_CID_WHITE_BALANCE_TEMPERATURE,
							   1000, 10000, 1, 5000, mod);
	v4l2_ctrl_auto_cluster(5, &awb->awb_ctrl, 0, RTS_ISP_TRUE);
	awb->awb_ctrl->flags |= ISP_V4L2_CTRL_FLAG_READ_ONLY;
	if (handler->error) {
		isp_perror(handler->error, "add awb ctrls fail");
		return handler->error;
	}
	awb->red_ctrl->flags |= ISP_V4L2_CTRL_FLAG_UPDATE;
	awb->green_ctrl->flags |= ISP_V4L2_CTRL_FLAG_UPDATE;
	awb->blue_ctrl->flags |= ISP_V4L2_CTRL_FLAG_UPDATE;
	awb->color_temp_ctrl->flags |= ISP_V4L2_CTRL_FLAG_UPDATE;

	return RTS_ISP_OK;
}

static struct isp_mod_awb awb;

int isp_register_mod_awb(void)
{
	isp_memset(&awb, 0, sizeof(awb));
	awb.mod.id = MOD_AWB;
	awb.mod.name = "awb";
	awb.mod.init = isp_mod_awb_init;
	awb.mod.cleanup = isp_mod_awb_cleanup;
	awb.mod.hardware_init = isp_mod_awb_hardware_init;
	awb.mod.add_ctrl = isp_mod_awb_add_ctrl;
	awb.mod.info_actions = awb_info_actions;
	awb.mod.info_actions_num = ARRAY_SIZE(awb_info_actions);
	awb.mod.exec_actions = awb_exec_actions;
	awb.mod.exec_actions_num = ARRAY_SIZE(awb_exec_actions);
	awb.mod.notify_actions = awb_notify_actions;
	awb.mod.notify_actions_num = ARRAY_SIZE(awb_notify_actions);
	awb.mod.size = sizeof(awb);
	return isp_mod_register(&awb.mod);
}

void isp_unregister_mod_awb(void)
{
	isp_mod_unregister(&awb.mod);
}
