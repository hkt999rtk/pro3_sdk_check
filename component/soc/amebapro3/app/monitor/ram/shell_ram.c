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
#include <stdarg.h>
#include "FreeRTOS.h"
#include "ameba_ipc_api.h"
//#include "strproc.h"

#define OpenShellRx		2

//#define LIB_INFO_CMD	"ATS?"
//#define ALL_CPU_RECV	0xFFFF


#if defined(CONFIG_SUPPORT_ATCMD)
#define ATCMD_ENABLE 1
#else
#define ATCMD_ENABLE 0
#endif

#define MASTER_IS_AP 1
#define MASTER_IS_NP 0
#if defined(CONFIG_CMD_MASTER_IS_AP)
#define CMD_MASTER MASTER_IS_AP	// default cmd to AP
#else
#define CMD_MASTER MASTER_IS_NP	// default cmd to NP
#endif

#define PROMPT_VP '@'
#define PROMPT_FP '!'
#define PROMPT_ANP '$'

#if (defined(CONFIG_MP_INCLUDED) && (defined(CONFIG_ARM_CORE_CM4_NP) || defined(CONFIG_WHC_HOST)))
#define MPCMD_ENABLE 1
#else
#define MPCMD_ENABLE 0
#endif

#if MPCMD_ENABLE==1 && ATCMD_ENABLE==0
#error ATCMD should enable if need MPCMD?
#endif

#if defined (CONFIG_ARM_CORE_CM4_NP)
#define SHELL_STACK_SIZE 4096	// 4096 WORD (4byte)
#else
#define SHELL_STACK_SIZE 1024	// 1024 WORD (4byte)
#endif

#if defined (CONFIG_SHELL_IPC) && CONFIG_SHELL_IPC == 1
#define SHELL_IPC 1
#else
#define SHELL_IPC 0
#endif


#if defined(CONFIG_LINK_ROM_SYMB)
// from ROM bss
extern void *shell_ctl_get(void);
#define shell_ctl (*((UART_LOG_CTL*)shell_ctl_get()))
#else
extern volatile UART_LOG_CTL		shell_ctl;
#endif

rtos_sema_t	shell_sema = NULL;

#if ATCMD_ENABLE==1
__weak int atcmd_service(char *line_buf)
{
	(void)line_buf;
	return 0;
}
__weak void atcmd_service_init(void)
{
	return;
}
#endif

#if MPCMD_ENABLE==1
extern int mp_command_handler(char *cmd);
#endif

static monitor_cmd_handler shell_get_cmd(char *argv)
{
	PCOMMAND_TABLE  pCmdTbl = shell_ctl.pCmdTbl;
	u32 CmdCnt = 0;
	u32 CmdNum = shell_ctl.CmdTblSz;

	monitor_cmd_handler cmd_handler = NULL;

	for (CmdCnt = 0; CmdCnt < CmdNum; CmdCnt++) {
		if ((_stricmp(argv, (const char *)pCmdTbl[CmdCnt].cmd)) == 0) {
			cmd_handler = pCmdTbl[CmdCnt].func;
			break;
		}
	}

	return cmd_handler;
}

static void shell_give_sema(void)
{
	if (shell_ctl.shell_task_rdy) {
		rtos_sema_give(shell_sema);
	}
}

static u32 shell_cmd_exec_ram(u8 *pbuf)
{
	monitor_cmd_handler cmd_handler = NULL;
	u8 argc = shell_get_argc((const u8 *) pbuf);
	u8 **argv = shell_get_argv((const u8 *) pbuf);

	if (argc == 0) {
		return FALSE;
	}

	cmd_handler = shell_get_cmd((char *)argv[0]);
	if (cmd_handler != NULL) {
		cmd_handler((argc - 1), (argv + 1));
		return TRUE;
	}

	return FALSE;
}


#include <stdatomic.h>
#define LOGBUF_SIZE (256 - 32)

