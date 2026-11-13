/* Runtime functions restored from the reference SDK's addremap_verify.c.
 * See ../SOURCE.md. Register programming is retained from the vendor source.
 */
#include "ameba_soc.h"
#include <stdbool.h>
#include <string.h>
#include "addremap_hal.h"

#define COMPRESS_HEADER_SIZE 32768
#define VSRAM_SIZE 196608
#define VSRAM_SIZE_CMPR 162816
#define HEADER_FLUSH_BUFFER 96
#define HEADER_SLOT 2
static u32 remap_stm_id;
static u32 slice_th;
static u32 hdr_vram;
static u32 parS_ratio = 5;
static volatile u32 isp_remap_sliceEOF_cnt;
static volatile u32 enc_remap_sliceEOF_cnt;

void ISP_addremap_IRQHandler(void)
{
	u32 event_value;
#ifndef __REMAP_REAL_HDR_SNR__
	event_value = HAL_READ32(REMAP_ISP_REG_BASE, 0x0);
#if 0
	if (event_value & 0x20) {
		if (xISPRemap_SliceTh_Sema != NULL) {
			rtos_sema_give(xISPRemap_SliceTh_Sema);
		}
	}
#endif
	//DiagPrintf("isp remap event 0x%08x tick %d \n\r", event_value, xTaskGetTickCount());

#ifdef _LOG_ON_AMG_FR_

	DiagPrintf("isp remap interrupt  0x%08x SLC_CNT 0x%08x RD %d WR %d \n\r", event_value, HAL_READ32(REMAP_ISP_REG_BASE, 0xD8), HAL_READ32(REMAP_ISP_REG_BASE,
			   0xB4), HAL_READ32(REMAP_ISP_REG_BASE, 0xB8));

#endif

	// Debug_RdBurst_Cnt only for HDR remap
	// Debug_WrBurst_Cnt only for slice remap

	//DiagPrintf("isp remap interrupt  0x%08x \n\r", event_value);

	if (event_value & 0x10) {
		isp_remap_sliceEOF_cnt++;
		//DiagPrintf("isp remap frame end \n\r");
	}
	if (event_value & 0xC7) {
		DiagPrintf("isp remap err 0x%08x \n\r", event_value);

	}
	if (event_value & 0x08) {
		//DiagPrintf("isp remap HDR EOF \n\r");

	}
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x0, event_value);
#endif

}

void ENC_addremap_IRQHandler(void)
{
	u32 event_value;

	event_value = HAL_READ32(REMAP_ENC_REG_BASE, 0x0);

#ifdef _LOG_ON_AMG_FR_
	DiagPrintf("encremap interrupt  0x%08x SLC_CNT 0x%08x RD %d WR %d \n\r", event_value, HAL_READ32(REMAP_ENC_REG_BASE, 0xD8), HAL_READ32(REMAP_ENC_REG_BASE,
			   0xB4), HAL_READ32(REMAP_ENC_REG_BASE, 0xB8));
#endif

	if (event_value & 0x10) {
		enc_remap_sliceEOF_cnt++;
	}
	if (event_value & 0xC7) {
		DiagPrintf("enc remap err interrupt 0x%08x \n\r", event_value);
	}
	HAL_WRITE32(REMAP_ENC_REG_BASE, 0x0, event_value);

}

#define ISP_WID         0x3
#define RMP_DDR_WID_0   0x0
#define RMP_DDR_WID_1   0x1
#define RMP_DDR_WID_2   0x2
#define RMP_SRAM_WID_0  0x4
#define RMP_SRAM_WID_1  0x5
#define RMP_SRAM_WID_2	0x6
#define RMP_SRAM_HEADER	0x7

//#define __TEMP_TEST_ Sync with DV

