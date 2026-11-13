#ifndef _U_RTS_CAMERA_ISP_H
#define _U_RTS_CAMERA_ISP_H

#include <isp_rtos_ioctl.h>

struct v4l2_fract {
	uint32_t numerator;
	uint32_t denominator;
};

struct rtscam_isp_frame_info {
	uint16_t width;
	uint16_t height;
	struct v4l2_fract fps_max;
	struct v4l2_fract fps_min;
};

struct rtscam_isp_iomem_info {
	uint32_t start;
	uint32_t size;
};

enum rtscam_isp_event {
	RTSCAM_ISP_DATA_START,
	RTSCAM_ISP_FRAME_END,
};

/* Events used between ISP pipeline and SOC/zoom layers */
enum rtscam_soc_event_id {
	RTSCAM_ISP_EVT_FPS_CHANGED = 0,
	RTSCAM_ZOOM_EVT_STREAM_INFO_CHANGED,
};

struct rtscam_isp_region {
	uint32_t base;
	uint32_t size;
};

struct rtscam_isp_i2c_info {
	uint8_t i2c_id;
	uint8_t addr_len;
	uint8_t data_len;
};

struct rtscam_isp_i2c_reg {
	uint16_t addr;
	uint16_t data;
};

struct rtscam_isp_i2c_info_reg {
	struct rtscam_isp_i2c_info info;
	struct rtscam_isp_i2c_reg reg;
};

struct rtscam_isp_i2c_msg {
	uint16_t addr;
	uint16_t flags;
	uint16_t len;
	uint8_t buf[64];
};

struct rtscam_isp_i2c_reg_mask {
	uint16_t addr;
	uint16_t data;
	uint16_t mask;
	uint8_t i2c_id;
	uint8_t addr_len : 4;
	uint8_t data_len : 4;
};

struct rtscam_isp_reg_mask {
	uint32_t addr;
	uint32_t data;
	uint32_t mask;
};

enum rtscam_isp_interrupt {
	RTSCAM_ISP_INT_NONE,
	RTSCAM_ISP_INT_DATA_START,
	RTSCAM_ISP_INT_FRAME_END,
	_MAX_RTSCAM_ISP_INT,
};

enum rtscam_isp_sync_type {
	RTSCAM_ISP_SYNC_TYPE_INFO,
	RTSCAM_ISP_SYNC_TYPE_I2C,
	RTSCAM_ISP_SYNC_TYPE_REG,
	_MAX_RTSCAM_ISP_SYNC_TYPE,
};

struct rtscam_isp_sync_info {
	uint32_t delay_frames; /* total max 4 */
	int32_t interrupt; /* enum rtscam_isp_interrupt */
};

struct rtscam_isp_sync_reg {
	int32_t type; /* enum rtscam_isp_sync_type */
	union {
		struct rtscam_isp_i2c_reg_mask i2c;
		struct rtscam_isp_reg_mask reg;
		struct rtscam_isp_sync_info info;
	};
};

struct rtscam_isp_i2c_sync_regs {
	uint32_t num;
	struct rtscam_isp_i2c_reg_mask regs[(256 - 16) / 8];
};

struct rtscam_isp_sync_regs {
	uint32_t num;
	struct rtscam_isp_sync_reg *reg; /* max 32 regs for sensor */

	/* private fields, do not set in sensor model */
	uint32_t split_index;
};

struct rtscam_isp_statis_info {
	uint32_t phy_addr;
	uint32_t size;
	uint32_t num;
};

struct rtscam_isp_statis_awb_reg {
	uint32_t illum_white_pixels[6];
	uint32_t fine_r_sum;
	uint32_t fine_g_sum;
	uint32_t fine_b_sum;
	uint32_t fine_white_pixels;
};

struct rtscam_isp_statis_flick_reg {
	uint32_t fft_sum2_9;
	uint32_t fft_sum2_127;
	uint32_t valid;
};

struct rtscam_isp_statis_data {
	uint32_t buf_id;

	uint32_t frame_count;
	struct rtscam_isp_statis_awb_reg awb_reg;
	struct rtscam_isp_statis_flick_reg flick_reg;
	struct rtscam_isp_statis_flick_reg flick_s_reg;
};

struct rtscam_isp_count {
	uint32_t frame_count;
	uint32_t statis_count;
};

enum rtscam_isp_snr_pwr_type {
	RTSCAM_ISP_SNR_RST_GPIO,
	RTSCAM_ISP_SNR_PWDN_GPIO,
	RTSCAM_ISP_SNR_HCLK,
	RTSCAM_ISP_SNR_IO_POWER,
	RTSCAM_ISP_SNR_ANALOG_POWER,
	RTSCAM_ISP_SNR_CORE_POWER,
	_MAX_RTSCAM_ISP_SNR_POWER_TYPE,
};

struct rtscam_isp_snr_pwr_item {
	int32_t type; /* enum rtscam_isp_snr_pwr_type */
	uint32_t value;
	uint32_t delay; /* us */
};

struct rtscam_isp_snr_pwr {
	uint32_t num;
	struct rtscam_isp_snr_pwr_item items[16];
};

enum rtscam_isp_pin_state {
	RTSCAM_ISP_PIN_STATE_NONE,
	RTSCAM_ISP_PIN_STATE_DVP,
	RTSCAM_ISP_PIN_STATE_MIPI,
};

