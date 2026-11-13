#ifndef _RTSCAM_ISP_STATIS_H_INC_
#define _RTSCAM_ISP_STATIS_H_INC_

#include <list.h>
#include <isp_rtos_atomic.h>
#include <isp_rtos_spinlock.h>
#include <rts_camera_isp.h>

#define NODE_MAX_NUM 2

struct rtscam_isp_statis_node {
	uint32_t addr;
	size_t size;
	struct rtscam_isp_statis_data data;
	struct list_head list;
};

struct rtscam_isp_statis {
	atomic_t valid;

	struct list_head idle;
	struct list_head busy;
	struct list_head done;
	struct list_head user;

	atomic_t count;

	struct rtscam_isp_iomem *iomem;
	isp_rtos_spinlock_t lock;
	struct rtscam_isp_statis_node node[NODE_MAX_NUM];
};

struct rtscam_isp_statis_info;
struct rtscam_isp_statis_data;

int rtscam_isp_statis_init(struct rtscam_isp_statis *statis,
						   struct rtscam_isp_iomem *iomem,
						   const struct rtscam_isp_statis_info *info);
int rtscam_isp_statis_cleanup(struct rtscam_isp_statis *statis);
int rtscam_isp_statis_valid(struct rtscam_isp_statis *statis);
int rtscam_isp_statis_get(struct rtscam_isp_statis *statis,
						  struct rtscam_isp_statis_data *data);
int rtscam_isp_statis_put(struct rtscam_isp_statis *statis, uint32_t id);
uint32_t rtscam_isp_statis_get_count(struct rtscam_isp_statis *statis);
void rtscam_isp_statis_irq(struct rtscam_isp_statis *statis);

#endif /* _RTSCAM_ISP_STATIS_H_INC_ */
