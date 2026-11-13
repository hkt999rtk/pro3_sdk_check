
/**
  ******************************************************************************
  * @file    ameba_gpioc
  * @author
  * @version V1.0.0
  * @date    2026-06-04
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the General Purpose Input Output (GPIO) peripheral:
  *           - Initialization and Configuration
  *           - GPIO Read and Write
  *           - GPIO Interrupts management
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "ameba_soc.h"
//HAL_ROM_DATA_SECTION GPIO_TypeDef *GPIO_PORTx[3] = {AON_GPIO_BASE, PON_GPIO_BASE, GPIO_BASE};
//HAL_ROM_DATA_SECTION GPIO_TypeDef *GPIO_PORTx_S[3] = {AON_GPIO_BASE, PON_GPIO_BASE, GPIO_BASE}; // NOT Secure ADDR! Temp!
static const u8 gpio_pin_idx_offset_groupA[8] = {0, 0, 0, 11, 21, 5, 0xFF, 21}; // L to R - GPIO A,B,C,D,E,F,S,P
static const u8 gpio_pin_idx_offset_groupB[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0, 0xFF, 6, 0}; // L to R - GPIO A,B,C,D,E,F,S,P
static u8 aon_gpio_irq_in_use_idx = 0; // tracks which AON GPIO pin is using the interrupt idx (max: 6 interrupts)
static u8 syson_gpio_irq_in_use_idx = 0; // tracks which SYSON GPIO pin is using the interrupt idx (max: 16 interrupts)
static u8 core_gpio_irq_in_use_idx = 0; // tracks which CORE GPIO pin is using the interrupt idx (max: 16 interrupts)
// An array that tracks what GPIO pins are used as IRQ pins
// u16 for the sake of initializing pin list to GPIO_IRQ_PIN_TRACKER_UNUSED
// Initial value GPIO_IRQ_PIN_TRACKER_UNUSED means the slot is not occupied
// This also means, the pin_idx, which is to be used for telling the interrupts will follow the array too
// pin_idx should be (array_pos + 1).
static u16 aon_gpio_irq_pin_tracker[MAX_Aon_GPIO_IRQ] = {
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
};
static u16 syson_gpio_irq_pin_tracker[MAX_GPIO_IRQ] = {
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
};
static u16 core_gpio_irq_pin_tracker[MAX_GPIO_IRQ] = {
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
	GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED, GPIO_IRQ_PIN_TRACKER_UNUSED,
};

static struct GPIO_IrqContext Aon_GPIO_IrqTable[MAX_Aon_GPIO_IRQ] = {0};
static struct GPIO_IrqContext Syson_GPIO_IrqTable[MAX_GPIO_IRQ] = {0};
static struct GPIO_IrqContext Core_GPIO_IrqTable[MAX_GPIO_IRQ] = {0};

/* Returns the GPIO base address for the given port, accounting for TrustZone. */
static GPIO_TypeDef *GPIO_GetBase(u8 port_num)
{
	if (TrustZone_IsSecure()) {
		if (port_num == GPIO_PORT_A) {
			return AON_GPIO_BASE_S;
		} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
			return PON_GPIO_BASE_S;
		} else {
			return GPIO_BASE_S;
		}
	} else {
		if (port_num == GPIO_PORT_A) {
			return AON_GPIO_BASE;
		} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
			return PON_GPIO_BASE;
		} else {
			return GPIO_BASE;
		}
	}
}

/* Searches the IRQ pin tracker for GPIO_Pin and returns its index, or 0xFF if not found. */
static u8 GPIO_FindPinIdx(u32 GPIO_Pin, u8 port_num)
{
	u8 idx;
	u32 irq_flags = irq_disable_save();
	if (port_num == GPIO_PORT_A) {
		for (idx = 0; idx < MAX_Aon_GPIO_IRQ; idx++) {
			if (aon_gpio_irq_pin_tracker[idx] == GPIO_Pin) {
				irq_enable_restore(irq_flags);
				return idx;
			}
		}
	} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
		for (idx = 0; idx < MAX_GPIO_IRQ; idx++) {
			if (syson_gpio_irq_pin_tracker[idx] == GPIO_Pin) {
				irq_enable_restore(irq_flags);
				return idx;
			}
		}
	} else {
		for (idx = 0; idx < MAX_GPIO_IRQ; idx++) {
			if (core_gpio_irq_pin_tracker[idx] == GPIO_Pin) {
				irq_enable_restore(irq_flags);
				return idx;
			}
		}
	}
	irq_enable_restore(irq_flags);
	return 0xFF;
}

/* Clears a pin from the IrqTable and its tracker slot.
 * pin_tracker is the primary key: it is always set by GPIO_AllocTrackerSlot,
 * even when GPIO_UserRegIrq was never called (IrqIdleEntryFlag stays 0). */
static void GPIO_UnRegFromTable(u32 GPIO_Pin, struct GPIO_IrqContext *IrqTable,
								u16 *pin_tracker, u32 max_irq, u8 *in_use_idx)
{
	u32 irq_flags = irq_disable_save();
	u32 idx;
	u8 warn_no_entry = 0;
	u8 err_underflow  = 0;
	for (idx = 0; idx < max_irq; idx++) {
		if (pin_tracker[idx] != (u16)GPIO_Pin) {
			continue;
		}

		pin_tracker[idx] = GPIO_IRQ_PIN_TRACKER_UNUSED;
		if (IrqTable[idx].IrqIdleEntryFlag == GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER) {
			IrqTable[idx].IrqIdleEntryFlag = 0x0;
			__DMB();
			IrqTable[idx].IrqPinName    = 0xFF;
			IrqTable[idx].UsrIrqHandler = (GPIO_IRQ_FUN)NULL;
			IrqTable[idx].UsrIrqData    = NULL;
		} else {
			warn_no_entry = 1;
		}
		if (*in_use_idx == 0) {
			err_underflow = 1;
		} else {
			(*in_use_idx)--;
		}
		irq_enable_restore(irq_flags);
		if (warn_no_entry) {
			RTK_LOGW("GPIO", "GPIO_Pin 0x%x: freed tracker slot with no IrqTable entry (GPIO_UserRegIrq never called)\r\n", GPIO_Pin);
		}
		if (err_underflow) {
			RTK_LOGE("GPIO", "in_use_idx underflow, state inconsistency detected\r\n");
		}
		return;
	}
	irq_enable_restore(irq_flags);
}

/* Resolves port_num + pin_num into the hardware group and actual bit position.
 * Sets *use_groupB=1 for Group B registers, 0 for Group A.
 * Sets *actual_pin to the bit index within that group's 32-bit register. */
