#include "drivers/irq/irq.h"
#include <string.h>
#include <errno.h>
#include <list.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <isp_rtos_completion.h>
#include <isp_rtos_dev_inner.h>
#include <isp_rtos_barrier.h>
#include <rts_camera_isp.h>
#include "rtscam_isp_mcu.h"
#include "rtscam_isp_pipeline.h"
#include "../../rtscam_isp_input.h"
#include <irq.h>

#define __NVIC_PRIO_BITS 3U /* Number of Bits used for Priority Levels */
#define NVIC_ISER(n) (*((volatile uint32_t *)(0xE000E100 + 4 * (n))))
#define NVIC_ICER(n) (*((volatile uint32_t *)(0xE000E180 + 4 * (n))))
#define NVIC_IPR(n) (*((volatile uint32_t *)(0xE000E400 + 4 * (n))))

#ifndef __DSB
#define __DSB() __asm__ __volatile__("dsb 0xF" ::: "memory")
#endif
#ifndef __ISB
#define __ISB() __asm__ __volatile__("isb 0xF" ::: "memory")
#endif
/**
 * enum irqreturn - irqreturn type values
 * @IRQ_NONE:		interrupt was not from this device or was not handled
 * @IRQ_HANDLED:	interrupt was handled by this device
 * @IRQ_WAKE_THREAD:	handler requests to wake the handler thread
 */
enum irqreturn {
	IRQ_NONE = (0 << 0),
	IRQ_HANDLED = (1 << 0),
	IRQ_WAKE_THREAD = (1 << 1),
};

typedef enum irqreturn irqreturn_t;

struct rtscam_isp_mcu *m_isp_mcu;

static int rtscam_isp_mcu_iomem_info(struct rtscam_isp_mcu *mcu, void *arg)
{
	struct rtscam_isp_iomem_info *info = arg;

	info->start = rtscam_isp_iomem_start(&mcu->iomem);
	info->size = rtscam_isp_iomem_size(&mcu->iomem);

	return 0;
}

static int rtscam_isp_mcu_wait_event(struct rtscam_isp_mcu *mcu, void *args)
{
	int32_t event; /* enum rtscam_isp_event */
	uint32_t timeout = pdMS_TO_TICKS(500);

	event = *(int32_t *)args;
	switch (event) {
	case RTSCAM_ISP_DATA_START:
		isp_rtos_wait_for_completion_timeout(
			&mcu->data_start_completion, timeout);
		return 0;
	case RTSCAM_ISP_FRAME_END:
		isp_rtos_wait_for_completion_timeout(&mcu->frame_end_completion,
											 timeout);
		return 0;
	default:
		return -EINVAL;
	}
}

static int rtscam_isp_mcu_open(struct isp_rtos_file *filp)
{
	struct isp_rtos_device *gdev = isp_rtos_devdata(filp);
	struct rtscam_isp_mcu *mcu = isp_rtos_dev_get_drvdata(gdev);

	filp->private_data = mcu;

	return 0;
}

static int rtscam_isp_mcu_close(struct isp_rtos_file *filp)
{
	filp->private_data = NULL;

	return 0;
}

static unsigned int rtscam_isp_mcu_poll(struct isp_rtos_file *filp)
{
	unsigned int mask = 0;
	struct rtscam_isp_mcu *mcu = filp->private_data;

	isp_rtos_poll_wait(filp, &mcu->wq);
	if (rtscam_isp_need_resume(mcu->id) ||
		isp_rtos_atomic_read(&mcu->frame_sync) ||
		rtscam_isp_statis_valid(&mcu->statis)) {
		mask |= ISP_RTOS_EPOLLIN;
	}

	return mask;
}

static int rtscam_isp_frame_sync_event(struct rtscam_isp_mcu *mcu, void *args)
{
	int old = isp_rtos_atomic_read(&mcu->frame_sync);

	if (old) {
		isp_rtos_atomic_set(&mcu->frame_sync, 0);
	}
	*(uint32_t *)args = (uint32_t)old;
	return 0;
}

static int rtscam_isp_set_aov_mode(struct rtscam_isp_mcu *mcu, void *args)
{
	isp_rtos_atomic_set(&mcu->aov_mode, (int)(*(uint32_t *)args));
	return 0;
}

static int rtscam_isp_get_count(struct rtscam_isp_mcu *mcu,
								struct rtscam_isp_count *count)
{
	count->frame_count = isp_rtos_atomic_read(&mcu->frame_count);
	count->statis_count = rtscam_isp_statis_get_count(&mcu->statis);
	return 0;
}

