/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2017 Grant Shen <grant_shen@realsil.com.cn>
 */

#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_time.h>
#include <isp_os_thread.h>
#include <isp_utils.h>
#include "isp_tracker.h"
#include "isp_poll.h"

#define ISP_DEBUG_TYPE ISP_DEBUG_POLL

struct isp_work_handle {
	isp_poll_t p;

	void *data;
	int ret;
	isp_work_cb work;
	isp_work_done_cb done;

	struct isp_list list;
};

struct isp_work_queue {
	isp_io_handle_t wq_io;

	isp_os_thread_mutex_t lock;
	isp_os_thread_cond_t cond;

	struct isp_list queue;
	int queue_handles;
	struct isp_list done;
	struct isp_list idle;
	int idle_handles;

	isp_os_thread_t threads[4];
	int idle_threads;

	int shutdown;
};

struct isp_poll {
	int efd;
	uint64_t time;

	struct avl_tree timers;
	struct isp_list pendings;
	struct isp_list watchers;

	struct isp_work_queue wq;

	isp_io_handle_t trig;

	int enable : 1;
};

static int poll_timer_insert(isp_poll_t p, isp_timer_handle_t *timer);
static int poll_wq_cleanup(isp_poll_t p);

int isp_timer_init(isp_timer_handle_t *timer, isp_poll_t p, isp_timer_cb cb,
				   void *data)
{
	if (!timer || !p || !cb) {
		return -RTS_ISP_EINVAL;
	}

	isp_memset(timer, 0, sizeof(*timer));
	timer->p = p;
	timer->cb = cb;
	timer->data = data;
	timer->avl.key = &timer->time;
	timer->started = RTS_ISP_FALSE;

	return RTS_ISP_OK;
}

void isp_timer_cleanup(isp_timer_handle_t *timer)
{
	if (!timer || !timer->p) {
		return;
	}

	isp_timer_stop(timer);
	timer->p = NULL;
	timer->time = 0;
	timer->cb = NULL;
	timer->data = NULL;
}

int isp_timer_start(isp_timer_handle_t *timer, int32_t timeout,
					int32_t interval)
{
	int ret;

	if (!timer || timeout < 0 || interval < 0) {
		return -RTS_ISP_EINVAL;
	}
	if (!timer->p) {
		return -RTS_ISP_ENOTREADY;
	}

	if (timer->started) {
		isp_timer_stop(timer);
	}
	timer->time = timer->p->time + timeout;
	timer->interval = interval;
	ret = poll_timer_insert(timer->p, timer);
	if (ret) {
		return ret;
	}
	timer->started = RTS_ISP_TRUE;
	return RTS_ISP_OK;
}

int isp_timer_stop(isp_timer_handle_t *timer)
{
	if (!timer) {
		return -RTS_ISP_EINVAL;
	}
	if (!timer->p) {
		return -RTS_ISP_ENOTREADY;
	}
	if (!timer->started) {
		return RTS_ISP_OK;
	}
	avl_delete(&timer->p->timers, &timer->avl);
	timer->started = RTS_ISP_FALSE;
	return RTS_ISP_OK;
}

static inline int io_is_polled(isp_io_handle_t *io)
{
	return !list_empty(&io->watcher);
}

static inline int io_fd_removed(isp_io_handle_t *io)
{
	return io->fd < 0;
}

static int io_remove_fd(isp_io_handle_t *io)
{
	if (io_fd_removed(io)) {
		return RTS_ISP_OK;
	}

	debug("io %p remove fd %d\n", io, isp_io_fd(io));
	if (!list_empty(&io->pending)) {
		list_remove_init(&io->pending);
	}
	if (!list_empty(&io->watcher)) {
		isp_os_poll_ctl(io->p->efd, ISP_OS_POLL_CTL_DEL, io->fd, NULL);
		list_remove_init(&io->watcher);
	}
	if (io->nonclose) {
		io->fd = -1;
	} else {
		isp_close(io->fd);
	}
	io->p = NULL;
	io->events = 0;

	return RTS_ISP_OK;
}

