/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

extern u32 UART_GetFCR(UART_TypeDef *UARTx);

/**
  * @brief  Set UART baud rate for the given baudrate.
  * @param  UARTx: UART device, where x can be 0~3.
  * @param  BaudRate: desired baud rate, unit: bps.
  * @note   Strong override of the __weak ROM stub, which derives the divisor from
  *         XTAL_ClkGet() (a fixed 40MHz). The RUART IP clock is SLOW_SCLK, which only
  *         happens to be 40MHz at reset -- once the bootloader re-points it at a PLL the
  *         ROM version programs a baud that is wrong by SLOW_SCLK/40MHz. LOGUART_SetBaud
  *         in ameba_loguart.c overrides the ROM stub for the same reason; this mirrors it.
  */
void UART_SetBaud(UART_TypeDef *UARTx, u32 BaudRate)
{
	u32 RegValue;
	u32 Ovsr;
	u32 Ovsr_adj;
	u32 Div, Dll, Dlm;
	u32 IPClk;

	assert_param(BaudRate != 0);

#if defined(CONFIG_FPGA)
	IPClk = XTAL_ClkGet();
#else
	IPClk = PERI_ClkGet(CLKID_SLOW_SCLK);
#endif
	assert_param(IPClk != 0);

	/* get baud rate parameter based on baudrate */
	UARTNormal_BaudParaGetFull(IPClk, BaudRate, &Div, &Ovsr, &Ovsr_adj);
	Dll = Div & 0xFF;
	Dlm = (Div & 0xFF00) >> 8;

	/* Set DLAB bit to 1 to access DLL/DLM */
	UARTx->LCR |= RUART_BIT_DLAB;

	UARTx->DLL = Dll;
	UARTx->DLM_OR_IER = Dlm;
	/*Clean Rx break signal interrupt status at initial stage*/
	UARTx->SCR |= RUART_BIT_SCRATCH_7;

	/* Set OVSR(xfactor) */
	RegValue = UARTx->STS;
	RegValue &= ~(RUART_MASK_XFACTOR);
	RegValue |= RUART_XFACTOR(Ovsr);
	UARTx->STS = RegValue;

	/* Set OVSR_ADJ[10:0] (xfactor_adj[26:16]) */
	RegValue = UARTx->SCR;
	RegValue &= ~(RUART_MASK_XFACTOR_ADJ);
	RegValue |= RUART_XFACTOR_ADJ(Ovsr_adj);
	UARTx->SCR = RegValue;

	/* clear DLAB bit */
	UARTx->LCR &= ~(RUART_BIT_DLAB);
}

/**
  * @brief  Set UART baud rate of low power rx path.
  * @param  UARTx: UART device, where x can be 0~3.
  * @param  BaudRate: desired baud rate, unit: bps.
  * @param  RxIPClockHz: UART rx clock, unit: Hz.
  * @note   AmebaPro3's UART has no low power rx monitor path (no MON_BAUD_STS,
  *         MON_BAUD_CTRL or RX_PATH_CTRL register), so there is nothing to program.
  *         This no-op exists only to keep the fwlib API surface aligned with the
  *         other SoCs, which implement this function.
  */
void UART_LPRxBaudSet(UART_TypeDef *UARTx, u32 BaudRate, u32 RxIPClockHz)
{
	UNUSED(UARTx);
	UNUSED(BaudRate);
	UNUSED(RxIPClockHz);
}

/**
  * @brief  Clear the specified UART interrupt status.
  * @param  UARTx: UART device, where x can be 0~3.
  * @param  UART_IT: the interrupt status to clear.
  * @note   AmebaPro3's UART has no interrupt-clear register: the line status
  *         interrupt is cleared by reading LSR, and the rx timeout indication by
  *         draining RBR. Nothing has to be written, so this is a no-op kept only
  *         to satisfy the declaration shared with the other SoCs.
  */
void UART_INT_Clear(UART_TypeDef *UARTx, u32 UART_IT)
{
	UNUSED(UARTx);
	UNUSED(UART_IT);
}

