#include <stdio.h>
#include "stdint.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "video_app_rcc.h"
#include "vfs.h"

extern void mmf2_video_example_vipnn_init(char *user_cmd);
extern void mmf2_video_example_joint_test_init(char *user_cmd);
extern void mmf2_video_example_bandwidth_test_case_1(char *user_cmd);
extern void mmf2_video_example_uvcd_array_init(char *user_cmd);
extern void mmf2_video_example_uvcd_video_init(char *user_cmd);
extern void mmf2_video_example_isp_init(char *user_cmd);
extern void mmf2_video_example_uvcd_video_tuning(char *user_cmd);
extern void mmf2_video_example_array_encoder_init(char *user_cmd);
extern void mmf2_video_example_test_vin_init(char *user_cmd);
extern void mmf2_video_example_test_vin_set_log_en(int enable);
extern void mmf2_video_example_test_vin_show_strm_info(void);
extern void mmf2_video_example_test_vin_capture(const char *postfix);
extern void mmf2_video_example_test_vin_load_iq(const char *filename);
extern void mmf2_video_example_test_vin_set_zoom_linespace(uint8_t ls);
extern void mmf2_video_example_test_vin_stream_test(int ch, int cycles, int hold_ms);
extern void mmf2_video_example_test_vin_set_mode(int mode, char *user_cmd);
extern void mmf2_video_example_test_vin_input(const char *file, int width, int height, int fmt);
extern void mmf2_video_example_test_vin_input_stop(void);
extern void mmf2_video_example_test_vin_input_set_large_image_mode(int enable);
extern void mmf2_video_example_test_vin_large_image(const char *file);
extern void mmf2_video_example_test_vin_large_image_stop(void);
extern void mmf2_video_example_test_vin_large_image_capture(const char *suffix);
extern void mmf2_video_example_large_image_pipeline_start(void);
extern void mmf2_video_example_large_image_pipeline_stop(void);
extern void mmf2_video_example_large_image_pipeline_set_log_en(int enable);
extern void mmf2_video_example_large_image_pipeline_capture(const char *suffix);
extern void mmf2_video_example_4k_test_init(char *user_cmd);

extern void mmf2_video_example_osdi_test_init(int argc, u8 **argv);
extern void mmf2_video_example_osdi_stop(int channel);

extern void mmf2_video_example_dbg_raw_init(int argc, u8 **argv);

#include "hal_md.h"
extern void md_test_task(void *pvParameters);   /* hal/test_md.c */
extern void osdi_test_task(void *pvParameters);
#ifndef RTS_ISP_RUNNING
#define RTS_ISP_RUNNING 2
#endif
extern int32_t rts_av_isp_get_status(void);

//------------------------------------------------------------------------------
// MD motion detection task
//------------------------------------------------------------------------------

#define MD_ISP_W   2688
#define MD_ISP_H   1520
#define MD_GRID_W   288
#define MD_GRID_H   162

static void md_poll_task(void *pvParameters)
{
	void *md = NULL;
	struct rts_isp_md_attr attr = {0};
	struct rtsc_dma_buffer hist = {0}, res = {0};
	uint32_t hist_sz, res_sz;
	int sx, sy, offx, offy;
	int ret;

	(void)pvParameters;

	while (rts_av_isp_get_status() < RTS_ISP_RUNNING) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}

	ret = rts_isp_md_init(&md);
	if (ret) {
		DiagPrintf("[md] init failed %d\r\n", ret);
		goto done;
	}

	sx   = MD_ISP_W / MD_GRID_W;
	sy   = MD_ISP_H / MD_GRID_H;
	offx = (MD_ISP_W - sx * MD_GRID_W) / 2;
	offy = (MD_ISP_H - sy * MD_GRID_H) / 2;

	attr.md_in.x       = offx;
	attr.md_in.y       = offy;
	attr.md_in.w       = MD_GRID_W;
	attr.md_in.h       = MD_GRID_H;
	attr.md_in.scale_x = sx;
	attr.md_in.scale_y = sy;
	attr.md_out.w      = MD_GRID_W;
	attr.md_out.h      = MD_GRID_H;
	attr.md_out.scale_x = 1;
	attr.md_out.scale_y = 1;
	attr.roi.w         = MD_GRID_W;
	attr.roi.h         = MD_GRID_H;
	attr.nr_bins       = MD_NR_BINS_16;
	attr.bin_bits      = MD_BIN_BITS_2;
	attr.thd.learn     = 248;
	attr.thd.forget    = 100;
	attr.thd.back      = 1;
	attr.train_frames  = 5;

	ret = rts_isp_md_set_attr(md, &attr);
	if (ret) {
		DiagPrintf("[md] set_attr failed %d\r\n", ret);
		goto release;
	}

	rts_isp_md_calc_dma_size(&attr, &hist_sz, &res_sz);
	hist.length = hist_sz;
	res.length  = res_sz;

	ret = rts_isp_md_alloc_dma(&hist);
	if (ret) {
		DiagPrintf("[md] hist alloc failed %d\r\n", ret);
		goto release;
	}
	ret = rts_isp_md_alloc_dma(&res);
	if (ret) {
		DiagPrintf("[md] res alloc failed %d\r\n", ret);
		goto free_hist;
	}

	ret = rts_isp_md_set_dma_addr(md,
								  hist.phy_addr, hist.length,
								  res.phy_addr,  res.length);
	if (ret) {
		DiagPrintf("[md] set_dma_addr failed %d\r\n", ret);
		goto free_all;
	}

	rts_isp_md_enable(md, 1);
	DiagPrintf("[md] running — polling for motion\r\n");

	while (1) {
		struct rts_isp_md_result result;

		if (rts_isp_md_poll(md, -1) != RTS_TRUE) {
			continue;
		}

		if (rts_isp_md_get_result(md, &result) == RTS_OK &&
			result.motion_flag)
			DiagPrintf("[md] motion detected! count=%u\r\n",
					   result.motion_count);

		rts_isp_md_done(md);
	}

