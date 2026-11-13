/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2026 Grant Shen <grant_shen@realsil.com.cn>
 */

//#include <unistd.h>
#include "isp_main.h"
#include "isp_io_type.h"
#include <FreeRTOS.h>
#include <stream_buffer.h>

#if !RTOS_TASK
#include "consumer_thread.h"
#include "producer_thread_ISP.h"
#else
void consume_buffer(void *arg);
void producer_ISP(void *arg);
#endif
//#define DUMP_BAYER

int g_exit;
void signal_handler(int signo)
{
	g_exit = 1;
}


#define BUFFER_SIZE 1024
#define TRIGGER_LEVEL 1
#if FIXP_STREAM
StreamBufferHandle_t xStreamBuffer_consumer[stream_count];
StreamBufferHandle_t xStreamBuffer_producer[stream_count];

int create_pipe_rtos()
{
	for (int i = 0; i < stream_count; i++) {
		xStreamBuffer_consumer[i] = xStreamBufferCreate(BUFFER_SIZE, TRIGGER_LEVEL);
		xStreamBuffer_producer[i] = xStreamBufferCreate(BUFFER_SIZE, TRIGGER_LEVEL);
	}
	return RTS_OK;
}

void close_pipe_rtos()
{
	for (int i = 0; i < stream_count; i++) {
		vStreamBufferDelete(xStreamBuffer_consumer[i]);
		vStreamBufferDelete(xStreamBuffer_producer[i]);
	}
}
#else
struct isp_pipe pipe_isp[stream_count];
#define QUEUE_BUFFER_CNT 10
#define ITEM_SIZE sizeof(int)

int create_pipe()
{
	for (int i = 0; i < stream_count; ++i) {
		pipe_isp[i].producerQueue =
			xQueueCreate(QUEUE_BUFFER_CNT, ITEM_SIZE);
		pipe_isp[i].consumerQueue =
			xQueueCreate(QUEUE_BUFFER_CNT, ITEM_SIZE);

		if (pipe_isp[i].producerQueue == NULL ||
			pipe_isp[i].consumerQueue == NULL) {
			RTS_ERR("Failed to create queues for pipe %d\n", i);
			return RTS_RETURN(RTS_FAIL);
		}
	}
	return RTS_OK;
}

void close_pipe()
{
	for (int i = 0; i < stream_count; ++i) {
		vQueueDelete(pipe_isp[i].producerQueue);
		vQueueDelete(pipe_isp[i].consumerQueue);
	}
}
#endif

void zoom_init(struct zoom_config *zoom, int stream_index)
{
	if (stream_index < 0 || stream_index > stream_count) {
		isperro_printf("out of stream range. \r\n");
	}

#if FIXP_STREAM
	int zoom_w[5] = {2560, 1280, 2560, 640, 640};
	int zoom_h[5] = {1440, 720, 1440, 360, 480};
	//int zoom_w[5] = {768, 640, 640, 640, 576};
	//int zoom_h[5] = {432, 360, 360, 360, 320};
#else
	int zoom_w[5] = {1920, 320, 320, 320, 576};
	int zoom_h[5] = {1080, 180, 180, 180, 320};
#endif

#ifdef DUMP_BAYER
	if (stream_index == 0) {
		zoom->enable = 0;
	}
	if (stream_index == 4) {
		zoom->out_fmt = FMT_RGB;
	} else {
		zoom->out_fmt = FMT_NV16;
	}
#else
	zoom->enable = 1;
	if (stream_index == 4) {
		zoom->out_fmt = FMT_RGB;
	} else {
		zoom->out_fmt = FMT_NV12;
	}
#endif
	zoom->left = 0;
	zoom->top = 0;
	zoom->right  = zoom_w[stream_index];
	zoom->bottom = zoom_h[stream_index];
	zoom->width  = zoom_w[stream_index];
	zoom->height = zoom_h[stream_index];
}



#if FIXP_STREAM

