#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <ameba_soc.h>
#include <ameba_ipc.h>
#include <ameba_ipc_api.h>

#include "ipc_video.h"
#include <dlist.h>

#define AP_IPC_DEBUG    0
#define TAG             "AP_VID_IPC"
#if AP_IPC_DEBUG
#define IpcApDbgInfo(...)  RTK_LOGI(TAG, __VA_ARGS__)
#else
#define IpcApDbgInfo(...)
#endif
#define IpcApDbgErr(...)  RTK_LOGE(TAG, __VA_ARGS__)

struct queue_node {
	QueueHandle_t queue_handle;
	struct list_head list;
};

static LIST_HEAD(notify_queue_list);

static void notify_queue_list_add(QueueHandle_t new_notify_queue)
{
	struct queue_node *new_node = malloc(sizeof(struct queue_node));
	if (new_node != NULL) {
		taskENTER_CRITICAL();
		new_node->queue_handle = new_notify_queue;
		list_add(&new_node->list, &notify_queue_list);
		taskEXIT_CRITICAL();
	} else {
		IpcApDbgErr("error: fail to malloc for new node\r\n");
	}
}

static void notify_queue_list_del(QueueHandle_t notify_queue)
{
	struct queue_node *node;
	struct queue_node *tmp;
	struct queue_node *need_free = NULL;
	taskENTER_CRITICAL();
	list_for_each_entry_safe(node, tmp, &notify_queue_list, list, struct queue_node) {
		if (node->queue_handle == notify_queue) {
			list_del(&node->list);
			need_free = node;
			break;
		}
	}
	taskEXIT_CRITICAL();
	if (need_free) {
		free(need_free);
	}
}

static int is_valid_notify_queue_handle(QueueHandle_t notify_queue)
{
	struct queue_node *node;
	int queue_is_valid = 0;
	taskENTER_CRITICAL();
	list_for_each_entry(node, &notify_queue_list, list, struct queue_node) {
		if (node->queue_handle == notify_queue) {
			queue_is_valid = 1;
			break;
		}
	}
	taskEXIT_CRITICAL();
	return queue_is_valid;
}

// AP handling response and channel info receive VP message
void ipc_VP2AP_interrupt_handler(void *Data, u32 IrqStatus, u32 ChanNum)
{
	(void)IrqStatus;
	(void)ChanNum;
	BaseType_t xHigherPriorityTaskWoken;
	u32 ipc_ch = (u32)Data;
	vp_resp_t *vp_resp = (vp_resp_t *)ipc_get_message(IPC_VP_TO_AP, ipc_ch);

	if (IS_CMD_RESP(vp_resp->resp)) {
		// unblock video command
		if (is_valid_notify_queue_handle(vp_resp->notify)) {
			xQueueSendFromISR(vp_resp->notify, vp_resp, &xHigherPriorityTaskWoken);
		}
	} else if (IS_FRM_RESP(vp_resp->resp)) {
		// wakeup video channel handing task
		DCache_Invalidate((u32)vp_resp->frame_info, sizeof(vp_frame_info_t));
		vp_frame_info_t *frm_info = (vp_frame_info_t *)vp_resp->frame_info;
		extern void video_send_to_ipc_rx_queue(vp_frame_info_t *frm_info);
		video_send_to_ipc_rx_queue(frm_info);
	}

	/*
	if(xHigherPriorityTaskWoken) {
		taskYIELD_FROM_ISR();
	}
	*/
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE   vp_ipc_host_event_table[5] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_VP2AP_interrupt_handler,
		.RxIrqData = (void *) IPC_V2A_VCMD_RESP_CH0,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_AP,
		.IPC_Channel = IPC_V2A_VCMD_RESP_CH0,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_VP2AP_interrupt_handler,
		.RxIrqData = (void *) IPC_V2A_VCMD_RESP_CH1,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_AP,
		.IPC_Channel = IPC_V2A_VCMD_RESP_CH1,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_VP2AP_interrupt_handler,
		.RxIrqData = (void *) IPC_V2A_VCMD_RESP_CH2,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_AP,
		.IPC_Channel = IPC_V2A_VCMD_RESP_CH2,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_VP2AP_interrupt_handler,
		.RxIrqData = (void *) IPC_V2A_VCMD_RESP_CH3,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_AP,
		.IPC_Channel = IPC_V2A_VCMD_RESP_CH3,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_VP2AP_interrupt_handler,
		.RxIrqData = (void *) IPC_V2A_VCMD_RESP_CH4,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_VP_TO_AP,
		.IPC_Channel = IPC_V2A_VCMD_RESP_CH4,
	},


	/* Frame Response */
	//TODO: seperate command response and data/frame response
};

