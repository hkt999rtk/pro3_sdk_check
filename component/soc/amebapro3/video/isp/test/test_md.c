/*
 * Copyright (C) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * MCU FreeRTOS port of test/test_md2.c.
 * Run md_test_task as a FreeRTOS task after ISP is started.
 */

#include <string.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "hal_md.h"
#include "diag.h"

/* ------------------------------------------------------------------ */
/* Assertion framework                                                 */
/* ------------------------------------------------------------------ */

static int g_pass;
static int g_fail;

#define MD_ASSERT(cond, name) do {                               \
	if (cond) {                                                  \
		g_pass++;                                                \
	} else {                                                     \
		g_fail++;                                                \
		DiagPrintf("[MD FAIL] %s\r\n", (name));                 \
	}                                                            \
} while (0)

#define MD_ASSERT_EQ(a, b, name)   MD_ASSERT((a) == (b), name)
#define MD_ASSERT_NE(a, b, name)   MD_ASSERT((a) != (b), name)
#define MD_ASSERT_NONNULL(p, name) MD_ASSERT((p) != NULL, name)

/* ------------------------------------------------------------------ */
/* Test state                                                          */
/* ------------------------------------------------------------------ */

#define TEST_COUNT  5

struct md_inst {
	void *handle;
	int md_in_w, md_in_h;
	struct rts_isp_md_attr attr;
	struct rts_isp_md_extra_attr e_attr;
	struct rtsc_dma_buffer hist;
	struct rtsc_dma_buffer res;
	uint32_t hist_sz;
	uint32_t res_sz;
};

static struct md_inst inst = {
	.md_in_w = 288,
	.md_in_h = 162,
};

static struct rts_isp_md_extra_attr def_e_attr = {
	.burst_length = {
		.hist_write = 1,
		.res_write  = 1,
		.isp_read   = 1,
	},
	.skip = {
		.enable = 0,
		.frames = 5,
	},
};

static struct rts_isp_md_attr def_attr = {
	.md_in = {
		.x = 0, .y = 0, .w = 288, .h = 162,
		.scale_x = 8, .scale_y = 8,
	},
	.md_out = {
		.x = 0, .y = 0, .w = 288, .h = 162,
		.scale_x = 1, .scale_y = 1,
	},
	.roi = {
		.x = 0, .y = 0, .w = 288, .h = 162,
	},
	.nr_bins  = 0,
	.bin_bits = 0,
	.thd = {
		.learn  = 248,
		.forget = 100,
		.ds     = 0,
		.motion = 0,
		.back   = 1,
	},
	.train_frames = 5,
};

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static void __set_default_args(struct rts_isp_md_attr *attr,
							   int isp_w, int isp_h, int w, int h)
{
	int sx   = isp_w / w;
	int sy   = isp_h / h;
	int offx = (isp_w - (sx * w)) / 2;
	int offy = (isp_h - (sy * h)) / 2;

	attr->md_in.x       = offx;
	attr->md_in.y       = offy;
	attr->md_in.w       = w;
	attr->md_in.h       = h;
	attr->md_in.scale_x = sx;
	attr->md_in.scale_y = sy;

	attr->md_out.x       = 0;
	attr->md_out.y       = 0;
	attr->md_out.w       = w;
	attr->md_out.h       = h;
	attr->md_out.scale_x = 1;
	attr->md_out.scale_y = 1;

	attr->roi.x = 0;
	attr->roi.y = 0;
	attr->roi.w = w;
	attr->roi.h = h;

	attr->nr_bins      = MD_NR_BINS_16;
	attr->bin_bits     = MD_BIN_BITS_2;
	attr->thd.learn    = 248;
	attr->thd.forget   = 100;
	attr->thd.ds       = 0;
	attr->thd.motion   = 0;
	attr->thd.back     = 1;
	attr->train_frames = 5;
}

/* On MCU, ISP is already running when this task executes. */
static void start_isp_stream(void) {}
static void stop_isp_stream(void)  {}

static int suite_init(void)
{
	int ret;

	ret = rts_isp_md_init(&inst.handle);
	if (ret) {
		DiagPrintf("[MD TEST] suite_init: md init failed %d\r\n", ret);
		return ret;
	}

	__set_default_args(&inst.attr, 2304, 1296,
					   inst.md_in_w, inst.md_in_h);

	ret = rts_isp_md_set_attr(inst.handle, &inst.attr);
	if (ret) {
		DiagPrintf("[MD TEST] suite_init: set attr failed %d\r\n", ret);
		return ret;
	}

	return 0;
}

