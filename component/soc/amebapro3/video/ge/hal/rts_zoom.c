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



#include "ameba_soc.h"
#include <errno.h>


#include "rts_zoom_regs.h"
#include "rts_zoom.h"
#include "rts_ge_driver.h"



#define pr_err DiagPrintf

struct rts_zoom_dev *rts_zoom_device;
#if 0
static void __iomem *regmap_base;
static void __iomem *zoom_clk;

struct mem_slot {
	void *mem_virt; /* Virtual address */
	dma_addr_t mem_phy; /* Physical address */
	u32 mem_len; /* Length of allocted mem */
};


static inline void ge_iowrite32(u32 offset, u32 val)
{
	iowrite32(val, (__le32 __iomem *)(regmap_base + offset));
}

static inline u32 ge_ioread32(u32 offset)
{
	return ioread32((__le32 __iomem *)(regmap_base + offset));
}
#endif

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



void rts_zoom_init(void)
{
	int i;
	int IRQ_Priority = INT_PRI_HIGH;

	HAL_WRITE32(GE_REG_BASE, ZOOM_CLK_EN, ZOOM_CLK_EN_BITS);

	//iowrite32(0x1, (__le32 __iomem *)(zoom_clk));
	ge_iowrite32(ZOOM_FULL_LIMIT, 1);  //1: full range, 0: limited range(-16)

	// config default RGB2YUV coef
	ge_iowrite32(ZOOM_TRANS_OFFSET0, 0x0);
	for (i = 0; i <= 32; i += 4) {
		ge_iowrite32(ZOOM_R2Y_COEF0 + i, rgb2yuv_cnvt_coef[i / 4]);
		//DiagPrintf("rgb2yuv %d 0x%08x\r\n", i/4, ge_ioread32(ZOOM_R2Y_COEF0 + i));
	}
	// config default YUV2RGB coef
	ge_iowrite32(ZOOM_TRANS_OFFSET1, 0x0);
	for (i = 0; i <= 32; i += 4) {
		ge_iowrite32(ZOOM_Y2R_COEF0 + i, yuv2rgb_cnvt_coef[i / 4]);
	}



	if (INT_ENABLE) {
		InterruptRegister((u32)rts_zoom_irq_handler, GE_IRQ, (u32)NULL, IRQ_Priority);
		InterruptEn(GE_IRQ, IRQ_Priority);
	}

}


void rts_zoom_initv2(void)
{
	int i;

	HAL_WRITE32(GE_REG_BASE, ZOOM_CLK_EN, ZOOM_CLK_EN_BITS);

	//iowrite32(0x1, (__le32 __iomem *)(zoom_clk));
	ge_iowrite32(ZOOM_FULL_LIMIT, 1);  //1: full range, 0: limited range(-16)

	// config default RGB2YUV coef
	ge_iowrite32(ZOOM_TRANS_OFFSET0, 0x0);
	for (i = 0; i <= 32; i += 4) {
		ge_iowrite32(ZOOM_R2Y_COEF0 + i, rgb2yuv_cnvt_coef[i / 4]);
		//DiagPrintf("rgb2yuv %d 0x%08x\r\n", i/4, ge_ioread32(ZOOM_R2Y_COEF0 + i));
	}
	// config default YUV2RGB coef
	ge_iowrite32(ZOOM_TRANS_OFFSET1, 0x0);
	for (i = 0; i <= 32; i += 4) {
		ge_iowrite32(ZOOM_Y2R_COEF0 + i, yuv2rgb_cnvt_coef[i / 4]);
	}

}










static __u32 zoom_dn_flag = 0;
static __u32 zoom_full_flag = 0;

__u32 zoom_get_int_status(void)
{
	__u32 reg_val = 0;

	reg_val = ge_ioread32(ZOOM_FRAME_WR_INT);
	return reg_val;
}

void zoom_clear_int(void)
{
	ge_iowrite32(ZOOM_FRAME_WR_INT_CLR, 1);
}

__u32 zoom_get_full_status(void)
{
	__u32 reg_val = 0;
	reg_val = ge_ioread32(ZOOM_BUFF_FULL_INT);
	return reg_val;
}

void zoom_clear_full_int(void)
{
	ge_iowrite32(ZOOM_BUFF_INT_CLR, 1);
}