free_all:
	rts_isp_md_free_dma(&res);
free_hist:
	rts_isp_md_free_dma(&hist);
release:
	rts_isp_md_enable(md, 0);
	rts_isp_md_release(md);
done:
	vTaskDelete(NULL);
}

//------------------------------------------------------------------------------
// video examples
//------------------------------------------------------------------------------
static void video_example_main(void *param)
{
	DiagPrintf("video_example_main...\r\n");
	//wifi_common_init();

	/* ISP/Array --> NN */
	//mmf2_video_example_vipnn_init((char *)param);

	/* ISP --> Encoder */
	/* ISP/Array --> NN */
	//mmf2_video_example_joint_test_init((char *)param);

#if defined(CONFIG_4K_DEV) && (CONFIG_4K_DEV == 1) && defined(CONFIG_ARM_CORE_CM4_VP) && (CONFIG_ARM_CORE_CM4_VP == 1)
	/* ISP 4K --> Encoder --> UVC */
	mmf2_video_example_4k_test_init((char *)param);
#elif defined(CONFIG_SUPPORT_UVC_TUNING) && (CONFIG_SUPPORT_UVC_TUNING == 1)
	/* Array ---> UVC */
	//mmf2_video_example_uvcd_array_init((char *)param);

	/* ISP/ENC --->UVC */
	mmf2_video_example_uvcd_video_init((char *)param);

	/* TUNING TOOL */
	//mmf2_video_example_uvcd_video_tuning((char *)param);
#endif
	/* bandwidth test case 1 */
	//mmf2_video_example_bandwidth_test_case_1((char *)param);

	/* ISP only */
	//mmf2_video_example_isp_init((char *)param);

	/* Array --> Encoder */
	//mmf2_video_example_array_encoder_init((char *)param);

	vTaskDelete(NULL);
}

/* VP self test example */
void video_example_media_framework(void *param)
{
	/* video IP function & clock init */
	video_app_common_rcc_init();

	/* user can start their own task here */
	DiagPrintf("video_example_media_framework...\r\n");
	if (xTaskCreate(video_example_main, ((const char *)"mmf2_video"), 4096, param, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		DiagPrintf("\r\n video_example_main: Create Task Error\n");
	}
}

extern int video_test_case_idx;



void write_to_sd(void *data, uint32_t size, char *filename)
{
	int ret = 0;
	FILE *finfo;
	char filepath[64];

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret != 0) {
		DiagPrintf("[%s] VFS-FAT Init Fail\r\n", __FUNCTION__);
		goto exit;
	}

	snprintf(filepath, sizeof(filepath), "fat:%s", filename);
	finfo = fopen(filepath, "w");
	if (finfo == NULL) {
		DiagPrintf("[%s] fopen(%s) failed\r\n", __FUNCTION__, filepath);
		goto exit;
	}

	ret = fwrite(data, 1, size, finfo);
	if (ret != (int)size) {
		DiagPrintf("[%s] fwrite() failed, wrote %d of %u bytes\r\n",
				   __FUNCTION__, ret, size);
	} else {
		DiagPrintf("[%s] fwrite() succeeded, wrote %u bytes to %s\r\n",
				   __FUNCTION__, size, filepath);
	}

	fclose(finfo);

exit:
	vfs_target_unregister("fat");
}

