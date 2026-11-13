/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SOC_H_
#define _AMEBA_SOC_H_

#ifndef CONFIG_BUILD_ROM
/* ram headers */
#include "platform_autoconf.h"
#endif

/* rom headers */
#include "ameba.h"
//#include "strproc.h"
#ifdef FEATURE_TODO
// check ameba_pmu.h
#endif
#include "ameba_pmu.h"

#ifndef CONFIG_BUILD_ROM
/* ram headers */
//#include "ameba_pmu.h" // move out, TODO: check here
#include "ameba_ota.h"
#include "os_wrapper.h"
#endif

#endif //_AMEBA_SOC_H_
