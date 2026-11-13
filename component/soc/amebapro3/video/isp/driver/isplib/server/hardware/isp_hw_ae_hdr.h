#ifndef _ISP_HW_AE_HDR_H_INC_
#define _ISP_HW_AE_HDR_H_INC_

#include <rts_isp_sensor.h>

void isp_hw_ae_hdr_set_statis(uint32_t isp_id, int win_sample_num,
							  const rts_isp_grid_t *grid,
							  const rts_isp_point_t *end);
void isp_hw_ae_hdr_get_sync_regs(uint32_t isp_id, uint32_t hdr_gain[2], int i,
								 struct rts_isp_sync_reg *reg);

#endif /* _ISP_HW_AE_HDR_H_INC_ */
