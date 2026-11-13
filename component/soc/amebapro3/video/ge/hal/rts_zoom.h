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


#ifndef __RTS_ZOOM_H__
#define __RTS_ZOOM_H__

//#include <linux/interrupt.h>
//#include <linux/miscdevice.h>

#include <basic_types.h>
#include "rts_ge_driver.h"

#if 0
#define GE_DEVNAME "ge_zoom"

struct rts_zoom_dev {
	struct miscdevice misc_dev;
	struct platform_device *pdev;
	unsigned int irq;
	struct clk *clk;

	struct completion complete;
	/*struct semaphore sem;*/
};
#endif

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

/* mirror/rotate flags */
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
	__u32 full_limit_flag;
	__u32 offset;
	__u32 dst_index_x;
	__u32 dst_index_y;
	__u32 dst_img_width;
	__u32 dst_img_height;
};

#define ZOOM_OUT_TIMEOUT 	10000
#define ZOOM_WAIT_LOG		2000

#define INT_ENABLE			1

void rts_zoom_irq_handler(void);

__s32 rts_ge_zoom(struct ge_zoom *para);
void ge_set_coef(__u16 *cnvt_coef, int rgb2yuv);
__s32 rts_ge_zoom_check_end(void);


#endif /* __ZOOM_H__ */
