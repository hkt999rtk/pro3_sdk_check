#ifndef _ISP_MSG_BUF_COMMON_H_INC_
#define _ISP_MSG_BUF_COMMON_H_INC_

#include <stdint.h>
#include "isp_utils_common.h"

#define ISP_MSG_SEQ_BASE 1024

/* isp msg flags */
#define ISP_MSG_TYPE_RAW (1U << 31)
#define ISP_MSG_NO_RESPONSE (1U << 30)

struct isp_msg_hdr {
	uint32_t flags;
	uint32_t msg_len;
	uint32_t capacity;
	uint32_t isp_id;
	uint32_t mod_id;
	uint32_t action;
	int32_t ret_val;
	uint32_t reserved;
};

struct isp_msg_buf {
	int refcnt;
	struct isp_msg_hdr hdr;
	void *data;
	unsigned short sequence;
};

static inline uint32_t isp_msg_data_len(const struct isp_msg_hdr *hdr)
{
	return hdr->msg_len - sizeof(*hdr);
}

static inline uint32_t isp_msg_data_capacity(const struct isp_msg_hdr *hdr)
{
	return hdr->capacity - sizeof(*hdr);
}

struct isp_msg_buf *isp_msg_new(void *data, uint32_t len, int shared);
struct isp_msg_buf *isp_msg_get(struct isp_msg_buf *msg);
void isp_msg_put(struct isp_msg_buf *msg);

#define simple_msg_size(action) \
	(sizeof(struct isp_msg_hdr) + ISP_AID_SIZE(action))
#define simple_read_msg_size(action)  \
	(sizeof(struct isp_msg_hdr) + \
	 (ISP_AID_IS_READ(action) ? ISP_AID_SIZE(action) : 0))
#define simple_write_msg_size(action) \
	(sizeof(struct isp_msg_hdr) + \
	 (ISP_AID_IS_WRITE(action) ? ISP_AID_SIZE(action) : 0))

#define get_buf_sized_ptr(ptr, size, buf, end)                \
	({                                                    \
		int ret = RTS_ISP_OK;                         \
		buf = PTR_ALIGN(buf, 4);                      \
		if ((uintptr_t)buf + size > (uintptr_t)end) { \
			ret = -RTS_ISP_ENOSPC;                \
		} else {                                      \
			ptr = buf;                            \
			buf = (void *)buf + size;             \
			buf = PTR_ALIGN(buf, 4);              \
		}                                             \
		ret;                                          \
	})

#define get_buf_ptr(ptr, buf, end) \
	get_buf_sized_ptr(ptr, sizeof(*ptr), buf, end)

#define get_buf_array_ptr(ptr, count, buf, end) \
	get_buf_sized_ptr(ptr, sizeof(*ptr) * count, buf, end)

#define copy_from_src(buf, src, size, end)                    \
	({                                                    \
		int ret = RTS_ISP_OK;                         \
		buf = PTR_ALIGN(buf, 4);                      \
		if ((uintptr_t)buf + size > (uintptr_t)end) { \
			ret = -RTS_ISP_ENOSPC;                \
		} else {                                      \
			isp_memcpy(buf, src, size);           \
			buf = (void *)buf + size;             \
			buf = PTR_ALIGN(buf, 4);              \
		}                                             \
		ret;                                          \
	})

#define copy_to_src(src, buf, size, end)                      \
	({                                                    \
		int ret = RTS_ISP_OK;                         \
		buf = PTR_ALIGN(buf, 4);                      \
		if ((uintptr_t)buf + size > (uintptr_t)end) { \
			ret = -RTS_ISP_ENOSPC;                \
		} else {                                      \
			isp_memcpy(src, buf, size);           \
			buf = (void *)buf + size;             \
			buf = PTR_ALIGN(buf, 4);              \
		}                                             \
		ret;                                          \
	})

#endif /* _ISP_MSG_BUF_COMMON_H_INC_ */
