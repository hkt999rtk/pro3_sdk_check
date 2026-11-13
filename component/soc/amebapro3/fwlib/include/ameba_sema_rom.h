/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SEMA_ROM_H_
#define _AMEBA_SEMA_ROM_H_

#include "ameba_ipc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup IPC
  * @brief IPC driver modules
  * @{
  */

/* Exported types --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Types IPC Exported Types
  * @{
  */
/**
  * @brief IPC SEM IDX
  */

#define IPC_SEM_CRYPTO_AES_SW_KEY   IPC_SEM_ID3
#define IPC_SEM_FLASH               IPC_SEM_ID4  /* cross-CPU flash controller lock (NP/VP/AP/FP) */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */

/** @defgroup SEMA_Exported_Functions Sema Exported Functions
  * @{
  */

void IPC_SEMAInit(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif

