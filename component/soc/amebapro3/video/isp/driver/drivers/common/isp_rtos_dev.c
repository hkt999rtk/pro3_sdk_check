#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "isp_rtos_dev_inner.h"

struct isp_rtos_cdev {
	int fd;
	char name[32];
	uint32_t events_watched;
	uint32_t events_polled;
	isp_rtos_wait_queue_head_t *wq;
	void *event_data;
	bool fd_open;
	struct isp_rtos_device *dev;
};

enum isp_os_fd_list {
	ISP_FD_STDIN,
	ISP_FD_STDOUT,
	ISP_FD_STDERR,
	ISP_FD_POLLFD,

	_ISP_FD_DEVFD_START = 4,
	ISP_FD_DEVFD_PIPELINE_0 = _ISP_FD_DEVFD_START,
	// Add new devfd from here, please remember modify _ISP_FD_DEVFD_END
	_ISP_FD_DEVFD_END = ISP_FD_DEVFD_PIPELINE_0,

	_ISP_FD_EVENTFD_START = 16,
	ISP_FD_EVENTFD_0 = _ISP_FD_EVENTFD_START,
	ISP_FD_EVENTFD_1,
	_ISP_FD_EVENTFD_END = ISP_FD_EVENTFD_1,
};

#define DEV_FD_CNT (_ISP_FD_DEVFD_END - _ISP_FD_DEVFD_START + 1)
#define EVENT_FD_CNT (_ISP_FD_EVENTFD_END - _ISP_FD_EVENTFD_START + 1)
#define CDEV_CNT (DEV_FD_CNT + EVENT_FD_CNT)
#define CDEV_EVENTFD0_INDEX (_ISP_FD_DEVFD_END - _ISP_FD_DEVFD_START + 1)

static struct isp_rtos_cdev cdev[CDEV_CNT];
static TaskHandle_t epoll_task_handle;

static inline int isp_rtos_is_registered(struct isp_rtos_device *rdev)
{
	if (!rdev) {
		return 0;
	}
	return !!rdev->flags;
}

struct isp_rtos_device *isp_rtos_device_alloc(void)
{
	struct isp_rtos_device *ptr = NULL;

	ptr = pvPortMalloc(sizeof(struct isp_rtos_device));
	if (!ptr) {
		return NULL;
	}
	memset(ptr, 0, sizeof(struct isp_rtos_device));
	ptr->filp = pvPortMalloc(sizeof(struct isp_rtos_file));
	if (!ptr->filp) {
		vPortFree(ptr);
		return NULL;
	}
	memset(ptr->filp, 0, sizeof(struct isp_rtos_file));
	return ptr;
}

void isp_rtos_device_release(struct isp_rtos_device *rdev)
{
	if (rdev) {
		vPortFree(rdev);
	}
	rdev = NULL;
}

void *isp_rtos_dev_get_drvdata(const struct isp_rtos_device *rdev)
{
	if (!rdev) {
		return NULL;
	}
	return rdev->driver_data;
}

void isp_rtos_dev_set_drvdata(struct isp_rtos_device *rdev, void *data)
{
	if (!rdev) {
		return;
	}
	rdev->driver_data = data;
}

struct isp_rtos_device *isp_rtos_devdata(struct isp_rtos_file *filp)
{
	if (!filp) {
		return NULL;
	}
	if (filp->node < 0 || filp->node >= CDEV_CNT) {
		return NULL;
	}
	return cdev[filp->node].dev;
}

static inline int is_devfd_index(int i)
{
	return i < CDEV_EVENTFD0_INDEX;
}

int isp_rtos_register_device(struct isp_rtos_device *rdev)
{
	int i;

	if (!rdev) {
		return -EINVAL;
	}
	if (isp_rtos_is_registered(rdev)) {
		return -EEXIST;
	}

	for (i = 0; is_devfd_index(i); i++) {
		if (!strcmp(rdev->name, cdev[i].name)) {
			rdev->flags = 1;
			rdev->filp->node = i;
			cdev[i].dev = rdev;
			break;
		}
	}

	return 0;
}

