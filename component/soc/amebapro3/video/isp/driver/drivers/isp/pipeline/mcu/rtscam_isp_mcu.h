#ifndef _RTSCAM_ISP_MCU_H_INC_
#define _RTSCAM_ISP_MCU_H_INC_

#include <isp_rtos_wait_queue.h>
#include <isp_rtos_completion.h>
#include "rtscam_isp_sync.h"
#include "rtscam_isp_iomem.h"
#include "rtscam_isp_statis.h"

#define RTS_ISP_DEV_NAME "/dev/rtsisp"
#define STACK_SIZE 1024

struct rtscam_isp_mcu {
	struct isp_rtos_device *gdev;
	isp_rtos_mutex_t lock;
	int id;

	int irq;
	StaticTask_t irq_task_handle;
	StackType_t irq_stack[STACK_SIZE];
	atomic_t frame_count;
	isp_rtos_completion_t data_start_completion;
	isp_rtos_completion_t frame_end_completion;

	struct rtscam_isp_sync sync;
	struct rtscam_isp_iomem iomem;
	struct rtscam_isp_statis statis;

	isp_rtos_wait_queue_head_t wq;

	/* AOV support */
	atomic_t frame_sync; /* set by frame-end ISR in AOV mode; cleared on read */
	atomic_t aov_mode;   /* non-zero = AOV active */
};

int rtscam_isp_mcu_init(void);
int rtscam_isp_mcu_cleanup(void);
void rtscam_isp_mcu_resume(void);
void rtscam_isp_mcu_suspend(void);
int rtscam_isp_mcu_get_aov_mode(void);

#endif /* _RTSCAM_ISP_mcu_H_INC_ */
