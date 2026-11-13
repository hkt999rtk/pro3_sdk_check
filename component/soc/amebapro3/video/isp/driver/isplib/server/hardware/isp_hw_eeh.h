#ifndef _ISP_HW_EEN_H_INC_
#define _ISP_HW_EEN_H_INC_

#include <stdint.h>
#include <isp_iq_table.pb.h>

#define SHARPNESS_CTRL_MIN 0
#define SHARPNESS_CTRL_DEF 50
#define SHARPNESS_CTRL_MAX 100

struct isp_hw_eeh_ops {
	int (*init)(uint32_t isp_id);
	int (*init_vreg)(uint32_t isp_id);
	int (*estimation)(uint32_t isp_id);
	int (*set_sharpness)(uint32_t isp_id, int sharpness);
	int (*get_position)(uint32_t isp_id);
	int (*set_dyn_item)(uint32_t isp_id,
						const isp_iq_eeh_v1_dyn_item_t *item);
};

const struct isp_hw_eeh_ops *isp_hw_eeh_get_ops(void);

#endif /* _ISP_HW_EEN_H_INC_ */
