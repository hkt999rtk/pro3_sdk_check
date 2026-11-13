/*
 * Realtek Semiconductor Corp.
 * Copyright (C) 2021 Grant Shen <grant_shen@realsil.com.cn>
 */

#include "isp_regs.h"
#include "isp_main.h"
#include "isp_io_type.h"
#include <ameba_soc.h>

#ifdef __REMAP_FIXP_HDR__

#include "addremap_hal.h"

extern int hdr_remap_ratio;
extern int hdr_burst_id;
extern int hdr_remap_en;
extern int hdrremap_fps;
extern int hdr_remap_ratio_base;
extern int hdr_remap_tnr;
extern int hdr_remap_vhdr_fbc;
extern int hdr_remap_maxburst;

#endif



#define ISP_REG_BASE 0xb8a00000
#define ISP_REG_SIZE 0x00100000

#define min(x, y) ((x) < (y) ? (x) : (y))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

extern int g_exit;
extern int frame_num;
static heap_used = 0x80000000;
struct isp_io *pio = NULL;
void ISP_ALL_IRQHandler(void)
{
	DiagPrintf("ISP_ALL_IRQHandler\n\r");
}

static int grid_size = 30;
static int grid_dire = 0;
static int frame_count[5] = {0};

#define MAX_DUMP_SIZE 2688*1520*2
static int dump_addr = 0;
static int dump_size = 2688 * 1520 * 2;
static int dump_inited = 0;
int data_dumped = 1;
int save_cnt = 0;
unsigned int hist_dif = 0xFFFFFFFF;
#if !VHDR
#define _MASK_
#endif

#ifdef __REMAP_FIXP_HDR__
#define _MASK_
#endif

#define _TNR_
void rmp_config_burst_length(struct isp_io *io, uint32_t value)
{
	void *base = io->base;
	write_reg_dbg(base, value, 0xF0190);    // MAX_BURST_LENGTH_SELECT
}

void rmp_fixp_start(struct isp_io *io)
{
	void *base = io->base;
	write_reg_dbg(base, FIXP_START, FIXP_CTRL);
}

void rmp_set_low_latency_mode(int mode)
{
	if (pio == NULL) {
		isperro_printf("isp struct is not initialized. \r\n");
		return;
	}
	pio->low_latency_mode = mode;
}

int rmp_get_low_latency_mode(void)
{
	if (pio == NULL) {
		isperro_printf("isp struct is not initialized. \r\n");
		return;
	}
	return pio->low_latency_mode;
}

void dump_init()
{
	if (dump_inited) {
		return;
	}
	dump_addr = malloc(MAX_DUMP_SIZE);//heap_used;
	//memset((void*)dump_addr, 0, MAX_DUMP_SIZE);
	//heap_used += MAX_DUMP_SIZE;
	dump_inited = 1;
	printf("[%s] init done, dump_addr = 0x%X\r\n", __func__, dump_addr);
}

void dump_reset()
{
	data_dumped = 0;
	printf("[%s] reset done.\r\n", __func__);
}

void dump_file()
{
	int ret;
	FILE *fp;
	char name[128] = {0};
	int seg_size = 960 * 1024;
	int seg_n = (dump_size + seg_size - 1) / seg_size;

	if (dump_addr == 0) {
		printf("[%s] Error: dump_addr is null.\r\n", __func__);
		return;
	}
	if (dump_size == 0) {
		printf("[%s] Error: dump_size is 0.\r\n", __func__);
		return;
	}

	ret = sprintf(name, "D:\\oscaryang\\00_Test\\dump_%08d.raw", save_cnt);
	printf("save size= %d\n", dump_size);
	for (int i = 0; i < seg_n; i++) {
		sprintf(name, "D:\\oscaryang\\00_Test\\dump_%08d_%d.raw", save_cnt, i);
		DiagPrintf("[%s] write seg..%d/%d (%s)..", __FUNCTION__, i + 1, seg_n, name);
		fp = fopen(name, "w");
		if (!fp) {
			return;
		}
		fwrite((void *)(dump_addr + seg_size * i), seg_size, 1, fp);
		fflush(fp);
		fclose(fp);
		DiagPrintf("Done\n\r");
	}
	save_cnt++;
}

void dump_save()
{
	dump_file();
	//xTaskCreate(dump_file, "dump_file", configMINIMAL_STACK_SIZE * 16, NULL,
	//			(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);

}

#include "realsnr_vhist_gt.h"
unsigned int *phist_golden = (unsigned int *)hist_golden;
uint32_t hist_calc(const uint8_t *buf, size_t len, int img_w, int img_h, int *histogram)
{
	for (int i = 0; i < img_h * 3 / 2; i++) {
		unsigned char *line_start = buf + i * img_w;
		for (int j = 0; j < img_w; j++) {
			histogram[i] += line_start[j];
		}
	}

	hist_dif = 0;
	for (int i = 0; i < img_h * 3 / 2; i++) {
		hist_dif += abs(histogram[i] - phist_golden[i]);
		if (i % 10 == 0) {
			DiagPrintf("histogram[i]=%d, phist_golden[i]=%d \r\n", histogram[i], phist_golden[i]);
		}
	}

	DiagPrintf("[%s] hist_dif=%d  img_w=%d, img_h=%d.\r\n", __func__, hist_dif, img_w, img_h);
}

void dump_buffer(struct isp_io *io, int stream_index, int frame_index)
{
	if (data_dumped) {
		return;
	}
	int buf_size = io->io_buffers[stream_index][frame_index].out_buf.length;
	int buf_addr = io->io_buffers[stream_index][frame_index].out_buf.phy_addr;
	DCache_CleanInvalidate((uint32_t)buf_addr, buf_size);

	unsigned char *tmp = (unsigned char *)buf_addr;

	if (buf_addr == 0) {
		printf("[%s] Error: input addr is null.\r\n", __func__);
		return;
	}
	if (buf_size == 0) {
		printf("[%s] Error: input size is 0.\r\n", __func__);
		return;
	}
	if (dump_addr == 0) {
		printf("[%s] Error: dump addr is null.\r\n", __func__);
		return;
	}
	if (MAX_DUMP_SIZE < buf_size) {
		printf("[%s] Error: dump dize too large.\r\n", __func__);
		return;
	}

	dump_size = buf_size;
	memcpy(dump_addr, buf_addr, dump_size);

	struct stream_verify_mode *pmode = stream_verify_get_mode();
	if (pmode->mode == 1) {
		void *base = io->base;
		uint32_t ch0_size = read_reg(base, ZOOM_CH0_OUTPUT_SIZE);
		unsigned int *hist_test = NULL;
		int hist_size = sizeof(unsigned int) * (ch0_size >> 16) * 3 / 2;
		if (!hist_test) {
			hist_test = malloc(hist_size);
			memset(hist_test, 0, hist_size);
		}
		unsigned int g_udCRC = 0;
		g_udCRC = hist_calc((void *)(dump_addr), dump_size, ch0_size & 0x0000FFFF, ch0_size >> 16, hist_test);
		g_udCRC = crc32_calc((void *)(dump_addr), dump_size);
		DiagPrintf("g_udCRC=0x%X.\r\n", g_udCRC);
	}
	DiagPrintf("[%s] dump done.\r\n", __func__);
	data_dumped = 1;
}

int isp_get_buf_idx(int stream_id, void *buf_addr)
{
	int ret = -1;
	for (int j = 0; j < buffer_count; j++) {
		void *curr_buf = (void *)pio->io_buffers[stream_id][j].out_buf.phy_addr;
		if (buf_addr == curr_buf) {
			ret = j;
		}
	}
	return ret;
}

void isp_buf_release(int stream_id, void *buf_addr, int buf_id)
{
	/* if buf_addr not NULL, use it to get buffer id */
	if (buf_addr) {
		buf_id = isp_get_buf_idx(stream_id, buf_addr);
		if (buf_id < 0) {
			printf("error: isp release fail with stream_id:%d, buf_id:%d\r\n", stream_id, buf_id);
			return;
		}
	}
	int offset = VIDEO_STREAM0_BUFFER_STATUS + stream_id * 0x4;
	if (stream_id < 4) {
		write_reg(pio->base, (0x0030 << (buf_id * 8)), offset);
	} else {
		write_reg(pio->base, (0x0018 << (buf_id * 8)), offset);
	}
}

