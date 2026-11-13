/*------------------------------------------------------------------------------
--        Copyright (c) 2015, VeriSilicon Inc. All rights reserved            --
--                                                                            --
-- This software is confidential and proprietary and may be used only as      --
--   expressly authorized by VeriSilicon in a written licensing agreement.    --
--                                                                            --
--         This entire notice must be reproduced on all copies                --
--                       and may not be removed.                              --
--                                                                            --
--------------------------------------------------------------------------------
-- Redistribution and use in source and binary forms, with or without         --
-- modification, are permitted provided that the following conditions are met:--
-- * Redistributions of source code must retain the above copyright notice,   --
--       this list of conditions and the following disclaimer.                --
-- * Redistributions in binary form must reproduce the above copyright        --
--       notice, this list of conditions and the following disclaimer in the  --
--       documentation and/or other materials provided with the distribution. --
-- * Neither the names of VeriSilicon nor the names of its contributors may be--
--       used to endorse or promote products derived from this software       --
--       without specific prior written permission.                           --
--------------------------------------------------------------------------------
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"--
-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  --
-- IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE --
-- ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  --
-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR        --
-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF       --
-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS   --
-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN    --
-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)    --
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE --
-- POSSIBILITY OF SUCH DAMAGE.                                                --
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
#include "base_type.h"
#include "dev_common_freertos.h"
#include "io_tools.h"
#include "frameworks.h"
#include "vcx_mmu_freertos.h"
#include <stddef.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

/****************** New MMU Defination *******************************/
#define MMU_MTLB_SHIFT 22
#define MMU_STLB_4K_SHIFT 12
#define MMU_STLB_64K_SHIFT 16

#define MMU_MTLB_BITS (32 - MMU_MTLB_SHIFT)
#define MMU_PAGE_4K_BITS MMU_STLB_4K_SHIFT
#define MMU_STLB_4K_BITS (32 - MMU_MTLB_BITS - MMU_PAGE_4K_BITS)
#define MMU_PAGE_64K_BITS MMU_STLB_64K_SHIFT
#define MMU_STLB_64K_BITS (32 - MMU_MTLB_BITS - MMU_PAGE_64K_BITS)

#define MMU_MTLB_ENTRY_NUM (1 << MMU_MTLB_BITS)
#define MMU_MTLB_SIZE (MMU_MTLB_ENTRY_NUM << 2)
#define MMU_STLB_4K_ENTRY_NUM (1 << MMU_STLB_4K_BITS)
#define MMU_STLB_4K_SIZE (MMU_STLB_4K_ENTRY_NUM << 2)
#define MMU_PAGE_4K_SIZE (1 << MMU_STLB_4K_SHIFT)
#define MMU_STLB_64K_ENTRY_NUM (1 << MMU_STLB_64K_BITS)
#define MMU_STLB_64K_SIZE (MMU_STLB_64K_ENTRY_NUM << 2)
#define MMU_PAGE_64K_SIZE (1 << MMU_STLB_64K_SHIFT)

#define MMU_MTLB_MASK (~((1U << MMU_MTLB_SHIFT) - 1))
#define MMU_STLB_4K_MASK ((~0U << MMU_STLB_4K_SHIFT) ^ MMU_MTLB_MASK)
#define MMU_PAGE_4K_MASK (MMU_PAGE_4K_SIZE - 1)
#define MMU_STLB_64K_MASK ((~((1U << MMU_STLB_64K_SHIFT) - 1)) ^ MMU_MTLB_MASK)
#define MMU_PAGE_64K_MASK (MMU_PAGE_64K_SIZE - 1)

/* Page offset definitions. */
#define MMU_OFFSET_4K_BITS (32 - MMU_MTLB_BITS - MMU_STLB_4K_BITS)
#define MMU_OFFSET_4K_MASK ((1U << MMU_OFFSET_4K_BITS) - 1)
#define MMU_OFFSET_16K_BITS (32 - MMU_MTLB_BITS - MMU_STLB_16K_BITS)
#define MMU_OFFSET_16K_MASK ((1U << MMU_OFFSET_16K_BITS) - 1)

#define MMU_MTLB_ENTRY_HINTS_BITS 6
#define MMU_MTLB_ENTRY_STLB_MASK (~((1U << MMU_MTLB_ENTRY_HINTS_BITS) - 1))

#define MMU_MTLB_PRESENT 0x00000001
#define MMU_MTLB_EXCEPTION 0x00000002
#define MMU_MTLB_4K_PAGE 0x00000000

#define MMU_STLB_PRESENT 0x00000001
#define MMU_STLB_EXCEPTION 0x00000002
#define MMU_STLB_4K_PAGE 0x00000000

#define MMU_FALSE 0
#define MMU_TRUE 1

#define MMU_ERR_OS_FAIL (0xffff)
#define MMU_EFAULT MMU_ERR_OS_FAIL
#define MMU_ENOTTY MMU_ERR_OS_FAIL

#define MMU_INFINITE ((u32)~0U)

#define MAX_NOPAGED_SIZE 0x20000
#define MMU_SUPPRESS_OOM_MESSAGE 1

#define MMU_IS_ERROR(status) ((status) < 0)
#define MMU_NO_ERROR(status) ((status) >= 0)
#define MMU_IS_SUCCESS(status) ((status) == MMU_STATUS_OK)

#undef MMUDEBUG
#ifdef HANTROMMU_DEBUG
#define MMUDEBUG(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define MMUDEBUG(fmt, args...)
#endif

#define MMU_ON_ERROR(func)                                                     \
	do {                                                                   \
		status = func;                                                 \
		if (MMU_IS_ERROR(status)) {                                    \
			goto onerror;                                          \
		}                                                              \
	} while (MMU_FALSE)

#define WritePageEntry(page_entry, entry_value)                                \
	(*(unsigned int *)(page_entry) = (unsigned int)(entry_value))

#define ReadPageEntry(page_entry) (*(unsigned int *)(page_entry))

#define DRIVER_NAME "hantrodma"
#define PAGE_SIZE 4096
/* simple map mode: generate mmu address which is same as input bus address*/
unsigned int simple_map;
/* this shift should be an integral multiple of mmu page size(4096).
 *  It can generate a mmu address shift in simple map mode
 */
unsigned int map_shift;

/* module_param(name, type, perm) */

enum MMURegion {
	MMU_REGION_IN,
	MMU_REGION_OUT,
	MMU_REGION_PRIVATE,
	MMU_REGION_PUB,

	MMU_REGION_COUNT
};

struct MMUNode {
	void *buf_virtual_address;
	ptr_t buf_bus_address; /* used in kernel map mode */
	int mtlb_start;
	int stlb_start;
	int mtlb_end;
	int stlb_end;
	unsigned int page_count;
	int process_id;

	struct MMUNode *next;
	struct MMUNode *prev;
};

struct MMUDDRRegion {
	unsigned long long physical_address;
	unsigned long long virtual_address;
	unsigned int page_count;

	void *node_mutex;
	struct MMUNode *simple_map_head;
	struct MMUNode *simple_map_tail;
	struct MMUNode *free_map_head;
	struct MMUNode *map_head;
	struct MMUNode *free_map_tail;
	struct MMUNode *map_tail;
};

struct MMU {
	void *page_table_mutex;
	/* Master TLB information. */
	unsigned int mtlb_size;
	unsigned long long mtlb_physical;
	void *mtlb_virtual;
	unsigned int mtlb_entries;

