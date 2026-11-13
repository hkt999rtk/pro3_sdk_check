/******************************************************************************
*
* Copyright(c) 2007 - 2025 Realtek Corporation. All rights reserved.
*
******************************************************************************/

#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "rts_isp_ae.h"
#include "rts_isp_awb.h"

#include <time.h>
#include "isp_os_time.h"

//--I2C setup--//
#include "gpio_api.h"   // mbed
#include "i2c_api.h"
#include "../../sensor/verification/sensor_start.h"
#define GPIO_SENSOR_PWDN    _PA_2
#define GPIO_SENSOR_RST     _PA_5

#define I2C_0               0
#define I2C_1               1
#define MBED_I2C_BUS_CLK    400000  //hz
#define MBED_I2C_MTR_SDA    0 //_PA_3
#define MBED_I2C_MTR_SCL    0 //_PA_4
sensor_i2c_t sensor_i2c;
int verify_i2c_inited = 0;
//--I2C setup--//


#ifdef __REMAP_REAL_HDR_SNR__
extern int hdr_remap_ratio;
extern int hdr_burst_id;
extern int hdr_remap_en;
extern int hdrremap_fps;
extern int hdr_remap_ratio_base;
extern int hdr_remap_tnr;
extern int hdr_remap_vhdr_fbc;
extern int hdr_remap_maxburst;
#endif

#if defined(REALSENSOR) && (REALSENSOR==1)
#define CTRL_SW 1
#else
#define CTRL_SW 0
#endif

static int verify_i2c_init(void)
{
	if (verify_i2c_inited == 1) {
		DiagPrintf("i2c is initialized.\r\n");
		return 1;
	}
	memset(&sensor_i2c, 0, sizeof(sensor_i2c));
	sensor_i2c.master.i2c_idx = I2C_1;
	//i2c_init(&sensor_i2c.master, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	//i2c_frequency(&sensor_i2c.master, MBED_I2C_BUS_CLK);
	memcpy(&sensor_i2c.master, sensor_i2c_get_adapter(), sizeof(i2c_t));

	sensor_i2c.info.slave_addr = 0x1A;
	printf("sensor i2c slave address: 0x%x\r\n", sensor_i2c.info.slave_addr);
	sensor_i2c.info.register_addr_len = 2;
	sensor_i2c.info.data_len = 1;
	verify_i2c_inited = 1;
	return 1;
}
static uint8_t verify_i2c_read(uint16_t addr)
{
	if (verify_i2c_inited == 0) {
		verify_i2c_init();
	}
	rts_isp_i2c_reg_t reg;

	reg.addr = addr;
	sensor_i2c_read_reg(&sensor_i2c, &reg);
	return reg.data;
}
static uint8_t verify_i2c_write(uint16_t addr, uint8_t data)
{
	if (verify_i2c_inited == 0) {
		verify_i2c_init();
	}
	rts_isp_i2c_reg_t reg;

	reg.addr = addr;
	reg.data = data;
	sensor_i2c_write_reg(&sensor_i2c, &reg);
	reg.data = 0;
	sensor_i2c_read_reg(&sensor_i2c, &reg);
	return reg.data;
}

