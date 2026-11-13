#include "ameba_soc.h"

#ifdef CONFIG_SHELL
__attribute__((constructor))
void loguart_init(void)
{
	extern void LOGUART_Init(LOGUART_InitTypeDef * LOGUART_InitStruct);
	extern void LOGUART_IRQInit(IRQ_FUN IrqFunc);
	LOGUART_InitTypeDef LOGUART_InitStruct;
	LOGUART_StructInit(&LOGUART_InitStruct);
	LOGUART_Init(&LOGUART_InitStruct);

#if !defined(CONFIG_LINUX_FW_EN) || (CONFIG_LINUX_FW_EN==0)
	/* Register Log Uart Callback function */
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_NP, ENABLE);
	LOGUART_IRQInit((IRQ_FUN) shell_uart_irq_rom);
#endif

	RTK_LOGI("INIT", "LOGUART Init\n");
}
#endif
