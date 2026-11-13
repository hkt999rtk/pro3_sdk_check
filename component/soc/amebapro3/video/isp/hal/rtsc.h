/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#ifndef _RTSC_H_
#define _RTSC_H_

#include <stdint.h>

enum {
	RTSC_BUF_COHERENT = 0,
	RTSC_BUF_FROM_DEVICE = 1,
	RTSC_BUF_TO_DEVICE = 2,
	RTSC_BUF_BIDIRECTIONAL = 3,
};

enum {
	RTSC_DMA_BUFFER_DEFAULT,
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

#endif /* _RTSC_H_ */
