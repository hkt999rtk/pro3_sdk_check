#ifndef _ISP_MSG_BUF_H_INC_
#define _ISP_MSG_BUF_H_INC_

#include <isp_os_io.h>
#include <isp_msg_buf_common.h>

int isp_msg_to_resp(struct isp_msg_buf *msg, int retval, uint32_t msg_len);

#endif /* _ISP_MSG_BUF_H_INC_ */
