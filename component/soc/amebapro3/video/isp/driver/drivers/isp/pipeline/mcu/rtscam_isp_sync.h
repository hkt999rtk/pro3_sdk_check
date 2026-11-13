#ifndef _RTSCAM_ISP_SYNC_H_INC_
#define _RTSCAM_ISP_SYNC_H_INC_

#include <list.h>
#include <isp_rtos_atomic.h>
#include <isp_rtos_mutex.h>
#include "rtscam_isp_iomem.h"

#define MAX_SYNC_DELAY 4

struct rtscam_isp_sync_delay {
	/* no item for RTSCAM_ISP_INT_NONE */
	struct list_head irq[2];
};

struct rtscam_isp_sync {
	StaticSemaphore_t lock;
	atomic_t irq;
	struct rtscam_isp_sync_delay delay[MAX_SYNC_DELAY + 1];
	uint32_t index;
	struct list_head idle;
	uint32_t idle_num;

	struct rtscam_isp_i2c_sync_regs *i2c_regs;

	struct rtscam_isp_iomem *iomem;
	int irq_no;
};

struct rtscam_isp_sync_init_info {
	struct rtscam_isp_iomem *iomem;
	int irq_no;
};

struct rtscam_isp_sync_regs;

int rtscam_isp_sync_init(struct rtscam_isp_sync *sync,
						 const struct rtscam_isp_sync_init_info *info);
void rtscam_isp_sync_cleanup(struct rtscam_isp_sync *sync);
int rtscam_isp_sync_add_regs(struct rtscam_isp_sync *sync,
							 struct rtscam_isp_sync_regs *regs);
int rtscam_isp_sync_clear_regs(struct rtscam_isp_sync *sync);
int rtscam_isp_sync_lock(struct rtscam_isp_sync *sync, int lock);
void rtscam_isp_sync_set_irq(struct rtscam_isp_sync *sync, int irq);
int rtscam_isp_sync_irq_thread(struct rtscam_isp_sync *sync);

#endif /* _RTSCAM_ISP_SYNC_H_INC_ */
