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
#include <ctype.h>

//#include <sys/types.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <linux/fb.h>
#include <setjmp.h>
#include <inttypes.h>
#include <unistd.h>

#include <stdint.h>
#include <basic_types.h>
#include <errno.h>
#include "ameba_soc.h"

#include "rts_ge_driver.h"
#include "rts_blit.h"

#include "dbg_ge.h"

#include "dbg_blit.h"

#include "ge_sys_api.h"


#define printf DiagPrintf

struct ge_context ge_context;

#define VAL_OPT_SRC_FMT 0X01
#define VAL_OPT_DST_FMT 0x02
#define VAL_OPT_SRC_X 0x03
#define VAL_OPT_SRC_Y 0x04
#define VAL_OPT_SRC_W 0x05
#define VAL_OPT_SRC_H 0x06
#define VAL_OPT_DST_X 0x07
#define VAL_OPT_DST_Y 0x08
#define VAL_OPT_DST_W 0x09
#define VAL_OPT_DST_H 0x0a
#define VAL_OPT_TEST 0x0b

#define VAL_OPT_YBT 0x0c
#define VAL_OPT_601_RANGE 0x0d

#define VAL_OPT_A_SRC	0x0e
#define VAL_OPT_A_DST	0x0f
#define VAL_OPT_A_RLT	0x10
#define VAL_OPT_SEL_SRC	0x11
#define VAL_OPT_SEL_DST	0x12
#define VAL_OPT_SEL_RLT	0x13
#define VAL_OPT_DMA_CORNER	0x14







//u32 buf_size = 1024*128;
//unsigned char src_buffer[buf_size];
//unsigned char dst_buffer[buf_size];



#if 0
void hexdump(const void *data, size_t size)
{
	char ascii[17];
	size_t i, j;

	ascii[16] = '\0';

	for (i = 0; i < size; ++i) {
		DiagPrintf("%02X ", ((unsigned char *)data)[i]);
		if (((unsigned char *)data)[i] >= ' ' &&
			((unsigned char *)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char *)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i + 1) % 8 == 0 || i + 1 == size) {
			DiagPrintf(" ");
			if ((i + 1) % 16 == 0) {
				DiagPrintf("|  %s\n", ascii);
			} else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				if ((i + 1) % 16 <= 8) {
					DiagPrintf(" ");
				}
				for (j = (i + 1) % 16; j < 16; ++j) {
					DiagPrintf("   ");
				}
				DiagPrintf("|  %s\n", ascii);
			}
		}
	}
}

/*
 * surface 0: load to src surface
 * surface 1: load to dst surface
 */
static int surface_loadpic(struct ge_context *pge_context, char *FilePath,
						   enum SURFACE_NO sno)
{
	FILE *fp;
	int filelen;
	void *buf;
	// int ret;
	u32 *buff;


	fp = fopen(FilePath, "rb");
	if (fp == NULL) {
		DiagPrintf("Open %s fail\n", FilePath);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	int flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	DiagPrintf("file len %d\n", flen);

	if (flen > pge_context->surface[sno].size) {
		flen = pge_context->surface[sno].size;
	}

	pge_context->surface[sno].file_len = flen;

	int rlen = fread(pge_context->surface[sno].phy_addr, flen, 1, fp);

	DiagPrintf("real read %s content %d:\n \n", FilePath, rlen);

	fclose(fp);

	hexdump(pge_context->surface[sno].phy_addr, 16);
	//memcpy(pge_context->surface[sno].mmap_addr, buf, filelen);
	//hexdump(pge_context->surface[sno].mmap_addr, 16);

	//munmap(buf, filelen);
	return 0;
}
#endif
extern void generate_crc_table(void);
extern unsigned long crcbitbybitfast(unsigned char *p, unsigned long len);




int save_pic_blit(uint8_t *bmp, __u32 size, __u32 cmd, __u32 src_fmt, __u32 srcx,
				  __u32 srcy, __u32 dst_fmt, __u32 dstx, __u32 dsty, __u32 yuv_sel,
				  __u32 width, __u32 height, __u32 work_mode, __u32 rop_code,
				  __u32 idx, __u32 pure_color_wr_msb)
{
	int32_t ret = -1;
	uint8_t *buf = NULL;
	FILE *file;
	char path[128] = { 0 };

	int remain_size = size;
	uint8_t *current_addr = bmp;
	int write_size;
	uint32_t current_tick, pre_tick;
	unsigned long crc32;

	DCache_Invalidate((uint32_t)bmp, size);


#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	if (idx)
		snprintf(path, 64,
				 "fat:\\blit_result\\result-cmd%lu-s%lu-sel%lu-d%lu-idx%lu.raw",
				 cmd, src_fmt, yuv_sel, dst_fmt, idx);
	else
		snprintf(
			path, 128,
			"fat:\\blit_result\\result-cmd%lu-s%lu-srcx%lu-srcy%lu-sel%lu-d%lu-dstx%lu-dsty%lu-%lu-%lu.raw",
			cmd, src_fmt, srcx, srcy, yuv_sel, dst_fmt, dstx, dsty,
			width, height);

	if (work_mode == 1)
		snprintf(path, 128, "fat:\\blit_result\\result-cmd%lu-s%lu-srcx%lu-srcy%lu-sel%lu-d%lu-dstx%lu-dsty%lu-%lu-%lu-rop%lu.raw",
				 cmd, src_fmt, srcx, srcy, yuv_sel, dst_fmt, dstx, dsty,
				 width, height, rop_code);

	generate_crc_table();


	crc32 = crcbitbybitfast(bmp, size);
	DiagPrintf("save to:%s, %u crc32 %u\n", path, size, crc32);

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
		DiagPrintf("save file failed  %d \n", ret);
		fclose(file);
		return -1;
	}

#else

	if (idx)
		snprintf(path, 64,
				 "C:\\martin\\result-cmd%lu-s%lu-sel%lu-d%lu-idx%lu.raw",
				 cmd, src_fmt, yuv_sel, dst_fmt, idx);
	else
		snprintf(
			path, 128,
			"C:\\martin\\result-cmd%lu-s%lu-srcx%lu-srcy%lu-sel%lu-d%lu-dstx%lu-dsty%lu-%lu-%lu.raw",
			cmd, src_fmt, srcx, srcy, yuv_sel, dst_fmt, dstx, dsty,
			width, height);

	if (work_mode == 1)
		snprintf(path, 128, "C:\\martin\\result-cmd%lu-s%lu-srcx%lu-srcy%lu-sel%lu-d%lu-dstx%lu-dsty%lu-%lu-%lu-rop%lu.raw",
				 cmd, src_fmt, srcx, srcy, yuv_sel, dst_fmt, dstx, dsty,
				 width, height, rop_code);

	generate_crc_table();
	crc32 = crcbitbybitfast(bmp, size);
	DiagPrintf("save to:%s, %u crc32 %u\n", path, size, crc32);

	pre_tick = xTaskGetTickCount();

	file = fopen(path, "wb");
	if (file == NULL) {
		DiagPrintf("save file open failed\n");
		return -1;
	}

	fseek(file, 0, SEEK_SET);

	//buf = bmp;

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

#endif

	fclose(file);
	current_tick = xTaskGetTickCount();

	DiagPrintf("save successfully time %d\n", current_tick - pre_tick);


	return 0;
}

