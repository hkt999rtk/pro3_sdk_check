/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "sensor_start.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// #define printf(...) \
// ({ \
// DBG_8195A(__VA_ARGS__); \
// })

//D2 peripheral configuration
#define GPIO_SENSOR_PWDN    _PA_2
#define GPIO_SENSOR_RST     _PA_5

#define I2C_0               0
#define I2C_1               1
#define MBED_I2C_BUS_CLK    400000  //hz
#define MBED_I2C_MTR_SDA    _PB_4 //_PA_3
#define MBED_I2C_MTR_SCL    _PB_3 //_PA_4

static void set_power_item(rts_isp_snr_pwr_item_t *item, enum rts_isp_snr_pwr_type type, uint32_t value, uint32_t delay)
{
	item->type = type;
	item->value = value;
	item->delay = delay;
}

void sensor_common_gpio_init(sensor_gpio_t *sensor_gpio)
{
#if defined(CONFIG_AMEBAPRO3)


#else
	gpio_init(&sensor_gpio->pwdn, GPIO_SENSOR_PWDN);
	gpio_dir(&sensor_gpio->pwdn, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&sensor_gpio->pwdn, PullNone);     // No pull

	gpio_init(&sensor_gpio->rst, GPIO_SENSOR_RST);
	gpio_dir(&sensor_gpio->rst, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&sensor_gpio->rst, PullNone);     // No pull
#endif
}

uint32_t sensor_sc2333_power_configure(rts_isp_snr_pwr_t *up, rts_isp_snr_pwr_t *down)
{
	uint32_t i = 0;

	//power up
	i = 0;
	set_power_item(&up->items[i++], SNR_RST_GPIO, GPIO_LOW, 0);
	set_power_item(&up->items[i++], SNR_PWDN_GPIO, GPIO_LOW, 0);
	set_power_item(&up->items[i++], SNR_HCLK, 0, 1000);

	set_power_item(&up->items[i++], SNR_RST_GPIO, GPIO_HIGH, 500);
	set_power_item(&up->items[i++], SNR_PWDN_GPIO, GPIO_HIGH, 500);
	set_power_item(&up->items[i++], SNR_HCLK, CLK_27M, 8000);
	up->num = i;

	//power down
	i = 0;
	set_power_item(&down->items[i++], SNR_RST_GPIO, 0, 1000);
	set_power_item(&down->items[i++], SNR_PWDN_GPIO, 0, 1000);
	set_power_item(&down->items[i++], SNR_HCLK, 0, 100000);
	down->num = i;

	return 0;
}

uint32_t sensor_imx307_power_configure(rts_isp_snr_pwr_t *up, rts_isp_snr_pwr_t *down)
{
	uint32_t i = 0;

	//power up
	i = 0;
	set_power_item(&up->items[i++], SNR_RST_GPIO, GPIO_LOW, 0);
	//set_power_item(&up->items[i++], SNR_PWDN_GPIO, GPIO_LOW, 0);
	set_power_item(&up->items[i++], SNR_HCLK, 0, 1000);

	set_power_item(&up->items[i++], SNR_RST_GPIO, GPIO_HIGH, 500);
	//set_power_item(&up->items[i++], SNR_PWDN_GPIO, GPIO_HIGH, 500);
	set_power_item(&up->items[i++], SNR_HCLK, CLK_27M, 8000);
	up->num = i;

	//power down
	i = 0;
	set_power_item(&down->items[i++], SNR_RST_GPIO, 0, 1000);
	set_power_item(&down->items[i++], SNR_PWDN_GPIO, 0, 1000);
	set_power_item(&down->items[i++], SNR_HCLK, 0, 100000);
	down->num = i;

	return 0;
}

static uint32_t sensor_set_power(void *p, uint32_t enable)
{
	sensor_ctx_t *ctx = (sensor_ctx_t *)p;
	sensor_gpio_t *gpio = &ctx->sensor_gpio;

	rts_isp_snr_pwr_t *power = NULL;
	if (enable) {
		power = &ctx->sensor_power_up;
	} else {
		power = &ctx->sensor_power_down;
	}

	for (uint32_t i = 0; i < (uint32_t)power->num; i++) {
		uint32_t type = power->items[i].type;
		uint32_t value = power->items[i].value;

		switch (type) {
		case SNR_RST_GPIO:
			//printf(">SNR_RST_GPIO \r\n");
			//gpio_write(&gpio->rst, value);
			break;
		case SNR_PWDN_GPIO:
			//printf(">SNR_PWDN_GPIO \r\n");
			//gpio_write(&gpio->pwdn, value);
			break;
		case SNR_HCLK:
			//printf(">SNR_HCLK \r\n");
			//assume pro2 can already provide the clock
			break;
		default:
			printf("unhandled operation \r\n");
			break;
		}

		DelayUs(power->items[i].delay);
	}

	return 0;
}

