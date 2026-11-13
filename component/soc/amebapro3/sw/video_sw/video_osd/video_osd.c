/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ameba_soc.h"
#include "video_api.h"
#include "video_osd.h"
#include "video_osd_font.h"
#include "video_osd_internal.h"

#define VIDEO_OSD_TIMESTAMP_DEFAULT_VALID_TIME 1577836800U
#define VIDEO_OSD_TIMESTAMP_FORMAT_CAPACITY    64
#define VIDEO_OSD_TIMESTAMP_TASK_PERIOD_MS     1000
#define VIDEO_OSD_TIMESTAMP_TASK_POLL_MS       100
#define VIDEO_OSD_TIMESTAMP_TASK_STACK_SIZE    2048

static const char *TAG = "VIDEO_OSD";

typedef struct video_osd_protected_region {
	struct video_osd_protected_region *next;
	int registered;
	int removing;
	uint32_t remove_revision;
	int stream_id;
	int osd_idx;
	uint32_t codec;
	video_osd_format_t format;
	video_osd_region_t region;
	uint8_t alpha;
	uint8_t y;
	uint8_t u;
	uint8_t v;
	uint8_t *buffer;
	uint64_t version;
	size_t storage_bytes;
} video_osd_protected_region_t;

struct video_osd_nn_group {
	struct video_osd_nn_group *next;
	int stream_id;
	uint32_t codec;
	int layer_count;
	int osd_idx[VIDEO_OSD_NN_MAX_LAYERS];
	int active[VIDEO_OSD_NN_MAX_LAYERS];
	int suppressed;
	video_osd_region_t region[VIDEO_OSD_NN_MAX_LAYERS];
	video_osd_layer_update_t shadow[VIDEO_OSD_NN_MAX_LAYERS];
	uint64_t version;
	size_t external_bytes;
	size_t transient_bytes;
};

typedef struct {
	video_osd_protected_region_t protected_region;
	video_osd_timestamp_config_t config;
	char format[VIDEO_OSD_TIMESTAMP_FORMAT_CAPACITY];
	char last_text[VIDEO_OSD_TIMESTAMP_MAX_CHARS + 1];
	void *buffer_raw[2];
	uint8_t *buffer[2];
	size_t buffer_size;
	int active_buffer;
	volatile int running;
	rtos_task_t task;
	rtos_sema_t start;
	rtos_sema_t stopped;
} video_osd_timestamp_impl_t;

typedef struct {
	video_osd_protected_region_t protected_region;
	void *buffer_raw;
	uint8_t *buffer;
	size_t buffer_size;
} video_osd_logo_impl_t;

static rtos_mutex_t video_osd_lock;
static video_osd_protected_region_t *video_osd_protected_list;
static video_osd_nn_group_t *video_osd_nn_group_list;

typedef struct {
	video_osd_target_t target;
	void *context;
	video_osd_commit_fn commit;
	video_osd_conflict_fn conflicts;
	uint32_t logical_indices;
} video_osd_compositor_slot_t;
static video_osd_compositor_slot_t compositor[VIDEO_ENC_VALID_CH];

int video_osd_manager_lock(void)
{
	if (!video_osd_lock) {
		rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
		if (!video_osd_lock) {
			rtos_mutex_create(&video_osd_lock);
		}
		rtos_critical_exit(RTOS_CRITICAL_DEFAULT);
	}
	if (!video_osd_lock) {
		return -1;
	}
	return rtos_mutex_take(video_osd_lock, RTOS_MAX_DELAY) == RTK_SUCCESS ? 0 : -1;
}

void video_osd_manager_unlock(void)
{
	rtos_mutex_give(video_osd_lock);
}

static int video_osd_codec_valid(uint32_t codec)
{
	return codec == VIDEO_H264 || codec == VIDEO_HEVC;
}

int video_osd_region_conflicts(const video_osd_region_t *first, const video_osd_region_t *second, uint32_t codec)
{
	uint32_t block_height = codec == VIDEO_H264 ? 16U : 64U;
	uint32_t first_left = first->x / 64U;
	uint32_t first_right = (first->x + first->width - 1U) / 64U;
	uint32_t first_top = first->y / block_height;
	uint32_t first_bottom = (first->y + first->height - 1U) / block_height;
	uint32_t second_left = second->x / 64U;
	uint32_t second_right = (second->x + second->width - 1U) / 64U;
	uint32_t second_top = second->y / block_height;
	uint32_t second_bottom = (second->y + second->height - 1U) / block_height;

	return !(first_right < second_left || second_right < first_left ||
			 first_bottom < second_top || second_bottom < first_top);
}

static int video_osd_target_valid(const video_osd_target_t *target)
{
	if (!target || target->stream_id < 0 || target->stream_id >= VIDEO_ENC_VALID_CH ||
		!video_osd_codec_valid(target->codec) || target->frame_width == 0 || target->frame_height == 0) {
		return 0;
	}
	return 1;
}

static int video_osd_resolve_position(const video_osd_target_t *target, const video_osd_placement_t *placement,
									  uint32_t width, uint32_t height, video_osd_region_t *region)
{
	uint32_t x;
	uint32_t y;

	if (!video_osd_target_valid(target) || !placement || !region || width == 0 || height == 0 ||
		width > target->frame_width || height > target->frame_height) {
		return -1;
	}

	switch (placement->position) {
	case VIDEO_OSD_POSITION_UPPER_LEFT:
		x = placement->margin_x;
		y = placement->margin_y;
		break;
	case VIDEO_OSD_POSITION_UPPER_RIGHT:
		if (placement->margin_x > target->frame_width - width) {
			return -1;
		}
		x = target->frame_width - width - placement->margin_x;
		y = placement->margin_y;
		break;
	case VIDEO_OSD_POSITION_LOWER_LEFT:
		if (placement->margin_y > target->frame_height - height) {
			return -1;
		}
		x = placement->margin_x;
		y = target->frame_height - height - placement->margin_y;
		break;
	case VIDEO_OSD_POSITION_LOWER_RIGHT:
		if (placement->margin_x > target->frame_width - width || placement->margin_y > target->frame_height - height) {
			return -1;
		}
		x = target->frame_width - width - placement->margin_x;
		y = target->frame_height - height - placement->margin_y;
		break;
	case VIDEO_OSD_POSITION_CUSTOM:
		x = placement->x;
		y = placement->y;
		break;
	default:
		return -1;
	}

	if (x > target->frame_width - width || y > target->frame_height - height || (x & 1U) || (y & 1U)) {
		return -1;
	}
	region->x = x;
	region->y = y;
	region->width = width;
	region->height = height;
	return 0;
}

