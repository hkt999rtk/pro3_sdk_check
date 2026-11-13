
#include "FreeRTOS.h"
#include "ameba_soc.h"
#include "diag.h"
#include "portmacrocommon.h"
#include "rts_blit_regs.h"
#include "rts_zoom_regs.h"
#include "rts_ge_regs.h"
#include "dbg_zoom.h"
#include "rts_blit.h"
#include "ge_sys_api.h"
#include "ge_reshape_api.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * Software crop for 16-bit per pixel buffers (short).
 *
 * @param src Source buffer
 * @param src_w Source width in pixels
 * @param src_h Source height in pixels
 * @param dst Destination buffer
 * @param crop_x Crop start X
 * @param crop_y Crop start Y
 * @param crop_w Crop width
 * @param crop_h Crop height
 */
void software_crop(short *src, u32 src_w, u32 src_h, short *dst, u32 crop_x,
				   u32 crop_y, u32 crop_w, u32 crop_h)
{
	if (!src || !dst) {
		return;
	}

	// Boundary check
	if (crop_x + crop_w > src_w) {
		crop_w = src_w - crop_x;
	}
	if (crop_y + crop_h > src_h) {
		crop_h = src_h - crop_y;
	}

	for (u32 y = 0; y < crop_h; y++) {
		short *src_line = src + (crop_y + y) * src_w + crop_x;
		short *dst_line = dst + y * crop_w;
		memcpy(dst_line, src_line, crop_w * sizeof(short));
	}
}

/**
 * CRC32 calculation function
 * Standard CRC32 (used in Ethernet, ZIP, etc.)
 *
 * @param data Pointer to data buffer
 * @param length Length of data in bytes
 * @return CRC32 value
 */
u32 calculate_crc32(const u8 *data, u32 length)
{
	u32 crc = 0xFFFFFFFF;
	u32 polynomial = 0xEDB88320; /* Reversed polynomial */

	for (u32 i = 0; i < length; i++) {
		crc ^= data[i];
		for (u32 j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = (crc >> 1) ^ polynomial;
			} else {
				crc = crc >> 1;
			}
		}
	}

	return ~crc;
}

int ge_check_reg_w(u32 addr, u32 set_value)
{
	volatile u32 org_value, read_value;
	org_value = HAL_READ32(GE_REG_BASE, addr);
	//org_value = HAL_READ32(VIDEO_GE_BASE, addr);
	//org_value = HAL_READ32(VIDEO_GE_BASE, addr);

	HAL_WRITE32(GE_REG_BASE, addr, set_value);

	read_value = HAL_READ32(GE_REG_BASE, addr);
	//read_value = HAL_READ32(VIDEO_GE_BASE,addr);
	//read_value = HAL_READ32(VIDEO_GE_BASE,addr);

	HAL_WRITE32(GE_REG_BASE, addr, 0x0);
	if (read_value == set_value) {
		//DiagPrintf("set addr 0x%08x, result OK 0x%08x, original 0x%08x\n\r", VIDEO_GE_BASE+addr, set_value, org_value);
		return 0;
	} else if (read_value == 0x0 && set_value != 0x0) {
		DiagPrintf(
			"write but zero result. set addr 0x%08x with 0x%08x, result 0x%08x, original 0x%08x\n\r",
			GE_REG_BASE + addr, set_value, read_value, org_value);
		return -1;
	} else {
		DiagPrintf(
			"write result diff. set addr 0x%08x with 0x%08x, result 0x%08x, original 0x%08x\n\r",
			GE_REG_BASE + addr, set_value, read_value, org_value);
		return -1;
	}
}

int ge_check_reg_hole_read(u32 addr_offset, u32 check_value)
{
	volatile u32 reg_value;

	reg_value = HAL_READ32(GE_REG_BASE, addr_offset);
	//reg_value = HAL_READ32(VIDEO_GE_BASE,addr_offset);
	//reg_value = HAL_READ32(VIDEO_GE_BASE,addr_offset);

	if (reg_value != check_value) {
		DiagPrintf(
			"register hole 0x%08x read unexpected result 0x%08x \n\r",
			GE_REG_BASE + addr_offset, reg_value);
		return -1;
	} else {
		//DiagPrintf("check register hole 0x%08x read OK  0x%08x \n\r", VIDEO_GE_BASE+addr_offset, check_value);
		return 0;
	}
}

int ge_check_reg_hole_write(u32 addr_offset, u32 write_value, u32 check_value)
{
	volatile u32 reg_value, org_value;

	org_value = HAL_READ32(GE_REG_BASE, addr_offset);
	//org_value = HAL_READ32(VIDEO_GE_BASE,addr_offset);
	// org_value = HAL_READ32(VIDEO_GE_BASE,addr_offset);
	HAL_WRITE32(GE_REG_BASE, addr_offset, write_value);
	reg_value = HAL_READ32(GE_REG_BASE, addr_offset);
	//reg_value = HAL_READ32(VIDEO_GE_BASE,addr_offset);
	//reg_value = HAL_READ32(VIDEO_GE_BASE,addr_offset);
	HAL_WRITE32(GE_REG_BASE, addr_offset, 0x0);
	if (reg_value != check_value) {
		DiagPrintf(
			"register hole 0x%08x write unexpected result 0x%08x org 0x%08x\n\r",
			GE_REG_BASE + addr_offset, reg_value, org_value);
		return -1;
	} else {
		//DiagPrintf("check register hole 0x%08x write OK 0x%08x \n\r", VIDEO_GE_BASE+addr_offset, check_value);
		return 0;
	}
}
#if 0
#define REG_HOLE_NUM_BLIT 8
static u32 reg_hole_addr_blit[REG_HOLE_NUM_BLIT] = {
	BLIT_TRANS_LENGTH_SEL + 4,
	BLIT_RD_DST_DBG_SIG - 4,
	BLIT_PURE_COLOR_WR_MSB_EN + 4,
	BLIT_LUT_MEM - 4,
	BLIT_LUT_MEM_LAST + 4,
	BLIT_LUTRAW_MEM - 4,
	BLIT_LUTRAW_MEM_LAST + 4,
	0x0FFC
};
#else
//timing error case
#define REG_HOLE_NUM_BLIT 14
#define GE_ZOOM_OFFSET0 0x1400

static u32 reg_hole_addr_blit[REG_HOLE_NUM_BLIT] = { BLIT_TRANS_LENGTH_SEL + 4,
													 BLIT_RD_DST_DBG_SIG - 4,
													 BLIT_PURE_COLOR_WR_MSB_EN +
													 4,
													 BLIT_LUT_MEM - 4,
													 BLIT_LUT_MEM_LAST + 4,
													 BLIT_LUTRAW_MEM - 4,
													 BLIT_LUTRAW_MEM_LAST + 4,
													 ZOOM_DST_INDEX_Y + 4,
													 ZOOM_RD_DBG_SIG0 - 4,
													 ZOOM_FUNC_DBG_SIG + 4,
													 GE_BLIT_CLK_EN - 4,
													 GE_AXI_ERR_INT + 4,
													 GE_ZOOM_OFFSET0 - 4,
													 0x1FFC
												   };
#endif



#define REG_HOLE_NUM_ZOOM 6
static u32 reg_hole_addr_zoom[REG_HOLE_NUM_ZOOM] = {
	ZOOM_DST_INDEX_Y + 4, ZOOM_RD_DBG_SIG0 - 4, ZOOM_FUNC_DBG_SIG + 4,
	GE_BLIT_CLK_EN - 4,   GE_AXI_ERR_INT + 4,   0x1BFC
};

#define REG_HOLE_NUM_GE 4
static u32 reg_hole_addr_ge[REG_HOLE_NUM_GE] = { 0x1000, GE_ZOOM_OFFSET0 - 4,
												 0x1CFC, 0x1FFC
											   };

