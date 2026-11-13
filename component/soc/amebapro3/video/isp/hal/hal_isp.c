/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

static const char *TAG = "HAL_ISP";
#include "diag.h"
#include <stdarg.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ameba_soc.h"
#include "rts_video.h"
#include "isp_regs.h"
#include "isp_common.h"
#include "rts_camera.h"
#include "isp_stream.h"
#include "isp_driver.h"
#include "rtsavisp.h"
#include "rts_isp_sensor.h"
#include "rts_raw/rts_raw.h"
#include "rts_isp_ctrl.h"
#include "hal_isp.h"

static stream_params_t  stream_param[STREAM_CNT_MAX];
static int isp_4k_mode;
static int isp_mode_locked;

int ISP_Set4KMode(int enable)
{
	if ((enable != 0 && enable != 1) || (isp_mode_locked && enable != isp_4k_mode)) {
		return -1;
	}
	isp_4k_mode = enable;
	return 0;
}

int ISP_Get4KMode(void)
{
	return isp_4k_mode;
}

/* Task argument structs — written before xTaskCreate, read by the tasks */
typedef struct {
	stream_params_t params[STREAM_CNT_MAX];
} hal_streamon_arg_t;

typedef struct {
	int flags[STREAM_CNT_MAX];
} hal_streamoff_arg_t;

typedef struct {
	int ch;
	int raw_index;
} hal_switchraw_arg_t;

static hal_streamon_arg_t s_streamon_arg;
static hal_streamoff_arg_t s_streamoff_arg;
static hal_switchraw_arg_t s_switchraw_arg;

void isp_mcu_apps_start(void);


int isp_rtos_printf(const char *format, ...)
{
	char buf[256];
	va_list args;
	int n;

	va_start(args, format);
	n = vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	RTK_LOGI(TAG, "%s", buf);
	return n;
}

static void hal_isp_streamon_task(void *arg)
{
	hal_streamon_arg_t *ta = (hal_streamon_arg_t *)arg;
	int i;

	for (i = 0; i < STREAM_CNT_MAX; i++) {
		stream_params_t *p = &ta->params[i];
		isp_stream_t *ctx = isp_stream_get_ctx(i);
		uint32_t pixfmt;
		int ret;

		if (!p->enable) {
			continue;
		}

		if (p->format == 1) {
			pixfmt = V4L2_PIX_FMT_NV12;
		} else if (p->format == 5) {
			pixfmt = V4L2_PIX_FMT_RGB24;
		} else {
			pixfmt = V4L2_PIX_FMT_YUYV;
		}

		isp_stream_open(ctx, pixfmt, p->width, p->height,
						p->fps, p->slot_num, 0, 0);
	}

	vTaskDelete(NULL);
}

static void hal_isp_streamoff_task(void *arg)
{
	hal_streamoff_arg_t *ta = (hal_streamoff_arg_t *)arg;
	int i;

	for (i = 0; i < STREAM_CNT_MAX; i++) {
		if (!ta->flags[i]) {
			continue;
		}
		isp_stream_close(isp_stream_get_ctx(i));
	}

	vTaskDelete(NULL);
}


static uint32_t hdr_en = 0;
static uint32_t hdr_maxfps = 20;
static int set_sensor_mode(int mode, int fps) // mode 0: linear 1: hdr
{
	uint32_t isp_id = 0;
	int i;
	struct rts_isp_sensor_modes sensor_modes;
	if (mode == 1) {
		mode = RTS_ISP_HDR_LINE_2TO1;
	}
	rts_av_isp_enum_sensor_modes(isp_id, &sensor_modes);
	for (i = 0; i < sensor_modes.num; i++) {
		RTK_LOGI(TAG, "[%s][L:%d] mode[%d] %d  %d  %d  %d.\r\n", __func__, __LINE__, i, mode, sensor_modes.mode[i].hdr, (int)sensor_modes.mode[i].fps, fps);
		if (sensor_modes.mode[i].hdr == mode && (int)(sensor_modes.mode[i].fps) == fps) {
			RTK_LOGI(TAG, "[%s][L:%d] mode[%d] .\r\n", __func__, __LINE__, i);
			break;
		}
	}
	if (i == sensor_modes.num) {
		RTK_LOGE(TAG, "[%s][L:%d] No compatible mode found\n", __func__, __LINE__);
		return -1;
	}
	return rts_av_isp_set_sensor_mode(isp_id, &sensor_modes.mode[i]);
}


