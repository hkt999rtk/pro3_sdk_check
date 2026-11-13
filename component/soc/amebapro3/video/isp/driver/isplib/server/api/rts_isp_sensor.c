/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2019 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_mem.h>
#include <isp_os_time.h>
#include <isp_os_driver.h>
#include <isp_top.h>
#include <isp_mod.h>
#include <isp_sensor.h>
#include <isp_mod_ae.h>
#include <isp_mod_sensor.h>
#include <isp_mod_interface.h>
#include <isp_mod_ae.h>
#include <isp_mod_global.h>
#include <isp_client_message.h>

/* Batch consecutive-address register writes into single I2C transactions.
 * Requires sensor to support auto-increment addressing. Disable if sensor
 * does not support it. */
#ifndef CONFIG_I2C_BATCH_WRITE
#define CONFIG_I2C_BATCH_WRITE 1
#endif

/* Max batch buffer: 64 bytes (rtscam_isp_i2c_msg.buf limit) */
#define I2C_BATCH_BUF_SIZE 64

static int process_i2c_regs_write_batch(const struct rts_isp_i2c_info *info,
										const struct rts_isp_i2c_regs *regs)
{
	int i, j, buf_pos, reg_count;
	int ret;
	uint8_t buf[I2C_BATCH_BUF_SIZE];
	int data_len = info->data_len;
	int max_regs_per_batch;

	/* Batching only supports 1/2-byte addr/data; reject the rest */
	if ((info->addr_len != 1 && info->addr_len != 2) ||
		(data_len != 1 && data_len != 2)) {
		return -RTS_ISP_EINVAL;
	}
	max_regs_per_batch = (I2C_BATCH_BUF_SIZE - info->addr_len) / data_len;

	i = 0;
	while (i < regs->num) {
		/* Build batch: start with register address */
		if (info->addr_len == 2) {
			buf[0] = regs->regs[i].addr >> 8;
			buf[1] = regs->regs[i].addr & 0xff;
		} else {
			buf[0] = regs->regs[i].addr & 0xff;
		}

		/* Batch consecutive-address register data */
		buf_pos = info->addr_len;
		reg_count = 0;
		j = i;
		while (j < regs->num && reg_count < max_regs_per_batch &&
			   regs->regs[j].addr == regs->regs[i].addr + reg_count) {
			if (data_len == 2) {
				buf[buf_pos++] = (regs->regs[j].data >> 8) & 0xff;
				buf[buf_pos++] = regs->regs[j].data & 0xff;
			} else {
				buf[buf_pos++] = regs->regs[j].data & 0xff;
			}
			reg_count++;
			j++;
		}

		/* Send the batch as one I2C transaction */
		ret = isp_os_driver_i2c_write(info->i2c_id, buf, buf_pos);
		if (ret) {
			return ret;
		}

		i = j;
	}

	return RTS_ISP_OK;
}

static int process_i2c_regs(const struct rts_isp_i2c_info *info,
							const struct rts_isp_i2c_regs *regs, int is_read)
{
	int i;
	int ret = -RTS_ISP_EINVAL;
	int batched = RTS_ISP_FALSE;

	if (!info || !regs || !regs->regs || !regs->num) {
		return ret;
	}

	if (!is_read && CONFIG_I2C_BATCH_WRITE) {
		ret = process_i2c_regs_write_batch(info, regs);
		/* Fall back to per-register writes if the sensor's addr/data
		 * length isn't batchable, instead of dropping the whole table. */
		batched = (ret != -RTS_ISP_EINVAL);
	}

	if (!batched) {
		if (!is_read) {
			for (i = 0; i < regs->num; i++) {
				ret = rts_isp_write_i2c_reg(info, &regs->regs[i]);
				if (ret) {
					break;
				}
			}
		} else {
			for (i = 0; i < regs->num; i++) {
				ret = rts_isp_read_i2c_reg(info, &regs->regs[i]);
				if (ret) {
					break;
				}
			}
		}
	}

	if (!ret && regs->udelay) {
		isp_os_usleep(regs->udelay);
	}
	return ret;
}

int rts_isp_read_i2c_regs(const struct rts_isp_i2c_info *info,
						  const struct rts_isp_i2c_regs *regs)
{
	return process_i2c_regs(info, regs, RTS_ISP_TRUE);
}

int rts_isp_write_i2c_regs(const struct rts_isp_i2c_info *info,
						   const struct rts_isp_i2c_regs *regs)
{
	return process_i2c_regs(info, regs, RTS_ISP_FALSE);
}

int rts_isp_read_i2c_reg(const struct rts_isp_i2c_info *info,
						 struct rts_isp_i2c_reg *reg)
{
	int ret;
	struct isp_i2c_info_reg i2c;

	i2c.info = *info;
	i2c.reg = *reg;
	ret = isp_os_driver_i2c_read_reg(&i2c);
	if (ret) {
		return ret;
	}
	reg->data = i2c.reg.data;
	return RTS_ISP_OK;
}

int rts_isp_write_i2c_reg(const struct rts_isp_i2c_info *info,
						  struct rts_isp_i2c_reg *reg)
{
	struct isp_i2c_info_reg i2c;

