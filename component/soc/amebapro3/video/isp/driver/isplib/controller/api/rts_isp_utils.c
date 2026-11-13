/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <rts_isp.h>
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_driver.h>
#include <isp_client_message.h>
#include <isp_mod.h>
#include <isp_mod_global.h>
#include <isp_mod_sensor.h>
#include <isp_mod_mcrop.h>
#include <isp_mod_iq.h>
#include <isp_mod_control.h>
#include <isp_mod_blc.h>
#include <isp_mod_eeh.h>

int rts_isp_get_image_size(uint32_t isp_id, struct rts_isp_image_size *size)
{
	int ret;
	struct isp_front_size front_size;

	if (!size) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_client_message_simple(isp_id, MOD_SENSOR,
									SENSOR_INFO_FRONT_SIZE, &front_size,
									sizeof(front_size));
	if (ret) {
		return ret;
	}
	size->sensor_bit_depth = front_size.bit_depth;
	size->sensor_size = front_size.snr;
	size->isp_middle_size = front_size.isp;

	return isp_client_message_simple(isp_id, MOD_MCROP, MCROP_INFO_SIZE,
									 &size->isp_final_size,
									 sizeof(size->isp_final_size));
}

int rts_isp_get_bypass(uint32_t isp_id, struct rts_isp_bypass *bypass)
{
	return isp_client_message_simple(isp_id, MOD_GLOBAL,
									 GLOBAL_EXEC_GET_BYPASS, bypass,
									 sizeof(*bypass));
}

int rts_isp_set_bypass(uint32_t isp_id, struct rts_isp_bypass *bypass)
{
	return isp_client_message_simple(isp_id, MOD_GLOBAL,
									 GLOBAL_EXEC_SET_BYPASS, bypass,
									 sizeof(*bypass));
}

int rts_isp_get_blc_position(uint32_t isp_id)
{
	int ret;
	int32_t position; /* enum rts_isp_blc_position */

	ret = isp_client_message_simple(isp_id, MOD_BLC, BLC_INFO_POSITION,
									&position, sizeof(position));
	if (ret) {
		return ret;
	}
	return position;
}

const char *rts_isp_get_bound_sensor_name(uint32_t isp_id)
{
	int ret;
	static char name[_ISP_ID_NUM][32];

	if (isp_id >= _ISP_ID_NUM) {
		return NULL;
	}

	ret = isp_client_message_simple(isp_id, MOD_SENSOR,
									SENSOR_INFO_BOUND_NAME, name[isp_id],
									sizeof(name[isp_id]));
	if (ret) {
		return NULL;
	}
	return name[isp_id];
}

int rts_av_isp_get_plugins_version(uint32_t isp_id,
								   struct rts_isp_plugins_version *version)
{
	return isp_client_message_simple(isp_id, MOD_IQ,
									 IQ_INFO_PLUGINS_VERSION, version,
									 sizeof(*version));
}

int rts_av_isp_set_stream_watchdog(uint32_t isp_id, uint32_t timeout)
{
	return isp_client_message_simple(isp_id, MOD_CONTROL,
									 CONTROL_EXEC_SET_WATCHDOG, &timeout,
									 sizeof(timeout));
}

int rts_isp_set_time_tracker(uint32_t isp_id, int on)
{
	return isp_client_message_simple(
			   isp_id, MOD_CONTROL, CONTROL_EXEC_SET_TRACK, &on, sizeof(on));
}

int rts_av_isp_get_scene(uint32_t isp_id, int32_t *scene)
{
	if (!scene) {
		return -RTS_ISP_EINVAL;
	}
	return isp_client_message_simple(isp_id, MOD_IQ, IQ_EXEC_GET_SCENE,
									 scene, sizeof(*scene));
}

int rts_av_isp_set_scene(uint32_t isp_id, int32_t scene)
{
	return isp_client_message_simple(isp_id, MOD_IQ, IQ_EXEC_SET_SCENE,
									 &scene, sizeof(scene));
}
#ifdef ISP_CPU_MODE
int rts_isp_test_preview(uint32_t isp_id, int fps)
{
	return isp_os_cdriver_test_preview(isp_id, fps);
}
#else
int rts_isp_test_preview(uint32_t isp_id, int fps)
{
	return isp_os_driver_set_fps(fps);
}
#endif

int rts_isp_get_eeh_position(uint32_t isp_id)
{
	int ret;
	int32_t position; /* enum rts_isp_eeh_position */

	ret = isp_client_message_simple(isp_id, MOD_EEH, EEH_INFO_POSITION,
									&position, sizeof(position));
	if (ret) {
		return ret;
	}
	return position;
}