static void io_release(isp_io_handle_t *io)
{
	isp_assert(io->release);
	io->release(io);
}

int isp_io_init(isp_io_handle_t *io, isp_poll_t p, isp_os_fd_t fd, isp_io_cb cb)
{
	int ret;

	if (!io || !p || fd < 0 || !cb) {
		return -RTS_ISP_EINVAL;
	}

	ret = isp_os_nonblock(fd, 1);
	if (!ret) {
		ret = isp_os_closexec(fd, 1);
	}
	if (ret) {
		return ret;
	}

	isp_memset(io, 0, sizeof(*io));
	io->p = p;
	io->fd = fd;
	io->cb = cb;
	list_init(&io->pending);
	list_init(&io->watcher);
	isp_io_set_release(io, isp_io_cleanup);

	return RTS_ISP_OK;
}

void isp_io_cleanup(isp_io_handle_t *io)
{
	if (!io || !io->p) {
		return;
	}

	debug("fd: %d, event: %u cleaning up\n", io->fd, io->events);

	io_remove_fd(io);
	io->cb = NULL;
	io->release = NULL;
	io->nonclose = RTS_ISP_FALSE;

	debug("%s ok\n", __func__);
}

int isp_io_event_add(isp_io_handle_t *io, uint32_t events)
{
	events &= ISP_OS_POLL_IN | ISP_OS_POLL_OUT | ISP_OS_POLL_ET;

	if (!io || !events) {
		return -RTS_ISP_EINVAL;
	}

	if (!io->p) {
		return -RTS_ISP_ENOTREADY;
	}

	if (io_fd_removed(io)) {
		return -RTS_ISP_EPERM;
	}

	if ((io->events & events) == events) {
		return RTS_ISP_OK;
	}

	io->events |= events;
	if (list_empty(&io->pending)) {
		list_insert_tail(&io->p->pendings, &io->pending);
	}

	return RTS_ISP_OK;
}

int isp_io_event_del(isp_io_handle_t *io, uint32_t events)
{
	events &= ISP_OS_POLL_IN | ISP_OS_POLL_OUT | ISP_OS_POLL_ET;

	if (!io || !events) {
		return -RTS_ISP_EINVAL;
	}

	if (!io->p) {
		return -RTS_ISP_ENOTREADY;
	}

	if (io_fd_removed(io)) {
		return -RTS_ISP_EPERM;
	}

	if ((io->events & events) == 0) {
		return RTS_ISP_OK;
	}

	io->events &= ~events;
	if (list_empty(&io->pending)) {
		list_insert_tail(&io->p->pendings, &io->pending);
	}

	return RTS_ISP_OK;
}

int isp_io_fd(isp_io_handle_t *io)
{
	if (io && io->p) {
		return io->fd;
	}
	return -1;
}

isp_poll_t isp_io_get_poll(isp_io_handle_t *io)
{
	if (io && io->p) {
		return io->p;
	}
	return NULL;
}

int isp_io_set_release(isp_io_handle_t *io, isp_io_release_cb release)
{
	if (!io || !release) {
		return -RTS_ISP_EINVAL;
	}
	if (!io->p) {
		return -RTS_ISP_ENOTREADY;
	}
	io->release = release;
	return RTS_ISP_OK;
}

static int poll_timer_cmp(const void *k1, const void *k2, void *ptr)
{
	return *(uint64_t *)k1 - *(uint64_t *)k2;
}

static void poll_timer_again(isp_timer_handle_t *timer)
{
	if (timer->interval) {
		isp_timer_start(timer, timer->interval, timer->interval);
	}
}

static int poll_alive(isp_poll_t p)
{
	if (!p) {
		return RTS_ISP_FALSE;
	}
	if (!p->enable) {
		return RTS_ISP_FALSE;
	}
	return !list_empty(&p->watchers) || !avl_is_empty(&p->timers) ||
		   !list_empty(&p->pendings);
}