static uint8_t *video_osd_aligned_alloc(size_t size, void **raw)
{
	uintptr_t aligned;
	void *allocation;

	if (!raw || size == 0 || size > SIZE_MAX - CACHE_LINE_SIZE) {
		return NULL;
	}
	allocation = malloc(size + CACHE_LINE_SIZE - 1U);
	if (!allocation) {
		return NULL;
	}
	aligned = ((uintptr_t)allocation + CACHE_LINE_SIZE - 1U) & ~((uintptr_t)CACHE_LINE_SIZE - 1U);
	*raw = allocation;
	return (uint8_t *)aligned;
}

int video_osd_compositor_has_clients(int stream_id)
{
	if (compositor[stream_id].logical_indices || video_osd_privacy_present_locked(stream_id)) {
		return 1;
	}
	for (video_osd_protected_region_t *p = video_osd_protected_list; p; p = p->next) {
		if (p->stream_id == stream_id) {
			return 1;
		}
	}
	for (video_osd_nn_group_t *g = video_osd_nn_group_list; g; g = g->next) {
		if (g->stream_id == stream_id) {
			return 1;
		}
	}
	return 0;
}

int video_osd_compositor_attach(const video_osd_target_t *target, void *context,
								video_osd_commit_fn commit, video_osd_conflict_fn conflicts)
{
	if (!video_osd_target_valid(target) || compositor[target->stream_id].context ||
		video_osd_compositor_has_clients(target->stream_id)) {
		return -1;
	}
	compositor[target->stream_id] = (video_osd_compositor_slot_t) {
		*target, context, commit, conflicts, 0
	};
	return 0;
}

void video_osd_compositor_detach(int stream_id)
{
	memset(&compositor[stream_id], 0, sizeof(compositor[stream_id]));
}

static uint64_t source_version;

uint64_t video_osd_next_version_locked(uint32_t count)
{
	uint64_t version = source_version + 1;
	source_version += count;
	return version;
}

int video_osd_client_alloc_index(int stream_id)
{
	if (stream_id < 0 || stream_id >= VIDEO_ENC_VALID_CH || video_osd_manager_lock()) {
		return -1;
	}
	int result = -1;
	if (compositor[stream_id].context) {
		for (int i = 0; i < VIDEO_OVERLAY_INDEX_COUNT; i++) {
			if (!(compositor[stream_id].logical_indices & (1U << i))) {
				compositor[stream_id].logical_indices |= 1U << i;
				result = i;
				break;
			}
		}
	} else {
		result = video_alloc_overlay_index(stream_id);
	}
	video_osd_manager_unlock();
	return result;
}

int video_osd_client_is_managed(int stream_id)
{
	if (stream_id < 0 || stream_id >= VIDEO_ENC_VALID_CH || video_osd_manager_lock()) {
		return 0;
	}
	int managed = compositor[stream_id].context != NULL;
	video_osd_manager_unlock();
	return managed;
}

void video_osd_client_free_index(int stream_id, int index)
{
	if (stream_id < 0 || stream_id >= VIDEO_ENC_VALID_CH || index < 0 || index >= VIDEO_OVERLAY_INDEX_COUNT || video_osd_manager_lock()) {
		return;
	}
	if (compositor[stream_id].context) {
		compositor[stream_id].logical_indices &= ~(1U << index);
	} else {
		video_free_overlay_index(stream_id, index);
	}
	video_osd_manager_unlock();
}

void video_osd_nn_group_source_bytes(video_osd_nn_group_t *group, size_t bytes)
{
	if (!group || video_osd_manager_lock()) {
		return;
	}
	group->external_bytes = bytes;
	video_osd_manager_unlock();
}

int video_osd_collect_layers_locked(int stream_id, video_osd_render_item_t *items, int capacity, size_t *source_bytes)
{
	memset(items, 0, (size_t)capacity * sizeof(*items));
	*source_bytes = 0;
	int count = video_osd_privacy_collect_locked(stream_id, items, capacity, source_bytes);
	if (count < 0) {
		return count;
	}
	video_osd_compositor_slot_t *slot = &compositor[stream_id];
	for (video_osd_nn_group_t *g = video_osd_nn_group_list; g; g = g->next) {
		if (g->stream_id != stream_id) {
			continue;
		}
		*source_bytes += sizeof(*g) + g->external_bytes + g->transient_bytes;
		int conflict = 0;
		for (int i = 0; i < g->layer_count; i++) {
			if (g->shadow[i].buffer) {
				*source_bytes += (size_t)(g->shadow[i].region.width / 8) * g->shadow[i].region.height;
			}
			if (!g->shadow[i].enable) {
				continue;
			}
			conflict |= slot->conflicts(slot->context, &g->shadow[i].region);
			for (video_osd_protected_region_t *p = video_osd_protected_list; p; p = p->next) {
				if (p->stream_id == stream_id && !p->removing && video_osd_region_conflicts(&p->region, &g->shadow[i].region, g->codec)) {
					conflict = 1;
				}
			}
		}
		if (conflict) {
			continue;
		}
		for (int i = 0; i < g->layer_count; i++) {
			if (!g->shadow[i].enable) {
				continue;
			}
			if (count == capacity) {
				return VIDEO_OSD_PRIVACY_ENOSPC;
			}
			items[count].layer = g->shadow[i];
			items[count].nn_group = g;
			items[count++].version = g->version + i;
		}
	}
	/* Privacy first, non-conflicting NN, logos, then bitmap text. */
	for (int pass = 0; pass < 2; pass++) {
		for (video_osd_protected_region_t *p = video_osd_protected_list; p; p = p->next) {
			if (p->stream_id != stream_id || (p->format == VIDEO_OSD_FORMAT_BITMAP) != pass) {
				continue;
			}
			*source_bytes += p->storage_bytes;
			if (p->removing) {
				continue;
			}
			if (count == capacity) {
				return VIDEO_OSD_PRIVACY_ENOSPC;
			}
			items[count].layer = (video_osd_layer_update_t) {
				1, p->osd_idx, p->format, p->region, p->alpha, p->y, p->u, p->v, p->buffer, 0
			};
			items[count++].version = p->version;
		}
	}
	return count;
}