static void GPIO_GetPinLocation(u8 port_num, u8 pin_num, u8 *use_groupB, u8 *actual_pin)
{
	if ((port_num == GPIO_PORT_E || port_num == GPIO_PORT_P) && pin_num > 10) {
		*use_groupB = 1;
		*actual_pin = gpio_pin_idx_offset_groupB[port_num] + pin_num - 11;
	} else if (port_num == GPIO_PORT_S) {
		*use_groupB = 1;
		*actual_pin = gpio_pin_idx_offset_groupB[port_num] + pin_num;
	} else {
		*use_groupB = 0;
		*actual_pin = gpio_pin_idx_offset_groupA[port_num] + pin_num;
	}
}

/* Finds or allocates a tracker slot for GPIO_Pin; updates tracker and in_use_idx. Returns pin_idx (0xFF if full). */
static u8 GPIO_AllocTrackerSlot(u32 GPIO_Pin, u16 *pin_tracker, u8 max_irq, u8 *in_use_idx)
{
	u8 pin_idx;
	u8 free_idx;
	u32 irq_flags = irq_disable_save();

	free_idx = max_irq; /* first idle (GPIO_IRQ_PIN_TRACKER_UNUSED) slot found, max_irq = none yet */

	/* Special implementation for AON GPIO
	 * Because AON GPIO is used as a wake source, a fixed 1:1 mapping is used for ALL pins:
	 * GPIO A0 <===> INT0
	 * GPIO A1 <===> INT1
	 * GPIO A2 <===> INT2
	 * GPIO A3 <===> INT3
	 * GPIO A4 <===> INT4
	 * GPIO A5 <===> INT5
	 */

	if (pin_tracker == aon_gpio_irq_pin_tracker) {
		pin_idx = PIN_NUM(GPIO_Pin);  // PA0..PA5 map directly to slot 0..5

		if (pin_idx >= max_irq) {
			irq_enable_restore(irq_flags);
			RTK_LOGE("GPIO", "AON pin 0x%x out of range, no fixed slot\r\n", GPIO_Pin);
			return 0xFF;
		}

		if (pin_tracker[pin_idx] == GPIO_Pin) {
			irq_enable_restore(irq_flags);
			return pin_idx;  // already owns this slot, counter unchanged
		}
		if (pin_tracker[pin_idx] != GPIO_IRQ_PIN_TRACKER_UNUSED) {
			irq_enable_restore(irq_flags);
			RTK_LOGE("GPIO", "AON pin hard-fixed slot %d already occupied by 0x%x\r\n",
					 pin_idx, pin_tracker[pin_idx]);
			return 0xFF;
		}

		pin_tracker[pin_idx] = GPIO_Pin;
		(*in_use_idx)++;
		irq_enable_restore(irq_flags);
		return pin_idx;
	}

	/* step1: if pin already owns a slot, reuse it WITHOUT bumping the counter */
	for (pin_idx = 0; pin_idx < max_irq; pin_idx++) {
		if (pin_tracker[pin_idx] == GPIO_Pin) {
			irq_enable_restore(irq_flags);
			return pin_idx; /* already tracked — in_use_idx unchanged */
		}
		if ((free_idx == max_irq) && (pin_tracker[pin_idx] == GPIO_IRQ_PIN_TRACKER_UNUSED)) {
			free_idx = pin_idx; /* remember first idle slot, keep scanning for a dup */
		}
	}

	/* step2: pin not tracked yet — occupy the first idle slot */
	if (free_idx == max_irq) {
		irq_enable_restore(irq_flags);
		RTK_LOGE("GPIO", "No idle tracker slot for pin 0x%x, please enlarge the IRQ table size!!!\r\n", GPIO_Pin);
		return 0xFF; /* table full */
	}
	pin_tracker[free_idx] = GPIO_Pin;
	(*in_use_idx)++;
	irq_enable_restore(irq_flags);
	return free_idx;
}

void GPIO_INTHandler_internal(GPIO_TypeDef *GPIO, struct GPIO_IrqContext *IrqTable, u8 max_irq_pins);

u32 GPIO_INTHandler(void *pData)
{
	GPIO_TypeDef *gpio_base = (GPIO_TypeDef *)pData;
	if (gpio_base == NULL) {
		return (u32)gpio_base;
	}

	if (gpio_base == AON_GPIO_BASE || gpio_base == AON_GPIO_BASE_S) {
		GPIO_INTHandler_internal(gpio_base, Aon_GPIO_IrqTable, MAX_Aon_GPIO_IRQ);
	} else if (gpio_base == PON_GPIO_BASE || gpio_base == PON_GPIO_BASE_S) {
		GPIO_INTHandler_internal(gpio_base, Syson_GPIO_IrqTable, MAX_GPIO_IRQ);
	} else {
		GPIO_INTHandler_internal(gpio_base, Core_GPIO_IrqTable, MAX_GPIO_IRQ);
	}

	return (u32)gpio_base;
}

void GPIO_INTHandler_internal(GPIO_TypeDef *GPIO, struct GPIO_IrqContext *IrqTable, u8 max_irq_pins)
{
	u32 int_sts;
	u32 i, event, state;
	u8 use_groupB, actual_pin;
	u8 port_num, pin_num;
	u32 ip_sts, polarity;
	// Store the int_sts, then clear all specific interrupt bit later, then continue searching, see if got more interrupts
	int_sts = GPIO->GPIO_INT_STS;
	ip_sts = GPIO->GPIO_IP_STS; /* valid for all domains; AON uses bits[11:0] (6 pins x 2), loop bound keeps i in range */
	// Find pin_name of IRQ pin asserting interrupt, via int_sts
	for (i = 0; i < max_irq_pins; i++) {
		if (int_sts & BIT(i)) { // IRQ pin found
			if (IrqTable[i].IrqIdleEntryFlag == GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER) {/* fix for entry PinName initial value is 0x0 */
				if (IrqTable[i].UsrIrqHandler != NULL) {
					/* Single-polarity edge: direction known from IP_STS, immune to short pulses.
					 * BOTHEDGE (polarity==2) or level: fall back to DP_STS sampling. */
					polarity = (ip_sts >> (i * 2)) & 0x3u;
					if (polarity != 2u) {
						/* 00=rising, 01=falling; direction unambiguous regardless of pulse width */
						event = (polarity == 0u ? HAL_IRQ_RISE : HAL_IRQ_FALL) | ((u32)(IrqTable[i].IrqPinName) << 16);
					} else {
						/* BOTHEDGE or level: must read current pin level */
						port_num = PORT_NUM(IrqTable[i].IrqPinName);
						pin_num  = PIN_NUM(IrqTable[i].IrqPinName);
						GPIO_GetPinLocation(port_num, pin_num, &use_groupB, &actual_pin);
						state = ((use_groupB ? GPIO->GPIOB_DP_STS : GPIO->GPIOA_DP_STS) >> actual_pin) & 1u;
						event = (state ? HAL_IRQ_RISE : HAL_IRQ_FALL) | ((u32)(IrqTable[i].IrqPinName) << 16);
					}
					GPIO->GPIO_INT_CLR = (1 << i);
					IrqTable[i].UsrIrqHandler(IrqTable[i].UsrIrqData, event);
				} else {
					GPIO->GPIO_INT_CLR = (1 << i);
					RTK_LOGW("GPIO", "Assert Port[%d]_Pin[%d] interrupt, but entry[%d] UsrIrqHandler is NULL \r\n",
							 PORT_NUM(IrqTable[i].IrqPinName), PIN_NUM(IrqTable[i].IrqPinName), i);
				}
			} else {
				GPIO->GPIO_INT_CLR = (1 << i);
			}
		}
	}
	__DSB(); /* ensure INT_CLR writes reach the GPIO before exception return, avoid spurious tail re-entry */
}
/**
  * @brief  To register a user interrupt handler for a specified pin.
  * GPIO_Init() must be called prior to this function.
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  IrqHandler: The IRQ handler to be assigned to the specified pin
  * @param  IrqData: The pointer will be pass the the IRQ handler
  *
  * @retval None
  */
