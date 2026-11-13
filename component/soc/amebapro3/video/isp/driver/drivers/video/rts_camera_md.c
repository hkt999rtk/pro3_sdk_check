/*
 * Copyright 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS:
 * - LOG_MODULE_REGISTER  -> removed
 * - sys_read32/write32   -> readl/writel
 * - k_mutex              -> isp_rtos_mutex_t
 * - k_poll_signal        -> replaced with rts_md_callback_t callback
 * - const struct device* -> removed (global singleton md_dev_data)
 * - DT_INST_REG_ADDR(0)  -> CONFIG_RTS_MD_BASE
 * - DT_INST_IRQN(0)      -> CONFIG_RTS_MD_IRQ
 * - IRQ_CONNECT(...)     -> interrupt handler registered via weak symbol
 * - atomic_get/inc/dec   -> isp_rtos_atomic_read/inc/dec
 */

#include <errno.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_atomic.h>
#include <isp_rtos_iomem.h>
#include "rts_camera.h"
#include "rts_camera_md.h"
#include "rts_camera_hw_config.h"
#include <irq.h>

#ifndef BIT
#define BIT(nr) (1UL << (nr))
#endif

/* MD register offsets */
#define MTD_IN_WIDTH         0x00
#define MTD_IN_HEIGHT        0x04
#define MTD_IN_SCALE         0x08
#define MTD_IN_START_X       0x0c
#define MTD_IN_START_Y       0x10
#define MTD_OUT_WIDTH        0x14
#define MTD_OUT_HEIGHT       0x18
#define MTD_OUT_SCALE        0x1c
#define MTD_OUT_START_X      0x20
#define MTD_OUT_START_Y      0x24
#define MTD_OUT_DS           0x28
#define MTD_ROI_START_X      0x2c
#define MTD_ROI_START_Y      0x30
#define MTD_ROI_X_END        0x34
#define MTD_ROI_Y_END        0x38
#define MTD_BIN_CTRL         0x3c
#define MTD_THD_CTRL         0x40
#define MTD_THD              0x44
#define MTD_TRAIN_NUM        0x48
#define MTD0_FRAME_ADDR      0x4c
#define MTD0_BUF             0x50
#define MTD0_DDR             0x54
#define MTD_INT_EN           0x5c
#define MTD_INT_CLR          0x60
#define MTD_BURST_LEN_SEL_WR0 0x64
#define MTD1_FRAME_ADDR      0x68
#define MTD1_BUF             0x6c
#define MTD1_DDR             0x70
#define MTD_BURST_LEN_SEL_WR1 0x78
#define MTD_BURST_LEN_SEL_RD  0x7c
#define MTD_RESULT           0x80
#define MTD_TRAIN_FLAG       0x84

/* interrupt masks */
#define MD_EN_INT            0x3fe  /* all except "frame end histo" (bit0) */
#define MD_XXFL_INT          0x3fc  /* all overflow/underflow */
#define MD_MAJOR_XXFL_INT    (BIT(9) | BIT(7) | BIT(6))
#define MD_DONE_INT          BIT(1)

struct rts_md_dev_data {
	uint32_t base;
	atomic_t open_count;
	isp_rtos_mutex_t lock;
	rts_md_callback_t callback;  /* replaces k_poll_signal */
};

static struct rts_md_dev_data md_dev_data = {
	.base = CONFIG_RTS_MD_BASE,
};

static inline uint32_t md_read_reg(uint32_t base, uint32_t offset)
{
	return readl(base + offset);
}

static inline void md_write_reg(uint32_t base, uint32_t offset, uint32_t value)
{
	writel(value, base + offset);
}

/* ------------------------------------------------------------------ */
/* Hardware helpers                                                    */
/* ------------------------------------------------------------------ */

static int md_check_attr(struct rts_md_attr *attr)
{
	if (attr->md_in.w > 1022 || attr->md_in.h > 510 ||
		attr->md_in.x > 1022 || attr->md_in.y > 510 ||
		attr->md_in.scale_w < 1 || attr->md_in.scale_w > 15 ||
		attr->md_in.scale_h < 1 || attr->md_in.scale_h > 15 ||
		attr->md_out.w > 1022 || attr->md_out.h > 510 ||
		attr->md_out.x > 1022 || attr->md_out.y > 510 ||
		attr->md_out.scale_w < 1 || attr->md_out.scale_w > 7 ||
		attr->md_out.scale_h < 1 || attr->md_out.scale_h > 7 ||
		attr->roi.x > attr->md_in.w || attr->roi.y > attr->md_in.h ||
		attr->roi.x_end > attr->md_in.w ||
		attr->roi.y_end > attr->md_in.h ||
		attr->roi.x_end < attr->roi.x ||
		attr->roi.y_end < attr->roi.y ||
		attr->nr_bins > 1 || attr->bin_bits > 1 ||
		attr->back_thd > 15 || attr->ds_thd > 15 ||
		attr->motion_thd > 131071 || attr->train_frames > 31) {
		return -EINVAL;
	}

	return 0;
}

