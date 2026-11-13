#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/cache.h>
#include <rts_camera_isp.h>
#include "rtscam_isp_iomem.h"
#include "rtscam_isp_statis.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static struct rtscam_isp_statis_node *
__rtscam_isp_statis_get_node(struct rtscam_isp_statis *statis,
							 struct list_head *list, uint32_t buf_id)

{
	struct rtscam_isp_statis_node *node;

	if (!statis || !list) {
		return NULL;
	}

	list_for_each_entry(node, list, list) {
		if (buf_id == node->data.buf_id) {
			return node;
		}
	}
	return NULL;
}

static int __rtscam_isp_statis_irq_read(struct rtscam_isp_statis *statis,
										struct rtscam_isp_statis_node *node)
{
	int i;
	struct rtscam_isp_statis_data *data = &node->data;

	data->frame_count = isp_rtos_atomic_inc_return(&statis->count);

	data->awb_reg.fine_r_sum =
		rtscam_isp_iomem_read(statis->iomem, AWB_FINE_SUM_R);
	data->awb_reg.fine_g_sum =
		rtscam_isp_iomem_read(statis->iomem, AWB_FINE_SUM_G);
	data->awb_reg.fine_b_sum =
		rtscam_isp_iomem_read(statis->iomem, AWB_FINE_SUM_B);
	data->awb_reg.fine_white_pixels =
		rtscam_isp_iomem_read(statis->iomem, AWB_FINE_WP_NUM);
	for (i = 0; i < ARRAY_SIZE(data->awb_reg.illum_white_pixels); i++)
		data->awb_reg.illum_white_pixels[i] = rtscam_isp_iomem_read(
				statis->iomem, AWB_WP_NUM + i * 4);

	if (rtscam_isp_iomem_read(statis->iomem, SYS_INT_FLAG1) &
		FLICK_STATIS_INT) {
		rtscam_isp_iomem_write(statis->iomem, FLICK_STATIS_INT,
							   SYS_INT_FLAG1);
		data->flick_reg.fft_sum2_9 = rtscam_isp_iomem_read(
										 statis->iomem, FLICK_FFT_RESUT_SUM2_9);
		data->flick_reg.fft_sum2_127 = rtscam_isp_iomem_read(
										   statis->iomem, FLICK_FFT_RESUT_SUM2_127);
		data->flick_reg.valid = true;
	} else {
		data->flick_reg.valid = false;
	}

	if (rtscam_isp_iomem_read(statis->iomem, SYS_INT_FLAG1) &
		FLICK_S_STATIS_INT) {
		rtscam_isp_iomem_write(statis->iomem, FLICK_S_STATIS_INT,
							   SYS_INT_FLAG1);
		data->flick_s_reg.fft_sum2_9 = rtscam_isp_iomem_read(
										   statis->iomem, FLICK_S_FFT_RESUT_SUM2_9);
		data->flick_s_reg.fft_sum2_127 = rtscam_isp_iomem_read(
											 statis->iomem, FLICK_S_FFT_RESUT_SUM2_127);
		data->flick_s_reg.valid = true;
	} else {
		data->flick_s_reg.valid = false;
	}

	return 0;
}

static void
rtscam_isp_statis_set_node_locked(struct rtscam_isp_statis *statis,
								  struct rtscam_isp_statis_node *node)
{
	if (!node && !list_empty(&statis->idle)) {
		node = list_first_entry(&statis->idle,
								struct rtscam_isp_statis_node, list);
		list_del(&node->list);
	}
	if (!node) {
		return;
	}

	list_add(&node->list, &statis->busy);
	rtscam_isp_iomem_write(statis->iomem, node->addr, STATIS_DDR_ADDR);
	rtscam_isp_iomem_write(statis->iomem, STATIS_STREAM_EN, STATIS_CTRL);
}

int rtscam_isp_statis_init(struct rtscam_isp_statis *statis,
						   struct rtscam_isp_iomem *iomem,
						   const struct rtscam_isp_statis_info *info)
{
	int i;

	if (!statis || !iomem || !info || !info->num || !info->size ||
		info->num > NODE_MAX_NUM) {
		return -EINVAL;
	}

	isp_rtos_atomic_set(&statis->valid, 0);

	INIT_LIST_HEAD(&statis->idle);
	INIT_LIST_HEAD(&statis->busy);
	INIT_LIST_HEAD(&statis->done);
	INIT_LIST_HEAD(&statis->user);
	statis->iomem = iomem;

	for (i = 0; i < info->num; i++) {
		struct rtscam_isp_statis_node *node = &statis->node[i];

		INIT_LIST_HEAD(&node->list);
		node->data.buf_id = i;
		node->addr = info->phy_addr + i * info->size;
		node->size = info->size;
		list_add_tail(&node->list, &statis->idle);
	}

	rtscam_isp_iomem_write(statis->iomem, info->size, STATIS_DDR_LEN);
	rtscam_isp_statis_set_node_locked(statis, NULL);

	isp_rtos_atomic_set(&statis->count, 0);
	return isp_rtos_spin_lock_init(&statis->lock);
}