typedef struct logbuf_s {
	u32 cmd_rdy;
	u32 dummy[7];
	u8  buf[LOGBUF_SIZE];
} logbuf_t;

typedef struct shm_ctrl_s {
	logbuf_t log_vp;
	logbuf_t log_fp;
	logbuf_t log_ap;
} shm_ctrl_t;

#if SHELL_IPC==1
extern uint8_t __shm_ssram_start__[];

// IPC table

/* shell_cmd_inject is used by both the VP/CM0/CA32 IPC path and the
 * CM4_NP+LINUX_FW_EN AP shell path; define it before both. */
void shell_cmd_inject(const char *cmd, u32 len)
{
	PUART_LOG_BUF pShellBuf = shell_ctl.pTmpLogBuf;

	/* Clamp to buffer capacity; leave room for NUL terminator. */
	len = MIN(len, (u32)(pShellBuf->UARTLogBufLen - 1));

	memcpy(pShellBuf->UARTLogBuf, cmd, len);
	pShellBuf->UARTLogBuf[len] = '\0';
	pShellBuf->BufCount = len;
	shell_ctl.ExecuteCmd = _TRUE;
	if (shell_ctl.shell_task_rdy) {
		shell_ctl.GiveSema();
	}
}

#if defined(CONFIG_ARM_CORE_CM4_VP) || defined(CONFIG_ARM_CORE_CM0) || defined(CONFIG_ARM_CORE_CA32)
void shell_loguratRx_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

#if defined(CONFIG_ARM_CORE_CM4_VP)
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_NP_TO_VP, IPC_N2V_SHELL_COMMAND);
#elif defined(CONFIG_ARM_CORE_CM0)
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_NP_TO_FP, IPC_N2F_SHELL_COMMAND);
#elif defined(CONFIG_ARM_CORE_CA32)
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_NP_TO_AP, IPC_N2A_SHELL_COMMAND);
#endif

	logbuf_t *log = (logbuf_t *)ipc_msg_temp->msg;

	DCache_Invalidate((u32)log, sizeof(logbuf_t));
	shell_cmd_inject((const char *)log->buf, strlen((char *)log->buf));
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_shell_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = shell_loguratRx_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
#if defined(CONFIG_ARM_CORE_CM4_VP)
	.IPC_Direction = IPC_NP_TO_VP,
	.IPC_Channel = IPC_N2V_SHELL_COMMAND
#elif defined(CONFIG_ARM_CORE_CM0)
	.IPC_Direction = IPC_NP_TO_FP,
	.IPC_Channel = IPC_N2F_SHELL_COMMAND
#elif defined(CONFIG_ARM_CORE_CA32)
	.IPC_Direction = IPC_NP_TO_AP,
	.IPC_Channel = IPC_N2A_SHELL_COMMAND
#endif
};
#endif //defined(CONFIG_ARM_CORE_CM4_VP) || defined(CONFIG_ARM_CORE_CM0) || defined(CONFIG_ARM_CORE_CA32)


#if defined (CONFIG_ARM_CORE_CM4_NP)
void ipc_shell_rx(void *Data, u32 IrqStatus, u32 ChanNum)
{
	PIPC_MSG_STRUCT IPC_MSG;
	IPC_MSG = ipc_get_message(IPC_VP_TO_NP, IPC_V2N_SHELL_COMMAND);
	//printf("IPC_MSG len %d\r\n",IPC_MSG->msg_len);
	(void)IPC_MSG;
	(void)Data;
	(void)IrqStatus;
	(void)ChanNum;
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_shell_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = ipc_shell_rx,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_VP_TO_NP,//IPC_FP_TO_NP,
	.IPC_Channel = IPC_V2N_SHELL_COMMAND
};

