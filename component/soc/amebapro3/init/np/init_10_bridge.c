#include "ameba_soc.h"

#if defined(CONFIG_ARM_CORE_CM4_NP) && defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
#include "bridge_dispatch.h"

__attribute__((constructor))
void app_bridge_init(void)
{
	bridge_dispatch_init();
}
#endif
