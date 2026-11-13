#ifndef EXAMPLE_WOWLAN_TICKLESS_H
#define EXAMPLE_WOWLAN_TICKLESS_H

#include "ameba_soc.h"

#if defined (CONFIG_ARM_CORE_CA32)
void example_wowlan_tickless(void);
#elif defined (CONFIG_ARM_CORE_CM4_NP)
void example_wowlan_ipc_process(void);
#endif

#endif /* EXAMPLE_WOWLAN_TICKLESS_H */