static long rtscam_isp_mcu_ioctl(struct isp_rtos_file *filp, unsigned int cmd,
								 void *arg)
{
	int ret;
	struct rtscam_isp_mcu *mcu = filp->private_data;

	isp_rtos_mutex_lock(&mcu->lock);
	switch (cmd) {
	case RTSISP_IOC_PIPE_GET_IOMEM_INFO:
		ret = rtscam_isp_mcu_iomem_info(mcu, arg);
		break;
	case RTSISP_IOC_PIPE_WAIT_EVENT:
		ret = rtscam_isp_mcu_wait_event(mcu, arg);
		break;
	case RTSISP_IOC_PIPE_SYNC_WRITE:
		ret = rtscam_isp_sync_add_regs(&mcu->sync, arg);
		break;
	case RTSISP_IOC_PIPE_SYNC_CLEAR:
		ret = rtscam_isp_sync_clear_regs(&mcu->sync);
		break;
	case RTSISP_IOC_PIPE_SYNC_LOCK:
		ret = rtscam_isp_sync_lock(&mcu->sync, *(int32_t *)arg);
		break;
	case RTSISP_IOC_PIPE_INIT_STATIS:
		isp_rtos_atomic_set(&mcu->frame_count, 0);
		ret = rtscam_isp_statis_init(&mcu->statis, &mcu->iomem, arg);
		break;
	case RTSISP_IOC_PIPE_CLEANUP_STATIS:
		ret = rtscam_isp_statis_cleanup(&mcu->statis);
		break;
	case RTSISP_IOC_PIPE_GET_STATIS:
		ret = rtscam_isp_statis_get(&mcu->statis, arg);
		break;
	case RTSISP_IOC_PIPE_PUT_STATIS:
		ret = rtscam_isp_statis_put(&mcu->statis, *(uint32_t *)arg);
		break;
	case RTSISP_IOC_PIPE_GET_COUNT:
		ret = rtscam_isp_get_count(mcu, arg);
		break;
	case RTSISP_IOC_PIPE_FRAME_SYNC_EVENT:
		ret = rtscam_isp_frame_sync_event(mcu, arg);
		break;
	case RTSISP_IOC_PIPE_SET_AOV_MODE:
		ret = rtscam_isp_set_aov_mode(mcu, arg);
		break;
	case RTSISP_IOC_PIPE_SET_FRAME_INFO:
	case RTSISP_IOC_PIPE_NOTIFY_DYN_FPS:
	case RTSISP_IOC_PIPE_NOTIFY_COLOR_RANGE:
	case RTSISP_IOC_PIPE_IS_FPGA:
	case RTSISP_IOC_PIPE_GET_TNR_BIT:
	case RTSISP_IOC_PIPE_I2C_READ_REG:
	case RTSISP_IOC_PIPE_I2C_WRITE_REG:
	case RTSISP_IOC_PIPE_I2C_TRANSTER:
	case RTSISP_IOC_PIPE_SET_POWER:
	case RTSISP_IOC_PIPE_SEL_PIN_STATE:
	case RTSISP_IOC_PIPE_SET_CLOCK:
	case RTSISP_IOC_PIPE_GET_STATIS_CFG:
	case RTSISP_IOC_PIPE_TEST_PREVIEW:
	case RTSISP_IOC_PIPE_PM_ENABLE:
	case RTSISP_IOC_PIPE_SET_LARGE_IMAGE_MODE:
	case RTSISP_IOC_PIPE_GET_BUS_ADDR:
	case RTSISP_IOC_PIPE_NEED_RESUME:
		isp_rtos_mutex_unlock(&mcu->lock);
		ret = rtscam_isp_request(mcu->id, cmd, arg, _IOC_SIZE(cmd),
								 false);
		isp_rtos_mutex_lock(&mcu->lock);
		break;
	default:
		ret = -ENOTTY;
	}
	isp_rtos_mutex_unlock(&mcu->lock);
	return ret;
}

static struct isp_rtos_file_operations rtscam_isp_mcu_fops = {
	.open = rtscam_isp_mcu_open,
	.release = rtscam_isp_mcu_close,
	.poll = rtscam_isp_mcu_poll,
	.ioctl = rtscam_isp_mcu_ioctl,
};

