#ifndef _ISP_OS_DRIVER_H_INC_
#define _ISP_OS_DRIVER_H_INC_

#include <stdbool.h>
#include <rts_isp_sensor.h>
#include <isp_os_io.h>

enum isp_clock_type {
	CLK_ISP,
	CLK_INTERFACE,
};

enum isp_event {
	ISP_DATA_START,
	ISP_FRAME_END,
};

struct isp_frame_info {
	uint16_t width;
	uint16_t height;
	struct rts_isp_fract fps_max;
	struct rts_isp_fract fps_min;
};

enum isp_mem_sync_type {
	ISP_SYNC_FOR_DEVICE,
	ISP_SYNC_FOR_CPU,
};

enum isp_mem_dma_dir {
	ISP_DMA_BIDIRECTIONAL,
	ISP_DMA_TO_DEVICE,
	ISP_DMA_FROM_DEVICE,
};

struct rts_isp_statis_info {
	uint32_t phy_addr;
	uint32_t size;
	uint32_t num;
};

struct isp_i2c_info_reg {
	struct rts_isp_i2c_info info;
	struct rts_isp_i2c_reg reg;
};

int isp_os_driver_init(void);
void isp_os_driver_cleanup(void);

isp_os_fd_t isp_os_driver_get_pfd(void);
isp_os_fd_t isp_os_driver_get_cfd(void);

void *isp_os_driver_mmap(uint32_t start, uint32_t size);
void isp_os_driver_munmap(void **ptr, uint32_t size);
int isp_os_driver_i2c_read_reg(struct isp_i2c_info_reg *i2c);
int isp_os_driver_i2c_write_reg(const struct isp_i2c_info_reg *i2c);
int isp_os_driver_i2c_read(uint8_t i2c_id, void *buf, uint16_t len);
int isp_os_driver_i2c_write(uint8_t i2c_id, void *buf, uint16_t len);
void isp_os_driver_wait_event(int32_t event);
int isp_os_driver_set_power(const struct rts_isp_snr_pwr *pwr);
int isp_os_driver_set_info(const struct isp_frame_info *info);
int isp_os_driver_write_sync_regs(const struct rts_isp_sync_regs *regs);
int isp_os_driver_write_sync_reg(uint32_t value, uint32_t offset, uint32_t mask,
								 uint32_t delay, int32_t interrupt);
int isp_os_driver_clear_sync_regs(void);
int isp_os_driver_init_statis(struct rts_isp_statis_info *info);
int isp_os_driver_cleanup_statis(void);
int isp_os_driver_get_statis(void *statis, uint32_t size);
int isp_os_driver_put_statis(uint32_t id);
int isp_os_driver_get_count(void *count);
int isp_os_driver_mem_alloc(uint32_t *phy_addr, uint32_t length,
							const char *info);
int isp_os_driver_mem_free(uint32_t phy_addr);
int isp_os_driver_sel_pin_state(int32_t interface);
int isp_os_driver_notify_dynamic_fps(float fps);
int isp_os_driver_set_clk(int32_t type, uint32_t *rate);
int isp_os_driver_is_fpga(void);
int isp_os_driver_get_ic_cut(void);
void isp_os_driver_sync_lock(int lock);
int isp_os_driver_notify_color_range(int range);
int isp_os_driver_get_statis_cfg(uint32_t *base, uint32_t *size);
int isp_os_driver_set_fps(uint32_t fps);
uint32_t isp_os_driver_get_bus_addr(void);
int isp_os_driver_pm_enable(bool enable);
int isp_os_driver_set_large_image_mode(bool mode);
#endif /* _ISP_OS_DRIVER_H_INC_ */
