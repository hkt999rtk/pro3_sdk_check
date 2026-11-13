#include <stdint.h>
#include <stdbool.h>
#include "ameba_soc.h"
#include "FreeRTOS.h"
#include "task.h"

static uint32_t UART_BASE[3] = {0x40202000, 0x40203000, 0x40204000};

#define LCR			0x0C
#define LSR			0x14
#define STSR		0x20
#define SCR			0x1C
#define AGGC		0x58
#define RX_CTRL		0x40
#define RX_STSR		0x48

#define THR			0x5c
#define TFLR		0x54

#define RBR			0x24

//#define BIT(x)	(1<<(x))

#define MASK(bs, be)	( ((1<<(bs)+1)-1) & (~(1<<(be+1)-1)) )

void uart_open(int idx)
{
	// DO nothing, NP has been initilized
}

__attribute__((optimize("O1")))
int uart_get_tx_fifo_not_full()
{
	return (*(uint32_t *)(UART_BASE[0] + LSR) >> 20) & 0x1;
}

__attribute__((optimize("O1")))
int uart_get_rx_fifolv(int idx)
{
	return (*(uint32_t *)(UART_BASE[idx] + LSR) >> 24) & 0xFF;
}


__attribute__((optimize("O1")))
void uart_write_byte(int idx, uint8_t c)
{
	while (uart_get_tx_fifo_not_full() == 0) {
		asm(" nop");
	}

	*(volatile uint32_t *)(UART_BASE[idx] + THR) = c;
}

__attribute__((optimize("O1")))
uint32_t uart_read_byte(int idx)
{
	return *(uint32_t *)(UART_BASE[idx] + RBR);
}

__attribute__((optimize("O1")))
void uart_puts(int idx, char *str)
{
	while (*str) {
		uart_write_byte(idx, *str++);
	}
}

__attribute__((optimize("O1")))
void uart_test(int idx)
{
	char *test_str = "\nUART _test_123456789_abcdefghijk\n";
	test_str[6] = '0' + idx;
	uart_puts(idx, test_str);
}

// for diag.c on FPGA with pro2 UART
void LOGUART_PutChar(uint8_t c)
{
	if (c == '\n') {
		uart_write_byte(0, '\r');    // insert return
	}
	uart_write_byte(0, c);
}

void LOGUART_StructInit(LOGUART_InitTypeDef  *LOGUART_InitStruct)
{
	(void)LOGUART_InitStruct;
}

void LOGUART_Init(LOGUART_InitTypeDef  *LOGUART_InitStruct)
{
	(void)LOGUART_InitStruct;
	uart_open(0);
}

uint32_t LOGUART_GetStatus(LOGUART_TypeDef *UARTLOG)
{
	if (uart_get_rx_fifolv(0) > 0) {
		return LOGUART_BIT_DRDY;
	} else {
		return 0;
	}
}

uint32_t LOGUART_GetIMR(void)
{
	return 0;
}

uint32_t LOGUART_GetRxCount(void)
{
	return uart_get_rx_fifolv(0);
}

void LOGUART_SetIMR(uint32_t Value)
{
	return;
}

__attribute__((optimize("O1")))
uint8_t LOGUART_GetChar(BOOL PullMode)
{
	while (LOGUART_GetRxCount() == 0) {
		asm(" nop");
	}
	return uart_read_byte(0);
}

/* for CTC startkit */

void __PutCharacter(char c1, char c2)
{
	LOGUART_PutChar(c2);
}

void __UartInit(void)
{
	uart_open(0);
}