static int video_osd_disable_nn_group_locked(video_osd_nn_group_t *group)
{
	int result = 0;
	video_osd_compositor_slot_t *slot = &compositor[group->stream_id];
	if (slot->context) {
		int enabled[VIDEO_OSD_NN_MAX_LAYERS];
		for (int i = 0; i < group->layer_count; i++) {
			enabled[i] = group->shadow[i].enable;
			group->shadow[i].enable = 0;
		}
		result = slot->commit(slot->context);
		if (result) {
			for (int i = 0; i < group->layer_count; i++) {
				group->shadow[i].enable = enabled[i];
			}
		} else {
			memset(group->active, 0, sizeof(group->active));
			for (int i = 0; i < group->layer_count; i++) {
				free(group->shadow[i].buffer);
				group->shadow[i].buffer = NULL;
			}
		}
		return result;
	}

	for (int i = 0; i < group->layer_count; i++) {
		if (!group->active[i]) {
			continue;
		}
		if (video_disable_osd(group->stream_id, group->osd_idx[i]) != 0) {
			result = -1;
		} else {
			group->active[i] = 0;
		}
	}
	if (result == 0) {
		group->suppressed = 1;
	}
	return result;
}

static int video_osd_protected_add(video_osd_protected_region_t *entry, uint32_t *revision)
{
	video_osd_protected_region_t *protected_entry;
	video_osd_nn_group_t *nn_group;
	int result = -1;

	if (!entry || entry->registered || video_osd_manager_lock() != 0) {
		return -1;
	}
	video_osd_compositor_slot_t *slot = &compositor[entry->stream_id];
	if (slot->context) {
		if (slot->target.codec != entry->codec || entry->region.x + entry->region.width > slot->target.frame_width ||
			entry->region.y + entry->region.height > slot->target.frame_height) {
			goto exit;
		}
		entry->version = video_osd_next_version_locked(1);
		entry->next = video_osd_protected_list;
		video_osd_protected_list = entry;
		entry->registered = 1;
		result = slot->commit(slot->context);
		if (result) {
			goto remove_entry;
		}
		if (revision) {
			video_osd_stream_status_t status;
			video_osd_stream_status_locked(slot->context, &status);
			*revision = status.requested_revision;
		}
		goto exit;
	}
	for (protected_entry = video_osd_protected_list; protected_entry; protected_entry = protected_entry->next) {
		if (protected_entry->stream_id != entry->stream_id) {
			continue;
		}
		if (protected_entry->codec != entry->codec ||
			video_osd_region_conflicts(&protected_entry->region, &entry->region, entry->codec)) {
			RTK_LOGE(TAG, "protected OSD conflicts on stream %d\r\n", entry->stream_id);
			goto exit;
		}
	}

	entry->next = video_osd_protected_list;
	video_osd_protected_list = entry;
	entry->registered = 1;
	for (nn_group = video_osd_nn_group_list; nn_group; nn_group = nn_group->next) {
		int conflict = 0;

		if (nn_group->stream_id != entry->stream_id || nn_group->codec != entry->codec) {
			continue;
		}
		for (int i = 0; i < nn_group->layer_count; i++) {
			if (nn_group->active[i] && video_osd_region_conflicts(&nn_group->region[i], &entry->region, entry->codec)) {
				conflict = 1;
				break;
			}
		}
		if (conflict && video_osd_disable_nn_group_locked(nn_group) != 0) {
			goto remove_entry;
		}
	}

	if (video_set_osd_bitmap(entry->stream_id, entry->osd_idx, entry->format,
							 entry->region.x, entry->region.y, entry->region.width, entry->region.height,
							 entry->alpha, entry->y, entry->u, entry->v, entry->buffer) != 0) {
		goto remove_entry;
	}
	result = 0;
	goto exit;

remove_entry:
	video_osd_protected_list = entry->next;
	entry->next = NULL;
	entry->registered = 0;
exit:
	video_osd_manager_unlock();
	return result;
}

static int video_osd_protected_update(video_osd_protected_region_t *entry, uint8_t *buffer)
{
	int result;

	if (!entry || !entry->registered || !buffer || video_osd_manager_lock() != 0) {
		return -1;
	}
	video_osd_compositor_slot_t *slot = &compositor[entry->stream_id];
	if (slot->context) {
		uint8_t *old = entry->buffer;
		uint64_t version = entry->version;
		entry->version = video_osd_next_version_locked(1);
		entry->buffer = buffer;
		result = slot->commit(slot->context);
		if (result) {
			entry->buffer = old;
			entry->version = version;
		}
		video_osd_manager_unlock();
		return result;
	}
	result = video_set_osd_bitmap(entry->stream_id, entry->osd_idx, entry->format,
								  entry->region.x, entry->region.y, entry->region.width, entry->region.height,
								  entry->alpha, entry->y, entry->u, entry->v, buffer);
	if (result == 0) {
		entry->buffer = buffer;
	}
	video_osd_manager_unlock();
	return result;
}

/* A synchronous removal keeps the client registered for lifetime/accounting,
 * but omits it from rendering. Only the completion ACK permits unlink/free. */