static int poll_run_timers(isp_poll_t p)
{
	int ret;
	isp_timer_handle_t *timer;
	isp_timer_handle_t *next;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	avl_for_each_element_safe(&p->timers, timer, avl, next) {
		if ((int64_t)(timer->time - p->time) > 0) {
			return RTS_ISP_OK;
		}
		time_tracker_begin("timer");
		if (isp_call_op_ret(ret, timer, cb, timer->data)) {
			isp_timer_stop(timer);
			time_tracker_end("timer");
			return ret;
		}
		if (!timer->started) {
			continue;
		}
		isp_timer_stop(timer);
		poll_timer_again(timer);
		time_tracker_end("timer");
	}

	return RTS_ISP_OK;
}

static int poll_timer_insert(isp_poll_t p, isp_timer_handle_t *timer)
{
	if (!p || !timer) {
		return -RTS_ISP_EINVAL;
	}

	avl_insert(&p->timers, &timer->avl);

	return RTS_ISP_OK;
}

static int32_t poll_get_next_timeout(isp_poll_t p)
{
	isp_timer_handle_t *timer;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	if (!list_empty(&p->pendings)) {
		return 0;
	}

	if (!avl_is_empty(&p->timers)) {
		timer = avl_first_element(&p->timers, timer, avl);
		return max((int)(timer->time - p->time), 0);
	}

	return -1;
}

static int poll_add_pending_io(isp_poll_t p)
{
	int ret;
	struct isp_list *list;
	isp_io_handle_t *io;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	while (!list_empty(&p->pendings)) {
		list = list_head(&p->pendings);
		list_remove_init(list);

		io = list_data(list, struct isp_io_handle, pending);
		isp_assert(p == io->p);

		debug("pending epoll fd: %d, events: %d\n", io->fd, io->events);
		if (io->events) {
			int operation;
			isp_os_poll_event_t e;

			e.events = io->events;
			e.data = io;
			if (io_is_polled(io)) {
				operation = ISP_OS_POLL_CTL_MOD;
			} else {
				operation = ISP_OS_POLL_CTL_ADD;
			}
			ret = isp_os_poll_ctl(p->efd, operation, io->fd, &e);
			if (ret) {
				return ret;
			}
			if (list_empty(&io->watcher)) {
				list_insert_tail(&p->watchers, &io->watcher);
			}
		} else if (io_is_polled(io)) {
			isp_os_poll_ctl(p->efd, ISP_OS_POLL_CTL_DEL, io->fd,
							NULL);
			list_remove_init(&io->watcher);
		}
	}
	return RTS_ISP_OK;
}

static int poll_wait(isp_poll_t p, int timeout)
{
	int i;
	int nfds;
	int ret;
	struct isp_os_poll_event events[4];
	struct isp_os_poll_event *pe;
	isp_io_handle_t *io;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	nfds = isp_os_poll_wait(p->efd, events, ARRAY_SIZE(events), timeout);
	if (nfds < 0) {
		ret = nfds;
		goto err;
	}
	if (nfds) {
		time_tracker_begin("total");
	}
	for (i = 0; i < nfds; i++) {
		pe = events + i;
		io = pe->data;
		debug("fd: %d, event: %u ocurred\n", io->fd, pe->events);
		pe->events &= io->events | ISP_OS_POLL_ERR | ISP_OS_POLL_HUP;
		if (pe->events & (ISP_OS_POLL_ERR | ISP_OS_POLL_HUP)) {
			isp_release(io, io_release);
			continue;
		}
		if (isp_call_op_ret(ret, io, cb, pe->events)) {
			time_tracker_end("total");
			goto err;
		}
	}

	if (nfds) {
		time_tracker_end("total");
	}

	return RTS_ISP_OK;

err:
	if (ret != -RTS_ISP_EINTR) {
		isp_perror(ret, "%s fail", __func__);
	}
	return ret;
}

