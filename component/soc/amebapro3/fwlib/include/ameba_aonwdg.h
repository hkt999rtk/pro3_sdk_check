/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_AONWDG_H_
#define _AMEBA_AONWDG_H_

_LONG_CALL_ void AONWDG_Init(u32 aon_wdt_timer_id, u32 timeout_ms);
_LONG_CALL_ void AONWDG_Enable(u32 aon_wdt_timer_id);
_LONG_CALL_ void AONWDG_Stop(u32 aon_wdt_timer_id);
_LONG_CALL_ void AONWDG_Refresh(u32 aon_wdt_timer_id);

#endif

/** @} */

/** @} */