void rts_zoom_irq_handler(void)
{
	//struct rts_zoom_dev *ge_dev;

	//ge_dev = (struct rts_zoom_dev *)data;

	zoom_dn_flag = zoom_get_int_status();
	zoom_full_flag = ge_ioread32(ZOOM_BUFF_FULL_INT);
	if (zoom_dn_flag != 0) {
		zoom_clear_int();
	}
	if (zoom_full_flag != 0) {
		ge_iowrite32(ZOOM_BUFF_INT_CLR, 0x1);

	}
}



static __u32 ge_fmt_order(__u32 fmt_order)
{
	__u32 order;

	switch (fmt_order) {
	case GE_YUYV_ORDER:
	case GE_UV_ORDER:
	case GE_YUV_ORDER:
	case GE_RGB_ORDER:
		order = 0;
		break;
	case GE_YVYU_ORDER:
	case GE_VU_ORDER:
	case GE_YVU_ORDER:
	case GE_RBG_ORDER:
		order = 1;
		break;
	case GE_UYVY_ORDER:
	case GE_UYV_ORDER:
	case GE_GRB_ORDER:
		order = 2;
		break;
	case GE_VYUY_ORDER:
	case GE_UVY_ORDER:
	case GE_GBR_ORDER:
		order = 3;
		break;
	case GE_VYU_ORDER:
	case GE_BRG_ORDER:
		order = 4;
		break;
	case GE_VUY_ORDER:
	case GE_BGR_ORDER:
		order = 5;
		break;
	default:
		order = 0;
		break;
	}

	return order;
}

__u32 zoom_check_para(struct ge_zoom *para)
{
	__u32 zoom_end_x = 0;
	__u32 zoom_end_y = 0;

	if (para->zoom_start_x > (para->src_img.frame_width - 7) ||
		para->zoom_start_y > para->src_img.frame_height) {
		return EINVAL;
	}
	if (para->crop_frame_width < 2 || para->crop_frame_height < 2) {
		return EINVAL;
	}
	if (para->zoom_start_x + para->crop_frame_width >
		para->src_img.frame_width) {
		return EINVAL;
	}
	if (para->zoom_start_y + para->crop_frame_height >
		para->src_img.frame_height) {
		return EINVAL;
	}
#if 0
	if (para->zoom_step_v < 341 || para->zoom_step_v > 16384) {
		return EINVAL;
	}
	if (para->zoom_step_h < 341 || para->zoom_step_h > 16384) {
		return EINVAL;
	}
#endif
	if (para->dst_index_x > para->dst_img_width ||
		para->dst_index_y > para->dst_img_height) {
		return EINVAL;
	}
	if (para->dst_img.frame_width > para->dst_img_width) {
		return EINVAL;
	}
	if (para->dst_img.frame_height > para->dst_img_height) {
		return EINVAL;
	}
	if (para->dst_index_x + para->dst_img.frame_width >
		para->dst_img_width + 1) {
		return EINVAL;
	}
	if (para->dst_index_y + para->dst_img.frame_height >
		para->dst_img_height + 1) {
		return EINVAL;
	}

	if (para->src_img.fmt <= GE_FMT_YUV422_SMI) {
		para->zoom_start_x = (para->zoom_start_x % 2) ?
							 (para->zoom_start_x - 1) :
							 para->zoom_start_x;
		zoom_end_x = para->zoom_start_x + para->crop_frame_width;
		para->crop_frame_width = (zoom_end_x % 2) ?
								 (para->crop_frame_width - 1) :
								 para->crop_frame_width;
	} else if (para->src_img.fmt == GE_FMT_YUV420_SMI) {
		para->zoom_start_x = (para->zoom_start_x % 2) ?
							 (para->zoom_start_x - 1) :
							 para->zoom_start_x;
		zoom_end_x = para->zoom_start_x + para->crop_frame_width;
		para->crop_frame_width = (zoom_end_x % 2) ?
								 (para->crop_frame_width - 1) :
								 para->crop_frame_width;
		para->zoom_start_y = (para->zoom_start_y % 2) ?
							 (para->zoom_start_y - 1) :
							 para->zoom_start_y;
		zoom_end_y = para->zoom_start_y + para->crop_frame_height;
		para->crop_frame_height =
			(zoom_end_y % 2) ? (para->crop_frame_height - 1) :
			para->crop_frame_height;
	} else if (para->dst_img.fmt <= GE_FMT_YUV422_SMI)
		para->dst_index_x = (para->dst_index_x % 2) ?
							(para->dst_index_x - 1) :
							para->dst_index_x;
	else if (para->src_img.fmt == GE_FMT_YUV420_SMI) {
		para->dst_index_x = (para->dst_index_x % 2) ?
							(para->dst_index_x - 1) :
							para->dst_index_x;
		para->dst_index_y = (para->dst_index_y % 2) ?
							(para->dst_index_y - 1) :
							para->dst_index_y;
	}

	para->src_img.frame_width = (para->src_img.frame_width % 2) ?
								(para->src_img.frame_width - 1) :
								para->src_img.frame_width;
	para->src_img.frame_height = (para->src_img.frame_height % 2) ?
								 (para->src_img.frame_height - 1) :
								 para->src_img.frame_height;
	para->crop_frame_width = (para->crop_frame_width % 2) ?
							 (para->crop_frame_width - 1) :
							 para->crop_frame_width;
	para->crop_frame_height = (para->crop_frame_height % 2) ?
							  (para->crop_frame_height - 1) :
							  para->crop_frame_height;
	para->dst_img.frame_width = (para->dst_img.frame_width % 2) ?
								(para->dst_img.frame_width - 1) :
								para->dst_img.frame_width;
	para->dst_img.frame_height = (para->dst_img.frame_height % 2) ?
								 (para->dst_img.frame_height - 1) :
								 para->dst_img.frame_height;
	para->dst_img_width = (para->dst_img_width % 2) ?
						  (para->dst_img_width - 1) :
						  para->dst_img_width;
	para->dst_img_height = (para->dst_img_height % 2) ?
						   (para->dst_img_height - 1) :
						   para->dst_img_height;
	if (para->quart_en_flag) {
		if (!((para->crop_frame_width / 2) % 2 == 1) &&
			!(para->crop_frame_width % 8 == 0)) {
			return -EINVAL;
		}
		if (!((para->crop_frame_height / 2) % 2 == 1) &&
			!(para->crop_frame_height % 8 == 0)) {
			return -EINVAL;
		}
	} else if (para->half_en_flag) {
		if (para->crop_frame_height % 4) {
			return -EINVAL;
		}
		if (para->crop_frame_width % 4) {
			return -EINVAL;
		}
	}

	return 0;
}

