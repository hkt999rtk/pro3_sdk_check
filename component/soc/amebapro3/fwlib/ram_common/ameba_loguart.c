/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define LOGUART_MIN_XFACTOR   6

/**
  * @brief  Get ovsr & ovsr_adj parameters for the given IP clock and baudrate.
  * @param  IPclk: the LOGUART IP clock, unit: Hz.
  * @param  baudrate: the desired baudrate, unit: bps.
  * @param  ovsr: parameter related to the desired baud rate (STS[23:4]).
  * @param  ovsr_adj: parameter related to the desired baud rate (SCR[26:16]).
  * @note   Mirrors AmebaSmart ROM UART_BaudParaGetFull (same IP); local copy
  *         because the AmebaPro3 ROM stub is not used here.
  */
void LOGUART_BaudParaGet(u32 IPclk, u32 baudrate, u32 *ovsr, u32 *ovsr_adj)
{
	u32 i;
	u32 Remainder;
	u32 TempAdj = 0;
	u32 TempMultly;

	*ovsr = IPclk / baudrate;

	Remainder = IPclk % baudrate;

	for (i = 0; i < 11; i++) {
		TempAdj = TempAdj << 1;
		TempMultly = (Remainder * (12 - i));
		TempAdj |= ((TempMultly / baudrate - (TempMultly - Remainder) / baudrate) ? 1 : 0);
	}

	*ovsr_adj = TempAdj;
}

/**
  * @brief  Highest baud LOGUART can produce from IPclk.
  * @param  IPclk: the LOGUART IP clock, unit: Hz.
  * @retval Maximum reachable baudrate.
  */
u32 LOGUART_BaudMax(u32 IPclk)
{
	return IPclk / LOGUART_MIN_XFACTOR;
}

/**
  * @brief    set loguart baud rate
  * @param  UARTLOG: LOGUART device.
  * @param  BaudRate: desired baud rate.
  */
void LOGUART_SetBaud(LOGUART_TypeDef *UARTLOG, u32 BaudRate)
{
	u32 RegValue;
	u32 Ovsr;
	u32 Ovsr_adj;
	u32 IPClk = 0;

	assert_param(BaudRate != 0);

#if defined(CONFIG_FPGA)
	IPClk = XTAL_ClkGet();
#else
	IPClk = PERI_ClkGet(CLKID_SLOW_SCLK);
#endif
	assert_param(IPClk != 0);

	LOGUART_BaudParaGet(IPClk, BaudRate, &Ovsr, &Ovsr_adj);

	/* Set DLAB to access baud registers */
	UARTLOG->LOGUART_UART_LCR |= LOGUART_BIT_DLAB;

	/*Clean Rx break signal interrupt status at initial stage*/
	UARTLOG->LOGUART_UART_SCR |= LOGUART_BIT_SCRATCH_7;

	RegValue = UARTLOG->LOGUART_UART_STSR;
	RegValue &= ~(LOGUART_MASK_XFACTOR);
	RegValue |= LOGUART_XFACTOR(Ovsr);
	UARTLOG->LOGUART_UART_STSR = RegValue;

	RegValue = UARTLOG->LOGUART_UART_SCR;
	RegValue &= ~(LOGUART_MASK_XFACTOR_ADJ);
	RegValue |= LOGUART_XFACTOR_ADJ(Ovsr_adj);
	UARTLOG->LOGUART_UART_SCR = RegValue;

	UARTLOG->LOGUART_UART_LCR &= ~(LOGUART_BIT_DLAB);

	/* RX baud rate configuration */
	RegValue = UARTLOG->LOGUART_UART_REG_MON_BAUD_STS;
	RegValue &= (~LOGUART_MASK_R_CYCNUM_PERBIT_XTAL);
	RegValue |= Ovsr;
	UARTLOG->LOGUART_UART_REG_MON_BAUD_STS = RegValue;

	RegValue = UARTLOG->LOGUART_UART_REG_MON_BAUD_CTRL;
	RegValue &= (~LOGUART_MASK_R_CYCNUM_PERBIT_OSC);
	RegValue |= LOGUART_R_CYCNUM_PERBIT_OSC(Ovsr);
	UARTLOG->LOGUART_UART_REG_MON_BAUD_CTRL = RegValue;

	RegValue = UARTLOG->LOGUART_UART_REG_RX_PATH_CTRL;
	RegValue &= (~LOGUART_MASK_RXBAUD_ADJ_10_0);
	RegValue |= LOGUART_RXBAUD_ADJ_10_0(Ovsr_adj);
	UARTLOG->LOGUART_UART_REG_RX_PATH_CTRL = RegValue;
}
