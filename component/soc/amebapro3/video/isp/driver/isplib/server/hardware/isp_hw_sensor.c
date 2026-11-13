/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_utils.h>
#include <isp_os_log.h>
#include <rts_isp_geom.h>
#include <rts_isp_errno.h>
#include <rts_isp_sensor.h>
#include <isp_reg_isp_sys.h>
#include <isp_reg_comps.h>
#include "isp_hw_sensor.h"

#define RTS_ISP_COMP_PTS_NUM 9

struct rts_isp_compand_table comp_def = {
	7,
	{
		{ 0, 0 },
		{ 256, 256 },
		{ 512, 384 },
		{ 2048, 768 },
		{ 8192, 1536 },
		{ 32768, 3072 },
		{ 65536, 4096 },
	},
};

static int isp_hw_sensor_min_exp_delay(uint32_t isp_id)
{
	return ISP_DEFINED(CONFIG_RTS3917) ? 32 : 8;
}

static int calc_points_slope(rts_isp_point_t p1, rts_isp_point_t p2)
{
	int i;
	uint32_t slope_bit_max = (1 << COMPS_LOGSLP_0_BITS) - 1;

	for (i = 0; i <= slope_bit_max; i++) {
		if ((p2.x - p1.x) == ((p2.y - p1.y) << i)) {
			break;
		}
	}

	if (i > slope_bit_max) {
		return -RTS_ISP_EINVAL;
	}

	return i;
}

static int
check_sensor_comp_table(uint32_t isp_id,
						uint32_t slope_bit[RTS_ISP_COMP_PTS_NUM - 1],
						const struct rts_isp_compand_table *comp)
{
	int i, ret;

	if (comp->point_num < 2 || comp->point_num > RTS_ISP_COMP_PTS_NUM) {
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	if (comp->point[0].x != 0 || comp->point[0].y != 0 ||
		comp->point[comp->point_num - 1].x != 65536 ||
		comp->point[comp->point_num - 1].y != 4096) {
		ret = -RTS_ISP_EINVAL;
		goto err;
	}

	for (i = 0; i < comp->point_num - 1; i++) {
		ret = calc_points_slope(comp->point[i], comp->point[i + 1]);
		if (ret < 0) {
			goto err;
		}

		slope_bit[i] = ret;
	}

	return RTS_ISP_OK;

err:
	isp_perror(ret, "sensor companding point invalid");
	return ret;
}

static int set_comp_seg_register(uint32_t isp_id,
								 const struct rts_isp_compand_table *comp)
{
	int i;
	uint32_t slope_fill = 0;
	rts_isp_point_t point_fill = { 0, 0 };
	uint32_t slope_bit[RTS_ISP_COMP_PTS_NUM - 1] = { 0 };
	const struct rts_isp_compand_table *comp_table;

	if (!comp || comp->point_num == 0) {
		comp_table = &comp_def;
	} else {
		comp_table = comp;
	}

	if (check_sensor_comp_table(isp_id, slope_bit, comp_table)) {
		return -RTS_ISP_EINVAL;
	}

	for (i = 0; i < comp_table->point_num - 2; i++) {
		isp_write_reg(comp_table->point[i + 1].x,
					  COMPS_COMPS_X_PTS_0 + i * 4);
		isp_write_reg(comp_table->point[i + 1].y,
					  COMPS_COMPS_Y_PTS_0 + i * 4);
		isp_write_reg(slope_bit[i], COMPS_COMPS_LOGSLP_0 + i * 4);
		point_fill = comp_table->point[i + 1];
	}
	slope_fill = slope_bit[comp_table->point_num - 2];
	if (comp_table->point_num < RTS_ISP_COMP_PTS_NUM) {
		for (i = comp_table->point_num - 2;
			 i < RTS_ISP_COMP_PTS_NUM - 2; i++) {
			isp_write_reg(point_fill.x,
						  COMPS_COMPS_X_PTS_0 + i * 4);
			isp_write_reg(point_fill.y,
						  COMPS_COMPS_Y_PTS_0 + i * 4);
			isp_write_reg(slope_fill, COMPS_COMPS_LOGSLP_0 + i * 4);
		}
	}
	isp_write_reg(slope_fill, COMPS_COMPS_LOGSLP_7);

	return RTS_ISP_OK;
}

static int isp_hw_sensor_set_comp(uint32_t isp_id, int enable)
{
	if (!enable) {
		isp_write_reg_mask(0 << SENSOR_COMP_EN_OFFSET, SYS_SENSOR_COMP,
						   SENSOR_COMP_EN_MASK);

		return RTS_ISP_OK;
	}

	isp_write_reg_mask(1 << SENSOR_COMP_EN_OFFSET, SYS_SENSOR_COMP,
					   SENSOR_COMP_EN_MASK);

	return RTS_ISP_OK;
}

static int isp_hw_sensor_set_decomp(uint32_t isp_id, int enable,
									const struct rts_isp_compand_table *comp)
{
	int ret;

	if (!enable) {
		isp_write_reg_mask(0 << SENSOR_DCOMP_EN_OFFSET, SYS_SENSOR_COMP,
						   SENSOR_DCOMP_EN_MASK);
		isp_write_reg_mask(0 << DECOMP_SEL_OFFSET, SYS_POS_SEL,
						   DECOMP_SEL_MASK);

		return RTS_ISP_OK;
	}

	ret = set_comp_seg_register(isp_id, comp);
	if (ret) {
		return ret;
	}
	isp_write_reg_mask(1 << SENSOR_DCOMP_EN_OFFSET, SYS_SENSOR_COMP,
					   SENSOR_DCOMP_EN_MASK);
	isp_write_reg_mask(1 << DECOMP_SEL_OFFSET, SYS_POS_SEL,
					   DECOMP_SEL_MASK);

	return RTS_ISP_OK;
}

static const struct isp_hw_sensor_ops hw_sensor_ops = {
	.min_exp_delay = isp_hw_sensor_min_exp_delay,
	.set_comp = isp_hw_sensor_set_comp,
	.set_decomp = isp_hw_sensor_set_decomp,
};

const struct isp_hw_sensor_ops *isp_hw_sensor_get_ops(void)
{
	return &hw_sensor_ops;
}
