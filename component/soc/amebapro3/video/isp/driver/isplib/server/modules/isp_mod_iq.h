#ifndef _ISP_MOD_IQ_H_INC_
#define _ISP_MOD_IQ_H_INC_

#include <rtsavisp.h>
#include <isp_utils_common.h>
#include <rts_isp_dynamic.h>

struct isp_mod_iq_packed_iq_info {
	uint32_t version;
	uint32_t cus_version;
	uint64_t timestamp;
};

enum isp_mod_iq_get_info_type {
	IQ_INFO_DYNAMIC =
		isp_action_r(INFO_TYPE, 0, const struct isp_notify_dynamic *),
	IQ_INFO_PACKED_IQ =
		isp_action_r(INFO_TYPE, 1, struct isp_mod_iq_packed_iq_info),
	IQ_INFO_PLUGINS_VERSION =
		isp_action_r(INFO_TYPE, 2, struct rts_isp_plugins_version),
};

/* for IQ_EXEC_GET/SET_ALGO_PRAM/IQ_TABLE */
struct isp_mod_iq_tuning_buffer {
	uint32_t len;
	uint32_t type;
	/* iq table or algo param buffer bebind here */
};

/* for IQ_EXEC_READ/WRITE_REGS */
struct isp_mod_iq_tuning_regs {
	uint32_t num;
	/* struct rts_isp_tuning_reg array bebind here */
};

enum isp_mod_iq_read_write_type {
	ISP_MOD_IQ_READ_REG,
	ISP_MOD_IQ_WRITE_REG,
};

struct isp_mod_iq_read_write_reg {
	int32_t type; /* enum isp_mod_iq_read_write_type */
	uint32_t value;
	uint32_t offset;
	uint32_t mask;
};

enum isp_mod_iq_execute_type {
	IQ_EXEC_SET_DYNAMIC_AE =
		isp_action_w(EXEC_TYPE, 0, struct isp_notify_dynamic_ae),
	IQ_EXEC_SET_DYNAMIC_AWB =
		isp_action_w(EXEC_TYPE, 1, struct isp_notify_dynamic_awb),
	IQ_EXEC_GET_ALGO_PARAM = isp_action_w(EXEC_TYPE, 2, void *),
	IQ_EXEC_SET_ALGO_PARAM = isp_action_w(EXEC_TYPE, 3, void *),
	IQ_EXEC_GET_IQ_TABLE = isp_action_w(EXEC_TYPE, 4, void *),
	IQ_EXEC_SET_IQ_TABLE = isp_action_w(EXEC_TYPE, 5, void *),
	IQ_EXEC_SET_DYNAMIC_SENSOR =
		isp_action_w(EXEC_TYPE, 6, struct isp_notify_dynamic_sensor),
	IQ_EXEC_READ_REGS = isp_action_w(EXEC_TYPE, 7, void *),
	IQ_EXEC_WRITE_REGS = isp_action_w(EXEC_TYPE, 8, void *),
	IQ_EXEC_RELOAD_PACKED_IQ = isp_action(EXEC_TYPE, 9),
	IQ_EXEC_GET_SCENE = isp_action_r(EXEC_TYPE, 10, int32_t),
	IQ_EXEC_SET_SCENE = isp_action_w(EXEC_TYPE, 11, int32_t),
	IQ_EXEC_APPLY_IQ = isp_action_w(EXEC_TYPE, 12, uint32_t),
	IQ_EXEC_READ_WRITE_REG =
		isp_action_wr(EXEC_TYPE, 13, struct isp_mod_iq_read_write_reg),
};

#endif /* _ISP_MOD_IQ_H_INC_ */
