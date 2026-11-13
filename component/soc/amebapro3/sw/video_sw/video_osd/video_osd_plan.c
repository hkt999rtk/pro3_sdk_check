#include <string.h>
#include "video_osd_plan.h"
#include "video_osd_polygon.h"

size_t video_osd_plan_region_bytes(const video_osd_plan_region_t *r)
{
	return (size_t)r->region.width * r->region.height * (r->format == VIDEO_OSD_FORMAT_ARGB8888 ? 4U : 1U) /
		   (r->format == VIDEO_OSD_FORMAT_BITMAP ? 8U : 1U);
}

static void align_region(const video_osd_target_t *target, video_osd_plan_region_t *r)
{
	uint32_t right = (r->region.x + r->region.width + 1U) & ~1U;
	uint32_t bottom = (r->region.y + r->region.height + 1U) & ~1U;
	r->region.x &= ~1U;
	r->region.y &= ~1U;
	r->region.width = right - r->region.x;
	r->region.height = bottom - r->region.y;
	if (r->format == VIDEO_OSD_FORMAT_BITMAP) {
		uint32_t width = (r->region.width + 7U) & ~7U;
		if (width > target->frame_width) {
			r->format = VIDEO_OSD_FORMAT_ARGB8888;
		} else {
			r->region.width = width;
			if (r->region.x + width > target->frame_width) {
				r->region.x = target->frame_width - width;
			}
		}
	}
}

static video_osd_plan_region_t combine(const video_osd_target_t *target,
									   const video_osd_plan_region_t *a, const video_osd_plan_region_t *b)
{
	video_osd_plan_region_t r = *a;
	uint32_t right = a->region.x + a->region.width;
	uint32_t bottom = a->region.y + a->region.height;
	if (right < b->region.x + b->region.width) {
		right = b->region.x + b->region.width;
	}
	if (bottom < b->region.y + b->region.height) {
		bottom = b->region.y + b->region.height;
	}
	if (r.region.x > b->region.x) {
		r.region.x = b->region.x;
	}
	if (r.region.y > b->region.y) {
		r.region.y = b->region.y;
	}
	r.region.width = right - r.region.x;
	r.region.height = bottom - r.region.y;
	r.members |= b->members;
	/* OR is equivalent to source-over only for the same opaque bitmap color. */
	if (a->format != VIDEO_OSD_FORMAT_BITMAP || b->format != VIDEO_OSD_FORMAT_BITMAP ||
		a->alpha != 255 || b->alpha != 255 || a->y != b->y || a->u != b->u || a->v != b->v) {
		r.format = VIDEO_OSD_FORMAT_ARGB8888;
	}
	align_region(target, &r);
	return r;
}

static void merge(const video_osd_target_t *target, video_osd_plan_t *plan, int a, int b)
{
	plan->regions[a] = combine(target, &plan->regions[a], &plan->regions[b]);
	memmove(&plan->regions[b], &plan->regions[b + 1], (size_t)(plan->count - b - 1) * sizeof(plan->regions[0]));
	plan->count--;
}

