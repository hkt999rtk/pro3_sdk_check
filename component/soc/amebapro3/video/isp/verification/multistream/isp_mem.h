#ifndef _ISP_MEM_H_INC_
#define _ISP_MEM_H_INC_

#include <stdint.h>
#include "rts_errno.h"

enum {
	RTSC_BUF_COHERENT = 0,
	RTSC_BUF_FROM_DEVICE = 1,
	RTSC_BUF_TO_DEVICE = 2,
	RTSC_BUF_BIDIRECTIONAL = 3,
};

enum {
	RTSC_DMA_BUFFER_FROM_HEAD,
	RTSC_DMA_BUFFER_FROM_END,
	RTSC_CMA_BUFFER_DEFAULT = (0x1 << 8),
};

struct rtsc_dma_buffer {
	void *vm_addr;
	uint32_t length;
	uint32_t phy_addr;
	uint32_t buf_io;
	uint32_t direction;
	char name[32];
};

int rtsc_alloc_dma(struct rtsc_dma_buffer *buffer);
int rtsc_free_dma(struct rtsc_dma_buffer *buffer);
int rtsc_mem_sync_device(unsigned long dma_addr, unsigned long size);
int rtsc_mem_sync_cpu(unsigned long dma_addr, unsigned long size);

#endif /* _ISP_MEM_H_INC_ */