void ISP_StreamOn(void *strm_param)
{
	memcpy(s_streamon_arg.params, strm_param, sizeof(s_streamon_arg.params));
	xTaskCreate(hal_isp_streamon_task, "streamon",
				configMINIMAL_STACK_SIZE * 16, &s_streamon_arg,
				(portPRIVILEGE_BIT | ((configMAX_PRIORITIES / 2) - 2)),
				NULL);
}

int ISP_StreamOnByChannel(void *arg)
{
	stream_params_t *strm_param = (stream_params_t *)arg;
	uint32_t ch = strm_param->ch;
	isp_stream_t *ctx = isp_stream_get_ctx(ch);
	uint32_t pixfmt;
	int ret;
	struct sensor_params_t *snr_params;
	const struct rts_isp_sensor_ops *ops;
	struct rts_isp_sensor_info local_info;
	struct rts_isp_sensor_info *snr_info;

	snr_info   = &local_info;
	ops        = ISP_GetSensorOps();
	if (!ops) {
		RTK_LOGE(TAG, "[%s][L:%d] sensor ops NULL, skip hdr setup\r\n", __func__, __LINE__);
	} else {
		snr_params = sensor_params + SENSOR_USE;
		if (ops->get_info(ISP0, snr_info)) {
			RTK_LOGE(TAG, "[%s][L:%d] get_info fail, use default hdr\r\n", __func__, __LINE__);
		} else {
			hdr_en = snr_params->hdr;
			if (snr_params->hdr < snr_info->modes.num) {
				hdr_maxfps = snr_info->modes.mode[snr_params->hdr].fps;
			}
		}
	}
	if (hdr_en) {
		set_sensor_mode(hdr_en, hdr_maxfps);
		strm_param->fps = hdr_maxfps;
	}

	if (strm_param->format == ISP_FORMAT_YUV420_SEMIPLANAR) {
		pixfmt = V4L2_PIX_FMT_NV12;
	} else if (strm_param->format == ISP_FORMAT_YUV422_SEMIPLANAR) {
		pixfmt = V4L2_PIX_FMT_NV16;
	} else if (strm_param->format == ISP_FORMAT_RGB888_PLANAR) {
		pixfmt = V4L2_PIX_FMT_RGB24;
	} else {
		RTK_LOGE(TAG, "[%s] unknown format %u\r\n", __func__, strm_param->format);
		return -1;
	}

	/* Wait for ISP server to be running before attempting stream start. */
	{
		int isp_wait_ms = 0;
		while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
			if (isp_wait_ms >= 10000) {
				RTK_LOGE(TAG, "[ISP] timeout waiting for ISP server ready\r\n");
				return -1;
			}
			vTaskDelay(pdMS_TO_TICKS(10));
			isp_wait_ms += 10;
		}
	}

	ret = isp_stream_open(ctx, pixfmt, strm_param->width,
						  strm_param->height, strm_param->fps,
						  strm_param->slot_num, strm_param->ar_feature, strm_param->ar_align_w);
	if (ret != RTS_OK) {
		RTK_LOGE(TAG, "[%s][L:%d] streamon fail\r\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

int ISP_StreamOffByChannel(int channel)
{
	int ret;

	if (channel < 0 || channel >= STREAM_CNT_MAX) {
		return -EINVAL;
	}

	ret = isp_stream_close(isp_stream_get_ctx(channel));
	if (ret)
		RTK_LOGE(TAG, "[%s][L:%d] Stream-off fail ch=%d ret=0x%x\r\n",
				 __func__, __LINE__, channel, ret);

	return ret;
}

void ISP_StreamOff(int nv0, int nv1, int nv2, int nv3, int rgb)
{
	int off_flag[STREAM_CNT_MAX] = {nv0, nv1, nv2, nv3, rgb};

	memcpy(s_streamoff_arg.flags, off_flag, sizeof(s_streamoff_arg.flags));
	xTaskCreate(hal_isp_streamoff_task, "streamoff",
				configMINIMAL_STACK_SIZE * 16, &s_streamoff_arg,
				(portPRIVILEGE_BIT | ((configMAX_PRIORITIES / 2) - 2)),
				NULL);
}

static void switch_raw_task(void *arg)
{
	hal_switchraw_arg_t *ta = (hal_switchraw_arg_t *)arg;
	int ch = ta->ch;
	int raw_index = ta->raw_index;
	uint32_t pixfmt;

	if (ch < 0 || ch >= STREAM_CNT_MAX) {
		RTK_LOGE(TAG, "[%s][L:%d] ch:%d channel index is incorrect.\r\n", __func__, __LINE__, ch);
		vTaskDelete(NULL);
		return;
	}
	if (stream_param[ch].width == 0) {
		RTK_LOGE(TAG, "[%s][L:%d] stream parameters is empty.\r\n", __func__, __LINE__);
		vTaskDelete(NULL);
		return;
	}

	rts_hal_set_raw(ch, raw_index);
	rts_hal_set_bypass(ch, raw_index);

	pixfmt = (raw_index > RTS_RAW_SRC_DISABLE) ? V4L2_PIX_FMT_128BIT_3PIXEL : V4L2_PIX_FMT_NV16;

	ISP_StreamOffByChannel(ch);
	isp_stream_open(isp_stream_get_ctx(ch), pixfmt, stream_param[ch].width,
					stream_param[ch].height, stream_param[ch].fps,
					stream_param[ch].slot_num, 0, 0);

	vTaskDelete(NULL);
}

void ISP_SwitchRAWData(int raw_index)
{
	s_switchraw_arg.ch = 0;
	s_switchraw_arg.raw_index = raw_index;
	xTaskCreate(switch_raw_task, "bayer_switch",
				configMINIMAL_STACK_SIZE * 16, &s_switchraw_arg,
				(portPRIVILEGE_BIT | ((configMAX_PRIORITIES / 2) - 2)),
				NULL);
}


int ISP_GetIntrEn(int is_yuv)
{
	if (is_yuv) {
		return read_reg(0x60000000, 0xF0250);
	} else {
		return read_reg(0x60000000, 0xF0260);
	}
}

int ISP_GetFrameCount(void)
{
	return read_reg(0x60000000, 0x20);
}

int ISP_GetMipiFrameCountL(void)
{
	return read_reg(0x60000000, 0xC0200);
}
int ISP_GetMipiFrameWidthL(void)
{
	return read_reg(0x60000000, 0xC0210);
}
int ISP_GetMipiFrameHeightL(void)
{
	return read_reg(0x60000000, 0xC020C);
}

int ISP_GetMipiFrameCountS(void)
{
	return read_reg(0x60000000, 0xC0214);
}
int ISP_GetMipiFrameWidthS(void)
{
	return read_reg(0x60000000, 0xC0224);
}
int ISP_GetMipiFrameHeightS(void)
{
	return read_reg(0x60000000, 0xC0220);
}

int ISP_GetBufferCount(int stream_idx)
{
	if (stream_idx < 4 && stream_idx >= 0) {
		int mask = (0xF << (stream_idx * 4));
		int buf_cnt = ((read_reg(0x60000000, 0xF0180) & mask) >> (stream_idx * 4));
		if (buf_cnt == 0) {
			buf_cnt = stream_param[stream_idx].slot_num;
		}
		return buf_cnt;
	} else if (stream_idx == 4) {
		int buf_cnt = read_reg(0x60000000, 0xF0188);
		if (buf_cnt == 0) {
			buf_cnt = stream_param[stream_idx].slot_num;
		}
		return buf_cnt;
	} else {
		RTK_LOGE(TAG, "[%s][L:%d] stream_idx is out of range.\r\n", __func__, __LINE__);
		return 0;
	}
}

int ISP_GetBufferStatus(int stream_idx, int buffer_idx)
{
	//VIDEO_STREAM0_BUFFER_STATUS 0xF0080
	uint32_t offset = 0x4;
	uint32_t shift = 8 * buffer_idx;
	int value = read_reg(0x60000000, VIDEO_STREAM0_BUFFER_STATUS + offset * stream_idx);
	return (value & (0xFF << shift)) >> shift;
}

int ISP_GetBufferSize(int stream_idx, int channel_idx)
{
	if (stream_idx < 4 && stream_idx >= 0) {
		return read_reg(0x60000000, VIDEO_FRAME_LEN_Y0 + channel_idx * 4 + stream_idx * 8);
	} else if (stream_idx == 4) {
		return read_reg(0x60000000, 0xF02C0 + channel_idx * 4);
	} else {
		RTK_LOGE(TAG, "[%s][L:%d] stream_idx is out of range.\r\n", __func__, __LINE__);
		return 0;
	}
}

int ISP_GetBufferAddr(int stream_idx, int buffer_idx, int channel_idx)
{
	if (stream_idx < 4 && stream_idx >= 0) {
		return read_reg(0x60000000, VIDEO_Y0_FRAME_ADDR0 + stream_idx * 0x20 + channel_idx * 16 + buffer_idx * 4);
	} else if (stream_idx == 4) {
		return read_reg(0x60000000, 0xF0140 + channel_idx * 12 + buffer_idx * 4);
	} else {
		RTK_LOGE(TAG, "[%s][L:%d] stream_idx is out of range.\r\n", __func__, __LINE__);
		return 0;
	}
}

int ISP_GetRAWBufSize(void)
{
	uint32_t w = 0, h = 0, fmt = 0;

	if (rts_hal_get_snr_fmt(&w, &h, &fmt)) {
		return 0;
	}

	return (int)(w * h * 2);
}

int ISP_GetRawIdx(void)
{
	return rts_raw_get_current_src();
}

int ISP_SetStreamFPS(uint32_t ch, uint32_t fps)
{
	struct rts_video_fps vfps = {0};

	if (ch >= STREAM_CNT_MAX || fps == 0) {
		return -EINVAL;
	}

	vfps.numerator = 1;
	vfps.denominator = fps;

	return rtscam_video_set_fps(ch, &vfps);
}

int ISP_GetStreamEn(uint32_t ch)
{
	//VIDEO_STREAM0_ENABLE
	uint32_t offset = 0xF0000 + 4 * ch;
	return read_reg(0x60000000, offset);
}

int ISP_GetYccInterleave(void)
{
	//VIDEO_YUV422_INTERLEAVED_SEL
	uint32_t offset = 0xF0074;
	return read_reg(0x60000000, offset);
}

int ISP_GetDataReady(int stream_idx, int buffer_idx)
{
	int offset = VIDEO_STREAM0_BUFFER_STATUS + stream_idx * 0x4; // VIDEO_STREAM0_BUFFER_STATUS:0xF0080
	int read_value = read_reg(0x60000000, offset);
	if (stream_idx < 4 && stream_idx >= 0) {
		return (read_value & (0x0020 << buffer_idx * 8));
	} else if (stream_idx == 4) {
		return (read_value & (0x0010 << buffer_idx * 8));
	} else {
		RTK_LOGE(TAG, "[%s][L:%d] stream_idx is out of range.\r\n", __func__, __LINE__);
		return 0;
	}
}

void ISP_Stop(struct isp_io *io)
{
	rts_av_isp_stop();
	rts_av_isp_cleanup();
	isp_mcu_driver_cleanup();
}

/* --- ISP buffer and callback interface ------------------------------------ */

static int (*s_isp_ch_handle)(int stream_id, int buf_id, uint32_t phy_addr);
static int (*s_isp_data_start_handle)(int buf_id);

void isp_buf_release(int stream_id, void *buf_addr, int buf_id)
{
	isp_stream_release_buf(isp_stream_get_ctx(stream_id), buf_addr, buf_id);
}

void isp_set_user_release(void)
{
	int i;
	for (i = 0; i < STREAM_CNT_MAX; i++) {
		isp_stream_set_user_release(isp_stream_get_ctx(i), 1);
	}
}

void isp_buf_recycle(int stream_id, int buf_id)
{
	if (stream_id < 0 || stream_id >= STREAM_CNT_MAX) {
		return;
	}
	isp_stream_recycle(isp_stream_get_ctx(stream_id), buf_id);
}

void isp_ch_handle_register(int (*handle)(int stream_id, int buf_id, uint32_t phy_addr))
{
	RTK_LOGI(TAG, "Registering frame done handler\n");
	for (int i = 0; i < STREAM_CNT_MAX; i++) {
		isp_stream_register_handler(isp_stream_get_ctx(i), handle);
	}
}

void isp_data_start_handle_register(int (*handle)(int buf_id))
{
	s_isp_data_start_handle = handle;
}

/* --- Platform helpers ----------------------------------------------------- */

void dcache_invalidate_by_addr(uint32_t addr, size_t dsize)
{
	DCache_Invalidate(addr, (u32)dsize);
}

int rts_camera_init(void)
{
	/* Initialize the video pipeline layer. */
	return rtscam_video_init();
}

int isp_initial(struct isp_io *io)
{
	extern void rtscam_soc_isr(void);

	if (!io) {
		return -1;
	}

	io->base        = (void *)0x60000000;
	io->initialized = 1;

	return 0;
}

void isp_release(struct isp_io *io)
{
	if (!io) {
		return;
	}
	InterruptDis(ISP0_IRQ);
	io->initialized = 0;
}



/**
*  @brief Description of ISP_Init
*         ISP_Init is used to do ISP initialization
*
*  @param[in]  void
*
*  @returns void
*/
void ISP_Init(void)
{
	/* Clock, sensor and buffer settings must remain consistent for this boot. */
	isp_mode_locked = 1;
	/* ISP SCLK: 4K mode uses SYS2_PLL / 2; normal mode uses SYS1_PLL / 3. */
	if (ISP_Get4KMode()) {
		RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 2, 1);
		RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS2_PLL);
	} else {
		RCC_PeriphClockDividerSet(SYS_ISP_SCLK, 3, 1);
		RCC_PeriphClockSourceSet(SYS_ISP_SCLK, SYS1_PLL);
	}
	RCC_PeriphClockCmd(APBPeriph_ISP, APBPeriph_ISP_CLOCK, ENABLE);

	/* ISP MBUS: match SCLK (4K DEV: SYS2_PLL / 2; otherwise SYS1_PLL / 3). */
	if (ISP_Get4KMode()) {
		RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 2, 1);
		RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS2_PLL);
	} else {
		RCC_PeriphClockDividerSet(SYS_ISP_BUSCLK, 3, 1);
		RCC_PeriphClockSourceSet(SYS_ISP_BUSCLK, SYS1_PLL);
	}
	RCC_PeriphClockCmd(APBPeriph_PLFM_CORE_ISPMBUS_CLK, APBPeriph_ISP_MBUS_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_CBUS_ISP_BUS_CLOCK, ENABLE);

	/* ISP MIPI CLK 600M */
	RCC_PeriphClockDividerSet(SYS_ISP_MIPICLK, 0, 0);
	RCC_PeriphClockSourceSet(SYS_ISP_MIPICLK, SYS1_PLL);
	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_ISP_MIPI_CLOCK, ENABLE);

	/* ISP Address Remap */
	RCC_PeriphClockCmd(APBPeriph_ISP_ADDR_REMAP, APBPeriph_ADRM_ISP_CLOCK, ENABLE);

	/* Sensor clock */
	RCC_PeriphClockDividerSet(SYS_SENSOR_CLK, 24, 0);
	RCC_PeriphClockSourceSet(SYS_SENSOR_CLK, SYS1_PLL);