__u32 blit_dma_corner(struct ge_blit dst, __u32 dma_starting_corner)
{
	// __u32 dma_starting_corner = 0;
	__u32 src_end_x = dst.src_rect.x + dst.src_rect.w - 1;
	__u32 src_end_y = dst.src_rect.y + dst.src_rect.h - 1;
	__u32 dst_end_x = dst.dst_rect.x + dst.dst_rect.w - 1;
	__u32 dst_end_y = dst.dst_rect.y + dst.dst_rect.h - 1;
	DiagPrintf("src x %d %d y %d %d dst x %d %d y %d %d \n", dst.src_rect.x, src_end_x, dst.src_rect.y, src_end_y, dst.dst_rect.x, dst_end_x, dst.dst_rect.y,
			   dst_end_y);
	if ((dst.dst_rect.x > dst.src_rect.x) && (dst.dst_rect.x <= src_end_x) &&
		(dst_end_y == src_end_y)) { //3 or 1 dst right
		if ((dma_starting_corner == 3) || (dma_starting_corner == 1)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	} else if ((dst_end_x >= dst.src_rect.x) && (dst_end_x < src_end_x) &&
			   (dst_end_y == src_end_y)) { //0 or 2 dst left
		if ((dma_starting_corner == 0) || (dma_starting_corner == 2)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	} else if ((dst_end_y >= dst.src_rect.y) && (dst_end_y < src_end_y)) { //1 or 0 dst up
		if ((dma_starting_corner == 0) || (dma_starting_corner == 1)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	} else if ((dst.dst_rect.y > dst.src_rect.y) && (dst.dst_rect.y <= src_end_y)) {//2 or 3 dst down
		if ((dma_starting_corner == 2) || (dma_starting_corner == 3)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	}
	return dma_starting_corner;
}
#if 0
static inline int
run_fb_blit_proc(struct ge_context *pge_context, __u32 cmd, __u32 src_fmt,
				 __u32 dst_fmt, __u32 width, __u32 height, __u32 srcx,
				 __u32 srcy, __u32 src_width, __u32 src_height, __u32 dstx,
				 __u32 dsty, __u32 dst_width, __u32 dst_height, __u32 yuv_sel,
				 __u32 overlay_colorblk_flag, __u32 fill_rect_color,
				 __u32 color_key_mode, __u32 save_flag, __u32 work_mode,
				 __u32 rop_code, __u32 idx, __u32 pure_color_rd_msb, __u32 pure_color_wr_msb)
{
#endif
	int run_fb_blit_proc(struct ge_context * pge_context, u32 cmd, u32 src_fmt,
						 u32 dst_fmt, u32 width, __u32 height, __u32 srcx,
						 __u32 srcy, __u32 src_width, __u32 src_height, __u32 dstx,
						 __u32 dsty, __u32 dst_width, __u32 dst_height, __u32 yuv_sel,
						 __u32 overlay_colorblk_flag, __u32 fill_rect_color,
						 __u32 color_key_mode, __u32 save_flag, __u32 work_mode,
						 __u32 rop_code, __u32 idx, __u32 pure_color_rd_msb, __u32 pure_color_wr_msb,
						 __u32 ybtmode, __u32 bt601range, struct ge_alpha * blit_alpha, __u32 dma_starting_corner) {



		struct ge_blit dst;
		int ret = 0;
		__u32 save_size = 8;

		memset(&dst, 0, sizeof(dst));
		dst.ybt_709_flag = ybtmode;
		dst.bt601_range = bt601range;

		switch (cmd) {
		case GE_BLT_ROI_BLEND:
			dst.cmd = cmd;
			dst.work_mode = work_mode;
			if (work_mode) {
				dst.rop_code = rop_code;
			}
			dst.src_img.w = src_width;
			dst.src_img.h = src_height;
			dst.src_img.img_fmt = src_fmt;
			dst.overlay_colorblk_flag = overlay_colorblk_flag;
			if (overlay_colorblk_flag) {
				dst.src_fill_rect_color = fill_rect_color;
			}
			dst.src_rect.x = srcx;
			dst.src_rect.y = srcy;
			dst.src_rect.w = width;
			dst.src_rect.h = height;

			dst.dst_rect.x = dstx;
			dst.dst_rect.y = dsty;
			dst.dst_yuv_sel = yuv_sel;
			dst.dst_img.w = dst_width;
			dst.dst_img.h = dst_height;
			dst.dst_img.img_fmt = dst_fmt;
			//dst.ybt_709_flag = 0;




			dst.alpha.sel_src_alpha = blit_alpha->sel_src_alpha;
			//dst.alpha.sel_dst_alpha = blit_alpha->sel_dst_alpha;
			dst.alpha.sel_rst_alpha = blit_alpha->sel_rst_alpha;
			dst.alpha.src_global_alpha = blit_alpha->src_global_alpha;
			//dst.alpha.dst_global_alpha = blit_alpha->dst_global_alpha;
			dst.alpha.rst_global_alpha = blit_alpha->rst_global_alpha;

			DiagPrintf("src %d %d rst %d %d\n", blit_alpha->sel_src_alpha,
					   blit_alpha->src_global_alpha,
					   blit_alpha->sel_rst_alpha, blit_alpha->rst_global_alpha);




#if 0
			blit_alpha.src_global_alpha = 255;
			//blit_alpha.dst_global_alpha = 0;
			blit_alpha.rst_global_alpha = 255;
			blit_alpha.sel_src_alpha = 0;
			//blit_alpha.sel_dst_alpha = 0;
			blit_alpha.sel_rst_alpha = 0;
#endif








			dst.src_addr[0] = (__u32)pge_context->surface[SRC].phy_addr;
			dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
			if (yuv_sel)
				dst.dst_addr[1] =
					(__u32)pge_context->surface[DST].phy_addr +
					dst.dst_img.w * dst.dst_img.h;
			dst.color_key_mode = color_key_mode;
			if (dst.color_key_mode) {
				dst.clorkey.src_color_key_max = 2667669;//2667669; //0x28_b4_95 b_g_r
				dst.clorkey.src_color_key_min = 0;
				dst.clorkey.dst_color_key_max = 1048575; //0xf_ff_ff
				dst.clorkey.dst_color_key_min = 4095; //0x0_f_ff
				if (dst.color_key_mode == 1) {
					dst.alpha.src_global_alpha = 255;
					//dst.alpha.dst_global_alpha = 0;
				} else {
					dst.alpha.src_global_alpha = 127;
					//dst.alpha.dst_global_alpha = 127;
				}
			}
			dst.pure_color_rd_msb = pure_color_rd_msb;
			dst.pure_color_wr_msb = pure_color_wr_msb;
			if (src_fmt == 7) {
				if (pure_color_rd_msb != pure_color_wr_msb) {
					DiagPrintf("when rop and alphablend, ge driver will let pure_color_rd_msb equal pure_color_wr_msb.\n");
					DiagPrintf("pure_color_rd_msb = %d, pure_color_wr_msb = %d\n", pure_color_rd_msb, pure_color_wr_msb);
				}
			}
			break;
		case GE_BLT_SELF_BLEND:
			dst.cmd = cmd;
			dst.src_rect.x = srcx;
			dst.src_rect.y = srcy;
			dst.src_rect.w = width;
			dst.src_rect.h = height;

			dst.dst_rect.x = dstx;
			dst.dst_rect.y = dsty;
			dst.dst_rect.w = width;
			dst.dst_rect.h = height;

			dst.dst_img.w = dst_width;
			dst.dst_img.h = dst_height;
			dst.dst_img.img_fmt = dst_fmt;
			//dst.dma_starting_corner = blit_dma_corner(dst);
			if ((dma_starting_corner > 3) || (dma_starting_corner < 0)) {
				DiagPrintf("dma_starting_corner should be in 0-3, while it = %d\n", dma_starting_corner);
				return -1;
			}
			dst.dma_starting_corner = blit_dma_corner(dst, dma_starting_corner);
			if (dst.dma_starting_corner == -1) {
				DiagPrintf("dma_starting_corner = %u can not be set.\n", dst.dma_starting_corner);
				DiagPrintf("please check srcx: %u, srcy: %u, w: %u, h: %u, dstx: %u, dsty: %u\n",
						   srcx, srcy, width, height, dstx, dsty);
				return -1;
			}

			dst.alpha.sel_src_alpha = 0;
			//dst.alpha.sel_dst_alpha = 0;
			dst.alpha.sel_rst_alpha = 0;
			dst.alpha.src_global_alpha = 255;
			//dst.alpha.dst_global_alpha = 0;
			dst.alpha.rst_global_alpha = 0;

			dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
			dst.pure_color_rd_msb = pure_color_rd_msb;
			dst.pure_color_wr_msb = pure_color_wr_msb;
			if (src_fmt == 7) {
				if (pure_color_rd_msb != pure_color_wr_msb) {
					DiagPrintf("when b_cover_b, ge driver will let pure_color_rd_msb equal pure_color_wr_msb.\n");
					DiagPrintf("pure_color_rd_msb = %d, pure_color_wr_msb = %d\n", pure_color_rd_msb, pure_color_wr_msb);
				}
			}
			break;
		case GE_BLT_FILLRECT:
			dst.cmd = cmd;
			dst.src_fill_rect_color = fill_rect_color;
			dst.src_rect.w = width;
			dst.src_rect.h = height;
			dst.src_img.img_fmt = dst_fmt;

			dst.dst_rect.x = dstx;
			dst.dst_rect.y = dsty;
			dst.dst_img.w = dst_width;
			dst.dst_img.h = dst_height;
			dst.dst_img.img_fmt = dst_fmt;

			dst.alpha.sel_src_alpha = 0;
			//dst.alpha.sel_dst_alpha = 0;
			dst.alpha.sel_rst_alpha = 0;
			dst.alpha.src_global_alpha = 255;
			//dst.alpha.dst_global_alpha = 255;
			dst.alpha.rst_global_alpha = 0;

			dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
			dst.pure_color_rd_msb = pure_color_rd_msb;
			dst.pure_color_wr_msb = pure_color_wr_msb;
			break;
		case GE_BLT_FMT_CONV:
			dst.cmd = cmd;
			// yuv_sel :
			//1 rgb2yuv
			//0 yuv2rgb
			//dst.ybt_709_flag = ybtmode;
			//dst.bt601_range = bt601range;
			if (yuv_sel) {
				DiagPrintf("%s, %d\n", __func__, __LINE__);
				dst.src_rect.x = srcx;
				dst.src_rect.y = srcy;
				dst.src_img.w = src_width;
				dst.src_img.h = src_height;
				dst.src_rect.w = width;
				dst.src_rect.h = height;
				dst.dst_rect.x = dstx;
				dst.dst_rect.y = dsty;
				dst.dst_img.w = width;
				dst.dst_img.h = height;
				dst.work_mode = 2;
				dst.src_addr[0] =
					(__u32)pge_context->surface[SRC].phy_addr;
				dst.src_img.img_fmt = src_fmt;
				dst.dst_addr[0] =
					(__u32)pge_context->surface[DST].phy_addr;
				dst.dst_addr[1] =
					(__u32)pge_context->surface[DST].phy_addr +
					dst.src_img.w * dst.src_img.h;
				dst.dst_img.img_fmt = dst_fmt;
				dst.dst_yuv_sel = yuv_sel;
				dst.pure_color_rd_msb = pure_color_rd_msb;
				dst.pure_color_wr_msb = pure_color_wr_msb;
			} else {
				dst.dst_rect.x = 0;
				dst.dst_rect.y = 0;
				dst.dst_rect.w = width;
				dst.dst_rect.h = height;
				dst.dst_img.w = dst_width;
				dst.dst_img.h = dst_height;
				dst.src_rect.w = width;
				dst.src_rect.h = height;
				dst.dst_addr[0] =
					(__u32)pge_context->surface[SRC].phy_addr;
				dst.dst_addr[1] =
					(__u32)pge_context->surface[SRC].phy_addr +
					dst.dst_img.w * dst.dst_img.h;
				dst.work_mode = 0;
				dst.dst_img.img_fmt = src_fmt;
				dst.rst_img.img_fmt = dst_fmt;
				dst.rst_addr[0] =
					(__u32)pge_context->surface[DST].phy_addr;
				dst.alpha.sel_src_alpha = 0;//6;
				//dst.alpha.sel_dst_alpha = 7;
				dst.alpha.sel_rst_alpha = 1;
				dst.alpha.src_global_alpha = 0;
				//dst.alpha.dst_global_alpha = 0;
				dst.alpha.rst_global_alpha = 255;
				dst.dst_yuv_sel = 1;
				dst.pure_color_rd_msb = pure_color_rd_msb;
				dst.pure_color_wr_msb = pure_color_wr_msb;
			}
			break;
		}

		ret = rts_ge_blit(&dst);


		if (save_flag && !ret) {
			if (yuv_sel && !dst_fmt) {
				save_size = dst.dst_img.w * dst.dst_img.h * 3 / 2;
			}
			if (yuv_sel && dst_fmt) {
				save_size = dst.dst_img.w * dst.dst_img.h * 2;
			}
			if (!yuv_sel) {
				if (dst_fmt == GE_BLT_RGB888) {
					save_size = dst.dst_img.w * dst.dst_img.h * 3;
				} else if (dst_fmt == GE_BLT_ARGB8888 ||
						   dst_fmt == GE_BLT_RGBA8888) {
					save_size = dst.dst_img.w * dst.dst_img.h * 4;
				} else if (dst_fmt == GE_BLT_PURECLOR) {
					save_size = dst.dst_img.w * dst.dst_img.h / 8;
				} else if (dst_fmt == GE_BLT_ARGB1111) {
					save_size = dst.dst_img.w * dst.dst_img.h / 2;
				} else if (dst_fmt == GE_BLT_ARGB2222) {
					save_size = dst.dst_img.w * dst.dst_img.h;
				} else {
					save_size = dst.dst_img.w * dst.dst_img.h * 2;
				}
			}
			save_pic_blit(pge_context->surface[DST].phy_addr, save_size, cmd,
						  src_fmt, srcx, srcy, dst_fmt, dstx, dsty, yuv_sel,
						  width, height, work_mode, rop_code, idx, pure_color_wr_msb);
		}
		return ret;
	}



	static int run_ge_blt(struct ge_context * pge_context, __u32 cmd, char *src_img,
						  char *dst_img, __u32 src_fmt, __u32 dst_fmt, __u32 width,
						  __u32 height, __u32 srcx, __u32 srcy, __u32 src_width,
						  __u32 src_height, __u32 dstx, __u32 dsty,
						  __u32 dst_width, __u32 dst_height, __u32 yuv_sel,
						  __u32 overlay_colorblk_flag, __u32 fill_rect_color,
						  __u32 color_key_mode, __u32 save_flag, __u32 work_mode,
						  __u32 rop_code, __u32 burning_flag, __u32 test_mode,
						  __u32 pure_color_rd_msb, __u32 pure_color_wr_msb,
						  __u32 ybtmode, __u32 bt601range, struct ge_alpha * blit_alpha,
						  __u32 dma_starting_corner) {
		int ret = 0;
		//__u32 sno;
		__u32 idx = 0;
		//__u32 max_size = 1024 * 128; // 0x400000; //request max 4M memory for each src/dst


		__u32 src_size = 0;
		__u32 dst_size = 196608;

		u8 *ge_buff_src ;
		u8 *ge_buff_dst ;
		u32 src_buf_loc, dst_buf_loc;
		u32 buffer_align = 16;

#ifdef __VIDEO_SRAM__
		if (strlen(src_img) != 0) {
			src_size = 61440;
			dst_size = 135168;
		}


		ge_buff_src = (u8 *)malloc(VIDEO_MEM_BASE;
								   ge_buff_dst = (u8 *)(VIDEO_MEM_BASE + src_size);
#else

		src_size = src_width * src_height * 4;
		dst_size = dst_width * dst_height * 4;


		src_buf_loc = malloc(src_size + buffer_align);
		dst_buf_loc = malloc(dst_size + buffer_align);

		ge_buff_src = (u8 *)((src_buf_loc + (buffer_align - 1)) & ~(buffer_align - 1));
		ge_buff_dst = (u8 *)((dst_buf_loc + (buffer_align - 1)) & ~(buffer_align - 1));

		DiagPrintf("src 0x%08x %d,  dst  0x%08x %dx%d %d \n", ge_buff_src, src_size, ge_buff_dst, dst_width, dst_height, dst_size);

#endif


								   pge_context->surface[SRC].phy_addr = (void *)ge_buff_src;
								   pge_context->surface[SRC].size = src_size;
								   pge_context->surface[DST].phy_addr = (void *)ge_buff_dst;
								   pge_context->surface[DST].size = dst_size;

								   DiagPrintf("src 0x%08x %d dst  0x%08x %d \n", ge_buff_src, src_size, ge_buff_dst, dst_size);



#if 0

		for (sno = SRC; sno < MAX; sno++) {

		ret = ge_io_requestmem(pge_context, sno, max_size);
			if (ret < 0) {
				return -1;
			}

			pge_context->surface[sno].mmap_addr = (void *)mmap(
					NULL, pge_context->surface[sno].size,
					PROT_READ | PROT_WRITE, MAP_SHARED, pge_context->ge_fd,
					(unsigned long)pge_context->surface[sno].phy_addr);
			if (pge_context->surface[sno].mmap_addr == NULL) {
				DiagPrintf("mmap failed.\n");
				return -1;
			}

			DiagPrintf("%s, %d, pge_context->surface[%d].slot %d, pge_context->surface[%d].phy_addr %p, pge_context->surface[%d].mmap_addr %p, pge_context->surface[sno].size 0x%x\n",
					   __func__, __LINE__, sno, pge_context->surface[sno].slot,
					   sno, pge_context->surface[sno].phy_addr, sno,
					   pge_context->surface[sno].mmap_addr,
					   pge_context->surface[sno].size);
		}
#endif



#if 1

		if (test_mode) {

		int i;
		struct test_param {
			__u32 cmd;
			__u32 srcx;
			__u32 srcy;
			__u32 dstx;
			__u32 dsty;
			__u32 w;
			__u32 h;
		};

		struct test_param parms[] = {
				{ 0, 0, 0, 0, 0, 640, 2 },
				//{ 0, 0, 0, 0, 688, 640, 24 },
				//{ 0, 0, 448, 0, 0, 640, 24 },
				{ 0, 0, 478, 0, 718, 640, 2 },
				//{ 0, 0, 0, 0, 0, 24, 480 },
				{ 0, 0, 0, 1278, 0, 2, 480 },
				{ 0, 638, 0, 0, 0, 2, 480 },
				//{ 0, 608, 0, 1248, 0, 24, 480 },
				{ 0, 0, 0, 640, 240, 640, 480 },
				//{ 0, 0, 0, 0, 0, 320, 320 },
				//{ 0, 0, 0, 952, 0, 320, 320 },
				//{ 0, 0, 0, 0, 392, 320, 320 },
				{ 0, 0, 0, 956, 398, 320, 320 },
				//{ 0, 0, 0, 160, 320, 320, 320 },
				//{ 0, 312, 0, 0, 0, 320, 320 },
				//{ 0, 312, 0, 952, 0, 320, 320 },
				{ 0, 312, 0, 0, 398, 320, 320 },
				//{ 0, 312, 0, 952, 392, 320, 320 },
				//{ 0, 312, 0, 160, 320, 320, 320 },
				//{ 0, 0, 152, 0, 0, 320, 320 },
				{ 0, 0, 152, 956, 0, 320, 320 },
				//{ 0, 0, 152, 0, 392, 320, 320 },
				//{ 0, 0, 152, 952, 392, 320, 320 },
				//{ 0, 0, 152, 160, 320, 320, 320 },
				//{ 0, 312, 152, 0, 0, 320, 320 },
				//{ 0, 312, 152, 952, 0, 320, 320 },
				//{ 0, 312, 152, 0, 392, 320, 320 },
				{ 0, 312, 152, 956, 398, 320, 320 },
				//{ 0, 312, 152, 160, 320, 320, 320 },
				{ 0, 320, 160, 0, 0, 320, 320 },
				//{ 0, 240, 160, 952, 0, 320, 320 },
				//{ 0, 240, 160, 0, 392, 320, 320 },
				{ 0, 320, 160, 956, 392, 320, 320 },
				//{ 0, 240, 160, 160, 320, 320, 320 },
			};

			memset(pge_context->surface[SRC].phy_addr, 0, pge_context->surface[SRC].size);
			if (!overlay_colorblk_flag) {
				DiagPrintf("surface_loadpic %s \n", src_img);
				if (strlen(src_img) != 0) {
					ret = surface_loadpic(pge_context, src_img, SRC);
					if (ret) {
						DiagPrintf("surface_loadpic src_img fail\n");
						free(src_buf_loc);
						free(dst_buf_loc);
						return ret;
					}
				}
			} else {
				DiagPrintf("src as rect fill \n");
			}


#if 0
			for (i = 0; i < sizeof(parms) / sizeof(parms[0]); i++)
#endif

			{
#if 0
				//cmd = parms[i].cmd;

				srcx = parms[i].srcx;
				srcy = parms[i].srcy;
				dstx = parms[i].dstx;
				dsty = parms[i].dsty;
				width = parms[i].w;
				height = parms[i].h;
#endif



				memset(pge_context->surface[DST].phy_addr, 0, pge_context->surface[DST].size);

				DiagPrintf("surface_loadpic %s \n", dst_img);


				if (strlen(dst_img) != 0) {
					ret = surface_loadpic(pge_context, dst_img, DST);
					if (ret) {
						DiagPrintf("surface_loadpic dst_img fail\n");
						free(src_buf_loc);
						free(dst_buf_loc);
						return ret;
					}
				}


				//for (work_mode = 0; (cmd == 0) && (work_mode <= 1);
				//	 work_mode++) {
				{
					//work_mode = 1;
					if (work_mode == 0) {
						memset(pge_context->surface[DST].phy_addr, 	0, pge_context->surface[DST].size);
						if (strlen(dst_img) != 0) {
							ret = surface_loadpic(pge_context, dst_img,	DST);
							if (ret) {
								DiagPrintf("surface_loadpic dst_img fail\n");
								free(src_buf_loc);
								free(dst_buf_loc);
								return ret;
							}
						}
						ret = run_fb_blit_proc(
								  pge_context, cmd, src_fmt,
								  dst_fmt, width, height, srcx,
								  srcy, src_width, src_height,
								  dstx, dsty, dst_width,
								  dst_height, yuv_sel,
								  overlay_colorblk_flag,
								  fill_rect_color, color_key_mode,
								  save_flag, work_mode, rop_code,
								  idx, pure_color_rd_msb,
								  pure_color_wr_msb,
								  ybtmode, bt601range, blit_alpha,
								  dma_starting_corner);

						if (ret) {
							DiagPrintf("run_fb_blit_proc fail, ret = %d\n", ret);
							free(src_buf_loc);
							free(dst_buf_loc);
							return ret;
						}
					}

					if (work_mode == 1) {
						for (rop_code = 0; rop_code <= 15; rop_code++) {

							memset(pge_context->surface[DST].phy_addr, 0, pge_context->surface[DST].size);
							DiagPrintf("surface_loadpic  %s\n", dst_img);
							if (strlen(dst_img) != 0) {
								ret = surface_loadpic(pge_context, dst_img, DST);
								if (ret) {
									DiagPrintf("surface_loadpic dst_img fail\n");
									free(src_buf_loc);
									free(dst_buf_loc);
									return ret;
								}
							}

							ret = run_fb_blit_proc(
									  pge_context, cmd,
									  src_fmt, dst_fmt, width,
									  height, srcx, srcy,
									  src_width, src_height,
									  dstx, dsty,
									  dst_width, dst_height, yuv_sel,
									  overlay_colorblk_flag,
									  fill_rect_color,
									  color_key_mode,
									  save_flag, work_mode,
									  rop_code, idx,
									  pure_color_rd_msb,
									  pure_color_wr_msb,
									  ybtmode, bt601range, blit_alpha,
									  dma_starting_corner);
							if (ret) {
								DiagPrintf("run_fb_blit_proc rop %d fail, ret = %d\n", rop_code, ret);
								break;
								free(src_buf_loc);
								free(dst_buf_loc);
								return ret;
							}
						}
						DiagPrintf("run_fb_blit_proc rop pass , run  %d\n", rop_code);
					}
				}
			}
		} else {
#endif
			do {
				memset(pge_context->surface[SRC].phy_addr, 0, pge_context->surface[SRC].size);
				memset(pge_context->surface[DST].phy_addr, 0, pge_context->surface[DST].size);

				DiagPrintf("surface_loadpic %s %s\n", src_img, dst_img);

				if (!overlay_colorblk_flag) {
					DiagPrintf("surface_loadpic %s \n", src_img);
					if (strlen(src_img) != 0) {
						ret = surface_loadpic(pge_context, src_img, SRC);
						if (ret) {
							DiagPrintf("surface_loadpic src_img fail\n");
							free(src_buf_loc);
							free(dst_buf_loc);
							return ret;
						}
					}
				} else {
					DiagPrintf("src as rect fill \n");
				}





				if (strlen(dst_img) != 0) {
					ret = surface_loadpic(pge_context, dst_img, DST);
					if (ret) {
						DiagPrintf("surface_loadpic dst_img fail\n");
						free(src_buf_loc);
						free(dst_buf_loc);
						return ret;
					}
				}



				ret = run_fb_blit_proc(
						  pge_context, cmd, src_fmt, dst_fmt, width,
						  height, srcx, srcy, src_width, src_height, dstx,
						  dsty, dst_width, dst_height, yuv_sel,
						  overlay_colorblk_flag, fill_rect_color,
						  color_key_mode, save_flag, work_mode, rop_code,
						  idx, pure_color_rd_msb, pure_color_wr_msb,
						  ybtmode, bt601range, blit_alpha, dma_starting_corner);

				if (ret) {
					DiagPrintf("run_fb_blit_proc fail, ret = %d\n", ret);
					break;
					free(src_buf_loc);
					free(dst_buf_loc);
					return ret;
				}
				idx++;
			} while (burning_flag);
		}
		free(src_buf_loc);
		free(dst_buf_loc);
		return ret;
	}


#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	char output_img_RGB[9][64] = {
		"fat:city\\city.rgb565",
		"fat:city\\city.argb1555",
		"fat:city\\city.argb4444",
		"fat:city\\city.argb8888",
		"fat:city\\city.rgba8888",
		"fat:city\\city.rgb888",
		"fat:city\\city.pure_color",
		"fat:city\\city.argb1111",
		"fat:city\\city.argb2222"
	};
	char output_img_YUV[2][64] = {
		"fat:city\\city.yuv420sp_nv12",
		"fat:city\\city.yuv422sp_uv"
	};

	char input_img_RGB[10][64] = {
		"fat:panda\\panda.index8",
		"fat:panda\\panda.rgb565",
		"fat:panda\\panda.argb1555",
		"fat:panda\\panda.argb4444",
		"fat:panda\\panda.argb8888",
		"fat:panda\\panda.rgba8888",
		"fat:panda\\panda.rgb888",
		"fat:panda\\panda.pure_color",
		"fat:panda\\panda.argb1111",
		"fat:panda\\panda.argb2222"
	};

#else
	char output_img_RGB[9][64] = {
		"c:\\martin\\city\\city.rgb565",
		"c:\\martin\\city\\city.argb1555",
		"c:\\martin\\city\\city.argb4444",
		"c:\\martin\\city\\city.argb8888",
		"c:\\martin\\city\\city.rgba8888",
		"c:\\martin\\city\\city.rgb888",
		"c:\\martin\\city\\city.pure_color",
		"c:\\martin\\city\\city.argb1111",
		"c:\\martin\\city\\city.argb2222"
	};
	char output_img_YUV[2][64] = {
		"c:\\martin\\city\\city.yuv420sp_nv12",
		"c:\\martin\\city\\city.yuv422sp_uv"
	};

	char input_img_RGB[10][64] = {
		"c:\\martin\\panda\\panda.index8",
		"c:\\martin\\panda\\panda.rgb565",
		"c:\\martin\\panda\\panda.argb1555",
		"c:\\martin\\panda\\panda.argb4444",
		"c:\\martin\\panda\\panda.argb8888",
		"c:\\martin\\panda\\panda.rgba8888",
		"c:\\martin\\panda\\panda.rgb888",
		"c:\\martin\\panda\\panda.pure_color",
		"c:\\martin\\panda\\panda.argb1111",
		"c:\\martin\\panda\\panda.argb2222"
	};
#endif




	int blit_test(u16 argc, u8 * argv[]) {
		int ret;
		int next_option = 0;
#ifdef __VIDEO_SRAM__
		__u32 cmd = 2;
		char input_img[64] = { 0 };
		char output_img[64] = "c:\\martin\\panda.dst"; // { 0 };
		__u32 src_fmt = 6;
		__u32 dst_fmt = 6;
		__u32 width = 100;
		__u32 height = 40;
		__u32 srcx = 50;
		__u32 srcy = 10;
		__u32 src_width = 320;
		__u32 src_height = 64;
		__u32 dstx = 10;
		__u32 dsty = 50;
		__u32 dst_width = 320;
		__u32 dst_height = 200;
		__u32 yuv_sel = 0;
		__u32 overlay_colorblk_flag = 0;
		__u32 fill_rect_color = 4293931184;
		__u32 color_key_mode = 0;
		__u32 save_flag = 1;
		__u32 burning_flag = 0;
		__u32 work_mode = 0;
		__u32 rop_code = 0;
		__u32 test_mode = 0;
		__u32 pure_color_rd_msb = 0;
		__u32 pure_color_wr_msb = 0;
#else
		__u32 cmd = 2;
		char input_img[64] = "c:\\martin\\panda\\panda.rgb888";
		char output_img[64] = "c:\\martin\\city\\city.rgb888"; // { 0 };
		__u32 src_fmt = 6;
		__u32 dst_fmt = 6;
		__u32 width = 320;
		__u32 height = 320;
		__u32 srcx = 0;//240;
		__u32 srcy = 0;//160;
		__u32 src_width = 640;
		__u32 src_height = 480;
		__u32 dstx = 0;//160;
		__u32 dsty = 0;//320;
		__u32 dst_width = 1280;
		__u32 dst_height = 720;
		__u32 yuv_sel = 0;
		__u32 overlay_colorblk_flag = 0;
		__u32 fill_rect_color = 4293931184;
		__u32 color_key_mode = 0;
		__u32 save_flag = 1;
		__u32 burning_flag = 0;
		__u32 work_mode = 0;
		__u32 rop_code = 0;
		__u32 test_mode = 0;
		__u32 pure_color_rd_msb = 0;
		__u32 pure_color_wr_msb = 0;
		__u32 dma_starting_corner = 0;
		__u32 ybtmode = 1;
		__u32 bt601range = 1;
#endif
		struct ge_alpha blit_alpha;

		blit_alpha.src_global_alpha = 255;
		//blit_alpha.dst_global_alpha = 0;
		blit_alpha.rst_global_alpha = 255;
		blit_alpha.sel_src_alpha = 0;
		//blit_alpha.sel_dst_alpha = 0;
		blit_alpha.sel_rst_alpha = 0;


		__u32 set_dst_flag = FALSE, set_src_flag = FALSE, set_src_img = FALSE, set_dst_img = FALSE;
		const char *short_options = "hc:i:o:z:a:w:t:x:y:j:q:e:f:u:v:l:b:r:k:g:n:m:p:d:s:";
		const struct option long_options[] = {
			{ "help", 0, NULL, 'h' },
			{ "cmd", 1, NULL, 'c' },
			{ "input_img", 1, NULL, 'i' },
			{ "output_img", 1, NULL, 'o' },
			{ "src_fmt", 1, NULL, 'z' },
			{ "dst_fmt", 1, NULL, 'a' },
			{ "width", 1, NULL, 'w' },
			{ "height", 1, NULL, 't' },
			{ "srcx", 1, NULL, 'x' },
			{ "srcy", 1, NULL, 'y' },
			{ "src_width", 1, NULL, 'j' },
			{ "src_height", 1, NULL, 'q' },
			{ "dstx", 1, NULL, 'e' },
			{ "dsty", 1, NULL, 'f' },
			{ "dst_width", 1, NULL, 'u' },
			{ "dst_height", 1, NULL, 'v' },
			{ "yuv_sel", 1, NULL, 'l' },
			{ "overlay_colorblk_flag", 1, NULL, 'b' },
			{ "fill_rect_color", 1, NULL, 'r' },
			{ "color_key_mode", 1, NULL, 'k' },
			{ "save_flag", 1, NULL, 'g' },
			{ "burning_flag", 1, NULL, 'n' },
			{ "work_mode", 1, NULL, 'm' },
			{ "rop_code", 1, NULL, 'p' },
			{ "t_m", 1, NULL, VAL_OPT_TEST },
			{ "pure_color_rd_msb", 1, NULL, 'd'},
			{ "pure_color_wr_msb", 1, NULL, 's'},
			{ "ybt", 1, NULL, VAL_OPT_YBT },
			{ "601rng", 1, NULL, VAL_OPT_601_RANGE },
			{ "src_a", 1, NULL, VAL_OPT_A_SRC },
			{ "dst_a", 1, NULL, VAL_OPT_A_DST },
			{ "rlt_a", 1, NULL, VAL_OPT_A_RLT },
			{ "src_sel", 1, NULL, VAL_OPT_SEL_SRC },
			{ "dst_sel", 1, NULL, VAL_OPT_SEL_DST },
			{ "rlt_sel", 1, NULL, VAL_OPT_SEL_RLT},
			{ "s_cnr", 1, NULL, VAL_OPT_DMA_CORNER},
			{ NULL, 0, NULL, 0 },
		};

		//DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
		//DCache_Disable();

		memset(&ge_context, 0, sizeof(ge_context));

		//signal(SIGINT, destructor);

		//ret = ge_open(&ge_context);
		//if (ret < 0) {
		//	DiagPrintf("ge_open failed!!\n");
		//	goto init_dev_err;
		//}
		//if (argc >= 2) {
		//	DiagPrintf(" %d %s %s %s %s \n", argc, argv[0], argv[1], argv[argc - 2], argv[argc - 1]);
		//}

		do {


			next_option = getopt_long(argc, argv, short_options,
									  long_options, NULL);
			//DiagPrintf(" %d %s %d \n", next_option, optarg, optind);



			switch (next_option) {
			case 'h':
				//print_usage(stdout, 0);
				break;
			case 'c':
				cmd = atoi(optarg);
				continue;
			case 'z':
				src_fmt = atoi(optarg);
				set_src_flag = TRUE;
				continue;
			case 'a':
				dst_fmt = atoi(optarg);
				set_dst_flag = TRUE;
				continue;
			case 'i':
				memset(input_img, 0, sizeof(input_img));
				snprintf(input_img, sizeof(input_img), "%s", optarg);
				set_src_img = TRUE;
				continue;
			case 'o':
				memset(output_img, 0, sizeof(output_img));
				snprintf(output_img, sizeof(output_img), "%s", optarg);
				set_dst_img = TRUE;
				continue;
			case 'w':
				width = atoi(optarg);
				continue;
			case 't':
				height = atoi(optarg);
				continue;
			case 'x':
				srcx = atoi(optarg);
				continue;
			case 'y':
				srcy = atoi(optarg);
				continue;
			case 'j':
				src_width = atoi(optarg);
				continue;
			case 'q':
				src_height = atoi(optarg);
				continue;
			case 'e':
				dstx = atoi(optarg);
				continue;
			case 'f':
				dsty = atoi(optarg);
				continue;
			case 'u':
				dst_width = atoi(optarg);
				continue;
			case 'v':
				dst_height = atoi(optarg);
				continue;
			case 'l':
				yuv_sel = atoi(optarg);
				continue;
			case 'b':
				overlay_colorblk_flag = atoi(optarg);
				continue;
			case 'r':
				fill_rect_color = atoll(optarg);
				continue;
			case 'k':
				color_key_mode = atoi(optarg);
				continue;
			case 'g':
				save_flag = atoi(optarg);
				continue;
			case 'n':
				burning_flag = atoi(optarg);
				continue;
			case 'm':
				work_mode = atoi(optarg);
				continue;
			case 'p':
				rop_code = atoi(optarg);
				continue;
			case VAL_OPT_TEST:
				test_mode = atoi(optarg);
				continue;
			case 'd':
				pure_color_rd_msb = atoi(optarg);
				continue;
			case 's':
				pure_color_wr_msb = atoi(optarg);
				continue;
			case '?':
				DiagPrintf("unknown option %c\n", isprint(optopt) ? optopt : '#');
				continue;
			case VAL_OPT_YBT :
				ybtmode = atoi(optarg);
				continue;
			case VAL_OPT_601_RANGE:
				bt601range = atoi(optarg);
				continue;
			case VAL_OPT_A_SRC:
				blit_alpha.src_global_alpha = atoi(optarg);
				continue;
			//case VAL_OPT_A_DST:
			//	blit_alpha.dst_global_alpha = atoi(optarg);
			//	continue;
			case VAL_OPT_A_RLT:
				blit_alpha.rst_global_alpha = atoi(optarg);
				continue;
			case VAL_OPT_SEL_SRC:
				blit_alpha.sel_src_alpha = atoi(optarg);
				continue;
			//case VAL_OPT_SEL_DST:
			//	blit_alpha.sel_dst_alpha = atoi(optarg);
			//	continue;
			case VAL_OPT_SEL_RLT:
				blit_alpha.sel_rst_alpha = atoi(optarg);
				continue;
			case VAL_OPT_DMA_CORNER:
				dma_starting_corner = atoi(optarg);
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

		if (set_dst_flag && !set_dst_img) {
			memset(output_img, 0, sizeof(output_img));
			if (yuv_sel) {
				if (dst_fmt >= 2) {
					DiagPrintf("error yuv dstfmt  = %d, reset to 0 \n", dst_fmt);
					dst_fmt = 0;
				}
				strncpy(output_img, output_img_YUV[dst_fmt], sizeof(output_img_YUV[dst_fmt]));
			} else {
				if (dst_fmt >= 10 || dst_fmt == 0) {
					DiagPrintf("error rgb dstfmt  = %d, reset to 6 \n", dst_fmt);
					dst_fmt = 6;
				}
				strncpy(output_img, output_img_RGB[dst_fmt - 1], sizeof(output_img_RGB[dst_fmt - 1]));
			}
		}

		if (set_src_flag && !set_src_img) {
			memset(input_img, 0, sizeof(input_img));

			if (src_fmt >= 10) {
				DiagPrintf("error srcfmt  = %d, reset to 0 \n", src_fmt);
				src_fmt = 0;
			}
			strncpy(input_img, input_img_RGB[src_fmt], sizeof(input_img_RGB[src_fmt]));

		}
		DiagPrintf("cmd %u, input_img %s, output_img %s, src_fmt %u, dst_fmt %u, width %u, height %u, srcx %u, srcy %u, srcw %u, srch %u\n",
				   cmd, input_img, output_img, src_fmt, dst_fmt, width, height,  srcx, srcy, src_width, src_height) ;
		DiagPrintf("dstx %u, dsty %u, dstw %u, dsth %u ,yuv_sel %u, overlay_colorblk_flag %u,fill_rect_color %u, color_key_mode %u\n",
				   dstx, dsty, dst_width, dst_height, yuv_sel, overlay_colorblk_flag, fill_rect_color, color_key_mode);
		DiagPrintf(" save_flag %u, work_mode %u, rop_code %u	, burning_flag %u, test_mode %u, pure_color_rd_msb %u, pure_color_wr_msb %u\n",
				   save_flag, work_mode, rop_code, burning_flag, test_mode, pure_color_rd_msb, pure_color_wr_msb);



#if 1
		ret = run_ge_blt(&ge_context, cmd, input_img, output_img, src_fmt,
						 dst_fmt, width, height, srcx, srcy, src_width,
						 src_height, dstx, dsty, dst_width, dst_height, yuv_sel,
						 overlay_colorblk_flag, fill_rect_color, color_key_mode,
						 save_flag, work_mode, rop_code, burning_flag,
						 test_mode, pure_color_rd_msb, pure_color_wr_msb,
						 ybtmode, bt601range, &blit_alpha, dma_starting_corner);
#endif
		if (ret < 0) {
			DiagPrintf("run_ge_blt failed!!\n");
		} else {
			DiagPrintf("run_ge_blt done!!\n");
		}

		return ret;

		//ge_close(&ge_context);
//init_dev_err:
//		DiagPrintf("%s exit with %d!\n", argv[0], ret);

//		return ret;
	}

	//static unsigned char GEbuff[430 * 1024];
	static unsigned char GEbuff[128];

	int test_load(char *FilePath, u32 size) {
		struct ge_context ge_context0;
		//unsigned char GEbuff[256*1024];
		int ret = 0;

		//__u32 max_size = 1024*256;  // 0x400000; //request max 4M memory for each src/dst

		//ge_context0.surface[SRC].phy_addr = (void *)malloc(size);

		ge_context0.surface[SRC].phy_addr = (void *)&GEbuff[0];

		if (ge_context0.surface[SRC].phy_addr == NULL) {
			DiagPrintf("malloc err\n");
			return -1;
		} else {
			DiagPrintf("malloc %d 0x%08x\n", size, ge_context0.surface[SRC].phy_addr);
		}

		ge_context0.surface[SRC].size = size;

		ret = surface_loadpic(&ge_context0, FilePath, SRC);

		return ret;



	}

	int run_ge_blt_v2(char *src_img, char *dst_img,
					  struct ge_blit * blit_cfg,
					  __u32 save_flag, __u32 burning_flag, __u32 test_mode) {

		int ret = 0;

		__u32 src_size;
		__u32 dst_size;

		u8 *ge_buff_src ;
		u8 *ge_buff_dst ;
		u32 src_buf_loc, dst_buf_loc;
		u32 buffer_align = 16;
		u32 src_width = blit_cfg->src_img.w;
		u32 src_height = blit_cfg->src_img.h;
		u32 dst_width = blit_cfg->dst_img.w;
		u32 dst_height = blit_cfg->dst_img.h;
		struct ge_context ge_blit_context;
		u32 output_yuv;
		u32 dst_fmt, src_fmt;
		u32 save_size;
		ge_drv_ctx_t *pge_drv_blit_ctx;

		src_size = src_width * src_height * 4;
		dst_size = dst_width * dst_height * 4;


		src_buf_loc = malloc(src_size + buffer_align);
		dst_buf_loc = malloc(dst_size + buffer_align);

		ge_buff_src = (u8 *)((src_buf_loc + (buffer_align - 1)) & ~(buffer_align - 1));
		ge_buff_dst = (u8 *)((dst_buf_loc + (buffer_align - 1)) & ~(buffer_align - 1));

		memset(&ge_blit_context, 0, sizeof(ge_blit_context));



		ge_blit_context.surface[SRC].phy_addr = (void *)ge_buff_src;
		ge_blit_context.surface[SRC].size = src_size;
		ge_blit_context.surface[DST].phy_addr = (void *)ge_buff_dst;
		ge_blit_context.surface[DST].size = dst_size;

		DiagPrintf("src 0x%08x %d %dx%d, dst  0x%08x %d %dx%d \n", ge_buff_src, src_size, src_width, src_height, ge_buff_dst, dst_size, dst_width, dst_height);


		memset(ge_blit_context.surface[SRC].phy_addr, 0, ge_blit_context.surface[SRC].size);
		memset(ge_blit_context.surface[DST].phy_addr, 0, ge_blit_context.surface[DST].size);

		DiagPrintf("surface_loadpic %s %s\n", src_img, dst_img);

		if (!blit_cfg->overlay_colorblk_flag) {
			DiagPrintf("surface_loadpic %s \n", src_img);
			if (strlen(src_img) != 0) {
				ret = surface_loadpic(&ge_blit_context, src_img, SRC);
				if (ret) {
					DiagPrintf("surface_loadpic src_img fail\n");
					free(src_buf_loc);
					free(dst_buf_loc);
					return ret;
				}
			}
		} else {
			DiagPrintf("src as rect fill \n");
		}

		if (strlen(dst_img) != 0) {
			ret = surface_loadpic(&ge_blit_context, dst_img, DST);
			if (ret) {
				DiagPrintf("surface_loadpic dst_img fail\n");
				free(src_buf_loc);
				free(dst_buf_loc);
				return ret;
			}
		}


		pge_drv_blit_ctx = ge_open();


		ret = run_fb_blit_proc_v2(&ge_blit_context, blit_cfg);
		if (ret) {
			DiagPrintf("run_fb_blit_proc_v2 fail %x\n", ret);
			free(src_buf_loc);
			free(dst_buf_loc);
			return ret;
		}


		ge_blit_check_end(pge_drv_blit_ctx);


		if (save_flag && !ret) {

			output_yuv = blit_cfg->dst_yuv_sel;
			dst_fmt = blit_cfg->dst_img.img_fmt;
			src_fmt = blit_cfg->src_img.img_fmt;

			if (output_yuv && !dst_fmt) {
				save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h * 3 / 2;
			}
			if (output_yuv && dst_fmt) {
				save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h * 2;
			}
			if (!output_yuv) {
				if (dst_fmt == GE_BLT_RGB888) {
					save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h * 3;
				} else if (dst_fmt == GE_BLT_ARGB8888 ||
						   dst_fmt == GE_BLT_RGBA8888) {
					save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h * 4;
				} else if (dst_fmt == GE_BLT_PURECLOR) {
					save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h / 8;
				} else if (dst_fmt == GE_BLT_ARGB1111) {
					save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h / 2;
				} else if (dst_fmt == GE_BLT_ARGB2222) {
					save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h;
				} else {
					save_size = blit_cfg->dst_img.w * blit_cfg->dst_img.h * 2;
				}
			}
			save_pic_blit(ge_blit_context.surface[DST].phy_addr, save_size, blit_cfg->cmd,
						  src_fmt, blit_cfg->src_rect.x, blit_cfg->src_rect.y, dst_fmt, blit_cfg->dst_rect.x, blit_cfg->dst_rect.y, output_yuv,
						  blit_cfg->src_rect.w, blit_cfg->src_rect.h, blit_cfg->work_mode, blit_cfg->rop_code, 0, blit_cfg->pure_color_wr_msb);
		}

		if (ret) {
			DiagPrintf("run_fb_blit_proc fail, ret = %x\n", ret);
		}

		free(src_buf_loc);
		free(dst_buf_loc);

		ge_close(pge_drv_blit_ctx);

		return ret;
	}




	int blit_test_v3(u16 argc, u8 * argv[]) {
		int ret;
		int next_option = 0;

		__u32 cmd = 2;
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
		char input_img[64] = "fat:\\panda\\panda.rgb888";
		char output_img[64] = "fat:\\city\\city.rgb888"; // { 0 };

#else
		char input_img[64] = "c:\\martin\\panda\\panda.rgb888";
		char output_img[64] = "c:\\martin\\city\\city.rgb888"; // { 0 };
#endif
		__u32 src_fmt = 6;
		__u32 dst_fmt = 6;
		__u32 width = 320;
		__u32 height = 320;
		__u32 srcx = 0;//240;
		__u32 srcy = 0;//160;
		__u32 src_width = 640;
		__u32 src_height = 480;
		__u32 dstx = 0;//160;
		__u32 dsty = 0;//320;
		__u32 dst_width = 1280;
		__u32 dst_height = 720;
		__u32 yuv_sel = 0;
		__u32 overlay_colorblk_flag = 0;
		__u32 fill_rect_color = 4293931184;
		__u32 color_key_mode = 0;
		__u32 save_flag = 1;
		__u32 burning_flag = 0;
		__u32 work_mode = 0;
		__u32 rop_code = 0;
		__u32 test_mode = 0;
		__u32 pure_color_rd_msb = 0;
		__u32 pure_color_wr_msb = 0;
		__u32 dma_starting_corner = 0;
		__u32 ybtmode = 1;
		__u32 bt601range = 1;

		struct ge_alpha blit_alpha;

		blit_alpha.src_global_alpha = 255;
		blit_alpha.rst_global_alpha = 255;
		blit_alpha.sel_src_alpha = 0;
		blit_alpha.sel_rst_alpha = 0;


		__u32 set_dst_flag = FALSE, set_src_flag = FALSE, set_src_img = FALSE, set_dst_img = FALSE;
		const char *short_options = "hc:i:o:z:a:w:t:x:y:j:q:e:f:u:v:l:b:r:k:g:n:m:p:d:s:";
		const struct option long_options[] = {
			{ "help", 0, NULL, 'h' },
			{ "cmd", 1, NULL, 'c' },
			{ "input_img", 1, NULL, 'i' },
			{ "output_img", 1, NULL, 'o' },
			{ "src_fmt", 1, NULL, 'z' },
			{ "dst_fmt", 1, NULL, 'a' },
			{ "width", 1, NULL, 'w' },
			{ "height", 1, NULL, 't' },
			{ "srcx", 1, NULL, 'x' },
			{ "srcy", 1, NULL, 'y' },
			{ "src_width", 1, NULL, 'j' },
			{ "src_height", 1, NULL, 'q' },
			{ "dstx", 1, NULL, 'e' },
			{ "dsty", 1, NULL, 'f' },
			{ "dst_width", 1, NULL, 'u' },
			{ "dst_height", 1, NULL, 'v' },
			{ "yuv_sel", 1, NULL, 'l' },
			{ "overlay_colorblk_flag", 1, NULL, 'b' },
			{ "fill_rect_color", 1, NULL, 'r' },
			{ "color_key_mode", 1, NULL, 'k' },
			{ "save_flag", 1, NULL, 'g' },
			{ "burning_flag", 1, NULL, 'n' },
			{ "work_mode", 1, NULL, 'm' },
			{ "rop_code", 1, NULL, 'p' },
			{ "t_m", 1, NULL, VAL_OPT_TEST },
			{ "pure_color_rd_msb", 1, NULL, 'd'},
			{ "pure_color_wr_msb", 1, NULL, 's'},
			{ "ybt", 1, NULL, VAL_OPT_YBT },
			{ "601rng", 1, NULL, VAL_OPT_601_RANGE },
			{ "src_a", 1, NULL, VAL_OPT_A_SRC },
			{ "dst_a", 1, NULL, VAL_OPT_A_DST },
			{ "rlt_a", 1, NULL, VAL_OPT_A_RLT },
			{ "src_sel", 1, NULL, VAL_OPT_SEL_SRC },
			{ "dst_sel", 1, NULL, VAL_OPT_SEL_DST },
			{ "rlt_sel", 1, NULL, VAL_OPT_SEL_RLT},
			{ "s_cnr", 1, NULL, VAL_OPT_DMA_CORNER},
			{ NULL, 0, NULL, 0 },
		};



		do {
			next_option = getopt_long(argc, argv, short_options, long_options, NULL);

			switch (next_option) {
			case 'h':
				//print_usage(stdout, 0);
				break;
			case 'c':
				cmd = atoi(optarg);
				continue;
			case 'z':
				src_fmt = atoi(optarg);
				set_src_flag = TRUE;
				continue;
			case 'a':
				dst_fmt = atoi(optarg);
				set_dst_flag = TRUE;
				continue;
			case 'i':
				memset(input_img, 0, sizeof(input_img));
				snprintf(input_img, sizeof(input_img), "%s", optarg);
				set_src_img = TRUE;
				continue;
			case 'o':
				memset(output_img, 0, sizeof(output_img));
				snprintf(output_img, sizeof(output_img), "%s", optarg);
				set_dst_img = TRUE;
				continue;
			case 'w':
				width = atoi(optarg);
				continue;
			case 't':
				height = atoi(optarg);
				continue;
			case 'x':
				srcx = atoi(optarg);
				continue;
			case 'y':
				srcy = atoi(optarg);
				continue;
			case 'j':
				src_width = atoi(optarg);
				continue;
			case 'q':
				src_height = atoi(optarg);
				continue;
			case 'e':
				dstx = atoi(optarg);
				continue;
			case 'f':
				dsty = atoi(optarg);
				continue;
			case 'u':
				dst_width = atoi(optarg);
				continue;
			case 'v':
				dst_height = atoi(optarg);
				continue;
			case 'l':
				yuv_sel = atoi(optarg);
				continue;
			case 'b':
				overlay_colorblk_flag = atoi(optarg);
				continue;
			case 'r':
				fill_rect_color = atoll(optarg);
				continue;
			case 'k':
				color_key_mode = atoi(optarg);
				continue;
			case 'g':
				save_flag = atoi(optarg);
				continue;
			case 'n':
				burning_flag = atoi(optarg);
				continue;
			case 'm':
				work_mode = atoi(optarg);
				continue;
			case 'p':
				rop_code = atoi(optarg);
				continue;
			case VAL_OPT_TEST:
				test_mode = atoi(optarg);
				continue;
			case 'd':
				pure_color_rd_msb = atoi(optarg);
				continue;
			case 's':
				pure_color_wr_msb = atoi(optarg);
				continue;
			case '?':
				DiagPrintf("unknown option %c\n", isprint(optopt) ? optopt : '#');
				continue;
			case VAL_OPT_YBT :
				ybtmode = atoi(optarg);
				continue;
			case VAL_OPT_601_RANGE:
				bt601range = atoi(optarg);
				continue;
			case VAL_OPT_A_SRC:
				blit_alpha.src_global_alpha = atoi(optarg);
				continue;
			//case VAL_OPT_A_DST:
			//	blit_alpha.dst_global_alpha = atoi(optarg);
			//	continue;
			case VAL_OPT_A_RLT:
				blit_alpha.rst_global_alpha = atoi(optarg);
				continue;
			case VAL_OPT_SEL_SRC:
				blit_alpha.sel_src_alpha = atoi(optarg);
				continue;
			//case VAL_OPT_SEL_DST:
			//	blit_alpha.sel_dst_alpha = atoi(optarg);
			//	continue;
			case VAL_OPT_SEL_RLT:
				blit_alpha.sel_rst_alpha = atoi(optarg);
				continue;
			case VAL_OPT_DMA_CORNER:
				dma_starting_corner = atoi(optarg);
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

		if (set_dst_flag && !set_dst_img) {
			memset(output_img, 0, sizeof(output_img));
			if (yuv_sel) {
				if (dst_fmt >= 2) {
					DiagPrintf("error yuv dstfmt  = %d, reset to 0 \n", dst_fmt);
					dst_fmt = 0;
				}
				strncpy(output_img, output_img_YUV[dst_fmt], sizeof(output_img_YUV[dst_fmt]));
			} else {
				if (dst_fmt >= 10 || dst_fmt == 0) {
					DiagPrintf("error rgb dstfmt  = %d, reset to 6 \n", dst_fmt);
					dst_fmt = 6;
				}
				strncpy(output_img, output_img_RGB[dst_fmt - 1], sizeof(output_img_RGB[dst_fmt - 1]));
			}
		}

		if (set_src_flag && !set_src_img) {
			memset(input_img, 0, sizeof(input_img));

			if (src_fmt >= 10) {
				DiagPrintf("error srcfmt  = %d, reset to 0 \n", src_fmt);
				src_fmt = 0;
			}
			strncpy(input_img, input_img_RGB[src_fmt], sizeof(input_img_RGB[src_fmt]));

		}



		DiagPrintf("cmd %u, input_img %s, output_img %s, src_fmt %u, dst_fmt %u, width %u, height %u, srcx %u, srcy %u, srcw %u, srch %u\n",
				   cmd, input_img, output_img, src_fmt, dst_fmt, width, height,  srcx, srcy, src_width, src_height) ;
		DiagPrintf("dstx %u, dsty %u, dstw %u, dsth %u ,yuv_sel %u, overlay_colorblk_flag %u,fill_rect_color %u, color_key_mode %u\n",
				   dstx, dsty, dst_width, dst_height, yuv_sel, overlay_colorblk_flag, fill_rect_color, color_key_mode);
		DiagPrintf(" save_flag %u, work_mode %u, rop_code %u	, burning_flag %u, test_mode %u, pure_color_rd_msb %u, pure_color_wr_msb %u\n",
				   save_flag, work_mode, rop_code, burning_flag, test_mode, pure_color_rd_msb, pure_color_wr_msb);

		struct ge_blit blit_config;
		memset(&blit_config, 0, sizeof(blit_config));

		blit_config.cmd = cmd;
		blit_config.src_img.img_fmt = src_fmt;
		blit_config.src_img.w = src_width;
		blit_config.src_img.h = src_height;
		blit_config.src_rect.w = width;
		blit_config.src_rect.h = height;
		blit_config.src_rect.x = srcx;
		blit_config.src_rect.y = srcy;
		blit_config.dst_rect.w = width;
		blit_config.dst_rect.h = height;
		blit_config.dst_rect.x = dstx;
		blit_config.dst_rect.y = dsty;
		blit_config.overlay_colorblk_flag = overlay_colorblk_flag;
		blit_config.src_fill_rect_color = fill_rect_color;
		blit_config.color_key_mode = color_key_mode;
		blit_config.work_mode = work_mode;
		blit_config.rop_code = rop_code;
		blit_config.pure_color_rd_msb = pure_color_rd_msb;
		blit_config.pure_color_wr_msb = pure_color_wr_msb;
		blit_config.ybt_709_flag = ybtmode;
		blit_config.bt601_range = bt601range;
		blit_config.dst_img.img_fmt = dst_fmt;
		blit_config.dst_img.w = dst_width;
		blit_config.dst_img.h = dst_height;
		blit_config.dst_yuv_sel = yuv_sel;

		blit_config.dma_starting_corner = dma_starting_corner;

		blit_config.alpha.src_global_alpha = blit_alpha.src_global_alpha;
		blit_config.alpha.rst_global_alpha = blit_alpha.rst_global_alpha;
		blit_config.alpha.sel_src_alpha = blit_alpha.sel_src_alpha;
		blit_config.alpha.sel_rst_alpha = blit_alpha.sel_rst_alpha;



		__u32 src_size;
		__u32 dst_size;
		u8 *ge_buff_src ;
		u8 *ge_buff_dst ;
		u32 src_buf_loc, dst_buf_loc;
		u32 buffer_align = 16;
		struct ge_context ge_blit_context;
		memset(&ge_blit_context, 0, sizeof(ge_blit_context));


		//u32 output_yuv, org_src_fmt, org_dst_fmt;

		src_width = blit_config.src_img.w;
		src_height = blit_config.src_img.h;
		dst_width = blit_config.dst_img.w;
		dst_height = blit_config.dst_img.h;


		src_size = src_width * src_height * 4;
		dst_size = dst_width * dst_height * 4;


		src_buf_loc = malloc(src_size + buffer_align);
		dst_buf_loc = malloc(dst_size + buffer_align);

		ge_buff_src = (u8 *)((src_buf_loc + (buffer_align - 1)) & ~(buffer_align - 1));
		ge_buff_dst = (u8 *)((dst_buf_loc + (buffer_align - 1)) & ~(buffer_align - 1));





		ge_blit_context.surface[SRC].phy_addr = (void *)ge_buff_src;
		ge_blit_context.surface[SRC].size = src_size;
		ge_blit_context.surface[DST].phy_addr = (void *)ge_buff_dst;
		ge_blit_context.surface[DST].size = dst_size;

		DiagPrintf("src 0x%08x %d %dx%d, dst  0x%08x %d %dx%d \n", ge_buff_src, src_size, src_width, src_height, ge_buff_dst, dst_size, dst_width, dst_height);


		memset(ge_blit_context.surface[SRC].phy_addr, 0, ge_blit_context.surface[SRC].size);
		memset(ge_blit_context.surface[DST].phy_addr, 0, ge_blit_context.surface[DST].size);

		DiagPrintf("surface_loadpic %s %s\n", input_img, output_img);

		if (!blit_config.overlay_colorblk_flag) {
			DiagPrintf("surface_loadpic %s \n", input_img);
			if (strlen(output_img) != 0) {
				ret = surface_loadpic(&ge_blit_context, input_img, SRC);
				if (ret) {
					DiagPrintf("surface_loadpic src_img fail\n");
					free(src_buf_loc);
					free(dst_buf_loc);
					return ret;
				}
			}
		} else {
			DiagPrintf("src as rect fill \n");
		}


		if (strlen(output_img) != 0) {
			ret = surface_loadpic(&ge_blit_context, output_img, DST);
			if (ret) {
				DiagPrintf("surface_loadpic dst_img fail\n");
				free(src_buf_loc);
				free(dst_buf_loc);
				return ret;
			}
		}


		//output_yuv = blit_config.dst_yuv_sel;
		//org_dst_fmt = blit_config.dst_img.img_fmt;
		//org_src_fmt = blit_config.src_img.img_fmt;

		blit_config.usr_src_buf = (__u32)ge_blit_context.surface[SRC].phy_addr;
		blit_config.usr_dst_buf = (__u32)ge_blit_context.surface[DST].phy_addr;

		switch (blit_config.cmd) {
		case GE_BLT_ROI_BLEND:
			if (blit_config.color_key_mode) {  //for verification
				blit_config.clorkey.src_color_key_max = 2667669;//2667669; //0x28_b4_95 b_g_r
				blit_config.clorkey.src_color_key_min = 0;
				blit_config.clorkey.dst_color_key_max = 1048575; //0xf_ff_ff
				blit_config.clorkey.dst_color_key_min = 4095; //0x0_f_ff
				if (blit_config.color_key_mode == 1) {
					blit_config.alpha.src_global_alpha = 255;
				} else {
					blit_config.alpha.src_global_alpha = 127;
				}
			}
			break;

		case GE_BLT_SELF_BLEND:
			break;

		case GE_BLT_FILLRECT:
			break;

		case GE_BLT_FMT_CONV:
			break;
		}


		ge_drv_ctx_t *pge_drv_blit_ctx;
		pge_drv_blit_ctx = ge_open();

		ge_ctrl(pge_drv_blit_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

		ret = ge_ctrl(pge_drv_blit_ctx, GEDRV_CTRL_BLIT_START, (void *)&blit_config);
		if (ret) {
			DiagPrintf("ge_blit_start fail %x\n", ret);
			free(src_buf_loc);
			free(dst_buf_loc);
			ge_close(pge_drv_blit_ctx);
			return ret;
		}

		ret = ge_ctrl(pge_drv_blit_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

		if (save_flag && !ret) {


			u32 save_size;
			if (blit_config.dst_yuv_sel && !blit_config.dst_img.img_fmt) {
				save_size = blit_config.dst_img.w * blit_config.dst_img.h * 3 / 2;
			}
			if (blit_config.dst_yuv_sel && blit_config.dst_img.img_fmt) {
				save_size = blit_config.dst_img.w * blit_config.dst_img.h * 2;
			}
			if (!blit_config.dst_yuv_sel) {
				if (blit_config.dst_img.img_fmt == GE_BLT_RGB888) {
					save_size = blit_config.dst_img.w * blit_config.dst_img.h * 3;
				} else if (blit_config.dst_img.img_fmt == GE_BLT_ARGB8888 ||
						   blit_config.dst_img.img_fmt == GE_BLT_RGBA8888) {
					save_size = blit_config.dst_img.w * blit_config.dst_img.h * 4;
				} else if (blit_config.dst_img.img_fmt == GE_BLT_PURECLOR) {
					save_size = blit_config.dst_img.w * blit_config.dst_img.h / 8;
				} else if (blit_config.dst_img.img_fmt == GE_BLT_ARGB1111) {
					save_size = blit_config.dst_img.w * blit_config.dst_img.h / 2;
				} else if (blit_config.dst_img.img_fmt == GE_BLT_ARGB2222) {
					save_size = blit_config.dst_img.w * blit_config.dst_img.h;
				} else {
					save_size = blit_config.dst_img.w * blit_config.dst_img.h * 2;
				}
			}
			save_pic_blit(ge_blit_context.surface[DST].phy_addr, save_size, blit_config.cmd,
						  blit_config.src_img.img_fmt, blit_config.src_rect.x, blit_config.src_rect.y, blit_config.dst_img.img_fmt, blit_config.dst_rect.x, blit_config.dst_rect.y,
						  blit_config.dst_yuv_sel,
						  blit_config.src_rect.w, blit_config.src_rect.h, blit_config.work_mode, blit_config.rop_code, 0, blit_config.pure_color_wr_msb);
		}


		free(src_buf_loc);
		free(dst_buf_loc);

		ge_close(pge_drv_blit_ctx);


		if (ret < 0) {
			DiagPrintf("ge_blt failed!!\n");
		} else {
			DiagPrintf("ge_blt done!!\n");
		}

		return ret;

	}

