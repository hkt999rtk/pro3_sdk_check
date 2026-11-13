#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

int init_heap(void);
void isp_mcu_apps_start(void);
#ifdef ISP_CPU_MODE
void cpu_connector_start(void);
#endif

int main(void)
{
	int ret;

	ret = init_heap();
	if (ret) {
		return ret;
	}

	isp_mcu_apps_start();
#ifdef ISP_CPU_MODE
	cpu_connector_start();
#endif
	vTaskStartScheduler();

	return 0;
}
