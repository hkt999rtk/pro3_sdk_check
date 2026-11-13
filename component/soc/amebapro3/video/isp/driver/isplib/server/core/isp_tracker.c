#include "isp_tracker.h"

#if TIME_TRACKER
#include <isp_os_log.h>
#include <isp_os_mem.h>
#include <isp_os_time.h>
#include <isp_os_thread.h>
#include <isp_utils.h>
#include "isp_avl.h"

struct tracker_node {
	struct avl_node node;
	uint64_t begin;
	uint64_t elapse;
	uint64_t count;
};

static int tracker_on;
static struct avl_tree tracker_tree;
static struct tracker_node tracker_nodes[64];
static int node_index;

static int tracker_cmp(const void *k1, const void *k2, void *ptr)
{
	return isp_strcmp(k1, k2);
}

static struct tracker_node *__new_node(void)
{
	if (node_index < ARRAY_SIZE(tracker_nodes)) {
		return &tracker_nodes[node_index++];
	}
	isp_warning("Track node is not enough!\n");
	return NULL;
}

void time_tracker_set_rt(void)
{
	isp_os_thread_set_rt();
}

void time_tracker_init(void)
{
	tracker_on = 1;
	avl_init(&tracker_tree, tracker_cmp, 0, NULL);
}

void time_tracker_begin(const char *str)
{
	struct tracker_node *node;

	if (!tracker_on) {
		return;
	}
	node = avl_find_element(&tracker_tree, str, node, node);
	if (!node) {
		node = __new_node();
		if (node) {
			node->node.key = str;
			avl_insert(&tracker_tree, &node->node);
		}
	}
	if (!node) {
		return;
	}
	node->begin = isp_os_time_get_us();
}

void time_tracker_end(const char *str)
{
	uint64_t end;
	struct tracker_node *node;

	if (!tracker_on) {
		return;
	}
	node = avl_find_element(&tracker_tree, str, node, node);
	if (!node) {
		return;
	}
	end = isp_os_time_get_us();
	if (isp_strcmp(str, "total")) {
		node->count++;
	}
	node->elapse += end - node->begin;
}

void time_tracker_done(void)
{
	struct tracker_node *node;

	if (!tracker_on) {
		return;
	}
	tracker_on = 0;
	isp_info("\n");
	avl_for_each_element(&tracker_tree, node, node) {
		isp_info("===> %12s: %8lld, %12llu us, %12.2f us/count\n",
				 (const char *)node->node.key, node->count,
				 node->elapse,
				 node->count ? (double)node->elapse / node->count :
				 node->elapse);
	}
	isp_memset(tracker_nodes, 0, sizeof(tracker_nodes));
	node_index = 0;
}
#endif
