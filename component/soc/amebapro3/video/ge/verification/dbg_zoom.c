/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */


#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <signal.h>

#include <setjmp.h>
#include <inttypes.h>
#include <unistd.h>


#include <stdint.h>
#include <basic_types.h>
#include <errno.h>
#include "ameba_soc.h"


#include "rts_ge_driver.h"
#include "rts_zoom.h"

#include "dbg_ge.h"

#include "dbg_zoom.h"
#include "ge_sys_api.h"


static __u16 rgb2yuv_cnvt_coef[9] = { 0xda,  0x2dc, 0x4a,  0x76, 0x18a,
									  0x200, 0x200, 0x1d1, 0x2f
									};

static __u16 yuv2rgb_cnvt_coef[9] = { 0x100, 0x100, 0x100, 0x0, 0x30,
									  0x1db, 0x193, 0x78,  0x0
									};

static const __u16 zoom_coef[20] = { 0x0,  0x0,	 0x0,  0x0,  0x0,  0x0,	 0x0,
									 0x0,  0x0,	 0x0,  0x0,  0x0,  0x01, 0x20,
									 0x40, 0x60, 0xa0, 0xc0, 0xe0, 0xff
								   };


#error "123"

#define printf DiagPrintf

#define N_ELEMENTS(X) (sizeof(X) / sizeof(*(X)))

struct ge_context ge_context_zoom;

#define VAL_OPT_RGB2YUV 0X01
#define VAL_OPT_YUV2RGB 0x02


extern void generate_crc_table(void);
extern unsigned long crcbitbybitfast(unsigned char *p, unsigned long len);

int save_pic(__u32 src_img_fmt, __u32 src_img_order_ctrl, __u32 dst_img_fmt,
			 __u32 dst_img_order_ctrl, uint8_t *bmp, int size, __u32 dst_img_w, __u32 dst_img_h)
{
	int32_t ret = -1;
	uint8_t *buf = NULL;
	FILE *file;
	char path[64] = { 0 };

	void *buf_end = (void *)(bmp + size - 20);

	int remain_size = size;
	uint8_t *current_addr = bmp;
	int write_size;
	uint32_t current_tick, pre_tick;
	unsigned long crc32;
	DCache_Invalidate((uint32_t)bmp, size);

	hexdump(buf_end, 16);

#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	snprintf(path, 64,
			 "fat:\\zoom_result\\panda-s%ld-c%ld-d%ld-o%ld-%ldx%ld.raw",
			 src_img_fmt, src_img_order_ctrl, dst_img_fmt,
			 dst_img_order_ctrl, dst_img_w, dst_img_h);


	generate_crc_table();
	crc32 = crcbitbybitfast(bmp, size);
	DiagPrintf("ge_zoom save to:%s, %u crc32 %u\n", path, size, crc32);

	pre_tick = xTaskGetTickCount();


	file = fopen(path, "wb");


	if (file == NULL) {
		DiagPrintf("save file open failed\n");
		return -1;
	}
	fseek(file, 0, SEEK_SET);

	write_size = remain_size;
	buf = current_addr;

	ret = fwrite(buf, write_size, 1, file);

	if (ret != write_size) {
		DiagPrintf("save file failed %d\n", ret);
		fclose(file);
		return -1;
	}

	fclose(file);
	current_tick = xTaskGetTickCount();

	DiagPrintf("save successfully time %d\n", current_tick - pre_tick);

#else
	snprintf(path, 64,
			 "c:\\martin\\panda-s%ld-c%ld-d%ld-o%ld-%ldx%ld.raw",
			 src_img_fmt, src_img_order_ctrl, dst_img_fmt,
			 dst_img_order_ctrl, dst_img_w, dst_img_h);


	generate_crc_table();

	crc32 = crcbitbybitfast(bmp, size);
	DiagPrintf("ge_zoom save to:%s, %u crc32 %u\n", path, size, crc32);

	pre_tick = xTaskGetTickCount();


	file = fopen(path, "wb");


	if (file == NULL) {
		DiagPrintf("save file open failed\n");
		return -1;
	}
	fseek(file, 0, SEEK_SET);

	while (remain_size > 0) {
		if (remain_size > 0xFFF00) {
			write_size = 0xFFF00;
		} else {
			write_size = remain_size;
		}

		buf = current_addr;

		ret = fwrite(buf, write_size, 1, file);

		if (ret != 1) {
			DiagPrintf("save file failed \n");
			fclose(file);
			return -1;
		}

		remain_size -= write_size;
		current_addr += write_size;
	}

	fclose(file);
	current_tick = xTaskGetTickCount();

	DiagPrintf("save successfully time %d\n", current_tick - pre_tick);

#endif

	return 0;
}

