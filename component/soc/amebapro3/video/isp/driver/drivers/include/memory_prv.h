
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>

int rtscam_rtos_alloc_mem(uint32_t *phy_addr, uint32_t length,
						  const char *info);
int rtscam_rtos_free_mem(uint32_t *phy_addr);
#endif /* _MEMORY_H_ */
