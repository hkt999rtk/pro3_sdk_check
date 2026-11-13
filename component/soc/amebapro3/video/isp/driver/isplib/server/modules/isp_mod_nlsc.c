/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_iq.h>
#include <isp_top.h>
#include <isp_statis.h>
#include <isp_notify.h>
#include <isp_hw_nlsc.h>
#include "isp_mod_sensor.h"
#include "isp_mod_awb.h"
#include "isp_mod_input.h"
#include "isp_mod_nlsc.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_LSC

struct isp_mod_nlsc {
	struct isp_mod mod;
	const struct isp_hw_nlsc_ops *hw_ops;

	const isp_iq_nlsc_v1_t *iq;
	isp_algo_param_nlsc_v1_t *algo_param;
	isp_iq_nlsc_v1_dyn_item_t dyn_items[3];

	rts_isp_point_t offset;

	uint32_t total_wp_num;
	uint32_t need_update_wp_num : 1;
	uint32_t in_std_illum : 1;

	struct rts_isp_mirror_flip mf_info;
};

static int isp_mod_nlsc_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_nlsc *nlsc;

	nlsc = get_mod(mod, nlsc);
	nlsc->hw_ops = isp_hw_nlsc_get_ops();
	if (nlsc->hw_ops == NULL) {
		isp_error("get nlsc hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	nlsc->algo_param = isp_algo_param_get_nlsc_v1(mod->owner_id);
	if (!nlsc->algo_param) {
		isp_error("get nlsc algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}
	nlsc->need_update_wp_num = RTS_ISP_TRUE;

	return RTS_ISP_OK;
err:
	return ret;
}

static int isp_mod_nlsc_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int nlsc_judge_in_std_illum(struct isp_mod_nlsc *nlsc,
								   const struct rts_isp_awb_statis *statis)
{
	int i;
	uint32_t max_wp_num = 0;
	uint32_t white_pixel_rate;

	if (nlsc->need_update_wp_num) {
		int ret;
		struct isp_mod_awb_stat_info awb_stat_info;

		nlsc->need_update_wp_num = RTS_ISP_FALSE;
		ret = isp_mod_sibling_get_info(&nlsc->mod, MOD_AWB,
									   AWB_INFO_STATIS, &awb_stat_info,
									   sizeof(awb_stat_info));
		if (ret < 0 || !awb_stat_info.info.fine_pixels) {
			nlsc->total_wp_num = 1000000;
		} else {
			nlsc->total_wp_num = awb_stat_info.info.fine_pixels;
		}
	}

	for (i = 0; i < ARRAY_SIZE(statis->illum_white_pixels); i++)
		if (max_wp_num < statis->illum_white_pixels[i]) {
			max_wp_num = statis->illum_white_pixels[i];
		}
	white_pixel_rate = max_wp_num * 1000 / nlsc->total_wp_num;
	if (white_pixel_rate > nlsc->iq->dynamic.config.en_thd.th0) {
		return RTS_ISP_TRUE;
	} else if (white_pixel_rate < nlsc->iq->dynamic.config.en_thd.th1) {
		return RTS_ISP_FALSE;
	} else {
		return nlsc->in_std_illum;
	}
}

static int check_nlsc_iq(const isp_iq_nlsc_v1_t *iq)
{
	if (!iq) {
		return -RTS_ISP_ENOITEM;
	}
	if (iq->common.item.step == 0 || iq->common.item.r_curve_count == 0 ||
		iq->common.item.r_curve_count != iq->common.item.g_curve_count ||
		iq->common.item.r_curve_count != iq->common.item.b_curve_count) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

static void intp_nlsc_v1_item(isp_iq_nlsc_v1_dyn_item_t *adj, int val, int th0,
							  const isp_iq_nlsc_v1_dyn_item_t *item0, int th1,
							  const isp_iq_nlsc_v1_dyn_item_t *item1)
{
	if (!adj || !item0 || !item1) {
		return;
	}

	adj->r = linear_intp(val, th0, item0->r, th1, item1->r);
	adj->g = linear_intp(val, th0, item0->g, th1, item1->g);
	adj->b = linear_intp(val, th0, item0->b, th1, item1->b);
}

DEFINE_ISP_IQ_INTP_FUNC(nlsc, v1, intp_nlsc_v1_item)

static int nlsc_bounds_active_cb(int type, void *priv)
{
	struct isp_mod_nlsc *nlsc = priv;

	if (type == ISP_IQ_BOUND_COLOR_TEMP_INTP ||
		type == ISP_IQ_BOUND_COLOR_TEMP_GRP)
		if (!nlsc->in_std_illum) {
			return RTS_ISP_FALSE;
		}
	return RTS_ISP_TRUE;
}

static int nlsc_set_center(struct isp_mod_nlsc *nlsc)
{
	int ret;
	struct isp_hw_nlsc_center center;
	const struct isp_sensor_mode *mode;

	if (!nlsc->iq) {
		return RTS_ISP_OK;
	}

	ret = isp_mod_sibling_get_info(&nlsc->mod, MOD_SENSOR, SENSOR_INFO_MODE,
								   &mode, sizeof(mode));
	if (ret) {
		return ret;
	}

	center.r.x = nlsc->iq->common.item.r_center.x - nlsc->offset.x;
	center.g.x = nlsc->iq->common.item.g_center.x - nlsc->offset.x;
	center.b.x = nlsc->iq->common.item.b_center.x - nlsc->offset.x;
	center.r.y = nlsc->iq->common.item.r_center.y - nlsc->offset.y;
	center.g.y = nlsc->iq->common.item.g_center.y - nlsc->offset.y;
	center.b.y = nlsc->iq->common.item.b_center.y - nlsc->offset.y;

	if (nlsc->mf_info.mirror) {
		center.r.x = mode->size.isp.w - center.r.x;
		center.g.x = mode->size.isp.w - center.g.x;
		center.b.x = mode->size.isp.w - center.b.x;
	}
	if (nlsc->mf_info.flip) {
		center.r.y = mode->size.isp.h - center.r.y;
		center.g.y = mode->size.isp.h - center.g.y;
		center.b.y = mode->size.isp.h - center.b.y;
	}

	center.r.x = clamp_t(int, center.r.x, 0, mode->size.isp.w);
	center.g.x = clamp_t(int, center.g.x, 0, mode->size.isp.w);
	center.b.x = clamp_t(int, center.b.x, 0, mode->size.isp.w);
	center.r.y = clamp_t(int, center.r.y, 0, mode->size.isp.h);
	center.g.y = clamp_t(int, center.g.y, 0, mode->size.isp.h);
	center.b.y = clamp_t(int, center.b.y, 0, mode->size.isp.h);

	return nlsc->hw_ops->set_center(nlsc->mod.owner_id, &center);
}

static int mod_nlsc_statis_done(struct isp_mod *mod, void *data)
{
	struct isp_mod_nlsc *nlsc = get_mod(mod, nlsc);
	const struct rts_isp_awb_statis *statis;

	statis = &((const struct rts_isp_statis *)data)->awb;
	nlsc->in_std_illum = nlsc_judge_in_std_illum(nlsc, statis);

	return RTS_ISP_OK;
}

static int mod_nlsc_iq_change(struct isp_mod *mod, void *data)
{
	int ret;
	const isp_iq_nlsc_v1_dyn_item_t *item;
	struct isp_mod_nlsc *nlsc = get_mod(mod, nlsc);

	nlsc->iq = isp_iq_get_nlsc_v1(mod->owner_id, &nlsc->algo_param->common);
	if (check_nlsc_iq(nlsc->iq)) {
		return -RTS_ISP_EINVAL;
	}

	ret = nlsc->hw_ops->set_nlsc(mod->owner_id, &nlsc->iq->common.item);
	if (ret) {
		return ret;
	}
	ret = nlsc_set_center(nlsc);
	if (ret) {
		return ret;
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_nlsc_v1_dyn_enabled(&nlsc->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_nlsc_v1_dyn_item_default(&nlsc->iq->dynamic.nest,
			&nlsc->algo_param->common);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	return nlsc->hw_ops->set_adj(mod->owner_id, item);
}

static int mod_nlsc_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_nlsc *nlsc = get_mod(mod, nlsc);
	const isp_iq_nlsc_v1_t *iq = nlsc->iq;
	const struct isp_notify_dynamic *dyn = data;
	const isp_iq_nlsc_v1_dyn_item_t *item;

	item = isp_iq_get_nlsc_v1_dyn_item_ex(&iq->dynamic.nest,
										  &nlsc->algo_param->common, dyn,
										  isp_iq_nlsc_v1_intp_cb, nlsc,
										  nlsc_bounds_active_cb, nlsc);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}

	return nlsc->hw_ops->set_adj(mod->owner_id, item);
}

static int mod_nlsc_crop_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_nlsc *nlsc = get_mod(mod, nlsc);

	nlsc->offset = *(const rts_isp_point_t *)data;

	return nlsc_set_center(nlsc);
}

static int mod_nlsc_mcrop_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_nlsc *nlsc = get_mod(mod, nlsc);

	nlsc->need_update_wp_num = RTS_ISP_TRUE;

	return RTS_ISP_OK;
}

static int mod_nlsc_mirror_flip_change(struct isp_mod *mod, void *data)
{
	int ret;
	int mode;
	struct isp_mod_nlsc *nlsc = get_mod(mod, nlsc);

	ret = isp_mod_sibling_get_info(mod, MOD_INPUT,
								   INPUT_INFO_LARGE_IMAGE_MODE, &mode,
								   sizeof(mode));
	if (ret) {
		return ret;
	}
	if (mode) {
		return RTS_ISP_OK;
	}

	nlsc->mf_info = *(struct rts_isp_mirror_flip *)data;
	return nlsc_set_center(nlsc);
}

static struct isp_mod_action_info nlsc_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_nlsc_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_nlsc_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_nlsc_dynamic),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_CROP_CHANGE, mod_nlsc_crop_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_nlsc_mcrop_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MIRROR_FLIP_CHANGE,
					  mod_nlsc_mirror_flip_change),
};

static struct isp_mod_nlsc nlsc;

int isp_register_mod_nlsc(void)
{
	isp_memset(&nlsc, 0, sizeof(nlsc));
	nlsc.mod.id = MOD_NLSC;
	nlsc.mod.name = "nlsc";
	nlsc.mod.init = isp_mod_nlsc_init;
	nlsc.mod.cleanup = isp_mod_nlsc_cleanup;
	nlsc.mod.notify_actions = nlsc_notify_actions;
	nlsc.mod.notify_actions_num = ARRAY_SIZE(nlsc_notify_actions);
	nlsc.mod.size = sizeof(nlsc);
	return isp_mod_register(&nlsc.mod);
}

void isp_unregister_mod_nlsc(void)
{
	isp_mod_unregister(&nlsc.mod);
}
