#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_driver.h>
#include <rts_isp.h>
#include "rts_camera.h"

unsigned char __vectors_start__[] = {0};
extern unsigned char __StackTop[];
#include "prebuilt_iq.h"

const struct rts_isp_ae_algo *rts_isp_get_ae_algo(void);
const struct rts_isp_awb_algo *rts_isp_get_awb_algo(void);
const struct rts_isp_af_algo *rts_isp_get_af_algo(void);
const struct rts_isp_other_algo *rts_isp_get_other_algo(void);
const struct rts_isp_sensor_ops *rts_isp_get_sc401ai_ops(void);
const struct rts_isp_sensor_ops *rts_isp_get_os04a10_ops(void);

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

static int register_sensor_iq(void)
{
	int ret;

	ret = register_plugin(RTS_ISP_PLUG_SENSOR_ID0, rts_isp_get_os04a10_ops,
						  RTS_ISP_TRUE, RTS_ISP_TRUE);
	if (ret) {
		goto out;
	}
	ret = register_plugin(RTS_ISP_PLUG_IQ_ID0,
						  (void *)__isp_iq_table_start__, RTS_ISP_TRUE,
						  RTS_ISP_TRUE);
out:
	if (ret) {
		printf("register sensor iq fail ret = %d\n", ret);
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
