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


#include <stdint.h>
#include <basic_types.h>

#include <errno.h>
#include "ameba_soc.h"

#include "rts_blit_regs.h"
#include "rts_blit.h"
#include "rts_ge_driver.h"

#define pr_err DiagPrintf

void set_clut(void)
{
	int i;

	for (i = 0; i < 256; i += 4)  // 0x0800~0x08FF
		ge_iowrite32(BLIT_LUT_MEM + i, (rgb5_clut[i / 4] << 16) +
					 (rgb6_clut[i / 4] << 8) +
					 rgb5_clut[i / 4]);

	for (i = 0; i < 128; i += 4) // 0x0900~0x097F
		ge_iowrite32((BLIT_LUT_MEM + 256) + i, (a_clut[i / 4] << 24) +
					 (rgb5_clut[i / 4] << 16) +
					 (rgb5_clut[i / 4] << 8) +
					 rgb5_clut[i / 4]);

	for (i = 0; i < 64; i += 4) // 0x0980~0x09BF
		ge_iowrite32((BLIT_LUT_MEM + 256 + 128) + i, (rgb4_clut[i / 4] << 24) +
					 (rgb4_clut[i / 4] << 16) +
					 (rgb4_clut[i / 4] << 8) +
					 rgb4_clut[i / 4]);

	for (i = 0; i < 16; i += 4) // 0x09C0~0x09CF
		ge_iowrite32((BLIT_LUT_MEM + 256 + 128 + 64) + i, (rgb2_clut[i / 4] << 24) +
					 (rgb2_clut[i / 4] << 16) +
					 (rgb2_clut[i / 4] << 8) +
					 rgb2_clut[i / 4]);
	ge_iowrite32((BLIT_LUT_MEM + 256 + 128 + 64 + 16), 0);
	ge_iowrite32((BLIT_LUT_MEM + 256 + 128 + 64 + 16 + 4), 0xffffffff);
	ge_iowrite32(BLIT_LUT_PURE_COLOR_CFG, 0xff0000);

	for (i = 0; i < 1024; i += 4) {
		ge_iowrite32(BLIT_LUTRAW_MEM + i, index8_clut[i / 4]);
	}
}

void rts_blit_init(void)
{
	//iowrite32(0x1, (__le32 __iomem *)(blit_clk));
	HAL_WRITE32(GE_REG_BASE, GE_BLIT_CLK_EN, BLIT_CLK_EN_BITS);
	set_clut();
}

__u32 blit_get_int_status(void)
{
	__u32 reg_val = 0;

	reg_val = ge_ioread32(BLIT_INT_FLAG);
	return reg_val;
}

void blit_clear_int(void)
{
	ge_iowrite32(BLIT_INT_CLR, 1);
}


