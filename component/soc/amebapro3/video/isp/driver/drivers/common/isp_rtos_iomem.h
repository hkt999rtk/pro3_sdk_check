#ifndef _ISP_RTOS_IOMEM_H_
#define _ISP_RTOS_IOMEM_H_

#include <stdint.h>
#include "isp_rtos_barrier.h"

#ifndef readl
#define readl readl
static inline uint32_t readl(uint32_t addr)
{
	uint32_t val;

	barrier();
	val = *(volatile uint32_t *)(addr);
	/* prevent prefetching of coherent DMA data ahead of a dma-complete */
	rmb();
	return val;
}
#endif

#ifndef writel
#define writel writel
static inline void writel(uint32_t value, uint32_t addr)
{
	/* flush writes to coherent DMA data before possibly triggering a DMA read */
	wmb();
	*(volatile uint32_t *)(addr) = value;
}
#endif

#ifndef writel_mask
#define writel_mask writel_mask
static inline void writel_mask(uint32_t value, uint32_t addr, uint32_t mask)
{
	uint32_t temp;

	temp = readl(addr);
	writel((temp & (~mask)) | (value & mask), addr);
}
#endif

#ifndef iomem_set_bit
#define iomem_set_bit iomem_set_bit
static inline void iomem_set_bit(uint32_t bit, uint32_t addr)
{
	writel((readl(addr) | (1 << bit)), addr);
}
#endif

#ifndef iomem_clear_bit
#define iomem_clear_bit iomem_clear_bit
static inline void iomem_clear_bit(uint32_t bit, uint32_t addr)
{
	writel((readl(addr) & ~(1 << bit)), addr);
}
#endif

#endif /* _ISP_RTOS_IOMEM_H_ */
