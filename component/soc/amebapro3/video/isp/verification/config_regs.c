#include <diag.h>
#include <log.h>

#include <FreeRTOS.h>
#include <task.h>

#include <stdio.h>
#include <stdlib.h>
#include "isp_reg.h"
#include "ameba_soc.h"
#include "isp_io.h"
#include "isp_regs.h"

void fixp_start(struct isp_io *io)
{
	void *base = io->base;
	write_reg_dbg(base, FIXP_START, FIXP_CTRL);
}
void fixp_stop(struct isp_io *io)
{
	void *base = io->base;
	write_reg_dbg(base, FIXP_STOP, FIXP_CTRL);
}
void verify_start(struct isp_io *io)
{
	void *base = io->base;
	uint32_t location;
	uint32_t format, i;
	int ret = get_in_location_fmt(io->in_type, &location, &format);
	if (ret) {
		RTS_ERR("parse input format fail\n");
		return ret;
	}
	printf("io->fixp_en Disable\n\r");
	printf("%d %d %d %d \n\r", io->in_y_addr, io->in_uv_addr, io->in_y_size, io->in_uv_size);
	write_reg_dbg(base, location, VERIFY_SEL);
	write_reg_dbg(base, 0x25, VERIFY_CTRL);
	write_reg_dbg(base, io->in_y_addr, VERIFY_Y_DDR_ADDR0);
	write_reg_dbg(base, io->in_uv_addr, VERIFY_UV_DDR_ADDR0);
	write_reg_dbg(base, io->in_y_size, VERIFY_Y_DDR_LEN);
	write_reg_dbg(base, io->in_uv_size, VERIFY_UV_DDR_LEN);
	write_reg_dbg(base, 1, VERIFY_FRAME_NUM);
	write_reg_dbg(base, format, VERIFY_FRAME_FORMAT);
	write_reg_dbg(base, (io->width - 1) | (io->height - 1) << 16, VERIFY_FRAME_SIZE);
	write_reg(base, 0x1010, VERIFY_FRAME_CONFG0);
#if 1//def SPACE_ISSUE_TEST
	write_reg(base, 0xFFF, VERIFY_FRAME_CONFG1);
#else
	write_reg(base, (64 - 1) | (64 - 1) << 16, VERIFY_FRAME_CONFG1);
#endif
	write_reg(base, 1, VERIFY_INT_FLAG);
	write_reg(base, 1, VERIFY_START_FLAG);
}
void config_fixp_reg(struct isp_io *io)
{
	if (!io) {
		return;
	}
	uint32_t hts = 0;
	uint32_t min_vts;
	uint32_t isp_clk;
	int size_w = io->width;
	int size_h = io->height;
	int fps = 10;
	uint32_t block_width, block_height, block_start_x, block_start_y;
	void *base = io->base;

	if (io->remap_fps) {
		fps = io->remap_fps;
	}
	printf("[%s] %d \n\r", __FUNCTION__, size_w);

	//if (isp_os_driver_is_fpga())
	isp_clk = 48e6;
	//else
	//	isp_clk = 160e6;

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

	//min_vts = isp_clk / fps / hts;
	min_vts = size_h * 1.2;
	hts = isp_clk / fps / min_vts;
	if (hts < size_w) {
		hts = size_w * 1.2;
		fps = isp_clk / hts / min_vts;
		hts = isp_clk / fps / min_vts;
	}
	printf("[%s] min_vts(%d)  isp_clk(%d)  fps(%d)  hts(%d) \n\r", __FUNCTION__, min_vts, isp_clk, fps, hts);


	write_reg_dbg(base, 0x2, 0x0044);
	write_reg_dbg(base, size_w, FIXP_FRAME_WIDTH);
	write_reg_dbg(base, size_h, FIXP_FRAME_HEIGHT);
	write_reg_dbg(base, hts - size_w, FIXP_DUMMY_PIX);
	write_reg_dbg(base, min_vts - size_h, FIXP_DUMMY_LINE);

	block_width = (size_w - 0x14 * 7) / 8;
	block_height = (size_h - 0x14 * 5) / 6;
	block_start_x = 0 + block_width;
	block_start_y = 0 + block_height;
	write_reg_dbg(base, block_width, BLOCK_WIDTH);
	write_reg_dbg(base, block_height, BLOCK_HEIGHT);
	write_reg_dbg(base, block_start_x, BLOCK_START_X);
	write_reg_dbg(base, block_start_y, BLOCK_START_Y);

	write_reg_dbg(base, 32, FIXP_VC_DELAY);

	return;
}

