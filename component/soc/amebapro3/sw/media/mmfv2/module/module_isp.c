/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <stream_buffer.h>
#include "os_wrapper.h"
#include "ameba_soc.h"

#include "mmf2.h"
#include "mmf2_dbg.h"
#include "mmf2_mediatime_pro3.h"

#include "module_isp.h"
#include "avcodec.h"
#include "video_common_type.h"
#ifdef CONFIG_VMODULE_ADDREMAP
#include "addremap_hal.h"
#include "slice_sram.h"
#include "isp_sensor.h"
#endif
#include "ainr_driver.h"
#include "hal_isp.h"
#include "isp_stream.h"
#include "rts_isp_sensor.h"
#include "ameba_sd.h"
#include "ameba_sdioh.h"
#include "vfs.h"

#define ISP_ADDR_REMAP_ENA  1
#define DBG_REMAP_REG_DUMP  1

typedef struct isp_out_s {
	uint32_t y_addr;
	uint32_t uv_addr;
	uint32_t rgb_addr;
	uint32_t buf_id;
	uint32_t irq_ts;
	uint8_t ch;
} isp_out_t;

typedef struct isp_ch_info_s {
	void (*cb_func)(isp_out_t *isp_item, void *arg);
	void *cb_arg;
} isp_ch_info_t;

typedef struct isp_remap_info_s {
#ifdef CONFIG_VMODULE_ADDREMAP
	struct isp_remap_config cfg;
#endif
	uint32_t align_width;
	uint32_t slice_out_addr0;
	uint32_t slice_out_addr1;
	uint32_t low_latency_set;
	uint32_t cmpr_fr_mode;
	uint32_t all_vsram;
	uint32_t slice_buff_size;
} isp_remap_info_t;

#ifdef CONFIG_VMODULE_ADDREMAP
typedef struct enc_remap_info_s {
	struct enc_remap_config cfg;
} enc_remap_info_t;
#endif

typedef struct isp_sensor_info_s {
	uint32_t width;
	uint32_t height;
} isp_sensor_info_t;

typedef enum {
	ISP_AINR_CMD_ENABLE,
	ISP_AINR_CMD_DISABLE
} isp_ainr_cmd_t;

typedef struct isp_global_s {
	isp_ch_info_t ch_info[VIDEO_ISP_VALID_CH];
	rtos_task_t output_task_handle;
	rtos_task_t comp_header_update_task_handle;
	rtos_queue_t data_start_notify;
	rtos_queue_t out_queue;
	int module_cnt;
	struct isp_io io;
	int ai_denoise_en;
	int ainr_model_loaded;
	uint32_t cmpr_header_y[2];
	uint32_t cmpr_header_uv[2];
	isp_remap_param_t *remap_param;
	isp_remap_info_t remap_info;
#ifdef CONFIG_VMODULE_ADDREMAP
	enc_remap_info_t enc_remap_info;
#endif
	isp_ctx_t *ctx[VIDEO_ISP_VALID_CH];  //TODO: should be modified after isp control sw being ready
	void *cmpr_header_ddr;
	ainr_ctx *ainr;
	rtos_queue_t ainr_cmd_queue;
	isp_sensor_info_t sensor;
	uint32_t irq_frm_cnt[VIDEO_ISP_VALID_CH];
	uint32_t irq_frm_drop_cnt[VIDEO_ISP_VALID_CH];
	uint32_t irq_frm_stat_ts;
	uint32_t active_stream_mask;
	int driver_opened;
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	int stage1_attached;
	/* Bit set for a channel that is currently running on a not-yet-consumed
	 * stage1 hand-off. Cleared the moment that channel is genuinely stream-off'd,
	 * so a later close/reopen of the SAME channel — or any OTHER channel — is
	 * governed purely by the normal stage2 active_stream_mask logic, with no
	 * comparison against the (otherwise never-updated) stage1 params. */
	uint32_t stage1_live_mask;
#endif
#if defined(REALSENSOR) && (REALSENSOR==1)
	int ctrlsw_started;
#endif
} isp_global_t;

static isp_global_t g_isp;

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
extern int video_fcs_isp_stage1_is_started(void);
extern struct isp_io *video_fcs_isp_stage1_get_io(void);
extern int video_fcs_isp_stage1_get_stream_params(stream_params_t *param);

static int isp_stage1_stream_param_match(const stream_params_t *param)
{
	stream_params_t stage1_param;

	/* Only the channel still carrying an unconsumed stage1 hand-off is
	 * subject to comparison; every other channel behaves as plain stage2. */
	if ((uint32_t)param->ch >= VIDEO_ISP_VALID_CH ||
		!(g_isp.stage1_live_mask & BIT(param->ch))) {
		return 1;
	}

	if (video_fcs_isp_stage1_get_stream_params(&stage1_param) != 0) {
		return 1;
	}

	return stage1_param.ch == param->ch &&
		   stage1_param.width == param->width &&
		   stage1_param.height == param->height &&
		   stage1_param.fps == param->fps &&
		   stage1_param.slot_num == param->slot_num &&
		   stage1_param.format == param->format;
}

static void isp_stage1_stream_mark_active(void)
{
	stream_params_t stage1_param;

	if (video_fcs_isp_stage1_get_stream_params(&stage1_param) != 0) {
		return;
	}

	if ((uint32_t)stage1_param.ch < VIDEO_ISP_VALID_CH) {
		g_isp.active_stream_mask |= BIT(stage1_param.ch);
		g_isp.stage1_live_mask |= BIT(stage1_param.ch);
	}
}
#endif /* CONFIG_FAST_CAMERA_START */

static void isp_set_stream_active(int ch, int active)
{
	if ((uint32_t)ch >= VIDEO_ISP_VALID_CH) {
		return;
	}

	if (active) {
		g_isp.active_stream_mask |= BIT(ch);
	} else {
		g_isp.active_stream_mask &= ~BIT(ch);
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
		/* Real stream-off consumes any pending stage1 hand-off for this
		 * channel — from here on it is a normal stage2-owned channel. */
		g_isp.stage1_live_mask &= ~BIT(ch);
#endif
	}
}

static int isp_any_stream_active(void)
{
	return g_isp.active_stream_mask != 0;
}

#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
static int isp_address_remap_pre_setup(isp_ctx_t *ctx);
static int isp_address_remap_post_setup(isp_ctx_t *ctx);
#endif
#define ALIGN_UP(addr, align) \
    (((uintptr_t)(addr) + ((align) - 1)) & ~((uintptr_t)((align) - 1)))

#define RAW_NUM 0
#if defined(RAW_NUM) && (RAW_NUM > 0)
static unsigned long raw_frame_cnt = 0;
static bool sd_init_done = false;
#endif

static void isp_frame_complete_cb(isp_out_t *isp_item, void *arg)
{
	isp_ctx_t *ctx = (isp_ctx_t *)arg;
	isp_params_t *params = &ctx->params;
	mm_context_t *mctx = (mm_context_t *)ctx->parent;
	mm_queue_item_t *output_item;

	//printf("[MMF] IN stream[%d] cb\r\n", params->stream_id);

	/* use the timestamp captured at the isp irq/trigger */
	uint32_t timestamp = isp_item->irq_ts;

	int is_output_ready = 0;
	int isp_overflow_flag = 0; //TODO, check overflow ?
	if (isp_overflow_flag == 0) {
		is_output_ready = (rtos_queue_receive(mctx->output_recycle, &output_item, portMAX_DELAY) == SUCCESS);
	} else {
		isp_overflow_flag = 0;
		ISP_DBG_WARNING("isp overflow = %d\r\n", params->stream_id);
	}

	if (is_output_ready) {

		output_item->timestamp = timestamp;
		output_item->hw_timestamp = timestamp;

		if (params->format == ISP_FORMAT_YUV420_SEMIPLANAR) {
			output_item->isp.y_addr = isp_item->y_addr;
			output_item->isp.uv_addr = isp_item->uv_addr;
			output_item->size = (int)(params->width * params->height * 1.5);
			output_item->type = AV_CODEC_ID_NV12;
			ISP_DBG_DEBUG("[MMF] ISP[%d] y addr:0x%x, uv addr:0x%x, size:%ld\r\n", params->stream_id, output_item->isp.y_addr, output_item->isp.uv_addr, output_item->size);
		} else if (params->format == ISP_FORMAT_RGB888_PLANAR) {
			output_item->data_addr = isp_item->rgb_addr;
			output_item->size = params->width * params->height * 3;
			output_item->type = AV_CODEC_ID_RGB888;
			ISP_DBG_DEBUG("[MMF] ISP[%d] rgb addr:0x%x, size:%ld\r\n", params->stream_id, output_item->data_addr, output_item->size);
		} else {
			output_item->isp.y_addr = isp_item->y_addr;
			output_item->isp.uv_addr = isp_item->uv_addr;
			output_item->size = ISP_GetRAWBufSize();
			output_item->type = AV_CODEC_ID_NV16;
			ISP_DBG_DEBUG("[MMF] ISP[%d] y addr:0x%x, uv addr:0x%x, size:%ld\r\n", params->stream_id, output_item->isp.y_addr, output_item->isp.uv_addr, output_item->size);
		}

		rtos_queue_send(mctx->output_ready, (void *)&output_item, portMAX_DELAY);

	} else {
		//isp_handle_buffer(ctx->stream, NULL, MODE_SKIP);

	}
}

