#include "ameba_soc.h"

#ifdef CONFIG_SHELL
__attribute__((constructor))
void loguart_init(void)
{
	extern void LOGUART_Init(LOGUART_InitTypeDef * LOGUART_InitStruct);
	extern void LOGUART_StructInit(LOGUART_InitTypeDef * LOGUART_InitStruct);
#if defined(CONFIG_LOGUART_SIMPLE) && (CONFIG_LOGUART_SIMPLE)
	LOGUART_Init(NULL);
#else
	LOGUART_InitTypeDef LOGUART_InitStruct;
	LOGUART_StructInit(&LOGUART_InitStruct);
	LOGUART_Init(&LOGUART_InitStruct);
#endif

	RTK_LOGI("VP_INIT", "LOGUART Init\n");
}
#endif
