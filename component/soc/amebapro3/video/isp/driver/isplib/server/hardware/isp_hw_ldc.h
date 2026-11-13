#ifndef _ISP_HW_LDC_H_INC_
#define _ISP_HW_LDC_H_INC_

#include <stdint.h>

struct isp_hw_ldc_ops {
	int (*init)(uint32_t isp_id);
	int (*set_sync)(uint32_t isp_id);
};

const struct isp_hw_ldc_ops *isp_hw_ldc_get_ops(void);

#endif /* _ISP_HW_LDC_H_INC_ */
