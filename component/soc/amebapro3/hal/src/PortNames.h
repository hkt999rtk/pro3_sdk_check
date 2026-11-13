/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_PORTNAMES_H
#define MBED_PORTNAMES_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GPIO
 *  @brief    MBED_GPIO driver modules.
 *  @{
 */

/** @addtogroup MBED_GPIO_Exported_Types MBED_GPIO Exported Types
  * @{
  */
/** @defgroup MBED_GPIO_Enumeration_Type Enumeration Type
  * @{
  */

typedef enum {
	PORT_A          = 0,
	PORT_B          = 1,
	PORT_C          = 2,
	PORT_D          = 3,
	PORT_E          = 4,
	PORT_F          = 5,
	PORT_S          = 6,
	PORT_P        	= 7,
	PORT_MAX_NUM    = 8
} PortName;

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

#ifdef __cplusplus
}
#endif
#endif