uint32_t sensor_common_power_on(void *p)
{
	printf("sensor power on \r\n");
	return sensor_set_power(p, 1);
}

uint32_t sensor_common_power_off(void *p)
{
	printf("sensor power off \r\n");
	return sensor_set_power(p, 0);
}

int sensor_i2c_inited = 0;

void sensor_sc2333_i2c_init(sensor_i2c_t *p_sensor_i2c)
{
	if (sensor_i2c_inited) {
		return;
	}

	p_sensor_i2c->master.i2c_idx = I2C_1;
	i2c_init(&p_sensor_i2c->master, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	i2c_frequency(&p_sensor_i2c->master, MBED_I2C_BUS_CLK);

	p_sensor_i2c->info.slave_addr = 0x30;
	printf("sensor i2c slave address: 0x%x\r\n", p_sensor_i2c->info.slave_addr);
	p_sensor_i2c->info.register_addr_len = 2;
	p_sensor_i2c->info.data_len = 1;

	sensor_i2c_inited = 1;
}

void sensor_imx307_i2c_init(sensor_i2c_t *p_sensor_i2c)
{
	if (sensor_i2c_inited) {
		return;
	}

	p_sensor_i2c->master.i2c_idx = I2C_0;  //I2C_1
	i2c_init(&p_sensor_i2c->master, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	i2c_frequency(&p_sensor_i2c->master, MBED_I2C_BUS_CLK);

	p_sensor_i2c->info.slave_addr = 0x1A; //0x30;
	printf("sensor i2c slave address: 0x%x\r\n", p_sensor_i2c->info.slave_addr);
	p_sensor_i2c->info.register_addr_len = 2;
	p_sensor_i2c->info.data_len = 1;

	sensor_i2c_inited = 1;
}

uint32_t sensor_i2c_read_reg(sensor_i2c_t *p_sensor_i2c, rts_isp_i2c_reg_t *p_reg)
{
	//the dummy write operation is required to set the sub address (i.e. register address) before the read operation.
	//shoud send the high 8bits of register address to sensor first
	//write 2 bytes --> | high register address[15:8] | low register address[7:0] |
	//read 1 byte --> | 8 bits data |
	uint8_t write_buf[p_sensor_i2c->info.register_addr_len];
	write_buf[0] = p_reg->addr >> 8;
	write_buf[1] = p_reg->addr & 0xff;

	printf("[%s] i2c write 0x%x 0x%x\r\n", __func__, write_buf[0], write_buf[1]);
	i2c_write(&p_sensor_i2c->master, p_sensor_i2c->info.slave_addr, write_buf, sizeof(write_buf), 0); //don't send the stop
	printf("[%s] i2c read %d byte\r\n", __func__, sizeof(p_reg->data));
	int timeout_ms = 2000; //ms
	// int read_len = i2c_read_timeout(&p_sensor_i2c->master, p_sensor_i2c->info.slave_addr, &p_reg->data, sizeof(p_reg->data), 1, timeout_ms);
	// if (read_len != sizeof(p_reg->data)) {
	// printf("i2c read timeout or fail\r\n");
	// return -1;
	// }
	i2c_read(&p_sensor_i2c->master, p_sensor_i2c->info.slave_addr, &p_reg->data, sizeof(p_reg->data), 1);

	printf("[%s] i2c read done\r\n", __func__);

	return 0;
}

uint32_t sensor_i2c_write_reg(sensor_i2c_t *p_sensor_i2c, rts_isp_i2c_reg_t *p_reg)
{
	//write 3 byte --> | high register address[15:8] | low register address[7:0] | 8 bits data |
	uint8_t write_buf[p_sensor_i2c->info.register_addr_len + p_sensor_i2c->info.data_len];
	write_buf[0] = p_reg->addr >> 8;
	write_buf[1] = p_reg->addr & 0xff;
	write_buf[2] = p_reg->data;

	i2c_write(&p_sensor_i2c->master, p_sensor_i2c->info.slave_addr, write_buf, sizeof(write_buf), 1);

	return 0;
}

uint32_t sensor_sc2333_i2c_check_id(sensor_i2c_t *p_sensor_i2c)
{
	uint16_t id;
	rts_isp_i2c_reg_t reg;

	reg.addr = 0x3107;
	sensor_i2c_read_reg(p_sensor_i2c, &reg);
	id = reg.data << 8;
	//printf("reg.data: 0x%x \r\n", reg.data);

	reg.addr = 0x3108;
	sensor_i2c_read_reg(p_sensor_i2c, &reg);
	id |= reg.data;
	//printf("reg.data: 0x%x \r\n", reg.data);

	printf("sensor id: 0x%x \r\n", id);

	uint16_t real_sensor_id = 0xcb3a;
	if (id == real_sensor_id) {
		printf("check sensor id ok \r\n");
		return 0;
	} else {
		printf("check sensor id fail \r\n");
		return -1;
	}
}

uint32_t sensor_imx307_i2c_check_id(sensor_i2c_t *p_sensor_i2c)
{
	printf("imx307 check sensor id\r\n");

	uint16_t id;
	rts_isp_i2c_reg_t reg;

	reg.addr = 0x31dc;
	sensor_i2c_read_reg(p_sensor_i2c, &reg);
	id = reg.data & 0x6;
	//printf("reg.data: 0x%x \r\n", reg.data);

	printf("sensor id: 0x%x \r\n", id);

	uint16_t real_sensor_id = 0x4;
	if (id == real_sensor_id) {
		printf("check sensor id ok \r\n");
		return 0;
	} else {
		printf("check sensor id fail \r\n");
		return -1;
	}
}

uint32_t sensor_sc2333_exposure_gain(sensor_i2c_t *p_sensor_i2c)
{
	rts_isp_i2c_reg_t reg;
	reg.addr = 0x3e00;
	reg.data = 0x78;
	sensor_i2c_write_reg(p_sensor_i2c, &reg);
	sensor_i2c_read_reg(p_sensor_i2c, &reg);
	printf("[%s] reg.data: 0x%x \r\n", __func__, reg.data);

	reg.addr = 0x3e01;
	reg.data = 0x34;
	sensor_i2c_write_reg(p_sensor_i2c, &reg);
	sensor_i2c_read_reg(p_sensor_i2c, &reg);
	printf("[%s] reg.data: 0x%x \r\n", __func__, reg.data);

	reg.addr = 0x3e02;
	reg.data = 0x56;
	sensor_i2c_write_reg(p_sensor_i2c, &reg);
	sensor_i2c_read_reg(p_sensor_i2c, &reg);
	printf("[%s] reg.data: 0x%x \r\n", __func__, reg.data);

	return 0;
}

static rts_isp_i2c_reg_t g_sc2333_i2c_init_regs_asic[] = {
	//Cleaned_0x67_SC2333-LP_MIPI_27Minput_2Lane_371.25Mbps_10bit_1920x1088_30fps_外供_DVDD_1.3v.ini
	{0x0103, 0x01},
	{0x0100, 0x00},
	{0x36e9, 0x80},
	{0x37f9, 0x80},
	{0x301f, 0x67},
	{0x3106, 0x05},
	{0x3200, 0x00},
	{0x3201, 0x00},
	{0x3202, 0x00},
	{0x3203, 0x00},
	{0x3204, 0x07},
	{0x3205, 0x87},
	{0x3206, 0x04},
	{0x3207, 0x43},
	{0x3208, 0x07},
	{0x3209, 0x80},
	{0x320a, 0x04},
	{0x320b, 0x40},
	{0x3210, 0x00},
	{0x3211, 0x04},
	{0x3212, 0x00},
	{0x3213, 0x01},
	{0x3248, 0x04},
	{0x3249, 0x0b},
	{0x3253, 0x08},
	{0x3301, 0x09},
	{0x3302, 0xff},
	{0x3303, 0x10},
	{0x3306, 0x70},
	{0x3307, 0x02},
	{0x3309, 0xc8},
	{0x330a, 0x01},
	{0x330b, 0x30},
	{0x330c, 0x16},
	{0x330d, 0xff},
	{0x3318, 0x02},
	{0x331f, 0xb9},
	{0x3321, 0x0a},
	{0x3327, 0x0e},
	{0x332b, 0x12},
	{0x3333, 0x10},
	{0x3334, 0x40},
	{0x335e, 0x06},
	{0x335f, 0x0a},
	{0x3364, 0x1f},
	{0x337c, 0x02},
	{0x337d, 0x0e},
	{0x3390, 0x09},
	{0x3391, 0x0f},
	{0x3392, 0x1f},
	{0x3393, 0x20},
	{0x3394, 0x20},
	{0x3395, 0xe0},
	{0x33a2, 0x04},
	{0x33b1, 0x80},
	{0x33b2, 0x68},
	{0x33b3, 0x42},
	{0x33f9, 0x90},
	{0x33fb, 0xd0},
	{0x33fc, 0x0f},
	{0x33fd, 0x1f},
	{0x349f, 0x03},
	{0x34a6, 0x0f},
	{0x34a7, 0x1f},
	{0x34a8, 0x42},
	{0x34a9, 0x18},
	{0x34aa, 0x01},
	{0x34ab, 0x43},
	{0x34ac, 0x01},
	{0x34ad, 0x80},
	{0x3630, 0xf4},
	{0x3632, 0x44},
	{0x3633, 0x22},
	{0x3639, 0xf4},
	{0x363c, 0x47},
	{0x3670, 0x09},
	{0x3674, 0xf4},
	{0x3675, 0xfb},
	{0x3676, 0xed},
	{0x367c, 0x09},
	{0x367d, 0x0f},
	{0x3690, 0x22},
	{0x3691, 0x22},
	{0x3692, 0x22},
	{0x3698, 0x89},
	{0x3699, 0x96},
	{0x369a, 0xd0},
	{0x369b, 0xd0},
	{0x369c, 0x09},
	{0x369d, 0x0f},
	{0x36a2, 0x09},
	{0x36a3, 0x0f},
	{0x36a4, 0x1f},
	{0x36d0, 0x01},
	{0x3722, 0x21},
	{0x3724, 0x41},
	{0x3725, 0xc1},
	{0x3728, 0x20},
	{0x3900, 0x0d},
	{0x3905, 0x98},
	{0x391b, 0x81},
	{0x391c, 0x10},
	{0x3933, 0x81},
	{0x3934, 0xc5},
	{0x393f, 0xef},
	{0x3940, 0x77},
	{0x3941, 0x00},
	{0x3942, 0x01},
	{0x3943, 0xc5},
	{0x3952, 0x02},
	{0x3953, 0x0f},
	{0x3e01, 0x45},
	{0x3e02, 0xf0},
	{0x3e08, 0x1f},
	{0x3e1b, 0x14},
	{0x440e, 0x02},
	{0x4509, 0x38},
	{0x5799, 0x06},
	{0x5ae0, 0xfe},
	{0x5ae1, 0x40},
	{0x5ae2, 0x30},
	{0x5ae3, 0x28},
	{0x5ae4, 0x20},
	{0x5ae5, 0x30},
	{0x5ae6, 0x28},
	{0x5ae7, 0x20},
	{0x5ae8, 0x3c},
	{0x5ae9, 0x30},
	{0x5aea, 0x28},
	{0x5aeb, 0x3c},
	{0x5aec, 0x30},
	{0x5aed, 0x28},
	{0x5aee, 0xfe},
	{0x5aef, 0x40},
	{0x5af4, 0x30},
	{0x5af5, 0x28},
	{0x5af6, 0x20},
	{0x5af7, 0x30},
	{0x5af8, 0x28},
	{0x5af9, 0x20},
	{0x5afa, 0x3c},
	{0x5afb, 0x30},
	{0x5afc, 0x28},
	{0x5afd, 0x3c},
	{0x5afe, 0x30},
	{0x5aff, 0x28},
	{0x36e9, 0x20},
	{0x37f9, 0x27},
#if (ORIENTATION == 0)
	{0x3221, 0x66},
#endif
	{0x0100, 0x01},
};

static rts_isp_i2c_reg_t g_imx307_i2c_init_regs_hdr[] = {
	{0x3002, 0x00}, {0x3005, 0x01}, {0x3007, 0x03}, {0x3009, 0x02},
	{0x300A, 0xF0}, {0x300C, 0x11}, {0x3011, 0x0A}, {0x3018, 0x46},
	{0x3019, 0x05}, {0x301C, 0x30}, {0x301D, 0x11}, {0x3020, 0x03},
	{0x3021, 0x00}, {0x3024, 0xB1}, {0x3025, 0x09}, {0x3030, 0x11},
	{0x3031, 0x00}, {0x3045, 0x05}, {0x3046, 0x01}, {0x304B, 0x0A},
	{0x305C, 0x18}, {0x305D, 0x03}, {0x305E, 0x20}, {0x305F, 0x01},
	{0x309E, 0x4A}, {0x309F, 0x4A}, {0x3106, 0x11}, {0x311C, 0x0E},
	{0x3128, 0x04}, {0x3129, 0x00}, {0x313B, 0x41}, {0x315E, 0x1A},
	{0x3164, 0x1A}, {0x317C, 0x00}, {0x31EC, 0x0E}, {0x3405, 0x10},
	{0x3407, 0x01}, {0x3414, 0x0A}, {0x3415, 0x00}, {0x3418, 0xBA},
	{0x3419, 0x08}, {0x3441, 0x0C}, {0x3442, 0x0C}, {0x3443, 0x01},
	{0x3444, 0x20}, {0x3445, 0x25}, {0x3446, 0x57}, {0x3447, 0x00},
	{0x3448, 0x37}, {0x3449, 0x00}, {0x344A, 0x1F}, {0x344B, 0x00},
	{0x344C, 0x1F}, {0x344D, 0x00}, {0x344E, 0x1F}, {0x344F, 0x00},
	{0x3450, 0x77}, {0x3451, 0x00}, {0x3452, 0x1F}, {0x3453, 0x00},
	{0x3454, 0x17}, {0x3455, 0x00}, {0x3472, 0xA0}, {0x3473, 0x07},
	{0x347B, 0x23}, {0x3480, 0x49}, {0x3010, 0x61}, {0x30F0, 0x64},
	{0x3000, 0x00},
};

uint32_t sensor_sc2333_set_init_i2c_regs_and_initialize(sensor_i2c_t *p_sensor_i2c)
{
	uint32_t ret = 0;
	uint32_t num = ARRAY_SIZE(g_sc2333_i2c_init_regs_asic);
	for (uint32_t i = 0; i < num; i++) {
		rts_isp_i2c_reg_t *p_reg = &g_sc2333_i2c_init_regs_asic[i];
		ret = sensor_i2c_write_reg(p_sensor_i2c, p_reg);
		if (ret != 0) {
			return -1;
		}
	}
	printf("sensor initialize done \r\n");
	return 0;
}

uint32_t sensor_imx307_set_init_i2c_regs_and_initialize(sensor_i2c_t *p_sensor_i2c)
{
	uint32_t ret = 0;
	uint32_t num = ARRAY_SIZE(g_imx307_i2c_init_regs_hdr);
	for (uint32_t i = 0; i < num; i++) {
		rts_isp_i2c_reg_t *p_reg = &g_imx307_i2c_init_regs_hdr[i];
		ret = sensor_i2c_write_reg(p_sensor_i2c, p_reg);
		if (ret != 0) {
			return -1;
		}
	}
	printf("imx307 sensor initialize done \r\n");
	return 0;
}

uint32_t sensor_sc2333_set_mirror_flip(sensor_i2c_t *p_sensor_i2c, uint32_t mirror_en, uint32_t flip_en)
{
	rts_isp_i2c_reg_t reg;
	reg.addr = 0x3221;
	reg.data = 0;

	if (mirror_en) {
		reg.data |= 0x06;
	}
	if (flip_en) {
		reg.data |= 0x60;
	}
	sensor_i2c_write_reg(p_sensor_i2c, &reg);

	return 0;
}

sensor_ctx_t sensor_sc2333 = {
	.sensor_name = "sc2333",
	//power on
	.sensor_gpio_init = sensor_common_gpio_init,
	.sensor_power_configure = sensor_sc2333_power_configure,
	.sensor_power_on = sensor_common_power_on,
	.sensor_power_off = sensor_common_power_off,
	// //check id
	.sensor_i2c_init = sensor_sc2333_i2c_init,
	.sensor_i2c_check_id = sensor_sc2333_i2c_check_id,
	//test exposure register write then read
	.sensor_exposure_gain = sensor_sc2333_exposure_gain,
	//set sensor init table and initialize
	.sensor_set_init_i2c_regs_and_initialize = sensor_sc2333_set_init_i2c_regs_and_initialize,
	//mirror flip
	.sensor_set_mirror_flip = sensor_sc2333_set_mirror_flip,
};

sensor_ctx_t sensor_imx307 = {
	.sensor_name = "imx307",
	//power on
	.sensor_gpio_init = sensor_common_gpio_init,
	.sensor_power_configure = sensor_imx307_power_configure,
	.sensor_power_on = sensor_common_power_on,
	.sensor_power_off = sensor_common_power_off,
	// //check id
	.sensor_i2c_init = sensor_imx307_i2c_init,
	.sensor_i2c_check_id = sensor_imx307_i2c_check_id,
	//test exposure register write then read
	//.sensor_exposure_gain = sensor_imx307_exposure_gain,
	//set sensor init table and initialize
	.sensor_set_init_i2c_regs_and_initialize = sensor_imx307_set_init_i2c_regs_and_initialize,
	//mirror flip
	//.sensor_set_mirror_flip = sensor_imx307_set_mirror_flip,
};

void sensor_on(sensor_ctx_t *sensor_ctx)
{
	printf("%s sensor on \r\n", sensor_ctx->sensor_name);

	//power on
	sensor_ctx->sensor_gpio_init(&sensor_ctx->sensor_gpio);
	sensor_ctx->sensor_power_configure(&sensor_ctx->sensor_power_up, &sensor_ctx->sensor_power_down);
	sensor_ctx->sensor_power_on(sensor_ctx);

	//check id
	sensor_ctx->sensor_i2c_init(&sensor_ctx->sensor_i2c);
	sensor_ctx->sensor_i2c_check_id(&sensor_ctx->sensor_i2c);

	//test exposure register write then read (used to check i2c write & read register ok)
	//sensor_ctx->sensor_exposure_gain(&sensor_ctx->sensor_i2c);

	//set sensor init table and initialize
	sensor_ctx->sensor_set_init_i2c_regs_and_initialize(&sensor_ctx->sensor_i2c);
}

void sensor_off(sensor_ctx_t *sensor_ctx)
{
	sensor_ctx->sensor_power_off(sensor_ctx);
}

void sensor_mirror_flip(sensor_ctx_t *sensor_ctx, uint32_t mirror_en, uint32_t flip_en)
{
	sensor_ctx->sensor_set_mirror_flip(&sensor_ctx->sensor_i2c, mirror_en, flip_en);
}

/*----------------------------------------------------------------------*/

void sensor_start(void)
{
	printf("==== pro3 sensor start ==== \r\n");
	//power on
	//sensor_on(&sensor_sc2333);
	sensor_on(&sensor_imx307);
}

#if 0
void app_example(void)
{
	printf("==== pro3 sensor start ==== \r\n");
	//power on
	sensor_on(&sensor_sc2333);

#if 0
	//set mirror flip test
	sensor_mirror_flip(&sensor_sc2333, 1, 1);
	DelayMs(2000);
	sensor_mirror_flip(&sensor_sc2333, 1, 0);
	DelayMs(2000);
	sensor_mirror_flip(&sensor_sc2333, 0, 1);
	DelayMs(2000);
	sensor_mirror_flip(&sensor_sc2333, 0, 0);
#endif

#if 0
	//power off
	printf("wait 10s to turn off sensor \r\n");
	DelayMs(10000);
	sensor_off(&sensor_sc2333);
#endif
}
#endif

uint32_t SensorCmdTest(IN uint16_t argc, IN uint8_t *argv[])
{
	printf("Sensor Cmd Test \n\r");

	printf("argc %d \n\r", argc);
	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n\r", i, argv[i]);
	}

	if (_strcmp((const char *)argv[0], "run") == 0) {

		sensor_start();

	}

	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_sensor_test_cmd[] = {
	/* command string is upper case only, and user input argv[0] in console will convert to upper case */
	{
		"SE", SensorCmdTest
	},
};