static int isp_default_ch_handle(struct isp_io *pio, int stream_id, int buf_id)
{
	// check_buffer(pio, stream_id, buf_id);
	// erase_buffer(pio, stream_id, buf_id);
	return 0;
}

static int (*isp_ch_handle)(struct isp_io *pio, int stream_id, int buf_id) = isp_default_ch_handle;

void isp_ch_handle_register(int (*handle)(struct isp_io *pio, int stream_id, int buf_id))
{
	isp_ch_handle = handle;
}

static int isp_user_release = 0;

void isp_set_user_release(void)
{
	printf("isp set user release\r\n");
	isp_user_release = 1;
}

void ISP0_IRQHandler(void)
{
	struct stream_verify_mode *pmode = stream_verify_get_mode();
	if (pmode->mode == 1) {
		if (hist_dif < 0xFF) {
			DiagPrintf("%s [ --hist_dif=0x%X(PASS)-- ]  fn: %d  %d  %d  %d  %d\n\r", __func__, hist_dif, frame_count[0], frame_count[1], frame_count[2], frame_count[3],
					   frame_count[4]);
		} else {
			DiagPrintf("%s [ --hist_dif=0x%X(Reject)-- ]  fn: %d  %d  %d  %d  %d\n\r", __func__, hist_dif, frame_count[0], frame_count[1], frame_count[2], frame_count[3],
					   frame_count[4]);
		}
	} else {
#if ISP_LOG
		DiagPrintf("ISP0_IRQHandler fn: %d  %d  %d  %d  %d [timestamp:%lu]\n\r", frame_count[0], frame_count[1], frame_count[2], frame_count[3], frame_count[4],
				   xTaskGetTickCountFromISR());
#endif
	}

	void *base = pio->base;
	int flag     = read_reg(base, VIDEO_AXI_INT_FLAG);
	int flag_rgb = read_reg(base, 0xF0264);
	int value = 0;
	for (int i = 0; i < stream_count; i++) {
		int offset = VIDEO_STREAM0_BUFFER_STATUS + i * 0x4;
		int j = frame_count[i] % 2;
		int read_value = read_reg(base, offset);
		if (i < 4) {
			if ((read_value & (0x0020 << j * 8))) { //chech "slot" data ready.
				if (isp_ch_handle) {
					isp_ch_handle(pio, i, j);
				}
				if (i == 0) {
					dump_buffer(pio, i, j);
#if 0 //Test image buffer 
					int buf_addr = pio->io_buffers[i][j].out_buf.phy_addr;
					int buf_size = pio->io_buffers[i][j].out_buf.length;
					DCache_CleanInvalidate((uint32_t)buf_addr, buf_size);
					uint8_t *tmp = (uint8_t *)buf_addr;
					printf("--data: %d %d \r\n", tmp[0], tmp[1]);
#endif

					//DiagPrintf("remap 0x%08x 0x%08x 0x%08x isp status 0x%08x flag 0x%08x vhdr int 0x%08x err %d tnr 0x%08x \n\r", HAL_READ32(0x60100000, 0x0) , HAL_READ32(0x60100000, 0xB4),HAL_READ32(0x60100000, 0xB8), read_value, flag, read_reg(base, 0x102C), read_reg(base, 0x103C),  read_reg(base, 0x1402C));

				}
				if (!isp_user_release) {
					isp_buf_release(i, NULL, j);
				}
				frame_count[i]++;
			}
		} else {
			if ((read_value & (0x0010 << j * 8))) { //chech "slot" data ready.
				if (isp_ch_handle) {
					isp_ch_handle(pio, i, j);
				}
				if (!isp_user_release) {
					isp_buf_release(i, NULL, j);
				}
				frame_count[i]++;
			}
		}
	}
#if FIXP_STREAM
	if (grid_dire) {
		grid_size += 2;
	} else {
		grid_size -= 2;
	}
	if (grid_size > 30) {
		grid_size = 30;
		grid_dire = 0;
	}
	if (grid_size < 8) {
		grid_size = 8;
		grid_dire = 1;
	}
	write_reg(base, grid_size + (grid_size << 16), 0x6048);
#endif


	ispdbeg_printf("422 select: 0x%X:   NV12 select: 0x%X \r\n", read_reg(base, ZOOM_SYS_IMAGE_MODE_SEL), read_reg(base, VIDEO_ISP_NV12_SELECT));

	//vTaskDelay(10);
	if (read_reg(base, 0xF0254) & 0x00FF3333) {
		printf("Error: ISP YUV overflow int flag:0x%08X\r\n", read_reg(base, 0xF0254));
	}
	if (read_reg(base, 0xF0264) & 0x000000E7) {
		printf("Error: ISP RGB overflow int flag:0x%08X\r\n", read_reg(base, 0xF0264));
	}
	ispdbeg_printf("0xF0254: 0x%X \r\n", read_reg(base, 0xF0254));
	write_reg(base, 0xFFFFFFFF, 0xF0254);
	ispdbeg_printf("0xF0264: 0x%X \r\n", read_reg(base, 0xF0264));
	write_reg(base, 0xFFFFFFFF, 0xF0264);
	ispdbeg_printf("0x100000: 0x%X 0x203C: 0x%X 0x2058: 0x%X \r\n", read_reg(base, 0x100000), read_reg(base, 0x203C), read_reg(base, 0x2058));

	return;
}

static int (*isp_frame_start_handle)(int buf_id) = NULL;
static int (*isp_data_start_handle)(int buf_id) = NULL;

void isp_frame_start_handle_register(int (*handle)(int))
{
	isp_frame_start_handle = handle;
}
void isp_data_start_handle_register(int (*handle)(int))
{
	isp_data_start_handle = handle;
}

static isp_frame_start_cnt = 0;
static isp_data_start_cnt = 0;

void ISP1_IRQHandler(void)
{
	ispdbeg_printf("ISP1_IRQHandler\n\r");

	void *base = pio->base;
	uint32_t int_value = read_reg(base, 0x00000010) & read_reg(base, 0x00000014);

	struct stream_verify_mode *pmode = stream_verify_get_mode();
	if (int_value & BIT(0)) {  /* frame start int */
		int buf_id = isp_frame_start_cnt % 2;
		if (pmode->mode == 0) {
#if ISP_LOG
			DiagPrintf(">>>ISP frame start IRQ [buf_id:%d][timestamp:%lu]\n\r", buf_id, xTaskGetTickCountFromISR());
#endif
		}
		if (isp_frame_start_handle) {
			isp_frame_start_handle(buf_id);
		}
		isp_frame_start_cnt++;
		write_reg(base, BIT(0), 0x00000014);  /* clear flag */
		return;
	}

	if (int_value & BIT(8)) {  /* data start int */
		int buf_id = isp_data_start_cnt % 2;
		if (pmode->mode == 0) {
#if ISP_LOG
			DiagPrintf(">>>ISP data start IRQ [buf_id:%d][timestamp:%lu]\n\r", buf_id, xTaskGetTickCountFromISR());
#endif
		}
		if (isp_data_start_handle) {
			isp_data_start_handle(buf_id);
		}
		isp_data_start_cnt++;
		write_reg(base, BIT(8), 0x00000014);  /* clear flag */
		return;
	}

	return;
}

void ISP2_IRQHandler(void)
{
	DiagPrintf("ISP2_IRQHandler\n\r");
	return;
}

void ISP3_IRQHandler(void)
{
	DiagPrintf("ISP3_IRQHandler\n\r");
	return;
}

void ISP4_IRQHandler(void)
{
	DiagPrintf("ISP4_IRQHandler\n\r");
	return;
}

void ISP5_IRQHandler(void)
{
	DiagPrintf("ISP5_IRQHandler\n\r");
	return;
}

void ISP6_IRQHandler(void)
{
	DiagPrintf("ISP6_IRQHandler\n\r");
	return;
}

void ISP7_IRQHandler(void)
{
	DiagPrintf("ISP7_IRQHandler\n\r");
	return;
}

void ISP8_IRQHandler(void)
{
	DiagPrintf("ISP8_IRQHandler\n\r");
	return;
}

