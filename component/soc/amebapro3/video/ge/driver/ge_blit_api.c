/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include <errno.h>

#include "rts_blit_regs.h"
#include "rts_blit.h"
#include "rts_ge_driver.h"





int ge_blit_dma_corner(struct ge_blit *dst, __u32 dma_starting_corner)
{
	// __u32 dma_starting_corner = 0;
	__u32 src_end_x = dst->src_rect.x + dst->src_rect.w - 1;
	__u32 src_end_y = dst->src_rect.y + dst->src_rect.h - 1;
	__u32 dst_end_x = dst->dst_rect.x + dst->dst_rect.w - 1;
	__u32 dst_end_y = dst->dst_rect.y + dst->dst_rect.h - 1;
	DiagPrintf("src x %d %d y %d %d dst x %d %d y %d %d \n", dst->src_rect.x, src_end_x, dst->src_rect.y, src_end_y, dst->dst_rect.x, dst_end_x, dst->dst_rect.y,
			   dst_end_y);
	if ((dst->dst_rect.x > dst->src_rect.x) && (dst->dst_rect.x <= src_end_x) &&
		(dst_end_y == src_end_y)) { //3 or 1 dst right
		if ((dma_starting_corner == 3) || (dma_starting_corner == 1)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	} else if ((dst_end_x >= dst->src_rect.x) && (dst_end_x < src_end_x) &&
			   (dst_end_y == src_end_y)) { //0 or 2 dst left
		if ((dma_starting_corner == 0) || (dma_starting_corner == 2)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	} else if ((dst_end_y >= dst->src_rect.y) && (dst_end_y < src_end_y)) { //1 or 0 dst up
		if ((dma_starting_corner == 0) || (dma_starting_corner == 1)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	} else if ((dst->dst_rect.y > dst->src_rect.y) && (dst->dst_rect.y <= src_end_y)) {//2 or 3 dst down
		if ((dma_starting_corner == 2) || (dma_starting_corner == 3)) {
			return dma_starting_corner;
		} else {
			return -1;
		}
	}
	return dma_starting_corner;
}


void ge_blit_open(void)
{
	rts_blit_init();

	if (ge_ioread32(BLIT_CTRL) >> 2) {
		ge_iowrite32(BLIT_CTRL, 0x2);
		DiagPrintf("%d, %s, glit busy after reset %d \n", __LINE__, __func__,  ge_ioread32(BLIT_CTRL));

	}

}

int ge_blit_start(struct ge_blit *blit_cfg)
{

	int ret;

	struct ge_blit hw_para = *blit_cfg;
	struct ge_blit *para = &hw_para;

	switch (hw_para.cmd) {
	case GE_BLT_ROI_BLEND:
		hw_para.src_addr[0] = hw_para.usr_src_buf;
		hw_para.dst_addr[0] = hw_para.usr_dst_buf;
		if (hw_para.dst_yuv_sel) {
			hw_para.dst_addr[1] = hw_para.usr_dst_buf + hw_para.dst_img.w * hw_para.dst_img.h;
		}
		break;

	case GE_BLT_SELF_BLEND:
		hw_para.alpha.sel_src_alpha = 0;
		hw_para.alpha.sel_rst_alpha = 0;
		hw_para.alpha.src_global_alpha = 255;
		hw_para.alpha.rst_global_alpha = 0;
		hw_para.dst_addr[0] = hw_para.usr_dst_buf;
		break;

	case GE_BLT_FILLRECT:
		hw_para.src_img.img_fmt = hw_para.dst_img.img_fmt;
		hw_para.alpha.sel_src_alpha = 0;
		hw_para.alpha.sel_rst_alpha = 0;
		hw_para.alpha.src_global_alpha = 255;
		hw_para.alpha.rst_global_alpha = 0;
		hw_para.dst_addr[0] = hw_para.usr_dst_buf;
		break;

	case GE_BLT_FMT_CONV:
		// dst_yuv_sel :
		//1 rgb2yuv
		//0 yuv2rgb
		if (blit_cfg->dst_yuv_sel) {	 // Source: input rgb ; Destination: N/A ; Result: output yuv
			hw_para.work_mode = 2;  //no dst, src direct to rslt
			hw_para.src_addr[0] = hw_para.usr_src_buf;
			hw_para.dst_addr[0] = hw_para.usr_dst_buf;
			hw_para.dst_addr[1] = hw_para.usr_dst_buf + hw_para.src_img.w * hw_para.src_img.h;
		} else {  //yuv2rgb: Source: rectfill, alpha=0 ; Destination: input yuv ; Result: output rgb
			hw_para.dst_rect.x = 0;
			hw_para.dst_rect.y = 0;
			hw_para.work_mode = 0;
			hw_para.rst_img.img_fmt = blit_cfg->dst_img.img_fmt;
			hw_para.dst_img.img_fmt = blit_cfg->src_img.img_fmt;
			hw_para.alpha.sel_src_alpha = 0;
			hw_para.alpha.sel_rst_alpha = 1;
			hw_para.alpha.src_global_alpha = 0;
			hw_para.alpha.rst_global_alpha = 255;
			hw_para.dst_yuv_sel = 1;
			hw_para.dst_addr[0] = hw_para.usr_src_buf;
			hw_para.dst_addr[1] = hw_para.usr_src_buf + hw_para.dst_img.w * hw_para.dst_img.h;
			hw_para.rst_addr[0] = hw_para.usr_dst_buf;
		}
		break;
	}


	ret = blit_check_para(para);
	if (ret) {
		DiagPrintf("%d, %s, Invalid argument\n", __LINE__, __func__);
		return EINVAL;
	}

	blit_set_color_space(para);

	if (para->src_img.img_fmt == 7) {
		if (para->pure_color_rd_msb != para->pure_color_wr_msb) {
			DiagPrintf("when rop , alphablend and b_cover_b, ge driver will let pure_color_rd_msb equal pure_color_wr_msb.\n");
			DiagPrintf("pure_color_rd_msb = %d, pure_color_wr_msb = %d\n", para->pure_color_rd_msb, para->pure_color_wr_msb);
		}
	}

	switch (para->cmd) {
	case GE_BLT_ROI_BLEND:
		blit_set_roi_blend(para);
		break;
	case GE_BLT_SELF_BLEND:
		if ((para->dma_starting_corner > 3) || (para->dma_starting_corner < 0)) {
			DiagPrintf("dma_starting_corner should be in 0-3, while it = %d\n", para->dma_starting_corner);
			return -1;
		}
		para->dma_starting_corner = ge_blit_dma_corner(para, para->dma_starting_corner);
		if (para->dma_starting_corner == -1) {
			DiagPrintf("dma_starting_corner = %u can not be set.\n", para->dma_starting_corner);
			DiagPrintf("please check srcx: %u, srcy: %u, w: %u, h: %u, dstx: %u, dsty: %u\n",
					   para->src_rect.x, para->src_rect.y, para->src_rect.w, para->src_rect.h, para->dst_rect.x, para->dst_rect.y);
			return -1;
		}
		blit_set_self_blend(para);
		break;
	case GE_BLT_FILLRECT:
		blit_set_fillrect(para);
		break;
	case GE_BLT_FMT_CONV:
		blit_set_fmt_conv(para);
		break;
	default:
		DiagPrintf("%d, %s, Invalid cmd %d\n", __LINE__, __func__, para->cmd);
		return -EINVAL;
	}

	blit_set_alpha_ch(para);

	blit_set_size(para);

	blit_start();

	return 0;

}

int run_fb_blit_proc_v2(struct ge_context *pge_context, struct ge_blit *blit_cfg)
{
	struct ge_blit dst;
	int ret = 0;
	u32 src_fmt;
	u32 dst_fmt;

	dst = *blit_cfg;
	src_fmt = dst.src_img.img_fmt;
	dst_fmt = dst.dst_img.img_fmt;

	switch (dst.cmd) {
	case GE_BLT_ROI_BLEND:

		dst.src_addr[0] = (__u32)pge_context->surface[SRC].phy_addr;
		dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
		if (dst.dst_yuv_sel) {
			dst.dst_addr[1] = (__u32)pge_context->surface[DST].phy_addr + dst.dst_img.w * dst.dst_img.h;
		}
		if (dst.color_key_mode) {
			dst.clorkey.src_color_key_max = 2667669;//2667669; //0x28_b4_95 b_g_r
			dst.clorkey.src_color_key_min = 0;
			dst.clorkey.dst_color_key_max = 1048575; //0xf_ff_ff
			dst.clorkey.dst_color_key_min = 4095; //0x0_f_ff
			if (dst.color_key_mode == 1) {
				dst.alpha.src_global_alpha = 255;
			} else {
				dst.alpha.src_global_alpha = 127;
			}
		}

		if (src_fmt == 7) {
			if (dst.pure_color_rd_msb != dst.pure_color_wr_msb) {
				DiagPrintf("when rop and alphablend, ge driver will let pure_color_rd_msb equal pure_color_wr_msb.\n");
				DiagPrintf("pure_color_rd_msb = %d, pure_color_wr_msb = %d\n", dst.pure_color_rd_msb, dst.pure_color_wr_msb);
			}
		}
		break;
	case GE_BLT_SELF_BLEND:
		dst.src_fill_rect_color = 0;
		if ((dst.dma_starting_corner > 3) || (dst.dma_starting_corner < 0)) {
			DiagPrintf("dma_starting_corner should be in 0-3, while it = %d\n", dst.dma_starting_corner);
			return -1;
		}
		dst.dma_starting_corner = ge_blit_dma_corner(&dst, dst.dma_starting_corner);
		if (dst.dma_starting_corner == -1) {
			DiagPrintf("dma_starting_corner = %u can not be set.\n", dst.dma_starting_corner);
			DiagPrintf("please check srcx: %u, srcy: %u, w: %u, h: %u, dstx: %u, dsty: %u\n",
					   dst.src_rect.x, dst.src_rect.y, dst.src_rect.w, dst.src_rect.h, dst.dst_rect.x, dst.dst_rect.y);
			return -1;
		}

		dst.alpha.sel_src_alpha = 0;
		dst.alpha.sel_rst_alpha = 0;
		dst.alpha.src_global_alpha = 255;
		dst.alpha.rst_global_alpha = 0;

		dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
		if (src_fmt == 7) {
			if (dst.pure_color_rd_msb != dst.pure_color_wr_msb) {
				DiagPrintf("when b_cover_b, ge driver will let pure_color_rd_msb equal pure_color_wr_msb.\n");
				DiagPrintf("pure_color_rd_msb = %d, pure_color_wr_msb = %d\n", dst.pure_color_rd_msb, dst.pure_color_wr_msb);
			}
		}
		break;
	case GE_BLT_FILLRECT:
		dst.src_img.img_fmt = dst_fmt;
		dst.alpha.sel_src_alpha = 0;
		dst.alpha.sel_rst_alpha = 0;
		dst.alpha.src_global_alpha = 255;
		dst.alpha.rst_global_alpha = 0;

		dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;

		break;
	case GE_BLT_FMT_CONV:
		// dst_yuv_sel :
		//1 rgb2yuv
		//0 yuv2rgb
		dst.src_fill_rect_color = 0;
		if (dst.dst_yuv_sel) {
			dst.work_mode = 2;
			dst.src_addr[0] = (__u32)pge_context->surface[SRC].phy_addr;
			dst.dst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
			dst.dst_addr[1] = (__u32)pge_context->surface[DST].phy_addr + dst.src_img.w * dst.src_img.h;
		} else {
			dst.dst_rect.x = 0;
			dst.dst_rect.y = 0;
			dst.dst_addr[0] = (__u32)pge_context->surface[SRC].phy_addr;
			dst.dst_addr[1] = (__u32)pge_context->surface[SRC].phy_addr + dst.dst_img.w * dst.dst_img.h;
			dst.work_mode = 0;
			dst.rst_img.img_fmt = dst.dst_img.img_fmt;
			dst.dst_img.img_fmt = src_fmt;
			dst.rst_addr[0] = (__u32)pge_context->surface[DST].phy_addr;
			dst.alpha.sel_src_alpha = 0;
			dst.alpha.sel_rst_alpha = 1;
			dst.alpha.src_global_alpha = 0;
			dst.alpha.rst_global_alpha = 255;
			dst.dst_yuv_sel = 1;
		}
		break;
	}

	ge_blit_open();

	ret = ge_blit_start(&dst);
	if (ret) {
		DiagPrintf("ge_blit_start fail %x\n", ret);
		return ret;
	}
#if 0
	int count = 0;
	u32 busy_status = 0;
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
#endif

	return ret;
}

