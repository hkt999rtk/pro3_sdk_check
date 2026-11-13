#include "ameba_soc.h"
#if defined(CONFIG_VMODULE_ISP) && (CONFIG_VMODULE_ISP == 1)
#include "hal_isp.h"

static const char *TAG = "VP_INIT";
#endif

__attribute__((constructor))
void video_init(void)
{
#if defined(CONFIG_VMODULE_ISP) && (CONFIG_VMODULE_ISP == 1)
#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1)
	/* Constructors run before main(), so this precedes ISP_Init(), which locks the ISP mode. */
	if (ISP_Set4KMode(1) != 0) {
		RTK_LOGE(TAG, "ISP 4K mode rejected, ISP already initialized\r\n");
		return;
	}
	if (ISP_SetSensorIQ(SENSOR_GC8613, 0) != 0) {
		RTK_LOGE(TAG, "ISP 4K sensor GC8613 select failed\r\n");
		return;
	}
	RTK_LOGI(TAG, "select 4K mode and 4K sensor\r\n");
#else
	if (ISP_SetSensorIQ(SENSOR_OS04A10, 0) != 0) {
		RTK_LOGE(TAG, "ISP non-4K sensor OS04A10 select failed\r\n");
		return;
	}
	RTK_LOGI(TAG, "select non-4K sensor\r\n");
#endif
#endif
}