void config_irq(struct isp_io *io)
{
	void *base = io->base;
	pio = io;
	isp_printf("--------config irq \r\n");
	isp_printf("--------ISP_ALL_IRQ=%d \r\n", ISP_ALL_IRQ);
	isp_printf("--------ISP0_IRQ   =%d \r\n", ISP0_IRQ);
	isp_printf("--------ISP1_IRQ   =%d \r\n", ISP1_IRQ);
	isp_printf("--------ISP2_IRQ   =%d \r\n", ISP2_IRQ);
	isp_printf("--------ISP3_IRQ   =%d \r\n", ISP3_IRQ);
	isp_printf("--------ISP4_IRQ   =%d \r\n", ISP4_IRQ);
	isp_printf("--------ISP5_IRQ   =%d \r\n", ISP5_IRQ);
	isp_printf("--------ISP6_IRQ   =%d \r\n", ISP6_IRQ);
	isp_printf("--------ISP7_IRQ   =%d \r\n", ISP7_IRQ);
	isp_printf("--------ISP8_IRQ   =%d \r\n", ISP8_IRQ);

	write_reg(base, 0x8888, 0xF0250);
	write_reg(base, 0x0010, 0xF0260);

	/* enable frame/data start interrupt */
	// write_reg(base, BIT(0) | BIT(8), 0x00010);


	int IRQ_Priority = INT_PRI_HIGH;
	InterruptRegister((u32)ISP_ALL_IRQHandler, ISP_ALL_IRQ, (u32)NULL, IRQ_Priority);
	InterruptRegister((u32)ISP0_IRQHandler, ISP0_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP1_IRQHandler, ISP1_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP2_IRQHandler, ISP2_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP3_IRQHandler, ISP3_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP4_IRQHandler, ISP4_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP5_IRQHandler, ISP5_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP6_IRQHandler, ISP6_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP7_IRQHandler, ISP7_IRQ, (u32)NULL, IRQ_Priority);
	//InterruptRegister((u32)ISP8_IRQHandler, ISP8_IRQ, (u32)NULL, IRQ_Priority);

	//InterruptEn(ISP_ALL_IRQ, IRQ_Priority);
	InterruptEn(ISP0_IRQ, IRQ_Priority);
	//InterruptEn(ISP1_IRQ, IRQ_Priority);
	//InterruptEn(ISP2_IRQ, IRQ_Priority);
	//InterruptEn(ISP3_IRQ, IRQ_Priority);
	//InterruptEn(ISP4_IRQ, IRQ_Priority);
	//InterruptEn(ISP5_IRQ, IRQ_Priority);
	//InterruptEn(ISP6_IRQ, IRQ_Priority);
	//InterruptEn(ISP7_IRQ, IRQ_Priority);
	//InterruptEn(ISP8_IRQ, IRQ_Priority);
}

#if FIXP_STREAM
static int get_out_location(int type)
{
	int location;

	switch (type) {
	case ISP_OUTPUT_CROP_OUT_L:
		location = 2;
		break;
	case ISP_OUTPUT_CROP_OUT_S:
		location = 3;
		break;
	case ISP_OUTPUT_TPNR_OUT:
		location = 4;
		break;
	case ISP_OUTPUT_FUSION_IN_L:
		location = 6;
		break;
	case ISP_OUTPUT_FUSION_IN_S:
		location = 5;
		break;
	case ISP_OUTPUT_FUSION_IN_HDR:
		location = 7;
		break;
	case ISP_OUTPUT_FUSION_OUT:
		location = 8;
		break;
	case ISP_OUTPUT_NLSC_IN:
		location = 9;
		break;
	case ISP_OUTPUT_AWB_GAIN_OUT:
		location = 10;
		break;
	case ISP_OUTPUT_TM_IN:
		location = 11;
		break;
	case ISP_OUTPUT_GTM_OUT:
		location = 12;
		break;
	case ISP_OUTPUT_LTM_OUT:
		location = 13;
		break;
	case ISP_OUTPUT_INTP_IN:
		location = 14;
		break;
	case ISP_OUTPUT_RGB:
		location = 15;
		break;
	case ISP_OUTPUT_YUV:
		location = 16;
		break;
	case ISP_OUTPUT_YUYV:
		location = 20;
		break;
	default:
		location = RTS_RETURN(RTS_E_NOT_SUPPORT);
		break;
	}
	return location;
}
#endif

int isp_init_multi(struct isp_io *io)
{
	int ret;

	if (!io) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	memset(io, 0, sizeof(*io));

	/* map register */
	io->base = 0x60000000;//map_register(ISP_REG_BASE, ISP_REG_SIZE);
	if (!io->base) {
		ret = RTS_RETURN(RTS_E_MMAP_FAIL);
		goto err;
	}

	if (ret) {
		goto err;
	}

	io->initialized = RTS_TRUE;
	return RTS_OK;
err:
	isp_release_multi(io);
	return ret;
}

#if FIXP_STREAM
void isp_start(struct isp_io *io)
{
	void *base = io->base;

	/* set crop */
	for (int i = 0; i < 2; i++) {
		write_reg(base, io->width, CROP_WIDTH + i * 0x0C00);
		write_reg(base, io->height, CROP_HEIGHT + i * 0x0C00);
		write_reg(base, 1, CROP_TRIG + i * 0x0C00);
	}
	write_reg(base, io->width, MCROP_WIDTH);
	write_reg(base, io->height, MCROP_HEIGHT);
	write_reg(base, 1, MCROP_TRIG);

	/* start isp */
	write_reg(base, 0x222222, SYS_SPEED_CTRL);
	write_reg(base, 0x10, SYS_CONTROL0);
}
#endif

void isp_stop(struct isp_io *io)
{
	if (!io || !io->initialized) {
		return;
	}

	write_reg(io->base, 0, SYS_DATA_DELAY);
	vTaskDelay(100);
#if FIXP_STREAM
	write_reg(io->base, FIXP_STOP, FIXP_CTRL);
	vTaskDelay(100);
#endif
	for (int i = 0; i < stream_count; i++) {
		write_reg(io->base, 0, VIDEO_STREAM0_ENABLE + 4 * i);
	}
	write_reg(io->base, 0x02, SYS_CONTROL0);
}

ssize_t isp_buffer_length_multi(struct zoom_config *in_zoom, size_t width, size_t height,
								int type)
{
	ssize_t length;

	switch (type) {
	case ISP_INPUT_CROP_L_8BIT:
	case ISP_INPUT_CROP_S_8BIT:
		length = width * height;
		break;
	case ISP_INPUT_CROP_L:
	case ISP_INPUT_CROP_S:
	case ISP_INPUT_RAW_FINAL:
	case ISP_INPUT_YUYV:
	case ISP_INPUT_NV16:
	case ISP_OUTPUT_CROP_OUT_L:
	case ISP_OUTPUT_CROP_OUT_S:
	case ISP_OUTPUT_TPNR_OUT:
	case ISP_OUTPUT_FUSION_IN_L:
	case ISP_OUTPUT_FUSION_IN_S:
	case ISP_OUTPUT_GTM_OUT:
	case ISP_OUTPUT_LTM_OUT:
	case ISP_OUTPUT_INTP_IN:
	case ISP_OUTPUT_YUYV:
	case ISP_INPUT_TPNR_IN:
		length = width * height * 2;
		break;
	case ISP_INPUT_FUSION_IN:
	case ISP_INPUT_FUSION_OUT:
	case ISP_INPUT_RAW_BACKEND:
	case ISP_OUTPUT_FUSION_IN_HDR:
	case ISP_OUTPUT_FUSION_OUT:
	case ISP_OUTPUT_NLSC_IN:
	case ISP_OUTPUT_AWB_GAIN_OUT:
	case ISP_OUTPUT_TM_IN:
		length = width * height * 4;
		break;
	case ISP_INPUT_RGB:
	case ISP_INPUT_YUV:
	case ISP_OUTPUT_RGB:
	case ISP_OUTPUT_YUV:
		length = (width * height + 2) / 3 * 128 / 8;
		break;
	case ISP_OUTPUT_RGB_8BIT:
		length = (width * height * 3);
		break;
	default:
		length = RTS_RETURN(RTS_E_NOT_SUPPORT);
	}

	if (in_zoom && in_zoom->enable) {
		if (in_zoom->out_fmt == FMT_NV12) {
			if (in_zoom->align_width) {
				length = in_zoom->align_width * in_zoom->height * 3 / 2;

			} else {
				length = in_zoom->width * in_zoom->height * 3 / 2;
			}
		} else {
			length = in_zoom->width * in_zoom->height * 2;
		}
		if (type == ISP_OUTPUT_RGB_8BIT) {
			length = (in_zoom->width * in_zoom->height * 3);
		}
	}

	return length;
}