static void isp_output_task(void *p)
{
	(void)p;

	while (1) {
		if (g_isp.out_queue) {
			isp_out_t isp_out;
			if (rtos_queue_receive(g_isp.out_queue, &isp_out, 1000) != SUCCESS) {
				if (isp_any_stream_active()) {
					ISP_DBG_WARNING("warning: no isp frame in queue\r\n");
				}
				continue;
			}
			int ch = isp_out.ch;
			/* check if this frame should be processed by the current channel */
			isp_ch_info_t *info = &g_isp.ch_info[ch];
			if (info->cb_func) {
				info->cb_func(&isp_out, info->cb_arg);
			} else {
				/* skip and release */
				isp_buf_release(ch, (void *)NULL, isp_out.buf_id);
			}
		} else {
			vTaskDelay(50);
		}
	}
}

static int isp_send_trigger_slice(int isp_slice_ch)
{
	uint32_t y_addr = g_isp.remap_info.slice_out_addr0;
	uint32_t uv_addr = g_isp.remap_info.slice_out_addr1;
	isp_out_t isp_out = {0};
	isp_out.y_addr = y_addr;
	isp_out.uv_addr = uv_addr;
	isp_out.irq_ts = mm_read_mediatime_ms();
	isp_out.ch = isp_slice_ch;
	ISP_DBG_DEBUG("send slice frame y_addr:0x%08x uv_addr:0x%08x\r\n", isp_out.y_addr, isp_out.uv_addr);
	if (rtos_queue_send(g_isp.out_queue, &isp_out, 0) != SUCCESS) {
		/* should not be here, isp.out_queue is empty now */
		ISP_DBG_WARNING("warning: g_isp.out_queue full, frame dropped\r\n");
	}
	return 0;
}

static void isp_stream_set_complete_callback(void (*cb)(isp_out_t *isp_item, void *arg), isp_ctx_t *ctx)
{
	int ch = ctx->params.stream_id;
	g_isp.ch_info[ch].cb_func = cb;
	g_isp.ch_info[ch].cb_arg = (void *)ctx;
}

#if 0
static void isp_handle_buffer(isp_buf_t *buf, int mode)
{
	(void)buf;
	(void)mode;
	//TODO
}
#endif

static void isp_exchange_compress_header_write_buf(int buf_id)
{
	int curr_buf_id = buf_id;
	int next_buf_id = curr_buf_id == 1 ? 0 : 1;
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xA8, g_isp.cmpr_header_y[next_buf_id]);
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xAC, g_isp.cmpr_header_uv[next_buf_id]);
	ISP_DBG_DEBUG("SET next comp y header addr: 0x%x\r\n", g_isp.cmpr_header_y[next_buf_id]);
	ISP_DBG_DEBUG("SET next comp uv header addr: 0x%x\r\n", g_isp.cmpr_header_uv[next_buf_id]);
}

/* isp channel frame ready callback */
int isp_frame_ready_irq_callback(int stream_id, int buf_id, uint32_t phy_addr)
{
#if defined(RAW_NUM) && (RAW_NUM > 0)
	char path[130] = { 0 };
	FILE *file;
#endif

	g_isp.irq_frm_cnt[stream_id]++;

	if (g_isp.ctx[stream_id] && g_isp.ctx[stream_id]->params.slice_mode_en) {
		ISP_DBG_DEBUG("[ISP ch%d] slice mode slot:%d release\r\n", stream_id, buf_id);
		if ((mm_read_mediatime_ms() - g_isp.irq_frm_stat_ts) > 1000) {
			ISP_DBG_INFO("[VP ISP] fps[%d %d %d %d %d]\r\n", g_isp.irq_frm_cnt[0],
						 g_isp.irq_frm_cnt[1], g_isp.irq_frm_cnt[2], g_isp.irq_frm_cnt[3], g_isp.irq_frm_cnt[4]);
			memset(g_isp.irq_frm_cnt, 0, sizeof(g_isp.irq_frm_cnt));

			g_isp.irq_frm_stat_ts = mm_read_mediatime_ms();
		}
		isp_buf_release(stream_id, (void *)NULL, buf_id);
		return 0;
	}
	if (g_isp.out_queue) {
		ISP_DBG_DEBUG("[ISP IRQ ch%d id%d] isp_frame_ready_irq_callback\r\n", stream_id, buf_id);
		isp_out_t isp_out;
		if (stream_id < 4) {
			uint32_t luma_size = (uint32_t)g_isp.ctx[stream_id]->params.width *
								 (uint32_t)g_isp.ctx[stream_id]->params.height;
			uint32_t uv_size;
			if (g_isp.ctx[stream_id]->params.slice_mode_en) {
				luma_size = (uint32_t)g_isp.remap_info.align_width *
							(uint32_t)g_isp.ctx[stream_id]->params.height;
				luma_size = ALIGN_UP(luma_size, 4096);
				isp_out.y_addr  = phy_addr;
				isp_out.uv_addr = phy_addr + luma_size;
			} else if (g_isp.ctx[stream_id]->params.compress_en) {
				luma_size = (uint32_t)g_isp.remap_info.align_width *
							(uint32_t)g_isp.ctx[stream_id]->params.height;
				luma_size = ALIGN_UP(luma_size, 4096);
				uv_size = luma_size / 2;
				uv_size = ALIGN_UP(uv_size, 4096);
				isp_out.y_addr  = phy_addr;
				isp_out.uv_addr = phy_addr + (g_isp.ctx[stream_id]->params.slot_num - buf_id) * luma_size + buf_id * uv_size;
			} else {
				isp_out.y_addr  = phy_addr;
				isp_out.uv_addr = phy_addr + luma_size;
			}
		} else {
			isp_out.rgb_addr = phy_addr;
		}
		isp_out.buf_id = buf_id;
		isp_out.irq_ts = mm_read_mediatime_ms();
		isp_out.ch = stream_id;
#if defined(RAW_NUM) && (RAW_NUM > 0)
		if (ISP_GetRawIdx() > RTS_RAW_SRC_DISABLE && raw_frame_cnt < RAW_NUM) {
			snprintf(path, 128, "fat:ch%d_slot%d_w%d_h%d_rawidx%d_%d.raw", stream_id, buf_id, (int)g_isp.sensor.width, (int)g_isp.sensor.height, ISP_GetRawIdx(),
					 (int)raw_frame_cnt);
			file = fopen(path, "wb");
			if (file == NULL) {
				printf("save file open failed\n");
				return 0;
			}
			fseek(file, 0, SEEK_SET);

			fwrite((void *)isp_out.y_addr, g_isp.sensor.width * g_isp.sensor.height * 6, 1, file);

			fclose(file);
			printf("save file: %s\n\r", path);
			raw_frame_cnt++;
		}
#endif
		if (rtos_queue_send(g_isp.out_queue, &isp_out, 0) != SUCCESS) {
			ISP_DBG_DEBUG("warning: g_isp.out_queue full, [ch%d]frame dropped\r\n", stream_id);
			g_isp.irq_frm_drop_cnt[stream_id]++;
			/* skip and release */
			isp_buf_release(stream_id, (void *)NULL, buf_id);
		}
		if ((mm_read_mediatime_ms() - g_isp.irq_frm_stat_ts) > 1000) {
			ISP_DBG_DEBUG("[VP ISP] fps[%d %d %d %d %d]\r\n", g_isp.irq_frm_cnt[0],
						  g_isp.irq_frm_cnt[1], g_isp.irq_frm_cnt[2], g_isp.irq_frm_cnt[3], g_isp.irq_frm_cnt[4]);
			memset(g_isp.irq_frm_cnt, 0, sizeof(g_isp.irq_frm_cnt));

			for (int i = 0; i < VIDEO_ISP_VALID_CH; i++) {
				if (g_isp.irq_frm_drop_cnt[i] != 0) {
					ISP_DBG_WARNING("isp drop [%d %d %d %d %d] in past 1s\r\n", g_isp.irq_frm_drop_cnt[0],
									g_isp.irq_frm_drop_cnt[1], g_isp.irq_frm_drop_cnt[2], g_isp.irq_frm_drop_cnt[3], g_isp.irq_frm_drop_cnt[4]);
					memset(g_isp.irq_frm_drop_cnt, 0, sizeof(g_isp.irq_frm_drop_cnt));
					break;
				}
			}
			g_isp.irq_frm_stat_ts = mm_read_mediatime_ms();
		}
	}

	if (g_isp.ainr && g_isp.ainr_cmd_queue) {
		isp_ainr_cmd_t rcv_cmd;
		if (rtos_queue_receive(g_isp.ainr_cmd_queue, &rcv_cmd, 0) == RTK_SUCCESS) {
			if (rcv_cmd == ISP_AINR_CMD_ENABLE) {
				ISP_DBG_INFO("[ISR] Enabling AINR...\r\n");
				ainr_driver_ctrl(g_isp.ainr, AINR_CTRL_CMD_ENABLE, NULL);
			} else if (rcv_cmd == ISP_AINR_CMD_DISABLE) {
				ISP_DBG_INFO("[ISR] Disabling AINR...\r\n");
				ainr_driver_ctrl(g_isp.ainr, AINR_CTRL_CMD_DISABLE, NULL);
			}
		}
	}

	return 0;
}

