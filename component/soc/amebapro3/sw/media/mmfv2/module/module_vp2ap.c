/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "mmf2_module.h"
#include "module_vp2ap.h"
#include "avcodec.h"
#include "ameba_soc.h"
#include "ipc_video.h"

/*-----------------------------------------------------------------------------------*/

#define VP2AP_SAVE_HEVC     0
#define VP2AP_SAVE_H264     0
#define VP2AP_SAVE_JPEG     0
#define VP2AP_SHOW_FPS      0

#if VP2AP_SHOW_FPS
static uint32_t current_fps[5] = {0};
static uint32_t ts = 0;
#endif
static int frame_cnt[5] = {0};
static SemaphoreHandle_t file_lock = NULL;

void vp2ap_save_file(void *buf, uint32_t size, char *fn);
static void vp2ap_send_vframe_to_ap_process(int ch, int type, void *buf, uint32_t size, uint32_t timestamp);

int vp2ap_handle(void *p, void *input, void *output)
{
	(void)output;
	vp2ap_ctx_t *ctx = (vp2ap_ctx_t *)p;

	mm_queue_item_t *input_item = (mm_queue_item_t *)input;
	int ch = ctx->params.stream_id;
	frame_cnt[ch]++;

	//printf("vp2ap module get data[ch:%d][cnt:%d], addr:0x%x size:%ld\r\n", ch, frame_cnt[ch], input_item->data_addr, input_item->size);
#if VP2AP_SAVE_HEVC
	char fn_hevc[128] = {0};
	if (input_item->type == AV_CODEC_ID_H265) {
		snprintf(fn_hevc, sizeof(fn_hevc), "D:\\hevc\\test_hevc_ch%d_%d.hevc", ch, frame_cnt[ch]);
		vp2ap_save_file((void *)input_item->data_addr, input_item->size, fn_hevc);
	}
#endif
#if VP2AP_SAVE_H264
	char fn_h264[128] = {0};
	if (input_item->type == AV_CODEC_ID_H264) {
		snprintf(fn_h264, sizeof(fn_h264), "D:\\h264\\test_h264_ch%d_%d.h264", ch, frame_cnt[ch]);
		vp2ap_save_file((void *)input_item->data_addr, input_item->size, fn_h264);
	}
#endif
#if VP2AP_SAVE_JPEG
	char fn_jpeg[128] = {0};
	if (input_item->type == AV_CODEC_ID_JPEG) {
		snprintf(fn_jpeg, sizeof(fn_jpeg), "D:\\jpeg\\test_jpeg_ch%d_%d.jpg", ch, frame_cnt[ch]);
		vp2ap_save_file((void *)input_item->data_addr, input_item->size, fn_jpeg);
	}
#endif

	/* show fps */
#if VP2AP_SHOW_FPS
	if (ts == 0) {
		ts = xTaskGetTickCount();
	}
	current_fps[ch]++;
	if ((xTaskGetTickCount() - ts) > 1000) {
		printf("vp2ap fps=%lu, %lu, %lu, %lu, %lu\r\n", current_fps[0], current_fps[1]
			   , current_fps[2], current_fps[3], current_fps[4]);
		memset(current_fps, 0, sizeof(current_fps));
		ts = xTaskGetTickCount();
	}
#endif

	/* send to AP and wait process done */
	if (ctx->params.send_ap_off == 0) {
		vp2ap_send_vframe_to_ap_process(ch, input_item->type, (void *)input_item->data_addr, input_item->size, input_item->timestamp);
	}

	return 0;
}

void vp2ap_save_file(void *buf, uint32_t size, char *fn)
{
	xSemaphoreTake(file_lock, portMAX_DELAY);
	DCache_Invalidate((u32)buf, size);
	FILE *fp = fopen(fn, "w+");
	fwrite(buf, size, 1, fp);
	fclose(fp);
	xSemaphoreGive(file_lock);
}

/* Send data to AP via IPC */
static ALIGNMTO(CA_CACHE_LINE_SIZE) vp_frame_info_t frame_info[5] = {0};
static void vp2ap_send_vframe_to_ap_process(int ch, int type, void *buf, uint32_t size, uint32_t timestamp)
{
	vp_frame_info_t *frm_info = &frame_info[ch];
	frm_info->ch = ch;
	frm_info->codec = type;
	frm_info->time_stamp = timestamp;
	frm_info->cmd_status = 0; //OK;
	frm_info->enc_addr = (uint32_t)buf;
	frm_info->enc_len = size;
	DCache_Clean((u32)frm_info, sizeof(vp_frame_info_t));
	/* VP no need to do dcache clean for encoded frame data because data in ddr is already newest */
	/* AP will need to do dcache invalidate before read the frame data */
	// DCache_Clean((u32)frm_info->enc_addr, frm_info->enc_len);

#if defined(CONFIG_PXP) && (CONFIG_PXP==1)
	//do something
#else
	vp_ipc_send_frame(ch, frm_info, 50);
#endif

	/* may need to wait AP finish frame process */
	//printf(">>wait ch%d buf process...\r\n", ch);
	vp_ipc_wait_frame_recycled(ch, 1000);
	//printf(">>ch%d buf process done\r\n", ch);
}

/*-----------------------------------------------------------------------------------*/

int vp2ap_control(void *p, int cmd, int arg)
{
	vp2ap_ctx_t *ctx = (vp2ap_ctx_t *)p;

	switch (cmd) {
	case CMD_VP2AP_SET_PARAMS:
		memcpy(&ctx->params, (void *)arg, sizeof(vp2ap_params_t));
		break;
	case CMD_VP2AP_GET_PARAMS:
		memcpy((void *)arg, &ctx->params, sizeof(vp2ap_params_t));
		break;
	case CMD_VP2AP_APPLY:
		if (file_lock == NULL) {
			file_lock = xSemaphoreCreateMutex();
		}
		break;
	}
	return 0;
}

void *vp2ap_destroy(void *p)
{
	vp2ap_ctx_t *ctx = (vp2ap_ctx_t *)p;

	if (ctx) {
		free(ctx);
	}

	return NULL;
}

void *vp2ap_create(void *parent)
{
	vp2ap_ctx_t *ctx = malloc(sizeof(vp2ap_ctx_t));
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(vp2ap_ctx_t));

	ctx->parent = parent;

	return ctx;
}

mm_module_t vp2ap_module = {
	.create = vp2ap_create,
	.destroy = vp2ap_destroy,
	.control = vp2ap_control,
	.handle = vp2ap_handle,

	.new_item = NULL,
	.del_item = NULL,

	.output_type = MM_TYPE_NONE,
	.module_type = MM_TYPE_AVSINK,
	.name = "VP2AP"
};
