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

#if defined  (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

static const char *const TAG = "VP_MONITOR";

u32
CmdRamHelp(
	u16 argc,
	u8  *argv[]
)
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

u32 cmd_dump_word(u16 argc, u8  *argv[])
{
	u32 Src = 0x0;
	u32 Len = 1; //default value
	u32 Is_Byte = FALSE;
	u32 OTF_Enable = SYSCFG_OTP_RSIPEn();

	/* get parameters */
	if (argc < 1 || argc > 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}
	if (argc >= 3) {
		Is_Byte = *((const char *)argv[2]) == 'B' || *((const char *)argv[2]) == 'b';
	}
	if (argc >= 2) {
		Len = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
	}
	if (argc >= 1) {
		Src = _strtoul((const char *)(argv[0]), (char **)NULL, 16);
	}

	/* align */
	Src &= ~(0x03);

	/* read encrypt image for FW protection */
	if (IS_FLASH_ADDR(Src)) {
		if (OTF_Enable != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "RSIP enabled, Can't Read Flash.\n");
			return TRUE;
		}
	}

	if (Is_Byte && Len >= 1) {
		DCache_CleanInvalidate(Src, sizeof(u8) * Len);
		rtk_log_memory_dump_byte((u8 *)Src, Len);
	} else if (Len >= 1) {
		DCache_CleanInvalidate(Src, sizeof(u32) * Len);
		rtk_log_memory_dump_word((u32 *)Src, Len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong cmd!\r\n");
		return FALSE;
	}

	return TRUE;
}

u32 cmd_write_word(u16 argc, u8  *argv[])
{
	UNUSED(argc);

	u32 Src;
	u32 Value;

	Src = _strtoul((const char *)(argv[0]), (char **)NULL, 16);

	Src &= ~(0x03);

	Value = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%08x] %08x \n", Src, Value);

	*(volatile u32 *)(Src) = Value;

	DCache_Clean(Src, sizeof(u32));

	return 0;
}

static u32 cmd_debugtimer(u16 argc, u8 *argv[])
{
	u32 passtime = 0;

	DTimer_Cmd(DISABLE);
	DTimer_Cmd(ENABLE);
#if defined(CONFIG_ARM_CORE_CM4	)
	DiagPrintf("DWT CYCCNT %x\n", DWT->CYCCNT);
#endif

	passtime = DTimestamp_Get();
	//DelayMs(100);
	extern void vTaskDelay(const long unsigned int);
	vTaskDelay(100);
	passtime = DTimestamp_Get() - passtime;

#if defined(CONFIG_ARM_CORE_CM4	)
	DiagPrintf("DWT CYCCNT %x\n", DWT->CYCCNT);
#endif

	DiagPrintf("Delay 100ms PassTime %d us\n", passtime);
	(void)argc;
	(void)argv;
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
COMMAND_TABLE __attribute__((unused)) shell_cmd_table[] = {
	{ "?", CmdRamHelp },
	{ "DW", cmd_dump_word },
	{ "EW", cmd_write_word },
	{ "DTIMER", cmd_debugtimer },
	{ "STATS", cmd_stats },
};