int estimate_multizoom_size(struct isp_io *io)
{
	int sum_ddr_output = 0;
	io->zoom_w[1] = io->zoom_w[2] = io->zoom_w[0];
	io->zoom_h[1] = io->zoom_h[2] = io->zoom_h[0];
	io->zoom_w[3] = 640;
	io->zoom_h[3] = 360;
	io->zoom_w[4] = 576;
	io->zoom_h[4] = 320;
	sum_ddr_output = 0;
	for (int stream_idx = 0; stream_idx < 4; stream_idx++) {
		int out_w = io->zoom_w[stream_idx];
		if (io->aligned_w[stream_idx]) {
			out_w = io->aligned_w[stream_idx];
		}
		//[Stream 0 (ch0)] 2560	1440 @ isp_zoom_fhd
		//[Stream 2 (ch2)] 2560	1440 @ isp_zoom_vga
		//[Stream 3 (ch3)] 640	360 @ isp_zoom_qvga
		//[Stream 1 (ch1)] 2560	1440 @ isp_zoom_hd
		if (io->zoomout_nv16 == 2) {
			io->out_y_size[stream_idx] = out_w * io->zoom_h[stream_idx] * 2;
			io->out_uv_size[stream_idx] = 0;
		} else if (io->zoomout_nv16 == 1) {
			io->out_y_size[stream_idx] = out_w * io->zoom_h[stream_idx];
			io->out_uv_size[stream_idx] = io->out_y_size[stream_idx];
		} else {
			io->out_y_size[stream_idx] = out_w * io->zoom_h[stream_idx];
			io->out_uv_size[stream_idx] = io->out_y_size[stream_idx] / 2;
		}
		sum_ddr_output += (2 * (io->out_y_size[stream_idx] + io->out_uv_size[stream_idx]));
		printf("[%s] stream:%d  io->zoom_w:%d  io->zoom_h:%d  io->out_y_size:%d  io->out_uv_size:%d \n\r", __FUNCTION__, stream_idx, out_w, io->zoom_h[stream_idx],
			   io->out_y_size[stream_idx],
			   io->out_uv_size[stream_idx]);
	}

	//[Stream 4 (ch4) RGB] 576	320 @ isp_zoom_ai
	if (io->aligned_w[4]) {
		io->out_rgb_size = io->aligned_w[4] * io->zoom_h[4];
	} else {
		io->out_rgb_size = io->zoom_w[4] * io->zoom_h[4];
	}
	sum_ddr_output += (2 * 3 * io->out_rgb_size);
	return sum_ddr_output;
}
void estimate_multizoom_buffaddr(struct isp_io *io)
{
	int i, j;
	int outbuf_size = 0;
	int stream_num = 5;
	int slot_num   = 2;
	int ddr_set_start = io->out_buf.phy_addr;
	if (!ddr_set_start) {
		isp_printf("[Warning]io->out_buf.phy_addr is not initialized.\r\n");
		return;
	}

	for (j = 0; j < slot_num; j++) {
		for (i = 0; i < stream_num - 1; i++) {
			io->out_y_addr[i][j] = ddr_set_start;
			ddr_set_start += io->out_y_size[i]; //  Y_FRAME_ADDR0
			io->out_uv_addr[i][j] = ddr_set_start;
			ddr_set_start += io->out_uv_size[i]; // UV_FRAME_ADDR0
		}
		for (i = 0; i < 3; i++) {
			io->out_rgb_addr[i][j] = ddr_set_start;
			ddr_set_start += io->out_rgb_size;   // RGB_FRAME_ADDR0
		}
	}

	outbuf_size = ddr_set_start - io->out_buf.phy_addr;
	printf("[%s] Input  DDR used: %d \n\r", __FUNCTION__, 2 * io->width * io->height);
	printf("[%s] Output DDR used: %d \n\r", __FUNCTION__, outbuf_size);
	printf("[%s] io->out_buf.phy_addr: 0x%X \n\r", __FUNCTION__, io->out_buf.phy_addr);
#if BUF_RESET
	memset((void *)(io->out_buf.phy_addr), 55, outbuf_size);
#else
	memset((void *)(io->out_buf.phy_addr), 55, 1024);
	memset((void *)(io->out_buf.phy_addr) + outbuf_size / 2 - 1024, 55, 1024);
	memset((void *)(io->out_buf.phy_addr) + outbuf_size   - 1024, 55, 1024);
#endif
}
void config_burst_length(struct isp_io *io, uint32_t value)
{
	void *base = io->base;
	write_reg_dbg(base, value, 0xF0190);    // MAX_BURST_LENGTH_SELECT
}

