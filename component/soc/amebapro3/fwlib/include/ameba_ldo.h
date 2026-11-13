/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_LDO_H_
#define _AMEBA_LDO_H_

/** @addtogroup AmebaPro3_Platform
  * @{
  */


/** @defgroup LDO Exported Functions
  * @{
  */
void LDO_PSRAMPowerOn(void);
void LDO_PSRAMPowerOff(void);
void LDO_SDIOPowerOn(void); // output SDIO LDO 3.3V or 1.8V depending on CONFIG_MP_INCLUDED flag
void LDO_SDIOPower_Select(u8 mode); // use this to switch between 1.8V <--> 3.3V signaling
void LDO_SDIOPowerOff(void);
void LDO_WLONPowerOn(void);
void LDO_WLONPowerOff(void);

// For SDIO LDO **only**
// Because SDIO (SD Host to be precise) needs support both 3.3V (3.15V) and 1.8V signaling
#define SDIO_HOST_3V3 1
#define SDIO_HOST_1V8 0

/**
  * @}
  */

/**
  * @}
  */

#endif
