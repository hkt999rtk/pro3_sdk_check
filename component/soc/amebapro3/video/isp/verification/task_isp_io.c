#include <diag.h>
#include <log.h>

#include <FreeRTOS.h>
#include <task.h>

#include <stdio.h>
#include <stdlib.h>
#include "isp_reg.h"
#include "ameba_soc.h"

#include "isp_regs.h"
#include "rts_errno.h"
#include "isp_mem.h"
#include "isp_io.h"
#include "reg_api.h"
#include "config_regs.h"
#include "crc32.h"

static int is_runing = 2;
static uint32_t CRC_MULTI_STREAM[5] = {0};
static uint32_t CRC_SINGLE_STREAM = 0;
static int input_read_already = 0;
static struct isp_io g_io;
static int slot_idx = 0;
static int frame_cnt = 0;
extern void *isp_io_base;

#ifndef barrier
#define barrier() __asm__ __volatile__("" : : : "memory")
#endif

void set_running_mode(int mode)
{
	is_runing = mode;
}

int get_running_mode(void)
{
	return is_runing;
}

void set_low_latency_mode(int mode)
{
	g_io.low_latency_mode = mode;
}

int get_low_latency_mode(void)
{
	return g_io.low_latency_mode;
}

int get_in_location_fmt(int type, uint32_t *location, uint32_t *format)
{
	if (!location || !format) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}
	switch (type) {
	case ISP_INPUT_CROP_L:
		*location = 0x1;
		*format = 0x1;
		break;
	case ISP_INPUT_CROP_S:
		*location = 0x400;
		*format = 0x1;
		break;
	case ISP_INPUT_TPNR_IN:
		*location = 0x800;
		*format = 0x1;
		break;
	case ISP_INPUT_FUSION_IN:
		*location = 0x2;
		*format = 0x2;
		break;
	case ISP_INPUT_FUSION_OUT:
		*location = 0x4;
		*format = 0x2;
		break;
	case ISP_INPUT_RAW_BACKEND:
		*location = 0x8;
		*format = 0x2;
		break;
	case ISP_INPUT_RAW_FINAL:
		*location = 0x10;
		*format = 0x1;
		break;
	case ISP_INPUT_RGB:
		*location = 0x20;
		*format = 0x3;
		break;
	case ISP_INPUT_YUV:
		*location = 0x40;
		*format = 0x3;
		break;
	case ISP_INPUT_YUYV:
		*location = 0x80;
		*format = 0x4;
		break;
	case ISP_INPUT_NV16:
		*location = 0x80;
		*format = 0x0;
		break;
	default:
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}
	return RTS_OK;
}

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

int isp_init(struct isp_io *io)
{
	void *base = io->base;
	isp_io_base = (void *)0x60000000;
	memset(io, 0, sizeof(struct isp_io));
	io->initialized = RTS_TRUE;

}

void isp_release(struct isp_io *io)
{
	if (!io || !io->initialized) {
		return;
	}

	memset(io, 0, sizeof(*io));
}

uint32_t isp_buffer_length(size_t width, size_t height, int type)
{
	uint32_t length;

	switch (type) {
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
	default:
		length = RTS_RETURN(RTS_E_NOT_SUPPORT);
	}
	return length;
}