void initialize_isp_io(struct isp_io *io)
{
	for (int i = 0; i < stream_count; i++) {
		io->io_buffers[i] = io->io_buffer_ISP[i];
	}
}

extern uint8_t *g_statis;
static int isp_alloc_buffer(struct isp_io *io, size_t width, size_t height,
							int out_type, struct zoom_config zoom[])
{
	int ret;
	int i, j;
	char numstr[4];
	char namestr[32];
	ssize_t out_length;
	ssize_t y_length;
	ssize_t uv_length;
	ssize_t r_length;
	ssize_t g_length;
	ssize_t b_length;
	struct zoom_config *in_zoom;
	struct io_buffer *current_buffer;
	u32 align_w = 4096;  // for Partial sram burst 256B


	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	if (!width || !height || !out_type) {
		return RTS_RETURN(RTS_E_NOT_READY);
	}

	initialize_isp_io(io);

	DiagPrintf("cmpr_fr_mode %d cmpr_fr_stm_id %d \r\n", io->cmpr_fr_mode,  io->cmpr_fr_stm_id);

	io->width = width;
	io->height = height;
	io->out_type = out_type;

	if (g_statis == NULL) {
#if REALSENSOR
		g_statis = malloc(2 * 24 * 1024);//heap_used;
		//heap_used += (2 * 24 * 1024);
#endif
	}
	for (j = 0; j < stream_count; j++) {
		in_zoom = &zoom[j];
		out_length = 0;
		y_length = 0;
		uv_length = 0;
		if (in_zoom && in_zoom->enable) {
			if (j < 4) {
				out_length = isp_buffer_length_multi(in_zoom, width, height, out_type);
			} else {
				out_length = isp_buffer_length_multi(in_zoom, width, height, ISP_OUTPUT_RGB_8BIT);
			}

			if (out_type <= 0) {
				return RTS_RETURN(RTS_E_NOT_SUPPORT);
			}
		}

		if (in_zoom && in_zoom->enable && in_zoom->out_fmt < FMT_YUYV) {

			if (in_zoom->align_width) {
				y_length = in_zoom->align_width * in_zoom->height ;

			} else {
				y_length = in_zoom->width * in_zoom->height;
			}
			uv_length = out_length - y_length;
		} else if (in_zoom && in_zoom->enable && in_zoom->out_fmt == FMT_RGB) {
			r_length = g_length = b_length = in_zoom->width * in_zoom->height;
		} else {
			y_length = out_length;
			uv_length = 0;
		}

		DiagPrintf("out_length %d y %d uv %d \n\r", out_length, y_length, uv_length);
		if (io->low_latency_mode && j == 0) {

			u32 all_buffer_addr_y = malloc(out_length + align_w + align_w);
			u32 all_buffer_addr_uv;

			all_buffer_addr_y = (all_buffer_addr_y + (align_w - 1)) & ~((u32)(align_w - 1));

			all_buffer_addr_uv = all_buffer_addr_y + y_length;
			all_buffer_addr_uv = (all_buffer_addr_uv + (align_w - 1)) & ~((u32)(align_w - 1));


			for (i = 0; i < buffer_count; i++) {

				current_buffer = &io->io_buffers[j][i];

				if (current_buffer->out_buf.length < out_length) {

					current_buffer->out_buf.length = out_length;
					current_buffer->out_buf.phy_addr = all_buffer_addr_y;
					//heap_used += current_buffer->out_buf.length;

					if (in_zoom->out_fmt < FMT_YUYV) {
						current_buffer->out_y_addr = current_buffer->out_buf.phy_addr;
						current_buffer->out_y_size = y_length;
						current_buffer->out_uv_addr = all_buffer_addr_uv;
						current_buffer->out_uv_size = uv_length;
					} else {
					}
					printf("s%d buf %d : y addr is 0x%08x %d uv 0x%08x %d\n", j, i, current_buffer->out_y_addr, current_buffer->out_y_size, current_buffer->out_uv_addr,
						   current_buffer->out_uv_size);
				}
			}

		} else if (io->cmpr_fr_mode && j == io->cmpr_fr_stm_id) {

			u32 buffer_addr_y;
			u32 buffer_addr_uv;
			u32 tmp_buf_len = (y_length + align_w + uv_length + align_w) * buffer_count;
			u32 tmp_buf_addr = malloc(tmp_buf_len);

			for (i = 0; i < buffer_count; i++) {

				current_buffer = &io->io_buffers[j][i];

				if (current_buffer->out_buf.length < out_length) {
					buffer_addr_y = (tmp_buf_addr + (align_w - 1)) & ~((u32)(align_w - 1));
					current_buffer->out_buf.length = y_length;
					current_buffer->out_buf.phy_addr = buffer_addr_y;
					current_buffer->out_y_addr = current_buffer->out_buf.phy_addr;
					current_buffer->out_y_size = y_length;
					printf("s%d buf %d : y addr is 0x%08x %d uv 0x%08x %d\n", j, i, current_buffer->out_y_addr, current_buffer->out_y_size, current_buffer->out_uv_addr,
						   current_buffer->out_uv_size);
					tmp_buf_addr = buffer_addr_y + y_length;
				}
			}
			for (i = 0; i < buffer_count; i++) {
				current_buffer = &io->io_buffers[j][i];
				if (current_buffer->out_buf.length < out_length) {
					buffer_addr_uv = (tmp_buf_addr + (align_w - 1)) & ~((u32)(align_w - 1));
					current_buffer->out_buf.length += uv_length;
					current_buffer->out_uv_addr = buffer_addr_uv;
					current_buffer->out_uv_size = uv_length;
					printf("s%d buf %d : y addr is 0x%08x %d uv 0x%08x %d\n", j, i, current_buffer->out_y_addr, current_buffer->out_y_size, current_buffer->out_uv_addr,
						   current_buffer->out_uv_size);
					tmp_buf_addr = buffer_addr_uv + uv_length;
				}
			}

		} else {
			for (i = 0; i < buffer_count; i++) {
				current_buffer = &io->io_buffers[j][i];

				if (current_buffer->out_buf.length < out_length) {
					current_buffer->out_buf.length = out_length;

					current_buffer->out_buf.phy_addr = malloc(current_buffer->out_buf.length);//heap_used;
					//heap_used += current_buffer->out_buf.length;
					ispdbeg_printf("buffer->phy_addr is 0x%x out_length=0x%X\n", current_buffer->out_buf.phy_addr, out_length);


					if (in_zoom->out_fmt < FMT_YUYV) {
						current_buffer->out_y_addr = current_buffer->out_buf.phy_addr;
						current_buffer->out_y_size = y_length;
						current_buffer->out_uv_addr =
							current_buffer->out_y_addr + current_buffer->out_y_size;
						current_buffer->out_uv_size = uv_length;

					} else if (in_zoom->out_fmt == FMT_RGB) {
						current_buffer->out_rgb_size = in_zoom->width * in_zoom->height;
						current_buffer->out_r_addr = current_buffer->out_buf.phy_addr + 0 * current_buffer->out_rgb_size;
						current_buffer->out_g_addr = current_buffer->out_buf.phy_addr + 1 * current_buffer->out_rgb_size;
						current_buffer->out_b_addr = current_buffer->out_buf.phy_addr + 2 * current_buffer->out_rgb_size;
					} else {
					}
				}
			}
		}
	}
	return RTS_OK;
}

