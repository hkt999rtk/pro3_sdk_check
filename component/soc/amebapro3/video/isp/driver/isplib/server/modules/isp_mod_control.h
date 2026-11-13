#ifndef _ISP_MOD_CONTROL_H_INC_
#define _ISP_MOD_CONTROL_H_INC_

#include <stdint.h>
#include <rts_isp_define.h>
#include <isp_utils_common.h>
#include <isp_statis_common.h>

struct isp_count {
	uint32_t frame_count;
	uint32_t statis_count;
};

enum isp_mod_control_info_type {
	CONTROL_INFO_TOP_STATUS = isp_action_r(INFO_TYPE, 0, int),
};

struct isp_preview_info {
	struct rts_isp_fract fps;
};

enum isp_mod_control_execute_type {
	CONTROL_EXEC_SET_FPS =
		isp_action_w(EXEC_TYPE, 0, struct isp_preview_info),
	CONTROL_EXEC_SET_TRACK = isp_action_w(EXEC_TYPE, 1, int),
	CONTROL_EXEC_SET_WATCHDOG = isp_action_w(EXEC_TYPE, 2, uint32_t),
	CONTROL_EXEC_GET_COUNT = isp_action_wr(EXEC_TYPE, 3, struct isp_count),
};

#endif /* _ISP_MOD_CONTROL_H_INC_ */
