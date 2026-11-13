/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "sheipa.h"
#include "cpu.h"
#include "ameba.h"

extern uint32_t ulPortInterruptNesting[ configNUMBER_OF_CORES ];

static volatile InterruptTable_t xInterruptTable[ GIC_MAX_NUM_INTR  ];

void vRegisterIRQHandler(uint32_t ulID, ISRCallback_t pxHandler, void *pvContext);
void vConfigureIPIInterrupt(void);

/*-----------------------------------------------------------*/

void prvSetupHardware(void)
{
	/* Ensure no interrupts execute while the scheduler is in an inconsistent
	state.  Interrupts are automatically enabled when the scheduler is
	started. */
	portDISABLE_INTERRUPTS();

	arm_gic_init();

	/* Booting other cores */
#if ( configNUMBER_OF_CORES > 1 )
	{
		psci_init();
		smp_init();
	}
#endif
}

#if ( configNUMBER_OF_CORES > 1 )
void prvSetupHardwareSecondary(void)
{
	portDISABLE_INTERRUPTS();

	arm_gic_init_secondary();
}
#endif
/*-----------------------------------------------------------*/

#if ( configNUMBER_OF_CORES > 1 )
void vConfigureIPIInterrupt(void)
{
	vRegisterIRQHandler(IPI_IRQ, (ISRCallback_t)FreeRTOS_IPI_Handler, 0);

	arm_gic_irq_set_priority(IPI_IRQ, IRQ_PRIO_DEFAULT, IRQ_TYPE_LEVEL);
	arm_gic_irq_enable(IPI_IRQ);
}

void vClearIPIInterrupt(void)
{
	return;
}

void vConfigureSMPSendIPI(uint32_t ulCoreID)
{
	arm_gic_raise_softirq(ulCoreID, IPI_IRQ);
}
#endif
/*-----------------------------------------------------------*/

#define CYCLES_PER_TICK		(configGT_CLOOCK_HZ / configTICK_RATE_HZ)

#if( configUSE_TICKLESS_IDLE == 1 )
static uint64_t xMaximumPossibleSuppressedTicks = 0;
#endif

void vConfigureTickInterrupt(void)
{
#if( configUSE_TICKLESS_IDLE == 1 )
	xMaximumPossibleSuppressedTicks = -1ULL / CYCLES_PER_TICK;
#endif

	vRegisterIRQHandler(ARM_ARCH_TIMER_IRQ, (ISRCallback_t)FreeRTOS_Tick_Handler, 0);

	arm_arch_timer_set_compare(arm_arch_timer_count() + CYCLES_PER_TICK);
	arm_arch_timer_enable(pdTRUE);

	arm_gic_irq_set_priority(ARM_ARCH_TIMER_IRQ, IRQ_PRIO_DEFAULT, IRQ_TYPE_LEVEL);
	arm_gic_irq_enable(ARM_ARCH_TIMER_IRQ);

#define CNT_CONTROL_BASE 	0x60460000
#define CNT_READ_BASE 		0x60410000

#define SYSTIM_CNTCR	0
	/*
		__set_CNTFRQ(12500000);
		__set_CNTP_CTL(0);
		__set_CNTP_TVAL(12500 - 1);
		//__set_CNTP_CVAL(20 * 1000 - 1);
		__set_CNTP_CTL(1);

		HAL_WRITE32(CNT_CONTROL_BASE, SYSTIM_CNTCR, 1);
	*/
}

void vClearTickInterrupt(void)
{
	uint32_t delta_ticks;
	uint64_t last_cycle;

	last_cycle = arm_arch_timer_compare();

#if( configUSE_TICKLESS_IDLE == 1 )
	delta_ticks = (uint32_t)((arm_arch_timer_count() - last_cycle) / CYCLES_PER_TICK) + 1;
#else
	delta_ticks = 1;
#endif

	arm_arch_timer_set_compare(last_cycle + delta_ticks * CYCLES_PER_TICK);
}