void config_multizoom_reg(struct isp_io *io)
{
#if 0
	unsigned int bk_w, bk_h;
	unsigned int step_h, step_v;

	if (!io->zoom_en) {
		return 0;
	}

	// init zoom
	write_reg_dbg(base, 0x1, 0xd8000); //(SYS_ZOOM_BASE_ADDR + 0x00000000)    //0xb8ad8000
	write_reg_dbg(base, 0x101, 0xd8004); //(SYS_ZOOM_BASE_ADDR + 0x00000004)    //0xb8ad8004
	write_reg_dbg(base, 0x4, 0xd8008); //(SYS_ZOOM_BASE_ADDR + 0x00000008)    //0xb8ad8008

	int reg_zoom_size = (io->zoom_h[0] << 16) + io->zoom_w[0];
	write_reg_mask_dbg(base, reg_zoom_size, 0xd8c08, 0xffffffff); // 798->5F0: 1944->1520 780x438 original: 0x07980a80 1080P:0x04380780
	write_reg_mask_dbg(base, 0x01010000, 0xd8c10, 0xffffffff); // ISP_ZOOM_UP_COEF
	write_reg_mask_dbg(base, 0x0e080502, 0xd8c14, 0xffffffff);
	write_reg_mask_dbg(base, 0x35291e15, 0xd8c18, 0xffffffff);
	write_reg_mask_dbg(base, 0x6b5e5042, 0xd8c1c, 0xffffffff);
	write_reg_mask_dbg(base, 0x86817c72, 0xd8c20, 0xffffffff);

	// Config step
	bk_w = read_reg(base, 0xd8c08) & BK_FRAME_WIDTH_MASK; //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000008)    //0xb8ad8c08
	bk_h = read_reg(base, 0xd8c08) & BK_FRAME_HEIGHT;
	bk_h >>= BK_FRAME_HEIGHT_OFFSET;

	step_h = (float) io->width  / bk_w * 1024;
	step_v = (float) io->height / bk_h * 1024;

	write_reg_dbg(base, step_h, 0xD8C4C); //(ISP_ZOOM_FHD_BASE_ADDR + 0x0000004c)    //0xb8ad8c4c
	write_reg_dbg(base, step_v, 0xD8C48); //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000048)    //0xb8ad8c48

	write_reg_dbg(base, 1, 0xD8C00); //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000000)    //0xb8ad8c00
	write_reg_dbg(base, 1, 0xD8C24); //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000024)    //0xb8ad8c24

	// wait zoom trig
	write_reg_mask_dbg(base, 0x8, 0xd8000, ZOOM_DOWN_REG_LATCH_MASK);

//1:422 0:420
	if (io->zoomout_nv16 > 0) {
		write_reg_dbg(base, 1, 0xd8018); //(SYS_ZOOM_BASE_ADDR + 0x00000018)    //0xb8ad8018
	} else {
		write_reg_dbg(base, 0, 0xd8018); //(SYS_ZOOM_BASE_ADDR + 0x00000018)    //0xb8ad8018
	}
	//write_reg_dbg(base, 0xf, 0xF0070);
	//write_reg_dbg(base, 1, 0xD8020);

	write_reg(base, io->out_y_addr[0][0], 0xF00C0);
	write_reg(base, io->out_uv_addr[0][0], 0xF00D0);
	write_reg(base, io->out_y_size[0], 0xF02A0);
	write_reg(base, io->out_uv_size[0], 0xF02A4);
	write_reg(base, 0x20000000, 0xF01A0); //Fixed blank data: 0x18000000 -> 0x20000000
	write_reg(base, 0x10002000, 0xF01A4);
	write_reg(base, 1, 0xF0180);

//ISP Stream 0 Output YUV422 InterleavedSelect:
//0: YUV420/YUV422 Semi-plannar,
//1: YUV422 Interleaved
	if (io->zoomout_nv16 == 2) {
		write_reg_dbg(base, 1, 0xF0074);
	} else {
		write_reg_dbg(base, 0, 0xF0074);
	}
	write_reg(base, 1,       0xF0000);
	write_reg(base, 0,       0xF0250);
	write_reg(base, 0x3000f, 0xF0254);
	write_reg(base, 0x30,    0xF0080);

	printf("--Single Zoom Config Done.--\r\n");
	printf("--Single Zoom Config Done.--\r\n");
	//return 0;
#endif


	void *base = io->base;
	int stream_num = 5;
	int en_addr[STREAM_N] = {0xD8C00, 0xD9000, 0xD9100, 0xD9200, 0xD9300};
	int stream_idx = 0;
	printf("ZOOM_MULTI Enabled(io->zoom_en=%d)\n\r", io->zoom_en);
	write_reg_dbg(base, 0x1, en_addr[0] + 0x24);
	write_reg_dbg(base, 0x8, en_addr[0] + 0x28);
	for (stream_idx = 0; stream_idx < stream_num; stream_idx++) {
		write_reg_dbg(base, 0x1, en_addr[stream_idx]);
		write_reg_dbg(base, 0x40000, en_addr[stream_idx] + 0x04);

		write_reg_dbg(base, io->zoom_w[stream_idx] + (io->zoom_h[stream_idx] << 16), en_addr[stream_idx] + 0x08);

		write_reg_dbg(base, 0x01010000, en_addr[stream_idx] + 0x10); //-> multizoom_dbg
		write_reg_dbg(base, 0x0e080502, en_addr[stream_idx] + 0x14); //-> multizoom_dbg
		write_reg_dbg(base, 0x35291e15, en_addr[stream_idx] + 0x18); //-> multizoom_dbg
		write_reg_dbg(base, 0x6b5e5042, en_addr[stream_idx] + 0x1C); //-> multizoom_dbg
		write_reg_dbg(base, 0x86817c72, en_addr[stream_idx] + 0x20); //-> multizoom_dbg

		//--STEPS--
		write_reg_dbg(base, io->crop_h * 1024 / io->zoom_h[stream_idx], en_addr[stream_idx] + 0x48);
		write_reg_dbg(base, io->crop_w * 1024 / io->zoom_w[stream_idx], en_addr[stream_idx] + 0x4C);
	}


	//----------------------------------------------------

	//sys_zoom
	write_reg_dbg(base, 0x1, 0xD8000);
	write_reg_dbg(base, 0x101, 0xD8004);
	write_reg_dbg(base, 0x4, 0xD8008);

	if (io->zoomout_nv16 > 0) {
		write_reg_dbg(base, 1, 0xD8018); //(SYS_ZOOM_BASE_ADDR + 0x00000018)    //0xb8ad8018
	} else {
		write_reg_dbg(base, 0, 0xD8018); //(SYS_ZOOM_BASE_ADDR + 0x00000018)    //0xb8ad8018
	}
	write_reg_dbg(base, 0x0, 0xD801C);
	write_reg_dbg(base, 0x1, 0xD8020);
	write_reg_dbg(base, 0x0, 0xD8024);
	write_reg_dbg(base, 0x0, 0xD8028);
	write_reg_dbg(base, 0x2, 0xD802C);

	write_reg_dbg(base, 0x0, 0xD8034);
	write_reg_dbg(base, 0x9, 0xD8000);


	//----------------------------------------------------

	//[video_amba   ]
	for (stream_idx = 0; stream_idx < stream_num; stream_idx++) {
		write_reg_dbg(base,    0x1, 0xF0000 + 4 * stream_idx);       // ISP_STREAM0_EN
		write_reg_dbg(base,    0x0, 0xF0030 + 4 * stream_idx);       // ISP_STREAM0_RST
		write_reg_dbg(base, 0x3030, 0xF0080 + 4 * stream_idx);       // ISP_STREAM0_DDR_FRAME_BUFFER_STATUS  //-> multizoom_dbg
	}

	write_reg_dbg(base, 0x0, 0xF0060);         // VIDEO_AXI_RST
	write_reg_dbg(base, 0x0, 0xF0064);         // VIDEO_APB_RST
	write_reg_dbg(base, 0x0, 0xF0068);         // VIDEO_AXI_WRITE_RST
	write_reg_dbg(base, 0x0, 0xF006C);         // VIDEO_AXI_READ_RST
	write_reg_dbg(base, 0xF, 0xF0070);         // ISP_NV12_SEL
	write_reg_dbg(base, 0x0, 0xF0074);         // YUV422_INTERLEAVED_SEL

	int slot_num   = 2;
	for (int slot_idx = 0; slot_idx < slot_num; slot_idx++) {
		for (stream_idx = 0; stream_idx < stream_num - 1; stream_idx++) {
			write_reg_dbg(base,  io->out_y_addr[stream_idx][slot_idx], 0xF00C0 + 4 * slot_idx + 0x20 * stream_idx);
			write_reg_dbg(base, io->out_uv_addr[stream_idx][slot_idx], 0xF00D0 + 4 * slot_idx + 0x20 * stream_idx);
		}
		for (stream_idx = 0; stream_idx < 3; stream_idx++) {
			write_reg_dbg(base, io->out_rgb_addr[stream_idx][slot_idx], 0xF0140 + 4 * slot_idx + 0x10 * stream_idx);
		}
	}


	write_reg_dbg(base, 0x2222, 0xF0180);      // ISP_DDR_FRAME_BUFFER_COUNT
	write_reg_dbg(base, 0x0, 0xF0184);         // ISP_DDR_FRAME_BUFFER_OVERFLOW_COUNT
	write_reg_dbg(base, 0x2, 0xF0188);         // ISP_RGB_DDR_FRAME_BUFFER_COUNT
	write_reg_dbg(base, 0x0, 0xF018C);         // ISP_RGB_DDR_FRAME_BUFFER_OVERFLOW_COUNT
	//write_reg_dbg(base, 0x2AAAAA, 0xF0190);    // MAX_BURST_LENGTH_SELECT


	write_reg_dbg(base, 0x10000000, 0xF01A0);         //  Y0_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x08001000, 0xF01A4);         // UV0_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x10001800, 0xF01A8);         //  Y1_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x08002800, 0xF01AC);         // UV1_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x10003000, 0xF01B0);         //  Y2_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x08004000, 0xF01B4);         // UV2_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x08004800, 0xF01B8);         //  Y3_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x04005000, 0xF01BC);         // UV3_AXI_BUF_CFG      //-> multizoom_dbg
	write_reg_dbg(base, 0x04005400, 0xF01C0);         // RGB_R_AXI_BUF_CFG    //-> multizoom_dbg
	write_reg_dbg(base, 0x04005800, 0xF01C4);         // RGB_G_AXI_BUF_CFG    //-> multizoom_dbg
	write_reg_dbg(base, 0x04005C00, 0xF01C8);         // RGB_B_AXI_BUF_CFG    //-> multizoom_dbg

	write_reg_dbg(base, 0x0, 0xF0200);         // ISP_Y_AXI_BUFFER_OVERFLOW_COUNT
	write_reg_dbg(base, 0x0, 0xF0204);         // ISP_UV_AXI_BUFFER_OVERFLOW_COUNT
	write_reg_dbg(base, 0x0, 0xF0208);         // ISP_RGB_AXI_BUFFER_OVERFLOW_COUNT

	//write_reg_dbg(base, 0x61AF4210, 0xF0210);  // WRITE_ARBITER_SLOT_CFG0
	//write_reg_dbg(base, 0x37E2C5D0, 0xF0214);  // WRITE_ARBITER_SLOT_CFG1
	//write_reg_dbg(base, 0xD20B41F0, 0xF0218);  // WRITE_ARBITER_SLOT_CFG2
	//write_reg_dbg(base, 0x3106A5E0, 0xF021C);  // WRITE_ARBITER_SLOT_CFG3
	//write_reg_dbg(base, 0x0, 0xF0220);         // WRITE_ARBITER_SLOT_CFG_READY
	//write_reg_dbg(base, 0x0, 0xF0224);         // WRITE_ARBITER_REQ_FORCE_UPDATE
	//write_reg_dbg(base, 0x33, 0xF0228);        // ARBITER_MASK_CFG
	//
	//write_reg_dbg(base, 0x8C8088, 0xF0230);    // READ_ARBITER_SLOT_CFG
	//write_reg_dbg(base, 0x0, 0xF0234);         // READ_ARBITER_SLOT_CFG_READY
	//
	//write_reg_dbg(base, 0x0, 0xF023C);         // READ_ARBITER_SLOT_CFG_SYNC


	write_reg_dbg(base, 0, 0xF0250);    // VIDEO_ISP_INT_EN  //-> multizoom_dbg
	write_reg_dbg(base, 0xFFFFFFFF, 0xF0254);         // VIDEO_ISP_INT_FLAG  //-> multizoom_dbg
	write_reg_dbg(base, 0x0, 0xF0258);         // VIDEO_AXI_INT_EN
	write_reg_dbg(base, 0x0, 0xF025C);         // VIDEO_AXI_INT_FLAG
	write_reg_dbg(base, 0xF8, 0xF0260);        // VIDEO_RGB_INT_EN
	write_reg_dbg(base, 0x0, 0xF0264);         // VIDEO_RGB_INT_FLAG


	write_reg_dbg(base, 0x1, 0xF0290);         // 3A_BUFFER_OVERFLOW_CFG

	for (stream_idx = 0; stream_idx < stream_num - 1; stream_idx++) {
		write_reg_dbg(base,  io->out_y_size[stream_idx], 0xF02A0 + 8 * stream_idx); // FRAME_LEN_Y0     ->0x384000
		write_reg_dbg(base, io->out_uv_size[stream_idx], 0xF02A4 + 8 * stream_idx); // FRAME_LEN_UV0    ->0x1C2000
	}
	write_reg_dbg(base,   io->out_rgb_size, 0xF02C0); // FRAME_LEN_R      ->0x2D000
	write_reg_dbg(base,   io->out_rgb_size, 0xF02C4); // FRAME_LEN_G      ->0x2D000
	write_reg_dbg(base,   io->out_rgb_size, 0xF02C8); // FRAME_LEN_B      ->0x2D000

	write_reg_dbg(base, 0x0, 0xF02D0);         // ISP_Y_DDR_FRAME_BUFFER_LEN_OVERFLOW_COUNT
	write_reg_dbg(base, 0x0, 0xF02D4);         // ISP_UV_DDR_FRAME_BUFFER_LEN_OVERFLOW_COUNT
	write_reg_dbg(base, 0x0, 0xF02D8);         // ISP_RGB_DDR_FRAME_BUFFER_LEN_OVERFLOW_COUNT
	write_reg_dbg(base, 0x0, 0xF02DC);         // LOW_LATENCY_CFG
	write_reg_dbg(base, 0x0, 0xF02E0);         // RING_MODE_CFG
	write_reg_dbg(base, 0x8, 0xF02E4);         // RING_END_DELAY_NUM
	write_reg_dbg(base, 0x0, 0xF02E8);         // RING_BUF_ADDR_Y
	write_reg_dbg(base, 0x0, 0xF02EC);         // RING_BUF_io->out_uv_addr (Ring Buffer Addrass UV, 0xB8AF_02EC)
	write_reg_dbg(base, 0x0, 0xF02F0);         // RING_BUF_LEN_Y (Ring Buffer Lenth Y, 0xB8AF_02F0)
	write_reg_dbg(base, 0x0, 0xF02F4);         // RING_BUF_LEN_UV (Ring Buffer Lenth UV, 0xB8AF_02F4)
	write_reg_dbg(base, 0x0, 0xF02F8);         // RING_BUF_OVFL_Y (Ring Buffer Overflow Y, 0xB8AF_02F8)
	write_reg_dbg(base, 0x0, 0xF02FC);         // RING_BUF_OVFL_UV (Ring Buffer Overflow UV, 0xB8AF_02FC)
	write_reg_dbg(base, 0x0, 0xF0300);         // VIDEO_IN_READ_STREAM_YUV (Video in AXI read FSM, 0xB8AF_0300)
	write_reg_dbg(base, 0x0, 0xF0304);         // VIDEO_IN_READ_STREAM_MTD (Video in AXI read FSM, 0xB8AF_0304)
	write_reg_dbg(base, 0x0, 0xF0308);         // VIDEO_IN_WRITE_STREAM_ISP01 (Video in AXI write FSM, 0xB8AF_0308)
	write_reg_dbg(base, 0x0, 0xF030C);         // VIDEO_IN_WRITE_STREAM_ISP23 (Video in AXI write FSM, 0xB8AF_030C)
	write_reg_dbg(base, 0x0, 0xF0310);         // VIDEO_IN_WRITE_STREAM_3A (Video in AXI write FSM, 0xB8AF_0310)
	write_reg_dbg(base, 0x0, 0xF0314);         // VIDEO_IN_WRITE_STREAM_MTD (Video in AXI write FSM, 0xB8AF_0314)
	write_reg_dbg(base, 0x0, 0xF0318);         // VIDEO_IN_WRITE_STREAM_RGB (Video in AXI write FSM, 0xB8AF_0318)
	//write_reg_dbg(base, 0x0        , 0xF031C); // 誤填
	write_reg_dbg(base, 0x1234567, 0xF0320);   // VIDEO_AR_SLOT_CTRL0 (video ar slot ctrl, 0xB8AF_0320)
	write_reg_dbg(base, 0x1234567, 0xF0324);   // VIDEO_AR_SLOT_CTRL1 (video ar slot ctrl, 0xB8AF_0324)
	write_reg_dbg(base, 0x1234567, 0xF0328);   // VIDEO_AW_SLOT_CTRL0 (video aw slot ctrl, 0xB8AF_0328)
	write_reg_dbg(base, 0x1234567, 0xF032C);   // VIDEO_AW_SLOT_CTRL1 (video aw slot ctrl, 0xB8AF_032C)
	write_reg_dbg(base, 0xC0300, 0xF0330);     //
	write_reg_dbg(base, 0x1E0, 0xF0334);       //


	write_reg_dbg(base, 0x0, 0xF03FC);         // VIDEO_AMBA_DUMMY (Video AMBA Dummy, 0xB8AF_03FC)
	return;
}

