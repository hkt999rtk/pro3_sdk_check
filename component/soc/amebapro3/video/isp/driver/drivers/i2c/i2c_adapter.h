#ifndef _I2C_ADAPTER_H_INC_
#define _I2C_ADAPTER_H_INC_

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define BIT(nr) (1 << nr)

/* To determine what functionality is present */

#define I2C_FUNC_I2C 0x00000001
#define I2C_FUNC_10BIT_ADDR 0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING 0x00000004 /* I2C_M_IGNORE_NAK etc. */
#define I2C_FUNC_SMBUS_PEC 0x00000008
#define I2C_FUNC_NOSTART 0x00000010 /* I2C_M_NOSTART */
#define I2C_FUNC_SLAVE 0x00000020
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL 0x00008000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_QUICK 0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE 0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE 0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA 0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA 0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA 0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA 0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL 0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA 0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA 0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK 0x04000000 /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK 0x08000000 /* w/ 1-byte reg. addr. */
#define I2C_FUNC_SMBUS_HOST_NOTIFY 0x10000000

#define I2C_FUNC_SMBUS_BYTE \
	(I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE)
#define I2C_FUNC_SMBUS_BYTE_DATA \
	(I2C_FUNC_SMBUS_READ_BYTE_DATA | I2C_FUNC_SMBUS_WRITE_BYTE_DATA)
#define I2C_FUNC_SMBUS_WORD_DATA \
	(I2C_FUNC_SMBUS_READ_WORD_DATA | I2C_FUNC_SMBUS_WRITE_WORD_DATA)
#define I2C_FUNC_SMBUS_BLOCK_DATA \
	(I2C_FUNC_SMBUS_READ_BLOCK_DATA | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)
