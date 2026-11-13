#ifndef _ISP_HW_FBC_VHDR_H_INC_
#define _ISP_HW_FBC_VHDR_H_INC_

struct isp_hw_fbc_vhdr_ops {
	int (*set_ddr)(uint32_t isp_id, uint32_t addr, uint32_t len,
				   uint32_t frame_len);
	int (*estimation)(uint32_t isp_id);
};

const struct isp_hw_fbc_vhdr_ops *isp_hw_fbc_vhdr_get_ops(void);

#endif /* _ISP_HW_FBC_VHDR_H_INC_ */
