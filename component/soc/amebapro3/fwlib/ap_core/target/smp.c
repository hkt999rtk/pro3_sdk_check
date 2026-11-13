/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include "sheipa.h"
#include "cpu.h"

#include "task.h"

extern void _boot(void);
extern void vPortRestoreTaskContext(void);

/*-----------------------------------------------------------*/

void vPortSecondaryStart(void)
{
	debug_printf("CPU%d: on\n", (int)portGET_CORE_ID());

	/* Wait until scheduler starts */
	while (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED);

	/* Configure the hardware ready to run the demo. */
	prvSetupHardwareSecondary();

	/* Enable ipi for yield core */
	configSETUP_IPI_INTERRUPT();

#if (( !CONFIG_TEST_TYPE ) && ( CONFIG_SMP ))
	extern void software_irq_init();
	software_irq_init();
#endif

	/* Start the first task executing. */
	vPortRestoreTaskContext();
}
/*-----------------------------------------------------------*/

void smp_init(void)
{
	BaseType_t xCoreID;
	BaseType_t err;

	debug_printf("smp: Bringing up secondary CPUs ...\n");

	for (xCoreID = 0; xCoreID < configNUMBER_OF_CORES; xCoreID++) {
		if (xCoreID == portGET_CORE_ID()) {
			continue;
		}

		err = psci_cpu_on(xCoreID << MPIDR_AFF0_SHIFT, (unsigned long)_boot);
		if (err < 0) {
			debug_printf("CPU%d: failed to boot: %d\n", (int)xCoreID, (int)err);
		}
	}
}
