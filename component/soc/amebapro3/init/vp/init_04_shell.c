#include "ameba_soc.h"

#ifdef CONFIG_SHELL
__attribute__((constructor))
void shell_init(void)
{
	/* init console */
	shell_init_rom(0, NULL);
	shell_init_ram();

	RTK_LOGI("VP_INIT", "SHELL Init\n");
}
#endif