static int poll_run(isp_poll_t p)
{
	int ret;
	int timeout;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	time_tracker_begin("total");
	isp_poll_update_time(p);
	ret = poll_run_timers(p);
	if (ret) {
		goto err;
	}

	ret = poll_add_pending_io(p);
	if (ret) {
		goto err;
	}

	timeout = poll_get_next_timeout(p);
	time_tracker_end("total");

	return poll_wait(p, timeout);
err:
	time_tracker_end("total");
	return ret;
}

static int poll_cleanup_handles(isp_poll_t p)
{
	struct isp_list *list;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	while (!avl_is_empty(&p->timers)) {
		isp_timer_handle_t *timer;

		debug("cleanup timers\n");
		timer = avl_first_element(&p->timers, timer, avl);
		isp_timer_cleanup(timer);
	}
	while (!list_empty(&p->pendings)) {
		isp_io_handle_t *io;

		debug("cleanup pendings\n");
		list = list_head(&p->pendings);
		io = list_data(list, struct isp_io_handle, pending);
		isp_release(io, io_release);
	}
	while (!list_empty(&p->watchers)) {
		isp_io_handle_t *io;

		debug("cleanup watchers\n");
		list = list_head(&p->watchers);
		io = list_data(list, struct isp_io_handle, watcher);
		isp_release(io, io_release);
	}
	return RTS_ISP_OK;
}

static int poll_trig_read(isp_io_handle_t *io, uint32_t events)
{
	int ret;

	if (!io || !events) {
		return -RTS_ISP_EINVAL;
	}

	isp_assert(events == ISP_OS_POLL_IN);

	ret = isp_os_eventfd_read(isp_io_fd(io));
	if (ret) {
		return ret;
	}

	debug("%s done\n", __func__);

	return RTS_ISP_OK;
}

static int poll_trig_init(isp_poll_t p)
{
	int ret;
	isp_os_fd_t fd;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	fd = isp_os_eventfd();
	if (fd < 0) {
		return fd;
	}

	ret = isp_io_init(&p->trig, p, fd, poll_trig_read);
	if (ret) {
		isp_close(fd);
		goto err;
	}
	ret = isp_io_event_add(&p->trig, ISP_OS_POLL_IN);
	if (ret) {
		goto err;
	}

	return RTS_ISP_OK;

err:
	isp_io_cleanup(&p->trig);
	return ret;
}

static int poll_trig_cleanup(isp_poll_t p)
{
	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	isp_io_cleanup(&p->trig);
	debug("%s ok\n", __func__);

	return RTS_ISP_OK;
}

static int poll_trig(isp_poll_t p)
{
	return isp_os_eventfd_write(isp_io_fd(&p->trig));
}

static void poll_wq_worker(void *arg)
{
	struct isp_work_queue *wq = arg;

	while (RTS_ISP_TRUE) {
		struct isp_work_handle *handle;

		isp_os_thread_mutex_lock(&wq->lock);
		wq->idle_threads++;
		if (list_empty(&wq->queue) && !wq->shutdown) {
			isp_os_thread_cond_wait(&wq->cond, &wq->lock);
		}
		wq->idle_threads--;

		if (wq->shutdown) {
			debug("worker shut down\n");
			isp_os_thread_mutex_unlock(&wq->lock);
			isp_os_thread_exit();
		}

		if (list_empty(&wq->queue)) {
			isp_os_thread_mutex_unlock(&wq->lock);
			continue;
		}
		handle = list_data(list_next(&wq->queue),
						   struct isp_work_handle, list);
		list_remove_init(&handle->list);
		wq->queue_handles--;
		isp_os_thread_mutex_unlock(&wq->lock);

		debug("work queue run work once\n");
		handle->ret = handle->work(handle->data);

		isp_os_thread_mutex_lock(&wq->lock);
		list_insert_tail(&wq->done, &handle->list);
		isp_os_thread_mutex_unlock(&wq->lock);
		isp_os_eventfd_write(isp_io_fd(&wq->wq_io));
	}
}

