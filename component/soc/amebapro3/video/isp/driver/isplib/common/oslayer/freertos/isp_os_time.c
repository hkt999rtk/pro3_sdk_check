/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2022 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/projdefs.h>
#include <isp_os_time.h>
#include <isp_utils_common.h>

uint64_t isp_os_time_get_us(void)
{
	const uint64_t MS2US = 1000;

	TickType_t tick = xTaskGetTickCount();

	return tick * portTICK_PERIOD_MS * MS2US;
}

uint64_t isp_os_time_get_ms(void)
{
	TickType_t tick = xTaskGetTickCount();

	return tick * portTICK_PERIOD_MS;
}

void isp_os_usleep(uint32_t us)
{
	vTaskDelay(pdMS_TO_TICKS(roundup_div(us, 1000)));
}

void isp_os_msleep(uint32_t ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}