	int enabled;
	unsigned int stlb_size;
	unsigned long long stlb_physical;
	void *stlb_virtual;
	struct MMUDDRRegion region[MMU_REGION_COUNT];
	unsigned int page_table_array_size;
	unsigned long long page_table_array_physical;
	void *page_table_array;
};

static struct MMU *g_mmu;
extern unsigned long gBaseDDRHw;
unsigned int mmu_enable = MMU_FALSE;
static unsigned int mmu_init = MMU_FALSE;
extern unsigned int pcie;
static unsigned int region_in_mmu_start = REGION_IN_MMU_START;
static unsigned int region_in_mmu_end = REGION_IN_MMU_END;
static unsigned int region_out_mmu_start = REGION_OUT_MMU_START;
static unsigned int region_out_mmu_end = REGION_OUT_MMU_END;
static unsigned int region_private_mmu_start = REGION_PRIVATE_MMU_START;
static unsigned int region_private_mmu_end = REGION_PRIVATE_MMU_END;
volatile u8 *mmu_hwregs[HXDEC_MAX_CORES][2];

static enum MMUStatus ZeroMemory(void *memory, unsigned int bytes)
{
	memset(memory, 0, bytes);

	return MMU_STATUS_OK;
}

static enum MMUStatus AllocateMemory(unsigned int bytes, void **memory)
{
	void *pointer;
	enum MMUStatus status;

	pointer = (void *)malloc(bytes);
	MMUDEBUG(" *****VMALLOC size*****%u\n", bytes);

	if (!pointer) {
		/* Out of memory. */
		status = MMU_STATUS_OUT_OF_MEMORY;
		goto onerror;
	}

	/* Return pointer to the memory allocation. */
	*memory = pointer;

	return MMU_STATUS_OK;

onerror:
	/* Return the status. */
	return status;
}

static enum MMUStatus FreeMemory(void *memory)
{
	/* Free the memory from the freertos. */
	MMUDEBUG(" *****VFREE*****%p\n", memory);
	free(memory);
	return MMU_STATUS_OK;
}

static enum MMUStatus SMDeleteNode(struct MMUNode **pp)
{
	(*pp)->prev->next = (*pp)->next;
	(*pp)->next->prev = (*pp)->prev;

	MMUDEBUG(" *****DeleteNode size*****%u\n", (*pp)->page_count);
	FreeMemory(*pp);

	return MMU_STATUS_OK;
}

static enum MMUStatus DeleteNode(struct MMUNode **pp)
{
	(*pp)->prev->next = (*pp)->next;
	(*pp)->next->prev = (*pp)->prev;

	MMUDEBUG(" *****%s size*****%u\n", __func__, (*pp)->page_count);
	FreeMemory(*pp);

	return MMU_STATUS_OK;
}

static enum MMUStatus MergeNode(struct MMUNode *h, struct MMUNode **pp)
{
	struct MMUNode *tmp0 = h->next;
	struct MMUNode *tmp1 = h->next;

	while (tmp0) {
		/* 1th step: find front contiguous memory node */
		if (tmp0->mtlb_end == (*pp)->mtlb_start &&
		    tmp0->stlb_end == (*pp)->stlb_start) {
			tmp0->mtlb_end = (*pp)->mtlb_end;
			tmp0->stlb_end = (*pp)->stlb_end;
			tmp0->page_count += (*pp)->page_count;
			DeleteNode(pp);
			MMUDEBUG(
				" *****first merge to front. node size*****%u\n",
				tmp0->page_count);
			/* after merge to front contiguous memory node,
		     * find if there is behind contiguous memory node
			 */
			while (tmp1) {
				/* merge */
				if (tmp1->mtlb_start == tmp0->mtlb_end &&
				    tmp1->stlb_start == tmp0->stlb_end) {
					tmp1->mtlb_start = tmp0->mtlb_start;
					tmp1->stlb_start = tmp0->stlb_start;
					tmp1->page_count += tmp0->page_count;
					MMUDEBUG(
						" *****second merge to behind. node size*****%u\n",
						tmp1->page_count);
					DeleteNode(&tmp0);
					return MMU_STATUS_OK;
				}
				tmp1 = tmp1->next;
			}
			return MMU_STATUS_OK;
			/* 1th step: find behind contiguous memory node */
		} else if (tmp0->mtlb_start == (*pp)->mtlb_end &&
			   tmp0->stlb_start == (*pp)->stlb_end) {
			tmp0->mtlb_start = (*pp)->mtlb_start;
			tmp0->stlb_start = (*pp)->stlb_start;
			tmp0->page_count += (*pp)->page_count;
			DeleteNode(pp);
			MMUDEBUG(
				" *****first merge to behind. node size*****%u\n",
				tmp0->page_count);
			/* after merge to behind contiguous memory node,
		     * find if there is front contiguous memory node
			 */
			while (tmp1) {
				/* merge */
				if (tmp1->mtlb_end == tmp0->mtlb_start &&
				    tmp1->stlb_end == tmp0->stlb_start) {
					tmp1->mtlb_end = tmp0->mtlb_end;
					tmp1->stlb_end = tmp0->stlb_end;
					tmp1->page_count += tmp0->page_count;
					MMUDEBUG(
						" *****second merge to front. node size*****%u\n",
						tmp1->page_count);
					DeleteNode(&tmp0);
					return MMU_STATUS_OK;
				}
				tmp1 = tmp1->next;
			}
			return MMU_STATUS_OK;
		}
		tmp0 = tmp0->next;
	}
	return MMU_STATUS_FALSE;
}

/* Insert a node to map list */
static enum MMUStatus SMInsertNode(enum MMURegion e, struct MMUNode **pp)
{
	struct MMUNode *h;

	h = g_mmu->region[e].simple_map_head;

	h->next->prev = *pp;
	(*pp)->next = h->next;
	(*pp)->prev = h;
	h->next = *pp;
	MMUDEBUG(" *****insert bm node*****%u\n", (*pp)->page_count);

	return MMU_STATUS_OK;
}

static enum MMUStatus InsertNode(enum MMURegion e, struct MMUNode **pp,
				 unsigned int free)
{
	enum MMUStatus status;
	struct MMUNode *h, *b;

	if (free) {
		h = g_mmu->region[e].free_map_head;
		b = g_mmu->region[e].map_head;
		status = MergeNode(h, pp);
		MMUDEBUG(" *****insert free*****%u\n", (*pp)->page_count);
		if (MMU_IS_ERROR(status)) {
			/* remove from map*/
			if ((*pp)->prev && (*pp)->next) {
				(*pp)->prev->next = (*pp)->next;
				(*pp)->next->prev = (*pp)->prev;
			}
			/* insert to free map */
			h->next->prev = *pp;
			(*pp)->next = h->next;
			(*pp)->prev = h;
			h->next = *pp;
		}
	} else {
		h = g_mmu->region[e].map_head;

		h->next->prev = *pp;
		(*pp)->next = h->next;
		(*pp)->prev = h;
		h->next = *pp;
		MMUDEBUG(" *****insert unfree*****%u\n", (*pp)->page_count);
	}

	return MMU_STATUS_OK;
}

/* Create a Node */
static enum MMUStatus SMCreateNode(enum MMURegion e, struct MMUNode **node,
				   unsigned int page_count)
{
	struct MMUNode *p, **new;

	p = malloc(sizeof(struct MMUNode));
	new = &p;

	(*new)->mtlb_start = -1;
	(*new)->stlb_start = -1;
	(*new)->mtlb_end = -1;
	(*new)->stlb_end = -1;
	(*new)->process_id = 0;
	(*new)->page_count = 0;
	(*new)->prev = NULL;
	(*new)->next = NULL;
	/* Insert a uncomplete Node, it will be initialized later */
	SMInsertNode(e, new);

	/* return a new node for map buffer */
	*node = *new;
	return MMU_STATUS_OK;
}

