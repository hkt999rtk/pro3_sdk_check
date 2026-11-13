#include <errno.h>
#include <string.h>
#include <list.h>
#include <rts_camera_isp.h>
#include <isp_rtos_mutex.h>
#include <isp_rtos_iomem.h>
#include <isp_rtos_atomic.h>
#include <isp_rtos_dev_inner.h>
#include <isp_rtos_atomic.h>
#include <pinctrl.h>
#include <clock.h>
#include <reset.h>
#include <gpio.h>
#include <i2c.h>
#include "rtscam_isp_command.h"
#include "rtscam_isp_pipeline.h"
#include "pipeline/mcu/rtscam_isp_mcu.h"
#include "pipeline/rtscam_isp_cru.h"
#include "pipeline/rtscam_isp_i2c.h"
#include "pipeline/rtscam_isp_power.h"
#include "../video/rts_camera_soc.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define SYS_VIDEO_CPU_SHMEM0_START 0x5886e00c

struct rtscam_isp_pipeline {
	isp_rtos_mutex_t lock;
	int id;

	uint32_t isp_clk_fix;
	struct rtscam_isp_cru cru;
	struct rtscam_isp_i2c i2c;
	struct rtscam_isp_power power;
	uint32_t bus_addr;

	struct {
		struct rtscam_rtos_pinctrl_state *default_state;
		struct rtscam_rtos_pinctrl_state *dvp_state;
		struct rtscam_rtos_pinctrl_state *mipi_state;
	} pins;

	struct v4l2_fract current_fps;
	struct rtscam_isp_region statis_cfg;

	/* frame info from sensor — valid when active != 0 */
	struct rtscam_isp_frame_info info;
	atomic_t active;

	/* streaming reference count — zoom increments/decrements */
	int stream_refcnt;

	/* AOV: keep ISP running after last stream is off */
	int isp_need_keep_streaming;

	/* FPS requested by soc before streaming starts */
	struct v4l2_fract soc_set_fps;

	/* set to 1 when ISP daemon needs to resume after suspend */
	int ispd_need_resume;

	/* TNR bit depth (8 or 10) */
	uint32_t tnr_bit;

	uint32_t is_fpga : 1;
	uint8_t ic_cut;
	uint32_t has_pmu : 1;
	uint32_t large_image_mode : 1;
};

static struct rtscam_isp_pipeline *m_pipeline[RTSCAM_ISPC_PIPELINE_NUM];

struct rtscam_isp_pipeline *rtscam_isp_get_pipeline(int id)
{
	if (id < 0 || id >= RTSCAM_ISPC_PIPELINE_NUM) {
		return NULL;
	}
	return m_pipeline[id];
}

int rtscam_isp_get_frame_info(struct rtscam_isp_pipeline *pipeline,
							  struct rtscam_isp_frame_info *info)
{
	if (!pipeline || !info) {
		return -EINVAL;
	}
	*info = pipeline->info;
	return 0;
}

static bool fps_zero(const struct v4l2_fract *fps)
{
	return !fps->denominator || !fps->numerator;
}

static bool fps_equal(const struct v4l2_fract *f1, const struct v4l2_fract *f2)
{
	if (fps_zero(f1) && fps_zero(f2)) {
		return true;
	}
	return (!fps_zero(f1) && !fps_zero(f2) &&
			f1->numerator * f2->denominator ==
			f1->denominator * f2->numerator);
}

int rtscam_isp_pm_enable(struct rtscam_isp_pipeline *pipeline, bool enable)
{
	if (!pipeline) {
		return -EINVAL;
	}

	if (enable) {
		rtscam_isp_cru_clk_enable(&pipeline->cru);
	} else {
		rtscam_isp_cru_clk_disable(&pipeline->cru);
	}

	return 0;
}

static int rtscam_isp_pm_enable_inner(struct rtscam_isp_pipeline *pipeline,
									  void *data)
{
	return rtscam_isp_pm_enable(pipeline, *(bool *)data);
}

static int rtscam_isp_set_large_image_mode(struct rtscam_isp_pipeline *pipeline,
		void *data)
{
	pipeline->large_image_mode = *(bool *)data;
	return 0;
}