void vfs_sd_task(void *dummy)
{
	UNUSED(dummy);
	int ret = 0;
	//vTaskDelay(5000);
	DiagPrintf("vfs_task \n");
	char content[] = "VFS module works normally !!!";
	// vfs_init();

	FILE *finfo;
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret == 0) {
		DiagPrintf("VFS-FAT Init Success \n");
	} else {
		DiagPrintf("VFS-FAT Init Fail \n");
	}

	finfo = fopen("fat:test.txt", "w");
	if (finfo == NULL) {
		DiagPrintf("[%s] fopen() failed\r\n", __FUNCTION__);
		goto exit;
	}

	ret = fwrite(content, strlen(content), 1, finfo);
	if (ret != (int)strlen(content)) {
		DiagPrintf("[%s] fwrite() failed, err is %d\r\n", __FUNCTION__,
				   ret);
	} else {
		DiagPrintf(
			"[%s] fwrite() succeeded, write %d characters:%s\r\n",
			__FUNCTION__, strlen(content), content);
	}

	fclose(finfo);

exit:
	vfs_target_unregister("fat");
	vTaskDelete(NULL);
}


static void reg_dump_task(void *param)
{
	const uint32_t base  = 0x60000000U;
	const uint32_t count = 262144U;  /* 1 MB / 4 bytes */
	uint32_t i;

	UNUSED(param);

	DiagPrintf("Dumping %u registers from 0x%08x...\r\n", count, base);
	for (i = 0; i < count; i += 8) {
		DiagPrintf("[%08x] %08x %08x %08x %08x %08x %08x %08x %08x\r\n",
				   (unsigned)(base + i * 4),
				   (unsigned)HAL_READ32(base, (i + 0) * 4),
				   (unsigned)HAL_READ32(base, (i + 1) * 4),
				   (unsigned)HAL_READ32(base, (i + 2) * 4),
				   (unsigned)HAL_READ32(base, (i + 3) * 4),
				   (unsigned)HAL_READ32(base, (i + 4) * 4),
				   (unsigned)HAL_READ32(base, (i + 5) * 4),
				   (unsigned)HAL_READ32(base, (i + 6) * 4),
				   (unsigned)HAL_READ32(base, (i + 7) * 4));
		/* One tick delay per line lets the UART ISR drain the log ring
		 * buffer (~85 bytes/line) before the next line is enqueued.
		 * Without this the ring buffer overflows and chars are silently
		 * dropped, producing corrupted output. */
		vTaskDelay(1);
	}
	DiagPrintf("Dump complete.\r\n");
	vTaskDelete(NULL);
}

#define TEST_VIN_REG_BASE 0x60000000U