/* Create initial Nodes */
static enum MMUStatus SMCreateNodes(void)
{
	struct MMUNode *simple_map_head;
	struct MMUNode *simple_map_tail;
	int i;

	/* Init each region map node */
	for (i = 0; i < MMU_REGION_COUNT; i++) {
		simple_map_head = malloc(sizeof(struct MMUNode));
		simple_map_tail = malloc(sizeof(struct MMUNode));

		simple_map_head->mtlb_start = -1;
		simple_map_head->stlb_start = -1;
		simple_map_head->mtlb_end = -1;
		simple_map_head->stlb_end = -1;
		simple_map_head->process_id = 0;
		simple_map_head->page_count = 0;
		simple_map_head->prev = NULL;
		simple_map_head->next = simple_map_tail;

		simple_map_tail->mtlb_start = -1;
		simple_map_tail->stlb_start = -1;
		simple_map_tail->mtlb_end = -1;
		simple_map_tail->stlb_end = -1;
		simple_map_tail->process_id = 0;
		simple_map_tail->page_count = 0;
		simple_map_tail->prev = simple_map_head;
		simple_map_tail->next = NULL;

		g_mmu->region[i].simple_map_head = simple_map_head;
		g_mmu->region[i].simple_map_tail = simple_map_tail;
	}
	return MMU_STATUS_OK;
}

static enum MMUStatus CreateNode(void)
{
	struct MMUNode *free_map_head, *map_head, *p, **pp;
	struct MMUNode *free_map_tail, *map_tail;
	int i;
	unsigned int page_count;
	unsigned int prev_stlb = 0, prev_mtlb = 0;

	/* Init each region map node */
	for (i = 0; i < MMU_REGION_COUNT; i++) {
		free_map_head = malloc(sizeof(struct MMUNode));
		map_head = malloc(sizeof(struct MMUNode));
		free_map_tail = malloc(sizeof(struct MMUNode));
		map_tail = malloc(sizeof(struct MMUNode));

		free_map_head->mtlb_start = map_head->mtlb_start = -1;
		free_map_head->stlb_start = map_head->stlb_start = -1;
		free_map_head->mtlb_end = map_head->mtlb_end = -1;
		free_map_head->stlb_end = map_head->stlb_end = -1;
		free_map_head->process_id = map_head->process_id = 0;
		free_map_head->page_count = map_head->page_count = 0;
		free_map_head->prev = map_head->prev = NULL;
		free_map_head->next = free_map_tail;
		map_head->next = map_tail;

		free_map_tail->mtlb_start = map_tail->mtlb_start = -1;
		free_map_tail->stlb_start = map_tail->stlb_start = -1;
		free_map_tail->mtlb_end = map_tail->mtlb_end = -1;
		free_map_tail->stlb_end = map_tail->stlb_end = -1;
		free_map_tail->process_id = map_tail->process_id = 0;
		free_map_tail->page_count = map_tail->page_count = 0;
		free_map_tail->prev = free_map_head;
		map_tail->prev = map_head;
		free_map_tail->next = map_tail->next = NULL;

		g_mmu->region[i].free_map_head = free_map_head;
		g_mmu->region[i].map_head = map_head;
		g_mmu->region[i].free_map_tail = free_map_tail;
		g_mmu->region[i].map_tail = map_tail;

		p = malloc(sizeof(struct MMUNode));
		pp = &p;

		switch (i) {
		case MMU_REGION_IN:
			page_count = (REGION_IN_END - REGION_IN_START + 1) /
				     PAGE_SIZE;
			p->stlb_start = region_in_mmu_start >> 12 &
					0x3FF; //hold mmu addr: 0x0
			p->mtlb_start = region_in_mmu_start >> 22;
			//end point next region start: +1; for remainder: +1
			p->stlb_end = prev_stlb =
				region_in_mmu_end >> 12 & 0x3FF;
			p->mtlb_end = prev_mtlb = region_in_mmu_end >> 22;
			p->page_count = page_count - 1; //hold mmu addr: 0x0
			break;
		case MMU_REGION_OUT:
			page_count = (REGION_OUT_END - REGION_OUT_START + 1) /
				     PAGE_SIZE;
			p->stlb_start = region_out_mmu_start >> 12 & 0x3FF;
			p->mtlb_start = region_out_mmu_start >> 22;
			p->stlb_end = prev_stlb =
				region_out_mmu_end >> 12 & 0x3FF;
			p->mtlb_end = prev_mtlb = region_out_mmu_end >> 22;
			p->page_count = page_count;
			break;
		case MMU_REGION_PRIVATE:
			page_count = (REGION_PRIVATE_END -
				      REGION_PRIVATE_START + 1) /
				     PAGE_SIZE;
			p->stlb_start = region_private_mmu_start >> 12 & 0x3FF;
			p->mtlb_start = region_private_mmu_start >> 22;
			p->stlb_end = prev_stlb =
				region_private_mmu_end >> 12 & 0x3FF;
			p->mtlb_end = prev_mtlb = region_private_mmu_end >> 22;
			p->page_count = page_count;
			break;
		case MMU_REGION_PUB:
			p->stlb_start = prev_stlb;
			p->mtlb_start = prev_mtlb;
			p->stlb_end = prev_stlb = MMU_STLB_4K_ENTRY_NUM - 1;
			p->mtlb_end = prev_mtlb = MMU_MTLB_ENTRY_NUM - 1;
			p->page_count = (p->mtlb_end - p->mtlb_start) *
						MMU_STLB_4K_ENTRY_NUM +
					p->stlb_end - p->stlb_start + 1;
			break;
		default:
			printf(" *****MMU Region Error*****\n");
			break;
		}

		p->process_id = 0;
		p->next = p->prev = NULL;

		InsertNode(i, pp, 1);
	}

	return MMU_STATUS_OK;
}

/* A simpile function to check if the map buffer is existed.
 *  it needs more complex version
 */
static enum MMUStatus SMCheckAddress(enum MMURegion e, void *virtual_address)
{
	struct MMUNode *p;

	p = g_mmu->region[e].simple_map_head->next;

	while (p) {
		if (p->buf_virtual_address == virtual_address)
			return MMU_STATUS_FALSE;
		p = p->next;
	}
	return MMU_STATUS_OK;
}

static enum MMUStatus FindFreeNode(enum MMURegion e, struct MMUNode **node,
				   unsigned int page_count)
{
	struct MMUNode *p;

	p = g_mmu->region[e].free_map_head->next;

	while (p) {
		if (p->page_count >= page_count) {
			*node = p;
			return MMU_STATUS_OK;
		}
		p = p->next;
	}
	return MMU_STATUS_FALSE;
}