static void comp_header_update_task(void *p)
{
	int comp_ch = (int)p;
	int buf_id;
	while (g_isp.data_start_notify) {
		rtos_queue_receive(g_isp.data_start_notify, &buf_id, portMAX_DELAY);
		vTaskDelay(5);

		/* update isp remap compress header register */
		if (g_isp.ctx[comp_ch]) {
			if (g_isp.ctx[comp_ch]->params.compress_en) {
				if (buf_id >= 2) {
					ISP_DBG_INFO("WARNING: buf id %d out of header slot !!\n\r", buf_id);
					goto error;
				}
				/* set the compress header address to next isp frame */
				isp_exchange_compress_header_write_buf(buf_id);
				/* update current compress header address to encoder module by callback fcuntion
				 * this callback function should be registered by application layer
				 */
				g_isp.remap_param->encDec400LumTableBase[buf_id] = g_isp.cmpr_header_y[buf_id];
				g_isp.remap_param->encDec400ChrTableBase[buf_id] = g_isp.cmpr_header_uv[buf_id];

				ISP_DBG_DEBUG("[ISP IRQ ch%d] buf_id:%d LumTableBase:0x%x ChrTableBase:0x%x\r\n", comp_ch, buf_id, g_isp.remap_param->encDec400LumTableBase[buf_id],
							  g_isp.remap_param->encDec400ChrTableBase[buf_id]);
			}
		}
	}

error:
	rtos_task_delete(NULL);
}


int isp_change_comp_header_output_addr(int buf_id)
{
	if (g_isp.data_start_notify) {
		rtos_queue_send(g_isp.data_start_notify, &buf_id, 0);
	}
	return 0;
}

/* TODO: should include header file provided by sensor owner */
#include "ainr_model_2688_1520_os04a10.c"
#include "ainr_model_2560_1440_gc4023.c"

static int isp_ai_denoise_stop(void)
{
	int ret = 0;
	int status;

	if (!g_isp.ainr) {
		g_isp.ainr_model_loaded = 0;
		return 0;
	}

	status = ainr_driver_ctrl(g_isp.ainr, AINR_CTRL_CMD_DISABLE, NULL);
	if (status != AINR_SUCCESS) {
		ISP_DBG_ERROR("ainr_driver_ctrl DISABLE fail\n");
		ret = -1;
	} else {
		ISP_DBG_INFO("AINR disabled\n");
	}

	ainr_driver_close(g_isp.ainr);
	g_isp.ainr = NULL;
	g_isp.ainr_model_loaded = 0;
	ISP_DBG_INFO("AINR driver closed\n");

	return ret;
}

static int isp_ai_denoise_load_model(void)
{
	int ret = 0;
	ISP_DBG_INFO("AINR load model...\n");

	if (g_isp.ainr != NULL) {
		ISP_DBG_INFO("close previous AINR context\n");
		ainr_driver_close(g_isp.ainr);
		g_isp.ainr = NULL;
	}
	g_isp.ainr = ainr_driver_open();
	if (g_isp.ainr == NULL) {
		ISP_DBG_ERROR("ainr_driver_open fail\n");
		ret = -1;
		goto error;
	}

	/* AINR model size ~39 KB */
	int status = AINR_SUCCESS;
	ainr_model_t model = {0};
	model.use_dma = 0;
	model.update_mixup_ratio = 0;
	if (g_isp.sensor.width > 2560) {
		model.model_data = (const void *)ainr_model_2688_1520_os04a10;
		model.model_size = sizeof(ainr_model_2688_1520_os04a10);
	} else {
		model.model_data = (const void *)ainr_model_2560_1440_gc4023;
		model.model_size = sizeof(ainr_model_2560_1440_gc4023);
	}
	status = ainr_driver_ctrl(g_isp.ainr, AINR_CTRL_CMD_LOAD_MODEL, &model);
	if (status != AINR_SUCCESS) {
		ISP_DBG_ERROR("ainr_driver_ctrl LOAD_MODEL fail\n");
		ret = -1;
		goto error;
	}
	ISP_DBG_INFO("AINR load model finish\n");

error:
	if (ret != 0 && g_isp.ainr) {
		ainr_driver_close(g_isp.ainr);
		g_isp.ainr = NULL;
		g_isp.ainr_model_loaded = 0;
	}
	return ret;
}

static int isp_ai_denoise_start(void)
{
	int ret = 0;
	if (!g_isp.ainr_model_loaded) {
		ret = isp_ai_denoise_load_model();
		if (ret < 0) {
			ISP_DBG_ERROR("error: ainr_load_model fail, ret = %d", ret);
			ret = -1;
			goto error;
		}
		g_isp.ainr_model_loaded = 1;
	}
	ISP_DBG_INFO("AINR model loaded success\r\n");

	int status = AINR_SUCCESS;
	ISP_DBG_INFO("enable AINR\r\n");
	ainr_param_t ainr_params = {
		.width = g_isp.sensor.width,
		.height = g_isp.sensor.height,
		.pixfmt = 0xE4,
		.mode = 0,
	};
	ISP_DBG_INFO("AINR set w[%d] h[%d] pixfmt[0x%x] mode[%d]\n", ainr_params.width, ainr_params.height, ainr_params.pixfmt, ainr_params.mode);
	status = ainr_driver_ctrl(g_isp.ainr, AINR_CTRL_CMD_SET_PARAM, &ainr_params);
	if (status != AINR_SUCCESS) {
		ISP_DBG_ERROR("ainr_driver_ctrl SET_PARAM fail\n\r");
		ret = -1;
		goto error;
	}
	status = ainr_driver_ctrl(g_isp.ainr, AINR_CTRL_CMD_ENABLE, NULL);
	if (status != AINR_SUCCESS) {
		ISP_DBG_ERROR("ainr_driver_ctrl ENABLE fail\n\r");
		ret = -1;
		goto error;
	}
	ISP_DBG_INFO("AINR enabled\n");

error:
	if (ret != 0) {
		isp_ai_denoise_stop();
	}
	return ret;
}

#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
static int isp_get_address_remap_ch(void)
{
	int isp_remap_ch = -1;
	for (int i = 0; i < VIDEO_ISP_VALID_CH; i++) {
		if (g_isp.ctx[i]) {
			if (g_isp.ctx[i]->params.compress_en || g_isp.ctx[i]->params.slice_mode_en) {
				isp_remap_ch = i;
				break;
			}
		}
	}
	return isp_remap_ch;
}

