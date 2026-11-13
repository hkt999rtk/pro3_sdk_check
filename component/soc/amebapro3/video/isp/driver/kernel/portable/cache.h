#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdint.h>
#include <stddef.h>

void icache_enable(void);
void icache_disable(void);
void icache_invalidate(void);
void icache_invalidate_by_addr(void *addr, int32_t isize);
void dcache_enable(void);
void dcache_disable(void);
void dcache_invalidate(void);
void dcache_clean(void);
void dcache_clean_invalidate(void);
void dcache_invalidate_by_addr(uint32_t addr, size_t dsize);
void dcache_clean_by_addr(uint32_t addr, size_t dsize);
void dcache_clean_invalidate_by_addr(uint32_t addr, size_t dsize);

#endif