#define min(x, y) ((x) < (y) ? (x) : (y))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

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

static int rtscam_zoom_set_zoom_filter(struct isp_io *io, uint32_t scale)
{
	unsigned long i;
	uint32_t offset;
	void *base = io->base;
	const uint8_t *filter = get_zoom_filter(scale);

	for (i = 0, offset = ZOOM_CH0_COEF0; i < ZOOM_FILTER_COEF_NUM;
		 i += 4, offset += 4) {
		uint32_t filter_value;

		filter_value = (filter[i] | filter[i + 1] << 8 | filter[i + 2] << 16 |
						filter[i + 3] << 24);
		write_reg(base, filter_value, offset);
	}

	return 0;
}
#if 0

int config_singlezoom(struct isp_io *io)
{
	void *base = io->base;
	uint16_t src_w, src_h;
	uint16_t dest_w, dest_h;
	uint32_t scale_h, scale_v;
	uint32_t crop_start_x, crop_start_y;

	printf("ZOOM_SINGLE Enabled(io->zoom_en=%d)\n\r", io->zoom_en);
	crop_start_x = 0;
	crop_start_y = 0;
	src_w = io->crop_w;
	src_h = io->crop_h;
	//if (src_w + crop_start_x > width || src_h + crop_start_y > height)
	//    return RTS_RETURN(RTS_E_INVALID_ARG);
	dest_w = io->zoom_w[0];
	dest_h = io->zoom_h[0];

	scale_h = ((src_w << 10) / dest_w);
	scale_v = ((src_h << 10) / dest_h);
	if (scale_h > 0x1ffff || scale_v > 0x1ffff) {
		return RTS_RETURN(RTS_E_INVALID_ARG);
	}

	write_reg_dbg(base, 1, ZOOM_SYS_CONTROL);
	write_reg_dbg(base, 0x101, ZOOM_SYS_SPEED_CTRL);
	write_reg_dbg(base, 0x4, ZOOM_SYS_LAST_BREAK);

	write_reg_dbg(base, 0xf, VIDEO_ISP_NV12_SELECT);
	//if (zoom->out_fmt == FMT_NV12)
	write_reg_dbg(base, 1, ZOOM_SYS_IMAGE_MODE_SEL);
	//else
	//    write_reg(base, 1, ZOOM_SYS_IMAGE_MODE_SEL);
	write_reg_dbg(base, 1, ZOOM_SYS_IMAGE_AI_CHAN_SEL);

	write_reg_dbg(base, (crop_start_y << 16 | crop_start_x), ZOOM_CH0_CROP_START);
	write_reg_dbg(base, (dest_w | dest_h << 16), ZOOM_CH0_OUTPUT_SIZE);
	write_reg_dbg(base, scale_h, ZOOM_CH0_STEP_H);
	write_reg_dbg(base, scale_v, ZOOM_CH0_STEP_V);
	rtscam_zoom_set_zoom_filter(io, min(scale_h, scale_v));
	write_reg_dbg(base, 1, ZOOM_CH0_REG_LOAD);
	write_reg_dbg(base, 1, ZOOM_CH0_CTRL);




	// VIDEO Setting
	printf("%d %d %d %d \n\r", io->out_y_addr[0][0], io->out_uv_addr[0][0], io->out_y_size[0], io->out_uv_size[0]);
	write_reg(base, io->out_y_addr[0][0], VIDEO_Y0_FRAME_ADDR0);
	write_reg(base, io->out_uv_addr[0][0], VIDEO_UV0_FRAME_ADDR0);
	write_reg(base, io->out_y_size[0], VIDEO_FRAME_LEN_Y0);
	write_reg(base, io->out_uv_size[0], VIDEO_FRAME_LEN_UV0);
	write_reg(base, 0x20000000, VIDEO_Y0_AXI_BUF_CFG); //Fixed blank data: 0x18000000 -> 0x20000000
	write_reg(base, 0x10002000, VIDEO_UV0_AXI_BUF_CFG);
	write_reg(base, 1, VIDEO_DDR_FRAME_BUFFER_COUNT);

	//ISP Stream 0 Output YUV422 InterleavedSelect:
	//0: YUV420/YUV422 Semi-plannar,
	//1: YUV422 Interleaved
	if (io->zoomout_nv16 > 1) {
		write_reg_dbg(base, 1, VIDEO_YUV422_INTERLEAVED_SEL);
	} else {
		write_reg_dbg(base, 0, VIDEO_YUV422_INTERLEAVED_SEL);
	}
	write_reg(base, 1, VIDEO_STREAM0_ENABLE);
	write_reg(base, 0, VIDEO_AXI_INT_ENABLE);
	write_reg(base, 0x3000f, VIDEO_AXI_INT_FLAG);
	write_reg(base, 0x30, VIDEO_STREAM0_BUFFER_STATUS);
	return RTS_OK;
}
#else
// add zoom 2024.0925
int config_singlezoom(struct isp_io *io)
{
	void *base = io->base;
	unsigned int bk_w, bk_h;
	unsigned int step_h, step_v;

	if (!io->zoom_en) {
		return 0;
	}

	printf("ZOOM_SINGLE Enabled(io->zoom_en=%d)\n\r", io->zoom_en);
	// init zoom
	write_reg_dbg(base, 0x1, R_SYS_ZOOM_CONTROL1); //(SYS_ZOOM_BASE_ADDR + 0x00000000)    //0xb8ad8000
	write_reg_dbg(base, 0x101, R_SYS_ZOOM_SPEED_CTRL); //(SYS_ZOOM_BASE_ADDR + 0x00000004)    //0xb8ad8004
	write_reg_dbg(base, 0x4, R_SYS_ZOOM_LAST_BREAK); //(SYS_ZOOM_BASE_ADDR + 0x00000008)    //0xb8ad8008

	int reg_zoom_size = (io->zoom_h[0] << 16) + io->zoom_w[0];
	write_reg_mask_dbg(base, reg_zoom_size, 0xd8c08, 0xffffffff); // 798->5F0: 1944->1520 780x438 original: 0x07980a80 1080P:0x04380780
	write_reg_mask_dbg(base, 0x01010000, 0xd8c10, 0xffffffff); // ISP_ZOOM_UP_COEF
	write_reg_mask_dbg(base, 0x0e080502, 0xd8c14, 0xffffffff);
	write_reg_mask_dbg(base, 0x35291e15, 0xd8c18, 0xffffffff);
	write_reg_mask_dbg(base, 0x6b5e5042, 0xd8c1c, 0xffffffff);
	write_reg_mask_dbg(base, 0x86817c72, 0xd8c20, 0xffffffff);

	// Config step
	bk_w = read_reg(base, R_ISP_ZOOM_UP_OUTPUT_SIZE) & BK_FRAME_WIDTH_MASK; //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000008)    //0xb8ad8c08
	bk_h = read_reg(base, R_ISP_ZOOM_UP_OUTPUT_SIZE) & BK_FRAME_HEIGHT;
	bk_h >>= BK_FRAME_HEIGHT_OFFSET;

	step_h = (float) io->crop_w  / bk_w * 1024;
	step_v = (float) io->crop_h / bk_h * 1024;

	write_reg_dbg(base, step_h, R_ISP_ZOOM_UP_STEP_H); //(ISP_ZOOM_FHD_BASE_ADDR + 0x0000004c)    //0xb8ad8c4c
	write_reg_dbg(base, step_v, R_ISP_ZOOM_UP_STEP_V); //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000048)    //0xb8ad8c48

	write_reg_dbg(base, 1, R_ISP_ZOOM_UP_CTRL); //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000000)    //0xb8ad8c00
	write_reg_dbg(base, 1, R_ISP_ZOOM_UP_REG_LOAD); //(ISP_ZOOM_FHD_BASE_ADDR + 0x00000024)    //0xb8ad8c24

	// wait zoom trig
	write_reg_mask_dbg(base, 0x8, R_SYS_ZOOM_CONTROL1, ZOOM_DOWN_REG_LATCH_MASK);

//1:422 0:420
	if (io->zoomout_nv16 > 0) {
		write_reg_dbg(base, 1, R_SYS_ZOOM_IMAGE_MODE_SEL); //(SYS_ZOOM_BASE_ADDR + 0x00000018)    //0xb8ad8018
	} else {
		write_reg_dbg(base, 0, R_SYS_ZOOM_IMAGE_MODE_SEL); //(SYS_ZOOM_BASE_ADDR + 0x00000018)    //0xb8ad8018
	}
	//write_reg_dbg(base, 0xf, 0xF0070);
	//write_reg_dbg(base, 1, 0xD8020);



	// VIDEO Setting
	printf("%d %d %d %d \n\r", io->out_y_addr[0][0], io->out_uv_addr[0][0], io->out_y_size[0], io->out_uv_size[0]);
	write_reg(base, io->out_y_addr[0][0], VIDEO_Y0_FRAME_ADDR0);
	write_reg(base, io->out_uv_addr[0][0], VIDEO_UV0_FRAME_ADDR0);
	write_reg(base, io->out_y_size[0], VIDEO_FRAME_LEN_Y0);
	write_reg(base, io->out_uv_size[0], VIDEO_FRAME_LEN_UV0);
	write_reg(base, 0x20000000, VIDEO_Y0_AXI_BUF_CFG); //Fixed blank data: 0x18000000 -> 0x20000000
	write_reg(base, 0x10002000, VIDEO_UV0_AXI_BUF_CFG);
	write_reg(base, 1, VIDEO_DDR_FRAME_BUFFER_COUNT);

	//ISP Stream 0 Output YUV422 InterleavedSelect:
	//0: YUV420/YUV422 Semi-plannar,
	//1: YUV422 Interleaved
	if (io->zoomout_nv16 > 1) {
		write_reg_dbg(base, 1, VIDEO_YUV422_INTERLEAVED_SEL);
	} else {
		write_reg_dbg(base, 0, VIDEO_YUV422_INTERLEAVED_SEL);
	}
	write_reg(base, 1, VIDEO_STREAM0_ENABLE);
	write_reg(base, 0, VIDEO_AXI_INT_ENABLE);
	write_reg(base, 0x3000f, VIDEO_AXI_INT_FLAG);
	write_reg(base, 0x30, VIDEO_STREAM0_BUFFER_STATUS);

	return 0;
}
// add zoom 2024.0925
#endif