static int build_regions(const video_osd_target_t *target, const video_osd_render_item_t *items,
						 int count, int max_regions, uint32_t excluded, video_osd_plan_t *plan)
{
	memset(plan, 0, sizeof(*plan));
	if (count && !max_regions) {
		return VIDEO_OSD_PRIVACY_ENOSPC;
	}
	for (int i = 0; i < count; i++) {
		if (excluded & (1U << i)) {
			continue;
		}
		const video_osd_layer_update_t *l = &items[i].layer;
		if (!l->enable || !l->region.width || !l->region.height ||
			(l->format != VIDEO_OSD_FORMAT_BITMAP && l->format != VIDEO_OSD_FORMAT_ARGB8888) ||
			l->region.width > target->frame_width || l->region.height > target->frame_height ||
			l->region.x > target->frame_width - l->region.width || l->region.y > target->frame_height - l->region.height) {
			return -1;
		}
		video_osd_plan_region_t *r = &plan->regions[plan->count++];
		*r = (video_osd_plan_region_t) {
			1U << i, l->region, l->format, l->alpha, l->y, l->u, l->v
		};
		align_region(target, r);
	}
	for (;;) {
		/* Recheck FINAL padded bounding rectangles after every merge. A union
		 * can bridge another region even when its original shapes did not touch. */
		int a = -1, b = -1;
		for (int i = 0; i < plan->count && a < 0; i++) {
			for (int j = i + 1; j < plan->count; j++) {
				if (video_osd_region_conflicts(&plan->regions[i].region, &plan->regions[j].region, target->codec)) {
					a = i;
					b = j;
					break;
				}
			}
		}
		if (a >= 0) {
			merge(target, plan, a, b);
			continue;
		}
		/* Bounded greedy packing: merge if it saves pixel payload,
		 * or choose the cheapest pair when the physical index pool is tight.
		 * This is not a claim of global geometric optimality. */
		int64_t best_delta = INT64_MAX;
		for (int i = 0; i < plan->count; i++) {
			for (int j = i + 1; j < plan->count; j++) {
				video_osd_plan_region_t r = combine(target, &plan->regions[i], &plan->regions[j]);
				int64_t delta = (int64_t)video_osd_plan_region_bytes(&r) - video_osd_plan_region_bytes(&plan->regions[i]) -
								video_osd_plan_region_bytes(&plan->regions[j]);
				if (delta < best_delta) {
					best_delta = delta;
					a = i;
					b = j;
				}
			}
		}
		if (a < 0 || (plan->count <= max_regions && best_delta >= 0)) {
			break;
		}
		merge(target, plan, a, b);
	}
	return 0;
}

int video_osd_plan_build(const video_osd_target_t *target, const video_osd_render_item_t *items,
						 int count, int max_regions, video_osd_plan_t *plan)
{
	if (!target || !items || !plan || count < 0 || count > VIDEO_OSD_MAX_ITEMS || max_regions < 0) {
		return -1;
	}
	uint32_t protected_mask = 0, excluded = 0;
	for (int i = 0; i < count; i++) {
		if (!items[i].nn_group) {
			protected_mask |= 1U << i;
		}
	}
	for (;;) {
		int result = build_regions(target, items, count, max_regions, excluded, plan);
		if (result) {
			return result;
		}
		/* Use FINAL regions, including bitmap padding and slot-pressure merges.
		 * No submitted region may contain both protected content and NN layers. */
		uint32_t conflicting = 0;
		for (int i = 0; i < plan->count; i++) {
			if (plan->regions[i].members & protected_mask) {
				conflicting |= plan->regions[i].members & ~protected_mask;
			}
		}
		if (!conflicting) {
			return 0;
		}
		for (int i = 0; i < count; i++) {
			if (conflicting & (1U << i)) {
				for (int j = 0; j < count; j++) {
					if (items[j].nn_group == items[i].nn_group) {
						excluded |= 1U << j;
					}
				}
			}
		}
		/* Rebuild without the entire group: removing a bridge may change packing.
		 * Exclusions only grow in this call; source shadows remain for recovery. */
	}
}

void video_osd_plan_render(uint8_t *buffer, const video_osd_plan_region_t *r,
						   const video_osd_render_item_t *items, int count)
{
	memset(buffer, 0, video_osd_plan_region_bytes(r));
	for (int i = 0; i < count; i++) {
		if (!(r->members & (1U << i))) {
			continue;
		}
		const video_osd_render_item_t *item = &items[i];
		if (item->polygon) {
			video_osd_polygon_render(buffer, r->format, &r->region, item->polygon, item->rgb);
		} else {
			video_osd_layer_update_t local = item->layer;
			local.region.x -= r->region.x;
			local.region.y -= r->region.y;
			if (r->format == VIDEO_OSD_FORMAT_ARGB8888) {
				video_osd_composite_layer((uint32_t *)buffer, r->region.width, r->region.height, &local);
			} else {
				for (uint32_t y = 0; y < local.region.height; y++) {
					for (uint32_t x = 0; x < local.region.width; x++) {
						if (local.buffer[(size_t)y * (local.region.width / 8) + x / 8] & (0x80U >> (x & 7))) {
							uint32_t dx = x + local.region.x, dy = y + local.region.y;
							buffer[(size_t)dy * (r->region.width / 8) + dx / 8] |= 0x80U >> (dx & 7);
						}
					}
				}
			}
		}
	}
}
