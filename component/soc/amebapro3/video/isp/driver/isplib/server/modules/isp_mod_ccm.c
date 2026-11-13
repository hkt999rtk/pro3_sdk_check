/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_top.h>
#include <isp_v4l2_ctrl.h>
#include <isp_notify.h>
#include <isp_iq.h>
#include <isp_mod.h>
#include <isp_hw_ccm.h>
#include "isp_mod_ccm.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_CCM

struct isp_mod_ccm {
	struct isp_mod mod;
	const struct isp_hw_ccm_ops *hw_ops;
	struct isp_ccm_hw_info hw_info;

	const isp_iq_ccm_v1_t *iq;
	isp_algo_param_ccm_v1_t *algo_param;
	isp_iq_ccm_v1_dyn_item_t dyn_items[3];

	isp_iq_ccm_v1_dyn_item_t current;
	int force_update;
	int patch_mode;

	struct {
		struct v4l2_ctrl *auto_ccm_ctrl;
		struct v4l2_ctrl *ccm_matrix_ctrl;
	};
};

static int isp_mod_ccm_init(struct isp_mod *mod)
{
	int ret;
	struct isp_mod_ccm *ccm;

	ccm = get_mod(mod, ccm);
	ccm->hw_ops = isp_hw_ccm_get_ops();
	if (ccm->hw_ops == NULL) {
		isp_error("get ccm hw ops fail\n");
		ret = -RTS_ISP_ENOOPS;
		goto err;
	}
	ccm->algo_param = isp_algo_param_get_ccm_v1(mod->owner_id);
	if (!ccm->algo_param) {
		isp_error("get ccm algo param fail\n");
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	return ccm->hw_ops->get_hw_info(mod->owner_id, &ccm->hw_info);
err:
	return ret;
}

static int isp_mod_ccm_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_ccm_hardware_init(struct isp_mod *mod)
{
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);

	return ccm->hw_ops->init_vreg(mod->owner_id);
}

static int ccm_set_matrix(struct isp_mod_ccm *ccm)
{
	if (!ccm->auto_ccm_ctrl->val) {
		return RTS_ISP_OK;
	}

	return ccm->hw_ops->set_ccm(ccm->mod.owner_id, &ccm->current);
}

static void intp_ccm_v1_item(isp_iq_ccm_v1_dyn_item_t *item, int val, int th0,
							 const isp_iq_ccm_v1_dyn_item_t *item0, int th1,
							 const isp_iq_ccm_v1_dyn_item_t *item1)
{
	linear_intp_array(item->ccm, val, th0, item0->ccm, th1, item1->ccm,
					  ARRAY_SIZE(item->ccm));
}

DEFINE_ISP_IQ_INTP_FUNC(ccm, v1, intp_ccm_v1_item)

static int mod_ccm_exec_set_patch_mode(struct isp_mod *mod, void *data)
{
	int ret = RTS_ISP_OK;
	int enable = *(int32_t *)data;
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);

	ret = isp_mod_set_patch_notify(mod, enable, RTS_ISP_TRUE);
	if (!ret) {
		ccm->patch_mode = enable;
	}

	return ret;
}

static int mod_ccm_exec_set_v1_dyn(struct isp_mod *mod, void *data)
{
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);
	isp_iq_ccm_v1_dyn_item_t *item = *(void **)data;

	if (!ccm->patch_mode) {
		return -RTS_ISP_EINVAL;
	}
	ccm->current = *item;
	return ccm_set_matrix(ccm);
}

static struct isp_mod_action_info ccm_exec_actions[] = {
	ISP_MOD_ACTION(CCM_EXEC_SET_PATCH_MODE, mod_ccm_exec_set_patch_mode),
	ISP_MOD_ACTION(CCM_EXEC_SET_V1_DYN, mod_ccm_exec_set_v1_dyn),
};

