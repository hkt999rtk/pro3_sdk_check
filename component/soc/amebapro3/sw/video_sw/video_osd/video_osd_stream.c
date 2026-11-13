#include <stdlib.h>
#include <string.h>
#include "ameba_soc.h"
#include "hal_video.h"
#include "video_api.h"
#include "video_osd_plan.h"

typedef struct {
	video_osd_plan_region_t region;
	uint64_t versions[VIDEO_OSD_MAX_ITEMS];
	int version_count;
	int references;
	size_t allocation;
	uint8_t *pixels;
} output_t;

typedef struct {
	video_osd_target_t target;
	video_osd_stream_status_t status;
	void *request_raw;
	video_osd_scene_t *request;
	volatile uint32_t *completion;
	output_t *active[VIDEO_OSD_SCENE_MAX_REGIONS];
	output_t *staged[VIDEO_OSD_SCENE_MAX_REGIONS];
	int active_count, staged_count;
	int indices[VIDEO_OSD_SCENE_MAX_REGIONS];
	int index_count;
	int claimed, releasing, released;
	/* Heap-resident work area: timestamp tasks must not need a large stack. */
	video_osd_render_item_t items[VIDEO_OSD_MAX_ITEMS];
	video_osd_plan_t plan;
} stream_t;

static stream_t *streams[VIDEO_ENC_VALID_CH];

int video_osd_sync_retry(int (*operation)(void *), void *context, uint32_t timeout_ms)
{
	uint64_t start = rtos_time_get_current_system_time_ms_64bit();
	for (;;) {
		int result = operation(context);
		if (result != VIDEO_OSD_EBUSY) {
			return result;
		}
		uint64_t elapsed = rtos_time_get_current_system_time_ms_64bit() - start;
		if (elapsed >= timeout_ms) {
			return VIDEO_OSD_ETIMEDOUT;
		}
		uint32_t remaining = timeout_ms - (uint32_t)elapsed;
		rtos_time_delay_ms(remaining < 20U ? remaining : 20U);
	}
}

int video_osd_sync_check_applied(const video_osd_stream_status_t *status, uint32_t revision)
{
	if (revision > status->requested_revision) {
		return VIDEO_OSD_EINVAL;
	}
	if (status->last_error) {
		return status->pending ? VIDEO_OSD_EBUSY : status->last_error;
	}
	if (status->applied_revision >= revision) {
		return 0;
	}
	return status->pending ? VIDEO_OSD_EBUSY : VIDEO_OSD_EENCODER;
}

static void account(stream_t *p)
{
	size_t total = p->status.allocated_bytes + p->status.source_bytes;
	if (total > p->status.peak_bytes) {
		p->status.peak_bytes = total;
	}
	p->status.index_count = p->index_count;
	p->status.bitmap_regions = p->status.argb_regions = 0;
	for (int i = 0; i < p->active_count; i++) {
		if (p->active[i]->region.format == VIDEO_OSD_FORMAT_BITMAP) {
			p->status.bitmap_regions++;
		} else {
			p->status.argb_regions++;
		}
	}
}

static void put(stream_t *p, output_t *out)
{
	if (out && --out->references == 0) {
		p->status.allocated_bytes -= out->allocation;
		free(out);
	}
}

static void trim_indices(stream_t *p, int count)
{
	while (p->index_count > count) {
		video_free_overlay_index(p->target.stream_id, p->indices[--p->index_count]);
	}
}

static void discard_staged(stream_t *p)
{
	for (int i = 0; i < p->staged_count; i++) {
		put(p, p->staged[i]);
		p->staged[i] = NULL;
	}
	p->staged_count = 0;
	trim_indices(p, p->active_count);
}

