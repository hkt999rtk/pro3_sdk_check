/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_time.h>
#include <isp_os_thread.h>
#include <isp_os_driver.h>
#include <isp_top.h>

int rts_av_isp_init(void)
{
	return isp_top_init();
}

int rts_av_isp_cleanup(void)
{
	return isp_top_cleanup();
}

int rts_av_isp_start(void)
{
	return isp_top_start();
}

int rts_av_isp_stop(void)
{
	return isp_top_stop();
}

int32_t rts_av_isp_get_status(void)
{
	return isp_top_status();
}

void rts_av_isp_open_log(int32_t behavior)
{
}

int rts_av_isp_register_plugin(const struct rts_isp_plugin *plug)
{
	return isp_top_plugin_register(plug);
}

int rts_av_isp_unregister_plugin(int plug_id)
{
	return isp_top_plugin_unregister(plug_id);
}

int rts_av_isp_get_plugin(struct rts_isp_plugin *plug)
{
	return isp_top_plugin_get(plug);
}

int rts_av_isp_check_plugin(uint32_t isp_id, int plug_id)
{
	return isp_top_plugin_check(isp_id, plug_id);
}

int rts_av_isp_bind_plugin(uint32_t isp_id, int plug_id)
{
	return isp_top_plugin_bind(isp_id, plug_id);
}

int rts_av_isp_unbind_plugin(uint32_t isp_id, int plug_id)
{
	return isp_top_plugin_unbind(isp_id, plug_id);
}