#if defined (CONFIG_LINUX_FW_EN)
void ipc_ap_shell_rx(void *Data, u32 IrqStatus, u32 ChanNum)
{
	PIPC_MSG_STRUCT ipc_msg;
	u32 cmd_addr;
	u32 cmd_len;

	(void)Data;
	(void)IrqStatus;
	(void)ChanNum;

	ipc_msg = ipc_get_message(IPC_AP_TO_NP, IPC_A2N_LOGUART_RX_SWITCH);
	if (!ipc_msg) {
		return;
	}

	cmd_addr = ipc_msg->msg;
	cmd_len = ipc_msg->msg_len;

	if (cmd_len > UART_LOG_CMD_BUFLEN - 1) {
		cmd_len = UART_LOG_CMD_BUFLEN - 1;
	}

	if (cmd_len > 0) {
		DCache_Invalidate(cmd_addr, cmd_len);

		shell_cmd_inject((const char *)cmd_addr, cmd_len);
	}
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_shell_ap_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = ipc_ap_shell_rx,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_AP_TO_NP,
	.IPC_Channel = IPC_A2N_LOGUART_RX_SWITCH,
};
#endif //defined (CONFIG_LINUX_FW_EN)
#endif //defined (CONFIG_ARM_CORE_CM4_NP)

#if defined(CONFIG_ARM_CORE_CM4_NP)
/* for uart bridge to close shell loguart rx */
void shell_uartbridge_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	u32 TempVal;
	SYSON_TypeDef *SYSON = (SYSON_TypeDef *)SYSON_BASE;
	AON_TypeDef *AON = (AON_TypeDef *)AON_BASE;
	PIPC_MSG_STRUCT	ipc_msg = (PIPC_MSG_STRUCT)ipc_get_message(IPC_AP_TO_NP, IPC_A2N_UARTBRIDGE);

	if (ipc_msg->msg == OpenShellRx) {
		TempVal = SYSON->PON_BT_CTRL;
		TempVal &= ~(SYSON_BIT_FORCE_LOGUART_USE_LOGUART_PAD);
		SYSON->PON_BT_CTRL = TempVal;

		TempVal = AON->AON_WL_CTRL;
		TempVal &= ~(AON_BIT_WL_USE_REQ);
		AON->AON_WL_CTRL = TempVal;

		/* re-register shell loguart irq handler */
		InterruptRegister((IRQ_FUN) shell_uart_irq_rom, LOG_UART_IRQ, (u32)NULL, INT_PRI_MIDDLE);
		InterruptEn(LOG_UART_IRQ, INT_PRI_MIDDLE);
	} else {
		TempVal = SYSON->PON_BT_CTRL;
		TempVal |= SYSON_BIT_FORCE_LOGUART_USE_LOGUART_PAD;
		SYSON->PON_BT_CTRL = TempVal;

		TempVal = AON->AON_WL_CTRL;
		TempVal |= AON_BIT_WL_USE_REQ;
		AON->AON_WL_CTRL = TempVal;

		/* disable shell loguart irq handler */
		InterruptDis(LOG_UART_IRQ);
	}
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_uartbridge_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = shell_uartbridge_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_AP_TO_NP,
	.IPC_Channel = IPC_A2N_UARTBRIDGE
};
#endif


shm_ctrl_t *shm_ctrl = (shm_ctrl_t *)__shm_ssram_start__;

int shm_put_logbuf(logbuf_t *log, char *cmd_str, uint32_t ipc_dir, uint32_t ipc_ch)
{
	// sync ready
	DCache_Invalidate((u32)&log->cmd_rdy, 32);
	IPC_MSG_STRUCT ipc_msg_temp;
	//if(log->cmd_rdy != 0)
	//return -1;

	int cmd_len = strlen(cmd_str);

	if (cmd_len > LOGBUF_SIZE - 1) {
		cmd_len = LOGBUF_SIZE - 1;
	}

	strncpy((char *)log->buf, cmd_str, cmd_len);
	log->buf[cmd_len] = 0;

	//log->cmd_rdy = 1;
	DCache_CleanInvalidate((u32)log, sizeof(logbuf_t));


	ipc_msg_temp.msg_type = IPC_USER_POINT;
	ipc_msg_temp.msg = (u32)log;
	ipc_msg_temp.msg_len = 1;
	ipc_msg_temp.rsvd = 0;

	ipc_send_message(ipc_dir, ipc_ch, &ipc_msg_temp);
	return 0;
}