static void reap(stream_t *p)
{
	if (!p->status.pending) {
		return;
	}
	DCache_Invalidate((uint32_t)p->completion, CACHE_LINE_SIZE);
	uint32_t result = *p->completion;
	if (result == VIDEO_OSD_SCENE_PENDING) {
		return;
	}
	p->status.pending = 0;
	if (result == VIDEO_OSD_SCENE_APPLIED) {
		for (int i = 0; i < p->active_count; i++) {
			put(p, p->active[i]);
		}
		memcpy(p->active, p->staged, sizeof(p->active));
		p->active_count = p->staged_count;
		memset(p->staged, 0, sizeof(p->staged));
		p->staged_count = 0;
		p->claimed = 1;
		p->released = p->releasing;
		p->status.applied_revision = p->request->revision;
		p->status.applied_timestamp = p->completion[1];
		p->status.last_error = 0;
		trim_indices(p, p->active_count);
	} else {
		if (result == VIDEO_OSD_SCENE_FAULT) {
			p->claimed = 1;
		}
		/* VP fault path emits no frames and never installs rejected DMA pointers. */
		discard_staged(p);
		p->status.last_error = VIDEO_OSD_PRIVACY_EENCODER;
		p->releasing = 0;
	}
	account(p);
}

void *video_osd_stream_lookup_locked(const video_osd_target_t *target)
{
	if (!target || target->stream_id < 0 || target->stream_id >= VIDEO_ENC_VALID_CH) {
		return NULL;
	}
	stream_t *p = streams[target->stream_id];
	if (!p || p->target.codec != target->codec || p->target.frame_width != target->frame_width ||
		p->target.frame_height != target->frame_height) {
		return NULL;
	}
	return p;
}

static int same_output(const output_t *out, const video_osd_plan_region_t *r,
					   const video_osd_render_item_t *items, int count)
{
	const video_osd_plan_region_t *a = &out->region;
	if (memcmp(&a->region, &r->region, sizeof(r->region)) || a->format != r->format ||
		a->alpha != r->alpha || a->y != r->y || a->u != r->u || a->v != r->v) {
		return 0;
	}
	int n = 0;
	for (int i = 0; i < count; i++) {
		if (r->members & (1U << i)) {
			if (n >= out->version_count || out->versions[n++] != items[i].version) {
				return 0;
			}
		}
	}
	return n == out->version_count;
}

