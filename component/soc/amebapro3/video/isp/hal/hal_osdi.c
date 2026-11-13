/*
 * Realtek Semiconductor Corp.
 *
 * rtstream/osdi/librtsosdi.c
 *
 * Copyright (C) 2020      Luis Han<luis_han@realsil.com.cn>
 */

#include <string.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "diag.h"
#include "hal_osdi.h"
#include "rts_camera_osdi.h"
#include "rts_camera_mem.h"
#include "rts_camera.h"

#define RTS_ERR(fmt, ...)  DiagPrintf("[OSDI HAL] " fmt "\r\n", ##__VA_ARGS__)

struct rts_osdi_blk_info {
	struct osdi_blk_info blk_info;
	struct rtsc_dma_buffer pict[RTS_OSDI_PICT_NUM];
	int buf_index;
	void *pdata;
	uint32_t length;
	uint32_t fixed_buf_len;
};

struct rts_osdi_t {
	int osdi_num;
	int stream_id;
	int init_max_frame;
	struct rts_osdi_blk_info blocks[RTS_OSDI_MAX_BLK_NUM];
	struct osdi_frame_info frame_info;
	struct osdi_color_table_info color_info;
};

static uint32_t __get_pic_size(uint32_t width, uint32_t height,
							   int fmt)
{
	uint32_t length;

	switch (fmt) {
	case RTS_OSDI_PURE_COLOR:
		length = (width * height + 7) >> 3;
		break;
	case RTS_OSDI_RGBA_1111:
		length = (width * height) >> 1;
		break;
	case RTS_OSDI_RGBA_2222:
		length = width * height;
		break;
	case RTS_OSDI_RGBA_4444:
	case RTS_OSDI_RGBA_5551:
		length = (width * 2) * height;
		break;
	case RTS_OSDI_RGBA_8888:
		length = (width * 4) * height;
		break;
	default:
		length = 0;
		break;
	}

	return length;
}

static uint8_t __get_color_mode_val(int fmt)
{
	uint8_t val;

	switch (fmt) {
	case RTS_PIX_FMT_1BPP:
		val = RTS_OSDI_PURE_COLOR;
		break;
	case RTS_PIX_FMT_RGBA1111:
		val = RTS_OSDI_RGBA_1111;
		break;
	case RTS_PIX_FMT_RGBA2222:
		val = RTS_OSDI_RGBA_2222;
		break;
	case RTS_PIX_FMT_RGBA5551:
		val = RTS_OSDI_RGBA_5551;
		break;
	case RTS_PIX_FMT_RGBA4444:
		val = RTS_OSDI_RGBA_4444;
		break;
	case RTS_PIX_FMT_RGBA8888:
		val = RTS_OSDI_RGBA_8888;
		break;
	default:
		RTS_ERR("invalid fmt");
		val = RTS_OSDI_RGBA_MAX;
		break;
	}
	return val;
}

static uint32_t __get_reverse_color_mode_val(int fmt)
{
	uint32_t val;

	switch (fmt) {
	case RTS_OSDI_PURE_COLOR:
		val = RTS_PIX_FMT_1BPP;
		break;
	case RTS_OSDI_RGBA_1111:
		val = RTS_PIX_FMT_RGBA1111;
		break;
	case RTS_OSDI_RGBA_2222:
		val = RTS_PIX_FMT_RGBA2222;
		break;
	case RTS_OSDI_RGBA_5551:
		val = RTS_PIX_FMT_RGBA5551;
		break;
	case RTS_OSDI_RGBA_4444:
		val = RTS_PIX_FMT_RGBA4444;
		break;
	case RTS_OSDI_RGBA_8888:
		val = RTS_PIX_FMT_RGBA8888;
		break;
	default:
		RTS_ERR("invalid fmt");
		val = RTS_PIX_FMT_UNDEFINED;
		break;
	}
	return val;
}

static int __check_osdi_blk_and_framesize(struct rts_osdi_t *osdi,
		struct rts_osdi_blk *block, int blkidx)
{
	struct rts_osdi_blk_info *blk = NULL;
	int ret = RTS_OK;