static int video_osd_protected_remove_sync_once(video_osd_protected_region_t *entry)
{
	if (video_osd_manager_lock()) {
		return VIDEO_OSD_EBUSY;
	}
	video_osd_compositor_slot_t *slot = &compositor[entry->stream_id];
	int result = VIDEO_OSD_EINVAL;
	if (!slot->context) {
		goto done;
	}
	video_osd_stream_status_t status;
	video_osd_stream_status_locked(slot->context, &status);
	if (entry->removing && !status.last_error && status.applied_revision >= entry->remove_revision) {
		video_osd_protected_region_t **cursor;
		for (cursor = &video_osd_protected_list; *cursor && *cursor != entry; cursor = &(*cursor)->next) {}
		if (!*cursor) {
			goto done;
		}
		*cursor = entry->next;
		entry->registered = 0;
		entry->next = NULL;
		result = 0;
		goto done;
	}
	if (status.pending) {
		result = VIDEO_OSD_EBUSY;
		goto done;
	}
	int was_removing = entry->removing;
	entry->removing = 1;
	result = slot->commit(slot->context);
	if (result) {
		entry->removing = was_removing;
		goto done;
	}
	video_osd_stream_status_locked(slot->context, &status);
	entry->remove_revision = status.requested_revision;
	/* Re-enter once to unlink, even when VP completed during submission. */
	result = !status.pending && status.last_error ? status.last_error : VIDEO_OSD_EBUSY;
done:
	video_osd_manager_unlock();
	return result;
}

static int video_osd_protected_remove(video_osd_protected_region_t *entry)
{
	video_osd_protected_region_t **cursor;
	int result;

	if (!entry || !entry->registered) {
		return 0;
	}
	if (video_osd_manager_lock() != 0) {
		return -1;
	}
	video_osd_compositor_slot_t *slot = &compositor[entry->stream_id];
	if (slot->context) {
		for (cursor = &video_osd_protected_list; *cursor && *cursor != entry; cursor = &(*cursor)->next) {}
		if (!*cursor) {
			video_osd_manager_unlock();
			return -1;
		}
		*cursor = entry->next;
		result = slot->commit(slot->context);
		if (result) {
			*cursor = entry;
		} else {
			entry->registered = 0;
			entry->next = NULL;
		}
		video_osd_manager_unlock();
		return result;
	}
	result = video_disable_osd(entry->stream_id, entry->osd_idx);
	if (result == 0) {
		for (cursor = &video_osd_protected_list; *cursor; cursor = &(*cursor)->next) {
			if (*cursor == entry) {
				*cursor = entry->next;
				break;
			}
		}
		entry->registered = 0;
		entry->next = NULL;
	}
	video_osd_manager_unlock();
	return result;
}

video_osd_nn_group_t *video_osd_nn_group_register(int stream_id, uint32_t codec, const int *osd_idx, int layer_count)
{
	video_osd_nn_group_t *group;
	video_osd_protected_region_t *entry;

	if (stream_id < 0 || stream_id >= VIDEO_ENC_VALID_CH || !video_osd_codec_valid(codec) || !osd_idx ||
		layer_count <= 0 || layer_count > VIDEO_OSD_NN_MAX_LAYERS) {
		return NULL;
	}
	for (int i = 0; i < layer_count; i++) {
		if (osd_idx[i] < 0 || osd_idx[i] >= VIDEO_OVERLAY_INDEX_COUNT) {
			return NULL;
		}
	}
	group = (video_osd_nn_group_t *)calloc(1, sizeof(*group));
	if (!group) {
		return NULL;
	}
	group->stream_id = stream_id;
	group->codec = codec;
	group->layer_count = layer_count;
	memcpy(group->osd_idx, osd_idx, (size_t)layer_count * sizeof(osd_idx[0]));

	if (video_osd_manager_lock() != 0) {
		free(group);
		return NULL;
	}
	if (compositor[stream_id].context && compositor[stream_id].target.codec != codec) {
		video_osd_manager_unlock();
		free(group);
		return NULL;
	}
	for (entry = video_osd_protected_list; entry; entry = entry->next) {
		if (entry->stream_id == stream_id && entry->codec != codec) {
			video_osd_manager_unlock();
			free(group);
			return NULL;
		}
	}
	group->next = video_osd_nn_group_list;
	video_osd_nn_group_list = group;
	video_osd_manager_unlock();
	return group;
}