/*******************************************/
/*			Add command to table           */
/*******************************************/
u32 CmdGETest0(IN u16 argc, IN u8 *argv[])
{
	volatile u32 reg_value;

	int ret_value = 0;
	int reg_check_pass = 0;
	u32 addr;
	char **restrict endptr;
	u32 check_addr;
	u32 default_value_nonzero = 0, default_value_zero = 0,
		reg_wr_check_cnt = 0, reg_wr_check_pass = 0;
	u32 reg_hole_pass = 0, reg_hole_check = 0;

	hal_ge_init();

	HAL_WRITE32(GE_REG_BASE, GE_BLIT_CLK_EN, BLIT_CLK_EN_BITS);
	HAL_WRITE32(GE_REG_BASE, ZOOM_CLK_EN, ZOOM_CLK_EN_BITS);

	DiagPrintf("ge test register command 1\n\rargc %d\n\r", argc);

	for (int i = 0; i < argc; i++) {
		DiagPrintf("argv[%d] = %s\n\r", i, argv[i]);
	}

	if (_strcmp((const char *)argv[0], "r") == 0) {
		if (argc >= 2) {
			addr = (u32)strtoul((const char *)argv[1], endptr, 16);
			DiagPrintf("0x%08x=0x%08x \n\r", addr,
					   HAL_READ32(addr, 0));
		} else {
			DiagPrintf("need register addr\n\r");
		}

	} else if (_strcmp((const char *)argv[0], "w") == 0) {
		if (argc >= 3) {
			addr = (u32)strtoul((const char *)argv[1], endptr, 16);
			reg_value =
				(u32)strtoul((const char *)argv[2], endptr, 16);
			HAL_WRITE32(addr, 0, reg_value);
			DiagPrintf("set_value=0x%08x read 0x%08x=0x%08x \n\r",
					   reg_value, addr, HAL_READ32(addr, 0));
		} else {
			DiagPrintf("need addr setvalue\n\r");
		}

	} else if (_strcmp((const char *)argv[0], "ge") == 0) {
		//check_addr = 0x1FFC;
		//reg_value = HAL_READ32(VIDEO_GE_BASE,check_addr);
		//DiagPrintf("addr 0x%08x default 0x%08x\n\r", VIDEO_GE_BASE+check_addr, reg_value);

		/* check bitblit registers */
		for (check_addr = BLIT_CTRL;
			 check_addr <= BLIT_PURE_COLOR_WR_MSB_EN; check_addr += 4) {
			reg_value = HAL_READ32(GE_REG_BASE, check_addr);
			if (reg_value != 0) {
				default_value_nonzero++;
				DiagPrintf(
					"addr 0x%08x default 0x%08x cnt %d \n\r",
					GE_REG_BASE + check_addr, reg_value,
					default_value_nonzero);

			} else {
				default_value_zero++;
			}
		}
		DiagPrintf("bitblit check default zero %d nonezero %d\n\r",
				   default_value_zero, default_value_nonzero);

		//ret_value = ge_check_reg_w(0x1000, 0xA5A5A5A5);

		reg_wr_check_cnt = 0;
		reg_wr_check_pass = 0;
		foWㄘ = BLIT_CTRL;
		check_addr <= BLIT_TRANS_LENGTH_SEL;
		check_addr += 4) {
			ret_value = ge_check_reg_w(check_addr, 0xA5A5A5A5);
			if (ret_value == 0) {
				reg_wr_check_pass++;
			}
			reg_wr_check_cnt++;
		}
		for (check_addr = BLIT_LUT_PURE_COLOR_CFG;
	check_addr <= BLIT_PURE_COLOR_WR_MSB_EN; check_addr += 4) {
		ret_value = ge_check_reg_w(check_addr, 0xA5A5A5A5);
			if (ret_value == 0) {
				reg_wr_check_pass++;
			}
			reg_wr_check_cnt++;
		}
		DiagPrintf("bitblit check reg wr %d pass %d\n\r",
				   reg_wr_check_cnt, reg_wr_check_pass);

		reg_wr_check_cnt = 0;
		reg_wr_check_pass = 0;
		for (check_addr = BLIT_LUT_MEM; check_addr <= BLIT_LUT_MEM_LAST;
	check_addr += 4) {
		ret_value = ge_check_reg_w(check_addr, 0xA5A5A5A5);
			if (ret_value == 0) {
				reg_wr_check_pass++;
			}
			reg_wr_check_cnt++;
		}
		DiagPrintf("bitblit check LUT_MEM wr %d pass %d\n\r",
				   reg_wr_check_cnt, reg_wr_check_pass);

		reg_wr_check_cnt = 0;
		reg_wr_check_pass = 0;
		for (check_addr = BLIT_LUTRAW_MEM;
	check_addr <= BLIT_LUTRAW_MEM_LAST; check_addr += 4) {
		ret_value = ge_check_reg_w(check_addr, 0xA5A5A5A5);
			if (ret_value == 0) {
				reg_wr_check_pass++;
			}
			reg_wr_check_cnt++;
		}
		DiagPrintf("bitblit check LUTRAW_MEM wr %d pass %d\n\r",
				   reg_wr_check_cnt, reg_wr_check_pass);

		/* check zoom registers */
		default_value_nonzero = 0;
		default_value_zero = 0;
		for (check_addr = ZOOM_CTRL; check_addr <= ZOOM_DST_INDEX_Y;
	check_addr += 4) {
		reg_value = HAL_READ32(GE_REG_BASE, check_addr);
			if (reg_value != 0) {
				default_value_nonzero++;
				DiagPrintf(
					"addr 0x%08x default 0x%08x cnt %d \n\r",
					GE_REG_BASE + check_addr, reg_value,
					default_value_nonzero);

			} else {
				default_value_zero++;
			}
		}
		DiagPrintf("ge_zoom check default zero %d nonezero %d\n\r",
				   default_value_zero, default_value_nonzero);

		default_value_nonzero = 0;
		default_value_zero = 0;
		for (check_addr = ZOOM_RD_DBG_SIG0;
	check_addr <= ZOOM_FUNC_DBG_SIG; check_addr += 4) {
		reg_value = HAL_READ32(GE_REG_BASE, check_addr);
			if (reg_value != 0) {
				default_value_nonzero++;
				DiagPrintf(
					"addr 0x%08x default 0x%08x cnt %d \n\r",
					GE_REG_BASE + check_addr, reg_value,
					default_value_nonzero);

			} else {
				default_value_zero++;
			}
		}
		DiagPrintf(
			"ge_zoom debug reg check default zero %d nonezero %d\n\r",
			default_value_zero, default_value_nonzero);

		reg_wr_check_cnt = 0;
		reg_wr_check_pass = 0;
		for (check_addr = ZOOM_CTRL; check_addr <= ZOOM_DST_INDEX_Y;
	check_addr += 4) {
		ret_value = ge_check_reg_w(check_addr, 0xA5A5A5A5);
			if (ret_value == 0) {
				reg_wr_check_pass++;
			}
			reg_wr_check_cnt++;
		}
		DiagPrintf("ge_zoom check reg wr %d pass %d\n\r",
				   reg_wr_check_cnt, reg_wr_check_pass);

		/* check ge registers */
		default_value_nonzero = 0;
		default_value_zero = 0;
		for (check_addr = GE_BLIT_CLK_EN; check_addr <= GE_AXI_ERR_INT;
	check_addr += 4) {
		reg_value = HAL_READ32(GE_REG_BASE, check_addr);
			if (reg_value != 0) {
				default_value_nonzero++;
				DiagPrintf(
					"addr 0x%08x default 0x%08x cnt %d \n\r",
					GE_REG_BASE + check_addr, reg_value,
					default_value_nonzero);

			} else {
				default_value_zero++;
			}
		}
		DiagPrintf("ge reg check default zero %d nonezero %d\n\r",
				   default_value_zero, default_value_nonzero);

		reg_wr_check_cnt = 0;
		reg_wr_check_pass = 0;
		for (check_addr = GE_DBG_SEL; check_addr <= GE_AXI_ERR_INT_CLR;
	check_addr += 4) {
		ret_value = ge_check_reg_w(check_addr, 0xA5A5A5A5);
			if (ret_value == 0) {
				reg_wr_check_pass++;
			}
			reg_wr_check_cnt++;
		}
		DiagPrintf("ge check reg wr %d pass %d\n\r", reg_wr_check_cnt,
				   reg_wr_check_pass);

		/* check regester hole */
		u32 hole_test_write = 0x5A5A5A5A;

		reg_hole_pass = 0;
		//u32 hole_def_value = 0xA5A5A5A5;
		u32 hole_def_value = 0xDEADBEEF;
		for (reg_hole_check = 0; reg_hole_check < REG_HOLE_NUM_BLIT;
	reg_hole_check++) {
		ret_value = ge_check_reg_hole_read(
						reg_hole_addr_blit[reg_hole_check],
						hole_def_value);
			if (ret_value == 0) {
				reg_hole_pass++;
			}
		}
		DiagPrintf(
			"ge check register hole read [blit region] pass %d/%d check value 0x%08x\n\r",
			reg_hole_pass, REG_HOLE_NUM_BLIT, hole_def_value);

		reg_hole_pass = 0;
		u32 test_write = 0x5A5A5A5A;
		for (reg_hole_check = 0; reg_hole_check < REG_HOLE_NUM_BLIT;
	reg_hole_check++) {
		ret_value = ge_check_reg_hole_write(
						reg_hole_addr_blit[reg_hole_check], test_write,
						hole_def_value);
			if (ret_value == 0) {
				reg_hole_pass++;
			}
		}
		DiagPrintf(
			"ge check register hole write [blit region] pass %d/%d check value 0x%08x\n\r",
			reg_hole_pass, REG_HOLE_NUM_BLIT, hole_def_value);

		reg_hole_pass = 0;
		//hole_def_value = 0x0;
		hole_def_value = 0xDEADBEEF;
		for (reg_hole_check = 0; reg_hole_check < REG_HOLE_NUM_ZOOM;
	reg_hole_check++) {
		ret_value = ge_check_reg_hole_read(
						reg_hole_addr_zoom[reg_hole_check],
						hole_def_value);
			if (ret_value == 0) {
				reg_hole_pass++;
			}
		}
		DiagPrintf(
			"ge check register hole read [zoom region] pass %d/%d check value 0x%08x\n\r",
			reg_hole_pass, REG_HOLE_NUM_ZOOM, hole_def_value);

		reg_hole_pass = 0;
		for (reg_hole_check = 0; reg_hole_check < REG_HOLE_NUM_ZOOM;
	reg_hole_check++) {
		ret_value = ge_check_reg_hole_write(
						reg_hole_addr_zoom[reg_hole_check], test_write,
						hole_def_value);
			if (ret_value == 0) {
				reg_hole_pass++;
			}
		}
		DiagPrintf(
			"ge check register hole write [zoom region] pass %d/%d check value 0x%08x\n\r",
			reg_hole_pass, REG_HOLE_NUM_ZOOM, hole_def_value);

		reg_hole_pass = 0;
		//hole_def_value = 0xdeadbeaf;
		hole_def_value = 0xDEADBEEF;
		for (reg_hole_check = 0; reg_hole_check < REG_HOLE_NUM_GE;
	reg_hole_check++) {
		ret_value = ge_check_reg_hole_read(
						reg_hole_addr_ge[reg_hole_check],
						hole_def_value);
			if (ret_value == 0) {
				reg_hole_pass++;
			}
		}
		DiagPrintf(
			"ge check register hole read [ge region] pass %d/%d check value 0x%08x\n\r",
			reg_hole_pass, REG_HOLE_NUM_GE, hole_def_value);

		reg_hole_pass = 0;
		for (reg_hole_check = 0; reg_hole_check < REG_HOLE_NUM_GE;
	reg_hole_check++) {
		ret_value = ge_check_reg_hole_write(
						reg_hole_addr_ge[reg_hole_check], test_write,
						hole_def_value);
			if (ret_value == 0) {
				reg_hole_pass++;
			}
		}
		DiagPrintf(
			"ge check register hole write [ge region] pass %d/%d check value 0x%08x\n\r",
			reg_hole_pass, REG_HOLE_NUM_GE, hole_def_value);

	} else if (_strcmp((const char *)argv[0], "dbg") == 0) {
		/* check bitblit debug registers */

		for (check_addr = BLIT_RD_DST_DBG_SIG;
			 check_addr <= BLIT_WR_BUFF1_DBG_SIG; check_addr += 4) {
			reg_value = HAL_READ32(GE_REG_BASE, check_addr);
			DiagPrintf("addr 0x%08x value 0x%08x  \n\r",
					   GE_REG_BASE + check_addr, reg_value);
		}
	} else if (_strcmp((const char *)argv[0], "range") == 0) {
		/* check bitblit debug registers */
		addr = (u32)strtoul((const char *)argv[1], endptr, 16);
		u32 range = (u32)strtoul((const char *)argv[2], endptr, 16);
		for (check_addr = addr; check_addr < (addr + range);
			 check_addr += 4) {
			reg_value = HAL_READ32(GE_REG_BASE, check_addr);
			DiagPrintf("addr 0x%08x value 0x%08x  \n\r",
					   GE_REG_BASE + check_addr, reg_value);
		}
	}

	return 0;
}

