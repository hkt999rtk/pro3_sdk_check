#ifndef _ISP_HW_INTP_H_INC_
#define _ISP_HW_INTP_H_INC_

#include <stdint.h>
#include <rts_isp_geom.h>

struct isp_hw_intp_ops {
	int (*init)(uint32_t isp_id);
	int (*est_per_frame)(uint32_t isp_id);
	int (*est_dynamic)(uint32_t isp_id);
};

const struct isp_hw_intp_ops *isp_hw_intp_get_ops(void);

#endif /* _ISP_HW_INTP_H_INC_ */
