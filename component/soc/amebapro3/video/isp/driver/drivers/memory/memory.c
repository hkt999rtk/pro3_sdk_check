#include <errno.h>
#include <stdint.h>
#include "memory_prv.h"
#include "rts_camera_mem.h"

/*
 * ISP DMA buffer allocator — backed by the rtscam_mem pool.
 *
 * rtscam_mem_alloc() allocates from a pre-initialised physically contiguous
 * pool whose base is page-aligned (see rtscam_mem_init).  Every allocation
 * starts on a page (4 KB) boundary, which satisfies the 16-byte alignment
 * required by the ISP hardware address registers.
 *
 * On bare-metal, virtual == physical address, so *phy_addr == vm_addr.
 */
int rtscam_rtos_alloc_mem(uint32_t *phy_addr, uint32_t length, const char *info)
{
	uint8_t *vm_addr;

	DiagPrintf("allocating mem %s\n", info);


	if (!phy_addr || !length) {
		return -EINVAL;
	}

	vm_addr = rtscam_mem_alloc(length, phy_addr, RTS_MEM_ATTR_CACHE, info);
	if (!vm_addr) {
		return -ENOMEM;
	}

	DiagPrintf("success to alloc mem %s\n", info);

	return 0;
}

int rtscam_rtos_free_mem(uint32_t *phy_addr)
{
	if (!phy_addr || !*phy_addr) {
		return 0;
	}

	rtscam_mem_free((void *)(uintptr_t)*phy_addr);
	*phy_addr = 0;
	return 0;
}