static void suite_clean(void)
{
	if (inst.handle) {
		rts_isp_md_enable(inst.handle, 0);
		rts_isp_md_release(inst.handle);
		inst.handle = NULL;
	}
}

/* ------------------------------------------------------------------ */
/* Phase 1: init / release                                            */
/* ------------------------------------------------------------------ */

static void test_init_release_normal(void)
{
	void *md = NULL;

	MD_ASSERT_EQ(rts_isp_md_init(&md), RTS_OK,
				 "init_release_normal: init returns RTS_OK");
	MD_ASSERT_NONNULL(md, "init_release_normal: handle non-NULL");
	rts_isp_md_release(md);
}

static void test_init_release_invalid(void)
{
	MD_ASSERT_NE(rts_isp_md_init(NULL), RTS_OK,
				 "init_release_invalid: NULL omd fails");
}

/* ------------------------------------------------------------------ */
/* Phase 2: single function tests                                      */
/* ------------------------------------------------------------------ */

static void test_set_get_extra_attr_invalid(void)
{
	struct rts_isp_md_extra_attr e;

	MD_ASSERT_NE(rts_isp_md_set_extra_attr(NULL, &def_e_attr), RTS_OK,
				 "extra_attr_invalid: NULL handle");
	MD_ASSERT_NE(rts_isp_md_set_extra_attr(inst.handle, NULL), RTS_OK,
				 "extra_attr_invalid: NULL attr");
	MD_ASSERT_NE(rts_isp_md_get_extra_attr(NULL, &e), RTS_OK,
				 "extra_attr_invalid: NULL handle get");
	MD_ASSERT_NE(rts_isp_md_get_extra_attr(inst.handle, NULL), RTS_OK,
				 "extra_attr_invalid: NULL attr get");
}

static void test_set_get_attr_normal(void)
{
	struct rts_isp_md_attr md_attr;

	MD_ASSERT_EQ(rts_isp_md_set_attr(inst.handle, &def_attr), RTS_OK,
				 "set_get_attr_normal: set");
	MD_ASSERT_EQ(rts_isp_md_get_attr(inst.handle, &md_attr), RTS_OK,
				 "set_get_attr_normal: get");
	MD_ASSERT_EQ(memcmp(&md_attr, &def_attr,
						sizeof(struct rts_isp_md_attr)), 0,
				 "set_get_attr_normal: round-trip match");
}

static void test_set_get_attr_invalid(void)
{
	struct rts_isp_md_attr a;

	MD_ASSERT_NE(rts_isp_md_set_attr(inst.handle, NULL), RTS_OK,
				 "set_get_attr_invalid: NULL attr");
	MD_ASSERT_NE(rts_isp_md_set_attr(NULL, &def_attr), RTS_OK,
				 "set_get_attr_invalid: NULL handle");
	MD_ASSERT_NE(rts_isp_md_get_attr(inst.handle, NULL), RTS_OK,
				 "set_get_attr_invalid: NULL attr get");
	MD_ASSERT_NE(rts_isp_md_get_attr(NULL, &a), RTS_OK,
				 "set_get_attr_invalid: NULL handle get");
}

static void test_done_normal(void)
{
	MD_ASSERT_EQ(rts_isp_md_done(inst.handle), RTS_OK,
				 "done_normal");
}

static void test_done_invalid(void)
{
	MD_ASSERT_NE(rts_isp_md_done(NULL), RTS_OK,
				 "done_invalid: NULL handle");
}

static void test_get_fd_invalid(void)
{
	MD_ASSERT_EQ(rts_isp_md_get_fd(NULL), -RTS_E_NULL_POINT,
				 "get_fd_invalid: NULL returns -RTS_E_NULL_POINT");
}

static void test_trigger_train_normal(void)
{
	start_isp_stream();
	MD_ASSERT_EQ(rts_isp_md_trigger_train(inst.handle), RTS_OK,
				 "trigger_train_normal");
	stop_isp_stream();
}

static void test_trigger_train_invalid(void)
{
	MD_ASSERT_NE(rts_isp_md_trigger_train(NULL), RTS_OK,
				 "trigger_train_invalid: NULL handle");
}

static void test_calc_dma_size_normal(void)
{
	uint32_t hist_sz, res_sz;

	__set_default_args(&inst.attr, 2304, 1296,
					   inst.md_in_w, inst.md_in_h);
	MD_ASSERT_EQ(rts_isp_md_calc_dma_size(&inst.attr, &hist_sz, &res_sz),
				 RTS_OK, "calc_dma_size_normal: return");
	MD_ASSERT_EQ(hist_sz, 188416u, "calc_dma_size_normal: hist_sz");
	MD_ASSERT_EQ(res_sz,    8192u, "calc_dma_size_normal: res_sz");
}

