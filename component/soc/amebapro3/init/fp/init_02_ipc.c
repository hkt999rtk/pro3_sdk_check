#include "ameba_soc.h"

__attribute__((constructor))
void ipc_init(void)
{
#if defined(CONFIG_SHELL_IPC)
	irq_register(IPC_INTHandler, IPC_FW_IRQ, (u32)IPCFP_DEV, INT_PRI_MIDDLE);
	irq_enable(IPC_FW_IRQ);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);
	ipc_table_init(IPCFP_DEV);

	RTK_LOGI("FP_INIT", "IPC Init\n");
#endif
}