int video_config(struct isp_io *io)
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


	printf("ZOOM Disabled\n\r");
	printf("%d %d %d %d \n\r", io->out_y_addr[0][0], io->out_uv_addr[0][0], io->out_y_size[0], io->out_uv_size[0]);
	/* config video in */
	write_reg(base, io->out_y_addr[0][0], VIDEO_Y0_FRAME_ADDR0);
	write_reg(base, io->out_uv_addr[0][0], VIDEO_UV0_FRAME_ADDR0);
	write_reg(base, io->out_y_size[0], VIDEO_FRAME_LEN_Y0);
	write_reg(base, io->out_uv_size[0], VIDEO_FRAME_LEN_UV0);
	write_reg(base, 0x20000000, VIDEO_Y0_AXI_BUF_CFG); //Fixed blank data: 0x18000000 -> 0x20000000
	write_reg(base, 0x10002000, VIDEO_UV0_AXI_BUF_CFG);
	write_reg(base, 1, VIDEO_DDR_FRAME_BUFFER_COUNT);

	//ISP Stream 0 Output YUV422 InterleavedSelect:
	//0: YUV420/YUV422 Semi-plannar,
	//1: YUV422 Interleaved
	if (io->zoomout_nv16 > 1) {
		write_reg_dbg(base, 1, VIDEO_YUV422_INTERLEAVED_SEL);
	} else {
		write_reg_dbg(base, 0, VIDEO_YUV422_INTERLEAVED_SEL);
	}
	write_reg(base, 1, VIDEO_STREAM0_ENABLE);
	write_reg(base, 0, VIDEO_AXI_INT_ENABLE);
	write_reg(base, 0x3000f, VIDEO_AXI_INT_FLAG);
	write_reg(base, 0x30, VIDEO_STREAM0_BUFFER_STATUS);
}