#if !defined(CONFIG_PXP) || (CONFIG_PXP != 1)
	// Config GPIO
	AON_PAD_BASE->PWRPAD3N_SHDN_CTRL |= AON_PAD_BIT_SYSON_GPIOB_PWD33;
	AON_PAD_BASE->PWRPAD1N_SHDN_CTRL |= AON_PAD_BIT_PON_GPIOB_PWD09;

	//_PB_2: SEN_RST/GPIOB_2
	GPIO_InitTypeDef GPIO_InitStruct_TempB2;
	GPIO_InitStruct_TempB2.GPIO_Pin = _PB_2;
	GPIO_InitStruct_TempB2.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_TempB2);

	//_PB_1: EN_PDN/GPIOB_1
	GPIO_InitTypeDef GPIO_InitStruct_TempB1;
	GPIO_InitStruct_TempB1.GPIO_Pin = _PB_1;
	GPIO_InitStruct_TempB1.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_TempB1);

	//_PA_5: Power
	GPIO_InitTypeDef GPIO_InitStruct_TempA5;
	GPIO_InitStruct_TempA5.GPIO_Pin = _PA_5;
	GPIO_InitStruct_TempA5.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_TempA5);
	DelayMs(1);


#if 0
	// imx307
	GPIO_WriteBit(_PA_5, 1);
	GPIO_WriteBit(_PB_2, 0);
	DelayMs(10);
	Pinmux_Config(_PB_0, PINMUX_FUNCTION_SENSOR_CLK);
	DelayMs(10);
	GPIO_WriteBit(_PB_2, 1);
