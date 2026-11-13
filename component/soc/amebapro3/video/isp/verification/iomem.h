#ifndef _IOMEM_H_
#define _IOMEM_H_

#include <stdint.h>

#define dsb(option) __asm__ __volatile__("dsb " #option : : : "memory")
#define rmb() dsb()
#define wmb() dsb(st)

#ifndef barrier
#define barrier() __asm__ __volatile__("" : : : "memory")
#endif

static inline uint32_t readl(uint32_t addr)
{
	uint32_t val;

	barrier();
	val = *(volatile uint32_t *)(addr);
	/* prevent prefetching of coherent DMA data ahead of a dma-complete */
	rmb();
	return val;
}

static inline void writel(uint32_t value, uint32_t addr)
{
	/* flush writes to coherent DMA data before possibly triggering a DMA read */
	wmb();
	*(volatile uint32_t *)(addr) = value;
}

static inline void writel_mask(uint32_t value, uint32_t addr, uint32_t mask)
{
	uint32_t temp;

	temp = readl(addr);
	writel((temp & (~mask)) | (value & mask), addr);
}
#endif /* _IOMEM_H_ */
