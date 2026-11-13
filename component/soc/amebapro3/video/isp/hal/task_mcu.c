#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_driver.h>
#include <rts_isp.h>
#include "ameba_soc.h"
#include "rts_camera.h"
#include "sensor.h"

unsigned char __vectors_start__[] = {0};
extern unsigned char __StackTop[];

const struct rts_isp_ae_algo *rts_isp_get_ae_algo(void);
const struct rts_isp_awb_algo *rts_isp_get_awb_algo(void);
const struct rts_isp_af_algo *rts_isp_get_af_algo(void);
const struct rts_isp_other_algo *rts_isp_get_other_algo(void);
const struct rts_isp_sensor_ops *rts_isp_get_sc401ai_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_os04a10_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_fixp_ops(void);

static int register_plugin(int id, void *entry, int bind, int check)
{
	int ret;
	struct rts_isp_plugin plug;

	plug.id = id;
	plug.bin_type = RTS_ISP_PLUG_BIN_TYPE_STATIC;
	plug.entry = entry;

	ret = rts_av_isp_register_plugin(&plug);

	if (ret) {
		return ret;
	}
	if (check) {
		ret = rts_av_isp_check_plugin(ISP0, id);
		if (ret < 0) {
			rts_av_isp_unregister_plugin(id);
			return ret;
		}
	}
	if (bind) {
		ret = rts_av_isp_bind_plugin(ISP0, id);
		if (ret) {
			rts_av_isp_unregister_plugin(id);
			return ret;
		}
	}

	return 0;
}

#if defined(CONFIG_PXP) && (CONFIG_PXP==1)
#define ISP_REAL_SENSOR 0
#else
#define ISP_REAL_SENSOR 1
#endif

#define REG_LOG_TAG "REG_SNR"

static int register_sensor_iq_sensor(struct sensor_params_t *snr_params)
{
	struct rts_isp_sensor_info snr_info;
	struct rts_isp_sensor_mode mode;
	struct rts_isp_sensor_init_info snr_ini_info;
	const struct rts_isp_sensor_ops *ops;
	uint32_t idx;
	int check = RTS_ISP_FALSE;

	ops = snr_params->get_ops();
	if (!ops) {
		RTK_LOGE(REG_LOG_TAG, "[%s] sensor ops NULL\r\n", __func__);
		return -RTS_ISP_EINVAL;
	}
	if (ops->get_info(ISP0, &snr_info)) {
		RTK_LOGE(REG_LOG_TAG, "[%s] get_info fail\r\n", __func__);
		return -RTS_ISP_EINVAL;
	}
	idx = snr_params->hdr;
	if (idx >= snr_info.modes.num) {
		idx = 0;
	}
	mode = snr_info.modes.mode[idx];
	if (ops->get_init_info(ISP0, &mode, &snr_ini_info)) {
		RTK_LOGE(REG_LOG_TAG, "[%s] get_init_info fail\r\n", __func__);
		return -RTS_ISP_EINVAL;
	}
	if (snr_ini_info.interface.interface == SNR_INTERFACE_NONE) {
		check = RTS_ISP_FALSE;
	} else {
		check = RTS_ISP_TRUE;
	}
	return register_plugin(RTS_ISP_PLUG_SENSOR_ID0, snr_params->get_ops,
						   RTS_ISP_TRUE, check);
}

