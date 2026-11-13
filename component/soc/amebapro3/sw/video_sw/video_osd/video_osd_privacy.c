#include <stdlib.h>
#include <string.h>
#include "video_api.h"
#include "video_osd_privacy.h"
#include "video_osd_polygon.h"

typedef struct {
	video_osd_target_t target;
	video_osd_stream_t stream;
	video_osd_privacy_scene_t scene;
	video_osd_privacy_polygon_t mapped[VIDEO_OSD_PRIVACY_MAX_POLYGONS];
	video_osd_region_t bounds[VIDEO_OSD_PRIVACY_MAX_POLYGONS];
	uint64_t version;
	uint32_t remove_revision;
	int owns_stream;
	int removing;
} privacy_t;

static privacy_t *privacy[VIDEO_ENC_VALID_CH];

int video_osd_privacy_get_caps(const video_osd_target_t *target, video_osd_privacy_caps_t *caps)
{
	if (!target || !caps || target->stream_id < 0 || target->stream_id >= VIDEO_ENC_VALID_CH ||
		(target->codec != VIDEO_H264 && target->codec != VIDEO_HEVC) ||
		!target->frame_width || !target->frame_height || target->frame_width > 8192 || target->frame_height > 8192 ||
		(target->frame_width & 1U) || (target->frame_height & 1U)) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	*caps = (video_osd_privacy_caps_t) {
		.version = VIDEO_OSD_PRIVACY_VERSION,
		.max_polygons = VIDEO_OSD_PRIVACY_MAX_POLYGONS,
		.max_vertices = VIDEO_OSD_PRIVACY_MAX_VERTICES,
		.normalized_max = VIDEO_OSD_PRIVACY_NORMALIZED_MAX,
		.concave_supported = 1,
		.reference_pixels_supported = 1,
		/* Compatibility estimate only: worst-case two full ARGB images.
		 * No image is allocated by init; actual usage is reported in status. */
		.canvas_bytes = (size_t)target->frame_width * target->frame_height * 8U,
	};
	return 0;
}

void video_osd_privacy_scene_default(video_osd_privacy_scene_t *scene)
{
	if (scene) {
		memset(scene, 0, sizeof(*scene));
		scene->version = VIDEO_OSD_PRIVACY_VERSION;
		scene->enabled = 1;
	}
}

int video_osd_privacy_present_locked(int stream_id)
{
	return privacy[stream_id] != NULL;
}

int video_osd_privacy_collect_locked(int stream_id, video_osd_render_item_t *items, int capacity, size_t *bytes)
{
	privacy_t *p = privacy[stream_id];
	if (!p) {
		return 0;
	}
	*bytes += sizeof(*p);
	if (p->removing || !p->scene.enabled) {
		return 0;
	}
	uint32_t rgb = p->scene.fill_rgb;
	int red = (rgb >> 16) & 255, green = (rgb >> 8) & 255, blue = rgb & 255;
	int count = 0;
	for (uint32_t i = 0; i < p->scene.polygon_count; i++) {
		if (!p->mapped[i].enabled) {
			continue;
		}
		if (count == capacity) {
			return VIDEO_OSD_PRIVACY_ENOSPC;
		}
		items[count].layer = (video_osd_layer_update_t) {
			.enable = 1, .format = VIDEO_OSD_FORMAT_BITMAP, .region = p->bounds[i], .alpha = 255,
			.y = (77 * red + 150 * green + 29 * blue) >> 8,
			.u = 128 + ((-43 * red - 85 * green + 128 * blue) >> 8),
			.v = 128 + ((128 * red - 107 * green - 21 * blue) >> 8),
		};
		items[count].polygon = &p->mapped[i];
		items[count].rgb = rgb;
		items[count++].version = p->version + i;
	}
	return count;
}

int video_osd_privacy_conflicts_locked(int stream_id, const video_osd_region_t *region)
{
	privacy_t *p = privacy[stream_id];
	if (!p || p->removing || !p->scene.enabled) {
		return 0;
	}
	for (uint32_t i = 0; i < p->scene.polygon_count; i++) {
		if (p->mapped[i].enabled && video_osd_region_conflicts(region, &p->bounds[i], p->target.codec)) {
			return 1;
		}
	}
	return 0;
}