__s32 rts_ge_zoom(struct ge_zoom *para)
{
	__u32 src_fmt_order, dst_fmt_order;
	__u32 hs_frame_width = 0;
	__u32 hs_frame_height = 0;
	__u32 zoom_step_v = 0;
	__u32 zoom_step_h = 0;
	int i, timeleft;
	int ret;

	ge_iowrite32(ZOOM_CTRL, 0x2);
	if (para->src_img.fmt_order >= GE_YUYV_ORDER &&
		para->src_img.fmt_order <= GE_BGR_ORDER) {
		src_fmt_order = ge_fmt_order(para->src_img.fmt_order);
	} else {
		return EINVAL;
	}
	if (para->dst_img.fmt_order >= GE_YUYV_ORDER &&
		para->dst_img.fmt_order <= GE_BGR_ORDER) {
		dst_fmt_order = ge_fmt_order(para->dst_img.fmt_order);
	} else {
		return EINVAL;
	}

	ret = zoom_check_para(para);
	if (ret) {
		pr_err("%d %s, Invalid argument\n", __LINE__, __func__);
		return EINVAL;
	}

	if ((para->src_img.fmt >= GE_FMT_YUV422_INT) &&
		(para->src_img.fmt < GE_FMT_RGB565)) {
		ge_iowrite32(ZOOM_SRC_IMG_FMT, para->src_img.fmt);
		ge_iowrite32(ZOOM_SRC_IMG_CTRL, src_fmt_order);
		ge_iowrite32(ZOOM_SRC_IMG_ADDR0, para->src_img.addr[0]);
		ge_iowrite32(ZOOM_SRC_IMG_ADDR1, para->src_img.addr[1]);
		ge_iowrite32(ZOOM_SRC_IMG_ADDR2, para->src_img.addr[2]);
	} else {
		return EINVAL;
	}

	if ((para->dst_img.fmt >= GE_FMT_YUV422_INT) &&
		(para->dst_img.fmt <= GE_FMT_RGB565)) {
		ge_iowrite32(ZOOM_DST_IMG_FMT, para->dst_img.fmt);
		ge_iowrite32(ZOOM_DST_IMG_CTRL, dst_fmt_order);
		ge_iowrite32(ZOOM_DST_IMG_ADDR0, para->dst_img.addr[0]);
		ge_iowrite32(ZOOM_DST_IMG_ADDR1, para->dst_img.addr[1]);
		ge_iowrite32(ZOOM_DST_IMG_ADDR2, para->dst_img.addr[2]);
	} else {
		return EINVAL;
	}

	ge_iowrite32(ZOOM_IMG_W_SIZE, para->src_img.frame_width);
	ge_iowrite32(ZOOM_IMG_H_SIZE, para->src_img.frame_height);

	ge_iowrite32(ZOOM_CROP_START_X, para->zoom_start_x);
	ge_iowrite32(ZOOM_CROP_START_Y, para->zoom_start_y);

	ge_iowrite32(ZOOM_CROP_IMG_W_SIZE, para->crop_frame_width);
	ge_iowrite32(ZOOM_CROP_IMG_H_SIZE, para->crop_frame_height);

	ge_iowrite32(ZOOM_OUTPUT_W_SIZE, para->dst_img.frame_width);
	ge_iowrite32(ZOOM_OUTPUT_H_SIZE, para->dst_img.frame_height);

	//ge_iowrite32(ZOOM_STEP_V, para->zoom_step_v);
	//ge_iowrite32(ZOOM_STEP_H, para->zoom_step_h);

	for (i = 0; i <= 76; i += 4) {
		ge_iowrite32(ZOOM_CH0_COEF0 + i, zoom_coef[i / 4]);
	}

	ge_iowrite32(ZOOM_FRAME_WR_INT_EN, 1);
	ge_iowrite32(ZOOM_FRAME_WR_INT_CLR, 0x1);

	if (para->quart_en_flag) {
		hs_frame_width = para->crop_frame_width / 4 +
						 ((para->crop_frame_width % 4) ? 1 : 0);
		hs_frame_height = para->crop_frame_height / 4 +
						  ((para->crop_frame_height % 4) ? 1 : 0);
	} else if (para->half_en_flag) {
		hs_frame_width = para->crop_frame_width / 2 +
						 ((para->crop_frame_width % 2) ? 1 : 0);
		hs_frame_height = para->crop_frame_height / 2 +
						  ((para->crop_frame_height % 2) ? 1 : 0);
	} else {
		hs_frame_width = para->crop_frame_width;
		hs_frame_height = para->crop_frame_height;
	}

	hs_frame_width =
		(hs_frame_width % 2) ? (hs_frame_width - 1) : hs_frame_width;
	hs_frame_height =
		(hs_frame_height % 2) ? (hs_frame_height - 1) : hs_frame_height;

	zoom_step_v = 1024 * hs_frame_height / para->dst_img.frame_height;
	zoom_step_h = 1024 * hs_frame_width / para->dst_img.frame_width;
	//para->dst_img.frame_height = bk_frame_height in spec
	if (zoom_step_v < 341 || zoom_step_v > 16384) {
		pr_err("check half_en %d and quart_en %d, dst h %d and dst w %d\n",
			   para->half_en_flag, para->quart_en_flag,
			   para->dst_img.frame_height, para->dst_img.frame_width);
		return -EINVAL;
	}
	if (zoom_step_h < 341 || zoom_step_h > 16384) {
		pr_err("check half_en %d and quart_en %d, dst h %d and dst w %d\n",
			   para->half_en_flag, para->quart_en_flag,
			   para->dst_img.frame_height, para->dst_img.frame_width);
		return -EINVAL;
	}
	ge_iowrite32(ZOOM_STEP_V, zoom_step_v);
	ge_iowrite32(ZOOM_STEP_H, zoom_step_h);

	ge_iowrite32(ZOOM_HS_W_SIZE, hs_frame_width);
	ge_iowrite32(ZOOM_HS_H_SIZE, hs_frame_height);

	ge_iowrite32(ZOOM_DST_IMG_SIZE_W, para->dst_img_width);
	ge_iowrite32(ZOOM_DST_IMG_SIZE_H, para->dst_img_height);
	ge_iowrite32(ZOOM_DST_INDEX_X, para->dst_index_x);
	ge_iowrite32(ZOOM_DST_INDEX_Y, para->dst_index_y);

	ge_iowrite32(ZOOM_FULL_LIMIT, !!para->full_limit_flag);
	if (para->full_limit_flag == 0) {
		ge_iowrite32(ZOOM_TRANS_OFFSET0, para->offset);
		ge_iowrite32(ZOOM_TRANS_OFFSET1, para->offset);
	}
	if (para->full_limit_flag) {
		ge_iowrite32(ZOOM_TRANS_OFFSET0, 0x0);
		ge_iowrite32(ZOOM_TRANS_OFFSET1, 0x0);
	}

	ge_iowrite32(ZOOM_CTRL, 0x1 | (para->zoom_ch_en << 2) |
				 (para->half_en_flag << 3) |
				 (para->quart_en_flag << 4));


#if 0
	timeleft = wait_for_completion_interruptible_timeout(
				   &rts_zoom_device->complete, msecs_to_jiffies(GE_TIMEOUT_MS));
	if (timeleft <= 0) {
		for (i = 0; i <= 308; i = i + 4)
			pr_err("addr:%x, val: %x\n", 0xb8d02000 + i,
				   ge_ioread32(i));
		for (i = 0; i <= 24; i = i + 4)
			pr_err("addr:%x, val: %x\n", 0xb8d02200 + i,
				   ge_ioread32(i));
		return ETIMEDOUT;
	}
#endif

	return 0;


}