static void isp_sensor_start(void)
{
	static BOOL first = TRUE;
	const struct rts_isp_sensor_ops *sensor_ops = NULL;
	if (first && rts_isp_get_remap_mode(ISP0) != 0) {
		if (isp_sensor_get_ops(ISP0, &sensor_ops) != 0 || !sensor_ops || !sensor_ops->start) {
			ISP_DBG_ERROR("slice sensor start callback unavailable\r\n");
			return;
		}
		if (sensor_ops->start(ISP0) != 0) {
			ISP_DBG_ERROR("slice sensor start failed\r\n");
			return;
		}
		first = FALSE;
		ISP_DBG_INFO("isp_sensor_start\n\r");
	}
}
#endif
static void isp_sensor_info_update(void)
{
	if (ISP_GetSensorDimension(&g_isp.sensor.width, &g_isp.sensor.height)) {
		ISP_DBG_ERROR("sensor ISP_GetSensorDimension fail\n");
		return;
	}

	ISP_DBG_INFO("expected sensor w[%d] h[%d]\n", g_isp.sensor.width, g_isp.sensor.height);
}

static void isp_driver_close(void)
{
	if (g_isp.ainr) {
		isp_ai_denoise_stop();
	}

	if (g_isp.ainr_cmd_queue) {
		rtos_queue_delete(g_isp.ainr_cmd_queue);
		g_isp.ainr_cmd_queue = NULL;
	}

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	if (!g_isp.stage1_attached && g_isp.io.initialized) {
#else
	if (g_isp.io.initialized) {
#endif
		extern void isp_release(struct isp_io * io);
		isp_release(&g_isp.io);
	}
	g_isp.driver_opened = 0;
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	g_isp.stage1_attached = 0;
#endif
}

/* Apply a new run's global AINR setting before any channel starts streaming.
 * ISP stays open across channel destruction, so driver init alone is not enough. */
static int isp_ai_denoise_apply(void)
{
	isp_ainr_cmd_t pending;

	/* A command queued before stream-off must not override the new run. */
	if (g_isp.ainr_cmd_queue) {
		rtos_queue_receive(g_isp.ainr_cmd_queue, &pending, 0);
	}
	if (!g_isp.ai_denoise_en) {
		return isp_ai_denoise_stop();
	}
	if (!g_isp.ainr_cmd_queue &&
		rtos_queue_create(&g_isp.ainr_cmd_queue, 1, sizeof(isp_ainr_cmd_t)) != SUCCESS) {
		ISP_DBG_ERROR("error: create AINR cmd queue fail\n");
		return -1;
	}
	return isp_ai_denoise_start();
}

static int isp_driver_open(void)
{
	if (g_isp.driver_opened) {
		ISP_DBG_INFO("ISP driver opened already\r\n");
		return 0;
	}
	int ret = 0;

#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
	/* isp remap pre setup */
	//TODO: should be modified after isp control sw being ready
	int isp_remap_ch = -1;
	isp_remap_ch = isp_get_address_remap_ch();
	if (isp_remap_ch != -1) {
		rts_isp_set_remap_mode(ISP0, 1);
	}
#endif

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	if (video_fcs_isp_stage1_is_started()) {
		struct isp_io *stage1_io = video_fcs_isp_stage1_get_io();
		if (!stage1_io || !stage1_io->initialized) {
			ISP_DBG_ERROR("error: FCS ISP state invalid\r\n");
			return -1;
		}
		memcpy(&g_isp.io, stage1_io, sizeof(g_isp.io));
		g_isp.stage1_attached = 1;
		isp_stage1_stream_mark_active();
		ISP_DBG_INFO("attach FCS ISP stage1\r\n");
	} else
#endif
	{
		/* ISP HW clock and function init */
		ISP_Init();
	}

	/* setup sensor info */
	isp_sensor_info_update();

#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
	if (!g_isp.stage1_attached)
#endif
	{
		/* init and start isp streaming */
#if defined(REALSENSOR) && (REALSENSOR==1)
		if (!g_isp.ctrlsw_started) {
			extern int rts_camera_init(void);
			int cam_ret = rts_camera_init();
			if (cam_ret) {
				ISP_DBG_ERROR("error: rts_camera_init fail, ret = %d", cam_ret);
				goto error;
			}
			g_isp.ctrlsw_started = 1;
		}
#endif
		ret = isp_initial(&g_isp.io);
		if (ret) {
			ISP_DBG_ERROR("error: init fail, ret = %d", ret);
			goto error;
		}
		ISP_DBG_INFO("isp_init_multi ok\r\n");

		isp_set_user_release();
	}

	isp_ch_handle_register(isp_frame_ready_irq_callback);

	g_isp.driver_opened = 1;
	return 0;

error:
	isp_driver_close();
	return -1;
}

static void isp_close(void)
{
	if (g_isp.output_task_handle) {
		rtos_task_delete(g_isp.output_task_handle);
		g_isp.output_task_handle = NULL;
	}

	if (g_isp.comp_header_update_task_handle) {
		rtos_task_delete(g_isp.comp_header_update_task_handle);
		g_isp.comp_header_update_task_handle = NULL;
	}

	if (g_isp.cmpr_header_ddr) {
		free(g_isp.cmpr_header_ddr);
		g_isp.cmpr_header_ddr = NULL;
	}

	if (g_isp.out_queue) {
		rtos_queue_delete(g_isp.out_queue);
		g_isp.out_queue = NULL;
	}

	if (g_isp.data_start_notify) {
		rtos_queue_delete(g_isp.data_start_notify);
		g_isp.data_start_notify = NULL;
	}
	g_isp.active_stream_mask = 0;
	g_isp.ai_denoise_en = 0;
	g_isp.ainr_model_loaded = 0;
}

static int isp_open(void)
{
	if (g_isp.out_queue == NULL) {
		if (rtos_queue_create(&g_isp.out_queue, 300, sizeof(isp_out_t)) != SUCCESS) {
			ISP_DBG_ERROR("error: fail to create isp out queue, memory not enough\r\n");
			goto error;
		}
	}
	ISP_DBG_INFO("g_isp.out_queue create success\n\r");

	int isp_remap_ch = -1;
#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
	/* isp remap pre setup */
	//TODO: should be modified after isp control sw being ready
	isp_remap_ch = isp_get_address_remap_ch();
	if (isp_remap_ch != -1) {
		if (isp_address_remap_pre_setup(g_isp.ctx[isp_remap_ch]) < 0) {
			goto error;
		}
	}
#endif

	/* isp remap compression header output address update task */
	if (isp_remap_ch != -1 && g_isp.ctx[isp_remap_ch]) {
		/* only frame mode compression require update header output address */
		if (g_isp.ctx[isp_remap_ch]->params.compress_en && !g_isp.ctx[isp_remap_ch]->params.slice_mode_en) {
			rtos_queue_create(&g_isp.data_start_notify, 1, sizeof(int));
			isp_data_start_handle_register(isp_change_comp_header_output_addr);
			if (g_isp.comp_header_update_task_handle == NULL) {
				if (rtos_task_create(&g_isp.comp_header_update_task_handle, "comp_header_update_task", comp_header_update_task, (void *)isp_remap_ch, 1024, 9) != RTK_SUCCESS) {
					ISP_DBG_ERROR("error: fail to create isp output task, memory not enough\r\n");
					goto error;
				}
			}
		}
	}


	if (g_isp.output_task_handle == NULL) {
		if (rtos_task_create(&g_isp.output_task_handle, "isp_output_task", isp_output_task, (void *)NULL, 1024, 9) != RTK_SUCCESS) {
			ISP_DBG_ERROR("error: fail to create isp output task, memory not enough\r\n");
			goto error;
		}
	}
	ISP_DBG_INFO("isp_output_task create success\n\r");

	return 0;

error:
	isp_close();
	return -1;
}

#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
extern uint8_t __com_vsram_start__[];
extern uint8_t __com_vsram_end__[];
#define COMPRESS_HEADER_SIZE	(32 * 1024)
extern uint8_t __vsram_start__[];
extern uint8_t __vsram_end__[];
#define VSRAM_SIZE  ((uint32_t)__vsram_end__ - (uint32_t)__vsram_start__)
extern uint8_t __slice_vsram_start__[];
extern uint8_t __slice_vsram_end__[];

extern void ISP_addremap_IRQHandler(void);
extern void ENC_addremap_IRQHandler(void);

static int isp_address_remap_pre_setup(isp_ctx_t *ctx)
{
	uint32_t y_bytes;
	isp_remap_param_t *param = ctx->params.remap_param;
	if (param->feature_bit & REMAP_SLICE) {
		if (param->feature_bit != (REMAP_SLICE | REMAP_RTT) ||
			param->enc_type != ISP_REMAP_ENC_H264 || param->burst_id != 0 ||
			ctx->params.stream_id != 0 || !ctx->params.height || ctx->params.height > 8192 ||
			slice_sram_y_bytes(ctx->params.width, 16, param->wrap_num,
				(uint32_t)__slice_vsram_end__ - (uint32_t)__slice_vsram_start__, &y_bytes) < 0) {
			ISP_DBG_ERROR("SRAM slice requires CH0 H264 RTT, no compression/partial SRAM, and room for both Y/UV\r\n");
			return -1;
		}
	}
	int IRQ_Priority = INT_PRI_LOWEST;
	InterruptRegister((IRQ_FUN)ISP_addremap_IRQHandler, ISP_REMAP_IRQ, (u32)NULL, IRQ_Priority);
	InterruptEn(ISP_REMAP_IRQ, 0);
	IRQ_Priority = INT_PRI_LOWEST;
	InterruptRegister((IRQ_FUN)ENC_addremap_IRQHandler, ENCODER_REMAP_IRQ, (u32)NULL, IRQ_Priority);
	InterruptEn(ENCODER_REMAP_IRQ, 0);

	int ch = ctx->params.stream_id;
	if (ch != 1 && ch != 0) {
		ISP_DBG_INFO("WARNING!!! please use isp compression on ch1");
	}
	ISP_DBG_DEBUG("isp set frame mode compressor for ch:%d\r\n", ch);

	uint32_t feature_bit = ctx->params.remap_param->feature_bit;
	uint32_t wrap_num = ctx->params.remap_param->wrap_num;
	uint32_t out_w = ctx->params.width;
	uint32_t out_h = ctx->params.height;
	ISP_DBG_INFO("feature_bit:%d wrap_num:%d out_w:%d out_h:%d\r\n", feature_bit, wrap_num, out_w, out_h);

	g_isp.remap_info.cfg.HdrEnb = (feature_bit & BIT(0)) ? 1 : 0;
	g_isp.remap_info.cfg.SWrapEnb = (feature_bit & BIT(1)) ? 1 : 0;
	g_isp.remap_info.cfg.RTTEnb = (feature_bit & BIT(3)) ? 1 : 0;
	g_isp.remap_info.cfg.PaREnb = (feature_bit & BIT(4)) ? 1 : 0;
	g_isp.remap_info.cfg.CompEnb = (feature_bit & BIT(5)) ? 1 : 0;
	if (g_isp.remap_info.cfg.SWrapEnb) {
		g_isp.remap_info.all_vsram = 1;  /* Y and UV must both reside in SRAM. */
	}
	ISP_DBG_INFO("isp remap all_vsram %d\r\n", g_isp.remap_info.all_vsram);
	ISP_DBG_INFO("g_isp.remap_info.cfg hdr %d slice %d RTT %d parSRAM %d compress%d  \r\n", g_isp.remap_info.cfg.HdrEnb, g_isp.remap_info.cfg.SWrapEnb,
				 g_isp.remap_info.cfg.RTTEnb,
				 g_isp.remap_info.cfg.PaREnb, g_isp.remap_info.cfg.CompEnb);

	g_isp.enc_remap_info.cfg.ParSWrapEnb = (feature_bit & BIT(2)) ? 1 : 0;
	g_isp.enc_remap_info.cfg.RTTEnb = (feature_bit & BIT(3)) ? 1 : 0;
	g_isp.enc_remap_info.cfg.PaREnb = (feature_bit & BIT(4)) ? 1 : 0;
	g_isp.enc_remap_info.cfg.CompEnb = (feature_bit & BIT(5)) ? 1 : 0;
	if (g_isp.enc_remap_info.cfg.RTTEnb) {
		g_isp.enc_remap_info.cfg.SWrapEnb = (feature_bit & BIT(1)) ? 1 : 0;
	}
	ISP_DBG_INFO("enc_remap_cfg slice %d Parslice %d RTT %d parSRAM %d compress%d  \r\n", g_isp.enc_remap_info.cfg.SWrapEnb, g_isp.enc_remap_info.cfg.ParSWrapEnb,
				 g_isp.enc_remap_info.cfg.RTTEnb,
				 g_isp.enc_remap_info.cfg.PaREnb, g_isp.enc_remap_info.cfg.CompEnb);

	/* set low latency when using slice mode */
	g_isp.remap_info.low_latency_set = (feature_bit & BIT(1)) ? 1 : 0;

	if (g_isp.remap_info.low_latency_set || g_isp.remap_info.cfg.CompEnb) {
		extern u32 isp_remap_preprocess(struct isp_remap_config * cfg, u32 width, u32 height, u32 enc_type, u32 wrap_num, u32 burst_id);
		g_isp.remap_info.align_width = isp_remap_preprocess(&g_isp.remap_info.cfg, out_w, out_h, ctx->params.remap_param->enc_type, wrap_num,
									   ctx->params.remap_param->burst_id);
		// g_isp.zoom[ch].align_width = g_isp.remap_info.align_width;
	} else {
		// g_isp.zoom[ch].align_width = 0;
	}
	g_isp.io.low_latency_mode = g_isp.remap_info.low_latency_set;
	g_isp.io.remap_fps = ctx->params.fps;
	g_isp.io.fixp_hts_ratio_2base = ctx->params.remap_param->fixp_hts_ratio_2;
	if (!g_isp.remap_info.low_latency_set && g_isp.remap_info.cfg.CompEnb) {
		g_isp.remap_info.cmpr_fr_mode = 1;
		g_isp.io.cmpr_fr_mode = 1;
		g_isp.io.cmpr_fr_stm_id = ch;
	}

	return 0;
}

static int isp_address_remap_post_setup(isp_ctx_t *ctx)
{
	int ret = 0;
	int ch = ctx->params.stream_id;

	uint32_t VSRAM_start = (uint32_t)__vsram_start__;
	uint32_t wrap_num = ctx->params.remap_param->wrap_num;
	uint32_t out_w = ctx->params.width;
	uint32_t out_h = ctx->params.height;
	uint32_t slice_out_size0 = 0;
	if (g_isp.remap_info.low_latency_set || g_isp.remap_info.cmpr_fr_mode) {
		uint32_t out_y_addr;
		uint32_t out_uv_addr;
		extern int rtscam_soc_get_isp_remap_buf(uint32_t streamid, uint32_t buf_id, uint32_t *y_buf, uint32_t *uv_buf);
		if (!g_isp.remap_info.cmpr_fr_mode) {
			if (rtscam_soc_get_isp_remap_buf(ch, 0, &out_y_addr, &out_uv_addr) < 0) {
				return -1;
			}
			printf("ch0 slot0: out_y_addr: %x, out_uv_addr: %x\n\r", (int)out_y_addr, (int)out_uv_addr);
			g_isp.io.io_buffer_ISP[ch][0].out_y_addr = out_y_addr;
			g_isp.io.io_buffer_ISP[ch][0].out_y_size = ALIGN_UP(g_isp.remap_info.align_width * out_h, 4096);
			g_isp.io.io_buffer_ISP[ch][0].out_uv_addr = out_uv_addr;
			g_isp.io.io_buffer_ISP[ch][0].out_uv_size = g_isp.remap_info.align_width * out_h / 2;
			g_isp.io.io_buffer_ISP[ch][1].out_y_addr = out_y_addr;
			g_isp.io.io_buffer_ISP[ch][1].out_y_size = ALIGN_UP(g_isp.remap_info.align_width * out_h, 4096);
			g_isp.io.io_buffer_ISP[ch][1].out_uv_addr = out_uv_addr;
			g_isp.io.io_buffer_ISP[ch][1].out_uv_size = g_isp.remap_info.align_width * out_h / 2;
		} else {
			if (rtscam_soc_get_isp_remap_buf(ch, 0, &out_y_addr, &out_uv_addr) < 0) {
				return -1;
			}
			printf("ch0 slot0: out_y_addr: %x, out_uv_addr: %x\n\r", (int)out_y_addr, (int)out_uv_addr);
			g_isp.io.io_buffer_ISP[ch][0].out_y_addr = out_y_addr;
			g_isp.io.io_buffer_ISP[ch][0].out_y_size = ALIGN_UP(g_isp.remap_info.align_width * out_h, 4096);
			g_isp.io.io_buffer_ISP[ch][0].out_uv_addr = out_uv_addr;
			g_isp.io.io_buffer_ISP[ch][0].out_uv_size = ALIGN_UP(g_isp.remap_info.align_width * out_h / 2, 4096);
			rtscam_soc_get_isp_remap_buf(ch, 1, &out_y_addr, &out_uv_addr);
			printf("ch0 slot1: out_y_addr: %x, out_uv_addr: %x\n\r", (int)out_y_addr, (int)out_uv_addr);
			g_isp.io.io_buffer_ISP[ch][1].out_y_addr = out_y_addr;
			g_isp.io.io_buffer_ISP[ch][1].out_y_size = ALIGN_UP(g_isp.remap_info.align_width * out_h, 4096);
			g_isp.io.io_buffer_ISP[ch][1].out_uv_addr = out_uv_addr;
			g_isp.io.io_buffer_ISP[ch][1].out_uv_size = g_isp.remap_info.align_width * out_h / 2;
		}

		if (!g_isp.remap_info.cmpr_fr_mode) {
			slice_out_size0 = g_isp.remap_info.align_width * g_isp.remap_info.cfg.SliceHeight * wrap_num;
		} else {
			slice_out_size0 = g_isp.io.io_buffer_ISP[ch][1].out_y_size;
		}
		if (g_isp.remap_info.cmpr_fr_mode) {
			/* frame mode compression */
			g_isp.remap_info.cfg.compress_component[0].headr_addr[0] = (uint32_t)__com_vsram_start__;
		} else if (g_isp.remap_info.cfg.CompEnb) {
			/* slice mode + compression */
			/* put compression header to ddr, vsram should be used for slice wrap */
			g_isp.cmpr_header_ddr = malloc(COMPRESS_HEADER_SIZE + 1024);
			g_isp.remap_info.cfg.compress_component[0].headr_addr[0] = ((uint32_t)g_isp.cmpr_header_ddr + 0x3FF) & ~((uint32_t)0x3FF); /* 1024 aligned */
			VSRAM_start = (uint32_t)__slice_vsram_start__;
		} else {
			VSRAM_start = (uint32_t)__slice_vsram_start__;
		}

		if (g_isp.remap_info.all_vsram && !g_isp.remap_info.cmpr_fr_mode) {
			if (g_isp.remap_info.cfg.PaREnb || g_isp.remap_info.cfg.CompEnb ||
				slice_sram_y_bytes(out_w, g_isp.remap_info.cfg.SliceHeight, wrap_num,
					(uint32_t)__slice_vsram_end__ - VSRAM_start, &slice_out_size0) < 0) {
				ISP_DBG_ERROR("SRAM slice Y+UV allocation failed; no DRAM fallback\r\n");
				return -1;
			}
			g_isp.remap_info.slice_out_addr0 = VSRAM_start;
			g_isp.remap_info.slice_out_addr1 = VSRAM_start + slice_out_size0;
			printf("[ISP SRAM] Y=0x%08lx bytes=%lu UV=0x%08lx bytes=%lu end=0x%08lx wrap=%lu\r\n",
				(unsigned long)VSRAM_start, (unsigned long)slice_out_size0,
				(unsigned long)g_isp.remap_info.slice_out_addr1, (unsigned long)(slice_out_size0 / 2),
				(unsigned long)(VSRAM_start + slice_out_size0 * 3 / 2), (unsigned long)wrap_num);
		} else {
			g_isp.remap_info.slice_out_addr0 = g_isp.io.io_buffer_ISP[ch][0].out_y_addr;
			g_isp.remap_info.slice_out_addr1 = g_isp.io.io_buffer_ISP[ch][0].out_uv_addr;
		}
		if (!g_isp.remap_info.cmpr_fr_mode) {
			g_isp.remap_info.cfg.slice_wrap_component[0].dram_saddr = g_isp.io.io_buffer_ISP[ch][0].out_y_addr;
			g_isp.remap_info.cfg.slice_wrap_component[0].dram_eaddr = g_isp.io.io_buffer_ISP[ch][0].out_y_addr + g_isp.io.io_buffer_ISP[ch][0].out_y_size - 1;
			g_isp.remap_info.cfg.slice_wrap_component[1].dram_saddr = g_isp.io.io_buffer_ISP[ch][0].out_uv_addr;
			g_isp.remap_info.cfg.slice_wrap_component[1].dram_eaddr = g_isp.io.io_buffer_ISP[ch][0].out_uv_addr + g_isp.io.io_buffer_ISP[ch][0].out_uv_size - 1;
		} else {
			g_isp.remap_info.cfg.slice_wrap_component[0].dram_saddr = g_isp.io.io_buffer_ISP[ch][0].out_y_addr;
			g_isp.remap_info.cfg.slice_wrap_component[0].dram_eaddr = g_isp.io.io_buffer_ISP[ch][1].out_y_addr + g_isp.io.io_buffer_ISP[ch][1].out_y_size - 1;
			g_isp.remap_info.cfg.slice_wrap_component[1].dram_saddr = g_isp.io.io_buffer_ISP[ch][0].out_uv_addr;
			g_isp.remap_info.cfg.slice_wrap_component[1].dram_eaddr = g_isp.io.io_buffer_ISP[ch][1].out_uv_addr + g_isp.io.io_buffer_ISP[ch][1].out_uv_size - 1;
		}
		g_isp.remap_info.cfg.slice_wrap_component[0].out_saddr_offset = g_isp.remap_info.cfg.slice_wrap_component[0].dram_saddr - g_isp.remap_info.slice_out_addr0;
		g_isp.remap_info.cfg.slice_wrap_component[1].out_saddr_offset = g_isp.remap_info.cfg.slice_wrap_component[1].dram_saddr - g_isp.remap_info.slice_out_addr1;
		ISP_DBG_DEBUG("Y 0x%08x UV 0x%08x HDR 0x%08x\r\n", g_isp.remap_info.slice_out_addr0, g_isp.remap_info.slice_out_addr1,
					  g_isp.remap_info.cfg.compress_component[0].headr_addr[0]);
		extern int set_isp_remap(struct isp_remap_config * cfg, u32 zoom_w, u32 zoom_h);
		ret = set_isp_remap(&g_isp.remap_info.cfg, out_w, out_h);
		if (ret != 0) {
			ISP_DBG_ERROR("error: fail to set isp remap\r\n");
			return -1;
		}

		if (!g_isp.remap_info.cmpr_fr_mode) {
			g_isp.enc_remap_info.cfg.SliceNumWrap = wrap_num;
			g_isp.enc_remap_info.cfg.SliceHeight = g_isp.remap_info.cfg.SliceHeight;
			g_isp.enc_remap_info.cfg.SliceBurstID = g_isp.remap_info.cfg.SliceBurstID;
			g_isp.enc_remap_info.cfg.Slice_Stride = g_isp.remap_info.cfg.Slice_Stride;
			g_isp.enc_remap_info.cfg.Slice_num = g_isp.remap_info.cfg.Slice_num;
			g_isp.enc_remap_info.cfg.partial_sram_config.sram_ratio = g_isp.remap_info.cfg.partial_sram_config.sram_ratio;

			g_isp.remap_info.slice_buff_size = g_isp.remap_info.align_width * g_isp.enc_remap_info.cfg.SliceHeight * g_isp.enc_remap_info.cfg.SliceNumWrap;
			g_isp.enc_remap_info.cfg.enc_remap_component[0].dram_saddr = g_isp.remap_info.slice_out_addr0;
			g_isp.enc_remap_info.cfg.enc_remap_component[0].dram_eaddr = g_isp.enc_remap_info.cfg.enc_remap_component[0].dram_saddr + g_isp.remap_info.slice_buff_size - 1;
			g_isp.enc_remap_info.cfg.enc_remap_component[1].dram_saddr = g_isp.remap_info.slice_out_addr1;
			g_isp.enc_remap_info.cfg.enc_remap_component[1].dram_eaddr = g_isp.enc_remap_info.cfg.enc_remap_component[1].dram_saddr + g_isp.remap_info.slice_buff_size / 2 -
					1;

			if (g_isp.remap_info.cfg.PaREnb) {
				g_isp.enc_remap_info.cfg.partial_sram_config.sram_ratio = g_isp.remap_info.cfg.partial_sram_config.sram_ratio;
				g_isp.enc_remap_info.cfg.partial_sram_config.sram_addr[0].start_addr = g_isp.remap_info.cfg.partial_sram_config.sram_addr[0].start_addr;
				g_isp.enc_remap_info.cfg.partial_sram_config.sram_addr[1].start_addr = g_isp.remap_info.cfg.partial_sram_config.sram_addr[1].start_addr;
			}
			extern void set_enc_remap(struct enc_remap_config * config,  u32 zoom_w, u32 zoom_h);
			set_enc_remap(&g_isp.enc_remap_info.cfg, out_w, out_h);
			/* Verify both physical planes and the shared slice/RTT mode before sensor start. */
			if (g_isp.remap_info.all_vsram) {
				for (unsigned int plane = 0; plane < 2; plane++) {
					uint32_t offset = plane ? 0x40 : 0x34;
					uint32_t phys = g_isp.enc_remap_info.cfg.enc_remap_component[plane].dram_saddr;
					uint32_t end = g_isp.enc_remap_info.cfg.enc_remap_component[plane].dram_eaddr;
					if (HAL_READ32(REMAP_ISP_REG_BASE, offset) -
						HAL_READ32(REMAP_ISP_REG_BASE, 0xE0 + plane * 4) != phys ||
						HAL_READ32(REMAP_ENC_REG_BASE, offset) != phys ||
						HAL_READ32(REMAP_ENC_REG_BASE, offset + 4) != end) {
						ISP_DBG_ERROR("SRAM remap plane %u readback mismatch\r\n", plane);
						return -1;
					}
				}
				if (HAL_READ32(REMAP_ISP_REG_BASE, 0xC) != 0x10100 ||
					HAL_READ32(REMAP_ENC_REG_BASE, 0xC) != 0x10100 ||
					HAL_READ32(REMAP_ISP_REG_BASE, 0x30) != HAL_READ32(REMAP_ENC_REG_BASE, 0x30)) {
					ISP_DBG_ERROR("SRAM remap mode/depth readback mismatch\r\n");
					return -1;
				}
				printf("[ISP SRAM] Y/UV mapping readback OK; partial SRAM disabled\r\n");
			}
		}
		if (g_isp.remap_info.cfg.CompEnb) {
			g_isp.cmpr_header_y[0] = g_isp.remap_info.cfg.compress_component[0].headr_addr[0];
			g_isp.cmpr_header_y[1] = g_isp.remap_info.cfg.compress_component[0].headr_addr[1];
			ISP_DBG_DEBUG("[ISP]cmpr_header_y[0]:0x%x cmpr_header_y[1]:0x%x\r\n", g_isp.cmpr_header_y[0], g_isp.cmpr_header_y[1]);
			g_isp.cmpr_header_uv[0] = g_isp.remap_info.cfg.compress_component[1].headr_addr[0];
			g_isp.cmpr_header_uv[1] = g_isp.remap_info.cfg.compress_component[1].headr_addr[1];
			ISP_DBG_DEBUG("[ISP]cmpr_header_uv[0]:0x%x cmpr_header_uv[1]:0x%x\r\n", g_isp.cmpr_header_uv[0], g_isp.cmpr_header_uv[1]);

			/* init slice mode compression header, no need to update in each isp data start */
			if (g_isp.remap_info.low_latency_set) {
				g_isp.remap_param->encDec400LumTableBase[0] = g_isp.cmpr_header_y[0];
				g_isp.remap_param->encDec400ChrTableBase[0] = g_isp.cmpr_header_uv[0];
			}
		}

#if DBG_REMAP_REG_DUMP
		for (u32 check_addr = 0x0; check_addr <=  0xEC; check_addr += 4) {
			DiagPrintf("addr 0x%08x value 0x%08x	\n\r", REMAP_ISP_REG_BASE + check_addr, HAL_READ32(REMAP_ISP_REG_BASE, check_addr));
		}
		for (u32 check_addr = 0x0; check_addr <=  0xEC; check_addr += 4) {
			DiagPrintf("addr 0x%08x value 0x%08x	\n\r", REMAP_ENC_REG_BASE + check_addr, HAL_READ32(REMAP_ENC_REG_BASE, check_addr));
		}
#endif
	}

	/* frame level compression should start sensor here */
	if (!g_isp.remap_info.low_latency_set) {
		isp_sensor_start();
	}

	return 0;
}
#endif

int isp_control(void *p, int cmd, int arg)
{
	isp_ctx_t *ctx = (isp_ctx_t *)p;
	mm_context_t *mctx = (mm_context_t *)ctx->parent;
	(void)mctx;
	int ch;
	isp_ainr_cmd_t ainr_cmd;
	int ret = 0;

	switch (cmd) {
	case CMD_ISP_SET_PARAMS:
		/* copy params */
		memcpy(&ctx->params, (void *)arg, sizeof(isp_params_t));
		ch = ctx->params.stream_id;
		if ((uint32_t)ch >= VIDEO_ISP_VALID_CH) {
			return -1;
		}
		/* register context */
		g_isp.ctx[ch] = ctx;  //TODO: should be modified after isp control sw being ready
		break;
	case CMD_ISP_GET_PARAMS:
		memcpy((void *)arg, &ctx->params, sizeof(isp_params_t));
		break;
	case CMD_ISP_STREAMID:
		ctx->params.stream_id = arg;
		break;
	case CMD_ISP_HW_SLOT:
		ctx->params.slot_num = arg;
		break;
	case CMD_ISP_SW_SLOT:
		ctx->params.buff_num = arg;
		break;
	case CMD_ISP_FPS:
		ctx->params.fps = arg;
		break;
	case CMD_ISP_WIDTH:
		ctx->params.width = arg;
		break;
	case CMD_ISP_HEIGHT:
		ctx->params.height = arg;
		break;
	case CMD_ISP_FORMAT:
		ctx->params.format = arg;
		break;
	case CMD_ISP_AINR_START:
		if (!g_isp.ainr_cmd_queue) {
			return -1;
		}
		rtos_queue_receive(g_isp.ainr_cmd_queue, &ainr_cmd, 0); /* remove old cmd if exist */
		ainr_cmd = ISP_AINR_CMD_ENABLE;
		rtos_queue_send(g_isp.ainr_cmd_queue, &ainr_cmd, 0);
		break;
	case CMD_ISP_AINR_STOP:
		if (!g_isp.ainr_cmd_queue) {
			return -1;
		}
		rtos_queue_receive(g_isp.ainr_cmd_queue, &ainr_cmd, 0); /* remove old cmd if exist */
		ainr_cmd = ISP_AINR_CMD_DISABLE;
		rtos_queue_send(g_isp.ainr_cmd_queue, &ainr_cmd, 0);
		break;
	case CMD_ISP_AINR_CLOSE:
		isp_ai_denoise_stop();
		break;
	case CMD_ISP_SET_REMAP:
		ctx->params.remap_param = (isp_remap_param_t *)arg;
		g_isp.remap_param = (isp_remap_param_t *)arg;
		break;
	case CMD_ISP_SET_FPS:
		ch = ctx->params.stream_id;
		ctx->params.fps = arg;
		ISP_SetStreamFPS((uint32_t)ch, (uint32_t)arg);
		break;
	case CMD_ISP_STREAM_STOP: {
		ch = ctx->params.stream_id;
		if ((uint32_t)ch >= VIDEO_ISP_VALID_CH) {
			ret = -1;
			break;
		}
		if (g_isp.active_stream_mask & BIT(ch)) {
			ret = ISP_StreamOffByChannel(ch);
			if (ret != 0) {
				ret = -1;
				break;
			}
			isp_set_stream_active(ch, 0);
		}
		g_isp.ch_info[ch].cb_func = NULL;
		g_isp.ch_info[ch].cb_arg = NULL;
		mm_module_ctrl(mctx, MM_CMD_CLEAR_QUEUE_ITEMS, 0);
		break;
	}
	case CMD_ISP_STREAM_START: {
		ch = ctx->params.stream_id;
		if ((uint32_t)ch >= VIDEO_ISP_VALID_CH || !ctx->applied || !g_isp.driver_opened) {
			return -1;
		}
		if (g_isp.active_stream_mask & BIT(ch)) {
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
			if (g_isp.stage1_live_mask & BIT(ch)) {
				stream_params_t strm_param = {0};
				strm_param.ch       = ch;
				strm_param.width    = ctx->params.width;
				strm_param.height   = ctx->params.height;
				strm_param.fps      = ctx->params.fps;
				strm_param.slot_num = ctx->params.slot_num;
				strm_param.format   = ctx->params.format;
				if (!isp_stage1_stream_param_match(&strm_param)) {
					ISP_DBG_ERROR("error: FCS ISP param mismatch\r\n");
					return -1;
				}
			}
#endif
			return 0;
		}
		isp_stream_set_complete_callback(isp_frame_complete_cb, (void *)ctx);
		/* isp stream on by channel */
		stream_params_t strm_param = {0};
		strm_param.ch       = ch;
		strm_param.enable   = 1;
		strm_param.width    = ctx->params.width;
		strm_param.height   = ctx->params.height;
		strm_param.fps      = ctx->params.fps;
		strm_param.slot_num = ctx->params.slot_num;
		strm_param.format   = ctx->params.format;
#if defined(RAW_NUM) && (RAW_NUM > 0)
		raw_frame_cnt = 0;
		if (sd_init_done == FALSE) {
			vfs_init();
			vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
			sd_init_done = true;
		}
#endif
		if (ISP_StreamOnByChannel((void *)&strm_param) < 0) {
			ISP_DBG_ERROR("ISP_StreamOnByChannel fail\n\r");
			return -1;
		}
		isp_set_stream_active(ch, 1);
		break;
	}
	case CMD_ISP_APPLY:
		ch = ctx->params.stream_id;
		if ((uint32_t)ch >= VIDEO_ISP_VALID_CH || ctx->applied) {
			return -1;
		}
		stream_params_t strm_param = {0};
		int driver_was_open = g_isp.driver_opened;
		/* Capture before driver open: FCS attachment can mark streams active. */
		int apply_ainr = !driver_was_open || !g_isp.active_stream_mask;
		/* set callback */
		isp_stream_set_complete_callback(isp_frame_complete_cb, (void *)ctx);
		ISP_DBG_DEBUG("isp_stream_set_complete_callback success\n\r");
		/* isp open */
		if (!g_isp.driver_opened) {
			if (isp_driver_open() < 0 || isp_open() < 0) {
				isp_driver_close();
				isp_close();
				return -1;
			}
#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
			if (g_isp.remap_info.cfg.SWrapEnb) {
				strm_param.ar_feature = 2;
			} else if (g_isp.remap_info.cfg.CompEnb) {
				strm_param.ar_feature = 1;
			} else {
				strm_param.ar_feature = 0;
			}
			strm_param.ar_align_w = g_isp.remap_info.align_width;
#endif
			ISP_DBG_INFO("[MMF] isp opened\r\n");
		}
		/* Recompute instead of retaining an ON flag from a previous run.
		 * Preserve shared AINR while another channel is streaming. */
		if (apply_ainr) {
			g_isp.ai_denoise_en = 0;
			for (int i = 0; i < VIDEO_ISP_VALID_CH; i++) {
				if (g_isp.ctx[i] && g_isp.ctx[i]->params.ai_denoise_en) {
					g_isp.ai_denoise_en = 1;
					break;
				}
			}
			if (isp_ai_denoise_apply() != 0) {
				ISP_DBG_ERROR("error: apply AINR setting fail\r\n");
				if (!driver_was_open) {
					isp_driver_close();
					isp_close();
				}
				return -1;
			}
		}
		g_isp.module_cnt++;
		ctx->applied = 1;
		/* isp stream on by channel */

		strm_param.ch       = ch;
		strm_param.enable   = 1;
		strm_param.width    = ctx->params.width;
		strm_param.height   = ctx->params.height;
		strm_param.fps      = ctx->params.fps;
		strm_param.slot_num = ctx->params.slot_num;
		strm_param.format   = ctx->params.format;
		if (g_isp.active_stream_mask & BIT(ch)) {
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
			if (!isp_stage1_stream_param_match(&strm_param)) {
				ISP_DBG_ERROR("error: FCS ISP param mismatch\r\n");
				ctx->applied = 0;
				g_isp.module_cnt--;
				return -1;
			}
#endif
			break;
		}
#if defined(RAW_NUM) && (RAW_NUM > 0)
		raw_frame_cnt = 0;
		if (sd_init_done == FALSE) {
			vfs_init();
			vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
			sd_init_done = TRUE;
		}
#endif
		if (ISP_StreamOnByChannel((void *)&strm_param) < 0) {
			ISP_DBG_ERROR("ISP_StreamOnByChannel fail\n\r");
			ctx->applied = 0;
			g_isp.module_cnt--;
			return -1;
		}
		isp_set_stream_active(ch, 1);
#if defined(CONFIG_VMODULE_ADDREMAP) && ISP_ADDR_REMAP_ENA
		/* slice mode */

		int isp_remap_ch = -1;

		isp_remap_ch = isp_get_address_remap_ch();

		if (g_isp.module_cnt == 1) {
			/* isp remap post setup */
			//TODO: should be modified after isp control sw being ready
			if (isp_remap_ch != -1) {
				if (isp_address_remap_post_setup(g_isp.ctx[isp_remap_ch]) < 0) {
					ISP_DBG_ERROR("error: fail to do isp address remap post setup\r\n");
					isp_close();
					return -1;
				}
			}
			if (isp_remap_ch != -1 && g_isp.ctx[isp_remap_ch]) {
				if (g_isp.ctx[isp_remap_ch]->params.slice_mode_en) {
					/* set slice mode sensor start callback func in encoder */
					extern void EncSetSliceModeSensorStartFunc(void (*Func)(void));
					/* the registered function will be exec after encoder start in slice mode */
					EncSetSliceModeSensorStartFunc(isp_sensor_start);

					/* send 1st isp slice frame to start slice encode */
					isp_send_trigger_slice(isp_remap_ch);
				}
			}
		}
#endif

		break;
	}
	return 0;
}

int isp_handle(void *ctx, void *input, void *output)
{
	/* do nothing */
	(void)ctx;
	(void)input;
	(void)output;
	return 0;
}

void *isp_create(void *parent)
{
	isp_ctx_t *ctx = malloc(sizeof(isp_ctx_t));
	if (!ctx) {
		return NULL;
	}
	memset(ctx, 0, sizeof(isp_ctx_t));

	ctx->parent = parent;

	return ctx;
}

void *isp_destroy(void *p)
{
	isp_ctx_t *ctx = (isp_ctx_t *)p;
	if (ctx) {
		int ch = ctx->params.stream_id;
		if ((uint32_t)ch < VIDEO_ISP_VALID_CH && g_isp.ctx[ch] == ctx) {
			g_isp.ch_info[ch].cb_func = NULL;
			g_isp.ch_info[ch].cb_arg = NULL;
			g_isp.ctx[ch] = NULL;
		}
		if (ctx->applied) {
			ctx->applied = 0;
			g_isp.module_cnt--;
			/* Keep the global ISP driver alive for channel stream-off/on. */
		}
		free(ctx);
	}

	return NULL;
}

void *isp_new_item(void *p)
{
	(void)p;
	/* if init item with MMQI_FLAG_DYNAMIC, return NULL */
	return NULL;
}

void *isp_del_item(void *p, void *d)
{
	(void)p;
	(void)d;
	/* if init item with MMQI_FLAG_DYNAMIC, return NULL */
	return NULL;
}

void *isp_release_item(void *p, void *d, int length)
{
	(void)length;
	isp_ctx_t *ctx = (isp_ctx_t *)p;
	mm_queue_item_t *free_item = (mm_queue_item_t *)d;
	int stream_id = ctx->params.stream_id;
	void *isp_buf_addr = (void *)free_item->data_addr;

	if (g_isp.ctx[stream_id]->params.slice_mode_en) {
		/* trigger next encoding, slot will be released in slice frame end/ready callback */
		isp_send_trigger_slice(stream_id);
	} else {
		/* release isp slot */
		isp_buf_release(stream_id, (void *)isp_buf_addr, -1);
		ISP_DBG_DEBUG("isp release stream_id:%d, buf_addr:%x\r\n", stream_id, isp_buf_addr);
	}

	return NULL;
}

mm_module_t isp_module = {
	.create = isp_create,
	.destroy = isp_destroy,
	.control = isp_control,
	.handle = isp_handle,

	.new_item = isp_new_item,
	.del_item = isp_del_item,
	.rsz_item = NULL,
	.vrelease_item = isp_release_item,

	.output_type = MM_TYPE_VDSP,    // output for video algorithm
	.module_type = MM_TYPE_VSRC,    // module type is video source
	.name = "ISP"
};
