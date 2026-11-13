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


#ifndef __DRIVERS_RTS_BLIT_REGS_H__
#define __DRIVERS_RTS_BLIT_REGS_H__

//#define VIDEO_GE_BASE               0x60102000

#define BLIT_CTRL 0x0000
#define BLIT_WORK_MODE 0x0004
#define BLIT_SRCRECT_SEL 0x0008
#define BLIT_COLOR_MODE 0x000C
#define BLIT_YUV_MODE 0x0010
#define BLIT_DMA_START 0x0014
#define BLIT_ROP_CODE 0x0018
#define BLIT_OUT_ALPHA 0x001C
#define BLIT_SRC_ALPHA 0x0020
#define BLIT_IMG_FMT 0x0024
#define BLIT_RST_YUV 0x0028
#define BLIT_DST_YUV 0x002C
#define BLIT_DST_IMG_FMT 0x0030
#define BLIT_SRC_IMG_FMT 0x0034
#define BLIT_DST_IMGADDR_0 0x0038
#define BLIT_DST_IMGADDR_1 0x003C
#define BLIT_SRC_IMGADDR 0x0040
#define BLIT_RST_IMGADDR_Y 0x0044
#define BLIT_RST_IMGADDR_UV 0x0048
#define BLIT_RST_GLOBAL_ALPHA 0x004C
#define BLIT_SRC_GLOBAL_ALPHA 0x0050
#define BLIT_SRC_IMGSIZE_W 0x0054
#define BLIT_SRC_IMGSIZE_H 0x0058
#define BLIT_SRC_RECT_OFFSET_X 0x005C
#define BLIT_SRC_RECT_OFFSET_Y 0x0060
#define BLIT_SRC_RECT_SIZE_W 0x0064
#define BLIT_SRC_RECT_SIZE_H 0x0068
#define BLIT_FILL_RECT_COLOR 0x006C
#define BLIT_DST_IMGSIZE_W 0x0070
#define BLIT_DST_IMGSIZE_H 0x0074
#define BLIT_DST_RECT_OFFSET_X 0x0078
#define BLIT_DST_RECT_OFFSET_Y 0x007C
#define BLIT_SRC_COLOR_KEY_MAX 0x0080
#define BLIT_SRC_COLOR_KEY_MIN 0x0084
#define BLIT_DST_COLOR_KEY_MAX 0x0088
#define BLIT_DST_COLOR_KEY_MIN 0x008C
#define BLIT_INT_EN 0x0090
#define BLIT_INT_CLR 0x0094
#define BLIT_INT_FLAG 0x0098
#define BLIT_R2Y_COEF0 0x009C
#define BLIT_R2Y_COEF1 0x00A0
#define BLIT_R2Y_COEF2 0x00A4
#define BLIT_R2Y_COEF3 0x00A8
#define BLIT_R2Y_COEF4 0x00AC
#define BLIT_R2Y_COEF5 0x00B0
#define BLIT_R2Y_COEF6 0x00B4
#define BLIT_R2Y_COEF7 0x00B8
#define BLIT_R2Y_COEF8 0x00BC
#define BLIT_RGB2YUV_YOFFSET 0x00C0
#define BLIT_YMODE 0x00C4
#define BLIT_YBT 0x00C8
#define BLIT_DITHER_EN 0x00CC
#define BLIT_LFSR_MODE 0x00D0
#define BLIT_RST_IMGSIZE_W 0x00D4
#define BLIT_RST_IMGSIZE_H 0x00D8
#define BLIT_RST_RECT_OFFSET_X 0x00DC
#define BLIT_RST_RECT_OFFSET_Y 0x00E0
#define BLIT_TRANS_LENGTH_SEL 0x00E4
#define BLIT_RD_DST_DBG_SIG 0x0200
#define BLIT_RD_SRC_DBG_SIG 0x0204
#define BLIT_RD_UV_DBG_SIG 0x0208
#define BLIT_WR_DBG_SIG 0x020C
#define BLIT_RD_BUFF0_DBG_SIG 0x0210
#define BLIT_RD_BUFF1_DBG_SIG 0x0214
#define BLIT_RD_BUFF2_DBG_SIG 0x0218
#define BLIT_WR_BUFF0_DBG_SIG 0x021C
#define BLIT_WR_BUFF1_DBG_SIG 0x0220

#define BLIT_LUT_PURE_COLOR_CFG 0x0224
#define BLIT_LUT_MEM 0x0800 //x0000_0800 ~ 0x0000_09D8
#define BLIT_LUTRAW_MEM 0x0A00 //x0000_0A00 ~ 0x0000_0DFC

#define BLIT_LUT_MEM_LAST    0x9D8
#define BLIT_LUTRAW_MEM_LAST 0xDFC

#define BLIT_PURE_COLOR_RD_MSB_EN 0x0228
#define BLIT_PURE_COLOR_WR_MSB_EN 0x022c
#define GE_BLIT_CLK_EN 0x1800
#define BLIT_CLK_EN_BITS 1


#endif
