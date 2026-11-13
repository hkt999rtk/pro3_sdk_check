/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_top.h>
#include <isp_core.h>
#include <isp_mod.h>
#include <isp_notify.h>
#include <isp_poll.h>
#include <isp_statis.h>
#include <isp_iq.h>
#include <isp_v4l2_ctrl.h>
#include <isp_hw_rawstat.h>
#include "isp_mod_rawstat.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_RAWSTAT

struct isp_mod_daynight {
	int32_t cur; /* enum rts_isp_day_night */
	int32_t result; /* enum rts_isp_day_night */
	int is_changing;
	uint32_t etgain;
	uint32_t win_num;
	uint16_t block_cnt;
	isp_timer_handle_t timer;
	const isp_iq_daynight_v1_t *iq;
	isp_algo_param_daynight_v1_t *algo_param;
	isp_iq_daynight_v1_dyn_item_t dyn_items[3];
};

struct isp_mod_rawstat {
	struct isp_mod mod;
	struct isp_rawstat_hw_info hw_info;
	const struct isp_hw_rawstat_ops *hw_ops;
	uint32_t pixels_per_cell;
	uint32_t fine_pixels;

	struct isp_mod_daynight dn;
	struct v4l2_ctrl *day_night_detect_ctrl;
};

static int day_night_info_init(struct isp_mod_rawstat *rawstat)
{
	rawstat->dn.win_num = rawstat->hw_info.grid_info.size.cols *
						  rawstat->hw_info.grid_info.size.rows;
	rawstat->dn.block_cnt = 0;
	return RTS_ISP_OK;
}

static int day_night_timer_callback(isp_timer_handle_t *timer, void *data)
{
	struct isp_mod *mod = data;
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	struct isp_mod_daynight *dn = &rawstat->dn;

	if (!dn->is_changing) {
		return RTS_ISP_OK;
	}

	if (dn->result == RTS_ISP_NIGHT) {
		dn->result = RTS_ISP_DAY;
	} else {
		dn->result = RTS_ISP_NIGHT;
	}

	return RTS_ISP_OK;
}

static void day_night_process_statis(struct isp_mod_daynight *dn,
									 const struct rts_isp_raw_statis *statis)
{
	int i;

	dn->block_cnt = 0;
	if (dn->result == RTS_ISP_NIGHT) {
		for (i = 0; i < dn->win_num; i++) {
			if (dn->iq->exit_ir_detect_table[i] &&
				(statis->rg[i] <=
				 dn->dyn_items[0].exit_ir_gain_th ||
				 statis->bg[i] <=
				 dn->dyn_items[0].exit_ir_gain_th)) {
				dn->block_cnt++;
			}
		}
	}
}

static int day_night_detect(struct isp_mod_daynight *dn)
{
	if (dn->result == RTS_ISP_NIGHT) {
		uint8_t block_th;

		block_th = dn->dyn_items[0].exit_ir_block_th * dn->win_num;
		if (dn->block_cnt > block_th &&
			dn->etgain < dn->dyn_items[0].exit_ir_etgain_th) {
			if (!dn->is_changing) {
				dn->is_changing = 1;
				return isp_timer_start(
						   &dn->timer,
						   dn->dyn_items[0].ir2rgb_time, 0);
			}
		} else {
			if (dn->is_changing) {
				dn->is_changing = 0;
				return isp_timer_stop(&dn->timer);
			}
		}
	} else {
		if (dn->etgain > dn->dyn_items[0].exit_rgb_etgain_th) {
			if (!dn->is_changing) {
				dn->is_changing = 1;
				return isp_timer_start(
						   &dn->timer,
						   dn->dyn_items[0].rgb2ir_time, 0);
			}
		} else {
			if (dn->is_changing) {
				dn->is_changing = 0;
				return isp_timer_stop(&dn->timer);
			}
		}
	}
	return RTS_ISP_OK;
}

static int rawstat_calc_pixels(struct isp_mod_rawstat *rawstat,
							   const rts_isp_grid_t *grid)
{
	rawstat->pixels_per_cell =
		(grid->cell.w * grid->cell.h / rawstat->hw_info.cell_sample.w /
		 rawstat->hw_info.cell_sample.h);
	rawstat->fine_pixels =
		(rawstat->pixels_per_cell * grid->size.cols * grid->size.rows);
	return RTS_ISP_OK;
}

