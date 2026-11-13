/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include <errno.h>
#include "FreeRTOS.h"

#include "semphr.h"
#include "os_wrapper_specific.h"


#include "rts_zoom_regs.h"
#include "rts_zoom.h"
#include "rts_blit_regs.h"
#include "rts_blit.h"
#include "rts_ge_driver.h"

#include "ge_sys_api.h"


void ge_irq_handler(void *data)
{

	u32 blit_fr_done_flag;
	u32 zm_fr_done_flag;
	u32 zm_buf_full_flag;
	zoom_int_evt_t zm_evt = { 0 };
	ge_drv_ctx_t *ge_ctx = (ge_drv_ctx_t *)data;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t ret;

	blit_fr_done_flag = blit_get_int_status();

	if (blit_fr_done_flag != 0) {
		blit_clear_int();
		rtos_sema_give(ge_ctx->xblit_Semaphore);
	}

	zm_fr_done_flag = zoom_get_int_status();
	zm_buf_full_flag = zoom_get_full_status();
	if (zm_fr_done_flag != 0 || zm_buf_full_flag != 0) {

		zoom_clear_int();
		zoom_clear_full_int();
		zm_evt.zoom_buf_full_flag = zm_buf_full_flag;
		zm_evt.zoom_frdn_flag = zm_fr_done_flag;

		ret = xQueueSendFromISR(ge_ctx->zoom_int_queue, &zm_evt, &xHigherPriorityTaskWoken);
		if (ret != pdTRUE) {
			DiagPrintf(" zoom_int_queue Queue Full \n");
		}
		if (xHigherPriorityTaskWoken) {
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}

}

ge_drv_ctx_t *ge_open(void)

{
	ge_drv_ctx_t *ctx = (ge_drv_ctx_t *)malloc(sizeof(ge_drv_ctx_t));


	memset(ctx, 0, sizeof(ge_drv_ctx_t));
	GE_hw_init();  //clock & reset


	ctx->xblit_Semaphore = xSemaphoreCreateBinary();
	if (ctx->xblit_Semaphore == NULL) {
		return EINVAL;
	}

	//ctx->xzoom_Semaphore = xSemaphoreCreateBinary();
	//if (ctx->xzoom_Semaphore == NULL) {
	//	return EINVAL;
	//}

	ctx->zoom_int_queue = xQueueCreate(1, sizeof(zoom_int_evt_t));
	if (ctx->zoom_int_queue == NULL) {
		return EINVAL;
	}

	xQueueReset(ctx->zoom_int_queue);

	ctx->IRQ_priority = INT_PRI_LOWEST;

	ctx->sema_timeout_ms = 10000;  //10s


	InterruptRegister((u32)ge_irq_handler, GE_IRQ, (u32)ctx, ctx->IRQ_priority);
	InterruptEn(GE_IRQ, 1);


	return ctx;
}

void ge_close(ge_drv_ctx_t *ctx)
{

	InterruptEn(GE_IRQ, 0);

	if (ctx->xblit_Semaphore != NULL) {
		vSemaphoreDelete(ctx->xblit_Semaphore);
		ctx->xblit_Semaphore = NULL;
	}

	if (ctx->zoom_int_queue != NULL) {
		vQueueDelete(ctx->zoom_int_queue);
		ctx->zoom_int_queue = NULL;
	}

	if (ctx) {
		free(ctx);
	}


}


int ge_blit_check_end(ge_drv_ctx_t *ctx)
{

	u32 val;

	if (NULL != ctx->xblit_Semaphore) {

		if (rtos_sema_take(ctx->xblit_Semaphore, RTOS_CONVERT_MS_TO_TICKS(ctx->sema_timeout_ms))) {
			DiagPrintf("GE blit frame done timeout\n\r");
			return -1;
		} else {
			return 0;
		}
	}
}

int ge_zoom_check_end(ge_drv_ctx_t *ctx)
{

	u32 val;
	zoom_int_evt_t zm_evt = { 0 };

	if (NULL != ctx->zoom_int_queue) {

		if (xQueueReceive(ctx->zoom_int_queue, &zm_evt, RTOS_CONVERT_MS_TO_TICKS(ctx->sema_timeout_ms)) != pdPASS) {
			DiagPrintf("GE zoom frame done timeout\n\r");
			return GE_ERR_TIMEOUT;
		} else {
			if (zm_evt.zoom_frdn_flag) {
				DiagPrintf("GE zoom frame done\n\r");
			}
			if (zm_evt.zoom_buf_full_flag) {
				DiagPrintf("GE zoom buf full \n\r");
				return GE_ERR_ZOOM_BUF_FULL;
			}
			if (!(zm_evt.zoom_frdn_flag || zm_evt.zoom_buf_full_flag)) {
				DiagPrintf("GE interrupt flags both NULLl \n\r");
				return GE_ERR_ZOOM_INT_FLAG_NULL;
			}
			return 0;
		}
	}
}

int ge_ctrl(ge_drv_ctx_t *ctx, ge_ctrl_cmd_t ctrl_cmd, void *arg)
{

	int ret = 0;
	switch (ctrl_cmd) {

	case GEDRV_CTRL_ZOOM_OPEN: {
		ge_zoom_open();
		break;
	}
	case GEDRV_CTRL_ZOOM_START: {
		ret = ge_zoom_start((ge_zoom_cfg_t *)arg);
		break;
	}
	case GEDRV_CTRL_ZOOM_CHK_END: {
		ret = ge_zoom_check_end(ctx);
		break;
	}
	case GEDRV_CTRL_ZOOM_SET_RGB2YUV: {
		rts_zoom_set_rgb2yuv(arg);
		break;
	}
	case GEDRV_CTRL_ZOOM_SET_YUV2RGB: {
		rts_zoom_set_yuv2rgb(arg);
		break;
	}
	case GEDRV_CTRL_BLIT_OPEN: {
		ge_blit_open();
		break;
	}
	case GEDRV_CTRL_BLIT_START: {
		ret = ge_blit_start((struct ge_blit *)arg);
		break;
	}
	case GEDRV_CTRL_BLIT_CHK_END: {
		ret = ge_blit_check_end(ctx);
		break;
	}
	default:
		;
	}

	return 0;
}

int ge_read(ge_drv_ctx_t *ctx, void *buf, int len)
{
	return 0;
}

int ge_write(ge_drv_ctx_t *ctx, void *buf, int len)
{
	return 0;
}

