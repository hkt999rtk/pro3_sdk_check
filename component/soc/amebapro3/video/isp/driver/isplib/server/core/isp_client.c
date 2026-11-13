/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_driver.h>
#include "isp_top.h"
#include "isp_mod.h"
#include "isp_core.h"
#include "isp_tracker.h"
#include "isp_notify.h"
#include "isp_statis.h"
#include "isp_client.h"
#include "isp_top.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_STREAM

#define MAX_READ_CNT 16

struct isp_pipeline_client {
	isp_io_handle_t io;
	uint32_t isp_id;
};

static int client_message_check_raw(struct isp_msg_buf *msg)
{
	struct isp_msg_hdr *hdr;

	hdr = &msg->hdr;
	if (hdr->mod_id == 0) {
		return -RTS_ISP_EINVAL;
	}
	if (simple_write_msg_size(hdr->action) !=
		sizeof(*hdr) + sizeof(void *) ||
		simple_read_msg_size(hdr->action) != sizeof(*hdr)) {
		return -RTS_ISP_EINVAL;
	}

	return RTS_ISP_OK;
}

static int client_message_check_normal(struct isp_msg_buf *msg)
{
	int action;
	struct isp_msg_hdr *hdr;

	hdr = &msg->hdr;
	action = hdr->action;
	if (hdr->msg_len != simple_write_msg_size(action) ||
		(hdr->capacity != simple_read_msg_size(action) &&
		 hdr->capacity != hdr->msg_len)) {
		return -RTS_ISP_ERANGE;
	}

	return RTS_ISP_OK;
}

static int client_message_check(struct isp_msg_buf *msg)
{
	if (msg->hdr.flags & ISP_MSG_TYPE_RAW) {
		if (client_message_check_raw(msg)) {
			return -RTS_ISP_EINVAL;
		}
	} else {
		if (client_message_check_normal(msg)) {
			return -RTS_ISP_EINVAL;
		}
	}
	return RTS_ISP_OK;
}

int client_raw_message_execute(void *data)
{
	int ret;
	struct isp_mod *mod;
	struct isp_msg_hdr *hdr;
	struct isp_msg_buf *msg = data;

	isp_top_lock_internal();
	hdr = (struct isp_msg_hdr *)&msg->hdr;
	mod = isp_top_get_mod(hdr->isp_id, hdr->mod_id);
	ret = isp_mod_do_action(mod, hdr->action, &msg, sizeof(msg));

	debug("%s isp: %u mod: 0x%x action: 0x%x ret = %d\n", __func__,
		  hdr->isp_id, hdr->mod_id, hdr->action, ret);

	isp_top_unlock_internal();
	return ret;
}

int client_normal_message_execute(void *data)
{
	int ret;
	struct isp_mod *mod;
	struct isp_msg_buf *msg = data;
	struct isp_msg_hdr *hdr;

	if (!msg) {
		return -RTS_ISP_EINVAL;
	}

	isp_top_lock_internal();
	hdr = (struct isp_msg_hdr *)&msg->hdr;
	mod = isp_top_get_mod(hdr->isp_id, hdr->mod_id);
	ret = isp_mod_do_action(mod, hdr->action, msg->data,
							ISP_AID_SIZE(hdr->action));

	debug("%s isp: %u mod: 0x%x action: 0x%x ret = %d\n", __func__,
		  hdr->isp_id, hdr->mod_id, hdr->action, ret);

	isp_top_unlock_internal();
	return ret;
}

int isp_client_service(struct isp_msg_buf *msg)
{
	int ret;

	if (!msg) {
		return -RTS_ISP_EINVAL;
	}

	debug("isp client service api\n");

	ret = client_message_check(msg);
	if (ret) {
		return ret;
	}

	if (msg->hdr.flags & ISP_MSG_TYPE_RAW) {
		ret = client_raw_message_execute(msg);
	} else {
		ret = client_normal_message_execute(msg);
		ret = isp_msg_to_resp(msg, ret,
							  simple_read_msg_size(msg->hdr.action));
	}

	return ret;
}

int pipeline_client_service(isp_io_handle_t *io, uint32_t events)
{
	int ret;
	isp_core_t core;
	struct isp_statis *statis;
	struct isp_pipeline_client *client;
	const struct rts_isp_statis *all = NULL;
	struct rts_isp_statis_data statis_data;

	client = container_of(io, struct isp_pipeline_client, io);
	ret = isp_os_driver_get_statis(&statis_data, sizeof(statis_data));
	if (ret) {
		if (ret != -RTS_ISP_EAGAIN) {
			return ret;
		} else {
			return RTS_ISP_OK;
		}
	}

	isp_top_lock_internal();
	time_tracker_begin("stat_notify");
	core = isp_top_get_core(client->isp_id);
	statis = isp_core_get_statis(core);
	ret = isp_statis_update(statis, &statis_data);
	if (ret) {
		isp_os_driver_put_statis(statis_data.buf_id);
		goto out;
	}
	ret = isp_statis_get_all(statis, &all);
	if (ret) {
		isp_os_driver_put_statis(statis_data.buf_id);
		goto out;
	}
	isp_statis_ref(statis, all);
	ret = isp_core_notify(core, ISP_NOTIFY_STATIS_DONE, (void *)all,
						  sizeof(*all));
	isp_statis_unref(statis, all);
out:
	time_tracker_end("stat_notify");
	if (ret) {
		isp_perror(ret, "statis done handle");
	}
	isp_top_unlock_internal();
	return RTS_ISP_OK;
}

static void pipeline_client_release(isp_io_handle_t *io)
{
	struct isp_pipeline_client *client;

	if (!io) {
		return;
	}

	client = container_of(io, struct isp_pipeline_client, io);

	isp_io_cleanup(io);
	isp_free(client);

	debug("%s ok\n", __func__);
}

static int isp_client_add_pipeline(isp_poll_t p)
{
	int ret;
	isp_os_fd_t fd;
	struct isp_pipeline_client *client = NULL;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	fd = isp_os_driver_get_pfd();
	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	if (!isp_calloc_item(client)) {
		return -RTS_ISP_ENOMEM;
	}

	client->isp_id = 0;
	ret = isp_io_init(&client->io, p, fd, pipeline_client_service);
	if (ret) {
		goto err;
	}
	isp_io_event_add(&client->io, ISP_OS_POLL_IN);

	isp_io_set_release(&client->io, pipeline_client_release);

	return RTS_ISP_OK;

err:
	isp_free(client);
	isp_perror(ret, "add stream client fail");
	return ret;
}

int isp_client_add(isp_poll_t p)
{
	return isp_client_add_pipeline(p);
}
