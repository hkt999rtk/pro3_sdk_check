/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SD_H
#define _AMEBA_SD_H

/*
 * Unified SD driver header — includes both the SD host controller
 * and SD card protocol headers.
 *
 *   ameba_sd_host.h — SD host controller (SDIOH) registers and API
 *   ameba_sd_card.h — SD card protocol, card init, read/write API
 */

#include "ameba_sd_host.h"
#include "ameba_sd_card.h"
#include "ameba_emmc_card.h"

/** @brief SD_Adapter — top-level driver state */
typedef struct {
	SD_CardInfo card_info;
	SD_HostCfg_TypeDef *host_cfg;
	const SD_HOST_OPS_TypeDef *host_ops;	/*!< Host controller ops vtable */
	u8 force_33v;							/*!< 1 = skip 1.8V, stay 3.3V */
	int (*sema_take)(u32);
	int (*sema_give_isr)(u32);
} SD_Adapter;

/** @brief SD bus pin map — one field per bus role (see board schematic) */
typedef struct {
	u8 clk;
	u8 d0;
	u8 d1;
	u8 d2;
	u8 d3;
	u8 cmd;
	u8 cd;
} SD_PinConfig;

extern SD_Adapter sd_adapter;
extern SDIOHCFG_TypeDef sdioh_config;
extern const SD_PinConfig sd_pins;

/* Millisecond delay (weak, override to yield to the scheduler) */
_LONG_CALL_ void SD_DelayMs(u32 ms);

/* Default ISR callbacks (weak, override for custom behavior) */
_LONG_CALL_ void SD_CardInsertISR(void);
_LONG_CALL_ void SD_CardRemoveISR(void);
_LONG_CALL_ void SD_TransferDoneISR(void);
_LONG_CALL_ void SD_TransferYieldDoneISR(void);

/* Hook registration */
_LONG_CALL_ void SD_card_insert_hook(void (*cd_callback)(void));
_LONG_CALL_ void SD_card_remove_hook(void (*cd_callback)(void));
_LONG_CALL_ void SD_transfer_done_hook(void (*transfer_done_callback)(void));
_LONG_CALL_ void SD_transfer_task_yield_hook(void (*transfer_task_yield_callback)(void));
_LONG_CALL_ void SD_SetSema(int (*sema_take_fn)(u32), int (*sema_give_isr_fn)(u32));
_LONG_CALL_ void SD_SetCdCallback(void (*cd_callback)(SD_RESULT));

/* Force 3.3V mode — call before SD_Init() to skip 1.8V negotiation */
_LONG_CALL_ void SD_Force33V(u8 enable);

/* Power and init/deinit */
_LONG_CALL_ SD_RESULT SD_Init(void);
_LONG_CALL_ SD_RESULT SD_DeInit(void);

/* Bus speed switching (orchestrates card CMD6 + host clock config) */
_LONG_CALL_ u32 SD_SwitchBusSpeed(u8 speed);

/* Card status / read / write / capacity */
_LONG_CALL_ SD_RESULT SD_Status(void);
_LONG_CALL_ SD_RESULT SD_ReadBlocks(u32 sector, u8 *data, u32 count);
_LONG_CALL_ SD_RESULT SD_WriteBlocks(u32 sector, const u8 *data, u32 count);
_LONG_CALL_ SD_RESULT SD_GetCapacity(u32 *sector_count);
_LONG_CALL_ SD_RESULT SD_Erase(u32 StartBlock, u32 EndBlock);
_LONG_CALL_ SD_RESULT SD_ReadMultiBlocks(u8 *readbuff, u32 BlockIdx, u32 NumberOfBlocks);
_LONG_CALL_ SD_RESULT SD_WriteMultiBlocks(const u8 *writebuff, u32 BlockIdx, u32 NumberOfBlocks);

#endif /* _AMEBA_SD_H */