int shm_get_logbuf(logbuf_t *log, char *cmd_str)
{
	DCache_Invalidate((u32)log, sizeof(logbuf_t));

	if (log->cmd_rdy == 0) {
		return 0;
	}

	strcpy(cmd_str, (char *)log->buf);

	log->cmd_rdy = 0;

	DCache_CleanInvalidate((u32)&log->cmd_rdy, 32);
	return strlen(cmd_str);
}
#else //SHELL_IPC==0
//uint8_t __fake_shm_ssram_start__[2048];

//shm_ctrl_t *shm_ctrl = (shm_ctrl_t*)__fake_shm_ssram_start__;
extern uint8_t __shm_ssram_start__[];
shm_ctrl_t *shm_ctrl = (shm_ctrl_t *)__shm_ssram_start__;
int shm_put_logbuf(logbuf_t *log, char *cmd_str)
{
	// sync ready
	DCache_Invalidate((u32)&log->cmd_rdy, 32);

	if (log->cmd_rdy != 0) {
		return -1;
	}

	int cmd_len = strlen(cmd_str);

	if (cmd_len > LOGBUF_SIZE - 1) {
		cmd_len = LOGBUF_SIZE - 1;
	}

	strncpy((char *)log->buf, cmd_str, cmd_len);
	log->buf[cmd_len] = 0;

	log->cmd_rdy = 1;

	DCache_CleanInvalidate((u32)log, sizeof(logbuf_t));
	return 0;
}

int shm_get_logbuf(logbuf_t *log, char *cmd_str)
{
	DCache_Invalidate((u32)log, sizeof(logbuf_t));

	if (log->cmd_rdy == 0) {
		return 0;
	}

	strcpy(cmd_str, (char *)log->buf);

	log->cmd_rdy = 0;

	DCache_CleanInvalidate((u32)&log->cmd_rdy, 32);
	return strlen(cmd_str);
}
#endif // SHELL_IPC


