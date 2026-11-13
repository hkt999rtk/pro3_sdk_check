#ifndef _RTSCAM_ISP_IOMEM_H_INC_
#define _RTSCAM_ISP_IOMEM_H_INC_

#include <stdint.h>
#include "rtscam_isp_regs.h"

struct rtscam_isp_iomem {
	long base;
	long io_start;
	long io_size;
};

int rtscam_isp_iomem_init(struct rtscam_isp_iomem *iomem);
uint32_t rtscam_isp_iomem_read(struct rtscam_isp_iomem *iomem, uint32_t offset);
void rtscam_isp_iomem_write(struct rtscam_isp_iomem *iomem, uint32_t value,
							uint32_t offset);
uint32_t rtscam_isp_iomem_start(struct rtscam_isp_iomem *iomem);
uint32_t rtscam_isp_iomem_size(struct rtscam_isp_iomem *iomem);

#endif /* _RTSCAM_ISP_IOMEM_H_INC_ */
