#ifndef _ISP_HW_BLC_H_INC_
#define _ISP_HW_BLC_H_INC_

#include <rts_isp_geom.h>
#include <isp_iq_table.pb.h>

struct isp_hw_blc_ops {
	int (*est_dynamic)(uint32_t isp_id);
	int (*set_bit_depth)(uint32_t isp_id, int bits);
	int (*set)(uint32_t isp_id, const isp_iq_blc_v1_dyn_item_t *blc);
	int (*get_position)(uint32_t isp_id,
						int32_t *blc_pos); /* enum rts_isp_blc_position */
};

const struct isp_hw_blc_ops *isp_hw_blc_get_ops(void);
const struct isp_hw_blc_ops *isp_hw_blch_get_ops(void);

#endif /* _ISP_HW_BLC_H_INC_ */
