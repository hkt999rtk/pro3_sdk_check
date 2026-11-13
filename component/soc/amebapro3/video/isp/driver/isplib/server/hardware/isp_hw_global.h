#ifndef _ISP_HW_GLOBAL_H_INC_
#define _ISP_HW_GLOBAL_H_INC_

#include <stdint.h>
#include <rts_isp.h>

struct isp_speed_ctrl {
	uint8_t speed_mult;
	uint8_t speed_div;
	uint16_t last_break;
};

struct isp_output_menu {
	int num;
	const char *const *menu;
};

struct isp_hw_global_ops {
	int (*set_func_enable)(uint32_t isp_id, int32_t type, int32_t hdr,
						   struct rts_isp_bypass bypass);
	int (*set_isp_path)(uint32_t isp_id, int32_t type);
	int (*enable_data)(uint32_t isp_id, int enable);
	int (*delay_data)(uint32_t isp_id, int delay);
	int (*select_interface)(uint32_t isp_id, int32_t inf);
	int (*start_transfer)(uint32_t isp_id);
	int (*stop_transfer)(uint32_t isp_id);
	int (*set_speed_ctrl)(uint32_t isp_id, struct isp_speed_ctrl *speed);
	int (*get_output_menu)(uint32_t isp_id, struct isp_output_menu *menu);
	int (*set_output)(uint32_t isp_id, uint32_t output);
	int (*set_line_cnt_int)(uint32_t isp_id, uint32_t line_cnt);
	int (*set_bayer)(uint32_t isp_id, int32_t bayer);
	int (*set_tnr_pos)(uint32_t isp_id, int32_t hdr);
};

const struct isp_hw_global_ops *isp_hw_global_get_ops(void);

#endif /* _ISP_HW_GLOBAL_H_INC_ */