#else
	//os04A10
	GPIO_WriteBit(_PB_1, 0);
	GPIO_WriteBit(_PB_2, 0);
	GPIO_WriteBit(_PA_5, 1);
	//DelayMs(200);

	GPIO_WriteBit(_PB_1, 1);//PWN
	//DelayMs(200);
	GPIO_WriteBit(_PB_2, 1);//RESET
	DelayMs(1);
	Pinmux_Config(_PB_0, PINMUX_FUNCTION_SENSOR_CLK);
	DelayMs(1);
#endif
#endif

	/* Initialise per-channel stream contexts — binds each ctx to its channel
	 * and preserves any callbacks registered before ISP_Init was called. */
	for (int i = 0; i < STREAM_CNT_MAX; i++) {
		isp_stream_init(isp_stream_get_ctx(i), i);
	}

	isp_mcu_apps_start();
}

int ISP_SetSensorMode(int mode, int fps) // mode 0: linear 1: hdr
{
	if (mode < 0 || mode > 1) {
		return -RTS_ISP_EINVAL;
	}
	if (fps < 1 || fps > 60) {
		return -RTS_ISP_EINVAL;
	}
	hdr_en = mode;
	hdr_maxfps = fps;
	return 0;
}
int ISP_SetISPCtrl(uint32_t id, int *value)
{
	int ret;
	struct isp_v4l2_control c;
	int final_id;
	if (!value) {
		return -RTS_ISP_EINVAL;
	}
	final_id = ISP_V4L2_CID_BASE + id;
	c.id = final_id;
	c.value = *value;
	RTK_LOGI(TAG, "[%s][L:%d]  ispctrl  id: 0x%X  set  :%d\r\n", __func__, __LINE__, id, *value);
	ret = rts_isp_v4l2_s_ctrl(0, &c);
	if (ret) {
		RTK_LOGE(TAG, "[%s][L:%d] failed to set ctrl 0x%08x ret 0x%08x", __func__, __LINE__, id, ret);
		return ret;
	}

	return 0;
}
int ISP_GetISPCtrl(uint32_t id, int *value)
{
	int ret;
	struct isp_v4l2_control c;
	int final_id;
	if (!value) {
		return -RTS_ISP_EINVAL;
	}
	final_id = ISP_V4L2_CID_BASE + id;

	c.id = final_id;
	c.value = -1;
	ret = rts_isp_v4l2_g_ctrl(0, &c);

	*value = c.value;
	RTK_LOGI(TAG, "[%s][L:%d]  ispctrl  id: 0x%X  get  :%d\r\n", __func__, __LINE__, id, *value);
	DCache_CleanInvalidate((u32)value, 4);

	return ret;
}