	assert(osdi);
	assert(block);

	if (osdi->frame_info.width < block->start_x ||
		osdi->frame_info.width < (block->start_x + block->width)) {
		ret = RTS_RETURN(RTS_E_INVALID_ARG);
	}

	if (osdi->frame_info.height < block->start_y ||
		osdi->frame_info.height < (block->start_y + block->height)) {
		ret = RTS_RETURN(RTS_E_INVALID_ARG);
	}

	if (RTS_IS_ERR(ret)) {
		blk = osdi->blocks + blkidx;
		blk->blk_info.enable = RTS_FALSE;
		rts_osdi_set_blk(osdi, blkidx);
		RTS_ERR("check osd blk[%d] area and framesize fail,\n"
				"\tblk_start: [%d %d], blk_end: [%d %d], "
				"framesize: [%d %d]\n", blkidx,
				block->start_x, block->start_y,
				block->start_x + block->width,
				block->start_y + block->height,
				osdi->frame_info.width, osdi->frame_info.height);
		return ret;
	}

	return RTS_OK;
}

static int __check_osdi_frame_info(struct rts_osdi_t *osdi)
{
	struct rts_osdi_blk block;
	int i;
	int ret;

	assert(osdi);

	if (osdi->frame_info.width == 0 || osdi->frame_info.height == 0) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	for (i = 0; i < RTS_OSDI_MAX_BLK_NUM; i++) {
		struct rts_osdi_blk_info *blk = osdi->blocks + i;

		/* use HAL in-memory state — hardware registers may be
		 * uninitialised before the first rts_osdi_start() */
		if (blk->blk_info.enable == RTS_FALSE) {
			continue;
		}

		block.enable   = RTS_TRUE;
		block.fmt      = __get_reverse_color_mode_val(blk->blk_info.fmt);
		block.start_x  = blk->blk_info.start_x;
		block.start_y  = blk->blk_info.start_y;
		block.width    = blk->blk_info.width;
		block.height   = blk->blk_info.height;

		ret = __check_osdi_blk_and_framesize(osdi, &block, i);
		if (RTS_IS_ERR(ret)) {
			return ret;
		}
	}

	return RTS_OK;
}

static int __check_osdi_blk_info(struct rts_osdi_t *osdi,
								 int blkidx, struct rts_osdi_blk *block)
{
	struct rts_osdi_blk_info *blk = NULL;
	int i = 0;
	int ret = RTS_OK;
	int blk0_start_x, blk0_start_y, blk0_end_x, blk0_end_y;
	int blk1_start_x, blk1_start_y, blk1_end_x, blk1_end_y;

	assert(osdi);
	assert(block);

	if (block->start_x > (block->start_x + block->width)) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	if (block->width % 2) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	if (block->start_y > (block->start_y + block->height)) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	if ((block->fmt < RTS_PIX_FMT_RGBA1111 ||
		 block->fmt > RTS_PIX_FMT_RGBA8888) &&
		block->fmt != RTS_PIX_FMT_1BPP) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	ret = __check_osdi_blk_and_framesize(osdi, block, blkidx);
	if (RTS_IS_ERR(ret)) {
		return ret;
	}

	/* check whether blk cover each other */
	for (i = 0; i < RTS_OSDI_MAX_BLK_NUM; i++) {
		if (i == blkidx) {
			continue;
		}

		rts_osdi_get_blk(osdi, i);
		blk = osdi->blocks + i;

		if (blk->blk_info.enable == RTS_FALSE ||
			blk->blk_info.width == 0 ||
			blk->blk_info.height == 0) {
			continue;
		}
		blk0_start_x = blk->blk_info.start_x;
		blk0_start_y = blk->blk_info.start_y;
		blk0_end_x = blk->blk_info.start_x + blk->blk_info.width;
		blk0_end_y = blk->blk_info.start_y + blk->blk_info.height;
		blk1_start_x = block->start_x;
		blk1_start_y = block->start_y;
		blk1_end_x = block->start_x + block->width;
		blk1_end_y = block->start_y + block->height;

		if ((CMP(blk0_start_x, blk1_start_x, blk0_end_x) &&
			 CMP(blk0_start_y, blk1_start_y, blk0_end_y)) ||
			(CMP(blk1_start_x, blk0_start_x, blk1_end_x) &&
			 CMP(blk1_start_y, blk0_start_y, blk1_end_y))) {
			RTS_ERR("osdi can not cover each other!!\n"
					"\tblk[%d]: [%d %d][%d %d] cover blk[%d]: [%d %d][%d %d]",
					i, blk0_start_x, blk0_start_y, blk0_end_x,
					blk0_end_y, blkidx, blk1_start_x, blk1_start_y,
					blk1_end_x, blk1_end_y);
			return RTS_RETURN(RTS_E_INVALID_ARG);
		}
	}

	return ret;
}

