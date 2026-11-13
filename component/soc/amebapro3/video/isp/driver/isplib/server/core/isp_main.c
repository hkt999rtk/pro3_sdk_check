/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp_other.h>
#include <rts_isp_sensor.h>
#include <isp_os_io.h>
#include <isp_os_log.h>
#include <isp_os_driver.h>
#include <isp_os_thread.h>
#include <isp_utils.h>
#include "isp_client.h"
#include "isp_top.h"
#include "isp_main.h"

static isp_poll_t g_poll;

int isp_server_init(void)
{
	int ret;

	isp_os_log_open();
	ret = isp_os_driver_init();
	if (ret) {
		return ret;
	}
	ret = isp_poll_create(&g_poll);
	if (ret) {
		goto out;
	}
	ret = isp_client_add(g_poll);
	if (ret) {
		goto out;
	}
	isp_poll_update_time(g_poll);
	return 0;
out:
	isp_server_cleanup();
	return ret;
}

void isp_server_cleanup(void)
{
	isp_release(g_poll, isp_poll_release);
	isp_os_driver_cleanup();
	isp_os_log_close();
}

int isp_server_start(void)
{
	int ret;

	isp_info("rts isp server start\n");
	ret = isp_poll_loop_run(g_poll);
	isp_info("rts isp server stop\n");
	return ret;
}

void isp_server_stop(void)
{
	isp_poll_loop_stop(isp_top_get_poll());
}

void *isp_top_get_poll(void)
{
	return g_poll;
}