void GPIO_UserRegIrq(u32 GPIO_Pin, void *IrqHandler, void *IrqData)
{
	u32 port_num = PORT_NUM(GPIO_Pin);
	struct GPIO_IrqContext *IrqTable;
	u16 *pin_tracker;
	u32 max_irq;
	u32 idx;

	/* Select table/tracker/limit by port */
	if (port_num == GPIO_PORT_A) {
		IrqTable    = Aon_GPIO_IrqTable;
		pin_tracker = aon_gpio_irq_pin_tracker;
		max_irq     = MAX_Aon_GPIO_IRQ;
	} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
		IrqTable    = Syson_GPIO_IrqTable;
		pin_tracker = syson_gpio_irq_pin_tracker;
		max_irq     = MAX_GPIO_IRQ;
	} else {
		IrqTable    = Core_GPIO_IrqTable;
		pin_tracker = core_gpio_irq_pin_tracker;
		max_irq     = MAX_GPIO_IRQ;
	}

	/* step1: check whether GPIO_Pin already owns an entry */
	for (idx = 0; idx < max_irq; idx++) {
		if ((GPIO_Pin == (u32)(IrqTable[idx].IrqPinName)) && (GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER == IrqTable[idx].IrqIdleEntryFlag)) {
			break; /* found, reuse this entry */
		}
		if (pin_tracker[idx] == GPIO_Pin) {
			break; /* pin already in tracker, reuse its slot */
		}
	}

	/* step2: register into the found entry, or report error */
	if (idx != max_irq) {
		if (IrqTable[idx].IrqIdleEntryFlag == GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER) {
			IrqTable[idx].IrqIdleEntryFlag = 0; /* close ISR gate before re-registration (C-03) */
			__DMB();
		}
		IrqTable[idx].IrqPinName       = (u16)GPIO_Pin;
		IrqTable[idx].UsrIrqHandler    = (GPIO_IRQ_FUN)IrqHandler;
		IrqTable[idx].UsrIrqData       = IrqData;
		__DMB();
		IrqTable[idx].IrqIdleEntryFlag = GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER;
	} else {
		RTK_LOGE("GPIO", "No idle entry in IrqTable for port %d, please enlarge the IRQ table size!!!\r\n", port_num);
	}
}

/**
  * @brief  To Unregister a user interrupt handler for a specified pin
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @retval None
  */
void GPIO_UserUnRegIrq(u32 GPIO_Pin)
{
	// Weide self-note: Will also remove IRQ pin from gpio_irq_pin_tracker[] pin list
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_idx;

	GPIO = GPIO_GetBase(port_num);

	pin_idx = GPIO_FindPinIdx(GPIO_Pin, port_num);
	if (pin_idx == 0xFF) {
		RTK_LOGD("GPIO", "GPIO_Pin not registered as IRQ, nothing to unregister.\n");
		return;
	}

	/* step1: Disable Interrupt and close mask gate — full quiesce (C-04) */
	GPIO->GPIO_INT_DIS = (1 << pin_idx);
	GPIO->GPIO_IM_DIS  = (1 << pin_idx);
	/* step2: Clear pending interrupt */
	GPIO->GPIO_INT_CLR = (1 << pin_idx);
	__DSB();

	/* step3: UnRegister IrqTable and remove from tracker */
	if (port_num == GPIO_PORT_A) {
		GPIO_UnRegFromTable(GPIO_Pin, Aon_GPIO_IrqTable, aon_gpio_irq_pin_tracker,
							MAX_Aon_GPIO_IRQ, &aon_gpio_irq_in_use_idx);
	} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
		GPIO_UnRegFromTable(GPIO_Pin, Syson_GPIO_IrqTable, syson_gpio_irq_pin_tracker,
							MAX_GPIO_IRQ, &syson_gpio_irq_in_use_idx);
	} else {
		GPIO_UnRegFromTable(GPIO_Pin, Core_GPIO_IrqTable, core_gpio_irq_pin_tracker,
							MAX_GPIO_IRQ, &core_gpio_irq_in_use_idx);
	}
}
/**
  * @brief  Config GPIO interrupt mode
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  NewState: ENABLE/DISABLE
  * @param  GPIO_ITTrigger: Specifies interrupt mode is level or edge trigger
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_INT_Trigger_LEVEL: level trigger
  *		 		@arg GPIO_INT_Trigger_EDGE: edge trigger
  *		 		@arg GPIO_INT_Trigger_BOTHEDGE: both-edge trigger
  * @param  GPIO_ITPolarity: Specifies interrupt mode is high or low active trigger
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_INT_POLARITY_ACTIVE_LOW: falling edge or low level active
  *		 		@arg GPIO_INT_POLARITY_ACTIVE_HIGH: rising edge or high level active
  * @param  GPIO_ITDebounce: enable or disable de-bounce for interrupt
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_INT_DEBOUNCE_DISABLE: disable de-bounce
  *		 		@arg GPIO_INT_DEBOUNCE_ENABLE: enable de-bounce
  *
  * @retval None
  */

