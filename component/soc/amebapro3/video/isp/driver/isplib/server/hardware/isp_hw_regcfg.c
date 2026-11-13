/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2021 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_time.h>
#include <isp_os_driver.h>
#include <isp_reg.h>
#include <isp_reg_regcfg.h>

#define ISP_REGCFG_CMD_LEN (256 * 16)
#define ISP_REGCFG_DATA_LEN (4096 * 4)
#define ISP_REGCFG_BUF_LEN (ISP_REGCFG_CMD_LEN + ISP_REGCFG_DATA_LEN)

#define CMD_ATTR_VALID 0x01
#define CMD_ATTR_LAST 0x02

#define CMD_ATTR_BATCH (0x0 << 2)
#define CMD_ATTR_LINK (0x1 << 2)
#define CMD_ATTR_CFG (0x2 << 2)

struct isp_regcfg_buffer {
	void *begin;
	void *end;
	void *buf;
};

struct isp_regcfg {
	void *buf;
	uint32_t phy_addr;
	struct isp_regcfg_buffer cmd;
	struct isp_regcfg_buffer data;

	struct isp_regcfg_buffer batch;

	uint32_t initialized : 1;
	uint32_t configuring : 1;
	uint32_t busy : 1;
	uint32_t error_flag : 1;
};

struct isp_reg_cmd {
	uint32_t attr;
	uint32_t offset;
	union {
		uint32_t length;
		uint32_t value;
	};
	uint32_t addr;
};

static struct isp_regcfg g_regcfg;

static void regcfg_wait_idle(struct isp_regcfg *regcfg)
{
	int cnt = 40;

	if (!regcfg->busy) {
		return;
	}
	while (cnt-- && isp_read_reg(RDMA_INT_FLAG) == 0) {
		isp_os_usleep(100);
	}
	if (cnt < 0) {
		isp_error("regcfg wait timedout!\n");
		isp_write_reg(RDMA_TRIG_STOP_MASK, RDMA_CTRL);
	}
	regcfg->busy = RTS_ISP_FALSE;
}

int isp_regcfg_init(void)
{
	int ret;
	void *buf;
	uint32_t phy_addr;
	struct isp_regcfg *regcfg = &g_regcfg;

	if (regcfg->initialized) {
		return RTS_ISP_OK;
	}

	ret = isp_os_driver_mem_alloc(&phy_addr, ISP_REGCFG_BUF_LEN, "REGCFG");
	if (ret) {
		return ret;
	}
	buf = isp_os_driver_mmap(phy_addr, ISP_REGCFG_BUF_LEN);
	if (!buf) {
		isp_os_driver_mem_free(phy_addr);
		return -RTS_ISP_EACCES;
	}

	isp_memset(regcfg, 0, sizeof(*regcfg));
	regcfg->buf = buf;
	regcfg->phy_addr = phy_addr;
	regcfg->cmd.begin = regcfg->cmd.buf = regcfg->buf;
	regcfg->cmd.end = regcfg->cmd.begin + ISP_REGCFG_CMD_LEN;
	regcfg->data.begin = regcfg->data.buf = regcfg->cmd.end;
	regcfg->data.end = regcfg->data.begin + ISP_REGCFG_DATA_LEN;
	regcfg->initialized = RTS_ISP_TRUE;

	return RTS_ISP_OK;
}

void isp_regcfg_cleanup(void)
{
	struct isp_regcfg *regcfg = &g_regcfg;

	if (!regcfg->initialized) {
		return;
	}

	regcfg_wait_idle(regcfg);

	isp_os_driver_munmap(&regcfg->cmd.buf, ISP_REGCFG_BUF_LEN);
	isp_os_driver_mem_free(regcfg->phy_addr);
	isp_memset(regcfg, 0, sizeof(*regcfg));
}

