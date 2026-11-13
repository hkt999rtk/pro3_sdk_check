#ifndef _ISP_HW_FUSION_H_INC_
#define _ISP_HW_FUSION_H_INC_

struct isp_hw_fusion_ops {
	int (*init)(uint32_t isp_id);
	int (*est_dynamic)(uint32_t isp_id);
};

const struct isp_hw_fusion_ops *isp_hw_fusion_get_ops(void);

#endif /* _ISP_HW_FUSION_H_INC_ */