void GPIO_INTMode(u32 GPIO_Pin, u32 NewState,
				  u32 GPIO_ITTrigger,
				  u32 GPIO_ITPolarity,
				  u32 GPIO_ITDebounce)
{
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_num = PIN_NUM(GPIO_Pin);
	u8 gpio_group_sel; // 0: Group A, 1: Group B
	u8 actual_pin_num; // Actual GPIO pin location in a 32-bit word after accounting for offset
	volatile u32 *GPIO_INTx_actual_reg; // Actual GPIO INTx register address after accounting for free GPIO IRQ pin
	volatile u32 *GPIO_DEBx_actual_reg; // Actual GPIO DEBx register address after accounting for free GPIO IRQ pin
	u8 pin_idx; // position of GPIO IRQ pin in gpio_irq_pin_tracker[], also pin_idx for registers like GPIO_IR_EN, etc

	RTK_LOGD("GPIO", "Port: %d Pin:%d\n", port_num, pin_num);
	GPIO = GPIO_GetBase(port_num);
	pin_idx = GPIO_FindPinIdx(GPIO_Pin, port_num);

	RTK_LOGD("GPIO", "pin_idx: %x\n", pin_idx);
	if (pin_idx == 0xFF) {
		RTK_LOGE("GPIO", "GPIO_Pin 0x%x is not registered as IRQ\r\n", GPIO_Pin);
		return;
	}

	// Resolve group/pin and compute INTx/DEBx register pointers — needed by both ENABLE and DISABLE paths.
	GPIO_GetPinLocation(port_num, pin_num, &gpio_group_sel, &actual_pin_num);
	GPIO_INTx_actual_reg = (volatile u32 *)&GPIO->GPIO_INT0_SEL + pin_idx;
	GPIO_DEBx_actual_reg = (volatile u32 *)&GPIO->GPIO_DEB0_SEL + pin_idx;

	if (NewState == ENABLE) {
		// Validate all parameters before touching hardware — prevents disarming pin on bad input (B-A)
		if (GPIO_ITTrigger == GPIO_INT_Trigger_EDGE) {
			if (GPIO_ITPolarity != GPIO_INT_POLARITY_ACTIVE_LOW &&
				GPIO_ITPolarity != GPIO_INT_POLARITY_ACTIVE_HIGH &&
				GPIO_ITPolarity != GPIO_INT_Trigger_BOTHEDGE) {
				RTK_LOGE("GPIO", "Illegal GPIO_ITPolarity setting: %d!\r\n", GPIO_ITPolarity);
				return;
			}
		} else if (GPIO_ITTrigger == GPIO_INT_Trigger_LEVEL) {
			if (GPIO_ITPolarity != GPIO_INT_POLARITY_ACTIVE_HIGH &&
				GPIO_ITPolarity != GPIO_INT_POLARITY_ACTIVE_LOW) {
				RTK_LOGE("GPIO", "Illegal GPIO_ITPolarity setting: %d!\r\n", GPIO_ITPolarity);
				return;
			}
		} else if (GPIO_ITTrigger != GPIO_INT_Trigger_BOTHEDGE) {
			RTK_LOGE("GPIO", "Illegal GPIO_ITTrigger setting: %d!\r\n", GPIO_ITTrigger);
			return;
		}
		if (GPIO_ITDebounce != GPIO_INT_DEBOUNCE_ENABLE &&
			GPIO_ITDebounce != GPIO_INT_DEBOUNCE_DISABLE) {
			RTK_LOGE("GPIO", "Illegal GPIO_ITDebounce setting: %d!\r\n", GPIO_ITDebounce);
			return;
		}

		*GPIO_INTx_actual_reg = (GPIO_INT0_MER_SEL(actual_pin_num)) | (GPIO_INT0_GP_SEL(gpio_group_sel));
		// Disable GPIO Interrupt of the current GPIO_Pin (GPIO IRQ pin), just in case
		GPIO->GPIO_INT_DIS = (0x1 << pin_idx);
		GPIO->GPIO_IM_DIS  = (0x1 << pin_idx);
		// Clear any stale edge latch before re-arming (H-01); harmless for level-type pins
		GPIO->GPIO_INT_CLR = (0x1 << pin_idx);
		__DSB();

		// Register interrupts for respective GPIO domain (repeat registration is safe — just extra steps)
		if (GPIO == AON_GPIO_BASE || GPIO == AON_GPIO_BASE_S) { // AON GPIO
			//DiagPrintf("AON GPIO IRQ reg\n");
			if (InterruptRegister(GPIO_INTHandler, AON_GPIO_IRQ, (u32)GPIO, INT_PRI_MIDDLE) != 1) {
				RTK_LOGE("GPIO", "InterruptRegister(), AON GPIO, is erroneous. \r\n");
				return;
			}
			InterruptEn(AON_GPIO_IRQ, INT_PRI_MIDDLE);
		} else if (GPIO == PON_GPIO_BASE || GPIO == PON_GPIO_BASE_S) { // PON GPIO
			//DiagPrintf("SYSON GPIO IRQ reg\n");
			if (InterruptRegister(GPIO_INTHandler, PON_GPIO_IRQ, (u32)GPIO, INT_PRI_MIDDLE) != 1) {
				RTK_LOGE("GPIO", "InterruptRegister(), SYSON GPIO, is erroneous. \r\n");
				return;
			}
			InterruptEn(PON_GPIO_IRQ, INT_PRI_MIDDLE);
		} else { // CORE GPIO
			//DiagPrintf("CORE GPIO IRQ reg\n");
			if (InterruptRegister(GPIO_INTHandler, GPIO_IRQ, (u32)GPIO, INT_PRI_MIDDLE) != 1) {
				RTK_LOGE("GPIO", "InterruptRegister(), CORE GPIO, is erroneous. \r\n");
				return;
			}
			InterruptEn(GPIO_IRQ, INT_PRI_MIDDLE);
		}

		// No need Read-Modify-Write — IR_EN, LI_EN, etc are WO registers
		if (GPIO_ITTrigger == GPIO_INT_Trigger_EDGE) {
			if (GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_HIGH) {
				GPIO->GPIO_IR_EN = (1 << pin_idx);
				GPIO->GPIO_EI_EN = (1 << pin_idx);
			} else if (GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_LOW) {
				GPIO->GPIO_IF_EN = (1 << pin_idx);
				GPIO->GPIO_EI_EN = (1 << pin_idx);
			} else { // GPIO_INT_Trigger_BOTHEDGE
				GPIO->GPIO_ID_EN = (1 << pin_idx);
				GPIO->GPIO_EI_EN = (1 << pin_idx);
			}
		} else if (GPIO_ITTrigger == GPIO_INT_Trigger_LEVEL) {
			if (GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_HIGH) {
				GPIO->GPIO_IR_EN = (1 << pin_idx);
				GPIO->GPIO_LI_EN = (1 << pin_idx);
			} else { // GPIO_INT_POLARITY_ACTIVE_LOW
				GPIO->GPIO_IF_EN = (1 << pin_idx);
				GPIO->GPIO_LI_EN = (1 << pin_idx);
			}
		} else { // GPIO_INT_Trigger_BOTHEDGE
			RTK_LOGI("GPIO", "Setting Dual Edge IRQ, polarity will be IGNORED!\n");
			GPIO->GPIO_ID_EN = (1 << pin_idx);
			GPIO->GPIO_EI_EN = (1 << pin_idx);
		}
		if (GPIO_ITDebounce == GPIO_INT_DEBOUNCE_ENABLE) {
			*GPIO_DEBx_actual_reg |= (GPIO_DEB0_MER_SEL(actual_pin_num)) | (GPIO_DEB0_GP_SEL(gpio_group_sel));
			GPIO->GPIO_DEB_EN = (1 << pin_idx);
			*GPIO_INTx_actual_reg |= GPIO_INT0_DEB_SEL(pin_idx) | GPIO_BIT_INT0_SUR_SEL;
		} else { // GPIO_INT_DEBOUNCE_DISABLE
			GPIO->GPIO_DEB_DIS = (1 << pin_idx);
			*GPIO_INTx_actual_reg &= ~(GPIO_INT0_DEB_SEL(pin_idx) | GPIO_BIT_INT0_SUR_SEL);
			*GPIO_DEBx_actual_reg &= ~((GPIO_DEB0_MER_SEL(actual_pin_num)) | (GPIO_DEB0_GP_SEL(gpio_group_sel)));
		}
		GPIO->GPIO_INT_EN = (0x01 << pin_idx); //int mode
		GPIO->GPIO_IM_EN  = (0x01 << pin_idx); // unmasks interrupt
	} else { // NewState == Disable
		//DiagPrintf("GPIO IRQ unregistering...\n");
		GPIO->GPIO_INT_DIS = (0x01 << pin_idx); // normal gpio mode
		GPIO->GPIO_IM_DIS  = (0x01 << pin_idx); // masks interrupt
		GPIO->GPIO_DEB_DIS = (0x01 << pin_idx); // safe even if debounce was not enabled
		*GPIO_INTx_actual_reg = 0;
		*GPIO_DEBx_actual_reg = 0;
	}
}
/**
  * @brief  Disable or Enable the interrupt of a specified pin
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  NewState: ENABLE/DISABLE
  *
  * @retval None
  */

