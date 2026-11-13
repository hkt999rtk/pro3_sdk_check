#include <errno.h>
#include <string.h>
#include <isp_rtos_iomem.h>
#include "gpio.h"

void *gpio_pin_reset;
void *gpio_pin_pwdn;
void *gpio_pin_pwrctrl;

enum rtscam_rtos_gpio_pin {
	RST,
	PWDN,
	PWRCTRL
};

struct rtscam_rtos_gpio_desc {
	enum rtscam_rtos_gpio_pin id;
	void *gpio_pin;
};

struct rtscam_rtos_gpio_desc gpio[3];

int gpio_desc_init(void)
{
	gpio[0].id = RST;
	gpio[0].gpio_pin = gpio_pin_reset;

	gpio[1].id = PWDN;
	gpio[1].gpio_pin = gpio_pin_pwdn;

	gpio[2].id = PWRCTRL;
	gpio[2].gpio_pin = gpio_pin_pwrctrl;

	return 0;
}

rtscam_rtos_gpiod_t rtscam_rtos_gpiod_get(const char *id)
{
	if (!strcmp("rst", id)) {
		return &gpio[0];
	}
	if (!strcmp("pwdn", id)) {
		return &gpio[1];
	}
	if (!strcmp("pwrctrl", id)) {
		return &gpio[2];
	}
	return NULL;
}

void rtscam_rtos_gpiod_put(rtscam_rtos_gpiod_t gpio)
{

}

static void sensor_gpio_write(void **gpio_pin, int value)
{
	//TODO
	(void)gpio_pin;
	(void)value;
}

void rtscam_rtos_gpiod_direction_output(rtscam_rtos_gpiod_t gpio, int value)
{
	struct rtscam_rtos_gpio_desc *gpio_desc = gpio;

	sensor_gpio_write(&gpio_desc->gpio_pin, value);
}
