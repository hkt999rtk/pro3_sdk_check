/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2022 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <rts_isp_errno.h>
#include <rts_isp_sensor.h>
#include <rts_camera_isp.h>
#include <isp_rtos_dev.h>
#include <isp_utils.h>
#include <memory_prv.h>
#include "isp_os_io.h"
#include "isp_os_log.h"
#include "isp_os_mem.h"
#include "isp_os_driver.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_DRIVER

#define xioctl(fd, request, ...)                                  \
	({                                                        \
		int ret;                                          \
		ret = isp_rtos_ioctl(fd, request, ##__VA_ARGS__); \
		if (ret < 0)                                      \
			ret = rts_isp_errno_from_os(-ret);        \
		ret;                                              \
	})

struct isp_driver {
	int pfd;
	int initialized;
};

void *isp_io_base;
static uint32_t isp_io_size;
static struct isp_driver g_driver;

static int driver_set_io_base(int fd)
{
	int ret;
	struct rtscam_isp_iomem_info info;
	void *io_base;

	if (fd < 0) {
		return -RTS_ISP_EINVAL;
	}

	ret = xioctl(fd, RTSISP_IOC_PIPE_GET_IOMEM_INFO, &info);
	if (ret) {
		goto err;
	}
	debug("io mem is 0x%x@0x%x\n", info.size, info.start);
	io_base = isp_os_driver_mmap(info.start, info.size);
	if (!io_base) {
		ret = -RTS_ISP_ENOMEM;
		goto err;
	}
	debug("map io to %p\n", io_base);

	isp_io_base = io_base;
	isp_io_size = info.size;

	return RTS_ISP_OK;

err:
	isp_perror(ret, "map io fail");
	return ret;
}

static void driver_unset_io_base(int fd)
{
	if (fd < 0) {
		return;
	}

	isp_os_driver_munmap(&isp_io_base, isp_io_size);
}

static int driver_get_pfd(void)
{
	if (g_driver.initialized) {
		return g_driver.pfd;
	} else {
		return -1;
	}
}

int isp_os_driver_init(void)
{
	int pfd;
	int ret;

	if (g_driver.initialized) {
		g_driver.initialized++;
		return RTS_ISP_OK;
	}
	pfd = isp_os_open("/dev/rtsisp");
	if (pfd < 0) {
		return -RTS_ISP_EINVAL;
	}
	memset(&g_driver, 0, sizeof(g_driver));
	g_driver.initialized = 1;
	g_driver.pfd = pfd;
	ret = driver_set_io_base(pfd);
	if (ret) {
		driver_unset_io_base(pfd);
		isp_close(pfd);
		memset(&g_driver, 0, sizeof(g_driver));
	}
	return ret;
}

void isp_os_driver_cleanup(void)
{
	if (!g_driver.initialized) {
		return;
	}
	if (g_driver.initialized > 1) {
		g_driver.initialized--;
		return;
	}
	driver_unset_io_base(g_driver.pfd);
	isp_close(g_driver.pfd);
	memset(&g_driver, 0, sizeof(g_driver));
}

isp_os_fd_t isp_os_driver_get_pfd(void)
{
	return driver_get_pfd();
}

void *isp_os_driver_mmap(uint32_t start, uint32_t size)
{
	return (void *)start;
}

void isp_os_driver_munmap(void **ptr, uint32_t size)
{
	if (!ptr || !*ptr || !size) {
		return;
	}

	*ptr = NULL;
}

int isp_os_driver_i2c_read_reg(struct isp_i2c_info_reg *i2c)
{
	int fd = driver_get_pfd();

	if (sizeof(*i2c) != sizeof(struct rtscam_isp_i2c_info_reg)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!i2c) {
		return -RTS_ISP_EINVAL;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_I2C_READ_REG, i2c);
}

int isp_os_driver_i2c_write_reg(const struct isp_i2c_info_reg *i2c)
{
	int fd = driver_get_pfd();

	if (sizeof(*i2c) != sizeof(struct rtscam_isp_i2c_info_reg)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!i2c) {
		return -RTS_ISP_EINVAL;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_I2C_WRITE_REG, (void *)i2c);
}

#define I2C_M_RD 0x0001 /* read data, from slave to master */
int isp_os_driver_i2c_read(uint8_t i2c_id, void *buf, uint16_t len)
{
	int ret;
	struct rtscam_isp_i2c_msg msg;
	int fd = driver_get_pfd();

	if (len > ARRAY_SIZE(msg.buf)) {
		return -RTS_ISP_EINVAL;
	}
	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	msg.addr = i2c_id;
	msg.flags = I2C_M_RD;
	msg.len = len;
	ret = xioctl(fd, RTSISP_IOC_PIPE_I2C_TRANSTER, &msg);
	if (ret) {
		isp_perror(ret, "read i2c buf fail");
	} else {
		memcpy(msg.buf, buf, len);
		debug("%s ok\n", __func__);
	}

	return ret;
}

int isp_os_driver_i2c_write(uint8_t i2c_id, void *buf, uint16_t len)
{
	int ret;
	struct rtscam_isp_i2c_msg msg;
	int fd = driver_get_pfd();

	if (len > ARRAY_SIZE(msg.buf)) {
		return -RTS_ISP_EINVAL;
	}
	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	msg.addr = i2c_id;
	msg.flags = 0;
	memcpy(msg.buf, buf, len);
	msg.len = len;
	ret = xioctl(fd, RTSISP_IOC_PIPE_I2C_TRANSTER, &msg);
	if (ret) {
		isp_perror(ret, "write i2c buf fail");
	} else {
		debug("%s ok\n", __func__);
	}

	return ret;
}

void isp_os_driver_wait_event(int32_t event)
{
	int fd = driver_get_pfd();
	int32_t e; /* enum rtscam_isp_event */

	if (fd < 0) {
		return;
	}

	switch (event) {
	case ISP_DATA_START:
		e = RTSCAM_ISP_DATA_START;
		break;
	case ISP_FRAME_END:
		e = RTSCAM_ISP_FRAME_END;
		break;
	default:
		return;
	}

	(void)xioctl(fd, RTSISP_IOC_PIPE_WAIT_EVENT, &e);
}

int isp_os_driver_set_info(const struct isp_frame_info *info)
{
	int ret;
	isp_os_fd_t fd = driver_get_pfd();

	if (sizeof(*info) != sizeof(struct rtscam_isp_frame_info)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!info) {
		return -RTS_ISP_EINVAL;
	}
	ret = xioctl(fd, RTSISP_IOC_PIPE_SET_FRAME_INFO, (void *)info);
	if (ret) {
		isp_perror(ret, "set isp info fail");
	} else {
		debug("%s ok\n", __func__);
	}

	return ret;
}

int isp_os_driver_set_power(const struct rts_isp_snr_pwr *pwr)
{
	isp_os_fd_t fd = driver_get_pfd();
	int ret;

	if (sizeof(*pwr) != sizeof(struct rtscam_isp_snr_pwr)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!pwr) {
		return -RTS_ISP_EINVAL;
	}
	ret = xioctl(fd, RTSISP_IOC_PIPE_SET_POWER, (void *)pwr);
	return ret;
}

int isp_os_driver_write_sync_regs(const struct rts_isp_sync_regs *regs)
{
	isp_os_fd_t fd = driver_get_pfd();
	int ret;

	if (sizeof(*regs) != sizeof(struct rtscam_isp_sync_regs)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!regs) {
		return -RTS_ISP_EINVAL;
	}
	ret = xioctl(fd, RTSISP_IOC_PIPE_SYNC_WRITE, (void *)regs);
	return ret;
}

int isp_os_driver_write_sync_reg(uint32_t value, uint32_t offset, uint32_t mask,
								 uint32_t delay, int32_t interrupt)
{
	struct rts_isp_sync_reg reg[2];
	struct rts_isp_sync_regs regs;

	regs.reg = reg;
	set_sync_info(&reg[0], 0, RTSCAM_ISP_INT_FRAME_END);
	set_sync_isp_mask(&reg[1], offset, value, mask);
	regs.num = ARRAY_SIZE(reg);
	regs.split_index = 0;

	return isp_os_driver_write_sync_regs(&regs);
}

int isp_os_driver_clear_sync_regs(void)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_SYNC_CLEAR, NULL);
}