static int mod_ccm_statis_done(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);

	if (!ccm->force_update && isp_memcmp(&ccm->current, &ccm->dyn_items[0],
										 sizeof(ccm->current)) == 0) {
		return RTS_ISP_OK;
	}
	ccm->force_update = RTS_ISP_FALSE;
	calc_smooth_array(ccm->current.ccm, ccm->dyn_items[0].ccm, 8, 16,
					  ARRAY_SIZE(ccm->current.ccm));
	ret = ccm_set_matrix(ccm);
	if (ret) {
		return ret;
	}
	/* need to call estimation to set the sync reg */
	return ccm->hw_ops->est_dynamic(mod->owner_id);
}

static int mod_ccm_iq_change(struct isp_mod *mod, void *data)
{
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);
	const isp_iq_ccm_v1_dyn_item_t *item;

	ccm->iq = isp_iq_get_ccm_v1(mod->owner_id, &ccm->algo_param->common);
	if (!ccm->iq) {
		return -RTS_ISP_EINVAL;
	}

	if (isp_top_get_status() == RTS_ISP_STREAMING &&
		isp_iq_ccm_v1_dyn_enabled(&ccm->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	item = isp_iq_get_ccm_v1_dyn_item_default(&ccm->iq->dynamic.nest,
			&ccm->algo_param->common);
	if (!item || ARRAY_SIZE(item->ccm) > CCM_ARRAY_SIZE_MAX) {
		return -RTS_ISP_EINVAL;
	}
	ccm->dyn_items[0] = ccm->current = *item;

	return ccm_set_matrix(ccm);
}

static int mod_ccm_dynamic(struct isp_mod *mod, void *data)
{
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);
	const isp_iq_ccm_v1_dyn_item_t *item;
	const struct isp_notify_dynamic *dyn = data;

	if (!isp_iq_ccm_v1_dyn_enabled(&ccm->iq->dynamic.nest)) {
		return RTS_ISP_OK;
	}

	ccm->force_update = RTS_ISP_TRUE;

	item = isp_iq_get_ccm_v1_dyn_item(&ccm->iq->dynamic.nest,
									  &ccm->algo_param->common, dyn,
									  isp_iq_ccm_v1_intp_cb, ccm);
	if (!item) {
		return -RTS_ISP_EINVAL;
	}
	if (item != ccm->dyn_items) {
		ccm->dyn_items[0] = *item;
	}

	return RTS_ISP_OK;
}

static int mod_ccm_dynamic_post(struct isp_mod *mod, void *data)
{
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);

	return ccm->hw_ops->est_dynamic(mod->owner_id);
}

static int mod_ccm_mcrop_change(struct isp_mod *mod, void *data)
{
	int ret;
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);
	const rts_isp_rect_t *rect = data;
	rts_isp_grid_t grid;

	ret = rts_isp_get_grid_from_rect(&grid, rect, &ccm->hw_info.grid_info);
	if (ret) {
		return ret;
	}
	return ccm->hw_ops->set_size(mod->owner_id, &grid);
}

static struct isp_mod_action_info ccm_notify_actions[] = {
	ISP_NOTIFY_ACTION(ISP_NOTIFY_STATIS_DONE, mod_ccm_statis_done),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_IQ_CHANGE, mod_ccm_iq_change),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_DYNAMIC, mod_ccm_dynamic),
	ISP_NOTIFY_PRIO_ACTION(ISP_NOTIFY_DYNAMIC, ISP_DYNAMIC_POST_PRIORITY,
						   mod_ccm_dynamic_post),
	ISP_NOTIFY_ACTION(ISP_NOTIFY_MCROP_CHANGE, mod_ccm_mcrop_change),
};

static int ccm_g_volatile_ctrl(struct v4l2_ctrl *ctrl)
{
	int i;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTO_CCM:
		for (i = 0; i < ARRAY_SIZE(ccm->current.ccm); i++)
			ccm->ccm_matrix_ctrl->p_new.p_s32[i] =
				ccm->current.ccm[i];
		break;
	default:
		return -RTS_ISP_ECTRL;
	}

	return RTS_ISP_OK;
}

