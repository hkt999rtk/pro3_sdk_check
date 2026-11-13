#ifndef _ISP_HW_TNR_H_INC_
#define _ISP_HW_TNR_H_INC_

#include <stdint.h>

struct isp_hw_tnr_ops {
	int (*enable)(uint32_t isp_id, int enable);
	int (*enable_axi)(uint32_t isp_id, int enable);
	int (*set_snr_depth)(uint32_t isp_id, int32_t bit_depth);
	int (*est_dynamic)(uint32_t isp_id);
};

const struct isp_hw_tnr_ops *isp_hw_tnr_get_ops(void);

#endif /* _ISP_HW_TNR_H_INC_ */