int video_osd_nn_group_submit(video_osd_nn_group_t *group, const video_osd_layer_update_t *update, int layer_count)
{
	video_osd_protected_region_t *entry;
	int conflict = 0;
	int result = 0;

	if (!group || !update || layer_count != group->layer_count || video_osd_manager_lock() != 0) {
		return -1;
	}
	video_osd_compositor_slot_t *slot = &compositor[group->stream_id];
	if (slot->context) {
		video_osd_layer_update_t candidate[VIDEO_OSD_NN_MAX_LAYERS] = {0};
		video_osd_layer_update_t old[VIDEO_OSD_NN_MAX_LAYERS];
		for (int i = 0; i < layer_count; i++) {
			candidate[i] = update[i];
			candidate[i].buffer = NULL;
			if (!update[i].enable) {
				continue;
			}
			if (update[i].osd_idx != group->osd_idx[i] || update[i].format != VIDEO_OSD_FORMAT_BITMAP ||
				!update[i].buffer || !update[i].region.width || !update[i].region.height || (update[i].region.width & 7U) ||
				update[i].region.width > slot->target.frame_width || update[i].region.height > slot->target.frame_height ||
				update[i].region.x > slot->target.frame_width - update[i].region.width ||
				update[i].region.y > slot->target.frame_height - update[i].region.height) {
				result = -1;
				break;
			}
			size_t bytes = (size_t)update[i].region.width / 8 * update[i].region.height;
			candidate[i].buffer = malloc(bytes);
			if (!candidate[i].buffer) {
				result = -1;
				break;
			}
			size_t row_bytes = update[i].region.width / 8;
			size_t stride = update[i].stride_bytes ? update[i].stride_bytes : row_bytes;
			if (stride < row_bytes) {
				result = -1;
				break;
			}
			for (uint32_t row = 0; row < update[i].region.height; row++) {
				memcpy(candidate[i].buffer + row * row_bytes, update[i].buffer + row * stride, row_bytes);
			}
			candidate[i].stride_bytes = 0;
		}
		if (!result) {
			uint64_t old_version = group->version;
			group->version = video_osd_next_version_locked(VIDEO_OSD_NN_MAX_LAYERS);
			memcpy(old, group->shadow, sizeof(old));
			for (int i = 0; i < layer_count; i++) {
				if (old[i].buffer) {
					group->transient_bytes += (size_t)(old[i].region.width / 8) * old[i].region.height;
				}
			}
			memcpy(group->shadow, candidate, sizeof(candidate));
			result = slot->commit(slot->context);
			if (result) {
				memcpy(group->shadow, old, sizeof(old));
				group->version = old_version;
			} else {
				for (int i = 0; i < layer_count; i++) {
					free(old[i].buffer);
					group->active[i] = candidate[i].enable;
				}
			}
		}
		group->transient_bytes = 0;
		if (result) {
			for (int i = 0; i < layer_count; i++) {
				free(candidate[i].buffer);
			}
		}
		goto exit;
	}
	for (int i = 0; i < layer_count; i++) {
		if (update[i].osd_idx != group->osd_idx[i]) {
			result = -1;
			goto exit;
		}
		if (update[i].enable && (!update[i].buffer || update[i].format != VIDEO_OSD_FORMAT_BITMAP ||
								 update[i].region.width == 0 || update[i].region.height == 0 || (update[i].region.width & 7U))) {
			video_osd_disable_nn_group_locked(group);
			result = -1;
			goto exit;
		}
		if (!update[i].enable) {
			continue;
		}
		for (entry = video_osd_protected_list; entry; entry = entry->next) {
			if (entry->stream_id == group->stream_id && entry->codec == group->codec &&
				video_osd_region_conflicts(&update[i].region, &entry->region, group->codec)) {
				conflict = 1;
				break;
			}
		}
		if (conflict) {
			break;
		}
	}

	if (conflict) {
		int has_active = 0;
		for (int i = 0; i < group->layer_count; i++) {
			has_active |= group->active[i];
		}
		if ((!group->suppressed || has_active) && video_osd_disable_nn_group_locked(group) != 0) {
			result = -1;
		} else {
			group->suppressed = 1;
			result = VIDEO_OSD_NN_SUPPRESSED;
		}
		goto exit;
	}

	for (int i = 0; i < layer_count; i++) {
		if (update[i].enable) {
			if (video_set_osd_bitmap(group->stream_id, update[i].osd_idx, update[i].format,
									 update[i].region.x, update[i].region.y, update[i].region.width, update[i].region.height,
									 update[i].alpha, update[i].y, update[i].u, update[i].v, update[i].buffer) != 0) {
				result = -1;
				break;
			}
			group->active[i] = 1;
			group->region[i] = update[i].region;
		} else if (group->active[i]) {
			if (video_disable_osd(group->stream_id, update[i].osd_idx) != 0) {
				result = -1;
				break;
			}
			group->active[i] = 0;
		}
	}
	if (result != 0) {
		video_osd_disable_nn_group_locked(group);
		goto exit;
	}
	group->suppressed = 0;

exit:
	video_osd_manager_unlock();
	return result;
}

void video_osd_nn_group_hide(video_osd_nn_group_t *group)
{
	if (!group || video_osd_manager_lock() != 0) {
		return;
	}
	video_osd_disable_nn_group_locked(group);
	group->suppressed = 0;
	video_osd_manager_unlock();
}

int video_osd_nn_group_unregister(video_osd_nn_group_t *group)
{
	video_osd_nn_group_t **cursor;
	int has_active = 0;

	if (!group || video_osd_manager_lock() != 0) {
		return -1;
	}
	for (int i = 0; i < group->layer_count; i++) {
		has_active |= group->active[i];
	}
	if (has_active && video_osd_disable_nn_group_locked(group) != 0) {
		video_osd_manager_unlock();
		return -1;
	}
	for (cursor = &video_osd_nn_group_list; *cursor; cursor = &(*cursor)->next) {
		if (*cursor == group) {
			*cursor = group->next;
			break;
		}
	}
	video_osd_manager_unlock();
	for (int i = 0; i < group->layer_count; i++) {
		free(group->shadow[i].buffer);
	}
	free(group);
	return 0;
}

void video_osd_timestamp_config_default(video_osd_timestamp_config_t *config)
{
	if (!config) {
		return;
	}
	memset(config, 0, sizeof(*config));
	config->placement.position = VIDEO_OSD_POSITION_LOWER_RIGHT;
	config->placement.margin_x = 16;
	config->placement.margin_y = 16;
	config->time_zone = VIDEO_OSD_TIME_LOCAL;
	config->valid_after = VIDEO_OSD_TIMESTAMP_DEFAULT_VALID_TIME;
	config->max_chars = VIDEO_OSD_TIMESTAMP_DEFAULT_MAX_CHARS;
	config->font_scale = 3;
	config->alpha = 255;
	config->y = 255;
	config->u = 128;
	config->v = 128;
}

static int video_osd_timestamp_format(video_osd_timestamp_impl_t *impl, char *text)
{
	time_t now = time(NULL);
	struct tm time_info;

	if (now < 0) {
		now = 0;
	}
	/* time() stays pinned at the epoch on this platform until NTP sets it, so
	 * before sync fall back to system uptime to count up from 1970-01-01 00:00:00. */
	if ((uint64_t)now < impl->config.valid_after) {
		now = (time_t)(rtos_time_get_current_system_time_ms_64bit() / 1000ULL);
		if (!gmtime_r(&now, &time_info)) {
			return -1;
		}
	} else if (impl->config.time_zone == VIDEO_OSD_TIME_UTC) {
		if (!gmtime_r(&now, &time_info)) {
			return -1;
		}
	} else if (!localtime_r(&now, &time_info)) {
		return -1;
	}
	return strftime(text, (size_t)impl->config.max_chars + 1U, impl->format, &time_info) == 0 ? -1 : 0;
}

