/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

// #include <isp_reg_input.h"
#include <stdio.h>
#include <isp_reg.h> // TODO: remove after move reg to header file
#include "isp_hw_input.h"

#define VERIFY_REG_BASE 0xf9000
#define VERIFY_SEL (VERIFY_REG_BASE + 0x0000)
#define VERIFY_CTRL (VERIFY_REG_BASE + 0x0004)
#define VERIFY_Y_DDR_ADDR0 (VERIFY_REG_BASE + 0x0008)
#define VERIFY_Y_DDR_ADDR1 (VERIFY_REG_BASE + 0x000C)
#define VERIFY_UV_DDR_ADDR0 (VERIFY_REG_BASE + 0x0010)
#define VERIFY_UV_DDR_ADDR1 (VERIFY_REG_BASE + 0x0014)
#define VERIFY_DDR_ADDR_SEL (VERIFY_REG_BASE + 0x0018)
#define VERIFY_Y_DDR_LEN (VERIFY_REG_BASE + 0x001C)
#define VERIFY_UV_DDR_LEN (VERIFY_REG_BASE + 0x0020)
#define VERIFY_FRAME_NUM (VERIFY_REG_BASE + 0x0024)
#define VERIFY_FRAME_FORMAT (VERIFY_REG_BASE + 0x0028)
#define VERIFY_FRAME_SIZE (VERIFY_REG_BASE + 0x002C)
#define VERIFY_FRAME_CONFG0 (VERIFY_REG_BASE + 0x0030)
#define VERIFY_FRAME_CONFG1 (VERIFY_REG_BASE + 0x0034)
#define VERIFY_INT_EN (VERIFY_REG_BASE + 0x0038)
#define VERIFY_INT_FLAG (VERIFY_REG_BASE + 0x003C)
#define VERIFY_START_FLAG (VERIFY_REG_BASE + 0x0040)
#define VERIFY_STOP_FLAG (VERIFY_REG_BASE + 0x0044)

static int __get_y_uv_len(uint32_t location, uint32_t width, uint32_t height,
						  uint32_t *y_len, uint32_t *uv_len)
{
	switch (location) {
	case INPUT_CROP_L_LOCATION:
	case INPUT_CROP_S_LOCATION:
	case INPUT_TNR_LOCATION:
	case INPUT_YUYV_LOCATION:
		*y_len = width * height * 2;
		*uv_len = 0;
		break;
	case INPUT_FUSION_IN_LOCATION:
	case INPUT_FUSION_OUT_LOCATION:
	case INPUT_RAW_BACKEND_LOCATION:
		*y_len = width * height * 4;
		*uv_len = 0;
		break;
	case INPUT_RGB_LOCATION:
	case INPUT_YUV_LOCATION:
		*y_len = (width * height + 2) / 3 * 16;
		*uv_len = 0;
		break;
	case INPUT_NV16_LOCATION:
		*y_len = width * height;
		*uv_len = width * height;
		break;
	default:
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int __get_location_fmt_reg(int location, uint32_t *location_reg,
								  uint32_t *format_reg)
{
	if (!location_reg || !format_reg) {
		return -RTS_ISP_EINVAL;
	}
	switch (location) {
	case INPUT_CROP_L_LOCATION:
		*location_reg = 0x1;
		*format_reg = 0x1;
		break;
	case INPUT_CROP_S_LOCATION:
		*location_reg = 0x400;
		*format_reg = 0x1;
		break;
	case INPUT_TNR_LOCATION:
		*location_reg = 0x800;
		*format_reg = 0x1;
		break;
	case INPUT_FUSION_IN_LOCATION:
		*location_reg = 0x2;
		*format_reg = 0x2;
		break;
	case INPUT_FUSION_OUT_LOCATION:
		*location_reg = 0x4;
		*format_reg = 0x2;
		break;
	case INPUT_RAW_BACKEND_LOCATION:
		*location_reg = 0x8;
		*format_reg = 0x2;
		break;
	case INPUT_RGB_LOCATION:
		*location_reg = 0x20;
		*format_reg = 0x3;
		break;
	case INPUT_YUV_LOCATION:
		*location_reg = 0x40;
		*format_reg = 0x3;
		break;
	case INPUT_YUYV_LOCATION:
		*location_reg = 0x80;
		*format_reg = 0x4;
		break;
	case INPUT_NV16_LOCATION:
		*location_reg = 0x80;
		*format_reg = 0x0;
		break;
	default:
		return -RTS_ISP_EINVAL;
	}
	return RTS_ISP_OK;
}

static int __set_verify_start(struct isp_hw_input_behavior *behavior)
{
	int ret;
	uint32_t y_len;
	uint32_t uv_len;
	uint32_t location_reg;
	uint32_t format_reg;

	ret = __get_y_uv_len(behavior->location, behavior->width,
						 behavior->height, &y_len, &uv_len);
	if (ret) {
		return ret;
	}
	ret = __get_location_fmt_reg(behavior->location, &location_reg,
								 &format_reg);
	if (ret) {
		return ret;
	}

	isp_write_reg(location_reg, VERIFY_SEL);
	isp_write_reg(0x3d, VERIFY_CTRL);
	isp_write_reg(y_len, VERIFY_Y_DDR_LEN);
	isp_write_reg(uv_len, VERIFY_UV_DDR_LEN);
	isp_write_reg(1, VERIFY_FRAME_NUM);
	isp_write_reg(format_reg, VERIFY_FRAME_FORMAT);
	isp_write_reg((behavior->width - 1) | (behavior->height - 1) << 16,
				  VERIFY_FRAME_SIZE);
	isp_write_reg(0x1010, VERIFY_FRAME_CONFG0);
	isp_write_reg((behavior->hblank - 1) | (behavior->vblank - 1) << 16,
				  VERIFY_FRAME_CONFG1);
	isp_write_reg(1, VERIFY_INT_FLAG);
	isp_write_reg(1, VERIFY_INT_EN);

	return RTS_ISP_OK;
}

static int __set_verify_stop(void)
{
	isp_write_reg(0, VERIFY_SEL);
	isp_write_reg(0, VERIFY_CTRL);
	isp_write_reg(0, VERIFY_INT_EN);

	return RTS_ISP_OK;
}

static int isp_hw_input_set_behavior(uint32_t isp_id,
									 struct isp_hw_input_behavior *behavior)
{
	int ret;

	if (behavior->ddr_mode) {
		ret = __set_verify_start(behavior);
	} else {
		ret = __set_verify_stop();
	}

	return ret;
}

static int isp_hw_input_set_buffer(uint32_t isp_id, uint32_t phy_addr)
{
	uint32_t uv_addr = phy_addr + isp_read_reg(VERIFY_Y_DDR_LEN);

	isp_write_reg(phy_addr, VERIFY_Y_DDR_ADDR0);
	isp_write_reg(uv_addr, VERIFY_UV_DDR_ADDR0);

	isp_write_reg(0x1, VERIFY_START_FLAG);

	return RTS_ISP_OK;
}

static const struct isp_hw_input_ops hw_input_ops = {
	.set_behavior = isp_hw_input_set_behavior,
	.set_buffer = isp_hw_input_set_buffer,
};

const struct isp_hw_input_ops *isp_hw_input_get_ops(void)
{
	return &hw_input_ops;
}
