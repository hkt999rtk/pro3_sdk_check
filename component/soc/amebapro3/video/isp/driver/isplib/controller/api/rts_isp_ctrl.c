/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <rts_isp_ctrl.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_client_message.h>
#include <isp_mod.h>
#include <isp_mod_vctrl.h>

int rts_isp_v4l2_query_ctrl(uint32_t isp_id, struct isp_v4l2_queryctrl *qc)
{
	return isp_client_message_simple(
			   isp_id, MOD_VCTRL, VCTRL_EXEC_QUERY_CTRL, qc, sizeof(*qc));
}

int rts_isp_v4l2_query_menu(uint32_t isp_id, struct isp_v4l2_querymenu *qm)
{
	return isp_client_message_simple(
			   isp_id, MOD_VCTRL, VCTRL_EXEC_QUERY_MENU, qm, sizeof(*qm));
}

int rts_isp_v4l2_g_ctrl(uint32_t isp_id, struct isp_v4l2_control *control)
{
	return isp_client_message_simple(isp_id, MOD_VCTRL, VCTRL_EXEC_G_CTRL,
									 control, sizeof(*control));
}

int rts_isp_v4l2_s_ctrl(uint32_t isp_id, struct isp_v4l2_control *control)
{
	return isp_client_message_simple(isp_id, MOD_VCTRL, VCTRL_EXEC_S_CTRL,
									 control, sizeof(*control));
}

static int v4l2_ext_ctrl_serialize(struct isp_v4l2_ext_controls *ecs,
								   struct isp_msg_buf *msg)
{
	int i;
	int ret;
	void *buf;
	void *end;
	struct isp_mod_vctrl_ext_controls *buf_ecs;

	if (!ecs || !msg) {
		return -RTS_ISP_EINVAL;
	}

	buf = msg->data;
	end = buf + isp_msg_data_len(&msg->hdr);

	ret = get_buf_ptr(buf_ecs, buf, end);
	if (ret) {
		return ret;
	}
	buf_ecs->ctrl_class = ecs->ctrl_class;
	buf_ecs->count = ecs->count;
	buf_ecs->error_idx = ecs->error_idx;
	buf_ecs->request_fd = ecs->request_fd;

	ret = copy_from_src(buf, ecs->controls,
						sizeof(*ecs->controls) * ecs->count, end);
	if (ret) {
		return ret;
	}
	for (i = 0; i < ecs->count; i++) {
		if (!ecs->controls[i].size) {
			continue;
		}

		ret = copy_from_src(buf, ecs->controls[i].ptr,
							ecs->controls[i].size, end);
		if (ret) {
			return ret;
		}
	}

	msg->hdr.msg_len = buf - msg->data + sizeof(msg->hdr);
	msg->hdr.capacity = msg->hdr.msg_len;

	return RTS_ISP_OK;
}

static int v4l2_ext_ctrl_deserialize(struct isp_v4l2_ext_controls *ecs,
									 struct isp_msg_buf *msg)
{
	int ret;
	int i;
	void *buf;
	void *end;
	void **ptr;
	struct isp_mod_vctrl_ext_controls *buf_ecs;

	if (!msg || !ecs) {
		return -RTS_ISP_EINVAL;
	}

	if (msg->hdr.ret_val) {
		return msg->hdr.ret_val;
	}

	end = msg->data + isp_msg_data_len(&msg->hdr);
	buf = msg->data;

	ret = get_buf_ptr(buf_ecs, buf, end);
	if (ret) {
		return ret;
	}
	if (ecs->count != buf_ecs->count) {
		return -RTS_ISP_EINVAL;
	}
	ecs->ctrl_class = buf_ecs->ctrl_class;
	ecs->count = buf_ecs->count;
	ecs->error_idx = buf_ecs->error_idx;
	ecs->request_fd = buf_ecs->request_fd;

	if (!isp_calloc_array(ptr, ecs->count)) {
		return -RTS_ISP_ENOMEM;
	}
	for (i = 0; i < ecs->count; i++)
		if (ecs->controls->size) {
			ptr[i] = ecs->controls[i].ptr;
		}
	ret = copy_to_src(ecs->controls, buf,
					  sizeof(*ecs->controls) * ecs->count, end);
	if (ret) {
		goto out;
	}

	for (i = 0; i < ecs->count; i++) {
		if (ecs->controls[i].size) {
			ecs->controls[i].ptr = ptr[i];
			ret = copy_to_src(ecs->controls[i].ptr, buf,
							  ecs->controls[i].size, end);
			if (ret) {
				goto out;
			}
		}
	}
	ret = RTS_ISP_OK;
out:
	isp_free(ptr);
	return ret;
}

static int __process_ext_ctrls(uint32_t isp_id, uint32_t action,
							   struct isp_v4l2_ext_controls *ecs)
{
	int ret;
	struct isp_msg_buf *msg;

	if (isp_id >= _ISP_ID_NUM || !ecs) {
		return -RTS_ISP_EINVAL;
	}

	msg = isp_msg_new(NULL, 2048, 0);
	if (!msg) {
		return -RTS_ISP_ENOMEM;
	}

	ret = v4l2_ext_ctrl_serialize(ecs, msg);
	if (ret < 0) {
		goto out;
	}

	msg->hdr.flags = ISP_MSG_TYPE_RAW;
	msg->hdr.isp_id = isp_id;
	msg->hdr.mod_id = MOD_VCTRL;
	msg->hdr.action = action;
	msg->hdr.ret_val = -1;

	ret = isp_client_message_process(msg);
	if (ret) {
		goto out;
	}
	ret = v4l2_ext_ctrl_deserialize(ecs, msg);
out:
	isp_release(msg, isp_msg_put);
	return ret;
}

int rts_isp_v4l2_query_ext_ctrl(uint32_t isp_id,
								struct isp_v4l2_query_ext_ctrl *qec)
{
	return isp_client_message_simple(isp_id, MOD_VCTRL,
									 VCTRL_EXEC_QUERY_EXT_CTRL, qec,
									 sizeof(*qec));
}

int rts_isp_v4l2_g_ext_ctrls(uint32_t isp_id, struct isp_v4l2_ext_controls *ecs)
{
	return __process_ext_ctrls(isp_id, VCTRL_EXEC_G_EXT_CTRLS, ecs);
}

int rts_isp_v4l2_s_ext_ctrls(uint32_t isp_id, struct isp_v4l2_ext_controls *ecs)
{
	return __process_ext_ctrls(isp_id, VCTRL_EXEC_S_EXT_CTRLS, ecs);
}

int rts_isp_v4l2_try_ext_ctrls(uint32_t isp_id,
							   struct isp_v4l2_ext_controls *ecs)
{
	return __process_ext_ctrls(isp_id, VCTRL_EXEC_TRY_EXT_CTRLS, ecs);
}