int rtscam_isp_statis_cleanup(struct rtscam_isp_statis *statis)
{
	isp_rtos_atomic_set(&statis->valid, 0);
	statis->iomem = NULL;

	return 0;
}

int rtscam_isp_statis_valid(struct rtscam_isp_statis *statis)
{
	return isp_rtos_atomic_read(&statis->valid);
}

int rtscam_isp_statis_get(struct rtscam_isp_statis *statis,
						  struct rtscam_isp_statis_data *data)
{
	unsigned long flags = 0;
	struct rtscam_isp_statis_node *node;

	isp_rtos_spin_lock_irqsave(&statis->lock, flags);
	if (list_empty(&statis->done)) {
		isp_rtos_spin_unlock_irqrestore(&statis->lock, flags);
		return -EAGAIN;
	}
	node = list_first_entry(&statis->done, struct rtscam_isp_statis_node,
							list);
	list_move(&node->list, &statis->user);
	if (list_empty(&statis->done)) {
		isp_rtos_atomic_set(&statis->valid, 0);
	}
	isp_rtos_spin_unlock_irqrestore(&statis->lock, flags);

	dcache_invalidate_by_addr(node->addr, node->size);

	*data = node->data;

	return 0;
}

int rtscam_isp_statis_put(struct rtscam_isp_statis *statis, uint32_t id)
{
	unsigned long flags = 0;
	struct rtscam_isp_statis_node *node;

	if (!statis) {
		return -EINVAL;
	}

	isp_rtos_spin_lock_irqsave(&statis->lock, flags);
	node = __rtscam_isp_statis_get_node(statis, &statis->user, id);
	if (!node) {
		printf("invalid put statis buf 0x%lx\n", id);
		isp_rtos_spin_unlock_irqrestore(&statis->lock, flags);
		return -ENOENT;
	}
	list_del(&node->list);
	isp_rtos_spin_unlock_irqrestore(&statis->lock, flags);

	dcache_invalidate_by_addr(node->addr, node->size);

	isp_rtos_spin_lock_irqsave(&statis->lock, flags);
	if (list_empty(&statis->busy)) {
		rtscam_isp_statis_set_node_locked(statis, node);
	} else {
		list_add_tail(&node->list, &statis->idle);
	}
	isp_rtos_spin_unlock_irqrestore(&statis->lock, flags);

	return 0;
}

uint32_t rtscam_isp_statis_get_count(struct rtscam_isp_statis *statis)
{
	return isp_rtos_atomic_read(&statis->count);
}

void rtscam_isp_statis_irq(struct rtscam_isp_statis *statis)
{
	struct rtscam_isp_statis_node *node;

	if (rtscam_isp_iomem_read(statis->iomem, STATIS_FRAME_STATE)) {
		printf("isp statis stream error occurred\n");
		rtscam_isp_iomem_write(statis->iomem, 1, STATIS_FRAME_STATE);
		rtscam_isp_iomem_write(statis->iomem, STATIS_STREAM_EN,
							   STATIS_CTRL);
		return;
	}
	rtscam_isp_iomem_write(statis->iomem, STATIS_ALL_INT, SYS_INT_FLAG1);
	rtscam_isp_iomem_write(statis->iomem, ISP_OUT_LINE_CNT_INT,
						   SYS_INT_FLAG0);

	isp_rtos_spin_lock(&statis->lock);

	while (!list_empty(&statis->done)) {
		list_move(statis->done.next, &statis->idle);
	}

	configASSERT(!list_empty(&statis->busy));

	node = list_first_entry(&statis->busy, struct rtscam_isp_statis_node,
							list);
	list_move(&node->list, &statis->done);
	__rtscam_isp_statis_irq_read(statis, node);

	rtscam_isp_statis_set_node_locked(statis, NULL);

	isp_rtos_spin_unlock(&statis->lock);

	isp_rtos_atomic_set(&statis->valid, 1);
}