void GPIO_INTConfig(u32 GPIO_Pin, u32 NewState)
{
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_idx;

	GPIO = GPIO_GetBase(port_num);
	pin_idx = GPIO_FindPinIdx(GPIO_Pin, port_num);

	if (pin_idx == 0xFF) {
		RTK_LOGD("GPIO", "GPIO_Pin is not an IRQ pin.\n");
		return; // not in pin list (HAL_ERR_PARA case)
	}

	if (NewState == DISABLE) {
		//DiagPrintf("INTConfig DISABLE pin_idx: 0x%x\n", pin_idx);
		GPIO->GPIO_IM_DIS = (1 << pin_idx);
	} else {
		//DiagPrintf("INTConfig ENABLE pin_idx: 0x%x\n", pin_idx);
		GPIO->GPIO_INT_CLR = (1 << pin_idx);
		__DSB();
		GPIO->GPIO_IM_EN = (1 << pin_idx);
	}
}
/**
  * @brief  Set the specified GPIO pin data Direction.
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  data_direction: Specifies GPIO pin direction
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_Mode_IN: GPIO pin direction config input mode
  *		 		@arg GPIO_Mode_OUT: GPIO pin direction config output mode
  * @retval None
  */

void GPIO_Direction(u32 GPIO_Pin, u32 data_direction)
{
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_num  = PIN_NUM(GPIO_Pin);
	u8 use_groupB;
	u8 actual_pin;
	volatile u32 *reg;

	RTK_LOGD("GPIO", " GPIO DIR port num: %d, pin num: %d\n", port_num, pin_num);
	GPIO = GPIO_GetBase(port_num);
	GPIO_GetPinLocation(port_num, pin_num, &use_groupB, &actual_pin);

	/* Select ODM (output) or IDM (input) register, then set the bit */
	if (data_direction == GPIO_Mode_OUT) {
		reg = use_groupB ? &GPIO->GPIOB_ODM_EN : &GPIO->GPIOA_ODM_EN;
	} else {
		reg = use_groupB ? &GPIO->GPIOB_IDM_EN : &GPIO->GPIOA_IDM_EN;
	}
	*reg = (1 << actual_pin);
}
/**
  * @brief  Initializes the GPIO registers according to the specified parameters
  *         in GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the GPIO peripheral.
  * @note   OpenDrain output: on drive + OUT + GPIO[gpio_bit] = 0, pin should have pull-up resistor
  * @note   Input HighZ: no drive + IN, user can input high or low use this pin
  *
  * @retval None
  */

void GPIO_Init(GPIO_InitTypeDef   *GPIO_InitStruct)
{
	u8 port_num;

	if (GPIO_InitStruct == NULL) {
		RTK_LOGE("GPIO", "GPIO_InitStruct is NULL\r\n");
		return;
	}

	if (GPIO_InitStruct->GPIO_Mode > GPIO_Mode_INT) {
		RTK_LOGE("GPIO", "GPIO_Mode %d is illegal!\r\n", GPIO_InitStruct->GPIO_Mode);
		return;
	}

	port_num = PORT_NUM(GPIO_InitStruct->GPIO_Pin);

	/* open gpio function and clock */
	if (port_num == GPIO_PORT_A) {
		RCC_PeriphClockCmd(APBPeriph_AON_GPIO, APBPeriph_AONGPIO_CLOCK, ENABLE);
	} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
		RCC_PeriphClockCmd(APBPeriph_PON_GPIO, APBPeriph_PON_GPIO_CLOCK, ENABLE);
	} else {
		RCC_PeriphClockCmd(APBPeriph_CORE_GPIO, APBPeriph_CORE_GPIO_CLOCK, ENABLE);
	}
	/* GPIO Pad shutdown control: Turn on the corresponding GPIO Pad and pinmux to GPIO */
	Pinmux_Config(GPIO_InitStruct->GPIO_Pin, PINMUX_FUNCTION_GPIO);

	if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_INT) {
		u8 slot;
		if (port_num == GPIO_PORT_A) {
			slot = GPIO_AllocTrackerSlot(GPIO_InitStruct->GPIO_Pin,
										 aon_gpio_irq_pin_tracker, MAX_Aon_GPIO_IRQ,
										 &aon_gpio_irq_in_use_idx);
		} else if (port_num == GPIO_PORT_B || port_num == GPIO_PORT_F || port_num == GPIO_PORT_P) {
			slot = GPIO_AllocTrackerSlot(GPIO_InitStruct->GPIO_Pin,
										 syson_gpio_irq_pin_tracker, MAX_GPIO_IRQ,
										 &syson_gpio_irq_in_use_idx);
		} else {
			slot = GPIO_AllocTrackerSlot(GPIO_InitStruct->GPIO_Pin,
										 core_gpio_irq_pin_tracker, MAX_GPIO_IRQ,
										 &core_gpio_irq_in_use_idx);
		}
		if (slot == 0xFF) {
			RTK_LOGE("GPIO", "IRQ table full, cannot init pin 0x%x as INT\r\n", GPIO_InitStruct->GPIO_Pin);
			return;
		}
		GPIO_Direction(GPIO_InitStruct->GPIO_Pin, GPIO_Mode_IN);
		PAD_PullCtrl(GPIO_InitStruct->GPIO_Pin, GPIO_InitStruct->GPIO_PuPd);

		// Set debounce period before arming, so DEB_CYC is in hardware
		// before interrupt gates open — prevents spurious firing with DEB_CYC = 0
		if (GPIO_InitStruct->GPIO_ITDebounce == GPIO_INT_DEBOUNCE_ENABLE &&
			GPIO_InitStruct->GPIO_DebounceUs != 0) {
			GPIO_DebounceClock_Pro3(GPIO_InitStruct->GPIO_Pin, GPIO_InitStruct->GPIO_DebounceUs);
		}

		GPIO_INTMode(GPIO_InitStruct->GPIO_Pin, ENABLE, GPIO_InitStruct->GPIO_ITTrigger,
					 GPIO_InitStruct->GPIO_ITPolarity, GPIO_InitStruct->GPIO_ITDebounce);

	} else {
		// Un-register only if pin was previously registered as INT (else GPIO_FindPinIdx asserts)
		if (GPIO_FindPinIdx(GPIO_InitStruct->GPIO_Pin, port_num) != 0xFF) {
			GPIO_UserUnRegIrq(GPIO_InitStruct->GPIO_Pin);
		}
		if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) {
			GPIO_Direction(GPIO_InitStruct->GPIO_Pin, GPIO_Mode_OUT);
		} else if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IN) {
			GPIO_Direction(GPIO_InitStruct->GPIO_Pin, GPIO_Mode_IN);
			PAD_PullCtrl(GPIO_InitStruct->GPIO_Pin, GPIO_InitStruct->GPIO_PuPd);
		}
	}
}
/**
  * @brief  De-Initializes a GPIO Pin, reset it as default setting.
  * @param  GPIO_Pin: GPIO pin num from PinName.
  *
  *
  * @retval None
  */