int isp_alloc_buffer(struct isp_io *io, size_t width, size_t height,
					 int in_type, int out_type)
{
	int ret;
	uint32_t in_length;
	uint32_t out_length;
	isp_io_base = (void *)0x60000000;
	void *base = io->base;
	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	in_length = isp_buffer_length(width, height, in_type);
	out_length = isp_buffer_length(io->crop_w, io->crop_h, out_type);
	if (in_length <= 0 || out_type <= 0) {
		return RTS_RETURN(RTS_E_NOT_SUPPORT);
	}

	io->width = width;
	io->height = height;
	io->in_type = in_type;
	io->out_type = out_type;

	printf("in buff len %d %d \n\r", io->in_buf.length, in_length);
	if (io->in_buf.length < in_length) {
		if (io->in_buf.length) {
			//rtsc_mem_free(-1, &io->in_buf);
			io->in_buf.vm_addr = NULL;
			io->in_buf.phy_addr = 0;
			io->in_buf.length = 0;
		}
		strncpy(io->in_buf.name, "in buffer", sizeof(io->in_buf.name) - 1);

		io->in_buf.phy_addr = 0;
		io->in_buf.length = in_length;
		if (io->in_buf.length > 0) {
			io->in_buf.phy_addr = io->mempool_start;//malloc(io->in_buf.length);
		}
		io->in_buf.buf_io = RTSC_BUF_TO_DEVICE;
		io->in_buf.direction = RTSC_DMA_BUFFER_FROM_HEAD;

		if (io->in_type == ISP_INPUT_NV16) {
			io->in_y_addr = io->in_buf.phy_addr;
			io->in_y_size = in_length / 2;
			io->in_uv_addr = io->in_y_addr + io->in_y_size;
			io->in_uv_size = io->in_y_size;
		} else {
			io->in_y_addr = io->in_buf.phy_addr;
			io->in_y_size = in_length;
			io->in_uv_addr = 0;
			io->in_uv_size = 0;
		}
		io->mempool_size = in_length;
	}

	printf("out buff len %d %d \n\r", io->out_buf.length, out_length);
	if (io->out_buf.length < out_length) {
		int out_w = io->crop_w;
		int out_h = io->crop_h;
		if (io->out_buf.length) {
			//rtsc_mem_free(-1, &io->out_buf);
			io->out_buf.vm_addr = NULL;
			io->out_buf.phy_addr = 0;
			io->out_buf.length = 0;
		}
		strncpy(io->out_buf.name, "out buffer", sizeof(io->out_buf.name) - 1);

		io->out_buf.phy_addr = 0;
		if (io->zoom_en == 2) {
			io->out_buf.length = estimate_multizoom_size(io);
		} else {
			io->out_buf.length = out_length;
		}
		if (io->out_buf.length > 0) {
			io->out_buf.phy_addr = io->in_buf.phy_addr + io->in_buf.length;//malloc(io->out_buf.length);
			estimate_multizoom_buffaddr(io);
			if (io->out_buf.phy_addr) {
				if (io->zoom_en != 2) {
#if BUF_RESET
					memset((void *)io->out_buf.phy_addr, 55, io->out_buf.length);
#else
					memset((void *)io->out_buf.phy_addr, 55, 1024);
					memset((void *)(io->out_buf.phy_addr + io->out_buf.length / 2 - 1024), 55, 1024);
					memset((void *)(io->out_buf.phy_addr + io->out_buf.length   - 1024), 55, 1024);
#endif
				}
			}
		}
		io->out_buf.buf_io = RTSC_BUF_FROM_DEVICE;
		io->out_buf.direction = RTSC_DMA_BUFFER_FROM_HEAD;

		if (io->zoom_en) {
			out_w = io->zoom_w[0];
			out_h = io->zoom_h[0];
		} else {
			out_w = io->crop_w;
			out_h = io->crop_h;
		}
		if (io->aligned_w[0]) {
			out_w = io->aligned_w[0];
		}

		if (io->zoomout_nv16 == 2) { //YUYV
			io->out_y_addr[0][0] = io->out_buf.phy_addr;
			io->out_y_size[0] = out_length;//out_w * out_h * 4 / 2;
			io->out_uv_addr[0][0] = 0;
			io->out_uv_size[0] = 0;
		} else if (io->zoomout_nv16 == 1) { //NV16
			io->out_y_addr[0][0] = io->out_buf.phy_addr;
			io->out_y_size[0] = (out_w * out_h);
			io->out_uv_addr[0][0] = io->out_y_addr[0][0] + io->out_y_size[0];
			io->out_uv_size[0] = io->out_y_size[0];
		} else if (io->zoomout_nv16 == 0) { // NV12
			io->out_y_addr[0][0] = io->out_buf.phy_addr;
			io->out_y_size[0] = (out_w * out_h);
			io->out_uv_addr[0][0] = io->out_y_addr[0][0] + io->out_y_size[0];
			io->out_uv_size[0] = io->out_y_size[0] / 2;
		}
		printf("-------0xF02A0=%d out_length=%d out_w=%d out_h=%d\r\n", io->out_y_size[0], out_length, out_w, out_h);
		io->mempool_size += out_length;
	}

	void *reg_base = io->base;
	int w = io->width;
	int h = io->height;

	// Check if framebuffer compression enable
	uint8_t fbc_en = read_reg(reg_base, TNR_FBC_EN) & 0x1;
	uint32_t tnr_en = read_reg(reg_base, SYS_CONTROL1) & (0x1 << 15);
	uint32_t img_len, img_width_padding;

	if (!tnr_en) {
		printf("tnr is not enabled.\n");
		return 0;
	}

	printf("fbc_en = %d\n", fbc_en);
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
		io->tnr_buf.phy_addr = io->out_buf.phy_addr + io->out_buf.length; //malloc(io->tnr_buf.length);
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
	io->mempool_size += img_len;


	// Config AXI
	write_reg(reg_base, io->tnr_buf.phy_addr, TNR_STREAM_FRAME_ADDR);
	write_reg(reg_base, img_len, TNR_STREAM_DDR_LEN);
	write_reg(reg_base, 0x1A, TNR_STREAM_CTRL);
	// Enable AXI
	write_reg(reg_base, 1, TNR_STREAM_EN);

	printf("Enable TNR buf: img_len = %d, TNR_STREAM_CTRL = %x\n", img_len,
		   read_reg(reg_base, TNR_STREAM_CTRL));

	return RTS_OK;
}

void isp_restart(struct isp_io *io)
{
	void *base = io->base;

	/* set crop */
	for (int i = 0; i < 2; i++) {
		if (io->zoom_en) {
			write_reg(base, 0, 0x0C00 + i * 0x0C00);
			write_reg(base, 0, 0x0C04 + i * 0x0C00);
		} else {
			write_reg(base, (io->width - io->crop_w) / 2, 0x0C00 + i * 0x0C00);
			write_reg(base, (io->height - io->crop_h) / 2, 0x0C04 + i * 0x0C00);
		}
		write_reg(base, io->crop_w, CROP_WIDTH + i * 0x0C00);
		write_reg(base, io->crop_h, CROP_HEIGHT + i * 0x0C00);
		write_reg(base, 1, CROP_TRIG + i * 0x0C00);
	}
	write_reg(base, io->crop_w, MCROP_WIDTH);
	write_reg(base, io->crop_h, MCROP_HEIGHT);
	write_reg(base, 1, MCROP_TRIG);

	/* start isp */
	write_reg(base, 0x222222, SYS_SPEED_CTRL); //Fixed blank data: 0x212121 -> 0x222222
	write_reg(base, 0x10, SYS_CONTROL0);
}

int buffer_sum(struct rtsc_dma_buffer *buffer, struct isp_io *io)
{
	int i = 0;
	int cnt = 0;
#ifdef BUFFER_SUM
	unsigned short *wtmp = (void *)buffer->phy_addr;
	printf("buffer_sum(address): 0x%X \n\r", buffer->phy_addr);
	for (i = 0; i < io->width * io->height; i++) {
		cnt += wtmp[i];
	}
#endif
	return cnt;
}


