#ifndef _ISP_MOD_VCTRL_H_INC_
#define _ISP_MOD_VCTRL_H_INC_

#include <rts_isp_ctrl.h>
#include <isp_utils_common.h>

/* for G/S/TRY_EXT_CTRLS */
struct isp_mod_vctrl_ext_controls {
	uint32_t ctrl_class;
	uint32_t count;
	uint32_t error_idx;
	int32_t request_fd;
	/* struct isp_v4l2_ext_control array bebind here */
};

enum isp_mod_vctrl_execution_type {
	VCTRL_EXEC_QUERY_CTRL =
		isp_action_wr(EXEC_TYPE, 0, struct isp_v4l2_queryctrl),
	VCTRL_EXEC_QUERY_MENU =
		isp_action_wr(EXEC_TYPE, 1, struct isp_v4l2_querymenu),
	VCTRL_EXEC_G_CTRL =
		isp_action_wr(EXEC_TYPE, 2, struct isp_v4l2_control),
	VCTRL_EXEC_S_CTRL =
		isp_action_wr(EXEC_TYPE, 3, struct isp_v4l2_control),
	VCTRL_EXEC_QUERY_EXT_CTRL =
		isp_action_wr(EXEC_TYPE, 4, struct isp_v4l2_query_ext_ctrl),
	VCTRL_EXEC_G_EXT_CTRLS = isp_action_w(EXEC_TYPE, 5, void *),
	VCTRL_EXEC_S_EXT_CTRLS = isp_action_w(EXEC_TYPE, 6, void *),
	VCTRL_EXEC_TRY_EXT_CTRLS = isp_action_w(EXEC_TYPE, 7, void *),
};

#endif /* _ISP_MOD_VCTRL_H_INC_ */
