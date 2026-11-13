#include "ameba_soc.h"

__attribute__((constructor))
void ipc_init(void)
{
	InterruptRegister(IPC_INTHandler, IPC_AP_IRQ, (u32)IPCAP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_AP_IRQ, INT_PRI_MIDDLE);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);
	ipc_table_init(IPCAP_DEV);

	RTK_LOGI("AP_INIT", "IPC Init\n");
}
