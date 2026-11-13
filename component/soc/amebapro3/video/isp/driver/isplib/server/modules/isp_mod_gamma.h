#ifndef _ISP_MOD_GAMMA_H_INC_
#define _ISP_MOD_GAMMA_H_INC_

enum isp_mod_gamma_execute_type {
	GAMMA_EXEC_SET_PATCH_MODE = isp_action_w(EXEC_TYPE, 0, int32_t),
	GAMMA_EXEC_SET_V1_DYN = isp_action_w(EXEC_TYPE, 1, void *),
};

#endif /* _ISP_MOD_GAMMA_H_INC_ */
