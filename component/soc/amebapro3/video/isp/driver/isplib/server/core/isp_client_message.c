/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_errno.h>
#include <rts_isp_define.h>
#include <isp_os_mem.h>
#include <isp_os_driver.h>
#include "isp_client_message.h"
#include "isp_client.h"

int isp_client_message_process(struct isp_msg_buf *msg)
{
	int ret;

	if (!msg) {
		return -RTS_ISP_EINVAL;
	}
	if (msg->hdr.msg_len < sizeof(msg->hdr) ||
		msg->hdr.capacity < sizeof(msg->hdr) ||
		msg->hdr.capacity < msg->hdr.msg_len) {
		return -RTS_ISP_EINVAL;
	}

	isp_os_driver_pm_enable(true);
	ret = isp_client_service(msg);
	isp_os_driver_pm_enable(false);

	return ret ? ret : msg->hdr.ret_val;
}

int isp_client_message_simple(uint32_t isp_id, uint32_t mod_id, uint32_t action,
							  void *data, uint32_t len)
{
	int ret;
	struct isp_msg_buf *msg;

	if (isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}
	if (ISP_AID_HAS_DATA(action) && !data) {
		return -RTS_ISP_EINVAL;
	}
	if (ISP_AID_SIZE(action) != len) {
		return -RTS_ISP_EINVAL;
	}

	msg = isp_msg_new(data, len, 1);
	if (!msg) {
		return -RTS_ISP_ENOMEM;
	}

	msg->hdr.msg_len = simple_write_msg_size(action);
	msg->hdr.capacity = simple_msg_size(action);
	msg->hdr.isp_id = isp_id;
	msg->hdr.mod_id = mod_id;
	msg->hdr.action = action;
	msg->hdr.ret_val = -1;
	msg->hdr.reserved = 0;

	ret = isp_client_message_process(msg);

	isp_release(msg, isp_msg_put);
	return ret;
}

int isp_client_message_raw(uint32_t isp_id, uint32_t mod_id, uint32_t action,
						   void *data, uint32_t len, uint32_t capacity)
{
	int ret;
	struct isp_msg_buf *msg;

	if (isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	msg = isp_msg_new(data, len, 1);
	if (!msg) {
		return -RTS_ISP_ENOMEM;
	}

	msg->hdr.flags = ISP_MSG_TYPE_RAW;
	msg->hdr.msg_len = sizeof(struct isp_msg_hdr) + len;
	msg->hdr.capacity = sizeof(struct isp_msg_hdr) + capacity;
	msg->hdr.isp_id = isp_id;
	msg->hdr.mod_id = mod_id;
	msg->hdr.action = action;
	msg->hdr.ret_val = -1;
	msg->hdr.reserved = 0;

	ret = isp_client_message_process(msg);

	isp_release(msg, isp_msg_put);
	return ret;
}