static int video_osd_timestamp_format_fits(const char *format, uint16_t max_chars)
{
	struct tm test_time = {
		.tm_sec = 59,
		.tm_min = 59,
		.tm_hour = 23,
		.tm_mday = 31,
		.tm_mon = 11,
		.tm_year = 124,
		.tm_wday = 2,
		.tm_yday = 365,
		.tm_isdst = -1,
	};
	char text[VIDEO_OSD_TIMESTAMP_MAX_CHARS + 1];

	return format[0] != '\0' && strftime(text, (size_t)max_chars + 1U, format, &test_time) != 0;
}

static void video_osd_timestamp_draw(video_osd_timestamp_impl_t *impl, uint8_t *buffer, const char *text)
{
	video_osd_font_canvas_t canvas;
	int text_width;
	int x = 0;

	memset(buffer, 0, impl->buffer_size);
	canvas.buffer = buffer;
	canvas.width = (int)impl->protected_region.region.width;
	canvas.height = (int)impl->protected_region.region.height;
	text_width = video_osd_font_string_width(text, impl->config.font_scale);
	if (impl->config.placement.position == VIDEO_OSD_POSITION_UPPER_RIGHT ||
		impl->config.placement.position == VIDEO_OSD_POSITION_LOWER_RIGHT) {
		x = canvas.width - text_width;
	}
	video_osd_font_draw_string(&canvas, x, 0, text, impl->config.font_scale);
}

static int video_osd_timestamp_refresh(video_osd_timestamp_impl_t *impl)
{
	char text[VIDEO_OSD_TIMESTAMP_MAX_CHARS + 1];
	int next_buffer;

	if (video_osd_timestamp_format(impl, text) != 0) {
		RTK_LOGE(TAG, "timestamp does not fit max_chars=%u\r\n", impl->config.max_chars);
		return -1;
	}
	if (strcmp(text, impl->last_text) == 0) {
		return 0;
	}
	next_buffer = impl->active_buffer ^ 1;
	video_osd_timestamp_draw(impl, impl->buffer[next_buffer], text);
	DCache_Clean((uint32_t)impl->buffer[next_buffer], impl->buffer_size);
	if (video_osd_protected_update(&impl->protected_region, impl->buffer[next_buffer]) != 0) {
		return -1;
	}
	impl->active_buffer = next_buffer;
	strcpy(impl->last_text, text);
	return 0;
}

static void video_osd_timestamp_task(void *param)
{
	video_osd_timestamp_impl_t *impl = (video_osd_timestamp_impl_t *)param;

	rtos_sema_take(impl->start, RTOS_MAX_DELAY);
	while (impl->running) {
		for (int elapsed = 0; elapsed < VIDEO_OSD_TIMESTAMP_TASK_PERIOD_MS && impl->running;
			 elapsed += VIDEO_OSD_TIMESTAMP_TASK_POLL_MS) {
			rtos_time_delay_ms(VIDEO_OSD_TIMESTAMP_TASK_POLL_MS);
		}
		if (impl->running) {
			video_osd_timestamp_refresh(impl);
		}
	}
	rtos_sema_give(impl->stopped);
	rtos_task_delete(NULL);
}

static int video_osd_timestamp_start_impl(video_osd_timestamp_t *ctx, const video_osd_timestamp_config_t *config,
		uint32_t *revision)
{
	video_osd_timestamp_impl_t *impl;
	video_osd_region_t region;
	const char *format;
	char text[VIDEO_OSD_TIMESTAMP_MAX_CHARS + 1];
	uint32_t width;
	uint32_t height;
	int result = VIDEO_OSD_ENOMEM;

	if (!ctx || ctx->priv || !config || !video_osd_target_valid(&config->target) ||
		config->max_chars == 0 || config->max_chars > VIDEO_OSD_TIMESTAMP_MAX_CHARS ||
		config->font_scale == 0 || (config->font_scale > 3 && config->font_scale != 5) || config->time_zone > VIDEO_OSD_TIME_UTC) {
		return -1;
	}
	format = config->format ? config->format : VIDEO_OSD_TIMESTAMP_FORMAT_YMD_DASH_24;
	if (strlen(format) >= VIDEO_OSD_TIMESTAMP_FORMAT_CAPACITY ||
		!video_osd_timestamp_format_fits(format, config->max_chars)) {
		return -1;
	}
	/* Reserve the worst-case character width, not this second's proportional
	 * text width. Protection bounds must remain fixed as the clock changes. */
	width = (uint32_t)config->max_chars * VIDEO_OSD_FONT_WIDTH * config->font_scale;
	height = VIDEO_OSD_FONT_HEIGHT * config->font_scale;
	if ((width & 7U) || (height & 7U) || video_osd_resolve_position(&config->target, &config->placement, width, height, &region) != 0) {
		return -1;
	}

	impl = (video_osd_timestamp_impl_t *)calloc(1, sizeof(*impl));
	if (!impl) {
		return VIDEO_OSD_ENOMEM;
	}
	impl->protected_region.osd_idx = -1;
	impl->config = *config;
	strcpy(impl->format, format);
	impl->config.format = impl->format;
	impl->buffer_size = (size_t)width * height / 8U;
	impl->protected_region.storage_bytes = sizeof(*impl) + 2 * (impl->buffer_size + CACHE_LINE_SIZE - 1U);
	for (int i = 0; i < 2; i++) {
		impl->buffer[i] = video_osd_aligned_alloc(impl->buffer_size, &impl->buffer_raw[i]);
		if (!impl->buffer[i]) {
			goto cleanup;
		}
	}
	rtos_sema_create_binary(&impl->stopped);
	rtos_sema_create_binary(&impl->start);
	if (!impl->stopped || !impl->start) {
		goto cleanup;
	}
	impl->protected_region.stream_id = config->target.stream_id;
	impl->protected_region.codec = config->target.codec;
	impl->protected_region.format = VIDEO_OSD_FORMAT_BITMAP;
	impl->protected_region.region = region;
	impl->protected_region.alpha = config->alpha;
	impl->protected_region.y = config->y;
	impl->protected_region.u = config->u;
	impl->protected_region.v = config->v;
	impl->protected_region.osd_idx = video_osd_client_alloc_index(config->target.stream_id);
	if (impl->protected_region.osd_idx < 0) {
		result = VIDEO_OSD_ENOSPC;
		goto cleanup;
	}
	if (video_osd_timestamp_format(impl, text) != 0) {
		result = VIDEO_OSD_EINVAL;
		goto cleanup;
	}
	video_osd_timestamp_draw(impl, impl->buffer[0], text);
	DCache_Clean((uint32_t)impl->buffer[0], impl->buffer_size);
	impl->protected_region.buffer = impl->buffer[0];
	if (rtos_task_create(&impl->task, "video_osd_time", video_osd_timestamp_task, impl,
						 VIDEO_OSD_TIMESTAMP_TASK_STACK_SIZE, 1) != RTK_SUCCESS) {
		goto cleanup;
	}
	result = video_osd_protected_add(&impl->protected_region, revision);
	if (result != 0) {
		rtos_sema_give(impl->start);
		rtos_sema_take(impl->stopped, RTOS_MAX_DELAY);
		impl->task = NULL;
		goto cleanup;
	}
	impl->active_buffer = 0;
	strcpy(impl->last_text, text);
	impl->running = 1;
	ctx->priv = impl;
	rtos_sema_give(impl->start);
	return 0;

cleanup:
	if (impl->protected_region.osd_idx >= 0) {
		video_osd_client_free_index(config->target.stream_id, impl->protected_region.osd_idx);
	}
	if (impl->stopped) {
		rtos_sema_delete(impl->stopped);
	}
	if (impl->start) {
		rtos_sema_delete(impl->start);
	}
	for (int i = 0; i < 2; i++) {
		free(impl->buffer_raw[i]);
	}
	free(impl);
	return result;
}

