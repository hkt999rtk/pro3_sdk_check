#ifndef _ISP_HW_INPUT_H_INC_
#define _ISP_HW_INPUT_H_INC_

#include <stdint.h>

enum isp_hw_input_location {
	INPUT_CROP_L_LOCATION,
	INPUT_CROP_S_LOCATION,
	INPUT_TNR_LOCATION,
	INPUT_FUSION_IN_LOCATION,
	INPUT_FUSION_OUT_LOCATION,
	INPUT_RAW_BACKEND_LOCATION,
	INPUT_RGB_LOCATION,
	INPUT_YUV_LOCATION,
	INPUT_YUYV_LOCATION,

	INPUT_NV16_LOCATION,
};

struct isp_hw_input_behavior {
	uint32_t ddr_mode;

	uint32_t location;
	uint32_t width;
	uint32_t height;
	uint32_t hblank;
	uint32_t vblank;
};


struct isp_hw_input_ops {
	int (*set_behavior)(uint32_t isp_id,
						struct isp_hw_input_behavior *behavior);
	int (*set_buffer)(uint32_t isp_id, uint32_t phy_addr);
};

const struct isp_hw_input_ops *isp_hw_input_get_ops(void);

#endif /* _ISP_HW_INPUT_H_INC_ */