int isp_os_driver_init_statis(struct rts_isp_statis_info *info)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (sizeof(*info) != sizeof(struct rtscam_isp_statis_info)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_INIT_STATIS, info);
}

int isp_os_driver_cleanup_statis(void)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_CLEANUP_STATIS, NULL);
}

int isp_os_driver_get_statis(void *statis, uint32_t size)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (size != sizeof(struct rtscam_isp_statis_data)) {
		return -RTS_ISP_EFAULT;
	}

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_GET_STATIS, statis);
}

int isp_os_driver_put_statis(uint32_t id)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_PUT_STATIS, &id);
}

int isp_os_driver_get_count(void *count)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	return xioctl(fd, RTSISP_IOC_PIPE_GET_COUNT, count);
}

int isp_os_driver_mem_alloc(uint32_t *phy_addr, uint32_t length,
							const char *info)
{
	int ret;

	if (!phy_addr || !length || !info) {
		return -RTS_ISP_EINVAL;
	}

	ret = rtscam_rtos_alloc_mem(phy_addr, length, info);
	return ret;
}

int isp_os_driver_mem_free(uint32_t phy_addr)
{
	int ret;

	ret = rtscam_rtos_free_mem(&phy_addr);
	if (ret) {
		return ret;
	}

	return RTS_ISP_OK;
}

