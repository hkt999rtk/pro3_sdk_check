#include <stdio.h>
#include <string.h>
#include "stdint.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "audio_api.h"

extern u8 __media_app_table_start__[];
extern u8 __media_app_table_end__[];
//------------------------------------------------------------------------------
// audio examples
//------------------------------------------------------------------------------
static void audio_example_main(void *param)
{
	int idx = (int)param;
	PAUDIO_APP_TABLE a_app_table = (PAUDIO_APP_TABLE)__media_app_table_start__;
	printf("run %s\r\n", a_app_table[idx].a_func_name);

	a_app_table[idx].a_func();

	vTaskDelete(NULL);
}

void audio_example_media_framework(void *param)
{
	/*user can start their own task here*/
	printf("audio_example_media_framework...\r\n");
	if (xTaskCreate(audio_example_main, ((const char *)"mmf2_audio"), 4096, param, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\r\n audio_example_main: Create Task Error\n");
	}
}

u32 CmdAudioAppTest(IN u16 argc, IN u8 *argv[])
{
	if (_strcmp((const char *)argv[0], "list") == 0) {

		int a_app_num = ((__media_app_table_end__ - __media_app_table_start__) / sizeof(AUDIO_APP_TABLE));
		PAUDIO_APP_TABLE a_app_table = (PAUDIO_APP_TABLE)__media_app_table_start__;

		printf("=============================================\r\n");
		for (int i = 0; i < a_app_num; i++) {
			if (strncmp(a_app_table[i].a_func_name, "mmf2_example_", strlen("mmf2_example_")) != 0 &&
				strncmp(a_app_table[i].a_func_name, "audio_", strlen("audio_")) != 0) {
				continue;
			}
			printf("audio example[%d]: %s\r\n", i, a_app_table[i].a_func_name);
		}
		printf("=============================================\r\n");
	} else if (_strcmp((const char *)argv[0], "run") == 0) {
		int aapp_exe_idx = 0;
		if (argc > 1) {
			aapp_exe_idx = atoi((const char *)argv[1]);
		}
		printf("audio run idx:%d\r\n", aapp_exe_idx);
		audio_example_media_framework((void *)aapp_exe_idx);
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE audio_app_test_cmd[] = {
	{
		"AUDIO", CmdAudioAppTest
	},
};
