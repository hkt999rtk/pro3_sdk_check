/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPC_API_H
#define _AMEBA_IPC_API_H

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC
  * @brief IPC driver modules
* @{
*/
/* Exported types --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Types IPC Exported Types
  * @{
  */
/**
  * @brief IPC Message Definition
 */
typedef struct ipc_msg_struct {
	u32 msg_type;
	u32 msg;
	u32 msg_len;
	u32 rsvd;
} IPC_MSG_STRUCT, *PIPC_MSG_STRUCT, ipc_msg_struct_t;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */
/** @defgroup IPC_RETURN_VALUE
  * @{
  */
#define IPC_SEMA_TIMEOUT -2
#define IPC_REQ_TIMEOUT -1
#define IPC_SEND_SUCCESS 0
#define IPC_SEND_TIMEOUT 1
#define IPC_SEMA_MAX_DELAY			0xFFFFFFFF

/**
  * @}
  */
/**
  * @}
  */

/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
void ipc_table_init(IPC_TypeDef *IPCx);
u32 ipc_send_message(u32 IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg);
PIPC_MSG_STRUCT ipc_get_message(u32 IPC_Dir, u8 IPC_ChNum);
/* TX-empty interrupts are driver-owned: IPC_wait_idle opens one only while a sender waits on that
 * channel, and IPC_INTHandler then wakes the waiter itself (IPC_TXHandler with the bank being served).
 * A custom Txfunc (anything but IPC_TXHandler) is called after that with its own TxIrqData, in
 * interrupt context, and only for sends that had to wait: it is not a per-message "TX done" callback.
 * IPC_TXHandler itself only accepts this core's IPC dev (bank 0) or dev + 0x100 (bank 1) as Data. */
void IPC_TXHandler(void *Data, u32 IrqStatus, u32 ChanNum);
u32 IPC_wait_idle_NonOS(IPC_TypeDef *IPCx, u32 IPC_ChNum, u32 max_ms);
u32 IPC_wait_idle(IPC_TypeDef *IPCx, u32 IPC_GroupShift, u32 IPC_ChNum, u32 budget_ms);
extern IPC_IRQ_FUN IPC_IrqHandler[IPC_CHANNEL_NUM];
u32 IPC_SEMGetId(void);

#endif
