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


#ifndef __RTS_BLIT_H__
#define __RTS_BLIT_H__

typedef s32 __s32;
typedef u32 __u32;
typedef u16 __u16;
typedef u32 __u32;
typedef u8 __u8;

#include "rts_ge_driver.h"


enum ge_blt_cmd {
	GE_BLT_ROI_BLEND = 0,
	GE_BLT_SELF_BLEND,
	GE_BLT_FILLRECT,
	GE_BLT_FMT_CONV
};

enum rgb_img_fmt {
	GE_BLT_INDEX8 = 0,
	GE_BLT_RGB565,
	GE_BLT_ARGB1555,
	GE_BLT_ARGB4444,
	GE_BLT_ARGB8888,
	GE_BLT_RGBA8888,
	GE_BLT_RGB888,
	GE_BLT_PURECLOR,
	GE_BLT_ARGB1111,
	GE_BLT_ARGB2222
};

enum yuv_img_fmt { GE_BLT_YUV420_SMI = 0, GE_BLT_YUV422_SMI };

enum blt_clorkey_mode {
	GE_NO_CLORKEY = 0,
	GE_SRC_CLORKEY,
	GE_DST_CLORKEY,
	GE_BOTH_CLORKEY
};

enum blt_work_mode { BLENDING = 0, ROP, FILL_SRCRECT };

enum blt_dma_corner {
	GE_TOP_LEFT = 0,
	GE_TOP_RIGHT,
	GE_BOTTOM_RIGHT,
	GE_BOTTOM_LEFT
};

struct blt_clorkey {
	__u32 src_color_key_max;
	__u32 src_color_key_min;
	__u32 dst_color_key_max;
	__u32 dst_color_key_min;
};

/* rectangle struct */
struct ge_rect {
	__u32 x; /* left top point coordinate x */
	__u32 y; /* left top point coordinate y */
	__u32 w; /* rectangle width */
	__u32 h; /* rectangle height */
};

struct ge_img {
	__u32 w; /* img width */
	__u32 h; /* img height */
	__u32 img_fmt; /* img fmt */
};

struct ge_alpha {
	__u32 sel_src_alpha;
	//__u32 sel_dst_alpha;
	__u32 sel_rst_alpha;
	__u32 src_global_alpha;
	//__u32 dst_global_alpha;
	__u32 rst_global_alpha;
};

struct ge_blit {
	struct ge_rect src_rect;
	struct ge_rect dst_rect;
	struct ge_img src_img;
	struct ge_img dst_img;
	struct ge_img rst_img;
	struct blt_clorkey clorkey;
	struct ge_alpha alpha;
	enum blt_clorkey_mode color_key_mode;
	enum blt_work_mode work_mode; /* ROP or BLENDING */
	enum ge_blt_cmd cmd;
	enum blt_dma_corner dma_starting_corner;
	__u32 dst_addr[2]; /* base addr of dst img frame buffer in byte */
	__u32 src_addr[1];
	__u32 rst_addr[1];
	__u32 src_fill_rect_color;
	__u32 dst_yuv_sel;
	__u32 rop_code;
	__u32 overlay_colorblk_flag;
	__u32 ybt_709_flag;
	__u32 pure_color_rd_msb;
	__u32 pure_color_wr_msb;
	__u32 bt601_range;
	__u32 usr_src_buf;
	__u32 usr_dst_buf;
};

static const __u8 a_clut[32] = { 0x0, 0xff };

static const __u8 rgb2_clut[4] = { 0x0, 0x0f, 0xf0, 0xff };

static const __u8 rgb4_clut[16] = {    0x00, 0x11, 0x22, 0x33,
									   0x44, 0x55, 0x66, 0x77,
									   0x88, 0x99, 0xaa, 0xbb,
									   0xcc, 0xdd, 0xee, 0xff
								  };

static const __u8 rgb5_clut[64] = {    0x00, 0x08, 0x10, 0x18, 0x21, 0x29, 0x31, 0x39,
									   0x42, 0x4a, 0x52, 0x5a, 0x63, 0x6b, 0x73, 0x7b,
									   0x84, 0x8c, 0x94, 0x9c, 0xa5, 0xad, 0xb5, 0xbd,
									   0xc6, 0xce, 0xd6, 0xde, 0xe7, 0xef, 0xf7, 0xff
								  };

static const __u8 rgb6_clut[64] = {
	0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28,
	0x2d, 0x31, 0x35, 0x39, 0x3d, 0x41, 0x45, 0x49, 0x4d, 0x51, 0x55,
	0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d, 0x82,
	0x86, 0x8a, 0x8e, 0x92, 0x96, 0x9a, 0x9e, 0xa2, 0xa6, 0xaa, 0xae,
	0xb2, 0xb6, 0xba, 0xbe, 0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd7, 0xdb,
	0xdf, 0xe3, 0xe7, 0xeb, 0xef, 0xf3, 0xf7, 0xfb, 0xff
};

