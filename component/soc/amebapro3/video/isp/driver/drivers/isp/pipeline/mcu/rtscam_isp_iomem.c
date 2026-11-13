#include "rtscam_isp_iomem.h"

int rtscam_isp_iomem_init(struct rtscam_isp_iomem *iomem)
{
	iomem->base = 0x60000000;
	iomem->io_size = 0xd8000;
	iomem->io_start = 0x60000000;
	return 0;
}

uint32_t rtscam_isp_iomem_read(struct rtscam_isp_iomem *iomem, uint32_t offset)
{
	return *(volatile uint32_t *)(iomem->base + offset);
}

void rtscam_isp_iomem_write(struct rtscam_isp_iomem *iomem, uint32_t value,
							uint32_t offset)
{
	*(volatile uint32_t *)(iomem->base + offset) = value;
}

uint32_t rtscam_isp_iomem_start(struct rtscam_isp_iomem *iomem)
{
	return iomem->io_start;
}

uint32_t rtscam_isp_iomem_size(struct rtscam_isp_iomem *iomem)
{
	return iomem->io_size;
}