int isp_verify_run(struct isp_io *io)
{
	int ret;
	void *base;
	uint32_t location;
	uint32_t format, i;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}
	if (!io->in_buf.length || !io->out_buf.length) {
		return RTS_RETURN(RTS_E_NOT_READY);
	}

	base = io->base;


	/* config output location */
	location = get_out_location(io->out_type);
	if (location < 0) {
		return location;
	}
	if (!io->zoom_en) {
		if (io->dbg_path > 20) {
			write_reg_dbg(base, location | 0x100, SYS_IMAGE_SEL1);
		} else {
			printf("[%s] Use io->dbg_path: %d \r\n", __FUNCTION__, io->dbg_path);
		}
	} else {
		write_reg_dbg(base, 0, SYS_IMAGE_SEL1);
		printf("[ZOOM] Zoom selected. \n\r");
	}

	/* config verfiy */
	ret = get_in_location_fmt(io->in_type, &location, &format);
	printf("------ io->in_type=%d, location=%d, format=%d \t\n", io->in_type, location, format);
	printf("------ io->in_type=%d, location=%d, format=%d \t\n", io->in_type, location, format);
	printf("------ io->in_type=%d, location=%d, format=%d \t\n", io->in_type, location, format);
	if (ret) {
		RTS_ERR("parse input format fail\n");
		return ret;
	}
	printf("[0x0004] ------> 0x%lX \n\r", read_reg(base, 0x0004));

#if SHOW_LOG
	if (io->zoom_en) {
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
		printf("[ZOOM_MEM]0xD8C5C: 0x%lX \n\r", read_reg(base, 0xD8C5C));
	}
#endif

	return RTS_OK;
}

int isp_wait_completion(struct isp_io *io)
{
	void *base;
	int count = 1;
	uint32_t flag = 0;
	uint32_t status = 0;

	if (!io || !io->initialized) {
		return RTS_RETURN(RTS_E_NULL_POINT);
	}

	base = io->base;

	int start_tick = xTaskGetTickCount();
	int max_cnt = io->wait_fd_round;
	count = 0;
	isp_printf("dummy: 0x%X , SYS_SPEED_CTRL: 0x%X\n", read_reg(base, VERIFY_FRAME_CONFG1), read_reg(base, SYS_SPEED_CTRL));
	while (count++ <= max_cnt) {
		uint32_t framedone;
		flag = read_reg(base, VIDEO_AXI_INT_FLAG);
		if (io->zoom_en == 2) {
			framedone = (flag & 0x8) && (flag & 0x80) && (flag & 0x800) && (flag & 0x8000);
		} else {
			framedone = flag & 0x8;
		}
		if (framedone) {
			status = read_reg(base, VIDEO_STREAM0_BUFFER_STATUS);
			write_reg(base, 0xf, VIDEO_AXI_INT_FLAG);
			if (flag & 0x7 || status & 0x4) {
				ispwarn_printf("framebuffer errror occurred, intr-flag: 0x%X status: 0x%X, time-used: %d(ms)\n",
							   flag, status, xTaskGetTickCount() - start_tick);
				return RTS_RETURN(RTS_E_BUF_OVERFLOW);
			}
			isp_printf("frame done, intr-flag: 0x%X status: 0x%X, time-used: %d(ms)\n", flag, status, xTaskGetTickCount() - start_tick);
			return RTS_OK;
		} else if (flag & 0x30000) {
			isperro_printf("framebuffer length overflow, intr-flag: 0x%X, time-used: %d(ms)\n", flag, xTaskGetTickCount() - start_tick);
			return RTS_RETURN(RTS_E_BUF_OVERFLOW);
		}
		vTaskDelay(io->wait_fd_delay);
	}
	isperro_printf("wait framebuffer timedout, intr-flag: 0x%X, time-used: %d(ms)\n", flag, xTaskGetTickCount() - start_tick);
	return RTS_RETURN(RTS_E_TIMEOUT);
}

#if POLL_MEM
static void polling_mem(void *pvParameters)
{
	struct isp_io *io = (struct isp_io *)pvParameters;
	void *base = io->base;
	int count = 50;
	int i = 0;
	while (i < count) {
		isp_printf("[%s] 0xF0080: 0x%lX \n\r", __FUNCTION__, read_reg(base, 0xF0080)); // ISP_STREAM0_DDR_FRAME_BUFFER_STATUS  //-> multizoom_dbg
		isp_printf("[%s] 0xF0084: 0x%lX \n\r", __FUNCTION__, read_reg(base, 0xF0084)); // ISP_STREAM1_DDR_FRAME_BUFFER_STATUS
		isp_printf("[%s] 0xF0088: 0x%lX \n\r", __FUNCTION__, read_reg(base, 0xF0088)); // ISP_STREAM2_DDR_FRAME_BUFFER_STATUS
		isp_printf("[%s] 0xF008C: 0x%lX \n\r", __FUNCTION__, read_reg(base, 0xF008C)); // ISP_STREAM3_DDR_FRAME_BUFFER_STATUS
		isp_printf("[%s] 0xF0090: 0x%lX \n\r", __FUNCTION__, read_reg(base, 0xF0090)); // ISP_RGB_STREAM_DDR_FRAME_BUFFER_STATUS
		isp_printf("\r\n[TICK] %d\r\n", xTaskGetTickCount());
		//if(read_reg(base, 0xF0080))
		//	write_reg(base, 0, 0xF0080);
		vTaskDelay(10);
		i++;
	}
	vTaskDelete(NULL);
	return;
}
#endif

int main_base_test(void)
{
	void *base;

	int y_value  = DDR_START;
	char *y_addr = (char *)y_value;
	isp_io_base = (void *)0x60000000;
	base = g_io.base;
	memset(y_addr, 0, 1024);
	memset(y_addr, 2, 1024);


	DiagPrintf("isp_write_reg \n\r");
	write_reg(base, 0x00000000, VIDEO_Y0_FRAME_ADDR0);
	write_reg(base, DDR_START, VIDEO_Y0_FRAME_ADDR0);

	DiagPrintf("print info \n\r");
	DiagPrintf("VIDEO_Y0_FRAME_ADDR0: 0x%.8X \n\r", isp_read_reg(VIDEO_Y0_FRAME_ADDR0));
	DiagPrintf("phy_addr: %d %d %d \n\r", y_addr[0], y_addr[1], y_addr[2]);
}