void GPIO_DeInit(u32 GPIO_Pin)
{
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_num = PIN_NUM(GPIO_Pin);

	RTK_LOGD("GPIO", "port num: %d, pin num: %d\n", port_num, pin_num);

	// Change GPIO pin to input
	GPIO_Direction(GPIO_Pin, GPIO_Mode_IN);

	// If pin was used as IRQ, fully unregister it: clears pending interrupt,
	// frees its IrqTable entry, restores tracker slot to GPIO_IRQ_PIN_TRACKER_UNUSED, decrements in_use_idx
	if (GPIO_FindPinIdx(GPIO_Pin, port_num) != 0xFF) {
		GPIO_UserUnRegIrq(GPIO_Pin);
	}
	// Set PAD control to High-Z
	PAD_PullCtrl(GPIO_Pin, GPIO_PuPd_NOPULL);
}

/**
  * @brief  Read a specified GPIO pin Value
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  DebounceEn: Selects if we should read GPIO pin value with or without debounce circuit enabled
  * @retval The input port pin current status(High or Low).
  */

u32 GPIO_ReadDataBit(u32 GPIO_Pin)
{
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_num  = PIN_NUM(GPIO_Pin);
	u8 use_groupB;
	u8 actual_pin;
	u32 sts;

	GPIO = GPIO_GetBase(port_num);
	GPIO_GetPinLocation(port_num, pin_num, &use_groupB, &actual_pin);

	sts = use_groupB ? GPIO->GPIOB_DP_STS : GPIO->GPIOA_DP_STS;
	return (sts >> actual_pin) & 1u;
}

/**
  * @brief  Write a specified output port pin.
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  Pin_State: The state going to be set to the assigned GPIO pin.
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_PIN_LOW: Pin state set to low
  *		 		@arg GPIO_PIN_HIGH: Pin state set to high
  * @retval None
  */

void GPIO_WriteBit(u32 GPIO_Pin, u32 Pin_State)
{
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_num  = PIN_NUM(GPIO_Pin);
	u8 use_groupB;
	u8 actual_pin;
	volatile u32 *reg;

	RTK_LOGD("GPIO", "port num: %d, pin num: %d\n", port_num, pin_num);
	GPIO = GPIO_GetBase(port_num);
	GPIO_GetPinLocation(port_num, pin_num, &use_groupB, &actual_pin);

	/* Select ODH (High) or ODL (Low) register and set the bit */
	if (Pin_State) {
		reg = use_groupB ? &GPIO->GPIOB_ODH_EN : &GPIO->GPIOA_ODH_EN;
	} else {
		reg = use_groupB ? &GPIO->GPIOB_ODL_EN : &GPIO->GPIOA_ODL_EN;
	}
	*reg = (1 << actual_pin);
}

/**
  * @brief  Toggle a specified output port pin.
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @retval None
  */

void GPIO_ToggleBit(u32 GPIO_Pin)
{
	GPIO_TypeDef *GPIO;
	u8 port_num = PORT_NUM(GPIO_Pin);
	u8 pin_num  = PIN_NUM(GPIO_Pin);
	u8 use_groupB;
	u8 actual_pin;

	RTK_LOGD("GPIO", "port num: %d, pin num: %d\n", port_num, pin_num);
	GPIO = GPIO_GetBase(port_num);
	GPIO_GetPinLocation(port_num, pin_num, &use_groupB, &actual_pin);

	if (use_groupB) {
		GPIO->GPIOB_ODT = (1 << actual_pin);
	} else {
		GPIO->GPIOA_ODT = (1 << actual_pin);
	}
}

// For all GPIO port, need to handle overflow case for mask.
// In the program below, have your own mask to discern where is the overflow, then act accordingly.
/**
  * @brief  set the specified GPIO port pins data direction.
  * @param  GPIO_Port: Specifies port number
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_PORT_A: port number is 0
  *		 		@arg GPIO_PORT_B: port number is 1
  *		 		@arg GPIO_PORT_C: port number is 2
  * @param  GPIO_Mask: One bit one GPIO pin, PortA/B/C:0x00000000 ~0xFFFFFFFF.
  * @param  data_direction: Specifies GPIO port pins direction
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_Mode_IN: GPIO port pins direction config input mode
  *		 		@arg GPIO_Mode_OUT: GPIO port pins direction config output mode
  * @note   It can config multiple pins of one port by parameter GPIO_Mask
  * @retval None
  */

