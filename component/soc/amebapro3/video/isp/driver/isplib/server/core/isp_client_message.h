#ifndef _ISP_CLIENT_MESSAGE_H_INC_
#define _ISP_CLIENT_MESSAGE_H_INC_

#include "isp_msg_buf.h"

int isp_client_message_process(struct isp_msg_buf *msg);
int isp_client_message_simple(uint32_t isp_id, uint32_t mod_id, uint32_t action,
							  void *data, uint32_t len);
int isp_client_message_raw(uint32_t isp_id, uint32_t mod_id, uint32_t action,
						   void *data, uint32_t len, uint32_t capacity);

#endif /* _ISP_CLIENT_MESSAGE_H_INC_ */