/**
  * @brief  Read the pending UART interrupt id from the IIR.
  * @param  UARTx: UART device, where x can be 0~3.
  * @note   Reading the IIR clears the interrupt pending state (e.g. the TX FIFO
  *         empty interrupt is cleared by this read).
  * @return The interrupt id (IIR bits[3:1], one of RUART_TX_FIFO_EMPTY /
  *         RUART_RECEIVER_DATA_AVAILABLE / RUART_RECEIVE_LINE_STATUS /
  *         RUART_TIME_OUT_INDICATION), or 0xFFFFFFFF if no interrupt is pending.
  */
u32 UART_IntReadId(UART_TypeDef *UARTx)
{
	u32 reg_iir = UART_IntStatus(UARTx);

	/* RUART_BIT_INT_PEND is inverted logic: 1 -> no interrupt pending */
	if (reg_iir & RUART_BIT_INT_PEND) {
		return 0xFFFFFFFF;
	}

	return (reg_iir >> 1) & 0x7;
}

const UART_DevTable UART_DEV_TABLE[MAX_UART_INDEX] = {
	{UART0_DEV, GDMA1_HANDSHAKE_INTERFACE_UART0_TX, GDMA1_HANDSHAKE_INTERFACE_UART0_RX, UART0_IRQ}, /* uart0 */
	{UART1_DEV, GDMA1_HANDSHAKE_INTERFACE_UART1_TX, GDMA1_HANDSHAKE_INTERFACE_UART1_RX, UART1_IRQ}, /* uart1 */
	{UART2_DEV, GDMA1_HANDSHAKE_INTERFACE_UART2_TX, GDMA1_HANDSHAKE_INTERFACE_UART2_RX, UART2_IRQ}, /* uart2 */
	{UART3_DEV, GDMA1_HANDSHAKE_INTERFACE_UART4_TX, GDMA1_HANDSHAKE_INTERFACE_UART4_RX, BTUART_IRQ}, /* uart3_bt */
};


const u32 APBPeriph_UARTx[MAX_UART_INDEX] = {
	APBPeriph_UART0,
	APBPeriph_UART1,
	APBPeriph_UART2,
};

const u32 APBPeriph_UARTx_CLOCK[MAX_UART_INDEX] = {
	APBPeriph_UART0_CLOCK,
	APBPeriph_UART1_CLOCK,
	APBPeriph_UART2_CLOCK,
};


u32 UART_StateTx[MAX_UART_INDEX];
u32 UART_StateRx[MAX_UART_INDEX];

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup UART
* @brief UART driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup UART_Exported_Functions UART Exported Functions
  * @{
  */

/**
* @brief Configure UART TX DMA burst size.
* @param UARTx: where x can be 0/1/2.
* @param TxDmaBurstSize: UART TX DMA burst size.
* @note Although UART tx fifo depth is 64 in hardware, TxDmaBurstSize should be no more than 16 due to DMA fifo depth limit.
* @note TxDmaBurstSize should not be smaller than GDMA_DstMsize in function UART_TXGDMA_Init();
*       For better performance, it is suggested to make TxDmaBurstSize be equal to GDMA_DstMsize, which is 4.
* @retval None
*/
void
UART_TXDMAConfig(UART_TypeDef *UARTx, u32 TxDmaBurstSize)
{
	u32 TempMiscr;

	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~(RUART_MASK_TXDMA_BURSTSIZE);
	TempMiscr |= RUART_TXDMA_BURSTSIZE(TxDmaBurstSize);

	UARTx->MISCR = TempMiscr;
}

/**
* @brief Configure UART RX DMA burst size.
* @param UARTx: where x can be 0/1/2.
* @param RxDmaBurstSize: UART RX DMA burst size.
* @note Although UART rx fifo depth is 64 in hardware, RxDmaBurstSize should be no more than 16 due to DMA fifo depth limit.
* @note RxDmaBurstSize should not be smaller than GDMA_SrcMsize in function UART_RXGDMA_Init();
*       For better performance, it is suggested to make RxDmaBurstSize be equal to GDMA_SrcMsize, which is 16.
* @retval None
*/
void
UART_RXDMAConfig(UART_TypeDef *UARTx, u32 RxDmaBurstSize)
{
	u32 TempMiscr;

	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~(RUART_MASK_RXDMA_BURSTSIZE);
	TempMiscr |= RUART_RXDMA_BURSTSIZE(RxDmaBurstSize);

	UARTx->MISCR = TempMiscr;
}

/**
* @brief    Enable or disable UART TX DMA.
* @param  UARTx: where x can be 0/1/2.
* @param  NewState: ENABLE or DISABLE.
* @retval   None
*/
void
UART_TXDMACmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		UARTx->MISCR |= RUART_BIT_TXDMA_EN;
	} else {
		UARTx->MISCR &= (~ RUART_BIT_TXDMA_EN);
	}
}

