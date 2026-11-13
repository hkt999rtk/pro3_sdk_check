#ifndef _ISP_RTOS_EPOLL_H_
#define _ISP_RTOS_EPOLL_H_

#define ISP_RTOS_EPOLLIN 0x00000001
#define ISP_RTOS_EPOLLOUT 0x00000004
#define ISP_RTOS_EPOLLERR 0x00000008
#define ISP_RTOS_EPOLLHUP 0x00000010
#define ISP_RTOS_EPOLLET 0x80000000
#define ISP_RTOS_EPOLL_VALID_EVENTS (ISP_RTOS_EPOLLIN | ISP_RTOS_EPOLLOUT)

enum ISP_RTOS_POLL_OP {
	ISP_RTOS_POLL_CTL_ADD = 1,
	ISP_RTOS_POLL_CTL_DEL,
	ISP_RTOS_POLL_CTL_MOD,
};

struct isp_rtos_epoll_event {
	int events; /* Epoll events */
	void *data; /* User data variable */
};

#endif /* _ISP_RTOS_EPOLL_H_ */