	i2c.info = *info;
	i2c.reg = *reg;
	return isp_os_driver_i2c_write_reg(&i2c);
}

static int rts_isp_server_i2c_read(uint32_t isp_id, uint8_t i2c_id, void *buf,
								   int len)
{
	return isp_os_driver_i2c_read(i2c_id, buf, len);
}

static int rts_isp_server_i2c_write(uint32_t isp_id, uint8_t i2c_id, void *buf,
									int len)
{
	return isp_os_driver_i2c_write(i2c_id, buf, len);
}

static int sensor_get_i2c_info(uint32_t isp_id, struct rts_isp_i2c_info *info)
{
	int ret;
	struct rts_isp_sensor_info snr_info;
	const struct rts_isp_sensor_ops *ops;

	ret = isp_sensor_get_ops(isp_id, &ops);
	if (ret) {
		return ret;
	}
	ret = ops->get_info(isp_id, &snr_info);
	if (ret) {
		return ret;
	}
	*info = snr_info.i2c;
	return RTS_ISP_OK;
}

static int rts_isp_server_read_sensor_reg(uint32_t isp_id,
		struct rts_isp_i2c_reg *reg)
{
	int ret;
	struct rts_isp_i2c_info info;

	ret = sensor_get_i2c_info(isp_id, &info);
	if (ret) {
		return ret;
	}
	return rts_isp_read_i2c_reg(&info, reg);
}

static int rts_isp_server_write_sensor_reg(uint32_t isp_id,
		struct rts_isp_i2c_reg *reg)
{
	int ret;
	struct rts_isp_i2c_info info;

	ret = sensor_get_i2c_info(isp_id, &info);
	if (ret) {
		return ret;
	}
	return rts_isp_write_i2c_reg(&info, reg);
}

int rts_isp_i2c_read(uint32_t isp_id, uint8_t i2c_id, void *buf, int len)
{
	return rts_isp_server_i2c_read(isp_id, i2c_id, buf, len);
}

int rts_isp_i2c_write(uint32_t isp_id, uint8_t i2c_id, void *buf, int len)
{
	return rts_isp_server_i2c_write(isp_id, i2c_id, buf, len);
}

int rts_isp_read_sensor_reg(uint32_t isp_id, struct rts_isp_i2c_reg *reg)
{
	return rts_isp_server_read_sensor_reg(isp_id, reg);
}

int rts_isp_write_sensor_reg(uint32_t isp_id, struct rts_isp_i2c_reg *reg)
{
	return rts_isp_server_write_sensor_reg(isp_id, reg);
}

void rts_isp_sensor_access_prepare(void)
{
	isp_os_driver_sync_lock(RTS_ISP_TRUE);
}

void rts_isp_sensor_access_unprepare(void)
{
	isp_os_driver_sync_lock(RTS_ISP_FALSE);
}

int rts_isp_report_sensor_exp_gain(uint32_t isp_id,
								   struct rts_isp_exp_gain_report *report)
{
	if (!report) {
		return -RTS_ISP_EINVAL;
	}

	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_AE),
						   AE_EXEC_REPORT_EXP_GAIN, report,
						   sizeof(*report));
}

void rts_isp_set_isp_gain_delay(uint32_t isp_id, uint32_t delay)
{
	if (isp_id >= _ISP_ID_NUM || delay > 3) {
		return;
	}
	isp_mod_execute(isp_top_get_mod(isp_id, MOD_SENSOR),
					SENSOR_EXEC_SET_ISP_GAIN_DELAY, &delay, sizeof(delay));
}

void rts_isp_set_short_exp_delay(uint32_t isp_id, uint32_t delay0,
								 uint32_t delay1, uint32_t delay2)
{
	struct isp_interface_novc_delay novc_delay;

	novc_delay.channel[0] = delay0;
	novc_delay.channel[1] = delay1;
	novc_delay.channel[2] = delay2;
	isp_mod_execute(isp_top_get_mod(isp_id, MOD_INTERFACE),
					INTERFACE_EXEC_SHORT_EXP_DELAY, &novc_delay,
					sizeof(novc_delay));
}

int rts_isp_set_exp_gain_ratio_range(uint32_t isp_id,
									 struct rts_isp_ratio_range *range)
{
	return isp_mod_execute(isp_top_get_mod(isp_id, MOD_AE),
						   AE_EXEC_SET_RATIO_RANGE, range, sizeof(*range));
}

void rts_isp_set_bayer_pattern(uint32_t isp_id, int bayer)
{
	isp_mod_execute(isp_top_get_mod(isp_id, MOD_GLOBAL),
					GLOBAL_EXEC_SET_BAYER_PATTERN, &bayer, sizeof(bayer));
}

void rts_isp_drop_frames(uint32_t isp_id, int frames)
{
	isp_mod_execute(isp_top_get_mod(isp_id, MOD_GLOBAL),
					GLOBAL_EXEC_DROP_FRAMES, &frames, sizeof(frames));
}

static uint32_t remap_mode = 0; //0:frame mode, 1:slice mode
void rts_isp_set_remap_mode(uint32_t isp_id, uint32_t mode)
{
	remap_mode = mode;
}

uint32_t rts_isp_get_remap_mode(uint32_t isp_id)
{
	return remap_mode;
}

