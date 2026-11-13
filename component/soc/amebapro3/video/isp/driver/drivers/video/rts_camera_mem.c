/*
 * Copyright 2024 Realtek
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Ported from Zephyr to FreeRTOS bare-metal:
 * - k_mutex         -> isp_rtos_mutex_t
 * - sys_dlist_t     -> struct list_head
 * - sys_dnode_t     -> struct list_head (node)
 * - sys_dlist_*     -> list_* (from list.h)
 * - k_calloc/k_free -> pvPortMalloc+memset / vPortFree
 * - DT_INST_*       -> CONFIG_RTS_MEM_* macros
 * - device_map      -> removed (phys == virt on bare-metal)
 * - sys_cache_*     -> __DSB() barrier
 */

#include "rts_camera.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_rtos_mutex.h>
#include <list.h>
#include "ameba_soc.h"
#include "rts_camera_mem.h"
#include "rts_camera_hw_config.h"

#ifndef __DSB
#define __DSB() __asm__ __volatile__("dsb 0xF" ::: "memory")
#endif

#define PAGE_SHIFT     12
#define PAGE_SIZE      (0x1 << PAGE_SHIFT)
#define ISP_BUF_ALIGN  16U   /* ISP Y/UV DDR address registers require 16-byte alignment */

#define ALIGN_UP(x, a)  (((uint32_t)(x) + (a) - 1U) & ~((a) - 1U))

struct rts_mem_dev {
	uint32_t mem_base;
	uint32_t mem_size;
	isp_rtos_mutex_t lock;

	struct list_head memorys;
	int initialized;
};

struct rtscam_mem_item {
	struct list_head node;
	uint32_t offset;    /* in pages */
	uint32_t page_cnt;
	uint32_t buf_io;
	void *virt_addr;
	uint32_t phy_addr;
	uint32_t mem_attr;

	char info[32];
};

static struct rts_mem_dev m_rmdev;

static struct rtscam_mem_item *__new_mem_item(uint32_t offset, uint32_t page_cnt)
{
	struct rtscam_mem_item *item;

	item = pvPortMalloc(sizeof(*item));
	if (!item) {
		return NULL;
	}
	memset(item, 0, sizeof(*item));

	INIT_LIST_HEAD(&item->node);
	item->offset = offset;
	item->page_cnt = page_cnt;
	item->buf_io = 0;
	item->phy_addr = m_rmdev.mem_base + (item->offset << PAGE_SHIFT);

	/* ISP hardware stores frame addresses in units of 16 bytes (bits [31:4]).
	 * mem_base is page-aligned by rtscam_mem_init, and offset is in pages,
	 * so phy_addr is always page-aligned — verify the 16-byte requirement. */
	configASSERT((item->phy_addr & (ISP_BUF_ALIGN - 1U)) == 0U);

	return item;
}

/* Allocate page_cnt pages from the top of the pool (highest address first) */
static struct rtscam_mem_item *__alloc_n(uint32_t page_cnt)
{
	int offset = 0;
	int found = 0;
	int psize = m_rmdev.mem_size >> PAGE_SHIFT;
	struct list_head *pos;
	struct list_head *next_node = NULL;
	struct rtscam_mem_item *item;

	offset = psize;

	/* Walk list from tail (highest offset) to head */
	list_for_each_prev(pos, &m_rmdev.memorys) {
		item = list_entry(pos, struct rtscam_mem_item, node);
		if (offset <= (int)item->offset) {
			continue;
		}

		if (offset - (int)(item->offset + item->page_cnt) >= (int)page_cnt) {
			found = 1;
			goto exit;
		}

		offset = item->offset;
		next_node = pos;
	}

	if (offset >= (int)page_cnt) {
		found = 1;
	}
exit:
	if (!found) {
		return NULL;
	}

	offset -= page_cnt;
	item = __new_mem_item(offset, page_cnt);
	if (!item) {
		return NULL;
	}

	if (next_node) {
		list_add(&item->node, next_node->prev);
	} else {
		list_add_tail(&item->node, &m_rmdev.memorys);
	}

	return item;
}

/* Allocate page_cnt pages from the bottom of the pool (lowest address first) */
static struct rtscam_mem_item *__alloc_p(uint32_t page_cnt)
{
	int offset = 0;
	int found = 0;
	int psize = m_rmdev.mem_size >> PAGE_SHIFT;
	struct list_head *pos;
	struct list_head *prev_node = NULL;
	struct rtscam_mem_item *item;