int video_osd_timestamp_start(video_osd_timestamp_t *ctx, const video_osd_timestamp_config_t *config)
{
	/* Preserve the legacy 0/-1 return contract. */
	return video_osd_timestamp_start_impl(ctx, config, NULL) == 0 ? 0 : -1;
}

static void video_osd_timestamp_free(video_osd_timestamp_t *ctx)
{
	video_osd_timestamp_impl_t *impl = ctx->priv;
	video_osd_client_free_index(impl->protected_region.stream_id, impl->protected_region.osd_idx);
	rtos_sema_delete(impl->start);
	rtos_sema_delete(impl->stopped);
	for (int i = 0; i < 2; i++) {
		free(impl->buffer_raw[i]);
	}
	free(impl);
	ctx->priv = NULL;
}

void video_osd_timestamp_stop(video_osd_timestamp_t *ctx)
{
	video_osd_timestamp_impl_t *impl;

	if (!ctx || !ctx->priv) {
		return;
	}
	impl = (video_osd_timestamp_impl_t *)ctx->priv;
	impl->running = 0;
	if (impl->task) {
		rtos_sema_take(impl->stopped, RTOS_MAX_DELAY);
		impl->task = NULL;
	}
	if (video_osd_protected_remove(&impl->protected_region) != 0) {
		RTK_LOGE(TAG, "failed to disable timestamp OSD\r\n");
		return;
	}
	/* The VP acknowledges queue insertion before the encoder consumes the
	 * disable. Keep the old DMA buffer alive for the in-flight V2 frames. */
	rtos_time_delay_ms(VIDEO_OSD_BUFFER_RELEASE_DELAY_MS);
	video_osd_timestamp_free(ctx);
}

static int timestamp_stop_once(void *context)
{
	video_osd_timestamp_t *ctx = context;
	if (!ctx) {
		return VIDEO_OSD_EINVAL;
	}
	if (!ctx->priv) {
		return 0;
	}
	video_osd_timestamp_impl_t *impl = ctx->priv;
	if (!video_osd_client_is_managed(impl->protected_region.stream_id)) {
		return VIDEO_OSD_EINVAL;
	}
	impl->running = 0;
	if (impl->task) {
		if (rtos_sema_take(impl->stopped, 0) != RTK_SUCCESS) {
			return VIDEO_OSD_EBUSY;
		}
		impl->task = NULL;
	}
	int result = video_osd_protected_remove_sync_once(&impl->protected_region);
	if (!result) {
		video_osd_timestamp_free(ctx);
	}
	return result;
}

int video_osd_timestamp_stop_sync(video_osd_timestamp_t *ctx, uint32_t timeout_ms)
{
	return video_osd_sync_retry(timestamp_stop_once, ctx, timeout_ms);
}

void video_osd_logo_config_default(video_osd_logo_config_t *config)
{
	if (!config) {
		return;
	}
	memset(config, 0, sizeof(*config));
	config->placement.position = VIDEO_OSD_POSITION_UPPER_LEFT;
	config->placement.margin_x = 16;
	config->placement.margin_y = 16;
}

