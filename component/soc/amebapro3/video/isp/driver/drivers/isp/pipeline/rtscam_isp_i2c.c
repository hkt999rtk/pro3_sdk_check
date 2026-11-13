#include <errno.h>
#include <stdio.h>
#include "rtscam_isp_i2c.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static int __rtscam_isp_i2c_transfer(i2c_adapter_t i2c, uint8_t slave_id,
									 uint8_t *buf, uint16_t len,
									 uint8_t is_read)
{
	int ret;

	if (is_read) {
		ret = rtscam_rtos_i2c_read(i2c, slave_id, len, buf);
	} else {
		ret = rtscam_rtos_i2c_write(i2c, slave_id, len, buf);
	}
	/* EGAIN is caused by snr soft reset(arbitration lost), we bypass it */
	if (ret != 1 && ret != -EAGAIN) {
		return ret;
	}
	return 0;
}

static inline int __rtscam_isp_i2c_read(i2c_adapter_t i2c, uint8_t slave_id,
										uint8_t *buf, uint16_t len)
{
	return __rtscam_isp_i2c_transfer(i2c, slave_id, buf, len, 1);
}

static inline int __rtscam_isp_i2c_write(i2c_adapter_t i2c, uint8_t slave_id,
		uint8_t *buf, uint16_t len)
{
	return __rtscam_isp_i2c_transfer(i2c, slave_id, buf, len, 0);
}

int rtscam_isp_i2c_init(struct rtscam_isp_i2c *i2c)
{
	if (!i2c) {
		return -EINVAL;
	}

	i2c->i2c_adapter = rtscam_rtos_i2c_dev_get();
	if (!i2c->i2c_adapter) {
		return -ENODEV;
	}
	return 0;
}

void rtscam_isp_i2c_cleanup(struct rtscam_isp_i2c *i2c)
{
	if (!i2c) {
		return;
	}
	rtscam_rtos_i2c_dev_put(i2c);
	i2c = NULL;
}

int rtscam_isp_i2c_read_reg(struct rtscam_isp_i2c *i2c,
							const struct rtscam_isp_i2c_info *info,
							struct rtscam_isp_i2c_reg *reg)
{
	int ret;
	uint8_t buf[2];

	if (!i2c || !info || !reg) {
		return -EINVAL;
	}

	if (info->addr_len == 1) {
		buf[0] = reg->addr & 0xff;
	} else if (info->addr_len == 2) {
		buf[0] = reg->addr >> 8;
		buf[1] = reg->addr & 0xff;
	} else {
		return -EINVAL;
	}

	ret = __rtscam_isp_i2c_write(i2c->i2c_adapter, info->i2c_id, buf,
								 info->addr_len);
	if (ret) {
		return ret;
	}
	ret = __rtscam_isp_i2c_read(i2c->i2c_adapter, info->i2c_id, buf,
								info->data_len);
	if (ret) {
		return ret;
	}

	if (info->data_len == 1) {
		reg->data = buf[0];
	} else {
		reg->data = (buf[0] << 8) | buf[1];
	}

	return 0;
}

int rtscam_isp_i2c_write_reg(struct rtscam_isp_i2c *i2c,
							 const struct rtscam_isp_i2c_info *info,
							 const struct rtscam_isp_i2c_reg *reg)
{
	int j = 0;
	uint8_t buf[4];

	if (!i2c || !info || !reg) {
		return -EINVAL;
	}

	if (info->addr_len == 1) {
		buf[j++] = reg->addr & 0xff;
	} else {
		buf[j++] = reg->addr >> 8;
		buf[j++] = reg->addr & 0xff;
	}
	if (info->data_len == 1) {
		buf[j++] = reg->data & 0xff;
	} else {
		buf[j++] = reg->data >> 8;
		buf[j++] = reg->data & 0xff;
	}

	return __rtscam_isp_i2c_write(i2c->i2c_adapter, info->i2c_id, buf, j);
}

int rtscam_isp_i2c_transfer(struct rtscam_isp_i2c *i2c, uint8_t slave_id,
							uint8_t *buf, uint16_t len, uint8_t is_read)
{
	int ret;

	if (!i2c || !buf) {
		return -EINVAL;
	}

	if (is_read) {
		ret = rtscam_rtos_i2c_read(i2c, slave_id, len, buf);
	} else {
		ret = rtscam_rtos_i2c_write(i2c, slave_id, len, buf);
	}
	if (ret != 1) {
		return ret;
	} else {
		return 0;
	}
}

int rtscam_isp_i2c_write_sync(struct rtscam_isp_i2c *i2c,
							  struct rtscam_isp_i2c_sync_regs *sync)
{
	int i;
	struct rtscam_isp_i2c_reg reg;
	struct rtscam_isp_i2c_info info;
	struct rtscam_isp_i2c_reg_mask *reg_mask;

	if (!i2c || !sync || sync->num > ARRAY_SIZE(sync->regs)) {
		return -EINVAL;
	}

	for (i = 0; i < sync->num; i++) {
		reg_mask = &sync->regs[i];

		info.i2c_id = reg_mask->i2c_id;
		info.addr_len = reg_mask->addr_len;
		info.data_len = reg_mask->data_len;

		reg.addr = reg_mask->addr;
		if (reg_mask->mask) {
			rtscam_isp_i2c_read_reg(i2c, &info, &reg);
			reg.data &= ~reg_mask->mask;
			reg.data |= reg_mask->data & reg_mask->mask;
		} else {
			reg.data = reg_mask->data;
		}
		rtscam_isp_i2c_write_reg(i2c, &info, &reg);
	}

	return 0;
}
