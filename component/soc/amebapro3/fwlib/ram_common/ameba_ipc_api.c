/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "ameba_ipc_api.h"

static const char *TAG = "IPC";

#define IPC_SEND_BUDGET_MS		5000	/* total wall clock one ipc_send_message may spend waiting */
#define IPC_NONOS_WAIT_MS		1000	/* hardware poll limit when the semaphore path is not used (no Txfunc, critical section, PMU suspend) */
#ifdef CONFIG_KERNEL
rtos_sema_t ipc_Semaphore[IPC_TX_CHANNEL_NUM];
#endif

//#if defined (CONFIG_ARM_CORE_CA32)
//#define DCache_Invalidate(x, y)
//#define DCache_CleanInvalidate(x, y)
//#endif
/**
  * @brief  init ipc interrupt handler table.
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @retval   None
  */
void ipc_table_init(IPC_TypeDef *IPCx)
{
	u32 i;
	u32 IPC_Dir;
	u32 IPC_ChNum;
	u32 IPC_GroupShift;
	u32 IPC_ChShift;
	u32 IPC_IMR;
	u32 IPC_IntMode;
	u32 IPC_Addr_offset = 0;//For the extra channel
	IPC_TypeDef *IPCxTemp = IPCx;
#if defined ( __ICCARM__ )
	__ipc_table_start__ = (u8 *)__section_begin(".ipc.table.data");
	__ipc_table_end__ = (u8 *)__section_end(".ipc.table.data");
#endif
	IPC_INIT_TABLE *ipc_init_table = (IPC_INIT_TABLE *)(void *)__ipc_table_start__;
	u32 ipc_num = ((__ipc_table_end__ - __ipc_table_start__) / sizeof(IPC_INIT_TABLE));
	for (i = 0; i < ipc_num; i++) {
		IPC_Dir = ipc_init_table[i].IPC_Direction;
		IPC_ChNum = ipc_init_table[i].IPC_Channel;
		/* check ipc entry has register on a correct cpu */
		if (IS_SEND_TO_NP(IPC_Dir)) {
			IPC_IntMode = (IPCx == IPCNP_DEV) ? IPC_RX_FULL : IPC_TX_EMPTY;
		} else if (IS_SEND_TO_VP(IPC_Dir)) {
			IPC_IntMode = (IPCx == IPCVP_DEV) ? IPC_RX_FULL : IPC_TX_EMPTY;
		} else if (IS_SEND_TO_AP(IPC_Dir)) {
			IPC_IntMode = (IPCx == IPCAP_DEV) ? IPC_RX_FULL : IPC_TX_EMPTY;
		} else if (IS_SEND_TO_FP(IPC_Dir)) {
			IPC_IntMode = (IPCx == IPCFP_DEV) ? IPC_RX_FULL : IPC_TX_EMPTY;
		} else {
			continue;
		}

		if (IPCx == IPCNP_DEV) {
			if (IPC_Dir == IPC_NP_TO_FP || IPC_Dir == IPC_FP_TO_NP) {
				IPC_GroupShift = 0x00;
			} else if (IPC_Dir == IPC_NP_TO_AP || IPC_Dir == IPC_AP_TO_NP) {
				IPC_GroupShift = 0x01;
			} else {
				IPC_GroupShift = 0x02;
			}
		} else if (IPCx == IPCVP_DEV) {
			if (IPC_Dir == IPC_VP_TO_NP || IPC_Dir == IPC_NP_TO_VP) {
				IPC_GroupShift = 0x00;
			} else if (IPC_Dir == IPC_VP_TO_AP || IPC_Dir == IPC_AP_TO_VP) {
				IPC_GroupShift = 0x01;
			} else {
				IPC_GroupShift = 0x02;
			}
		} else if (IPCx == IPCAP_DEV) {
			if (IPC_Dir == IPC_AP_TO_FP || IPC_Dir == IPC_FP_TO_AP) {
				IPC_GroupShift = 0x00;
			} else if (IPC_Dir == IPC_AP_TO_NP || IPC_Dir == IPC_NP_TO_AP) {
				IPC_GroupShift = 0x01;
			} else {
				IPC_GroupShift = 0x02;
			}
		} else if (IPCx == IPCFP_DEV) {
			if (IPC_Dir == IPC_FP_TO_NP || IPC_Dir == IPC_NP_TO_FP) {
				IPC_GroupShift = 0x00;
			} else if (IPC_Dir == IPC_FP_TO_AP || IPC_Dir == IPC_AP_TO_FP) {
				IPC_GroupShift = 0x01;
			} else {
				IPC_GroupShift = 0x02;
			}
		}
		if (IPC_ChNum >= 8) {
			IPC_Addr_offset = 0x100;
			IPCx = (IPC_TypeDef *)((u8 *)(IPCxTemp) + IPC_Addr_offset);
		} else {
			IPC_Addr_offset = 0x00;
			IPCx = (IPC_TypeDef *)(IPCxTemp + IPC_Addr_offset);
		}

		IPC_IMR = IPCx->IPC_IMR[IPC_GroupShift];
		if (IPC_IntMode == IPC_RX_FULL) {
			IPC_ChShift = IPC_ChNum % 8 + 0x08;
		} else {
			IPC_ChShift = IPC_ChNum % 8;
		}

		if (IPC_IMR & BIT(IPC_ChShift)) {
			/* overwrite the old call back func for cpu reset happens */
			RTK_LOGI(TAG, "Channel Conflict for CPU %d Channel %d ! Ignore If CPU Has Reset\r\n", (int)SYS_CPUID(), (int)IPC_ChShift);
			//continue; //Check for workaround
		}

		if (IPC_IntMode == IPC_RX_FULL) {
			/* enable rx intr and register call back func*/
			IPC_INTUserHandler(IPCx, IPC_GroupShift, IPC_ChShift, (void *)ipc_init_table[i].Rxfunc, ipc_init_table[i].RxIrqData);
		} else {
			/* enable tx intr and register call back func*/
			IPC_INTUserHandler(IPCx, IPC_GroupShift, IPC_ChShift, (void *)ipc_init_table[i].Txfunc, ipc_init_table[i].TxIrqData);
		}
		IPCx = IPCxTemp;
	}
}