static int isp_preview_task(void)
{
	int ret;
	struct isp_io io;
	static struct zoom_config zoom[stream_count];
	memset(zoom, 0, sizeof(zoom));

	/* create pipe*/
	ret = create_pipe_rtos();

	if (ret) {
		goto out_pipe;
	}

#ifdef DUMP_BAYER // fixp
#include "rts_isp_ctrl.h"
	struct isp_v4l2_control c;

	c.id = ISP_V4L2_CID_OUTPUT_POSITION;
	c.value = 3;
	ret = rts_isp_v4l2_s_ctrl(0, &c);
#endif
	ret = isp_init_multi(&io);
	if (ret) {
		isperro_printf("init fail, ret = %d", ret);
		goto out;
	}

	for (int i = 0; i < stream_count; i++) {
		zoom_init(&zoom[i], i);
	}

	isp_printf("start buffer_streaming\n");
	ret = isp_streaming(&io, 2560, 1440, ISP_OUTPUT_CROP_OUT_L, zoom, NULL);
	if (ret) {
		if (ret != RTS_EXIT) {
			isperro_printf("buffer streaming fail, ret = %d\n", ret);
		}
		g_exit = 1;
	}

#if !CONSUMER_BYPASS
	xTaskCreate(consume_buffer, "consume_buffer", configMINIMAL_STACK_SIZE * 16, &io,
				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
	xTaskCreate(producer_ISP, "producer_ISP", configMINIMAL_STACK_SIZE * 16, &io,
				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
#endif
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(500));
	}


out:
	isp_stop(&io);
	isp_release_multi(&io);
out_pipe:
	close_pipe_rtos();
	isp_printf("exit buffer_streaming\n");
	exit(-ret);
}

#else // real sensor



static int test_preview(void)
{
	int ret;
	struct isp_io io;
	static struct zoom_config zoom[stream_count];
	memset(zoom, 0, sizeof(zoom));

	/* create pipe*/
	ret = create_pipe();

	if (ret) {
		goto out_pipe;
	}

#ifdef DUMP_BAYER // real sensor
#include "rts_isp_ctrl.h"
	struct isp_v4l2_control c;

	c.id = ISP_V4L2_CID_OUTPUT_POSITION;
	c.value = 3;
	ret = rts_isp_v4l2_s_ctrl(0, &c);
#endif
	ret = isp_init_multi(&io);
	if (ret) {
		isperro_printf("init fail, ret = %d", ret);
		goto out;
	}

	for (int i = 0; i < stream_count; i++) {
		zoom_init(&zoom[i], i);
	}

	isp_printf("start buffer_streaming\n");
	ret = isp_streaming(&io, 2688, 1520, ISP_OUTPUT_YUYV, zoom, NULL);
	if (ret) {
		if (ret != RTS_EXIT) {
			isperro_printf("buffer streaming fail, ret = %d\n", ret);
		}
		g_exit = 1;
	}

#if !CONSUMER_BYPASS
	xTaskCreate(consume_buffer, "consume_buffer", configMINIMAL_STACK_SIZE * 16, &io,
				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
	xTaskCreate(producer_ISP, "producer_ISP", configMINIMAL_STACK_SIZE * 16, &io,
				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);
#endif
	while (1) {
	}


out:
	isp_stop(&io);
	isp_release_multi(&io);
out_pipe:
	close_pipe();
	isp_printf("exit buffer_streaming\n");
	exit(-ret);
}

static void isp_preview_task(void *pvParameters)
{
	int ret;

	ret = test_preview();
	if (ret) {
		vTaskDelete(NULL);
	}
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
#if VHDR
#include "rts_isp_define.h"
void set_hdr()
{
	struct rts_isp_sensor_modes modes;
	uint32_t isp_id = 0;

	rts_av_isp_enum_sensor_modes(isp_id, &modes);
	for (int i = 0; i < 8; i++) {
		ispdbeg_printf("hdr:%d  size.w:%d  size.h:%d  fps:%d   \r\n", modes.mode[i].hdr, modes.mode[i].size.w, modes.mode[i].size.h, (int)modes.mode[i].fps);
	}

	rts_av_isp_set_sensor_mode(isp_id, &modes.mode[1]);
}
#endif
#endif
static struct stream_verify_mode vr_mode = {0};
struct stream_verify_mode *stream_verify_get_mode(void)
{
	return &vr_mode;
}
void main_multistream(int *arg)
{
#if !FIXP_STREAM
#if VHDR
	set_hdr();
#endif
#endif
	vr_mode.mode = arg[0];
	xTaskCreate(isp_preview_task, "isp_preview",
				configMINIMAL_STACK_SIZE * 16, NULL,
				(portPRIVILEGE_BIT | ((configMAX_PRIORITIES / 2) - 2)),
				NULL);
}
