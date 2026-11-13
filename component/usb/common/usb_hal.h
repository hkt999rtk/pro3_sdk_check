/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_HAL_H
#define USB_HAL_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"
#include "usb_def.h"
#include "basic_types.h"
#include "ameba_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 *  @{
 */
/** @defgroup USB_Common_HAL_Constants USB HAL Constants
 *  @brief Constants of the DWC OTG core HAL shared by the device and host cores.
 *  @{
 */

#define USB_IN_TOKEN_QUEUE_DEPTH         8U  /**< Depth of the IN token queue for periodic transfers (Shared FIFO mode only). */

/** @} End of USB_Common_HAL_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */
/** @defgroup USB_Common_HAL_Types USB HAL Types
 *  @brief Types of the DWC OTG core HAL and of the SoC adapter interface.
 *  @{
 */

/**
 * @brief USB mode.
 */
typedef enum {
	USB_OTG_MODE_DEVICE = 0,            /**< USB device mode */
	USB_OTG_MODE_HOST                   /**< USB host mode */
} usb_otg_mode_t;

/**
 * @brief Defines the status of the USB device bus lines (D+ and D-).
 */
typedef enum {
	USB_DEV_BUS_STATUS_DN = BIT0,       /**< D- line status bit. */
	USB_DEV_BUS_STATUS_DP = BIT1,       /**< D+ line status bit. */
	USB_DEV_BUS_STATUS_SUSPEND = BIT2,  /**< Suspend indication bit. */
} usb_dev_bus_state_t;

/**
 * @brief Structure to hold USB calibration data for a PHY register.
 */
typedef struct {
	u8 page;        /**< Page number for the PHY register. */
	u8 addr;        /**< Register address. */
	u8 val;         /**< Value to be written to the register. */
} usb_cal_data_t;

/**
 * @brief Structure defining the platform-specific HAL driver interface.
 * @details This structure holds pointers to functions that are implemented
 *          at the platform level to control the USB hardware.
 */
typedef struct {
	/**
	 * @brief Initializes the underlying SoC-specific hardware for the USB controller.
	 * @param[in] mode: The USB mode to initialize, see @ref usb_otg_mode_t.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(u8 mode);

	/**
	 * @brief De-initializes the underlying SoC-specific hardware.
	 * @note  Teardown only, it cannot fail.
	 */
	void (*deinit)(void);

	/**
	 * @brief Enables the USB interrupt with a specified priority.
	 * @param[in] priority: The interrupt priority to set.
	 */
	void (*enable_interrupt)(u8 priority);

	/**
	 * @brief Disables the USB interrupt.
	 */
	void (*disable_interrupt)(void);

	/**
	 * @brief Registers the ISR handler for USB interrupts.
	 * @param[in] handler: Pointer to the ISR function.
	 * @param[in] priority: The interrupt handler priority to set.
	 */
	void (*register_irq_handler)(void *handler, u8 priority);

	/**
	 * @brief Unregisters the USB ISR handler.
	 */
	void (*unregister_irq_handler)(void);

	/**
	 * @brief Retrieves the SoC-specific USB PHY calibration data for a specific USB mode.
	 * @param[in] mode: The USB mode for which to get calibration data, see ` USB_OTG_MODE_XX`
	 * @return Pointer to an array of `usb_cal_data_t` structures.
	 */
	const usb_cal_data_t *(*get_cal_data)(u8 mode);

	/**
	 * @brief Register the sleep callbacks for Clock Gating (CG) power management.
	 * @note  Wakelock operations are handled by the caller, not inside this callback.
	 */
	void (*cg_register)(void);

	/**
	 * @brief Unregister the sleep callbacks for Clock Gating (CG) power management.
	 * @note  Wakelock operations are handled by the caller, not inside this callback.
	 */
	void (*cg_unregister)(void);
} usb_hal_driver_t;

/** @} End of USB_Common_HAL_Types group */
/** @} End of USB_Common_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/** @addtogroup USB_Common_Variables USB Common Variables
 *  @{
 */
/** @defgroup USB_Common_HAL_Variables USB HAL Variables
 *  @brief Variables of the DWC OTG core HAL.
 *  @{
 */

/** @brief The SoC adapter implementing @ref usb_hal_driver_t, defined by the matching `hal/<soc>_usb.c`. */
extern const usb_hal_driver_t usb_hal_driver;