int __update_osdi_blk_fixed_buf_len(struct rts_osdi_t *osdi)
{
	struct rts_osdi_blk_info *blk = NULL;
	int i;
	uint32_t w = 0;
	uint32_t h = 0;
	uint32_t len = 0;
	uint32_t fixed_buf_len = 0;

	if (!osdi) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (osdi->init_max_frame) {
		return RTS_OK;
	}

	struct rts_video_caps caps = {0};
	int j;

	if (rtscam_video_get_caps(osdi->stream_id, &caps) || !caps.format_caps) {
		return RTS_OK;
	}

	for (j = 0; caps.format_caps[j].pixelformat; j++) {
		uint32_t area = caps.format_caps[j].width_max *
						caps.format_caps[j].height_max;
		if (area > w * h) {
			w = caps.format_caps[j].width_max;
			h = caps.format_caps[j].height_max;
		}
	}

	if (!(w && h)) {
		return RTS_OK;
	}

	len = w * h;

	if (len > 2560 * 1440) {
		fixed_buf_len = 256 * 1024;
	} else if (len > 1920 * 1080) {
		fixed_buf_len = 160 * 1024;
	} else if (len > 640 * 480) {
		fixed_buf_len = 80 * 1024;
	} else {
		fixed_buf_len = 16 * 1024;
	}

	for (i = 0; i < RTS_OSDI_MAX_BLK_NUM; i++) {
		blk = osdi->blocks + i;
		blk->fixed_buf_len = fixed_buf_len;
	}

	osdi->init_max_frame = 1;

	return RTS_OK;
}

int __init_osdi_blocks(struct rts_osdi_t *osdi)
{
	struct rts_osdi_blk_info *blk = NULL;
	int i, j;

	if (!osdi) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	for (i = 0; i < RTS_OSDI_MAX_BLK_NUM; i++) {
		blk = osdi->blocks + i;
		blk->blk_info.enable = RTS_FALSE;
		blk->buf_index = 0;
		for (j = 0; j < RTS_OSDI_PICT_NUM; j++) {
			blk->pict[j].vm_addr = NULL;
		}
		blk->pdata = NULL;
		blk->length = 0;
		blk->fixed_buf_len = RTS_OSDI_BLK_FIXED_BUF_LEN;
	}

	return RTS_OK;
}

int __init_osdi_color_table(struct rts_osdi_t *osdi)
{
	return RTS_OK;
}

int rts_osdi_init(RtsOsdiInst *posd, int num)
{
	struct rts_osdi_t *osdi = NULL;
	int ret;

	assert(posd);

	osdi = pvPortMalloc(sizeof(*osdi));
	if (!osdi) {
		return RTS_RETURN(RTS_E_NO_MEMORY);
	}
	memset(osdi, 0, sizeof(*osdi));

	ret = __init_osdi_blocks(osdi);
	if (ret < 0) {
		RTS_ERR("init osdi blocks fail\n");
		goto exit;
	}
	ret = __init_osdi_color_table(osdi);
	if (ret < 0) {
		RTS_ERR("init color table fail\n");
		goto exit;
	}

	rtscam_osdi_init();
	ret = rtscam_osdi_open(num);
	if (ret) {
		RTS_ERR("rtscam_osdi_open failed[%d]\n", ret);
		goto exit;
	}
	osdi->osdi_num = num;
	osdi->stream_id = num;

	*posd = osdi;
	return RTS_OK;

exit:
	vPortFree(osdi);
	return ret;
}

