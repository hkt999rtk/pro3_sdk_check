/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

#ifdef CONFIG_ARM_CORE_CM4_NP

#endif

u8 PERIPH_POWER_PIN = _PA_0;

SDIOHCFG_TypeDef sdioh_config = {
	.sdioh_bus_speed = SD_SPEED_HS, //SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, //SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
};