static int md_set_attr(struct rts_md_attr *attr)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;
	int ret;

	if (!attr) {
		return -EINVAL;
	}

	ret = md_check_attr(attr);
	if (ret) {
		return ret;
	}

	md_write_reg(dev_data->base, MTD_IN_WIDTH, attr->md_in.w);
	md_write_reg(dev_data->base, MTD_IN_HEIGHT, attr->md_in.h);
	md_write_reg(dev_data->base, MTD_IN_START_X, attr->md_in.x);
	md_write_reg(dev_data->base, MTD_IN_START_Y, attr->md_in.y);
	val = (attr->md_in.scale_h << 8) | attr->md_in.scale_w;
	md_write_reg(dev_data->base, MTD_IN_SCALE, val);

	md_write_reg(dev_data->base, MTD_OUT_WIDTH, attr->md_out.w);
	md_write_reg(dev_data->base, MTD_OUT_HEIGHT, attr->md_out.h);
	md_write_reg(dev_data->base, MTD_OUT_START_X, attr->md_out.x);
	md_write_reg(dev_data->base, MTD_OUT_START_Y, attr->md_out.y);
	val = (attr->md_out.scale_h << 8) | attr->md_out.scale_w;
	md_write_reg(dev_data->base, MTD_OUT_SCALE, val);

	md_write_reg(dev_data->base, MTD_ROI_START_X, attr->roi.x);
	md_write_reg(dev_data->base, MTD_ROI_START_Y, attr->roi.y);
	md_write_reg(dev_data->base, MTD_ROI_X_END, attr->roi.x_end);
	md_write_reg(dev_data->base, MTD_ROI_Y_END, attr->roi.y_end);

	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	val &= ~0x3;
	val |= (attr->bin_bits << 1) | attr->nr_bins;
	md_write_reg(dev_data->base, MTD_BIN_CTRL, val);
	val = (attr->forget << 16) | (attr->learn << 8) | (attr->back_thd);
	md_write_reg(dev_data->base, MTD_THD_CTRL, val);
	md_write_reg(dev_data->base, MTD_OUT_DS, attr->ds_thd);
	md_write_reg(dev_data->base, MTD_THD, attr->motion_thd);
	md_write_reg(dev_data->base, MTD_TRAIN_NUM, attr->train_frames);

	md_write_reg(dev_data->base, MTD_BURST_LEN_SEL_WR0, attr->bl_hist_wr);
	md_write_reg(dev_data->base, MTD_BURST_LEN_SEL_WR1, attr->bl_res_wr);
	md_write_reg(dev_data->base, MTD_BURST_LEN_SEL_RD, attr->bl_isp_rd);

	return 0;
}

