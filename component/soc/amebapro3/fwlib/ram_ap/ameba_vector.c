/**
  ******************************************************************************
  * @file    amebapro3_vector.c
  * @author
  * @version V1.0.0
  * @date    2024-07-01
  * @brief   This file contains all the functions prototypes for the IRQ firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"
#include "sheipa.h"
//static const char *TAG = "VECTOR";

#if defined(CONFIG_ARM_CORE_CA32)

extern void vUnRegisterIRQHandler(u32);
//extern void vRegisterIRQHandler(u32);
extern void vRegisterIRQHandler(uint32_t ulID, ISRCallback_t pxHandler, void *pvContext);

/**
  * @brief  Init vector table.
  * @param  StackP: MSP TOP.
  */

void irq_table_init(uint32_t StackP)
{
	(void)StackP;
}

void irq_enable(IRQn_Type irqn)
{
	GIC_EnableIRQ(irqn);
}

void irq_disable(IRQn_Type irqn)
{
	GIC_DisableIRQ(irqn);
}

void irq_set_priority(IRQn_Type irqn, uint32_t priority)
{
	GIC_SetPriority(irqn, priority);
}

uint32_t irq_get_priority(IRQn_Type irqn)
{
	return GIC_GetPriority(irqn);
}

void irq_set_pending(IRQn_Type irqn)
{
	GIC_SetPendingIRQ(irqn);
}

uint32_t irq_get_pending(IRQn_Type irqn)
{
	return GIC_GetPendingIRQ(irqn);
}

void irq_clear_pending(IRQn_Type irqn)
{
	GIC_ClearPendingIRQ(irqn);
}

BOOL irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, uint32_t Data,  uint32_t Priority)
{
	void *IrqFun_temp = (void *)IrqFun;
	vRegisterIRQHandler(IrqNum, (ISRCallback_t)IrqFun_temp, (void *)Data);

	GIC_SetPriority(IrqNum, Priority);


	return _TRUE;
}


/**
  * @brief  Unregister IRQ Handler.
  * @param  IrqNum: a value of @ref IRQn_enum.
  */

BOOL irq_unregister(IRQn_Type   IrqNum)
{
	vRegisterIRQHandler(IrqNum, NULL, NULL);

	return _TRUE;
}
#endif

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
