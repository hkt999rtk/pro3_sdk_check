/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_driver.h>
#include <isp_os_thread.h>
#include <isp_utils.h>
#include "isp_mod.h"
#include "isp_core.h"
#include "isp_tracker.h"
#include "isp_top.h"
#include "isp_plugin.h"
#include "isp_sensor.h"
#include "isp_notify.h"
#include "isp_poll.h"
#include "isp_main.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_CORE

struct isp_top {
	isp_core_t core[_ISP_ID_NUM];
	int32_t status; /* enum rts_isp_status */
};

static int isp_top_core_stop(struct isp_top *isp);
static int top_core_cleanup(struct isp_top *isp);

static struct isp_top *g_isp;
static isp_os_thread_mutex_t g_isp_lock;
static struct isp_plugin_manager *g_pluginm;

static int top_core_init(struct isp_top *isp)
{
	int ret;
	int i;

	if (!isp) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < _ISP_ID_NUM; i++) {
		ret = isp_core_create(&isp->core[i], i);
		if (ret) {
			isp_perror(ret, "create isp core%d failed", i);
			goto err;
		}
	}
	ret = isp_register_all_modules();
	if (ret) {
		return ret;
	}
	for (i = 0; i < _ISP_ID_NUM; i++) {
		ret = isp_core_init(isp->core[i]);
		if (ret) {
			isp_perror(ret, "init isp core%d failed", i);
			goto err;
		}
	}

	return RTS_ISP_OK;

err:
	top_core_cleanup(isp);
	return ret;
}

static int top_core_cleanup(struct isp_top *isp)
{
	int i;

	if (!isp) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < _ISP_ID_NUM; i++) {
		isp_core_cleanup(isp->core[i]);
	}
	isp_unregister_all_modules();
	for (i = 0; i < _ISP_ID_NUM; i++) {
		isp_core_release(&isp->core[i]);
	}

	return RTS_ISP_OK;
}

static int top_core_start(struct isp_top *isp)
{
	int ret;
	int i;

	if (!isp) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < _ISP_ID_NUM; i++) {
		if (!isp->core[i]) {
			ret = -RTS_ISP_ENOTREADY;
			goto err;
		}
		ret = isp_core_start(isp->core[i]);
		if (ret) {
			goto err;
		}
	}
	return RTS_ISP_OK;

err:
	isp_perror(ret, "isp core run fail");
	isp_top_core_stop(isp);
	return ret;
}

static int isp_top_core_stop(struct isp_top *isp)
{
	int i;

	if (!isp) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < _ISP_ID_NUM; i++)
		if (isp->core[i]) {
			isp_core_stop(isp->core[i]);
		}

	return RTS_ISP_OK;
}

static struct isp_top *isp_top_get_unlock(void)
{
	return g_isp;
}

static void isp_top_lock_init(void)
{
	isp_os_thread_mutex_init(&g_isp_lock);
}

static void isp_top_lock(void)
{
	isp_os_thread_mutex_lock(&g_isp_lock);
}

static void isp_top_unlock(void)
{
	isp_os_thread_mutex_unlock(&g_isp_lock);
}

static struct isp_top *isp_top_get(void)
{
	isp_top_lock();
	if (!g_isp || isp_top_get_status() == RTS_ISP_UNINITIALIZED) {
		isp_top_unlock();
		return NULL;
	}
	return g_isp;
}

static void isp_top_put(struct isp_top *isp)
{
	if (isp && isp == g_isp) {
		isp_top_unlock();
	}
}

static struct isp_plugin_manager *isp_get_pluginm(void)
{
	if (!g_pluginm) {
		return NULL;
	}
	return g_pluginm;
}

void isp_top_lock_internal(void)
{
	isp_top_get();
}

void isp_top_unlock_internal(void)
{
	isp_top_put(isp_top_get_unlock());
}

