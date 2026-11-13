#ifndef _ISP_MOD_YGAMMA_H_INC_
#define _ISP_MOD_YGAMMA_H_INC_

enum isp_mod_ygamma_execute_type {
	YGAMMA_EXEC_SET_PATCH_MODE = isp_action_w(EXEC_TYPE, 0, int32_t),
	YGAMMA_EXEC_SET_V1_DYN = isp_action_w(EXEC_TYPE, 1, void *),
};

#endif /* _ISP_MOD_YGAMMA_H_INC_ */