static int submit(stream_t *p, int release)
{
	if (p->status.requested_revision == UINT32_MAX && !release) {
		discard_staged(p);
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	memset(p->request, 0, sizeof(*p->request));
	p->request->codec = p->target.codec;
	p->request->release = release;
	p->request->count = p->staged_count;
	for (int i = 0; i < p->staged_count; i++) {
		output_t *out = p->staged[i];
		video_osd_plan_region_t *r = &out->region;
		p->request->overlays[i] = (osd_overlay_t) {
			.osdOverlayIdx = p->indices[i], .osdOverlayEnable = 1, .osdOverlayFormat = r->format,
			.osdOverlayXoffset = r->region.x, .osdOverlayYoffset = r->region.y,
			.osdOverlayWidth = r->region.width, .osdOverlayHeight = r->region.height,
			.osdOverlayAlpha = r->alpha, .osdOverlayBitmapY = r->y,
			.osdOverlayBitmapU = r->u, .osdOverlayBitmapV = r->v, .osdOverlayPicBuf = out->pixels,
		};
	}
	p->request->revision = p->status.requested_revision;
	if (p->request->revision != UINT32_MAX) {
		p->request->revision++;
	}
	p->request->completion = p->completion;
	*p->completion = VIDEO_OSD_SCENE_PENDING;
	DCache_Clean((uint32_t)p->completion, CACHE_LINE_SIZE);
	p->status.pending = 1;
	int result = VIDEO_SetOSDScene(p->target.stream_id, p->request);
	/* -2 is pre-send AP allocation failure. A lost IPC ACK is not proof that
	 * the request was rejected: keep buffers alive until completion arrives. */
	if (result == -2) {
		p->status.pending = 0;
		discard_staged(p);
		return VIDEO_OSD_PRIVACY_ENOMEM;
	}
	reap(p);
	if (!p->status.pending && p->status.last_error) {
		return p->status.last_error;
	}
	p->status.requested_revision = p->request->revision;
	return 0;
}

int video_osd_stream_commit_locked(void *context)
{
	stream_t *p = context;
	reap(p);
	if (p->status.pending || p->releasing || p->released) {
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	size_t sources = 0;
	int count = video_osd_collect_layers_locked(p->target.stream_id, p->items, VIDEO_OSD_MAX_ITEMS, &sources);
	p->status.source_bytes = sources;
	account(p);
	if (count < 0) {
		return count;
	}
	int result = video_osd_plan_build(&p->target, p->items, count, VIDEO_OSD_SCENE_MAX_REGIONS, &p->plan);
	if (result) {
		return result;
	}
	while (p->index_count < p->plan.count) {
		int index = video_alloc_overlay_index(p->target.stream_id);
		if (index < 0) {
			break;
		}
		p->indices[p->index_count++] = index;
	}
	result = video_osd_plan_build(&p->target, p->items, count, p->index_count, &p->plan);
	if (result) {
		trim_indices(p, p->active_count);
		return result;
	}
	if (p->status.memory_budget && p->status.allocated_bytes + sources > p->status.memory_budget) {
		result = VIDEO_OSD_PRIVACY_ENOMEM;
		goto fail;
	}
	for (int i = 0; i < p->plan.count; i++) {
		video_osd_plan_region_t *r = &p->plan.regions[i];
		output_t *out = NULL;
		for (int j = 0; j < p->active_count; j++) {
			if (same_output(p->active[j], r, p->items, count)) {
				out = p->active[j];
				out->references++;
				break;
			}
		}
		if (!out) {
			size_t payload = video_osd_plan_region_bytes(r);
			size_t allocation = sizeof(*out) + CACHE_LINE_SIZE - 1U + payload;
			if (p->status.memory_budget && allocation > p->status.memory_budget - p->status.allocated_bytes - sources) {
				result = VIDEO_OSD_PRIVACY_ENOMEM;
				goto fail;
			}
			out = calloc(1, allocation);
			if (!out) {
				result = VIDEO_OSD_PRIVACY_ENOMEM;
				goto fail;
			}
			out->allocation = allocation;
			out->references = 1;
			out->region = *r;
			out->pixels = (uint8_t *)(((uintptr_t)(out + 1) + CACHE_LINE_SIZE - 1U) & ~((uintptr_t)CACHE_LINE_SIZE - 1U));
			for (int j = 0; j < count; j++) {
				if (r->members & (1U << j)) {
					out->versions[out->version_count++] = p->items[j].version;
				}
			}
			video_osd_plan_render(out->pixels, r, p->items, count);
			DCache_Clean((uint32_t)out->pixels, payload);
			p->status.allocated_bytes += allocation;
			account(p);
		}
		p->staged[p->staged_count++] = out;
	}
	int unchanged = p->staged_count == p->active_count;
	for (int i = 0; i < p->staged_count && unchanged; i++) {
		unchanged = p->staged[i] == p->active[i];
	}
	if (unchanged && p->claimed && !p->status.last_error && VIDEO_GetOSDSceneState(p->target.stream_id) == 1) {
		discard_staged(p);
		account(p);
		return 0;
	}
	return submit(p, 0);
fail:
	discard_staged(p);
	account(p);
	return result;
}

static int conflicts(void *context, const video_osd_region_t *region)
{
	return video_osd_privacy_conflicts_locked(((stream_t *)context)->target.stream_id, region);
}

int video_osd_stream_init(video_osd_stream_t *ctx, const video_osd_stream_config_t *config)
{
	video_osd_privacy_caps_t caps;
	if (!ctx || ctx->priv || !config || video_osd_privacy_get_caps(&config->target, &caps)) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	size_t request_bytes = ((sizeof(video_osd_scene_t) + CACHE_LINE_SIZE - 1U) & ~(CACHE_LINE_SIZE - 1U)) + CACHE_LINE_SIZE;
	size_t total = sizeof(stream_t) + request_bytes + CACHE_LINE_SIZE - 1U;
	if (config->memory_budget && total > config->memory_budget) {
		return VIDEO_OSD_PRIVACY_ENOMEM;
	}
	stream_t *p = calloc(1, sizeof(*p));
	if (!p) {
		return VIDEO_OSD_PRIVACY_ENOMEM;
	}
	p->request_raw = calloc(1, request_bytes + CACHE_LINE_SIZE - 1U);
	if (!p->request_raw) {
		free(p);
		return VIDEO_OSD_PRIVACY_ENOMEM;
	}
	p->request = (void *)(((uintptr_t)p->request_raw + CACHE_LINE_SIZE - 1U) & ~((uintptr_t)CACHE_LINE_SIZE - 1U));
	p->completion = (volatile uint32_t *)((uint8_t *)p->request + request_bytes - CACHE_LINE_SIZE);
	p->target = config->target;
	p->status.memory_budget = config->memory_budget;
	p->status.allocated_bytes = total;
	int result = VIDEO_OSD_PRIVACY_EBUSY;
	if (!video_osd_manager_lock()) {
		if (!streams[p->target.stream_id] && !video_osd_compositor_attach(&p->target, p, video_osd_stream_commit_locked, conflicts)) {
			streams[p->target.stream_id] = p;
			ctx->priv = p;
			result = 0;
			account(p);
		}
		video_osd_manager_unlock();
	}
	if (result) {
		free(p->request_raw);
		free(p);
	}
	return result;
}

int video_osd_stream_status_locked(void *context, video_osd_stream_status_t *status)
{
	stream_t *p = context;
	reap(p);
	size_t sources = 0;
	video_osd_collect_layers_locked(p->target.stream_id, p->items, VIDEO_OSD_MAX_ITEMS, &sources);
	p->status.source_bytes = sources;
	if (!p->status.pending && p->claimed && !p->released && VIDEO_GetOSDSceneState(p->target.stream_id) != 1) {
		p->status.last_error = VIDEO_OSD_PRIVACY_EENCODER;
	}
	account(p);
	*status = p->status;
	return 0;
}

int video_osd_stream_get_status(video_osd_stream_t *ctx, video_osd_stream_status_t *status)
{
	if (!ctx || !ctx->priv || !status) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	if (video_osd_manager_lock()) {
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	int result = video_osd_stream_status_locked(ctx->priv, status);
	video_osd_manager_unlock();
	return result;
}

int video_osd_stream_deinit(video_osd_stream_t *ctx)
{
	if (!ctx) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	if (!ctx->priv) {
		return 0;
	}
	if (video_osd_manager_lock()) {
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	stream_t *p = ctx->priv;
	reap(p);
	int result = VIDEO_OSD_PRIVACY_EBUSY;
	if (p->status.pending || video_osd_compositor_has_clients(p->target.stream_id)) {
		goto done;
	}
	if (p->claimed && !p->released) {
		p->releasing = 1;
		result = submit(p, 1);
		if (result) {
			p->releasing = 0;
		} else {
			result = VIDEO_OSD_PRIVACY_EBUSY;
		}
		goto done;
	}
	trim_indices(p, 0);
	video_osd_compositor_detach(p->target.stream_id);
	streams[p->target.stream_id] = NULL;
	free(p->request_raw);
	free(p);
	ctx->priv = NULL;
	result = 0;
done:
	video_osd_manager_unlock();
	return result;
}

typedef struct {
	video_osd_stream_t *ctx;
	uint32_t revision;
	int captured;
	video_osd_stream_status_t *out;
} video_osd_wait_t;

static int wait_applied_once(void *context)
{
	video_osd_wait_t *wait = context;
	video_osd_stream_status_t status;
	int result = video_osd_stream_get_status(wait->ctx, &status);
	if (result) {
		return result;
	}
	if (wait->out) {
		*wait->out = status;
	}
	if (!wait->captured) {
		wait->revision = status.requested_revision;
		wait->captured = 1;
	}
	return video_osd_sync_check_applied(&status, wait->revision);
}

int video_osd_stream_wait_applied(video_osd_stream_t *ctx, uint32_t revision,
								  uint32_t timeout_ms, video_osd_stream_status_t *status)
{
	video_osd_wait_t wait = {
		.ctx = ctx, .revision = revision, .captured = revision != 0, .out = status,
	};
	return video_osd_sync_retry(wait_applied_once, &wait, timeout_ms);
}

static int stream_deinit_once(void *context)
{
	return video_osd_stream_deinit(context);
}

int video_osd_stream_deinit_sync(video_osd_stream_t *ctx, uint32_t timeout_ms)
{
	return video_osd_sync_retry(stream_deinit_once, ctx, timeout_ms);
}