// zoom_flag 0 宽高不变
// zoom_flag 1 宽高各放大2倍
// zoom_flag 2 宽高各缩小16倍
static inline int run_fb_zoom_proc(struct ge_context *pge_context, __u32 img_w, __u32 img_h,
								   __u32 roi_x, __u32 roi_y, __u32 half_en_flag,
								   __u32 quart_en_flag, __u32 src_img_fmt,
								   __u32 src_img_order_ctrl, __u32 dst_img_fmt,
								   __u32 dst_img_order_ctrl,
								   __u32 zoom_flag, __u32 save_flag, __u32 regression_flag,
								   __u32 crop_w, __u32 crop_h, __u32 dst_w, __u32 dst_h,
								   __u32 full_limit_flag, __u32 offset)
{
	struct ge_zoom dst;
	int ret = 0;
	int save_size = 0;
	__u32 dst_img_order_ctrl_curr = 0;
	__u32 dst_img_order_ctrl_max = 0;
	__u32 temp = 0;
	__u32 dst_ord_start = 12;
	__u32 dst_ord_end = 13;
	__u32 dst_fmt_start = dst_img_fmt;
	__u32 dst_fmt_end = dst_img_fmt;
	__u32 dst_img_fmt_regr = dst_img_fmt;
	__u32 align_src_size, align_dst_size;

	if (regression_flag == 2) {  // output format/order case

		dst_fmt_start = 0;
		dst_fmt_end = 7;
	}


	for (dst_img_fmt_regr = dst_fmt_start; dst_img_fmt_regr <= dst_fmt_end;  dst_img_fmt_regr++) {


		switch (dst_img_fmt_regr) {
		case GE_FMT_YUV422_INT:
			temp = 0;
			dst_img_order_ctrl_max = 4;
			break;
		case GE_FMT_YUV422_SMI:
			temp = 4;
			dst_img_order_ctrl_max = 2;
			break;
		case GE_FMT_YUV444_INT:
			dst_img_order_ctrl_max = 6;
			temp = 6;
			break;
		case GE_FMT_RGB888_INT:
			dst_img_order_ctrl_max = 6;
			temp = 12;
			break;
		case GE_FMT_YUV444_PLA:
			dst_img_order_ctrl_max = 1;
			temp = 6;
			break;
		case GE_FMT_RGB888_PLA:
			dst_img_order_ctrl_max = 1;
			temp = 12;
			break;
		case GE_FMT_YUV420_SMI:
			dst_img_order_ctrl_max = 2;
			temp = 4;
			break;
		case GE_FMT_RGB565:
			temp = 12;
			dst_img_order_ctrl_max = 6;
			break;
		}



		if (regression_flag == 2) {  // output format/order case
			dst_ord_start = temp;
			dst_ord_end = temp + dst_img_order_ctrl_max;
		} else if (dst_img_order_ctrl != 0xff) {   // output format/order case
			dst_ord_start = dst_img_order_ctrl;
			dst_ord_end = dst_img_order_ctrl + 1;
		}

		for (dst_img_order_ctrl_curr = dst_ord_start;
			 dst_img_order_ctrl_curr < dst_ord_end;
			 dst_img_order_ctrl_curr++) {


			memset(&dst, 0, sizeof(dst));
			dst.src_img.frame_width = img_w;
			dst.src_img.frame_height = img_h;
			dst.src_img.addr[0] = (__u32)pge_context->surface[SRC].phy_addr;
			align_src_size = ((img_w * img_h + 0xF) & 0xFFFFFFF0) - img_w * img_h;
			if (src_img_fmt == 1 || src_img_fmt == 6)
				dst.src_img.addr[1] = dst.src_img.addr[0] +
									  dst.src_img.frame_width *
									  dst.src_img.frame_height + align_src_size;
			if (src_img_fmt == 4 || src_img_fmt == 5) {
				dst.src_img.addr[1] = dst.src_img.addr[0] +
									  dst.src_img.frame_width *
									  dst.src_img.frame_height + align_src_size;
				dst.src_img.addr[2] = dst.src_img.addr[1] +
									  dst.src_img.frame_width *
									  dst.src_img.frame_height + align_src_size;
			}

			if (align_src_size) {
				DiagPrintf("[Warning] align_src_size %d, src size %d wasn't 16 alignment , it needs extra memory copy \n\r", align_src_size, img_w * img_h);
			}


			dst.src_img.fmt = src_img_fmt;
			dst.src_img.fmt_order = src_img_order_ctrl;
			dst.dst_img.fmt = dst_img_fmt_regr;
			dst.dst_img.fmt_order = dst_img_order_ctrl_curr;
			dst.zoom_start_x = roi_x;
			dst.zoom_start_y = roi_y;
			dst.crop_frame_width = crop_w;
			dst.crop_frame_height = crop_h;

			if (zoom_flag == 0) {
				//dst.zoom_step_v = 1024;
				//dst.zoom_step_h = 1024;
				dst.dst_img.frame_width = dst.crop_frame_width;
				dst.dst_img.frame_height = dst.crop_frame_height;
			} else if (zoom_flag == 1) {
				//dst.zoom_step_v = 1024 / 2;
				//dst.zoom_step_h = 1024 / 2;
				dst.dst_img.frame_width = dst.crop_frame_width * 2;
				dst.dst_img.frame_height = dst.crop_frame_height * 2;
			} else if (zoom_flag == 2) {
				//dst.zoom_step_v = 1024 * 16;
				//dst.zoom_step_h = 1024 * 16;
				dst.dst_img.frame_width = dst.crop_frame_width / 16;
				dst.dst_img.frame_height = dst.crop_frame_height / 16;
			} else {
				dst.dst_img.frame_width = dst_w;
				dst.dst_img.frame_height = dst_h;
				dst.zoom_step_v = 1024 * crop_h / dst_h;
				dst.zoom_step_h = 1024 * crop_w / dst_w;
			}
			dst.quart_en_flag = quart_en_flag;
			dst.half_en_flag = half_en_flag;
			dst.zoom_ch_en = 1;

			dst.dst_index_x = 0;
			dst.dst_index_y = 0;
			dst.dst_img_width = dst.dst_img.frame_width;
			dst.dst_img_height = dst.dst_img.frame_height;
			dst.zoom_ch_en = 1;
			dst.full_limit_flag = full_limit_flag;
			dst.offset = offset;


			DiagPrintf("src fmt %ld odr %ld w %ld h %ld  \n\r", dst.src_img.fmt, dst.src_img.fmt_order, dst.crop_frame_width, dst.crop_frame_height);
			DiagPrintf("dst fmt %ld odr %ld w %ld h %ld  \n\r", dst.dst_img.fmt, dst.dst_img.fmt_order, dst.dst_img.frame_width, dst.dst_img.frame_height);

			align_dst_size = ((dst.dst_img.frame_width * dst.dst_img.frame_height + 0xF) & 0xFFFFFFF0) - dst.dst_img.frame_width * dst.dst_img.frame_height;

			dst.dst_img.addr[0] = (__u32)pge_context->surface[DST].phy_addr;
			if (dst_img_fmt_regr == 1 || dst_img_fmt_regr == 6)
				dst.dst_img.addr[1] = dst.dst_img.addr[0] +
									  dst.dst_img.frame_width *
									  dst.dst_img.frame_height + align_dst_size;
			if (dst_img_fmt_regr == 4 || dst_img_fmt_regr == 5) {
				dst.dst_img.addr[1] = dst.dst_img.addr[0] +
									  dst.dst_img.frame_width *
									  dst.dst_img.frame_height + align_dst_size;
				dst.dst_img.addr[2] = dst.dst_img.addr[1] +
									  dst.dst_img.frame_width *
									  dst.dst_img.frame_height + align_dst_size;
			}

			if (align_dst_size) {
				DiagPrintf("[Warning] align_dst_size %d, dst size %d wasn't 16 alignment , it needs extra memory copy \n\r", align_dst_size,
						   dst.dst_img.frame_width * dst.dst_img.frame_height);
			}


			if (dst_img_fmt_regr == 2 || dst_img_fmt_regr == 3 || dst_img_fmt_regr == 4 ||
				dst_img_fmt_regr == 5)
				save_size = dst.dst_img.frame_width *
							dst.dst_img.frame_height * 3;
			else if (dst_img_fmt_regr == 0 || dst_img_fmt_regr == 1 ||
					 dst_img_fmt_regr == 7)
				save_size = dst.dst_img.frame_width *
							dst.dst_img.frame_height * 2;
			else
				save_size = dst.dst_img.frame_width *
							dst.dst_img.frame_height * 3 / 2;

			pge_context->surface[DST].size = save_size;

			DiagPrintf("zoom dst 0x%08x size %d \n", pge_context->surface[DST].phy_addr, pge_context->surface[DST].size);

			memset(pge_context->surface[DST].phy_addr, 0, pge_context->surface[DST].size);


			rts_zoom_init();

			ret = rts_ge_zoom(&dst);
			if (ret) {
				DiagPrintf("zoom init failed:  src_img: %u, %u, dst_img: %u, %u\n",
						   src_img_fmt, src_img_order_ctrl,
						   dst_img_fmt, dst_img_order_ctrl);
				return ret;
			}

			ret = rts_ge_zoom_check_end();

			if (ret) {
				DiagPrintf("zoom wait end timeout \n");
				return ret;
			}


			if (save_flag)
				save_pic(src_img_fmt, src_img_order_ctrl, dst_img_fmt_regr,
						 dst_img_order_ctrl_curr,
						 pge_context->surface[DST].phy_addr,
						 save_size, dst.dst_img.frame_width, dst.dst_img.frame_height);

		}
	}

	return ret;
}



