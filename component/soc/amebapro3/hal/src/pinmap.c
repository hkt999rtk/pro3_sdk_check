/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */
#include "objects.h"
#include "pinmap.h"

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_PINMAP
 *  @brief      MBED_PINMAP driver modules.
 *  @{
 */

/** @defgroup MBED_PINMAP_Exported_Functions MBED_PINMAP Exported Functions
  * @{
  */

/**
  *  @brief Configure pin function type.
  *  @param pin : value of @ref PinName.
  *  @param function : the function type for the pin, value of PINMUX_FUNCTION_XXX.
  *  @retval None
  */
void pin_function(PinName pin, int function)
{
	assert_param(pin != NC);

	Pinmux_Config((u8)pin, (u32)function);
}

/**
  *  @brief Configure pin pull-up/pull-down mode.
  *  @param pin : value of @ref PinName.
  *  @param mode : value of @ref PinMode.
  *  @retval None
  */
void pin_mode(PinName pin, PinMode mode)
{
	switch (mode) {
	case PullDown:
		PAD_PullCtrl((u8)pin, GPIO_PuPd_DOWN);
		break;

	case PullUp:
		PAD_PullCtrl((u8)pin, GPIO_PuPd_UP);
		break;

	case PullNoneSlp:
		PAD_SleepPullCtrl((u8)pin, GPIO_PuPd_NOPULL);
		break;

	case PullDownSlp:
		PAD_SleepPullCtrl((u8)pin, GPIO_PuPd_DOWN);
		break;

	case PullUpSlp:
		PAD_SleepPullCtrl((u8)pin, GPIO_PuPd_UP);
		break;

	case PullNone:
	default:
		PAD_PullCtrl((u8)pin, GPIO_PuPd_NOPULL);
		break;
	}
}

uint32_t pinmap_find_peripheral(PinName pin, const PinMap *map)
{
	while (map->pin != NC) {
		if (map->pin == pin) {
			return map->peripheral;
		}
		map++;
	}
	return (uint32_t)NC;
}

uint32_t pinmap_peripheral(PinName pin, const PinMap *map)
{
	uint32_t peripheral;

	if (pin == (PinName)NC) {
		return (uint32_t)NC;
	}

	peripheral = pinmap_find_peripheral(pin, map);
	if (peripheral == (uint32_t)NC) {
		RTK_LOGW(NOTAG, "pinmap not found\n");
	}

	return peripheral;
}

uint32_t pinmap_merge(uint32_t a, uint32_t b)
{
	if (a == b) {
		return a;
	}

	if (a == (uint32_t)NC) {
		return b;
	}
	if (b == (uint32_t)NC) {
		return a;
	}

	RTK_LOGW(NOTAG, "pinmap mis-match\n");

	return (uint32_t)NC;
}

void pinmap_pinout(PinName pin, const PinMap *map)
{
	if (pin == NC) {
		return;
	}

	while (map->pin != NC) {
		if (map->pin == pin) {
			pin_mode(pin, PullNone);
			pin_function(pin, map->function);
			return;
		}
		map++;
	}
}

/** @} */
/** @} */
/** @} */
