#include <errno.h>
#include <string.h>
#include <rts_camera_isp.h>
#include "rtscam_isp_mcu.h"
#include "rtscam_isp_sync.h"
#include "rtscam_isp_pipeline.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

extern void rtscam_isp_mcu_irq_wake_thread(unsigned int irq);

struct rtscam_isp_sync_item {
	struct list_head list;
	struct rtscam_isp_sync_reg reg;
};

static inline int rtscam_isp_sync_max_delay(struct rtscam_isp_sync *sync)
{
	return ARRAY_SIZE(sync->delay) - 1;
}

static inline int rtscam_isp_sync_index(struct rtscam_isp_sync *sync, int delay)
{
	return (sync->index + delay) % ARRAY_SIZE(sync->delay);
}

static inline struct list_head *
rtscam_isp_sync_get_head(struct rtscam_isp_sync *sync, int delay, int irq)
{
	return &sync->delay[rtscam_isp_sync_index(sync, delay)].irq[irq - 1];
}

int rtscam_isp_sync_init(struct rtscam_isp_sync *sync,
						 const struct rtscam_isp_sync_init_info *info)
{
	int i;
	int j;
	int ret;

	if (!sync || !info || !info->iomem) {
		return -EINVAL;
	}

	configASSERT(ARRAY_SIZE(sync->delay[0].irq) == _MAX_RTSCAM_ISP_INT - 1);

	memset(sync, 0, sizeof(*sync));

	sync->i2c_regs = pvPortMalloc(sizeof(*sync->i2c_regs));
	if (!sync->i2c_regs) {
		return -ENOMEM;
	}
	memset(sync->i2c_regs, 0, sizeof(*sync->i2c_regs));

	ret = isp_rtos_mutex_init(&sync->lock);
	if (ret) {
		return ret;
	}
	isp_rtos_atomic_set(&sync->irq, RTSCAM_ISP_INT_NONE);
	for (i = 0; i < ARRAY_SIZE(sync->delay); i++)
		for (j = 0; j < ARRAY_SIZE(sync->delay->irq); j++) {
			INIT_LIST_HEAD(&sync->delay[i].irq[j]);
		}
	INIT_LIST_HEAD(&sync->idle);
	sync->idle_num = 0;

	sync->iomem = info->iomem;
	sync->irq_no = info->irq_no;

	return 0;
}

static int rtscam_isp_sync_clean_list(struct list_head *head)
{
	struct rtscam_isp_sync_item *item;
	struct rtscam_isp_sync_item *next;

	list_for_each_entry_safe(item, next, head, list) {
		list_del(&item->list);
		vPortFree(item);
	}
	return 0;
}

void rtscam_isp_sync_cleanup(struct rtscam_isp_sync *sync)
{
	rtscam_isp_sync_clear_regs(sync);
	vPortFree(sync->i2c_regs);
}

static struct rtscam_isp_sync_item *
__rtscam_isp_sync_get_idle(struct rtscam_isp_sync *sync)
{
	struct rtscam_isp_sync_item *item;

	if (!sync) {
		return NULL;
	}

	if (sync->idle_num) {
		item = list_last_entry(&sync->idle, struct rtscam_isp_sync_item,
							   list);
		list_del_init(&item->list);
		sync->idle_num--;
	} else {
		item = pvPortMalloc(sizeof(*item));
		if (item) {
			memset(item, 0, sizeof(*item));
			INIT_LIST_HEAD(&item->list);
		}
	}

	return item;
}

static int __rtscam_isp_sync_put_idle(struct rtscam_isp_sync *sync,
									  struct rtscam_isp_sync_item *item)
{
	if (!sync || !item) {
		return -EINVAL;
	}

	list_add(&item->list, &sync->idle);
	sync->idle_num++;
	if (sync->idle_num <= 32) {
		return 0;
	}
	while (sync->idle_num > 16) {
		item = list_last_entry(&sync->idle, struct rtscam_isp_sync_item,
							   list);
		list_del(&item->list);
		vPortFree(item);
		sync->idle_num--;
	}
	return 0;
}