int isp_client_message_simple(uint32_t isp_id, uint32_t mod_id, uint32_t action,
							  void *data, uint32_t len);
static int __rtscam_isp_set_fps(struct rtscam_isp_pipeline *pipeline,
								const struct v4l2_fract *fps)
{
	int ret;
	int stream_on;
	int stream_off;
	struct v4l2_fract fps_backup;
	struct rtscam_isp_preview_info info;

	if (!pipeline || !fps) {
		return -EINVAL;
	}

	if (!pipeline || !fps) {
		return -EINVAL;
	}

	if (fps_equal(&pipeline->current_fps, fps)) {
		return 0;
	}

	stream_on = fps_zero(&pipeline->current_fps);
	stream_off = fps_zero(fps);

	if (stream_on) {
		rtscam_isp_pm_enable(pipeline, true);
	}

	fps_backup = pipeline->current_fps;
	pipeline->current_fps = *fps;

	info.fps = *fps;

	isp_rtos_mutex_unlock(&pipeline->lock);
	ret = isp_client_message_simple(RTSCAM_ISPC_PIPELINE_0,
									RTSCAM_ISP_MOD_CONTROL,
									RTSCAM_ISP_SET_FPS, &info,
									sizeof(info));
	isp_rtos_mutex_lock(&pipeline->lock);
	if (ret) {
		* &pipeline->current_fps = fps_backup;
		if (stream_on) {
			rtscam_isp_pm_enable(pipeline, false);
		}
	}

	if (stream_off) {
		rtscam_isp_pm_enable(pipeline, false);
	}

	return ret;
}

static int rtscam_isp_test_preview(struct rtscam_isp_pipeline *pipeline,
								   void *args)
{
	return __rtscam_isp_set_fps(pipeline, (const struct v4l2_fract *)args);
}

