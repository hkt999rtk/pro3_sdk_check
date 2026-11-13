#ifndef _ISP_RTOS_DEV_H_
#define _ISP_RTOS_DEV_H_

#include "isp_rtos_epoll.h"

int isp_rtos_open(const char *file);
int isp_rtos_close(const int fd);
int isp_rtos_ioctl(int fd, unsigned int request, void *arg);
int isp_rtos_epoll_create(void);
int isp_rtos_epoll_ctl(int pfd, int op, int fd,
					   struct isp_rtos_epoll_event *event);
int isp_rtos_epoll_wait(int pfd, struct isp_rtos_epoll_event *events,
						int maxevents, int timeout);
int isp_rtos_eventfd(void);
int isp_rtos_efd_read(int efd);
int isp_rtos_efd_write(int efd);

#endif /* _ISP_RTOS_DEV_H_ */