__s32 rts_ge_zoom_check_end(void)
{

	int timeout = 0, count = 0, buff_full_cnt = 0;
	u32 fr_done_int = 0, buff_full_int = 0;




	do {


		if (INT_ENABLE) {

			if (zoom_dn_flag) {
				fr_done_int = 1;
				zoom_dn_flag = 0;
			}
			if (zoom_full_flag) {
				buff_full_cnt ++;
				zoom_full_flag = 0;
			}
		} else {
			fr_done_int = ge_ioread32(ZOOM_FRAME_WR_INT);
			buff_full_int = ge_ioread32(ZOOM_BUFF_FULL_INT);

			if (buff_full_int) {
				ge_iowrite32(ZOOM_BUFF_INT_CLR, 0x1);
				buff_full_cnt++;
			}
			if (fr_done_int) {
				ge_iowrite32(ZOOM_FRAME_WR_INT_CLR, 0x1);
			}
		}
		count++;

		if (count % ZOOM_WAIT_LOG == 0) {
			DiagPrintf("wait ge zoom done count %d \n\r", count);
			DiagPrintf("interrupt  flag %ld \n\r", fr_done_int);
		}

		if (count == ZOOM_OUT_TIMEOUT) {
			timeout = 1;
		}

		vTaskDelay(1);

	} while (fr_done_int == 0 && !timeout);


	//DiagPrintf("buff_full_int %ld \n\r", ge_ioread32(ZOOM_BUFF_FULL_INT));

	DiagPrintf("interrupt  flag %ld buff_full_cnt %ld \n\r", fr_done_int, buff_full_cnt);


	if (timeout) {
		return -1;
	} else {
		return 0;
	}

}