static enum MMUStatus SplitFreeNode(enum MMURegion e, struct MMUNode **node,
				    unsigned int page_count)
{
	struct MMUNode *p, **new;

	p = malloc(sizeof(struct MMUNode));
	new = &p;

	**new = **node;

	(*new)->mtlb_start = (*node)->mtlb_start;
	(*new)->stlb_start = (*node)->stlb_start;
	(*new)->mtlb_end =
		(page_count + (*node)->stlb_start) / MMU_STLB_4K_ENTRY_NUM +
		(*node)->mtlb_start;
	(*new)->stlb_end =
		(page_count + (*node)->stlb_start) % MMU_STLB_4K_ENTRY_NUM;
	(*new)->process_id = (*node)->process_id;
	(*new)->page_count = page_count;

	MMUDEBUG(" *****new mtlb_start*****%d\n", (*new)->mtlb_start);
	MMUDEBUG(" *****new stlb_start*****%d\n", (*new)->stlb_start);
	MMUDEBUG(" *****new mtlb_end*****%d\n", (*new)->mtlb_end);
	MMUDEBUG(" *****new stlb_end*****%d\n", (*new)->stlb_end);
	/* Insert a new node in map */
	InsertNode(e, new, 0);

	/* Update free node in free map*/
	(*node)->page_count -= page_count;
	if ((*node)->page_count == 0) {
		DeleteNode(node);
		MMUDEBUG(" *****old node deleted*****\n");
	} else {
		(*node)->mtlb_start = (*new)->mtlb_end;
		(*node)->stlb_start = (*new)->stlb_end;

		MMUDEBUG(" *****old mtlb_start*****%d\n", (*node)->mtlb_start);
		MMUDEBUG(" *****old stlb_start*****%d\n", (*node)->stlb_start);
		MMUDEBUG(" *****old mtlb_end*****%d\n", (*node)->mtlb_end);
		MMUDEBUG(" *****old stlb_end*****%d\n", (*node)->stlb_end);
	}
	/* return a new node for map buffer */
	*node = *new;

	return MMU_STATUS_OK;
}

static enum MMUStatus SMRemoveNode(enum MMURegion e, void *buf_virtual_address,
				   unsigned int process_id)
{
	struct MMUNode *p, **pp;

	p = g_mmu->region[e].simple_map_head->next;
	pp = &p;

	while (*pp) {
		if ((*pp)->buf_virtual_address == buf_virtual_address &&
		    (*pp)->process_id == process_id) {
			SMDeleteNode(pp);
			break;
		}
		*pp = (*pp)->next;
	}

	return MMU_STATUS_OK;
}

static enum MMUStatus RemoveNode(enum MMURegion e, void *buf_virtual_address,
				 unsigned int process_id)
{
	struct MMUNode *p, **pp;

	p = g_mmu->region[e].map_head->next;
	pp = &p;

	while (*pp) {
		if ((*pp)->buf_virtual_address == buf_virtual_address &&
		    (*pp)->process_id == process_id) {
			InsertNode(e, pp, 1);
			break;
		}
		*pp = (*pp)->next;
	}

	return MMU_STATUS_OK;
}

static enum MMUStatus SMRemoveKernelNode(enum MMURegion e,
					 unsigned int buf_bus_address,
					 unsigned int process_id)
{
	struct MMUNode *p, **pp;

	p = g_mmu->region[e].simple_map_head->next;
	pp = &p;

	while (*pp) {
		if ((*pp)->buf_bus_address == buf_bus_address &&
		    (*pp)->process_id == process_id) {
			SMDeleteNode(pp);
			break;
		}
		*pp = (*pp)->next;
	}

	return MMU_STATUS_OK;
}

static enum MMUStatus RemoveKernelNode(enum MMURegion e,
				       unsigned int buf_bus_address,
				       unsigned int process_id)
{
	struct MMUNode *p, **pp;

	p = g_mmu->region[e].map_head->next;
	pp = &p;

	while (*pp) {
		if ((*pp)->buf_bus_address == buf_bus_address &&
		    (*pp)->process_id == process_id) {
			InsertNode(e, pp, 1);
			break;
		}
		*pp = (*pp)->next;
	}

	return MMU_STATUS_OK;
}

static enum MMUStatus Delay(unsigned int delay)
{
	if (delay > 0) {
		msleep(delay);
	}

	return MMU_STATUS_OK;
}

static enum MMUStatus CreateMutex(void **mtx)
{
	enum MMUStatus status;

	/* Allocate the mutex structure. */
	status = AllocateMemory(sizeof(pthread_mutex_t), mtx);
	if (MMU_IS_SUCCESS(status)) {
		/* Initialize the mutex. */
		**(pthread_mutex_t **)mtx = PTHREAD_MUTEX_INITIALIZER;
	}

	return status;
}

static enum MMUStatus DeleteMutex(void *mtx)
{
	/* Destroy the mutex. */
	mutex_destroy((pthread_mutex_t *)mtx);

	/* Free the mutex structure. */
	FreeMemory(mtx);

	return MMU_STATUS_OK;
}

static enum MMUStatus AcquireMutex(void *mtx, unsigned int timeout)
{
	if (timeout == MMU_INFINITE) {
		/* Lock the mutex. */
		down_interruptible(mtx);
		/* Success. */
		return MMU_STATUS_OK;
	}

	for (;;) {
		/* Try to acquire the mutex. */
		if (mutex_trylock(mtx)) {
			/* Success. */
			return MMU_STATUS_OK;
		}

		if (timeout-- == 0)
			break;

		/* Wait for 1 millisecond. */
		Delay(1);
	}

	return MMU_STATUS_OK;
}

static enum MMUStatus ReleaseMutex(void *mtx)
{
	/* Release the mutex. */
	up(mtx);
	return MMU_STATUS_OK;
}

static inline enum MMUStatus QueryProcessPageTable(void *logical,
						   unsigned long long *address)
{// cppcheck-suppress uselessAssignmentPtrArg
	logical = address;

	return MMU_STATUS_OK;
}

static inline int GetProcessID(void)
{
	return getpid();
}

static enum MMUStatus GetPhysicalAddress(void *logical,
					 unsigned long long *address)
{
	enum MMUStatus status;

	status = QueryProcessPageTable(logical, address);

	return status;
}

static enum MMUStatus GetPageEntry(struct MMUNode *node,
				   unsigned int **page_table_entry,
				   unsigned int i)
{
	int num =
		node->mtlb_start * MMU_STLB_4K_ENTRY_NUM + node->stlb_start + i;
	*page_table_entry = (unsigned int *)g_mmu->stlb_virtual + num;
	return MMU_STATUS_OK;
}

static enum MMUStatus SetupDynamicSpace(void)
{
	int i;
	enum MMUStatus status;
	unsigned int stlb_entry;
	void *pointer;
	unsigned long long address;
	unsigned int num_entries = MMU_MTLB_ENTRY_NUM;
	unsigned int *mtlb_virtual = (unsigned int *)g_mmu->mtlb_virtual;

	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	address = g_mmu->stlb_physical;

	for (i = 0; i < num_entries; i++) {
		stlb_entry = address
			     /* 4KB page size */
			     | (0 << 2)
			     /* Ignore exception */
			     | (0 << 1)
			     /* Present */
			     | (1 << 0);
		WritePageEntry(mtlb_virtual++, stlb_entry);
		address += MMU_STLB_4K_SIZE;
	}
	ReleaseMutex(g_mmu->page_table_mutex);

	/* Initial map info. */
	if (simple_map)
		SMCreateNodes();
	else
		CreateNode();
	(void)status;

	return MMU_STATUS_OK;
}

enum MMUStatus MMUInit(volatile unsigned char *hwregs)
{
	enum MMUStatus status;
	unsigned int i;
	void *pointer;

	if (mmu_init == MMU_TRUE) {
		/* All mmu use common table and dev, just initial once*/
		printf(" *****MMU Already Initialed*****\n");
		return MMU_STATUS_OK;
	}