int video_osd_privacy_init(video_osd_privacy_t *ctx, const video_osd_privacy_config_t *config)
{
	video_osd_privacy_caps_t caps;
	if (!ctx || ctx->priv || !config || video_osd_privacy_get_caps(&config->target, &caps)) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	privacy_t *p = calloc(1, sizeof(*p));
	if (!p) {
		return VIDEO_OSD_PRIVACY_ENOMEM;
	}
	p->target = config->target;
	video_osd_privacy_scene_default(&p->scene);
	if (video_osd_manager_lock()) {
		free(p);
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	if (privacy[p->target.stream_id]) {
		video_osd_manager_unlock();
		free(p);
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	p->stream.priv = video_osd_stream_lookup_locked(&p->target);
	video_osd_manager_unlock();
	if (!p->stream.priv) {
		video_osd_stream_config_t stream_config = {p->target, config->memory_budget};
		int result = video_osd_stream_init(&p->stream, &stream_config);
		if (result) {
			free(p);
			return result;
		}
		p->owns_stream = 1;
	}
	if (video_osd_manager_lock()) {
		if (p->owns_stream) {
			video_osd_stream_deinit(&p->stream);
		}
		free(p);
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	privacy[p->target.stream_id] = p;
	video_osd_stream_status_t status;
	video_osd_stream_status_locked(p->stream.priv, &status);
	if (status.memory_budget && status.allocated_bytes + status.source_bytes > status.memory_budget) {
		privacy[p->target.stream_id] = NULL;
		video_osd_manager_unlock();
		if (p->owns_stream) {
			video_osd_stream_deinit(&p->stream);
		}
		free(p);
		return VIDEO_OSD_PRIVACY_ENOMEM;
	}
	ctx->priv = p;
	video_osd_manager_unlock();
	return 0;
}

int video_osd_privacy_apply(video_osd_privacy_t *ctx, const video_osd_privacy_scene_t *scene, uint32_t *revision)
{
	if (!ctx || !ctx->priv || !scene) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	if (video_osd_manager_lock()) {
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	privacy_t *p = ctx->priv;
	int result = VIDEO_OSD_PRIVACY_EBUSY;
	if (!p->removing) {
		video_osd_privacy_scene_t old = p->scene;
		uint64_t old_version = p->version;
		result = video_osd_polygon_map(scene, p->target.frame_width, p->target.frame_height, p->mapped, p->bounds);
		if (!result) {
			p->scene = *scene;
			p->version = video_osd_next_version_locked(VIDEO_OSD_PRIVACY_MAX_POLYGONS);
			result = video_osd_stream_commit_locked(p->stream.priv);
		}
		if (result) {
			p->scene = old;
			p->version = old_version;
			video_osd_polygon_map(&old, p->target.frame_width, p->target.frame_height, p->mapped, p->bounds);
		} else if (revision) {
			video_osd_stream_status_t status;
			video_osd_stream_status_locked(p->stream.priv, &status);
			*revision = status.requested_revision;
		}
	}
	video_osd_manager_unlock();
	return result;
}

int video_osd_privacy_get(video_osd_privacy_t *ctx, video_osd_privacy_scene_t *scene)
{
	if (!ctx || !ctx->priv || !scene) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	if (video_osd_manager_lock()) {
		return VIDEO_OSD_PRIVACY_EBUSY;
	}
	*scene = ((privacy_t *)ctx->priv)->scene;
	video_osd_manager_unlock();
	return 0;
}

int video_osd_privacy_get_status(video_osd_privacy_t *ctx, video_osd_privacy_status_t *status)
{
	if (!ctx || !ctx->priv || !status) {
		return VIDEO_OSD_PRIVACY_EINVAL;
	}
	return video_osd_stream_get_status(&((privacy_t *)ctx->priv)->stream, status);
}

int video_osd_privacy_deinit(video_osd_privacy_t *ctx)
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
	privacy_t *p = ctx->priv;
	video_osd_stream_status_t status;
	video_osd_stream_status_locked(p->stream.priv, &status);
	int result = VIDEO_OSD_PRIVACY_EBUSY;
	if (status.pending) {
		goto done;
	}
	if (p->version && (!p->removing || status.last_error || status.applied_revision < p->remove_revision)) {
		p->removing = 1;
		result = video_osd_stream_commit_locked(p->stream.priv);
		if (result) {
			p->removing = 0;
			goto done;
		}
		video_osd_stream_status_locked(p->stream.priv, &status);
		p->remove_revision = status.requested_revision;
		result = VIDEO_OSD_PRIVACY_EBUSY;
		goto done;
	}
	privacy[p->target.stream_id] = NULL;
	video_osd_manager_unlock();
	if (p->owns_stream) {
		result = video_osd_stream_deinit(&p->stream);
		if (result) {
			/* Keep the owner context available for another shutdown attempt. */
			if (!video_osd_manager_lock()) {
				privacy[p->target.stream_id] = p;
				video_osd_manager_unlock();
			}
			return result;
		}
	}
	free(p);
	ctx->priv = NULL;
	return 0;
done:
	video_osd_manager_unlock();
	return result;
}

typedef struct {
	video_osd_privacy_t *ctx;
	const video_osd_privacy_scene_t *scene;
	uint32_t revision;
	uint32_t *out_revision;
	int accepted;
} video_osd_privacy_apply_sync_t;

static int privacy_apply_once(void *context)
{
	video_osd_privacy_apply_sync_t *update = context;
	if (!update->accepted) {
		int result = video_osd_privacy_apply(update->ctx, update->scene, &update->revision);
		if (result) {
			return result;
		}
		update->accepted = 1;
		if (update->out_revision) {
			*update->out_revision = update->revision;
		}
	}
	video_osd_privacy_status_t status;
	int result = video_osd_privacy_get_status(update->ctx, &status);
	return result ? result : video_osd_sync_check_applied(&status, update->revision);
}

int video_osd_privacy_apply_sync(video_osd_privacy_t *ctx, const video_osd_privacy_scene_t *scene,
								 uint32_t *revision, uint32_t timeout_ms)
{
	video_osd_privacy_apply_sync_t update = {
		.ctx = ctx, .scene = scene, .out_revision = revision,
	};
	if (revision) {
		*revision = 0;
	}
	return video_osd_sync_retry(privacy_apply_once, &update, timeout_ms);
}

static int privacy_deinit_once(void *context)
{
	return video_osd_privacy_deinit(context);
}

int video_osd_privacy_deinit_sync(video_osd_privacy_t *ctx, uint32_t timeout_ms)
{
	return video_osd_sync_retry(privacy_deinit_once, ctx, timeout_ms);
}