int isp_os_driver_sel_pin_state(int32_t interface)
{
	int ret;
	isp_os_fd_t fd = driver_get_pfd();
	int32_t state; /* enum rtscam_isp_pin_state */

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	switch (interface) {
	case SNR_INTERFACE_NONE:
		state = RTSCAM_ISP_PIN_STATE_NONE;
		break;
	case SNR_INTERFACE_DVP:
		state = RTSCAM_ISP_PIN_STATE_DVP;
		break;
	case SNR_INTERFACE_MIPI:
		state = RTSCAM_ISP_PIN_STATE_MIPI;
		break;
	default:
		state = RTSCAM_ISP_PIN_STATE_NONE;
	}
	ret = xioctl(fd, RTSISP_IOC_PIPE_SEL_PIN_STATE, &state);
	if (ret) {
		return ret;
	}

	return RTS_ISP_OK;
}

int isp_os_driver_notify_dynamic_fps(float fps)
{
	isp_os_fd_t fd = driver_get_pfd();
	struct v4l2_fract dyn_fps;

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	dyn_fps.numerator = SENSOR_FPS_PRECISION;
	dyn_fps.denominator = fps * SENSOR_FPS_PRECISION;
	return xioctl(fd, RTSISP_IOC_PIPE_NOTIFY_DYN_FPS, &dyn_fps);
}

int isp_os_driver_set_clk(int32_t type, uint32_t *rate)
{
	int ret;
	struct rtscam_isp_clock clock;
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	switch (type) {
	case CLK_INTERFACE:
		clock.type = RTSCAM_ISP_CLOCK_INTERFACE;
		break;
	case CLK_ISP:
		clock.type = RTSCAM_ISP_CLOCK_ISP;
		break;
	default:
		return -RTS_ISP_EINVAL;
	}
	clock.rate = *rate;
	ret = xioctl(fd, RTSISP_IOC_PIPE_SET_CLOCK, &clock);
	if (ret) {
		return ret;
	}
	*rate = clock.rate;
	return 0;
}

int isp_os_driver_is_fpga(void)
{
	isp_os_fd_t fd = driver_get_pfd();
	int is_fpga = 0;

	if (fd < 0) {
		return 0;
	}

	(void)xioctl(fd, RTSISP_IOC_PIPE_IS_FPGA, &is_fpga);
	return is_fpga;
}

int isp_os_driver_get_ic_cut(void)
{
	isp_os_fd_t fd = driver_get_pfd();
	uint8_t ic_cut = 0;

	if (fd < 0) {
		return 0;
	}

	(void)xioctl(fd, RTSISP_IOC_PIPE_GET_IC_CUT, &ic_cut);
	return ic_cut;
}

void isp_os_driver_sync_lock(int lock)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return;
	}

	(void)xioctl(fd, RTSISP_IOC_PIPE_SYNC_LOCK, &lock);
}

int isp_os_driver_notify_color_range(int range)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_NOTIFY_COLOR_RANGE, &range);
}

int isp_os_driver_get_statis_cfg(uint32_t *base, uint32_t *size)
{
	int ret;
	struct rtscam_isp_region statis_cfg;
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	ret = xioctl(fd, RTSISP_IOC_PIPE_GET_STATIS_CFG, &statis_cfg);
	if (ret) {
		return ret;
	}
	*base = statis_cfg.base;
	*size = statis_cfg.size;

	return RTS_ISP_OK;
}

int isp_os_driver_set_fps(uint32_t fps)
{
	isp_os_fd_t fd = driver_get_pfd();
	struct v4l2_fract _fps;

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	_fps.denominator = fps;
	_fps.numerator = 1;
	return xioctl(fd, RTSISP_IOC_PIPE_TEST_PREVIEW, &_fps);
}

uint32_t isp_os_driver_get_bus_addr(void)
{
	int ret;
	uint32_t addr;
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	ret = xioctl(fd, RTSISP_IOC_PIPE_GET_BUS_ADDR, &addr);
	if (ret) {
		return ret;
	}
	return addr;
}

int isp_os_driver_pm_enable(bool enable)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}

	return xioctl(fd, RTSISP_IOC_PIPE_PM_ENABLE, &enable);
}

int isp_os_driver_set_large_image_mode(bool mode)
{
	isp_os_fd_t fd = driver_get_pfd();

	if (fd < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	return xioctl(fd, RTSISP_IOC_PIPE_SET_LARGE_IMAGE_MODE, &mode);
}