	if (!hwregs ||
	    (ioread32((void *)(hwregs + MMU_REG_HW_ID)) >> 16) != 0x4D4D)
		return MMU_STATUS_NOT_FOUND;

	printf(" *****MMU Init*****\n");

	/* Allocate memory for the MMU object. */
	MMU_ON_ERROR(AllocateMemory(sizeof(struct MMU), &pointer));
	ZeroMemory(pointer, sizeof(struct MMU));

	g_mmu = pointer;

	g_mmu->page_table_mutex = NULL;

	/* Create the page table mutex. */
	MMU_ON_ERROR(CreateMutex(&g_mmu->page_table_mutex));

	for (i = 0; i < MMU_REGION_COUNT; i++)
		MMU_ON_ERROR(CreateMutex(&g_mmu->region[i].node_mutex));

	mmu_init = MMU_TRUE;
	return MMU_STATUS_OK;

onerror:
	printf(" *****MMU Init Error*****\n");
	return status;
}

enum MMUStatus MMURelease(void *filp, volatile unsigned char *hwregs)
{
	int i, j;
	struct MMUNode *p, *tmp;
	unsigned long long address;
	unsigned int *page_table_entry;

	if (!hwregs ||
	    (ioread32((void *)(hwregs + MMU_REG_HW_ID)) >> 16) != 0x4D4D)
		return MMU_STATUS_FALSE;

	/* if mmu or TLB not enabled, return */
	if (simple_map) {
		if (!g_mmu || !g_mmu->region[0].simple_map_head)
			return MMU_STATUS_OK;
	} else {
		if (!g_mmu || !g_mmu->region[0].map_head)
			return MMU_STATUS_OK;
	}

	printf("*****MMU Release*****\n");

	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);

	if (simple_map) {
		for (i = 0; i < MMU_REGION_COUNT; i++) {
			p = g_mmu->region[i].simple_map_head->next;

			while (p) {
				tmp = p->next;
				{
					for (j = 0; j < p->page_count; j++) {
						GetPageEntry(p,
							     &page_table_entry,
							     j);
						address = 0;
						WritePageEntry(page_table_entry,
							       address);
					}

					SMRemoveNode(i, p->buf_virtual_address,
						     p->process_id);
				}
				p = tmp;
			}
		}
	} else {
		for (i = 0; i < MMU_REGION_COUNT; i++) {
			p = g_mmu->region[i].map_head->next;

			while (p) {
				tmp = p->next;
				{
					for (j = 0; j < p->page_count; j++) {
						GetPageEntry(p,
							     &page_table_entry,
							     j);
						address = 0;
						WritePageEntry(page_table_entry,
							       address);
					}

					RemoveNode(i, p->buf_virtual_address,
						   p->process_id);
				}
				p = tmp;
			}
		}
	}
	ReleaseMutex(g_mmu->page_table_mutex);

	return MMU_STATUS_OK;
}

enum MMUStatus MMUCleanup(volatile unsigned char *hwregs[MAX_SUBSYS_NUM][2])
{
	int i;
	struct MMUNode *p, *tmp;
	struct MMUNode *fp;

	for (i = 0; i < MAX_SUBSYS_NUM; i++) {
		if (hwregs[i][0] &&
		    (ioread32((void *)(hwregs[i][0] + MMU_REG_HW_ID)) >> 16) !=
			    0x4D4D)
			return MMU_STATUS_FALSE;
		if (hwregs[i][1] &&
		    (ioread32((void *)(hwregs[i][1] + MMU_REG_HW_ID)) >> 16) !=
			    0x4D4D)
			return MMU_STATUS_FALSE;
	}

	printf("*****MMU cleanup*****\n");
	{
		if (g_mmu->stlb_virtual)
			free(g_mmu->stlb_virtual); //linux use the dma to get the continue size buffer(virsual address).
		if (g_mmu->mtlb_virtual)
			free(g_mmu->mtlb_virtual); //linux use the dma to get the continue size buffer(virsual address).
		if (g_mmu->page_table_array)
			free(g_mmu->page_table_array); //linux use the dma to get the continue size buffer(virsual address).
	}
	DeleteMutex(g_mmu->page_table_mutex);

	for (i = 0; i < MMU_REGION_COUNT; i++) {
		DeleteMutex(g_mmu->region[i].node_mutex);
		if (simple_map) {
			p = g_mmu->region[i].simple_map_head;
			while (p) {
				tmp = p->next;
				FreeMemory(p);
				p = tmp;
				MMUDEBUG(" *****clean node*****\n");
			}
		} else {
			fp = g_mmu->region[i].free_map_head;
			p = g_mmu->region[i].map_head;
			while (fp) {
				tmp = fp->next;
				FreeMemory(fp);
				fp = tmp;
				MMUDEBUG(" *****clean free node*****\n");
			}

			while (p) {
				tmp = p->next;
				FreeMemory(p);
				p = tmp;
				MMUDEBUG(" *****clean node*****\n");
			}
		}
	}
	FreeMemory(g_mmu);

	for (i = 0; i < MAX_SUBSYS_NUM; i++) {
		if (hwregs[i][0])
			iowrite32(0, (void *)(hwregs[i][0] + MMU_REG_CONTROL));
		if (hwregs[i][1])
			iowrite32(0, (void *)(hwregs[i][1] + MMU_REG_CONTROL));
	}
	mmu_enable = 0;
	mmu_init = 0;

	return MMU_STATUS_OK;
}

/*
 *-----------------------------------------------------------------------------
 *  Function name: MMUEnable
 *	Description:
 *	    Create TLB, set registers and enable MMU

    For pcie, TLB buffers come from FPGA memory and The distribution is as follows
	   MTLB:              start from: 0x00100000, size: 4K bits
	   page table array:              0x00200000        64 bits
	   STLB:                          0x00300000        4M bits
 ------------------------------------------------------------------------------*/
enum MMUStatus MMUEnable(volatile unsigned char *hwregs[MAX_SUBSYS_NUM][2])
{
	enum MMUStatus status;
	unsigned int address;
	unsigned int mutex = MMU_FALSE;
	u32 i = 0;
	u32 address_ext;

	if (mmu_enable == MMU_TRUE) {
		printf("*****MMU Already Enabled*****\n");
		return MMU_STATUS_OK;
	}

	printf("*****MMU Enable...*****\n");

	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	mutex = MMU_TRUE;
	{
		/* Allocate the 4K mode MTLB table. */
		g_mmu->mtlb_size = MMU_MTLB_SIZE;
		g_mmu->mtlb_virtual = malloc(g_mmu->mtlb_size);
		MMUDEBUG(" *****g_mmu->mtlb_virtual = 0x%p\n",
			 g_mmu->mtlb_virtual);
		if (!g_mmu->mtlb_virtual) {
			printf("hantrodec alloc buffer fail\n");
			status = MMU_STATUS_FALSE;
			goto onerror;
		}
		g_mmu->stlb_size = MMU_MTLB_ENTRY_NUM * MMU_STLB_4K_SIZE;
		g_mmu->stlb_virtual = malloc(g_mmu->stlb_size);
		MMUDEBUG(" *****g_mmu->stlb_virtual = 0x%p\n",
			 g_mmu->stlb_virtual);
		if (!g_mmu->stlb_virtual) {
			printf("hantrodec alloc buffer fail\n");
			status = MMU_STATUS_FALSE;
			goto onerror;
		}

		g_mmu->page_table_array_size = PAGE_TABLE_ENTRY_SIZE;
		g_mmu->page_table_array = malloc(g_mmu->page_table_array_size);
		MMUDEBUG(" *****g_mmu->page_table_array = 0x%p\n",
			 g_mmu->page_table_array);
		if (!g_mmu->page_table_array) {
			printf("hantrodec alloc buffer fail\n");
			status = MMU_STATUS_FALSE;
			goto onerror;
		}
	}