void isp_rtos_unregister_device(struct isp_rtos_device *rdev)
{
	int i;

	if (!rdev || !isp_rtos_is_registered(rdev)) {
		return;
	}

	for (i = 0; is_devfd_index(i); i++) {
		if (!strcmp(rdev->name, cdev[i].name)) {
			cdev[i].dev = NULL;
			break;
		}
	}
	rdev->flags = 0;
	rdev->filp->node = -1;
	if (rdev->release) {
		rdev->release(rdev);
	}
}

int isp_rtos_device_init(void)
{
	int i;

	strcpy(cdev[0].name, "/dev/rtsisp");
	cdev[0].fd = ISP_FD_DEVFD_PIPELINE_0;

	// Add new devfd from here, please add eventfd0/1 cdev num
	strcpy(cdev[1].name, "eventfd0");
	cdev[1].fd = ISP_FD_EVENTFD_0;

	strcpy(cdev[2].name, "eventfd1");
	cdev[2].fd = ISP_FD_EVENTFD_1;

	for (i = 0; i < CDEV_CNT; i++) {
		cdev[i].event_data = NULL;
		cdev[i].dev = NULL;
		cdev[i].fd_open = 0;
	}
	return 0;
}

int isp_rtos_open(const char *file)
{
	int i;
	int ret = 0;
	struct isp_rtos_device *rdev;

	for (i = 0; is_devfd_index(i); i++) {
		if (!strcmp(file, cdev[i].name)) {
			rdev = cdev[i].dev;
			if (isp_rtos_is_registered(rdev)) {
				if (rdev->fops->open) {
					ret = rdev->fops->open(rdev->filp);
				}
				if (!ret) {
					cdev[i].fd_open = 1;
					return cdev[i].fd;
				}
			}
			break;
		}
	}
	return -ENODEV;
}

int isp_rtos_close(const int fd)
{
	int i;
	int ret = 0;
	struct isp_rtos_device *rdev;

	if (fd < 0) {
		return -EINVAL;
	}

	for (i = 0; i < CDEV_CNT; i++) {
		if (fd == cdev[i].fd) {
			break;
		}
	}
	if (i == CDEV_CNT) {
		return -ENODEV;
	}
	cdev[i].fd_open = 0;
	if (!is_devfd_index(i)) {
		return ret;
	}
	rdev = cdev[i].dev;
	if (isp_rtos_is_registered(rdev) && rdev->fops->release) {
		ret = rdev->fops->release(rdev->filp);
	}
	return ret;
}

int isp_rtos_ioctl(int fd, unsigned int request, void *arg)
{
	int i;
	int ret = 0;
	struct isp_rtos_device *rdev;

	for (i = 0; is_devfd_index(i); i++) {
		if (fd == cdev[i].fd) {
			rdev = cdev[i].dev;
			if (isp_rtos_is_registered(rdev) && rdev->fops->ioctl)
				ret = rdev->fops->ioctl(rdev->filp, request,
										arg);
			return ret;
		}
	}
	return -ENODEV;
}

int isp_rtos_epoll_create(void)
{
	epoll_task_handle = xTaskGetCurrentTaskHandle();
	return ISP_FD_POLLFD;
}

static void isp_rtos_epoll_ctl_op(int pfd, int op, int fd,
								  struct isp_rtos_epoll_event *event)
{
	struct isp_rtos_device *rdev;
	int i;
	int delete;

	for (i = 0; i < CDEV_CNT; i++) {
		if (fd == cdev[i].fd) {
			break;
		}
	}
	if (i == CDEV_CNT) {
		return;
	}
	if (op == ISP_RTOS_POLL_CTL_DEL) {
		delete = 1;
	} else {
		delete = 0;
	}
	cdev[i].events_watched = delete ? 0 : event->events;
	cdev[i].event_data = delete ? NULL : event->data;
	if (!is_devfd_index(i)) {
		return;
	}
	rdev = cdev[i].dev;
	if (!isp_rtos_is_registered(rdev)) {
		return;
	}
	if (op == ISP_RTOS_POLL_CTL_ADD) {
		cdev[i].events_polled = rdev->fops->poll(rdev->filp);
	} else if (delete)
		isp_rtos_remove_wait_queue(cdev[i].wq,
								   xTaskGetCurrentTaskHandle());
}

