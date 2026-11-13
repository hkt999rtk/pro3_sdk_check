/*
 * Copyright (c) 2015, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPI_SHEIPA_H
#define SPI_SHEIPA_H

/* SPIC flash base */
#define SPIC_BASE		0x29000000
#define SYSON_BASE		0x40230000
//#define SPIC_SIZE		0x200			//?
//#define FLASH_BASE		0x08000000
//#define FLASH_SIZE		(64*1024*1024)
//#define FLASH_FIP_OFFSET	0x100000	// from BKUP
/* SPIC max slaves is 16(hw fixed value) */
#define MAX_SLAVES		16
/* FLASH base address for auto mode */
#define	FIFO_SIZE		64
#define	FIFO_HALF_SIZE	(FIFO_SIZE / 2)
/* Indicate num of data frames for read */
#define MAX_NUM_FRAME	0x10000

/* Soc tunning dummy cycle only. */
#define DEF_RD_TUNING_DUMMY_CYCLE  0x0
/* General flash opcode. */
#define WRSR		0x01
#define PP		0x02
#define NORM_READ	0x03
#define WRDI		0x04	/* write disable */
#define RDSR		0x05
#define WREN		0x06
#define FAST_READ       0x0b
#define RDID		0x9f
#define CE		0xc7	/* chip erase */
#define BE_4K		0x20	/* erase 4KiB Block */
#define SE		0xd8	/* sector erase(usually 64KiB) */


/*
 *  Used in conjunction with bitops.h to access register bitfields.
 *  They are defined as bit offset/mask pairs for each DMA register
 *  bitfield.
 */
#define BFO_SPI_FLASH_SR_BUSY                ((uint32_t)    0)
#define BFO_SPI_FLASH_SR_TXE                 ((uint32_t)    5)
#define BFO_SPI_FLASH_AUTO_LEN_DUM           ((uint32_t)     0)
#define BFW_SPI_FLASH_AUTO_LEN_DUM           ((uint32_t)    16)

#define SPIC_VALID_CMD_MASK		(0xffff)

#define BIT_SER 		((u32)0x00000001 << 0) 
#define SCKDV(x)     	(((u32)((x) & 0x00000FFF) << 0))

#define MASK_AUTO_ADDR_LENGTH    ((u32)0x0000000F << 16)
#define AUTO_ADDR_LENGTH(x)      (((u32)((x) & 0x0000000F) << 16))

#define MASK_USER_ADDR_LENGTH       ((u32)0x0000000F << 16) 
#define MASK_USER_CMD_LENGHT        ((u32)0x00000003 << 12) 
#define USER_ADDR_LENGTH(x)         (((u32)((x) & 0x0000000F) << 16))
#define USER_CMD_LENGHT(x)          (((u32)((x) & 0x00000003) << 12))

#define BIT_DR_FIXED         ((u32)0x00000001 << 3) 
#define BIT_WPN_DNUM         ((u32)0x00000001 << 2)          
#define BIT_WPN_SET          ((u32)0x00000001 << 1)          
#define BIT_SO_DNUM          ((u32)0x00000001 << 0) 

#define BIT_PHY_RD_VALID     ((u32)0x00000001 << 16)  
#define MASK_IN_PHYSICAL_CYC     ((u32)0x0000000F << 12)
#define SYSON_DPHY_RD_DELAY(x)                      ((u32)(((x) & 0x00000007) << 2))
#define SYSON_BIT_DPHY_EN                           ((u32)0x00000001 << 5)

#define SYSON_MASK_DPHY_RD_DELAY_FACTOR             ((u32)0x00000003 << 0) 
#define SYSON_MASK_DPHY_RD_DELAY                    ((u32)0x00000007 << 2)   

#define MASK_CMD_CH         ((u32)0x00000003 << 20)           /*!<R/W 0x0  Indicates channel number of command phase in transmitting or receiving data. Command phase is usually used to send SPI command.  0: single channel 1: dual channels 2: quad channels 3: octal channel*/
#define MASK_DATA_CH        ((u32)0x00000003 << 18)           /*!<R/W 0x0  Indicates channel number of data phase in transmitting or receiving data. Data phase is used to send data after address phase.  0: single channel 1: dual channels 2: quad channels 3: octal channel*/
#define MASK_ADDR_CH        ((u32)0x00000003 << 16)  

