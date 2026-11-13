/*
 * Copyright (c) 2025 Realtek Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <osif.h>
#include <string.h>
#include <stdbool.h>
#include "ameba_soc.h"
#include "hci_uart.h"
#include "bt_debug.h"
#include "hal_platform.h"
#include "hci_platform.h"
#include "hci_controller.h"

#define HCI_UART_DEV             (UART3_DEV)
#define HCI_UART_IRQ             (BTUART_IRQ)
#define HCI_UART_IRQ_PRIO        (INT_PRI_LOWEST)

static struct hci_uart_t {
	/* UART */
	UART_InitTypeDef UART_InitStruct;

	/* UART TX */
	uint8_t         *tx_buf;
	uint16_t         tx_len;
	void            *tx_done_sem;
} *g_uart = NULL;

void hci_uart_op_irq(bool op)
{
	if (op) {
		/* Enable HCI uart irq */
		InterruptEn(HCI_UART_IRQ, HCI_UART_IRQ_PRIO);
	} else {
		/* Disable HCI uart irq */
		InterruptDis(HCI_UART_IRQ);
	}
}

void hci_uart_op_host_wake_bt(bool op)
{
	if (op) {
		/* Enable HOST_WAKE_BT */
		set_reg_value(0x4023061C, BIT0, 1);
	} else {
		/* Disable HOST_WAKE_BT */
		set_reg_value(0x4023061C, BIT0, 0);
	}
}

void hci_wakeup_bt_controller(void)
{
	hci_uart_op_host_wake_bt(true);
	osif_delay(2);    /* Ensure the high pulse >= 1ms */
	hci_uart_op_host_wake_bt(false);
	hci_uart_op_host_wake_bt(true);
	osif_delay(8);
}

uint8_t hci_uart_set_bdrate(uint32_t baudrate)
{
	UART_SetBaud(HCI_UART_DEV, baudrate);
	BT_LOGA("Set baudrate to %d success!\r\n", baudrate);
	return HCI_SUCCESS;
}

static inline void transmit_chars(void)
{
	uint16_t cnt = 0;

	if (g_uart->tx_len == 0) { /* Set TX done after TX FIFO is empty. */
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
		if (g_uart->tx_done_sem) {
			osif_sem_give(g_uart->tx_done_sem);
		}
		return;
	}

	/* Send data to TX FIFO */
	cnt = (uint16_t)UART_SendDataTO(HCI_UART_DEV, g_uart->tx_buf, g_uart->tx_len, 0);
	g_uart->tx_buf += cnt;
	g_uart->tx_len -= cnt;
}

static inline void receive_chars(void)
{
	hci_uart_rx_irq_handler(true);
}

static uint32_t _uart_irq(void *data)
{
	(void)data;
	uint32_t reg_ier, reg_iir, int_id;

	while (1) {
		reg_ier = HCI_UART_DEV->DLM_OR_IER;
		reg_iir = UART_IntStatus(HCI_UART_DEV); /* read IIR → clears interrupt */
		int_id = (reg_iir >> 1) & 0x7;

		if (reg_iir & RUART_BIT_INT_PEND) {
			return 0;                           /* no more pending */
		}

		switch (int_id) {
		case RUART_RECEIVER_DATA_AVAILABLE:     /* 0x2 */
		case RUART_TIME_OUT_INDICATION:         /* 0x6 */
			if (reg_ier & RUART_BIT_ERBI) {
				receive_chars();
			}
			break;
		case RUART_TX_FIFO_EMPTY:               /* 0x1 */
			if (reg_ier & RUART_BIT_ETBEI) {
				transmit_chars();
			}
			break;
		case RUART_RECEIVE_LINE_STATUS:         /* 0x3 */
			UART_LineStatusGet(HCI_UART_DEV);   /* read-to-clear errors */
			break;
		default:
			break;
		}
	}

	return 0;
}

uint16_t hci_uart_send(uint8_t *buf, uint16_t len)
{
	if (!g_uart) {
		BT_LOGE("g_uart is NULL!\r\n");
		return 0;
	}

	/* UART_SendData() does not work */
	g_uart->tx_buf = buf;
	g_uart->tx_len = len;

#if defined(HCI_BT_POWER_SAVE_MODE) && (HCI_BT_POWER_SAVE_MODE == 1)
	hci_acquire_bt_wakelock();

	if (!hci_is_mp_mode() && hci_get_first_wakeup()) {
		hci_wakeup_bt_controller();
		hci_set_first_wakeup(false);
	}
#endif

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, ENABLE);

	if (g_uart->tx_done_sem) {
		if (osif_sem_take(g_uart->tx_done_sem, 0xFFFFFFFF) == false) {
			BT_LOGE("g_uart->tx_done_sem take fail!\r\n");
			len = 0;
		}
	}

