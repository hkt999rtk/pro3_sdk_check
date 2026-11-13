#ifndef _ISP_STATIS_H_INC_
#define _ISP_STATIS_H_INC_

#include <isp_os_driver.h>
#include <isp_statis_common.h>

struct isp_statis {
	uint32_t isp_id;
	const void *virt_addr;
	uint32_t size;
	uint32_t num;
	int buf_id;

	struct rts_isp_statis *all;
	int *refcnt;
};

int isp_statis_init(struct isp_statis *statis, uint32_t isp_id,
					const struct isp_statis_buf_offset *offset);
int isp_statis_cleanup(struct isp_statis *statis);
int isp_statis_update(struct isp_statis *statis,
					  const struct rts_isp_statis_data *data);
int isp_statis_get_all(struct isp_statis *statis,
					   const struct rts_isp_statis **all);
void isp_statis_ref(struct isp_statis *statis,
					const struct rts_isp_statis *all);
void isp_statis_unref(struct isp_statis *statis,
					  const struct rts_isp_statis *all);

#endif /* _ISP_STATIS_H_INC_ */
