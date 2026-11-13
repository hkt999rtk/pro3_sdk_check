/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_driver.h>
#include <isp_utils.h>
#include "isp_statis.h"

#define STATIS_SIZE (2 * 21 * 1024)
static uint8_t __attribute__((aligned(16))) g_statis[STATIS_SIZE];

static inline const void *statis_get_ptr(const void *base, int offset)
{
	return base + offset;
}

static void statis_init_ptr(struct rts_isp_statis *statis,
							const struct isp_statis_buf_offset *offset,
							const void *virt)
{
	int i;
	struct rts_isp_raw_statis *raw = &statis->raw;
	struct rts_isp_awb_statis *awb = &statis->awb;
	struct rts_isp_ae_statis *ae = &statis->ae;
	struct rts_isp_af_statis *af = &statis->af;
	struct rts_isp_af_statis *af1 = &statis->af1;
	struct rts_isp_flick_statis *flick = &statis->flick;
	struct rts_isp_y_statis *y = &statis->y;

	statis->buf = virt;

	raw->r_mean = statis_get_ptr(virt, offset->raw.r_offset);
	raw->g_mean = statis_get_ptr(virt, offset->raw.g_offset);
	raw->b_mean = statis_get_ptr(virt, offset->raw.b_offset);
	raw->y_mean = statis_get_ptr(virt, offset->raw.y_offset);
	raw->rg = statis_get_ptr(virt, offset->raw.rg_offset);
	raw->bg = statis_get_ptr(virt, offset->raw.bg_offset);
	raw->pixels = statis_get_ptr(virt, offset->raw.pixels_offset);

	awb->r_mean = statis_get_ptr(virt, offset->awb.r_offset);
	awb->g_mean = statis_get_ptr(virt, offset->awb.g_offset);
	awb->b_mean = statis_get_ptr(virt, offset->awb.b_offset);
	awb->y_mean = statis_get_ptr(virt, offset->awb.y_offset);
	awb->rg = statis_get_ptr(virt, offset->awb.rg_offset);
	awb->bg = statis_get_ptr(virt, offset->awb.bg_offset);
	awb->pixels = statis_get_ptr(virt, offset->awb.pixels_offset);

	for (i = 0; i < RTS_ISP_HDR_CHAN_MAX; i++) {
		ae->hdr[i].hist =
			statis_get_ptr(virt, offset->ae.hdr[i].hist_offset);
		ae->hdr[i].y_mean =
			statis_get_ptr(virt, offset->ae.hdr[i].y_offset);
	}
	ae->hist = statis_get_ptr(virt, offset->ae.hist_offset);
	ae->y_mean = statis_get_ptr(virt, offset->ae.y_offset);

	af->y_mean = ae->hdr[0].y_mean;
	af->high_illum_cnt = statis_get_ptr(virt, offset->af.illum_cnt_offset);
	af->gr_focus_value = statis_get_ptr(virt, offset->af.gr_filter_offset);
	af->gb_focus_value = statis_get_ptr(virt, offset->af.gb_filter_offset);

	af1->y_mean = ae->hdr[1].y_mean;
	af1->high_illum_cnt =
		statis_get_ptr(virt, offset->af1.illum_cnt_offset);
	af1->gr_focus_value =
		statis_get_ptr(virt, offset->af1.gr_filter_offset);
	af1->gb_focus_value =
		statis_get_ptr(virt, offset->af1.gb_filter_offset);

	flick->ae_y_mean = ae->y_mean;

	y->hist = statis_get_ptr(virt, offset->y.hist_offset);
	y->y_mean = statis_get_ptr(virt, offset->y.y_offset);
}

static int statis_get_id(struct isp_statis *statis,
						 const struct rts_isp_statis *all)
{
	int id;

	for (id = statis->num - 1; id >= 0; id--)
		if (&statis->all[id] == all) {
			break;
		}
	return id;
}

