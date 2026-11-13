/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "basic_types.h"
#include "ameba_soc.h"

// not good if include RTOS header in low level code
#ifndef RTOS_TICK_RATE_HZ
#define RTOS_TICK_RATE_HZ 1000
#endif
/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
__weak u32 SystemCoreClock = 300 * 1000 * 1000; /*!< System Clock Frequency (Core Clock)*/

/**
  * Update SystemCoreClock variable
  *
  * @param  none
  * @return none
  *
  * @brief  Updates the SystemCoreClock with current core Clock
  *         retrieved from cpu registers.
  */
void SystemCoreClockUpdate(void)             /* Get Core Clock Frequency      */
{
	SystemCoreClock = CPU_ClkGet();
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit(void)
{
	// TODO: Hardware initial
#ifdef UNALIGNED_SUPPORT_DISABLE
	SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
}

void Systick_Cmd(u32 enable)
{
#if !defined(CONFIG_ARM_CORE_CA32)
	if (enable) {
		/* Enable */
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	} else {
		/* Disable */
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	}

	__DSB();
#else
	(void)enable;
#endif
}

u32 Systick_State(void)
{
#if !defined(CONFIG_ARM_CORE_CA32)
	if (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) {
		return ENABLE;
	} else {
		return DISABLE;
	}
#else
	return ENABLE;
#endif
}

/* Notice: Maximum miss 1 tick because of cpu clock changed */
void Systick_update(void)
{
#if !defined(CONFIG_ARM_CORE_CA32)
	/* write any value will reset current register */
	SysTick->VAL = 0UL;

	/* Configure SysTick to interrupt at the requested rate. */
	SysTick->LOAD = (SystemCoreClock / RTOS_TICK_RATE_HZ) - 1UL;
#endif
}

/**
  * @brief  Set CPU clock Source.
  * @param  Source: This parameter can be one of the following values:
  *		 @arg CLK_CPU_USB_PLL
  *		 @arg CLK_CPU_SYS_PLL
  *		 @arg CLK_CPU_XTAL
  */
void CPU_ClkSrcSet(u32 Source)
{
	(void)Source;
#if 0
	u32 PreState;

	/* 1. update for NonOS */
	switch (Source) {
	case CLK_CPU_USB_PLL:
		//RCC_PeriphClockSourceSet(HP, USB_PLL);
		break;
	case CLK_CPU_SYS1_PLL:
		//RCC_PeriphClockSourceSet(HP, SYS_PLL);
		break;
	case CLK_CPU_SYS2_PLL:
		//RCC_PeriphClockSourceSet(HP, SYS_PLL);
		break;
	case CLK_CPU_XTAL:
		//RCC_PeriphClockSourceSet(HP, XTAL);
		break;
	default:
		assert_param(0);
		break;
	}

	/* 2. update for OS */
	SystemCoreClock = CPU_ClkGet(); /* CPU Clock is changed*/

	/* 3. update systick */
	PreState = Systick_State();
	Systick_Cmd(DISABLE);
	Systick_update();
	Systick_Cmd(PreState);
#endif
}

/**
  * @brief  Get CPU interrupt status.
  * @return CPU interrupt status 0/1
  */
u32 CPU_InInterrupt(void)
{
#if !defined(CONFIG_ARM_CORE_CA32)
	return __get_IPSR() != 0;
#else
	/* No IPSR on CA32. Tasks run in SYS mode, exceptions run in
	 * FIQ/IRQ/SVC/ABT/UND, so any other mode means interrupt context. */
	u32 Mode = __get_mode();
	return (Mode != CPSR_M_USR && Mode != CPSR_M_SYS) ? 1 : 0;
#endif
}

/**
 * Disable IRQ Interrupts and Save Previous Status
 *
 * @param  none
 * @return the previous value of IQR status
 *
 * @brief  disables interrupts by setting the primask register,
 * and returns the previous value of that register.
 */
u32 irq_disable_save(void)
{
#if !defined(CONFIG_ARM_CORE_CA32)

	uint32_t PrevStatus = __get_PRIMASK();
	__disable_irq();
	return PrevStatus;
#else
	/* CA32 has no PRIMASK; the CPSR I bit does the same job. Return only that
	 * bit so irq_enable_restore() puts back exactly what this call changed. */
	uint32_t PrevStatus = __get_CPSR() & CPSR_I_Msk;
	__disable_irq();
	return PrevStatus;
#endif
}

/**
 * enable IRQ Interrupts according to Previous Status
 *
 * @param  the previous value of IQR status
 * @return none
 *
 * @brief  This function enables interrupt by restoring the saved interrupt status register.
 * irq_disable_save and irq_enable_restore normally appear in pairs
 */
void irq_enable_restore(u32 PrevStatus)
{
#if !defined(CONFIG_ARM_CORE_CA32)

	__set_PRIMASK(PrevStatus);
#else
	/* Only the I bit is ever saved by irq_disable_save(), and this may be called
	 * from an ISR, so toggle it with cpsie/cpsid. A plain CPSR write would also
	 * rewrite the mode bits and drop the core out of IRQ/SVC mode mid-exception. */
	if (PrevStatus & CPSR_I_Msk) {
		__disable_irq();
	} else {
		__enable_irq();
	}
#endif
}