	MMUDEBUG(" Page table array[0]: lsb = 0x%08x\n",
		 ((int *)g_mmu->page_table_array)[0]);
	MMUDEBUG("                      msb = 0x%08x\n",
		 ((int *)g_mmu->page_table_array)[1]);

	ZeroMemory(g_mmu->mtlb_virtual, g_mmu->mtlb_size);
	ZeroMemory(g_mmu->stlb_virtual, g_mmu->stlb_size);

	ReleaseMutex(g_mmu->page_table_mutex);

	MMU_ON_ERROR(SetupDynamicSpace());
	address = g_mmu->page_table_array;
	address_ext =
		((u32)((unsigned long long)g_mmu->page_table_array >> 32)) &
		0xff;
#ifndef HANTROVCMD_ENABLE_IP_SUPPORT
	/* set regs of all MMUs */
	for (i = 0; i < MAX_SUBSYS_NUM; i++) {
		if (hwregs[i][0]) {
			MMUDEBUG(
				"hwregs[%u][0]=%p, id=0x%08x", i, hwregs[i][0],
				ioread32((void *)hwregs[i][0] + MMU_REG_HW_ID));
			iowrite32(address,
				  (void *)(hwregs[i][0] + MMU_REG_ADDRESS));
			iowrite32(address_ext,
				  (void *)(hwregs[i][0] + MMU_REG_ADDRESS_MSB));

			iowrite32(0x10000, (void *)(hwregs[i][0] +
						    MMU_REG_PAGE_TABLE_ID));
			iowrite32(0x00000, (void *)(hwregs[i][0] +
						    MMU_REG_PAGE_TABLE_ID));

			iowrite32(1, (void *)(hwregs[i][0] + MMU_REG_CONTROL));
		}
		if (hwregs[i][1]) {
			MMUDEBUG(
				"hwregs[%u][1]=%p, id=0x%08x", i, hwregs[i][1],
				ioread32((void *)hwregs[i][1] + MMU_REG_HW_ID));
			iowrite32(address,
				  (void *)(hwregs[i][1] + MMU_REG_ADDRESS));
			iowrite32(address_ext,
				  (void *)(hwregs[i][1] + MMU_REG_ADDRESS_MSB));

			iowrite32(0x10000, (void *)(hwregs[i][1] +
						    MMU_REG_PAGE_TABLE_ID));
			iowrite32(0x00000, (void *)(hwregs[i][1] +
						    MMU_REG_PAGE_TABLE_ID));

			iowrite32(1, (void *)(hwregs[i][1] + MMU_REG_CONTROL));
		}
	}
#endif
	mmu_enable = MMU_TRUE;
	return MMU_STATUS_OK;

onerror:
	if (mutex)
		ReleaseMutex(g_mmu->page_table_mutex);
	MMUDEBUG(" *****MMU Enable Error*****\n");
	return status;
}

/*
 *------------------------------------------------------------------------------
 *  Function name: MMUFlush
 *	Description:
 *	    Flush MMU reg to update cache in MMU.
 *------------------------------------------------------------------------------
 */
static enum MMUStatus
MMUFlush(u32 core_id, volatile unsigned char *hwregs[MAX_SUBSYS_NUM][2])
{
	enum MMUStatus status;
	unsigned int mutex = MMU_FALSE;

	MMUDEBUG(" *****MMU Flush*****\n");
	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	mutex = MMU_TRUE;

	if (hwregs[core_id][0]) {
		iowrite32(0x10, (void *)(hwregs[core_id][0] + MMU_REG_FLUSH));
		iowrite32(0x00, (void *)(hwregs[core_id][0] + MMU_REG_FLUSH));
	} else {
		printf("hantrodec alloc buffer fail\n");
		status = MMU_STATUS_FALSE;
		goto onerror;
	}
	if (hwregs[core_id][1]) {
		iowrite32(0x10, (void *)(hwregs[core_id][1] + MMU_REG_FLUSH));
		iowrite32(0x00, (void *)(hwregs[core_id][1] + MMU_REG_FLUSH));
	}

	ReleaseMutex(g_mmu->page_table_mutex);
	return MMU_STATUS_OK;

onerror:
	if (mutex)
		ReleaseMutex(g_mmu->page_table_mutex);
	MMUDEBUG(" *****MMU Flush Error*****\n");
	return status;
}

static enum MMUStatus MMUMemNodeMap(struct addr_desc *addr)
{
	enum MMUStatus status;
	unsigned int page_count = 0;
	unsigned int i = 0;
	struct MMUNode *p;
	unsigned long long address = 0x0;
	unsigned int *page_table_entry;
	enum MMURegion e;
	unsigned int mutex = MMU_FALSE;
	u32 ext_addr;
	u32 page_entry_value = 0;

	MMUDEBUG(" *****MMU Map*****\n");
	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	mutex = MMU_TRUE;

	page_count = (addr->size - 1) / PAGE_SIZE + 1;

	GetPhysicalAddress(addr->virtual_address, &address);
	MMUDEBUG(" *****MMU map address*****%llx\n", address);
	if (address >= REGION_IN_START && address + addr->size < REGION_IN_END)
		e = MMU_REGION_IN;
	else if (address >= REGION_OUT_START &&
		 address + addr->size < REGION_OUT_END)
		e = MMU_REGION_OUT;
	else if (address >= REGION_PRIVATE_START &&
		 address + addr->size < REGION_PRIVATE_END)
		e = MMU_REGION_PRIVATE;
	else
		e = MMU_REGION_PUB;