static int run_ge_zoom(struct ge_context *pge_context, char *src_img, __u32 roi_x, __u32 roi_y,
					   __u32 half_en_flag, __u32 quart_en_flag,
					   __u32 dst_img_fmt, __u32 dst_img_order_ctrl,
					   __u32 save_flag, __u32 zoom_flag, __u32 regression_flag,
					   __u32 src_w, __u32 src_h, __u32 crop_w, __u32 crop_h, __u32 dst_w, __u32 dst_h, __u32 full_limit_flag, __u32 offset)
{
	int ret = 0;
	__u32 sno;
	//__u32 max_size = 0x400000; //request max 4M memory for each src/dst img
	__u32 src_img_fmt = GE_FMT_RGB888_INT;
	__u32 src_img_order_ctrl = 0;
	__u32 src_img_order_ctrl_max = 0;
	__u32 i;
	__u32 img_w, img_h;

	__u32 src_size = 32768;
	__u32 dst_size = 163840;
	char int_img[64] = "internal";
	char regr_img[128];

	__u32 zoom_start = 0;
	__u32 zoom_end = 2;
	__u32 fmt_start = GE_FMT_RGB888_INT;
	__u32 fmt_end = GE_FMT_RGB888_INT;
	__u32 order_start = 0;
	__u32 order_end = 1;
	__u32 current_zoom;
	__u32 align_src_size0 = 0;//, align_src_size = 0;
	__u32 in_buf_cnt = 1;

	__u32 src_buf_loc, dst_buf_loc;
	__u32 input_align = 16;


	u8 *ge_buff_src; //= (u8 *)DDRC_P0_REG_BASE + 16; //VIDEO_MEM_BASE;
	u8 *ge_buff_dst; //= (u8 *)(DDRC_P0_REG_BASE + 16 + src_size); //(u8 *)(VIDEO_MEM_BASE + src_size);

	if (!regression_flag) {

		img_w = src_w;
		img_h = src_h;
		src_img_fmt = GE_FMT_RGB888_INT;
		src_img_order_ctrl = GE_RGB_ORDER;


		switch (src_img_fmt) {
		case GE_FMT_YUV422_INT:
			src_img_order_ctrl_max = 4;
			src_size = img_w * img_h * 2;
			in_buf_cnt = 1;
			break;
		case GE_FMT_YUV422_SMI:
			src_img_order_ctrl_max = 2;
			src_size = img_w * img_h * 2;
			in_buf_cnt = 2;
			break;
		case GE_FMT_YUV444_INT:
			src_img_order_ctrl_max = 6;
			src_size = img_w * img_h * 3;
			in_buf_cnt = 1;
			break;
		case GE_FMT_RGB888_INT:
			src_img_order_ctrl_max = 6;
			src_size = img_w * img_h * 3;
			in_buf_cnt = 1;
			break;
		case GE_FMT_YUV444_PLA:
			src_img_order_ctrl_max = 1;
			src_size = img_w * img_h * 3;
			in_buf_cnt = 3;
			break;
		case GE_FMT_RGB888_PLA:
			src_img_order_ctrl_max = 1;
			src_size = img_w * img_h * 3;
			in_buf_cnt = 3;
			break;
		case GE_FMT_YUV420_SMI:
			src_img_order_ctrl_max = 2;
			src_size = img_w * img_h * 3 / 2;
			in_buf_cnt = 2;
			break;
		}


		align_src_size0 = ((img_w * img_h + 0xF) & 0xFFFFFFF0) - img_w * img_h;

		dst_size = dst_w * dst_h * 3; //largest size


		src_buf_loc = malloc(src_size + input_align * in_buf_cnt);
		dst_buf_loc = malloc(dst_size + input_align * 3);

		ge_buff_src = (src_buf_loc + 0xF) & 0xFFFFFFF0;
		ge_buff_dst = (dst_buf_loc + 0xF) & 0xFFFFFFF0;


		pge_context->surface[SRC].phy_addr = (void *)ge_buff_src;
		pge_context->surface[SRC].size = src_size;
		pge_context->surface[DST].phy_addr = (void *)ge_buff_dst;
		pge_context->surface[DST].size = dst_size;

		DiagPrintf("zoom src 0x%08x dst  0x%08x\n", ge_buff_src, ge_buff_dst);



		if (strcmp(src_img, int_img)) {
			memset(pge_context->surface[SRC].phy_addr, 0, pge_context->surface[SRC].size);
			ret = surface_loadpic(pge_context, src_img, SRC);
		} else {
			DiagPrintf("use internal pattern.\n");
			DiagPrintf(" prepare pattern @0x%08x len %d \n", pge_context->surface[SRC].phy_addr, pge_context->surface[SRC].size);
			pge_context->surface[SRC].file_len = pge_context->surface[SRC].size;
			hexdump(pge_context->surface[SRC].phy_addr, 16);
		}

		if (ret) {
			DiagPrintf("loadpic fail\n");
			return ret;
		}

		memset(pge_context->surface[DST].phy_addr, 0, pge_context->surface[DST].size);
		ret = run_fb_zoom_proc(
				  pge_context, img_w, img_h, roi_x, roi_y, half_en_flag,
				  quart_en_flag, src_img_fmt,
				  src_img_order_ctrl, dst_img_fmt,
				  dst_img_order_ctrl, zoom_flag,
				  save_flag, regression_flag, crop_w, crop_h, dst_w, dst_h, full_limit_flag, offset);

		free(src_buf_loc);
		free(dst_buf_loc);

		if (ret) {
			DiagPrintf("run_fb_zoom_proc fail, ret = %d\n", ret);
			return ret;
		}

	}	else {

		img_w = src_w;
		img_h = src_h;


		if (regression_flag == 1) {  	// src fmt/order test
			fmt_start = GE_FMT_YUV422_INT;
			fmt_end = GE_FMT_YUV420_SMI;
		}


		for (src_img_fmt = fmt_start; src_img_fmt <= fmt_end;    src_img_fmt++) {
			switch (src_img_fmt) {
			case GE_FMT_YUV422_INT:
				src_img_order_ctrl_max = 4;
				src_size = img_w * img_h * 2;
				in_buf_cnt = 1;
				break;
			case GE_FMT_YUV422_SMI:
				src_img_order_ctrl_max = 2;
				src_size = img_w * img_h * 2;
				in_buf_cnt = 2;
				break;
			case GE_FMT_YUV444_INT:
				src_img_order_ctrl_max = 6;
				src_size = img_w * img_h * 3;
				in_buf_cnt = 1;
				break;
			case GE_FMT_RGB888_INT:
				src_img_order_ctrl_max = 6;
				src_size = img_w * img_h * 3;
				in_buf_cnt = 1;
				break;
			case GE_FMT_YUV444_PLA:
				src_img_order_ctrl_max = 1;
				src_size = img_w * img_h * 3;
				in_buf_cnt = 3;
				break;
			case GE_FMT_RGB888_PLA:
				src_img_order_ctrl_max = 1;
				src_size = img_w * img_h * 3;
				in_buf_cnt = 3;
				break;
			case GE_FMT_YUV420_SMI:
				src_img_order_ctrl_max = 2;
				src_size = img_w * img_h * 3 / 2;
				in_buf_cnt = 2;
				break;
			}

			//dst_size = src_size;

			align_src_size0 = ((img_w * img_h + 0xF) & 0xFFFFFFF0) - img_w * img_h;

			dst_size = dst_w * dst_h * 3; //largest size

			src_buf_loc = malloc(src_size + input_align * in_buf_cnt);
			dst_buf_loc = malloc(dst_size + input_align * 3);

			ge_buff_src = (src_buf_loc + 0xF) & 0xFFFFFFF0;
			ge_buff_dst = (dst_buf_loc + 0xF) & 0xFFFFFFF0;

			pge_context->surface[SRC].phy_addr = (void *)ge_buff_src;
			pge_context->surface[SRC].size = src_size;
			pge_context->surface[DST].phy_addr = (void *)ge_buff_dst;
			//pge_context->surface[DST].size = dst_size;

			DiagPrintf("zoom src 0x%08x w %d h %d size %d \n", ge_buff_src, img_w, img_h, src_size);

			if (regression_flag == 1) {   // src fmt/order test
				order_start = 0;
				order_end = src_img_order_ctrl_max;
			}

			for (i = order_start; i < order_end; i++) {

				memset(pge_context->surface[SRC].phy_addr, 0, pge_context->surface[SRC].size);
				if (src_img_fmt == 0) {
					switch (i) {
					case 0:
						//ret = surface_loadpic(pge_context, "c:\\martin\\zoom\\panda.yuv422_packed_yuyv",SRC);
						src_img_order_ctrl = GE_YUYV_ORDER;
						break;
					case 1:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv422_packed_yvyu",SRC);
						src_img_order_ctrl = GE_YVYU_ORDER;
						break;
					case 2:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv422_packed_uyvy",SRC);
						src_img_order_ctrl = GE_UYVY_ORDER;
						break;
					case 3:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv422_packed_vyuy",SRC);
						src_img_order_ctrl = GE_VYUY_ORDER;
						break;
					default:
						ret = -1;
						break;
					}
					//ret = surface_loadpic(pge_context, src_img, SRC);
				}

				if (src_img_fmt == 1) {
					switch (i) {
					case 0:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv422_sp_uv",SRC);
						src_img_order_ctrl = GE_UV_ORDER;
						break;
					case 1:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv422_sp_vu",SRC);
						src_img_order_ctrl = GE_VU_ORDER;
						break;
					default:
						ret = -1;
						break;
					}
				}

				if (src_img_fmt == 2) {
					switch (i) {
					case 0:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv444_packed_YUV",SRC);
						src_img_order_ctrl = GE_YUV_ORDER;
						break;
					case 1:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv444_packed_YVU",SRC);
						src_img_order_ctrl = GE_YVU_ORDER;
						break;
					case 2:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv444_packed_UYV",SRC);
						src_img_order_ctrl = GE_UYV_ORDER;
						break;
					case 3:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv444_packed_UVY",SRC);
						src_img_order_ctrl = GE_UVY_ORDER;
						break;
					case 4:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv444_packed_VYU",SRC);
						src_img_order_ctrl = GE_VYU_ORDER;
						break;
					case 5:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.yuv444_packed_VUY",SRC);
						src_img_order_ctrl = GE_VUY_ORDER;
						break;
					default:
						ret = -1;
						break;
					}
				}

				if (src_img_fmt == 3) {
					switch (i) {
					case 0:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.rgb888_packed_rgb",SRC);
						src_img_order_ctrl = GE_RGB_ORDER;
						break;
					case 1:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.rgb888_packed_rbg",SRC);
						src_img_order_ctrl = GE_RBG_ORDER;
						break;
					case 2:
						//ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.rgb888_packed_grb",SRC);
						src_img_order_ctrl = GE_GRB_ORDER;
						break;
					case 3:
						//ret = surface_loadpic(
						//	pge_context,
						//	"c:\\martin\\zoom\\panda.rgb888_packed_gbr",
						//	SRC);
						src_img_order_ctrl = GE_GBR_ORDER;
						break;
					case 4:
//						ret = surface_loadpic(
//							pge_context,
//							"c:\\martin\\zoom\\panda.rgb888_packed_brg",
//							SRC);
						src_img_order_ctrl = GE_BRG_ORDER;
						break;
					case 5:
//						ret = surface_loadpic(
//							pge_context,
//							"c:\\martin\\zoom\\panda.rgb888_packed_bgr",
//							SRC);
						src_img_order_ctrl = GE_BGR_ORDER;
						break;
					default:
						ret = -1;
						break;
					}
				}

				if (src_img_fmt == 4) {
//					ret = surface_loadpic(
//						pge_context,
//						"c:\\martin\\zoom\\panda.yuv444_planar_YUV",
//						SRC);
					src_img_order_ctrl = GE_YUV_ORDER;
				}

				if (src_img_fmt == 5) {
//					ret = surface_loadpic(
//						pge_context,
//						"c:\\martin\\zoom\\panda.rgb888_planar_rgb",
//						SRC);
					src_img_order_ctrl = GE_RGB_ORDER;
				}

				if (src_img_fmt == 6) {
					switch (i) {
					case 0:
//						ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.nv12", SRC);
						src_img_order_ctrl = GE_UV_ORDER;
						break;
					case 1:
//						ret = surface_loadpic(pge_context,"c:\\martin\\zoom\\panda.nv21", SRC);
						src_img_order_ctrl = GE_VU_ORDER;
						break;
					default:
						ret = -1;
						break;
					}

				}



				memset(regr_img, 0, sizeof(regr_img));
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
				sprintf(regr_img, "fat:zoom\\panda-s3-c12-d%d-o%d-%dx%d.raw", src_img_fmt, src_img_order_ctrl, img_w, img_h);
#else
				sprintf(regr_img, "c:\\martin\\zoom\\panda-s3-c12-d%d-o%d-%dx%d.raw", src_img_fmt, src_img_order_ctrl, img_w, img_h);
#endif
				ret = surface_loadpic(pge_context, regr_img, SRC);
				if (align_src_size0 != 0) {
					DiagPrintf("[Warn] need source align and only support YUV 2 planner \n");
					memmove(pge_context->surface[SRC].phy_addr + img_w * img_h + align_src_size0, pge_context->surface[SRC].phy_addr + img_w * img_h,
							pge_context->surface[SRC].size - img_w * img_h);
				}
				if (ret) {
					DiagPrintf("loadpic %s fail \n", regr_img);
					continue;
				}

				if (zoom_flag > 3) {
					zoom_start = 0;
					zoom_end = 2;
				} else {
					zoom_start = zoom_flag;
					zoom_end = zoom_flag;
				}
				for (current_zoom = zoom_start; current_zoom <= zoom_end; current_zoom++) {



					//memset(pge_context->surface[DST].phy_addr, 0, pge_context->surface[DST].size);
					ret = run_fb_zoom_proc(
							  pge_context, img_w, img_h, roi_x, roi_y, half_en_flag,
							  quart_en_flag, src_img_fmt,
							  src_img_order_ctrl, dst_img_fmt,
							  dst_img_order_ctrl, current_zoom,
							  save_flag, regression_flag, crop_w, crop_h, dst_w, dst_h, full_limit_flag, offset);
					if (ret) {
						DiagPrintf("run_fb_zoom_proc fail, ret = %d\n", ret);
						return ret;
					}
				}

			}



			free(src_buf_loc);
			free(dst_buf_loc);


		}


	}

	return ret;
}