const struct rts_isp_sensor_ops *ISP_GetSensorOps(void)
{
	struct sensor_params_t *snr_params;
	const struct rts_isp_sensor_ops *ops;

	if (SENSOR_USE >= SENSOR_PARAM_COUNT) {
		return NULL;
	}
	snr_params = sensor_params + SENSOR_USE;
	ops = snr_params->get_ops();
	if (!ops) {
		return NULL;
	}
	return ops;
}

static const struct rts_isp_sensor_mode *isp_get_sensor_mode(struct rts_isp_sensor_info *snr_info)
{
	struct sensor_params_t *snr_params;
	const struct rts_isp_sensor_ops *ops = ISP_GetSensorOps();
	uint32_t idx;

	if (!ops || !snr_info) {
		return NULL;
	}
	if (ops->get_info(ISP0, snr_info)) {
		return NULL;
	}
	snr_params = sensor_params + SENSOR_USE;
	idx = snr_params->hdr;
	if (idx >= snr_info->modes.num) {
		idx = 0;
	}
	return &snr_info->modes.mode[idx];
}

int ISP_GetSensorDimension(uint32_t *sensor_w, uint32_t *sensor_h)
{
	struct rts_isp_sensor_info snr_info;
	const struct rts_isp_sensor_mode *mode = isp_get_sensor_mode(&snr_info);

	if (!mode || !sensor_w || !sensor_h) {
		return -RTS_ISP_EINVAL;
	}
	*sensor_w = mode->size.w;
	*sensor_h = mode->size.h;
	return 0;
}

