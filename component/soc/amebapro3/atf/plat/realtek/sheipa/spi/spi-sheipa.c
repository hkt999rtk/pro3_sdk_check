/*
 * Copyright (c) 2015, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <arch_helpers.h>
#include <assert.h>
#include <common/debug.h>
#include <platform_def.h>
#include <stdint.h>
#include <string.h>
/* SPIC FLASH Headder */
#include "spi-sheipa.h"
#include "../sboot/sboot-sheipa.h"

MANIFEST g_manifest;
flash_init_t flash_init_para;
IMAGE_HEADER g_Hdr;

static void nor_memcpy(u8 *pData, u8 *HostAddr, size_t DataLen)
{
	u32 *addr = NULL;
	u32 *end = (u32 *)(HostAddr + DataLen);
	u32 *base = (u32 *)HostAddr;
	u32 *mem = (u32 *)pData;

	for (addr = base; addr < end; addr++) {
		*mem = *addr;
		mem++;
	}
}
#if defined(CONFIG_LOAD_CHECKSUM) && (CONFIG_LOAD_CHECKSUM == 1)
static u32 cal_sum(u8 *pData, size_t DataLen)
{
	u32 chksum = 0;
	u8 *dst = (u8 *)pData;
	for (u32 b = 0; b < DataLen; b++) {
		chksum += dst[b];
	}
	return chksum;
}
#endif
#if defined(CONFIG_LOAD_CRC32) && (CONFIG_LOAD_CRC32 == 1)
static u32 cal_crc32(u8 *pData, size_t DataLen)
{
	u8 *message = pData;
	u32 len = DataLen;
	int i, j;
	u32 byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	while (i < len) {
		byte = message[i];            // Get next byte.
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i = i + 1;
	}
	return ~crc;
}
#endif
static u32 cal_checksum(u8 *pData, size_t DataLen)
{
#if defined(CONFIG_LOAD_CHECKSUM) && (CONFIG_LOAD_CHECKSUM == 1)
	return cal_sum(pData, DataLen);
#elif defined(CONFIG_LOAD_CRC32) && (CONFIG_LOAD_CRC32 == 1)
	return cal_crc32(pData, DataLen);
#else
	return 0;
#endif
}

static void check_array(u8 *pData, u8 *HostAddr, size_t DataLen)
{
	NOTICE("NG DUMP START-----------------\n");
	for (u32 i = 0; i < DataLen; i++) {
		if (pData[i] != HostAddr[i]) {
			NOTICE("NG@%08x, src %02x, dst %02x\n", i, HostAddr[i], pData[i]);
		}
	}
	NOTICE("NG DUMP END-------------------\n");
}