/**
* @brief    Enable or disable UART RX DMA.
* @param  UARTx: where x can be 0/1/2.
* @param  NewState: ENABLE or DISABLE.
* @retval   None
*/
void
UART_RXDMACmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		UARTx->MISCR |= RUART_BIT_RXDMA_EN;
	} else {
		UARTx->MISCR &= (~ RUART_BIT_RXDMA_EN);
	}
}

/**
  * @brief    Init and Enable UART TX GDMA.
  * @param  UartIndex: 0/1/2.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxBuf: Tx Buffer.
  * @param  TxCount: Tx Count.
  * @retval   TRUE/FALSE
  */
bool UART_TXGDMA_Init(
	u8 UartIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxBuf,
	int TxCount
)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pTxBuf, TxCount);

	GdmaChnl = GDMA_ChnlAlloc(1, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 0);
	if (GdmaChnl == 0xFF) {
		return FALSE;
	}

	GDMA_StructInit(GDMA_InitStruct);

	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock  = 1;

	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = UART_DEV_TABLE[UartIndex].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RBR_OR_UART_THR;
	GDMA_InitStruct->GDMA_Index   = 1;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	if (((TxCount & 0x03) == 0) && (((u32)(pTxBuf) & 0x03) == 0)) {
		/* 4-byte aligned: transfer 4 bytes at a time */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = TxCount >> 2;
	} else {
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = TxCount;
	}

	//assert_param(GDMA_InitStruct->GDMA_BlockSize <= 65535);//remove to avoid warning

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief    Init and Enable UART RX GDMA.
  * @param  UartIndex:0/1/2.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxBuf: Rx Buffer.
  * @param  RxCount: Rx Count, 0 will use UART as DMA controller.
  * @retval   TRUE/FALSE
  */
bool UART_RXGDMA_Init(
	u8 UartIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	int RxCount
)
{
	u8 GdmaChnl;
	UART_TypeDef *UARTx;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pRxBuf, RxCount);

	GdmaChnl = GDMA_ChnlAlloc(1, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 0);
	if (GdmaChnl == 0xFF) {
		return FALSE;
	}

	GDMA_StructInit(GDMA_InitStruct);

	UARTx = UART_DEV_TABLE[UartIndex].UARTx;

	GDMA_InitStruct->GDMA_DIR     = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = UART_DEV_TABLE[UartIndex].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&UARTx->RBR_OR_UART_THR;
	GDMA_InitStruct->GDMA_Index   = 1;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_BlockSize = RxCount;
	GDMA_InitStruct->GDMA_DstAddr = (u32)(pRxBuf);

	//assert_param(GDMA_InitStruct->GDMA_BlockSize <= 65535);//remove to avoid warning

	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt  = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock   = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief    Fill IrDA_InitStruct with default values.
  * @param  IrDA_InitStruct : pointer to a IrDA_InitTypeDef structure.
  * @retval   None
  */
void
UART_IrDAStructInit(IrDA_InitTypeDef *IrDA_InitStruct)
{
	IrDA_InitStruct->UART_IrDARxInv     = DISABLE;
	IrDA_InitStruct->UART_IrDATxInv     = DISABLE;
	IrDA_InitStruct->UART_LowShift      = UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->UART_LowShiftVal   = 0;
	IrDA_InitStruct->UART_UpperShift    = UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->UART_UpperShiftVal = 0;
	IrDA_InitStruct->UART_RxFilterCmd   = ENABLE;
	IrDA_InitStruct->UART_RxFilterThres = 7;
}

