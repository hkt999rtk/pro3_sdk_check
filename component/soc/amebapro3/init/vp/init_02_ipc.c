#include "ameba_soc.h"

__attribute__((constructor))
void ipc_init(void)
{
	InterruptRegister(IPC_INTHandler, IPC_MP_IRQ, (u32)IPCVP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_MP_IRQ, INT_PRI_MIDDLE);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);
	ipc_table_init(IPCVP_DEV);

	RTK_LOGI("VP_INIT", "IPC Init\n");
}