#if 0
void destructor(int signum)
{
	DiagPrintf("\ng2bench sigint %d caught\n", signum);
	ge_close(&ge_context);

	exit(0);
}
#endif

void print_usage(FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: options\n");
	fprintf(stream, " -h --help\n"
			" -f --half_en_flag\n"
			" -q --quart_en_flag\n"
			" -d --dst_img_fmt\n"
			" -o --dst_img_order_ctrl\n"
			" -x --roi_x\n"
			" -y --roi_y\n"
			" -g --save_flag"
			" -z --zoom_flag\n"
			" -u --full_limit_flag\n"
			" -s --offset\n");

	exit(exit_code);
}

/* src_img_fmt, dst_img_fmt :  YUV422_INT(0), YUV422_SMI(1), YUV444_INT(2), RGB888_INT(3),
						YUV444_PLA(4), RGB888_PLA(5), YUV420_SMI(6), RGB565(7) */
/* src_img_order_ctrl, dst_img_order_ctrl :  YUYV/YVYU/UYVY/VYUY (0 ~3) ; UV/VU (4,5) ;
	YUV/YVU/UYV/UVY/VYU/VUY (6~11) ; RGB/RBG/GRB/GBR/BRG/BGR (12~17) */
/* full_limit_flag :  1- color space full range ;  0 - color space limit range */
/* offset :  color space limit range's offset */
int run_zoom_proc(__u32 src_addr, __u32 dst_addr,
				  __u32 img_w, __u32 img_h,
				  __u32 roi_x, __u32 roi_y,
				  __u32 half_en_flag, __u32 quart_en_flag,
				  __u32 src_img_fmt, __u32 src_img_order_ctrl,
				  __u32 dst_img_fmt, __u32 dst_img_order_ctrl,
				  __u32 crop_w, __u32 crop_h, __u32 dst_w, __u32 dst_h,
				  __u32 full_limit_flag, __u32 offset)
{
	struct ge_zoom dst;
	int ret = 0;
	int save_size = 0;

	__u32 align_src_size;

	memset(&dst, 0, sizeof(dst));

	dst.src_img.frame_width = img_w;
	dst.src_img.frame_height = img_h;

	dst.src_img.addr[0] = src_addr;
	align_src_size = ((img_w * img_h + 0xF) & 0xFFFFFFF0) - img_w * img_h;
	if (align_src_size) {
		DiagPrintf("[Warn] Y src_size need align shift with %d \n", align_src_size);
	}
	if (src_img_fmt == 1 || src_img_fmt == 6)
		dst.src_img.addr[1] = dst.src_img.addr[0] +
							  dst.src_img.frame_width *
							  dst.src_img.frame_height + align_src_size;
	if (src_img_fmt == 4 || src_img_fmt == 5) {
		dst.src_img.addr[1] = dst.src_img.addr[0] +
							  dst.src_img.frame_width *
							  dst.src_img.frame_height + align_src_size;
		dst.src_img.addr[2] = dst.src_img.addr[1] +
							  dst.src_img.frame_width *
							  dst.src_img.frame_height + align_src_size;
	}

	dst.src_img.fmt = src_img_fmt;
	dst.src_img.fmt_order = src_img_order_ctrl;
	dst.dst_img.fmt = dst_img_fmt;
	dst.dst_img.fmt_order = dst_img_order_ctrl;
	dst.zoom_start_x = roi_x;
	dst.zoom_start_y = roi_y;
	dst.crop_frame_width = crop_w;
	dst.crop_frame_height = crop_h;

	dst.dst_img.frame_width = dst_w;
	dst.dst_img.frame_height = dst_h;
	dst.zoom_step_v = 1024 * crop_h / dst_h;
	dst.zoom_step_h = 1024 * crop_w / dst_w;

	dst.quart_en_flag = quart_en_flag;
	dst.half_en_flag = half_en_flag;
	dst.zoom_ch_en = 1;

	dst.dst_index_x = 0;
	dst.dst_index_y = 0;

	dst.dst_img_width = dst.dst_img.frame_width;
	dst.dst_img_height = dst.dst_img.frame_height;

	dst.full_limit_flag = full_limit_flag;
	dst.offset = offset;

	dst.zoom_ch_en = 1;


	DiagPrintf("src fmt %ld odr %ld w %ld h %ld  \n\r", dst.src_img.fmt, dst.src_img.fmt_order, dst.crop_frame_width, dst.crop_frame_height);
	DiagPrintf("dst fmt %ld odr %ld w %ld h %ld  \n\r", dst.dst_img.fmt, dst.dst_img.fmt_order, dst.dst_img.frame_width, dst.dst_img.frame_height);


	dst.dst_img.addr[0] = dst_addr;
	if (dst_img_fmt == 1 || dst_img_fmt == 6)
		dst.dst_img.addr[1] = dst.dst_img.addr[0] +
							  dst.dst_img.frame_width *
							  dst.dst_img.frame_height;
	if (dst_img_fmt == 4 || dst_img_fmt == 5) {
		dst.dst_img.addr[1] = dst.dst_img.addr[0] +
							  dst.dst_img.frame_width *
							  dst.dst_img.frame_height;
		dst.dst_img.addr[2] = dst.dst_img.addr[1] +
							  dst.dst_img.frame_width *
							  dst.dst_img.frame_height;
	}
	rts_zoom_init();
	ret = rts_ge_zoom(&dst);
	if (ret) {
		DiagPrintf("zoom init failed:  src_img: %u, %u, dst_img: %u, %u\n",
				   src_img_fmt, src_img_order_ctrl,
				   dst_img_fmt, dst_img_order_ctrl);
		return ret;
	}

	ret = rts_ge_zoom_check_end();

	if (ret) {
		DiagPrintf("zoom wait end timeout \n");
		return ret;
	}

	return ret;
}