static void test_calc_dma_size_invalid(void)
{
	uint32_t hist_sz, res_sz;

	__set_default_args(&inst.attr, 2304, 1296,
					   inst.md_in_w, inst.md_in_h);
	MD_ASSERT_EQ(rts_isp_md_calc_dma_size(NULL, &hist_sz, &res_sz),
				 -RTS_E_NULL_POINT, "calc_dma_size_invalid: NULL attr");
	MD_ASSERT_EQ(rts_isp_md_calc_dma_size(&inst.attr, NULL, &res_sz),
				 -RTS_E_NULL_POINT, "calc_dma_size_invalid: NULL hist");
	MD_ASSERT_EQ(rts_isp_md_calc_dma_size(&inst.attr, &hist_sz, NULL),
				 -RTS_E_NULL_POINT, "calc_dma_size_invalid: NULL res");
}

static void test_alloc_free_dma_normal(void)
{
	struct rtsc_dma_buffer h = {0}, r = {0};

	__set_default_args(&inst.attr, 2304, 1296,
					   inst.md_in_w, inst.md_in_h);
	rts_isp_md_calc_dma_size(&inst.attr, &inst.hist_sz, &inst.res_sz);
	h.length = inst.hist_sz;
	r.length = inst.res_sz;

	MD_ASSERT_EQ(rts_isp_md_alloc_dma(&h), RTS_OK,
				 "alloc_free_dma_normal: hist alloc");
	MD_ASSERT_EQ(rts_isp_md_alloc_dma(&r), RTS_OK,
				 "alloc_free_dma_normal: res alloc");
	rts_isp_md_free_dma(&h);
	rts_isp_md_free_dma(&r);
}

static void test_alloc_free_dma_invalid(void)
{
	MD_ASSERT_EQ(rts_isp_md_alloc_dma(NULL), -RTS_E_NULL_POINT,
				 "alloc_free_dma_invalid: NULL buf");
}

static void test_dma_set_normal(void)
{
	struct rtsc_dma_buffer h = {0}, r = {0};

	__set_default_args(&inst.attr, 2304, 1296,
					   inst.md_in_w, inst.md_in_h);
	rts_isp_md_calc_dma_size(&inst.attr, &inst.hist_sz, &inst.res_sz);
	h.length = inst.hist_sz;
	r.length = inst.res_sz;

	rts_isp_md_alloc_dma(&h);
	rts_isp_md_alloc_dma(&r);

	MD_ASSERT_EQ(rts_isp_md_set_dma_addr(inst.handle,
										 h.phy_addr, h.length,
										 r.phy_addr, r.length),
				 RTS_OK, "dma_set_normal");

	rts_isp_md_free_dma(&h);
	rts_isp_md_free_dma(&r);
}

static void test_dma_set_invalid(void)
{
	MD_ASSERT_NE(rts_isp_md_set_dma_addr(NULL, 0, 0, 0, 0), RTS_OK,
				 "dma_set_invalid: NULL handle");
}

static void test_set_axi_buffer_normal(void)
{
	MD_ASSERT_EQ(rts_isp_md_set_axi_buffer(inst.handle,
										   0x400, 0x800, 0xc00, 0x200),
				 RTS_OK, "set_axi_buffer_normal");
}

static void test_set_axi_buffer_invalid(void)
{
	MD_ASSERT_NE(rts_isp_md_set_axi_buffer(NULL,
										   0x400, 0x800, 0xc00, 0x200),
				 RTS_OK, "set_axi_buffer_invalid: NULL handle");
}

static void test_enable_normal(void)
{
	MD_ASSERT_EQ(rts_isp_md_enable(inst.handle, 1), RTS_OK,
				 "enable_normal: enable");
	MD_ASSERT_EQ(rts_isp_md_enable(inst.handle, 0), RTS_OK,
				 "enable_normal: disable");
}

static void test_enable_invalid(void)
{
	MD_ASSERT_NE(rts_isp_md_enable(NULL, 1), RTS_OK,
				 "enable_invalid: NULL handle");
}

static void test_get_result_normal(void)
{
	struct rts_isp_md_result res;

	MD_ASSERT_EQ(rts_isp_md_get_result(inst.handle, &res), RTS_OK,
				 "get_result_normal");
}

static void test_get_result_invalid(void)
{
	struct rts_isp_md_result res;

	MD_ASSERT_NE(rts_isp_md_get_result(NULL, &res), RTS_OK,
				 "get_result_invalid: NULL handle");
	MD_ASSERT_NE(rts_isp_md_get_result(inst.handle, NULL), RTS_OK,
				 "get_result_invalid: NULL result");
}

