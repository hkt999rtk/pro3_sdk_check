/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "os_wrapper.h"
#if defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
#include "whc_def.h"
#endif
#if defined  (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

static const char *const TAG = "AP_MONITOR";

u32
CmdRamHelp2(u16 argc, u8 *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;
	(void) argv;

	COMMAND_TABLE *cmd_table = (COMMAND_TABLE *)__cmd_table_start__;
	u32 cmd_mum = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));
	u32	index ;

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "----------------- TEST COMMAND MODE HELP %x------------------\n", cmd_mum);
	for (index = 0  ; index < cmd_mum; index++) {
		if (cmd_table[index].cmd) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s\n", cmd_table[index].cmd);
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "----------------- TEST COMMAND MODE END  %x------------------\n", SYS_CPUID());

	return TRUE ;
}

u32 cmd_enter_sleep(u16 argc, u8 *argv[])
{
	int cpuid = SYS_CPUID();
	DiagPrintf("Before sleep, make sure wakup source has been set\n", cpuid);
	DiagPrintf("[%d] Enter Deep Sleep\n", cpuid);
#if defined(CONFIG_WHC_HOST) && !defined(CONFIG_MP_SHRINK)
	extern int wifi_resume_backup(void);
	wifi_resume_backup();
#endif
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
	/* Notify NP: CA32 going to sleep, stop sending IPC */
	u32 param = 0;  /* 0 = going to sleep */
	extern int whc_ipc_host_api_message_send(u32 id, u32 * param_buf, u32 buf_len);
	whc_ipc_host_api_message_send(WHC_API_WIFI_HOST_READY, &param, 1);
#endif
	extern void SOCPS_Sleep(void);
	SOCPS_Sleep();
	return TRUE;
}

u32 cmd_enter_dsleep(u16 argc, u8 *argv[])
{
	int cpuid = SYS_CPUID();
	DiagPrintf("Before sleep, make sure wakup source has been set\n", cpuid);
	DiagPrintf("[%d] Enter Deep Sleep\n", cpuid);
	extern void SOCPS_DeepSleep(void);
	SOCPS_DeepSleep();
	return TRUE;
}

u32 cmd_set_ro(u16 argc, u8 *argv[])
{
	extern bool mmu_dynamic_modify_readonly(uint32_t target_vaddr, uint32_t size);
	if (argc != 2)	{
		DiagPrintf("Usage: SETRO <address> <size>\n");
		return 0;
	}
	u32 addr = strtoul((char *)argv[0], NULL, 0);
	u32 size = strtoul((char *)argv[1], NULL, 0);
	mmu_dynamic_modify_readonly(addr, size);
	return 0;
}

#include "FreeRTOS.h"
#include "task.h"
static u32 cmd_stats(u16 argc, u8 *argv[])
{
	(void)argc;
	(void)argv;

	/* Task List */
	int task_n = uxTaskGetNumberOfTasks();
	size_t bufSize = task_n * (configMAX_TASK_NAME_LEN + 40);
	char *pcWriteBuffer = (char *)malloc(bufSize);
	if (pcWriteBuffer == NULL) {
		DiagPrintf("fail to malloc buffer for vTaskList\n");
		return FALSE;
	}
	memset(pcWriteBuffer, 0, bufSize);
	vTaskList((char *)pcWriteBuffer);
	DiagPrintf("Task List: \r\n%s\r\n\r\n", pcWriteBuffer);
	free(pcWriteBuffer);

#if (configUSE_STATS_FORMATTING_FUNCTIONS == 1) && (configGENERATE_RUN_TIME_STATS == 1)
	/* Task Run-time Stats */
	bufSize = task_n * (configMAX_TASK_NAME_LEN + 40);
	char *cBuffer = (char *)malloc(bufSize);
	if (cBuffer == NULL) {
		DiagPrintf("fail to malloc buffer for vTaskGetRunTimeStats\n");
		return FALSE;
	}
	memset(cBuffer, 0, bufSize);
	vTaskGetRunTimeStats(cBuffer);
	DiagPrintf("Task Run Time Stats: \r\n%s\r\n\r\n", cBuffer);
	free(cBuffer);
#endif

	/* Heap Stats */
	extern u32 total_heap_size;
	HeapStats_t pxHeapStats;
	vPortGetHeapStats(&pxHeapStats);
	DiagPrintf("HeapStats: \r\n");
	DiagPrintf("Total Heap:\t%u\r\n", total_heap_size);
	DiagPrintf("Heap Free Now:\t%u\r\n", pxHeapStats.xAvailableHeapSpaceInBytes);
	DiagPrintf("Heap Used Now:\t%u\r\n", total_heap_size - pxHeapStats.xAvailableHeapSpaceInBytes);
	DiagPrintf("Heap Used Max:\t%u\r\n", total_heap_size - pxHeapStats.xMinimumEverFreeBytesRemaining);

	return TRUE;
}

CMD_TABLE_DATA_SECTION
COMMAND_TABLE __attribute__((unused)) shell_ap_cmd_table[] = {
	{ "??", CmdRamHelp2 },
	{ "SLEEP", cmd_enter_sleep },
	{ "DSLEEP", cmd_enter_dsleep },
	{ "SETRO", cmd_set_ro},
	{ "STATS", cmd_stats },
};
