#ifndef _RTSCAM_ISP_I2C_H_INC_
#define _RTSCAM_ISP_I2C_H_INC_

#include <i2c.h>
#include <rts_camera_isp.h>

typedef void *i2c_adapter_t;

struct rtscam_isp_i2c {
	i2c_adapter_t i2c_adapter;
};

int rtscam_isp_i2c_init(struct rtscam_isp_i2c *i2c);
void rtscam_isp_i2c_cleanup(struct rtscam_isp_i2c *i2c);

int rtscam_isp_i2c_read_reg(struct rtscam_isp_i2c *i2c,
							const struct rtscam_isp_i2c_info *info,
							struct rtscam_isp_i2c_reg *reg);
int rtscam_isp_i2c_write_reg(struct rtscam_isp_i2c *i2c,
							 const struct rtscam_isp_i2c_info *info,
							 const struct rtscam_isp_i2c_reg *reg);
int rtscam_isp_i2c_transfer(struct rtscam_isp_i2c *i2c, uint8_t slave_id,
							uint8_t *buf, uint16_t len, uint8_t is_read);
int rtscam_isp_i2c_write_sync(struct rtscam_isp_i2c *i2c,
							  struct rtscam_isp_i2c_sync_regs *sync);

#endif /* _RTSCAM_ISP_I2C_H_INC_ */