int rts_osdi_release(RtsOsdiInst posd)
{
	int i, j, ret;
	struct rts_osdi_blk_info *blk = NULL;
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;

	if (!osdi) {
		return RTS_OK;
	}

	for (i = 0; i < RTS_OSDI_MAX_BLK_NUM; i++) {
		blk = osdi->blocks + i;
		blk->blk_info.enable = RTS_FALSE;
		rts_osdi_set_blk(osdi, i);
		for (j = 0; j < RTS_OSDI_PICT_NUM; j++) {
			if (blk->pict[j].vm_addr) {
				rtscam_mem_free(blk->pict[j].vm_addr);
				blk->pict[j].vm_addr = NULL;
				blk->pict[j].phy_addr = 0;
			}
		}
	}

	ret = rts_osdi_immediate_stop(osdi);
	if (RTS_IS_ERR(ret)) {
		return ret;
	}

	rtscam_osdi_close(osdi->osdi_num);
	vPortFree(osdi);

	return RTS_OK;
}

int rts_osdi_set_blk(RtsOsdiInst posd, int blkidx)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	int ret;

	assert(osdi);

	if (blkidx < 0 || blkidx >= RTS_OSDI_MAX_BLK_NUM) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	ret = rtscam_osdi_ioctl(osdi->osdi_num, RTSOSDI_IOC_SET_BLK,
							&osdi->blocks[blkidx].blk_info);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_SET_BLK failed[%d]\n", ret);
	}

	return ret;
}

int rts_osdi_get_blk(RtsOsdiInst posd, int blkidx)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	int ret;

	assert(osdi);

	if (blkidx < 0 || blkidx >= RTS_OSDI_MAX_BLK_NUM) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	osdi->blocks[blkidx].blk_info.index = blkidx;

	ret = rtscam_osdi_ioctl(osdi->osdi_num, RTSOSDI_IOC_GET_BLK,
							&osdi->blocks[blkidx].blk_info);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_GET_BLK failed[%d]\n", ret);
	}

	return ret;
}


int rts_osdi_config_frame(RtsOsdiInst posd,
						  struct rts_osdi_frame_info *frame_info)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	int ret;

	assert(osdi);

	osdi->frame_info.width = frame_info->width;
	osdi->frame_info.height = frame_info->height;

	ret = __check_osdi_frame_info(osdi);
	if (ret) {
		RTS_ERR("invalid frame info\n");
		return ret;
	}

	__update_osdi_blk_fixed_buf_len(osdi);

	ret = rtscam_osdi_ioctl(osdi->osdi_num, RTSOSDI_IOC_CONFIG_FRAME, &osdi->frame_info);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_CONFIG_FRAME failed[%d]\n", ret);
	}

	return ret;
}

int rts_osdi_start(RtsOsdiInst posd)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	int ret = RTS_OK;

	assert(osdi);

	ret = rtscam_osdi_ioctl(osdi->osdi_num, RTSOSDI_IOC_START, NULL);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_START failed[%d]\n", ret);
	}

	return ret;
}

int rts_osdi_frame_stop(RtsOsdiInst posd)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	int ret = RTS_OK;

	assert(osdi);

	ret = rtscam_osdi_ioctl(osdi->osdi_num, RTSOSDI_IOC_FRAME_STOP, NULL);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_FRAME_STOP failed[%d]\n", ret);
	}

	return ret;
}

int rts_osdi_immediate_stop(RtsOsdiInst posd)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	int ret = RTS_OK;

	assert(osdi);

	ret = rtscam_osdi_ioctl(osdi->osdi_num, RTSOSDI_IOC_IMMEDIATE_STOP, NULL);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_IMMEDIATE_STOP failed[%d]\n", ret);
	}

	return ret;
}

