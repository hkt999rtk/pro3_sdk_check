/*
 * Copyright (C) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * MCU port of rts_isp_md HAL — replaces Linux /dev/rtsmd file ops with
 * direct calls to rts_md_init / rts_md_open / rts_md_close / rts_md_ioctl.
 */

#include <string.h>
#include <errno.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "hal_md.h"
#include "rts_camera_md.h"
#include "rts_camera_mem.h"
#include "diag.h"

#define RTS_ERR(fmt, ...)    DiagPrintf("[MD HAL] " fmt "\r\n", ##__VA_ARGS__)
#define RTS_DEBUG(fmt, ...)  /* no-op */
#define RTS_ALIGN(x, a)      (((x) + (a) - 1) & ~((a) - 1))
#define RET(x)               return RTS_RETURN(RTS_E_##x)

/* ------------------------------------------------------------------ */
/* State                                                               */
/* ------------------------------------------------------------------ */

struct rtsmd {
	SemaphoreHandle_t sem;
	uint32_t          poll_flags;
};

static bool          md_inited;
static struct rtsmd *g_md_handle;

/* ------------------------------------------------------------------ */
/* Internal poll callback (called from MD ISR context)                */
/* ------------------------------------------------------------------ */

static void md_poll_callback(uint32_t flags)
{
	BaseType_t woken = pdFALSE;

	if (!g_md_handle) {
		return;
	}

	g_md_handle->poll_flags = flags;
	DiagPrintf("0x%08x\n", flags);
	xSemaphoreGiveFromISR(g_md_handle->sem, &woken);
	portYIELD_FROM_ISR(woken);
}

/* ------------------------------------------------------------------ */
/* Struct translation helpers                                          */
/* ------------------------------------------------------------------ */

static void to_mcu_attr(const struct rts_isp_md_attr *h, struct rts_md_attr *m)
{
	memset(m, 0, sizeof(*m));

	m->md_in.x       = h->md_in.x;
	m->md_in.y       = h->md_in.y;
	m->md_in.w       = h->md_in.w;
	m->md_in.h       = h->md_in.h;
	m->md_in.scale_w = (uint8_t)h->md_in.scale_x;
	m->md_in.scale_h = (uint8_t)h->md_in.scale_y;

	m->md_out.x       = h->md_out.x;
	m->md_out.y       = h->md_out.y;
	m->md_out.w       = h->md_out.w;
	m->md_out.h       = h->md_out.h;
	m->md_out.scale_w = (uint8_t)h->md_out.scale_x;
	m->md_out.scale_h = (uint8_t)h->md_out.scale_y;

	/* HAL roi uses {x,y,w,h}; MCU roi uses {x,y,x_end,y_end} */
	m->roi.x     = h->roi.x;
	m->roi.y     = h->roi.y;
	m->roi.x_end = h->roi.x + h->roi.w;
	m->roi.y_end = h->roi.y + h->roi.h;

	m->bin_bits     = (uint8_t)h->bin_bits;
	m->nr_bins      = (uint8_t)h->nr_bins;
	m->train_frames = (uint8_t)h->train_frames;

	m->back_thd  = h->thd.back;
	m->learn     = h->thd.learn;
	m->forget    = h->thd.forget;
	m->ds_thd    = h->thd.ds;
	m->motion_thd = h->thd.motion;   /* both uint32_t — no truncation */
}

static void from_mcu_attr(const struct rts_md_attr *m, struct rts_isp_md_attr *h)
{
	memset(h, 0, sizeof(*h));

	h->md_in.x       = m->md_in.x;
	h->md_in.y       = m->md_in.y;
	h->md_in.w       = m->md_in.w;
	h->md_in.h       = m->md_in.h;
	h->md_in.scale_x = m->md_in.scale_w;
	h->md_in.scale_y = m->md_in.scale_h;

	h->md_out.x       = m->md_out.x;
	h->md_out.y       = m->md_out.y;
	h->md_out.w       = m->md_out.w;
	h->md_out.h       = m->md_out.h;
	h->md_out.scale_x = m->md_out.scale_w;
	h->md_out.scale_y = m->md_out.scale_h;

	h->roi.x = m->roi.x;
	h->roi.y = m->roi.y;
	h->roi.w = m->roi.x_end - m->roi.x;
	h->roi.h = m->roi.y_end - m->roi.y;

	h->bin_bits     = (enum MD_BIN_BITS)m->bin_bits;
	h->nr_bins      = (enum MD_NR_BINS)m->nr_bins;
	h->train_frames = m->train_frames;

	h->thd.back   = m->back_thd;
	h->thd.learn  = m->learn;
	h->thd.forget = m->forget;
	h->thd.ds     = m->ds_thd;
	h->thd.motion = m->motion_thd;
}

/* ------------------------------------------------------------------ */
/* DMA size calculation helpers (pure computation — unchanged)        */
/* ------------------------------------------------------------------ */

