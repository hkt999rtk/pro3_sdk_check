#ifndef _ISP_HW_SENSOR_H_INC_
#define _ISP_HW_SENSOR_H_INC_

#include <stdint.h>

struct isp_hw_sensor_ops {
	int (*min_exp_delay)(uint32_t isp_id);
	int (*set_comp)(uint32_t isp_id, int enable);
	int (*set_decomp)(uint32_t isp_id, int enable,
					  const struct rts_isp_compand_table *comp);
};

const struct isp_hw_sensor_ops *isp_hw_sensor_get_ops(void);

#endif /* _ISP_HW_SENSOR_H_INC_ */