/**
  * @brief  The common IPC Tx interrupt handler
  * @param  Data: the data pointer to IPCx
  * @param  IPC_GroupShift : 0~2. For different core.
  * @param  ChanNum: ChanNum
  */
void IPC_TXHandler(void *Data, u32 IPC_GroupShift, u32 ChanNum)
{
	UNUSED(Data);
	//UNUSED(IrqStatus);

	IPC_TypeDef *IPCx = (IPC_TypeDef *)Data;
	IPC_TypeDef *IPCx_DEV = IPC_GetDevById(0x00);
	u32 ipc_extra_channel = 0;
	/* Data must be this core's bank dev (+0x100 for ch 8~15); NULL or a user context would be used as a register base */
	if (IPCx != IPCx_DEV && IPCx != (IPC_TypeDef *)((u8 *)IPCx_DEV + 0x100)) {
		static u8 bad_dev_reported;
		if (!bad_dev_reported) {
			bad_dev_reported = 1;
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "IPC_TXHandler: Data 0x%08x is not an IPC bank\r\n", (unsigned int)(u32)Data);
		}
		return;
	}
	/* bits 8~15 are RX full (receiver side): no semaphore, not ours to disable */
	if (ChanNum >= 8) {
		static u8 reported;
		if (!reported) {
			reported = 1;
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "IPC_TXHandler: bad ChanNum %d\r\n", ChanNum);
		}
		return;
	}
	IPC_INTConfig(IPCx, IPC_GroupShift, ChanNum, DISABLE);
	if (IPCx != IPCx_DEV) {
		ipc_extra_channel = 8;
	}