int ISP_GetSensorInfo(struct sensor_info_t *sensor_info)
{
	struct rts_isp_sensor_info snr_info;
	const struct rts_isp_sensor_mode *mode = isp_get_sensor_mode(&snr_info);

	if (!mode || !sensor_info) {
		return -RTS_ISP_EINVAL;
	}
	sensor_info->width  = mode->size.w;
	sensor_info->height = mode->size.h;
	sensor_info->fps    = (uint32_t)mode->fps;
	return 0;
}

int ISP_SetSensorIQ(unsigned int sensor_idx, unsigned int iq_idx)
{
	const struct rts_isp_sensor_ops *ops;
	const void *iq_buf;

	if (sensor_idx >= SENSOR_PARAM_COUNT) {
		RTK_LOGE(TAG, "[%s][L:%d] Fail: Too large sensor index(sensor_idx:%d)\r\n", __func__, __LINE__, sensor_idx);
		return -1;
	}
	ops = sensor_params[sensor_idx].get_ops();
	if (ops == NULL) {
		RTK_LOGE(TAG, "[%s][L:%d] Fail: Sensor driver is NULL(sensor_idx:%d)\r\n", __func__, __LINE__, sensor_idx);
		return -1;
	}
	if (iq_idx >= IQ_COUNT) {
		RTK_LOGE(TAG, "[%s][L:%d] Fail: Too large IQ index(iq_idx:%d)\r\n", __func__, __LINE__, iq_idx);
		return -1;
	}
	iq_buf = sensor_params[sensor_idx].iq_buf[iq_idx];
	if (iq_buf == NULL) {
		RTK_LOGE(TAG, "[%s][L:%d] Fail: IQ buffer is NULL(iq_idx:%d)\r\n", __func__, __LINE__, iq_idx);
		return -1;
	}
	SENSOR_USE   = sensor_idx;
	IQ_USE       = iq_idx;
	return 0;
}

