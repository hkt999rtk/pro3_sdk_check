
#include "iomem.h"
/*
 ******************************
 * Uart
 ******************************
 */
/* Uart2 register base address mapping */
#define UART2_IOBASE 0x58810200
/* Serial Clock	*/
#define UART2_FREQ 24000000
/* baudrate platform specific setting */
#define UART_BAUD_BASE (UART2_FREQ >> 4)
#define UART_DL_BAUD_57600 (UART_BAUD_BASE / 57600)
#define UART_BAUD_DEFAULT UART_DL_BAUD_57600
#define UART_BAUD_RATEL (UART_BAUD_DEFAULT & 0x00ff)
#define UART_BAUD_RATEH ((UART_BAUD_DEFAULT & 0xff00) >> 8)
#define UART_IOBASE UART2_IOBASE

/* Register offset from UARTx_IOBASE */
#define UART_RBR 0x00
#define UART_THR 0x00
#define UART_IER 0x04
#define UART_IIR 0x08
#define UART_LCR 0x0C
#define UART_LSR 0x14
#define UART_DLL 0x00
#define UART_DLH 0x04
#define UART_CTR 0xFC

/* uart LCR register bits */
#define UART_LCR_PEN 0x00000008 /* parity enable bit */
#define UART_LCR_EPS 0X00000010
#define UART_LCR_PARITY (UART_LCR_PEN | UART_LCR_EPS)
#define UART_LCR_STOP 0x00000004 /* stop bit          */
#define UART_LCR_DLS 0x00000003 /* 8 bits mode       */
#define UART_LCR_DLAB 0x00000080 /* DLAB bit          */
/*
 ******************************
 * CLK
 ******************************
 */
#define CLK_ENABLE 0x1000000
#define CLOCK_SELECT_DIV4 (0x8)

#define UART2_CLK_CFG_REG 0x588d0108
#define UART_SELECT_USBPLL_5 0x0
/*
 ******************************
 * GPIO
 ******************************
 */
#define UART2_PULLCTRL 0x58800068
#define UART2_TX_PE_SHIFT 2
#define UART2_TX_PE_MASK 0xc
#define UART2_RX_PE_SHIFT 0
#define UART2_RX_PE_MASK 3
#define UART2_PE_PU 0x2
#define UART2_PE_PD 0x1

#define UART2_PAD_CFG 0x5880007c
#define PAD_XB2_UTXD_SHIFT 8
#define PAD_XB2_UTXD_MASK (0x3 << 8)
#define UART2_TX_EN 0x2
#define PAD_XB2_URXD_SHIFT 12
#define PAD_XB2_URXD_MASK (0x3 << 12)
#define UART2_RX_EN 0x2

void serial_init(void)
{
	/*enable uart2 pad*/
	writel_mask(UART2_TX_EN << PAD_XB2_UTXD_SHIFT, UART2_PAD_CFG,
				PAD_XB2_UTXD_MASK);
	writel_mask(UART2_RX_EN << PAD_XB2_URXD_SHIFT, UART2_PAD_CFG,
				PAD_XB2_URXD_MASK);
	/*enable uart2 pad pull up*/
	writel_mask(UART2_PE_PU << UART2_TX_PE_SHIFT, UART2_PULLCTRL,
				UART2_TX_PE_MASK);
	writel_mask(UART2_PE_PU << UART2_RX_PE_SHIFT, UART2_PULLCTRL,
				UART2_RX_PE_MASK);

	writel(0, UART_IOBASE + UART_IER);
	writel(UART_LCR_DLAB, UART_IOBASE + UART_LCR);
	writel(UART_BAUD_RATEL, UART_IOBASE + UART_DLL);
	writel(UART_BAUD_RATEH, UART_IOBASE + UART_DLH);
	writel(UART_LCR_DLS, UART_IOBASE + UART_LCR);

	if (readl(UART_IOBASE + UART_CTR) == 0x44570110)
		writel(CLK_ENABLE | UART_SELECT_USBPLL_5 | CLOCK_SELECT_DIV4,
			   UART2_CLK_CFG_REG);
}

void serial_putc(int c)
{
	uint32_t status = 0;

	while (!status) {
		status = readl(UART_IOBASE + UART_LSR) & 0x00000020;
	}

	writel(c, UART_IOBASE + UART_THR);
	if (c == '\n') {
		status = 0;
		while (!status) {
			status = readl(UART_IOBASE + UART_LSR) & 0x00000020;
		}
		writel('\r', UART_IOBASE + UART_THR);
	}
}