void GPIO_PortDirection(u32 GPIO_Port, u32 GPIO_Mask, u32 data_direction)
{
	GPIO_TypeDef *GPIO;
	u32 RegValue_forOutput;
	u32 RegValue_forInput;
	u32 split_threshold;

	if (!IS_GPIO_PORT_NUM(GPIO_Port)) {
		RTK_LOGE("GPIO", "GPIO_PortDirection: invalid port %d\n", GPIO_Port);
		return;
	}
	GPIO = GPIO_GetBase((u8)GPIO_Port);

	if (data_direction == GPIO_Mode_OUT) {
		RegValue_forOutput = GPIO_Mask;
		RegValue_forInput  = 0;
	} else {
		RegValue_forOutput = 0;
		RegValue_forInput  = GPIO_Mask;
	}

	// GPIO S is Group B only; all other ports start with Group A
	if (GPIO_Port == GPIO_PORT_S) {
		GPIO->GPIOB_ODM_EN = RegValue_forOutput << gpio_pin_idx_offset_groupB[GPIO_Port];
		GPIO->GPIOB_IDM_EN = RegValue_forInput  << gpio_pin_idx_offset_groupB[GPIO_Port];
		return;
	}

	// Group A write (all non-S ports)
	GPIO->GPIOA_ODM_EN = RegValue_forOutput << gpio_pin_idx_offset_groupA[GPIO_Port];
	GPIO->GPIOA_IDM_EN = RegValue_forInput  << gpio_pin_idx_offset_groupA[GPIO_Port];

	// GPIO E and P may also span Group B; split_threshold is the Group A pin count boundary
	split_threshold = (GPIO_Port == GPIO_PORT_E) ? 0x7FFu :
					  (GPIO_Port == GPIO_PORT_P) ? 0x7FFu : 0u;
	if (split_threshold && GPIO_Mask > split_threshold) {
		GPIO->GPIOB_ODM_EN = (RegValue_forOutput >> 11) << gpio_pin_idx_offset_groupB[GPIO_Port];
		GPIO->GPIOB_IDM_EN = (RegValue_forInput  >> 11) << gpio_pin_idx_offset_groupB[GPIO_Port];
	}
}

/**
  * @brief  Reads the specified GPIO pins.
  * @param  GPIO_Port: Specifies port number
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_PORT_A: port number is 0
  *		 		@arg GPIO_PORT_B: port number is 1
  *		 		@arg GPIO_PORT_C: port number is 2
  * @param  GPIO_Mask: One bit one GPIO pin, PortA/B/C:0x00000000 ~0xFFFFFFFF.
  * @note   It can config multiple pins of one port by parameter GPIO_Mask
  * @retval The value of the specified port pins
  */

u32 GPIO_PortRead(u32 GPIO_Port, u32 GPIO_Mask)
{
	GPIO_TypeDef *GPIO;
	u32 RegValue_GroupA;
	u32 RegValue_GroupB;
	u32 RegValue_actual;
	u32 groupA_mask;

	if (!IS_GPIO_PORT_NUM(GPIO_Port)) {
		RTK_LOGE("GPIO", "GPIO_PortRead: invalid port %d\n", GPIO_Port);
		return 0;
	}
	GPIO = GPIO_GetBase((u8)GPIO_Port);

	if (GPIO_Port == GPIO_PORT_E || GPIO_Port == GPIO_PORT_P) {
		/* Split ports: lower pins in Group A, upper pins in Group B */
		groupA_mask     = 0x7FFu;
		RegValue_GroupA = (GPIO->GPIOA_DP_STS >> gpio_pin_idx_offset_groupA[GPIO_Port]) & groupA_mask;
		RegValue_GroupB = GPIO->GPIOB_DP_STS & ((GPIO_Port == GPIO_PORT_E) ? 0x3Fu : 0x1u);
		RegValue_actual = (RegValue_GroupB << 11) | RegValue_GroupA;
	} else if (GPIO_Port == GPIO_PORT_S) {
		/* Group B only */
		RegValue_actual = (GPIO->GPIOB_DP_STS >> gpio_pin_idx_offset_groupB[GPIO_Port]) & 0x7Fu;
	} else if (GPIO_Port == GPIO_PORT_A) {
		RegValue_actual = GPIO->GPIOA_DP_STS & 0x3Fu;
	} else if (GPIO_Port == GPIO_PORT_B) {
		RegValue_actual = GPIO->GPIOA_DP_STS & 0x1Fu;
	} else if (GPIO_Port == GPIO_PORT_C) {
		RegValue_actual = GPIO->GPIOA_DP_STS & 0x7FFu;
	} else if (GPIO_Port == GPIO_PORT_D) {
		RegValue_actual = (GPIO->GPIOA_DP_STS >> gpio_pin_idx_offset_groupA[GPIO_Port]) & 0x3FFu;
	} else { /* GPIO_PORT_F */
		RegValue_actual = (GPIO->GPIOA_DP_STS >> gpio_pin_idx_offset_groupA[GPIO_Port]) & 0xFFFFu;
	}
	return RegValue_actual & GPIO_Mask;
}

/**
  * @brief  Write the specified port pins.
  * @param  GPIO_Port: Specifies port number
  *          This parameter can be one of the following values:
  *		 		@arg GPIO_PORT_A: port number is 0
  *		 		@arg GPIO_PORT_B: port number is 1
  *		 		@arg GPIO_PORT_B: port number is 2
  * @param	GPIO_Mask: One bit one GPIO pin, PortA/B/C:0x00000000 ~0xFFFFFFFF.
  * @param  Port_State: Specifies the output state to be set to the assigned GPIO pins. Can be 0 (OFF) or 1 (ON).
  * @retval None
  */

void GPIO_PortWrite(u32 GPIO_Port, u32 GPIO_Mask, u32 Port_State)
{
	GPIO_TypeDef *GPIO;
	u32 RegValue_WriteOne;
	u32 RegValue_WriteZero;
	u32 split_threshold;

	if (!IS_GPIO_PORT_NUM(GPIO_Port)) {
		RTK_LOGE("GPIO", "GPIO_PortWrite: invalid port %d\n", GPIO_Port);
		return;
	}
	GPIO = GPIO_GetBase((u8)GPIO_Port);

	// No need to do Read-Modify-Write: separate ODH/ODL registers, writing 0 has no effect.
	// GPIO_Mask selects which pins to affect; Port_State selects High or Low per pin.
	RegValue_WriteOne  = GPIO_Mask & Port_State;
	RegValue_WriteZero = GPIO_Mask & (~Port_State);

	// GPIO S is Group B only; all other ports start with Group A
	if (GPIO_Port == GPIO_PORT_S) {
		GPIO->GPIOB_ODH_EN = RegValue_WriteOne  << gpio_pin_idx_offset_groupB[GPIO_Port];
		GPIO->GPIOB_ODL_EN = RegValue_WriteZero << gpio_pin_idx_offset_groupB[GPIO_Port];
		return;
	}

	// Group A write (all non-S ports)
	GPIO->GPIOA_ODH_EN = RegValue_WriteOne  << gpio_pin_idx_offset_groupA[GPIO_Port];
	GPIO->GPIOA_ODL_EN = RegValue_WriteZero << gpio_pin_idx_offset_groupA[GPIO_Port];

	// GPIO E and P may also span Group B; split_threshold is the Group A pin count boundary
	split_threshold = (GPIO_Port == GPIO_PORT_E) ? 0x7FFu :
					  (GPIO_Port == GPIO_PORT_P) ? 0x7FFu : 0u;
	if (split_threshold && GPIO_Mask > split_threshold) {
		GPIO->GPIOB_ODH_EN = (RegValue_WriteOne  >> 11) << gpio_pin_idx_offset_groupB[GPIO_Port];
		GPIO->GPIOB_ODL_EN = (RegValue_WriteZero >> 11) << gpio_pin_idx_offset_groupB[GPIO_Port];
	}
}