void isp_release_multi(struct isp_io *io)
{
	int i, j;
	struct io_buffer *current_buffer;

	if (!io || !io->initialized) {
		isperro_printf("ISP IO not initialized or null\n");
		return;
	}

	//for (j = 0; j < stream_count; j++) {
	//    for (i = 0; i < buffer_count; i++) {
	//        current_buffer = &io->io_buffers[j][i];
	//        if (current_buffer->out_buf.length > 0) {
	//            rtsc_free_dma(&current_buffer->out_buf);
	//        }
	//    }
	//}

	//hal_clk_set_isp(0);
	//unmap_register(io->base, ISP_REG_SIZE);

	memset(io, 0, sizeof(*io));
}


#define ZOOM_FILTER_COEF_NUM 20
static const uint8_t zoom_filter[6][ZOOM_FILTER_COEF_NUM] = {
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x20, 0x40, 0x60, 0xa0, 0xc0, 0xe0, 0xff
	},
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20,
		0x30, 0x40, 0x40, 0x50, 0x60, 0x70, 0x80, 0x80, 0x80, 0x80
	},
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x15, 0x20, 0x35, 0x40,
		0x4b, 0x55, 0x55, 0x55, 0x55, 0x55, 0x56, 0x56, 0x55, 0x56
	},
	{
		0x08, 0x10, 0x18, 0x20, 0x20, 0x28, 0x30, 0x38, 0x40, 0x40,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
	},
	{
		0x20, 0x26, 0x2d, 0x33, 0x33, 0x3a, 0x40, 0x46, 0x33, 0x33,
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x33, 0x34
	},
	{
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x34
	},
};

static const uint8_t *get_zoom_filter(uint32_t scale)
{
	int i;
	const uint32_t thd[] = { 64 * 16, 128 * 16, 192 * 16, 256 * 16, 320 * 16 };

	for (i = 0; i < ARRAY_SIZE(thd); i++) {
		if (scale <= thd[i]) {
			break;
		}
	}
	return zoom_filter[i];
}

static int rtscam_zoom_set_zoom_filter(struct isp_io *io, uint32_t scale,
									   int stream_index)
{
	unsigned long i;
	uint32_t offset;
	uint32_t zoom_coef;
	void *base = io->base;
	const uint8_t *filter = get_zoom_filter(scale);

	uint32_t ZOOM_COEF_START[5] = {0xD8C10, 0xD9010, 0xD9110, 0xD9210, 0xD9310};
	zoom_coef = ZOOM_COEF_START[stream_index];
	for (i = 0, offset = zoom_coef; i < ZOOM_FILTER_COEF_NUM;
		 i += 4, offset += 4) {
		uint32_t filter_value;

		filter_value = (filter[i] | filter[i + 1] << 8 | filter[i + 2] << 16 |
						filter[i + 3] << 24);
		write_reg(base, filter_value, offset);
	}

	return 0;
}

static int config_zoom(struct isp_io *io, struct zoom_config *in_zoom,
					   int stream_index, size_t width, size_t height)
{
	void *base = io->base;
	static int reg_count = 0;
	uint16_t src_w, src_h;
	uint16_t dest_w, dest_h;
	uint32_t scale_h, scale_v;
	uint32_t crop_start_x, crop_start_y;
	uint32_t offset = 0; /* only between stream0 and stream1*/
	if (stream_index) {
		offset = 0x300 + stream_index * 0x100;
	}

	crop_start_x = in_zoom->left;
	crop_start_y = in_zoom->top;
	src_w = width;
	src_h = height;
	if (src_w + crop_start_x > width || src_h + crop_start_y > height) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}
	dest_w = in_zoom->width;
	dest_h = in_zoom->height;

	scale_h = ((src_w << 10) / dest_w);
	scale_v = ((src_h << 10) / dest_h);
	if (scale_h > 0x1ffff || scale_v > 0x1ffff) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	// NV12 or NV21
	if (stream_index == 0) {
		write_reg(base, 1, VIDEO_ISP_NV12_SELECT);
	} else {
		uint32_t write_value = 1;
		uint32_t read_value = read_reg(base, VIDEO_ISP_NV12_SELECT);
		write_reg(base, read_value | (write_value << (1 * stream_index)), VIDEO_ISP_NV12_SELECT);
	}

	if (reg_count == 0) {
		write_reg(base, 1, ZOOM_SYS_CONTROL);
		write_reg(base, 0x101, ZOOM_SYS_SPEED_CTRL);
		write_reg(base, 0x4, ZOOM_SYS_LAST_BREAK);
		write_reg(base, 1, ZOOM_SYS_IMAGE_AI_CHAN_SEL);

		reg_count++;
	}

	// NV12 or NV21
	if (stream_index == 0) {
		write_reg(base, 1, VIDEO_ISP_NV12_SELECT);
	} else {
		uint32_t write_value = 1;
		uint32_t read_value = read_reg(base, VIDEO_ISP_NV12_SELECT);
		write_reg(base, read_value | (write_value << (1 * stream_index)), VIDEO_ISP_NV12_SELECT);
	}

	if (stream_index < 4) {
		if (in_zoom->out_fmt != FMT_NV12) {
			uint32_t write_value = 1;
			uint32_t read_value = read_reg(base, ZOOM_SYS_IMAGE_MODE_SEL);
			write_reg(base, read_value | (write_value << stream_index), ZOOM_SYS_IMAGE_MODE_SEL);
		}
	}

	write_reg(base, (crop_start_y << 16 | crop_start_x),
			  ZOOM_CH0_CROP_START + offset);
	write_reg(base, (dest_w | dest_h << 16),
			  ZOOM_CH0_OUTPUT_SIZE + offset);
	write_reg(base, scale_h, ZOOM_CH0_STEP_H + offset);
	write_reg(base, scale_v, ZOOM_CH0_STEP_V + offset);

	rtscam_zoom_set_zoom_filter(io, min(scale_h, scale_v), stream_index);
	write_reg(base, 1, ZOOM_CH0_REG_LOAD + offset);
	write_reg(base, 1, ZOOM_CH0_CTRL + offset);

	return RTS_OK;
}

static int config_ISP_stream(struct isp_io *io, struct zoom_config *in_zoom,
							 int stream_index)
{
	void *base;
	static int reg_count = 0;
#if FIXP_STREAM
	uint32_t location;
#endif
	uint32_t offset = 0x4;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	write_reg(base, 1, VIDEO_STREAM0_RST + offset * stream_index);
	vTaskDelay(1);
	write_reg(base, 0, VIDEO_STREAM0_RST + offset * stream_index);
	vTaskDelay(1);

	write_reg(base, 0x3030,
			  VIDEO_STREAM0_BUFFER_STATUS + offset * stream_index);
	write_reg(base, 1, VIDEO_STREAM0_ENABLE + offset * stream_index);

	if (stream_index < 4) {
		uint32_t write_value = buffer_count;
		uint32_t read_value = read_reg(base, 0xF0180);
		write_reg(base, read_value | (write_value << (4 * stream_index)), 0xF0180); /* s VIDEO_DDR_FRAME_BUFFER_COUNT */
		ispdbeg_printf("NV12 buffer count: 0x%X \r\n", read_reg(base, 0xF0180));

		write_value = 0;
		read_value = read_reg(base, 0xF0250);
		write_reg(base, read_value | (write_value << (4 * stream_index)), 0xF0250);
		write_value = 0;
		read_value = read_reg(base, 0xF0250);
		write_reg(base, read_value | (write_value << (2 * stream_index + 16)), 0xF0250);
		ispdbeg_printf("interrupt enable: 0x%X set: 0x%X \r\n", read_reg(base, 0xF0250), read_value | (write_value << (2 * stream_index + 16)));


		write_value = 0xf;
		read_value = read_reg(base, 0xF0254);
		write_reg(base, read_value | (write_value << (4 * stream_index)), 0xF0254);
		ispdbeg_printf("interrupt status1: 0x%X set: 0x%X \r\n", read_reg(base, 0xF0254), read_value | (write_value << (2 * stream_index + 16)));
		write_value = 0x3;
		read_value = read_reg(base, 0xF0254);
		write_reg(base, read_value | (write_value << (2 * stream_index + 16)), 0xF0254);
		ispdbeg_printf("interrupt status2: 0x%X set: 0x%X \r\n", read_reg(base, 0xF0254), read_value | (write_value << (2 * stream_index + 16)));
	} else {
		write_reg(base, buffer_count, 0xF0188);
		ispdbeg_printf("RGB buffer count: 0x%X \r\n", read_reg(base, 0xF0188));
		write_reg(base, 0, 0xF0260);
		write_reg(base, 0xff, 0xF0264); /* isp4 s */
	}

	if (reg_count == 0) {
		if (in_zoom && in_zoom->enable && in_zoom->out_fmt < FMT_YUYV) {
			write_reg(base, 0, VIDEO_YUV422_INTERLEAVED_SEL);
		} else {
			write_reg(base, 1, VIDEO_YUV422_INTERLEAVED_SEL);
		}

		//write_reg(base, 0, VIDEO_AXI_INT_ENABLE);
		//write_reg(base, 0xf00ff, VIDEO_AXI_INT_FLAG); /* isp0+isp1 s */

#if FIXP_STREAM
		/* config output location */
		if (!in_zoom || !in_zoom->enable) {
			location = get_out_location(io->out_type);
			if (location < 0) {
				return location;
			}
			write_reg(base, location | 0x100, SYS_IMAGE_SEL1);
		} else {
			write_reg(base, 0, SYS_IMAGE_SEL1);
		}
#endif
		reg_count++;
	}

	return RTS_OK;
}