static uint32_t savefile(struct isp_io *io, int start_addr, int len, int seg_size, char *fn_out)
{
	int i;
	FILE *fp = NULL;;
	int seg_n = (len + seg_size - 1) / seg_size;
	char fn[128] = {0};
	uint32_t udCRC = 0;
	unsigned char decimation[64];

	ispinfo_printf("[%s] start_addr:0x%X, len:%d, seg_size:%d \n\r", __FUNCTION__, start_addr, len, seg_size);
	ispinfo_printf("[%s] CRC32 Estimating..\r\n", __FUNCTION__);

#if CRC_DO
	udCRC = crc32_calc((void *)(start_addr), len);
#else
	unsigned char *src = (unsigned char *)start_addr;
	int dec_seg = len / (64 + 1);
	for (int i = 0; i < 64; i++) {
		decimation[i] = *src;
		src += dec_seg;
	}

	udCRC = crc32_calc((void *)(decimation), 64);
#endif

	ispinfo_printf("[%s] CRC32 result: 0x%lX\r\n", __FUNCTION__, udCRC);
	if (io->save_output) {
		for (i = 0; i < seg_n; i++) {
			sprintf(fn, "%s_dbgpath_%d_frmcnt_%d_%lX_%d.raw", fn_out, io->dbg_path, frame_cnt, udCRC, i);
			DiagPrintf("[%s] write seg..%d/%d (%s)..", __FUNCTION__, i + 1, seg_n, fn);
			fp = fopen(fn, "w");
			if (!fp) {
				return;
			}
			fwrite((void *)(start_addr + seg_size * i), seg_size, 1, fp);
			fclose(fp);
			DiagPrintf("Done\n\r");
		}
	}
	return udCRC;
}

void data_in(struct isp_io *io)
{
	int IMG_W = io->width;
	int IMG_H = io->height;
	int flen, rlen;
	char buf[128] = {0};
	char fn_in[128] = {0};
	char fn_out[128] = {0};
	FILE *fp;
	int i, j, seg_n;
	int seg_size = 0x100000 / 2; // limit:0x100000
	if (!io->fixp_en) {
		if (io->pattern_input == 0) {
			if (input_read_already == 0) {
				int file_size = IMG_W * IMG_H * 2;
				DiagPrintf("fopen read start: 0x%X  W,H=%d,%d\n\r", io->in_buf.phy_addr, IMG_W, IMG_H);//
				if (io->vhdr_en) {
					sprintf(fn_in, "D:\\oscaryang\\00_Test\\in_VHDR_%dx%d_12bit.raw", IMG_W, IMG_H);
					file_size = IMG_W * IMG_H * 4;
				} else {
					sprintf(fn_in, "D:\\oscaryang\\00_Test\\in_crop(%dx%d).raw", IMG_W, IMG_H);
					file_size = IMG_W * IMG_H * 2;
				}
				fp = fopen(fn_in, "rb");
				DiagPrintf("fopen end\n\r");
				if (!fp) {
					return;
				}

				fseek(fp, 0, SEEK_END);
				flen = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				DiagPrintf("file len %d\n", flen);

				seg_n = (file_size + seg_size - 1) / seg_size;
				for (i = 0; i < seg_n; i++) {
					DiagPrintf("fopen read seg..%d/%d\n\r", i + 1, seg_n);
					if (seg_size * i + seg_size > file_size) {
						rlen = fread((void *)(io->in_buf.phy_addr + seg_size * i), file_size - seg_size * i, 1, fp);
					} else {
						rlen = fread((void *)(io->in_buf.phy_addr + seg_size * i), seg_size, 1, fp);
					}
				}
				input_read_already = 1;

				fclose(fp);
			}
		} else {
			unsigned short *uwtemp = (unsigned short *)(io->in_buf.phy_addr);
			DiagPrintf("Input start: 0x%X  W,H=%d,%d\n\r", io->in_buf.phy_addr, IMG_W, IMG_H);
			for (i = 0; i < IMG_W * IMG_H; i++) {
				uwtemp[i] = 256;
			}
			for (i = IMG_H / 4; i < IMG_H * 3 / 4; i += 2) {
				for (j = IMG_W / 4; j < IMG_W * 3 / 4; j += 2) {
					uwtemp[i * IMG_W + j + 1] = 4095;
				}
			}
			for (i = IMG_H / 16; i < IMG_H * 3 / 16; i += 2) {
				for (j = IMG_W / 16; j < IMG_W * 3 / 16; j += 2) {
					uwtemp[(i + 1 + IMG_H * 0 / 4)*IMG_W + (j + IMG_W * 0 / 4)] = 4095;
					uwtemp[(i + 1 + IMG_H * 0 / 4)*IMG_W + (j + IMG_W * 3 / 4)] = 4095;
				}
			}
			for (i = IMG_H / 16; i < IMG_H * 3 / 16; i += 2) {
				for (j = IMG_W / 16; j < IMG_W * 3 / 16; j += 2) {
					uwtemp[(i + 1 + IMG_H * 3 / 4)*IMG_W + (j + IMG_W * 0 / 4)] = 4095;
					uwtemp[(i + 1 + IMG_H * 3 / 4)*IMG_W + (j + IMG_W * 3 / 4)] = 4095;
				}
			}
		}
	}
}

