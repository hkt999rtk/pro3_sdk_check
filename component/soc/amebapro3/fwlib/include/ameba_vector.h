/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_VECTOR_H_
#define _AMEBA_VECTOR_H_

#if defined (CONFIG_ARM_CORE_CM4)
#define MAX_PERIPHERAL_IRQ_NUM			80
#define MAX_VECTOR_TABLE_NUM			(MAX_PERIPHERAL_IRQ_NUM + 16)
#define MAX_IRQ_PRIORITY_VALUE			7
#else
#define MAX_VECTOR_TABLE_NUM			(16+44)
#define MAX_PERIPHERAL_IRQ_NUM			44
#define MAX_NVIC_IPR_NUM				((MAX_PERIPHERAL_IRQ_NUM + 3) >> 2)
#define MAX_IRQ_PRIORITY_VALUE			3
#endif

/** @addtogroup AmebaPro3_Platform
  * @{
  */

/** @defgroup IRQ
  * @brief IRQ modules
  * @{
  */

/** @addtogroup IRQ
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * IRQ table, please refer to IRQ Exported Constants->IRQn_enum->IRQn
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		1. register/unregister IRQ use: InterruptRegister/InterruptUnRegister
  *		2. enable/disable IRQ use: InterruptEn/InterruptDis
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup IRQ_Exported_Types IRQ Exported Types
  * @{
  */
typedef u32 IRQn_Type;
typedef void (*HAL_VECTOR_FUN)(void);
typedef u32(*IRQ_FUN)(void *Data);
typedef u32(*Fault_Patch)(uint32_t *MSP, uint32_t lr, uint32_t fault_id);
/**
  * @}
  */

#include "ameba_irqn.h"

// 0613 IRQN define wrapper
//#define WL_DMA_IRQ			WIFI0_IRQ
//#define WL_PROTOCOL_IRQ		WIFI1_IRQ
//#define WIFI_FISR_IRQ			WIFI2_IRQ
//#define WIFI_FTSR_IRQ			WIFI3_IRQ
//#define BT_SCB_IRQ			WIFI4_IRQ

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/

/** @defgroup IRQn_Priority_def
  * @{
  */
#if defined (CONFIG_ARM_CORE_CM4)
enum IRQnPriority {
	INT_PRI0 = 0,	//Priority value 0 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
	INT_PRI1,		//lower numbers have higher priority in AP, But higher numbers have higher priority in NP.
	INT_PRI2,
	INT_PRI_HIGH 	= INT_PRI2,
	INT_PRI3,
	INT_PRI4,
	INT_PRI_MIDDLE	= INT_PRI4,
	INT_PRI5,
	INT_PRI6,
	INT_PRI7,
	INT_PRI_LOWEST	= INT_PRI7,
};
#elif defined (CONFIG_ARM_CORE_CM0)
enum IRQnPriority {
	INT_PRI0 = 0,	//Priority value 0 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
	INT_PRI1,		//lower numbers have higher priority in AP, But higher numbers have higher priority in NP.
	INT_PRI_HIGH 	= INT_PRI1,
	INT_PRI2,
	INT_PRI_MIDDLE	= INT_PRI2,
	INT_PRI3,
	INT_PRI_LOWEST	= INT_PRI3,
};
#elif defined (CONFIG_ARM_CORE_CA32)
enum IRQnPriority {
	INT_PRI0 = 0x00,	//Priority value 0 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
	INT_PRI1 = 0x10,
	INT_PRI2 = 0x20,
	INT_PRI3 = 0x30,
	INT_PRI4 = 0x40,
	INT_PRI_HIGH	= INT_PRI4,
	INT_PRI5 = 0x50,
	INT_PRI6 = 0x60,
	INT_PRI7 = 0x70,
	INT_PRI8 = 0x80,
	INT_PRI9 = 0x90,
	INT_PRI_MIDDLE	= INT_PRI9,
	INT_PRI10 = 0xa0,
	INT_PRI11 = 0xb0,
	INT_PRI12 = 0xc0,
	INT_PRI13 = 0xd0,
	INT_PRI14 = 0xe0,
	INT_PRI_LOWEST	= INT_PRI14,
	/* IRQ masked if priority >= PMR */
	INT_PRI15 = 0xf0,
};
#endif
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IRQ_Exported_Functions IRQ Exported Functions
  * @{
  */
extern _LONG_CALL_ void irq_enable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_disable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_set_priority(IRQn_Type irqn, uint32_t priority);
extern _LONG_CALL_ uint32_t irq_get_priority(IRQn_Type irqn);
extern _LONG_CALL_ void irq_set_pending(IRQn_Type irqn);
extern _LONG_CALL_ uint32_t irq_get_pending(IRQn_Type irqn);
extern _LONG_CALL_ void irq_clear_pending(IRQn_Type irqn);
extern _LONG_CALL_ bool irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data, u32 Priority);
extern _LONG_CALL_ bool irq_unregister(IRQn_Type IrqNum);
extern _LONG_CALL_ bool PreFaultPatch_register(Fault_Patch PatchFun);
extern _LONG_CALL_ bool PostFaultPatch_register(Fault_Patch PatchFun);

extern _LONG_CALL_ void irq_errata_clear_pending_core(void);
extern _LONG_CALL_ void irq_errata_disable_core(void);

#define InterruptRegister			irq_register
#define InterruptUnRegister			irq_unregister

#define InterruptEn(a,b)			irq_enable(a)
#define InterruptDis(a)				irq_disable(a)
/**
  * @}
  */

/**
  * @}
  */

extern IRQ_FUN UserIrqFunTable[];
extern u32 UserIrqDataTable[];
extern HAL_VECTOR_FUN  RomVectorTable[];

#endif //_AMEBA_VECTOR_H_
