#ifndef _MODULE_ISP_H
#define _MODULE_ISP_H

#include <stdint.h>
#include "mmf2_module.h"
#include "video_common_type.h"

// #include "isp_utils.h"
#include "isp_regs.h"
#include "isp_common.h"
#include "rts_errno.h"
//#include "isp_io_type.h"

#define CMD_ISP_SET_PARAMS      MM_MODULE_CMD(0x00)  // set parameter
#define CMD_ISP_GET_PARAMS      MM_MODULE_CMD(0x01)  // get parameter
#define CMD_ISP_STREAMID        MM_MODULE_CMD(0x02)
#define CMD_ISP_HW_SLOT         MM_MODULE_CMD(0x03)
#define CMD_ISP_SW_SLOT         MM_MODULE_CMD(0x04)
#define CMD_ISP_FPS             MM_MODULE_CMD(0x05)
#define CMD_ISP_HEIGHT          MM_MODULE_CMD(0x06)
#define CMD_ISP_WIDTH           MM_MODULE_CMD(0x07)
#define CMD_ISP_FORMAT          MM_MODULE_CMD(0x08)
#define CMD_ISP_AINR_START      MM_MODULE_CMD(0x09)  // start AI denoise
#define CMD_ISP_AINR_STOP       MM_MODULE_CMD(0x0a)  // stop AI denoise
#define CMD_ISP_AINR_CLOSE      MM_MODULE_CMD(0x0b)  // close AI denoise
#define CMD_ISP_SET_REMAP       MM_MODULE_CMD(0x0c)  // set isp remap param
#define CMD_ISP_SET_FPS         MM_MODULE_CMD(0x0d)  // set fps on running stream

#define CMD_ISP_APPLY           MM_MODULE_CMD(0x20)  // apply setting and start stream
#define CMD_ISP_STREAM_START    MM_MODULE_CMD(0x21)  // start stream only
#define CMD_ISP_STREAM_STOP     MM_MODULE_CMD(0x22)  // stop stream only

#define MODE_EXCHANGE   0		// exchange used frame and ready frame's buf.y_addr/uv_addr
#define MODE_SNAPSHOT   1		// only get ready frame from ISP
#define MODE_SKIP       2		// enqueue ready frame to ISP
#define MODE_SETUP      3		// enqueue buffer to ISP 

typedef struct isp_buf_s {
	uint32_t slot_id;
	uint32_t y_addr;
	uint32_t uv_addr;

	uint32_t timestamp;
} isp_buf_t;

typedef struct isp_param_s {
	uint32_t width;
	uint32_t height;
	uint32_t fps;
	uint32_t slot_num;
	uint32_t buff_num;
	uint32_t stream_id;
	uint32_t format;

	int ai_denoise_en;  // enable isp AI denoise

	int compress_en;  // enable isp compression in isp_remap
	int slice_mode_en; // enable isp slice mode to encoder
	isp_remap_param_t *remap_param;

	//...
} isp_params_t;

typedef struct isp_ctx_s {
	void *parent;

	isp_params_t params;
	int applied;

	mm_queue_item_t *hw_slot_item[4];

	uint32_t buf_width, buf_height;
} isp_ctx_t;

extern mm_module_t isp_module;

#endif