/** @} End of USB_Common_HAL_Variables group */
/** @} End of USB_Common_Variables group */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Common_Functions USB Common Functions
 *  @{
 */
/** @defgroup USB_Common_HAL_Functions USB HAL Functions
 *  @brief Core and PHY operations of the DWC OTG HAL, used by the device and host cores.
 *  @{
 */

/**
 * @brief  Initialize the USB core.
 * @param  dma_enable: 1 to run the core in DMA mode, 0 for slave (FIFO) mode.
 * @return RTK_SUCCESS on success, -RTK_ERR_TIMEOUT if the core reset did not complete.
 */
int usb_hal_core_init(u8 dma_enable);

/**
 * @brief  Enable the controller's global interrupt in the AHB configuration register.
 * @return None
 */
void usb_hal_enable_global_interrupt(void);

/**
 * @brief  Disable the controller's global interrupt in the AHB configuration register.
 * @return None
 */
void usb_hal_disable_global_interrupt(void);

/**
 * @brief  Read the global USB interrupt status and clear the reported bits.
 * @return The global USB interrupt status that was pending before the clear.
 */
u32 usb_hal_read_clear_interrupts(void);

/**
 * @brief  Clear the given USB global interrupt flags.
 * @param  interrupt: Bitmask of the interrupt flags to clear.
 * @return None
 */
void usb_hal_clear_interrupts(u32 interrupt);

/**
 * @brief  Set the OTG functional mode.
 * @param  mode: The mode to enter, see @ref usb_otg_mode_t.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG for an unknown mode, -RTK_ERR_TIMEOUT if the
 *         mode change was not acknowledged.
 */
int usb_hal_set_otg_mode(usb_otg_mode_t mode);

/**
 * @brief  Get the current OTG functional mode of the core.
 * @return The current mode, see @ref usb_otg_mode_t.
 */
usb_otg_mode_t usb_hal_get_otg_mode(void);

/**
 * @brief  Flush a TX FIFO.
 * @param  num: FIFO number, 0 to 15; 16 flushes all TX FIFOs.
 * @return RTK_SUCCESS on success, -RTK_ERR_TIMEOUT if the flush did not complete.
 */
int usb_hal_flush_tx_fifo(u32 num);

/**
 * @brief  Flush the RX FIFO.
 * @return RTK_SUCCESS on success, -RTK_ERR_TIMEOUT if the flush did not complete.
 */
int usb_hal_flush_rx_fifo(void);

/**
 * @brief  Set the RX FIFO depth.
 * @param  size: RX FIFO depth in DWORDs.
 */
void usb_hal_set_rx_fifo(u16 size);

/**
 * @brief  Set the non-periodic TX FIFO depth.
 * @param  size: Non-periodic TX FIFO depth in DWORDs.
 */
void usb_hal_set_np_tx_fifo(u16 size);

/**
 * @brief  Set a periodic TX FIFO depth.
 * @param  fifo: Periodic TX FIFO number, 1 to 15 in device mode, ignored in host mode.
 * @param  size: FIFO depth in DWORDs.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG for a FIFO number out of range in device mode.
 */
int usb_hal_set_ptx_fifo(u8 fifo, u16 size);

/**
 * @brief  Write a packet into the TX FIFO of an endpoint or host channel.
 * @param  src: Source buffer.
 * @param  ep_ch_num: Endpoint number (device) or host channel number.
 * @param  len: Number of bytes to write.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG if src is NULL.
 */
int usb_hal_write_packet(const u8 *src, u8 ep_ch_num, u16 len);

/**
 * @brief  Read a packet from the RX FIFO of an endpoint or host channel.
 * @param  dest: Destination buffer, which must be large enough for len bytes.
 * @param  ep_ch_num: Endpoint number (device) or host channel number.
 * @param  len: Number of bytes to read.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG if dest is NULL.
 */
int usb_hal_read_packet(u8 *dest, u8 ep_ch_num, u16 len);

/**
 * @brief  Get the device bus line status.
 * @param  bus_status: Output, receives a bitmask of @ref usb_dev_bus_state_t values.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG if bus_status is NULL.
 */
int usb_hal_get_device_bus_status(u32 *bus_status);

/**
 * @brief  Control the device soft-disconnect state.
 * @param  en: 1 to assert soft disconnect (detach from the bus), 0 to release it.
 * @return None
 */
void usb_hal_device_disconnect(u8 en);

/**
 * @brief  Dump the USB controller registers for debugging.
 * @return None
 */
void usb_hal_dump_registers(void);

#ifndef CONFIG_SUPPORT_USB_NO_PHY

/**
 * @brief  Read a USB PHY register.
 * @param  addr: PHY register address.
 * @param  value: Output, receives the register value.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG if value is NULL, -RTK_ERR_TIMEOUT on PHY access timeout.
 */
int usb_hal_read_phy_register(u8 addr, u8 *value);

/**
 * @brief  Write a USB PHY register.
 * @param  addr: PHY register address.
 * @param  value: Value to write.
 * @return RTK_SUCCESS on success, -RTK_ERR_TIMEOUT on PHY access timeout.
 */
int usb_hal_write_phy_register(u8 addr, u8 value);

/**
 * @brief  Apply the SoC PHY calibration data for the given USB mode.
 * @param  mode: The USB mode to calibrate for, see @ref usb_otg_mode_t.
 * @return RTK_SUCCESS on success, -RTK_ERR_BADARG if the SoC supplies no calibration data.
 */
int usb_hal_calibrate(u8 mode);

#ifdef CONFIG_USB_OTG_VERIFY
/**
 * @brief  Enable the OTG function in the PHY.
 * @note   For test use only.
 * @return RTK_SUCCESS on success, -RTK_ERR_TIMEOUT on PHY access timeout.
 */
int usb_hal_enable_otg(void);
#endif

#endif /* CONFIG_SUPPORT_USB_NO_PHY */

/** @} End of USB_Common_HAL_Functions group */
/** @} End of USB_Common_Functions group */

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_HAL_H */