static int config_frame(struct isp_io *io, int stream_index)
{
	void *base;
	uint32_t frame_offset = 0x4;
	uint32_t stream_offset = 0x20;
	struct io_buffer *current_buffer;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;
	current_buffer = &io->io_buffers[stream_index][0];

	if (stream_index < 4) {
		write_reg(base, current_buffer->out_y_size,  VIDEO_FRAME_LEN_Y0  + stream_index * 8);
		write_reg(base, current_buffer->out_uv_size, VIDEO_FRAME_LEN_UV0 + stream_index * 8);
		ispdbeg_printf("buffer size (s%d):  y=%d  uv=%d \r\n", stream_index, current_buffer->out_y_size, current_buffer->out_uv_size);


		/* config ISP0/ISP1 frame0 and frame1 */
		for (int j = 0; j < buffer_count; j++) {
			current_buffer = &io->io_buffers[stream_index][j];
			write_reg(base, current_buffer->out_y_addr,
					  VIDEO_Y0_FRAME_ADDR0 + stream_offset * stream_index + frame_offset * j);
			write_reg(base, current_buffer->out_uv_addr,
					  VIDEO_UV0_FRAME_ADDR0 + stream_offset * stream_index + frame_offset * j);
		}
	} else {
		write_reg(base, current_buffer->out_rgb_size, 0xF02C0);
		write_reg(base, current_buffer->out_rgb_size, 0xF02C4);
		write_reg(base, current_buffer->out_rgb_size, 0xF02C8);
		ispdbeg_printf("buffer size (s%d):  rgb=%d \r\n", stream_index, current_buffer->out_rgb_size);

		/* config ISP RGB frame0 and frame1 */
		for (int j = 0; j < buffer_count; j++) {
			current_buffer = &io->io_buffers[stream_index][j];
			write_reg(base, current_buffer->out_r_addr, 0xF0140 + frame_offset * j);
			write_reg(base, current_buffer->out_g_addr, 0xF0150 + frame_offset * j);
			write_reg(base, current_buffer->out_b_addr, 0xF0160 + frame_offset * j);
			//write_reg(base, current_buffer->out_r_addr, 0xF0140);
			//write_reg(base, current_buffer->out_g_addr, 0xF0144);
			//write_reg(base, current_buffer->out_b_addr, 0xF0148);
		}
	}

	return RTS_OK;
}

static void axi_estimate(int *zoom_w, int *zoom_h, int *add_size, unsigned int *axi_set, unsigned int axi_size)
{
	unsigned int buf_size[stream_count * 2] = {0};
	unsigned int buf_perc[stream_count * 2] = {0};
	unsigned int total_size = 0;
	static int estimate_done = 0;
	if (!estimate_done) {
		for (int i = 0; i < stream_count - 1; i++) {
			buf_perc[2 * i + 0] = (zoom_w[i] * zoom_h[i]) >> 8;
			total_size += buf_perc[2 * i + 0];
			buf_perc[2 * i + 1] = (zoom_w[i] * zoom_h[i]) >> 9;
			total_size += buf_perc[2 * i + 1];
		}

		unsigned int accu = 0;
		for (int i = 0; i < stream_count - 1; i++) {
			int a = 2 * i + 0;
			buf_size[a] = (buf_perc[a] * axi_size) / total_size;
			buf_size[a] += add_size[i];
			buf_size[a] >>= 4;
			buf_size[a] <<= 4;
			axi_set[a] = (buf_size[a] << 16) | accu;
			accu += buf_size[a];

			a = 2 * i + 1;
			buf_size[a] = (buf_perc[a] * axi_size) / total_size;
			buf_size[a] += add_size[i] / 2;
			buf_size[a] >>= 4;
			buf_size[a] <<= 4;
			axi_set[a] = (buf_size[a] << 16) | accu;
			accu += buf_size[a];
		}
	}
	estimate_done = 1;
}

static int config_video_AXI(struct isp_io *io, int stream_index, int is_rgb, struct zoom_config zoom[])
{
	void *base;
	uint32_t offset = 0x4;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	int zoom_w[5] = {zoom[0].width, zoom[1].width, zoom[2].width, zoom[3].width, zoom[4].width};
	int zoom_h[5] = {zoom[0].height, zoom[1].height, zoom[2].height, zoom[3].height, zoom[4].height};
#if FIXP_STREAM
	int add_size[5] = {0, 0, 0, 1024, 0};
#else
	int add_size[5] = {0, 512, 512, 512, 0};
#endif
	static unsigned int axi_set[stream_count * 2] = {0};
	axi_estimate(zoom_w, zoom_h, add_size, axi_set, 24 * 1024 - (add_size[0] + add_size[1] + add_size[2] + add_size[3]) * 3 / 2);
	for (int i = 0; i < 8; i++) {
		printf("[%s] 0x%08X\r\n", __func__, axi_set[i]);
	}
	/* config AXI buffer */
	if (stream_index == 0) {
		write_reg(base, axi_set[0],
				  VIDEO_Y0_AXI_BUF_CFG + offset * stream_index * 2); /* isp0 */
		write_reg(base, axi_set[1],
				  VIDEO_UV0_AXI_BUF_CFG + offset * stream_index * 2);
	} else if (stream_index == 1) {
		write_reg(base, axi_set[2],
				  VIDEO_Y0_AXI_BUF_CFG + offset * stream_index * 2); /* isp1 */
		write_reg(base, axi_set[3],
				  VIDEO_UV0_AXI_BUF_CFG + offset * stream_index * 2);
	} else if (stream_index == 2) {
		write_reg(base, axi_set[4],
				  VIDEO_Y0_AXI_BUF_CFG + offset * stream_index * 2); /* isp2 */
		write_reg(base, axi_set[5],
				  VIDEO_UV0_AXI_BUF_CFG + offset * stream_index * 2);
	} else if (stream_index == 3) {
		write_reg(base, axi_set[6],
				  VIDEO_Y0_AXI_BUF_CFG + offset * stream_index * 2); /* isp3 */
		write_reg(base, axi_set[7],
				  VIDEO_UV0_AXI_BUF_CFG + offset * stream_index * 2);
	}

	if (is_rgb) {
		// 2025.02.19 RS Update
		write_reg(base, 0x06000e00, VIDEO_RGB_R_AXI_BUF_CFG);
		write_reg(base, 0x06001400, VIDEO_RGB_G_AXI_BUF_CFG);
		write_reg(base, 0x06001a00, VIDEO_RGB_B_AXI_BUF_CFG);
//		write_reg(base, 0x15003500, VIDEO_RGB_R_AXI_BUF_CFG);
//		write_reg(base, 0x15005000, VIDEO_RGB_G_AXI_BUF_CFG);
//		write_reg(base, 0x15006500, VIDEO_RGB_B_AXI_BUF_CFG);
	}

	return RTS_OK;
}