int rts_osdi_config_block(RtsOsdiInst posd,
						  int blkidx, struct rts_osdi_blk *block)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	struct rts_osdi_blk_info *blk = NULL;
	struct rtsc_dma_buffer blk_pict;
	int i, ret, length;
	int buf_index;

	assert(osdi);

	if (blkidx < 0 || blkidx >= RTS_OSDI_MAX_BLK_NUM) {
		RTS_ERR("osdi block index(%d) should be 0 to %d\n", blkidx,
				(RTS_OSDI_MAX_BLK_NUM - 1));
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	blk = osdi->blocks + blkidx;
	ret = __check_osdi_blk_info(osdi, blkidx, block);
	if (RTS_IS_ERR(ret)) {
		return ret;
	}

	if (block->pdata == NULL) {
		blk->blk_info.enable = RTS_FALSE;
		rts_osdi_set_blk(osdi, blkidx);
		for (i = 0; i < RTS_OSDI_PICT_NUM; i++) {
			if (blk->pict[i].vm_addr) {
				rtscam_mem_free(blk->pict[i].vm_addr);
				blk->pict[i].vm_addr = NULL;
				blk->pict[i].phy_addr = 0;
			}
		}

		return RTS_OK;
	}

	length = __get_pic_size(block->width, block->height,
							__get_color_mode_val(block->fmt));
	if (length > block->length) {
		return RTS_RETURN(RTS_E_INVALID_LENGTH);
	}

	if (blk->buf_index) {
		blk->buf_index = 0;
	} else {
		blk->buf_index = 1;
	}

	buf_index = blk->buf_index;

	if (!blk->pict[buf_index].vm_addr ||
		blk->pict[buf_index].length < length ||
		blk->pict[buf_index].length < block->fixed_buf_len) {
		memset(&blk_pict, 0, sizeof(blk_pict));
		blk_pict.vm_addr = NULL;
		blk_pict.buf_io = RTSC_BUF_TO_DEVICE;
		blk_pict.direction |= RTSC_DMA_BUFFER_FROM_END;
		blk_pict.length = length > block->fixed_buf_len ?
						  length : block->fixed_buf_len;
		sprintf(blk_pict.name, "OSDI Pict");
		if (blk->pict[buf_index].vm_addr) {
			rtscam_mem_free(blk->pict[buf_index].vm_addr);
			blk->pict[buf_index].vm_addr = NULL;
			blk->pict[buf_index].phy_addr = 0;
		}

		blk_pict.vm_addr = rtscam_mem_alloc(blk_pict.length, &blk_pict.phy_addr,
											RTS_MEM_ATTR_NOCACHE, "OSDI Pict");
		if (!blk_pict.vm_addr) {
			RTS_ERR("alloc osd blk dma fail\n");
			return RTS_RETURN(RTS_E_NO_MEMORY);
		}
		memcpy(&blk->pict[buf_index], &blk_pict,
			   sizeof(struct rtsc_dma_buffer));
	}

	memcpy(blk->pict[buf_index].vm_addr, block->pdata, length);

	blk->blk_info.index = block->index;
	blk->blk_info.enable = block->enable;
	blk->blk_info.fmt = __get_color_mode_val(block->fmt);
	blk->blk_info.width = block->width;
	blk->blk_info.height = block->height;
	blk->blk_info.start_x = block->start_x;
	blk->blk_info.start_y = block->start_y;
	blk->blk_info.transfer_length = 2;
	blk->blk_info.phy_addr = blk->pict[buf_index].phy_addr;
	blk->pdata = block->pdata;
	blk->length = block->length;
	blk->fixed_buf_len = block->fixed_buf_len;

	rts_osdi_set_blk(osdi, blkidx);

	return RTS_OK;
}

