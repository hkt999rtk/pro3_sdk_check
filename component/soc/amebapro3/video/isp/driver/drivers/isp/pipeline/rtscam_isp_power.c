#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <clock.h>
#include <gpio.h>
#include <rts_camera_isp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "rtscam_isp_power.h"

#define MAX_ERRNO 4095
#define IS_ERR_VALUE(x) \
	((unsigned long)(void *)(x) >= (unsigned long)-MAX_ERRNO)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
static inline long PTR_ERR(const void *ptr)
{
	return (long)ptr;
}

static inline bool IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

static inline int power_is_gpio(const struct rtscam_isp_snr_power *item)
{
	return item->type <= RTSCAM_ISP_SNR_PWDN_GPIO;
}

static inline int power_is_clk(const struct rtscam_isp_snr_power *item)
{
	return item->type == RTSCAM_ISP_SNR_HCLK;
}

static inline int power_is_voltage(const struct rtscam_isp_snr_power *item)
{
	return item->type >= RTSCAM_ISP_SNR_IO_POWER;
}

static int rtscam_isp_snr_power_check(struct rtscam_isp_snr_power *item,
									  bool has_pmu)
{
	if (power_is_voltage(item) && !has_pmu) {
		return 0;
	}
	if (!item->handle) {
		return -EINVAL;
	}
	if (IS_ERR(item->handle)) {
		return PTR_ERR(item->handle);
	}
	return 0;
}

static int rtscam_isp_snr_power_init(struct rtscam_isp_snr_power *item,
									 uint32_t type)
{
	static const char *const type_str[] = {
		"rst", "pwdn", "hclk", "io", "analog", "core",
	};

	if (!item) {
		return -EINVAL;
	}

	item->type = type;
	item->name = type_str[type];
	item->handle = NULL;
	item->current_value = 0;

	return 0;
}

static int rtscam_isp_snr_power_get(struct rtscam_isp_snr_power *item,
									struct rtscam_isp_power *power)
{
	if (!item) {
		return -EINVAL;
	}

	if (item->handle) {
		return 0;
	}

	if (power_is_gpio(item)) {
		item->handle = rtscam_rtos_gpiod_get(item->name);
	} else if (power_is_clk(item)) {
		item->handle = rtscam_rtos_clk_get(item->name);
	} else if (power_is_voltage(item) && power->has_pmu) {
		return -EINVAL;
	}

	return rtscam_isp_snr_power_check(item, power->has_pmu);
}

static int rtscam_isp_snr_power_put(struct rtscam_isp_snr_power *item,
									struct rtscam_isp_power *power)
{
	if (!item) {
		return -EINVAL;
	}

	if (!item->handle) {
		return 0;
	}

	if (power_is_gpio(item)) {
		rtscam_rtos_gpiod_put(item->handle);
	} else if (power_is_clk(item)) {
		if (item->current_value) {
			rtscam_rtos_clk_disable(item->handle);
			rtscam_rtos_clk_put(item->handle);
		}
	} else if (power_is_voltage(item) && power->has_pmu) {
		return -EINVAL;
	}

	item->handle = NULL;
	item->current_value = 0;

	return 0;
}

static int rtscam_isp_snr_power_set(struct rtscam_isp_snr_power *item,
									uint32_t value,
									struct rtscam_isp_power *power)
{
	int ret = 0;

	if (!item) {
		return -EINVAL;
	}

	if (rtscam_isp_snr_power_check(item, power->has_pmu)) {
		return -EFAULT;
	}

	if (power_is_gpio(item)) {
		rtscam_rtos_gpiod_direction_output(item->handle, value);
	} else if (power_is_clk(item)) {
		if (item->current_value) {
			rtscam_rtos_clk_disable(item->handle);
		}
		if (value) {
			ret = rtscam_rtos_clk_set_rate(item->handle, value);
			if (!ret) {
				ret = rtscam_rtos_clk_enable(item->handle);
			}
		}
	} else if (power_is_voltage(item)) {
		if (power->has_pmu) {
			return -EINVAL;
		} else if (power->pwrctl_gpio_handle) {
			if (value)
				rtscam_rtos_gpiod_direction_output(
					power->pwrctl_gpio_handle, 1);
			else
				rtscam_rtos_gpiod_direction_output(
					power->pwrctl_gpio_handle, 0);
		}
	}

	if (!ret) {
		item->current_value = value;
	}

	return ret;
}

int rtscam_isp_power_init(struct rtscam_isp_power *power, bool has_pmu)
{
	int i;
	int ret = 0;

	if (ARRAY_SIZE(power->powers) != _MAX_RTSCAM_ISP_SNR_POWER_TYPE) {
		return -EINVAL;
	}

	power->has_pmu = has_pmu;
	for (i = 0; i < ARRAY_SIZE(power->powers); i++) {
		ret = rtscam_isp_snr_power_init(&power->powers[i], i);
		if (ret) {
			break;
		}
	}
	if (!power->has_pmu) {
		power->pwrctl_gpio_handle = NULL;
	}

	return ret;
}

void rtscam_isp_power_cleanup(struct rtscam_isp_power *power)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(power->powers); i++) {
		rtscam_isp_snr_power_put(&power->powers[i], power);
	}
	if (power->pwrctl_gpio_handle) {
		rtscam_rtos_gpiod_direction_output(power->pwrctl_gpio_handle,
										   0);
		rtscam_rtos_gpiod_put(power->pwrctl_gpio_handle);
		power->pwrctl_gpio_handle = NULL;
	}
}

int rtscam_isp_power_set(struct rtscam_isp_power *power,
						 struct rtscam_isp_snr_pwr *powers)
{
	int i;
	int ret = 0;

	if (!power || !powers) {
		return -EINVAL;
	}

	if (powers->num > ARRAY_SIZE(powers->items)) {
		return -EINVAL;
	}

	for (i = 0; i < powers->num; i++)
		if (powers->items[i].type >= _MAX_RTSCAM_ISP_SNR_POWER_TYPE) {
			return -EINVAL;
		}

	for (i = 0; i < powers->num; i++) {
		uint32_t type = powers->items[i].type;
		uint32_t value = powers->items[i].value;

		ret = rtscam_isp_snr_power_get(&power->powers[type], power);
		if (ret) {
			break;
		}
		ret = rtscam_isp_snr_power_set(&power->powers[type], value,
									   power);
		if (ret) {
			break;
		}
		if (powers->items[i].delay)
			vTaskDelay(pdMS_TO_TICKS(
						   (powers->items[i].delay / 1000 + 1)));
	}

	return ret;
}