__u32 blit_check_para(struct ge_blit *para)
{
	__u32 r, g, b, a;

	if (para->work_mode > FILL_SRCRECT) {
		DiagPrintf("work_mode err %d \n", para->work_mode);
		return EINVAL;
	}
	if (para->src_img.img_fmt > GE_BLT_ARGB2222 ||
		para->dst_img.img_fmt > GE_BLT_ARGB2222 ||
		para->rst_img.img_fmt > GE_BLT_ARGB2222) {
		DiagPrintf("img_fmt err %d %d %d \n", para->src_img.img_fmt, para->dst_img.img_fmt, para->rst_img.img_fmt);
		return EINVAL;
	}
	if (para->src_img.img_fmt == GE_BLT_ARGB1111) {
		if ((para->src_img.w % 2) || (para->src_img.h % 2) ||
			(para->src_rect.x % 2) || (para->src_rect.y % 2) ||
			(para->src_rect.w % 2) || (para->src_rect.h % 2)) {
			DiagPrintf("src_ARGB1111 err img %d %d rect %d %d %d %d\n", para->src_img.w, para->src_img.h, para->src_rect.x, para->src_rect.y, para->src_rect.w,
					   para->src_rect.h);
			return EINVAL;
		}
	}
	if (para->dst_img.img_fmt == GE_BLT_ARGB1111 ||
		para->rst_img.img_fmt == GE_BLT_ARGB1111) {
		if ((para->dst_img.w % 2) || (para->dst_img.h % 2) ||
			(para->dst_rect.x % 2) || (para->dst_rect.y % 2) ||
			(para->dst_rect.w % 2) || (para->dst_rect.h % 2)) {
			DiagPrintf("dst_ARGB1111 err img %d %d rect %d %d %d %d\n", para->dst_img.w, para->dst_img.h, para->dst_rect.x, para->dst_rect.y, para->dst_rect.w,
					   para->dst_rect.h);
			return EINVAL;
		}
	}
	if (para->src_img.img_fmt == GE_BLT_PURECLOR) {
		if ((para->src_img.w % 8) ||
			(para->src_rect.x % 8) ||
			(para->src_rect.w % 8)) {
			DiagPrintf("src_PURECLOR err img %d %d rect %d %d %d %d\n", para->src_img.w, para->src_img.h, para->src_rect.x, para->src_rect.y, para->src_rect.w,
					   para->src_rect.h);
			return EINVAL;
		}
	}
	if (para->dst_img.img_fmt == GE_BLT_PURECLOR ||
		para->rst_img.img_fmt == GE_BLT_PURECLOR) {
		if ((para->dst_img.w % 8) ||
			(para->dst_rect.x % 8) ||
			(para->dst_rect.w % 8)) {
			DiagPrintf("dst_PURECLOR err img %d %d rect %d %d %d %d\n", para->dst_img.w, para->dst_img.h, para->dst_rect.x, para->dst_rect.y, para->dst_rect.w,
					   para->dst_rect.h);
			return EINVAL;
		}
	}
	if (para->cmd != GE_BLT_FILLRECT && para->dst_img.img_fmt == 0 &&
		!para->dst_yuv_sel) {
		DiagPrintf("not GE_BLT_FILLRECT err img_fmt 0  dst_yuv_sel 0\n");
		return EINVAL;
	}
	if (para->cmd == GE_BLT_ROI_BLEND) {
		if (para->src_img.w < 2 || para->src_img.h < 2) {
			DiagPrintf("ROI_BLEND err img %d %d rect %d %d %d %d\n", para->src_img.w, para->src_img.h, para->src_rect.x, para->src_rect.y, para->src_rect.w,
					   para->src_rect.h);
			return EINVAL;
		}
		if (para->src_rect.x >= para->src_img.w ||
			para->src_rect.y >= para->src_img.h) {
			DiagPrintf("ROI_BLEND err img %d %d rect %d %d %d %d\n", para->src_img.w, para->src_img.h, para->src_rect.x, para->src_rect.y, para->src_rect.w,
					   para->src_rect.h);
			return EINVAL;
		}
		if (para->src_rect.w > para->src_img.w ||
			para->src_rect.h > para->src_img.h) {
			DiagPrintf("ROI_BLEND err img %d %d rect %d %d %d %d\n", para->src_img.w, para->src_img.h, para->src_rect.x, para->src_rect.y, para->src_rect.w,
					   para->src_rect.h);
			return EINVAL;
		}
	}
	if (para->cmd == GE_BLT_SELF_BLEND) {
		if (para->src_rect.x + para->src_rect.w - 1 >=
			para->dst_img.w) {
			return -EINVAL;
		}
		if (para->src_rect.y + para->src_rect.h - 1 >=
			para->dst_img.h) {
			return -EINVAL;
		}
		if (para->dst_rect.x + para->dst_rect.w - 1 >=
			para->dst_img.w) {
			return -EINVAL;
		}
		if (para->dst_rect.y + para->dst_rect.h - 1 >=
			para->dst_img.h) {
			return -EINVAL;
		}
	}
	if (para->dst_rect.x >= para->dst_img.w ||
		para->dst_rect.y >= para->dst_img.h) {
		DiagPrintf("dst size err img %d %d rect %d %d %d %d\n", para->dst_img.w, para->dst_img.h, para->dst_rect.x, para->dst_rect.y, para->dst_rect.w,
				   para->dst_rect.h);
		return EINVAL;
	}
	if (para->src_rect.w > para->dst_img.w ||
		para->src_rect.h > para->dst_img.h) {
		DiagPrintf("src size err img src %d %d dst  %d %d\n", para->src_img.w, para->src_img.h, para->dst_img.w, para->dst_img.h);
		return EINVAL;
	}
	if (para->src_rect.w < 2 || para->src_rect.h < 2) {
		DiagPrintf("src size err img %d %d rect %d %d %d %d\n", para->src_img.w, para->src_img.h, para->src_rect.x, para->src_rect.y, para->src_rect.w,
				   para->src_rect.h);
		return EINVAL;
	}
	if (para->dst_img.w < 2 || para->dst_img.h < 2) {
		DiagPrintf("dst size err img %d %d rect %d %d %d %d\n", para->dst_img.w, para->dst_img.h, para->dst_rect.x, para->dst_rect.y, para->dst_rect.w,
				   para->dst_rect.h);
		return EINVAL;
	}
	if (para->alpha.sel_src_alpha > 5 || para->alpha.sel_rst_alpha > 2) {
		DiagPrintf("alpha sel %d %d \n", para->alpha.sel_src_alpha, para->alpha.sel_rst_alpha);
		return EINVAL;
	}
	if (para->rop_code > 15) {
		DiagPrintf("para->rop_code %d \n", para->rop_code);
		return EINVAL;
	}
	if (para->dst_yuv_sel && (para->src_rect.w % 2)) {
		para->src_rect.w = para->src_rect.w - 1;
	}
	para->src_rect.h = (para->src_rect.h % 2) ? (para->src_rect.h - 1) :
					   para->src_rect.h;
	para->src_img.w =
		(para->src_img.w % 2) ? (para->src_img.w - 1) : para->src_img.w;
	para->src_img.h =
		(para->src_img.h % 2) ? (para->src_img.h - 1) : para->src_img.h;
	para->dst_img.w =
		(para->dst_img.w % 2) ? (para->dst_img.w - 1) : para->dst_img.w;
	para->dst_img.h =
		(para->dst_img.h % 2) ? (para->dst_img.h - 1) : para->dst_img.h;
	if (para->dst_yuv_sel) {
		if (para->dst_img.img_fmt && (para->dst_rect.x % 2)) {
			DiagPrintf("para->dst_img.img_fmt %d para->dst_rect.x %d\n", para->dst_img.img_fmt, para->dst_rect.x);
			return EINVAL;
		} else if ((para->dst_rect.x % 2) || (para->dst_rect.y % 2)) {
			DiagPrintf("dst_rect %d %d\n", para->dst_rect.x, para->dst_rect.y);
			return EINVAL;
		}
	}
	if ((para->dst_addr[0] & 0x0F) || (para->dst_addr[1] & 0x0F)) {
		return EINVAL;
	}

	if ((para->src_addr[0] & 0x0F) || (para->rst_addr[0] & 0x0F)) {
		return EINVAL;
	}


	if (para->src_fill_rect_color && para->src_img.img_fmt &&
		!para->dst_yuv_sel) {
		switch (para->src_img.img_fmt) {
		case GE_BLT_RGB565:
			b = (para->src_fill_rect_color & 0x000000ff) >> 3;
			g = (para->src_fill_rect_color & 0x0000ff00) >> 10;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 19;
			para->src_fill_rect_color = (b << 11) + (g << 5) + r;
			break;
		case GE_BLT_ARGB1555:
			b = (para->src_fill_rect_color & 0x000000ff) >> 3;
			g = (para->src_fill_rect_color & 0x0000ff00) >> 11;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 19;
			a = (para->src_fill_rect_color & 0xff000000) >> 31;
			para->src_fill_rect_color =
				(b << 11) + (g << 6) + (r << 1) + a;
			break;
		case GE_BLT_ARGB4444:
			b = (para->src_fill_rect_color & 0x000000ff) >> 4;
			g = (para->src_fill_rect_color & 0x0000ff00) >> 12;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 20;
			a = (para->src_fill_rect_color & 0xff000000) >> 28;
			para->src_fill_rect_color =
				(b << 12) + (g << 8) + (r << 4) + a;
			break;
		case GE_BLT_ARGB8888:
			b = (para->src_fill_rect_color & 0x000000ff);
			g = (para->src_fill_rect_color & 0x0000ff00) >> 8;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 16;
			a = (para->src_fill_rect_color & 0xff000000) >> 24;
			para->src_fill_rect_color =
				(b << 24) + (g << 16) + (r << 8) + a;
			break;
		case GE_BLT_RGBA8888:
			b = (para->src_fill_rect_color & 0x000000ff);
			g = (para->src_fill_rect_color & 0x0000ff00) >> 8;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 16;
			a = (para->src_fill_rect_color & 0xff000000) >> 24;
			para->src_fill_rect_color =
				(a << 24) + (b << 16) + (g << 8) + r;
			break;
		case GE_BLT_RGB888:
			b = (para->src_fill_rect_color & 0x000000ff);
			g = (para->src_fill_rect_color & 0x0000ff00) >> 8;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 16;
			a = (para->src_fill_rect_color & 0xff000000) >> 24;
			para->src_fill_rect_color = (b << 16) + (g << 8) + r;
			break;
		case GE_BLT_PURECLOR:
			b = (para->src_fill_rect_color & 0x000000ff) >> 7;
			g = (para->src_fill_rect_color & 0x0000ff00) >> 15;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 23;
			a = (para->src_fill_rect_color & 0xff000000) >> 31;
			para->src_fill_rect_color = (b && g && r && a) ? 1 : 0;
			break;
		case GE_BLT_ARGB1111:
			b = (para->src_fill_rect_color & 0x000000ff) >> 7;
			g = (para->src_fill_rect_color & 0x0000ff00) >> 15;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 23;
			a = (para->src_fill_rect_color & 0xff000000) >> 31;
			para->src_fill_rect_color =
				(b << 3) + (g << 2) + (r << 1) + a;
			break;
		case GE_BLT_ARGB2222:
			b = (para->src_fill_rect_color & 0x000000ff) >> 6;
			g = (para->src_fill_rect_color & 0x0000ff00) >> 14;
			r = (para->src_fill_rect_color & 0x00ff0000) >> 22;
			a = (para->src_fill_rect_color & 0xff000000) >> 30;
			para->src_fill_rect_color =
				(b << 6) + (g << 4) + (r << 2) + a;
			break;
		}
	}

	if (para->dst_yuv_sel && para->src_fill_rect_color) {
		b = (para->src_fill_rect_color & 0x000000ff);
		g = (para->src_fill_rect_color & 0x0000ff00) >> 8;
		r = (para->src_fill_rect_color & 0x00ff0000) >> 16;
		a = (para->src_fill_rect_color & 0xff000000) >> 24;
		para->src_fill_rect_color =
			(b << 24) + (g << 16) + (r << 8) + a;
	}

	return 0;
}




