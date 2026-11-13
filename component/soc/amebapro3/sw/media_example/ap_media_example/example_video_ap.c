#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "video_api.h"
#include "ipc_video.h"

extern u8 __media_app_table_start__[];
extern u8 __media_app_table_end__[];
//------------------------------------------------------------------------------
// video examples
//------------------------------------------------------------------------------
static void video_example_main(void *param)
{
	int idx = (int)param;
	PVIDEO_APP_TABLE v_app_table = (PVIDEO_APP_TABLE)__media_app_table_start__;
	printf("run %s\r\n", v_app_table[idx].v_func_name);

	v_app_table[idx].v_func();

	vTaskDelete(NULL);
}

void video_example_media_framework(void *param)
{
	/*user can start their own task here*/
	printf("video_example_media_framework...\r\n");
	if (xTaskCreate(video_example_main, ((const char *)"mmf2_video"), 4096, param, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n video_example_main: Create Task Error\n");
	}
}

u32 CmdVideoAppTest(IN u16 argc, IN u8 *argv[])
{
	if (_strcmp((const char *)argv[0], "list") == 0) {

		int v_app_num = ((__media_app_table_end__ - __media_app_table_start__) / sizeof(VIDEO_APP_TABLE));
		PVIDEO_APP_TABLE v_app_table = (PVIDEO_APP_TABLE)__media_app_table_start__;

		printf("=============================================\r\n");
		for (int i = 0; i < v_app_num; i++) {
			if (strncmp(v_app_table[i].v_func_name, "mmf2_video_example_", strlen("mmf2_video_example_")) != 0) {
				continue;
			}
			printf("video example[%d]: %s\r\n", i, v_app_table[i].v_func_name);
		}
		printf("=============================================\r\n");
	} else if (_strcmp((const char *)argv[0], "run") == 0) {
		int vapp_exe_idx = 0;
		if (argc > 1) {
			vapp_exe_idx = atoi((const char *)argv[1]);
		}
		printf("video run idx:%d\r\n", vapp_exe_idx);
		video_example_media_framework((void *)vapp_exe_idx);
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   video_app_test_cmd[] = {
	{
		"VIDEO", CmdVideoAppTest
	},
};

//------------------------------------------------------------------------------
// FCS video examples
//------------------------------------------------------------------------------
#if defined(CONFIG_FAST_CAMERA_START) && (CONFIG_FAST_CAMERA_START != 0)
#include "mmf2_video_example_fast_camera_start_init.h"
#include "video_fcs_params.h"

static void video_fcs_example_task(void *param)
{
	int boot_state;

	(void)param;

	fcs_params_prepare();
	boot_state = video_fcs_get_boot_state();
	if (boot_state == VIDEO_FCS_BOOT_NORMAL) {
		RTK_LOGI("FCS", "[AP FCS] normal boot; regular CMD_VIDEO_APPLY is available\r\n");
		vTaskDelete(NULL);
		return;
	}
	if (boot_state != VIDEO_FCS_BOOT_ACTIVE) {
		RTK_LOGE("FCS", "[AP FCS] failed to query an active VP FCS boot\r\n");
		vTaskDelete(NULL);
		return;
	}

	mmf2_video_example_fast_camera_start_rtsp_init();

	vTaskDelete(NULL);
}

void app_example(void)
{
	if (xTaskCreate(video_fcs_example_task, ((const char *)"mmf2_video"), 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		RTK_LOGE("FCS", "[AP FCS] create task error\r\n");
	}
}
#else

static void video_normal_example_task(void *param)
{
	(void)param;

	//mmf2_video_example_v1_rtsp_init();
	//mmf2_video_example_nn_face_detection_rtsp_init();

	vTaskDelete(NULL);
}
void app_example(void)
{
	if (xTaskCreate(video_normal_example_task, ((const char *)"mmf2_video"), 4096, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		RTK_LOGE("VAPP", "AP video example, create task error\r\n");
	}
}
#endif