#ifdef CONFIG_KERNEL
	if (ipc_Semaphore[IPC_GroupShift * 16 + ChanNum + ipc_extra_channel] != NULL) {
		rtos_sema_give(ipc_Semaphore[IPC_GroupShift * 16 + ChanNum + ipc_extra_channel]);
	}
#endif
	(void)ipc_extra_channel;
}

u32 IPC_wait_idle_NonOS(IPC_TypeDef *IPCx, u32 IPC_ChNum, u32 max_ms)
{
	/* TIM0 runs from ROM boot; unlike a loop count, this bounds real time.
	 * 1000ms preserves the worst-case budget of the loop count this replaces:
	 * 10M iterations take ~1s on FP (KM0 @40MHz), ~135ms on NP/VP (@296MHz)
	 * and ~40ms on AP (CA32 @1GHz). max_ms: the caller's limit, at most the remaining send budget. */
	u32 start = SYSTIMER_TickGet();
	u32 timeout_ms = CPU_InInterrupt() ? 1 : max_ms;

	while (IPCx->IPC_TX_DATA & BIT(IPC_ChNum)) {
		if (SYSTIMER_GetPassTime(start) >= timeout_ms) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, " IPC Request Timeout\r\n");
			return IPC_REQ_TIMEOUT;
		}
	}
	return 0;
}

/**
  * @brief  Processing functions when the IPC channel is occupied
  * @param  IPCx: where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @param  IPC_ChNum: IPC_ChNum
  * @retval IPC_REQ_TIMEOUT or IPC_SEMA_TIMEOUT or SUCCESS
  */
u32 IPC_wait_idle(IPC_TypeDef *IPCx, u32 IPC_GroupShift, u32 IPC_ChNum, u32 budget_ms)
{
	u32 ipc_extra_channel = 0;
	u32 can_yield = 1;
	u32 poll_ms = (budget_ms < IPC_NONOS_WAIT_MS) ? budget_ms : IPC_NONOS_WAIT_MS;

	if (IPCx != IPC_GetDevById(0x00)) {
		ipc_extra_channel = 8;
	}

	/* sema_take cannot block in PMU suspend or a critical section: poll the hardware instead */
	if (!pmu_yield_os_check()) {
		can_yield = 0;
	}
#ifdef CONFIG_KERNEL
	if (rtos_get_critical_state()) {
		can_yield = 0;
	}
#endif
	/* IPC_INTHandler gives the semaphore on every TX empty, whatever Txfunc is registered */
	if (!CPU_InInterrupt() && can_yield && (IPC_IrqHandler[IPC_GroupShift * 32 + IPC_ChNum + (ipc_extra_channel ? 16 : 0)] != NULL)) {
#ifdef CONFIG_KERNEL
		/* Avoid muiti-task use same channel */
		if (ipc_Semaphore[IPC_GroupShift * 16 + IPC_ChNum + ipc_extra_channel] == NULL) {
			rtos_sema_create_binary(&ipc_Semaphore[IPC_GroupShift * 16 + IPC_ChNum + ipc_extra_channel]);
		}
		/* allocation failed: bounded poll below instead of taking a NULL handle */
		if (ipc_Semaphore[IPC_GroupShift * 16 + IPC_ChNum + ipc_extra_channel] == NULL) {
			return IPC_wait_idle_NonOS(IPCx, IPC_GroupShift * 8 + IPC_ChNum, poll_ms);
		}
		/* clear stale TX empty status left by previous sends */
		IPC_INTClear(IPCx, IPC_GroupShift, IPC_ChNum);
		u32 start = SYSTIMER_TickGet();
		while (IPCx->IPC_TX_DATA & BIT(IPC_GroupShift * 8 + IPC_ChNum)) {
			if (SYSTIMER_GetPassTime(start) >= budget_ms) {
				RTK_LOGS(TAG, RTK_LOG_ALWAYS, " IPC Get Semaphore Timeout\r\n");
				IPC_INTConfig(IPCx, IPC_GroupShift, IPC_ChNum, DISABLE);
				return IPC_SEMA_TIMEOUT;
			}
			/* 1ms slice: the give may be stale or taken by another waiter */
			IPC_INTConfig(IPCx, IPC_GroupShift, IPC_ChNum, ENABLE);
			rtos_sema_take(ipc_Semaphore[IPC_GroupShift * 16 + IPC_ChNum + ipc_extra_channel], 1);
		}
		IPC_INTConfig(IPCx, IPC_GroupShift, IPC_ChNum, DISABLE);
		/* seen idle within budget_ms: done. If another sender takes the channel before
		 * ipc_send_message rechecks it, that recheck waits again on the remaining budget */
		return 0;
#endif
		(void)ipc_extra_channel;
	}
	return IPC_wait_idle_NonOS(IPCx, IPC_GroupShift * 8 + IPC_ChNum, poll_ms);
}