#if defined(HCI_BT_POWER_SAVE_MODE) && (HCI_BT_POWER_SAVE_MODE == 1)
	hci_release_bt_wakelock();
#endif

	/* Trigger TX Empty Interrrupt, so TX done here */
	return len;
}

uint16_t hci_uart_read(uint8_t *buf, uint16_t len)
{
	if (len) {
		return UART_ReceiveDataTO(HCI_UART_DEV, buf, len, 0);
	}

	return 0;
}

void hci_uart_rx_irq(bool enable)
{
	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI, enable ? ENABLE : DISABLE);
}

uint8_t hci_uart_open(void)
{
	/* Enable Clock */
	RCC_PeriphClockCmd(APBPeriph_BTUART, APBPeriph_BTUART_CLOCK, ENABLE);
	osif_delay_us(3); /* Delay 3us after enabling HCI UART clock to avoid hang on UART init before clock is ready. */

	/* Init g_uart */
	if (!g_uart) {
		g_uart = (struct hci_uart_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct hci_uart_t));
		if (!g_uart) {
			BT_LOGE("g_uart is NULL!\r\n");
			return HCI_FAIL;
		}
		memset(g_uart, 0, sizeof(struct hci_uart_t));
	}

	if (osif_sem_create(&g_uart->tx_done_sem, 0, 1) == false) {
		BT_LOGE("g_uart->tx_done_sem create fail!\r\n");
		osif_mem_free(g_uart);
		return HCI_FAIL;
	}

	/* Enable UART
	 * Use Flow Control (When rx FIFO reaches level, RTS will be pulled high)
	 * Use Baudrate 115200 (Default)
	 */
	UART_InitTypeDef *pUARTStruct = &g_uart->UART_InitStruct;
	UART_StructInit(pUARTStruct);
	pUARTStruct->WordLen = RUART_WLS_8BITS;
	pUARTStruct->StopBit = RUART_STOP_BIT_1;
	pUARTStruct->Parity = RUART_PARITY_DISABLE;
	pUARTStruct->ParityType = RUART_EVEN_PARITY;
	pUARTStruct->StickParity = RUART_STICK_PARITY_DISABLE;
	pUARTStruct->RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_16BYTES;
	pUARTStruct->FlowControl = ENABLE;
	UART_Init(HCI_UART_DEV, pUARTStruct);
	UART_SetBaud(HCI_UART_DEV, 115200);

	/* Disable and Enable UART Interrupt */
	InterruptDis(HCI_UART_IRQ);
	InterruptUnRegister(HCI_UART_IRQ);
	InterruptRegister((IRQ_FUN)_uart_irq, HCI_UART_IRQ, (u32)NULL, HCI_UART_IRQ_PRIO);
	InterruptEn(HCI_UART_IRQ, HCI_UART_IRQ_PRIO);

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ELSI, ENABLE);

	return HCI_SUCCESS;
}

uint8_t hci_uart_close(void)
{
	if (!g_uart) {
		return HCI_FAIL;
	}

	/* Disable UART Interrupt and UART */
	InterruptDis(HCI_UART_IRQ);
	InterruptUnRegister(HCI_UART_IRQ);
	UART_DeInit(HCI_UART_DEV);

	/* Disable Clock */
	RCC_PeriphClockCmd(APBPeriph_BTUART, APBPeriph_BTUART_CLOCK, DISABLE);

	return HCI_SUCCESS;
}

uint8_t hci_uart_free(void)
{
	if (!g_uart) {
		return HCI_FAIL;
	}

	if (g_uart->tx_done_sem) {
		osif_sem_delete(g_uart->tx_done_sem);
		g_uart->tx_done_sem = NULL;
	}

	osif_mem_free(g_uart);
	g_uart = NULL;

	return HCI_SUCCESS;
}

uint8_t hci_uart_deinit(void)
{
	if (!hci_uart_close()) {
		return HCI_FAIL;
	}

	if (!hci_uart_free()) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

uint8_t hci_uart_reinit(void)
{
	if (!hci_uart_open()) {
		return HCI_FAIL;
	}

	UART_SetBaud(HCI_UART_DEV, 3000000);
	return HCI_SUCCESS;
}