static int md_get_attr(struct rts_md_attr *attr)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	if (!attr) {
		return -EINVAL;
	}

	attr->md_in.w = md_read_reg(dev_data->base, MTD_IN_WIDTH);
	attr->md_in.h = md_read_reg(dev_data->base, MTD_IN_HEIGHT);
	attr->md_in.x = md_read_reg(dev_data->base, MTD_IN_START_X);
	attr->md_in.y = md_read_reg(dev_data->base, MTD_IN_START_Y);
	val = md_read_reg(dev_data->base, MTD_IN_SCALE);
	attr->md_in.scale_w = val & 0xf;
	attr->md_in.scale_h = val >> 8;

	attr->md_out.w = md_read_reg(dev_data->base, MTD_OUT_WIDTH);
	attr->md_out.h = md_read_reg(dev_data->base, MTD_OUT_HEIGHT);
	attr->md_out.x = md_read_reg(dev_data->base, MTD_OUT_START_X);
	attr->md_out.y = md_read_reg(dev_data->base, MTD_OUT_START_Y);
	val = md_read_reg(dev_data->base, MTD_OUT_SCALE);
	attr->md_out.scale_w = val & 0xf;
	attr->md_out.scale_h = val >> 8;

	attr->roi.x = md_read_reg(dev_data->base, MTD_ROI_START_X);
	attr->roi.y = md_read_reg(dev_data->base, MTD_ROI_START_Y);
	attr->roi.x_end = md_read_reg(dev_data->base, MTD_ROI_X_END);
	attr->roi.y_end = md_read_reg(dev_data->base, MTD_ROI_Y_END);

	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	attr->nr_bins = val & 0x1;
	attr->bin_bits = (val >> 1) & 0x1;
	val = md_read_reg(dev_data->base, MTD_THD_CTRL);
	attr->back_thd = val & 0xf;
	attr->learn = (val >> 8) & 0xff;
	attr->forget = (val >> 16) & 0xff;
	attr->ds_thd = md_read_reg(dev_data->base, MTD_OUT_DS);
	attr->motion_thd = md_read_reg(dev_data->base, MTD_THD);
	attr->train_frames = md_read_reg(dev_data->base, MTD_TRAIN_NUM);

	attr->bl_hist_wr = md_read_reg(dev_data->base, MTD_BURST_LEN_SEL_WR0);
	attr->bl_res_wr = md_read_reg(dev_data->base, MTD_BURST_LEN_SEL_WR1);
	attr->bl_isp_rd = md_read_reg(dev_data->base, MTD_BURST_LEN_SEL_RD);

	return 0;
}

static int md_check_buffer_alignment(struct rts_md_buf *buf)
{
	if (buf->hist_addr & 0xf || buf->hist_size & 0xf ||
		buf->res_addr & 0xf || buf->res_size & 0xf) {
		return -EINVAL;
	}

	return 0;
}

static int md_set_frame_buffer(struct rts_md_buf *buf)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	int ret;

	if (!buf ||
		buf->hist_addr == 0 || (buf->hist_size & 0xfffff0) == 0 ||
		buf->res_addr == 0 || (buf->res_size & 0xfffff0) == 0) {
		return -EINVAL;
	}

	ret = md_check_buffer_alignment(buf);
	if (ret) {
		return ret;
	}

	md_write_reg(dev_data->base, MTD0_FRAME_ADDR, buf->hist_addr);
	md_write_reg(dev_data->base, MTD0_DDR, buf->hist_size);
	md_write_reg(dev_data->base, MTD1_FRAME_ADDR, buf->res_addr);
	md_write_reg(dev_data->base, MTD1_DDR, buf->res_size);

	return 0;
}

static int md_set_axi_buffer(struct rts_md_buf *buf)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;
	int ret;

	if (!buf ||
		(buf->hist_size & 0xfff0) == 0 || (buf->res_size & 0xfff0) == 0) {
		return -EINVAL;
	}

	ret = md_check_buffer_alignment(buf);
	if (ret) {
		return ret;
	}

	val = buf->hist_size << 16 | buf->hist_addr;
	md_write_reg(dev_data->base, MTD0_BUF, val);
	val = buf->res_size << 16 | buf->res_addr;
	md_write_reg(dev_data->base, MTD1_BUF, val);

	return 0;
}

static int md_set_skip_frames(uint8_t *num)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	if (!num || *num > 127) {
		return -EINVAL;
	}

	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	if (*num > 0) {
		val &= 0xff;
		val |= (*num << 8) | BIT(7);
	} else {
		val &= ~BIT(7);
	}
	md_write_reg(dev_data->base, MTD_BIN_CTRL, val);

	return 0;
}

static void md_enable(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	val |= BIT(4);
	md_write_reg(dev_data->base, MTD_BIN_CTRL, val);
}

static void md_train(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;

	md_write_reg(dev_data->base, MTD_TRAIN_FLAG, 1);
}

static int md_get_result(struct rts_md_result *res)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	if (!res) {
		return -EINVAL;
	}

	val = md_read_reg(dev_data->base, MTD_RESULT);
	res->motion_flag = val & BIT(0);
	res->count = val >> 1;

	return 0;
}

static void md_pause(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	val |= BIT(3);
	md_write_reg(dev_data->base, MTD_BIN_CTRL, val);
}

static void md_resume_hw(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	val &= ~BIT(3);
	md_write_reg(dev_data->base, MTD_BIN_CTRL, val);
}