// ybt709 full
static __u32 rgb2yuv_coef[9] = {   872, 2929, 295,
								   471, 1577, 2048,
								   2048, 1860, 188
							   };

void blit_rgb2yuv_coef(void)
{
	int i;
	ge_iowrite32(BLIT_RGB2YUV_YOFFSET, 0);
	for (i = 0; i <= 32; i += 4) {
		ge_iowrite32(BLIT_R2Y_COEF0 + i, rgb2yuv_coef[i / 4]);
	}
}


void blit_set_color_space(struct ge_blit *para)
{

	ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
	ge_iowrite32(BLIT_YMODE, para->bt601_range);
	if (para->ybt_709_flag) {
		blit_rgb2yuv_coef();
	}
};

void blit_set_roi_blend(struct ge_blit *para)
{
	if (para->overlay_colorblk_flag) {
		ge_iowrite32(BLIT_SRCRECT_SEL, 1);
		ge_iowrite32(BLIT_FILL_RECT_COLOR, para->src_fill_rect_color);
		if (!para->dst_yuv_sel) {
			ge_iowrite32(BLIT_SRC_IMG_FMT, para->src_img.img_fmt);
		} else {
			ge_iowrite32(BLIT_SRC_IMG_FMT, GE_BLT_ARGB8888);
		}
	} else {
		ge_iowrite32(BLIT_SRCRECT_SEL, 0);
		ge_iowrite32(BLIT_SRC_IMGSIZE_W, para->src_img.w);
		ge_iowrite32(BLIT_SRC_IMGSIZE_H, para->src_img.h);
		ge_iowrite32(BLIT_SRC_RECT_OFFSET_X, para->src_rect.x);
		ge_iowrite32(BLIT_SRC_RECT_OFFSET_Y, para->src_rect.y);
		ge_iowrite32(BLIT_SRC_IMG_FMT, para->src_img.img_fmt);
		ge_iowrite32(BLIT_SRC_IMGADDR, para->src_addr[0]);
	}
	ge_iowrite32(BLIT_DST_RECT_OFFSET_X, para->dst_rect.x);
	ge_iowrite32(BLIT_DST_RECT_OFFSET_Y, para->dst_rect.y);
	ge_iowrite32(BLIT_DST_IMGSIZE_W, para->dst_img.w);
	ge_iowrite32(BLIT_DST_IMGSIZE_H, para->dst_img.h);
	//dst yuv sel rst yuv sel
	ge_iowrite32(BLIT_RST_YUV, para->dst_yuv_sel);
	ge_iowrite32(BLIT_DST_YUV, para->dst_yuv_sel);
	if (!para->dst_yuv_sel) {
		ge_iowrite32(BLIT_DST_IMG_FMT, para->dst_img.img_fmt);
		//rst img fmt
		ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
	} else {
		ge_iowrite32(BLIT_IMG_FMT, GE_BLT_ARGB8888);
		ge_iowrite32(BLIT_DST_IMG_FMT, GE_BLT_ARGB8888);
		ge_iowrite32(BLIT_YUV_MODE, para->dst_img.img_fmt);
		ge_iowrite32(BLIT_DST_IMGADDR_1, para->dst_addr[1]);
		ge_iowrite32(BLIT_RST_IMGADDR_UV, para->dst_addr[1]);
		//ge_iowrite32(BLIT_YMODE, para->bt601_range);
		//ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
		//if (para->ybt_709_flag) {
		//	blit_rgb2yuv_coef();
		//}
	}
	ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
	ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
	if (para->color_key_mode) {
		ge_iowrite32(BLIT_COLOR_MODE, para->color_key_mode);
		ge_iowrite32(BLIT_SRC_COLOR_KEY_MAX,
					 para->clorkey.src_color_key_max);
		ge_iowrite32(BLIT_SRC_COLOR_KEY_MIN,
					 para->clorkey.src_color_key_min);
		ge_iowrite32(BLIT_DST_COLOR_KEY_MAX,
					 para->clorkey.dst_color_key_max);
		ge_iowrite32(BLIT_DST_COLOR_KEY_MIN,
					 para->clorkey.dst_color_key_min);
	} else {
		ge_iowrite32(BLIT_COLOR_MODE, 0);
	}

	if (para->work_mode) {
		ge_iowrite32(BLIT_ROP_CODE, para->rop_code);
	}
	ge_iowrite32(BLIT_WORK_MODE, para->work_mode);
	para->pure_color_wr_msb = para->pure_color_rd_msb;
	ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
	ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);
}


