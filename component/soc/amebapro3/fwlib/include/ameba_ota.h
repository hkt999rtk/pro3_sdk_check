/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_OTA_H_
#define _AMEBA_OTA_H_

#include <stdlib.h>
/**
  * @brief  OTA Image ID
  */
#define OTA_IMGID_BOOT 0  /* Bootloader */
#define OTA_IMGID_APP  1  /* Application */
#if defined(CONFIG_AMEBAPRO3) && defined(CONFIG_LINUX_FW_EN)
#define OTA_IMGID_APIMG 2  /* Linux AP Image */
#define OTA_IMGID_MAX   3
#else
#define OTA_IMGID_MAX   2
#endif

#define MAX_IMG_NUM    OTA_IMGID_MAX

u8 ota_get_cur_index(u8 img_id);
u8 ota_check_bootloader_ota2(void);

#endif //_AMEBA_OTA_H_
