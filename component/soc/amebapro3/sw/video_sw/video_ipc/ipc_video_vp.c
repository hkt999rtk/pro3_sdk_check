#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <ameba_soc.h>
#include <ameba_ipc.h>
#include <ameba_ipc_api.h>

#include "ipc_video.h"
#include "video_app_main.h"

#define VP_IPC_DEBUG    0
#define TAG             "VP_VID_IPC"
#if VP_IPC_DEBUG
#define IpcVpDbgInfo(...)  RTK_LOGI(TAG, __VA_ARGS__)
#else
#define IpcVpDbgInfo(...)
#endif
#define IpcVpDbgErr(...)  RTK_LOGE(TAG, __VA_ARGS__)

#define MAX_CMD_CNT     64
static QueueHandle_t ap_cmd_lo_queue = NULL;
static StaticQueue_t ap_cmd_lo_queue_buff;
uint8_t ap_cmd_lo_buff[ MAX_CMD_CNT * sizeof(vp_cmd_t) ];

static QueueHandle_t ap_cmd_hi_queue = NULL;
static StaticQueue_t ap_cmd_hi_queue_buff;
uint8_t ap_cmd_hi_buff[ MAX_CMD_CNT * sizeof(vp_cmd_t) ];

static QueueHandle_t ap_resp_queue = NULL;
static StaticQueue_t ap_resp_queue_buff;
uint8_t ap_resp_buff[ MAX_CMD_CNT * 2 * sizeof(vp_resp_t) ];

static int (*cmd_executer)(uint32_t cmd, uint32_t arg1, uint32_t arg2) = NULL;

static void vp_ipc_set_cmd_executer(int (*exe)(uint32_t cmd, uint32_t arg1, uint32_t arg2))
{
	cmd_executer = exe;
}

static int VP2AP_SendCmdRes(vp_cmd_t cmd, uint32_t timeout)
{
	int ret = 0;
	vp_resp_t resp = {0};
	resp.notify = cmd.notify;
	resp.resp = SET_CHANNEL(resp.resp, GET_CHANNEL(cmd.cmd));
	resp.resp = SET_COMMAND(resp.resp, GET_COMMAND(cmd.cmd));
	resp.resp = SET_COMMAND_RESP(resp.resp);
	if (cmd_executer) {
		resp.cmd_res = cmd_executer(cmd.cmd, cmd.arg1, cmd.arg2);
	}

	int retry_cnt = 3;
	while (ap_resp_queue && (xQueueSend(ap_resp_queue, &resp, timeout) != pdPASS) && (retry_cnt > 0)) {
		retry_cnt--;
	}
	if (retry_cnt == 0) {
		IpcVpDbgErr("fail to send command response\r\n");
		ret = -1;
	}
	return ret;
}

static int VP2AP_SendFrmRes(int ch, vp_frame_info_t *frm_info, uint32_t timeout)
{
	int ret = 0;
	vp_resp_t resp = {0};
	resp.resp = SET_CHANNEL(resp.resp, ch);
	resp.resp = SET_FRAME_RESP(resp.resp);
	resp.frame_info = frm_info;

	int retry_cnt = 3;
	while (ap_resp_queue && (xQueueSend(ap_resp_queue, &resp, timeout) != pdPASS) && (retry_cnt > 0)) {
		retry_cnt--;
	}
	if (retry_cnt == 0) {
		IpcVpDbgErr("fail to send frame response\r\n");
		ret = -1;
	}
	return ret;
}

static void ap_cmd_handler(void *cmd_queue)
{
	vp_cmd_t cmd;
	// wait notify
	while (1) {
		if (cmd_queue && xQueueReceive(cmd_queue,  &cmd, (TickType_t) 50) == pdPASS) {

			if (VP2AP_SendCmdRes(cmd, 50) < 0) {
				IpcVpDbgErr("VP2AP_SendCmdRes fail\r\n");
			}
		}
	}
}

static SemaphoreHandle_t ipc_mutex;

// NOTE : seems no need response queue
static void ap_resp_handler(void *resp_queue)
{
	if (!ipc_mutex) {
		ipc_mutex = xSemaphoreCreateMutex();
		if (!ipc_mutex) {
			return;
		}
	}

	vp_resp_t resp;
	// wait notify
	while (1) {
		if (resp_queue && xQueueReceive(resp_queue,  &resp, (TickType_t) 50) == pdPASS) {
			if (xSemaphoreTake(ipc_mutex, (TickType_t) 0xFFFFFFFF) == pdTRUE) {
				//printf("RESP start\n\r");
				int ch = GET_CHANNEL(resp.resp);
				int vp2ap_ipc_ch = 0;
				switch (ch) {
				case 0:
					vp2ap_ipc_ch = IPC_V2A_VCMD_RESP_CH0;
					break;
				case 1:
					vp2ap_ipc_ch = IPC_V2A_VCMD_RESP_CH1;
					break;
				case 2:
					vp2ap_ipc_ch = IPC_V2A_VCMD_RESP_CH2;
					break;
				case 3:
					vp2ap_ipc_ch = IPC_V2A_VCMD_RESP_CH3;
					break;
				case 4:
					vp2ap_ipc_ch = IPC_V2A_VCMD_RESP_CH4;
					break;
				default:
					break;
				}
				ipc_send_message(IPC_VP_TO_AP, vp2ap_ipc_ch, (PIPC_MSG_STRUCT)&resp);
				//printf("RESP end\n\r");
				xSemaphoreGive(ipc_mutex);
			}
		}
	}
}