enum rtscam_isp_clock_type {
	RTSCAM_ISP_CLOCK_INTERFACE,
	RTSCAM_ISP_CLOCK_ISP,
};

struct rtscam_isp_clock {
	int32_t type; /* enum rtscam_isp_clock_type */
	unsigned long rate;
};

#define RTSISP_IOC_PIPE_MAGIC 'P'

#define RTSISP_IOC_PIPE_SET_FRAME_INFO \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 0, struct rtscam_isp_frame_info)
#define RTSISP_IOC_PIPE_NOTIFY_DYN_FPS \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 1, struct v4l2_fract)
#define RTSISP_IOC_PIPE_NOTIFY_COLOR_RANGE \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 2, int32_t)

#define RTSISP_IOC_PIPE_IS_FPGA _IOR(RTSISP_IOC_PIPE_MAGIC, 16, int32_t)
#define RTSISP_IOC_PIPE_GET_IC_CUT _IOR(RTSISP_IOC_PIPE_MAGIC, 71, uint8_t)
#define RTSISP_IOC_PIPE_GET_IOMEM_INFO \
	_IOR(RTSISP_IOC_PIPE_MAGIC, 17, struct rtscam_isp_iomem_info)
#define RTSISP_IOC_PIPE_GET_TNR_BIT _IOR(RTSISP_IOC_PIPE_MAGIC, 18, uint32_t)
#define RTSISP_IOC_PIPE_WAIT_EVENT _IOW(RTSISP_IOC_PIPE_MAGIC, 19, int32_t)
#define RTSISP_IOC_PIPE_GET_STATIS_CFG \
	_IOR(RTSISP_IOC_PIPE_MAGIC, 20, struct rtscam_isp_region)

#define RTSISP_IOC_PIPE_I2C_READ_REG \
	_IOWR(RTSISP_IOC_PIPE_MAGIC, 32, struct rtscam_isp_i2c_info_reg)
#define RTSISP_IOC_PIPE_I2C_WRITE_REG \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 33, struct rtscam_isp_i2c_info_reg)
#define RTSISP_IOC_PIPE_I2C_TRANSTER \
	_IOWR(RTSISP_IOC_PIPE_MAGIC, 34, struct rtscam_isp_i2c_msg)
/* realtime i2c request */
#define RTSISP_IOC_PIPE_I2C_SYNC_WRITE \
	_IOWR(RTSISP_IOC_PIPE_MAGIC, 35, struct rtscam_isp_i2c_sync_regs)

#define RTSISP_IOC_PIPE_SYNC_WRITE \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 40, struct rtscam_isp_sync_regs)
#define RTSISP_IOC_PIPE_SYNC_CLEAR _IO(RTSISP_IOC_PIPE_MAGIC, 41)
#define RTSISP_IOC_PIPE_SYNC_LOCK _IOW(RTSISP_IOC_PIPE_MAGIC, 42, int32_t)

#define RTSISP_IOC_PIPE_INIT_STATIS \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 48, struct rtscam_isp_statis_info)
#define RTSISP_IOC_PIPE_CLEANUP_STATIS _IO(RTSISP_IOC_PIPE_MAGIC, 49)
#define RTSISP_IOC_PIPE_GET_STATIS \
	_IOR(RTSISP_IOC_PIPE_MAGIC, 50, struct rtscam_isp_statis_data)
#define RTSISP_IOC_PIPE_PUT_STATIS _IOW(RTSISP_IOC_PIPE_MAGIC, 51, uint32_t)
#define RTSISP_IOC_PIPE_GET_COUNT \
	_IOR(RTSISP_IOC_PIPE_MAGIC, 52, struct rtscam_isp_count)
#define RTSISP_IOC_PIPE_GET_BUS_ADDR _IOR(RTSISP_IOC_PIPE_MAGIC, 53, uint32_t)
#define RTSISP_IOC_PIPE_PM_ENABLE _IOW(RTSISP_IOC_PIPE_MAGIC, 54, bool)
#define RTSISP_IOC_PIPE_SET_LARGE_IMAGE_MODE \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 55, bool)

#define RTSISP_IOC_PIPE_SET_POWER \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 64, struct rtscam_isp_snr_pwr)
#define RTSISP_IOC_PIPE_SEL_PIN_STATE _IOW(RTSISP_IOC_PIPE_MAGIC, 65, int32_t)
#define RTSISP_IOC_PIPE_SET_CLOCK \
	_IOWR(RTSISP_IOC_PIPE_MAGIC, 66, struct rtscam_isp_clock)
#define RTSISP_IOC_PIPE_TEST_PREVIEW \
	_IOW(RTSISP_IOC_PIPE_MAGIC, 67, struct v4l2_fract)
#define RTSISP_IOC_PIPE_NEED_RESUME _IOR(RTSISP_IOC_PIPE_MAGIC, 68, int32_t)
#define RTSISP_IOC_PIPE_FRAME_SYNC_EVENT \
	_IOR(RTSISP_IOC_PIPE_MAGIC, 69, uint32_t)
#define RTSISP_IOC_PIPE_SET_AOV_MODE _IOW(RTSISP_IOC_PIPE_MAGIC, 70, uint32_t)

#endif /* _U_RTS_CAMERA_ISP_H */
