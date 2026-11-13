#ifndef _ISP_CLIENT_H_INC_
#define _ISP_CLIENT_H_INC_

#include "isp_poll.h"
#include "isp_msg_buf.h"

int isp_client_add(isp_poll_t p);
int isp_client_service(struct isp_msg_buf *msg);

#endif /* _ISP_CLIENT_H_INC_ */