/* Define macro */
#define DW_BIT_MASK_WIDTH(__bfws, __bits) ((uint32_t)((__bfws) == 32) ?  \
		0xFFFFFFFF : (((1 << (__bits)) - 1) << (__bfws)))
#define DW_BITS_SET_VAL(__datum, __bfws, __val, bit_num)	\
	((__datum) = ((uint32_t)(__datum) &	\
			~DW_BIT_MASK_WIDTH(__bfws, bit_num)) |	\
		((__val << (__bfws)) & DW_BIT_MASK_WIDTH(__bfws, bit_num)))

struct sheipa_spi {
	void	*regs;
	void	*auto_regs;
};

/*
 * This is the structure used for accessing the spi_flash register
 * portmap.
 */
struct spi_flash_portmap {
/* Channel registers
 * The offset address for each of the channel registers
 * is shown for channel 0. For other channel numbers
 * use the following equation.
 * offset = (channel_num * 0x058) + channel_0 offset
 */
	struct {
		volatile uint32_t ctrlr0;	/* Control Reg 0 (0x000) */
		volatile uint32_t rxndf;	/* Control Reg 1 (0x004) */
		volatile uint32_t ssienr;	/* SPIC enable Reg1 (0x008) */
		volatile uint32_t rsvd0;
		volatile uint32_t ser;		/* Slave enable Reg (0x010) */
		volatile uint32_t baudr;
		volatile uint32_t txftlr;	/* TX_FIFO th level (0x018) */
		volatile uint32_t rxftlr;
		volatile uint32_t txflr;	/* TX_FIFO th level (0x020) */
		volatile uint32_t rxflr;
		volatile uint32_t sr;		/* Dest Status Reg  (0x028) */
		volatile uint32_t imr;
		volatile uint32_t isr;		/* Intr Stauts Reg (0x030) */
		volatile uint32_t risr;
		volatile uint32_t txoicr;	/* TX_FIFO overflow (0x038) */
		volatile uint32_t rxoicr;
		volatile uint32_t rxuicr;	/* RX_FIFO underflow (0x040) */
		volatile uint32_t msticr;
		volatile uint32_t icr;		/* Intr clear Reg (0x048) */
		volatile uint32_t dmacr;
		volatile uint32_t dmatdlr;	/* DMA TX_data level (0x050) */
		volatile uint32_t dmardlr;
		volatile uint32_t idr;		/* Identify Scatter (0x058) */
		volatile uint32_t spi_flash_version;
		union{
			volatile uint8_t  byte;
			volatile uint16_t half;
			volatile uint32_t word;
		} dr[16];
		volatile uint32_t dm_dr[16];	/* data mask data register */
		volatile uint32_t rd_fast_single;
		volatile uint32_t rd_dual_o;	/* Read dual data (0x0e4) */
		volatile uint32_t rd_dual_io;
		volatile uint32_t rd_quad_o;	/* Read quad data (0x0ec) */
		volatile uint32_t rd_quad_io;
		volatile uint32_t wr_single;	/* write single (0x0f4) */
		volatile uint32_t wr_dual_i;
		volatile uint32_t wr_dual_ii;	/* write dual addr/data(0x0fc)*/
		volatile uint32_t wr_quad_i;
		volatile uint32_t wr_quad_ii;	/* write quad addr/data(0x104)*/
		volatile uint32_t wr_enable;
		volatile uint32_t rd_status;	/* read status (0x10c) */
		volatile uint32_t ctrlr2;
		volatile uint32_t fbaudr;		/* fast baud rate (0x114) */
		volatile uint32_t user_length;
		volatile uint32_t auto_length;	/* Auto addr length (0x11c) */
		volatile uint32_t valid_cmd;
		volatile uint32_t flash_size;	/* Flash size (0x124) */
		volatile uint32_t flush_fifo;
		volatile uint32_t dum_byte;
		volatile uint32_t txndf;
		volatile uint32_t device_info;
		volatile uint32_t trp0;
		volatile uint32_t auto_length2;	/* Auto addr length 2 (0x13c) */
		volatile uint32_t trp1;
		volatile uint32_t slot_stavre;	/* SLOT STARVATION CONTROL REGISTER (0x144) */
		volatile uint32_t pdex_ctrl0;	/* PDEX CONTROL REGISTER 0 Register (0x148) */
		volatile uint32_t pdex_ctrl1;	/* PDEX CONTROL REGISTER 1 Register (0x14C) */
		volatile uint32_t pdex_ctrl2;	/* PDEX CONTROL REGISTER 2 Register (0x150) */
		volatile uint32_t trp2;
		volatile uint32_t icg_en;
		volatile uint32_t wbuf_id;
		volatile uint32_t dcx_ctrl;
		volatile uint32_t dcx_cmd;
		volatile uint32_t dcx_addr;
		volatile uint32_t dcx_data;
		volatile uint32_t page_ctrl0;
		volatile uint32_t page_ctrl1;
		volatile uint32_t rsvd1[2];
		union{
			volatile uint8_t  byte;
			volatile uint16_t half;
			volatile uint32_t word;
		} st_dr[16];
		volatile uint32_t stflr;
		volatile uint32_t rsvd2[3];
		volatile uint32_t page_read;
		volatile uint32_t rsvd3[11];
		/* SEC setting, below*/
	};
};

