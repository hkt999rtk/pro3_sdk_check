/* --------------------------*/
/* ENC verification           */
/* --------------------------*/
#include "ameba_soc.h"
#include <diag.h>
#include <log.h>

#include "stdio.h"

#include <FreeRTOS.h>
#include <task.h>

/* ENC header */
#include "hevcencapi.h"
#include "ewl.h"

extern void dump_addr(u8 *buf, int size);

static int pass_cnt = 0, fail_cnt = 0;

#define TEST(condition, message, ...)       \
    do {                                    \
        if (!(condition)) {                 \
            DiagPrintf("[FAIL] ");          \
            fail_cnt++;                     \
        } else {                            \
            DiagPrintf("[PASS] ");          \
            pass_cnt++;                     \
        }                                   \
        DiagPrintf(message, ##__VA_ARGS__); \
        DiagPrintf("\r\n\r\n");             \
    } while (0)


#define TEST_BENCH  1

#if TEST_BENCH
int hevc_test_bench(const uint32_t CRC32ExpValue, char *arg)
{
	DiagPrintf("enc test func\n\r");
	extern int main_HEVC_H264_Test(const uint32_t CRC32ExpValue, char *arg) ;
	int ret = main_HEVC_H264_Test(CRC32ExpValue, arg);
	if (ret != 0) {
		return FAIL;
	}
	return SUCCESS;
}
int jpeg_test_bench(const uint32_t CRC32ExpValue, char *arg)
{
	DiagPrintf("jpeg test func\n\r");
	extern int main_JPEG_Test(const uint32_t CRC32ExpValue, char *arg);
	int ret = main_JPEG_Test(CRC32ExpValue, arg);
	if (ret != 0) {
		return FAIL;
	}
	return SUCCESS;
}
#endif

void enc_run_verification(void)
{
	DiagPrintf("ENC verification start...\n\r");

#if TEST_BENCH

	DiagPrintf("TEST BENCH start...\n\r");

#if CONFIG_BUILD_ENC_HEVC_TB
	//#[hevc case_id 100 cmodel cmds]:
	TEST(hevc_test_bench(0xa16cf41a,
						 "./hevc_testenc_vce -a0 -b2 --AXIAlignment=000000000 -i=D:\\FPGA_test_data\\foreman_176x144_3f.nv12 --inputFormat=1 --formatCustomizedType=-1 --gopSize=1 --compressor=3 --rdoLevel=1 --enableRdoQuant=0 --lumWidthSrc=176 --lumHeightSrc=144 --width=176 --height=144 --codecFormat=hevc --inputAlignmentExp=8 --refAlignmentExp=0 --refChromaAlignmentExp=0 --aqInfoAlignmentExp=6 --bitDepthLuma=8 --bitDepthChroma=8  -o=D:\\output_enc\\hevc_case_id_100_cmodel_cmds.hevc --useVcmd=1")
		 == SUCCESS, "ENC HEVC TEST");

	//#[h264 case_id 237 cmodel cmds]:
	TEST(hevc_test_bench(0xe85dc72d,
						 "./h264_testenc_vce -a0 -b2 --AXIAlignment=000000000 --refRingBufEnable=1 -i=D:\\FPGA_test_data\\foreman_176x144_3f.nv12 --inputFormat=1 --formatCustomizedType=-1 --gopSize=1 --bitDepthLuma=8 --bitDepthChroma=8 --profile=0 --compressor=3 --rdoLevel=1 --enableRdoQuant=0 --codedChromaIdc=0 --lumWidthSrc=176 --lumHeightSrc=144 --width=176 --height=144 --codecFormat=h264 --inputAlignmentExp=8 --refAlignmentExp=0 --refChromaAlignmentExp=0 --aqInfoAlignmentExp=6 --bitDepthLuma=8 --bitDepthChroma=8  -o=D:\\output_enc\\h264_case_id_237_cmodel_cmds.h264 --useVcmd=1")
		 == SUCCESS, "ENC H264 TEST");
#else
	//#[jpeg case_id 0 cmodel cmds]:
	TEST(jpeg_test_bench(0x52d8514f,
						 "./jpeg_testenc_vce -a0 -b0 -i=D:\\FPGA_test_data\\foreman_176x144_3f.nv12 --frameType=1 --XformCustomerPrivateFormat=-1 --lumWidthSrc=176 --lumHeightSrc=144 --width=176 --height=144 --codingMode=0 --inputAlignmentExp=0 --hashtype=1 -W=0 -o=D:\\FPGA_test_data\\jpeg_case_id_00_cmodel_cmds.jpeg --useVcmd=1")
		 == SUCCESS, "ENC JPEG TEST");

#endif
#else

	DiagPrintf("SIMPLE DEMO start...\n\r");

	/* init VCMD driver */
	extern int encoder_rtos_driver_init(void);
	encoder_rtos_driver_init();

	/* HEVC Demo */
	TEST(Demo_HEVC_Encoding() == OK, "Demo HEVC Encoding TEST");

	/* H264 Demo */
	TEST(Demo_H264_Encoding() == OK, "Demo H264 Encoding TEST");

	/* decompress Demo */
	TEST(Demo_DEC_Encoding() == OK, "Demo DEC Encoding TEST");

	/* OSD Demo */
	TEST(Demo_OSD_Encoding() == OK, "Demo OSD Encoding TEST");

	/* JPEG Demo */
	TEST(Demo_JPEG_Encoding() == OK, "Demo JPEG Encoding TEST");

	//extern int encoder_rtos_driver_deinit(void);
	encoder_rtos_driver_deinit();

#endif
}

#if 1
#include "user_freertos.h"
#include "io_tools.h"
#include "memalloc_freertos.h"
#include "vcx_driver_freertos.h"
#include "vcx_vcmd_priv_freertos.h"
extern rtos_vpu_dev g_vpu_dev[VPU_FD_NUM];

void enc_reg_dump(void)
{
	printf("\r\n\r\n");
	printf("==================================================\r\n");
	printf("VCMD / VCE Reg Dump:\r\n");

	uint32_t *vcmd_swreg_base = 0x60108000;
	uint32_t *vce_swreg_base = 0x60109000;

	int vcmd_swreg_num = 64;
	for (int i = 0; i < vcmd_swreg_num; i++) {
		printf("vcmd_swreg[%d]: 0x%x\r\n", i, vcmd_swreg_base[i]);
	}
	printf("\r\n\r\n");

	int vce_swreg_num = 500;
	for (int i = 0; i < vce_swreg_num; i++) {
		printf("vce_swreg[%d]: 0x%x\r\n", i, vce_swreg_base[i]);
	}
	printf("\r\n\r\n");
#if 0
	printf("==================================================\r\n");
	vcmd_mgr_t *vcmd_mgr = g_vpu_dev[ENC_FD].ctx;
	extern void dump_addr(u8 * buf, int size);
	printf("VCMD BUF\r\n");
	dump_addr(vcmd_mgr->mem_vcmd.va, vcmd_mgr->mem_vcmd.size);
	printf("\r\n");

	printf("STATUS BUF\r\n");
	dump_addr(vcmd_mgr->mem_status.va, vcmd_mgr->mem_status.size);
	printf("\r\n");

	printf("==================================================\r\n");
#endif

}
#endif

static int run_flag = 0;
void enc_verify_task(void *dummy)
{
	DiagPrintf("== enc_verify_task ==\n\r");

	enc_run_verification();

	DiagPrintf("enc_verify_task END\n\r");
	run_flag = 0;
	vTaskDelete(NULL);
}

#if TEST_BENCH
#define VERIFY_TASK_STACK_SIZE  (40 * 1024)
#else
#define VERIFY_TASK_STACK_SIZE  (6 * 1024)
#endif

u32 EncCmdTest(IN u16 argc, IN u8 *argv[])
{
	DiagPrintf("Encoder Cmd Test \n\r");

	/*
	 * Note:
	 * Encoder HW can only access "video sram" and "DDR" !
	 */

	/* security setting */
	*((uint32_t *)0x6010c540) = 0x1;
	*((uint32_t *)0x6010c500) = 0x6f000000;
	*((uint32_t *)0x6010c504) = 0x6f02ffff;

	uint32_t *vce_swreg_base = 0x60109000;

	DiagPrintf("argc %d \n\r", argc);
	for (int i = 0; i < argc; i++) {
		DiagPrintf("argv[%d] = %s\n\r", i, argv[i]);
	}

	if (_strcmp((const char *)argv[0], "run") == 0) {
		run_flag = 1;
		if (xTaskCreate(enc_verify_task, "enc_verify", VERIFY_TASK_STACK_SIZE, NULL, 2, NULL) != pdPASS) {
			printf("Fail to create encoder test task, memory not enough\r\n");
			return 0;
		}
		while (run_flag) {
			vTaskDelay(1000);
		}
		DiagPrintf("======================================\n\r");
		if (fail_cnt) {
			DiagPrintf("FAIL with %d PASS, %d FAIL\n\r", pass_cnt, fail_cnt);
		} else {
			DiagPrintf("TOTAL PASS!\n\r");
		}
		DiagPrintf("======================================\n\r");
	} else if (_strcmp((const char *)argv[0], "regr") == 0) {
		printf("0x%X \n\r", HAL_READ32(0x60109000, (strtol(argv[1], NULL, 0) * 4)));
	} else if (_strcmp((const char *)argv[0], "regw") == 0) {
		printf("0x%X (B) \n\r", HAL_READ32(0x60109000, (strtol(argv[1], NULL, 0) * 4)));
		HAL_WRITE32(0x60109000, (strtol(argv[1], NULL, 0) * 4), strtol(argv[2], NULL, 0));
		printf("0x%X (A) \n\r", HAL_READ32(0x60109000, (strtol(argv[1], NULL, 0) * 4)));
	} else if (_strcmp((const char *)argv[0], "dump") == 0) {
		enc_reg_dump();
	} else {
		DiagPrintf("Fail: No option\n\r");
	}

	return 0;
}

extern u32 EncCmdTest2(IN u16 argc, IN u8 *argv[]);

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_encoder_test_cmd[] = {
	/* command string is upper case only, and user input argv[0] in console will convert to upper case */
	{
		"ENC", EncCmdTest
	},
	{
		"ENC2", EncCmdTest2
	},
};
