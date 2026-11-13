#ifndef _SENSOR_I2C_H_INC_
#define _SENSOR_I2C_H_INC_

void *sensor_i2c_get_adapter(void);
int sensor_i2c_init(void);
int sensor_i2c_cleanup(void);
int sensor_i2c_read(void *i2c, uint16_t addr, uint16_t len,
					uint8_t *buf);
int sensor_i2c_write(void *i2c, uint16_t addr, uint16_t len,
					 uint8_t *buf);

#endif /* _SENSOR_I2C_H_INC_ */