static int ccm_s_ctrl(struct v4l2_ctrl *ctrl)
{
	int i;
	int ret = RTS_ISP_OK;
	isp_iq_ccm_v1_dyn_item_t item;
	struct isp_mod *mod = ctrl->priv;
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);

	switch (ctrl->id) {
	case ISP_V4L2_CID_AUTO_CCM:
		if (ctrl->val) {
			if (ctrl->is_new) {
				ret = ccm_set_matrix(ccm);
			}
		} else if (ccm->ccm_matrix_ctrl->is_new) {
			for (i = 0; i < ARRAY_SIZE(item.ccm); i++)
				item.ccm[i] =
					ccm->ccm_matrix_ctrl->p_new.p_s32[i];
			ret = ccm->hw_ops->set_ccm(mod->owner_id, &item);
		}
		break;
	default:
		ret = -RTS_ISP_ECTRL;
		break;
	}
	if (ret) {
		isp_perror(ret, "set ccm ctrl fail");
	}
	return ret;
}

static const struct v4l2_ctrl_ops ccm_ctrl_ops = {
	.g_volatile_ctrl = ccm_g_volatile_ctrl,
	.s_ctrl = ccm_s_ctrl,
};

static int isp_mod_ccm_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_ccm *ccm = get_mod(mod, ccm);
	struct v4l2_ctrl_handler *handler = phandler;
	const struct v4l2_ctrl_config auto_ccm_config = {
		.ops = &ccm_ctrl_ops,
		.id = ISP_V4L2_CID_AUTO_CCM,
		.name = "Auto CCM",
		.type = ISP_V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 1,
	};
	const struct v4l2_ctrl_config ccm_matrix_config = {
		.ops = &ccm_ctrl_ops,
		.id = ISP_V4L2_CID_CCM_MATRIX,
		.name = "CCM matrix",
		.type = ISP_V4L2_CTRL_TYPE_INTEGER,
		.min = -2048,
		.max = 2047,
		.step = 1,
		.def = 0,
		.dims = { CCM_ARRAY_SIZE_MAX },
	};

	ccm->auto_ccm_ctrl =
		v4l2_ctrl_new_custom(handler, &auto_ccm_config, mod);
	ccm->ccm_matrix_ctrl =
		v4l2_ctrl_new_custom(handler, &ccm_matrix_config, mod);
	v4l2_ctrl_auto_cluster(2, &ccm->auto_ccm_ctrl, 0, RTS_ISP_TRUE);
	if (handler->error) {
		isp_perror(handler->error, "add ccm ctrl fail");
		return handler->error;
	}
	return RTS_ISP_OK;
}

static struct isp_mod_ccm ccm;

int isp_register_mod_ccm(void)
{
	isp_memset(&ccm, 0, sizeof(ccm));
	ccm.mod.id = MOD_CCM;
	ccm.mod.name = "ccm";
	ccm.mod.init = isp_mod_ccm_init;
	ccm.mod.cleanup = isp_mod_ccm_cleanup;
	ccm.mod.hardware_init = isp_mod_ccm_hardware_init;
	ccm.mod.add_ctrl = isp_mod_ccm_add_ctrl;
	ccm.mod.exec_actions = ccm_exec_actions;
	ccm.mod.exec_actions_num = ARRAY_SIZE(ccm_exec_actions);
	ccm.mod.notify_actions = ccm_notify_actions;
	ccm.mod.notify_actions_num = ARRAY_SIZE(ccm_notify_actions);
	ccm.mod.size = sizeof(ccm);
	return isp_mod_register(&ccm.mod);
}

void isp_unregister_mod_ccm(void)
{
	isp_mod_unregister(&ccm.mod);
}