void blit_set_self_blend(struct ge_blit *para)
{
	ge_iowrite32(BLIT_SRCRECT_SEL, 0);
	ge_iowrite32(BLIT_DST_YUV, 0);
	ge_iowrite32(BLIT_RST_YUV, 0);
	ge_iowrite32(BLIT_DMA_START, para->dma_starting_corner);
	ge_iowrite32(BLIT_DST_IMGSIZE_W, para->dst_img.w);
	ge_iowrite32(BLIT_DST_IMGSIZE_H, para->dst_img.h);
	ge_iowrite32(BLIT_DST_RECT_OFFSET_X, para->dst_rect.x);
	ge_iowrite32(BLIT_DST_RECT_OFFSET_Y, para->dst_rect.y);
	ge_iowrite32(BLIT_SRC_RECT_OFFSET_X, para->src_rect.x);
	ge_iowrite32(BLIT_SRC_RECT_OFFSET_Y, para->src_rect.y);
	ge_iowrite32(BLIT_SRC_IMGSIZE_W, para->dst_img.w);
	ge_iowrite32(BLIT_SRC_IMGSIZE_H, para->dst_img.h);
	ge_iowrite32(BLIT_SRC_IMG_FMT, para->dst_img.img_fmt);
	ge_iowrite32(BLIT_DST_IMG_FMT, para->dst_img.img_fmt);
	ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
	ge_iowrite32(BLIT_SRC_IMGADDR, para->dst_addr[0]);
	ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
	ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
	ge_iowrite32(BLIT_WORK_MODE, 0);
	ge_iowrite32(BLIT_COLOR_MODE, 0);
	para->pure_color_wr_msb = para->pure_color_rd_msb;
	ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
	ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);


}