static void print_ae_statis(const struct rts_isp_ae_stat_info *stat_info,
							const struct rts_isp_ae_statis *statis)
{
	int i;
	int chan;
	int win_num;

	printf("frame count: %d\n", statis->frame_count);

	for (chan = 0; chan < RTS_ISP_HDR_CHAN_MAX; chan++) {
		win_num = (stat_info->hdr[chan].win.cols *
				   stat_info->hdr[chan].win.rows);
		if (!stat_info->hdr[chan].hist_bins || !win_num) {
			break;
		}

		printf("ae[%d] statis:\n", chan);
		printf("y_mean:\n");
		for (i = 0; i < win_num; i++) {
			printf("%03x ", statis->hdr[chan].y_mean[i]);
			if ((i + 1) % 16 == 0) {
				printf("\n");
			}
		}
		printf("histogram:\n");
		for (i = 0; i < stat_info->hdr[chan].hist_bins; i++) {
			printf("%08x ", statis->hdr[chan].hist[i]);
			if ((i + 1) % 8 == 0) {
				printf("\n");
			}
		}
	}

	printf("ae statis:\n");
	printf("y_mean:\n");
	win_num = stat_info->win.cols * stat_info->win.rows;
	for (i = 0; i < win_num; i++) {
		printf("%03x ", statis->y_mean[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		}
	}
	printf("histogram:\n");
	for (i = 0; i < stat_info->hist_bins; i++) {
		printf("%08x ", statis->hist[i]);
		if ((i + 1) % 8 == 0) {
			printf("\n");
		}
	}
}
static void print_awb_statis(const struct rts_isp_awb_stat_info *stat_info,
							 const struct rts_isp_awb_statis *statis)
{
	int i;
	int win_num;

	win_num = stat_info->win.cols * stat_info->win.rows;

	printf("frame count: %d\n", statis->frame_count);

	printf("r_mean:\n");
	for (i = 0; i < win_num; i++) {
		printf("%04x", statis->r_mean[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("g_mean:\n");
	for (i = 0; i < win_num; i++) {
		printf("%04x", statis->g_mean[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("b_mean:\n");
	for (i = 0; i < win_num; i++) {
		printf("%04x", statis->g_mean[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("y_mean:\n");
	for (i = 0; i < win_num; i++) {
		printf("%04x", statis->y_mean[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("rg:\n");
	for (i = 0; i < win_num; i++) {
		printf("%03x", statis->rg[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("bg:\n");
	for (i = 0; i < win_num; i++) {
		printf("%03x", statis->bg[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("pixels:\n");
	for (i = 0; i < win_num; i++) {
		printf("%04x", statis->pixels[i]);
		if ((i + 1) % 16 == 0) {
			printf("\n");
		} else {
			printf(" ");
		}
	}

	printf("illum_white_pixels:\n");
	for (i = 0; i < 6; i++) {
		printf("%06x ", statis->illum_white_pixels[i]);
	}
	printf("\n");

	printf("fine_r_sum: %08x\n", statis->fine_r_sum);
	printf("fine_g_sum: %08x\n", statis->fine_g_sum);
	printf("fine_b_sum: %08x\n", statis->fine_b_sum);
	printf("fine_white_pixels: %06x\n", statis->fine_white_pixels);
}

int isp_verification_reg(void)
{
	DiagPrintf("ISP test start\n\r");
	//main_base_test();
	return 0;
}

int isp_verification_stream(int *arg)
{
	DiagPrintf("ISP IO Stream test start\n\r");
	return main_multistream(arg);
}

int isp_verification_io(int *arg)
{
	DiagPrintf("ISP IO test start\n\r");
#if CTRL_SW
	DiagPrintf("Control SW enable(verification off)\n\r");
#else
	return isp_io_start(arg);
#endif
}

int isp_verification_ctrlsw_init(void)
{
	int ret = 0;
#if CTRL_SW
	int value = HAL_READ32(0x40220000, 0xAC);
	DiagPrintf("0x4022_00ac: 0x%X\n\r", value);

	HAL_WRITE32(0x40220000, 0xAC, value | 1); //0x4022_00ac
	ret = probe_all();
	if (ret) {
		printf("[%s] probe_all ret Fail", __FUNCTION__);
		return ret;
	}
	DiagPrintf("ISP SW Init test(isp_mcu_apps_start) start\n\r");
	isp_mcu_apps_start();
#else
	DiagPrintf("Control SW disable\n\r");
#endif
	return ret;
}


extern void *isp_io_base;
int isp_verification_ctrlsw_camera(void)
{
//#if CTRL_SW
//	isp_io_base = (void *)0x60000000;
//	DiagPrintf("ISP SW Camera test start\n\r");
//	rts_camera_start();
//	return 1;
//#else
//	DiagPrintf("Control SW disable\n\r");
//	return 0;
//#endif
}

/*******************************************/
/*			Add command to table           */
/*******************************************/
u32
CmdISPTest(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	DiagPrintf("ISP command\n\rargc %d\n\r", argc);

	for (int i = 0; i < argc; i++) {
		DiagPrintf("argv[%d] = %s\n\r", i, argv[i]);
	}

	//if(argc!=4) {
	//	DiagPrintf("argc num is not 4\n\r");
	//	return 0;
	//}

	if (_strcmp((const char *)argv[0], "reg_test") == 0) {
		if (isp_verification_reg() == 0) {
			DiagPrintf("Pass\n\r");
		} else {
			DiagPrintf("Fail\n\r");
		}
	} else if (_strcmp((const char *)argv[0], "stream_real_fixp") == 0) {
		dump_init();
		dump_reset();
		DiagPrintf("cmd: stream_real_fixp\n\r");
		int arg[] = { 1, 0, 0 };
		if (isp_verification_ctrlsw_init() == 0) {
			DiagPrintf("ctrlsw_init Pass\n\r");
			vTaskDelay(500);
			verify_i2c_write(0x308C, 0x33); // 0x23 0x2 0x33
			DiagPrintf("read i2c 0x308C: 0x%X\n\r", verify_i2c_read(0x308C));
			if (isp_verification_stream(arg) == 0) {
				DiagPrintf("stream Pass\n\r");
			} else {
				DiagPrintf("stream Fail\n\r");
			}
		} else {
			DiagPrintf("ctrlsw_init Fail\n\r");
		}
	} else if (_strcmp((const char *)argv[0], "stream") == 0) {
		DiagPrintf("cmd: stream\n\r");
		int arg[] = { 0, 0, 0 };
		if (isp_verification_stream(arg) == 0) {
			DiagPrintf("Pass\n\r");
		} else {
			DiagPrintf("Fail\n\r");
		}
	} else if (_strcmp((const char *)argv[0], "io") == 0) {
		DiagPrintf("cmd: io\n\r");
		int arg[] = {
			strtol(argv[1], NULL, 0), strtol(argv[2], NULL, 0), strtol(argv[3], NULL, 0), strtol(argv[4], NULL, 0), strtol(argv[5], NULL, 0), /* input-type, output-type, zoom_en, save_output, NV12/NV16/YUYV */
			strtol(argv[6], NULL, 0), strtol(argv[7], NULL, 0), strtol(argv[8], NULL, 0), strtol(argv[9], NULL, 0), strtol(argv[10], NULL, 0), strtol(argv[11], NULL, 0), /* input_w, h, crop_w, h, output_w, h */
			strtol(argv[12], NULL, 0), strtol(argv[13], NULL, 0), strtol(argv[14], NULL, 0), strtol(argv[15], NULL, 0), strtol(argv[16], NULL, 0), strtol(argv[17], NULL, 0), /* design_pattern, dbg_path, fixp, modules, mask, LDC */
		};
		if (isp_verification_io(arg) == 0) {
			DiagPrintf("Pass\n\r");
		} else {
			DiagPrintf("Fail\n\r");
		}
	} else if (_strcmp((const char *)argv[0], "ctrlsw") == 0) {
		DiagPrintf("cmd: ctrlsw\n\r");
		if (_strcmp((const char *)argv[1], "init") == 0) {
			DiagPrintf("cmd: ctrlsw-init\n\r");
#ifdef __REMAP_REAL_HDR_SNR__
			if (argc >= 3) {
				hdr_remap_ratio = strtol(argv[2], NULL, 0);
				DiagPrintf("hdr_remap_ratio %d \n\r", hdr_remap_ratio);
			}
			if (argc >= 4) {
				hdr_burst_id = strtol(argv[3], NULL, 0);
				DiagPrintf("hdr_burst_id %d \n\r", hdr_burst_id);
			}
			if (argc >= 5) {
				hdr_remap_en = strtol(argv[4], NULL, 0);
				DiagPrintf("hdr_remap_en %d \n\r", hdr_remap_en);
			}
			if (argc >= 6) {
				hdrremap_fps = strtol(argv[5], NULL, 0);
				DiagPrintf("hdrremap_fps %d \n\r", hdrremap_fps);
			}
			if (argc >= 7) {
				hdr_remap_ratio_base = strtol(argv[6], NULL, 0);
				DiagPrintf("hdr_remap_ratio_base %d \n\r", hdr_remap_ratio_base);
			}
			if (argc >= 8) {
				hdr_remap_tnr = strtol(argv[7], NULL, 0);
				DiagPrintf("hdr_remap_tnr %d \n\r", hdr_remap_tnr);
			}
			if (argc >= 9) {
				hdr_remap_vhdr_fbc = strtol(argv[8], NULL, 0);
				DiagPrintf("hdr_remap_vhdr_fbc %d \n\r", hdr_remap_vhdr_fbc);
			}
			if (argc >= 10) {
				hdr_remap_maxburst = strtol(argv[9], NULL, 0);
				DiagPrintf("hdr_remap_maxburst %d \n\r", hdr_remap_maxburst);
			}
#if 0
			if (argc >= 11) {
				hdr_remap_ddr = strtol(argv[10], NULL, 0);
				DiagPrintf("hdr_remap_ddr %d \n\r", hdr_remap_ddr);
			}
			if (argc >= 12) {
				target_frame = strtol(argv[11], NULL, 0);
				DiagPrintf("target_frame %d \n\r", target_frame);
			}
#endif

#endif
			if (isp_verification_ctrlsw_init() == 0) {
				DiagPrintf("Pass\n\r");
			} else {
				DiagPrintf("Fail\n\r");
			}
		} else if (_strcmp((const char *)argv[1], "camera") == 0) {
			DiagPrintf("cmd: ctrlsw-camera\n\r");
			if (isp_verification_ctrlsw_camera() == 0) {
				DiagPrintf("Pass\n\r");
			} else {
				DiagPrintf("Fail\n\r");
			}
		}
	} else if (_strcmp((const char *)argv[0], "awb_statics") == 0) {
		int ret;
		struct rts_isp_awb_stat_info stat_info;
		const struct rts_isp_awb_statis *statis;

		ret = rts_isp_get_awb_statis_info(ISP0, &stat_info);
		if (ret) {
			DiagPrintf("get stat info fail, ret = 0x%X(%d)\r\n", ret, ret);
			return ret;
		}
		ret = rts_isp_query_awb_statis(ISP0, &statis);
		if (ret) {
			DiagPrintf("query statis fail, ret = 0x%X(%d)\r\n", ret, ret);
			return ret;
		}
		ret = rts_isp_get_awb_statis(ISP0, statis);
		if (ret) {
			DiagPrintf("get statis fail, ret = 0x%X(%d)\r\n", ret, ret);
			rts_isp_release_awb_statis(ISP0, statis);
			return ret;
		}

		print_awb_statis(&stat_info, statis);

		rts_isp_release_awb_statis(ISP0, statis);

	} else if (_strcmp((const char *)argv[0], "ae_statics") == 0) {
		int ret;
		struct rts_isp_ae_stat_info stat_info;
		const struct rts_isp_ae_statis *statis;

		ret = rts_isp_get_ae_statis_info(ISP0, &stat_info);
		if (ret) {
			DiagPrintf("get stat info fail, ret = 0x%X(%d)\r\n", ret, ret);
			return ret;
		}
		ret = rts_isp_query_ae_statis(ISP0, &statis);
		if (ret) {
			DiagPrintf("query statis fail, ret = 0x%X(%d)\r\n", ret, ret);
			return ret;
		}
		ret = rts_isp_get_ae_statis(ISP0, statis);
		if (ret) {
			DiagPrintf("get statis fail, ret = 0x%X(%d)\r\n", ret, ret);
			rts_isp_release_ae_statis(ISP0, statis);
			return ret;
		}
		print_ae_statis(&stat_info, statis);

		rts_isp_release_ae_statis(ISP0, statis);

	} else if (_strcmp((const char *)argv[0], "dump") == 0) {
		if (_strcmp((const char *)argv[1], "init") == 0) {
			dump_init();
		} else if (_strcmp((const char *)argv[1], "reset") == 0) {
			dump_reset();
		} else if (_strcmp((const char *)argv[1], "save") == 0) {
			dump_save();
		}
	} else if (_strcmp((const char *)argv[0], "snrregr") == 0) {
		unsigned char buf[32] = {0};
		int addr = strtol(argv[1], NULL, 0);
		int len  = strtol(argv[2], NULL, 0);
		sensor_i2c_t sensor_i2c;
		rts_isp_i2c_reg_t reg;
		reg.addr = addr;
		sensor_imx307_i2c_init(&sensor_i2c);
		sensor_i2c_read_reg(&sensor_i2c, &reg);
		printf("0x%X\r\n", reg.data);

	} else if (_strcmp((const char *)argv[0], "regr") == 0) {
		extern void *isp_io_base;
		isp_io_base = (void *)0x60000000;
		printf("0x%X \n\r", read_reg_dbg(0, strtol(argv[1], NULL, 0)));
	} else if (_strcmp((const char *)argv[0], "regw") == 0) {
		extern void *isp_io_base;
		isp_io_base = (void *)0x60000000;
		printf("0x%X (B) \n\r", read_reg_dbg(0, strtol(argv[1], NULL, 0)));
		write_reg_dbg(0, strtol(argv[2], NULL, 0), strtol(argv[1], NULL, 0));
		printf("0x%X (A) \n\r", read_reg_dbg(0, strtol(argv[1], NULL, 0)));
	} else if (_strcmp((const char *)argv[0], "i2c") == 0) {
		if (_strcmp((const char *)argv[1], "init") == 0) {
			verify_i2c_init();
		} else if (_strcmp((const char *)argv[1], "read") == 0) {
			uint16_t addr = strtol(argv[2], NULL, 0);
			printf("i2c read addr 0x%X: 0x%X \n\r", addr, verify_i2c_read(addr));
		} else if (_strcmp((const char *)argv[1], "write") == 0) {
			uint16_t addr = strtol(argv[2], NULL, 0);
			uint8_t  data = strtol(argv[3], NULL, 0);
			printf("i2c write addr 0x%X: 0x%X(read) \n\r", addr, verify_i2c_write(addr, data));
		}
	} else {
		DiagPrintf("Fail: No option\n\r");
	}

	return 0;
}

static const char *get_clk_src(u32 reg)
{
	switch (reg & 0x07) {
	case 0:
		return "SYS1_PLL";
	case 1:
		return "SYS2_PLL";
	case 2:
		return "PERI_PLL";
	case 3:
		return "DDR_PLL";
	case 4:
		return "USB2_PLL";
	case 5:
		return "CPU_PLL";
	case 6:
		return "XTAL40M";
	case 7:
		return "OSC4M";
	default:
		return "UNKNOWN";
	}
}

static void dump_clk_tree(const char *name, u32 addr, u32 base, int fen_bit)
{
	u32 val = HAL_READ32(addr, 0);
	u32 fen_reg = (fen_bit >= 32) ? (base + 0x01BC) : (base + 0x01B8);
	u32 fen_val = HAL_READ32(fen_reg, 0);
	int fen_on = !!(fen_val & BIT(fen_bit % 32));

	DiagPrintf("  %-12s: reg=0x%08X, src=%-10s, div=%-2d, status=%s\r\n",
			   name, val, get_clk_src(val), (int)((val >> 8) & 0x0F) + 1,
			   fen_on ? "ON" : "OFF");
}

static u32 cmd_isp_enc_dump(u16 argc, u8 *argv[])
{
	(void)argc;
	(void)argv;
	u32 base = 0x60250000;
	u32 sys_1_func = HAL_READ32(base, 0x01B8);
	if (sys_1_func == 0) {
		base = 0x40300000;
		sys_1_func = HAL_READ32(base, 0x01B8);
	}
	u32 sys_2_func = HAL_READ32(base, 0x01BC);

	DiagPrintf("\r\n========== [System Gating Status] ==========\r\n");
	DiagPrintf("  Base Address      : 0x%08X\r\n", base);
	DiagPrintf("  ISP_FUNC_FEN      : %s\r\n", (sys_1_func & BIT(5)) ? "ON" : "OFF");
	DiagPrintf("  ENC_FUNC_FEN      : %s\r\n", (sys_1_func & BIT(8)) ? "ON" : "OFF");
	DiagPrintf("  AIDN_FUNC_FEN     : %s\r\n", (sys_2_func & BIT(27)) ? "ON" : "OFF");
	DiagPrintf("  AI_PSI_SEL        : %s\r\n", (HAL_READ32(base, 0x01B0) & BIT(13)) ? "AI_SCLK" : "PERI_PLL");

	DiagPrintf("\r\n========== [Clock Tree Decode] ==========\r\n");
	dump_clk_tree("AI_SCLK",    base + 0x0160, base, 32 + 27);
	dump_clk_tree("ENC_ACLK",   base + 0x0168, base, 8);
	dump_clk_tree("ENC_SCLK",   base + 0x016C, base, 8);
	dump_clk_tree("FAST_PCLK",  base + 0x0170, base, 15);
	dump_clk_tree("GE_SCLK",    base + 0x0174, base, 32 + 31);
	dump_clk_tree("ISP_MIPICLK", base + 0x017C, base, 5);
	dump_clk_tree("ISP_BUSCLK", base + 0x0180, base, 5);
	dump_clk_tree("ISP_SCLK",   base + 0x0184, base, 5);
	dump_clk_tree("NN_ACLK",    base + 0x018C, base, 12);
	dump_clk_tree("NN_SCLK",    base + 0x0190, base, 12);
	dump_clk_tree("PLF_CLK",    base + 0x0194, base, 10);
	dump_clk_tree("SENSOR_CLK", base + 0x01A0, base, 5);

	if ((sys_1_func & BIT(5)) || (HAL_READ32(base, 0x0184) & BIT(24))) {
		DiagPrintf("\r\n========== [ISP Internal Status] ==========\r\n");
		u32 ctrl1 = HAL_READ32(0x60000000, 0x04);
		u32 idle  = HAL_READ32(0x60000000, 0x28);
		u32 fcnt  = HAL_READ32(0x60000000, 0x20);
		u32 err   = HAL_READ32(0x60000000, 0x24);
		u32 clk_ctrl = HAL_READ32(0x60000000, 0x64);
		DiagPrintf("  ISP_RUNNING_STATE : %s (IdleReg:0x%08X)\r\n", (idle == 0x1FF) ? "IDLE" : "RUNNING", idle);
		DiagPrintf("  ISP_FRAME_COUNT   : %u (8-bit wrap)\r\n", fcnt & 0xFF);
		DiagPrintf("  ISP_ERROR_COUNT   : %u\r\n", err);
		DiagPrintf("  SYS_ISP_CLK_CTRL  : 0x%08X\r\n", clk_ctrl);

		const char *clk_mods[] = {
			"DUMMY_CLK", "MTD_FUNC_CLK", "HDR_FUNC_CLK", "UVT_FUNC_CLK",
			"LDC_FUNC_CLK", "YUVSIN_FUNC_CLK", "YUV2RGB_FUNC_CLK"
		};
		for (int i = 0; i < 7; i++) {
			DiagPrintf("  %-18s: %s\r\n", clk_mods[i], (clk_ctrl & BIT(i)) ? "ON" : "OFF");
		}

		const char *mods[] = {
			"YGamma", "Gamma", "MLSC", "NLSC", "UVT", "HDR", "AE_G", "AWB_G",
			"BLC_L", "LDC", "YGC", "CCM", "DPC_L", "EEH", "SPE", "TNR",
			"DPC_S", "BLC_S", "HUE", "TM", "CAC", "AE_GAIN_L",
			"AE_GAIN_S", "LCE", "BLCH", "MOT_INFO_SYS", "DITHER"
		};
		for (int i = 0; i < 27; i++) {
			DiagPrintf("  %-18s: %s\r\n", mods[i], (ctrl1 & BIT(i)) ? "ENABLED" : "DISABLED");
		}
	}

	DiagPrintf("\r\n========== [OSDI Overlay Status] ==========\r\n");
	u32 osd_stat = HAL_READ32(0x600E0000, 0x04);
	DiagPrintf("  OSDI_RUNNING      : %s\r\n", (osd_stat & BIT(1)) ? "YES" : "NO");
	DiagPrintf("  OSDI_BLK_EN       : 0x%02X\r\n", HAL_READ32(0x600E0000, 0x18) & 0x3F);

	DiagPrintf("\r\n========== [VCE Encoder Status] ==========\r\n");
	if ((sys_1_func & BIT(8)) || (HAL_READ32(base, 0x0168) & BIT(24))) {
		u32 vce_hw_regs = 0x60108010;
		u32 w_reg = HAL_READ32(vce_hw_regs, 0x38);
		u32 h_reg = HAL_READ32(vce_hw_regs, 0x3C);
		DiagPrintf("  ASIC_PRODUCT_ID   : 0x%08X\r\n", HAL_READ32(vce_hw_regs, 0x00));
		DiagPrintf("  ENC_ENABLE_BIT    : %s\r\n", (HAL_READ32(vce_hw_regs, 0x50) & BIT(31)) ? "YES" : "NO");
	} else {
		DiagPrintf("  Encoder Function is OFF.\r\n");
	}
	DiagPrintf("==========================================\r\n");

	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_isp_test_cmd[] = {
	/* command string is upper case only, and user input argv[0] in console will convert to upper case */
	{
		"ISP", CmdISPTest
	},
	{
		"ISP2", CmdISPTest
	},
	{
		"ISP_ENC_DUMP", cmd_isp_enc_dump
	},
};
