#ifndef _ISP_HW_FBC_TNR_H_INC_
#define _ISP_HW_FBC_TNR_H_INC_

struct isp_hw_fbc_tnr_ops {
	int (*set_ddr)(uint32_t isp_id, uint32_t addr, uint32_t len);
	int (*estimation)(uint32_t isp_id);
};

const struct isp_hw_fbc_tnr_ops *isp_hw_fbc_tnr_get_ops(void);

#endif /* _ISP_HW_FBC_TNR_H_INC_ */