static int
rtscam_isp_sync_try_or_set_regs(struct rtscam_isp_sync *sync, int cur_int,
								const struct rtscam_isp_sync_regs *regs,
								int set)
{
	int i;
	int delay;
	int interrupt;

	if (!sync || !regs || regs->split_index > regs->num) {
		return -EINVAL;
	}

	for (i = 0; i < regs->num; i++) {
		int start_delay;
		int start_interrupt;
		struct rtscam_isp_sync_reg *reg = &regs->reg[i];

		if (reg->type < 0 || reg->type >= _MAX_RTSCAM_ISP_SYNC_TYPE) {
			return -EINVAL;
		}

		if (i == 0) {
			if (reg->type == RTSCAM_ISP_SYNC_TYPE_INFO) {
				delay = 0;
				interrupt = cur_int;
			} else {
				delay = cur_int > RTSCAM_ISP_INT_DATA_START;
				interrupt = RTSCAM_ISP_INT_DATA_START;
			}
		} else {
			if (i == 1) {
				start_delay = delay;
				start_interrupt = interrupt;
			}
			if (i == regs->split_index) {
				delay = start_delay;
				interrupt = start_interrupt;
			}
		}

		if (reg->type == RTSCAM_ISP_SYNC_TYPE_INFO) {
			delay += reg->info.delay_frames;
			if (reg->info.delay_frames == 0 &&
				reg->info.interrupt <= interrupt) {
				delay++;
			}
			interrupt = reg->info.interrupt;

			if (delay > rtscam_isp_sync_max_delay(sync)) {
				return -EINVAL;
			}
			if (reg->info.interrupt <= RTSCAM_ISP_INT_NONE ||
				reg->info.interrupt >= _MAX_RTSCAM_ISP_INT) {
				return -EINVAL;
			}
		} else if (set) {
			struct list_head *head;
			struct rtscam_isp_sync_item *item;

			item = __rtscam_isp_sync_get_idle(sync);
			if (!item) {
				return -ENOMEM;
			}
			item->reg = regs->reg[i];
			head = rtscam_isp_sync_get_head(sync, delay, interrupt);
			list_add_tail(&item->list, head);
		}
	}

	return 0;
}

int rtscam_isp_sync_add_regs(struct rtscam_isp_sync *sync,
							 struct rtscam_isp_sync_regs *regs)
{
	int ret;
	int cur_int;
	int miss_data_start = 0;

	if (!sync || !regs) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&sync->lock);
	cur_int = isp_rtos_atomic_read(&sync->irq);
	if (rtscam_isp_iomem_read(sync->iomem, SYS_INT_FLAG0) &
		ISP_OUT_LINE_CNT_INT) {
		cur_int++;
		miss_data_start = 1;
	}
	ret = rtscam_isp_sync_try_or_set_regs(sync, cur_int, regs, false);
	if (ret) {
		goto out;
	}
	ret = rtscam_isp_sync_try_or_set_regs(sync, cur_int, regs, true);
	if (!ret && !miss_data_start) {
		rtscam_isp_mcu_irq_wake_thread(sync->irq_no);
	}
out:
	isp_rtos_mutex_unlock(&sync->lock);
	return ret;
}

int rtscam_isp_sync_clear_regs(struct rtscam_isp_sync *sync)
{
	int i;
	int j;

	if (!sync) {
		return -EINVAL;
	}

	isp_rtos_mutex_lock(&sync->lock);
	isp_rtos_atomic_set(&sync->irq, RTSCAM_ISP_INT_NONE);
	for (i = 0; i < ARRAY_SIZE(sync->delay); i++)
		for (j = 0; j < ARRAY_SIZE(sync->delay->irq); j++) {
			rtscam_isp_sync_clean_list(&sync->delay[i].irq[j]);
		}
	sync->index = 0;
	rtscam_isp_sync_clean_list(&sync->idle);
	sync->idle_num = 0;
	isp_rtos_mutex_unlock(&sync->lock);

	return 0;
}