void blit_set_fillrect(struct ge_blit *para)
{

	ge_iowrite32(BLIT_SRCRECT_SEL, 1);
	ge_iowrite32(BLIT_WORK_MODE, 0x2);
	ge_iowrite32(BLIT_FILL_RECT_COLOR, para->src_fill_rect_color);
	//DiagPrintf("RECT_COLOR read 0x%08x 0x%08lx %ld\n\r", BLIT_FILL_RECT_COLOR, ge_ioread32(BLIT_FILL_RECT_COLOR), para->src_fill_rect_color);
	ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
	//DiagPrintf("RST_IMGADDR_Y read 0x%08x 0x%08lx 0x%08lx\n\r", BLIT_RST_IMGADDR_Y, ge_ioread32(BLIT_RST_IMGADDR_Y), para->dst_addr[0]);
	ge_iowrite32(BLIT_RST_YUV, 0);
	ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
	//DiagPrintf("IMG_FMT read 0x%08x 0x%08lx %ld\n\r", BLIT_IMG_FMT, ge_ioread32(BLIT_IMG_FMT), para->dst_img.img_fmt);
	ge_iowrite32(BLIT_COLOR_MODE, 0);
	ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
	ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);

}

void blit_set_fmt_conv(struct ge_blit *para)
{

	if (para->work_mode) { //rgb2yuv
		ge_iowrite32(BLIT_SRCRECT_SEL, 0);
		ge_iowrite32(BLIT_SRC_IMGSIZE_W, para->src_img.w);
		ge_iowrite32(BLIT_SRC_IMGSIZE_H, para->src_img.h);
		ge_iowrite32(BLIT_SRC_RECT_OFFSET_X, para->src_rect.x);
		ge_iowrite32(BLIT_SRC_RECT_OFFSET_Y, para->src_rect.y);
		ge_iowrite32(BLIT_SRC_IMGADDR, para->src_addr[0]);
		ge_iowrite32(BLIT_SRC_IMG_FMT, para->src_img.img_fmt);
		ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
		ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
		if (para->dst_yuv_sel) {
			ge_iowrite32(BLIT_DST_IMGADDR_1, para->dst_addr[1]);
			ge_iowrite32(BLIT_RST_IMGADDR_UV, para->dst_addr[1]);
		}
		//rst yuv sel
		ge_iowrite32(BLIT_RST_YUV, para->dst_yuv_sel);
		ge_iowrite32(BLIT_IMG_FMT, 4);
		//rst img fmt
		if (!para->dst_yuv_sel) {
			ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
		} else {
			ge_iowrite32(BLIT_YUV_MODE, para->dst_img.img_fmt);
		}
	} else { //yuv2rgb
		ge_iowrite32(BLIT_SRCRECT_SEL, 1);
		ge_iowrite32(BLIT_DST_IMGSIZE_W, para->dst_img.w);
		ge_iowrite32(BLIT_DST_IMGSIZE_H, para->dst_img.h);
		ge_iowrite32(BLIT_DST_RECT_OFFSET_X, para->dst_rect.x);
		ge_iowrite32(BLIT_DST_RECT_OFFSET_Y, para->dst_rect.y);
		ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
		ge_iowrite32(BLIT_DST_IMGADDR_1, para->dst_addr[1]);
		ge_iowrite32(BLIT_RST_IMGADDR_Y, para->rst_addr[0]);
		ge_iowrite32(BLIT_RST_YUV, 0);
		//dst yuv sel
		ge_iowrite32(BLIT_DST_YUV, para->dst_yuv_sel);
		ge_iowrite32(BLIT_YUV_MODE, para->dst_img.img_fmt);
		ge_iowrite32(BLIT_IMG_FMT, para->rst_img.img_fmt);
	}
	ge_iowrite32(BLIT_WORK_MODE, para->work_mode);
	ge_iowrite32(BLIT_COLOR_MODE, 0);
	ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
	ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);

}

void blit_set_alpha_ch(struct ge_blit *para)
{
	ge_iowrite32(BLIT_SRC_ALPHA, para->alpha.sel_src_alpha);
	ge_iowrite32(BLIT_OUT_ALPHA, para->alpha.sel_rst_alpha);
	ge_iowrite32(BLIT_SRC_GLOBAL_ALPHA, para->alpha.src_global_alpha);
	ge_iowrite32(BLIT_RST_GLOBAL_ALPHA, para->alpha.rst_global_alpha);
}

void blit_set_size(struct ge_blit *para)
{
	ge_iowrite32(BLIT_SRC_RECT_SIZE_W, para->src_rect.w);
	ge_iowrite32(BLIT_SRC_RECT_SIZE_H, para->src_rect.h);
	ge_iowrite32(BLIT_RST_RECT_OFFSET_X, para->dst_rect.x);
	ge_iowrite32(BLIT_RST_RECT_OFFSET_Y, para->dst_rect.y);
	ge_iowrite32(BLIT_RST_IMGSIZE_W, para->dst_img.w);
	ge_iowrite32(BLIT_RST_IMGSIZE_H, para->dst_img.h);

	//DiagPrintf("src_rect.w read 0x%08x 0x%08lx %ld\n\r", BLIT_SRC_RECT_SIZE_W, ge_ioread32(BLIT_SRC_RECT_SIZE_W), para->src_rect.w);
	//DiagPrintf("src_rect.h read 0x%08x 0x%08lx %ld\n\r", BLIT_SRC_RECT_SIZE_H, ge_ioread32(BLIT_SRC_RECT_SIZE_H), para->src_rect.h);
	//DiagPrintf("dst_rect.x read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_RECT_OFFSET_X, ge_ioread32(BLIT_RST_RECT_OFFSET_X), para->dst_rect.x);
	//DiagPrintf("dst_rect.y read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_RECT_OFFSET_Y, ge_ioread32(BLIT_RST_RECT_OFFSET_Y), para->dst_rect.y);
	//DiagPrintf("dst_img.w read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_IMGSIZE_W, ge_ioread32(BLIT_RST_IMGSIZE_W), para->dst_img.w);
	//DiagPrintf("dst_img.h read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_IMGSIZE_H, ge_ioread32(BLIT_RST_IMGSIZE_H), para->dst_img.h);

}

