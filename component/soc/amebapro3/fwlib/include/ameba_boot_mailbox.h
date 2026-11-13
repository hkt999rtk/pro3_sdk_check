/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOT_MAILBOX_H_
#define _AMEBA_BOOT_MAILBOX_H_

#define MB_IDX_MAILBOX_SIZE 0
#define MB_IDX_FLASH_PARA 1
#define MB_IDX_FIP_SRCA	2
#define MB_IDX_FIP_DSTA 3
#define MB_IDX_FIP_SIZE 4
#define MB_IDX_CERT_RAM 5

#define MB_SIZE	32


#if defined(CONFIG_BOOTLOADER)
void BOOT_MAILBOX_Init(void);
void BOOT_MAILBOX_Write(u32 index, u32 value);
#endif

/* only read by application */
u32 BOOT_MAILBOX_Read(u32 index);

#endif
