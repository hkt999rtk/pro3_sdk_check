/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <math.h>
#include <isp_os_time.h>
#include <isp_reg_uvt.h>
#include <isp_utils_common.h>
#include "isp_hw_uvtune.h"

#define Y_INDEX_MAX_CNT 9
#define UV_INDEX_MAX_CNT 16

#define uvtune_slope_value(th0, th1) ceil(16384 / (double)max(th1 - th0, 1))

#define REG_FUNCTHD_EZ /* UVT 3.0 */
#define UV_PREPROC
#define UV_GLOB_GAIN

#define MOD_NAME UV_TUNE

static void isp_hw_uv_preproc_slp(FW_EST_MOD_INFO mod_info)
{
	// const int bit_fract_blending_rate = 6;
	const int bit_fract_uvpp_slp = 8;

	uint16_t uvpp_mi_th0 = READ_REG(mod_info, UVPP_MI_TH0);
	uint16_t uvpp_mi_th1 = READ_REG(mod_info, UVPP_MI_TH1);

	uint16_t uvpp_blending_rate_min =
		READ_REG(mod_info, UVPP_BLENDING_RATE_MIN);
	uint16_t uvpp_blending_rate_max =
		READ_REG(mod_info, UVPP_BLENDING_RATE_MAX);

	uint16_t uvpp_diffthd_ofst = READ_REG(mod_info, UVPP_DIFFTHD_OFST);

	uint32_t uvpp_blending_rate_slp =
		(uint32_t)((float)((uvpp_blending_rate_max -
							uvpp_blending_rate_min)
						   << bit_fract_uvpp_slp) /
				   (float)(uvpp_mi_th1 - uvpp_mi_th0) +
				   0.5);
	uint32_t uvpp_diffthd_slp =
		(uint32_t)((float)(uvpp_diffthd_ofst << bit_fract_uvpp_slp) /
				   (float)(uvpp_mi_th1 - uvpp_mi_th0) +
				   0.5);

	WRITE_REG(mod_info, UVPP_BLENDING_RATE_SLP, uvpp_blending_rate_slp);
	WRITE_REG(mod_info, UVPP_DIFFTHD_SLP, uvpp_diffthd_slp);
}

static void isp_hw_uv_glob_gain_slp(FW_EST_MOD_INFO mod_info)
{
	const int bit_fract_globgain_rate = 6;
	const int bit_fract_globgain_mi_rate_slp = 8;
	const int bit_fract_globgain_mag_rate_slp = 12;

	uint16_t globgain_mi_th0 = READ_REG(mod_info, GLOBGAIN_MI_TH0);
	uint16_t globgain_mi_th1 = READ_REG(mod_info, GLOBGAIN_MI_TH1);
	uint16_t globgain_mi_rate_min =
		READ_REG(mod_info, GLOBGAIN_MI_RATE_MIN);
	uint16_t globgain_mi_rate_max =
		READ_REG(mod_info, GLOBGAIN_MI_RATE_MAX);

	uint16_t globgain_mag_rate_min =
		READ_REG(mod_info, GLOBGAIN_MAG_RATE_MIN);
	uint16_t globgain_mag_th0 = READ_REG(mod_info, GLOBGAIN_MAG_TH0);
	uint16_t globgain_mag_th1 = READ_REG(mod_info, GLOBGAIN_MAG_TH1);

	uint32_t globgain_mi_rate_slp =
		(uint32_t)((float)((globgain_mi_rate_max - globgain_mi_rate_min)
						   << bit_fract_globgain_mi_rate_slp) /
				   (float)(globgain_mi_th1 - globgain_mi_th0) +
				   0.5);
	uint32_t globgain_mag_rate_slp =
		(uint32_t)((float)(((1 << bit_fract_globgain_rate) -
							globgain_mag_rate_min)
						   << bit_fract_globgain_mag_rate_slp) /
				   (float)(globgain_mag_th1 -
						   globgain_mag_th0) +
				   0.5);

	WRITE_REG(mod_info, GLOBGAIN_MI_RATE_SLP, globgain_mi_rate_slp);
	WRITE_REG(mod_info, GLOBGAIN_MAG_RATE_SLP, globgain_mag_rate_slp);
}

static int isp_hw_uvt_est_dynamic(FW_EST_MOD_INFO mod_info)
{
#ifdef UV_PREPROC
	isp_hw_uv_preproc_slp(mod_info);
#endif

#ifdef UV_GLOB_GAIN
	isp_hw_uv_glob_gain_slp(mod_info);
#endif

	return 0;
}

