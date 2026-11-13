/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_driver.h>
#include <isp_utils.h>
#include "isp_plugin.h"
#include "isp_sensor.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_SENSOR

/* Set when the probe left the sensor streaming; consumed by the next start. */
static uint8_t sensor_early_started[_ISP_ID_NUM];

static int sensor_check_ops(const struct rts_isp_sensor_ops *ops)
{
	if (sensor_magic(ops->api_version) != SENSOR_API_VERSION_MAGIC ||
		sensor_major_version(ops->api_version) !=
		SENSOR_API_MAJOR_VERSION) {
		isp_error("sensor version is unmatch, please rebuild it!\n");
		return -RTS_ISP_EINVAL;
	}
	if (!ops->get_info || !ops->get_init_info) {
		return -RTS_ISP_EPLUGIN;
	}
	if (!ops->get_tuned_again || !ops->get_tuned_dgain) {
		return -RTS_ISP_EPLUGIN;
	}
	if (!ops->get_exposure_gain_info) {
		return -RTS_ISP_EPLUGIN;
	}
	return RTS_ISP_OK;
}

int isp_sensor_check(uint32_t isp_id)
{
	int ret;
	const struct rts_isp_sensor_ops *ops;
	struct rts_isp_sensor_info info;

	if (isp_id > _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_sensor_get_ops(isp_id, &ops);
	if (ret) {
		return ret;
	}
	if (!ops->get_info || !ops->check) {
		ret = -RTS_ISP_ENOOPS;
		return ret;
	}
	if (ops->init) {
		ret = ops->init(isp_id);
		if (ret) {
			return ret;
		}
	}
	ret = ops->get_info(isp_id, &info);
	if (ret) {
		goto out_init;
	}
	ret = isp_os_driver_set_power(&info.power_up);
	if (ret) {
		goto out_power;
	}
	ret = ops->check(isp_id);
	if (!ret && ops->early_start && isp_id < _ISP_ID_NUM &&
		ops->early_start(isp_id) == RTS_ISP_OK) {
		/* Keep power: a power cycle would restart the sensor. */
		sensor_early_started[isp_id] = 1;
		goto out_init;
	}

out_power:
	isp_os_driver_set_power(&info.power_down);
out_init:
	if (ops->cleanup) {
		ops->cleanup(isp_id);
	}
	return ret;
}

int isp_sensor_take_early_started(uint32_t isp_id)
{
	int started;

	if (isp_id >= _ISP_ID_NUM) {
		return 0;
	}
	started = sensor_early_started[isp_id];
	sensor_early_started[isp_id] = 0;
	return started;
}

int isp_sensor_get_ops(uint32_t isp_id,
					   const struct rts_isp_sensor_ops **sensor_ops)
{
	const struct rts_isp_sensor_ops *ops;

	if (isp_id >= _ISP_ID_NUM || !sensor_ops) {
		return -RTS_ISP_EINVAL;
	}

	ops = isp_plugin_handle(isp_id, RTS_ISP_PLUG_TYPE_SENSOR);
	if (!ops) {
		return -RTS_ISP_ENOTREADY;
	}
	if (sensor_check_ops(ops)) {
		return -RTS_ISP_EPLUGIN;
	}

	*sensor_ops = ops;

	return RTS_ISP_OK;
}