static void to_nr_bins(enum MD_NR_BINS *cfg, int *nrbins, int reverse)
{
	if (!reverse) {
		switch (*cfg) {
		case MD_NR_BINS_32:
			*nrbins = 32;
			break;
		default:
			*nrbins = 16;
			break;
		}
	} else {
		if (*nrbins == 32) {
			*cfg = MD_NR_BINS_32;
		} else {
			*cfg = MD_NR_BINS_16;
		}
	}
}

static void to_bin_bits(enum MD_BIN_BITS *cfg, int *bin_bits, int reverse)
{
	if (!reverse) {
		switch (*cfg) {
		case MD_BIN_BITS_4:
			*bin_bits = 4;
			break;
		default:
			*bin_bits = 2;
			break;
		}
	} else {
		if (*bin_bits == 4) {
			*cfg = MD_BIN_BITS_4;
		} else {
			*cfg = MD_BIN_BITS_2;
		}
	}
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

int rts_isp_md_init(void **omd)
{
	struct rtsmd *md;
	int ret;

	if (!omd) {
		RET(NULL_POINT);
	}

	md = pvPortMalloc(sizeof(*md));
	if (!md) {
		RET(NO_MEMORY);
	}
	memset(md, 0, sizeof(*md));

	md->sem = xSemaphoreCreateBinary();
	if (!md->sem) {
		vPortFree(md);
		RET(NO_MEMORY);
	}

	if (!md_inited) {
		rts_md_init();
		md_inited = true;
	}

	ret = rts_md_open();
	if (ret) {
		vSemaphoreDelete(md->sem);
		vPortFree(md);
		RET(OPEN_FAIL);
	}

	g_md_handle = md;
	rts_md_set_callback(md_poll_callback);

	*omd = md;
	return RTS_OK;
}

void rts_isp_md_release(void *p)
{
	struct rtsmd *md = p;

	if (!md) {
		return;
	}

	/* Unlink from ISR before touching the semaphore */
	g_md_handle = NULL;
	rts_md_set_callback(NULL);

	/* Unblock any task waiting in poll */
	xSemaphoreGive(md->sem);
	vSemaphoreDelete(md->sem);

	rts_md_close();
	vPortFree(md);
}

int rts_isp_md_set_attr(void *p, struct rts_isp_md_attr *cfg)
{
	struct rts_md_attr mcu;

	if (!p || !cfg) {
		RET(NULL_POINT);
	}

	to_mcu_attr(cfg, &mcu);
	return rts_md_ioctl(RTSMD_IOC_SET_ATTR, &mcu);
}

int rts_isp_md_get_attr(void *p, struct rts_isp_md_attr *cfg)
{
	struct rts_md_attr mcu;
	int ret;

	if (!p || !cfg) {
		RET(NULL_POINT);
	}

	ret = rts_md_ioctl(RTSMD_IOC_GET_ATTR, &mcu);
	if (ret) {
		return ret;
	}

	from_mcu_attr(&mcu, cfg);
	return RTS_OK;
}

int rts_isp_md_set_extra_attr(void *p, struct rts_isp_md_extra_attr *attr)
{
	struct rts_md_attr mcu;
	uint8_t skip_num;
	int ret;

	if (!p || !attr) {
		RET(NULL_POINT);
	}

	/* Apply burst_length via read-modify-write on the full attr */
	ret = rts_md_ioctl(RTSMD_IOC_GET_ATTR, &mcu);
	if (ret) {
		return ret;
	}

	mcu.bl_hist_wr = (uint8_t)attr->burst_length.hist_write;
	mcu.bl_res_wr  = (uint8_t)attr->burst_length.res_write;
	mcu.bl_isp_rd  = (uint8_t)attr->burst_length.isp_read;

	ret = rts_md_ioctl(RTSMD_IOC_SET_ATTR, &mcu);
	if (ret) {
		return ret;
	}

	/* Apply skip separately */
	skip_num = attr->skip.enable ? (uint8_t)attr->skip.frames : 0;
	return rts_md_ioctl(RTSMD_IOC_SET_SKIP, &skip_num);
}

int rts_isp_md_get_extra_attr(void *p, struct rts_isp_md_extra_attr *attr)
{
	if (!p || !attr) {
		RET(NULL_POINT);
	}

	/* TODO: no dedicated MCU ioctl; return zeroed defaults */
	memset(attr, 0, sizeof(*attr));
	return RTS_OK;
}

int rts_isp_md_trigger_train(void *p)
{
	if (!p) {
		RET(NULL_POINT);
	}

	return rts_md_ioctl(RTSMD_IOC_TRAIN, NULL);
}

int rts_isp_md_get_train_flag(void *p, uint32_t *flag)
{
	if (!p || !flag) {
		RET(NULL_POINT);
	}

	/* TODO: no MCU ioctl for train flag */
	*flag = 0;
	return RTS_OK;
}

int rts_isp_md_set_dma_addr(void *p,
							uint32_t hist_addr, uint32_t hist_sz,
							uint32_t res_addr, uint32_t res_sz)
{
	struct rts_md_buf buf;

	if (!p) {
		RET(NULL_POINT);
	}

	buf.hist_addr = hist_addr;
	buf.hist_size = hist_sz;
	buf.res_addr  = res_addr;
	buf.res_size  = res_sz;

	return rts_md_ioctl(RTSMD_IOC_SET_FRAME_BUF, &buf);
}

int rts_isp_md_calc_dma_size(struct rts_isp_md_attr *cfg,
							 uint32_t *hist_sz, uint32_t *res_sz)
{
	int nr_bins, bin_bits;

	if (!cfg || !hist_sz || !res_sz) {
		RET(NULL_POINT);
	}

	to_nr_bins(&cfg->nr_bins, &nr_bins, 0);
	to_bin_bits(&cfg->bin_bits, &bin_bits, 0);

	*hist_sz = RTS_ALIGN(cfg->md_in.w * cfg->md_in.h
						 * bin_bits * nr_bins / 8, 4096);
	*res_sz = RTS_ALIGN(cfg->md_out.w * cfg->md_out.h / 8, 4096);

	RTS_DEBUG("md calc dma len: %d %d\n", *hist_sz, *res_sz);

	return RTS_OK;
}

int rts_isp_md_alloc_dma(struct rtsc_dma_buffer *b)
{
	if (!b) {
		RET(NULL_POINT);
	}

	b->length = RTS_ALIGN(b->length, 4096);
	b->vm_addr = rtscam_mem_alloc(b->length, &b->phy_addr,
								  RTS_MEM_ATTR_NOCACHE, "md_dma");
	if (!b->vm_addr) {
		RET(NO_MEMORY);
	}

	return RTS_OK;
}

void rts_isp_md_free_dma(struct rtsc_dma_buffer *b)
{
	if (!b || !b->vm_addr) {
		return;
	}

	rtscam_mem_free(b->vm_addr);
	b->vm_addr = NULL;
	b->phy_addr = 0;
}

int rts_isp_md_set_axi_buffer(void *p,
							  uint32_t hist_addr, uint32_t hist_len,
							  uint32_t result_addr, uint32_t result_len)
{
	struct rts_md_buf buf;

	if (!p) {
		RET(NULL_POINT);
	}

	buf.hist_addr = hist_addr;
	buf.hist_size = hist_len;
	buf.res_addr  = result_addr;
	buf.res_size  = result_len;

	return rts_md_ioctl(RTSMD_IOC_SET_AXI_BUF, &buf);
}

int rts_isp_md_enable(void *md, int enable)
{
	if (!md) {
		RET(NULL_POINT);
	}

	if (enable) {
		return rts_md_ioctl(RTSMD_IOC_ENABLE, NULL);
	} else {
		return rts_md_ioctl(RTSMD_IOC_DISABLE, NULL);
	}
}

int rts_isp_md_get_result(void *p, struct rts_isp_md_result *res)
{
	struct rts_md_result r;
	int ret;

	if (!p || !res) {
		RET(NULL_POINT);
	}

	ret = rts_md_ioctl(RTSMD_IOC_GET_RESULT, &r);
	if (ret) {
		return ret;
	}

	res->motion_flag  = r.motion_flag;
	res->motion_count = r.count;

	return RTS_OK;
}

int rts_isp_md_done(void *md)
{
	if (!md) {
		RET(NULL_POINT);
	}

	/* Resume MD hardware for next detection cycle.
	 * rts_md_isr() calls md_pause() on MD_DONE_INT; done() re-arms it. */
	return rts_md_ioctl(RTSMD_IOC_RESUME, NULL);
}

int rts_isp_md_poll(void *p, int timeout_ms)
{
	struct rtsmd *md = p;
	TickType_t ticks;

	if (!md) {
		return RTS_FALSE;
	}

	ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
	if (xSemaphoreTake(md->sem, ticks) == pdTRUE) {
		return RTS_TRUE;
	}

	return RTS_FALSE;
}

int rts_isp_md_get_fd(void *p)
{
	if (!p) {
		RET(NULL_POINT);
	}
	return -1;  /* no file descriptor concept on MCU */
}

int rts_isp_md_poll2(void *p, int timeout_ms, uint32_t *status)
{
	struct rtsmd *md = p;
	TickType_t ticks;

	if (!md) {
		return RTS_FALSE;
	}

	ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
	if (xSemaphoreTake(md->sem, ticks) != pdTRUE) {
		return RTS_FALSE;
	}

	/* Mirror Linux semantics: fill status only on error-like condition */
	if (status && (md->poll_flags & MD_POLL_FLAG_NEED_RETRAIN)) {
		*status = md->poll_flags;
	}

	return RTS_TRUE;
}

void rts_isp_md_dump_regs(void *md)
{
	(void)md;
	/* TODO: not implemented on MCU */
}