	if (simple_map) {
		MMU_ON_ERROR(SMCheckAddress(e, addr->virtual_address));

		SMCreateNode(e, &p, page_count);
		MMUDEBUG(" *****Node map size*****%u\n", page_count);

		p->buf_virtual_address = addr->virtual_address;
		p->process_id = GetProcessID();

		p->mtlb_start = ((address + map_shift) >> MMU_MTLB_SHIFT);
		p->stlb_start =
			((address + map_shift) >> MMU_STLB_4K_SHIFT) & 0x3FF;
		p->mtlb_end =
			(page_count + p->stlb_start) / MMU_STLB_4K_ENTRY_NUM +
			p->mtlb_start;
		p->stlb_end =
			(page_count + p->stlb_start) % MMU_STLB_4K_ENTRY_NUM;
		p->page_count = page_count;

		for (i = 0; i < page_count; i++) {
			GetPhysicalAddress(addr->virtual_address +
						   i * PAGE_SIZE,
					   &address);
			GetPageEntry(p, &page_table_entry, i);
			ext_addr = ((u32)(address >> 32)) & 0xff;
			page_entry_value =
				(address & 0xFFFFF000)
				/* ext address , physical address bits [39,32]*/
				| (ext_addr << 4)
				/* writable */
				| (1 << 2)
				/* Ignore exception */
				| (0 << 1)
				/* Present */
				| (1 << 0);
			WritePageEntry(page_table_entry, page_entry_value);
		}

		/* Purpose of Bare_metal mode: input bus address==mmu address*/
		addr->bus_address = p->mtlb_start << MMU_MTLB_SHIFT |
				    p->stlb_start << MMU_STLB_4K_SHIFT;
	} else {
		MMU_ON_ERROR(FindFreeNode(e, &p, page_count));

		SplitFreeNode(e, &p, page_count);
		MMUDEBUG(" *****Node map size*****%u\n", p->page_count);

		p->buf_virtual_address = addr->virtual_address;
		p->process_id = GetProcessID();

		for (i = 0; i < page_count; i++) {
			GetPhysicalAddress(addr->virtual_address +
						   i * PAGE_SIZE,
					   &address);
			GetPageEntry(p, &page_table_entry, i);
			ext_addr = ((u32)(address >> 32)) & 0xff;
			page_entry_value =
				(address & 0xFFFFF000)
				/* ext address , physical address bits [39,32]*/
				| (ext_addr << 4)
				/* writable */
				| (1 << 2)
				/* Ignore exception */
				| (0 << 1)
				/* Present */
				| (1 << 0);
			WritePageEntry(page_table_entry, page_entry_value);
		}
		addr->bus_address = p->mtlb_start << MMU_MTLB_SHIFT |
				    p->stlb_start << MMU_STLB_4K_SHIFT;
	}
	MMUDEBUG(" MMU_MTLB_SHIFT %d MMU_STLB_4K_SHIFT %d\n", MMU_MTLB_SHIFT,
		 MMU_STLB_4K_SHIFT);
	MMUDEBUG(
		" %s map total %u pages in region %d\nMTLB/STLB starts %d/%d, MTLB/STLB ends %d/%d\n",
		__func__, page_count, e, p->mtlb_start, p->stlb_start,
		p->mtlb_end, p->stlb_end);
	MMUDEBUG(" %s map %p -> 0x%llx\n", __func__, addr->virtual_address,
		 addr->bus_address);

	ReleaseMutex(g_mmu->page_table_mutex);

	return MMU_STATUS_OK;

onerror:
	if (mutex)
		ReleaseMutex(g_mmu->page_table_mutex);
	MMUDEBUG(" *****MMU Map Error*****\n");
	return status;
}

static enum MMUStatus MMUMemNodeUnmap(struct addr_desc *addr)
{
	unsigned int i;
	unsigned long long address = 0x0;
	unsigned int *page_table_entry;
	int process_id = GetProcessID();
	enum MMURegion e = MMU_REGION_COUNT;
	enum MMUStatus status = MMU_STATUS_OUT_OF_MEMORY;
	struct MMUNode *p;
	unsigned int mutex = MMU_FALSE;

	MMUDEBUG(" *****MMU Unmap*****\n");
	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	mutex = MMU_TRUE;

	GetPhysicalAddress(addr->virtual_address, &address);
	if (address >= REGION_IN_START && address < REGION_IN_END)
		e = MMU_REGION_IN;
	else if (address >= REGION_OUT_START && address < REGION_OUT_END)
		e = MMU_REGION_OUT;
	else if (address >= REGION_PRIVATE_START &&
		 address < REGION_PRIVATE_END)
		e = MMU_REGION_PRIVATE;
	else
		e = MMU_REGION_PUB;

	if (simple_map)
		p = g_mmu->region[e].simple_map_head->next;
	else
		p = g_mmu->region[e].map_head->next;
	/* Reset STLB of the node */
	while (p) {
		if (p->buf_virtual_address == addr->virtual_address &&
		    p->process_id == process_id) {
			for (i = 0; i < p->page_count; i++) {
				GetPageEntry(p, &page_table_entry, i);
				address = 0;
				WritePageEntry(page_table_entry, address);
			}
			break;
		}
		p = p->next;
	}
	if (!p)
		goto onerror;

	if (simple_map)
		SMRemoveNode(e, addr->virtual_address, process_id);
	else
		RemoveNode(e, addr->virtual_address, process_id);

	ReleaseMutex(g_mmu->page_table_mutex);
	return MMU_STATUS_OK;

onerror:
	if (mutex)
		ReleaseMutex(g_mmu->page_table_mutex);
	MMUDEBUG(" *****MMU Unmap Error*****\n");
	return status;
}

enum MMUStatus MMUKernelMemNodeMap(struct kernel_addr_desc *addr)
{
	enum MMUStatus status;
	unsigned int page_count = 0;
	unsigned int i = 0;
	struct MMUNode *p;
	unsigned long long address = 0x0;
	unsigned int *page_table_entry;
	enum MMURegion e;
	unsigned int mutex = MMU_FALSE;
	u32 ext_addr;
	u32 page_entry_value = 0;

	MMUDEBUG(" *****MMU Map*****\n");
	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	mutex = MMU_TRUE;

	page_count = (addr->size - 1) / PAGE_SIZE + 1;

	address = addr->bus_address;
	MMUDEBUG(" *****MMU map address*****%llx\n", address);
	if (address >= REGION_IN_START && address + addr->size < REGION_IN_END)
		e = MMU_REGION_IN;
	else if (address >= REGION_OUT_START &&
		 address + addr->size < REGION_OUT_END)
		e = MMU_REGION_OUT;
	else if (address >= REGION_PRIVATE_START &&
		 address + addr->size < REGION_PRIVATE_END)
		e = MMU_REGION_PRIVATE;
	else
		e = MMU_REGION_PUB;

	if (simple_map) {
		//TODO: should check bus addr
		//MMU_ON_ERROR(SMCheckAddress(e, addr->virtual_address));

		SMCreateNode(e, &p, page_count);
		MMUDEBUG(" *****Node map size*****%u\n", page_count);

		p->buf_bus_address = addr->bus_address;
		p->process_id = GetProcessID();

		p->mtlb_start = ((address + map_shift) >> MMU_MTLB_SHIFT);
		p->stlb_start =
			((address + map_shift) >> MMU_STLB_4K_SHIFT) & 0x3FF;
		p->mtlb_end =
			(page_count + p->stlb_start) / MMU_STLB_4K_ENTRY_NUM +
			p->mtlb_start;
		p->stlb_end =
			(page_count + p->stlb_start) % MMU_STLB_4K_ENTRY_NUM;
		p->page_count = page_count;

		for (i = 0; i < page_count; i++) {
			/* this function used in kernel only, so we think it's a
			 * contunuous buffer
			 */
			address += (i ? PAGE_SIZE : 0);
			GetPageEntry(p, &page_table_entry, i);
			ext_addr = ((u32)(address >> 32)) & 0xff;
			page_entry_value =
				(address & 0xFFFFF000)
				/* ext address , physical address bits [39,32]*/
				| (ext_addr << 4)
				/* writable */
				| (1 << 2)
				/* Ignore exception */
				| (0 << 1)
				/* Present */
				| (1 << 0);
			WritePageEntry(page_table_entry, page_entry_value);
		}

		/* Purpose of Bare_metal mode: input bus address==mmu address*/
		addr->mmu_bus_address = p->mtlb_start << MMU_MTLB_SHIFT |
					p->stlb_start << MMU_STLB_4K_SHIFT;
	} else {
		MMU_ON_ERROR(FindFreeNode(e, &p, page_count));

		SplitFreeNode(e, &p, page_count);
		MMUDEBUG(" *****Node map size*****%u\n", p->page_count);

		p->buf_bus_address = addr->bus_address;
		p->process_id = GetProcessID();

		for (i = 0; i < page_count; i++) {
			/* this function used in kernel only, so we think it's a
			 * contunuous buffer
			 */
			address += (i ? PAGE_SIZE : 0);
			GetPageEntry(p, &page_table_entry, i);
			ext_addr = ((u32)(address >> 32)) & 0xff;
			page_entry_value =
				(address & 0xFFFFF000)
				/* ext address , physical address bits [39,32]*/
				| (ext_addr << 4)
				/* writable */
				| (1 << 2)
				/* Ignore exception */
				| (0 << 1)
				/* Present */
				| (1 << 0);
			WritePageEntry(page_table_entry, page_entry_value);
		}
		addr->mmu_bus_address = p->mtlb_start << MMU_MTLB_SHIFT |
					p->stlb_start << MMU_STLB_4K_SHIFT;
	}
	MMUDEBUG(" KERNEL MMU_MTLB_SHIFT %d MMU_STLB_4K_SHIFT %d\n",
		 MMU_MTLB_SHIFT, MMU_STLB_4K_SHIFT);
	MMUDEBUG(
		" %s map total %u pages in region %d\nMTLB/STLB starts %d/%d, MTLB/STLB ends %d/%d\n",
		__func__, page_count, e, p->mtlb_start, p->stlb_start,
		p->mtlb_end, p->stlb_end);
	MMUDEBUG(" %s map 0x%lx -> 0x%llx\n", __func__, addr->bus_address,
		 addr->mmu_bus_address);