void data_out(struct isp_io *io)
{
	char fn_out[128] = {0};
	FILE *fp;
	int i, j, seg_n;
	int seg_size = 0x100000 / 2; // limit:0x100000
	void *base = io->base;

	//SYS_LOG();
	if (io->zoom_en) {
		//ZOOM_DBG_LOG();
	}

	//MAX_BURST_LENGTH_SELECT (Max Burst Length Select, 0xB8AF_0190)
	if (io->zoom_en < 2) {
		int out_w = 0;
		int out_h = 0;
		if (io->zoom_en == 1) {
			out_w = io->zoom_w[0];
			out_h = io->zoom_h[0];
		} else {
			out_w = io->crop_w;
			out_h = io->crop_h;
		}
		memset(fn_out, 0, sizeof(fn_out));
		sprintf(fn_out, "D:\\oscaryang\\00_Test\\test_output(%dx%d_zoom%d_ch0)", out_w, out_h, io->zoom_en);
		CRC_SINGLE_STREAM = savefile(io, read_reg(base, 0xF00C0), read_reg(base, 0xF02A0) + read_reg(base, 0xF02A4), seg_size, fn_out);
#if SHOW_LOG
		DiagPrintf("CH0 Output write Done..\n\r");
#endif

	} else if (io->zoom_en == 2) {
		int bk_w = 0, bk_h = 0;
		// stream 0
		bk_w = read_reg(base, 0xD8C08) & BK_FRAME_WIDTH_MASK;
		bk_h = read_reg(base, 0xD8C08) & BK_FRAME_HEIGHT;
		bk_h >>= BK_FRAME_HEIGHT_OFFSET;
		memset(fn_out, 0, sizeof(fn_out));
		sprintf(fn_out, "D:\\oscaryang\\00_Test\\test_output(%dx%d_zoom%d_ch0)", bk_w, bk_h, io->zoom_en);
		CRC_MULTI_STREAM[0] = savefile(io, read_reg(base, 0xF00C0), read_reg(base, 0xF02A0) + read_reg(base, 0xF02A4), seg_size, fn_out);
#if SHOW_LOG
		DiagPrintf("CH0 Output write Done..\n\r");
#endif

		// stream 1
		bk_w = read_reg(base, 0xD9008) & BK_FRAME_WIDTH_MASK;
		bk_h = read_reg(base, 0xD9008) & BK_FRAME_HEIGHT;
		bk_h >>= BK_FRAME_HEIGHT_OFFSET;
		memset(fn_out, 0, sizeof(fn_out));
		sprintf(fn_out, "D:\\oscaryang\\00_Test\\test_output(%dx%d_zoom%d_ch1)", bk_w, bk_h, io->zoom_en);
		CRC_MULTI_STREAM[1] = savefile(io, read_reg(base, 0xF00E0), read_reg(base, 0xF02A8) + read_reg(base, 0xF02AC), seg_size, fn_out);
#if SHOW_LOG
		DiagPrintf("CH1 Output write Done..\n\r");
#endif

		// stream 2
		bk_w = read_reg(base, 0xD9108) & BK_FRAME_WIDTH_MASK;
		bk_h = read_reg(base, 0xD9108) & BK_FRAME_HEIGHT;
		bk_h >>= BK_FRAME_HEIGHT_OFFSET;
		memset(fn_out, 0, sizeof(fn_out));
		sprintf(fn_out, "D:\\oscaryang\\00_Test\\test_output(%dx%d_zoom%d_ch2)", bk_w, bk_h, io->zoom_en);
		CRC_MULTI_STREAM[2] = savefile(io, read_reg(base, 0xF0100), read_reg(base, 0xF02B0) + read_reg(base, 0xF02B4), seg_size, fn_out);
#if SHOW_LOG
		DiagPrintf("CH2 Output write Done..\n\r");
#endif

		// stream 3
		bk_w = read_reg(base, 0xD9208) & BK_FRAME_WIDTH_MASK;
		bk_h = read_reg(base, 0xD9208) & BK_FRAME_HEIGHT;
		bk_h >>= BK_FRAME_HEIGHT_OFFSET;
		memset(fn_out, 0, sizeof(fn_out));
		sprintf(fn_out, "D:\\oscaryang\\00_Test\\test_output(%dx%d_zoom%d_ch3)", bk_w, bk_h, io->zoom_en);
		CRC_MULTI_STREAM[3] = savefile(io, read_reg(base, 0xF0120), read_reg(base, 0xF02B8) + read_reg(base, 0xF02BC), seg_size, fn_out);
#if SHOW_LOG
		DiagPrintf("CH3 Output write Done..\n\r");
#endif

		// stream 4
		bk_w = read_reg(base, 0xD9308) & BK_FRAME_WIDTH_MASK;
		bk_h = read_reg(base, 0xD9308) & BK_FRAME_HEIGHT;
		bk_h >>= BK_FRAME_HEIGHT_OFFSET;
		memset(fn_out, 0, sizeof(fn_out));
		sprintf(fn_out, "D:\\oscaryang\\00_Test\\test_output(%dx%d_zoom%d_ch4)", bk_w, bk_h, io->zoom_en);
		CRC_MULTI_STREAM[4] = savefile(io, read_reg(base, 0xF0140), read_reg(base, 0xF02C0) + read_reg(base, 0xF02C4) + read_reg(base, 0xF02C8), seg_size, fn_out);
#if SHOW_LOG
		DiagPrintf("CH4 Output write Done..\n\r");
#endif
	}
}