int zoom_test(u16 argc, u8 *argv[])
{
	int ret;
	int next_option;
	__u32 half_en_flag = 0;
	__u32 quart_en_flag = 0;
	__u32 dst_img_fmt = 3;
	__u32 dst_img_order_ctrl = 0xFF;
	__u32 roi_x = 0;
	__u32 roi_y = 0;
	__u32 save_flag = 1;
	__u32 zoom_flag = 1;
	__u32 regression_flag = 0;
	__u16 rgb2yuv_cnvt_coef[9] = { 0 };
	__u16 yuv2rgb_cnvt_coef[9] = { 0 };
	__u32 src_width = 1280;
	__u32 src_height = 720;
	__u32 crop_w = 640;
	__u32 crop_h = 480;
	__u32 dst_w = 1280;
	__u32 dst_h = 960;
	__u32 full_limit_flag = 1;
	__u32 offset = 0;


	//char src_img[64] = "c:\\martin\\panda.dst";
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	char src_img[64] = "fat:zoom\\panda-s3-c12-d3-o12-1280x720.raw";
#else
	char src_img[64] = "c:\\martin\\zoom\\panda-s3-c12-d3-o12-1280x720.raw";
#endif

	const char *short_options = "hf:q:d:o:x:y:b:v:g:z:i:r:w:t:k:l:m:n:u:s:";
	const struct option long_options[] = {
		{ "help", 0, NULL, 'h' },
		{ "half_en_flag", 1, NULL, 'f' },
		{ "quart_en_flag", 1, NULL, 'q' },
		{ "dst_img_fmt", 1, NULL, 'd' },
		{ "dst_img_order_ctrl", 1, NULL, 'o' },
		{ "roi_x", 1, NULL, 'x' },
		{ "roi_y", 1, NULL, 'y' },
		{ "rgb2yuv", 1, NULL, 'b' },
		{ "yuv2rgb", 1, NULL, 'v' },
		{ "save_flag", 1, NULL, 'g' },
		{ "zoom_flag", 1, NULL, 'z' },
		{ "input_img", 1, NULL, 'i' },
		{ "regression", 1, NULL, 'r' },
		{ "width", 1, NULL, 'w' },
		{ "height", 1, NULL, 't' },
		{ "crop_w", 1, NULL, 'k' },
		{ "crop_h", 1, NULL, 'l' },
		{ "dst_w", 1, NULL, 'm' },
		{ "dst_h", 1, NULL, 'n' },
		{ "full_limit_flag", 1, NULL, 'u'},
		{ "offset", 1, NULL, 's'},

		{ NULL, 0, NULL, 0 },
	};

	//DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	//DCache_Disable();



	memset(&ge_context_zoom, 0, sizeof(ge_context_zoom));
#if 0
	signal(SIGINT, destructor);

	ret = ge_open(&ge_context);
	if (ret < 0) {
		DiagPrintf("ge_open failed!!\n");
		goto init_dev_err;
	}
#endif
	do {
		next_option = getopt_long(argc, argv, short_options,
								  long_options, NULL);

		switch (next_option) {
		case 'h':
			print_usage(stdout, 0);
			break;
		case 'f':
			half_en_flag = atoi(optarg);
			continue;
		case 'q':
			quart_en_flag = atoi(optarg);
			continue;
		case 'd':
			dst_img_fmt = atoi(optarg);
			continue;
		case 'o':
			dst_img_order_ctrl = atoi(optarg);
			continue;
		case 'x':
			roi_x = atoi(optarg);
			continue;
		case 'y':
			roi_y = atoi(optarg);
			continue;
		case 'b':
			for (int i = 0; i < 9; i++) {
				rgb2yuv_cnvt_coef[i] = atoi(argv[optind - 1 + i]);
			}
			ge_set_coef(rgb2yuv_cnvt_coef, 1);
			continue;
		case 'v':
			for (int i = 0; i < 9; i++) {
				//DiagPrintf(" coef %d %d\n", optind, atoi(argv[optind - 1 + i]));
				yuv2rgb_cnvt_coef[i] = atoi(argv[optind - 1 + i]);
			}
			ge_set_coef(yuv2rgb_cnvt_coef, 0);
			continue;
		case 'g':
			save_flag = atoi(optarg);
			continue;
		case 'z':
			zoom_flag = atoi(optarg);
			continue;
		case 'i':
			memset(src_img, 0, strlen(src_img));
			snprintf(src_img, sizeof(src_img), "%s", optarg);
			DiagPrintf(" src_img %s len %d\n", src_img, strlen(src_img));
			continue;
		case 'r':
			regression_flag = atoi(optarg);
			continue;
		case 'w':
			src_width = atoi(optarg);
			continue;
		case 't':
			src_height = atoi(optarg);
			continue;
		case 'k':
			crop_w = atoi(optarg);
			continue;
		case 'l':
			crop_h = atoi(optarg);
			continue;
		case 'm':
			dst_w = atoi(optarg);
			continue;
		case 'n':
			dst_h = atoi(optarg);
			continue;
		case 'u':
			full_limit_flag = atoi(optarg);
			continue;
		case 's':
			offset = atoi(optarg);
			continue;

		default:
			break;
		}
	} while (next_option != -1);

	if (argc > optind) {
		int i = 0;
		for (i = optind; i < argc; i++) {
			DiagPrintf("argv[%d] = %s\n", i, argv[i]);
		}
	}
	optind = 1;

	DiagPrintf("dst_img_fmt %d,  roi_x %d, roi_y %d, save_flag %d,  half_en_flag %d, quart_en_flag %d, full_limit_flag %d, offset, %d\n",
			   dst_img_fmt, roi_x, roi_y, save_flag,
			   half_en_flag, quart_en_flag, full_limit_flag, offset);


	ret = run_ge_zoom(&ge_context_zoom, src_img, roi_x, roi_y, half_en_flag,
					  quart_en_flag, dst_img_fmt, dst_img_order_ctrl,
					  save_flag, zoom_flag, regression_flag, src_width, src_height, crop_w, crop_h, dst_w, dst_h, full_limit_flag, offset);
	if (ret < 0) {
		DiagPrintf("run_ge_zoom failed!!\n");
	} else {
		DiagPrintf("run_ge_zoom done!!\n");
	}

	//ge_close(&ge_context);
	return ret;
}