#if defined(CONFIG_ARM_CORE_CM4_NP)
void shell_loguartRx_dispatch(void)
{
	u32 i, CpuId, cmd_done = FALSE;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	for (i = 0; i < pUartLogBuf->UARTLogBufLen; i++) {
		if (pUartLogBuf->UARTLogBuf[i] != ' ') {
			if (pUartLogBuf->UARTLogBuf[i] == '\0') {
				CpuId = NP_CPU_ID;
				CONSOLE_AMEBA(); /* '\0' put # */
				cmd_done = TRUE;
				break;
			}

			if (pUartLogBuf->UARTLogBuf[i] == PROMPT_VP) {
				i = i + 1; /* remove flag */
				CpuId = VP_CPU_ID;
			} else if (pUartLogBuf->UARTLogBuf[i] == PROMPT_FP) {
				i = i + 1; /* remove flag */
				CpuId = FP_CPU_ID;
			} else if (pUartLogBuf->UARTLogBuf[i] == PROMPT_ANP) {
				i = i + 1; /* remove flag */
#if CMD_MASTER==MASTER_IS_NP
				CpuId = AP_CPU_ID;
#else
				CpuId = NP_CPU_ID;
#endif
			} else if (pUartLogBuf->UARTLogBuf[i] == '>' && pUartLogBuf->UARTLogBuf[i + 2] == ' ') {
				switch (pUartLogBuf->UARTLogBuf[i + 1]) {
				case '1':
				case 'V':
					CpuId = VP_CPU_ID;
					break;
				case '2':
				case 'F':
					CpuId = FP_CPU_ID;
					break;
				case '3':
				case 'A':
					CpuId = AP_CPU_ID;
					break;
				case '0':
				case 'N':
				default:
					CpuId = NP_CPU_ID;
					break;
				}
				i = i + 3;
			} else {
#if CMD_MASTER==MASTER_IS_NP
				CpuId = NP_CPU_ID;
#else
				CpuId = AP_CPU_ID;
#endif
			}

			/* avoid useless space */
			memcpy(&pUartLogBuf->UARTLogBuf[0], &pUartLogBuf->UARTLogBuf[i], UART_LOG_CMD_BUFLEN - i);
			break;
		}
	}

	switch (CpuId) {
	case VP_CPU_ID:
		//DiagPrintf("put to vp buf, %d, %s\n", pUartLogBuf->UARTLogBufLen, pUartLogBuf->UARTLogBuf);
#if SHELL_IPC==1
		shm_put_logbuf(&shm_ctrl->log_vp, (char *)pUartLogBuf->UARTLogBuf, IPC_NP_TO_VP, IPC_N2V_SHELL_COMMAND);
#else
		shm_put_logbuf(&shm_ctrl->log_vp, (char *)pUartLogBuf->UARTLogBuf);
#endif
		cmd_done = TRUE;
		break;
	case FP_CPU_ID:
#if SHELL_IPC==1
		shm_put_logbuf(&shm_ctrl->log_fp, (char *)pUartLogBuf->UARTLogBuf, IPC_NP_TO_FP, IPC_N2F_SHELL_COMMAND);
#else
		shm_put_logbuf(&shm_ctrl->log_fp, (char *)pUartLogBuf->UARTLogBuf);
#endif
		cmd_done = TRUE;
		break;
	case AP_CPU_ID:
#if SHELL_IPC==1
		shm_put_logbuf(&shm_ctrl->log_ap, (char *)pUartLogBuf->UARTLogBuf, IPC_NP_TO_AP, IPC_N2A_SHELL_COMMAND);
#else
		shm_put_logbuf(&shm_ctrl->log_ap, (char *)pUartLogBuf->UARTLogBuf);
#endif
		cmd_done = TRUE;
		break;
	}

	if (cmd_done) {
		shell_array_init((u8 *)pUartLogBuf->UARTLogBuf, pUartLogBuf->UARTLogBufLen, '\0');
		pUartLogBuf->BufCount = 0;
		shell_ctl.ExecuteCmd = _FALSE;
	}
}

void shell_shm_init(void)
{
	memset(shm_ctrl, 0, sizeof(shm_ctrl_t));
	DCache_CleanInvalidate((u32)shm_ctrl, sizeof(shm_ctrl_t));

}
#else // defined(CONFIG_ARM_CORE_CM4_NP)
void shell_loguartRx_dispatch(void)
{

}

void shell_shm_rx(void)
{
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;
#if defined(CONFIG_ARM_CORE_CM4_VP)
	pUartLogBuf->UARTLogBufLen = shm_get_logbuf(&shm_ctrl->log_vp, (char *)pUartLogBuf->UARTLogBuf);
#elif defined(CONFIG_ARM_CORE_CM0)
	pUartLogBuf->UARTLogBufLen = shm_get_logbuf(&shm_ctrl->log_fp, (char *)pUartLogBuf->UARTLogBuf);
#elif defined(CONFIG_ARM_CORE_CA32)
	pUartLogBuf->UARTLogBufLen = shm_get_logbuf(&shm_ctrl->log_ap, (char *)pUartLogBuf->UARTLogBuf);
#endif

	//DiagPrintf("SHM %d, %s\n\r", pUartLogBuf->UARTLogBufLen, pUartLogBuf->UARTLogBuf);
	if (pUartLogBuf->UARTLogBufLen != 0) {
		shell_ctl.ExecuteCmd = _TRUE;
		if (shell_ctl.shell_task_rdy) {
			shell_ctl.GiveSema();
		}
	}
}

#if SHELL_IPC == 0
static void shell_shm_rx_polling(void *Data)
{
	/* To avoid gcc warnings */
	(void) Data;

	while (1) {
		shell_shm_rx();
		rtos_time_delay_ms(50);
	}
}
#endif