static int flash_read(void)
{
	volatile u8 *fipbase;

	u32 *BKUP = NULL;
	u32 *MB = NULL;
	u32 chksum = 0;
	u32 chksum_nor = 0;
	u32 boot_src = 0;

	IMAGE_HEADER *Hdr = &g_Hdr;
	MANIFEST *manifest = &g_manifest;
	//
	BKUP = (u32 *)AON_BACKUP_BASE;
	MB = (u32 *)BKUP[5];

	if (MB[2] == 0) {
		ERROR("Invalid FIP source address\n");
		return -1;
	}
	NOTICE("fip srca %08x\n", MB[2]);

	fipbase = (volatile u8 *)(MB[2]);

	boot_src = BKUP[0] & BIT18;

	if (boot_src) {

		nor_memcpy((u8 *)manifest, (u8 *)fipbase, sizeof(MANIFEST));
		fipbase += 4096;
		nor_memcpy((u8 *)Hdr, (u8 *)fipbase, sizeof(IMAGE_HEADER));
		fipbase += 32;

		NOTICE("hdr image_addr %08x\n", Hdr->image_addr);
		NOTICE("hdr image_size %08x\n", Hdr->image_size);
		NOTICE("fip @ flash    %p\n", fipbase);

		nor_memcpy((u8 *)Hdr->image_addr, (u8 *)fipbase, Hdr->image_size);
		/* chksum_nor in log for developer debug */
		chksum_nor = cal_checksum((u8 *)fipbase, Hdr->image_size);
		NOTICE("cal in nor chksum %08x\n", chksum_nor);
	} else {
		extern void nand_copy(u8 * pData, u8 * HostAddr, size_t DataLen);
		nand_copy((u8 *)manifest, (u8 *)fipbase, sizeof(MANIFEST));
		fipbase += 4096;
		nand_copy((u8 *)Hdr, (u8 *)fipbase, sizeof(IMAGE_HEADER));

		NOTICE("hdr image_addr %08x\n", Hdr->image_addr);
		NOTICE("hdr image_size %08x\n", Hdr->image_size);
		NOTICE("fip @ flash    %p\n", fipbase);

		fipbase += 32;
		nand_copy((u8 *)Hdr->image_addr, (u8 *)fipbase, Hdr->image_size);
	}

	/* flush, necessary step, this api called after MMU/Cache enable, need flush for NS */
	flush_dcache_range((uint32_t)(Hdr->image_addr), (uint32_t)Hdr->image_size);

	chksum = cal_checksum((u8 *)Hdr->image_addr, Hdr->image_size);
	NOTICE("FIP HDR chksum %08x, cal chksum %08x\n", Hdr->reserved[0], chksum);

	if (chksum != 0 && Hdr->reserved[0] != chksum) {
		/* chksum is for debug, will not block boot */
		ERROR("FIP image chksum NG, maybe cause boot fail\n");
		if (boot_src) {
			check_array((u8 *)Hdr->image_addr, (u8 *)fipbase, Hdr->image_size);
		}
	}

	return 0;
}

int rtk_flash_read(void)
{
	u32 *BKUP = NULL;
	u32 *MB = NULL;
	//
	BKUP = (u32 *)AON_BACKUP_BASE;
	MB = (u32 *)BKUP[5];

	memcpy(&flash_init_para, (void *)(MB[1]), sizeof(flash_init_t));

	if (BKUP[0] & BIT18) {
		NOTICE("BOOT From NOR\n");
		extern void nor_init(void);
		nor_init();
	} else {
		NOTICE("BOOT From NAND\n");
		extern void nand_init(void);
		nand_init();
	}

	INFO("Read Secure Boot Image\n");
	if (flash_read() != 0) {
		ERROR("flash read fail\n");
		return -1;
	}

	if (sboot_is_en()) {
		CERTIFICATE *cert = (CERTIFICATE *)MB[5];
		MANIFEST *manifest = &g_manifest;
		IMAGE_HEADER *Hdr = &g_Hdr;

		if (sboot_validate_pubkey(manifest->auth_alg, manifest->pubkey, cert->pk_info[0].hash) != 0) {
			ERROR("pubkey verify fail");
			memset((u8 *)Hdr->image_addr, 0, Hdr->image_size);
			flush_dcache_range(Hdr->image_addr, Hdr->image_size);
			while (1);
		}
		if (sboot_validate_signature(manifest->auth_alg, manifest->hash_alg, manifest->pubkey, (u8 *)manifest, sizeof(MANIFEST) - SIGN_MAX_LEN,
									 manifest->signature) != 0) {
			ERROR("signature verify fail");
			memset((u8 *)Hdr->image_addr, 0, Hdr->image_size);
			flush_dcache_range(Hdr->image_addr, Hdr->image_size);
			while (1);
		}
		if (sboot_validate_img_hash(manifest->hash_alg, manifest->img_hash, Hdr) != 0) {
			ERROR("image hash verify fail");
			memset((u8 *)Hdr->image_addr, 0, Hdr->image_size);
			flush_dcache_range(Hdr->image_addr, Hdr->image_size);
			while (1);
		}

		INFO("SBOOT verified\n");
	}

	return 0;
}

