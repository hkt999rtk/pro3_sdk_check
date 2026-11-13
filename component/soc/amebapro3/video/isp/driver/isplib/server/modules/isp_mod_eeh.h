#ifndef _ISP_MOD_EEH_H_INC_
#define _ISP_MOD_EEH_H_INC_

enum isp_mod_eeh_info_type {
	EEH_INFO_POSITION = isp_action_r(INFO_TYPE, 0, int32_t),
};

#endif /* _ISP_MOD_EEH_H_INC_ */
