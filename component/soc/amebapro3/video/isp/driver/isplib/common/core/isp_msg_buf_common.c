/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_errno.h>
#include <isp_os_mem.h>
#include "isp_msg_buf_common.h"

struct isp_msg_buf *isp_msg_new(void *data, uint32_t len, int shared)
{
	uint32_t total;
	struct isp_msg_buf *msg;

	if (!data && len && shared) {
		return NULL;
	}
	if (data && !len) {
		return NULL;
	}

	total = shared ? sizeof(*msg) : sizeof(*msg) + len;
	msg = isp_malloc(total);
	if (!msg) {
		return NULL;
	}

	isp_memset(&msg->hdr, 0, sizeof(msg->hdr));
	msg->hdr.msg_len = sizeof(msg->hdr) + len;
	if (shared) {
		msg->refcnt = -1;
		msg->data = data;
	} else {
		msg->refcnt = 1;
		msg->data = len ? msg + 1 : NULL;
		if (data && len) {
			isp_memcpy(msg->data, data, len);
		}
	}

	return msg;
}

struct isp_msg_buf *isp_msg_get(struct isp_msg_buf *msg)
{
	struct isp_msg_buf *new;

	if (msg->refcnt == -1) {
		new = isp_msg_new(msg->data, isp_msg_data_len(&msg->hdr), 0);
		if (!new) {
			return NULL;
		}
		new->hdr = msg->hdr;
		return new;
	}
	msg->refcnt += 1;
	return msg;
}

void isp_msg_put(struct isp_msg_buf *msg)
{
	switch (msg->refcnt) {
	case -1:
	case 1:
		isp_free(msg);
		break;
	default:
		msg->refcnt--;
	}
}
