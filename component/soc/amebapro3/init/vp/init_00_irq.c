#include <string.h>
#include <stdint.h>
#include <ameba.h>
#include <platform_autoconf.h>

/* default weak main and handlers */
__weak void SVC_Handler(void)
{
	return;
}

__weak void PendSV_Handler(void)
{
	return;
}

__weak void SysTick_Handler(void)
{
	return;
}

extern HAL_VECTOR_FUN VectorTable[];
static void irq_table_init(u32 StackP)
{
	VectorTable[0] = (HAL_VECTOR_FUN)StackP;

	SCB->VTOR = (uint32_t)VectorTable;
}

__attribute__((constructor))
void irq_init(void)
{
	u32 stacktop = HAL_READ32(SCB->VTOR, 0);
	irq_table_init(stacktop);

#if defined(CONFIG_PLATFORM_FREERTOS) && (CONFIG_PLATFORM_FREERTOS==1)
	__NVIC_SetVector(SVCall_IRQn, (u32)(void *)SVC_Handler);
	__NVIC_SetVector(PendSV_IRQn, (u32)(void *)PendSV_Handler);
	__NVIC_SetVector(SysTick_IRQn, (u32)(void *)SysTick_Handler);
#endif
	RTK_LOGI("VP_INIT", "IRQ Init\n");
}
