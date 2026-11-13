#include <rts_isp.h>
#include <isp_client_message.h>
#include <isp_utils_common.h>

#define TASK_NUM_MAX 16
#define ISP_SERVER_EXEC_STATUS isp_action_r(EXEC_TYPE, 4, int32_t)
#define ISP_SERVER_INFO_TASKS isp_action_r(INFO_TYPE, 0, struct isp_tasks_stats)
#define ISP_SERVER_INFO_HEAP isp_action_r(INFO_TYPE, 1, struct isp_heap_stats)

struct isp_task_stats {
	char task_name[16];
	uint16_t task_num;
	uint16_t current_state;
	uint16_t current_priority;
	uint16_t base_priority;
	uint32_t runtime_counter;
};

struct isp_tasks_stats {
	uint32_t total_time;
	uint32_t stats_count;
	struct isp_task_stats stats[TASK_NUM_MAX];
};

struct isp_heap_stats {
	size_t available_heap_space_inbytes;
	size_t largest_freeblock_inbytes;
	size_t smallest_freeblock_inbytes;
	size_t number_of_freeblocks;
	size_t minimum_ever_freebytes_remaining;
	size_t number_of_successful_allocations;
	size_t number_of_successful_frees;
};

static int server_status(void)
{
	int ret;
	int status = RTS_ISP_UNINITIALIZED;

	ret = isp_client_message_simple(0, 0, ISP_SERVER_EXEC_STATUS, &status,
									sizeof(status));
	if (ret) {
		return ret;
	}
	return status;
}

int32_t rts_av_isp_get_status(void)
{
	int status;

	status = server_status();
	if (status < 0) {
		return RTS_ISP_UNINITIALIZED;
	}
	return status;
}

int rts_isp_get_mcu_tasks_stats(struct isp_tasks_stats *stats)
{
	int ret;

	ret = isp_client_message_simple(0, 0, ISP_SERVER_INFO_TASKS, stats,
									sizeof(struct isp_tasks_stats));

	return ret;
}

int rts_isp_get_mcu_heap_stats(struct isp_heap_stats *stats)
{
	int ret;

	ret = isp_client_message_simple(0, 0, ISP_SERVER_INFO_HEAP, stats,
									sizeof(struct isp_heap_stats));

	return ret;
}