static void isp_hw_uvtune_set_transform(uint32_t isp_id,
										const isp_iq_uvtune_v1_index_t *index,
										const isp_iq_uvtune_v1_value_t *value)
{
	const uint32_t reg_off =
		(index->y_index * 16 + index->uv_index) * 1 * 4;
	const uint32_t mats_addr = UV_TUNE_CTRL_PT_Y_0_TO_8_BASE + reg_off;
	const uint32_t thd_addr = UV_TUNE_FUNC2_THD_12_BASE + reg_off;
	const uint32_t slope_addr =
		UV_TUNE_FUNC2_THD_01_23_SLOPE_BASE + reg_off;
	const uint16_t *mats = (const uint16_t *)value->mats;
	const uint16_t *thds = (const uint16_t *)value->thds;
	uint16_t slopes[2];

	isp_write_reg(((mats[0] & 0xfff) << CTRL_PT_Y_U_OFFSET |
				   (mats[1] & 0xfff) << CTRL_PT_Y_V_OFFSET),
				  mats_addr);

	isp_write_reg((thds[1] << THD1_OFFSET | thds[2] << THD2_OFFSET),
				  thd_addr);

	slopes[0] = uvtune_slope_value(thds[0], thds[1]);
	slopes[1] = uvtune_slope_value(thds[2], thds[3]);
	isp_write_reg(slopes[0] << SLOPE01_OFFSET | slopes[1] << SLOPE23_OFFSET,
				  slope_addr);
}

static void isp_hw_uvtune_set_suppress(uint32_t isp_id,
									   const uint16_t suppress[9])
{
	int i;

	for (i = 0; i < 9; i++) {
		isp_write_reg(suppress[i], UV_TUNE_UVS_RATE_Y_0 + i * 4);
	}
}

static const uint16_t mats_init[] = {
	-128, 0,    -128, 64,	-128, 128,  -64,  128,	0,    128, 64,
		128,  128,  128,  128,	64,   128,  0,	  128,	-64,  128, -128,
		64,   -128, 0,	  -128, -64,  -128, -128, -128, -128, -64,
	};

static int isp_hw_uvtune_init_memregs(uint32_t isp_id)
{
	int i;
	int j;
	isp_iq_uvtune_v1_index_t index;
	isp_iq_uvtune_v1_value_t value = {
		.thds = { 0, 0, 0, 0 },
	};

	for (i = 0; i < Y_INDEX_MAX_CNT; i++) {
		for (j = 0; j < UV_INDEX_MAX_CNT; j++) {
			index.y_index = i;
			index.uv_index = j;
			value.mats[0] = mats_init[2 * j];
			value.mats[1] = mats_init[2 * j + 1];
			isp_hw_uvtune_set_transform(isp_id, &index, &value);
		}
	}
	isp_write_reg(UVT_MEM_LOAD_MASK, UV_TUNE_MEM_SYNC);

	return RTS_ISP_OK;
}

static int isp_hw_uvtune_init(uint32_t isp_id)
{
	isp_hw_uvtune_init_memregs(isp_id);
	isp_write_reg(UVS_EN_MASK, UV_TUNE_UVS_EN);
	isp_write_reg(UVTUNE_EN_MASK, UV_TUNE_UVTUNE_EN);
	isp_write_reg(1, UV_TUNE_UVPP_BY_MI_EN);
	isp_write_reg(1, UV_TUNE_GLOBGAIN_BY_MI_EN);

	return RTS_ISP_OK;
}

static int isp_hw_uvtune_set_uvtune(uint32_t isp_id,
									const isp_iq_uvtune_v1_index_t *indexes,
									const isp_iq_uvtune_v1_dyn_item_t *item)
{
	int i;

	for (i = 0; i < item->values_count; i++)
		isp_hw_uvtune_set_transform(isp_id, &indexes[i],
									&item->values[i]);
	isp_write_reg(UVT_MEM_LOAD_MASK, UV_TUNE_MEM_SYNC);
	isp_hw_uvtune_set_suppress(isp_id, item->suppress);

	return RTS_ISP_OK;
}

static int isp_hw_uvtune_wait_sync(uint32_t isp_id)
{
	int cnt = 100;

	while (cnt--) {
		if (isp_read_reg(UV_TUNE_MEM_SYNC) == 0) {
			break;
		}
		isp_os_usleep(1000);
	}
	if (cnt >= 0) {
		return RTS_ISP_OK;
	} else {
		return -RTS_ISP_ENOTREADY;
	}
}

static int isp_hw_uvtune_estimation(uint32_t isp_id)
{
	int ret;
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	ret = isp_hw_uvt_est_dynamic(&info);
	if (ret) {
		return ret;
	}
	isp_write_reg(UVT_REG_LOAD_MASK, UV_TUNE_REG_SYNC);

	return 0;
}

static const struct isp_hw_uvtune_ops hw_uvtune_ops = {
	.init = isp_hw_uvtune_init,
	.set_uvtune = isp_hw_uvtune_set_uvtune,
	.wait_sync = isp_hw_uvtune_wait_sync,
	.est_dynamic = isp_hw_uvtune_estimation,
};

const struct isp_hw_uvtune_ops *isp_hw_uvtune_get_ops(void)
{
	return &hw_uvtune_ops;
}
