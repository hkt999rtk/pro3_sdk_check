/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "ameba_ipc_api.h"

static const char *TAG = "IPC";
IPC_IRQ_FUN IPC_IrqHandler[IPC_CHANNEL_NUM];
void *IPC_IrqData[IPC_CHANNEL_NUM];

//static u8 PXID_Idx[32] = {1, 1, 1, 1,1, 1, 1, 1,1, 1, 1, 1,1, 1, 1, 1,1, 1, 1, 1,1, 1, 1, 1,1, 1, 1, 1,1, 1, 1, 1};

void (*ipc_delay)(uint32_t);

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC
  * @brief  IPC driver modules
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */

/**
  * @brief  Enables or disables the specified IPC Channel interrupts.
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @param  IPC_Shiftbit: 0 ~ 15. For rx/tx channel
  * @param  NewState: DISABLE/ENABLE.
  * @retval   None
  */
void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u8 IPC_Shiftbit, u32 NewState)
{
	/* IMR read-modify-write is shared by task and ISR; local irq mask is enough while every core runs single-core (SUPPORT_SMP=n).
	 * The saved state is restored, not forced on: called from an ISR it hands back whatever mask the ISR already had. */
	u32 PrevIrqStatus = irq_disable_save();

	if (ENABLE == NewState) {
		IPCx->IPC_IMR[IPC_GroupShift] |= (BIT(IPC_Shiftbit));
	} else {
		IPCx->IPC_IMR[IPC_GroupShift] &= ~(BIT(IPC_Shiftbit));
	}
	irq_enable_restore(PrevIrqStatus);
}

/**
  * @brief  set IMR of specified IPC Channel interrupts.
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @param  IPC_Chs: the Channels that want to be enable.
  * @retval   None
  */
void IPC_IERSet(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u32 IPC_Chs)
{
	IPCx->IPC_IMR[IPC_GroupShift] = IPC_Chs;
}

/**
  * @brief  get IMR of specified IPC Channel interrupts.
  * @param  IPCx: where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @retval The IMR status of specified IPC
  */
u32 IPC_IERGet(IPC_TypeDef *IPCx, u8 IPC_GroupShift)
{
	return IPCx->IPC_IMR[IPC_GroupShift];
}

/**
  * @brief  Request a core-to-core interrupt.
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
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
  * @param  IPC_ChNum: 0 ~ 7.
  * @retval   0/1
  */
u32 IPC_INTRequest(IPC_TypeDef *IPCx, u32 IPC_Dir, u8 IPC_ChNum)
{
	/* Check the parameters */
	assert_param(IS_IPC_DIR_MODE(IPC_Dir));
	u32 ipc_shift = 0;

	switch (IPC_Dir) {
	case IPC_NP_TO_AP:
	case IPC_VP_TO_AP:
	case IPC_FP_TO_AP:
	case IPC_AP_TO_NP:
		ipc_shift = 8;
		break;
	case IPC_NP_TO_VP:
	case IPC_VP_TO_FP:
	case IPC_FP_TO_VP:
	case IPC_AP_TO_VP:
		ipc_shift = 16;
		break;

	default:
		break;
	}

	if (IPCx->IPC_TX_DATA & (BIT(IPC_ChNum + ipc_shift))) {
		return 0;
	} else {
		IPCx->IPC_TX_DATA = (BIT(IPC_ChNum + ipc_shift));
		return 1;
	}
}

/**
  * @brief  Get core-to-core interrupt status.
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @retval  tx_empty or rx_full interrupt status
  */
u32 IPC_INTGet(IPC_TypeDef *IPCx, u8 IPC_GroupShift)
{
	return IPCx->IPC_ISR[IPC_GroupShift];
}

/**
  * @brief  Clear a core-to-core interrupt.
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @param  IPC_Shiftbit: 0 ~ 15.
  * @retval   None
  */
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u8 IPC_Shiftbit)
{
	IPCx->IPC_ISR[IPC_GroupShift] = BIT(IPC_Shiftbit);
}

/**
  * @brief  The common IPC interrupt handler
  * @param  Data: the data pointer to IPCx
  * @retval 0
  */
