/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "gpio_api.h"   // mbed
#include "i2c_api.h"
// #include "pinmap.h"

enum rts_isp_sensor_clock {
	CLK_NONE = 0,
	CLK_12M = 12000000,
	CLK_24M = 24000000,
	CLK_27M = 27000000,
	CLK_37M125 = 37125000,
	CLK_54M = 54000000,
	CLK_74M25 = 74250000,
};

enum rts_isp_sensor_gpio {
	GPIO_LOW = 0,
	GPIO_HIGH = 1,
};

enum rts_isp_snr_pwr_type {
	SNR_RST_GPIO = 0,
	SNR_PWDN_GPIO,
	SNR_PWRCTRL_GPIO,
	SNR_HCLK,
	SNR_IO_POWER,
	SNR_ANALOG_POWER,
	SNR_CORE_POWER,
	_MAX_SNR_POWER_TYPE,
};

typedef struct rts_isp_snr_pwr_item_s {
	enum rts_isp_snr_pwr_type type;
	uint32_t value;
	uint32_t delay; /* us */
} rts_isp_snr_pwr_item_t;

typedef struct rts_isp_snr_pwr_s {
	uint32_t num;
	rts_isp_snr_pwr_item_t items[16];
} rts_isp_snr_pwr_t;

typedef struct sensor_gpio_s {
	gpio_t pwdn;
	gpio_t rst;
	gpio_t hclk;
} sensor_gpio_t;

typedef struct rts_isp_i2c_reg_s {
	uint16_t addr;
	uint8_t data;
} rts_isp_i2c_reg_t;

typedef struct rts_isp_i2c_info_s {
	uint8_t slave_addr;
	uint8_t register_addr_len;
	uint8_t data_len;
} rts_isp_i2c_info_t;

typedef struct sensor_i2c_s {
	i2c_t master;
	rts_isp_i2c_info_t info;
} sensor_i2c_t;

typedef struct sensor_ctx_s {
	const char *sensor_name;

	sensor_gpio_t sensor_gpio;
	rts_isp_snr_pwr_t sensor_power_up;
	rts_isp_snr_pwr_t sensor_power_down;
	void (*sensor_gpio_init)(sensor_gpio_t *sensor_gpio);
	uint32_t(*sensor_power_configure)(rts_isp_snr_pwr_t *up, rts_isp_snr_pwr_t *down);
	uint32_t(*sensor_power_on)(void *p);
	uint32_t(*sensor_power_off)(void *p);

	sensor_i2c_t sensor_i2c;
	void (*sensor_i2c_init)(sensor_i2c_t *p_sensor_i2c);
	uint32_t(*sensor_i2c_check_id)(sensor_i2c_t *p_sensor_i2c);

	uint32_t(*sensor_exposure_gain)(sensor_i2c_t *p_sensor_i2c);

	uint32_t(*sensor_set_init_i2c_regs_and_initialize)(sensor_i2c_t *p_sensor_i2c);

	uint32_t(*sensor_set_mirror_flip)(sensor_i2c_t *p_sensor_i2c, uint32_t mirror_en, uint32_t flip_en);

} sensor_ctx_t;