/**
  * @brief  exchange messages between KM0 and KM4.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_NP_TO_FP: NP send request to FP
  *		 		@arg IPC_NP_TO_AP: NP send request to AP
  *		 		@arg IPC_NP_TO_VP: NP send request to VP
  *		 		@arg IPC_VP_TO_NP: VP send request to NP
  *		 		@arg IPC_VP_TO_AP: VP send request to AP
  *		 		@arg IPC_VP_TO_FP: VP send request to FP
  *		 		@arg IPC_AP_TO_NP: AP send request to NP
  *		 		@arg IPC_AP_TO_VP: AP send request to VP
  *		 		@arg IPC_AP_TO_FP: AP send request to FP
  *		 		@arg IPC_FP_TO_NP: FP send request to NP
  *		 		@arg IPC_FP_TO_VP: FP send request to VP
  *		 		@arg IPC_FP_TO_AP: FP send request to AP
  * @param  IPC_ChNum: the IPC channel number.
  * @param  Message: pointer to the message to be exchanged,and should not stored in stack.
  * @retval   None
  */
u32 ipc_send_message(u32 IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg)
{
	u32 IPC_GroupShift = 0;
	/* Check the parameters */
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	assert_param(IS_IPC_VALID_CHNUM(IPC_ChNum));

	PIPC_MSG_STRUCT IPC_MSG = (PIPC_MSG_STRUCT)__ipc_ssram_start__;
	u32 msg_idx = IPC_Dir * 16 + IPC_ChNum;

	IPC_TypeDef *IPCx = IPC_GetDevById(0x00);

	if (IPCx == IPCNP_DEV) {
		if (IPC_Dir == IPC_NP_TO_FP || IPC_Dir == IPC_FP_TO_NP) {
			IPC_GroupShift = 0x00;
		} else if (IPC_Dir == IPC_NP_TO_AP || IPC_Dir == IPC_AP_TO_NP) {
			IPC_GroupShift = 0x01;
		} else {
			IPC_GroupShift = 0x02;
		}
	} else if (IPCx == IPCVP_DEV) {
		if (IPC_Dir == IPC_VP_TO_NP || IPC_Dir == IPC_NP_TO_VP) {
			IPC_GroupShift = 0x00;
		} else if (IPC_Dir == IPC_VP_TO_AP || IPC_Dir == IPC_AP_TO_VP) {
			IPC_GroupShift = 0x01;
		} else {
			IPC_GroupShift = 0x02;
		}
	} else if (IPCx == IPCAP_DEV) {
		if (IPC_Dir == IPC_AP_TO_FP || IPC_Dir == IPC_FP_TO_AP) {
			IPC_GroupShift = 0x00;
		} else if (IPC_Dir == IPC_AP_TO_NP || IPC_Dir == IPC_NP_TO_AP) {
			IPC_GroupShift = 0x01;
		} else {
			IPC_GroupShift = 0x02;
		}
	} else if (IPCx == IPCFP_DEV) {
		if (IPC_Dir == IPC_FP_TO_NP || IPC_Dir == IPC_NP_TO_FP) {
			IPC_GroupShift = 0x00;
		} else if (IPC_Dir == IPC_FP_TO_AP || IPC_Dir == IPC_AP_TO_FP) {
			IPC_GroupShift = 0x01;
		} else {
			IPC_GroupShift = 0x02;
		}
	}
	u32 IPC_Addr_Offset = 0x00;
	if (IPC_ChNum >= 8) {
		IPC_Addr_Offset = 0x100;
		IPCx = (IPC_TypeDef *)((u8 *)(IPCx) + IPC_Addr_Offset);
	}


	u32 IPC_Ch_Set_Bit = IPC_ChNum % 8 + IPC_GroupShift * 8;
	u32 PrevIrqStatus;
	u32 send_start = SYSTIMER_TickGet();
	while (1) {
		if (IPCx->IPC_TX_DATA & BIT(IPC_Ch_Set_Bit)) {
			u32 used = SYSTIMER_GetPassTime(send_start);
			if (used >= IPC_SEND_BUDGET_MS || IPC_wait_idle(IPCx, IPC_GroupShift, IPC_ChNum % 8, IPC_SEND_BUDGET_MS - used)) {
				return IPC_SEND_TIMEOUT;
			}
		}

		/* disable irq */
		PrevIrqStatus = irq_disable_save();
		if ((IPCx->IPC_TX_DATA & BIT(IPC_Ch_Set_Bit)) == 0) {
			break;
		}
		/* another task sent first: wait again */
		irq_enable_restore(PrevIrqStatus);
		if (SYSTIMER_GetPassTime(send_start) >= IPC_SEND_BUDGET_MS) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "IPC_Dir:%d, IPC_TX_DATA 0x%08x Not Clean!\r\n", IPC_Dir, BIT(IPC_Ch_Set_Bit));
			return IPC_SEND_TIMEOUT;
		}
	}

	if (IPC_Msg) {
		memcpy(&IPC_MSG[msg_idx], IPC_Msg, sizeof(IPC_MSG_STRUCT));
		DCache_Clean((u32)&IPC_MSG[msg_idx], sizeof(IPC_MSG_STRUCT));
	}

	IPCx->IPC_TX_DATA = BIT(IPC_Ch_Set_Bit);

	/* restore irq */
	irq_enable_restore(PrevIrqStatus);

	return IPC_SEND_SUCCESS;
}

