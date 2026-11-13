#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <isp_rtos_completion.h>
#include <isp_rtos_iomem.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_barrier.h>

#include "i2c.h"
#include "sensor_i2c.h"

rtscam_rtos_i2c_t rtscam_rtos_i2c_dev_get(void)
{
	return (rtscam_rtos_i2c_t)sensor_i2c_get_adapter();
}

void rtscam_rtos_i2c_dev_put(rtscam_rtos_i2c_t i2c)
{
	return;
}

int rtscam_rtos_i2c_init(void)
{
	return sensor_i2c_init();
}

int rtscam_rtos_i2c_cleanup(void)
{
	return sensor_i2c_cleanup();
}

int rtscam_rtos_i2c_read(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len, uint8_t *buf)
{
	return sensor_i2c_read(i2c, addr, len, buf);
}

int rtscam_rtos_i2c_write(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len, uint8_t *buf)
{
	return sensor_i2c_write(i2c, addr, len, buf);
}