u32 CmdVideoAppTest(IN u16 argc, IN u8 *argv[])
{
	if (_strcmp((const char *)argv[0], "run") == 0) {
		DiagPrintf("video run...\r\n");
		if (argc > 1) {
			video_test_case_idx = atoi((const char *)argv[1]);
		}
		video_example_media_framework(NULL);
	} else if (_strcmp((const char *)argv[0], "vin") == 0) {
		if (argc > 1 && _strcmp((const char *)argv[1], "log_en") == 0) {
			int en = (argc > 2) ? (int)strtol((const char *)argv[2], NULL, 0) : 0;
			mmf2_video_example_test_vin_set_log_en(en);
		} else if (argc > 1 && _strcmp((const char *)argv[1], "strm_info") == 0) {
			mmf2_video_example_test_vin_show_strm_info();
		} else if (argc > 1 && _strcmp((const char *)argv[1], "capture") == 0) {
			mmf2_video_example_test_vin_capture(argc > 2 ? (const char *)argv[2] : NULL);
		} else if (argc > 1 && _strcmp((const char *)argv[1], "ycol") == 0) {
			mmf2_video_example_test_vin_ycol();
		} else if (argc > 1 && _strcmp((const char *)argv[1], "zoom_ls") == 0) {
			if (argc < 3) {
				DiagPrintf("usage: video vin zoom_ls <val>\r\n");
			} else {
				uint8_t ls = (uint8_t)strtol((const char *)argv[2], NULL, 0);
				mmf2_video_example_test_vin_set_zoom_linespace(ls);
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "load_iq") == 0) {
			if (argc > 2) {
				mmf2_video_example_test_vin_load_iq((const char *)argv[2]);
			} else {
				DiagPrintf("usage: video vin load_iq <packed_iq_file_name>\r\n");
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "mode") == 0) {
			if (argc > 2) {
				int mode = (int)strtol((const char *)argv[2], NULL, 0);

				mmf2_video_example_test_vin_set_mode(mode, argc > 3 ? (char *)argv[3] : NULL);
			} else {
				DiagPrintf("usage: video vin mode <0-3>  (0=linear 1=VC 2=DUMMY 3=NODUMMY)\r\n");
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "write_reg") == 0) {
			if (argc > 3) {
				uint32_t addr = (uint32_t)strtoul((const char *)argv[2], NULL, 0);
				uint32_t val  = (uint32_t)strtoul((const char *)argv[3], NULL, 0);

				HAL_WRITE32(TEST_VIN_REG_BASE, addr, val);
				DiagPrintf("[test_vin] write [0x%08x] = 0x%08x\r\n",
						   (unsigned)(TEST_VIN_REG_BASE + addr), (unsigned)val);
			} else {
				DiagPrintf("usage: video vin write_reg <addr> <val>\r\n");
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "large_image_test") == 0) {
			if (argc > 2 && _strcmp((const char *)argv[2], "stop") == 0) {
				mmf2_video_example_test_vin_large_image_stop();
			} else if (argc > 2 && _strcmp((const char *)argv[2], "capture") == 0) {
				mmf2_video_example_test_vin_large_image_capture(argc > 3 ? (const char *)argv[3] : NULL);
			} else if (argc > 2) {
				mmf2_video_example_test_vin_large_image((const char *)argv[2]);
			} else {
				DiagPrintf("usage: video vin large_image_test <file>\r\n");
				DiagPrintf("       video vin large_image_test stop\r\n");
				DiagPrintf("       video vin large_image_test capture [suffix]\r\n");
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "large_image_pipeline") == 0) {
			if (argc > 2 && _strcmp((const char *)argv[2], "stop") == 0) {
				mmf2_video_example_large_image_pipeline_stop();
			} else if (argc <= 2 || _strcmp((const char *)argv[2], "start") == 0) {
				mmf2_video_example_large_image_pipeline_start();
			} else if (argc > 2 && _strcmp((const char *)argv[2], "log_en") == 0) {
				int en = (argc > 3) ? (int)strtol((const char *)argv[3], NULL, 0) : 0;
				mmf2_video_example_large_image_pipeline_set_log_en(en);
			} else if (argc > 2 && _strcmp((const char *)argv[2], "capture") == 0) {
				mmf2_video_example_large_image_pipeline_capture(argc > 3 ? (const char *)argv[3] : NULL);
			} else {
				DiagPrintf("usage: video vin large_image_pipeline [start]\r\n");
				DiagPrintf("       video vin large_image_pipeline stop\r\n");
				DiagPrintf("       video vin large_image_pipeline log_en <0|1>\r\n");
				DiagPrintf("       video vin large_image_pipeline capture [suffix]\r\n");
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "input") == 0) {
			if (argc > 2 && _strcmp((const char *)argv[2], "stop") == 0) {
				mmf2_video_example_test_vin_input_stop();
			} else if (argc > 2 && _strcmp((const char *)argv[2], "large_image_mode") == 0) {
				if (argc > 3 && _strcmp((const char *)argv[3], "on") == 0) {
					mmf2_video_example_test_vin_input_set_large_image_mode(1);
				} else if (argc > 3 && _strcmp((const char *)argv[3], "off") == 0) {
					mmf2_video_example_test_vin_input_set_large_image_mode(0);
				} else {
					DiagPrintf("usage: video vin input large_image_mode <on|off>\r\n");
				}
			} else if (argc > 5) {
				const char *file = (const char *)argv[2];
				int w = (int)strtol((const char *)argv[3], NULL, 0);
				int h = (int)strtol((const char *)argv[4], NULL, 0);
				int fmt = (int)strtol((const char *)argv[5], NULL, 0);
				mmf2_video_example_test_vin_input(file, w, h, fmt);
			} else {
				DiagPrintf("usage: video vin input <file> <w> <h> <fmt>\r\n");
				DiagPrintf("       video vin input stop\r\n");
				DiagPrintf("       video vin input large_image_mode <on|off>\r\n");
				DiagPrintf("  fmt: 0=SGRBG12 1=SBGGR12 2=SGBRG12 3=SRGGB12 4=SGRBG32\r\n");
				DiagPrintf("       5=RGB12P 6=YUV12P 7=YUYV 8=NV16\r\n");
			}
		} else if (argc > 1 && _strcmp((const char *)argv[1], "read_reg") == 0) {
			if (argc > 2) {
				uint32_t addr  = (uint32_t)strtoul((const char *)argv[2], NULL, 0);
				uint32_t count = (argc > 3) ? (uint32_t)strtoul((const char *)argv[3], NULL, 0) : 1;
				uint32_t i;

				for (i = 0; i < count; i++) {
					uint32_t off = addr + i * 4;

					DiagPrintf("[test_vin] [0x%08x] = 0x%08x\r\n",
							   (unsigned)(TEST_VIN_REG_BASE + off),
							   (unsigned)HAL_READ32(TEST_VIN_REG_BASE, off));
				}
			} else {
				DiagPrintf("usage: video vin read_reg <addr> [count]\r\n");
			}
		} else {
			DiagPrintf("video vin test...\r\n");
			mmf2_video_example_test_vin_init(argc > 1 ? (char *)argv[1] : NULL);
		}
	} else if (_strcmp((const char *)argv[0], "dump_regs") == 0) {
		if (xTaskCreate(reg_dump_task, "reg_dump", 1024, NULL,
						configMAX_PRIORITIES - 1, NULL) != pdPASS) {
			DiagPrintf("Failed to create reg_dump task\r\n");
		}
	} else if (_strcmp((const char *)argv[0], "md_test") == 0) {
		DiagPrintf("Starting ISP + MD HAL test...\r\n");
		mmf2_video_example_test_vin_init(argc > 1 ? (char *)argv[1] : NULL);
		if (xTaskCreate(md_test_task, "md_test", 4096, NULL,
						tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			DiagPrintf("Failed to create md_test task\r\n");
		}
	} else if (_strcmp((const char *)argv[0], "md") == 0) {
		DiagPrintf("Starting ISP + MD motion detection...\r\n");
		mmf2_video_example_test_vin_init(argc > 1 ? (char *)argv[1] : NULL);
		if (xTaskCreate(md_poll_task, "md_poll", 4096, NULL,
						tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			DiagPrintf("Failed to create md_poll task\r\n");
		}
	} else if (_strcmp((const char *)argv[0], "dbg_osdi") == 0) {
		mmf2_video_example_osdi_test_init(argc - 1, (u8 **)(argv + 1));
	} else if (_strcmp((const char *)argv[0], "dbg_raw") == 0) {
		mmf2_video_example_dbg_raw_init(argc - 1, (u8 **)(argv + 1));
	} else if (_strcmp((const char *)argv[0], "osdi_stop") == 0) {
		int ch = (argc > 1) ? (int)strtol((const char *)argv[1], NULL, 0) : 0;
		mmf2_video_example_osdi_stop(ch);
	} else if (_strcmp((const char *)argv[0], "sd_test") == 0) {
		DiagPrintf("Testing VFS SD card...\n");

		/* Create a task with sufficient stack for VFS operations */
		if (xTaskCreate(vfs_sd_task, "vfs_sd_task",
						4096, /* Stack size in words (16KB) */
						NULL, /* Parameter */
						tskIDLE_PRIORITY + 1, /* Priority */
						NULL /* Task handle */
					   ) != pdPASS) {
			DiagPrintf("Failed to create vfs_sd_task\n");
		} else {
			DiagPrintf("VFS SD task created successfully\n");
		}
	} else if (_strcmp((const char *)argv[0], "osdi_verify") == 0) {
		DiagPrintf("Starting OSDI HAL verify...\r\n");
		if (xTaskCreate(osdi_test_task, "osdi_test", 4096, NULL,
						tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
			DiagPrintf("[osdi_test] create task fail\r\n");
		}
	} else if (_strcmp((const char *)argv[0], "stream_test") == 0) {
		int ch     = (argc > 1) ? (int)strtol((const char *)argv[1], NULL, 0) : 0;
		int cycles = (argc > 2) ? (int)strtol((const char *)argv[2], NULL, 0) : 10;
		int hold   = (argc > 3) ? (int)strtol((const char *)argv[3], NULL, 0) : 1000;
		DiagPrintf("Starting stream open/close test: ch=%d cycles=%d hold=%dms\r\n", ch, cycles, hold);
		mmf2_video_example_test_vin_stream_test(ch, cycles, hold);
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   video_app_test_cmd[] = {
	{
		"VIDEO", CmdVideoAppTest
	},
};
