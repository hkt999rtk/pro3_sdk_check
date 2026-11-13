/**
  ******************************************************************************
  * @file    amebapro3_vector.c
  * @author
  * @version V1.0.0
  * @date    2025-05-01
  * @brief   This file contains all the functions prototypes for the IRQ firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"

static const char *const TAG = "VECTOR";

void (*INT_SVCall_patch)(void);
void (*INT_PendSV_patch)(void);
void (*INT_SysTick_patch)(void);


IRQ_FUN         UserIrqFunTable[MAX_PERIPHERAL_IRQ_NUM];
Fault_Patch     PrePatchHandler;
Fault_Patch     PostPatchHandler;
u32             UserIrqDataTable[MAX_PERIPHERAL_IRQ_NUM];

extern void crash_Main(uint32_t *mstack, uint32_t lr_value, uint32_t fault_id);

__NO_RETURN
void INT_HardFault_C(uint32_t mstack[], uint32_t lr_value, uint32_t fault_id)
{
	u32 ret = TRUE;
	/* Hardfault Log is disabled in OTP */
	if (SYSCFG_OTP_FaultLogDis() == TRUE) {
		RTK_LOGI(TAG, "Hard Fault happens \r\n");
		RTK_LOGI(TAG, "Do not dump log dure to security \r\n");
		while (1);
	}

	/* 1.User-defined patch 1 */
	if (PrePatchHandler) {
		ret = PrePatchHandler(mstack, lr_value, fault_id);
	}

	/* If the previous patch returns normally, do not perform crashdump*/
	if (ret) {
		crash_Main(mstack, lr_value, fault_id);			// Print detailed error messages and exception stack content
	}

	/* User-defined patch 2 */
	if (PostPatchHandler) {
		PostPatchHandler(mstack, lr_value, fault_id);
	}

	for (;;);
}
//Exception Number = 2

void INT_NMI(void)
{
	//u32 CtrlReg;
	RTK_LOGE(TAG, "INT_NMI:NMI Error!!!!\n");
}


void FaultHandler(void)
{
	__ASM volatile(
		"MRS	R0, MSP					\n\t"
		"MOV	R1, LR					\n\t"/* R2 has stored the exception number. */
		"PUSH	{R4-R11}				\n\t"
		"B		INT_HardFault_C			\n\t"
	);
}

//Exception Number = 3

void INT_HardFault(void)
{
	__ASM volatile(
		"MOV R2, #0						\n\t"
		"B	FaultHandler				\n\t"
	);
}

//Exception Number = 4

void INT_MemManage(void)
{
	__ASM volatile(
		"MOV R2, #3						\n\t"
		"B	FaultHandler				\n\t"
	);
}

//Exception Number = 5

void INT_BusFault(void)
{
	__ASM volatile(
		"MOV R2, #2						\n\t"
		"B	FaultHandler				\n\t"
	);
}

//Exception Number = 6

void INT_UsageFault(void)
{
	__ASM volatile(
		"MOV R2, #1						\n\t"
		"B	FaultHandler				\n\t"
	);
}

//Exception Number = 7

void INT_SecureFault(void)
{
	__ASM volatile(
		"MOV R2, #4						\n\t"
		"B	FaultHandler				\n\t"
	);
}

//Exception Number = 11

void INT_SVCall(void)
{
	if (INT_SVCall_patch) {
		INT_SVCall_patch();
	} else {
		RTK_LOGE(TAG, "SVCall Fun Not Assign!!!!!\n");
	}
}

//Exception Number = 14

void INT_PendSV(void)
{
	if (INT_PendSV_patch) {
		INT_PendSV_patch();
	} else {
		RTK_LOGE(TAG, "PendSV Fun Not Assign!!!!!\n");
	}
}

//Exception Number = 15

void INT_SysTick(void)
{
	if (INT_SysTick_patch) {
		INT_SysTick_patch();
	} else {
		RTK_LOGE(TAG, "SysTick Fun Not Assign!!!!!\n");
	}
}

//following is External Interrupts & Exception Number >= 16


static void INT_Entry(u32 IntIndex)
{
	//Clear ARM Cortex-M3 IRQ
	NVIC_ClearPendingIRQ(IntIndex);

	if (UserIrqFunTable[IntIndex] != NULL) {
		UserIrqFunTable[IntIndex]((void *)(UserIrqDataTable[IntIndex]));
	} else {
		RTK_LOGW(TAG, "INT_Entry Irq %d Fun Not Assign!!!!!\n", IntIndex);
		DelayMs(1000);
	}
}