u32 isp_remap_preprocess(struct isp_remap_config *cfg, u32 width, u32 height, u32 enc_type, u32 wrap_num, u32 burst_id)
{

	u32 zm_w = width;
	u32 zm_h = height;

	//bool IspEncdEnable = 1;

	//u16 TotalSliceNum = 67;  // 0-based
	u8 SliceNumWrap = 3;  // 0-based
	u8 SliceBurstSize = 1;  //128B
	u8 Slice_Comp0_SliceHeight = 1; //16 line
	u16 Slice_Comp0_Stride = 14;   // 0-based
	bool CompEnable0 = 1;
	u8 Slice_Comp1_SliceHeight = 0; //8 line
	u16 Slice_Comp1_Stride = 14;//2;
	bool CompEnable1 = 1;
	u32 isp_burst_set = 0;
	u32 read_temp;
	u32 burst_size = 128;
	u32 comp0_SliceHeight = 16;
	u32 comp1_SliceHeight = 8;
	u32 real_slice_num = 0;
	u32 slice_height;
	u32 align_width = width;

	u32 read_value;
	u8 RTTEnb = 0;
	u8 PaREnb = 0;
	u8 CompEnb = 0;
	u8 SWrapEnb = 0;
	u32 Feature_En;
	u32 readvalue;

	Feature_En = ((cfg->PaREnb & 0x1) << 24) | ((cfg->RTTEnb & 0x1) << 16) | ((cfg->SWrapEnb & 0x1) << 8) | (cfg->HdrEnb & 0x1) ;

	readvalue = HAL_READ32(REMAP_ISP_REG_BASE, 0xC);
	if (readvalue == 0x1) {
		Feature_En = Feature_En | 0x1 ;
	}
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xC, Feature_En);
	DiagPrintf("isp_ remap feature 0x%08x \r\n", HAL_READ32(REMAP_ISP_REG_BASE, 0xC));

	PaREnb = cfg->PaREnb;
	RTTEnb = cfg->RTTEnb;
	SWrapEnb = cfg->SWrapEnb;
	CompEnb = cfg->CompEnb;

	/*
		read_value = HAL_READ32(REMAP_ISP_REG_BASE, 0xC);
		PaREnb = (read_value >> 24) & 0x1;
		RTTEnb = (read_value >> 16) & 0x1;
		SWrapEnb = (read_value >> 8) & 0x1;
		cfg->PaREnb = PaREnb;
		cfg->RTTEnb = RTTEnb;
		cfg->SWrapEnb = SWrapEnb;
	*/

	cfg->SliceNumWrap = wrap_num;
	cfg->SliceBurstID = burst_id;

	SliceNumWrap = wrap_num - 1;
	SliceBurstSize = burst_id;

	if (enc_type == 1) {
		Slice_Comp0_SliceHeight = 3;
		comp0_SliceHeight = 64;
	} else {
		Slice_Comp0_SliceHeight = 1;
		comp0_SliceHeight = 16;
	}

	cfg->SliceHeight = comp0_SliceHeight;

	Slice_Comp1_SliceHeight = Slice_Comp0_SliceHeight - 1;

#if 1
	switch (SliceBurstSize) {
	case 0:
		isp_burst_set = 0x0;
		burst_size = 64;
		break;
	case 1:
		isp_burst_set = 0x155555;
		burst_size = 128;
		break;
	case 2:
		isp_burst_set = 0x2AAAAA;
		burst_size = 256;
		break;
	case 3:
		isp_burst_set = 0x3FFFFF;
		burst_size = 512;
		break;
	default:
		isp_burst_set = 0x155555;
		burst_size = 128;
		break;
	}
	//HAL_WRITE32(ISP_REG_BASE, 0xF0190, isp_burst_set); //MAX_BURST_LENGTH_SELECT
	//DiagPrintf("set isp burst siz 0x%08x  \r\n", read_reg(ISP_REG_BASE, 0xF0190));

	if (zm_w % burst_size) {
		if (SliceBurstSize < 3) {
			//write_reg(ISP_REG_BASE, (SliceBurstSize + 4), 0xD8034); //STREAM_ALIGN_MODE
			Slice_Comp0_Stride = zm_w / burst_size + 1 - 1;
			Slice_Comp1_Stride = zm_w / burst_size + 1 - 1;
			align_width = (Slice_Comp0_Stride + 1) * burst_size;
			DiagPrintf("update output width  %d \r\n", align_width);

		} else {
			DiagPrintf("[Err] isp can't align to 512 burst.\r\n");
		}
	} else {
		Slice_Comp0_Stride = zm_w / burst_size - 1;
		Slice_Comp1_Stride = zm_w / burst_size - 1;
		align_width = zm_w;

	}
	cfg->AlignWidth = align_width;
	cfg->Slice_Stride = Slice_Comp0_Stride;

	//DiagPrintf("ISP MAX_BURST_LENGTH_SELECT  0x%08x \r\n", HAL_READ32(ISP_REG_BASE, 0xF0190));
	//DiagPrintf("ISP STREAM_ALIGN_MODE  0x%08x \r\n", HAL_READ32(ISP_REG_BASE, 0xD8034));
	DiagPrintf("Comp stride  %d %d  \r\n", Slice_Comp0_Stride, Slice_Comp1_Stride);