int isp_statis_init(struct isp_statis *statis, uint32_t isp_id,
					const struct isp_statis_buf_offset *offset)
{
	int i;
	int ret;
	struct rts_isp_statis_info info;
	uint32_t addr;

	if (!statis || !offset || isp_id >= _ISP_ID_NUM) {
		return -RTS_ISP_EINVAL;
	}

	/* On AmebaPro3 VP (no MMU): virtual address == physical address */
	(void)addr;
	info.phy_addr = (uint32_t)g_statis;
	info.num = 2;
	info.size = ALIGN(offset->size, 16);

	ret = isp_os_driver_init_statis(&info);
	if (ret) {
		goto err;
	}
	statis->num = info.num;
	statis->size = info.size;
	statis->virt_addr = g_statis;

	if (!isp_calloc_array(statis->all, statis->num)) {
		goto err;
	}
	if (!isp_calloc_array(statis->refcnt, statis->num)) {
		goto err;
	}
	for (i = 0; i < statis->num; i++)
		statis_init_ptr(&statis->all[i], offset,
						statis->virt_addr + i * statis->size);
	statis->buf_id = -1;

	return RTS_ISP_OK;
err:
	isp_perror(ret, "isp statis init fail");
	isp_statis_cleanup(statis);
	return ret;
}

int isp_statis_cleanup(struct isp_statis *statis)
{
	if (!statis) {
		return -RTS_ISP_EINVAL;
	}

	statis->buf_id = -1;
	isp_free(statis->all);
	isp_free(statis->refcnt);
	isp_os_driver_cleanup_statis();
	return RTS_ISP_OK;
}

int isp_statis_update(struct isp_statis *statis,
					  const struct rts_isp_statis_data *data)
{
	struct rts_isp_raw_statis *raw;
	struct rts_isp_awb_statis *awb;
	struct rts_isp_ae_statis *ae;
	struct rts_isp_af_statis *af;
	struct rts_isp_flick_statis *flick;
	struct rts_isp_y_statis *y;

	if (!statis || !data || data->buf_id >= statis->num) {
		return -RTS_ISP_EINVAL;
	}

	statis->buf_id = data->buf_id;

	raw = &statis->all[statis->buf_id].raw;
	awb = &statis->all[statis->buf_id].awb;
	ae = &statis->all[statis->buf_id].ae;
	af = &statis->all[statis->buf_id].af;
	flick = &statis->all[statis->buf_id].flick;
	y = &statis->all[statis->buf_id].y;

	raw->frame_count = data->frame_count;

	awb->frame_count = data->frame_count;
	isp_memcpy(awb->illum_white_pixels, data->awb_reg.illum_white_pixels,
			   sizeof(awb->illum_white_pixels));
	awb->fine_r_sum = data->awb_reg.fine_r_sum;
	awb->fine_g_sum = data->awb_reg.fine_g_sum;
	awb->fine_b_sum = data->awb_reg.fine_b_sum;
	awb->fine_white_pixels = data->awb_reg.fine_white_pixels;

	ae->frame_count = data->frame_count;

	af->frame_count = data->frame_count;

	flick->frame_count = data->frame_count;
	flick->fft_sum_2_9 = data->flick_reg.fft_sum2_9;
	flick->fft_sum_2_127 = data->flick_reg.fft_sum2_127;
	flick->valid = data->flick_reg.valid;
	flick->ae_y_mean = ae->y_mean;

	y->frame_count = data->frame_count;

	return RTS_ISP_OK;
}

int isp_statis_get_all(struct isp_statis *statis,
					   const struct rts_isp_statis **all)
{
	if (!statis || !all) {
		return -RTS_ISP_EINVAL;
	}
	if (statis->buf_id < 0) {
		return -RTS_ISP_ENOTREADY;
	}
	*all = &statis->all[statis->buf_id];
	return RTS_ISP_OK;
}

void isp_statis_ref(struct isp_statis *statis, const struct rts_isp_statis *all)
{
	int id;

	if (!statis || statis->buf_id < 0 || !all) {
		return;
	}

	id = statis_get_id(statis, all);
	if (id < 0) {
		return;
	}

	statis->refcnt[id]++;
}

void isp_statis_unref(struct isp_statis *statis,
					  const struct rts_isp_statis *all)
{
	int id;

	if (!statis || statis->buf_id < 0 || !all) {
		return;
	}

	id = statis_get_id(statis, all);
	if (id < 0) {
		return;
	}

	if (statis->refcnt[id] == 0) {
		return;
	}
	statis->refcnt[id]--;
	if (statis->refcnt[id] == 0) {
		isp_os_driver_put_statis(id);
	}
}