u32 CmdGETest1(IN u16 argc, IN u8 *argv[])
{
	int ret = 0;
	u32 size = 0;
	FILE *fp;
	char buf[128];
	char **restrict endptr;

	DiagPrintf("ge test command 1\n\rargc %d\n\r", argc);

	for (int i = 0; i < argc; i++) {
		DiagPrintf("argv[%d] = %s\n\r", i, argv[i]);
	}

	if (_strcmp((const char *)argv[0], "load") == 0) {
		if (argc >= 2) {
			fp = fopen(argv[1], "rb");
			if (!fp) {
				DiagPrintf("open file %s failed \n\r", argv[1]);
				return -1;
			}

			fseek(fp, 0, SEEK_END);
			int flen = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			DiagPrintf("file len %d\n", flen);

			if (flen > 128) {
				flen = 128;
			}
			int rlen = fread(buf, flen, 1, fp);

			buf[127] = 0;

			DiagPrintf("content %d:\n %s\n", rlen, buf);

			fclose(fp);

			if (argc >= 3) {
				fp = fopen(argv[2], "w");
			} else {
				fp = fopen("C:\\martin\\martin_output.txt",
						   "w");
			}
			if (!fp) {
				DiagPrintf("open file %s for write failed \n\r",
						   argv[2]);
				return -1;
			}
			char test_str[] = "edit by martin\n";
			fwrite(test_str, sizeof(test_str), 1, fp);
			fwrite(buf, 127, 1, fp);
			fclose(fp);
		}

	} else if (_strcmp((const char *)argv[0], "ld") == 0) {
		if (argc >= 3) {
			size = (u32)strtoul((const char *)argv[2], endptr, 10);
			ret = test_load((char *)argv[1], size);
			DiagPrintf("load file %s ret %d \n\r", argv[1], ret);
		} else {
			DiagPrintf("need register addr\n\r");
		}
	}

	return 0;
}

u32 CmdGETest2(IN u16 argc, IN u8 *argv[])
{
	int ret = 0;
	//struct ge_blit dst;

	//DiagPrintf("ge test 2\n\rargc %d\n\r", argc);

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

	//DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	//DCache_Disable();

	hal_ge_init();

#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	ret = vfs_init_remap();
	if (ret != 0) {
		return ret;
	}
#endif

	ret = blit_test_v3(argc, argv);

	DiagPrintf("ge test 2 for bitblit ge_ret %d\n\r", ret);

	return 0;
}

u32 CmdGETest3(IN u16 argc, IN u8 *argv[])
{
	int ret = 0;
	//struct ge_blit dst;
	//DiagPrintf("ge test  3\n\rargc %d\n\r", argc);

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

	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x540, 0xff);

	//DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	//DCache_Disable();

	hal_ge_init();

#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD

	ret = vfs_init_remap();
	if (ret != 0) {
		return ret;
	}
#endif

	ret = zoom_test(argc, argv);

	DiagPrintf("ge test 3 for zoom ge_ret %d\n\r", ret);

	return ret;
}

u32 CmdGETest4(IN u16 argc, IN u8 *argv[])
{
	int ret = 0;
	//struct ge_blit dst;
	//DiagPrintf("ge test  3\n\rargc %d\n\r", argc);

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

	HAL_WRITE32(RXI300_CORE_REG_BASE, 0x540, 0xff);

	hal_ge_init();

#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
	ret = vfs_init_remap();
	if (ret != 0) {
		return ret;
	}
#endif

	//ret = RGBp2i_test(argc, argv);
	ret = zoom_testv2(argc, argv);

	DiagPrintf("ge test 4 for rgb planar to interleave ge_ret %d\n\r", ret);

	return 0;
}