void dbg_path_config(struct isp_io *io)
{
	unsigned int mask = 0;
	void *base = io->base;
	printf("[%s] Config Debug Path Start: %d\r\n", __FUNCTION__, io->dbg_path);
	switch (io->dbg_path) {
	case RTS_RAW_SRC_DISABLE: //0
		write_reg(base, 0, SYS_CONTROL1);
		write_reg(base, 20 | 0x100, SYS_IMAGE_SEL1);
		break;
	case RTS_RAW_SRC_BLC_LONGEXP_B: //1
		write_reg(base, 0, SYS_CONTROL1);
		write_reg(base, 2 | 0x100, SYS_IMAGE_SEL1);
		break;
	case RTS_RAW_SRC_BLC_LONGEXP_A: //2
		write_reg_mask(base, 0x104, 0x4c, 0x11f);
		write_reg_mask(base, 0x80, 0x94, 0x8c);
		write_reg_mask(base, 0xac5dce, 0x4, 0x7ffffff);
		//mask  = (1 << 7);
		//write_reg(base, read_reg(base, 0x0094) | mask, 0x0094);
		//mask  = (1 << 8);
		//write_reg(base, read_reg(base, SYS_CONTROL1) | mask, SYS_CONTROL1);
		//write_reg(base, 4 | 0x100, SYS_IMAGE_SEL1);
		break;
	case RTS_RAW_SRC_DNS_LONGEXP_A: //3
		mask  = (1 << 7);
		write_reg(base, read_reg(base, 0x0094) | mask, 0x0094);
		mask  = (1 << 8);
		mask |= (1 << 12);
		mask |= (1 << 15);
		mask |= (1 << 21);
		write_reg(base, read_reg(base, SYS_CONTROL1) | mask, SYS_CONTROL1);
		write_reg(base, 0, 0x14080);
		write_reg(base, 4 | 0x100, SYS_IMAGE_SEL1);
		break;
	case RTS_RAW_SRC_DPC_LONGEXP_A: //4
		write_reg_mask(base, 0x104, 0x4c, 0x11f);
		write_reg_mask(base, 0x80, 0x94, 0x8c);
		write_reg_mask(base, 0xac5dce, 0x4, 0x7ffffff);
		//mask  = (1 << 7);
		//write_reg(base, read_reg(base, 0x0094) | mask, 0x0094);
		//mask  = (1 << 8);
		//mask |= (1 << 12);
		//write_reg(base, read_reg(base, SYS_CONTROL1) | mask, SYS_CONTROL1);
		//write_reg(base, 4 | 0x100, SYS_IMAGE_SEL1);
		break;
	case RTS_RAW_SRC_AE_B: //5
//		mask  = (1 << 7);
//		write_reg(base, read_reg(base, 0x0094) | mask, 0x0094);
//		mask  = (1 << 8);
//		mask |= (1 << 12);
//		mask |= (1 << 21);
//		write_reg(base, read_reg(base, SYS_CONTROL1) | mask, SYS_CONTROL1);
//		write_reg(base, 6 | 0x100, SYS_IMAGE_SEL1);
		write_reg_mask(base, 0x106, 0x4c, 0x11f);
		write_reg_mask(base, 0x80, 0x94, 0x8c);
		write_reg_mask(base, 0xac5dce, 0x4, 0x7ffffff);	//	0xac5dce:1010,1100,0101,1101,1100,1110
		//	0xacddce:1010,1100,1101,1101,1100,1110
		break;
	case RTS_RAW_SRC_R_CCM_B:
//		mask  = ~(1 << 1);
//		mask &= ~(1 << 5);
//		mask &= ~(1 << 11);
//		mask &= ~(1 << 25);
//		write_reg(base, read_reg(base, SYS_CONTROL1)&mask, SYS_CONTROL1);
//		write_reg(base, 15 | 0x100, SYS_IMAGE_SEL1);
		write_reg_mask(base, 0x10f, 0x4c, 0x11f);
		write_reg_mask(base, 0x0, 0x94, 0xc);
		write_reg_mask(base, 0x0, 0x4, 0x7ffffff);
		break;
	case RTS_RAW_SRC_G_CCM_B:
		mask  = ~(1 << 1);
		mask &= ~(1 << 5);
		mask &= ~(1 << 11);
		mask &= ~(1 << 25);
		write_reg(base, read_reg(base, SYS_CONTROL1)&mask, SYS_CONTROL1);
		write_reg(base, 15 | 0x100, SYS_IMAGE_SEL1);
		break;
	case RTS_RAW_SRC_B_CCM_B:
		mask  = ~(1 << 1);
		mask &= ~(1 << 5);
		mask &= ~(1 << 11);
		mask &= ~(1 << 25);
		write_reg(base, read_reg(base, SYS_CONTROL1)&mask, SYS_CONTROL1);
		write_reg(base, 15 | 0x100, SYS_IMAGE_SEL1);
		break;
	default:
		break;
	}
	printf("[%s] Config Debug Path Done: %d\r\n", __FUNCTION__, io->dbg_path);
}