#endif

	comp0_SliceHeight = 8 * (1 << Slice_Comp0_SliceHeight);
	comp1_SliceHeight = 8 * (1 << Slice_Comp1_SliceHeight);

	if (zm_h % comp0_SliceHeight) {
		real_slice_num = zm_h / comp0_SliceHeight + 1 - 1;
		DiagPrintf("[Warn] isp output height can't be multiple of slice height, update total slice to %d \r\n", real_slice_num);
	} else {
		real_slice_num = zm_h / comp0_SliceHeight - 1;
	}

	cfg->Slice_num = real_slice_num;

	//if (real_slice_num != TotalSliceNum)  {
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x30, (SliceBurstSize & 0x3) | ((SliceNumWrap & 0x7) << 2) | ((real_slice_num & 0x1FF) << 5));

	//}

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x5C, 0);

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x3C, (CompEnable0 & 0x1) | ((Slice_Comp0_Stride & 0xFFF) << 1) | ((Slice_Comp0_SliceHeight & 0x3) << 13));

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x48, (CompEnable1 & 0x1) | ((Slice_Comp1_Stride & 0xFFF) << 1) | ((Slice_Comp1_SliceHeight & 0x3) << 13));

	return align_width;

}

int set_isp_remap(struct isp_remap_config *cfg, u32 zoom_w, u32 zoom_h)
{

	//u32 y_addr = io->out_y_addr;
	//u32 y_length = io->out_y_size;
	//u32 uv_addr = io->out_uv_addr;
	//u32 uv_length = io->out_uv_size;

	u32 zm_w = zoom_w;
	u32 zm_h = zoom_h;

	//bool IspEncdEnable = 1;
	bool SWrapEnb = 1;
	//u16 TotalSliceNum = 64;
	//u8 SliceNumWrap = 7;  // 0-based ??
	//u8 SliceBurstSize = 1;  //128B
	//u32 Slice_Comp0_DRAM_Saddr = y_addr;
	//u32 Slice_Comp0_DRAM_Eaddr = y_addr + y_length-1;  // y_length = 32768
	//u8 Slice_Comp0_SliceHeight = 1; //16 line
	//u16 Slice_Comp0_Stride = 14;   // 0-based
	bool CompEnable0 = 1;
	//u32 Slice_Comp1_DRAM_Saddr = uv_addr;
	//u32 Slice_Comp1_DRAM_Eaddr = uv_addr + uv_length-1;
	//u8 Slice_Comp1_SliceHeight = 0; //8 line
	//u16 Slice_Comp1_Stride = 14;//2;
	bool CompEnable1 = 1;
	//u32 Slice_th =64; // 0-based ??
	//u32 isp_burst_set = 0;
	u32 read_value;
	//u32 burst_size = 128;
	u8 RTTEnb = 0;
	u8 PaREnb = 0;
	u8 SramRatio = 0;
	u16 Sramsize_PaR = 0;
	u8 CompEnb = 0;
	u32 isp_burst_set = 0;
	u32 burst_size = 128;
	u32 C0_flush_flag = 0, C1_flush_flag = 0;
	int y_header_size, align_y_hd_size, align_uv_hd_size;
	u32 VSRAM_start = VIDEO_MEM_BASE;
	u32 VSRAM_valid_size = VSRAM_SIZE;
	u32 VSRAM_for_Y = 0;
	u32 cmpr_hdr_align = 32;

	PaREnb = cfg->PaREnb;
	RTTEnb = cfg->RTTEnb;
	SWrapEnb = cfg->SWrapEnb;
	CompEnb = cfg->CompEnb;

	if (slice_th == 0) {
		// mask SliceThreshold
#ifndef __REMAP_REAL_HDR_SNR__
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x4, 0x20); //INT_MASK
#else
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x4, 0x38); //INT_MASK
#endif
	} else {
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x5C, slice_th - 1);
	}

	//HAL_WRITE32(REMAP_ISP_REG_BASE, 0xC, read_value|(SWrapEnb << 8) ); //FUNC_ENB

	switch (cfg->SliceBurstID) {
	case 0:
		isp_burst_set = 0x0;
		burst_size = 64;
		break;
	case 1:
		isp_burst_set = 0x155555;
		burst_size = 128;
		break;
	case 2:
		isp_burst_set = 0x2AAAAA;
		burst_size = 256;
		break;
	case 3:
		isp_burst_set = 0x3FFFFF;
		burst_size = 512;
		break;
	default:
		isp_burst_set = 0x155555;
		burst_size = 128;
		break;
	}

	if (CompEnb && (burst_size != 256)) {
		DiagPrintf("[Err] burst %d, compress need 256 burst.\r\n", burst_size);
		return -1;
	}

	HAL_WRITE32(ISP_REG_BASE, 0xF0190, isp_burst_set); //MAX_BURST_LENGTH_SELECT

	if (zm_w % burst_size) {
		if (cfg->SliceBurstID < 3 && remap_stm_id == 0) {

			HAL_WRITE32(ISP_REG_BASE, 0xD8034, (cfg->SliceBurstID + 4) << (remap_stm_id * 8)); //STREAM_ALIGN_MODE
			DiagPrintf("update output width  %d \r\n", (zm_w / burst_size + 1) * burst_size);

		} else {
			DiagPrintf("[Err]SliceBurstID %d remap_stm_id %d \r\n", cfg->SliceBurstID, remap_stm_id);
		}
	}
	DiagPrintf("ISP MAX_BURST_LENGTH_SELECT  0x%08x \r\n", HAL_READ32(ISP_REG_BASE, 0xF0190));
	DiagPrintf("ISP STREAM_ALIGN_MODE  0x%08x \r\n", HAL_READ32(ISP_REG_BASE, 0xD8034));

	DiagPrintf("addr 0x%08x %d  uv 0x%08x %d \r\n", cfg->slice_wrap_component[0].dram_saddr,
			   cfg->slice_wrap_component[0].dram_eaddr - cfg->slice_wrap_component[0].dram_saddr + 1,
			   cfg->slice_wrap_component[1].dram_saddr, cfg->slice_wrap_component[1].dram_eaddr - cfg->slice_wrap_component[1].dram_saddr + 1);

	if (PaREnb) {

		if (hdr_vram) {
//#ifdef __CMPR_HDR_SRAM__

			if (CompEnb) {
				VSRAM_start = cfg->compress_component[0].headr_addr[0] + COMPRESS_HEADER_SIZE;
				VSRAM_valid_size = VSRAM_SIZE_CMPR;
			} else {
				VSRAM_start = VIDEO_MEM_BASE;
				VSRAM_valid_size = VSRAM_SIZE;
			}

		} else {
//#else
			VSRAM_start = VIDEO_MEM_BASE;
			VSRAM_valid_size = VSRAM_SIZE;
		}

//#endif

		VSRAM_for_Y = VSRAM_valid_size * 2 / 3;

		SramRatio = parS_ratio;

		u32 need_SRAM_y = (zm_w * cfg->SliceHeight * cfg->SliceNumWrap) * SramRatio / 16;

		if (need_SRAM_y > VSRAM_for_Y) {
			DiagPrintf("ISP remap: Sram is not enough for Ratio %d/16 , need %d but only %d \r\n", SramRatio, need_SRAM_y, VSRAM_for_Y);
			return -1;
		}

		DiagPrintf("ISP remap: Parial SRAM Ratio %d/16 , need %d reserved  %d \r\n", SramRatio, need_SRAM_y, VSRAM_for_Y);

		cfg->partial_sram_config.sram_ratio = SramRatio;

		cfg->partial_sram_config.sram_addr[0].start_addr = VSRAM_start;
		cfg->partial_sram_config.sram_addr[1].start_addr = VSRAM_start + VSRAM_for_Y;

		cfg->partial_sram_config.sram_size = VSRAM_valid_size;

		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x58, SramRatio & 0xF) ; //PARTIAL_SRAM_INFO

		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x60, cfg->partial_sram_config.sram_addr[0].start_addr) ;  //SLW_SRAM_CP0_SADDR

		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x68, cfg->partial_sram_config.sram_addr[1].start_addr) ;  //SLW_SRAM_CP1_SADDR

	}

	if (CompEnb) {

		y_header_size = cfg->AlignWidth * zoom_h / 256 * 4 / 8;

		align_y_hd_size = ((y_header_size + (cmpr_hdr_align - 1)) / cmpr_hdr_align) * cmpr_hdr_align ;
		align_uv_hd_size = ((y_header_size / 2 + (cmpr_hdr_align - 1)) / cmpr_hdr_align) * cmpr_hdr_align ;

		DiagPrintf("y header size %d aligned %d %d\r\n", y_header_size, align_y_hd_size, align_uv_hd_size);

		C0_flush_flag = ((y_header_size % 32) > 0) ? BIT(0) : 0;
		C1_flush_flag = (((y_header_size / 2) % 32) > 0) ? BIT(1) : 0;

		if (C0_flush_flag || C1_flush_flag) {
			DiagPrintf("need flush  header Y %d UV %d \r\n", C0_flush_flag, C1_flush_flag);
			HAL_WRITE32(REMAP_ISP_REG_BASE, 0xEC, C0_flush_flag | C1_flush_flag) ;  //CMPR_HEADER_FLUSH_ENB
		}

		//cfg->compress_component[0].headr_addr[0] = COMPRESS_HEADER_ADDR;
		cfg->compress_component[1].headr_addr[0] = cfg->compress_component[0].headr_addr[0] + (align_y_hd_size + HEADER_FLUSH_BUFFER) * HEADER_SLOT; // for flush space

		cfg->compress_component[0].headr_size[0] = align_y_hd_size;
		cfg->compress_component[1].headr_size[0] = align_uv_hd_size;

		cfg->compress_component[0].valid_headr_size[0] = y_header_size;
		cfg->compress_component[1].valid_headr_size[0] = y_header_size / 2;

		cfg->compress_component[0].headr_addr[1] = cfg->compress_component[0].headr_addr[0] + (align_y_hd_size + HEADER_FLUSH_BUFFER);
		cfg->compress_component[1].headr_addr[1] = cfg->compress_component[1].headr_addr[0] + (align_uv_hd_size + HEADER_FLUSH_BUFFER) ;  // for flush space

		cfg->compress_component[0].headr_size[1] = align_y_hd_size;
		cfg->compress_component[1].headr_size[1] = align_uv_hd_size;

		cfg->compress_component[0].valid_headr_size[1] = y_header_size;
		cfg->compress_component[1].valid_headr_size[1] = y_header_size / 2;

		memset((void *)cfg->compress_component[0].headr_addr[0], 'z', (align_y_hd_size + HEADER_FLUSH_BUFFER + align_uv_hd_size + HEADER_FLUSH_BUFFER)*HEADER_SLOT);

		DiagPrintf("HDR 0x%08x 0x%08x %d %d \r\n", cfg->compress_component[0].headr_addr[0], cfg->compress_component[1].headr_addr[0], align_y_hd_size,
				   align_uv_hd_size);
		DiagPrintf("HDR1 0x%08x 0x%08x %d %d \r\n", cfg->compress_component[0].headr_addr[1], cfg->compress_component[1].headr_addr[1], align_y_hd_size,
				   align_uv_hd_size);

		cfg->compress_component[0].compress_dram_addr.start_addr = cfg->slice_wrap_component[0].dram_saddr;
		cfg->compress_component[0].compress_dram_addr.end_addr = cfg->slice_wrap_component[0].dram_eaddr;

		cfg->compress_component[1].compress_dram_addr.start_addr = cfg->slice_wrap_component[1].dram_saddr;
		cfg->compress_component[1].compress_dram_addr.end_addr = cfg->slice_wrap_component[1].dram_eaddr;

		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x78, cfg->compress_component[0].compress_dram_addr.start_addr);
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x7C, cfg->compress_component[0].compress_dram_addr.end_addr);
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x80, cfg->compress_component[1].compress_dram_addr.start_addr);
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x84, cfg->compress_component[1].compress_dram_addr.end_addr);

		HAL_WRITE32(REMAP_ISP_REG_BASE, 0xA8, cfg->compress_component[0].headr_addr[0]);
		HAL_WRITE32(REMAP_ISP_REG_BASE, 0xAC, cfg->compress_component[1].headr_addr[0]);

		if (PaREnb) {

			cfg->compress_component[0].compress_sram_addr.start_addr = cfg->partial_sram_config.sram_addr[0].start_addr;
			cfg->compress_component[0].compress_sram_addr.end_addr = cfg->partial_sram_config.sram_addr[1].start_addr - 1;
			cfg->compress_component[1].compress_sram_addr.start_addr = cfg->partial_sram_config.sram_addr[1].start_addr;
			cfg->compress_component[1].compress_sram_addr.end_addr = cfg->partial_sram_config.sram_addr[0].start_addr + cfg->partial_sram_config.sram_size;

			HAL_WRITE32(REMAP_ISP_REG_BASE, 0x90, cfg->compress_component[0].compress_sram_addr.start_addr);
			HAL_WRITE32(REMAP_ISP_REG_BASE, 0x94, cfg->compress_component[0].compress_sram_addr.end_addr);
			HAL_WRITE32(REMAP_ISP_REG_BASE, 0x98, cfg->compress_component[1].compress_sram_addr.start_addr);
			HAL_WRITE32(REMAP_ISP_REG_BASE, 0x9C, cfg->compress_component[1].compress_sram_addr.end_addr);

		}

		HAL_WRITE32(REMAP_ISP_REG_BASE, 0x10, 0x1);

	}

	DiagPrintf("ISP remap SWrap %d RTT %d  PaR %d  Compress %d ratio %d \r\n", SWrapEnb, RTTEnb, PaREnb, CompEnb, SramRatio);

	//HAL_WRITE32(REMAP_ISP_REG_BASE, 0x8, IspEncdEnable);     //HANDSHAKE_CTRL

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x28, ISP_WID | (RMP_DDR_WID_0 << 8) | (RMP_DDR_WID_1 << 16) | (RMP_DDR_WID_2 << 24)) ;  //SliceWrapDInID
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x2C, 0x32 | (RMP_SRAM_WID_0 << 8) | (RMP_SRAM_WID_1 << 16) | (RMP_SRAM_WID_2 << 24))
	;   //AXI write ID for the ISP remap writing compression header to memory.

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x34, cfg->slice_wrap_component[0].dram_saddr);
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x38, cfg->slice_wrap_component[0].dram_eaddr);

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x40, cfg->slice_wrap_component[1].dram_saddr);
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x44, cfg->slice_wrap_component[1].dram_eaddr);

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xE0, cfg->slice_wrap_component[0].out_saddr_offset);
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xE4, cfg->slice_wrap_component[1].out_saddr_offset);

	DiagPrintf("ISP remap interrupt status 0x%08x \r\n", HAL_READ32(REMAP_ISP_REG_BASE, 0x0));
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x0, HAL_READ32(REMAP_ISP_REG_BASE, 0x0));
	DiagPrintf("reset int ISP remap interrupt status 0x%08x \r\n", HAL_READ32(REMAP_ISP_REG_BASE, 0x0));

	return 0;

}