/**
  * @brief  get ipc message.
  * @param  IPC_Dir: Specifies core to core direction
  *          This parameter can be one of the following values:
  *		 		@arg IPC_NP_TO_FP: NP send request to FP
  *		 		@arg IPC_NP_TO_AP: NP send request to AP
  *		 		@arg IPC_NP_TO_VP: NP send request to VP
  *		 		@arg IPC_VP_TO_NP: VP send request to NP
  *		 		@arg IPC_VP_TO_AP: VP send request to AP
  *		 		@arg IPC_VP_TO_FP: VP send request to FP
  *		 		@arg IPC_AP_TO_NP: AP send request to NP
  *		 		@arg IPC_AP_TO_VP: AP send request to VP
  *		 		@arg IPC_AP_TO_FP: AP send request to FP
  *		 		@arg IPC_FP_TO_NP: FP send request to NP
  *		 		@arg IPC_FP_TO_VP: FP send request to VP
  *		 		@arg IPC_FP_TO_AP: FP send request to AP
  * @param  IPC_ChNum: the IPC channel number.
  * @retval  : pointer to the message to be exchanged.
  * @note for data massage, corresponding data cache should be invalidate before access.
  */
PIPC_MSG_STRUCT ipc_get_message(u32 IPC_Dir, u8 IPC_ChNum)
{
	PIPC_MSG_STRUCT IPC_MSG = NULL;
	u32 msg_idx = IPC_Dir * 16 + IPC_ChNum;
	IPC_MSG = (PIPC_MSG_STRUCT)(void *)__ipc_ssram_start__;
	DCache_Invalidate((u32)&IPC_MSG[msg_idx], sizeof(IPC_MSG_STRUCT));
	return &IPC_MSG[msg_idx];
}