static int register_sensor_iq(void)
{
	struct sensor_params_t *snr_params;
	int ret;

	if (SENSOR_USE >= SENSOR_PARAM_COUNT) {
		RTK_LOGE(REG_LOG_TAG, "[%s] SENSOR_USE:%d out of range\r\n", __func__, SENSOR_USE);
		return -RTS_ISP_EINVAL;
	}
#if ISP_REAL_SENSOR
	snr_params = sensor_params + SENSOR_USE;
	ret = register_sensor_iq_sensor(snr_params);
#else
	snr_params = sensor_params + SENSOR_FIXP;
	ret = register_plugin(RTS_ISP_PLUG_SENSOR_ID0, snr_params->get_ops,
						  RTS_ISP_TRUE, RTS_ISP_FALSE);
#endif

	if (ret) {
		goto out;
	}
	if (IQ_USE >= IQ_COUNT) {
		RTK_LOGE(REG_LOG_TAG, "[%s] IQ_USE:%d out of range\r\n", __func__, IQ_USE);
		ret = -RTS_ISP_EINVAL;
		goto out;
	}
	if (snr_params->iq_buf[IQ_USE] == NULL) {
		RTK_LOGE(REG_LOG_TAG, "[%s] IQ buffer NULL\r\n", __func__);
		ret = -RTS_ISP_EINVAL;
		goto out;
	}
	RTK_LOGI(REG_LOG_TAG, "[%s] SENSOR_USE:%d  IQ_USE:%d\r\n", __func__, SENSOR_USE, IQ_USE);
	ret = register_plugin(RTS_ISP_PLUG_IQ_ID0,
						  (void *)snr_params->iq_buf[IQ_USE], RTS_ISP_TRUE,
						  RTS_ISP_TRUE);
out:
	if (ret) {
		RTK_LOGE(REG_LOG_TAG, "register sensor iq fail ret = %d\r\n", ret);
	}
	return ret;
}

static int register_all_algos(void)
{
	int ret;

	ret = register_plugin(RTS_ISP_PLUG_ALGO_AE_ID0, rts_isp_get_ae_algo,
						  RTS_ISP_TRUE, RTS_ISP_TRUE);
	if (ret) {
		goto out;
	}
	ret = register_plugin(RTS_ISP_PLUG_ALGO_AWB_ID0, rts_isp_get_awb_algo,
						  RTS_ISP_TRUE, RTS_ISP_TRUE);
	if (ret) {
		goto out;
	}
	ret = register_plugin(RTS_ISP_PLUG_ALGO_AF_ID0, rts_isp_get_af_algo,
						  RTS_ISP_TRUE, RTS_ISP_TRUE);
	if (ret) {
		goto out;
	}
	ret = register_plugin(RTS_ISP_PLUG_ALGO_OTHER_ID0,
						  rts_isp_get_other_algo, RTS_ISP_TRUE,
						  RTS_ISP_TRUE);
out:
	if (ret) {
		printf("register algos fail ret = %d\n", ret);
	}
	return ret;
}

static void isp_task(void *pvParameters)
{
	int ret;

	ret = driver_init();
	if (ret) {
		goto exit_noclean;
	}
	ret = isp_mcu_driver_init();
	if (ret) {
		goto exit_clean_driver;
	}
	ret = rts_av_isp_init();
	if (ret) {
		goto exit_clean_driver;
	}
#ifndef ISP_CPU_MODE
	ret = rtscam_video_init();
	if (ret) {
		goto exit;
	}
#endif
	ret = register_sensor_iq();
	if (ret) {
		goto exit;
	}
	ret = register_all_algos();
	if (ret) {
		goto exit;
	}
	ret = rts_av_isp_start();
	if (ret) {
		goto exit;
	}
exit:
	rts_av_isp_cleanup();
	isp_mcu_driver_cleanup();
exit_clean_driver:
	driver_cleanup();
exit_noclean:
	if (ret) {
		printf("rts isp fail %d\n", ret);
	}
	vTaskDelete(NULL);
}

static void isp_preview_task(void *pvParameters)
{
	int ret;

	ret = rts_isp_test_preview(0, 10);
	if (ret) {
		vTaskDelete(NULL);
	}
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void isp_mcu_apps_start(void)
{
	xTaskCreate(isp_task, "isp", configMINIMAL_STACK_SIZE * 16, NULL,
				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
	if (0) {
		xTaskCreate(isp_preview_task, "isp_preview",
					configMINIMAL_STACK_SIZE * 16, NULL,
					(portPRIVILEGE_BIT |
					 ((configMAX_PRIORITIES / 2) - 2)),
					NULL);
	}
}
