/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2024 Sally Peng <sally_peng@realsil.com.cn>
 */

#include <math.h>
#include <isp_reg_vhdr_axi.h>
#include <isp_reg_fbc_vhdr.h>
#include "isp_hw_fbc_vhdr.h"
#include "isp_os_log.h"

#define RTC_QT_SECTION_NUM 11
#define MOD_NAME FBC_VHDR_FBC

static void fbc_qt_val_val(int *step_ext, int *idx, int *val)
{
	for (int i = 0; i < RTC_QT_SECTION_NUM - 1; i++) {
		val[i + 1] = val[i] + step_ext[i] * (idx[i + 1] - idx[i]);
	}

	//for (j = 0; j < RTC_QT_SECTION_NUM; j++) {
	//	if (j == 0)
	//		dst_val[j] = src->min;
	//	else
	//		dst_val[j] = dst_val[j - 1] + dst_step[j - 1] *
	//		(dst_idx[j] - dst_idx[j - 1]);
	//}
}

static void isp_hw_qt_6bits_1_0(FW_EST_MOD_INFO mod_info)
{
	int step_idx,
		step_ext[RTC_QT_SECTION_NUM - 1] = { 0 },
										   idx[RTC_QT_SECTION_NUM] = { 0 },
												   val[RTC_QT_SECTION_NUM] = { 0 };

	for (int i = 0; i < RTC_QT_SECTION_NUM; i++) {
		idx[i] = READ_REG_ARRAY_I(mod_info, FBC_QT_6BIT_IDX, i);
		val[i] = READ_REG_ARRAY_I(mod_info, FBC_QT_6BIT_VAL, i);
		if (i < RTC_QT_SECTION_NUM - 1) {
			step_idx =
				READ_REG_ARRAY_I(mod_info, FBC_QT_6BIT_STEP, i);
			step_ext[i] = step_idx ?
						  (int)pow((float)2,
								   (float)(step_idx - 1)) :
						  0;
		}
	}

	fbc_qt_val_val(step_ext, idx, val);

	for (int i = 0; i < RTC_QT_SECTION_NUM; i++) {
		WRITE_REG_ARRAY_I(mod_info, FBC_QT_6BIT_VAL, i, val[i]);
	}
}

static void isp_hw_qt_7bits_2_0(FW_EST_MOD_INFO mod_info)
{
	int step_idx,
		step_ext[RTC_QT_SECTION_NUM - 1] = { 0 },
										   idx[RTC_QT_SECTION_NUM] = { 0 },
												   val[RTC_QT_SECTION_NUM] = { 0 };

	for (int i = 0; i < RTC_QT_SECTION_NUM; i++) {
		idx[i] = READ_REG_ARRAY_I(mod_info, FBC_QT_7BIT_IDX0, i);
		val[i] = READ_REG_ARRAY_I(mod_info, FBC_QT_7BIT_VAL0, i);
		if (i < RTC_QT_SECTION_NUM - 1) {
			step_idx = READ_REG_ARRAY_I(mod_info, FBC_QT_7BIT_STEP0,
										i);
			step_ext[i] = step_idx ?
						  (int)pow((float)2,
								   (float)(step_idx - 1)) :
						  0;
		}
	}

	fbc_qt_val_val(step_ext, idx, val);

	for (int i = 0; i < RTC_QT_SECTION_NUM; i++) {
		WRITE_REG_ARRAY_I(mod_info, FBC_QT_7BIT_VAL0, i, val[i]);
	}
}

static void isp_hw_qt_7bits_3_0(FW_EST_MOD_INFO mod_info)
{
	int step_idx,
		step_ext[RTC_QT_SECTION_NUM - 1] = { 0 },
										   idx[RTC_QT_SECTION_NUM] = { 0 },
												   val[RTC_QT_SECTION_NUM] = { 0 };

	for (int i = 0; i < RTC_QT_SECTION_NUM; i++) {
		idx[i] = READ_REG_ARRAY_I(mod_info, FBC_QT_7BIT_IDX1, i);
		val[i] = READ_REG_ARRAY_I(mod_info, FBC_QT_7BIT_VAL1, i);
		if (i < RTC_QT_SECTION_NUM - 1) {
			step_idx = READ_REG_ARRAY_I(mod_info, FBC_QT_7BIT_STEP1,
										i);
			step_ext[i] = step_idx ?
						  (int)pow((float)2,
								   (float)(step_idx - 1)) :
						  0;
		}
	}

	fbc_qt_val_val(step_ext, idx, val);

	for (int i = 0; i < RTC_QT_SECTION_NUM; i++) {
		WRITE_REG_ARRAY_I(mod_info, FBC_QT_7BIT_VAL1, i, val[i]);
	}
}

static int isp_hw_fbc_est_dynamic(FW_EST_MOD_INFO mod_info)
{
	/* -------------- New DPMC QT -------------- */
	isp_hw_qt_6bits_1_0(mod_info);

	/* -------------- DPCM - A QT -------------- */
	isp_hw_qt_7bits_2_0(mod_info);

	/* -------------- DPCM - B QT -------------- */
	isp_hw_qt_7bits_3_0(mod_info);

	return 0;
}

static int isp_hw_fbc_vhdr_set_ddr(uint32_t isp_id, uint32_t addr, uint32_t len,
								   uint32_t frame_len)
{
	isp_debug("[%s][L:%d] vHDR debug isp_id:%d, addr:0x%X, len:%d, frame_len:%d.\r\n", __func__, __LINE__, isp_id, addr, len, frame_len);
	isp_write_reg(addr, VHDR_STREAM_FRAME_ADDR);
	isp_write_reg(frame_len, VHDR_STREAM_DDR_LEN);
	isp_write_reg(len, VHDR_STREAM_RING_LEN);
	isp_set_reg_bit(DDR_DATA_READY_EN_OFFSET, VHDR_STREAM_CTRL);
	isp_write_reg(256, VHDR_STREAM_START_PIXELNUM);

	if (len) {
		isp_write_reg(1, FBC_VHDR_FBC_EN);
	} else {
		isp_write_reg(0, FBC_VHDR_FBC_EN);
	}

	return RTS_ISP_OK;
}

static int isp_hw_fbc_vhdr_estimation(uint32_t isp_id)
{
	struct isp_hw_mod_info info;

	info.isp_id = isp_id;
	info.chan_offset = 0;

	return isp_hw_fbc_est_dynamic(&info);
}

static const struct isp_hw_fbc_vhdr_ops hw_fbc_vhdr_ops = {
	.set_ddr = isp_hw_fbc_vhdr_set_ddr,
	.estimation = isp_hw_fbc_vhdr_estimation,
};

const struct isp_hw_fbc_vhdr_ops *isp_hw_fbc_vhdr_get_ops(void)
{
	return &hw_fbc_vhdr_ops;
}
