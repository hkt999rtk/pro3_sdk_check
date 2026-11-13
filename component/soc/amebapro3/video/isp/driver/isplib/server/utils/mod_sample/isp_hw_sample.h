#ifndef _ISP_HW_SAMPLE_H_INC_
#define _ISP_HW_SAMPLE_H_INC_

#include <stdint.h>

struct isp_hw_sample_ops {};

const struct isp_hw_sample_ops *isp_hw_sample_get_ops(void);

#endif /* _ISP_HW_SAMPLE_H_INC_ */