static struct isp_work_handle *poll_wq_get_handle(struct isp_work_queue *wq)
{
	struct isp_work_handle *handle;

	isp_os_thread_mutex_lock(&wq->lock);
	if (wq->shutdown) {
		isp_os_thread_mutex_unlock(&wq->lock);
		return NULL;
	}
	if (wq->idle_handles) {
		isp_assert(!list_empty(&wq->idle));
		wq->idle_handles--;
		handle = list_data(list_next(&wq->idle), struct isp_work_handle,
						   list);
		list_remove_init(&handle->list);
	} else {
		if (!isp_calloc_item(handle)) {
			goto out;
		}
		list_init(&handle->list);
	}
out:
	isp_os_thread_mutex_unlock(&wq->lock);
	return handle;
}

static void poll_wq_put_handle(struct isp_work_handle *handle)
{
	struct isp_work_queue *wq;

	if (!handle || !handle->p) {
		return;
	}

	wq = &handle->p->wq;
	isp_os_thread_mutex_lock(&wq->lock);
	if (wq->shutdown || wq->idle_handles > 8) {
		isp_free(handle);
	} else {
		list_insert_tail(&wq->idle, &handle->list);
		wq->idle_handles++;
	}
	isp_os_thread_mutex_unlock(&wq->lock);
}

static int poll_wq_read(isp_io_handle_t *io, uint32_t events)
{
	int ret;
	struct isp_work_queue *wq;
	struct isp_list done;
	struct isp_work_handle *handle;
	struct isp_work_handle *next;

	if (!io || !events) {
		return -RTS_ISP_EINVAL;
	}

	isp_assert(events == ISP_OS_POLL_IN);

	ret = isp_os_eventfd_read(isp_io_fd(io));
	if (ret) {
		return ret;
	}

	wq = &io->p->wq;

	isp_os_thread_mutex_lock(&wq->lock);
	list_move_list(&wq->done, &done);
	isp_os_thread_mutex_unlock(&wq->lock);

	list_foreach_data_safe(handle, next, &done, list) {
		list_remove_init(&handle->list);
		if (handle->done) {
			ret = handle->done(handle->data, handle->ret);
		}
		poll_wq_put_handle(handle);
		if (ret) {
			isp_os_thread_mutex_lock(&wq->lock);
			list_move_list(&done, &wq->done);
			isp_os_thread_mutex_unlock(&wq->lock);
			isp_os_eventfd_write(isp_io_fd(io));
			goto err;
		}
	}

	debug("%s done\n", __func__);

	return RTS_ISP_OK;
err:
	isp_perror(ret, "work queue done callback error");
	return ret;
}

static void poll_wq_stop_threads(struct isp_work_queue *wq)
{
	int i;

	debug("%s begin\n", __func__);
	isp_os_thread_mutex_lock(&wq->lock);
	wq->shutdown = 1;
	for (i = 0; i < ARRAY_SIZE(wq->threads); i++) {
		isp_os_thread_cond_signal(&wq->cond);
	}
	isp_os_thread_mutex_unlock(&wq->lock);
	for (i = 0; i < ARRAY_SIZE(wq->threads); i++) {
		isp_os_thread_join(wq->threads[i]);
	}
	debug("%s ok\n", __func__);
}

static int poll_wq_start_threads(struct isp_work_queue *wq)
{
	int i;
	int ret;

	wq->shutdown = 0;
	for (i = 0; i < ARRAY_SIZE(wq->threads); i++) {
		ret = isp_os_thread_create(&wq->threads[i], "work queue",
								   ISP_OS_THREAD_PRIORITY_MID,
								   poll_wq_worker, wq);
		if (ret) {
			poll_wq_stop_threads(wq);
			break;
		}
	}
	if (ret) {
		isp_perror(ret, "%s fail", __func__);
	} else {
		debug("%s ok\n", __func__, ret);
	}

	return ret;
}