void config_ldc_reg(struct isp_io *io)
{
	if (!io->ldc_en) {
		printf("LDC_EN Disable\n\r");
		return;
	}

	printf("LDC_EN Enable\n\r");
	void *base = io->base;
	//int reg[10] = {0x000ad375, 0x00024468, 0x0000449c, 0xffffe780, 0x0005901e, 0xffff98e5, 0x05ff05ff, 0x0023f401, 0x0023f401, 0x00000001};
	//int reg[10] = {0x000ad375, 0x0002044b, 0x0000449c, 0xffffe780, 0x0005901e, 0xffff98e5, 0x05ff05ff, 0x0023f401, 0x0023f401, 0x00000001};
	//int reg[10] = {0x000ad375, 0x0001c5a7, 0x0000449c, 0xffffe780, 0x0005901e, 0xffff98e5, 0x05ff05ff, 0x0023f401, 0x0023f401, 0x00000001};
	//int reg[10] = {0x000ad375, 0x0001886f, 0x0000449c, 0xffffe780, 0x0005901e, 0xffff98e5, 0x05ff05ff, 0x0023f401, 0x0023f401, 0x00000001};
	//int reg[10] = {0x000ad375, 0x00014c98, 0x0000449c, 0xffffe780, 0x0005901e, 0xffff98e5, 0x05ff05ff, 0x0023f401, 0x0023f401, 0x00000001};
	int reg[10] = {0x000ad375, 0x00011215, 0x0000449c, 0xffffe780, 0x0005901e, 0xffff98e5, 0x05ff05ff, 0x0023f401, 0x0023f401, 0x00000001};
	int value0x4 = read_reg(base, SYS_CONTROL1) | (1 << 9);

	read_reg_dbg(base, 0x0064);
	//write_reg_dbg(base, 1, 0x5000);
	write_reg_dbg(base, value0x4, SYS_CONTROL1);
	write_reg_dbg(base, 1, 0x5000);

	write_reg_dbg(base, reg[0], 0x5010);
	write_reg_dbg(base, reg[1], 0x5014);
	write_reg_dbg(base, reg[2], 0x5018);
	write_reg_dbg(base, reg[3], 0x501C);
	write_reg_dbg(base, reg[4], 0x5020);
	write_reg_dbg(base, reg[5], 0x5024);
	write_reg_dbg(base, reg[6], 0x5028);
	write_reg_dbg(base, reg[7], 0x502C);
	write_reg_dbg(base, reg[8], 0x5030);
	write_reg_dbg(base, reg[9], 0x5034);
}