static int video_osd_logo_show_impl(video_osd_logo_t *ctx, const video_osd_logo_config_t *config, uint32_t *revision)
{
	video_osd_logo_impl_t *impl;
	video_osd_region_t region;
	size_t source_stride;
	size_t destination_stride;

	if (!ctx || ctx->priv || !config || !config->argb8888 || !video_osd_target_valid(&config->target) ||
		config->width == 0 || config->height == 0 || config->width > SIZE_MAX / sizeof(uint32_t)) {
		return -1;
	}
	destination_stride = (size_t)config->width * sizeof(uint32_t);
	source_stride = config->stride_bytes ? config->stride_bytes : destination_stride;
	if (source_stride < destination_stride || (source_stride & (sizeof(uint32_t) - 1U)) ||
		config->height > SIZE_MAX / destination_stride || config->height > SIZE_MAX / source_stride ||
		video_osd_resolve_position(&config->target, &config->placement, config->width, config->height, &region) != 0) {
		return -1;
	}

	impl = (video_osd_logo_impl_t *)calloc(1, sizeof(*impl));
	if (!impl) {
		return VIDEO_OSD_ENOMEM;
	}
	impl->protected_region.osd_idx = -1;
	impl->buffer_size = destination_stride * config->height;
	impl->protected_region.storage_bytes = sizeof(*impl) + impl->buffer_size + CACHE_LINE_SIZE - 1U;
	impl->buffer = video_osd_aligned_alloc(impl->buffer_size, &impl->buffer_raw);
	if (!impl->buffer) {
		free(impl);
		return VIDEO_OSD_ENOMEM;
	}
	for (uint32_t row = 0; row < config->height; row++) {
		memcpy(impl->buffer + row * destination_stride,
			   (const uint8_t *)config->argb8888 + row * source_stride, destination_stride);
	}
	DCache_Clean((uint32_t)impl->buffer, impl->buffer_size);
	impl->protected_region.stream_id = config->target.stream_id;
	impl->protected_region.codec = config->target.codec;
	impl->protected_region.format = VIDEO_OSD_FORMAT_ARGB8888;
	impl->protected_region.region = region;
	impl->protected_region.buffer = impl->buffer;
	impl->protected_region.osd_idx = video_osd_client_alloc_index(config->target.stream_id);
	int result = impl->protected_region.osd_idx < 0 ? VIDEO_OSD_ENOSPC :
				 video_osd_protected_add(&impl->protected_region, revision);
	if (result != 0) {
		if (impl->protected_region.osd_idx >= 0) {
			video_osd_client_free_index(config->target.stream_id, impl->protected_region.osd_idx);
		}
		free(impl->buffer_raw);
		free(impl);
		return result;
	}
	ctx->priv = impl;
	return 0;
}

int video_osd_logo_show(video_osd_logo_t *ctx, const video_osd_logo_config_t *config)
{
	/* Preserve the legacy 0/-1 return contract. */
	return video_osd_logo_show_impl(ctx, config, NULL) == 0 ? 0 : -1;
}

static void video_osd_logo_free(video_osd_logo_t *ctx)
{
	video_osd_logo_impl_t *impl = ctx->priv;
	video_osd_client_free_index(impl->protected_region.stream_id, impl->protected_region.osd_idx);
	free(impl->buffer_raw);
	free(impl);
	ctx->priv = NULL;
}

void video_osd_logo_hide(video_osd_logo_t *ctx)
{
	video_osd_logo_impl_t *impl;

	if (!ctx || !ctx->priv) {
		return;
	}
	impl = (video_osd_logo_impl_t *)ctx->priv;
	if (video_osd_protected_remove(&impl->protected_region) != 0) {
		RTK_LOGE(TAG, "failed to disable logo OSD\r\n");
		return;
	}
	rtos_time_delay_ms(VIDEO_OSD_BUFFER_RELEASE_DELAY_MS);
	video_osd_logo_free(ctx);
}

static int logo_hide_once(void *context)
{
	video_osd_logo_t *ctx = context;
	if (!ctx) {
		return VIDEO_OSD_EINVAL;
	}
	if (!ctx->priv) {
		return 0;
	}
	video_osd_logo_impl_t *impl = ctx->priv;
	int result = video_osd_protected_remove_sync_once(&impl->protected_region);
	if (!result) {
		video_osd_logo_free(ctx);
	}
	return result;
}

int video_osd_logo_hide_sync(video_osd_logo_t *ctx, uint32_t timeout_ms)
{
	return video_osd_sync_retry(logo_hide_once, ctx, timeout_ms);
}

/* Retry submission only before acceptance, then wait for the revision captured
 * under the manager lock. Both phases share one timeout; no extra task/queue. */
typedef struct {
	video_osd_stream_t stream;
	video_osd_timestamp_t *clock;
	const video_osd_timestamp_config_t *clock_config;
	video_osd_logo_t *logo;
	const video_osd_logo_config_t *logo_config;
	uint32_t revision;
	int accepted;
} video_osd_start_sync_t;

static int start_sync_once(void *context)
{
	video_osd_start_sync_t *start = context;
	if (!start->accepted) {
		const video_osd_target_t *target = start->clock ? &start->clock_config->target : &start->logo_config->target;
		if (video_osd_manager_lock()) {
			return VIDEO_OSD_EBUSY;
		}
		start->stream.priv = video_osd_stream_lookup_locked(target);
		/* Avoid allocating sources/tasks while an earlier scene is pending.
		 * Background producers may still race after unlock; commit handles that. */
		video_osd_stream_status_t before;
		if (start->stream.priv) {
			video_osd_stream_status_locked(start->stream.priv, &before);
		}
		video_osd_manager_unlock();
		if (!start->stream.priv) {
			return VIDEO_OSD_EINVAL;
		}
		if (before.pending) {
			return VIDEO_OSD_EBUSY;
		}
		int result = start->clock ?
					 video_osd_timestamp_start_impl(start->clock, start->clock_config, &start->revision) :
					 video_osd_logo_show_impl(start->logo, start->logo_config, &start->revision);
		if (result) {
			return result;
		}
		start->accepted = 1;
	}
	video_osd_stream_status_t status;
	int result = video_osd_stream_get_status(&start->stream, &status);
	return result ? result : video_osd_sync_check_applied(&status, start->revision);
}

int video_osd_timestamp_start_sync(video_osd_timestamp_t *ctx, const video_osd_timestamp_config_t *config, uint32_t timeout_ms)
{
	if (!ctx || ctx->priv || !config || !video_osd_target_valid(&config->target)) {
		return VIDEO_OSD_EINVAL;
	}
	video_osd_start_sync_t start = {.clock = ctx, .clock_config = config};
	return video_osd_sync_retry(start_sync_once, &start, timeout_ms);
}

int video_osd_logo_show_sync(video_osd_logo_t *ctx, const video_osd_logo_config_t *config, uint32_t timeout_ms)
{
	if (!ctx || ctx->priv || !config || !video_osd_target_valid(&config->target)) {
		return VIDEO_OSD_EINVAL;
	}
	video_osd_start_sync_t start = {.logo = ctx, .logo_config = config};
	return video_osd_sync_retry(start_sync_once, &start, timeout_ms);
}