void isp_io_task(void *pvParameters)
{
	struct isp_io *io = (struct isp_io *)pvParameters;
	int ret = 0;
	size_t width = io->width;
	size_t height = io->height;
	int in_type = io->g_input_type;
	int out_type = io->g_output_type;
	void *buf;
	isp_io_base = (void *)0x60000000;
	void *base = io->base;


	if (!io || !io->initialized || !width || !height) {
		return;// RTS_RETURN(RTS_E_INVALID_ARG);
	}
	printf("----------io->dbg_path:%d width:%d  height:%d \n\r", io->dbg_path, io->width, io->height);


	if (io->low_latency_mode) {
		printf("run isp with low latency \n\r");
		is_runing = 2;
	} else {
		is_runing = 1;
	}

	ret = isp_alloc_buffer(io, io->width, io->height, io->g_input_type, io->g_output_type);
	if (ret) {
		RTS_ERR("config image fail, ret = %d", ret);
		return;
	}

	data_in(io);


	config_security(io);

	if (io->modules_en) {
		testisp_config_regs(io);
	} else {
		write_reg(base, 0x0, SYS_CONTROL1);
	}

	if (io->dbg_path >= RTS_RAW_SRC_DISABLE && io->dbg_path <= RTS_RAW_SRC_B_CCM_B) {
		dbg_path_config(io);
	}

	config_ldc_reg(io);

	config_mask(io);

	if (io->zoom_en == 0) {
		video_config(io);
	} else if (io->zoom_en == 1) {
		ret = config_singlezoom(io);
	} else if (io->zoom_en == 2) {
		config_multizoom_reg(io);
		if (!io->low_latency_mode) {
			config_burst_length(io, 0x2AAAAA);
		}
	}

	if (io->vhdr_en) {
		config_vhdr_reg();
	}

	isp_restart(io);

	ret = isp_verify_run(io);
	if (ret) {
		RTS_ERR("run fail, ret = %d", ret);
		return;// ret;
	}

	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	if (io->fixp_en) {
		printf("io->fixp_en Enable\n\r");
		config_fixp_reg(io); // isp_clk = 48e6
		if (!io->low_latency_mode) {
			fixp_start(io);
		}
	} else {
		verify_start(io);
	}

	is_runing = 1;
	ret = isp_wait_completion(io);
	if (ret) {
		RTS_ERR("[warning]wait isp completion, ret = %d\n", ret);
		return;// ret;
	}

	fixp_stop(io);

	data_out(io);

	is_runing = 0;
	vTaskDelete(NULL);
	is_runing = 0;
	return;
}

int params_verify(struct isp_io *io)
{
	int IMG_W  = io->width;
	int IMG_H   = io->height;
	int crop_w = io->crop_w;
	int crop_h = io->crop_h;
	int izoom_w = io->zoom_w[0];
	int izoom_h = io->zoom_h[0];
	DiagPrintf("input type   : %d\n\r", io->g_input_type - ISP_INPUT_CROP_L);
	DiagPrintf("output type  : %d\n\r", io->g_output_type - ISP_OUTPUT_CROP_OUT_L);
	DiagPrintf("function     : 0x%08X\n\r", io->zoom_en);
	DiagPrintf("io->save_output  : %d\n\r", io->save_output);
	DiagPrintf("io->zoomout_nv16 : %d\n\r", io->zoomout_nv16);
	DiagPrintf("io->width        : %d\n\r", io->width);
	DiagPrintf("io->height       : %d\n\r", io->height);
	DiagPrintf("io->crop_w       : %d\n\r", io->crop_w);
	DiagPrintf("io->crop_h       : %d\n\r", io->crop_h);
	DiagPrintf("io->zoom_w[0]    : %d\n\r", io->zoom_w[0]);
	DiagPrintf("io->zoom_h[0]    : %d\n\r", io->zoom_h[0]);
	DiagPrintf("io->pattern_input: %d\n\r", io->pattern_input);
	DiagPrintf("io->dbg_path     : %d\n\r", io->dbg_path);
	DiagPrintf("io->fixp_en      : %d\n\r", io->fixp_en);
	DiagPrintf("io->modules_en   : %d\n\r", io->modules_en);
	DiagPrintf("io->mask_en      : %d\n\r", io->mask_en);
	DiagPrintf("io->ldc_en      : %d\n\r", io->ldc_en);

	if (io->g_input_type < ISP_INPUT_CROP_L || io->g_input_type > ISP_INPUT_NV16) {
		printf("[ERR][%s] io->g_input_type<%d || io->g_input_type>%d\r\n", __FUNCTION__, ISP_INPUT_CROP_L, ISP_INPUT_NV16);
		return 1;
	} else if (io->g_output_type < ISP_OUTPUT_CROP_OUT_L || io->g_output_type > ISP_OUTPUT_YUYV) {
		printf("[ERR][%s] io->g_output_type<%d || io->g_output_type>%d\r\n", __FUNCTION__, ISP_OUTPUT_CROP_OUT_L, ISP_OUTPUT_YUYV);
		return 2;
	} else if (io->zoom_en < 0 || io->zoom_en > 2) {
		printf("[ERR][%s] io->zoom_en<0 || io->zoom_en>2\r\n", __FUNCTION__);
		return 3;
	} else if (io->save_output < 0 || io->save_output > 1) {
		printf("[ERR][%s] io->save_output<0 || io->save_output>1\r\n", __FUNCTION__);
		return 4;
	} else if (io->zoomout_nv16 < 0 || io->zoomout_nv16 > 2) {
		printf("[ERR][%s] io->zoomout_nv16<0 || io->zoomout_nv16>2\r\n", __FUNCTION__);
		return 5;
	} else if (IMG_W < 160 || IMG_W > 2688 || IMG_H < 90 || IMG_H > 1944) {
		printf("[ERR][%s] IMG_W<160 || IMG_W>2688 || IMG_H<90 || IMG_H>1944\r\n", __FUNCTION__);
		return 6;
	} else if (crop_w < 160 || crop_w > 2688 || crop_h < 90 || crop_h > 1944) {
		printf("[ERR][%s] crop_w<160 || crop_w>2688 || crop_h<90 || crop_h>1944\r\n", __FUNCTION__);
		return 7;
	} else if (izoom_w < 160 || izoom_w > 2688 || izoom_h < 90 || izoom_h > 1944) {
		printf("[ERR][%s] izoom_w<160 || izoom_w>2688 || izoom_h<90 || izoom_h>1944\r\n", __FUNCTION__);
		return 8;
	} else if (IMG_W % 2 || IMG_H % 2 || crop_w % 2 || crop_h % 2 || izoom_w % 2 || izoom_h % 2) {
		printf("[ERR][%s] width and height 2-alignment\r\n", __FUNCTION__);
		return 9;
	} else if (izoom_w > crop_w || crop_w > IMG_W || izoom_h > crop_h || crop_h > IMG_H) {
		printf("[ERR][%s] izoom_w < crop_w < IMG_H\r\n", __FUNCTION__);
		return 10;
	} else if (io->pattern_input < 0 || io->pattern_input > 1) {
		printf("[ERR][%s] io->pattern_input<0 || io->pattern_input>1\r\n", __FUNCTION__);
		return 11;
	} else if (io->dbg_path < RTS_RAW_SRC_DISABLE || io->dbg_path > 255) {
		printf("[ERR][%s] io->dbg_path<%d || io->dbg_path>255\r\n", __FUNCTION__, RTS_RAW_SRC_DISABLE);
		return 12;
	} else if (io->fixp_en < 0 || io->fixp_en > 1) {
		printf("[ERR][%s] io->fixp_en<0 || io->fixp_en>1\r\n", __FUNCTION__);
		return 13;
	} else if (io->modules_en < 0 || io->modules_en > 1) {
		printf("[ERR][%s] io->modules_en<0 || io->modules_en>1\r\n", __FUNCTION__);
		return 14;
	} else if (io->mask_en < 0 || io->mask_en > 1) {
		printf("[ERR][%s] io->mask_en<0 || io->mask_en>1\r\n", __FUNCTION__);
		return 15;
	} else if (io->ldc_en < 0 || io->ldc_en > 1) {
		printf("[ERR][%s] io->ldc_en<0 || io->ldc_en>1\r\n", __FUNCTION__);
		return 16;
	}
	return 0;
}

