#include "cache.h"
#include "ameba_soc.h"

#ifndef DCACHE_LINE_SIZE
#define DCACHE_LINE_SIZE    32U
#endif


/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */

void icache_enable(void)
{
	ICache_Enable();
}

/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
void icache_disable(void)
{
	ICache_Disable();
}

/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
void icache_invalidate(void)
{
	ICache_Invalidate();
}

/**
  \brief   I-Cache Invalidate by address
  \details Invalidates I-Cache for the given address.
           I-Cache is invalidated starting from a 32 byte aligned address in 32 byte granularity.
           I-Cache memory blocks which are part of given address + given size are invalidated.
  \param[in]   addr    address
  \param[in]   isize   size of memory block (in number of bytes)
*/
void icache_invalidate_by_addr(void *addr, int32_t isize)
{
	if (isize > 0) {
		ICache_Invalidate();
	}
}

/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  */
void dcache_enable(void)
{
	DCache_Enable();
}

/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  */
void dcache_disable(void)
{
	DCache_Disable();
}

/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  */
void dcache_invalidate(void)
{
	DCache_Invalidate(0xFFFFFFFF, 0xFFFFFFFF);
}

/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  */
void dcache_clean(void)
{
	DCache_Clean(0xFFFFFFFF, 0xFFFFFFFF);
}

/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  */
void dcache_clean_invalidate(void)
{
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
}

/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D_Cache for the given address
           D-Cache is invalidated starting from a 32 byte aligned address in 32 byte granularity.
           D-Cache memory blocks which are part of given address + given size are invalidated.
  \param[in]   addr    address
  \param[in]   dsize   size of memory block (in number of bytes)
*/
// FIXME: addr may not aligned to cache line
void dcache_invalidate_by_addr(uint32_t addr, size_t dsize)
{
	int32_t op_size;
	uint32_t op_addr;

	if (dsize > 0) {
		op_size = dsize + (addr & (DCACHE_LINE_SIZE - 1U));
		op_addr = addr;

		DCache_Invalidate(op_addr, op_size);
	}
}

/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
           D-Cache is cleaned starting from a 32 byte aligned address in 32 byte granularity.
           D-Cache memory blocks which are part of given address + given size are cleaned.
  \param[in]   addr    address
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void dcache_clean_by_addr(uint32_t addr, size_t dsize)
{
	int32_t op_size;
	uint32_t op_addr;

	if (dsize > 0) {
		op_size = dsize + (addr & (DCACHE_LINE_SIZE - 1U));
		op_addr = addr;

		DCache_Clean(op_addr, op_size);
	}
}

/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
           D-Cache is cleaned and invalidated starting from a 32 byte aligned address in 32 byte granularity.
           D-Cache memory blocks which are part of given address + given size are cleaned and invalidated.
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
// FIXME: addr may not aligned to cache line
void dcache_clean_invalidate_by_addr(uint32_t addr, size_t dsize)
{
	int32_t op_size;
	uint32_t op_addr;

	if (dsize > 0) {
		op_size = dsize + (addr & (DCACHE_LINE_SIZE - 1U));
		op_addr = addr;

		DCache_CleanInvalidate(op_addr, op_size);
	}
}