static int rtscam_isp_mcu_create_device(struct rtscam_isp_mcu *mcu)
{
	struct isp_rtos_device *gdev;
	int ret;

	if (mcu->gdev) {
		return 0;
	}

	gdev = isp_rtos_device_alloc();
	if (!gdev) {
		return -ENOMEM;
	}

	strlcpy(gdev->name, RTS_ISP_DEV_NAME, sizeof(gdev->name));
	gdev->release = isp_rtos_device_release;
	gdev->fops = &rtscam_isp_mcu_fops;

	isp_rtos_dev_set_drvdata(gdev, mcu);
	ret = isp_rtos_register_device(gdev);
	if (ret) {
		isp_rtos_device_release(gdev);
		gdev = NULL;
		return ret;
	}

	ret = isp_rtos_spin_lock_init(&mcu->wq.lock);
	if (ret) {
		isp_rtos_device_release(gdev);
		gdev = NULL;
		return ret;
	}

	mcu->gdev = gdev;

	return 0;
}

static void rtscam_isp_mcu_remove_device(struct rtscam_isp_mcu *mcu)
{
	struct isp_rtos_device *gdev;

	gdev = mcu->gdev;
	if (!gdev) {
		return;
	}
	isp_rtos_unregister_device(gdev);

	mcu->gdev = NULL;
}

static void rtscam_isp_mcu_irq_thread(void)
{
	rtscam_isp_sync_irq_thread(&m_isp_mcu->sync);
}