	ReleaseMutex(g_mmu->page_table_mutex);

	return MMU_STATUS_OK;

onerror:
	if (mutex)
		ReleaseMutex(g_mmu->page_table_mutex);
	MMUDEBUG(" *****MMU Map Error*****\n");
	return status;
}

enum MMUStatus MMUKernelMemNodeUnmap(struct kernel_addr_desc *addr)
{
	unsigned int i;
	unsigned long long address = 0x0;
	unsigned int *page_table_entry;
	int process_id = GetProcessID();
	enum MMURegion e = MMU_REGION_COUNT;
	enum MMUStatus status = MMU_STATUS_OUT_OF_MEMORY;
	struct MMUNode *p;
	unsigned int mutex = MMU_FALSE;

	MMUDEBUG(" *****MMU Unmap*****\n");
	AcquireMutex(g_mmu->page_table_mutex, MMU_INFINITE);
	mutex = MMU_TRUE;

	address = addr->bus_address;
	if (address >= REGION_IN_START && address < REGION_IN_END)
		e = MMU_REGION_IN;
	else if (address >= REGION_OUT_START && address < REGION_OUT_END)
		e = MMU_REGION_OUT;
	else if (address >= REGION_PRIVATE_START &&
		 address < REGION_PRIVATE_END)
		e = MMU_REGION_PRIVATE;
	else
		e = MMU_REGION_PUB;

	if (simple_map)
		p = g_mmu->region[e].simple_map_head->next;
	else
		p = g_mmu->region[e].map_head->next;
	/* Reset STLB of the node */
	while (p) {
		if (p->buf_bus_address == addr->bus_address &&
		    p->process_id == process_id) {
			for (i = 0; i < p->page_count; i++) {
				GetPageEntry(p, &page_table_entry, i);
				address = 0;
				WritePageEntry(page_table_entry, address);
			}
			break;
		}
		p = p->next;
	}
	if (!p)
		goto onerror;

	if (simple_map)
		SMRemoveKernelNode(e, addr->bus_address, process_id);
	else
		RemoveKernelNode(e, addr->bus_address, process_id);

	ReleaseMutex(g_mmu->page_table_mutex);
	return MMU_STATUS_OK;

onerror:
	if (mutex)
		ReleaseMutex(g_mmu->page_table_mutex);
	MMUDEBUG(" *****MMU Unmap Error*****\n");
	return status;
}

static long MMUCtlBufferMap(unsigned long arg)
{
	struct addr_desc addr;
	long tmp;

	tmp = copy_from_user(&addr, (void *)arg, sizeof(struct addr_desc));
	if (tmp) {
		MMUDEBUG("copy_from_user failed, returned %li\n", tmp);
		return -MMU_EFAULT;
	}

	MMUMemNodeMap(&addr);

	tmp = copy_to_user((void *)arg, &addr, sizeof(struct addr_desc));
	if (tmp) {
		MMUDEBUG("copy_to_user failed, returned %li\n", tmp);
		return -MMU_EFAULT;
	}
	return 0;
}

static long MMUCtlBufferUnmap(unsigned long arg)
{
	struct addr_desc addr;
	long tmp;

	tmp = copy_from_user(&addr, (void *)arg, sizeof(struct addr_desc));
	if (tmp) {
		MMUDEBUG("copy_from_user failed, returned %li\n", tmp);
		return -MMU_EFAULT;
	}

	MMUMemNodeUnmap(&addr);
	return 0;
}

//Just maintain for the ioctl HANTRO_IOCS_MMU_ENABLE
#if 0
static long MMUCtlEnable(unsigned long arg,
			 volatile unsigned char *hwregs[HXDEC_MAX_CORES][2])
{
	unsigned int enable;
	long tmp;

	tmp = copy_from_user(&enable, (void *)arg, sizeof(unsigned int));
	if (tmp) {
		MMUDEBUG("copy_from_user failed, returned %li\n", tmp);
		return -MMU_EFAULT;
	}

	MMUEnable(hwregs);

	return 0;
}
#endif

static long MMUCtlFlush(unsigned long arg,
			volatile unsigned char *hwregs[HXDEC_MAX_CORES][2])
{
	unsigned int core_id;
	long tmp;

	tmp = copy_from_user(&core_id, (void *)arg, sizeof(unsigned int));
	if (tmp) {
		MMUDEBUG("copy_from_user failed, returned %li\n", tmp);
		return -MMU_EFAULT;
	}

	MMUFlush(core_id, hwregs);

	return 0;
}

long MMUIoctl(unsigned int cmd, void *filp, unsigned long arg,
	      volatile unsigned char *hwregs[HXDEC_MAX_CORES][2])
{
	u32 i = 0;

	for (i = 0; i < MAX_SUBSYS_NUM; i++) {
		if (hwregs[i][0] &&
		    (ioread32((void *)(hwregs[i][0] + MMU_REG_HW_ID)) >> 16) !=
			    0x4D4D)
			return -MMU_ENOTTY;

		if (hwregs[i][1] &&
		    (ioread32((void *)(hwregs[i][1] + MMU_REG_HW_ID)) >> 16) !=
			    0x4D4D)
			return -MMU_ENOTTY;
		MMUDEBUG("mmu_hwregs[%u][0].mmu_hwregs[0]=%p", i, hwregs[i][0]);
		MMUDEBUG("mmu_hwregs[%u][1].mmu_hwregs[0]=%p", i, hwregs[i][1]);
	}

	switch (cmd) {
	case HANTRO_IOCS_MMU_MEM_MAP: {
		return MMUCtlBufferMap(arg);
	}
	case HANTRO_IOCS_MMU_MEM_UNMAP: {
		return MMUCtlBufferUnmap(arg);
	}
	/* Just maintain not delete temparary
	case HANTRO_IOCS_MMU_ENABLE: {
	return (MMUCtlEnable(arg, hwregs));
	}
	*/
	case HANTRO_IOCS_MMU_FLUSH: {
		return MMUCtlFlush(arg, hwregs);
	}
	default:
		return -MMU_ENOTTY;
	}
}

unsigned long long GetMMUAddress(void)
{
	unsigned long long address = 0;

	if (pcie)
		address = PAGE_PCIE_START_ADDRESS;
	else
		address = g_mmu->page_table_array_physical;

	return address;
}