u32 IPC_INTHandler(void *Data)
{
	IPC_TypeDef *IPCx = (IPC_TypeDef *)Data;
	IPC_TypeDef *IPCx_TEMP = (IPC_TypeDef *)Data;
	u32 IrqStatus;
	u32 i, j, k;

	for (i = 0 ; i < 3; i++) {
		for (k = 0; k < 2; k++) {
			if (k == 0) {
				IPCx_TEMP = (IPC_TypeDef *)((u8 *)(IPCx) + 0X00);
			} else if (k == 1) {
				IPCx_TEMP = (IPC_TypeDef *)((u8 *)(IPCx) + 0X100);
			}

			/* ISR bits are set even when masked, only handle enabled ones */
			IrqStatus = IPCx_TEMP->IPC_ISR[i] & IPCx_TEMP->IPC_IMR[i];

			for (j = 0; j < 16; j++) {
				if (IrqStatus & BIT(j)) {

					IPC_IRQ_FUN handler = IPC_IrqHandler[i * 32 + j + k * 16];

					if (j < 8) {	/* bit 0~7: TX empty (sender side) */
						/* wake the waiter with the bank being served, then the user Txfunc with its own TxIrqData */
						IPC_TXHandler(IPCx_TEMP, i, j);
						if (handler != NULL && handler != IPC_TXHandler) {
							handler(IPC_IrqData[i * 32 + j + k * 16], i, j);
						}
					} else if (handler != NULL) {	/* bit 8~15: RX full (receiver side) */
						handler(IPC_IrqData[i * 32 + j + k * 16], i, j);
					}

					IPCx_TEMP->IPC_ISR[i] = BIT(j);
				}
			}
		}
	}

	return 0;
}

/**
  * @brief  To register a user interrupt handler for a specified IPC channel
  * @param  where IPCx can be IPCNP_DEV,IPCVP_DEV,IPCAP_DEV and IPCFP_DEV
  * @param  IPC_GroupShift : 0~2. For different core.
  * @param  IPC_Shiftbit: 0 ~ 15.
  * @param  IrqHandler: The IRQ handler to be assigned to the specified IPC channel
  * @param  IrqData: The pointer will be pass the the IRQ handler
  * @retval None
  */
void IPC_INTUserHandler(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u8 IPC_Shiftbit, void *IrqHandler, void *IrqData)
{
	/* Check the parameters */
	if (IrqHandler == NULL) {
		return;
	}

	u32 ipc_extra_channel = 0;
	if (IPCx != IPC_GetDevById(0x00)) {
		ipc_extra_channel = 16;
	}

	RTK_LOGD(TAG, "%s %d %d\r\n", __FUNCTION__, __LINE__, IPC_GroupShift * 32 + IPC_Shiftbit + ipc_extra_channel);
	IPC_IrqHandler[IPC_GroupShift * 32 + IPC_Shiftbit + ipc_extra_channel] = (IPC_IRQ_FUN)IrqHandler;
	IPC_IrqData[IPC_GroupShift * 32 + IPC_Shiftbit + ipc_extra_channel] = IrqData;

	if (IS_IPC_RX_CHNUM(IPC_Shiftbit)) {
		IPC_INTConfig(IPCx, IPC_GroupShift, IPC_Shiftbit, ENABLE);
		RTK_LOGD(TAG, "IPC_GroupShift %d IPC_Shiftbit %d\r\n", IPC_GroupShift, IPC_Shiftbit);
	}
}

/**
  * @brief  Get core-to-core hardware semphone ID.
  * @param  None
  * @retval   SEMA_ID
  * @note  None
  */
u32 IPC_SEMGetId(void)
{
	u32 ipc_id = 0xff;
#if defined (CONFIG_ARM_CORE_CM4_NP)
	ipc_id = NP_IPC_PXID;
#elif defined(CONFIG_ARM_CORE_CM4_VP)
	ipc_id = VP_IPC_PXID;
#elif defined(CONFIG_ARM_CORE_CM0)
	ipc_id = FP_IPC_PXID;
#elif defined(CONFIG_ARM_CORE_CA32)
	ipc_id = AP_IPC_PXID;
#endif
	return ipc_id;
}
/**
  * @brief  Get core-to-core hardware semphone.
  * @param  SEM_Idx: 0~31.
  * @param  timeout: timeout to wait. 0 means never wait, 0xffffffff means waiting permanently.
  * @retval TRUE/FALSE
  * @note  None
  */