_LONG_CALL_  static void INT_USER(void)
{
	// get irq number
	int irqn = __get_IPSR() - 16;
	INT_Entry(irqn);
}

HAL_VECTOR_FUN VectorTable[] ALIGNMTO(512) = {
	(HAL_VECTOR_FUN) MSP_RAM_NP,  // 0
	(HAL_VECTOR_FUN) NULL,
	(HAL_VECTOR_FUN) INT_NMI,
	(HAL_VECTOR_FUN) INT_HardFault,
	(HAL_VECTOR_FUN) INT_MemManage,
	(HAL_VECTOR_FUN) INT_BusFault,
	(HAL_VECTOR_FUN) INT_UsageFault,
	(HAL_VECTOR_FUN) INT_SecureFault,
	(HAL_VECTOR_FUN) NULL,
	(HAL_VECTOR_FUN) NULL,
	(HAL_VECTOR_FUN) NULL,  //10
	(HAL_VECTOR_FUN) INT_SVCall,
	(HAL_VECTOR_FUN) NULL,
	(HAL_VECTOR_FUN) NULL,
	(HAL_VECTOR_FUN) INT_PendSV,
	(HAL_VECTOR_FUN) INT_SysTick,

	/* General IRQ functions for USER IRQ 1~80 */
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
	(HAL_VECTOR_FUN) INT_USER,	// general user defined irq handler
};

void irq_enable(IRQn_Type irqn)
{
	__NVIC_EnableIRQ(irqn);
}

void irq_disable(IRQn_Type irqn)
{
	__NVIC_DisableIRQ(irqn);
}

void irq_set_priority(IRQn_Type irqn, uint32_t priority)
{
	if (priority > MAX_IRQ_PRIORITY_VALUE) {
		RTK_LOGW(TAG, "irq[%d] priority %d shall <= %d\n", irqn, priority, MAX_IRQ_PRIORITY_VALUE);
		while (1);
	}
	__NVIC_SetPriority(irqn, priority);
}

uint32_t irq_get_priority(IRQn_Type irqn)
{
	return __NVIC_GetPriority(irqn);
}

void irq_set_pending(IRQn_Type irqn)
{
	__NVIC_SetPendingIRQ(irqn);
}

uint32_t irq_get_pending(IRQn_Type irqn)
{
	return __NVIC_GetPendingIRQ(irqn);
}

void irq_clear_pending(IRQn_Type irqn)
{
	__NVIC_ClearPendingIRQ(irqn);
}

bool irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data,  u32 Priority)
{
	UserIrqFunTable[IrqNum] = (IRQ_FUN)((u32)IrqFun | 0x1);
	UserIrqDataTable[IrqNum] = (u32)(Data);

	irq_set_priority(IrqNum, Priority);

	return TRUE;
}

/**
  * @brief  Unregister IRQ Handler.
  * @param  IrqNum: a value of @ref IRQn_enum.
  */

bool irq_unregister(IRQn_Type   IrqNum)
{
	UserIrqFunTable[IrqNum] = (IRQ_FUN)NULL;
	UserIrqDataTable[IrqNum] = (u32)NULL;

	return TRUE;
}

bool PreFaultPatch_register(Fault_Patch PatchFun)
{
	PrePatchHandler = PatchFun;

	return TRUE;
}

bool PostFaultPatch_register(Fault_Patch PatchFun)
{
	PostPatchHandler = PatchFun;

	return TRUE;
}

void SVCall_irqfunc_set(void (*pfunc)(void))
{
	INT_SVCall_patch = pfunc;
}

void *SVCall_irqfunc_get(void)
{
	return INT_SVCall_patch;
}

void PendSV_irqfunc_set(void (*pfunc)(void))
{
	INT_PendSV_patch = pfunc;
}

void *PendSV_irqfunc_get(void)
{
	return INT_PendSV_patch;
}

void Systick_irqfunc_set(void (*pfunc)(void))
{
	INT_SysTick_patch = pfunc;
}

void *Systick_irqfunc_get(void)
{
	return INT_SysTick_patch;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