/**
  * @brief    Configure the UART IrDA SIR interface.
  * @param  UARTx: where x can be 0/1/2.
  * @param  IrDA_InitStruct: IrDA configuration (Rx/Tx invert, pulse edge shift, Rx filter).
  * @retval   None
  */
void
UART_IrDAInit(UART_TypeDef *UARTx, IrDA_InitTypeDef *IrDA_InitStruct)
{
	u32 TempMiscr;
	u32 TempTxpulse;
	//u32 TempRxPulse;

	assert_param(IrDA_InitStruct->UART_UpperShiftVal <= 0x7fff);
	assert_param(IrDA_InitStruct->UART_LowShiftVal <= 0x7fff);
	assert_param(IS_IRDA_PUL_SHIFT(IrDA_InitStruct->UART_LowShift));
	assert_param(IS_IRDA_PUL_SHIFT(IrDA_InitStruct->UART_UpperShift));
	assert_param((IrDA_InitStruct->UART_RxFilterThres <= 0x7fff));

	TempMiscr = UARTx->MISCR;

	if (IrDA_InitStruct->UART_IrDARxInv != DISABLE) {
		TempMiscr |= RUART_BIT_IRDA_RX_INV;
	} else {
		TempMiscr &= (~ RUART_BIT_IRDA_RX_INV);
	}

	if (IrDA_InitStruct->UART_IrDATxInv != DISABLE) {
		TempMiscr |= RUART_BIT_IRDA_TX_INV;
	} else {
		TempMiscr &= (~ RUART_BIT_IRDA_TX_INV);
	}
	UARTx->MISCR = TempMiscr;

	TempTxpulse = UARTx->SIR_TX_PWC0;

	/* Configure SIR TX pulse width (upper/lower bound shift direction and value) */
	TempTxpulse &= (~ RUART_BIT_UPPERBOUND_SHIFTRIGHT);
	TempTxpulse |= (IrDA_InitStruct->UART_UpperShift << 31);

	TempTxpulse &= (~ RUART_MASK_UPPERBOUND_SHIFTVAL);
	TempTxpulse |= (IrDA_InitStruct->UART_UpperShiftVal << 16);

	TempTxpulse &= (~ RUART_BIT_LOWBOUND_SHIFTRIGHT);
	TempTxpulse |= (IrDA_InitStruct->UART_LowShift << 15);

	TempTxpulse &= (~ RUART_MASK_LOWBOUND_SHIFTVAL);
	TempTxpulse |= (IrDA_InitStruct->UART_LowShiftVal);

	UARTx->SIR_TX_PWC0 = TempTxpulse;
}

/** @brief  Enable or disable the IrDA SIR function. */
void
UART_IrDACmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		UARTx->MISCR |= RUART_BIT_IRDA_ENABLE;
	} else {
		UARTx->MISCR &= (~ RUART_BIT_IRDA_ENABLE);
	}
}

/**
 * @brief  Get ovsr & ovsr_adj parameters according to the given baudrate and UART IP clock.
 * @param  IPclk UART IP clock in Hz.
 * @param  baudrate Desired baudrate in bps.
 * @param  div Pointer to div parameter.
 * @param  ovsr Pointer to ovsr parameter.
 * @param  ovsr_adj Pointer to ovsr_adj parameter.
 * @note   override ROM __weak version to fix overflow with u64 cast
 */