u32 CmdGETest5(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;
	u32 width = 3840;
	u32 height = 2160;
	u32 size = width * height * 2; // Total bytes for 16-bit 3840x2160
	u32 sub_w = 1920;
	u32 sub_h = 2160;
	u32 sub_size = sub_w * sub_h * 2; // Total bytes for 16-bit 1920x2160
	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_l_raw = NULL;
	void *dst_r_raw = NULL;

	u32 *src_buf = NULL;
	u32 *dst_l = NULL;
	u32 *dst_r = NULL;

	u32 start_tick, end_tick;
	ge_drv_ctx_t *pge_drv_ctx = NULL;
	struct ge_blit blit_cfg;

	DiagPrintf(
		"GE Test 5: Split 3840x2160 image into Left/Right using 32-bit optimization\n\r");

	hal_ge_init();

	src_buf_raw = malloc(size + buffer_align);
	dst_l_raw = malloc(sub_size + buffer_align);
	dst_r_raw = malloc(sub_size + buffer_align);

	if (!src_buf_raw || !dst_l_raw || !dst_r_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	src_buf = (u32 *)(((u32)src_buf_raw + (buffer_align - 1)) &
					  ~(buffer_align - 1));
	dst_l = (u32 *)(((u32)dst_l_raw + (buffer_align - 1)) &
					~(buffer_align - 1));
	dst_r = (u32 *)(((u32)dst_r_raw + (buffer_align - 1)) &
					~(buffer_align - 1));

	DiagPrintf("Filling source buffer (as 16-bit shorts)...\n\r");
	short *src_ptr_16 = (short *)src_buf;
	for (u32 i = 0; i < width * height; i++) {
		src_ptr_16[i] = (short)(i % 3840);
	}

	DCache_CleanInvalidate((u32)src_buf, size);
	DCache_CleanInvalidate((u32)dst_l, sub_size);
	DCache_CleanInvalidate((u32)dst_r, sub_size);

	pge_drv_ctx = ge_open();
	if (!pge_drv_ctx || (pge_drv_ctx == (ge_drv_ctx_t *)EINVAL)) {
		DiagPrintf("Failed to open GE driver\n\r");
		goto exit;
	}

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	memset(&blit_cfg, 0, sizeof(blit_cfg));
	blit_cfg.cmd = GE_BLT_ROI_BLEND;
	blit_cfg.work_mode = FILL_SRCRECT;
	// Treat 3840x2160@16bpp as 1920x2160@32bpp
	blit_cfg.src_img.w = 1920;
	blit_cfg.src_img.h = 2160;
	blit_cfg.src_img.img_fmt = GE_BLT_ARGB8888;

	// Destination is 1920x2160@16bpp -> Treat as 960x2160@32bpp
	blit_cfg.dst_img.w = 960;
	blit_cfg.dst_img.h = 2160;
	blit_cfg.dst_img.img_fmt = GE_BLT_ARGB8888;

	blit_cfg.src_rect.w = 960;
	blit_cfg.src_rect.h = 2160;
	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = 960;
	blit_cfg.dst_rect.h = 2160;
	blit_cfg.usr_src_buf = (u32)src_buf;

	unsigned short *dst_l_16 = (unsigned short *)dst_l;
	unsigned short *dst_r_16 = (unsigned short *)dst_r;
	DiagPrintf("Before:\n\r");
	dst_l_16[200] = 1234;
	dst_r_16[200] = 5678;
	DiagPrintf("Src[200]: %d, DstL[200]: %d\n\r", src_ptr_16[200],
			   dst_l_16[200]);
	DiagPrintf("Src[1920+200]: %d, DstR[200]: %d\n\r",
			   src_ptr_16[1920 + 200], dst_r_16[200]);

	DiagPrintf("Starting bitblit operations (32-bit width: 960)...\n\r");
	start_tick = xTaskGetTickCount();

	// Left Part
	blit_cfg.src_rect.x = 0;
	blit_cfg.usr_dst_buf = (u32)dst_l;
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	// Right Part
	blit_cfg.src_rect.x = 960;
	blit_cfg.usr_dst_buf = (u32)dst_r;
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	end_tick = xTaskGetTickCount();
	DiagPrintf("Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	// Verify a few pixels
	DCache_Invalidate((u32)dst_l, sub_size);
	DCache_Invalidate((u32)dst_r, sub_size);

	DiagPrintf("Verification:\n\r");
	DiagPrintf("Src[200]: %d, DstL[200]: %d\n\r", src_ptr_16[200],
			   dst_l_16[200]);
	DiagPrintf("Src[1920+200]: %d, DstR[200]: %d\n\r",
			   src_ptr_16[1920 + 200], dst_r_16[200]);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_l_raw) {
		free(dst_l_raw);
	}
	if (dst_r_raw) {
		free(dst_r_raw);
	}
	if (pge_drv_ctx && (pge_drv_ctx != (ge_drv_ctx_t *)EINVAL)) {
		ge_close(pge_drv_ctx);
	}

	return 0;
}

u32 CmdGETest6(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;
	u32 width = 3840;
	u32 height = 2160;
	u32 size = width * height * 2;
	u32 sub_w = 1920;
	u32 sub_h = 2160;
	u32 sub_size = sub_w * sub_h * 2;
	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_buf_raw = NULL;
	short *src_buf = NULL;
	short *dst_buf = NULL;
	u32 start_tick, end_tick;

	DiagPrintf("GE Test 6: Software Crop 3840x2160 -> 1920x2160\n\r");

	src_buf_raw = malloc(size + buffer_align);
	dst_buf_raw = malloc(sub_size + buffer_align);

	if (!src_buf_raw || !dst_buf_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	src_buf = (short *)(((u32)src_buf_raw + (buffer_align - 1)) &
						~(buffer_align - 1));
	dst_buf = (short *)(((u32)dst_buf_raw + (buffer_align - 1)) &
						~(buffer_align - 1));

	DiagPrintf("Filling source buffer...\n\r");
	for (u32 i = 0; i < width * height; i++) {
		src_buf[i] = (short)(i % 3840);
	}

	DiagPrintf("Starting software crop (horizontal center)...\n\r");
	start_tick = xTaskGetTickCount();

	// Crop the horizontal center 1920x2160 area
	software_crop(src_buf, width, height, dst_buf, (width - sub_w) / 2, 0,
				  sub_w, sub_h);

	end_tick = xTaskGetTickCount();
	DiagPrintf("Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	// Verify result
	DiagPrintf("Verification (Center crop):\n\r");
	u32 offset_x = (width - sub_w) / 2;
	DiagPrintf("Src[%d]: %d, Dst[0]: %d\n\r", offset_x, src_buf[offset_x],
			   dst_buf[0]);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_buf_raw) {
		free(dst_buf_raw);
	}

	return 0;
}

u32 CmdGETest7(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;
	u32 width = 3840;
	u32 height = 2160;
	u32 size = width * height * 2;
	u32 sub_w = 1920;
	u32 sub_h = 2160;
	u32 sub_size = sub_w * sub_h * 2;
	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_l_raw = NULL;
	void *dst_r_raw = NULL;

	u16 *src_buf = NULL;
	u16 *dst_l = NULL;
	u16 *dst_r = NULL;

	u32 start_tick, end_tick;
	ge_drv_ctx_t *pge_drv_ctx = NULL;
	struct ge_blit blit_cfg;

	DiagPrintf(
		"GE Test 7: Split 3840x2160 image into Left/Right using GE_BLT_RGB565\n\r");

	hal_ge_init();

	src_buf_raw = malloc(size + buffer_align);
	dst_l_raw = malloc(sub_size + buffer_align);
	dst_r_raw = malloc(sub_size + buffer_align);

	if (!src_buf_raw || !dst_l_raw || !dst_r_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	src_buf = (u16 *)(((u32)src_buf_raw + (buffer_align - 1)) &
					  ~(buffer_align - 1));
	dst_l = (u16 *)(((u32)dst_l_raw + (buffer_align - 1)) &
					~(buffer_align - 1));
	dst_r = (u16 *)(((u32)dst_r_raw + (buffer_align - 1)) &
					~(buffer_align - 1));

	DiagPrintf("Filling source buffer (16-bit)...\n\r");
	for (u32 i = 0; i < width * height; i++) {
		src_buf[i] = (u16)(i % 3840);
	}

	DCache_CleanInvalidate((u32)src_buf, size);
	DCache_CleanInvalidate((u32)dst_l, sub_size);
	DCache_CleanInvalidate((u32)dst_r, sub_size);

	pge_drv_ctx = ge_open();
	if (!pge_drv_ctx || (pge_drv_ctx == (ge_drv_ctx_t *)EINVAL)) {
		DiagPrintf("Failed to open GE driver\n\r");
		goto exit;
	}

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	memset(&blit_cfg, 0, sizeof(blit_cfg));
	blit_cfg.cmd = GE_BLT_ROI_BLEND;
	blit_cfg.src_img.w = width;
	blit_cfg.src_img.h = height;
	blit_cfg.src_img.img_fmt = GE_BLT_RGB565;
	blit_cfg.dst_img.w = sub_w;
	blit_cfg.dst_img.h = sub_h;
	blit_cfg.dst_img.img_fmt = GE_BLT_RGB565;
	blit_cfg.src_rect.w = sub_w;
	blit_cfg.src_rect.h = sub_h;
	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = sub_w;
	blit_cfg.dst_rect.h = sub_h;
	blit_cfg.usr_src_buf = (u32)src_buf;

	DiagPrintf("Starting RGB565 bitblit operations...\n\r");
	start_tick = xTaskGetTickCount();

	// Left Part
	blit_cfg.src_rect.x = 0;
	blit_cfg.usr_dst_buf = (u32)dst_l;
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	// Right Part
	blit_cfg.src_rect.x = 1920;
	blit_cfg.usr_dst_buf = (u32)dst_r;
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	end_tick = xTaskGetTickCount();
	DiagPrintf("Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	// Verify results
	DCache_Invalidate((u32)dst_l, sub_size);
	DCache_Invalidate((u32)dst_r, sub_size);
	DiagPrintf("Verification:\n\r");
	DiagPrintf("Src[200]: %d, DstL[200]: %d\n\r", src_buf[200], dst_l[200]);
	DiagPrintf("Src[1920+200]: %d, DstR[200]: %d\n\r", src_buf[1920 + 200],
			   dst_r[200]);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_l_raw) {
		free(dst_l_raw);
	}
	if (dst_r_raw) {
		free(dst_r_raw);
	}
	if (pge_drv_ctx && (pge_drv_ctx != (ge_drv_ctx_t *)EINVAL)) {
		ge_close(pge_drv_ctx);
	}

	return 0;
}

u32 CmdGETest8(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	DiagPrintf(
		"=================================================================\n\r");
	DiagPrintf(
		"WARNING: GE8 uses YUV420_SMI format which does NOT support\n\r");
	DiagPrintf(
		"         horizontal ROI offset (dst_rect.x) for splicing.\n\r");
	DiagPrintf("         This test will produce INCORRECT results!\n\r");
	DiagPrintf(
		"         Use GE9 instead (ARGB8888 approach) for correct output.\n\r");
	DiagPrintf(
		"=================================================================\n\r\n\r");

	/* Input: 1920x4080 YUV420_SMI */
	u32 src_width = 1920;
	u32 src_height = 4080;
	u32 src_half_height = 2040;

	/* Output: 3840x2040 YUV420_SMI */
	u32 dst_width = 3840;
	u32 dst_height = 2040;

	/* YUV420_SMI memory size calculation:
	 * Y plane: width * height bytes
	 * UV plane: width * height/2 bytes
	 * Total: width * height * 1.5 bytes
	 */
	u32 src_y_size = src_width * src_height;
	u32 src_uv_size = src_width * src_height / 2;
	u32 src_total_size = src_y_size + src_uv_size;

	u32 dst_y_size = dst_width * dst_height;
	u32 dst_uv_size = dst_width * dst_height / 2;
	u32 dst_total_size = dst_y_size + dst_uv_size;

	u32 half_y_size = src_width * src_half_height;
	u32 half_uv_size = src_width * src_half_height / 2;
	u32 half_total_size = half_y_size + half_uv_size;

	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_buf_raw = NULL;

	u8 *src_buf = NULL;
	u8 *dst_buf = NULL;

	u8 *src_y_addr = NULL;
	u8 *src_uv_addr = NULL;
	u8 *dst_y_addr = NULL;
	u8 *dst_uv_addr = NULL;

	u32 start_tick, end_tick;
	ge_drv_ctx_t *pge_drv_ctx = NULL;
	struct ge_blit blit_cfg;

	DiagPrintf(
		"GE Test 8: YUV420_SMI 1920x4080 -> 3840x2040 Left/Right Splicing\n\r");
	DiagPrintf("Source: %dx%d YUV420_SMI (%d bytes)\n\r", src_width,
			   src_height, src_total_size);
	DiagPrintf("Destination: %dx%d YUV420_SMI (%d bytes)\n\r", dst_width,
			   dst_height, dst_total_size);

	hal_ge_init();

	/* Allocate memory with alignment */
	src_buf_raw = malloc(src_total_size + buffer_align);
	dst_buf_raw = malloc(dst_total_size + buffer_align);

	if (!src_buf_raw || !dst_buf_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	/* Align buffers */
	src_buf = (u8 *)(((u32)src_buf_raw + (buffer_align - 1)) &
					 ~(buffer_align - 1));
	dst_buf = (u8 *)(((u32)dst_buf_raw + (buffer_align - 1)) &
					 ~(buffer_align - 1));

	/* Set Y and UV plane addresses */
	src_y_addr = src_buf;
	src_uv_addr = src_buf + src_y_size;
	dst_y_addr = dst_buf;
	dst_uv_addr = dst_buf + dst_y_size;

	/* Fill source buffer with test pattern */
	DiagPrintf("Filling source buffer with test pattern...\n\r");
	for (u32 i = 0; i < src_y_size; i++) {
		src_y_addr[i] = (u8)(i % 256);
	}
	for (u32 i = 0; i < src_uv_size; i++) {
		src_uv_addr[i] = (u8)((i + 128) % 256);
	}

	/* Clear destination buffer */
	memset(dst_buf, 0, dst_total_size);

	/* Clean and invalidate cache */
	DCache_CleanInvalidate((u32)src_buf, src_total_size);
	DCache_CleanInvalidate((u32)dst_buf, dst_total_size);

	/* Open GE driver */
	pge_drv_ctx = ge_open();
	if (!pge_drv_ctx || (pge_drv_ctx == (ge_drv_ctx_t *)EINVAL)) {
		DiagPrintf("Failed to open GE driver\n\r");
		goto exit;
	}

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	/* Configure blit operation
	 * Source: 1920x2040 from input buffer
	 * Destination: 3840x2040 output buffer (write to specific region)
	 */
	memset(&blit_cfg, 0, sizeof(blit_cfg));
	blit_cfg.cmd = GE_BLT_ROI_BLEND;
	blit_cfg.work_mode = FILL_SRCRECT;

	/* Source: 1920x2040 YUV420_SMI */
	blit_cfg.src_img.w = src_width;
	blit_cfg.src_img.h = src_half_height;
	blit_cfg.src_img.img_fmt = GE_BLT_YUV420_SMI;

	/* Destination: 3840x2040 YUV420_SMI (full output buffer) */
	blit_cfg.dst_img.w = dst_width;
	blit_cfg.dst_img.h = dst_height;
	blit_cfg.dst_img.img_fmt = GE_BLT_YUV420_SMI;

	/* Source rectangle: full 1920x2040 region */
	blit_cfg.src_rect.x = 0;
	blit_cfg.src_rect.y = 0;
	blit_cfg.src_rect.w = src_width;
	blit_cfg.src_rect.h = src_half_height;

	/* Enable YUV mode */
	blit_cfg.dst_yuv_sel = 1;

	start_tick = xTaskGetTickCount();
	DiagPrintf("Starting GE operations...\n\r");

	/* GE Operation 1: Copy top half (1920x2040) to left side (x=0) */
	DiagPrintf("GE copy top half -> left side (x=0)...\n\r");

	/* Source addresses: Y and UV planes of top half */
	blit_cfg.src_addr[0] = (u32)src_y_addr;
	/* For YUV420_SMI source via usr_src_buf */
	blit_cfg.usr_src_buf = (u32)src_y_addr;

	/* Destination addresses: Y and UV planes */
	blit_cfg.dst_addr[0] = (u32)dst_y_addr;
	blit_cfg.dst_addr[1] = (u32)dst_uv_addr;
	blit_cfg.usr_dst_buf = (u32)dst_y_addr;

	/* Destination rectangle: left side at x=0 */
	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = src_width;
	blit_cfg.dst_rect.h = src_half_height;

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* GE Operation 2: Copy bottom half (1920x2040) to right side (x=1920) */
	DiagPrintf("GE copy bottom half -> right side (x=1920)...\n\r");

	/* Source addresses: Y and UV planes of bottom half */
	blit_cfg.src_addr[0] = (u32)(src_y_addr + half_y_size);
	blit_cfg.usr_src_buf = (u32)(src_y_addr + half_y_size);

	/* Destination addresses: same as before (GE uses dst_rect.x for positioning) */
	blit_cfg.dst_addr[0] = (u32)dst_y_addr;
	blit_cfg.dst_addr[1] = (u32)dst_uv_addr;
	blit_cfg.usr_dst_buf = (u32)dst_y_addr;

	/* Destination rectangle: right side at x=1920 */
	blit_cfg.dst_rect.x = src_width;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = src_width;
	blit_cfg.dst_rect.h = src_half_height;

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	end_tick = xTaskGetTickCount();
	DiagPrintf("Total Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	/* Invalidate cache to read results */
	DCache_Invalidate((u32)dst_buf, dst_total_size);

	/* Calculate CRC32 for verification */
	DiagPrintf("Calculating CRC32...\n\r");
	u32 crc_y = calculate_crc32(dst_y_addr, dst_y_size);
	u32 crc_uv = calculate_crc32(dst_uv_addr, dst_uv_size);
	u32 crc_total = calculate_crc32(dst_buf, dst_total_size);

	DiagPrintf("CRC32 Results (GE8):\n\r");
	DiagPrintf("  Y Plane:  0x%08X (%d bytes)\n\r", crc_y, dst_y_size);
	DiagPrintf("  UV Plane: 0x%08X (%d bytes)\n\r", crc_uv, dst_uv_size);
	DiagPrintf("  Total:    0x%08X (%d bytes)\n\r", crc_total,
			   dst_total_size);

	/* Verify a few pixels */
	DiagPrintf("Verification:\n\r");
	DiagPrintf("Src Y[200]: %d, Dst Y[200] (left): %d\n\r", src_y_addr[200],
			   dst_y_addr[200]);
	DiagPrintf("Src Y[%d]: %d, Dst Y[%d] (right): %d\n\r",
			   half_y_size + 200, src_y_addr[half_y_size + 200],
			   src_width + 200, dst_y_addr[src_width + 200]);
	DiagPrintf("Src UV[100]: %d, Dst UV[100]: %d\n\r", src_uv_addr[100],
			   dst_uv_addr[100]);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_buf_raw) {
		free(dst_buf_raw);
	}
	if (pge_drv_ctx && (pge_drv_ctx != (ge_drv_ctx_t *)EINVAL)) {
		ge_close(pge_drv_ctx);
	}

	return 0;
}

u32 CmdGETest9(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	/* Input: 1920x4080 YUV420_SMI */
	u32 src_width = 1920;
	u32 src_height = 4080;
	u32 src_half_height = 2040;

	/* Output: 3840x2040 YUV420_SMI */
	u32 dst_width = 3840;
	u32 dst_height = 2040;

	/* YUV420_SMI memory size calculation */
	u32 src_y_size = src_width * src_height;
	u32 src_uv_size = src_width * src_height / 2;
	u32 src_total_size = src_y_size + src_uv_size;

	u32 dst_y_size = dst_width * dst_height;
	u32 dst_uv_size = dst_width * dst_height / 2;
	u32 dst_total_size = dst_y_size + dst_uv_size;

	u32 half_y_size = src_width * src_half_height;
	u32 half_uv_size = src_width * src_half_height / 2;

	/* Treat YUV data as ARGB8888 (4 bytes per pixel)
	 * Y plane: 1920x4080 bytes = 480x1020 ARGB8888 pixels
	 * UV plane: 1920x2040 bytes = 480x510 ARGB8888 pixels
	 */
	u32 y_argb_width = src_width / 4; /* 480 */
	u32 y_argb_height = src_height; /* 4080 */
	u32 y_argb_half_height = src_half_height; /* 2040 */

	u32 uv_argb_width = src_width / 4; /* 480 */
	u32 uv_argb_height = src_height / 2; /* 2040 */
	u32 uv_argb_half_height = src_half_height / 2; /* 1020 */

	u32 dst_y_argb_width = dst_width / 4; /* 960 */
	u32 dst_uv_argb_width = dst_width / 4; /* 960 */

	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_buf_raw = NULL;

	u8 *src_buf = NULL;
	u8 *dst_buf = NULL;

	u8 *src_y_addr = NULL;
	u8 *src_uv_addr = NULL;
	u8 *dst_y_addr = NULL;
	u8 *dst_uv_addr = NULL;

	u32 start_tick, end_tick;
	ge_drv_ctx_t *pge_drv_ctx = NULL;
	struct ge_blit blit_cfg;

	DiagPrintf(
		"GE Test 9: YUV420_SMI reshape using ARGB8888 (1920x4080 -> 3840x2040)\n\r");
	DiagPrintf("Y plane as ARGB: %dx%d -> %dx%d (split at y=%d)\n\r",
			   y_argb_width, y_argb_height, dst_y_argb_width, dst_height,
			   y_argb_half_height);
	DiagPrintf("UV plane as ARGB: %dx%d -> %dx%d (split at y=%d)\n\r",
			   uv_argb_width, uv_argb_height, dst_uv_argb_width,
			   dst_height / 2, uv_argb_half_height);

	hal_ge_init();

	/* Allocate memory with alignment */
	src_buf_raw = malloc(src_total_size + buffer_align);
	dst_buf_raw = malloc(dst_total_size + buffer_align);

	if (!src_buf_raw || !dst_buf_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	/* Align buffers */
	src_buf = (u8 *)(((u32)src_buf_raw + (buffer_align - 1)) &
					 ~(buffer_align - 1));
	dst_buf = (u8 *)(((u32)dst_buf_raw + (buffer_align - 1)) &
					 ~(buffer_align - 1));

	/* Set Y and UV plane addresses */
	src_y_addr = src_buf;
	src_uv_addr = src_buf + src_y_size;
	dst_y_addr = dst_buf;
	dst_uv_addr = dst_buf + dst_y_size;

	/* Fill source buffer with test pattern */
	DiagPrintf("Filling source buffer with test pattern...\n\r");
	for (u32 i = 0; i < src_y_size; i++) {
		src_y_addr[i] = (u8)(i % 256);
	}
	for (u32 i = 0; i < src_uv_size; i++) {
		src_uv_addr[i] = (u8)((i + 128) % 256);
	}

	/* Clear destination buffer */
	memset(dst_buf, 0, dst_total_size);

	/* Clean and invalidate cache */
	DCache_CleanInvalidate((u32)src_buf, src_total_size);
	DCache_CleanInvalidate((u32)dst_buf, dst_total_size);

	/* Open GE driver */
	pge_drv_ctx = ge_open();
	if (!pge_drv_ctx || (pge_drv_ctx == (ge_drv_ctx_t *)EINVAL)) {
		DiagPrintf("Failed to open GE driver\n\r");
		goto exit;
	}

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	start_tick = xTaskGetTickCount();
	DiagPrintf("Starting GE operations (treating YUV as ARGB8888)...\n\r");

	/* ===== Process Y Plane ===== */
	DiagPrintf("Processing Y plane...\n\r");

	memset(&blit_cfg, 0, sizeof(blit_cfg));
	blit_cfg.cmd = GE_BLT_ROI_BLEND;
	blit_cfg.work_mode = FILL_SRCRECT;

	/* Y Plane Operation 1: Top half (480x2040) -> Left side (x=0) */
	DiagPrintf("  Y: Top half -> Left side...\n\r");

	blit_cfg.src_img.w = y_argb_width;
	blit_cfg.src_img.h = y_argb_half_height;
	blit_cfg.src_img.img_fmt = GE_BLT_ARGB8888;

	blit_cfg.dst_img.w = dst_y_argb_width;
	blit_cfg.dst_img.h = dst_height;
	blit_cfg.dst_img.img_fmt = GE_BLT_ARGB8888;

	blit_cfg.src_rect.x = 0;
	blit_cfg.src_rect.y = 0;
	blit_cfg.src_rect.w = y_argb_width;
	blit_cfg.src_rect.h = y_argb_half_height;

	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = y_argb_width;
	blit_cfg.dst_rect.h = y_argb_half_height;

	blit_cfg.usr_src_buf = (u32)src_y_addr;
	blit_cfg.usr_dst_buf = (u32)dst_y_addr;

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* Y Plane Operation 2: Bottom half (480x2040) -> Right side (x=480) */
	DiagPrintf("  Y: Bottom half -> Right side...\n\r");

	blit_cfg.usr_src_buf = (u32)(src_y_addr + half_y_size);
	blit_cfg.dst_rect.x = y_argb_width; /* 480 */

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* ===== Process UV Plane ===== */
	DiagPrintf("Processing UV plane...\n\r");

	/* UV Plane Operation 1: Top half (480x1020) -> Left side (x=0) */
	DiagPrintf("  UV: Top half -> Left side...\n\r");

	blit_cfg.src_img.w = uv_argb_width;
	blit_cfg.src_img.h = uv_argb_half_height;
	blit_cfg.src_img.img_fmt = GE_BLT_ARGB8888;

	blit_cfg.dst_img.w = dst_uv_argb_width;
	blit_cfg.dst_img.h = dst_height / 2;
	blit_cfg.dst_img.img_fmt = GE_BLT_ARGB8888;

	blit_cfg.src_rect.x = 0;
	blit_cfg.src_rect.y = 0;
	blit_cfg.src_rect.w = uv_argb_width;
	blit_cfg.src_rect.h = uv_argb_half_height;

	blit_cfg.dst_rect.x = 0;
	blit_cfg.dst_rect.y = 0;
	blit_cfg.dst_rect.w = uv_argb_width;
	blit_cfg.dst_rect.h = uv_argb_half_height;

	blit_cfg.usr_src_buf = (u32)src_uv_addr;
	blit_cfg.usr_dst_buf = (u32)dst_uv_addr;

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	/* UV Plane Operation 2: Bottom half (480x1020) -> Right side (x=480) */
	DiagPrintf("  UV: Bottom half -> Right side...\n\r");

	blit_cfg.usr_src_buf = (u32)(src_uv_addr + half_uv_size);
	blit_cfg.dst_rect.x = uv_argb_width; /* 480 */

	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_START, &blit_cfg);
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_CHK_END, NULL);

	end_tick = xTaskGetTickCount();
	DiagPrintf("Total Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	/* Invalidate cache to read results */
	DCache_Invalidate((u32)dst_buf, dst_total_size);

	/* Calculate CRC32 for verification */
	DiagPrintf("Calculating CRC32...\n\r");
	u32 crc_y = calculate_crc32(dst_y_addr, dst_y_size);
	u32 crc_uv = calculate_crc32(dst_uv_addr, dst_uv_size);
	u32 crc_total = calculate_crc32(dst_buf, dst_total_size);

	DiagPrintf("CRC32 Results (GE9):\n\r");
	DiagPrintf("  Y Plane:  0x%08X (%d bytes)\n\r", crc_y, dst_y_size);
	DiagPrintf("  UV Plane: 0x%08X (%d bytes)\n\r", crc_uv, dst_uv_size);
	DiagPrintf("  Total:    0x%08X (%d bytes)\n\r", crc_total,
			   dst_total_size);

	/* Verify a few pixels */
	DiagPrintf("Verification:\n\r");
	DiagPrintf("Src Y[200]: %d, Dst Y[200] (left): %d\n\r", src_y_addr[200],
			   dst_y_addr[200]);
	DiagPrintf("Src Y[%d]: %d, Dst Y[%d] (right): %d\n\r",
			   half_y_size + 200, src_y_addr[half_y_size + 200],
			   src_width + 200, dst_y_addr[src_width + 200]);
	DiagPrintf("Src UV[100]: %d, Dst UV[100]: %d\n\r", src_uv_addr[100],
			   dst_uv_addr[100]);
	DiagPrintf("Src UV[%d]: %d, Dst UV[%d] (right): %d\n\r",
			   half_uv_size + 100, src_uv_addr[half_uv_size + 100],
			   src_width + 100, dst_uv_addr[src_width + 100]);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_buf_raw) {
		free(dst_buf_raw);
	}
	if (pge_drv_ctx && (pge_drv_ctx != (ge_drv_ctx_t *)EINVAL)) {
		ge_close(pge_drv_ctx);
	}

	return 0;
}

u32 CmdGETest10(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	/* Example: 3848x2168 RAW16 -> reshape with overlap
	 * Two regions: 1920+8 (overlap) = 1928 pixels wide, 2040+6 (overlap) = 2046 pixels tall
	 * Output: 1928x4092 (two stacked regions, within 4095 limit)
	 * y_overlap reduced to 6 to keep dst_h at 2040
	 */
	u32 src_width = 3848;
	u32 src_height = 2168;
	u32 start_x = 4; /* Starting X position for alignment */
	u32 start_y = 64; /* Starting Y position for alignment */
	u32 dst_w = 1920; /* Width per half (without overlap) */
	u32 dst_h = 2040; /* Height per half (without overlap) */
	u32 x_overlap = 8; /* Horizontal overlap */
	u32 y_overlap =
		6; /* Vertical overlap - reduced to stay within 4095 limit */

	/* Calculate actual output dimensions */
	u32 output_width = dst_w + x_overlap; /* 1928 */
	u32 output_height =
		(dst_h + y_overlap) * 2; /* 4092 - within 4095 limit */

	u32 src_size = src_width * src_height * 2; /* 16-bit per pixel */
	u32 dst_size = output_width * output_height * 2;
	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_buf_raw = NULL;
	u16 *src_buf = NULL;
	u16 *dst_buf = NULL;
	u32 start_tick, end_tick;
	int ret;

	DiagPrintf(
		"=================================================================\n\r");
	DiagPrintf(
		"GE Test 10: RAW Reshape Function Test (ge_raw_reshape)\n\r");
	DiagPrintf("Source: %dx%d RAW16 (%d bytes)\n\r", src_width, src_height,
			   src_size);
	DiagPrintf("Crop start: (%d,%d), dst_w=%d, dst_h=%d\n\r", start_x,
			   start_y, dst_w, dst_h);
	DiagPrintf("Overlap: x=%d, y=%d\n\r", x_overlap, y_overlap);
	DiagPrintf("Output: %dx%d RAW16 (%d bytes)\n\r", output_width,
			   output_height, dst_size);
	DiagPrintf(
		"=================================================================\n\r");

	hal_ge_init();

	/* Allocate memory with alignment */
	src_buf_raw = malloc(src_size + buffer_align);
	dst_buf_raw = malloc(dst_size + buffer_align);

	if (!src_buf_raw || !dst_buf_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	/* Align buffers */
	src_buf = (u16 *)(((u32)src_buf_raw + (buffer_align - 1)) &
					  ~(buffer_align - 1));
	dst_buf = (u16 *)(((u32)dst_buf_raw + (buffer_align - 1)) &
					  ~(buffer_align - 1));

	/* Fill source buffer with test pattern */
	DiagPrintf("Filling source buffer with test pattern...\n\r");
	for (u32 i = 0; i < src_width * src_height; i++) {
		src_buf[i] = (u16)(i % 4096); /* 12-bit RAW simulation */
	}

	/* Clear destination buffer */
	memset(dst_buf, 0, dst_size);

	/* Clean cache for source and destination buffers before GE operation */
	DCache_CleanInvalidate((u32)src_buf, src_size);
	DCache_CleanInvalidate((u32)dst_buf, dst_size);

	/* Open GE driver */
	ge_drv_ctx_t *pge_drv_ctx = ge_open();
	if (!pge_drv_ctx || (pge_drv_ctx == (ge_drv_ctx_t *)EINVAL)) {
		DiagPrintf("Failed to open GE driver\n\r");
		goto exit;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	DiagPrintf(
		"Calling ge_raw_reshape(src=%dx%d, start=(%d,%d), dst=%dx%d, overlap=%dx%d)...\n\r",
		src_width, src_height, start_x, start_y, dst_w, dst_h,
		x_overlap, y_overlap);

	start_tick = xTaskGetTickCount();

	/* Call the raw reshape function */
	ret = ge_raw_reshape(pge_drv_ctx, src_buf, dst_buf, src_width,
						 src_height, start_x, start_y, dst_w, dst_h,
						 x_overlap, y_overlap);

	end_tick = xTaskGetTickCount();

	/* Close GE driver */
	ge_close(pge_drv_ctx);

	if (ret != 0) {
		DiagPrintf("ge_raw_reshape failed with error code: %d\n\r",
				   ret);
		goto exit;
	}

	/* Invalidate cache to read results */
	DCache_Invalidate((u32)dst_buf, dst_size);

	DiagPrintf("Reshape completed successfully!\n\r");
	DiagPrintf("Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	/* Verify a few pixels - account for crop offset */
	DiagPrintf("Verification (sample pixels with crop offset):\n\r");

	/* Calculate expected source indices for destination pixels */
	/* Left region: Dst pixel (x,y) comes from Src(start_x + x, start_y + y) */
	u32 expected_src_idx_100 = (start_y + 0) * src_width + (start_x + 100);
	u32 expected_src_idx_1000 =
		(start_y + 0) * src_width + (start_x + 1000);

	DiagPrintf(
		"  Dst[100]: %d, Expected from Src[%d]: %d (start+100 at row %d)\n\r",
		dst_buf[100], expected_src_idx_100,
		src_buf[expected_src_idx_100], start_y);
	DiagPrintf(
		"  Dst[1000]: %d, Expected from Src[%d]: %d (start+1000 at row %d)\n\r",
		dst_buf[1000], expected_src_idx_1000,
		src_buf[expected_src_idx_1000], start_y);

	/* Verify correctness */
	bool verify_ok = true;
	if (dst_buf[100] != src_buf[expected_src_idx_100]) {
		DiagPrintf("  ERROR: Dst[100] mismatch!\n\r");
		verify_ok = false;
	}
	if (dst_buf[1000] != src_buf[expected_src_idx_1000]) {
		DiagPrintf("  ERROR: Dst[1000] mismatch!\n\r");
		verify_ok = false;
	}

	if (verify_ok) {
		DiagPrintf("  Verification PASSED ✓\n\r");
	} else {
		DiagPrintf("  Verification FAILED ✗\n\r");
	}

	/* Calculate checksum for cropped region verification */
	u32 src_crop_sum = 0, dst_sum = 0;
	DiagPrintf("Calculating checksums...\n\r");
	/* Sum first row of left crop region from source */
	for (u32 i = 0; i < 1000 && i < output_width; i++) {
		u32 src_idx = start_y * src_width + start_x + i;
		src_crop_sum += src_buf[src_idx];
		dst_sum += dst_buf[i];
	}
	DiagPrintf("Checksum (first 1000 pixels of cropped region):\n\r");
	DiagPrintf("  Src cropped: 0x%08X, Dst: 0x%08X\n\r", src_crop_sum,
			   dst_sum);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_buf_raw) {
		free(dst_buf_raw);
	}

	DiagPrintf(
		"=================================================================\n\r");
	DiagPrintf("Test completed\n\r");
	DiagPrintf(
		"=================================================================\n\r");

	return 0;
}

u32 CmdGETest11(IN u16 argc, IN u8 *argv[])
{
	(void)argc;
	(void)argv;

	/* Example: 1926x4092 YUV420 -> 3840x2040 YUV420
	 * (vertical stacked with overlap -> horizontal with overlap removed)
	 * This simulates the output from ge_raw_reshape after ISP processing
	 * Adjusted to stay within 4095x4095 GE hardware limit
	 * y_overlap reduced to 6 to keep input at 4092 (within 4095 limit)
	 * Note: dst_w and dst_h are now FINAL output dimensions (3840x2040)
	 */
	u32 src_width = 1926; /* dst_w/2 (1920) + x_overlap (6) */
	u32 src_height =
		4092; /* (dst_h (2040) + y_overlap (6)) × 2 - within 4095 limit */
	u32 dst_width = 3840; /* Final output width (horizontal side-by-side) */
	u32 dst_height = 2040; /* Final output height */
	u32 x_overlap = 6; /* Horizontal overlap to remove */
	u32 y_overlap =
		6; /* Vertical overlap to remove - reduced to stay within 4095 limit */

	/* YUV420 memory size calculation */
	u32 src_y_size = src_width * src_height;
	u32 src_uv_size = src_width * src_height / 2;
	u32 src_total_size = src_y_size + src_uv_size;

	u32 dst_y_size = dst_width * dst_height;
	u32 dst_uv_size = dst_width * dst_height / 2;
	u32 dst_total_size = dst_y_size + dst_uv_size;

	u32 buffer_align = 64;

	void *src_buf_raw = NULL;
	void *dst_buf_raw = NULL;
	u8 *src_buf = NULL;
	u8 *dst_buf = NULL;
	u8 *src_y_addr = NULL;
	u8 *src_uv_addr = NULL;
	u8 *dst_y_addr = NULL;
	u8 *dst_uv_addr = NULL;
	u32 start_tick, end_tick;
	int ret;

	DiagPrintf(
		"=================================================================\n\r");
	DiagPrintf(
		"GE Test 11: YUV420 Reshape Function Test (ge_yuv_reshape)\n\r");
	DiagPrintf(
		"Source: %dx%d YUV420 (%d bytes, vertically stacked with overlap)\n\r",
		src_width, src_height, src_total_size);
	DiagPrintf("Final output dimensions: %dx%d\n\r", dst_width, dst_height);
	DiagPrintf("Overlap to remove: x=%d, y=%d\n\r", x_overlap, y_overlap);
	DiagPrintf(
		"Output: %dx%d YUV420 (%d bytes, horizontal, no overlap)\n\r",
		dst_width, dst_height, dst_total_size);
	DiagPrintf(
		"=================================================================\n\r");

	hal_ge_init();

	/* Allocate memory with alignment */
	src_buf_raw = malloc(src_total_size + buffer_align);
	dst_buf_raw = malloc(dst_total_size + buffer_align);

	if (!src_buf_raw || !dst_buf_raw) {
		DiagPrintf("Memory allocation failed!\n\r");
		goto exit;
	}

	/* Align buffers */
	src_buf = (u8 *)(((u32)src_buf_raw + (buffer_align - 1)) &
					 ~(buffer_align - 1));
	dst_buf = (u8 *)(((u32)dst_buf_raw + (buffer_align - 1)) &
					 ~(buffer_align - 1));

	/* Set Y and UV plane addresses */
	src_y_addr = src_buf;
	src_uv_addr = src_buf + src_y_size;
	dst_y_addr = dst_buf;
	dst_uv_addr = dst_buf + dst_y_size;

	/* Fill source buffer with test pattern */
	DiagPrintf("Filling source buffer with test pattern...\n\r");
	for (u32 i = 0; i < src_y_size; i++) {
		src_y_addr[i] = (u8)(i % 256);
	}
	for (u32 i = 0; i < src_uv_size; i++) {
		src_uv_addr[i] = (u8)((i + 128) % 256);
	}

	/* Clear destination buffer */
	memset(dst_buf, 0, dst_total_size);

	/* Clean cache for source and destination buffers before GE operation */
	DCache_CleanInvalidate((u32)src_buf, src_total_size);
	DCache_CleanInvalidate((u32)dst_buf, dst_total_size);

	/* Open GE driver */
	ge_drv_ctx_t *pge_drv_ctx = ge_open();
	if (!pge_drv_ctx || (pge_drv_ctx == (ge_drv_ctx_t *)EINVAL)) {
		DiagPrintf("Failed to open GE driver\n\r");
		goto exit;
	}
	ge_ctrl(pge_drv_ctx, GEDRV_CTRL_BLIT_OPEN, NULL);

	DiagPrintf(
		"Calling ge_yuv_reshape(src=%dx%d, dst=%dx%d, overlap=%dx%d)...\n\r",
		src_width, src_height, dst_width, dst_height, x_overlap,
		y_overlap);

	start_tick = xTaskGetTickCount();

	/* Call the YUV reshape function */
	ret = ge_yuv_reshape(pge_drv_ctx, src_buf, dst_buf, src_width,
						 src_height, dst_width, dst_height, x_overlap,
						 y_overlap);

	end_tick = xTaskGetTickCount();

	/* Close GE driver */
	ge_close(pge_drv_ctx);

	if (ret != 0) {
		DiagPrintf("ge_yuv_reshape failed with error code: %d\n\r",
				   ret);
		goto exit;
	}

	/* Invalidate cache to read results */
	DCache_Invalidate((u32)dst_buf, dst_total_size);

	DiagPrintf("Reshape completed successfully!\n\r");
	DiagPrintf("Execution time: %d ms (ticks: %d)\n\r",
			   (end_tick - start_tick) * portTICK_PERIOD_MS,
			   end_tick - start_tick);

	/* Verify a few pixels */
	DiagPrintf("Verification (sample pixels):\n\r");
	DiagPrintf("  Y: Src[200]: %d, Dst[200]: %d\n\r", src_y_addr[200],
			   dst_y_addr[200]);
	DiagPrintf("  Y: Src[src_w+200]: %d, Dst[dst_w+200]: %d\n\r",
			   src_y_addr[src_width + 200], dst_y_addr[dst_width + 200]);
	DiagPrintf("  UV: Src[100]: %d, Dst[100]: %d\n\r", src_uv_addr[100],
			   dst_uv_addr[100]);
	DiagPrintf("  UV: Src[src_w+100]: %d, Dst[dst_w+100]: %d\n\r",
			   src_uv_addr[src_width + 100], dst_uv_addr[dst_width + 100]);

	/* Calculate CRC32 for verification */
	DiagPrintf("Calculating CRC32...\n\r");
	u32 crc_src_y = calculate_crc32(src_y_addr, src_y_size);
	u32 crc_src_uv = calculate_crc32(src_uv_addr, src_uv_size);
	u32 crc_dst_y = calculate_crc32(dst_y_addr, dst_y_size);
	u32 crc_dst_uv = calculate_crc32(dst_uv_addr, dst_uv_size);

	DiagPrintf("CRC32 Results:\n\r");
	DiagPrintf("  Src Y:  0x%08X (%d bytes)\n\r", crc_src_y, src_y_size);
	DiagPrintf("  Src UV: 0x%08X (%d bytes)\n\r", crc_src_uv, src_uv_size);
	DiagPrintf("  Dst Y:  0x%08X (%d bytes)\n\r", crc_dst_y, dst_y_size);
	DiagPrintf("  Dst UV: 0x%08X (%d bytes)\n\r", crc_dst_uv, dst_uv_size);

exit:
	if (src_buf_raw) {
		free(src_buf_raw);
	}
	if (dst_buf_raw) {
		free(dst_buf_raw);
	}

	DiagPrintf(
		"=================================================================\n\r");
	DiagPrintf("Test completed\n\r");
	DiagPrintf(
		"=================================================================\n\r");

	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_ge_test_cmd[] = {
	/* command string is upper case only, and user input argv[0] in console will convert to upper case */
	{ "REG", CmdGETest0 },	 { "GE1", CmdGETest1 }, { "GE2", CmdGETest2 },
	{ "GE3", CmdGETest3 },	 { "GE4", CmdGETest4 }, { "GE5", CmdGETest5 },
	{ "GE8", CmdGETest8 },	 { "GE9", CmdGETest9 }, { "GE10", CmdGETest10 },
	{ "GE11", CmdGETest11 },
};