int isp_rtos_epoll_ctl(int pfd, int op, int fd,
					   struct isp_rtos_epoll_event *event)
{
	if (((op == ISP_RTOS_POLL_CTL_ADD || op == ISP_RTOS_POLL_CTL_MOD) &&
		 (!event || !(event->events & ISP_RTOS_EPOLL_VALID_EVENTS))) ||
		(op == ISP_RTOS_POLL_CTL_DEL && event) || pfd != ISP_FD_POLLFD) {
		return -EINVAL;
	}

	isp_rtos_epoll_ctl_op(pfd, op, fd, event);

	return 0;
}

static void isp_rtos_epoll_wait_events(struct isp_rtos_epoll_event *events,
									   int maxevents, int *nfds)
{
	struct isp_rtos_device *rdev;
	int i;

	for (i = 0; i < CDEV_CNT; i++) {
		if (is_devfd_index(i)) {
			rdev = cdev[i].dev;
			if (!isp_rtos_is_registered(rdev) ||
				!cdev[i].events_watched) {
				continue;
			}
			cdev[i].events_polled = rdev->fops->poll(rdev->filp);
		}
		if (cdev[i].events_polled & cdev[i].events_watched) {
			events[*nfds].data = cdev[i].event_data;
			events[*nfds].events = cdev[i].events_polled;
			(*nfds)++;
			if (*nfds == maxevents) {
				return;
			}
		}
	}
}

int isp_rtos_epoll_wait(int pfd, struct isp_rtos_epoll_event *events,
						int maxevents, int timeout)
{
	uint32_t events_notify;
	int nfds = 0;

	if (pfd != ISP_FD_POLLFD || !events) {
		return -EINVAL;
	}

	do {
		isp_rtos_epoll_wait_events(events, maxevents, &nfds);
		if (nfds) {
			return nfds;
		}
		events_notify =
			ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(timeout));
		if (!events_notify) {
			break;
		}
	} while (1);

	return nfds;
}

int isp_rtos_eventfd(void)
{
	int i;

	for (i = CDEV_EVENTFD0_INDEX; i < CDEV_EVENTFD0_INDEX + EVENT_FD_CNT;
		 i++) {
		if (cdev[i].fd_open) {
			continue;
		}
		cdev[i].fd_open = 1;
		cdev[i].events_polled = ISP_RTOS_EPOLLOUT;
		return cdev[i].fd;
	}
	return -EBUSY;
}

int isp_rtos_efd_read(int efd)
{
	int i;

	if (efd < 0) {
		return -EINVAL;
	}
	for (i = CDEV_EVENTFD0_INDEX; i < CDEV_EVENTFD0_INDEX + EVENT_FD_CNT;
		 i++) {
		if (efd == cdev[i].fd) {
			cdev[i].events_polled &= ~ISP_RTOS_EPOLLIN;
		}
	}
	return 0;
}

int isp_rtos_efd_write(int efd)
{
	int i;

	if (efd < 0) {
		return -EINVAL;
	}
	for (i = CDEV_EVENTFD0_INDEX; i < CDEV_EVENTFD0_INDEX + EVENT_FD_CNT;
		 i++) {
		if (efd == cdev[i].fd) {
			cdev[i].events_polled |= ISP_RTOS_EPOLLIN;
		}
	}
	xTaskNotifyGive(epoll_task_handle);
	return 0;
}

void isp_rtos_poll_wait(struct isp_rtos_file *filp,
						isp_rtos_wait_queue_head_t *wq)
{
	if (!filp || !wq) {
		return;
	}

	isp_rtos_add_wait_queue(wq, xTaskGetCurrentTaskHandle());
	cdev[filp->node].wq = wq;
}