/* ------------------------------------------------------------------ */
/* Phase 3: poll tests                                                 */
/* ------------------------------------------------------------------ */

static void get_md_result(int use_poll)
{
	struct rts_isp_md_result resu;
	int ret;
	int i;

	rts_isp_md_calc_dma_size(&inst.attr, &inst.hist_sz, &inst.res_sz);
	inst.hist.length = inst.hist_sz;
	inst.res.length  = inst.res_sz;

	ret = rts_isp_md_alloc_dma(&inst.hist);
	MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: hist alloc");
	ret = rts_isp_md_alloc_dma(&inst.res);
	MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: res alloc");

	ret = rts_isp_md_set_dma_addr(inst.handle,
								  inst.hist.phy_addr, inst.hist.length,
								  inst.res.phy_addr,  inst.res.length);
	MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: set dma addr");

	start_isp_stream();

	ret = rts_isp_md_enable(inst.handle, 1);
	MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: enable");

	for (i = 0; i < TEST_COUNT; i++) {
		if (use_poll) {
			ret = rts_isp_md_poll(inst.handle, -1);
			MD_ASSERT_EQ(ret, RTS_TRUE, "poll: returns RTS_TRUE");
			DiagPrintf("[MD TEST] poll ret %d\r\n", ret);
		} else {
			uint32_t status = 0;

			ret = rts_isp_md_poll2(inst.handle, -1, &status);
			MD_ASSERT_EQ(ret, RTS_TRUE, "poll2: returns RTS_TRUE");
			MD_ASSERT_EQ(status & RTSMD_OL_MASK, 0u,
						 "poll2: no overflow flags");
			DiagPrintf("[MD TEST] poll2 ret %d status 0x%x\r\n",
					   ret, status);
		}

		ret = rts_isp_md_get_result(inst.handle, &resu);
		MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: get_result");
		DiagPrintf("[MD TEST] motion_flag %d count %u\r\n",
				   resu.motion_flag, resu.motion_count);

		ret = rts_isp_md_done(inst.handle);
		MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: done");
	}

	ret = rts_isp_md_enable(inst.handle, 0);
	MD_ASSERT_EQ(ret, RTS_OK, "get_md_result: disable");

	stop_isp_stream();

	rts_isp_md_free_dma(&inst.hist);
	rts_isp_md_free_dma(&inst.res);
}

static void test_poll(void)
{
	get_md_result(1);
}

static void test_poll_train(void)
{
	MD_ASSERT_EQ(rts_isp_md_trigger_train(inst.handle), RTS_OK,
				 "poll_train: trigger");
	get_md_result(1);
}

static void test_poll2(void)
{
	get_md_result(0);
}

static void test_poll2_train(void)
{
	MD_ASSERT_EQ(rts_isp_md_trigger_train(inst.handle), RTS_OK,
				 "poll2_train: trigger");
	get_md_result(0);
}

/* ------------------------------------------------------------------ */
/* Task entry                                                          */
/* ------------------------------------------------------------------ */

void md_test_task(void *pvParameters)
{
	(void)pvParameters;

	g_pass = 0;
	g_fail = 0;

	DiagPrintf("[MD TEST] starting\r\n");

	/* Phase 1: init / release */
	test_init_release_normal();
	test_init_release_invalid();

	/* Phase 2: single function tests */
	if (suite_init() == 0) {
		test_set_get_extra_attr_invalid();
		test_set_get_attr_normal();
		test_set_get_attr_invalid();
		test_done_normal();
		test_done_invalid();
		test_get_fd_invalid();
		test_trigger_train_normal();
		test_trigger_train_invalid();
		test_calc_dma_size_normal();
		test_calc_dma_size_invalid();
		test_alloc_free_dma_normal();
		test_alloc_free_dma_invalid();
		test_dma_set_normal();
		test_dma_set_invalid();
		test_set_axi_buffer_normal();
		test_set_axi_buffer_invalid();
		test_enable_normal();
		test_enable_invalid();
		test_get_result_normal();
		test_get_result_invalid();
		suite_clean();
	}

	/* Phase 3: poll tests */
	if (suite_init() == 0) {
		test_poll();
		test_poll_train();
		test_poll2();
		test_poll2_train();
		suite_clean();
	}

	DiagPrintf("[MD TEST] done — PASS: %d  FAIL: %d\r\n", g_pass, g_fail);
	vTaskDelete(NULL);
}