/**
  * @brief  Set the specified port debounce clock. DO NOT USE for AmebaPro3.
  * @param  GPIO_Port Specifies port number, which can be any GPIO_PORT_X defined in @ref GPIO_Port_Type.
  * @param	DivideCount Debounce clock division with 32KHz.range: 0x0 - 0x3FFF.
  *			debounce time = (DivideCount +1) * 2 * 32μs.
  * @retval None
  */
void GPIO_DebounceClock(u32 GPIO_Port, u32 DivideCount)
{
	GPIO_TypeDef *GPIO;
	volatile u32 *GPIO_DEBx_actual_reg; // Actual GPIO DEBx register address after accounting for free GPIO IRQ pin
	u8 idx;
	u8 max_deb; // Maxmium available debounce registers

	max_deb = (GPIO_Port == GPIO_PORT_A) ? MAX_Aon_GPIO_IRQ : MAX_GPIO_IRQ;

	GPIO = GPIO_GetBase(GPIO_Port);
	GPIO_DEBx_actual_reg = (volatile u32 *)&GPIO->GPIO_DEB0_SEL;

	for (idx = 0; idx < max_deb; idx++) {
		*(GPIO_DEBx_actual_reg + idx) &= ~GPIO_MASK_DEB0_CYC; // clear before setting new debounce cycle time
		*(GPIO_DEBx_actual_reg + idx) |= GPIO_DEB0_CYC(DivideCount);
	}
}

/**
  * @brief  Set the specified GPIO pin's debounce time for Pro3.
  * @param  GPIO_Pin: The GPIO IRQ pin which has its debounce function enabled.
  * @param  debounce_us  The time filter for the debounce, in micro-second.
  * 						  But the time resolution is 31.25us (1/32K) and the
  * 						  maximum time is 512 ms.
  * @retval None
  */

void GPIO_DebounceClock_Pro3(u32 GPIO_Pin, u32 debounce_us)
{
	u8 port_num = PORT_NUM(GPIO_Pin);
	GPIO_TypeDef *GPIO;
	u32 debounce_cyc;
	u8 pin_idx;
	volatile u32 *GPIO_DEBx_actual_reg;

	// GPIO_Pin must already be registered in the tracker, i.e., it must be an IRQ pin
	GPIO = GPIO_GetBase(port_num);
	pin_idx = GPIO_FindPinIdx(GPIO_Pin, port_num);
	if (pin_idx == 0xFF) {
		RTK_LOGE("GPIO", "GPIO_Pin 0x%x is not registered as IRQ\r\n", GPIO_Pin);
		return;
	}

	if (debounce_us > 512000) {
		RTK_LOGW("GPIO", "GPIO debounce time: %d exceeds max of 512ms. \r\n", debounce_us);
		debounce_us = 512000;
	}

	debounce_cyc = debounce_us * 32 / 1000;
	if (debounce_cyc == 0) {
		debounce_cyc = 1;
	} else if (debounce_cyc > 16383) {
		debounce_cyc = 16383;   // cycle count only has 14 bits
	}

	GPIO_DEBx_actual_reg = (volatile u32 *)&GPIO->GPIO_DEB0_SEL + pin_idx;
	*GPIO_DEBx_actual_reg &= ~GPIO_MASK_DEB0_CYC; // clear before setting new debounce cycle time
	*GPIO_DEBx_actual_reg |= GPIO_DEB0_CYC(debounce_cyc);
}
/**
  * @brief  Set the level sensitive interrupt synchronized to pclk_intr.
  * @param  GPIO_Port: Specifies port number
  *          This parameter can be one of the following values:
  *				@arg GPIO_PORT_A: port number is 0
  *				@arg GPIO_PORT_B: port number is 1
  *				@arg GPIO_PORT_C: port number is 2
  * @param	NewState: all level-sensitive interrupts being synchronized to pclk_intr
  *          This parameter can be one of the folling values:
  *				@ENABLE: Synchronize to pclk_intr with 2 cycles
  *				@DISABLE: No synchronization to pclk_intr (default)
  * @retval None
  */

void GPIO_LevelSync(u32 GPIO_Port, u32 NewState)
{
	(void)GPIO_Port;
	(void)NewState;
#if 0 // 15JAN2026 - unused in AmebaProIII
	GPIO_TypeDef *GPIO = NULL;
	assert_param(IS_GPIO_PORT_NUM(GPIO_Port));
	if (TrustZone_IsSecure()) {
		GPIO = GPIO_PORTx_S[GPIO_Port];
	} else {
		GPIO = GPIO_PORTx[GPIO_Port];
	}
	if (NewState == ENABLE) {
		GPIO->GPIO_ITN_LS_SYNC = GPIO_BIT_LEVEL_SYNC;
	} else {
		GPIO->GPIO_ITN_LS_SYNC = 0;
	}
#endif
}
/**
  * @brief Initialize specified GPIO pin output in Open Drain mode.
  * @param PinName: Value of @ref PINMUX_Pin_Name_definitions.
  * @param PUInternal: which can be High-Z (PullNone), Pull-Up or Pull-Down.
  * @retval None.
  */

void GPIO_ODInit(u8 PinName, u32 PUInternal)
{
	GPIO_Direction(PinName, GPIO_Mode_IN);
	if (GPIO_PuPd_UP == PUInternal) {
		PAD_PullCtrl(PinName, GPIO_PuPd_UP);
	} else if (GPIO_PuPd_DOWN == PUInternal) {
		PAD_PullCtrl(PinName, GPIO_PuPd_DOWN);
	} else {
		PAD_PullCtrl(PinName, GPIO_PuPd_NOPULL);
	}

}
/**
  * @brief Write a specified output port pin in Open Drain mode.
  * @param PinName: Value of @ref PINMUX_Pin_Name_definitions.
  * @param PinState: This parameter can be one of the following values:
  * 		@arg GPIO_PIN_LOW: Pin state set to low
  * 		@arg GPIO_PIN_HIGH: Pin state set to high
  * @retval None
  */

void GPIO_ODWriteBit(u8 PinName, u32 PinState)
{
	(void)PinName;
	(void)PinState;
#if 0
	if (PinState == GPIO_PIN_LOW) {
		GPIO_Direction(PinName, GPIO_Mode_OUT);
	} else {
		GPIO_Direction(PinName, GPIO_Mode_IN);
	}
#endif
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