#if FIXP_STREAM
void config_vhdr(struct isp_io *io)
{
	void *base;
	base = io->base;
	uint32_t v;
	uint32_t frame_len = 2560 * 1440;
	uint32_t length = (((2560 * (1800 / 8)) + (4095)) & ~(4095));
	uint32_t vhdr_addr = (uint32_t)malloc(length);

	write_reg(base, vhdr_addr, 0x0000100C);    //VHDR_STREAM_FRAME_ADDR
	write_reg(base, frame_len, 0x00001010);    //VHDR_STREAM_DDR_LEN
	write_reg(base, length, 0x00001034);            //VHDR_STREAM_RING_LEN
	//isp_set_reg_bit(5, 0x00001008);        //VHDR_STREAM_CTRL
	v = read_reg(base, 0x00001008) | (1 << 5);
	write_reg(base, v, 0x00001008);

	write_reg(base, 256, 0x00001018);          //VHDR_STREAM_START_PIXELNUM
	write_reg(base, 1, 0x00012400);            //FBC_VHDR_FBC_EN

	//write_reg(base, 0, 0x203C); //Fusion test
	//write_reg(base, 0, 0x2058);
	write_reg(base, 1, 0x2000);

	write_reg(base, 1, 0x00001000);
#ifdef __REMAP_FIXP_HDR__

	{

		write_reg(base, 0, 0x203C); //Fusion test
		write_reg(base, 0, 0x2058);

		struct addremap_hdr remap_hdr_config;
		if (hdr_remap_en) {
			memset(&remap_hdr_config, 0, sizeof(struct addremap_hdr));
			remap_hdr_config.burstSize = hdr_burst_id;
			remap_hdr_config.denominator = hdr_remap_ratio_base;
			remap_hdr_config.numerator = hdr_remap_ratio;
			remap_hdr_config.max_burst = hdr_remap_maxburst;
			set_hdr_remap(2560, 1440, length, &remap_hdr_config);
		}
	}
#endif

}
static int config_fixp(struct isp_io *io)
{
	void *base;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	if (io->remap_fps) {
		uint32_t hts = 0;
		uint32_t min_vts;
		uint32_t isp_clk;
		int size_w = io->width;
		int size_h = io->height;
		int fps = 10;
		uint32_t block_width, block_height, block_start_x, block_start_y;

#ifdef __PXP_RUN__
		isp_clk = 200e6; //200e6;
#else
		isp_clk = 800e6; // simulate 4x fps accuracy
#endif
		fps = io->remap_fps;
		switch (size_w) {
		case 640:
			hts = 800;
			break;
		case 1280:
			hts = 1400;
			break;
		case 1920:
			hts = 2100;
			break;
		case 2304:
			hts = 2400;
			break;
		case 2560:
			hts = 2800;
			break;
		case 2688:
			hts = 2900;
			break;
		case 3072:
			hts = 3200;
			break;
		default:
			hts = (size_w / 100 + 2) * 100;
			break;
		}

		//min_vts = size_h * 4;
		//hts = isp_clk / fps / min_vts;
		hts = size_w * io->fixp_hts_ratio_2base / 2;
		min_vts = isp_clk / fps / hts;

		if (min_vts < size_h) {
			hts = size_w * 1.2;
			min_vts = isp_clk / fps / hts;
		}



		write_reg(base, 2, SYS_INPUT_CTRL);
		write_reg_dbg(base, size_w, FIXP_FRAME_WIDTH);
		write_reg_dbg(base, size_h, FIXP_FRAME_HEIGHT);
		write_reg_dbg(base, hts - size_w, FIXP_DUMMY_PIX);
		write_reg_dbg(base, min_vts - size_h, FIXP_DUMMY_LINE);
		//write_reg_dbg(base, 0xFFFF, FIXP_DUMMY_LINE);

		printf("[%s] min_vts(%d)  isp_clk(%d)  fps(%d)  hts(%d) dummy P %d L %d \n\r", __FUNCTION__, min_vts, isp_clk, fps, hts,  hts - size_w, min_vts - size_h);

		write_reg(base, 0xde, BLOCK_WIDTH);
		write_reg(base, 0xa3, BLOCK_HEIGHT);
		write_reg(base, 0xde, BLOCK_START_X);
		write_reg(base, 0xa3, BLOCK_START_Y);
		write_reg(base, 0x9, FIXP_SPEED_CTRL);
		write_reg(base, 0x0, FIXP_FRAME_NUM);


#ifdef __REMAP_FIXP_HDR__

		config_vhdr(io);
		write_reg(base, 128, FIXP_VC_DELAY);
#else

		write_reg(base, 32, FIXP_VC_DELAY);
#endif

		printf("[Low latency] fixp start delay \r\n");

		return RTS_OK;
	}

	/* config fixp */
	write_reg(base, 2, SYS_INPUT_CTRL);
#if VHDR
	config_vhdr(io);
#endif
	write_reg(base, io->width, FIXP_FRAME_WIDTH);
	write_reg(base, io->height, FIXP_FRAME_HEIGHT);
	write_reg(base, 8e6 / (io->height + 200) - io->width, FIXP_DUMMY_PIX);
	write_reg(base, 200, FIXP_DUMMY_LINE);
	write_reg(base, 0xde, BLOCK_WIDTH);
	write_reg(base, 0xa3, BLOCK_HEIGHT);
	write_reg(base, 0xde, BLOCK_START_X);
	write_reg(base, 0xa3, BLOCK_START_Y);
	write_reg(base, 0x9, FIXP_SPEED_CTRL);
	write_reg(base, 0x0, FIXP_FRAME_NUM);
#if VHDR

#ifdef __REMAP_FIXP_HDR__

	config_vhdr(io);
	write_reg(base, 128, FIXP_VC_DELAY);
#else
	write_reg(base, 32, FIXP_VC_DELAY);
#endif
	write_reg(base, FIXP_START | (1 << 7), FIXP_CTRL);
#else
	write_reg(base, 32, FIXP_VC_DELAY);
	write_reg(base, FIXP_START, FIXP_CTRL);
#endif

	return RTS_OK;
}
#endif

#define write_reg_dbg write_reg
static void config_mask(struct isp_io *io)
{
	int i;
	void *base = io->base;
	//MASK_GLOBLE_EN (Private mask enable register, 0xB8A0_6000)
	write_reg_dbg(base, 0x1F, 0x6000);

	//MASK_PATTERN (MASK replace pattern, 0xB8A0_6004)
	write_reg_dbg(base, 0xBF6AA5, 0x6004);


	//MASK_RECTANGLE1_START (MASK rectangle1 start point, 0xB8A0_6010)
	//MASK_RECTANGLE1_END (MASK rectangle1 end point, 0xB8A0_6014)
	write_reg_dbg(base, 0x00080008, 0x6010);
	write_reg_dbg(base, 0x01080108, 0x6014);

	//MASK_RECTANGLE2_START (MASK rectangle2 start point, 0xB8A0_6018)
	//MASK_RECTANGLE2_END (MASK rectangle2 end point, 0xB8A0_601C)
	write_reg_dbg(base, 0x01080108, 0x6018);
	write_reg_dbg(base, 0x02080208, 0x601C);

	//MASK_RECTANGLE3_START (MASK rectangle3 start point, 0xB8A0_6020)
	//MASK_RECTANGLE3_END (MASK rectangle3 end point, 0xB8A0_6024)
	write_reg_dbg(base, 0x02080208, 0x6020);
	write_reg_dbg(base, 0x03080308, 0x6024);

	//MASK_RECTANGLE4_START (MASK rectangle4 start point, 0xB8A0_6028)
	//MASK_RECTANGLE4_END (MASK rectangle4 end point, 0xB8A0_602C)
	write_reg_dbg(base, 0x03080308, 0x6028);
	write_reg_dbg(base, 0x04080408, 0x602C);


	//MASK_GRID_START (MASK grid start point, 0xB8A0_6040)
	write_reg_dbg(base, 0x00080008, 0x6040);

	//MASK_GRID_WINUM (MASK grid macro window number, 0xB8A0_6044)
	write_reg_dbg(base, 0x00001E28, 0x6044);//(30:40)

	//MASK_GRID_WIN_SIZE (MASK grid macro window width, 0xB8A0_6048)
	write_reg_dbg(base, 0x00200020, 0x6048);//(30X32:40*32)

	//MASK_MEM_REG (MASK memory register, 0xB8A0_6050 ~ 0xB8A0_60CC)
	for (i = 0; i < 15; i++) {
		write_reg_dbg(base, 0xAAAAAAAA, 0x6050 + 16 * i);
		write_reg_dbg(base, 0xAAAAAAAA, 0x6054 + 16 * i);
		write_reg_dbg(base, 0x55555555, 0x6058 + 16 * i);
		write_reg_dbg(base, 0x55555555, 0x605C + 16 * i);
	}
}

