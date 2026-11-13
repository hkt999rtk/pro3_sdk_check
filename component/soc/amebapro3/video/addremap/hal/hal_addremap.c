#include "ameba_soc.h"
#include "hal_addremap.h"

#include "sysreg_addremap_isp.h"
#include "sysreg_addremap_enc.h"


//extern int hdr_remap_ddr;



void REMAP_HWInit(void)
{

//	Set 0x602501A8[30] =1  //enable clk
//	Set 0x602501A8[31] =1 //enable clk
//	Set 0x602501BC[22] =1 // release reset
//	Set 0x602501BC[17] =1 // release reset
//  set 0x6025_01B8[31] = 1 //for reset control of remap address isp register file.
//  set 0x6025_01B8[30] = 1 //for reset control of remap address encoder register file.

	RCC_PeriphClockCmd(APBPeriph_NULL, APBPeriph_ADRM_ENC_CLOCK,
					   ENABLE);  // REG_SYS_0_CLK_CTRL(0x1A8)[30,31] - Clock enable SYS_ADRM_ISP_SCLKEN, 'SYS_ADRM_ENC_SCLKEN


	RCC_PeriphClockCmd(APBPeriph_ISP_ADDR_REMAP, APBPeriph_CLOCK_NULL, ENABLE);  //REG_SYS_2_FUNC_CTRL(0x1BC)[22] - ISP Address Map function enable
	RCC_PeriphClockCmd(APBPeriph_ENC_ADDR_REMAP, APBPeriph_CLOCK_NULL, ENABLE);  //REG_SYS_2_FUNC_CTRL(0x1BC)[17] - Encoder Address Map function enable


	//	set 0x6025_01B8[31] = 1 //for reset control of remap address isp register file.
	//	set 0x6025_01B8[30] = 1 //for reset control of remap address encoder register file.


}



u32 RMPISP_GetINT(void)
{
	return REMAPADR_ISP_BASE->INT_ENB;
}

void RMPISP_SetINT(u32 set_value)
{
	REMAPADR_ISP_BASE->INT_ENB = set_value;
}

u32 RMPENC_GetINT(void)
{
	return REMAPADR_ENCD_BASE->INT_ENB;
}

void RMPENC_SetINT(u32 set_value)
{
	REMAPADR_ENCD_BASE->INT_ENB = set_value;
}



int set_hdr_remap(uint32_t width, uint32_t height, uint32_t vhdr_length,
				  struct addremap_hdr *hdr_config) //uint8_t HDRburstID, uint8_t HdrNumerator, uint8_t HdrDenominator)
{

#define ISP_WID         0x3
#define RMP_DDR_WID_0   0x0
#define RMP_DDR_WID_1   0x1
#define RMP_DDR_WID_2   0x2
#define RMP_SRAM_WID_0  0x4
#define RMP_SRAM_WID_1  0x5
#define RMP_SRAM_WID_2	0x6
#define RMP_SRAM_HEADER	0x7
#define RMP_HDR_SRAM_W		0x31
#define RMP_HDR_SRAM_R		0x32
#define RMP_HDR_DRAM_RW		0x33
#define VIDEO_SRAM_SIZE 	196608  //192KB
//#define VIDEO_MEM_BASE		0x6f000000
//#define ISP_REG_BASE		0x60000000
//#define REMAP_ISP_REG_BASE	0x60100000





	uint32_t isp_burst_set = 0x2AAAAA;
	uint32_t burst_size = 256;

	//uint8_t HdrDenominator = 32;
	uint32_t vHDR_ring_buf;
	uint32_t HdrMaxBurst;
	uint32_t read_value;


	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x500, 0x6f000000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x504, 0x6f00ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x508, 0x6f010000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x50C, 0x6f01ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x510, 0x6f020000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x514, 0x6f02ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x518, 0x6f030000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x51C, 0x6f03ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x520, 0x6f040000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x524, 0x6f04ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x528, 0x6f050000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x52C, 0x6f05ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x530, 0x6f060000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x534, 0x6f06ffff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x538, 0x6f070000);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x53C, 0x6f07ffff);

	//HAL_WRITE32(RXI300_CORE_REG_BASE, 0x540, 0x7ff);
	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x540, 0xff);



	read_value = HAL_READ32(REMAP_ISP_REG_BASE, 0xC);
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0xC, read_value | 0x1);
	DiagPrintf("[HDR_ remap] feature 0x%08x %d %d \r\n", HAL_READ32(REMAP_ISP_REG_BASE, 0xC), width, height);



	switch (hdr_config->burstSize) {
	case 0:
		isp_burst_set = 0x0;
		burst_size = 64;
		break;
	case 1:
		isp_burst_set = 0x5;
		burst_size = 128;
		break;
	case 2:
		isp_burst_set = 0xA;
		burst_size = 256;
		break;
	case 3:
		isp_burst_set = 0xF;
		burst_size = 512;
		break;
	default:
		isp_burst_set = 0xA;
		burst_size = 256;
		break;
	}


	//HAL_WRITE32(ISP_REG_BASE, 0xF0190, isp_burst_set); //MAX_BURST_LENGTH_SELECT
	//DiagPrintf("ISP MAX_BURST_LENGTH_SELECT  0x%08x \r\n", HAL_READ32(ISP_REG_BASE, 0xF0190));

	read_value = HAL_READ32(ISP_REG_BASE, 0x1008); //VHDR_STREAM_CTRL
	write_reg(ISP_REG_BASE, (read_value & 0xFFFFFFF0) | isp_burst_set, 0x1008); //VHDR_STREAM_CTRL
	DiagPrintf("ISP VHDR_STREAM_CTRL  org 0x%08x set 0x%08x\r\n", read_value, HAL_READ32(ISP_REG_BASE, 0x1008));





