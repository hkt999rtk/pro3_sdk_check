#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/* AmebaPro3 header */
#include "ameba_soc.h"
#include "i2c_api.h"

#define SENSOR_I2C_DEBUG  0
#if defined(SENSOR_I2C_DEBUG) && (SENSOR_I2C_DEBUG==1)
#define SENSOR_I2C_LOG(fmt, ...)   printf(fmt, ##__VA_ARGS__)
#else
#define SENSOR_I2C_LOG(fmt, ...)
#endif

#define MBED_I2C_BUS_CLK    400000  //hz
#define MBED_I2C_MTR_SDA    _PB_4
#define MBED_I2C_MTR_SCL    _PB_3

static void *m_i2c_adapter = NULL;

void *sensor_i2c_get_adapter(void)
{
	return m_i2c_adapter;
}

int sensor_i2c_init(void)
{
	int ret = 0;
	i2c_t *i2c = (i2c_t *)malloc(sizeof(i2c_t));
	if (!i2c) {
		printf("error: sensor i2c init OOM\r\n");
		return -ENOMEM;
	}
	memset(i2c, 0, sizeof(i2c_t));

	i2c_init(i2c, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	i2c_frequency(i2c, MBED_I2C_BUS_CLK);

	m_i2c_adapter = (void *)i2c;

	return 0;
err:
	return ret;
}

int sensor_i2c_cleanup(void)
{
	if (!m_i2c_adapter) {
		return 0;
	}

	vPortFree(m_i2c_adapter);
	m_i2c_adapter = NULL;

	return 0;
}

int sensor_i2c_read(void *i2c, uint16_t addr, uint16_t len, uint8_t *buf)
{
	int i2c_result;

	SENSOR_I2C_LOG("IN %s:%d\r\n", __func__, __LINE__);
	if (!i2c) {
		return -EINVAL;
	}
	SENSOR_I2C_LOG("IN %s:%d\r\n", __func__, __LINE__);

	i2c_result = i2c_read_timeout(m_i2c_adapter, addr, buf, len, 1, 10000);
	if (i2c_result == 0) {
		printf("i2c read timeout with ret=%d\r\n", i2c_result);
		return -1;
	}
	if (i2c_result != len) {
		printf("i2c read return length[%d] not equal to wanted length[%d]\r\n", i2c_result, len);
		return -1;
	}

	return 0;
}

int sensor_i2c_write(void *i2c, uint16_t addr, uint16_t len, uint8_t *buf)
{
	int i2c_result;

	SENSOR_I2C_LOG("IN %s:%d\r\n", __func__, __LINE__);
	if (!i2c) {
		return -EINVAL;
	}
	SENSOR_I2C_LOG("IN %s:%d\r\n", __func__, __LINE__);

	i2c_result = i2c_write_timeout(m_i2c_adapter, addr, buf, len, 1, 10000);
	if (i2c_result == 0) {
		printf("i2c write timeout with ret=%d\r\n", i2c_result);
		return -1;
	}
	if (i2c_result != len) {
		printf("i2c write return length[%d] not equal to wanted length[%d]\r\n", i2c_result, len);
		return -1;
	}

	return 0;
}