u32 IPC_SEMTake(IPC_SEM_IDX SEM_Idx, u32 timeout)
{
	uint32_t ipcs_group = 0;
	uint32_t ipcs_channel = 0;
	uint32_t ipcs_idx = 0;

	IPC_SEMA_TypeDef *IPC_SEMA = IPCSE_DEV;
	IPC_TypeDef *IPCx = IPCNP_DEV;

	if (SEM_Idx > 31) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "The SEM_Idx %d don't can't be zero or larger than 31\r\n", SEM_Idx);
		return FALSE;
	}

	do {
		if (IPC_SEMA->IPC_SEMA_INDEX[SEM_Idx] == 0x00) {
			break;
		} else {
			if (timeout == 0) {
				return FALSE;
			}

			if (ipc_delay) {
				ipc_delay(1);
			} else {
				DelayMs(1);
			}
			timeout--;
		}
	} while (1);

	ipcs_group = SEM_Idx / 8;
	ipcs_channel = SEM_Idx % 8;
	ipcs_idx = (IPCx->IPC_SEMx[ipcs_group] >> (4 * ipcs_channel)) & (0x0f);
	if (ipcs_idx == (IPC_SEMGetId())  && ((IPCx->IPC_SEMSTS >> SEM_Idx & 0x01) == 0x01)) {

	} else {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "wrong ipcs_idx %x\r\n", ipcs_idx);
		return FALSE;
	}
	return TRUE;
}

/**
  * @brief  Free core-to-core hardware semphone.
  * @param  SEM_Idx: 0~31.
  * @retval   TRUE/FALSE
  * @note  NONE
  */
u32 IPC_SEMFree(IPC_SEM_IDX SEM_Idx)
{
	uint32_t ipcs_group = 0;
	uint32_t ipcs_channel = 0;
	uint32_t ipcs_idx = 0;

	IPC_SEMA_TypeDef *IPC_SEMA = IPCSE_DEV;
	IPC_TypeDef *IPCx = IPCNP_DEV;

	if (SEM_Idx > 31) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "The SEM_Idx %d can't be zero or larger than 31\r\n", SEM_Idx);
		return FALSE;
	}

	ipcs_group = SEM_Idx / 8;
	ipcs_channel = SEM_Idx % 8;
	ipcs_idx = (IPCx->IPC_SEMx[ipcs_group] >> (4 * ipcs_channel)) & (0x0f);
	if (ipcs_idx == (IPC_SEMGetId())  && ((IPCx->IPC_SEMSTS >> SEM_Idx & 0x01) == 0x01)) {
		IPC_SEMA->IPC_SEMA_INDEX[SEM_Idx] = 0x01;
	} else {
		return FALSE;
	}

	return TRUE;
}

/**
  * @brief  Set delay stub function for ipc sema.
  * @param  pfunc: delay function.
  * @retval   None
  */
void IPC_SEMDelayStub(void (*pfunc)(uint32_t))
{
	ipc_delay = pfunc;
}

/**
  * @brief  Get IPC device.
  * @param  cpuid:
  *			0: CPU_NP
  *		 	1: CPU_VP
  *			2: CPU_AP
			3: CPU FP
  * @retval IPC device
  */
IPC_TypeDef *IPC_GetDevById(u32 cpu_id)
{
	/* Check the parameters */
	assert_param(IS_IPC_Valid_CPUID(cpu_id));
#ifdef CONFIG_ARM_CORE_CM4_NP
	return IPCNP_DEV;
#elif defined(CONFIG_ARM_CORE_CM4_VP)
	return IPCVP_DEV;
#elif defined(CONFIG_ARM_CORE_CA32)
	return IPCAP_DEV;
#elif defined(CONFIG_ARM_CORE_CM0)
	return IPCFP_DEV;
#else
	return NULL;
#endif
}

/** @} */

/** @} */
/** @} */
