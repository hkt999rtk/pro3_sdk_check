/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef __GE_SYS_API_H__
#define __GE_SYS_API_H__

#include "semphr.h"
#include <stdint.h>

typedef struct ge_drv_ctx_s {
	rtos_sema_t xblit_Semaphore;
	//rtos_sema_t xzoom_Semaphore;
	QueueHandle_t zoom_int_queue;
	int sema_timeout_ms;
	int IRQ_priority;
	;
} ge_drv_ctx_t;

typedef struct {
	unsigned char zoom_frdn_flag;
	unsigned char zoom_buf_full_flag;
} zoom_int_evt_t;

typedef enum {
	GEDRV_CTRL_ZOOM_OPEN,
	GEDRV_CTRL_ZOOM_START,
	GEDRV_CTRL_ZOOM_CHK_END,
	GEDRV_CTRL_ZOOM_SET_RGB2YUV,
	GEDRV_CTRL_ZOOM_SET_YUV2RGB,
	GEDRV_CTRL_ZOOM_CLOSE,
	GEDRV_CTRL_ZOOM_RSVD3,
	GEDRV_CTRL_BLIT_OPEN,
	GEDRV_CTRL_BLIT_START,
	GEDRV_CTRL_BLIT_CHK_END,
	GEDRV_CTRL_BLIT_CLOSE,
} ge_ctrl_cmd_t;

/* src_img_fmt, dst_img_fmt :  YUV422_INT(0), YUV422_SMI(1), YUV444_INT(2), RGB888_INT(3),
						YUV444_PLA(4), RGB888_PLA(5), YUV420_SMI(6), RGB565(7) */
/* src_img_order_ctrl, dst_img_order_ctrl :  YUYV/YVYU/UYVY/VYUY (0 ~3) ; UV/VU (4,5) ;
	YUV/YVU/UYV/UVY/VYU/VUY (6~11) ; RGB/RBG/GRB/GBR/BRG/BGR (12~17) */
/* full_limit_flag :  1- color space full range ;  0 - color space limit range */
/* offset :  color space limit range's offset */
typedef struct ge_zoom_cfg {
	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int src_buf_size;
	unsigned int dst_buf_size;
	unsigned int img_w;
	unsigned int img_h;
	unsigned int roi_x;
	unsigned int roi_y;
	unsigned int crop_w;
	unsigned int crop_h;
	unsigned int dst_w;
	unsigned int dst_h;
	unsigned char src_img_fmt;
	unsigned char src_img_order_ctrl;
	unsigned char dst_img_fmt;
	unsigned char dst_img_order_ctrl;
	unsigned int offset;
	unsigned char full_limit_flag;
	unsigned char half_en_flag;
	unsigned char quart_en_flag;
} ge_zoom_cfg_t;

#define GE_ERR_TIMEOUT -2
#define GE_ERR_ZOOM_BUF_FULL -3
#define GE_ERR_ZOOM_INT_FLAG_NULL -4

void ge_zoom_err_handler_register(int (*err_handle)(void));
ge_drv_ctx_t *ge_open(void);
void ge_close(ge_drv_ctx_t *ctx);
int ge_blit_check_end(ge_drv_ctx_t *ctx);
int ge_zoom_check_end(ge_drv_ctx_t *ctx);
int ge_ctrl(ge_drv_ctx_t *ctx, ge_ctrl_cmd_t ctrl_cmd, void *arg);

#endif /* __GE_SYS_API_H__ */
