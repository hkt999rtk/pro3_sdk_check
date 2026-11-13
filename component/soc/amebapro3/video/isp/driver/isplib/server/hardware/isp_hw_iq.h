#ifndef _ISP_HW_IQ_H_INC_
#define _ISP_HW_IQ_H_INC_

#include <stdint.h>

struct isp_hw_iq_ops {
	uint32_t (*read_reg_mask)(uint32_t isp_id, uint32_t offset,
							  uint32_t mask);
	void (*write_reg_mask)(uint32_t isp_id, uint32_t value, uint32_t offset,
						   uint32_t mask);
};

const struct isp_hw_iq_ops *isp_hw_iq_get_ops(void);

#endif /* _ISP_HW_IQ_H_INC_ */
