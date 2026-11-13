#include "atcmd_service.h"

#include "atcmd_demo.h"
#include "atcmd_wifi.h"
#include "atcmd_network.h"

volatile log_init_t log_init_table[] = {
	at_demo_init,	// DEMO item
#if defined(RUNWLAN)
	at_wifi_init,
	at_network_init,
#endif
	NULL,			// terminate word, keep this
};

void atcmd_command_table_init(void)
{
	for (int i = 0; log_init_table[i] != NULL ; i++) {
		log_init_table[i]();
	}
}
