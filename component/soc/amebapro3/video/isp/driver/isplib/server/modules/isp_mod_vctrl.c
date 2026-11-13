/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_mod.h>
#include <isp_msg_buf.h>
#include <isp_v4l2_ctrl.h>
#include "isp_mod_vctrl.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_V4L2

struct isp_mod_vctrl {
	struct isp_mod mod;
	struct v4l2_ctrl_handler *handler;
};

static int deserialize_ext_ctrls(struct isp_v4l2_ext_controls *ecs, void *buf,
								 size_t size)
{
	int i;
	int ret;
	void *end = buf + size;
	struct isp_mod_vctrl_ext_controls *buf_ecs;

	ret = get_buf_ptr(buf_ecs, buf, end);
	if (ret) {
		return ret;
	}
	ecs->ctrl_class = buf_ecs->ctrl_class;
	ecs->count = buf_ecs->count;
	ecs->error_idx = buf_ecs->error_idx;
	ecs->request_fd = buf_ecs->request_fd;
	ecs->reserved[0] = 0;

	ret = get_buf_array_ptr(ecs->controls, ecs->count, buf, end);
	if (ret) {
		return ret;
	}
	for (i = 0; i < ecs->count; i++) {
		if (!ecs->controls[i].size) {
			continue;
		}
		ret = get_buf_sized_ptr(ecs->controls[i].ptr,
								ecs->controls[i].size, buf, end);
		if (ret) {
			return ret;
		}
	}

	return RTS_ISP_OK;
}

static int serialize_ext_ctrls(struct isp_v4l2_ext_controls *ecs, void *buf,
							   size_t size)
{
	int ret;
	void *end = buf + size;
	struct isp_mod_vctrl_ext_controls *buf_ecs;

	ret = get_buf_ptr(buf_ecs, buf, end);
	if (ret) {
		return ret;
	}
	buf_ecs->ctrl_class = ecs->ctrl_class;
	buf_ecs->count = ecs->count;
	buf_ecs->error_idx = ecs->error_idx;
	buf_ecs->request_fd = ecs->request_fd;

	return RTS_ISP_OK;
}

static int
mod_vctrl_process_ext_ctrls(struct isp_mod *mod, void *data,
							int (*callback)(struct v4l2_ctrl_handler *,
									struct isp_v4l2_ext_controls *))
{
	int ret;
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);
	struct isp_msg_buf *msg = *(void **)data;
	struct isp_v4l2_ext_controls ecs;

	if (msg->hdr.msg_len != msg->hdr.capacity) {
		ret = -RTS_ISP_EINVAL;
		goto out;
	}
	ret = deserialize_ext_ctrls(&ecs, msg->data,
								isp_msg_data_len(&msg->hdr));
	if (ret) {
		goto out;
	}
	ret = callback(vctrl->handler, &ecs);
	if (ret) {
		goto out;
	}
	ret = serialize_ext_ctrls(&ecs, msg->data, isp_msg_data_len(&msg->hdr));
out:
	isp_msg_to_resp(msg, ret, msg->hdr.msg_len);
	return RTS_ISP_OK;
}

static int mod_vctrl_exec_query_ctrl(struct isp_mod *mod, void *data)
{
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);
	struct isp_v4l2_queryctrl *qc = data;

	return v4l2_queryctrl(vctrl->handler, qc);
}

static int mod_vctrl_exec_query_menu(struct isp_mod *mod, void *data)
{
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);
	struct isp_v4l2_querymenu *qm = data;

	return v4l2_querymenu(vctrl->handler, qm);
}

static int mod_vctrl_exec_s_ctrl(struct isp_mod *mod, void *data)
{
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);
	struct isp_v4l2_control *c = data;

	return v4l2_s_ctrl(vctrl->handler, c);
}

static int mod_vctrl_exec_g_ctrl(struct isp_mod *mod, void *data)
{
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);
	struct isp_v4l2_control *c = data;

	return v4l2_g_ctrl(vctrl->handler, c);
}

static int mod_vctrl_exec_query_ext_ctrl(struct isp_mod *mod, void *data)
{
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);
	struct isp_v4l2_query_ext_ctrl *qec = data;

	return v4l2_query_ext_ctrl(vctrl->handler, qec);
}

static int mod_vctrl_exec_g_ext_ctrls(struct isp_mod *mod, void *data)
{
	return mod_vctrl_process_ext_ctrls(mod, data, v4l2_g_ext_ctrls);
}

static int mod_vctrl_exec_s_ext_ctrls(struct isp_mod *mod, void *data)
{
	return mod_vctrl_process_ext_ctrls(mod, data, v4l2_s_ext_ctrls);
}

static int mod_vctrl_exec_try_ext_ctrls(struct isp_mod *mod, void *data)
{
	return mod_vctrl_process_ext_ctrls(mod, data, v4l2_try_ext_ctrls);
}

static struct isp_mod_action_info vctrl_exec_actions[] = {
	ISP_MOD_ACTION(VCTRL_EXEC_QUERY_CTRL, mod_vctrl_exec_query_ctrl),
	ISP_MOD_ACTION(VCTRL_EXEC_QUERY_MENU, mod_vctrl_exec_query_menu),
	ISP_MOD_ACTION(VCTRL_EXEC_G_CTRL, mod_vctrl_exec_g_ctrl),
	ISP_MOD_ACTION(VCTRL_EXEC_S_CTRL, mod_vctrl_exec_s_ctrl),
	ISP_MOD_ACTION(VCTRL_EXEC_QUERY_EXT_CTRL,
				   mod_vctrl_exec_query_ext_ctrl),
	ISP_MOD_ACTION(VCTRL_EXEC_G_EXT_CTRLS, mod_vctrl_exec_g_ext_ctrls),
	ISP_MOD_ACTION(VCTRL_EXEC_S_EXT_CTRLS, mod_vctrl_exec_s_ext_ctrls),
	ISP_MOD_ACTION(VCTRL_EXEC_TRY_EXT_CTRLS, mod_vctrl_exec_try_ext_ctrls),
};

static int isp_mod_vctrl_init(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_vctrl_cleanup(struct isp_mod *mod)
{
	return RTS_ISP_OK;
}

static int isp_mod_vctrl_add_ctrl(struct isp_mod *mod, void *phandler)
{
	struct isp_mod_vctrl *vctrl = get_mod(mod, vctrl);

	vctrl->handler = phandler;
	return RTS_ISP_OK;
}

static struct isp_mod_vctrl vctrl;

int isp_register_mod_vctrl(void)
{
	isp_memset(&vctrl, 0, sizeof(vctrl));
	vctrl.mod.id = MOD_VCTRL;
	vctrl.mod.name = "vctrl";
	vctrl.mod.init = isp_mod_vctrl_init;
	vctrl.mod.cleanup = isp_mod_vctrl_cleanup;
	vctrl.mod.add_ctrl = isp_mod_vctrl_add_ctrl;
	vctrl.mod.exec_actions = vctrl_exec_actions;
	vctrl.mod.exec_actions_num = ARRAY_SIZE(vctrl_exec_actions);
	vctrl.mod.virtual = RTS_ISP_TRUE;
	vctrl.mod.size = sizeof(vctrl);
	return isp_mod_register(&vctrl.mod);
}

void isp_unregister_mod_vctrl(void)
{
	isp_mod_unregister(&vctrl.mod);
}