void UARTNormal_BaudParaGetFull(u32 IPclk, u32 baudrate, u32 *div, u32 *ovsr, u32 *ovsr_adj)
{
	u32 clock_to_baudrate_ratio = IPclk / baudrate;
	u32 current_ovsr_adj_popcount = 0, amount_of_ovsr_bits = 4, amount_Of_ovsr_adj_bits = 11;
	int current_divisor, current_ovsr, error_value, minimum_error = INT_MAX;

	for (current_ovsr = (1 << amount_of_ovsr_bits) - 1; current_ovsr >= 0; current_ovsr--) {
		current_divisor =  clock_to_baudrate_ratio / (current_ovsr + 5);
		if (current_divisor == 0) {
			continue;
		}
		for (current_ovsr_adj_popcount = 0; current_ovsr_adj_popcount <= amount_Of_ovsr_adj_bits; current_ovsr_adj_popcount++) {
			error_value = (u64)IPclk * 128 / (current_divisor * ((current_ovsr * 128) + 640 + (current_ovsr_adj_popcount * 128) / 11)) - baudrate;
			error_value = error_value < 0 ? -error_value : error_value;
			if (error_value < minimum_error) {
				minimum_error = error_value;
				*div = current_divisor;
				*ovsr = current_ovsr;
				*ovsr_adj = (1 << current_ovsr_adj_popcount) - 1;
			}
		}
	}
}

/**
 * @brief Override ROM UART_Init to fix two bugs:
 *        1. RUART_RECVTRG() double-shifts pre-shifted RxFifoTrigLevel values,
 *           causing trigger level to always be 1 byte.
 *        2. RUART_MCL_FLOW_ENABLE was missing RUART_BIT_AFE_CTS, causing TX
 *           to ignore CTS in auto flow control mode.
 */
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct)
{
	u32 TempFcr = 0;

	assert_param(IS_UART_RX_ERROR_REPORT(UART_InitStruct->RxErReportCtrl));

	UARTx->STS |= RUART_BIT_RESET_RCV;
	UARTx->STS &= ~RUART_BIT_RESET_RCV;

	UART_ClearRxFifo(UARTx);
	UART_ClearTxFifo(UARTx);

	UARTx->DLM_OR_IER = 0x00;
	UARTx->SCR |= RUART_BIT_SCRATCH_7;

	TempFcr = UART_GetFCR(UARTx);
	TempFcr &= ~(RUART_BIT_FIFO_EN | RUART_MASK_RECVTRG);

	/* Fix: use pre-shifted value directly instead of RUART_RECVTRG() which double-shifts */
	TempFcr |= (UART_InitStruct->RxFifoTrigLevel & RUART_MASK_RECVTRG);

	if (UART_InitStruct->RxErReportCtrl == UART_RX_ERROR_REPORT_ENABLE) {
		TempFcr |= RUART_BIT_FIFO_EN;
	} else {
		TempFcr &= ~RUART_BIT_FIFO_EN;
	}

	UARTx->IIR_OR_FCR = TempFcr;

	/* Fix: RUART_MCL_FLOW_ENABLE now includes RUART_BIT_AFE_CTS */
	if (UART_InitStruct->FlowControl == ENABLE) {
		UARTx->MCR |= RUART_MCL_FLOW_ENABLE;
	} else {
		UARTx->MCR &= ~RUART_MCL_FLOW_ENABLE;
	}

	UARTx->LCR = ((UART_InitStruct->WordLen) |
				  (UART_InitStruct->StopBit << 2) |
				  (UART_InitStruct->Parity << 3) |
				  (UART_InitStruct->ParityType << 4) |
				  (UART_InitStruct->StickParity << 5));

	TempFcr = UART_GetFCR(UARTx);
	if (UART_InitStruct->DmaModeCtrl != DISABLE) {
		TempFcr |= RUART_BIT_DMA_MODE;
	} else {
		TempFcr &= (~RUART_BIT_DMA_MODE);
	}
	UARTx->IIR_OR_FCR = TempFcr;
}

/**
 * @brief Override ROM UART_SetRxLevel to fix RUART_RECVTRG() double-shift bug.
 */
void UART_SetRxLevel(UART_TypeDef *UARTx, u32 FifoLv)
{
	u32 RegValue = 0;

	RegValue = UART_GetFCR(UARTx);
	RegValue &= ~RUART_MASK_RECVTRG;
	/* Fix: use pre-shifted value directly instead of RUART_RECVTRG() */
	RegValue |= (FifoLv & RUART_MASK_RECVTRG);
	UARTx->IIR_OR_FCR = RegValue;
}

void
UART_RxCmd(UART_TypeDef *UARTLOG, u32 NewState)
{
	(void)UARTLOG;
	(void)NewState;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