static int rtscam_isp_sel_pin_state(struct rtscam_isp_pipeline *pipeline,
									void *args)
{
	int ret;
	int32_t state; /* enum rtscam_isp_pin_state */

	state = *(int32_t *)args;
	switch (state) {
	case RTSCAM_ISP_PIN_STATE_NONE:
		ret = rtscam_rtos_pinctrl_select_state(
				  pipeline->pins.default_state);
		break;
	case RTSCAM_ISP_PIN_STATE_DVP:
		ret = rtscam_rtos_pinctrl_select_state(
				  pipeline->pins.dvp_state);
		break;
	case RTSCAM_ISP_PIN_STATE_MIPI:
		ret = rtscam_rtos_pinctrl_select_state(
				  pipeline->pins.mipi_state);
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}

static int rtscam_isp_set_clock(struct rtscam_isp_pipeline *pipeline,
								void *args)
{
	struct rtscam_isp_clock *clk = args;

	if (clk->rate && pipeline->isp_clk_fix) {
		clk->rate = pipeline->isp_clk_fix;
	}

	return rtscam_isp_cru_clk_set_rate(&pipeline->cru, clk);
}

static int rtscam_isp_is_fpga(struct rtscam_isp_pipeline *pipeline, void *args)
{
	*(int32_t *)args = pipeline->is_fpga;
	return 0;
}

static int rtscam_isp_get_ic_cut(struct rtscam_isp_pipeline *pipeline,
								 void *args)
{
	*(uint8_t *)args = pipeline->ic_cut;
	return 0;
}

static int rtscam_isp_read_i2c_reg(struct rtscam_isp_pipeline *pipeline,
								   struct rtscam_isp_i2c_info_reg *reg)
{
	return rtscam_isp_i2c_read_reg(&pipeline->i2c, &reg->info, &reg->reg);
}

static int rtscam_isp_write_i2c_reg(struct rtscam_isp_pipeline *pipeline,
									struct rtscam_isp_i2c_info_reg *reg)
{
	return rtscam_isp_i2c_write_reg(&pipeline->i2c, &reg->info, &reg->reg);
}

static int rtscam_isp_i2c_process(struct rtscam_isp_pipeline *pipeline,
								  void *args)
{
	struct rtscam_isp_i2c_msg *msg = args;

	if (msg->len > sizeof(msg->buf)) {
		return -ERANGE;
	}

	return rtscam_isp_i2c_transfer(&pipeline->i2c, msg->addr, msg->buf,
								   msg->len, msg->flags);
}

static int rtscam_isp_get_statis_cfg(struct rtscam_isp_pipeline *pipeline,
									 void *args)
{
	*(struct rtscam_isp_region *)args = pipeline->statis_cfg;
	return 0;
}

static int rtscam_isp_get_bus_addr(struct rtscam_isp_pipeline *pipeline,
								   void *args)
{
	*(uint32_t *)args = pipeline->bus_addr;
	return 0;
}

static int rtscam_isp_get_tnr_bit(struct rtscam_isp_pipeline *pipeline,
								  void *args)
{
	*(uint32_t *)args = pipeline->tnr_bit;
	return 0;
}

static int rtscam_isp_refresh_zoom_info(struct rtscam_isp_pipeline *pipeline)
{
	uint32_t i;
	int ret;
	int force_reset;
	struct rtscam_zoom_dev *zoom_dev;

	if (!pipeline) {
		return -EINVAL;
	}

	if (!pipeline->info.fps_max.denominator ||
		!pipeline->info.fps_max.numerator || !pipeline->info.width ||
		!pipeline->info.height) {
		return -EINVAL;
	}

	if (pipeline->info.fps_max.denominator >
		RTSCAM_SOC_MAX_FPS * pipeline->info.fps_max.numerator) {
		return -EINVAL;
	}

	zoom_dev = rtscam_soc_get_zoom_dev();
	for (i = 0; i < zoom_dev->stream_num; i++) {
		zoom_dev->zoom[i].stream_fmt.fmt =
			zoom_dev->zoom[i].dt_stream_fmt.fmt;
		if (i == 0 && zoom_dev->zoom[i].zoom_in_enable) {
			zoom_dev->zoom[i].stream_fmt.width =
				zoom_dev->zoom[i].dt_stream_fmt.width;
			zoom_dev->zoom[i].stream_fmt.height =
				zoom_dev->zoom[i].dt_stream_fmt.height;
		} else {
			zoom_dev->zoom[i].stream_fmt.width =
				MIN(zoom_dev->zoom[i].dt_stream_fmt.width,
					pipeline->info.width);
			zoom_dev->zoom[i].stream_fmt.height =
				MIN(zoom_dev->zoom[i].dt_stream_fmt.height,
					pipeline->info.height);
		}
		zoom_dev->zoom[i].crop_info.start_x = 0;
		zoom_dev->zoom[i].crop_info.start_y = 0;
		zoom_dev->zoom[i].crop_info.width = pipeline->info.width;
		zoom_dev->zoom[i].crop_info.height = pipeline->info.height;
	}

	force_reset = pipeline->large_image_mode ? 1 : 0;

	ret = rtscam_soc_event(RTSCAM_ZOOM_EVT_STREAM_INFO_CHANGED, &force_reset);
	if (ret) {
		return ret;
	}
	ret = rtscam_soc_event(RTSCAM_ISP_EVT_FPS_CHANGED, &force_reset);
	return ret;
}

static int rtscam_isp_set_frame_info(struct rtscam_isp_pipeline *pipeline,
									 void *args)
{
	int ret;
	struct rtscam_isp_frame_info backup;
	const struct rtscam_isp_frame_info *info = args;

	if (!info->width && !info->height) {
		isp_rtos_atomic_set(&pipeline->active, 0);
		return 0;
	}
	backup = pipeline->info;
	pipeline->info = *info;

	ret = rtscam_isp_refresh_zoom_info(pipeline);
	if (ret) {
		pipeline->info = backup;
		rtscam_isp_refresh_zoom_info(pipeline);
		return ret;
	}
	isp_rtos_atomic_set(&pipeline->active, 1);
	return 0;
}

int rtscam_isp_need_resume(uint32_t isp_id)
{
	if (!m_pipeline[isp_id]) {
		return 0;
	}
	return m_pipeline[isp_id]->ispd_need_resume;
}

static int
rtscam_isp_query_and_clear_need_resume(struct rtscam_isp_pipeline *pipeline,
									   void *args)
{
	*(int32_t *)args = pipeline->ispd_need_resume;
	if (pipeline->ispd_need_resume) {
		pipeline->ispd_need_resume = 0;
	}
	return 0;
}

static int rtscam_isp_request_process(struct rtscam_isp_pipeline *pipeline,
									  int cmd, void *data)
{
	int ret;

	isp_rtos_mutex_lock(&pipeline->lock);

	switch (cmd) {
	case RTSISP_IOC_PIPE_SET_FRAME_INFO:
		isp_rtos_mutex_unlock(&pipeline->lock);
		ret = rtscam_isp_set_frame_info(pipeline, data);
		isp_rtos_mutex_lock(&pipeline->lock);
		break;
	case RTSISP_IOC_PIPE_NOTIFY_DYN_FPS:
		isp_rtos_mutex_unlock(&pipeline->lock);
		// TODO: notify zoom layer — rtscam_soc_event(RTSCAM_ZOOM_EVT_FPS_DYNAMIC_CHANGED, data)
		ret = 0;
		isp_rtos_mutex_lock(&pipeline->lock);
		break;
	case RTSISP_IOC_PIPE_NOTIFY_COLOR_RANGE:
		isp_rtos_mutex_unlock(&pipeline->lock);
		// TODO: notify zoom layer — rtscam_zoom_set_color_range(...)
		ret = 0;
		isp_rtos_mutex_lock(&pipeline->lock);
		break;
	case RTSISP_IOC_PIPE_IS_FPGA:
		ret = rtscam_isp_is_fpga(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_GET_IC_CUT:
		ret = rtscam_isp_get_ic_cut(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_GET_TNR_BIT:
		ret = rtscam_isp_get_tnr_bit(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_I2C_READ_REG:
		ret = rtscam_isp_read_i2c_reg(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_I2C_WRITE_REG:
		ret = rtscam_isp_write_i2c_reg(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_I2C_TRANSTER:
		ret = rtscam_isp_i2c_process(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_SET_POWER:
		ret = rtscam_isp_power_set(&pipeline->power, data);
		break;
	case RTSISP_IOC_PIPE_SEL_PIN_STATE:
		ret = rtscam_isp_sel_pin_state(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_SET_CLOCK:
		ret = rtscam_isp_set_clock(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_GET_STATIS_CFG:
		ret = rtscam_isp_get_statis_cfg(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_TEST_PREVIEW:
		ret = rtscam_isp_test_preview(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_PM_ENABLE:
		ret = rtscam_isp_pm_enable_inner(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_SET_LARGE_IMAGE_MODE:
		ret = rtscam_isp_set_large_image_mode(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_GET_BUS_ADDR:
		// FIXME: this api don't need for vender
		ret = rtscam_isp_get_bus_addr(pipeline, data);
		break;
	case RTSISP_IOC_PIPE_NEED_RESUME:
		ret = rtscam_isp_query_and_clear_need_resume(pipeline, data);
		break;
	default:
		ret = -ENOTTY;
		break;
	}
	isp_rtos_mutex_unlock(&pipeline->lock);

	return ret;
}

static int rtscam_isp_rt_request_process(struct rtscam_isp_pipeline *pipeline,
		int cmd, void *data)
{
	int ret = 0;

	switch (cmd) {
	case RTSISP_IOC_PIPE_I2C_SYNC_WRITE:
		ret = rtscam_isp_i2c_write_sync(&pipeline->i2c, data);
		break;
	default:
		ret = -ENOTTY;
		break;
	}
	return ret;
}

int rtscam_isp_request(int id, int cmd, void *data, uint32_t size,
					   bool realtime)
{
	if (!m_pipeline[id]) {
		return -EINVAL;
	}

	if (realtime) {
		return rtscam_isp_rt_request_process(m_pipeline[id], cmd, data);
	} else {
		return rtscam_isp_request_process(m_pipeline[id], cmd, data);
	}
}

static int rtscam_isp_parse(struct rtscam_isp_pipeline *pipeline)
{
	if (!pipeline) {
		return -EINVAL;
	}

	pipeline->is_fpga = 0; // FIXME: is fpga or not
	pipeline->ic_cut = 0; // FIXME: read actual cut version
	pipeline->statis_cfg.base = 0x200;
	pipeline->statis_cfg.size = 0x200;
	pipeline->has_pmu = false;
	pipeline->tnr_bit = 8;
	/* bus_addr not needed on AmebaPro3 VP — isp_statis.c computes phy_addr directly.
	 * SYS_VIDEO_CPU_SHMEM0_START (0x5886E00C) is not accessible from the VP. */
	pipeline->bus_addr = 0;
	return 0;
}

static int rtscam_isp_init_pinctrl(struct rtscam_isp_pipeline *pipeline)
{
	pipeline->pins.default_state =
		rtscam_rtos_pinctrl_lookup_state("default");
	pipeline->pins.dvp_state = rtscam_rtos_pinctrl_lookup_state("dvp");
	pipeline->pins.mipi_state = rtscam_rtos_pinctrl_lookup_state("mipi");
	if (!pipeline->pins.default_state || !pipeline->pins.dvp_state ||
		!pipeline->pins.mipi_state) {
		return -EINVAL;
	}

	return 0;
}

static int rtscam_isp_init_resource(struct rtscam_isp_pipeline *pipeline)
{
	int ret;

	ret = rtscam_isp_cru_init(&pipeline->cru, !pipeline->is_fpga);
	if (ret) {
		return ret;
	}
	rtscam_isp_cru_reset_device(&pipeline->cru);
	rtscam_isp_cru_deassert_device(&pipeline->cru);
	ret = rtscam_isp_init_pinctrl(pipeline);
	if (ret) {
		return ret;
	}
	return rtscam_isp_i2c_init(&pipeline->i2c);
}

static void rtscam_isp_cleanup_resource(struct rtscam_isp_pipeline *pipeline)
{
	rtscam_isp_i2c_cleanup(&pipeline->i2c);
	rtscam_isp_cru_assert_device(&pipeline->cru);
	rtscam_isp_cru_reset_device(&pipeline->cru);
	rtscam_isp_cru_cleanup(&pipeline->cru);
}

static void rtscam_isp_cleanup_status(struct rtscam_isp_pipeline *pipeline);

static int rtscam_isp_init_status(struct rtscam_isp_pipeline *pipeline)
{
	int ret;

	ret = rtscam_isp_power_init(&pipeline->power, pipeline->has_pmu);
	if (ret) {
		rtscam_isp_cleanup_status(pipeline);
	}
	return ret;
}

static void rtscam_isp_cleanup_status(struct rtscam_isp_pipeline *pipeline)
{
	rtscam_isp_power_cleanup(&pipeline->power);
}

static void rtscam_isp_cleanup(struct rtscam_isp_pipeline *pipeline)
{
	rtscam_isp_cleanup_status(pipeline);
	rtscam_isp_cleanup_resource(pipeline);
}

static int rtscam_isp_init(struct rtscam_isp_pipeline *pipeline)
{
	int ret;

	pipeline->id = RTSCAM_ISPC_PIPELINE_0;
	ret = rtscam_isp_parse(pipeline);
	if (ret) {
		goto err;
	}
	ret = rtscam_isp_init_resource(pipeline);
	if (ret) {
		goto err;
	}
	ret = rtscam_isp_init_status(pipeline);
	if (ret) {
		goto err;
	}

	/* Set safe defaults so rtscam_soc_attach() can run without divide-by-zero.
	 * Real sensor fps/dims arrive later via RTSISP_IOC_PIPE_SET_FRAME_INFO. */
	pipeline->info.fps_max.numerator   = 1;
	pipeline->info.fps_max.denominator = 30;
	pipeline->info.fps_min.numerator   = 1;
	pipeline->info.fps_min.denominator = 1;
	pipeline->info.width  = 2688;   /* os04a10 sensor width — updated via SET_FRAME_INFO */
	pipeline->info.height = 1520;   /* os04a10 sensor height */

	return 0;
err:
	rtscam_isp_cleanup(pipeline);
	return ret;
}

int rtscam_isp_probe(void)
{
	int ret;
	struct rtscam_isp_pipeline *pipeline;

	pipeline = pvPortMalloc(sizeof(struct rtscam_isp_pipeline));
	if (!pipeline) {
		return -ENOMEM;
	}
	memset(pipeline, 0, sizeof(struct rtscam_isp_pipeline));

	ret = rtscam_isp_init(pipeline);
	if (ret) {
		return ret;
	}
	ret = rtscam_isp_mcu_init();
	if (ret) {
		goto isp_mcu_init_err;
	}

	m_pipeline[0] = pipeline;

	isp_rtos_mutex_init(&pipeline->lock);

	return 0;

isp_mcu_init_err:
	rtscam_isp_cleanup(pipeline);
	vPortFree(m_pipeline[0]);
	m_pipeline[0] = NULL;
	return ret;
}

int rtscam_isp_remove(void)
{
	rtscam_isp_mcu_cleanup();
	rtscam_isp_cleanup(m_pipeline[0]);
	vPortFree(m_pipeline[0]);
	m_pipeline[0] = NULL;
	return 0;
}

/* pipeline API for zoom layer */

int rtscam_isp_zoom_set_stream_on(struct rtscam_isp_pipeline *pipeline)
{
	int ret = 0;

	if (!pipeline) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&pipeline->lock);
	if (pipeline->stream_refcnt) {
		pipeline->isp_need_keep_streaming = 0;
	} else {
		ret = __rtscam_isp_set_fps(pipeline, &pipeline->soc_set_fps);
	}
	pipeline->stream_refcnt++;
	isp_rtos_mutex_unlock(&pipeline->lock);
	return ret;
}

int rtscam_isp_zoom_set_stream_off(struct rtscam_isp_pipeline *pipeline)
{
	struct v4l2_fract fps = { 1, 0 };

	if (!pipeline) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&pipeline->lock);
	pipeline->stream_refcnt--;
	if (!pipeline->stream_refcnt) {
		if (rtscam_isp_mcu_get_aov_mode()) {
			pipeline->isp_need_keep_streaming = 1;
		}
		__rtscam_isp_set_fps(pipeline, &fps);
	}
	isp_rtos_mutex_unlock(&pipeline->lock);
	return 0;
}

int rtscam_isp_zoom_get_aov_mode(void)
{
	return rtscam_isp_mcu_get_aov_mode();
}

/* pipeline API for soc layer */

int rtscam_isp_soc_set_fps(struct rtscam_isp_pipeline *pipeline,
						   struct v4l2_fract fps)
{
	int ret = 0;

	if (!pipeline) {
		return -EINVAL;
	}
	if (isp_rtos_atomic_read(&pipeline->active) == 0) {
		return -EHOSTDOWN;
	}

	isp_rtos_mutex_lock(&pipeline->lock);
	if (pipeline->stream_refcnt) {
		ret = __rtscam_isp_set_fps(pipeline, &fps);
	} else
		memcpy(&pipeline->soc_set_fps, &fps,
			   sizeof(pipeline->soc_set_fps));
	isp_rtos_mutex_unlock(&pipeline->lock);
	return ret;
}

int rtscam_isp_soc_get_fps(struct rtscam_isp_pipeline *pipeline,
						   struct v4l2_fract *fps_min,
						   struct v4l2_fract *fps_max)
{
	if (!pipeline) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&pipeline->lock);
	memcpy(fps_min, &pipeline->info.fps_min, sizeof(struct v4l2_fract));
	memcpy(fps_max, &pipeline->info.fps_max, sizeof(struct v4l2_fract));
	isp_rtos_mutex_unlock(&pipeline->lock);
	return 0;
}

void rtscam_isp_resume(struct rtscam_isp_pipeline *pipeline)
{
	pipeline->ispd_need_resume = 1;
	rtscam_isp_mcu_resume();
	rtscam_isp_cru_reset_device(&pipeline->cru);
	rtscam_isp_cru_clk_enable(&pipeline->cru);
}

void rtscam_isp_suspend(struct rtscam_isp_pipeline *pipeline)
{
	rtscam_isp_mcu_suspend();
}