void shell_shm_init(void)
{
#if SHELL_IPC == 0
	if (SUCCESS != rtos_task_create(NULL, "LOGUART_SHM", shell_shm_rx_polling, NULL, SHELL_STACK_SIZE, 5)) {
		DiagPrintf("Create Log UART SHM Task Err!!\n");
	}
#endif
}
#endif //defined(CONFIG_ARM_CORE_CM4_NP)

#if defined(CONFIG_ARM_CORE_CM4_NP)
#define CPUNAME "NP"
#define ECHO_CMD(cmd)
#elif defined(CONFIG_ARM_CORE_CM4_VP)
#define CPUNAME "VP"
#define ECHO_CMD(cmd) DiagPrintf("Exec on VP > %s\n", (const char *)(cmd));
#elif defined(CONFIG_ARM_CORE_CM0)
#define CPUNAME "FP"
#define ECHO_CMD(cmd) DiagPrintf("Exec on FP > %s\n", (const char *)(cmd));
#elif defined(CONFIG_ARM_CORE_CA32)
#define CPUNAME "AP"
#define ECHO_CMD(cmd) DiagPrintf("Exec on AP > %s\n", (const char *)(cmd));
#endif


static void shell_task_ram(void *Data)
{
	/* To avoid gcc warnings */
	(void) Data;
	u32 ret = TRUE;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	//4 Set this for UartLog check cmd history
	shell_ctl.shell_task_rdy = 1;

	rtos_create_secure_context(RTOS_MINIMAL_SECURE_STACK_SIZE);

	do {
		rtos_sema_take(shell_sema, RTOS_MAX_DELAY);

		shell_loguartRx_dispatch();

		if (shell_ctl.ExecuteCmd) {
			//ECHO_CMD(pUartLogBuf->UARTLogBuf);
#if ATCMD_ENABLE==1
			ret = atcmd_service((char *)pUartLogBuf->UARTLogBuf);

#if MPCMD_ENABLE==1
			if (ret == FALSE) {
				ret = mp_command_handler((char *)pUartLogBuf->UARTLogBuf);
			}
#endif
#endif
			if (ret == FALSE) {
				if (shell_cmd_exec_ram(pUartLogBuf->UARTLogBuf) == FALSE) {
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\nunknown command '%s'", pUartLogBuf->UARTLogBuf);
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n\n#\r\n");
				}
			}

			shell_array_init((u8 *)pUartLogBuf->UARTLogBuf, pUartLogBuf->UARTLogBufLen, '\0');
			pUartLogBuf->BufCount = 0;
			shell_ctl.ExecuteCmd = _FALSE;
		}
	} while (1);
}

void shell_init_ram(void)
{
#if ATCMD_ENABLE==1
	atcmd_service_init();
#endif

	shell_ctl.pCmdTbl = (PCOMMAND_TABLE)__cmd_table_start__;
	shell_ctl.CmdTblSz = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));

	shell_ctl.ExecuteCmd = FALSE;
	shell_ctl.ExecuteEsc = TRUE; //don't check Esc anymore
	shell_ctl.GiveSema = shell_give_sema;

	/* Create a Semaphone */
	rtos_sema_create_binary(&shell_sema);

#if defined (CONFIG_WHC_HOST)
	if (SUCCESS != rtos_task_create(NULL, "LOGUART_TASK", shell_task_ram, NULL, SHELL_STACK_SIZE * 5,
									5)) {
		DiagPrintf("%s Create Log UART Task Err!!\n", CPUNAME);
	}
#else
	if (SUCCESS != rtos_task_create(NULL, "LOGUART_TASK", shell_task_ram, NULL, SHELL_STACK_SIZE,
									5)) {
		DiagPrintf("%s Create Log UART Task Err!!\n", CPUNAME);
	}
#endif

#if SHELL_IPC == 0
	shell_shm_init();
#endif
	//CONSOLE_AMEBA();
}
