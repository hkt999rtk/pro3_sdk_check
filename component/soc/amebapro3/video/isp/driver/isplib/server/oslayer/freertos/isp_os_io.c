/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2022 Sherry Cheng <sherry_cheng@realsil.com.cn>
 */

#include <stdio.h>
#include <string.h>
#include <rts_isp_errno.h>
#include <isp_os_io.h>
#include <isp_rtos_dev.h>
#include <isp_rtos_epoll.h>

#define do_it(do, ...)                                     \
	({                                                 \
		int ret;                                   \
		ret = do(__VA_ARGS__);                     \
		if (ret < 0)                               \
			ret = rts_isp_errno_from_os(-ret); \
		ret;                                       \
	})

isp_os_fd_t isp_os_open(const char *pathname)
{
	return do_it(isp_rtos_open, pathname);
}

int isp_os_close(isp_os_fd_t fd)
{
	return do_it(isp_rtos_close, fd);
}

isp_os_fd_t isp_os_poll_create(void)
{
	return do_it(isp_rtos_epoll_create);
}

static uint32_t os_event_to_rtos_event(uint32_t event)
{
	uint32_t e = 0;

	if (event & ISP_OS_POLL_IN) {
		e |= ISP_RTOS_EPOLLIN;
	}
	if (event & ISP_OS_POLL_OUT) {
		e |= ISP_RTOS_EPOLLOUT;
	}
	if (event & ISP_OS_POLL_ET) {
		e |= ISP_RTOS_EPOLLET;
	}
	if (event & ISP_OS_POLL_ERR) {
		e |= ISP_RTOS_EPOLLERR;
	}
	if (event & ISP_OS_POLL_HUP) {
		e |= ISP_RTOS_EPOLLHUP;
	}
	return e;
}

static uint32_t rtos_event_to_os_event(uint32_t event)
{
	uint32_t e = 0;

	if (event & ISP_RTOS_EPOLLIN) {
		e |= ISP_OS_POLL_IN;
	}
	if (event & ISP_RTOS_EPOLLOUT) {
		e |= ISP_OS_POLL_OUT;
	}
	if (event & ISP_RTOS_EPOLLET) {
		e |= ISP_OS_POLL_ET;
	}
	if (event & ISP_RTOS_EPOLLERR) {
		e |= ISP_OS_POLL_ERR;
	}
	if (event & ISP_RTOS_EPOLLHUP) {
		e |= ISP_OS_POLL_HUP;
	}
	return e;
}

int isp_os_poll_ctl(isp_os_fd_t pfd, int op, isp_os_fd_t fd,
					isp_os_poll_event_t *event)
{
	if (event) {
		struct isp_rtos_epoll_event e;

		e.events = os_event_to_rtos_event(event->events);
		e.data = event->data;
		return do_it(isp_rtos_epoll_ctl, pfd, op, fd, &e);
	} else {
		return do_it(isp_rtos_epoll_ctl, pfd, op, fd, NULL);
	}
}

int isp_os_poll_wait(isp_os_fd_t pfd, isp_os_poll_event_t *events,
					 int maxevents, int timeout)
{
	int i;
	int ret;
	struct isp_rtos_epoll_event evts[maxevents];

	ret = isp_rtos_epoll_wait(pfd, evts, maxevents, timeout);
	if (ret >= 0) {
		for (i = 0; i < ret; i++) {
			events[i].events =
				rtos_event_to_os_event(evts[i].events);
			events[i].data = evts[i].data;
		}
	}
	return ret;
}

isp_os_fd_t isp_os_eventfd(void)
{
	return do_it(isp_rtos_eventfd);
}

int isp_os_eventfd_read(isp_os_fd_t efd)
{
	return do_it(isp_rtos_efd_read, efd);
}

int isp_os_eventfd_write(isp_os_fd_t efd)
{
	return do_it(isp_rtos_efd_write, efd);
}

int isp_os_closexec(isp_os_fd_t fd, int set)
{
	/* FIXME */
	return RTS_ISP_OK;
}

int isp_os_nonblock(isp_os_fd_t fd, int set)
{
	/* FIXME */
	return RTS_ISP_OK;
}