int RGBp2i_test(u16 argc, u8 *argv[])
{

	__u32 half_en_flag = 0;
	__u32 quart_en_flag = 0;
	__u32 dst_img_fmt = GE_FMT_RGB888_INT;
	__u32 dst_img_order_ctrl = GE_RGB_ORDER;
	__u32 roi_x = 0;
	__u32 roi_y = 0;
	__u16 rgb2yuv_cnvt_coef[9] = { 0 };
	__u16 yuv2rgb_cnvt_coef[9] = { 0 };
	__u32 crop_w = 1280;
	__u32 crop_h = 720;
	__u32 dst_w = 1280;
	__u32 dst_h = 720;
	__u32 full_limit_flag = 1;
	__u32 offset = 0;
	__u32 img_w = 1280;
	__u32 img_h = 720;
	__u32 src_img_fmt = GE_FMT_RGB888_PLA;
	__u32 src_img_order_ctrl = GE_RGB_ORDER;
	__u32 src_size;
	__u32 in_buf_cnt;
	__u32 align_src_size0 = 0, align_src_size = 0;
	__u32 input_addr ;///= DDRC_P0_REG_BASE + 16;
	char src_img[64] = "c:\\martin\\zoom\\panda-s3-c12-d5-o12-1280x720.raw";
	//char src_img[64] = "";
	unsigned long crc32;
	struct ge_context ge_context;
	__u32 save_size;
	char **restrict endptr;
	int ret = 0;

	__u32 src_buf_loc, dst_buf_loc;
	__u32 input_align = 16;


	//DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	//DCache_Disable();

	if (argc >= 1) {
		img_w = (u32)strtoul((const char *)argv[0], endptr, 10);
		DiagPrintf(" set img_w %d \n\r", img_w);
	}

	if (argc >= 2) {
		img_h = (u32)strtoul((const char *)argv[1], endptr, 10);
		DiagPrintf(" set img_h %d \n\r", img_h);
	}

	crop_w = img_w;
	crop_h = img_h;
	dst_w = img_w;
	dst_h = img_h;

	if (argc >= 3) {
		dst_w = (u32)strtoul((const char *)argv[2], endptr, 10);
		DiagPrintf(" set dst_w %d \n\r", dst_w);
	}

	if (argc >= 4) {
		dst_h = (int)strtoul((const char *)argv[3], endptr, 10);
		DiagPrintf(" set dst_h %d \n\r", dst_h);
	}

	if (argc >= 5) {
		crop_w = (int)strtoul((const char *)argv[4], endptr, 10);
		DiagPrintf(" set crop_w %d \n\r", crop_w);
	}

	if (argc >= 6) {
		crop_h = (int)strtoul((const char *)argv[5], endptr, 10);
		DiagPrintf(" set crop_h %d \n\r", crop_h);
	}

	if (argc >= 7) {
		roi_x = (int)strtoul((const char *)argv[6], endptr, 10);
		DiagPrintf(" set roi_x %d \n\r", roi_x);
	}

	if (argc >= 8) {
		roi_y = (int)strtoul((const char *)argv[7], endptr, 10);
		DiagPrintf(" set roi_y %d \n\r", roi_y);
	}


	memset(&ge_context_zoom, 0, sizeof(ge_context_zoom));

	switch (src_img_fmt) {
	case GE_FMT_YUV422_INT:
		src_size = img_w * img_h * 2;
		in_buf_cnt = 1;
		break;
	case GE_FMT_YUV422_SMI:
		src_size = img_w * img_h * 2;
		in_buf_cnt = 2;
		break;
	case GE_FMT_YUV444_INT:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 1;
		break;
	case GE_FMT_RGB888_INT:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 1;
		break;
	case GE_FMT_YUV444_PLA:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 3;
		break;
	case GE_FMT_RGB888_PLA:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 3;
		break;
	case GE_FMT_YUV420_SMI:
		src_size = img_w * img_h * 3 / 2;
		in_buf_cnt = 2;
		break;
	default:
		DiagPrintf("GE_FMT input failed %d  \n\r", src_img_fmt);
		return;
	}


	src_buf_loc = malloc(src_size + input_align * in_buf_cnt);

	input_addr = (src_buf_loc + 0xF) & 0xFFFFFFF0;

	ge_context.surface[SRC].phy_addr = (void *)input_addr;
	ge_context.surface[SRC].size = src_size;


#if 0

	align_src_size0 = ((img_w * img_h + 0xF) & 0xFFFFFFF0) - img_w * img_h;

	if (src_img_fmt == GE_FMT_YUV420_SMI) {
		align_src_size = align_src_size0 + (((img_w * img_h / 2 + 0xF) & 0xFFFFFFF0) - img_w * img_h / 2);
	} else {
		align_src_size = align_src_size0 * in_buf_cnt;
	}
	if (align_src_size0) {
		DiagPrintf("[Warn]align_src_size for Y %d for all %d \n", align_src_size0, align_src_size);
	}
#endif




	if (dst_img_fmt == GE_FMT_YUV444_INT || dst_img_fmt == GE_FMT_RGB888_INT || dst_img_fmt == GE_FMT_YUV444_PLA ||
		dst_img_fmt == GE_FMT_RGB888_PLA) {
		save_size = dst_w * dst_h * 3;
	} else if (dst_img_fmt == GE_FMT_YUV422_INT || dst_img_fmt == GE_FMT_YUV422_SMI ||
			   dst_img_fmt == GE_FMT_RGB565) {
		save_size = dst_w * dst_h * 2;
	} else {
		save_size = dst_w * dst_h * 3 / 2;
	}

	dst_buf_loc = malloc(save_size + input_align * 3);


	ge_context.surface[DST].phy_addr = (void *)((dst_buf_loc + 0xF) & 0xFFFFFFF0);

	ge_context.surface[DST].size = save_size;

	DiagPrintf("src addr 0x%08x size %d \n", ge_context.surface[SRC].phy_addr, ge_context.surface[SRC].size);
	DiagPrintf("dst addr 0x%08x size %d \n", ge_context.surface[DST].phy_addr, ge_context.surface[DST].size);


	if (!strcmp(src_img, "")) {
		printf("use internal pattern.\n");
		printf(" prepare pattern @0x%08x len %d \n", ge_context.surface[SRC].phy_addr, ge_context.surface[SRC].size);
		ge_context.surface[SRC].file_len = ge_context.surface[SRC].size;
		hexdump(ge_context.surface[SRC].phy_addr, 16);
	} else {
		memset(ge_context.surface[SRC].phy_addr, 0, ge_context.surface[SRC].size);
		ret = surface_loadpic(&ge_context, src_img, SRC);
	}


	ret = run_zoom_proc((__u32)ge_context.surface[SRC].phy_addr,
						(__u32)ge_context.surface[DST].phy_addr,
						img_w, img_h, roi_x, roi_y, half_en_flag, quart_en_flag,
						src_img_fmt, src_img_order_ctrl, dst_img_fmt, dst_img_order_ctrl,
						crop_w, crop_h, dst_w, dst_h,
						full_limit_flag, offset);

	if (ret < 0) {
		printf("run_ge_zoom failed!!\n");
	} else {

		if (!strcmp(src_img, "")) {
			hexdump(ge_context.surface[DST].phy_addr, 16);
			generate_crc_table();
			crc32 = crcbitbybitfast((unsigned char *)(ge_context.surface[DST].phy_addr), save_size);
			DiagPrintf("ge_zoom output size %d crc32 %u\n", save_size, crc32);

		} else {
			save_pic(src_img_fmt, src_img_order_ctrl, dst_img_fmt, dst_img_order_ctrl,
					 ge_context.surface[DST].phy_addr, save_size, dst_w, dst_h);
			DiagPrintf("run_ge_zoom done!!\n");
		}

	}
	free(src_buf_loc);
	free(dst_buf_loc);


	return ret;
}



