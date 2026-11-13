#ifndef _ISP_MOD_INPUT_H_INC_
#define _ISP_MOD_INPUT_H_INC_

#include <rts_isp_define.h>
#include <isp_utils_common.h>

enum isp_mod_input_info_type {
	INPUT_INFO_SNR_OPS = isp_action_r(INFO_TYPE, 0, void *),
	INPUT_INFO_LARGE_IMAGE_MODE = isp_action_r(INFO_TYPE, 1, uint32_t),
};

enum isp_mod_input_fmt {
	ISP_INPUT_SBGGR16 = 0,
	ISP_INPUT_SGBRG16,
	ISP_INPUT_SGRBG16,
	ISP_INPUT_SRGGB16,
	ISP_INPUT_SGRBG32 = 6,
	ISP_INPUT_RGB = 8,
	ISP_INPUT_YUV = 12,
	ISP_INPUT_YUYV = 16,
	ISP_INPUT_NV16,
};

struct isp_mod_input_behavior {
	uint32_t ddr_mode;

	uint32_t fmt;
	uint32_t width;
	uint32_t height;
	struct rts_isp_fract fival;
};

struct isp_mod_input_buffer {
	uint32_t phy_addr;
};

enum isp_mod_input_exec_type {
	INPUT_EXEC_SET_BEHAVIOR =
		isp_action_w(EXEC_TYPE, 0, struct isp_mod_input_behavior),
	INPUT_EXEC_SET_BUFFER =
		isp_action_w(EXEC_TYPE, 1, struct isp_mod_input_buffer),
	INPUT_EXEC_SET_LARGE_IMAGE_MODE = isp_action_w(EXEC_TYPE, 2, uint32_t),
};

#endif /* _ISP_MOD_INPUT_H_INC_ */