static irqreturn_t rtscam_isp_mcu_irq(void)
{
	uint32_t int_value;
	struct rtscam_isp_mcu *mcu = m_isp_mcu;

	int_value = (rtscam_isp_iomem_read(&mcu->iomem, SYS_INT_EN0) &
				 rtscam_isp_iomem_read(&mcu->iomem, SYS_INT_FLAG0));

	if (int_value & ISP_OUT_DATA_START_INT) {
		rtscam_isp_iomem_write(&mcu->iomem, ISP_OUT_DATA_START_INT,
							   SYS_INT_FLAG0);
		rtscam_isp_sync_set_irq(&mcu->sync, RTSCAM_ISP_INT_DATA_START);
		isp_rtos_complete(&mcu->data_start_completion);
		return IRQ_WAKE_THREAD;
	}

	if (int_value & SENSOR0_FRAME_END_INT) {
		rtscam_isp_iomem_write(&mcu->iomem, SENSOR0_FRAME_END_INT,
							   SYS_INT_FLAG0);
		isp_rtos_atomic_inc(&mcu->frame_count);
		rtscam_isp_sync_set_irq(&mcu->sync, RTSCAM_ISP_INT_FRAME_END);
		isp_rtos_complete(&mcu->frame_end_completion);
		if (isp_rtos_atomic_read(&mcu->aov_mode)) {
			isp_rtos_atomic_set(&mcu->frame_sync, 1);
			isp_rtos_wake_up(&mcu->wq);
		}
		return IRQ_WAKE_THREAD;
	}

	int_value = (rtscam_isp_iomem_read(&mcu->iomem, SYS_INT_EN1) &
				 rtscam_isp_iomem_read(&mcu->iomem, SYS_INT_FLAG1));
	if (int_value & STATIS_DONE_INT) {
		rtscam_isp_iomem_write(&mcu->iomem, STATIS_DONE_INT,
							   SYS_INT_FLAG1);
		rtscam_isp_statis_irq(&mcu->statis);
		isp_rtos_wake_up(&mcu->wq);
		return IRQ_HANDLED;
	}

	int_value = (rtscam_isp_iomem_read(&mcu->iomem, VERIFY_INT_EN) &
				 rtscam_isp_iomem_read(&mcu->iomem, VERIFY_INT_FLAG));
	if (int_value) {
		rtscam_isp_iomem_write(&mcu->iomem, int_value, VERIFY_INT_FLAG);
		rtscam_isp_input_frame_done();
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static void isp_rtos_irq_task(void *pvParameters)
{
	uint32_t irq_notify;
	int timeout = -1;

	while (1) {
		irq_notify = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(timeout));

		if (!irq_notify) {
			continue;
		}
		rtscam_isp_mcu_irq_thread();
	}
}

void interrupt1_handler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	int ret;

	ret = rtscam_isp_mcu_irq();
	switch (ret) {
	case IRQ_NONE:
	case IRQ_HANDLED:
		break;
	case IRQ_WAKE_THREAD:
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(
			(TaskHandle_t)&m_isp_mcu->irq_task_handle,
			&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		break;
	default:
		break;
	}
	return;
}

void rtscam_isp_mcu_irq_wake_thread(unsigned int irq)
{
	if (irq == m_isp_mcu->irq) {
		xTaskNotifyGive((TaskHandle_t)&m_isp_mcu->irq_task_handle);
	}
}

static int rtscam_isp_mcu_init_irq(struct rtscam_isp_mcu *mcu)
{
	TaskHandle_t xHandle = NULL;

	mcu->irq = 1;
	xHandle = xTaskCreateStatic(
				  isp_rtos_irq_task, "isp_irq_task", STACK_SIZE, NULL,
				  (portPRIVILEGE_BIT | configIRQ_THREAD_TASK_PRIORITY),
				  mcu->irq_stack, &mcu->irq_task_handle);
	if (xHandle != (TaskHandle_t)&mcu->irq_task_handle) {
		return -ENOMEM;
	}
	isp_irq_register(ISP_IRQ_ISP_EVENT, interrupt1_handler, INT_PRI_HIGH);
	isp_irq_enable(ISP_IRQ_ISP_EVENT);
	return 0;
}

static int rtscam_isp_mcu_init_resource(struct rtscam_isp_mcu *mcu)
{
	int ret;

	if (!mcu) {
		return -EINVAL;
	}

	ret = rtscam_isp_iomem_init(&mcu->iomem);
	if (ret) {
		return ret;
	}
	return rtscam_isp_mcu_init_irq(mcu);
}

static void rtscam_isp_mcu_cleanup_resource(struct rtscam_isp_mcu *mcu)
{
}

static int rtscam_isp_mcu_init_status(struct rtscam_isp_mcu *mcu)
{
	int ret;
	struct rtscam_isp_sync_init_info info;

	ret = isp_rtos_mutex_init(&mcu->lock);
	if (ret) {
		return ret;
	}

	ret = isp_rtos_init_completion(&mcu->data_start_completion);
	if (ret) {
		return ret;
	}
	ret = isp_rtos_init_completion(&mcu->frame_end_completion);
	if (ret) {
		return ret;
	}

	info.iomem = &mcu->iomem;
	info.irq_no = mcu->irq;

	isp_rtos_atomic_set(&mcu->frame_sync, 0);
	isp_rtos_atomic_set(&mcu->aov_mode, 0);

	return rtscam_isp_sync_init(&mcu->sync, &info);
}

static void rtscam_isp_mcu_cleanup_status(struct rtscam_isp_mcu *mcu)
{
	rtscam_isp_sync_cleanup(&mcu->sync);
}

int rtscam_isp_mcu_init(void)
{
	int ret;
	struct rtscam_isp_mcu *mcu;

	mcu = pvPortMalloc(sizeof(struct rtscam_isp_mcu));
	if (!mcu) {
		return -ENOMEM;
	}
	memset(mcu, 0, sizeof(struct rtscam_isp_mcu));

	mcu->id = RTSCAM_ISPC_PIPELINE_0;

	rtscam_isp_mcu_create_device(mcu);
	m_isp_mcu = mcu;

	ret = rtscam_isp_mcu_init_resource(mcu);
	if (ret) {
		goto init_resource_err;
	}
	ret = rtscam_isp_mcu_init_status(mcu);
	if (ret) {
		goto init_status_err;
	}

	return 0;
init_status_err:
	rtscam_isp_mcu_cleanup_resource(mcu);
init_resource_err:
	vPortFree(mcu);
	mcu = NULL;
	m_isp_mcu = NULL;
	return ret;
}

int rtscam_isp_mcu_cleanup(void)
{
	if (!m_isp_mcu) {
		return 0;
	}
	isp_irq_disable(ISP_IRQ_ISP_EVENT);
	vTaskDelete((TaskHandle_t)&m_isp_mcu->irq_task_handle);
	rtscam_isp_mcu_remove_device(m_isp_mcu);
	rtscam_isp_mcu_cleanup_status(m_isp_mcu);
	rtscam_isp_mcu_cleanup_resource(m_isp_mcu);
	vPortFree(m_isp_mcu);
	m_isp_mcu = NULL;

	return 0;
}

void rtscam_isp_mcu_resume(void)
{
	if (!m_isp_mcu) {
		return;
	}
	isp_irq_enable(ISP_IRQ_ISP_EVENT);
	isp_rtos_wake_up(&m_isp_mcu->wq);
}

void rtscam_isp_mcu_suspend(void)
{
	if (!m_isp_mcu) {
		return;
	}
	isp_irq_disable(ISP_IRQ_ISP_EVENT);
}

int rtscam_isp_mcu_get_aov_mode(void)
{
	if (!m_isp_mcu) {
		return 0;
	}
	return isp_rtos_atomic_read(&m_isp_mcu->aov_mode);
}
