#ifndef _ISP_MOD_INTERFACE_H_INC_
#define _ISP_MOD_INTERFACE_H_INC_

#include <isp_utils_common.h>

struct isp_interface_novc_delay {
	uint32_t channel[RTS_ISP_HDR_CHAN_MAX - 1];
};

enum isp_mod_interface_execution_type {
	INTERFACE_EXEC_START = isp_action(EXEC_TYPE, 0),
	INTERFACE_EXEC_STOP = isp_action(EXEC_TYPE, 1),
	INTERFACE_EXEC_SHORT_EXP_DELAY =
		isp_action_w(EXEC_TYPE, 2, struct isp_interface_novc_delay),
};

#endif /* _ISP_MOD_INTERFACE_H_INC_ */
