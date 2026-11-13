/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_io.h>
#include "isp_msg_buf.h"

int isp_msg_to_resp(struct isp_msg_buf *msg, int retval, uint32_t msg_len)
{
	if (!msg) {
		return -RTS_ISP_EINVAL;
	}
	if (msg->hdr.flags & ISP_MSG_NO_RESPONSE) {
		return RTS_ISP_OK;
	}
	if (msg_len < sizeof(msg->hdr)) {
		isp_error("isp %d mod %d action %d reply msg len error\n",
				  msg->hdr.isp_id, msg->hdr.mod_id, msg->hdr.action);
		return -RTS_ISP_EFAULT;
	}
	if (retval) {
		msg->hdr.msg_len = sizeof(msg->hdr);
	} else {
		msg->hdr.msg_len = msg_len;
	}
	msg->hdr.ret_val = retval;
	return RTS_ISP_OK;
}