SemaphoreHandle_t ipc_mutex[5];

// AP send VP command
int AP2VP_SendCmd(uint32_t cmd, uint32_t arg1, uint32_t arg2, uint32_t timeout)
{
	int ret = 0;

	vp_cmd_t vpcmd;
	vp_resp_t vpresp;

	// use stack resource
	QueueHandle_t xQueue = NULL;
	if (timeout) {
		xQueue = xQueueCreate(1, sizeof(vp_resp_t));
		if (xQueue == NULL) {
			IpcApDbgErr("fail to create notify queue\r\n");
			return -2;
		}
	}
	vpcmd.cmd = cmd;
	vpcmd.arg1 = arg1;
	vpcmd.arg2 = arg2;
	vpcmd.notify = (void *)xQueue;
	notify_queue_list_add(xQueue);

	uint32_t vp2ap_ipc_ch = 0;
	uint32_t stream_id = GET_CHANNEL(cmd);
	switch (stream_id) {
	case 0:
		vp2ap_ipc_ch = IPC_A2V_VCMD_SEND_CH0;
		break;
	case 1:
		vp2ap_ipc_ch = IPC_A2V_VCMD_SEND_CH1;
		break;
	case 2:
		vp2ap_ipc_ch = IPC_A2V_VCMD_SEND_CH2;
		break;
	case 3:
		vp2ap_ipc_ch = IPC_A2V_VCMD_SEND_CH3;
		break;
	case 4:
		vp2ap_ipc_ch = IPC_A2V_VCMD_SEND_CH4;
		break;
	default:
		break;
	}

	// TODO : Add resource lock here
	IpcApDbgInfo("AP send IPC cmd 0x%lx [time:%lu]\n\r", vpcmd.cmd, xTaskGetTickCount());

	if (!ipc_mutex[stream_id]) {
		ipc_mutex[stream_id] = xSemaphoreCreateMutex();
		if (!ipc_mutex[stream_id]) {
			return -2;
		}
	}

	if (xSemaphoreTake(ipc_mutex[stream_id], (TickType_t) 0xFFFFFFFF) == pdTRUE) {
		ipc_send_message(IPC_AP_TO_VP, vp2ap_ipc_ch, (void *)&vpcmd);

	}
	// TODO : Add resource unlock here
	if (timeout) {
		// give by IRQ
		if (xQueueReceive(xQueue,  &vpresp, (TickType_t) timeout) == pdTRUE) {
			IpcApDbgInfo("AP IPC cmd 0x%lx Acked %ld by VP [time:%lu]\n\r", GET_COMMAND(vpresp.resp), vpresp.cmd_res, xTaskGetTickCount());
			ret = vpresp.cmd_res;
		} else {
			IpcApDbgErr("error: AP fail to receive cmd 0x%lx Ack from VP\r\n", vpcmd.cmd);
			ret = -1;
		}
	}

	if (xQueue) {
		notify_queue_list_del(xQueue);
		vQueueDelete(xQueue);
	}

	xSemaphoreGive(ipc_mutex[stream_id]);

	IpcApDbgInfo("AP finish IPC cmd 0x%lx\n\r", vpcmd.cmd);
	return ret;
}
