#include "ameba_soc.h"
#include "os_wrapper.h"
#include "vip_lite.h"
#include "img_ppu_rotate.h"
#include <string.h>

extern void nn_init(void);
extern const unsigned char img_rotate_nb[];
extern const unsigned int img_rotate_nb_size;
static rtos_task_t owner;
static vip_network net;
static vip_buffer in_buffer, out_buffer;
static vip_buffer_create_params_t in_param, out_param;
static void *in_address, *out_address;
static uint32_t bound_width, bound_height;
static int vip_live, prepared, ready, first_run;
static img_rotate_timing_t timing;
static img_rotate_info_t info;
uint64_t img_ppu_rotate_now_us(void)
{
	return rtos_time_get_current_system_time_us();
}
static img_rotate_heap_t heap_snapshot(void)
{
	img_rotate_heap_t h = {rtos_mem_get_free_heap_size(), rtos_mem_get_minimum_ever_free_heap_size()};
	return h;
}
static int owned(void)
{
	return owner == rtos_task_handle_get();
}
static int metadata(int output, vip_buffer_create_params_t *p)
{
	unsigned i;
	int s;
#define QUERY(prop,ptr) do { s = output ? vip_query_output(net,0,prop,ptr) : vip_query_input(net,0,prop,ptr); if(s) return s; } while(0)
	memset(p, 0, sizeof(*p));
	QUERY(VIP_BUFFER_PROP_NUM_OF_DIMENSION, &p->num_of_dims);
	if (p->num_of_dims < 2 || p->num_of_dims > 6) {
		return ROTATE_BAD_MODEL;
	}
	QUERY(VIP_BUFFER_PROP_SIZES_OF_DIMENSION, p->sizes);
	QUERY(VIP_BUFFER_PROP_DATA_FORMAT, &p->data_format);
	QUERY(VIP_BUFFER_PROP_QUANT_FORMAT, &p->quant_format);
	if (p->data_format != VIP_BUFFER_FORMAT_UINT16 || !p->sizes[0] || !p->sizes[1] ||
		((p->sizes[0] | p->sizes[1]) & (ROTATE_SIZE_ALIGNMENT - 1u))) {
		return ROTATE_BAD_MODEL;
	}
	if (p->quant_format == VIP_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT) {
		QUERY(VIP_BUFFER_PROP_FIXED_POINT_POS, &p->quant_data.dfp.fixed_point_pos);
		if (p->quant_data.dfp.fixed_point_pos != 0) {
			return ROTATE_BAD_MODEL;
		}
	} else if (p->quant_format != VIP_BUFFER_QUANTIZE_NONE) {
		return ROTATE_BAD_MODEL;
	}
	for (i = 2; i < p->num_of_dims; i++) if (p->sizes[i] != 1) {
			return ROTATE_BAD_MODEL;
		}
	p->memory_type = VIP_BUFFER_MEMORY_TYPE_HOST;
	return 0;
#undef QUERY
}
int img_ppu_rotate_deinit(void)
{
	int s = 0, r;
	if (!owner) {
		return 0;
	}
	if (!owned()) {
		return ROTATE_BUSY;
	}
	ready = 0;
#define CLEAN(call) do { r=(call); if(r && !s) s=r; } while(0)
	if (prepared) {
		CLEAN(vip_finish_network(net));
		prepared = 0;
	}
	if (net) {
		CLEAN(vip_destroy_network(net));
		net = NULL;
	}
	if (in_buffer) {
		CLEAN(vip_destroy_buffer(in_buffer));
		in_buffer = NULL;
	}
	if (out_buffer) {
		CLEAN(vip_destroy_buffer(out_buffer));
		out_buffer = NULL;
	}
	if (vip_live) {
		CLEAN(vip_destroy());
		vip_live = 0;
	}
	in_address = out_address = NULL;
	bound_width = bound_height = 0;
	rtos_critical_enter(RTOS_CRITICAL_SOC);
	owner = NULL;
	rtos_critical_exit(RTOS_CRITICAL_SOC);
	return s;
#undef CLEAN
}
int img_ppu_rotate_init(void)
{
	uint64_t t = img_ppu_rotate_now_us();
	uint32_t count = 0, change_ppu = 1;
	int s;
	rtos_critical_enter(RTOS_CRITICAL_SOC);
	if (owner) {
		rtos_critical_exit(RTOS_CRITICAL_SOC);
		return ROTATE_BUSY;
	}
	owner = rtos_task_handle_get();
	rtos_critical_exit(RTOS_CRITICAL_SOC);
	memset(&info, 0, sizeof(info));
	memset(&timing, 0, sizeof(timing));
	info.nb_bytes = img_rotate_nb_size;
	info.before_init = heap_snapshot();
	info.heap_valid_mask = 1;
	nn_init();
#define TRY(call) do { s=(call); if(s) goto fail; } while(0)
	TRY(vip_init());
	vip_live = 1;
	info.after_init = heap_snapshot();
	info.heap_valid_mask |= 2;
	TRY(vip_query_hardware(VIP_QUERY_HW_PROP_CID, sizeof(info.customer_id), &info.customer_id));
	if (info.customer_id != ROTATE_TARGET_CID) {
		s = ROTATE_WRONG_TARGET;
		goto fail;
	}
	TRY(vip_create_network((void *)img_rotate_nb, img_rotate_nb_size, VIP_CREATE_NETWORK_FROM_MEMORY, &net));
	TRY(vip_set_network(net, VIP_NETWORK_PROP_CHANGE_PPU_PARAM, &change_ppu));
	info.after_create = heap_snapshot();
	info.heap_valid_mask |= 4;
	TRY(vip_query_network(net, VIP_NETWORK_PROP_INPUT_COUNT, &count));
	if (count != 1) {
		s = ROTATE_BAD_MODEL;
		goto fail;
	}
	TRY(vip_query_network(net, VIP_NETWORK_PROP_OUTPUT_COUNT, &count));
	if (count != 1) {
		s = ROTATE_BAD_MODEL;
		goto fail;
	}
	TRY(metadata(0, &in_param));
	TRY(metadata(1, &out_param));
	if (in_param.sizes[0] != out_param.sizes[1] || in_param.sizes[1] != out_param.sizes[0]) {
		s = ROTATE_BAD_MODEL;
		goto fail;
	}
	TRY(vip_query_network(net, VIP_NETWORK_PROP_MEMORY_POOL_SIZE, &info.pool_bytes));
	info.pool_valid = 1;
	TRY(vip_prepare_network(net));
	prepared = 1;
	info.after_prepare = heap_snapshot();
	info.heap_valid_mask |= 8;
	info.init_us = img_ppu_rotate_now_us() - t;
	ready = 1;
	first_run = 1;
	return 0;
fail:
	info.init_us = img_ppu_rotate_now_us() - t;
	img_ppu_rotate_deinit();
	return s;
#undef TRY
}
int img_ppu_rotate_u16(const img_rotate_image_t *src, const img_rotate_image_t *dst)
{
	uint64_t t = img_ppu_rotate_now_us(), run_start;
	int s;
	if (!owned()) {
		return owner ? ROTATE_BUSY : ROTATE_NOT_READY;
	}
	timing.run_us = 0;
	if (!ready) {
		s = ROTATE_NOT_READY;
		goto done;
	}
#define TRY(call) do { s=(call); if(s) goto done; } while(0)
	/* Change shape/dispatch only when configuring a different pair or size. */
	if (in_address != src->data || out_address != dst->data ||
		bound_width != src->width || bound_height != src->height) {
		uint32_t bytes = src->width * src->height * 2u;
		vip_ppu_param_t dispatch = {2, {0, 0, 0}, {8, 8, 1}, {2, 2, 1}, {1, 1, 1}};
		if (in_buffer) {
			TRY(vip_destroy_buffer(in_buffer));
			in_buffer = NULL;
		}
		if (out_buffer) {
			TRY(vip_destroy_buffer(out_buffer));
			out_buffer = NULL;
		}
		in_address = out_address = NULL;
		bound_width = bound_height = 0;
		in_param.sizes[0] = src->width;
		in_param.sizes[1] = src->height;
		out_param.sizes[0] = dst->width;
		out_param.sizes[1] = dst->height;
		TRY(vip_create_buffer_from_handle(&in_param, src->data, bytes, &in_buffer));
		TRY(vip_create_buffer_from_handle(&out_param, dst->data, bytes, &out_buffer));
		TRY(vip_set_input(net, 0, in_buffer));
		TRY(vip_set_output(net, 0, out_buffer));
		dispatch.global_size[0] = src->width / 8u;
		dispatch.global_size[1] = src->height / 8u;
		TRY(vip_set_ppu_param(net, &dispatch, 0));
		in_address = src->data;
		out_address = dst->data;
		bound_width = src->width;
		bound_height = src->height;
	}
	/* Caller owns image cache synchronization. Device-only pipelines do not
	 * need CPU image cache maintenance on each rotation. */
	run_start = img_ppu_rotate_now_us();
	s = vip_run_network(net);
	timing.run_us = img_ppu_rotate_now_us() - run_start;
	if (s) {
		goto done;
	}
done:
	if (first_run) {
		info.after_first = heap_snapshot();
		info.heap_valid_mask |= 16;
		first_run = 0;
	}
	timing.api_us = img_ppu_rotate_now_us() - t;
	timing.status = s;
	return s;
#undef TRY
}
int img_ppu_rotate_get_timing(img_rotate_timing_t *out)
{
	if (!owned() || !out) {
		return ROTATE_NOT_READY;
	}
	*out = timing;
	return 0;
}
int img_ppu_rotate_get_info(img_rotate_info_t *out)
{
	if (!out) {
		return ROTATE_NOT_READY;
	}
	/* Also available after failed init, for diagnostics in the test task. */
	*out = info;
	return 0;
}