void blit_start(void)
{
	ge_iowrite32(BLIT_INT_EN, 0x1);
	ge_iowrite32(BLIT_CTRL, 1);

	//DiagPrintf("busy_flag %d \n", ge_ioread32(BLIT_CTRL));
	//DiagPrintf("int flag %d \n",  ge_ioread32(BLIT_INT_FLAG));
}

__s32 rts_ge_blit(struct ge_blit *para)
{

	int ret;
	int count = 0;
	u32 busy_status = 0;

	rts_blit_init();


	if (ge_ioread32(BLIT_CTRL) >> 2) {
		pr_err("%d, %s, busy_flag %d \n", __LINE__, __func__,
			   ge_ioread32(BLIT_CTRL));
		ge_iowrite32(BLIT_CTRL, 0x2);
	}
	DiagPrintf("BLIT_CTRL done \n");

	ret = blit_check_para(para);

	if (ret) {
		pr_err("%d, %s, Invalid argument\n", __LINE__, __func__);
		return EINVAL;
	}
	DiagPrintf("blit_check_para done \n");

	ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
	ge_iowrite32(BLIT_YMODE, para->bt601_range);
	if (para->ybt_709_flag) {
		blit_rgb2yuv_coef();
	}



	switch (para->cmd) {
	case GE_BLT_ROI_BLEND:
		if (para->overlay_colorblk_flag) {
			ge_iowrite32(BLIT_SRCRECT_SEL, 1);
			ge_iowrite32(BLIT_FILL_RECT_COLOR,
						 para->src_fill_rect_color);
			if (!para->dst_yuv_sel)
				ge_iowrite32(BLIT_SRC_IMG_FMT,
							 para->src_img.img_fmt);
			else {
				ge_iowrite32(BLIT_SRC_IMG_FMT, GE_BLT_ARGB8888);
			}
		} else {
			ge_iowrite32(BLIT_SRCRECT_SEL, 0);
			ge_iowrite32(BLIT_SRC_IMGSIZE_W, para->src_img.w);
			ge_iowrite32(BLIT_SRC_IMGSIZE_H, para->src_img.h);
			ge_iowrite32(BLIT_SRC_RECT_OFFSET_X, para->src_rect.x);
			ge_iowrite32(BLIT_SRC_RECT_OFFSET_Y, para->src_rect.y);
			ge_iowrite32(BLIT_SRC_IMG_FMT, para->src_img.img_fmt);
			ge_iowrite32(BLIT_SRC_IMGADDR, para->src_addr[0]);
		}
		ge_iowrite32(BLIT_DST_RECT_OFFSET_X, para->dst_rect.x);
		ge_iowrite32(BLIT_DST_RECT_OFFSET_Y, para->dst_rect.y);
		ge_iowrite32(BLIT_DST_IMGSIZE_W, para->dst_img.w);
		ge_iowrite32(BLIT_DST_IMGSIZE_H, para->dst_img.h);
		//dst yuv sel rst yuv sel
		ge_iowrite32(BLIT_RST_YUV, para->dst_yuv_sel);
		ge_iowrite32(BLIT_DST_YUV, para->dst_yuv_sel);
		if (!para->dst_yuv_sel) {
			ge_iowrite32(BLIT_DST_IMG_FMT, para->dst_img.img_fmt);
			//rst img fmt
			ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
		} else {
			ge_iowrite32(BLIT_IMG_FMT, GE_BLT_ARGB8888);
			ge_iowrite32(BLIT_DST_IMG_FMT, GE_BLT_ARGB8888);
			ge_iowrite32(BLIT_YUV_MODE, para->dst_img.img_fmt);
			ge_iowrite32(BLIT_DST_IMGADDR_1, para->dst_addr[1]);
			ge_iowrite32(BLIT_RST_IMGADDR_UV, para->dst_addr[1]);
			//ge_iowrite32(BLIT_YMODE, para->bt601_range);
			//ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
			//if (para->ybt_709_flag) {
			//	blit_rgb2yuv_coef();
			//}
		}
		ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
		ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
		if (para->color_key_mode) {
			ge_iowrite32(BLIT_COLOR_MODE, para->color_key_mode);
			ge_iowrite32(BLIT_SRC_COLOR_KEY_MAX,
						 para->clorkey.src_color_key_max);
			ge_iowrite32(BLIT_SRC_COLOR_KEY_MIN,
						 para->clorkey.src_color_key_min);
			ge_iowrite32(BLIT_DST_COLOR_KEY_MAX,
						 para->clorkey.dst_color_key_max);
			ge_iowrite32(BLIT_DST_COLOR_KEY_MIN,
						 para->clorkey.dst_color_key_min);
		} else {
			ge_iowrite32(BLIT_COLOR_MODE, 0);
		}

		if (para->work_mode) {
			ge_iowrite32(BLIT_ROP_CODE, para->rop_code);
		}
		ge_iowrite32(BLIT_WORK_MODE, para->work_mode);
		para->pure_color_wr_msb = para->pure_color_rd_msb;
		ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
		ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);
		break;
	case GE_BLT_SELF_BLEND:
		ge_iowrite32(BLIT_SRCRECT_SEL, 0);
		ge_iowrite32(BLIT_DST_YUV, 0);
		ge_iowrite32(BLIT_RST_YUV, 0);
		ge_iowrite32(BLIT_DMA_START, para->dma_starting_corner);
		ge_iowrite32(BLIT_DST_IMGSIZE_W, para->dst_img.w);
		ge_iowrite32(BLIT_DST_IMGSIZE_H, para->dst_img.h);
		ge_iowrite32(BLIT_DST_RECT_OFFSET_X, para->dst_rect.x);
		ge_iowrite32(BLIT_DST_RECT_OFFSET_Y, para->dst_rect.y);
		ge_iowrite32(BLIT_SRC_RECT_OFFSET_X, para->src_rect.x);
		ge_iowrite32(BLIT_SRC_RECT_OFFSET_Y, para->src_rect.y);
		ge_iowrite32(BLIT_SRC_IMGSIZE_W, para->dst_img.w);
		ge_iowrite32(BLIT_SRC_IMGSIZE_H, para->dst_img.h);
		ge_iowrite32(BLIT_SRC_IMG_FMT, para->dst_img.img_fmt);
		ge_iowrite32(BLIT_DST_IMG_FMT, para->dst_img.img_fmt);
		ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
		ge_iowrite32(BLIT_SRC_IMGADDR, para->dst_addr[0]);
		ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
		ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
		ge_iowrite32(BLIT_WORK_MODE, 0);
		ge_iowrite32(BLIT_COLOR_MODE, 0);
		para->pure_color_wr_msb = para->pure_color_rd_msb;
		ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
		ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);
		break;
	case GE_BLT_FILLRECT:

		ge_iowrite32(BLIT_SRCRECT_SEL, 1);
		ge_iowrite32(BLIT_WORK_MODE, 0x2);
		ge_iowrite32(BLIT_FILL_RECT_COLOR, para->src_fill_rect_color);
		//DiagPrintf("RECT_COLOR read 0x%08x 0x%08lx %ld\n\r", BLIT_FILL_RECT_COLOR, ge_ioread32(BLIT_FILL_RECT_COLOR), para->src_fill_rect_color);
		ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
		//DiagPrintf("RST_IMGADDR_Y read 0x%08x 0x%08lx 0x%08lx\n\r", BLIT_RST_IMGADDR_Y, ge_ioread32(BLIT_RST_IMGADDR_Y), para->dst_addr[0]);
		ge_iowrite32(BLIT_RST_YUV, 0);
		ge_iowrite32(BLIT_IMG_FMT, para->dst_img.img_fmt);
		//DiagPrintf("IMG_FMT read 0x%08x 0x%08lx %ld\n\r", BLIT_IMG_FMT, ge_ioread32(BLIT_IMG_FMT), para->dst_img.img_fmt);
		ge_iowrite32(BLIT_COLOR_MODE, 0);
		ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
		ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);
		break;
	case GE_BLT_FMT_CONV:

		//ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
		//ge_iowrite32(BLIT_YMODE, para->bt601_range);
		if (para->work_mode) { //rgb2yuv
			ge_iowrite32(BLIT_SRCRECT_SEL, 0);
			ge_iowrite32(BLIT_SRC_IMGSIZE_W, para->src_img.w);
			ge_iowrite32(BLIT_SRC_IMGSIZE_H, para->src_img.h);
			ge_iowrite32(BLIT_SRC_RECT_OFFSET_X, para->src_rect.x);
			ge_iowrite32(BLIT_SRC_RECT_OFFSET_Y, para->src_rect.y);
			ge_iowrite32(BLIT_SRC_IMGADDR, para->src_addr[0]);
			ge_iowrite32(BLIT_SRC_IMG_FMT, para->src_img.img_fmt);
			ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
			ge_iowrite32(BLIT_RST_IMGADDR_Y, para->dst_addr[0]);
			if (para->dst_yuv_sel) {
				ge_iowrite32(BLIT_DST_IMGADDR_1,
							 para->dst_addr[1]);
				ge_iowrite32(BLIT_RST_IMGADDR_UV,
							 para->dst_addr[1]);
				//ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
				//if (para->ybt_709_flag) {
				//	blit_rgb2yuv_coef();
				//}
			}
			//rst yuv sel
			ge_iowrite32(BLIT_RST_YUV, para->dst_yuv_sel);
			ge_iowrite32(BLIT_IMG_FMT, 4);
			//rst img fmt
			if (!para->dst_yuv_sel)
				ge_iowrite32(BLIT_IMG_FMT,
							 para->dst_img.img_fmt);
			else
				ge_iowrite32(BLIT_YUV_MODE,
							 para->dst_img.img_fmt);
		} else { //yuv2rgb
			ge_iowrite32(BLIT_SRCRECT_SEL, 1);
			ge_iowrite32(BLIT_DST_IMGSIZE_W, para->dst_img.w);
			ge_iowrite32(BLIT_DST_IMGSIZE_H, para->dst_img.h);
			ge_iowrite32(BLIT_DST_RECT_OFFSET_X, para->dst_rect.x);
			ge_iowrite32(BLIT_DST_RECT_OFFSET_Y, para->dst_rect.y);
			ge_iowrite32(BLIT_DST_IMGADDR_0, para->dst_addr[0]);
			ge_iowrite32(BLIT_DST_IMGADDR_1, para->dst_addr[1]);
			ge_iowrite32(BLIT_RST_IMGADDR_Y, para->rst_addr[0]);
			ge_iowrite32(BLIT_RST_YUV, 0);
			//dst yuv sel
			ge_iowrite32(BLIT_DST_YUV, para->dst_yuv_sel);
			//ge_iowrite32(BLIT_YBT, para->ybt_709_flag);
			//if (para->ybt_709_flag) {
			//	blit_rgb2yuv_coef();
			//}
			ge_iowrite32(BLIT_YUV_MODE, para->dst_img.img_fmt);
			ge_iowrite32(BLIT_IMG_FMT, para->rst_img.img_fmt);
		}
		ge_iowrite32(BLIT_WORK_MODE, para->work_mode);
		ge_iowrite32(BLIT_COLOR_MODE, 0);
		ge_iowrite32(BLIT_PURE_COLOR_RD_MSB_EN, para->pure_color_rd_msb);
		ge_iowrite32(BLIT_PURE_COLOR_WR_MSB_EN, para->pure_color_wr_msb);
		break;
	default:
		return -EINVAL;
	}

	ge_iowrite32(BLIT_SRC_ALPHA, para->alpha.sel_src_alpha);
	//ge_iowrite32(BLIT_DST_ALPHA, para->alpha.sel_dst_alpha);
	ge_iowrite32(BLIT_OUT_ALPHA, para->alpha.sel_rst_alpha);
	ge_iowrite32(BLIT_SRC_GLOBAL_ALPHA, para->alpha.src_global_alpha);
	//£¶ge_iowrite32(BLIT_DST_GLOBAL_ALPHA, para->alpha.dst_global_alpha);
	ge_iowrite32(BLIT_RST_GLOBAL_ALPHA, para->alpha.rst_global_alpha);
	ge_iowrite32(BLIT_SRC_RECT_SIZE_W, para->src_rect.w);
	ge_iowrite32(BLIT_SRC_RECT_SIZE_H, para->src_rect.h);
	ge_iowrite32(BLIT_RST_RECT_OFFSET_X, para->dst_rect.x);
	ge_iowrite32(BLIT_RST_RECT_OFFSET_Y, para->dst_rect.y);
	ge_iowrite32(BLIT_RST_IMGSIZE_W, para->dst_img.w);
	ge_iowrite32(BLIT_RST_IMGSIZE_H, para->dst_img.h);

	//for (int addr = BLIT_DST_ALPHA; addr <= BLIT_SRC_ALPHA; addr += 4) {
	//	DiagPrintf("alpha read 0x%08x 0x%08lx\n\r", addr, ge_ioread32(addr));
	//}
	//for (int addr = BLIT_RST_GLOBAL_ALPHA; addr <= BLIT_SRC_GLOBAL_ALPHA; addr += 4) {
	//	DiagPrintf("global alpha read 0x%08x 0x%08lx\n\r", addr, ge_ioread32(addr));
	//}

	//DiagPrintf("src_rect.w read 0x%08x 0x%08lx %ld\n\r", BLIT_SRC_RECT_SIZE_W, ge_ioread32(BLIT_SRC_RECT_SIZE_W), para->src_rect.w);
	//DiagPrintf("src_rect.h read 0x%08x 0x%08lx %ld\n\r", BLIT_SRC_RECT_SIZE_H, ge_ioread32(BLIT_SRC_RECT_SIZE_H), para->src_rect.h);
	//DiagPrintf("dst_rect.x read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_RECT_OFFSET_X, ge_ioread32(BLIT_RST_RECT_OFFSET_X), para->dst_rect.x);
	//DiagPrintf("dst_rect.y read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_RECT_OFFSET_Y, ge_ioread32(BLIT_RST_RECT_OFFSET_Y), para->dst_rect.y);
	//DiagPrintf("dst_img.w read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_IMGSIZE_W, ge_ioread32(BLIT_RST_IMGSIZE_W), para->dst_img.w);
	//DiagPrintf("dst_img.h read 0x%08x 0x%08lx %ld\n\r", BLIT_RST_IMGSIZE_H, ge_ioread32(BLIT_RST_IMGSIZE_H), para->dst_img.h);



	//DiagPrintf("before start\n");

	ge_iowrite32(BLIT_INT_EN, 0x1);
	//DiagPrintf("interrupt enable  flag %ld\n\r", ge_ioread32(BLIT_INT_FLAG));

	ge_iowrite32(BLIT_CTRL, 1);

	//timeleft = wait_for_completion_interruptible_timeout(
	//	&rts_blit_device->complete, msecs_to_jiffies(GE_TIMEOUT_MS));
	DiagPrintf("busy_flag %d \n",  ge_ioread32(BLIT_CTRL));
	DiagPrintf("int flag %d \n",  ge_ioread32(BLIT_INT_FLAG));


	int timeout = 0;
	do {

		busy_status = ge_ioread32(BLIT_CTRL);
		count++;
		if (count % 2000000 == 0) {
			DiagPrintf("wait ge blit done count %d \n\r", count);
			DiagPrintf("interrupt  flag %ld busy %ld \n\r", ge_ioread32(BLIT_INT_FLAG), busy_status);
		}
		if (count == 10000000) {
			timeout = 1;
		}

	} while (busy_status != 0 && !timeout);

	DiagPrintf("interrupt  flag %ld busy %ld \n\r", ge_ioread32(BLIT_INT_FLAG), ge_ioread32(BLIT_CTRL));


	if (timeout) {
		return -1;
	} else {
		return 0;
	}
}






