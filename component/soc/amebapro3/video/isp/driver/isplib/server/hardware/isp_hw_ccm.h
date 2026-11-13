#ifndef _ISP_HW_CCM_H_INC_
#define _ISP_HW_CCM_H_INC_

#include <stdint.h>
#include <rts_isp_geom.h>
#include <isp_iq_table.pb.h>

#define CCM_ARRAY_SIZE_MAX 9

struct isp_ccm_hw_info {
	rts_isp_grid_info_t grid_info;
};

struct isp_hw_ccm_ops {
	int (*init_vreg)(uint32_t isp_id);
	int (*get_hw_info)(uint32_t isp_id, struct isp_ccm_hw_info *info);
	int (*set_ccm)(uint32_t isp_id, const isp_iq_ccm_v1_dyn_item_t *ccm);
	int (*set_size)(uint32_t isp_id, const rts_isp_grid_t *grid);
	int (*est_dynamic)(uint32_t isp_id);
};

const struct isp_hw_ccm_ops *isp_hw_ccm_get_ops(void);

#endif /* _ISP_HW_CCM_H_INC_ */
