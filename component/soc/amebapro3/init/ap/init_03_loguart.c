#include "ameba_soc.h"

#ifdef CONFIG_SHELL
__attribute__((constructor))
void loguart_init(void)
{
	extern void LOGUART_Init(LOGUART_InitTypeDef * LOGUART_InitStruct);
	extern void LOGUART_StructInit(LOGUART_InitTypeDef * LOGUART_InitStruct);

	LOGUART_InitTypeDef LOGUART_InitStruct;
	LOGUART_StructInit(&LOGUART_InitStruct);
	LOGUART_Init(&LOGUART_InitStruct);

	RTK_LOGI("AP_INIT", "LOGUART Init\n");
}
#endif
