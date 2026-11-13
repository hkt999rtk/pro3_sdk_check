#ifndef _ISP_MOD_SENSOR_H_INC_
#define _ISP_MOD_SENSOR_H_INC_

#include <stdint.h>
#include <rts_isp_geom.h>
#include <rts_isp_ae.h>
#include <rts_isp_sensor.h>
#include <isp_utils_common.h>

#define ISP_PLUGIN_UPDATE_SENSOR -1

struct isp_front_size {
	int32_t bit_depth; /* enum rts_isp_sensor_bit_depth */
	rts_isp_size_t snr;
	rts_isp_size_t isp;
};

enum isp_mod_sensor_get_info_type {
	SENSOR_INFO_CROP = isp_action_r(INFO_TYPE, 0, rts_isp_point_t),
	SENSOR_INFO_MODE =
		isp_action_r(INFO_TYPE, 1, const struct isp_sensor_mode *),
	SENSOR_INFO_INTERFACE = isp_action_r(
								INFO_TYPE, 2, const struct rts_isp_interface_info *),
	SENSOR_INFO_BOUND_NAME = isp_action_r(INFO_TYPE, 3, char[32]),
	SENSOR_INFO_VCM = isp_action_r(INFO_TYPE, 4, struct rts_isp_focus_info),
	SENSOR_INFO_PATCH_OPS = isp_action_r(INFO_TYPE, 5, const void *),
	SENSOR_INFO_VERSION = isp_action_r(INFO_TYPE, 6, uint16_t),
	SENSOR_INFO_TUNE_OPS = isp_action_r(
							   INFO_TYPE, 7, const struct rts_isp_exp_gain_tune_ops *),
	SENSOR_INFO_FRONT_SIZE =
		isp_action_r(INFO_TYPE, 8, struct isp_front_size),
};

struct isp_sensor_exp_gain {
	const struct rts_isp_ae_exp_gain *exp_gain;
	float fps;
};

/* for SENSOR_EXEC_I2C_TRANSFER */
struct isp_mod_sensor_i2c_msg {
	uint8_t i2c_id;
	uint8_t is_read;
	uint16_t len;
	/* i2c transfer buffer bebind here */
};

enum isp_mod_sensor_execute_type {
	SENSOR_EXEC_SET_FPS = isp_action_w(EXEC_TYPE, 0, float),
	SENSOR_EXEC_SET_EXP_GAIN =
		isp_action_w(EXEC_TYPE, 1, struct isp_sensor_exp_gain),
	SENSOR_EXEC_READ_SEN_REG =
		isp_action_wr(EXEC_TYPE, 2, struct rts_isp_i2c_reg),
	SENSOR_EXEC_WRITE_SEN_REG =
		isp_action_w(EXEC_TYPE, 3, struct rts_isp_i2c_reg),
	SENSOR_EXEC_SET_DUMMY = isp_action_w(EXEC_TYPE, 4, uint32_t),
	SENSOR_EXEC_SET_FOCUS = isp_action_w(EXEC_TYPE, 5, uint32_t),
	SENSOR_EXEC_ENUM_MODES =
		isp_action_r(EXEC_TYPE, 6, struct rts_isp_sensor_modes),
	SENSOR_EXEC_GET_MODE =
		isp_action_r(EXEC_TYPE, 7, struct rts_isp_sensor_mode),
	SENSOR_EXEC_SET_MODE =
		isp_action_w(EXEC_TYPE, 8, struct rts_isp_sensor_mode),
	SENSOR_EXEC_I2C_TRANSFER = isp_action_w(EXEC_TYPE, 9, void *),
	SENSOR_EXEC_SET_ISP_GAIN_DELAY = isp_action_w(EXEC_TYPE, 10, uint32_t),
};

#endif /* _ISP_MOD_SENSOR_H_INC_ */
