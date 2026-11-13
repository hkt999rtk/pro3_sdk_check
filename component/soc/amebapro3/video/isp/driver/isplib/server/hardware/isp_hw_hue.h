#ifndef _ISP_HW_HUE_H_INC_
#define _ISP_HW_HUE_H_INC_

#include <stdint.h>

struct isp_hw_hue_ops {
	int (*set_hue)(uint32_t isp_id, int hue);
};

const struct isp_hw_hue_ops *isp_hw_hue_get_ops(void);

#endif /* _ISP_HW_HUE_H_INC_ */