static int mod_rawstat_info_statis(struct isp_mod *mod, void *data)
{
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	struct isp_mod_rawstat_stat_info *info = data;

	info->info.win = rawstat->hw_info.grid_info.size;
	info->info.pixels_per_cell = rawstat->pixels_per_cell;
	info->info.fine_pixels = rawstat->fine_pixels;
	info->rgby_size = rawstat->hw_info.rgby_size;
	info->rgbg_size = rawstat->hw_info.rgbg_size;
	info->pixels_size = rawstat->hw_info.pixels_size;

	return RTS_ISP_OK;
}

static struct isp_mod_action_info rawstat_info_actions[] = {
	ISP_MOD_ACTION(RAWSTAT_INFO_STATIS, mod_rawstat_info_statis),
};

static void intp_daynight_v1_item(isp_iq_daynight_v1_dyn_item_t *item, int val,
								  int th0,
								  const isp_iq_daynight_v1_dyn_item_t *item0,
								  int th1,
								  const isp_iq_daynight_v1_dyn_item_t *item1)
{
	item->exit_ir_block_th = linear_intp_float(val, th0,
							 item0->exit_ir_block_th, th1,
							 item1->exit_ir_block_th);
	item->exit_rgb_etgain_th = linear_intp(val, th0,
										   item0->exit_rgb_etgain_th, th1,
										   item1->exit_rgb_etgain_th);
	item->exit_ir_etgain_th = linear_intp(val, th0,
										  item0->exit_ir_etgain_th, th1,
										  item1->exit_ir_etgain_th);
	item->exit_ir_gain_th = linear_intp(val, th0, item0->exit_ir_gain_th,
										th1, item1->exit_ir_gain_th);
	item->ir2rgb_time = linear_intp(val, th0, item0->ir2rgb_time, th1,
									item1->ir2rgb_time);
	item->rgb2ir_time = linear_intp(val, th0, item0->rgb2ir_time, th1,
									item1->rgb2ir_time);
}

DEFINE_ISP_IQ_INTP_FUNC(daynight, v1, intp_daynight_v1_item)

static int mod_rawstat_statis_done(struct isp_mod *mod, void *data)
{
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	const struct rts_isp_raw_statis *statis;

	statis = &((const struct rts_isp_statis *)data)->raw;

	if (statis->frame_count % (rawstat->dn.iq->frame_interval + 1)) {
		return RTS_ISP_OK;
	}

	day_night_process_statis(&rawstat->dn, statis);
	return day_night_detect(&rawstat->dn);
}

static int mod_rawstat_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	const isp_iq_daynight_v1_dyn_item_t *item;

	rawstat->dn.cur = isp_iq_is_night(mod->owner_id);
	rawstat->dn.result = rawstat->dn.cur;
	rawstat->dn.iq = isp_iq_get_daynight_v1(
						 mod->owner_id, &rawstat->dn.algo_param->common);
	if (!rawstat->dn.iq) {
		return -RTS_ISP_ENOITEM;
	}
	if (rawstat->dn.iq->exit_ir_detect_table_count != rawstat->dn.win_num) {
		return -RTS_ISP_ERANGE;
	}

	item = isp_iq_get_daynight_v1_dyn_item_default(
			   &rawstat->dn.iq->dynamic.nest, &rawstat->dn.algo_param->common);
	if (!item) {
		return -RTS_ISP_ENOITEM;
	}
	rawstat->dn.dyn_items[0] = *item;

	rawstat->dn.is_changing = 0;

	return RTS_ISP_OK;
}

static int mod_rawstat_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	struct isp_notify_dynamic *dyn = data;
	const isp_iq_daynight_v1_dyn_item_t *item;

	rawstat->dn.etgain = dyn->ae.iq_exp_gain[0];
	item = isp_iq_get_daynight_v1_dyn_item(&rawstat->dn.iq->dynamic.nest,
										   &rawstat->dn.algo_param->common,
										   dyn, isp_iq_daynight_v1_intp_cb,
										   &rawstat->dn);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (&rawstat->dn.dyn_items[0] != item) {
		rawstat->dn.dyn_items[0] = *item;
	}

	return RTS_ISP_OK;
}