int ISP_GetISPSlotNum(uint32_t id)
{
	extern int rtscam_soc_get_hw_slot_num(uint32_t streamid);
	return rtscam_soc_get_hw_slot_num(id);
}

static int tuning_sensor_i2c(int is_read, uint16_t addr, unsigned char *reg_buf)
{
	int err = 0;
	const struct rts_isp_sensor_ops *ops;
	struct rts_isp_i2c_reg reg;
	struct rts_isp_sensor_info sensor_info;

	if (!reg_buf) {
		return -RTS_ISP_EINVAL;
	}
	ops = ISP_GetSensorOps();
	if (!ops) {
		return -RTS_ISP_EINVAL;
	}
	if (ops->get_info(0, &sensor_info)) {
		return -RTS_ISP_EINVAL;
	}
	RTK_LOGI(TAG, "[%s][L:%d] i2c info  id:%d  addr_len:%d  data_len:%d.\r\n", __func__, __LINE__, sensor_info.i2c.i2c_id, sensor_info.i2c.addr_len,
			 sensor_info.i2c.data_len);

	reg.addr = addr;
	if (is_read) {
		reg.data = 0;
		err = rts_isp_read_i2c_reg(&sensor_info.i2c, &reg);

		RTK_LOGI(TAG, "[%s][L:%d] read addr:0x%04X, data:0x%04X.\r\n", __func__, __LINE__, reg.addr, reg.data);
		reg_buf[0] = reg.data & 0xFF;
		reg_buf[1] = reg.data >> 8;
		reg_buf[2] = 0;
		reg_buf[3] = 0;
		DCache_CleanInvalidate((u32)reg_buf, 4);
	} else {
		reg.data = reg_buf[0];
		if (sensor_info.i2c.data_len >= 2) {
			reg.data += (reg_buf[1] << 8);
		}

		RTK_LOGI(TAG, "[%s][L:%d] write addr:0x%04X, data:0x%04X.\r\n", __func__, __LINE__, reg.addr, reg.data);
		err = rts_isp_write_i2c_reg(&sensor_info.i2c, &reg);
	}

	RTK_LOGI(TAG, "[%s][L:%d] err: %d\n", __func__, __LINE__, err);
	return err;
}

int  ISP_GetI2C(int addr, int *value)
{
	int err = tuning_sensor_i2c(1, (uint16_t)addr, (unsigned char *)value);
	return err;
}
int  ISP_SetI2C(int addr, int value)
{
	int err = tuning_sensor_i2c(0, (uint16_t)addr, (unsigned char *)&value);
	return err;
}
