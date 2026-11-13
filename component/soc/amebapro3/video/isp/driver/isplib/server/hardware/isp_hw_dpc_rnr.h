#ifndef _ISP_HW_DPC_RNR_H_INC_
#define _ISP_HW_DPC_RNR_H_INC_

struct isp_hw_dpc_rnr_ops {
	int (*est_dynamic)(uint32_t isp_id);
};

const struct isp_hw_dpc_rnr_ops *isp_hw_dpc_rnr_get_ops(void);

#endif /* _ISP_HW_DPC_RNR_H_INC_ */
