#ifndef _ISP_HW_YGAMMA_H_INC_
#define _ISP_HW_YGAMMA_H_INC_

#include <isp_iq_table.pb.h>

#define YGAMMA_ARRAY_SIZE_MAX 16

struct isp_hw_ygamma_ops {
	int (*set_ygamma)(uint32_t isp_id,
					  const isp_iq_ygamma_v1_dyn_item_t *item);
};

const struct isp_hw_ygamma_ops *isp_hw_ygamma_get_ops(void);

#endif /* _ISP_HW_YGAMMA_H_INC_ */