static int config_reg(struct isp_io *io, struct zoom_config zoom[], size_t width,
					  size_t height)
{
	int ret;
	int i;
	int j;
	struct zoom_config *in_zoom;

	void *base = io->base;
	write_reg(base, 0, ZOOM_SYS_IMAGE_MODE_SEL);
	for (i = 0; i < stream_count; i++) {
		in_zoom = &zoom[i];
		if (in_zoom && in_zoom->enable) {
			ret = config_zoom(io, in_zoom, i, width, height);
			if (ret) {
				isperro_printf("zoom config fail, ret = %d", ret);
				return ret;
			}
		}
	}

	for (i = 0; i < stream_count; i++) {
		ret = config_frame(io, i);
		if (ret) {
			isperro_printf("config frame fail!\n");
			return ret;
		}
	}

	for (i = 0; i < stream_count; i++) {
		in_zoom = &zoom[i];
		ret = config_ISP_stream(io, in_zoom, i);
		if (ret) {
			isperro_printf("config ISP_stream fail!\n");
			return ret;
		}
		if (i < 4) {
			ret = config_video_AXI(io, i, 0, zoom);
		} else {
			ret = config_video_AXI(io, i, 1, zoom);
		}
		if (ret) {
			isperro_printf("config video_AXI fail!\n");
			return ret;
		}
	}

#if FIXP_STREAM
	ret = config_fixp(io);
	if (ret) {
		isperro_printf("config fixp fail!\n");
		return ret;
	}
#ifdef _MASK_
	config_mask(io);
#endif
#endif
	return RTS_OK;
}

static int config_tnr(struct isp_io *io)
{
	void *reg_base = io->base;
	int w = io->width;
	int h = io->height;

	// Check if framebuffer compression enable
	uint8_t fbc_en = read_reg(reg_base, 0x12000) & 0x1; // fbc_tpnr : FBC_EN (TNR_FBC_EN)
	uint32_t tnr_en = read_reg(reg_base, SYS_CONTROL1) & (0x1 << 15);
	uint32_t img_len, img_width_padding;
	ispinfo_printf("fbc_en = %d\n", fbc_en);
	ispinfo_printf("tnr_en = %d\n", tnr_en);

	if (!tnr_en) {
		ispinfo_printf("tnr is not enabled.\n");
		return 0;
	}

	ispinfo_printf("fbc_en = %d\n", fbc_en);
	if (fbc_en) {
		// Formula from rts3923 spec
		img_width_padding = (w & 0x0000000f) ? (w & 0x0000ff0) + 0x10 : w;
		img_len = img_width_padding * h;
	} else {
		img_len = w * h * 3 / 2;
	}

	strncpy(io->tnr_buf.name, "TNR_REF", sizeof(io->tnr_buf.name) - 1);
	io->tnr_buf.phy_addr = 0;
	io->tnr_buf.length = img_len;
	if (io->tnr_buf.length > 0) {
		io->tnr_buf.phy_addr = malloc(io->tnr_buf.length);//io->out_buf.phy_addr + io->out_buf.length; //malloc(io->tnr_buf.length);
		memset(io->tnr_buf.phy_addr, 55, img_len);
		unsigned char *uctnr_buf = (unsigned char *)io->tnr_buf.phy_addr;
		ispdbeg_printf("(tnr chrck) addr: 0x%X  chech point: %d, %d, %d \r\n", uctnr_buf, uctnr_buf[1024 * 100], uctnr_buf[1024 * 500], uctnr_buf[1024 * 900]);
		ispinfo_printf("tnr malloc info addr:0x%X  size:%d.\n", io->tnr_buf.phy_addr, io->tnr_buf.length);
		if (io->tnr_buf.phy_addr) {
#if BUF_RESET
			memset((void *)io->tnr_buf.phy_addr, 55, io->tnr_buf.length);
#else
			memset((void *)io->tnr_buf.phy_addr, 55, 1024);
			memset((void *)(io->tnr_buf.phy_addr + io->tnr_buf.length / 2 - 1024), 55, 1024);
			memset((void *)(io->tnr_buf.phy_addr + io->tnr_buf.length   - 1024), 55, 1024);
#endif
		}
	}
	io->tnr_buf.buf_io = RTSC_BUF_BIDIRECTIONAL;
	io->tnr_buf.direction = RTSC_DMA_BUFFER_FROM_HEAD;
	//io->mempool_size += img_len;


	// Config AXI
	write_reg(reg_base, io->tnr_buf.phy_addr, TNR_STREAM_FRAME_ADDR);
	write_reg(reg_base, img_len, TNR_STREAM_DDR_LEN);
	write_reg(reg_base, 0x1A, TNR_STREAM_CTRL);
	// Enable AXI
	write_reg(reg_base, 1, TNR_STREAM_EN);

	ispinfo_printf("Enable TNR buf: img_len = %d, TNR_STREAM_CTRL = %x\n", img_len,
				   read_reg(reg_base, TNR_STREAM_CTRL));
}

#if !FIXP_STREAM
int rts_isp_test_preview(uint32_t isp_id, int fps);
#endif
int isp_streaming(struct isp_io *io, size_t width, size_t height, int out_type,
				  struct zoom_config zoom[], void *priv)
{
	int ret;

	if (!io || !io->initialized || !width || !height) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	rtk_log_level_set("*", RTK_LOG_INFO);
	RTK_LOGS(NOTAG, RTK_LOG_NONE,  "RTK_LOG_NONE\r\n");
	RTK_LOGA(NOTAG,   "RTK_LOG_ALWAYS\r\n");
	RTK_LOGE(NOTAG,  "RTK_LOG_ERROR\r\n");
	RTK_LOGW(NOTAG,   "RTK_LOG_WARN\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO,  "RTK_LOG_INFO\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "RTK_LOG_DEBUG\r\n");

#ifdef _TNR_
	write_reg(io->base, 0x0, 0x12000);//TNR_FBC_EN
	write_reg_mask(io->base, 0x1 << 15, SYS_CONTROL1, 0x1 << 15);

	uint8_t fbc_en = read_reg(io->base, 0x12000) & 0x1;//TNR_FBC_EN
	uint32_t tnr_en = read_reg(io->base, SYS_CONTROL1) & (0x1 << 15);
	ispinfo_printf("fbc_en = %d\n", fbc_en);
	ispinfo_printf("tnr_en = %d\n", tnr_en);
#endif
	//return;
	//ret = hal_reset_isp();
	//if (ret) {
	//    isperro_printf("reset fail, ret = %d", ret);
	//    return ret;
	//}

#if CONSUMER_BYPASS
	config_irq(io);
#endif
	DiagPrintf("config_irq \n\r");

	grid_size = 30;
	grid_dire = 0;
	memset(frame_count, 0, sizeof(frame_count));

	heap_used = 0x80000000;
	ret = isp_alloc_buffer(io, width, height, out_type, zoom);
#ifdef _TNR_
	ret = config_tnr(io);
#endif
	for (int i = 0; i < stream_count; i++) {
		if (zoom[i].enable) {
			for (int j = 0; j < buffer_count; j++) {
				erase_buffer(io, i, j);
				check_buffer(io, i, j);
			}
		}
	}

	if (ret) {
		isperro_printf("config image fail, ret = %d", ret);
		return ret;
	}

	ret = config_reg(io, zoom, width, height);
	if (ret) {
		isperro_printf("config reg fail, ret = %d", ret);
		return ret;
	}

#if FIXP_STREAM
	isp_start(io);
#else
#if VHDR

#ifndef __REMAP_REAL_HDR_SNR__
	ret = rts_isp_test_preview(0, 4);
#else
	DiagPrintf("delay rts_isp_test_preview \n");
#endif

#else
	ret = rts_isp_test_preview(0, 30);
#endif
	if (ret) {
		return ret;
	}
#endif

	return RTS_OK;
}