void config_mask(struct isp_io *io)
{
	if (!io->mask_en) {
		printf("MASK_EN Disable\n\r");
		return;
	}

	printf("MASK_EN Enable\n\r");
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

void config_security(struct isp_io *io)
{
	void *base;

	isp_io_base = (void *)0x60000000;
	base = io->base;
	//printf("0x10c540: 0x%lX \n\r", read_reg(base, 0x10c540));
	//printf("0x10c540: 0x%lX \n\r", read_reg(base, 0x10c540));
	//write_reg(base, 0x7ff, 0x10c540);
	//write_reg(base, DDR_START, 0x10c500);
	//write_reg(base, 0x6f030000, 0x10c504);
	//printf("0x10c540: 0x%lX \n\r", read_reg(base, 0x10c540));
	//printf("0x10c540: 0x%lX \n\r", read_reg(base, 0x10c540));
	//printf("0x10c500: 0x%lX \n\r", read_reg(base, 0x10c500));
	//printf("0x10c500: 0x%lX \n\r", read_reg(base, 0x10c500));
	//printf("0x10c504: 0x%lX \n\r", read_reg(base, 0x10c504));
	//printf("0x10c504: 0x%lX \n\r", read_reg(base, 0x10c504));
	write_reg(base, 0x6f000000,  0x10c500);
	write_reg(base, 0x6f00ffff,  0x10c504);
	write_reg(base, 0x6f010000,  0x10c508);
	write_reg(base, 0x6f01ffff,  0x10c50c);
	write_reg(base, 0x6f020000,  0x10c510);
	write_reg(base, 0x6f02ffff,  0x10c514);
	write_reg(base, 0x6f030000,  0x10c518);
	write_reg(base, 0x6f03ffff,  0x10c51c);
	write_reg(base, 0x6f040000,  0x10c520);
	write_reg(base, 0x6f04ffff,  0x10c524);
	write_reg(base, 0x6f050000,  0x10c528);
	write_reg(base, 0x6f05ffff,  0x10c52c);
	write_reg(base, 0x6f060000,  0x10c530);
	write_reg(base, 0x6f06ffff,  0x10c534);
	write_reg(base, 0x6f070000,  0x10c538);
	write_reg(base, 0x6f07ffff,  0x10c53c);
	write_reg(base, 0xff,  0x10c540);
}

