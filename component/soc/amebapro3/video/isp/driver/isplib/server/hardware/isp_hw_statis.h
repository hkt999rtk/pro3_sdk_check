#ifndef _ISP_HW_STATIS_H_INC_
#define _ISP_HW_STATIS_H_INC_

#include <stdint.h>

struct isp_hw_statis_ops {
	int (*enable)(uint32_t isp_id, int enable, int hdr_mode);
	int (*set_cfg)(uint32_t isp_id, uint32_t base, uint32_t size);
};

const struct isp_hw_statis_ops *isp_hw_statis_get_ops(void);

#endif /* _ISP_HW_STATIS_H_INC_ */