void rts_zoom_set_rgb2yuv(void *coef_array)
{
	__u16 *karg = (__u16 *) coef_array;
	int i;

	for (i = 0; i < 9; i++) {
		rgb2yuv_cnvt_coef[i] = karg[i];
	}
	for (i = 0; i <= 32; i += 4)
		ge_iowrite32(ZOOM_R2Y_COEF0 + i,
					 rgb2yuv_cnvt_coef[i / 4]);
}

void rts_zoom_set_yuv2rgb(void *coef_array)
{
	__u16 *karg = (__u16 *) coef_array;
	int i;

	for (i = 0; i < 9; i++) {
		yuv2rgb_cnvt_coef[i] = karg[i];
	}
	for (i = 0; i <= 32; i += 4)
		ge_iowrite32(ZOOM_Y2R_COEF0 + i,
					 rgb2yuv_cnvt_coef[i / 4]);

}

void ge_set_coef(__u16 *cnvt_coef, int rgb2yuv)
{
	int ret;
	__u16 karg[9];
	int i;

	memcpy((void *)karg, (const void *)cnvt_coef,  9 * sizeof(__u16));

	if (rgb2yuv) {

		for (i = 0; i < 9; i++) {
			rgb2yuv_cnvt_coef[i] = karg[i];
		}

		for (i = 0; i <= 32; i += 4) {
			ge_iowrite32(ZOOM_R2Y_COEF0 + i, rgb2yuv_cnvt_coef[i / 4]);
			DiagPrintf("set r2y 0x%08x %d \n\r", ZOOM_R2Y_COEF0 + i, rgb2yuv_cnvt_coef[i / 4]);
		}
	} else {

		for (i = 0; i < 9; i++) {
			yuv2rgb_cnvt_coef[i] = karg[i];
		}

		for (i = 0; i <= 32; i += 4) {
			ge_iowrite32(ZOOM_Y2R_COEF0 + i, yuv2rgb_cnvt_coef[i / 4]);
			DiagPrintf("set y2r 0x%08x %d \n\r", ZOOM_Y2R_COEF0 + i, yuv2rgb_cnvt_coef[i / 4]);
		}
	}

}