struct spi_flash_param {
	uint32_t num_slaves;	/* slaves number */
	uint32_t fifo_size;	/* TX fifo depth number */
	uint32_t baudr_div;
};


/* From ameba_flash.h */
typedef struct {
	u8 FLASH_Id;				/*!< Specifies the flash vendor ID.
								This parameter can be a value of @ref FLASH_Vendor_ID */
	u8 FLASH_cur_bitmode;		/*!< Specifies the current bitmode of SPIC.
								This parameter can be a value of @ref FLASH_Bit_Mode */
	u8 FLASH_baud_rate;			/*!< Specifies the spi_sclk divider value. The frequency of spi_sclk is derived from:
								Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
	u8 FLASH_baud_boot;			/*!< Specifies the spi_sclk divider value for rom boot. The frequency of spi_sclk is derived from:
								Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */

	u8 debug;					/*!< Specifies whether or not to print debug log.*/
	u8 FLASH_cur_cmd; 			/*!< Specifies the current read cmd which is used to read data from flash in current bitmode. */
	u8 FLASH_addr_phase_len;	/*!< Specifies the number of bytes in address phase (between command phase and write/read phase).
								This parameter can be 0/1/2/3. 0 means 4-byte address mode in SPI Flash.*/
	u8 FLASH_pinmux;			/*!< Specifies which pinmux is used. PINMUX_S0 or PINMUX_S1*/

	/* status bits define */
	u32 FLASH_QuadEn_bit;		/*!< Specifies the QE bit in status register which is used to enable Quad I/O mode . */
	u32 FLASH_Busy_bit;			/*!< Specifies the WIP(Write in Progress) bit in status register which indicates whether
								the device is busy in program/erase/write status register progress. */
	u32 FLASH_WLE_bit; 			/*!< Specifies the WEL(Write Enable Latch) bit in status register which indicates whether
								the device will accepts program/erase/write status register instructions*/
	u8 FLASH_Status2_exist;	/*!< Specifies whether this flash chip has Status Register2 or not.
								This parameter can be 0/1. 0 means it doesn't have Status Register2, 1 means it has Status Register2.*/

	/* valid R/W command set */
	u8 FLASH_rd_dual_o; 			/*!< Specifies dual data read cmd */
	u8 FLASH_rd_dual_io; 			/*!< Specifies dual data/addr read cmd */
	u8 FLASH_rd_quad_o; 			/*!< Specifies quad data read cmd */
	u8 FLASH_rd_quad_io; 			/*!< Specifies quad data/addr read cmd */
	u8 FLASH_rd_fast_single;		/*!< Specifies fast read cmd in auto mode.*/

	/* other command set */
	u8 FLASH_cmd_wr_en;			/*!< Specifies the Write Enable(WREN) instruction which is for setting WEL bit*/
	u8 FLASH_cmd_rd_id;			/*!< Specifies the Read ID instruction which is for getting the identity of the flash divice.*/
	u8 FLASH_cmd_rd_status;		/*!< Specifies the Read Status Register instruction which is for getting the status of flash */
	u8 FLASH_cmd_rd_status2;	/*!< Specifies the Read Status Register2 instruction which is for getting the status2 of flash */
	u8 FLASH_cmd_wr_status;		/*!< Specifies the Write Status Register instruction which is for setting the status register of flash */
	u8 FLASH_cmd_wr_status2;	/*!< Specifies the Write Status Register2 instruction which is for setting the status register2 of flash.
								In some flash chips, status2 write cmd != status1 write cmd, like: GD25Q32C, GD25Q64C,GD25Q128C etc.*/
	u8 FLASH_cmd_chip_e;		/*!< Specifies the Erase Chip instruction which is for erasing the whole chip*/
	u8 FLASH_cmd_block_e;		/*!< Specifies the Erase Block instruction which is for erasing 64kB*/
	u8 FLASH_cmd_sector_e;		/*!< Specifies the Erase Sector instruction which is for erasing 4kB*/
	u8 FLASH_cmd_pwdn_release;	/*!< Specifies the Release from Deep Power Down instruction which is for exiting power down mode.*/
	u8 FLASH_cmd_pwdn;			/*!< Specifies the Deep Power Down instruction which is for entering power down mode.*/
	u8 FLASH_prm_byte;
	u8 FLASH_prm_avoid;			/*!< Specifies enable to push one dummy byte (DUM_BYTE_VAL) after pushing address to Flash in auto read.
								If RD_DUMMY_LENGTH = 0, it won't push the dummy byte. */

	u8 FLASH_rd_dummy_cycle[5];	/*!< Specifies the read dummy cycle of different bitmode according to flash datasheet*/
	u8 FLASH_rd_c_dummy_cycle[5];	/*!< Specifies the read dummy cycle (continuous mode) of different bitmode according to flash datasheet*/
	/* nand flash command set */
	u8 FLASH_pagesize_main_bit_exp;		/*!< Specifies main aera of the page size is expressed as a power of 2*/
	u8 FLASH_pagesize_spare_bit_exp;	/*!< Specifies spare aera of the page size is expressed as a power of 2*/
	u8 FLASH_page_per_block_bit_exp;	/*!< Specifies page size per block is expressed as a power of 2*/

	u8 FLASH_cmd_page_read;		/*!< Specifies array read cmd in nand flash.*/
	u8 FLASH_cmd_page_write;	/*!< Specifies array write cmd in nand flash.*/

	u8 FLASH_EFail_bit; 		/*!< Specifies the E_Fail bit in status register which shows the status of erase failure or
								if host erase any invalid address or protected area (including protected blocks or protected Secure OTP area).*/
	u8 FLASH_PFail_bit;		/*!< Specifies the P_Fail bit in status register which shows the status of program failure or
								if host program any invalid address or protected area (including protected blocks or protected Secure OTP area).*/
	u8 FLASH_ECC_bit;			/*!< Specifies the ECC_Sx bit in status register which shows the ECC status of the current valid Read operation.*/
	u8 FLASH_ECC_EN_bit;		/*!< Specifies the ECC_EN bit in Configuration register which can enable or disable the internal ECC of nand flash.*/
	u8 FLASH_BUF_CONT_bit;			/*!< Specifies the Buffer Read / Continuous Read Mode Bit in status register which shows the Buffer Read / Continuous Read Mode of the current valid Read operation.*/
	u8 FLASH_SPIC_DPHY_bit;		/*!<Specifies the state of SPIC DPHY (enabled/disabled)>*/
	u8 FLASH_IN_PHYSICAL_CYC;	/*!<Specifies the IN_PHYSICAL_CYC value>*/
} flash_init_t;
#endif