static int mod_rawstat_mcrop_change(struct isp_mod *mod, void *data)
{
	int ret;
	rts_isp_grid_t grid;
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	const rts_isp_rect_t *rect = data;

	ret = rts_isp_get_grid_from_rect(&grid, rect,
									 &rawstat->hw_info.grid_info);
	if (ret) {
		return ret;
	}
	ret = rawstat_calc_pixels(rawstat, &grid);
	if (ret) {
		return ret;
	}
	return rawstat->hw_ops->set_statis(mod->owner_id, &grid);
}

static struct isp_mod_action_info rawstat_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_rawstat_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_rawstat_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_rawstat_dynamic),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_rawstat_mcrop_change),
};

static int isp_mod_rawstat_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_rawstat *rawstat;

	rawstat = get_mod(mod, rawstat);
	rawstat->hw_ops = isp_hw_rawstat_get_ops();
	if (rawstat->hw_ops == NULL) {
		isp_error("get rawstat hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto out;
	}
	rawstat->dn.algo_param = isp_algo_param_get_daynight_v1(mod->owner_id);
	if (!rawstat->dn.algo_param) {
		isp_error("get daynight algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto out;
	}

	ret = rawstat->hw_ops->get_hw_info(mod->owner_id, &rawstat->hw_info);
	if (ret) {
		goto out;
	}

	day_night_info_init(rawstat);

	ret = isp_timer_init(&rawstat->dn.timer, isp_top_get_poll(),
						 day_night_timer_callback, mod);
out:
	return ret;
}

static int isp_mod_rawstat_cleanup(struct isp_mod *mod)
{
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);

	isp_timer_cleanup(&rawstat->dn.timer);
	return RTS_ISP_OK;
}

static int rawstat_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);

	switch (ctrl->id) {
	case ISP_V4L2_CID_DAYNIGHT_DETECTION:
		rawstat->day_night_detect_ctrl->val = rawstat->dn.result;
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	return RTS_ISP_OK;
}

static const struct v4l2_ctrl_ops rawstat_ctrl_ops = {
	.g_volatile_ctrl = rawstat_g_volatile_ctrl,
};

static int isp_mod_rawstat_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_rawstat *rawstat = get_mod(mod, rawstat);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config get_dn_config = {
		.ops = &rawstat_ctrl_ops,
		.id = ISP_V4L2_CID_DAYNIGHT_DETECTION,
		.name = "Day Night Detect",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
		.flags = ISP_V4L2_CTRL_FLAG_READ_ONLY |
		ISP_V4L2_CTRL_FLAG_VOLATILE,
	};

	rawstat->day_night_detect_ctrl =
		v4l2_ctrl_new_custom(handler, &get_dn_config, mod);
	if (handler->error) {
		isp_perror(handler->error, "add rawstat ctrls fail");
		return handler->error;
	}

	return RTS_ISP_OK;
}

static struct isp_mod_rawstat rawstat;

int isp_register_mod_rawstat(void)
{
	isp_memset(&rawstat, 0, sizeof(rawstat));
	rawstat.mod.id = MOD_RAWSTAT;
	rawstat.mod.name = "rawstat";
	rawstat.mod.init = isp_mod_rawstat_init;
	rawstat.mod.cleanup = isp_mod_rawstat_cleanup;
	rawstat.mod.add_ctrl = isp_mod_rawstat_add_ctrl;
	rawstat.mod.info_actions = rawstat_info_actions;
	rawstat.mod.info_actions_num = ARRAY_SIZE(rawstat_info_actions);
	rawstat.mod.notify_actions = rawstat_notify_actions;
	rawstat.mod.notify_actions_num = ARRAY_SIZE(rawstat_notify_actions);
	rawstat.mod.size = sizeof(rawstat);
	return isp_mod_register(&rawstat.mod);
}

void isp_unregister_mod_rawstat(void)
{
	isp_mod_unregister(&rawstat.mod);
}