static int poll_wq_init(isp_poll_t p)
{
	int ret;
	isp_os_fd_t fd;
	struct isp_work_queue *wq;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	wq = &p->wq;

	isp_os_thread_mutex_init(&wq->lock);
	isp_os_thread_cond_init(&wq->cond);
	list_init(&wq->queue);
	list_init(&wq->done);
	list_init(&wq->idle);
	wq->queue_handles = 0;
	wq->idle_handles = 0;
	wq->shutdown = 1;

	fd = isp_os_eventfd();
	if (fd < 0) {
		return fd;
	}
	ret = isp_io_init(&wq->wq_io, p, fd, poll_wq_read);
	if (ret) {
		isp_close(fd);
		goto err;
	}
	ret = isp_io_event_add(&wq->wq_io, ISP_OS_POLL_IN);
	if (ret) {
		goto err;
	}

	debug("%s ok\n", __func__);
	return RTS_ISP_OK;
err:
	poll_wq_cleanup(p);
	return ret;
}

static void poll_wq_cleanup_handles(struct isp_work_queue *wq)
{
	struct isp_work_handle *handle;
	struct isp_work_handle *next;

	if (!wq || !wq->shutdown) {
		return;
	}

	list_foreach_data_safe(handle, next, &wq->queue, list) {
		list_remove_init(&handle->list);
		if (handle->done) {
			handle->done(handle->data, -RTS_ISP_ECANCELD);
		}
		isp_free(handle);
	}
	wq->queue_handles = 0;

	list_foreach_data_safe(handle, next, &wq->done, list) {
		list_remove_init(&handle->list);
		if (handle->done) {
			handle->done(handle->data, handle->ret);
		}
		isp_free(handle);
	}

	list_foreach_data_safe(handle, next, &wq->idle, list) {
		list_remove_init(&handle->list);
		wq->idle_handles--;
		isp_free(handle);
	}
}

static int poll_wq_cleanup(isp_poll_t p)
{
	struct isp_work_queue *wq;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	wq = &p->wq;

	isp_assert(wq->shutdown);
	isp_io_cleanup(&wq->wq_io);
	poll_wq_cleanup_handles(wq);
	isp_os_thread_cond_destroy(&wq->cond);
	isp_os_thread_mutex_destroy(&wq->lock);

	debug("%s ok\n", __func__);

	return RTS_ISP_OK;
}

int isp_poll_create(isp_poll_t *pp)
{
	int ret;
	isp_poll_t p = NULL;

	if (!pp) {
		return -RTS_ISP_EINVAL;
	}
	if (!isp_calloc_item(p)) {
		return -RTS_ISP_ENOMEM;
	}

	p->efd = isp_os_poll_create();
	if (p->efd < 0) {
		ret = p->efd;
		goto err;
	}
	avl_init(&p->timers, poll_timer_cmp, RTS_ISP_TRUE, NULL);
	list_init(&p->pendings);
	list_init(&p->watchers);
	ret = poll_wq_init(p);
	if (ret) {
		goto err;
	}
	ret = poll_trig_init(p);
	if (ret) {
		goto err;
	}

	isp_poll_update_time(p);

	*pp = p;

	debug("%s ok\n", __func__);

	return RTS_ISP_OK;
err:
	isp_release(p, isp_poll_release);
	return ret;
}

void isp_poll_release(isp_poll_t p)
{
	if (!p) {
		return;
	}

	poll_trig_cleanup(p);
	poll_wq_cleanup(p);
	poll_cleanup_handles(p);
	isp_close(p->efd);
	isp_free(p);
}

int isp_poll_update_time(isp_poll_t p)
{
	p->time = isp_os_time_get_ms();
	return RTS_ISP_OK;
}

