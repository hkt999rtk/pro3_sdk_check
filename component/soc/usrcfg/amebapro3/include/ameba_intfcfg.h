/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_INTCFG_NP_H_
#define _AMEBA_INTCFG_NP_H_

#ifdef CONFIG_ARM_CORE_CM4_NP

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED

extern const u8 ftl_phy_page_num;									/* The number of physical map pages, default is 3*/
extern const u32 ftl_phy_page_start_addr;					/* The start offset of flash pages which is allocated to FTL physical map.
																	Users should modify it according to their own memory layout!! */
#endif

#endif

#endif