// receive VP message, AP send command to VP, enqueue to command queue
static void ipc_AP2VP_interrupt_handler(void *Data, u32 IrqStatus, u32 ChanNum)
{
	(void)IrqStatus;
	(void)ChanNum;
	BaseType_t xHigherPriorityTaskWoken;
	u32 ipc_ch = (u32)Data;
	IpcVpDbgInfo("IN ipc_AP2VP_interrupt_handler with ipc_ch:%d\r\n", ipc_ch);
	vp_cmd_t *cmd = (vp_cmd_t *)ipc_get_message(IPC_AP_TO_VP, ipc_ch);
	IpcVpDbgInfo("ipc_AP2VP_interrupt_handler get cmd:0x%08x\r\n", cmd->cmd);

	if (IS_CMD_HIPRIO(cmd->cmd)) {
		xQueueSendFromISR(ap_cmd_hi_queue, cmd, &xHigherPriorityTaskWoken);
	} else {
		xQueueSendFromISR(ap_cmd_lo_queue, cmd, &xHigherPriorityTaskWoken);
	}

	/*
	if(xHigherPriorityTaskWoken) {
		taskYIELD_FROM_ISR();
	}
	*/
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE   ipc_host_event_table[5] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_AP2VP_interrupt_handler,
		.RxIrqData = (void *) IPC_A2V_VCMD_SEND_CH0,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_VP,
		.IPC_Channel = IPC_A2V_VCMD_SEND_CH0,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_AP2VP_interrupt_handler,
		.RxIrqData = (void *) IPC_A2V_VCMD_SEND_CH1,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_VP,
		.IPC_Channel = IPC_A2V_VCMD_SEND_CH1,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_AP2VP_interrupt_handler,
		.RxIrqData = (void *) IPC_A2V_VCMD_SEND_CH2,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_VP,
		.IPC_Channel = IPC_A2V_VCMD_SEND_CH2,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_AP2VP_interrupt_handler,
		.RxIrqData = (void *) IPC_A2V_VCMD_SEND_CH3,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_VP,
		.IPC_Channel = IPC_A2V_VCMD_SEND_CH3,
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = ipc_AP2VP_interrupt_handler,
		.RxIrqData = (void *) IPC_A2V_VCMD_SEND_CH4,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_VP,
		.IPC_Channel = IPC_A2V_VCMD_SEND_CH4,
	},
};

void vp_ipc_sw_service_init(void)
{
	/* register ap cmd executor */
	extern int vp_ipc_cmd_executer(uint32_t cmd, uint32_t arg1, uint32_t arg2);
	vp_ipc_set_cmd_executer(vp_ipc_cmd_executer);

	// init low priority cmd queue, OSD, MD
	if (!ap_cmd_lo_queue) {
		memset(&ap_cmd_lo_queue_buff, 0, sizeof(StaticQueue_t));
		ap_cmd_lo_queue = xQueueCreateStatic(MAX_CMD_CNT, sizeof(vp_cmd_t), ap_cmd_lo_buff, &ap_cmd_lo_queue_buff);
	}

	// high priority cmd queue, encoder/isp/sensor OP
	if (!ap_cmd_hi_queue) {
		memset(&ap_cmd_hi_queue_buff, 0, sizeof(StaticQueue_t));
		ap_cmd_hi_queue = xQueueCreateStatic(MAX_CMD_CNT, sizeof(vp_cmd_t), ap_cmd_hi_buff, &ap_cmd_hi_queue_buff);
	}

	if (!ap_resp_queue) {
		memset(&ap_resp_queue_buff, 0, sizeof(StaticQueue_t));
		ap_resp_queue = xQueueCreateStatic(MAX_CMD_CNT, sizeof(vp_cmd_t), ap_resp_buff, &ap_resp_queue_buff);
	}

	// init cmd handler task, 2 priority tasks
	if (xTaskCreate(ap_cmd_handler, "ap_cmd_hi", 1024, ap_cmd_hi_queue, 7, NULL) != pdPASS) {
		IpcVpDbgErr("fail to create AP high priority cmd handler task \r\n");
	}
	if (xTaskCreate(ap_cmd_handler, "ap_cmd_lo", 1024, ap_cmd_lo_queue, 6, NULL) != pdPASS) {
		IpcVpDbgErr("fail to create AP low priority cmd handler task \r\n");
	}
	if (xTaskCreate(ap_resp_handler, "ap_resp", 1024, ap_resp_queue, 8, NULL) != pdPASS) {
		IpcVpDbgErr("fail to create AP cmd reponse handler task \r\n");
	}
}

void vp_ipc_send_frame(int ch, vp_frame_info_t *frm_info, uint32_t timeout)
{
	if (VP2AP_SendFrmRes(ch, frm_info, timeout) < 0) {
		IpcVpDbgErr("VP2AP_SendFrmRes fail\r\n");
	}
}