int isp_poll_loop_run(isp_poll_t p)
{
	int ret;

	if (!p) {
		return -RTS_ISP_EINVAL;
	}

	ret = poll_wq_start_threads(&p->wq);
	if (ret) {
		return ret;
	}

	p->enable = RTS_ISP_TRUE;
	isp_poll_update_time(p);

	debug("%s event loop start\n", __func__);
	while (poll_alive(p)) {
		ret = poll_run(p);
		if (ret && ret != -RTS_ISP_EINTR) {
			isp_perror(ret, "isp poll loop exit err");
			break;
		}
		ret = RTS_ISP_OK;
	}
	debug("%s event loop stoped\n", __func__);
	poll_wq_stop_threads(&p->wq);
	return ret;
}

int isp_poll_loop_stop(isp_poll_t p)
{
	if (!p) {
		return -RTS_ISP_EINVAL;
	}
	p->enable = RTS_ISP_FALSE;
	poll_trig(p);
	return RTS_ISP_OK;
}

int isp_poll_add_work(isp_poll_t p, void *data, isp_work_cb work,
					  isp_work_done_cb done)
{
	struct isp_work_handle *handle;
	struct isp_work_queue *wq;

	if (!p || !work) {
		return -RTS_ISP_EINVAL;
	}

	wq = &p->wq;
	handle = poll_wq_get_handle(wq);
	if (!handle) {
		return -RTS_ISP_ENOTREADY;
	}
	handle->p = p;
	handle->data = data;
	handle->work = work;
	handle->done = done;

	isp_os_thread_mutex_lock(&wq->lock);
	if (wq->shutdown || wq->queue_handles == ARRAY_SIZE(wq->threads)) {
		isp_os_thread_mutex_unlock(&wq->lock);
		poll_wq_put_handle(handle);
		return -RTS_ISP_EAGAIN;
	}
	list_insert_tail(&wq->queue, &handle->list);
	wq->queue_handles++;
	if (wq->idle_threads > 0) {
		isp_os_thread_cond_signal(&wq->cond);
	}
	isp_os_thread_mutex_unlock(&wq->lock);

	return RTS_ISP_OK;
}

int isp_poll_cancel_work(isp_poll_t p, void *data, isp_work_cb work,
						 isp_work_done_cb done)
{
	struct isp_work_queue *wq;
	struct isp_work_handle *handle;
	struct isp_work_handle *next;

	if (!p || !work) {
		return -RTS_ISP_EINVAL;
	}

	wq = &p->wq;
	isp_os_thread_mutex_lock(&wq->lock);
	if (wq->shutdown) {
		isp_os_thread_mutex_unlock(&wq->lock);
		return -RTS_ISP_ENOTREADY;
	}
	list_foreach_data_safe(handle, next, &wq->queue, list) {
		int ret = RTS_ISP_OK;

		if (handle->p == p && handle->data == data &&
			handle->work == work && handle->done == done) {
			list_remove_init(&handle->list);
			wq->queue_handles--;
			isp_os_thread_mutex_unlock(&wq->lock);
			if (handle->done)
				ret = handle->done(handle->data,
								   -RTS_ISP_ECANCELD);
			poll_wq_put_handle(handle);
			return ret;
		}
	}
	isp_os_thread_mutex_unlock(&wq->lock);
	return -RTS_ISP_ENOITEM;
}

void isp_poll_set_fd_nonclose(isp_poll_t p, isp_os_fd_t fd, int nonclose)
{
	struct isp_io_handle *io;

	if (!p || fd < 0) {
		return;
	}

	list_foreach_data(io, &p->pendings, pending) {
		if (io->fd == fd) {
			io->nonclose = nonclose;
			return;
		}
	}
	list_foreach_data(io, &p->watchers, watcher) {
		if (io->fd == fd) {
			io->nonclose = nonclose;
			return;
		}
	}
}
