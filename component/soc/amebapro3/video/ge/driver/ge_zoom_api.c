/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "FreeRTOS.h"
#include "ameba_soc.h"
#include <errno.h>

#include "rts_zoom.h"
#include "rts_ge_driver.h"
#include "ge_sys_api.h"


void ge_zoom_open(void)
{
	rts_zoom_initv2();

}

int ge_zoom_start(ge_zoom_cfg_t *zoom_cfg)
{

	struct ge_zoom dst;
	int ret = 0;
	int save_size = 0;
	unsigned int align_src_size;
	unsigned int img_size_ch;
	unsigned int align_dst_size;
	unsigned int dst_size_ch;

	memset(&dst, 0, sizeof(dst));

	dst.src_img.frame_width = zoom_cfg->img_w;
	dst.src_img.frame_height = zoom_cfg->img_h;


	if ((zoom_cfg->src_addr & 0xF) != 0) {
		DiagPrintf("[Err] source address need 16 bytes alignment \n");
		return -1;
	}
	dst.src_img.addr[0] = zoom_cfg->src_addr;

	img_size_ch = zoom_cfg->img_w * zoom_cfg->img_h;
	align_src_size = ((img_size_ch + 0xF) & 0xFFFFFFF0) - img_size_ch;
	if (align_src_size) {
		DiagPrintf("[Warn] component buffer addr need align shift with %d \n", align_src_size);
	}
	if (zoom_cfg->src_img_fmt == 1 ||  zoom_cfg->src_img_fmt == 6) {
		if (zoom_cfg->src_buf_size < (img_size_ch * 2 + align_src_size)) {
			DiagPrintf("[Err] source buffer size %d is not enough for 16 bytes alignment %d \n", zoom_cfg->src_buf_size, img_size_ch * 2 + align_src_size);
			return -1;
		}
		dst.src_img.addr[1] = dst.src_img.addr[0] + img_size_ch + align_src_size;
	}
	if (zoom_cfg->src_img_fmt == 4 || zoom_cfg->src_img_fmt == 5) {
		if (zoom_cfg->src_buf_size < (img_size_ch * 3 + align_src_size * 2)) {
			DiagPrintf("[Err] source buffer size %d is not enough for 16 bytes alignment %d \n", zoom_cfg->src_buf_size, img_size_ch * 3 + align_src_size * 2);
			return -1;
		}
		dst.src_img.addr[1] = dst.src_img.addr[0] + img_size_ch + align_src_size;
		dst.src_img.addr[2] = dst.src_img.addr[1] + img_size_ch + align_src_size;
	}

	dst.src_img.fmt = zoom_cfg->src_img_fmt;
	dst.src_img.fmt_order = zoom_cfg->src_img_order_ctrl;
	dst.dst_img.fmt = zoom_cfg->dst_img_fmt;
	dst.dst_img.fmt_order = zoom_cfg->dst_img_order_ctrl;
	dst.zoom_start_x = zoom_cfg->roi_x;
	dst.zoom_start_y = zoom_cfg->roi_y;
	dst.crop_frame_width = zoom_cfg->crop_w;
	dst.crop_frame_height = zoom_cfg->crop_h;

	dst.dst_img.frame_width = zoom_cfg->dst_w;
	dst.dst_img.frame_height = zoom_cfg->dst_h;
	dst.zoom_step_v = 1024 * zoom_cfg->crop_h / zoom_cfg->dst_h;
	dst.zoom_step_h = 1024 * zoom_cfg->crop_w / zoom_cfg->dst_w;

	dst.quart_en_flag = zoom_cfg->quart_en_flag;
	dst.half_en_flag = zoom_cfg->half_en_flag;
	dst.zoom_ch_en = 1;

	dst.dst_index_x = 0;
	dst.dst_index_y = 0;

	dst.dst_img_width = dst.dst_img.frame_width;
	dst.dst_img_height = dst.dst_img.frame_height;

	dst.full_limit_flag = zoom_cfg->full_limit_flag;
	dst.offset = zoom_cfg->offset;

	if ((zoom_cfg->dst_addr & 0xF) != 0) {
		DiagPrintf("[Err] output address need 16 bytes alignment \n");
		return -1;
	}
	dst.dst_img.addr[0] = zoom_cfg->dst_addr;

	dst_size_ch = dst.dst_img.frame_width * dst.dst_img.frame_height;
	align_dst_size = ((dst_size_ch + 0xF) & 0xFFFFFFF0) - dst_size_ch;
	if (align_dst_size) {
		DiagPrintf("[Warn] component output buffer addr need align shift with %d \n", align_dst_size);
	}

	if (zoom_cfg->dst_img_fmt == 1 ||  zoom_cfg->dst_img_fmt == 6) {
		if (zoom_cfg->dst_buf_size < (dst_size_ch * 2 + align_dst_size)) {
			DiagPrintf("[Err] output buffer size %d is not enough for 16 bytes alignment %d \n", zoom_cfg->dst_buf_size, (dst_size_ch * 2 + align_dst_size));
			return -1;
		}
		dst.dst_img.addr[1] = dst.dst_img.addr[0] + dst_size_ch + align_dst_size;
	}
	if (zoom_cfg->dst_img_fmt == 4 ||  zoom_cfg->dst_img_fmt == 5) {
		if (zoom_cfg->dst_buf_size < (dst_size_ch * 3 + align_dst_size * 2)) {
			DiagPrintf("[Err] output buffer size %d is not enough for 16 bytes alignment %d \n", zoom_cfg->dst_buf_size, (dst_size_ch * 3 + align_dst_size * 2));
			return -1;
		}
		dst.dst_img.addr[1] = dst.dst_img.addr[0] + dst_size_ch + align_dst_size;
		dst.dst_img.addr[2] = dst.dst_img.addr[1] + dst_size_ch + align_dst_size;
	}

	dst.zoom_ch_en = 1;

	DiagPrintf("src fmt %ld odr %ld w %ld h %ld  \n\r", dst.src_img.fmt, dst.src_img.fmt_order, dst.crop_frame_width, dst.crop_frame_height);
	DiagPrintf("dst fmt %ld odr %ld w %ld h %ld  \n\r", dst.dst_img.fmt, dst.dst_img.fmt_order, dst.dst_img.frame_width, dst.dst_img.frame_height);


	ret = rts_ge_zoom(&dst);

	return ret;
}