#if 0
	if (zoom_w % burst_size) {
		if (HDRBurstID < 3) {
			write_reg(ISP_REG_BASE, (HDRBurstID + 4), 0xD8034); //STREAM_ALIGN_MODE
			DiagPrintf("[HDR_ remap] update output width  %d \r\n", (width / burst_size + 1) * burst_size);

		} else {
			DiagPrintf("[Err] isp can't align to 512 burst.\r\n");
		}
	}
	DiagPrintf("ISP STREAM_ALIGN_MODE  0x%08x \r\n", HAL_READ32(ISP_REG_BASE, 0xD8034));
#endif


	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x14, RMP_HDR_DRAM_RW | (RMP_HDR_SRAM_W << 16));
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x18, RMP_HDR_DRAM_RW);


	vHDR_ring_buf = vhdr_length;
	DiagPrintf("vHDR ring buf %d burst %d wrap %d \r\n", vhdr_length, burst_size, vhdr_length / burst_size);

	//HdrNumerator = HdrDenominator * VIDEO_SRAM_SIZE / vHDR_ring_buf;


	if (hdr_config->numerator > hdr_config->denominator) {
		hdr_config->numerator = hdr_config->denominator;
	}

	HdrMaxBurst = hdr_config->max_burst;


	if (HdrMaxBurst > 4096) {
		HdrMaxBurst = 4096;
	}

	u32 temp_addr;

	//if (hdr_remap_ddr) {
	//	temp_addr =malloc(VIDEO_SRAM_SIZE + 0x400);
	//	temp_addr = (temp_addr+0x3FF) & ~((u32)0x3FF);
	//} else {
	temp_addr = VIDEO_MEM_BASE;
	//}


	DiagPrintf("HDR remap: SRAM share %d/%d used %d maxburst %d  wrapback %d \r\n", hdr_config->numerator, hdr_config->denominator, HdrMaxBurst * burst_size,
			   HdrMaxBurst, HdrMaxBurst * hdr_config->denominator / hdr_config->numerator);

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x1C, hdr_config->numerator | (hdr_config->denominator << 6) | ((HdrMaxBurst - 1) << 12) | (hdr_config->burstSize << 24)) ;

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x24, temp_addr);

	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x28, ISP_WID | (RMP_DDR_WID_0 << 8) | (RMP_DDR_WID_1 << 16) | (RMP_DDR_WID_2 << 24)) ;  //SliceWrapDInID
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x2C, RMP_HDR_SRAM_R | (RMP_SRAM_WID_0 << 8) | (RMP_SRAM_WID_1 << 16) | (RMP_SRAM_WID_2 << 24));

	//HAL_WRITE32(REMAP_ISP_REG_BASE, 0x28, ISP_WID) ;  //SliceWrapDInID
	//HAL_WRITE32(REMAP_ISP_REG_BASE, 0x2C, RMP_HDR_SRAM_R);

	DiagPrintf("ISP remap interrupt status 0x%08x \r\n", HAL_READ32(REMAP_ISP_REG_BASE, 0x0));
	HAL_WRITE32(REMAP_ISP_REG_BASE, 0x0, HAL_READ32(REMAP_ISP_REG_BASE, 0x0));
	DiagPrintf("reset int ISP remap interrupt status 0x%08x \r\n", HAL_READ32(REMAP_ISP_REG_BASE, 0x0));

#if 0	//  Log remap reg  

	for (u32 check_addr = 0x0; check_addr <=  0xEC; check_addr += 4) {
		DiagPrintf("addr 0x%08x value 0x%08x  \n\r", REMAP_ISP_REG_BASE + check_addr, HAL_READ32(REMAP_ISP_REG_BASE, check_addr));
	}
#endif



	return 0;


}