void isp_regcfg_write(uint32_t value, uint32_t offset)
{
	struct isp_regcfg *regcfg = &g_regcfg;
	struct isp_reg_cmd *cmd;

	if (!regcfg->configuring ||
		regcfg->cmd.buf + sizeof(*cmd) > regcfg->cmd.end) {
		regcfg->error_flag = RTS_ISP_TRUE;
		return;
	}

	cmd = regcfg->cmd.buf;
	cmd->attr = CMD_ATTR_VALID | CMD_ATTR_CFG;
	cmd->offset = offset;
	cmd->value = value;

	regcfg->cmd.buf += sizeof(*cmd);
}

uint32_t *isp_regcfg_batch_req(uint32_t offset, uint32_t num)
{
	const uint32_t data_size = num * 4;
	struct isp_regcfg *regcfg = &g_regcfg;
	struct isp_reg_cmd *cmd;

	if (!regcfg->configuring ||
		regcfg->cmd.buf + sizeof(*cmd) > regcfg->cmd.end ||
		regcfg->data.buf + data_size > regcfg->data.end) {
		regcfg->error_flag = RTS_ISP_TRUE;
		return NULL;
	}

	cmd = regcfg->cmd.buf;
	cmd->attr = CMD_ATTR_VALID | CMD_ATTR_BATCH;
	cmd->offset = offset;
	cmd->length = data_size;
	cmd->addr = regcfg->phy_addr + (regcfg->data.buf - regcfg->buf);

	regcfg->batch.begin = regcfg->batch.buf = regcfg->data.buf;
	regcfg->batch.end = regcfg->batch.begin + data_size;

	regcfg->cmd.buf += sizeof(*cmd);
	regcfg->data.buf += data_size;

	return regcfg->batch.begin;
}

void isp_regcfg_batch_write(uint32_t value)
{
	struct isp_regcfg *regcfg = &g_regcfg;

	if (regcfg->batch.buf >= regcfg->batch.end) {
		regcfg->error_flag = RTS_ISP_TRUE;
		return;
	}
	*(uint32_t *)regcfg->batch.buf = value;
	regcfg->batch.buf += 4;
}

int isp_regcfg_start(void)
{
	struct isp_regcfg *regcfg = &g_regcfg;

	if (!regcfg->initialized) {
		return -RTS_ISP_ENOTREADY;
	}

	regcfg_wait_idle(regcfg);

	regcfg->cmd.buf = regcfg->cmd.begin;
	regcfg->data.buf = regcfg->data.begin;
	regcfg->configuring = RTS_ISP_TRUE;

	return RTS_ISP_OK;
}

int isp_regcfg_complete(void)
{
	int ret;
	struct isp_reg_cmd *cmd;
	struct isp_regcfg *regcfg = &g_regcfg;

	if (!regcfg->initialized || !regcfg->configuring) {
		return -RTS_ISP_ENOTREADY;
	}
	if (regcfg->busy) {
		ret = -RTS_ISP_EBUSY;
		goto err;
	}
	if (regcfg->error_flag) {
		regcfg->error_flag = RTS_ISP_FALSE;
		ret = -RTS_ISP_EFAULT;
		goto err;
	}

	regcfg->configuring = RTS_ISP_FALSE;
	if (regcfg->cmd.buf == regcfg->cmd.begin) {
		return RTS_ISP_OK;
	}

	regcfg->busy = RTS_ISP_TRUE;

	cmd = regcfg->cmd.buf - sizeof(*cmd);
	cmd->attr |= CMD_ATTR_LAST;

	isp_write_reg(regcfg->phy_addr, RDMA_DDR_ADDR);
	isp_write_reg(RDMA_TRIG_START_MASK, RDMA_CTRL);

	return RTS_ISP_OK;
err:
	regcfg->configuring = RTS_ISP_FALSE;
	isp_perror(ret, "regcfg error, all cfg canceled!");
	return ret;
}

int isp_regcfg_available(void)
{
	struct isp_regcfg *regcfg = &g_regcfg;

	return regcfg->configuring;
}