void update_cmpr_hdr_addr(u32    y_hdr_addr, u32 uv_hdr_addr)
{
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xA8, y_hdr_addr);
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xAC, uv_hdr_addr);
}

void set_enc_remap(struct enc_remap_config *config,  u32 zoom_w, u32 zoom_h)
{

	u32 zm_w = zoom_w;
	u32 zm_h = zoom_h;

	bool CompEnable0 = 1;
	bool CompEnable1 = 1;
	u32 read_value;
	u8 RTTEnb = 0;
	u8 PaREnb = 0;
	u8 SramRatio = 0;
	u8 CompEnb = 0;
	u8 SWrapEnb = 0;
	u8 ParSWrapEnb = 0;
	u32 Feature_En;
	u8 Slice_Comp0_SliceHeight;
	u16 Slice_Comp0_Stride;
	u8 Slice_Comp1_SliceHeight; //8 line
	u16 Slice_Comp1_Stride;//2;

	// only IntBusErr
	HAL_WRITE32(REMAP_ENC_REG_BASE, 0x4, 0x7F); //INT_MASK

	Feature_En = ((config->PaREnb & 0x1) << 24) | ((config->RTTEnb & 0x1) << 16) | ((config->ParSWrapEnb & 0x1) << 9) | ((config->SWrapEnb & 0x1) << 8);
	//Feature_En = ((config->PaREnb&0x1)<<24) | ((config->RTTEnb&0x1)<<16) | ((config->ParSWrapEnb&0x1)<<9);  // Partial SRAM can't set

	HAL_WRITE32(REMAP_ENC_REG_BASE, 0xC, Feature_En);

	//if (config->PaREnb | config->RTTEnb | config->SWrapEnb) {
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x8, 0x1);    //current RTL didn't use this bit
	//}

	HAL_WRITE32(REMAP_ENC_REG_BASE, 0x8, 0x1);  // For Handshake module

	DiagPrintf("enc_remap feature 0x%08x \r\n", HAL_READ32(REMAP_ENC_REG_BASE, 0xC));
	SWrapEnb = config->SWrapEnb;
	PaREnb = config->PaREnb;
	RTTEnb = config->RTTEnb;
	ParSWrapEnb = config->ParSWrapEnb;
	CompEnb = config->CompEnb;

	if (config->SliceHeight == 64) {
		Slice_Comp0_SliceHeight = 3;
	} else {
		Slice_Comp0_SliceHeight = 1;
	}
	Slice_Comp1_SliceHeight = Slice_Comp0_SliceHeight - 1;

	Slice_Comp0_Stride = config->Slice_Stride;
	Slice_Comp1_Stride = config->Slice_Stride;

	if (RTTEnb || CompEnb) {
		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x30, (config->SliceBurstID & 0x3) | (((config->SliceNumWrap - 1) & 0x7) << 2) | ((config->Slice_num & 0x1FF) << 5));
	}

	if (PaREnb || RTTEnb || CompEnb) {

		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x3C, (CompEnable0 & 0x1) | ((Slice_Comp0_Stride & 0xFFF) << 1) | ((Slice_Comp0_SliceHeight & 0x3) << 13));

		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x48, (CompEnable1 & 0x1) | ((Slice_Comp1_Stride & 0xFFF) << 1) | ((Slice_Comp1_SliceHeight & 0x3) << 13));

		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x34, config->enc_remap_component[0].dram_saddr);
		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x38, config->enc_remap_component[0].dram_eaddr);
		//HAL_WRITE32(REMAP_ENC_REG_BASE, 0x3C,CompEnable0 & 0x1);

		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x40, config->enc_remap_component[1].dram_saddr);
		HAL_WRITE32(REMAP_ENC_REG_BASE, 0x44, config->enc_remap_component[1].dram_eaddr);
		//HAL_WRITE32(REMAP_ENC_REG_BASE, 0x48, CompEnable1 & 0x1);

		// DiagPrintf(" ENC remap slice_buff_size %d \r\n", slice_buff_size);

		if (PaREnb) {

			HAL_WRITE32(REMAP_ENC_REG_BASE, 0x58, config->partial_sram_config.sram_ratio & 0xF) ; //PARTIAL_SRAM_INFO

			HAL_WRITE32(REMAP_ENC_REG_BASE, 0x60, config->partial_sram_config.sram_addr[0].start_addr) ;  //SLW_SRAM_CP0_SADDR

			HAL_WRITE32(REMAP_ENC_REG_BASE, 0x68, config->partial_sram_config.sram_addr[1].start_addr) ;  //SLW_SRAM_CP1_SADDR
		}

	}

	DiagPrintf("ENC remap RTT %d  PaR %d  ratio %d \r\n", RTTEnb, PaREnb, config->partial_sram_config.sram_ratio);

	DiagPrintf("ENC remap interrupt status 0x%08x \r\n", HAL_READ32(REMAP_ENC_REG_BASE, 0x0));
	HAL_WRITE32(REMAP_ENC_REG_BASE, 0x0, HAL_READ32(REMAP_ENC_REG_BASE, 0x0));
	DiagPrintf("reset int ENC remap interrupt status 0x%08x \r\n", HAL_READ32(REMAP_ENC_REG_BASE, 0x0));

}