#if( configUSE_TICKLESS_IDLE == 1 )
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
	uint64_t ulCompleteTickPeriods, ulLeftTickPeriods;
	BaseType_t xCoreID = portGET_CORE_ID();

	/* Make sure the Generic Timer reload value does not overflow the counter. */
	if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
		xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
	}

	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
	portDISABLE_INTERRUPTS();
	portGET_TASK_LOCK();
	portGET_ISR_LOCK();

	/* If a context switch is pending or a task is waiting for the scheduler
	to be unsuspended then abandon the low power entry. */
	if ((xCoreID == 0) && (eTaskConfirmSleepModeStatus() == eAbortSleep)) {
		portRELEASE_TASK_LOCK();
		portENABLE_INTERRUPTS();

		return;
	}

	portRELEASE_ISR_LOCK();
	portRELEASE_TASK_LOCK();

	/* Calculate the reload value required to wait xExpectedIdleTime
	tick periods.  -1 is used because this code will execute part way
	through one of the tick periods. */
	arm_arch_timer_set_compare(arm_arch_timer_compare() + (xExpectedIdleTime - 1) * CYCLES_PER_TICK);

	__asm("	DSB");
	__asm("	WFI");
	__asm("	ISB");

	if (arm_arch_timer_condition_met()) {
		/* As the pending tick will be processed as soon as this
		function exits, the tick value maintained by the tick is stepped
		forward by one less than the time spent waiting. */
		ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
	} else {
		/* Something other than the tick interrupt ended the sleep.
		Work out how long the sleep lasted rounded to complete tick
		periods. */
		ulLeftTickPeriods = (arm_arch_timer_compare() - arm_arch_timer_count()) / CYCLES_PER_TICK;
		arm_arch_timer_set_compare(arm_arch_timer_compare() - ulLeftTickPeriods * CYCLES_PER_TICK);
		ulCompleteTickPeriods = xExpectedIdleTime - ulLeftTickPeriods - 1;
	}

	if (xCoreID == 0) {
		vTaskStepTick(ulCompleteTickPeriods);
	}

	/* Re-enable interrupts to allow the interrupt that brought the MCU
	out of sleep mode to execute immediately. */
	portENABLE_INTERRUPTS();
}

#endif /* configUSE_TICKLESS_IDLE */

/*-----------------------------------------------------------*/

void vRegisterIRQHandler(uint32_t ulID, ISRCallback_t pxHandler, void *pvContext)
{
	if (ulID < GIC_MAX_NUM_INTR) {
		xInterruptTable[ ulID ].pxISR = pxHandler;
		xInterruptTable[ ulID ].pvContext = pvContext;
	}
}
/*-----------------------------------------------------------*/

void vApplicationIRQHandlerCA9(uint32_t ulICCIAR)
{
	ISRCallback_t pxISR;
	uint32_t ulInterruptID = ulICCIAR & 0x3FF;

	if (ulInterruptID == 0x3FF) {
		return;
	}

	if (ulInterruptID < GIC_MAX_NUM_INTR) {
		/* Call the function installed in the array of installed handler
		functions. */
		pxISR = xInterruptTable[ ulInterruptID ].pxISR;
		if (pxISR) {
			pxISR(xInterruptTable[ ulInterruptID ].pvContext);
			//printf("ISR id(%lu) \n ", (unsigned long)ulInterruptID);
		} else {
			printf("ISR for interrupt id(%lu) is not registered!\n ", (unsigned long)ulInterruptID);
			//for (;;);
		}
	}
}


void __vApplicationIRQHandler(void)
{
	uint32_t ulInterruptID;
	ISRCallback_t pxISR;
	UBaseType_t ulPrioMask;

	do {
		ulInterruptID = arm_gic_get_active();

		if (ulInterruptID == 0x3FF) {
			break;
		} else if (ulInterruptID < GIC_MAX_NUM_INTR) {
			/* Store Current ICC_PMR*/
			ulPrioMask = portICCPMR_PRIORITY_MASK_REGISTER;

			/* Update ICC_PMR, For Nesting interrupt,
			 irq priority lower than current irq will be masked */
			portICCPMR_PRIORITY_MASK_REGISTER = portICCRPR_RUNNING_PRIORITY_REGISTER;
			asm volatile("DSB \n");
			asm volatile("ISB \n");

			if (ulPortInterruptNesting[GET_CPUID] < MAX_IRQ_NESTING_DEPTH) {
				portCPU_IRQ_ENABLE();
			}

			/* Call the function installed in the array of installed handler
			functions. */
			pxISR = xInterruptTable[ ulInterruptID ].pxISR;
			if (pxISR) {
				pxISR(xInterruptTable[ ulInterruptID ].pvContext);
			} else {
				printf("ISR for interrupt id(%ld) is not registered!\n ", ulInterruptID);
				for (;;);
			}

			/* Disable interrupt on exception return,
			 *Interrupt will be enabled after restore context
			 */
			portCPU_IRQ_DISABLE();
			arm_gic_eoi(ulInterruptID);

			/* Restore ICC_PMR */
			portICCPMR_PRIORITY_MASK_REGISTER = ulPrioMask;

		}
	} while (1);
}

void vApplicationIRQHandler(void)
{
	/* save NEON context */
	uint64_t neon_ctx[32];  // d0-d31
	uint32_t fpscr;

	__asm__ volatile(
		"vstm   %1, {d0-d15}    \n"
		"vstm   %2, {d16-d31}   \n"
		"vmrs   %0, fpscr       \n"
		: "=r"(fpscr)
		: "r"(neon_ctx), "r"(neon_ctx + 16)
		: "memory"
	);

	__vApplicationIRQHandler();

	/* restore NEON context */
	__asm__ volatile(
		"vmsr   fpscr, %0       \n"
		"vldm   %1, {d0-d15}    \n"
		"vldm   %2, {d16-d31}   \n"
		:
		: "r"(fpscr), "r"(neon_ctx), "r"(neon_ctx + 16)
		: "memory"
	);
}

