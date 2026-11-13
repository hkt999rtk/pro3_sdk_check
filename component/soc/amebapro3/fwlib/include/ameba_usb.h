/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_USB_H
#define _AMEBA_USB_H

/* Includes ------------------------------------------------------------------*/

#include "ameba_vector.h"
#include "basic_types.h"
#include "hal_platform.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup USB
* @brief USB driver modules
* @{
*/

/** @defgroup USB_Register_Definitions USB Register Definitions
 * @{
 *****************************************************************************/

//#define USB_REG_BASE
#define USB_ADDON_REG_BASE                                  (USB_REG_BASE + 0x30000UL)

/** @defgroup USB_ADDON_REG_PWR_CTRL
 * @brief USB AddOn PHY Power Control Register
 * @{
 **/

#define USB_ADDON_REG_PWR_CTRL                              (USB_ADDON_REG_BASE + 0x04UL)
/** @} */

/** @defgroup USB_ADDON_REG_AUTOLOAD_CTRL
 * @brief USB AddOn Autoload Control Register
 * @{
 **/

#define USB_ADDON_REG_AUTOLOAD_CTRL                         (USB_ADDON_REG_BASE + 0xA4UL)

#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM           BIT(0)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP       BIT(1)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN       BIT(2)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_UPHY_EN    BIT(3)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN    BIT(4)
#define USB_ADDON_REG_AUTOLOAD_CTRL_BIT_BT_INTR_INTERVAL    BIT(5)

/* USB OTG addon autoload registers */
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0E0                    (USB_ADDON_REG_BASE + 0xE0UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0E4                    (USB_ADDON_REG_BASE + 0xE4UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P1E0                    (USB_ADDON_REG_BASE + 0xE8UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P1E4                    (USB_ADDON_REG_BASE + 0xECUL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0F0                    (USB_ADDON_REG_BASE + 0xF0UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P0F4                    (USB_ADDON_REG_BASE + 0xF4UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P1F4                    (USB_ADDON_REG_BASE + 0xF8UL)
#define USB_ADDON_REG_AUTOLOAD_UPHY_P2E4                    (USB_ADDON_REG_BASE + 0xFCUL)
/** @} */

/** @} */

/** @defgroup USB_Exported_Constants USB Exported Constants
  * @{
  */

/** @defgroup USB_Hardware_Configurations
  * @{
  */
#define CONFIG_USB_OTG										0U
#define CONFIG_USB_PHY										1U

#define USB_MAX_ENDPOINTS									8U
#define USB_MAX_PIPES										8U
#define USB_IN_TOKEN_QUEUE_DEPTH							8U
/** @} */

/** @defgroup USB_ID
  * @{
  */
#define USB_VID												0x0BDA
#define USB_PID												0x8006
/** @} */

/** @defgroup USB_IRQ_Configurations
  * @{
  */
#define USB_IRQ												USBOTG_IRQ
#define USB_IRQ_PRI											INT_PRI_MIDDLE
/** @} */

/* Exported types ------------------------------------------------------------*/

/** @defgroup USB_ADDON_PWR_CTRL_TypeDef
  * @{
  */
typedef union {
	u32 w;                         /*!< Type      used for word access */
	struct {
		u32 pwr_sav_en: 1;         /*!< bit:   0       power saving enable */
		u32 susp_dis: 1;           /*!< bit:   1       suspend signal disable */
		u32 susp_to_one: 1;        /*!< bit:   2       suspend signal to 1 */
		u32 rsvd: 1;               /*!< bit:   3       reserved */
		u32 suspendm: 1;           /*!< bit:   4       suspendm signal (RO) */
		u32 phy_clk_rdy: 1;        /*!< bit:   5       PHY clock ready (RO) */
		u32 ls: 2;                 /*!< bit:   6 to 7  line state (RO) */
		u32 otg_co_en: 1;          /*!< bit:   8       enable otg core */
		u32 phy_en: 1;             /*!< bit:   9       enable PHY */
		u32 gate_ls_en: 1;         /*!< bit:   10      enable gate USB line state function */
	} b;
} usbaddon_pwr_ctrl_t, *pusbaddon_pwr_ctrl_t;

#define USB_ADDON_PWR_CTRL                                  ((volatile usbaddon_pwr_ctrl_t *)USB_ADDON_REG_PWR_CTRL)
/** @} */

typedef IRQ_FUN usb_irq_fun_t;
/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} */

/** @} */

/** @} */
#define SIM_ROM_STARTED                    BIT(0)
#define SIM_BSS_CLEAR                      BIT(1)
#define SIM_OSC_ENABLE                     BIT(2)
#define SIM_SPIC_INIT                      BIT(3)
#define SIM_USBD_START                     BIT(4)
#define SIM_USB_INIT                       BIT(5)
#define SIM_USB_PCD_INIT                   BIT(6)
#define SIM_USB_PHY_INIT                   BIT(7)
#define SIM_USB_HAL_INIT                   BIT(8)
#define SIM_USB_DEV_INIT                   BIT(9)
#define SIM_USB_CAL                        BIT(10)
#define SIM_USB_CAL_PAGE0_DONE             BIT(11)
#define SIM_USB_CAL_PAGE1_DONE             BIT(12)
#define SIM_USB_INT_INIT                   BIT(13)
#define SIM_USB_START                      BIT(14)
#define SIM_USB_PROCESS_START              BIT(15)
#define SIM_USB_INIT_SUSP                  BIT(16)
#define SIM_USB_INIT_RST                   BIT(17)
#define SIM_USB_INIT_ENUMDNE               BIT(18)
#define SIM_USB_ENUM_GET_DEV_DESC  	       BIT(19)
#define SIM_USB_INIT_FAIL                  BIT(20)
#define SIM_USB_PROCESS_DONE               BIT(21)
#define SIM_USB_PROCESS_ABORTED    	       BIT(22)
#define SIM_USB_PROCESS_DEINIT             BIT(23)
#define SIM_USB_DEINIT_DONE                BIT(24)
#define SIM_DOWNLOAD_DONE                  BIT(25)
#define SIM_USB_DUMP                       BIT(30)
#define SIM_USB_ENUM_SET_ADDR              BIT(31)

#define USBD_SIM(x)  	BKUP_Set(BKUP_REG4, x)

#endif
