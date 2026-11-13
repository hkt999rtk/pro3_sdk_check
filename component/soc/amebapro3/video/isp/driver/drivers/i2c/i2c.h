#ifndef _I2C_H_INC_
#define _I2C_H_INC_

#include <stdint.h>

typedef void *rtscam_rtos_i2c_t;

rtscam_rtos_i2c_t rtscam_rtos_i2c_dev_get(void);
void rtscam_rtos_i2c_dev_put(rtscam_rtos_i2c_t i2c);
int rtscam_rtos_i2c_read(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len,
						 uint8_t *buf);
int rtscam_rtos_i2c_write(rtscam_rtos_i2c_t i2c, uint16_t addr, uint16_t len,
						  uint8_t *buf);

#endif /* _I2C_H_INC_ */