int rts_osdi_query_block(RtsOsdiInst posd, int blkidx,
						 struct rts_osdi_blk *block)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	struct rts_osdi_blk_info *blk = NULL;

	assert(osdi);

	if (blkidx < 0 || blkidx >= RTS_OSDI_MAX_BLK_NUM) {
		RTS_ERR("osdi block index(%d) should be 0 to %d\n", blkidx,
				(RTS_OSDI_MAX_BLK_NUM - 1));
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	rts_osdi_get_blk(osdi, blkidx);

	blk = osdi->blocks + blkidx;

	block->index = blk->blk_info.index;
	block->enable = blk->blk_info.enable;
	block->fmt = __get_reverse_color_mode_val(blk->blk_info.fmt);
	block->length = blk->length;
	block->pdata = blk->pdata;
	block->width = blk->blk_info.width;
	block->height = blk->blk_info.height;
	block->start_x = blk->blk_info.start_x;
	block->start_y = blk->blk_info.start_y;
	block->transfer_length = blk->blk_info.transfer_length;
	block->fixed_buf_len = blk->fixed_buf_len;

	return RTS_OK;
}

int rts_osdi_enable_block(RtsOsdiInst posd, int blkidx)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	struct rts_osdi_blk_info *blk = NULL;
	struct rts_osdi_blk block;
	int ret;

	assert(osdi);

	if (blkidx < 0 || blkidx >= RTS_OSDI_MAX_BLK_NUM) {
		RTS_ERR("osdi block index(%d) should be 0 to %d\n", blkidx,
				(RTS_OSDI_MAX_BLK_NUM - 1));
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	blk = osdi->blocks + blkidx;
	ret = rts_osdi_query_block(osdi, blkidx, &block);
	if (RTS_IS_ERR(ret)) {
		return ret;
	}
	ret = __check_osdi_blk_info(osdi, blkidx, &block);
	if (RTS_IS_ERR(ret)) {
		return ret;
	}

	blk->blk_info.enable = RTS_TRUE;
	rts_osdi_set_blk(osdi, blkidx);

	return RTS_OK;
}

int rts_osdi_disable_block(RtsOsdiInst posd, int blkidx)
{
	struct rts_osdi_t *osdi = (struct rts_osdi_t *)posd;
	struct rts_osdi_blk_info *blk = NULL;

	assert(osdi);

	if (blkidx < 0 || blkidx >= RTS_OSDI_MAX_BLK_NUM) {
		RTS_ERR("osdi block index(%d) should be 0 to %d\n", blkidx,
				(RTS_OSDI_MAX_BLK_NUM - 1));
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	blk = osdi->blocks + blkidx;
	blk->blk_info.index = blkidx;
	blk->blk_info.enable = RTS_FALSE;

	rts_osdi_set_blk(osdi, blkidx);

	return RTS_OK;
}

int rts_osdi_set_color_table(int fmt, uint32_t val,
							 uint8_t red, uint8_t green, uint8_t blue,
							 uint8_t alpha)
{
	struct osdi_color_table_info info = {0};

	info.fmt = __get_color_mode_val(fmt);
	if (info.fmt == RTS_OSDI_RGBA_MAX) {
		RTS_ERR("set fmt fail\n");
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}
	info.val = val;
	info.red = red;
	info.green = green;
	info.blue = blue;
	info.alpha = alpha;

	return rtscam_osdi_ioctl(0, RTSOSDI_IOC_SET_COLOR_TABLE, &info);
}

int rts_osdi_get_color_table(int fmt, uint32_t *val,
							 uint8_t red, uint8_t green,
							 uint8_t blue, uint8_t alpha)
{
	struct osdi_color_table_info info = {0};
	int ret;

	info.fmt = __get_color_mode_val(fmt);
	if (info.fmt == RTS_OSDI_RGBA_MAX) {
		RTS_ERR("set fmt fail\n");
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}
	info.red = red;
	info.green = green;
	info.blue = blue;
	info.alpha = alpha;

	ret = rtscam_osdi_ioctl(0, RTSOSDI_IOC_GET_COLOR_TABLE, &info);
	if (ret) {
		RTS_ERR("ioctl RTSOSDI_IOC_GET_COLOR_TABLE failed[%d]\n", ret);
	}

	if (val) {
		*val = info.val;
	}

	return ret;
}