int rtscam_isp_sync_lock(struct rtscam_isp_sync *sync, int lock)
{
	if (!sync) {
		return -EINVAL;
	}

	if (lock) {
		isp_rtos_mutex_lock(&sync->lock);
	} else {
		isp_rtos_mutex_unlock(&sync->lock);
	}

	return 0;
}

void rtscam_isp_sync_set_irq(struct rtscam_isp_sync *sync, int irq)
{
	if (!sync) {
		return;
	}
	isp_rtos_atomic_set(&sync->irq, irq);
}

static void rtscam_isp_sync_request_write_i2c(struct rtscam_isp_sync *sync)
{
	if (!sync->i2c_regs->num) {
		return;
	}

	rtscam_isp_request(RTSCAM_ISPC_PIPELINE_0,
					   RTSISP_IOC_PIPE_I2C_SYNC_WRITE, sync->i2c_regs,
					   sizeof(*sync->i2c_regs), true);
	sync->i2c_regs->num = 0;
}

static void rtscam_isp_write_sync_i2c(struct rtscam_isp_sync *sync,
									  struct rtscam_isp_i2c_reg_mask *i2c_mask)
{
	sync->i2c_regs->regs[sync->i2c_regs->num++] = *i2c_mask;

	if (sync->i2c_regs->num == ARRAY_SIZE(sync->i2c_regs->regs)) {
		rtscam_isp_sync_request_write_i2c(sync);
	}
}

static void rtscam_isp_write_sync_reg(struct rtscam_isp_sync *sync,
									  struct rtscam_isp_reg_mask *reg)
{
	uint32_t data;

	if (reg->mask) {
		data = rtscam_isp_iomem_read(sync->iomem, reg->addr);
		data &= ~reg->mask;
		data |= (reg->data & reg->mask);
	} else {
		data = reg->data;
	}
	rtscam_isp_iomem_write(sync->iomem, data, reg->addr);
}

static void __rtscam_isp_irq_sync_write_one(struct rtscam_isp_sync *sync,
		struct rtscam_isp_sync_item *item)
{
	if (item->reg.type == RTSCAM_ISP_SYNC_TYPE_REG) {
		rtscam_isp_write_sync_reg(sync, &item->reg.reg);
	} else if (item->reg.type == RTSCAM_ISP_SYNC_TYPE_I2C) {
		rtscam_isp_write_sync_i2c(sync, &item->reg.i2c);
	}
}

int rtscam_isp_sync_irq_thread(struct rtscam_isp_sync *sync)
{
	int32_t irq; /* enum rtscam_isp_interrupt */
	struct rtscam_isp_sync_item *item;
	struct rtscam_isp_sync_item *next;

	isp_rtos_mutex_lock(&sync->lock);
	for (irq = RTSCAM_ISP_INT_NONE + 1;
		 irq <= isp_rtos_atomic_read(&sync->irq); irq++) {
		struct list_head *head;

		head = &sync->delay[sync->index].irq[irq - 1];
		list_for_each_entry_safe(item, next, head, list) {
			list_del(&item->list);
			__rtscam_isp_irq_sync_write_one(sync, item);
			__rtscam_isp_sync_put_idle(sync, item);
		}
		rtscam_isp_sync_request_write_i2c(sync);
		if (irq == _MAX_RTSCAM_ISP_INT - 1) {
			sync->index = rtscam_isp_sync_index(sync, 1);
			isp_rtos_atomic_set(&sync->irq, RTSCAM_ISP_INT_NONE);
		}
	}
	isp_rtos_mutex_unlock(&sync->lock);
	return 0;
}
