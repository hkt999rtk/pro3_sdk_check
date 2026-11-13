#ifndef _ISP_HW_INTERFACE_H_INC_
#define _ISP_HW_INTERFACE_H_INC_

#include <rts_isp_sensor.h>

struct isp_interface_info {
	const struct rts_isp_interface_info *info;
	rts_isp_size_t size;
	uint32_t pixel_clk;
	uint32_t interface_clk;
	int is_fpga;
};

struct isp_hw_interface_ops {
	int (*start_transfer)(uint32_t isp_id,
						  const struct isp_interface_info *info);
	int (*stop_transfer)(uint32_t isp_id,
						 const struct isp_interface_info *info);
	int (*set_hdr_delay)(uint32_t isp_id, uint32_t short_exp_delay);
};

const struct isp_hw_interface_ops *isp_hw_dvp_get_ops(void);
const struct isp_hw_interface_ops *isp_hw_mipi_get_ops(void);
const struct isp_hw_interface_ops *isp_hw_vinf_get_ops(void);

#endif /* _ISP_HW_INTERFACE_H_INC_ */