	list_for_each(pos, &m_rmdev.memorys) {
		item = list_entry(pos, struct rtscam_mem_item, node);
		if ((int)(item->offset - offset) >= (int)page_cnt) {
			found = 1;
			prev_node = pos->prev;
			goto exit;
		}
		offset = item->offset + item->page_cnt;
	}

	if (offset + (int)page_cnt <= psize) {
		found = 1;
	}
exit:
	if (!found) {
		return NULL;
	}

	item = __new_mem_item(offset, page_cnt);
	if (!item) {
		return NULL;
	}

	if (prev_node) {
		list_add(&item->node, prev_node);
	} else {
		list_add_tail(&item->node, &m_rmdev.memorys);
	}

	return item;
}

int rtscam_mem_init(uint32_t base, uint32_t size)
{
	uint32_t aligned_base;

	if (m_rmdev.initialized) {
		return 0;
	}

	/* Round base up to page size so every page-offset allocation is also
	 * ISP_BUF_ALIGN (16-byte) aligned. PAGE_SIZE (4096) is a multiple of 16. */
	aligned_base = ALIGN_UP(base, PAGE_SIZE);
	m_rmdev.mem_base = aligned_base;
	m_rmdev.mem_size = size - (aligned_base - base);
	isp_rtos_mutex_init(&m_rmdev.lock);
	INIT_LIST_HEAD(&m_rmdev.memorys);
	m_rmdev.initialized = 1;

	return 0;
}

void rtscam_mem_cleanup(void)
{
	struct list_head *pos, *tmp;
	struct rtscam_mem_item *item;

	if (!m_rmdev.initialized) {
		return;
	}

	isp_rtos_mutex_lock(&m_rmdev.lock);
	list_for_each_safe(pos, tmp, &m_rmdev.memorys) {
		item = list_entry(pos, struct rtscam_mem_item, node);
		list_del(&item->node);
		vPortFree(item);
	}
	isp_rtos_mutex_unlock(&m_rmdev.lock);
	m_rmdev.initialized = 0;
}

/*
 * Allocate a physically contiguous frame buffer.
 * On bare-metal MCU physical == virtual address.
 * mem_attr: upper 16 bits of the caller's flag (RTS_MEM_ATTR_*).
 */
uint8_t *rtscam_mem_alloc(size_t size, uint32_t *phy_addr,
						  uint32_t mem_attr, const char *info)
{
	uint32_t page_cnt;
	struct rtscam_mem_item *item = NULL;

	if (!m_rmdev.initialized || !size || !phy_addr) {
		return NULL;
	}

	page_cnt = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;

	isp_rtos_mutex_lock(&m_rmdev.lock);

	if (mem_attr & RTS_MEM_ATTR_NOCACHE) {
		item = __alloc_n(page_cnt);
	} else {
		item = __alloc_p(page_cnt);
	}

	isp_rtos_mutex_unlock(&m_rmdev.lock);

	if (!item) {
		return NULL;
	}

	/* On bare-metal physical == virtual */
	item->virt_addr = (void *)(uintptr_t)item->phy_addr;
	item->mem_attr = mem_attr;
	if (info) {
		strncpy(item->info, info, sizeof(item->info) - 1);
	}

	*phy_addr = item->phy_addr;
	rtsprintk(RTS_TRACE_ERROR, "alloc = 0x%08x", *phy_addr);
	return (uint8_t *)item->virt_addr;
}

void rtscam_mem_free(void *virt_addr)
{
	struct list_head *pos, *tmp;
	struct rtscam_mem_item *item;
	uint32_t phy = (uint32_t)(uintptr_t)virt_addr;

	if (!m_rmdev.initialized || !virt_addr) {
		return;
	}

	isp_rtos_mutex_lock(&m_rmdev.lock);
	list_for_each_safe(pos, tmp, &m_rmdev.memorys) {
		item = list_entry(pos, struct rtscam_mem_item, node);
		if (item->phy_addr == phy) {
			list_del(&item->node);
			vPortFree(item);
			break;
		}
	}
	isp_rtos_mutex_unlock(&m_rmdev.lock);
}

void rtscam_mem_flush_cache(void *virt_addr, size_t size)
{
	DCache_Clean((u32)(uintptr_t)virt_addr, (u32)size);
}

void rtscam_mem_inv_cache(void *virt_addr, size_t size)
{
	DCache_Invalidate((u32)(uintptr_t)virt_addr, (u32)size);
}