#define I2C_FUNC_SMBUS_I2C_BLOCK \
	(I2C_FUNC_SMBUS_READ_I2C_BLOCK | I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

#define I2C_FUNC_SMBUS_EMUL                                           \
	(I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE |                 \
	 I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |        \
	 I2C_FUNC_SMBUS_PROC_CALL | I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | \
	 I2C_FUNC_SMBUS_I2C_BLOCK | I2C_FUNC_SMBUS_PEC)

/* I2C Frequency Modes */
#define I2C_MAX_STANDARD_MODE_FREQ 100000
#define I2C_MAX_FAST_MODE_FREQ 400000
#define I2C_MAX_FAST_MODE_PLUS_FREQ 1000000
#define I2C_MAX_TURBO_MODE_FREQ 1400000
#define I2C_MAX_HIGH_SPEED_MODE_FREQ 3400000
#define I2C_MAX_ULTRA_FAST_MODE_FREQ 5000000

#define DW_IC_DEFAULT_FUNCTIONALITY                                      \
	(I2C_FUNC_I2C | I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA | \
	 I2C_FUNC_SMBUS_WORD_DATA | I2C_FUNC_SMBUS_BLOCK_DATA |          \
	 I2C_FUNC_SMBUS_I2C_BLOCK)

#define DW_IC_CON_MASTER 0x1
#define DW_IC_CON_SPEED_STD 0x2
#define DW_IC_CON_SPEED_FAST 0x4
#define DW_IC_CON_SPEED_HIGH 0x6
#define DW_IC_CON_SPEED_MASK 0x6
#define DW_IC_CON_10BITADDR_SLAVE 0x8
#define DW_IC_CON_10BITADDR_MASTER 0x10
#define DW_IC_CON_RESTART_EN 0x20
#define DW_IC_CON_SLAVE_DISABLE 0x40
#define DW_IC_CON_STOP_DET_IFADDRESSED 0x80
#define DW_IC_CON_TX_EMPTY_CTRL 0x100
#define DW_IC_CON_RX_FIFO_FULL_HLD_CTRL 0x200

/*
 * Registers offset
 */
#define FORCE_REG_ASYNC_RST 0x588d8008
#define I2C1_CLK_CFG_REG 0x588D010c
#define CLK_ENABLE BIT(24)
#define CLK_SELECT_DIV2 BIT(3)
#define I2C_SELECT_PLL1_5 0x3

#define I2C1_BASE 0x58881000
#define I2C_REG(x) (I2C1_BASE + x)

#define DW_IC_CON I2C_REG(0x0)
#define DW_IC_TAR I2C_REG(0x4)
#define DW_IC_SAR I2C_REG(0x8)
#define DW_IC_DATA_CMD I2C_REG(0x10)
#define DW_IC_SS_SCL_HCNT I2C_REG(0x14)
#define DW_IC_SS_SCL_LCNT I2C_REG(0x18)
#define DW_IC_FS_SCL_HCNT I2C_REG(0x1c)
#define DW_IC_FS_SCL_LCNT I2C_REG(0x20)
#define DW_IC_HS_SCL_HCNT I2C_REG(0x24)
#define DW_IC_HS_SCL_LCNT I2C_REG(0x28)
#define DW_IC_INTR_STAT I2C_REG(0x2c)
#define DW_IC_INTR_MASK I2C_REG(0x30)
#define DW_IC_RAW_INTR_STAT I2C_REG(0x34)
#define DW_IC_RX_TL I2C_REG(0x38)
#define DW_IC_TX_TL I2C_REG(0x3c)
#define DW_IC_CLR_INTR I2C_REG(0x40)
#define DW_IC_CLR_RX_UNDER I2C_REG(0x44)
#define DW_IC_CLR_RX_OVER I2C_REG(0x48)
#define DW_IC_CLR_TX_OVER I2C_REG(0x4c)
#define DW_IC_CLR_RD_REQ I2C_REG(0x50)
#define DW_IC_CLR_TX_ABRT I2C_REG(0x54)
#define DW_IC_CLR_RX_DONE I2C_REG(0x58)
#define DW_IC_CLR_ACTIVITY I2C_REG(0x5c)
#define DW_IC_CLR_STOP_DET I2C_REG(0x60)
#define DW_IC_CLR_START_DET I2C_REG(0x64)
#define DW_IC_CLR_GEN_CALL I2C_REG(0x68)
#define DW_IC_ENABLE I2C_REG(0x6c)
#define DW_IC_STATUS I2C_REG(0x70)
#define DW_IC_TXFLR I2C_REG(0x74)
#define DW_IC_RXFLR I2C_REG(0x78)
#define DW_IC_SDA_HOLD I2C_REG(0x7c)
#define DW_IC_TX_ABRT_SOURCE I2C_REG(0x80)
#define DW_IC_ENABLE_STATUS I2C_REG(0x9c)
#define DW_IC_CLR_RESTART_DET I2C_REG(0xa8)
#define DW_IC_COMP_PARAM_1 I2C_REG(0xf4)
#define DW_IC_COMP_VERSION I2C_REG(0xf8)
#define DW_IC_SDA_HOLD_MIN_VERS 0x3131312A
#define DW_IC_COMP_TYPE I2C_REG(0xfc)
#define DW_IC_COMP_TYPE_VALUE 0x44570140

#define DW_IC_INTR_RX_UNDER 0x001
#define DW_IC_INTR_RX_OVER 0x002
#define DW_IC_INTR_RX_FULL 0x004
#define DW_IC_INTR_TX_OVER 0x008
#define DW_IC_INTR_TX_EMPTY 0x010
#define DW_IC_INTR_RD_REQ 0x020
#define DW_IC_INTR_TX_ABRT 0x040
#define DW_IC_INTR_RX_DONE 0x080
#define DW_IC_INTR_ACTIVITY 0x100
#define DW_IC_INTR_STOP_DET 0x200
#define DW_IC_INTR_START_DET 0x400
#define DW_IC_INTR_GEN_CALL 0x800
#define DW_IC_INTR_RESTART_DET 0x1000

#define DW_IC_INTR_DEFAULT_MASK \
	(DW_IC_INTR_RX_FULL | DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET)
#define DW_IC_INTR_MASTER_MASK (DW_IC_INTR_DEFAULT_MASK | DW_IC_INTR_TX_EMPTY)
#define DW_IC_INTR_SLAVE_MASK                                                 \
	(DW_IC_INTR_DEFAULT_MASK | DW_IC_INTR_RX_DONE | DW_IC_INTR_RX_UNDER | \
	 DW_IC_INTR_RD_REQ)

#define DW_IC_STATUS_ACTIVITY 0x1
#define DW_IC_STATUS_TFE BIT(2)
#define DW_IC_STATUS_MASTER_ACTIVITY BIT(5)
#define DW_IC_STATUS_SLAVE_ACTIVITY BIT(6)

#define DW_IC_SDA_HOLD_RX_SHIFT 16
#define DW_IC_SDA_HOLD_RX_MASK 0xff0000

#define DW_IC_ERR_TX_ABRT 0x1

#define DW_IC_TAR_10BITADDR_MASTER BIT(12)

#define DW_IC_COMP_PARAM_1_SPEED_MODE_HIGH (BIT(2) | BIT(3))
#define DW_IC_COMP_PARAM_1_SPEED_MODE_MASK 0xc

/*
 * status codes
 */
#define STATUS_IDLE 0x0
#define STATUS_WRITE_IN_PROGRESS 0x1
#define STATUS_READ_IN_PROGRESS 0x2

/*
 * operation modes
 */
#define DW_IC_MASTER 0
#define DW_IC_SLAVE 1

/*
 * Hardware abort codes from the DW_IC_TX_ABRT_SOURCE register
 *
 * Only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK 0
#define ABRT_10ADDR1_NOACK 1
#define ABRT_10ADDR2_NOACK 2
#define ABRT_TXDATA_NOACK 3
#define ABRT_GCALL_NOACK 4
#define ABRT_GCALL_READ 5
#define ABRT_SBYTE_ACKDET 7
#define ABRT_SBYTE_NORSTRT 9
#define ABRT_10B_RD_NORSTRT 10
#define ABRT_MASTER_DIS 11
#define ARB_LOST 12
#define ABRT_SLAVE_FLUSH_TXFIFO 13
#define ABRT_SLAVE_ARBLOST 14
#define ABRT_SLAVE_RD_INTX 15

#define DW_IC_TX_ABRT_7B_ADDR_NOACK (1UL << ABRT_7B_ADDR_NOACK)
#define DW_IC_TX_ABRT_10ADDR1_NOACK (1UL << ABRT_10ADDR1_NOACK)
#define DW_IC_TX_ABRT_10ADDR2_NOACK (1UL << ABRT_10ADDR2_NOACK)
#define DW_IC_TX_ABRT_TXDATA_NOACK (1UL << ABRT_TXDATA_NOACK)
#define DW_IC_TX_ABRT_GCALL_NOACK (1UL << ABRT_GCALL_NOACK)
#define DW_IC_TX_ABRT_GCALL_READ (1UL << ABRT_GCALL_READ)
#define DW_IC_TX_ABRT_SBYTE_ACKDET (1UL << ABRT_SBYTE_ACKDET)
#define DW_IC_TX_ABRT_SBYTE_NORSTRT (1UL << ABRT_SBYTE_NORSTRT)
#define DW_IC_TX_ABRT_10B_RD_NORSTRT (1UL << ABRT_10B_RD_NORSTRT)
#define DW_IC_TX_ABRT_MASTER_DIS (1UL << ABRT_MASTER_DIS)
#define DW_IC_TX_ARB_LOST (1UL << ARB_LOST)
#define DW_IC_RX_ABRT_SLAVE_RD_INTX (1UL << ABRT_SLAVE_RD_INTX)
#define DW_IC_RX_ABRT_SLAVE_ARBLOST (1UL << ABRT_SLAVE_ARBLOST)
#define DW_IC_RX_ABRT_SLAVE_FLUSH_TXFIFO (1UL << ABRT_SLAVE_FLUSH_TXFIFO)

#define DW_IC_TX_ABRT_NOACK                                          \
	(DW_IC_TX_ABRT_7B_ADDR_NOACK | DW_IC_TX_ABRT_10ADDR1_NOACK | \
	 DW_IC_TX_ABRT_10ADDR2_NOACK | DW_IC_TX_ABRT_TXDATA_NOACK |  \
	 DW_IC_TX_ABRT_GCALL_NOACK)

#define ACCESS_INTR_MASK 0x00000001
#define ACCESS_NO_IRQ_SUSPEND 0x00000002

/*
 * These flags used only by the kernel as part of the
 * irq handling routines.
 *
 * IRQF_SHARED - allow sharing the irq among several devices
 * IRQF_PROBE_SHARED - set by callers when they expect sharing mismatches to occur
 * IRQF_TIMER - Flag to mark this interrupt as timer interrupt
 * IRQF_PERCPU - Interrupt is per cpu
 * IRQF_NOBALANCING - Flag to exclude this interrupt from irq balancing
 * IRQF_IRQPOLL - Interrupt is used for polling (only the interrupt that is
 *                registered first in a shared interrupt is considered for
 *                performance reasons)
 * IRQF_ONESHOT - Interrupt is not reenabled after the hardirq handler finished.
 *                Used by threaded interrupts which need to keep the
 *                irq line disabled until the threaded handler has been run.
 * IRQF_NO_SUSPEND - Do not disable this IRQ during suspend.  Does not guarantee
 *                   that this interrupt will wake the system from a suspended
 *                   state.  See Documentation/power/suspend-and-interrupts.rst
 * IRQF_FORCE_RESUME - Force enable it on resume even if IRQF_NO_SUSPEND is set
 * IRQF_NO_THREAD - Interrupt cannot be threaded
 * IRQF_EARLY_RESUME - Resume IRQ early during syscore instead of at device
 *                resume time.
 * IRQF_COND_SUSPEND - If the IRQ is shared with a NO_SUSPEND user, execute this
 *                interrupt handler after suspending interrupts. For system
 *                wakeup devices users need to implement wakeup detection in
 *                their interrupt handlers.
 */
#define IRQF_SHARED 0x00000080
#define IRQF_PROBE_SHARED 0x00000100
#define __IRQF_TIMER 0x00000200
#define IRQF_PERCPU 0x00000400
#define IRQF_NOBALANCING 0x00000800
#define IRQF_IRQPOLL 0x00001000
#define IRQF_ONESHOT 0x00002000
#define IRQF_NO_SUSPEND 0x00004000
#define IRQF_FORCE_RESUME 0x00008000
#define IRQF_NO_THREAD 0x00010000
#define IRQF_EARLY_RESUME 0x00020000
#define IRQF_COND_SUSPEND 0x00040000

#define IRQF_TIMER (__IRQF_TIMER | IRQF_NO_SUSPEND | IRQF_NO_THREAD)

#define I2C_M_RD 0x0001 /* read data, from slave to master */
/* I2C_M_RD is guaranteed to be 0x0001! */
#define I2C_M_TEN 0x0010 /* this is a ten bit chip address */
#define I2C_M_DMA_SAFE 0x0200 /* the buffer of this message is DMA safe */
/* makes only sense in kernelspace */
/* userspace buffers are copied anyway */
#define I2C_M_RECV_LEN 0x0400 /* length will be first received byte */
#define I2C_M_NO_RD_ACK 0x0800 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK 0x1000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR 0x2000 /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART 0x4000 /* if I2C_FUNC_NOSTART */
#define I2C_M_STOP 0x8000 /* if I2C_FUNC_PROTOCOL_MANGLING */

/**
 * struct i2c_msg - an I2C transaction segment beginning with START
 * @addr: Slave address, either seven or ten bits.  When this is a ten
 *	bit address, I2C_M_TEN must be set in @flags and the adapter
 *	must support I2C_FUNC_10BIT_ADDR.
 * @flags: I2C_M_RD is handled by all adapters.  No other flags may be
 *	provided unless the adapter exported the relevant I2C_FUNC_*
 *	flags through i2c_check_functionality().
 * @len: Number of data bytes in @buf being read from or written to the
 *	I2C slave address.  For read transactions where I2C_M_RECV_LEN
 *	is set, the caller guarantees that this buffer can hold up to
 *	32 bytes in addition to the initial length byte sent by the
 *	slave (plus, if used, the SMBus PEC); and this value will be
 *	incremented by the number of block data bytes received.
 * @buf: The buffer into which data is read, or from which it's written.
 *
 * An i2c_msg is the low level representation of one segment of an I2C
 * transaction.  It is visible to drivers in the @i2c_transfer() procedure,
 * to userspace from i2c-dev, and to I2C adapter drivers through the
 * @i2c_adapter.@master_xfer() method.
 *
 * Except when I2C "protocol mangling" is used, all I2C adapters implement
 * the standard rules for I2C transactions.  Each transaction begins with a
 * START.  That is followed by the slave address, and a bit encoding read
 * versus write.  Then follow all the data bytes, possibly including a byte
 * with SMBus PEC.  The transfer terminates with a NAK, or when all those
 * bytes have been transferred and ACKed.  If this is the last message in a
 * group, it is followed by a STOP.  Otherwise it is followed by the next
 * @i2c_msg transaction segment, beginning with a (repeated) START.
 *
 * Alternatively, when the adapter supports I2C_FUNC_PROTOCOL_MANGLING then
 * passing certain @flags may have changed those standard protocol behaviors.
 * Those flags are only for use with broken/nonconforming slaves, and with
 * adapters which are known to support the specific mangling options they
 * need (one or more of IGNORE_NAK, NO_RD_ACK, NOSTART, and REV_DIR_ADDR).
 */
struct i2c_msg {
	uint16_t addr; /* slave address			*/
	uint16_t flags;

	uint16_t len; /* msg length				*/
	uint8_t *buf; /* pointer to msg data			*/
};

/**
 * struct i2c_timings - I2C timing information
 * @bus_freq_hz: the bus frequency in Hz
 * @scl_rise_ns: time SCL signal takes to rise in ns; t(r) in the I2C specification
 * @scl_fall_ns: time SCL signal takes to fall in ns; t(f) in the I2C specification
 * @scl_int_delay_ns: time IP core additionally needs to setup SCL in ns
 * @sda_fall_ns: time SDA signal takes to fall in ns; t(f) in the I2C specification
 * @sda_hold_ns: time IP core additionally needs to hold SDA in ns
 * @digital_filter_width_ns: width in ns of spikes on i2c lines that the IP core
 *	digital filter can filter out
 * @analog_filter_cutoff_freq_hz: threshold frequency for the low pass IP core
 *	analog filter
 */
struct i2c_timings {
	uint32_t bus_freq_hz;
	uint32_t scl_rise_ns;
	uint32_t scl_fall_ns;
	uint32_t scl_int_delay_ns;
	uint32_t sda_fall_ns;
	uint32_t sda_hold_ns;
	uint32_t digital_filter_width_ns;
	uint32_t analog_filter_cutoff_freq_hz;
};

struct i2c_adapter {
	StaticSemaphore_t lock;
	StaticSemaphore_t cmd_complete;
	struct i2c_timings timings;

	int irq;
	int mode;
	int msg_err;
	int cmd_err;

	struct i2c_msg *msgs;
	int msgs_num;
	int msg_write_idx;
	int msg_read_idx;

	uint32_t rx_buf_len;
	uint8_t *rx_buf;
	uint32_t tx_buf_len;
	uint8_t *tx_buf;
	int rx_outstanding;

	uint32_t status;
	uint32_t tx_fifo_depth;
	uint32_t rx_fifo_depth;
	uint32_t sda_hold_time;
	uint16_t ss_hcnt;
	uint16_t ss_lcnt;
	uint16_t fs_hcnt;
	uint16_t fs_lcnt;
	uint16_t fp_hcnt;
	uint16_t fp_lcnt;
	uint16_t hs_hcnt;
	uint16_t hs_lcnt;
	uint32_t functionality;
	uint32_t master_cfg;
};

#endif /* _I2C_ADAPTER_H_INC_ */
