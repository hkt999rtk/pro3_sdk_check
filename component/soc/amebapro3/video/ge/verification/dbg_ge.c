/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2024 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <basic_types.h>
#include <errno.h>
#include "ameba_soc.h"

#include "rts_ge_driver.h"

#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
#include "ameba_sd.h"
#include "ameba_sdioh.h"
#include "os_wrapper.h"
#include "vfs.h"
#endif



#define printf DiagPrintf


void hexdump(const void *data, size_t size)
{
	char ascii[17];
	size_t i, j;

	ascii[16] = '\0';

	for (i = 0; i < size; ++i) {
		DiagPrintf("%02X ", ((unsigned char *)data)[i]);
		if (((unsigned char *)data)[i] >= ' ' &&
			((unsigned char *)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char *)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i + 1) % 8 == 0 || i + 1 == size) {
			DiagPrintf(" ");
			if ((i + 1) % 16 == 0) {
				DiagPrintf("|  %s\n", ascii);
			} else if (i + 1 == size) {
				ascii[(i + 1) % 16] = '\0';
				if ((i + 1) % 16 <= 8) {
					DiagPrintf(" ");
				}
				for (j = (i + 1) % 16; j < 16; ++j) {
					DiagPrintf("   ");
				}
				DiagPrintf("|  %s\n", ascii);
			}
		}
	}
}

extern void generate_crc_table(void);
extern unsigned long crcbitbybitfast(unsigned char *p, unsigned long len);

static const char *TAG = "SD_IO";


#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD

static int vfs_init_remap_done = 0;


int vfs_init_remap(void)
{
	int ret = 0;

	if (vfs_init_remap_done) {
		RTK_LOGI(TAG, "VFS-FAT had initialized\n");
		return 0;
	}

	u32 temp = HAL_READ32(0x6010c30c, 0x00);//Switch LX BUS to secure region
	HAL_WRITE32(0x6010c30c, 0X00, temp & ~BIT27);

	vfs_init();
	ret = vfs_target_register("fat", VFS_TARGET_SD, VFS_RW);
	if (ret == 0) {
		RTK_LOGI(TAG, "VFS-FAT Init Success \n");
		vfs_init_remap_done = 1;
	} else {
		RTK_LOGI(TAG, "VFS-FAT Init Fail \n");
	}
	return ret;
}




#endif


/*
 * surface 0: load to src surface
 * surface 1: load to dst surface
 */
int surface_loadpic(struct ge_context *pge_context, char *FilePath,  enum SURFACE_NO sno)
{
	FILE *fp;
	int rlen, flen;
	uint32_t current_tick, pre_tick;
	unsigned long crc32;

	pre_tick = xTaskGetTickCount();

	fp = fopen(FilePath, "rb");
	if (fp == NULL) {
		DiagPrintf("Open %s fail\n", FilePath);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	DiagPrintf("file len %d\n", flen);

	if (flen > pge_context->surface[sno].size) {
		flen = pge_context->surface[sno].size;
		DiagPrintf("flen is larger buffer size, only read len %d\n", flen);
	}
	if (flen == 0) {
		DiagPrintf("%s didn't existed\n", FilePath);
		fclose(fp);
		return -1;
	}

	pge_context->surface[sno].file_len = flen;

	pre_tick = xTaskGetTickCount();

	rlen = fread(pge_context->surface[sno].phy_addr, flen, 1, fp);

	if (rlen != flen) {
		DiagPrintf("fread failed read %d target %d \n", rlen, flen);
		return -1;
	} else {
		DiagPrintf("fread OK \n");
	}

	fclose(fp);

	current_tick = xTaskGetTickCount();



	generate_crc_table();
	crc32 = crcbitbybitfast((unsigned char *)pge_context->surface[sno].phy_addr, flen);

	DiagPrintf("real read %s content %d time %d crc32 %u\n \n", FilePath, rlen, current_tick - pre_tick, crc32);


	hexdump(pge_context->surface[sno].phy_addr, 16);

	return 0;
}

