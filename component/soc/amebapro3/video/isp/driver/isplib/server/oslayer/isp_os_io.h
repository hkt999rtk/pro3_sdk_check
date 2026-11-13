#ifndef _ISP_OS_IO_H_INC_
#define _ISP_OS_IO_H_INC_

#include <stdint.h>

typedef int isp_os_fd_t;

isp_os_fd_t isp_os_open(const char *pathname);
int isp_os_close(isp_os_fd_t fd);
int isp_os_closexec(isp_os_fd_t fd, int set);
int isp_os_nonblock(isp_os_fd_t fd, int set);

#define isp_close(fd)                     \
	do {                              \
		if (fd >= 0)              \
			isp_os_close(fd); \
		fd = -1;                  \
	} while (0)

#define ISP_OS_POLL_WAIT_INFINITE (-1)

enum ISP_OS_POLL_EVENTS {
	ISP_OS_POLL_IN = 0x01,
	ISP_OS_POLL_OUT = 0x02,
	ISP_OS_POLL_ET = 0x04,
	ISP_OS_POLL_ERR = 0x08,
	ISP_OS_POLL_HUP = 0x10,
};

typedef struct isp_os_poll_event {
	uint32_t events;
	void *data;
} isp_os_poll_event_t;

enum ISP_OS_POLL_OP {
	ISP_OS_POLL_CTL_ADD = 1,
	ISP_OS_POLL_CTL_DEL,
	ISP_OS_POLL_CTL_MOD,
};

isp_os_fd_t isp_os_poll_create(void);
int isp_os_poll_ctl(isp_os_fd_t pfd, int op, isp_os_fd_t fd,
					isp_os_poll_event_t *event);
int isp_os_poll_wait(isp_os_fd_t pfd, isp_os_poll_event_t *events,
					 int maxevents, int timeout);

isp_os_fd_t isp_os_eventfd(void);
int isp_os_eventfd_read(isp_os_fd_t efd);
int isp_os_eventfd_write(isp_os_fd_t efd);

#endif /* _ISP_OS_IO_H_INC_ */