void rts_md_isr(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t int_flag;
	uint32_t poll_flag = 0;

	int_flag = md_read_reg(dev_data->base, MTD_INT_CLR);
	md_write_reg(dev_data->base, MTD_INT_CLR, int_flag);
	if ((int_flag & MD_EN_INT) == 0) {
		return;
	}

	if (int_flag & MD_XXFL_INT) {
		if (int_flag & MD_MAJOR_XXFL_INT) {
			poll_flag |= MD_POLL_FLAG_NEED_RETRAIN;
		}
	} else {
		if (int_flag & MD_DONE_INT) {
			md_pause();
			poll_flag |= MD_POLL_FLAG_OK;
		} else {
			return; /* ignore frame-end-histo interrupt */
		}
	}

	if (dev_data->callback) {
		dev_data->callback(poll_flag);
	}
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int rts_md_init(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;

	dev_data->base = CONFIG_RTS_MD_BASE;
	isp_rtos_atomic_set(&dev_data->open_count, 0);
	isp_rtos_mutex_init(&dev_data->lock);
	dev_data->callback = NULL;

	return 0;
}

int rts_md_open(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;

	isp_rtos_mutex_lock(&dev_data->lock);

	if (isp_rtos_atomic_read(&dev_data->open_count) > 0) {
		isp_rtos_mutex_unlock(&dev_data->lock);
		return -EBUSY;
	}

	md_write_reg(dev_data->base, MTD_INT_EN, MD_EN_INT);
	isp_irq_register(ISP_IRQ_MD, rts_md_isr, INT_PRI_MIDDLE);
	isp_irq_enable(ISP_IRQ_MD);
	isp_rtos_atomic_inc(&dev_data->open_count);

	isp_rtos_mutex_unlock(&dev_data->lock);
	return 0;
}

void rts_md_close(void)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	uint32_t val;

	isp_rtos_mutex_lock(&dev_data->lock);

	if (isp_rtos_atomic_read(&dev_data->open_count) == 0) {
		isp_rtos_mutex_unlock(&dev_data->lock);
		return;
	}

	isp_irq_disable(ISP_IRQ_MD);
	isp_irq_unregister(ISP_IRQ_MD);
	md_write_reg(dev_data->base, MTD_INT_EN, 0x0);
	val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
	val &= ~BIT(4); /* disable mtd_en */
	md_write_reg(dev_data->base, MTD_BIN_CTRL, val);
	md_write_reg(dev_data->base, MTD_TRAIN_NUM, 0);
	isp_rtos_atomic_dec(&dev_data->open_count);

	isp_rtos_mutex_unlock(&dev_data->lock);
}

int rts_md_ioctl(unsigned int cmd, void *arg)
{
	struct rts_md_dev_data *dev_data = &md_dev_data;
	int ret;

	isp_rtos_mutex_lock(&dev_data->lock);
	switch (cmd) {
	case RTSMD_IOC_SET_ATTR:
		ret = md_set_attr(arg);
		break;
	case RTSMD_IOC_GET_ATTR:
		ret = md_get_attr(arg);
		break;
	case RTSMD_IOC_SET_SKIP:
		ret = md_set_skip_frames(arg);
		break;
	case RTSMD_IOC_SET_FRAME_BUF:
		ret = md_set_frame_buffer(arg);
		break;
	case RTSMD_IOC_SET_AXI_BUF:
		ret = md_set_axi_buffer(arg);
		break;
	case RTSMD_IOC_ENABLE:
		md_enable();
		ret = 0;
		break;
	case RTSMD_IOC_TRAIN:
		md_train();
		ret = 0;
		break;
	case RTSMD_IOC_GET_RESULT:
		ret = md_get_result(arg);
		break;
	case RTSMD_IOC_RESUME:
		md_resume_hw();
		ret = 0;
		break;
	case RTSMD_IOC_DISABLE: {
		uint32_t val = md_read_reg(dev_data->base, MTD_BIN_CTRL);
		val &= ~BIT(4);
		md_write_reg(dev_data->base, MTD_BIN_CTRL, val);
		ret = 0;
		break;
	}
	default:
		rtsprintk(RTS_TRACE_ERROR, "unknown md ioctl: 0x%08x", cmd);
		ret = -ENOTTY;
	}
	isp_rtos_mutex_unlock(&dev_data->lock);

	return ret;
}

void rts_md_set_callback(rts_md_callback_t cb)
{
	md_dev_data.callback = cb;
}
