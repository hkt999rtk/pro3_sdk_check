/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_mem.h>
#include <isp_client_message.h>
#include <isp_mod.h>
#include <isp_mod_sensor.h>
#include <isp_mod_interface.h>
#include <isp_mod_input.h>

int rts_av_isp_enum_sensor_modes(uint32_t isp_id,
								 struct rts_isp_sensor_modes *modes)
{
	return isp_client_message_simple(isp_id, MOD_SENSOR,
									 SENSOR_EXEC_ENUM_MODES, modes,
									 sizeof(*modes));
}

int rts_av_isp_get_sensor_mode(uint32_t isp_id,
							   struct rts_isp_sensor_mode *mode)
{
	return isp_client_message_simple(
			   isp_id, MOD_SENSOR, SENSOR_EXEC_GET_MODE, mode, sizeof(*mode));
}

int rts_av_isp_set_sensor_mode(uint32_t isp_id,
							   struct rts_isp_sensor_mode *mode)
{
	return isp_client_message_simple(
			   isp_id, MOD_SENSOR, SENSOR_EXEC_SET_MODE, mode, sizeof(*mode));
}

static int __i2c_msg_serial(struct isp_msg_buf *msg, uint8_t i2c_id,
							void *i2c_buf, uint32_t len, int is_read)
{
	int ret;
	void *buf;
	void *end;
	struct isp_mod_sensor_i2c_msg *buf_i2c_msg;

	buf = msg->data;
	end = buf + isp_msg_data_len(&msg->hdr);

	ret = get_buf_ptr(buf_i2c_msg, buf, end);
	if (ret) {
		return ret;
	}
	buf_i2c_msg->i2c_id = i2c_id;
	buf_i2c_msg->is_read = is_read;
	buf_i2c_msg->len = len;

	if (!is_read) {
		ret = copy_from_src(buf, i2c_buf, len, end);
		if (ret) {
			return ret;
		}
	}

	msg->hdr.msg_len = buf - msg->data + sizeof(msg->hdr);
	msg->hdr.capacity = is_read ? msg->hdr.msg_len + len : msg->hdr.msg_len;

	return RTS_ISP_OK;
}

static int __i2c_msg_deserial(struct isp_msg_buf *msg, uint8_t i2c_id,
							  void *i2c_buf, uint32_t len, int is_read)
{
	int ret;
	void *buf;
	void *end;
	struct isp_mod_sensor_i2c_msg *buf_i2c_msg;

	if (!msg) {
		return -RTS_ISP_EINVAL;
	}

	if (msg->hdr.ret_val) {
		return msg->hdr.ret_val;
	}

	if (!is_read) {
		if (isp_msg_data_len(&msg->hdr) != 0) {
			return -RTS_ISP_EINVAL;
		}
		return RTS_ISP_OK;
	}

	buf = msg->data;
	end = buf + isp_msg_data_len(&msg->hdr);

	ret = get_buf_ptr(buf_i2c_msg, buf, end);
	if (ret) {
		return ret;
	}
	if (buf_i2c_msg->i2c_id != i2c_id || buf_i2c_msg->is_read != is_read ||
		buf_i2c_msg->len != len) {
		return -RTS_ISP_EINVAL;
	}

	return copy_to_src(i2c_buf, buf, len, end);
}

static int __process_i2c_transfer(uint32_t isp_id, uint8_t i2c_id, void *buf,
								  uint32_t len, int is_read)
{
	int ret;
	struct isp_msg_buf *msg;

	if (isp_id >= _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	msg = isp_msg_new(NULL, 4096, 0);
	if (!msg) {
		return -RTS_ISP_ENOMEM;
	}

	ret = __i2c_msg_serial(msg, i2c_id, buf, len, is_read);
	if (ret) {
		goto out;
	}

	msg->hdr.flags = ISP_MSG_TYPE_RAW;
	msg->hdr.isp_id = isp_id;
	msg->hdr.mod_id = MOD_SENSOR;
	msg->hdr.action = SENSOR_EXEC_I2C_TRANSFER;
	msg->hdr.ret_val = -1;

	ret = isp_client_message_process(msg);
	if (ret) {
		goto out;
	}
	ret = __i2c_msg_deserial(msg, i2c_id, buf, len, is_read);
out:
	isp_release(msg, isp_msg_put);
	return ret;
}

int rts_isp_controller_i2c_read(uint32_t isp_id, uint8_t i2c_id, void *buf,
								int len)
{
	return __process_i2c_transfer(isp_id, i2c_id, buf, len, 1);
}

int rts_isp_controller_i2c_write(uint32_t isp_id, uint8_t i2c_id, void *buf,
								 int len)
{
	return __process_i2c_transfer(isp_id, i2c_id, buf, len, 0);
}

int rts_isp_controller_read_sensor_reg(uint32_t isp_id,
									   struct rts_isp_i2c_reg *reg)
{
	return isp_client_message_simple(isp_id, MOD_SENSOR,
									 SENSOR_EXEC_READ_SEN_REG, reg,
									 sizeof(*reg));
}

int rts_isp_controller_write_sensor_reg(uint32_t isp_id,
										struct rts_isp_i2c_reg *reg)
{
	return isp_client_message_simple(isp_id, MOD_SENSOR,
									 SENSOR_EXEC_WRITE_SEN_REG, reg,
									 sizeof(*reg));
}

int rts_isp_set_input_behavior(uint32_t isp_id, uint32_t mode, uint32_t fmt,
							   uint32_t width, uint32_t height, float fps)
{
	struct isp_mod_input_behavior behavior;

	behavior.ddr_mode = mode;
	behavior.fmt = fmt;
	behavior.width = width;
	behavior.height = height;
	behavior.fival.denominator = fps * 1000;
	behavior.fival.numerator = 1000;

	return isp_client_message_simple(isp_id, MOD_INPUT,
									 INPUT_EXEC_SET_BEHAVIOR, &behavior,
									 sizeof(behavior));
}

int rts_isp_set_large_image_mode(uint32_t isp_id, uint32_t mode)
{
	return isp_client_message_simple(isp_id, MOD_INPUT,
									 INPUT_EXEC_SET_LARGE_IMAGE_MODE, &mode,
									 sizeof(mode));
}