int zoom_testv2(u16 argc, u8 *argv[])
{

	__u32 half_en_flag = 0;
	__u32 quart_en_flag = 0;
	__u32 dst_img_fmt = GE_FMT_RGB888_INT;
	__u32 dst_img_order_ctrl = GE_RGB_ORDER;
	__u32 roi_x = 0;
	__u32 roi_y = 0;
	__u16 rgb2yuv_cnvt_coef[9] = { 0 };
	__u16 yuv2rgb_cnvt_coef[9] = { 0 };
	__u32 crop_w = 1280;
	__u32 crop_h = 720;
	__u32 dst_w = 1280;
	__u32 dst_h = 720;
	__u32 full_limit_flag = 1;
	__u32 offset = 0;
	__u32 img_w = 1280;
	__u32 img_h = 720;
	__u32 src_img_fmt = GE_FMT_RGB888_PLA;
	__u32 src_img_order_ctrl = GE_RGB_ORDER;
	__u32 src_size;
	__u32 in_buf_cnt;
	__u32 align_src_size0 = 0, align_src_size = 0;
	__u32 input_addr ;///= DDRC_P0_REG_BASE + 16;
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	char src_img[64] = "fat:panda-s3-c12-d5-o12-1280x720.raw";
#else
	char src_img[64] = "c:panda-s3-c12-d5-o12-1280x720.raw";
#endif
	//char src_img[64] = "";
	unsigned long crc32;
	struct ge_context ge_context;
	__u32 save_size;
	char **restrict endptr;
	int ret = 0;

	__u32 src_buf_loc, dst_buf_loc;
	__u32 input_align = 16;


	if (argc >= 1) {
		img_w = (u32)strtoul((const char *)argv[0], endptr, 10);
		DiagPrintf(" set img_w %d \n\r", img_w);
	}

	if (argc >= 2) {
		img_h = (u32)strtoul((const char *)argv[1], endptr, 10);
		DiagPrintf(" set img_h %d \n\r", img_h);
	}

	crop_w = img_w;
	crop_h = img_h;
	dst_w = img_w;
	dst_h = img_h;

	if (argc >= 3) {
		dst_w = (u32)strtoul((const char *)argv[2], endptr, 10);
		DiagPrintf(" set dst_w %d \n\r", dst_w);
	}

	if (argc >= 4) {
		dst_h = (int)strtoul((const char *)argv[3], endptr, 10);
		DiagPrintf(" set dst_h %d \n\r", dst_h);
	}

	if (argc >= 5) {
		crop_w = (int)strtoul((const char *)argv[4], endptr, 10);
		DiagPrintf(" set crop_w %d \n\r", crop_w);
	}

	if (argc >= 6) {
		crop_h = (int)strtoul((const char *)argv[5], endptr, 10);
		DiagPrintf(" set crop_h %d \n\r", crop_h);
	}

	if (argc >= 7) {
		roi_x = (int)strtoul((const char *)argv[6], endptr, 10);
		DiagPrintf(" set roi_x %d \n\r", roi_x);
	}

	if (argc >= 8) {
		roi_y = (int)strtoul((const char *)argv[7], endptr, 10);
		DiagPrintf(" set roi_y %d \n\r", roi_y);
	}


	memset(&ge_context_zoom, 0, sizeof(ge_context_zoom));

	switch (src_img_fmt) {
	case GE_FMT_YUV422_INT:
		src_size = img_w * img_h * 2;
		in_buf_cnt = 1;
		break;
	case GE_FMT_YUV422_SMI:
		src_size = img_w * img_h * 2;
		in_buf_cnt = 2;
		break;
	case GE_FMT_YUV444_INT:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 1;
		break;
	case GE_FMT_RGB888_INT:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 1;
		break;
	case GE_FMT_YUV444_PLA:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 3;
		break;
	case GE_FMT_RGB888_PLA:
		src_size = img_w * img_h * 3;
		in_buf_cnt = 3;
		break;
	case GE_FMT_YUV420_SMI:
		src_size = img_w * img_h * 3 / 2;
		in_buf_cnt = 2;
		break;
	default:
		DiagPrintf("GE_FMT input failed %d  \n\r", src_img_fmt);
		return;
	}


	src_buf_loc = malloc(src_size + input_align * in_buf_cnt);

	input_addr = (src_buf_loc + 0xF) & 0xFFFFFFF0;

	ge_context.surface[SRC].phy_addr = (void *)input_addr;
	ge_context.surface[SRC].size = src_size;


	if (dst_img_fmt == GE_FMT_YUV444_INT || dst_img_fmt == GE_FMT_RGB888_INT || dst_img_fmt == GE_FMT_YUV444_PLA ||
		dst_img_fmt == GE_FMT_RGB888_PLA) {
		save_size = dst_w * dst_h * 3;
	} else if (dst_img_fmt == GE_FMT_YUV422_INT || dst_img_fmt == GE_FMT_YUV422_SMI ||
			   dst_img_fmt == GE_FMT_RGB565) {
		save_size = dst_w * dst_h * 2;
	} else {
		save_size = dst_w * dst_h * 3 / 2;
	}

	dst_buf_loc = malloc(save_size + input_align * 3);


	ge_context.surface[DST].phy_addr = (void *)((dst_buf_loc + 0xF) & 0xFFFFFFF0);

	ge_context.surface[DST].size = save_size;

	DiagPrintf("src addr 0x%08x size %d \n", ge_context.surface[SRC].phy_addr, ge_context.surface[SRC].size);
	DiagPrintf("dst addr 0x%08x size %d \n", ge_context.surface[DST].phy_addr, ge_context.surface[DST].size);


	if (!strcmp(src_img, "")) {
		printf("use internal pattern.\n");
		printf(" prepare pattern @0x%08x len %d \n", ge_context.surface[SRC].phy_addr, ge_context.surface[SRC].size);
		ge_context.surface[SRC].file_len = ge_context.surface[SRC].size;
		hexdump(ge_context.surface[SRC].phy_addr, 16);
	} else {
		memset(ge_context.surface[SRC].phy_addr, 0, ge_context.surface[SRC].size);
		ret = surface_loadpic(&ge_context, src_img, SRC);
	}

	ge_drv_ctx_t *pge_drv_blit_ctx;
	pge_drv_blit_ctx = ge_open();

	//ge_zoom_open();

	ge_ctrl(pge_drv_blit_ctx, GEDRV_CTRL_ZOOM_OPEN, NULL);

	ge_zoom_cfg_t zoom_cfg;

	memset(&zoom_cfg, 0, sizeof(ge_zoom_cfg_t));

	zoom_cfg.src_addr = (__u32)ge_context.surface[SRC].phy_addr;
	zoom_cfg.src_buf_size = (__u32)ge_context.surface[SRC].size;
	zoom_cfg.dst_addr = (__u32)ge_context.surface[DST].phy_addr;
	zoom_cfg.dst_buf_size = (__u32)ge_context.surface[DST].size;
	zoom_cfg.img_w = img_w;
	zoom_cfg.img_h = img_h;
	zoom_cfg.roi_x = roi_x;
	zoom_cfg.roi_y = roi_y;
	zoom_cfg.half_en_flag = half_en_flag;
	zoom_cfg.quart_en_flag = quart_en_flag;
	zoom_cfg.src_img_fmt = src_img_fmt;
	zoom_cfg.src_img_order_ctrl = src_img_order_ctrl;
	zoom_cfg.dst_img_fmt = dst_img_fmt;
	zoom_cfg.dst_img_order_ctrl = dst_img_order_ctrl;
	zoom_cfg.crop_w = crop_w;
	zoom_cfg.crop_h = crop_h;
	zoom_cfg.dst_w = dst_w;
	zoom_cfg.dst_h = dst_h;
	zoom_cfg.full_limit_flag = full_limit_flag;
	zoom_cfg.offset = offset;

	//ret = ge_zoom_start(&zoom_cfg);

	ret = ge_ctrl(pge_drv_blit_ctx, GEDRV_CTRL_ZOOM_START, (void *)&zoom_cfg);
	if (ret) {
		DiagPrintf("ge_zoom_start fail %x\n", ret);
		free(src_buf_loc);
		free(dst_buf_loc);
		ge_close(pge_drv_blit_ctx);
		return ret;
	}

	ret = ge_ctrl(pge_drv_blit_ctx, GEDRV_CTRL_ZOOM_CHK_END, NULL);

	//ret = rts_ge_zoom_check_end();


	if (ret) {
		printf("run_ge_zoom failed!!\n");
	} else {

		if (!strcmp(src_img, "")) {
			hexdump(ge_context.surface[DST].phy_addr, 16);
			generate_crc_table();
			crc32 = crcbitbybitfast((unsigned char *)(ge_context.surface[DST].phy_addr), save_size);
			DiagPrintf("ge_zoom output size %d crc32 %u\n", save_size, crc32);

		} else {
			save_pic(src_img_fmt, src_img_order_ctrl, dst_img_fmt, dst_img_order_ctrl,
					 ge_context.surface[DST].phy_addr, save_size, dst_w, dst_h);
			DiagPrintf("run_ge_zoom done!!\n");
		}

	}
	free(src_buf_loc);
	free(dst_buf_loc);

	ge_close(pge_drv_blit_ctx);


	if (ret) {
		DiagPrintf("ge_zoom failed!!\n");
	} else {
		DiagPrintf("ge_zoom pass!!\n");
	}



	return ret;
}




