#ifndef _ISP_HW_LCE_H_INC_
#define _ISP_HW_LCE_H_INC_

#include <rts_isp_geom.h>

struct isp_hw_lce_ops {
	int (*init_vreg)(uint32_t isp_id);
	int (*set_size)(uint32_t isp_id, const rts_isp_rect_t *rect);
};

const struct isp_hw_lce_ops *isp_hw_lce_get_ops(void);

#endif /* _ISP_HW_LCE_H_INC_ */
