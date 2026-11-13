#ifndef _ISP_HAL_CACHE_H_
#define _ISP_HAL_CACHE_H_

#include <stdint.h>
#include <stddef.h>

/* Declarations for cache operations used by the ISP driver.
 * Implementations are provided by hal_isp.c (wrap AmebaPro3 DCache_CleanInvalidate). */
void dcache_invalidate_by_addr(uint32_t addr, size_t dsize);
void dcache_clean_by_addr(uint32_t addr, size_t dsize);
void dcache_clean_invalidate_by_addr(uint32_t addr, size_t dsize);

#endif /* _ISP_HAL_CACHE_H_ */
