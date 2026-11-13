/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2023 Mike Wong <mike.wong@realtek.com>
 */

#include <stdio.h>
#include <rts_isp_sensor.h>

//#define DEBUG
#ifdef DEBUG
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define SUPPORTED_ISP_NUM 1
#define RTS_ISP_HDR_CHAN_MAX 2

// 4M @ 5fps
#define SENSOR_WIDTH 2560
#define SENSOR_HEIGHT 1440
#define SENSOR_HTS 4000
#define SENSOR_VTS 2000
#define PCLK 40000000

#define FPS (float)PCLK / (float)SENSOR_HTS / (float)SENSOR_VTS

#define BIT_DEPTH SNR_12BIT
#define TYPE RAW_SENSOR

static int pseudo_get_info(uint32_t isp_id, struct rts_isp_sensor_info *info)
{
	struct rts_isp_snr_pwr *up = &info->power_up;
	struct rts_isp_snr_pwr *down = &info->power_down;

	if (isp_id >= SUPPORTED_ISP_NUM || !info) {
		return -RTS_ISP_EINVAL;
	}

	info->modes.mode[0].hdr = RTS_ISP_HDR_NONE;
	info->modes.mode[0].hdr_variant = RTS_ISP_HDR_VAR_INVALID;
	info->modes.mode[0].size.w = SENSOR_WIDTH;
	info->modes.mode[0].size.h = SENSOR_HEIGHT;
	info->modes.mode[0].fps = FPS;
	info->modes.num = 1;

	info->i2c.i2c_id = 0x01;
	info->i2c.addr_len = 1;
	info->i2c.data_len = 1;

	up->num = 0;
	down->num = 0;

	return RTS_ISP_OK;
}

static int pseudo_get_init_info(uint32_t isp_id,
								const struct rts_isp_sensor_mode *mode,
								struct rts_isp_sensor_init_info *info)
{
	if (isp_id >= SUPPORTED_ISP_NUM || !info) {
		return -RTS_ISP_EINVAL;
	}

	debug("pseudo get fps %.1f init info\n", mode->fps);

	info->interface.interface = SNR_INTERFACE_MIPI;
	info->interface.mipi.lanes = MIPI_LANE0 | MIPI_LANE1;
	info->interface.mipi.hs_term = 0x4;
	info->interface.type = TYPE;
	info->interface.bit_depth = BIT_DEPTH;

	info->size.w = SENSOR_WIDTH;
	info->size.h = SENSOR_HEIGHT;
	info->start.x = 0;
	info->start.y = 0;

	info->hts = SENSOR_HTS;
	info->pclk = PCLK;
	info->min_vts = SENSOR_VTS;
	info->max_vts = 65535;
	/* exposure step: time per line in microseconds */
	info->exposure_step = (float)SENSOR_HTS / (float)PCLK * 1000000.0f;

	return RTS_ISP_OK;
}

static int pseudo_check(uint32_t isp_id)
{
	return RTS_ISP_OK;
}

static int pseudo_get_exposure_range(uint32_t isp_id, uint32_t vts,
									 float ratio[RTS_ISP_HDR_CHAN_MAX - 1],
									 float min_exposure[RTS_ISP_HDR_CHAN_MAX],
									 float max_exposure[RTS_ISP_HDR_CHAN_MAX])
{
	float exp_step;

	if (isp_id >= SUPPORTED_ISP_NUM) {
		return -RTS_ISP_EINVAL;
	}

	exp_step = (float)SENSOR_HTS / (float)PCLK * 1000000.0f;
	min_exposure[0] = 2 * exp_step;
	max_exposure[0] = (vts - 8) * exp_step;

	return RTS_ISP_OK;
}

static int pseudo_get_tuned_again(uint32_t isp_id,
								  float again[RTS_ISP_HDR_CHAN_MAX])
{
	return RTS_ISP_OK;
}

static int pseudo_get_tuned_dgain(uint32_t isp_id,
								  float dgain[RTS_ISP_HDR_CHAN_MAX])
{
	return RTS_ISP_OK;
}

static int pseudo_get_exposure_gain_info(uint32_t isp_id,
		const struct rts_isp_sensor_exp_gain *exp_gain,
		struct rts_isp_sync_regs *regs)
{
	return RTS_ISP_OK;
}

static const struct rts_isp_sensor_ops pseudo_4m_ops = {
	.api_version = SENSOR_API_VERSION,
	.name = "pseudo_4m",
	.get_info = pseudo_get_info,
	.get_init_info = pseudo_get_init_info,
	.check = pseudo_check,
	.get_exposure_range = pseudo_get_exposure_range,
	.get_tuned_again = pseudo_get_tuned_again,
	.get_tuned_dgain = pseudo_get_tuned_dgain,
	.get_exposure_gain_info = pseudo_get_exposure_gain_info,
};

RTS_ISP_DEFINE_SENSOR_PLUGIN(pseudo_4m_ops)