static const __u32 index8_clut[256] = {
	0xff000000, 0xff240000, 0xff480000, 0xff6d0000, 0xff910000, 0xffb60000,
	0xffda0000, 0xffff0000, 0xff002400, 0xff242400, 0xff482400, 0xff6d2400,
	0xff912400, 0xffb62400, 0xffda2400, 0xffff2400, 0xff004800, 0xff244800,
	0xff484800, 0xff6d4800, 0xff914800, 0xffb64800, 0xffda4800, 0xffff4800,
	0xff006d00, 0xff246d00, 0xff486d00, 0xff6d6d00, 0xff916d00, 0xffb66d00,
	0xffda6d00, 0xffff6d00, 0xff009100, 0xff249100, 0xff489100, 0xff6d9100,
	0xff919100, 0xffb69100, 0xffda9100, 0xffff9100, 0xff00b600, 0xff24b600,
	0xff48b600, 0xff6db600, 0xff91b600, 0xffb6b600, 0xffdab600, 0xffffb600,
	0xff00da00, 0xff24da00, 0xff48da00, 0xff6dda00, 0xff91da00, 0xffb6da00,
	0xffdada00, 0xffffda00, 0xff00ff00, 0xff24ff00, 0xff48ff00, 0xff6dff00,
	0xff91ff00, 0xffb6ff00, 0xffdaff00, 0xffffff00, 0xff000055, 0xff240055,
	0xff480055, 0xff6d0055, 0xff910055, 0xffb60055, 0xffda0055, 0xffff0055,
	0xff002455, 0xff242455, 0xff482455, 0xff6d2455, 0xff912455, 0xffb62455,
	0xffda2455, 0xffff2455, 0xff004855, 0xff244855, 0xff484855, 0xff6d4855,
	0xff914855, 0xffb64855, 0xffda4855, 0xffff4855, 0xff006d55, 0xff246d55,
	0xff486d55, 0xff6d6d55, 0xff916d55, 0xffb66d55, 0xffda6d55, 0xffff6d55,
	0xff009155, 0xff249155, 0xff489155, 0xff6d9155, 0xff919155, 0xffb69155,
	0xffda9155, 0xffff9155, 0xff00b655, 0xff24b655, 0xff48b655, 0xff6db655,
	0xff91b655, 0xffb6b655, 0xffdab655, 0xffffb655, 0xff00da55, 0xff24da55,
	0xff48da55, 0xff6dda55, 0xff91da55, 0xffb6da55, 0xffdada55, 0xffffda55,
	0xff00ff55, 0xff24ff55, 0xff48ff55, 0xff6dff55, 0xff91ff55, 0xffb6ff55,
	0xffdaff55, 0xffffff55, 0xff0000aa, 0xff2400aa, 0xff4800aa, 0xff6d00aa,
	0xff9100aa, 0xffb600aa, 0xffda00aa, 0xffff00aa, 0xff0024aa, 0xff2424aa,
	0xff4824aa, 0xff6d24aa, 0xff9124aa, 0xffb624aa, 0xffda24aa, 0xffff24aa,
	0xff0048aa, 0xff2448aa, 0xff4848aa, 0xff6d48aa, 0xff9148aa, 0xffb648aa,
	0xffda48aa, 0xffff48aa, 0xff006daa, 0xff246daa, 0xff486daa, 0xff6d6daa,
	0xff916daa, 0xffb66daa, 0xffda6daa, 0xffff6daa, 0xff0091aa, 0xff2491aa,
	0xff4891aa, 0xff6d91aa, 0xff9191aa, 0xffb691aa, 0xffda91aa, 0xffff91aa,
	0xff00b6aa, 0xff24b6aa, 0xff48b6aa, 0xff6db6aa, 0xff91b6aa, 0xffb6b6aa,
	0xffdab6aa, 0xffffb6aa, 0xff00daaa, 0xff24daaa, 0xff48daaa, 0xff6ddaaa,
	0xff91daaa, 0xffb6daaa, 0xffdadaaa, 0xffffdaaa, 0xff00ffaa, 0xff24ffaa,
	0xff48ffaa, 0xff6dffaa, 0xff91ffaa, 0xffb6ffaa, 0xffdaffaa, 0xffffffaa,
	0xff0000ff, 0xff2400ff, 0xff4800ff, 0xff6d00ff, 0xff9100ff, 0xffb600ff,
	0xffda00ff, 0xffff00ff, 0xff0024ff, 0xff2424ff, 0xff4824ff, 0xff6d24ff,
	0xff9124ff, 0xffb624ff, 0xffda24ff, 0xffff24ff, 0xff0048ff, 0xff2448ff,
	0xff4848ff, 0xff6d48ff, 0xff9148ff, 0xffb648ff, 0xffda48ff, 0xffff48ff,
	0xff006dff, 0xff246dff, 0xff486dff, 0xff6d6dff, 0xff916dff, 0xffb66dff,
	0xffda6dff, 0xffff6dff, 0xff0091ff, 0xff2491ff, 0xff4891ff, 0xff6d91ff,
	0xff9191ff, 0xffb691ff, 0xffda91ff, 0xffff91ff, 0xff00b6ff, 0xff24b6ff,
	0xff48b6ff, 0xff6db6ff, 0xff91b6ff, 0xffb6b6ff, 0xffdab6ff, 0xffffb6ff,
	0xff00daff, 0xff24daff, 0xff48daff, 0xff6ddaff, 0xff91daff, 0xffb6daff,
	0xffdadaff, 0xffffdaff, 0xff00ffff, 0xff24ffff, 0xff48ffff, 0xff6dffff,
	0xff91ffff, 0xffb6ffff, 0xffdaffff, 0xffffffff
};

void ge_blit_irq_handler(void);
__s32 rts_ge_blit(struct ge_blit *para);
void blit_set_color_space(struct ge_blit *para);




#endif /* __BLIT_H__ */
