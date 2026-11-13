#include "ameba_soc.h"

__attribute__((constructor))
void ipc_init(void)
{
#if defined(CONFIG_SHELL_IPC)
	InterruptRegister(IPC_INTHandler, IPC_NP_IRQ, (u32)IPCNP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_NP_IRQ, INT_PRI_MIDDLE);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);

	ipc_table_init(IPCNP_DEV);
	RTK_LOGI("INIT", "IPC Init\n");
#endif
}