int isp_io_start(int *arg)
{
	int CRC_GT;
	int ret = isp_init(&g_io);
	void *base = g_io.base;
	is_runing = 2;
	printf("[HW Verify]0x2C: 0x%lX \n\r", read_reg(base, 0x2C));
	if (ret) {
		RTS_ERR("init fail, ret = %d", ret);
		return;
	}
	g_io.g_input_type  = arg[0] + ISP_INPUT_CROP_L;
	g_io.g_output_type = arg[1] + ISP_OUTPUT_CROP_OUT_L;
	g_io.zoom_en       = arg[2];
	g_io.save_output   = arg[3];
	g_io.zoomout_nv16  = arg[4];
	g_io.width         = arg[5];
	g_io.height        = arg[6];
	g_io.crop_w        = arg[7];
	g_io.crop_h        = arg[8];
	g_io.zoom_w[0]     = arg[9];
	g_io.zoom_h[0]     = arg[10];
	g_io.pattern_input = arg[11];
	g_io.dbg_path      = arg[12];
	g_io.fixp_en       = arg[13];
	g_io.modules_en    = arg[14];
	g_io.mask_en       = arg[15];
	g_io.ldc_en        = arg[16];
	g_io.mempool_start = DDR_START;
	g_io.wait_fd_round = 100;
	g_io.wait_fd_delay = 10;

	if (g_io.g_input_type == ISP_INPUT_FUSION_IN) {
		g_io.vhdr_en = 1;
		g_io.g_output_type = ISP_OUTPUT_YUYV;
		g_io.zoom_en = 0;
		g_io.zoomout_nv16 = 2;
		g_io.width  = g_io.crop_w = g_io.zoom_w[0] = 2688;
		g_io.height = g_io.crop_h = g_io.zoom_h[0] = 1944;
		g_io.pattern_input = 0;
		g_io.dbg_path = 255;
		g_io.fixp_en = 0;
		g_io.mask_en = 0;
		g_io.ldc_en = 0;
		printf("[vHDR=ON] Below parameters are forced.");
		printf(":\t g_io.vhdr_en = 1;                    ");
		printf(":\t g_io.g_output_type = ISP_OUTPUT_YUYV;");
		printf(":\t g_io.zoom_en = 0;               ");
		printf(":\t g_io.zoomout_nv16 = 2;               ");
		printf(":\t IMG_W = crop_w = izoom_w = 2688; ");
		printf(":\t IMG_H = crop_h = izoom_h = 1944; ");
		printf(":\t g_io.pattern_input = 0;              ");
		printf(":\t g_io.dbg_path = 255;                 ");
		printf(":\t g_io.fixp_en = 0;                    ");
		printf(":\t g_io.mask_en = 0;                    ");
		printf(":\t g_io.ldc_en = 0;                     ");
	} else {
		g_io.vhdr_en = 0;
	}
	switch (g_io.dbg_path) {
	case RTS_RAW_SRC_DISABLE:
		g_io.g_output_type = ISP_OUTPUT_YUYV;
		break;
	case RTS_RAW_SRC_BLC_LONGEXP_B:
		g_io.g_output_type = ISP_OUTPUT_CROP_OUT_L;
		break;
	case RTS_RAW_SRC_BLC_LONGEXP_A:
		g_io.g_output_type = ISP_OUTPUT_TPNR_OUT;
		break;
	case RTS_RAW_SRC_DNS_LONGEXP_A:
		g_io.g_output_type = ISP_OUTPUT_TPNR_OUT;
		break;
	case RTS_RAW_SRC_DPC_LONGEXP_A:
		g_io.g_output_type = ISP_OUTPUT_TPNR_OUT;
		break;
	case RTS_RAW_SRC_AE_B:
		g_io.g_output_type = ISP_OUTPUT_FUSION_IN_L;
		break;
	case RTS_RAW_SRC_R_CCM_B:
		g_io.g_output_type = ISP_OUTPUT_RGB;
		break;
	case RTS_RAW_SRC_G_CCM_B:
		g_io.g_output_type = ISP_OUTPUT_RGB;
		break;
	case RTS_RAW_SRC_B_CCM_B:
		g_io.g_output_type = ISP_OUTPUT_RGB;
		break;
	default:
		break;
	}
	if (params_verify(&g_io)) {
		return 1;
	}

	xTaskCreate(isp_io_task, "isp_io_task", configMINIMAL_STACK_SIZE * 16, &g_io,
				(portPRIVILEGE_BIT | (configMAX_PRIORITIES / 2)), NULL);

	printf("isp_io_task is runing(%d). \r\n", is_runing);
	while (is_runing) {
		printf("isp_io_task is runing(%d). \r\n", is_runing);
		vTaskDelay(1000);
	}
	printf("isp_io_task is done(%d). \r\n", is_runing);

	if (g_io.zoom_en == 0) {
		if (g_io.modules_en) {
			if (g_io.crop_w == 2688 && g_io.crop_h == 1520) {
				if (g_io.dbg_path == 0) {
					CRC_GT = 0xBD8E3FAE;
				} else if (g_io.dbg_path == 1) {
					CRC_GT = 0x315FF340;
				} else if (g_io.dbg_path == 2) {
					CRC_GT = 0x39DEAF7B;
				} else if (g_io.dbg_path == 3) {
					CRC_GT = 0x0DAFE5F2;
				} else if (g_io.dbg_path == 4) {
					CRC_GT = 0x39DEAF7B;
				} else if (g_io.dbg_path == 5) {
					CRC_GT = 0x21F42AB1;
				} else if (g_io.dbg_path == 6) {
					CRC_GT = 0xFAB594FE;
				}
				printf("g_io.dbg_path: %d. \r\n", g_io.dbg_path);
			} else if (g_io.crop_w == 1920 && g_io.crop_h == 1080) {
				CRC_GT = 0x86B2C0A6;
			} else if (g_io.crop_w == 1280 && g_io.crop_h == 720) {
				CRC_GT = 0xEB5FBF51;
			} else if (g_io.crop_w == 320 && g_io.crop_h == 180) {
				CRC_GT = 0xD8FC3E42;
			}
		} else {
			if (g_io.crop_w == 2688 && g_io.crop_h == 1520) {
				CRC_GT = 0xD7593768;
			} else if (g_io.crop_w == 1920 && g_io.crop_h == 1080) {
				CRC_GT = 0xA2EF5518;
			} else {
				CRC_GT = 0x00000000;
			}
		}
		if (CRC_SINGLE_STREAM == CRC_GT) {
			printf("ISP SINGLE OUTPUT SUCCESS. \r\n");
			ret = 0;
		} else {
			printf("ISP SINGLE OUTPUT FAIL \r\n");
			ret = 1;
		}
		printf("g_io.zoom_en=%d  CRC: 0x%08lX(CRC_GT: 0x%08lX) W=%d H=%d MODU=%d \r\n", g_io.zoom_en, CRC_SINGLE_STREAM, CRC_GT, g_io.crop_w, g_io.crop_h,
			   g_io.modules_en);
	} else if (g_io.zoom_en == 1) {
		if (g_io.mask_en) {
			CRC_GT = 0x2638C84D;
		} else if (g_io.ldc_en) {
			CRC_GT = 0x285B1831;
		} else {
#if CRC_DO
			CRC_GT = 0x79339793;
#else
			CRC_GT = 0x1DD12D01;
#endif
		}
		if (CRC_SINGLE_STREAM == CRC_GT) {
			printf("ISP SINGLE ZOOM OUTPUT SUCCESS \r\n");
			ret = 0;
		} else {
			printf("ISP SINGLE ZOOM OUTPUT FAIL \r\n");
			ret = 1;
		}
		printf("g_io.zoom_en=%d  CRC: 0x%08lX(CRC_GT: 0x%08lX) W=%d H=%d MODU=%d \r\n", g_io.zoom_en, CRC_SINGLE_STREAM, CRC_GT, g_io.crop_w, g_io.crop_h,
			   g_io.modules_en);
	} else if (g_io.zoom_en == 2) {
#if CRC_DO
		if (CRC_MULTI_STREAM[0] == 0xC8AB70C2 &&
			CRC_MULTI_STREAM[1] == 0x715EDBD0 &&
			CRC_MULTI_STREAM[2] == 0x715EDBD0 &&
			CRC_MULTI_STREAM[3] == 0xB563B8E4 &&
			CRC_MULTI_STREAM[4] == 0x676AEE26) {
//MULTI_ZOOM(zoom_opt=2) CRC: 0xC8AB70C2  0x715EDBD0  0x715EDBD0  0xB563B8E4  0x676AEE26
#else
		if (CRC_MULTI_STREAM[0] == 0xDD129A40 &&
			CRC_MULTI_STREAM[1] == 0xDD129A40 &&
			CRC_MULTI_STREAM[2] == 0xDD129A40 &&
			CRC_MULTI_STREAM[3] == 0xF805FA9B &&
			CRC_MULTI_STREAM[4] == 0x54B26B2D) {
//MULTI_ZOOM(zoom_opt=2) CRC: 0xDD129A40  0xDD129A40  0xDD129A40  0xF805FA9B  0x54B26B2D
#endif
			isp_printf("ISP MULTIPLE ZOOM OUTPUT SUCCESS \r\n");
			ret = 0;
		} else {
			printf("ISP MULTIPLE ZOOM OUTPUT FAIL \r\n");
			ret = 1;
		}
		printf("MULTI_ZOOM(zoom_opt=%d) CRC: 0x%08lX  0x%08lX  0x%08lX  0x%08lX  0x%08lX \r\n", g_io.zoom_en, CRC_MULTI_STREAM[0], CRC_MULTI_STREAM[1],
			   CRC_MULTI_STREAM[2],
			   CRC_MULTI_STREAM[3], CRC_MULTI_STREAM[4]);
	}
	isp_release(&g_io);
	return ret;
}
