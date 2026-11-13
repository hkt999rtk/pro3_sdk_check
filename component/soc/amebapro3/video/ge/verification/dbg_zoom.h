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


#ifndef __GE_DRIVER_H
#define __GE_DRIVER_H

//#define GE_DEVNAME "ge_zoom"

/* mixer data fmt */
enum ge_data_fmt {
	GE_FMT_YUV422_INT = 0,
	GE_FMT_YUV422_SMI,
	GE_FMT_YUV444_INT,
	GE_FMT_RGB888_INT,
	GE_FMT_YUV444_PLA,
	GE_FMT_RGB888_PLA,
	GE_FMT_YUV420_SMI,
	GE_FMT_RGB565
};

/* fmt pixel sequence */
enum ge_fmt_order {
	GE_YUYV_ORDER = 0,
	GE_YVYU_ORDER,
	GE_UYVY_ORDER,
	GE_VYUY_ORDER,
	GE_UV_ORDER,
	GE_VU_ORDER,
	GE_YUV_ORDER,
	GE_YVU_ORDER,
	GE_UYV_ORDER,
	GE_UVY_ORDER,
	GE_VYU_ORDER,
	GE_VUY_ORDER,
	GE_RGB_ORDER,
	GE_RBG_ORDER,
	GE_GRB_ORDER,
	GE_GBR_ORDER,
	GE_BRG_ORDER,
	GE_BGR_ORDER
};

/* zoom img struct */
struct ge_zoom_img {
	enum ge_data_fmt fmt; /* pixel fmt of img frame buffer */
	enum ge_fmt_order fmt_order; /* pixel sequence of img frame buffer */
	__u32 addr[3]; /* base addr of img frame buffer in byte */
	__u32 frame_width; /* width of img frame buffer in pixel */
	__u32 frame_height; /* height of img frame buffer in pixel */
};

struct ge_zoom {
	struct ge_zoom_img src_img; /* zoom src img */
	struct ge_zoom_img dst_img; /* zoom dst img */
	__u32 zoom_start_x;
	__u32 zoom_start_y;
	__u32 crop_frame_width;
	__u32 crop_frame_height;
	__u32 zoom_step_v;
	__u32 zoom_step_h;
	__u32 zoom_ch_en;
	__u32 half_en_flag;
	__u32 quart_en_flag;
	__u32 dst_index_x;
	__u32 dst_index_y;
	__u32 dst_img_width;
	__u32 dst_img_height;
};

int zoom_test(u16 argc, u8 *argv[]) ;
/* src_img_fmt, dst_img_fmt :  YUV422_INT(0), YUV422_SMI(1), YUV444_INT(2), RGB888_INT(3),
						YUV444_PLA(4), RGB888_PLA(5), YUV420_SMI(6), RGB565(7) */
/* src_img_order_ctrl, dst_img_order_ctrl :  YUYV/YVYU/UYVY/VYUY (0 ~3) ; UV/VU (4,5) ;
	YUV/YVU/UYV/UVY/VYU/VUY (6~11) ; RGB/RBG/GRB/GBR/BRG/BGR (12~17) */
/* full_limit_flag :  1- color space full range ;  0 - color space limit range */
/* offset :  color space limit range's offset */
int run_zoom_proc(
	__u32 src_addr, __u32 dst_addr,
	__u32 img_w, __u32 img_h,
	__u32 roi_x, __u32 roi_y,
	__u32 half_en_flag, __u32 quart_en_flag,
	__u32 src_img_fmt, __u32 src_img_order_ctrl,
	__u32 dst_img_fmt, __u32 dst_img_order_ctrl,
	__u32 crop_w, __u32 crop_h, __u32 dst_w, __u32 dst_h,
	__u32 full_limit_flag, __u32 offset);


#endif /* __GE_DRIVER_H */