int isp_top_init(void)
{
	int ret;
	struct isp_plugin_manager *pluginm;

	isp_top_lock_init();
	time_tracker_set_rt();
	isp_top_lock();
	if (g_pluginm) {
		isp_top_unlock();
		return RTS_ISP_OK;
	}
	if (!isp_calloc_item(pluginm)) {
		isp_top_unlock();
		return -RTS_ISP_ENOMEM;
	}
	ret = isp_server_init();
	if (ret) {
		isp_free(pluginm);
		isp_top_unlock();
		return ret;
	}

	// isp_top_set_debug_select(0xFFFFFFFF);

	debug("debug log system ready\r\n");
	if (g_isp) {
		int32_t status = isp_top_get_status();

		if (status != RTS_ISP_UNINITIALIZED) {
			isp_top_unlock();
			return RTS_ISP_OK;
		} else {
			isp_free(pluginm);
			isp_top_unlock();
			return -RTS_ISP_EAGAIN;
		}
	}

	if (!isp_calloc_item(g_isp)) {
		isp_free(pluginm);
		isp_top_unlock();
		return -RTS_ISP_ENOMEM;
	}

	isp_os_driver_pm_enable(RTS_ISP_TRUE);
	ret = top_core_init(g_isp);
	isp_os_driver_pm_enable(RTS_ISP_FALSE);
	if (ret) {
		goto err;
	}
	isp_top_set_status(RTS_ISP_STOPPED);

	ret = isp_plugin_init(pluginm);
	if (ret) {
		goto err_plugin_init;
	}
	g_pluginm = pluginm;
	isp_info("rts isp init ok\n");

	isp_top_unlock();
	return RTS_ISP_OK;

err_plugin_init:
	isp_top_cleanup();
err:
	isp_free(g_isp);
	isp_free(pluginm);
	isp_top_unlock();
	return ret;
}

int isp_top_cleanup(void)
{
	int status;

	isp_top_lock();
	status = isp_top_get_status();

	if (!g_isp || !g_pluginm) {
		isp_top_unlock();
		return RTS_ISP_OK;
	}
	if (status < 0)
		/* error occurs, still need to run cleanup flow */;
	else if (status >= RTS_ISP_RUNNING) {
		isp_top_unlock();
		return -RTS_ISP_EBUSY;
	}

	isp_plugin_cleanup(g_pluginm);
	isp_free(g_pluginm);
	isp_top_set_status(RTS_ISP_UNINITIALIZED);
	isp_os_driver_pm_enable(RTS_ISP_TRUE);
	top_core_cleanup(g_isp);
	isp_os_driver_pm_enable(RTS_ISP_FALSE);
	isp_free(g_isp);
	isp_info("rts isp cleanup ok\n");

	isp_top_unlock();
	return RTS_ISP_OK;
}

int isp_top_start(void)
{
	int ret = RTS_ISP_OK;
	struct isp_top *isp;
	int status;

	isp = isp_top_get();
	if (!isp) {
		return -RTS_ISP_ENOTREADY;
	}

	status = isp_top_get_status();
	if (status < 0) {
		isp_top_put(isp);
		return status;
	}
	if (status < RTS_ISP_STOPPED) {
		isp_top_put(isp);
		return -RTS_ISP_ENOTREADY;
	}
	if (status >= RTS_ISP_RUNNING) {
		isp_top_put(isp);
		return -RTS_ISP_EBUSY;
	}

	isp_os_driver_pm_enable(RTS_ISP_TRUE);
	ret = top_core_start(isp);
	isp_os_driver_pm_enable(RTS_ISP_FALSE);
	if (ret) {
		isp_top_put(isp);
		return ret;
	}
	isp_top_set_status(RTS_ISP_RUNNING);
	isp_info("rts isp start ok\n");
	isp_top_put(isp);
	ret = isp_server_start();
	isp_server_cleanup();
	isp_top_stop();
	return ret;
}

int isp_top_stop(void)
{
	int ret;
	struct isp_top *isp;

	isp = isp_top_get_unlock();
	if (!isp) {
		return -RTS_ISP_ENOTREADY;
	}
	if (isp_top_get_status() < RTS_ISP_RUNNING) {
		return RTS_ISP_OK;
	}
	if (isp_top_get_status() > RTS_ISP_RUNNING) {
		return -RTS_ISP_EBUSY;
	}

	isp_os_driver_pm_enable(RTS_ISP_TRUE);
	ret = isp_top_core_stop(isp);
	isp_os_driver_pm_enable(RTS_ISP_FALSE);
	if (ret) {
		return ret;
	}

	isp_top_set_status(RTS_ISP_STOPPED);

	isp_info("rts isp stop ok\n");

	return RTS_ISP_OK;
}

int isp_top_status(void)
{
	enum rts_isp_status status;
	struct isp_top *isp = isp_top_get();

	if (!isp) {
		return RTS_ISP_UNINITIALIZED;
	}

	status = isp_top_get_status();
	isp_top_put(isp);

	return status;
}

int isp_top_get_status(void)
{
	struct isp_top *isp = isp_top_get_unlock();

	if (!isp) {
		return RTS_ISP_UNINITIALIZED;
	}
	return isp->status;
}

void isp_top_set_status(int32_t status)
{
	struct isp_top *isp = isp_top_get_unlock();

	if (isp) {
		isp->status = status;
	}
}

void *isp_top_get_core(uint32_t isp_id)
{
	struct isp_top *isp = isp_top_get_unlock();

	if (!isp || isp_id >= _ISP_ID_NUM) {
		return NULL;
	}
	return isp->core[isp_id];
}

struct isp_mod *isp_top_get_mod(int isp_id, int mod_id)
{
	struct isp_mod *mod;
	isp_core_t core;

	core = isp_top_get_core(isp_id);
	if (!core) {
		return NULL;
	}
	if (isp_core_get_mod(core, mod_id, &mod)) {
		return NULL;
	}
	return mod;
};

static inline int check_plug_id(int plug_id)
{
	if (plug_id < RTS_ISP_PLUG_ID_MIN || plug_id > RTS_ISP_PLUG_ID_MAX) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static inline int check_plug_bin_type(int bin_type)
{
	if (bin_type < RTS_ISP_PLUG_BIN_TYPE_FILE ||
		bin_type > RTS_ISP_PLUG_BIN_TYPE_STATIC) {
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int check_plugin_status(void)
{
	int status;

	status = isp_top_get_status();
	if (status < 0) {
		return status;
	}
	if (status <= RTS_ISP_UNINITIALIZED) {
		return -RTS_ISP_ENOTREADY;
	}
	if (status >= RTS_ISP_RUNNING) {
		return -RTS_ISP_EBUSY;
	}
	return RTS_ISP_OK;
}

int isp_top_plugin_register(const struct rts_isp_plugin *plug)
{
	int ret;
	struct isp_plugin_manager *pluginm;

	if (!plug || check_plug_id(plug->id) ||
		check_plug_bin_type(plug->bin_type)) {
		return -RTS_ISP_EINVAL;
	}

	isp_top_lock();
	pluginm = isp_get_pluginm();
	if (!pluginm) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	ret = check_plugin_status();
	if (ret) {
		goto out;
	}
	ret = isp_plugin_register(pluginm, plug);
out:
	isp_top_unlock();
	return ret;
}

int isp_top_plugin_unregister(int plug_id)
{
	int ret;
	struct isp_plugin_manager *pluginm;

	if (check_plug_id(plug_id)) {
		return -RTS_ISP_EINVAL;
	}

	isp_top_lock();
	pluginm = isp_get_pluginm();
	if (!pluginm) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	ret = check_plugin_status();
	if (ret) {
		goto out;
	}
	ret = isp_plugin_unregister(pluginm, plug_id);
out:
	isp_top_unlock();
	return ret;
}

int isp_top_plugin_get(struct rts_isp_plugin *plug)
{
	int ret;
	struct isp_plugin_manager *pluginm;

	if (!plug || check_plug_id(plug->id)) {
		return -RTS_ISP_EINVAL;
	}

	isp_top_lock();
	pluginm = isp_get_pluginm();
	if (!pluginm) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	ret = check_plugin_status();
	if (ret) {
		goto out;
	}
	ret = isp_plugin_get(pluginm, plug);
out:
	isp_top_unlock();
	return ret;
}

int isp_top_plugin_check(uint32_t isp_id, int plug_id)
{
	int ret = RTS_ISP_OK;
	int plug_type;
	int backup_bind_id;
	struct isp_plugin_manager *pluginm;

	if (isp_id >= _ISP_ID_NUM || check_plug_id(plug_id)) {
		return -RTS_ISP_EINVAL;
	}
	if (plug_id < RTS_ISP_PLUG_SENSOR_ID_MIN ||
		plug_id > RTS_ISP_PLUG_SENSOR_ID_MAX) {
		return RTS_ISP_OK;
	}

	isp_top_lock();
	pluginm = isp_get_pluginm();
	if (!pluginm) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	ret = check_plugin_status();
	if (ret) {
		goto out;
	}
	backup_bind_id =
		isp_plugin_get_bind(pluginm, RTS_ISP_PLUG_TYPE_SENSOR, isp_id);

	ret = isp_plugin_bind(pluginm, plug_id, isp_id);
	if (ret) {
		goto out;
	}
	plug_type = rts_isp_plugin_id_to_type(plug_id);
	if (plug_type == RTS_ISP_PLUG_TYPE_SENSOR) {
		ret = isp_sensor_check(isp_id);
	}

	if (backup_bind_id >= RTS_ISP_PLUG_ID_MIN &&
		backup_bind_id <= RTS_ISP_PLUG_ID_MAX) {
		isp_plugin_bind(pluginm, backup_bind_id, isp_id);
	} else {
		isp_plugin_unbind(pluginm, plug_id, isp_id);
	}
out:
	isp_top_unlock();
	return ret;
}

int isp_top_plugin_bind(uint32_t isp_id, int plug_id)
{
	int ret = RTS_ISP_OK;
	int status;
	int plug_type;
	int cur_bind_id;
	struct isp_plugin_manager *pluginm;

	if (isp_id >= _ISP_ID_NUM || check_plug_id(plug_id)) {
		return -RTS_ISP_EINVAL;
	}

	isp_top_lock();
	pluginm = isp_get_pluginm();
	if (!pluginm) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	status = isp_top_get_status();
	if (status <= 0) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	plug_type = rts_isp_plugin_id_to_type(plug_id);
	if (status == RTS_ISP_STREAMING &&
		plug_type == RTS_ISP_PLUG_TYPE_SENSOR) {
		isp_error("status is streaming, can not bind sensor\n");
		ret = -RTS_ISP_EBUSY;
		goto out;
	}
	cur_bind_id = isp_plugin_get_bind(pluginm, plug_type, isp_id);
	if (cur_bind_id == plug_id) {
		ret = RTS_ISP_OK;
		goto out;
	}
	ret = isp_plugin_bind(pluginm, plug_id, isp_id);
	if (ret) {
		goto out;
	}
	if (status >= RTS_ISP_RUNNING) {
		isp_os_driver_pm_enable(RTS_ISP_TRUE);
		ret = isp_core_notify(isp_top_get_core(isp_id),
							  ISP_NOTIFY_PLUGIN_UPDATE, &plug_type,
							  sizeof(plug_type));
		if (ret) {
			isp_plugin_bind(pluginm, cur_bind_id, isp_id);
			isp_core_notify(isp_top_get_core(isp_id),
							ISP_NOTIFY_PLUGIN_UPDATE, &plug_type,
							sizeof(plug_type));
		}
		isp_os_driver_pm_enable(RTS_ISP_FALSE);
	}
out:
	isp_top_unlock();
	return ret;
}

int isp_top_plugin_unbind(uint32_t isp_id, int plug_id)
{
	int ret = RTS_ISP_OK;
	int status;
	struct isp_plugin_manager *pluginm;

	if (isp_id >= _ISP_ID_NUM || check_plug_id(plug_id)) {
		return -RTS_ISP_EINVAL;
	}

	isp_top_lock();
	pluginm = isp_get_pluginm();
	if (!pluginm) {
		ret = -RTS_ISP_ENOTREADY;
		goto out;
	}
	status = isp_top_get_status();
	if (status < 0) {
		ret = status;
		goto out;
	}
	if (status >= RTS_ISP_RUNNING) {
		ret = -RTS_ISP_EBUSY;
		goto out;
	}
	ret = isp_plugin_unbind(pluginm, plug_id, isp_id);
out:
	isp_top_unlock();
	return ret;
}

int isp_top_set_debug_select(uint32_t debug_select)
{
	isp_os_log_set_debug_select(debug_select);
	return RTS_ISP_OK;
}
