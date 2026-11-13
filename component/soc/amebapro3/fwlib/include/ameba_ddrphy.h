/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DDRPHY_H_
#define _AMEBA_DDRPHY_H_

/** @defgroup DDRPHY
  * @brief DDRPHY driver modules
  * @{
  */

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup DDRPHY_Register_Definitions DDRPHY Register Definitions
 * @{
 *****************************************************************************/


/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_RST_N                        ((u32)0x00000001 << 31)          /*!<R/W 'h0 reset all digital circuits, low active*/
#define DDRPHY_MASK_RST_FIFO_MODE               ((u32)0x00000003 << 28)          /*!<R/W 'h1 Reset read FIFO pointer mode [00] during no read operation [01] during refresh [10] disable reset FIFO pointer [11] force reset FIFO pointer */
#define DDRPHY_RST_FIFO_MODE(x)                 ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_CMD_RD_STR_NUM              ((u32)0x00000007 << 24)          /*!<R/W 'h1 CMD   TX FIFO threshold setting*/
#define DDRPHY_CMD_RD_STR_NUM(x)                ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_RX_RD_STR_NUM_1             ((u32)0x00000007 << 20)          /*!<R/W 'h1 slice1 RX FIFO threshold setting*/
#define DDRPHY_RX_RD_STR_NUM_1(x)               ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_RX_RD_STR_NUM_0             ((u32)0x00000007 << 12)          /*!<R/W 'h1 slice0 RX FIFO threshold setting*/
#define DDRPHY_RX_RD_STR_NUM_0(x)               ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_DQS_RD_STR_NUM_1            ((u32)0x00000007 << 12)          /*!<R/W 'h1 slice1 DQS TX FIFO threshold setting*/
#define DDRPHY_DQS_RD_STR_NUM_1(x)              ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQ_RD_STR_NUM_1             ((u32)0x00000007 << 8)          /*!<R/W 'h1 slice1 DQ   TX FIFO threshold setting*/
#define DDRPHY_DQ_RD_STR_NUM_1(x)               ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQS_RD_STR_NUM_0            ((u32)0x00000007 << 4)          /*!<R/W 'h1 slice0 DQS TX FIFO threshold setting*/
#define DDRPHY_DQS_RD_STR_NUM_0(x)              ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQ_RD_STR_NUM_0             ((u32)0x00000007 << 0)          /*!<R/W 'h1 slice0 DQ   TX FIFO threshold setting*/
#define DDRPHY_DQ_RD_STR_NUM_0(x)               ((u32)(((x) & 0x00000007) << 0))
/** @} */


/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DATA_INIT                    ((u32)0x00000001 << 31)          /*!<R/W 'h1 Set the Tx initial value of DQ/DM*/
#define DDRPHY_BIT_2R_WR_PRE                    ((u32)0x00000001 << 30)          /*!<R/W 'h0 [1] 2T preamble for DDR4 only(Write) [0] 1T preamble */
#define DDRPHY_2R_WR_PRE(x)                     ((u32)(((x) & 0x00000001) << 30))
#define DDRPHY_MASK_TM_RD_FIFO                  ((u32)0x0000003f << 24)          /*!<R/W 'h2 read FIFO latency from read command 2T DCK unit */
#define DDRPHY_TM_RD_FIFO(x)                    ((u32)(((x) & 0x0000003f) << 24))
#define DDRPHY_MASK_DQM_OE_TIME1                ((u32)0x0000000f << 20)          /*!<R/W 'h0 Slice1 : [21:20] Control dq/dm oe enable timing. 00: original, 01: pre 0.5T enable, 10: pre 1T enable, 11: pre 1.5T enable, [23:22] Control dq/dm oe disable timing., 00: original, 01: post 1T disable, 11: pre 1T (0.5T if en_crc = 1) disable */
#define DDRPHY_DQM_OE_TIME1(x)                  ((u32)(((x) & 0x0000000f) << 20))
#define DDRPHY_MASK_DQM_OE_TIME0                ((u32)0x0000000f << 16)          /*!<R/W 'h0 Slice0 : [17:16] Control dq/dm oe enable timing. 00: original, 01: pre 0.5T enable, 10: pre 1T enable, 11: pre 1.5T enable, [19:18] Control dq/dm oe disable timing., 00: original, 01: post 1T disable, 11: pre 1T (0.5T if en_crc = 1) disable */
#define DDRPHY_DQM_OE_TIME0(x)                  ((u32)(((x) & 0x0000000f) << 16))
#define DDRPHY_MASK_TM_DQS_EN1                  ((u32)0x0000001f << 2)          /*!<R/W 'h0 Slice1 dqs_en latency from read command 2T DCK unit */
#define DDRPHY_TM_DQS_EN1(x)                    ((u32)(((x) & 0x0000001f) << 10))
#define DDRPHY_MASK_TM_DQS_EN_FTUN1             ((u32)0x00000003 << 0)          /*!<R/W 'h3 Slice1 dqs_en_ftun latency from read command 0.5T DCK unit */
#define DDRPHY_TM_DQS_EN_FTUN1(x)               ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_TM_DQS_EN0                  ((u32)0x0000001f << 2)          /*!<R/W 'h0 Slice0 dqs_en latency from read command 2T DCK unit */
#define DDRPHY_TM_DQS_EN0(x)                    ((u32)(((x) & 0x0000001f) << 2))
#define DDRPHY_MASK_TM_DQS_EN_FTUN0             ((u32)0x00000003 << 0)          /*!<R/W 'h3 Slice0 dqs_en_ftun latency from read command 0.5T DCK unit*/
#define DDRPHY_TM_DQS_EN_FTUN0(x)               ((u32)(((x) & 0x00000003) << 0))
/** @} */


/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_FCLK_RX_SEL                 ((u32)0x000001FF << 23)          /*!<R/W 'h0 Bit[31]: Reserved. Bit[30]: RX clock phase selection, 0:90 degree, 1:90 degree + delay chain, Bit[29:26] for delay chain setting Available value : 0~11 (Any value larger than 11 will be treated as 11) */
#define DDRPHY_FCLK_RX_SEL(x)                   ((u32)(((x) & 0x000001FF) << 23))
#define DDRPHY_MASK_RX_RD_STR_NUM_3             ((u32)0x00000003 << 20)          /*!<R/W 'h1 slice3 RX FIFO threshold setting*/
#define DDRPHY_RX_RD_STR_NUM_3(x)               ((u32)(((x) & 0x00000003) << 20))
#define DDRPHY_MASK_RX_RD_STR_NUM_2             ((u32)0x00000003 << 16)          /*!<R/W 'h1 slice2 RX FIFO threshold setting*/
#define DDRPHY_RX_RD_STR_NUM_2(x)               ((u32)(((x) & 0x00000003) << 16))
#define DDRPHY_MASK_DQS_RD_STR_NUM_3            ((u32)0x00000003 << 12)          /*!<R/W 'h1 slice3 DQS TX FIFO threshold setting*/
#define DDRPHY_DQS_RD_STR_NUM_3(x)              ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_DQ_RD_STR_NUM_3             ((u32)0x00000003 << 8)          /*!<R/W 'h1 slice3 DQ   TX FIFO threshold setting*/
#define DDRPHY_DQ_RD_STR_NUM_3(x)               ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_DQS_RD_STR_NUM_2            ((u32)0x00000003 << 4)          /*!<R/W 'h1 slice2 DQS TX FIFO threshold setting*/
#define DDRPHY_DQS_RD_STR_NUM_2(x)              ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_MASK_DQ_RD_STR_NUM_2             ((u32)0x00000003 << 0)          /*!<R/W 'h1 slice2 DQ   TX FIFO threshold setting*/
#define DDRPHY_DQ_RD_STR_NUM_2(x)               ((u32)(((x) & 0x00000003) << 0))
/** @} */


/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTL_6
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_FORCE_RSTN                   ((u32)0x00000001 << 27)          /*!<R/W 'h1 1 for forcing PAD_DDR_RSTN value to rstn_value*/
#define DDRPHY_BIT_RSTN_VALUE                   ((u32)0x00000001 << 26)          /*!<R/W 'h1 Value of PAD_DDR_RSTN when force_rstn = 1*/
#define DDRPHY_BIT_FORCE_CKE_1                  ((u32)0x00000001 << 18)          /*!<R/W 'h1 1 for forcing PAD_DDR_CKE_1 value to cke_1_value*/
#define DDRPHY_BIT_CKE_1_VALUE                  ((u32)0x00000001 << 17)          /*!<R/W 'h0 Value of PAD_DDR_CKE_1 when force_cke_1 = 1*/
#define DDRPHY_BIT_FORCE_CS                     ((u32)0x00000001 << 16)          /*!<R/W 'h0 1 for forcing PAD_DDR_CS_N value to cs_value*/
#define DDRPHY_BIT_CS_VALUE                     ((u32)0x00000001 << 15)          /*!<R/W 'h1 Value of PAD_DDR_CS_N when force_cs = 1*/
#define DDRPHY_BIT_FORCE_CS_1                   ((u32)0x00000001 << 14)          /*!<R/W 'h0 1 for forcing PAD_DDR_CS_1_N value to cs_1_value*/
#define DDRPHY_BIT_CS_1_VALUE                   ((u32)0x00000001 << 13)          /*!<R/W 'h1 Value of PAD_DDR_CS_1_N when force_cs_1 = 1*/
#define DDRPHY_BIT_FORCE_CKE                    ((u32)0x00000001 << 11)          /*!<R/W 'h1 1 for forcing PAD_DDR_CKE value to cke_value*/
#define DDRPHY_BIT_CKE_VALUE                    ((u32)0x00000001 << 10)          /*!<R/W 'h0 Value of PAD_DDR_CKE when force_cke = 1*/
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAPRO3_FPGA_DDRPHY
 * @{
 * @brief AMEBAPRO3_FPGA_DDRPHY Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t DDRPHY_DPI_CTL_0;                        /*!<  Register,  Address offset: 0X000 */
	__IO uint32_t DDRPHY_DPI_CTL_1;                        /*!<  Register,  Address offset: 0X004 */
	__IO uint32_t DDRPHY_DPI_CTL_2;                        /*!<  Register,  Address offset: 0X008 */
	__IO uint32_t DDRPHY_DPI_CTL_3;                        /*!<  Register,  Address offset: 0X00C */
	__IO uint32_t DDRPHY_DPI_CTL_4;                        /*!<  Register,  Address offset: 0X010 */
	__IO uint32_t DDRPHY_DPI_CTL_5;                        /*!<  Register,  Address offset: 0X014 */
	__IO uint32_t DDRPHY_DPI_CTL_6;                        /*!<  Register,  Address offset: 0X018 */
} DDRPHY_FPGA_TypeDef;


/**************************************************************************//**
 * @defgroup DDRPHY_CRT_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_CRT_CTL_MCK_CKE_SEL           ((u32)0x00000001 << 30)            /*!<R/W 'b0 1'b0 : mck_cke = mck_ca 1'b1 : mck_cke = mck_cs*/
#define DDRPHY_BIT_CRT_CTL_CLK_TEN_VALUE         ((u32)0x00000001 << 29)            /*!<R/W 'b0 Value for ten mode of clk*/
#define DDRPHY_BIT_CRT_CTL_CLK_TEN_MODE          ((u32)0x00000001 << 28)            /*!<R/W 'b0 1 for ten mode*/
#define DDRPHY_BIT_CRT_CTL_FORCE_RSTN            ((u32)0x00000001 << 27)            /*!<R/W 'b1 1 for forcing PAD_DDR_RSTN value to rstn_value*/
#define DDRPHY_BIT_CRT_CTL_RSTN_VALUE            ((u32)0x00000001 << 26)            /*!<R/W 'b1 Value of PAD_DDR_RSTN when force_rstn = 1*/
#define DDRPHY_MASK_CRT_CTL_FIFO_RST_INI_TX      ((u32)0x00000007 << 23)            /*!<R/W 'h0 Initial value of Tx CTS FIFO read point*/
#define DDRPHY_CRT_CTL_FIFO_RST_INI_TX(x)        ((u32)(((x) & 0x00000007) << 23))
#define DDRPHY_MASK_CRT_CTL_FIFO_RST_INI_RX      ((u32)0x00000007 << 20)            /*!<R/W 'h0 Initial value of Rx CTS FIFO read point*/
#define DDRPHY_CRT_CTL_FIFO_RST_INI_RX(x)        ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_BIT_CRT_CTL_CA_DDR3_MODE          ((u32)0x00000001 << 19)            /*!<R/W 'h1 Connect to command/address pad's DDR3_MODE*/
#define DDRPHY_BIT_CRT_CTL_FORCE_CKE_1           ((u32)0x00000001 << 18)            /*!<R/W 'b1 1 for forcing PAD_DDR_CKE_1 value to cke_1_value*/
#define DDRPHY_BIT_CRT_CTL_CKE_1_VALUE           ((u32)0x00000001 << 17)            /*!<R/W 'b0 Value of PAD_DDR_CKE_1 when force_cke_1 = 1*/
#define DDRPHY_BIT_CRT_CTL_FORCE_CS              ((u32)0x00000001 << 16)            /*!<R/W 'b0 1 for forcing PAD_DDR_CS_N value to cs_value*/
#define DDRPHY_BIT_CRT_CTL_CS_VALUE              ((u32)0x00000001 << 15)            /*!<R/W 'b1 Value of PAD_DDR_CS_N when force_cs = 1*/
#define DDRPHY_BIT_CRT_CTL_FORCE_CS_1            ((u32)0x00000001 << 14)            /*!<R/W 'b0 1 for forcing PAD_DDR_CS_1_N value to cs_1_value*/
#define DDRPHY_BIT_CRT_CTL_CS_1_VALUE            ((u32)0x00000001 << 13)            /*!<R/W 'b1 Value of PAD_DDR_CS_1_N when force_cs_1 = 1*/
#define DDRPHY_BIT_CRT_CTL_DDR_MODE              ((u32)0x00000001 << 12)            /*!<R/W 'h1*/
#define DDRPHY_BIT_CRT_CTL_FORCE_CKE             ((u32)0x00000001 << 11)            /*!<R/W 'b1 1 for forcing PAD_DDR_CKE value to cke_value*/
#define DDRPHY_BIT_CRT_CTL_CKE_VALUE             ((u32)0x00000001 << 10)            /*!<R/W 'b0 Value of PAD_DDR_CKE when force_cke = 1*/
#define DDRPHY_BIT_CRT_CTL_PIN_MUX_LPDDR3_2CELL  ((u32)0x00000001 << 9)             /*!<R/W 'b0*/
#define DDRPHY_BIT_CRT_CTL_PIN_MUX_LPDDR3_1CELL  ((u32)0x00000001 << 8)             /*!<R/W 'b0*/
#define DDRPHY_BIT_CRT_CTL_PIN_MUX_LPDDR2_2CELL  ((u32)0x00000001 << 7)             /*!<R/W 'b0*/
#define DDRPHY_BIT_CRT_CTL_PIN_MUX_LPDDR2_1CELL  ((u32)0x00000001 << 6)             /*!<R/W 'b0 : 5'b0_0010*/
#define DDRPHY_BIT_CRT_CTL_PIN_MUX_DDR3          ((u32)0x00000001 << 5)             /*!<R/W 'b1 DDR3 : 5'b0_0001 DDR4 : 5'b0_0000*/
#define DDRPHY_BIT_CRT_CTL_PIN_MUX_MODE          ((u32)0x00000001 << 4)             /*!<R/W 'b0 1'b1 : pin_mux bypass mode 1'b0 : pin_mux mode*/
#define DDRPHY_BIT_CRT_CTL_DPI_LPDDR_EN          ((u32)0x00000001 << 3)             /*!<R/W 'b0 1'b1 : LPDDR mode enable*/
#define DDRPHY_BIT_CRT_CTL_DPI_DAT_RATE_SEL      ((u32)0x00000001 << 2)             /*!<R/W 'b0 1'b0 : 2to1 mode mck_cmd = mck_mc = 1/2 DCK 1'b1 : 1to1 mode mck_cmd = mck_mc = DCK*/
#define DDRPHY_BIT_CRT_CTL_DPI_DDR2_EN           ((u32)0x00000001 << 1)             /*!<R/W 'b0 ddr2_en=1 & ddr3_mod=1 DDR2(LPDDR2) mode*/
#define DDRPHY_BIT_CRT_CTL_DPI_DDR3_MODE         ((u32)0x00000001 << 0)             /*!<R/W 'b1 ddr2_en=0 & ddr3_mod=1 DDR3(LPDDR3) mode ddr2_en=0 & ddr3_mod=0 DDR4 mode*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_CTL0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PLL_CTL0_AFA_DCG_EN       ((u32)0x00000001 << 29)           /*!<R/W 'h0 Rx Alpha eye calibration clock gating enable*/
#define DDRPHY_BIT_PLL_CTL0_TMUX_DCG_EN      ((u32)0x00000001 << 28)           /*!<R/W 'h0 Bist test mux clock gating enable*/
#define DDRPHY_MASK_PLL_CTL0_DPI_MCK_CLK_EN  ((u32)0x00001FFF << 0)            /*!<R/W 'h0000 DDR_PHY internal clock ICG enable ck_en[12:0] :clock enable ck_en[0]: mck_ck ck_en[1]: mck_cmd ck_en[2]: mck_dqs_0 ck_en[3]: mck_dqs_1 ck_en[4]: mck_dqs_2 ck_en[5]: mck_dqs_3 ck_en[6]: mck_dq_0 ck_en[7]: mck_dq_1 ck_en[8]: mck_dq_2 ck_en[9]: mck_dq_3 ck_en[10]: mck_cs ck_en[11]: mck_cs_1 ck_en[12]: mck_ck_1*/
#define DDRPHY_PLL_CTL0_DPI_MCK_CLK_EN(x)    ((u32)(((x) & 0x00001FFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_CTL1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PLL_CTL1_MCK_MC_EN               ((u32)0x00000001 << 31)            /*!<R/W 'h0 0: gated mck_mc to low*/
#define DDRPHY_BIT_PLL_CTL1_DPI_REG_FIX_DIV1        ((u32)0x00000001 << 30)            /*!<R/W 'h0 PLL sync CLK_OE clk is fix div1(1) or div2 (0 default)*/
#define DDRPHY_BIT_PLL_CTL1_DPI_REG_CRT_PWROFF_DIS  ((u32)0x00000001 << 29)            /*!<R/W 'h1 clk_oe to PLL is from crt table or FSM, default is from crt table*/
#define DDRPHY_BIT_PLL_CTL1_DPI_PICLK_SEL           ((u32)0x00000001 << 27)            /*!<R/W 'h0 0 : ckvco 1 : ckvco/2*/
#define DDRPHY_BIT_PLL_CTL1_DPI_LDO_POW_CK          ((u32)0x00000001 << 26)            /*!<R/W 'b0 PLL 1.8/0.9 LDO for CK tree 1 : LDO power on 0 : LDO power down*/
#define DDRPHY_BIT_PLL_CTL1_DPI_LDO_POW             ((u32)0x00000001 << 25)            /*!<R/W 'b0 PLL core power select 1: Internal LDO 0: External core power*/
#define DDRPHY_MASK_PLL_CTL1_DPI_CK_PWR_SEL         ((u32)0x0000001F << 20)            /*!<R/W 'h00 Ck tree power select: 4'b0000: LDO/AVDD mode, ck tree power base on dpi_ldo_pow_ck to choose LDO or AVDD 4'b0xxx: Partial short DVDD mode. 4'b1xxx: Entirely short DVDD model.*/
#define DDRPHY_PLL_CTL1_DPI_CK_PWR_SEL(x)           ((u32)(((x) & 0x0000001F) << 20))
#define DDRPHY_MASK_PLL_CTL1_DPI_CLK_OE             ((u32)0x00000007 << 16)            /*!<R/W 'h0 clk_oe[2:0]: PLL clock output enable clk_oe[0]:mck_ck clk_oe[1]: mck_cmd , mck_dqs_0, mck_dqs_1, mck_dqs_2, mck_dqs_3, mck_dq_0, mck_dq_1, mck_dq_2, mck_dq_3 clk_oe[2]: mck_ck_1*/
#define DDRPHY_PLL_CTL1_DPI_CLK_OE(x)               ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_BIT_PLL_CTL1_PLL_EXT_SEL             ((u32)0x00000001 << 14)
#define DDRPHY_BIT_PLL_CTL1_V11_EXT_SEL             ((u32)0x00000001 << 14)
#define DDRPHY_BIT_PLL_CTL1_DPI_OESYNC_OP_SEL_FIX   ((u32)0x00000001 << 13)            /*!<R/W 'b0 free run fix clock op_sel*/
#define DDRPHY_MASK_PLL_CTL1_DPI_OESYNC_OP_SEL      ((u32)0x00001FFF << 0)             /*!<R/W 'h0000 control signal for output clock enable sync op_sel[0]:mck_ck op_sel[1]:mck_cmd op_sel[2]:mck_dqs_0 op_sel[3]:mck_dqs_1 op_sel[4]:mck_dqs_2 op_sel[5]:mck_dqs_3 op_sel[6]:mck_dq_0 op_sel[7]:mck_dq_1 op_sel[8]:mck_dq_2 op_sel[9]:mck_dq_3 op_sel[10]:mck_cs op_sel[11]:mck_cs_1 op_sel[12]:mck_ck_1*/
#define DDRPHY_PLL_CTL1_DPI_OESYNC_OP_SEL(x)        ((u32)(((x) & 0x00001FFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_CTL2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PLL_CTL2_DPI_REG_CTS_FIFO_SYNC_CNT  ((u32)0x00000007 << 29)            /*!<R/W 'h2 sync cts fifo tx/rx pointer , wait cycles when pweroff_rst_n sync from tx to rx*/
#define DDRPHY_PLL_CTL2_DPI_REG_CTS_FIFO_SYNC_CNT(x)    ((u32)(((x) & 0x00000007) << 29))
#define DDRPHY_MASK_PLL_CTL2_DPI_PLL_DUM                ((u32)0x000000FF << 0)             /*!<R/W 'h0 dummy Register*/
#define DDRPHY_PLL_CTL2_DPI_PLL_DUM(x)                  ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_PI0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PLL_PI0_DPI_POST_PI_SEL3  ((u32)0x0000001F << 24)            /*!<R/W 'h00 PLL phase select for mck_dqs_1*/
#define DDRPHY_PLL_PI0_DPI_POST_PI_SEL3(x)    ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_PLL_PI0_DPI_POST_PI_SEL2  ((u32)0x0000001F << 16)            /*!<R/W 'h00 PLL phase select for mck_dqs_0*/
#define DDRPHY_PLL_PI0_DPI_POST_PI_SEL2(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_PLL_PI0_DPI_POST_PI_SEL1  ((u32)0x0000001F << 8)             /*!<R/W 'h00 PLL phase select for mck_cmd*/
#define DDRPHY_PLL_PI0_DPI_POST_PI_SEL1(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_PLL_PI0_DPI_POST_PI_SEL0  ((u32)0x0000001F << 0)             /*!<R/W 'h00 PLL phase select for mck_ck*/
#define DDRPHY_PLL_PI0_DPI_POST_PI_SEL0(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_PI1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PLL_PI1_DPI_POST_PI_SEL   ((u32)0x0000001F << 24)            /*!<R/W 'h00 PLL internal clock phase select*/
#define DDRPHY_PLL_PI1_DPI_POST_PI_SEL(x)     ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_PLL_PI1_DPI_POST_PI_SEL6  ((u32)0x0000001F << 16)            /*!<R/W 'h00 PLL phase select for mck_dq_0*/
#define DDRPHY_PLL_PI1_DPI_POST_PI_SEL6(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_PLL_PI1_DPI_POST_PI_SEL5  ((u32)0x0000001F << 8)             /*!<R/W 'h00 PLL phase select for mck_dqs_3*/
#define DDRPHY_PLL_PI1_DPI_POST_PI_SEL5(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_PLL_PI1_DPI_POST_PI_SEL4  ((u32)0x0000001F << 0)             /*!<R/W 'h00 PLL phase select for mck_dqs_2*/
#define DDRPHY_PLL_PI1_DPI_POST_PI_SEL4(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_CTL3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PLL_CTL3_DPI_V11_LDO_VSEL   ((u32)0x00000007 << 29)            /*!<R/W 'h0 Select PLL AVDD LDO output 3'b000: LDO tracking AVDD power 3'b001: 0.85V 3'b010: 0.9V 3'b011: 1.0V 3'b100: 1.05V 3'b101: 1.1V 3'b110: 1.15V 3'b111: 1.2V*/
#define DDRPHY_PLL_CTL3_DPI_V11_LDO_VSEL(x)     ((u32)(((x) & 0x00000007) << 29))
#define DDRPHY_BIT_PLL_CTL3_DPI_CKREF_SEL       ((u32)0x00000001 << 28)            /*!<R/W 'b0 Ref Clock select 1'b0: use CKREF_PAD 1'b1: use CKREF_TOP*/
#define DDRPHY_BIT_PLL_CTL3_DPI_POST_PI_RS      ((u32)0x00000001 << 27)            /*!<R/W 'b0 Post PI loading cap control: 0: 112.5fF 1: 15fF*/
#define DDRPHY_MASK_PLL_CTL3_DPI_POST_PI_RL     ((u32)0x00000003 << 25)            /*!<R/W 'h2 Post PI RL control, changed with PLL freq: 00:0.2G~0.5G 01:0.5G~0.8G 10:0.8G~1.25G*/
#define DDRPHY_PLL_CTL3_DPI_POST_PI_RL(x)       ((u32)(((x) & 0x00000003) << 25))
#define DDRPHY_MASK_PLL_CTL3_DPI_POST_DIVN      ((u32)0x00000003 << 23)            /*!<R/W 'h1 fvco/4*/
#define DDRPHY_PLL_CTL3_DPI_POST_DIVN(x)        ((u32)(((x) & 0x00000003) << 23))
#define DDRPHY_BIT_PLL_CTL3_DPI_PLL_SEL_CPMODE  ((u32)0x00000001 << 22)            /*!<R/W 'b1 PLL charge pump mode selector, always set 1*/
#define DDRPHY_MASK_PLL_CTL3_DPI_CK_LDO_VSEL    ((u32)0x00000007 << 19)            /*!<R/W 'h0 Select PLL CK LDO output 000:tracking core power 001:0.85V; 010:0.9V; 011:1V; 100:1.05V; 101:1.1V 110:1.15V 111:1.2V*/
#define DDRPHY_PLL_CTL3_DPI_CK_LDO_VSEL(x)      ((u32)(((x) & 0x00000007) << 19))
#define DDRPHY_BIT_PLL_CTL3_DPI_PLL_DBUG_EN     ((u32)0x00000001 << 18)            /*!<R/W 'b0 Enable PLL debug mode*/
#define DDRPHY_MASK_PLL_CTL3_DPI_PDIV           ((u32)0x00000003 << 16)            /*!<R/W 'h0 PLL pre-divider divisor register 00: /1; 01: /2; 10: /4; 11: /8*/
#define DDRPHY_PLL_CTL3_DPI_PDIV(x)             ((u32)(((x) & 0x00000003) << 16))
#define DDRPHY_MASK_PLL_CTL3_DPI_LPF_SR         ((u32)0x00000007 << 13)            /*!<R/W 'h5 PLL LPF resistor selector Register for LPF RS, combined used with REG_ICP and REG_LPF_CP to tune PLL bandwidth*/
#define DDRPHY_PLL_CTL3_DPI_LPF_SR(x)           ((u32)(((x) & 0x00000007) << 13))
#define DDRPHY_BIT_PLL_CTL3_DPI_LPF_CP          ((u32)0x00000001 << 12)            /*!<R/W 'b1 Register for LPF CP, combined used with REG_ICP and REG_LPF_SR to tune PLL bandwidth*/
#define DDRPHY_MASK_PLL_CTL3_DPI_LOOP_PI_ISEL   ((u32)0x0000000F << 8)             /*!<R/W 'h7 Select loop pi current, changed with PLL freq: 0010:0.2G~0.4G; 0011:0.4G~0.6G; 0100:0.6G~0.8G; 0101:0.8G~1G; 0110:1G~1.25G 0111:1.25G~1.6G*/
#define DDRPHY_PLL_CTL3_DPI_LOOP_PI_ISEL(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_PLL_CTL3_DPI_ICP            ((u32)0x0000000F << 4)             /*!<R/W 'h8 CP bias current control, combined used with REG_LPF_CP and REG_LPF_SR to tune PLL bandwidth*/
#define DDRPHY_PLL_CTL3_DPI_ICP(x)              ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_BIT_PLL_CTL3_DPI_CCO_KVCO        ((u32)0x00000001 << 2)             /*!<R/W 'b1 PLL kvco control, combined used with REG_CCO_BAND to tune PLL freq*/
#define DDRPHY_MASK_PLL_CTL3_DPI_CCO_BAND       ((u32)0x00000003 << 0)             /*!<R/W 'h3 PLL band select, combined used with REG_CCO_KVCO to tune PLL freq CLOCK 200M ~ 350M REG_CCO_BAND = 0,REG_CCO_KVCO = 0 CLOCK 350M ~ 450M REG_CCO_BAND = 1,REG_CCO_KVCO = 0 CLOCK 450M ~ 750M REG_CCO_BAND = 1,REG_CCO_KVCO = 1 CLOCK 750M ~ 1600M REG_CCO_BAND = 3,REG_CCO_KVCO = 1*/
#define DDRPHY_PLL_CTL3_DPI_CCO_BAND(x)         ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_SSC0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_SSC0_DPI_OC_DONE_DELAY    ((u32)0x0000003F << 8)            /*!<R/W 'h3F Delay to setting OC_DONE=1'b1 after over-clock or under-clock is done by (REG_OC_DONE_delay*Tref)*/
#define DDRPHY_SSC0_DPI_OC_DONE_DELAY(x)      ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_SSC0_DPI_HS_OC_STOP_DIFF  ((u32)0x00000003 << 6)            /*!<R/W 'h1 HS stop control 2'b00: HS OC stop at (Target_freq-2*fref) 2'b01: HS OC stop at (Target_freq-6*fref) 2'b10: HS OC stop at (Target_freq-10*fref) 2'b11: HS OC stop at (Target_freq-14*fref)*/
#define DDRPHY_SSC0_DPI_HS_OC_STOP_DIFF(x)    ((u32)(((x) & 0x00000003) << 6))
#define DDRPHY_MASK_SSC0_DPI_SEL_OC_MODE      ((u32)0x00000003 << 4)            /*!<R/W 'h0 Over-clock / Under-clock mode selection 2'b00: Low speed mode 2'b01: High speed mode 2'b10: Adaptive high speed mode*/
#define DDRPHY_SSC0_DPI_SEL_OC_MODE(x)        ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_BIT_SSC0_DPI_SSC_FLAG_INIT     ((u32)0x00000001 << 3)            /*!<R/W 'b0 SSC initial flag*/
#define DDRPHY_BIT_SSC0_DPI_ORDER             ((u32)0x00000001 << 2)            /*!<R/W 'b0 Select 1st-order or 2nd-order SDM*/
#define DDRPHY_BIT_SSC0_DPI_EN_SSC            ((u32)0x00000001 << 1)            /*!<R/W 'b0 Enable SSC function "1" : Enable "0" : Disable*/
#define DDRPHY_BIT_SSC0_DPI_BYPASS_PI         ((u32)0x00000001 << 0)            /*!<R/W 'b0 When no fractional divider or SSC, bypass Phase Interpolator for CMU loop 0: including PI inside the loop 1: bypass PI inside the loop*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_SSC1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_SSC1_DPI_GRAN_AUTO_RST  ((u32)0x00000001 << 23)            /*!<R/W 'b1 When FMOD transition, reset SDM*/
#define DDRPHY_MASK_SSC1_DPI_DOT_GRAN      ((u32)0x00000007 << 20)            /*!<R/W 'h4 Decimal point position of GRAN 3'b000: 4.15 3'b001: 5.14 3'b010: 6.13 3'b011: 7.12 3'b100: 8.11*/
#define DDRPHY_SSC1_DPI_DOT_GRAN(x)        ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_SSC1_DPI_GRAN_SET      ((u32)0x0007FFFF << 0)             /*!<R/W 'h021FB SSC step SSC step=(Fssc-F) / (0.5*Tssc)*/
#define DDRPHY_SSC1_DPI_GRAN_SET(x)        ((u32)(((x) & 0x0007FFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_SSC2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_SSC2_DPI_F_CODE_T  ((u32)0x000007FF << 16)            /*!<R/W 'h50A Fractional code of loop divider (target Fcode of overclocking)*/
#define DDRPHY_SSC2_DPI_F_CODE_T(x)    ((u32)(((x) & 0x000007FF) << 16))
#define DDRPHY_MASK_SSC2_DPI_F_CODE    ((u32)0x000007FF << 0)             /*!<R/W 'h5ED Fractional code of loop divider*/
#define DDRPHY_SSC2_DPI_F_CODE(x)      ((u32)(((x) & 0x000007FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_SSC3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_SSC3_DPI_OC_EN         ((u32)0x00000001 << 26)            /*!<R/W 'b0 Enable over-clock or under-clock*/
#define DDRPHY_MASK_SSC3_DPI_OC_STEP_SET  ((u32)0x000003FF << 16)            /*!<R/W 'h028 Over-clock / Under-clock slope control of low speed mode*/
#define DDRPHY_SSC3_DPI_OC_STEP_SET(x)    ((u32)(((x) & 0x000003FF) << 16))
#define DDRPHY_MASK_SSC3_DPI_N_CODE_T     ((u32)0x000000FF << 8)             /*!<R/W 'h1A Integer code of loop divider,real divider code is (target Ncode of overclocking) CKREF=27M, REG_PDIV=2'b00, REG_BYPASS_PI=1'b0, VCO frequency=27*( REG_N_CODE [7:0] +3+ REG_F_CODE<10:0>/2048)=1.6GHz -> REG_N_CODE [7:0] =56, REG_F_CODE<10:0>=531*/
#define DDRPHY_SSC3_DPI_N_CODE_T(x)       ((u32)(((x) & 0x000000FF) << 8))
#define DDRPHY_MASK_SSC3_DPI_N_CODE       ((u32)0x000000FF << 0)             /*!<R/W 'h19 Integer code of loop divider,real divider code is CKREF=27M, REG_PDIV=2'b00, REG_BYPASS_PI=1'b0, VCO frequency=27*( REG_N_CODE [7:0] +3+ REG_F_CODE<10:0>/2048)=1.6GHz -> REG_N_CODE [7:0] =56, REG_F_CODE<10:0>=531*/
#define DDRPHY_SSC3_DPI_N_CODE(x)         ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_WDOG
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_WDOG_DPI_TIME2_RST_WIDTH  ((u32)0x00000003 << 12)            /*!<R/W 'h1 Reset timing control (REG_TIME_RST_WIDTH x CKREF) 2'd0: 2 2'd1: 4 2'd2: 8 2'd3: 20*/
#define DDRPHY_WDOG_DPI_TIME2_RST_WIDTH(x)    ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_WDOG_DPI_TIME_RDY_CKOUT   ((u32)0x00000003 << 8)             /*!<R/W 'h2 Clock ready timing control ( clock ready time : 2 times the value of REG_TIME_WD x CKREF + REG_TIME_RDY_CKOUT x CKTXPLL) 2'd0: 3 2'd1: 100 2'd2: 200 2'd3: 600*/
#define DDRPHY_WDOG_DPI_TIME_RDY_CKOUT(x)     ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_WDOG_DPI_TIME0_CK         ((u32)0x00000007 << 4)             /*!<R/W 'h3 Watchdog timing control (clock output time: 2 times the value of REG_TIME_WD x CKREF) 3'd0: 2 3'd1: 200 3'd2: 600 3'd3: 1200 3'd4: 3000 3'd5: 6400 3'd6: 12800 3'd7: 16000*/
#define DDRPHY_WDOG_DPI_TIME0_CK(x)           ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_WDOG_DPI_F390K            ((u32)0x00000003 << 1)             /*!<R/W 'h1 F390K output delay, not used*/
#define DDRPHY_WDOG_DPI_F390K(x)              ((u32)(((x) & 0x00000003) << 1))
#define DDRPHY_BIT_WDOG_DPI_WD_ENABLE         ((u32)0x00000001 << 0)             /*!<R/W 'b0 Enable watch dog function 0: disable Watch Dog function 1: enable Watch Dog function*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DCC_CMD
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DCC_CMD_DPI_DCC_CMD_DSP_DBG_SEL    ((u32)0x00000001 << 31)            /*!<R/W 'b0 mck_cmd duty control debug output enable*/
#define DDRPHY_BIT_DCC_CMD_DPI_DCC_CMD_DUTY_SEL       ((u32)0x00000001 << 29)            /*!<R/W 'b0 mck_cmd duty control reg_dpi_dcc_cmd_duty_sel=0: output from close loop; reg_dpi_dcc_cmd_duty_sel=1: output from reg_dpi_dcc_cmd_duty[6:0]*/
#define DDRPHY_BIT_DCC_CMD_DPI_DCC_CMD_DSP_STOP       ((u32)0x00000001 << 28)            /*!<R/W 'b1 mck_cmd duty control dsp function enable*/
#define DDRPHY_MASK_DCC_CMD_DPI_DCC_CMD_DIV           ((u32)0x00000007 << 24)            /*!<R/W 'h5 mck_cmd duty control dsp clock frequency setting: 000~111: /2^4~/2^11*/
#define DDRPHY_DCC_CMD_DPI_DCC_CMD_DIV(x)             ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DCC_CMD_DPI_DCC_CMD_INT_GAIN      ((u32)0x0000001F << 16)            /*!<R/W 'h05 mck_cmd duty control close loop gain setting: 00000~11111: /2^6~/2*/
#define DDRPHY_DCC_CMD_DPI_DCC_CMD_INT_GAIN(x)        ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DCC_CMD_DPI_DCC_CMD_DUTY          ((u32)0x0000007F << 8)             /*!<R/W 'h38 mck_cmd duty control when reg_dpi_dcc_cmd_duty_sel=1'b,output value from this register*/
#define DDRPHY_DCC_CMD_DPI_DCC_CMD_DUTY(x)            ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DCC_CMD_DPI_DCC_CMD_DUTY_PRESETB  ((u32)0x00000007 << 4)             /*!<R/W 'h3 mck_cmd duty control DCD negitive duty preset setting*/
#define DDRPHY_DCC_CMD_DPI_DCC_CMD_DUTY_PRESETB(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DCC_CMD_DPI_DCC_CMD_DUTY_PRESETA  ((u32)0x00000007 << 0)             /*!<R/W 'h3 mck_cmd duty control DCD positive duty preset setting*/
#define DDRPHY_DCC_CMD_DPI_DCC_CMD_DUTY_PRESETA(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DCC_DQS
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DCC_DQS_DPI_DCC_DQS_DUTY_SEL       ((u32)0x00000001 << 29)            /*!<R/W 'b0 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_BIT_DCC_DQS_DPI_DCC_DQS_DSP_STOP       ((u32)0x00000001 << 28)            /*!<R/W 'b1 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_MASK_DCC_DQS_DPI_DCC_DQS_DIV           ((u32)0x00000007 << 24)            /*!<R/W 'h5 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQS_DPI_DCC_DQS_DIV(x)             ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DCC_DQS_DPI_DCC_DQS_INT_GAIN      ((u32)0x0000001F << 16)            /*!<R/W 'h05 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQS_DPI_DCC_DQS_INT_GAIN(x)        ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DCC_DQS_DPI_DCC_DQS_DUTY          ((u32)0x0000007F << 8)             /*!<R/W 'h38 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQS_DPI_DCC_DQS_DUTY(x)            ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DCC_DQS_DPI_DCC_DQS_DUTY_PRESETB  ((u32)0x00000007 << 4)             /*!<R/W 'h3 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQS_DPI_DCC_DQS_DUTY_PRESETB(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DCC_DQS_DPI_DCC_DQS_DUTY_PRESETA  ((u32)0x00000007 << 0)             /*!<R/W 'h3 mck_dqs duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQS_DPI_DCC_DQS_DUTY_PRESETA(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DCC_DQ
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DCC_DQ_DPI_DCC_DQ_DUTY_SEL       ((u32)0x00000001 << 29)            /*!<R/W 'b0 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_BIT_DCC_DQ_DPI_DCC_DQ_DSP_STOP       ((u32)0x00000001 << 28)            /*!<R/W 'b1 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_MASK_DCC_DQ_DPI_DCC_DQ_DIV           ((u32)0x00000007 << 24)            /*!<R/W 'h5 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQ_DPI_DCC_DQ_DIV(x)             ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DCC_DQ_DPI_DCC_DQ_INT_GAIN      ((u32)0x0000001F << 16)            /*!<R/W 'h05 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQ_DPI_DCC_DQ_INT_GAIN(x)        ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DCC_DQ_DPI_DCC_DQ_DUTY          ((u32)0x0000007F << 8)             /*!<R/W 'h38 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQ_DPI_DCC_DQ_DUTY(x)            ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DCC_DQ_DPI_DCC_DQ_DUTY_PRESETB  ((u32)0x00000007 << 4)             /*!<R/W 'h3 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQ_DPI_DCC_DQ_DUTY_PRESETB(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DCC_DQ_DPI_DCC_DQ_DUTY_PRESETA  ((u32)0x00000007 << 0)             /*!<R/W 'h3 mck_dq duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_DQ_DPI_DCC_DQ_DUTY_PRESETA(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DCC_CK
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DCC_CK_DPI_DCC_CK_DUTY_SEL       ((u32)0x00000001 << 29)            /*!<R/W 'b0 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_BIT_DCC_CK_DPI_DCC_CK_DSP_STOP       ((u32)0x00000001 << 28)            /*!<R/W 'b1 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_MASK_DCC_CK_DPI_DCC_CK_DIV           ((u32)0x00000007 << 24)            /*!<R/W 'h5 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_CK_DPI_DCC_CK_DIV(x)             ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DCC_CK_DPI_DCC_CK_INT_GAIN      ((u32)0x0000001F << 16)            /*!<R/W 'h05 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_CK_DPI_DCC_CK_INT_GAIN(x)        ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DCC_CK_DPI_DCC_CK_DUTY          ((u32)0x0000007F << 8)             /*!<R/W 'h38 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_CK_DPI_DCC_CK_DUTY(x)            ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DCC_CK_DPI_DCC_CK_DUTY_PRESETB  ((u32)0x00000007 << 4)             /*!<R/W 'h3 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_CK_DPI_DCC_CK_DUTY_PRESETB(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DCC_CK_DPI_DCC_CK_DUTY_PRESETA  ((u32)0x00000007 << 0)             /*!<R/W 'h3 mck_ck duty control setting as mck_cmd setting*/
#define DDRPHY_DCC_CK_DPI_DCC_CK_DUTY_PRESETA(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CRT_CTL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CRT_CTL_2_MCK_TEST_CLK_SEL  ((u32)0x0000000F << 28)            /*!<R/W 'h0 to PAD MP test debug clock select*/
#define DDRPHY_CRT_CTL_2_MCK_TEST_CLK_SEL(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_BIT_CRT_CTL_2_ZQ_CAL_CLK_DIS     ((u32)0x00000001 << 19)            /*!<R/W 'b0 set 1 to disable zq calibration clock*/
#define DDRPHY_BIT_CRT_CTL_2_ALERT_CHK_EN       ((u32)0x00000001 << 3)             /*!<R/W 'b0 set 1 to enable alert pad security check*/
#define DDRPHY_BIT_CRT_CTL_2_TOPREG_CLK_DIS     ((u32)0x00000001 << 1)             /*!<R/W 'b0 set 1 to disable regif clock of top*/
#define DDRPHY_BIT_CRT_CTL_2_RWREG_CLK_DIS      ((u32)0x00000001 << 0)             /*!<R/W 'b0 set 1 to disable regif clock of rw_blk*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CRT_DMY
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CRT_DMY_DMY_0  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 Dummy Registers*/
#define DDRPHY_CRT_DMY_DMY_0(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CRT_DMY1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CRT_DMY1_DMY_1  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 Dummy Registers*/
#define DDRPHY_CRT_DMY1_DMY_1(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_AFIFO_STR_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_AFIFO_STR_0_DQS_RD_STR_NUM_3  ((u32)0x00000007 << 28)            /*!<R/W 'h1 slice3 DQS TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_3(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_AFIFO_STR_0_DQ_RD_STR_NUM_3   ((u32)0x00000007 << 24)            /*!<R/W 'h1 slice3 DQ TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_3(x)     ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_AFIFO_STR_0_DQS_RD_STR_NUM_2  ((u32)0x00000007 << 20)            /*!<R/W 'h1 slice2 DQS TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_2(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_AFIFO_STR_0_DQ_RD_STR_NUM_2   ((u32)0x00000007 << 16)            /*!<R/W 'h1 slice2 DQ TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_2(x)     ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_AFIFO_STR_0_DQS_RD_STR_NUM_1  ((u32)0x00000007 << 12)            /*!<R/W 'h1 slice1 DQS TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_1(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_AFIFO_STR_0_DQ_RD_STR_NUM_1   ((u32)0x00000007 << 8)             /*!<R/W 'h1 slice1 DQ TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_1(x)     ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_AFIFO_STR_0_DQS_RD_STR_NUM_0  ((u32)0x00000007 << 4)             /*!<R/W 'h1 slice0 DQS TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQS_RD_STR_NUM_0(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_AFIFO_STR_0_DQ_RD_STR_NUM_0   ((u32)0x00000007 << 0)             /*!<R/W 'h1 slice0 DQ TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_0_DQ_RD_STR_NUM_0(x)     ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_AFIFO_STR_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_AFIFO_STR_1_BYPASS_EN_RX           ((u32)0x00000001 << 21)           /*!<R/W 'h0 Bypass RX FIFO*/
#define DDRPHY_BIT_AFIFO_STR_1_BYPASS_EN_TX           ((u32)0x00000001 << 20)           /*!<R/W 'h0 Bypass TX FIFO*/
#define DDRPHY_BIT_AFIFO_STR_1_FORCE_RST_N_RX         ((u32)0x00000001 << 19)           /*!<R/W 'h1 Force reset RX FIFO*/
#define DDRPHY_BIT_AFIFO_STR_1_FORCE_RST_N_TX         ((u32)0x00000001 << 18)           /*!<R/W 'h1 Force reset TX FIFO*/
#define DDRPHY_BIT_AFIFO_STR_1_REF_RST_EN_RX          ((u32)0x00000001 << 17)           /*!<R/W 'h0 Enable reset RX FIFO during refresh*/
#define DDRPHY_BIT_AFIFO_STR_1_REF_RST_EN_TX          ((u32)0x00000001 << 16)           /*!<R/W 'h0 Enable reset TX FIFO during refresh*/
#define DDRPHY_MASK_AFIFO_STR_1_CMD_RD_STR_NUM_SEL_H  ((u32)0x000000FF << 8)            /*!<R/W 'h00 [15] : PAD cke_1 [14] : PAD alert_1 [13] : PAD par_1 [12] : PAD ddr4_cke [11] : PAD act_n [10] : PAD bg0 [09] : PAD par [08] : PAD ten*/
#define DDRPHY_AFIFO_STR_1_CMD_RD_STR_NUM_SEL_H(x)    ((u32)(((x) & 0x000000FF) << 8))
#define DDRPHY_MASK_AFIFO_STR_1_CMD_EX_RD_STR_NUM     ((u32)0x00000007 << 4)            /*!<R/W 'h2 CMD TX FIFO threshold setting (Another, Select by cmd_rd_str_num_sel)*/
#define DDRPHY_AFIFO_STR_1_CMD_EX_RD_STR_NUM(x)       ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_AFIFO_STR_1_CMD_RD_STR_NUM        ((u32)0x00000007 << 0)            /*!<R/W 'h1 CMD TX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_1_CMD_RD_STR_NUM(x)          ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_AFIFO_STR_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_AFIFO_STR_2_RX_RD_STR_NUM_3  ((u32)0x00000007 << 12)            /*!<R/W 'h1 slice3 RX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_3(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_AFIFO_STR_2_RX_RD_STR_NUM_2  ((u32)0x00000007 << 8)             /*!<R/W 'h1 slice2 RX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_2(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_AFIFO_STR_2_RX_RD_STR_NUM_1  ((u32)0x00000007 << 4)             /*!<R/W 'h1 slice1 RX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_1(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_AFIFO_STR_2_RX_RD_STR_NUM_0  ((u32)0x00000007 << 0)             /*!<R/W 'h1 slice0 RX FIFO threshold setting*/
#define DDRPHY_AFIFO_STR_2_RX_RD_STR_NUM_0(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CRT_RST_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_CRT_RST_CTL_PTR_RST_N          ((u32)0x00000001 << 3)  /*!<R/W 'b1 reset 3-point and dqsen calibration results, low active*/
#define DDRPHY_BIT_CRT_RST_CTL_RST_N              ((u32)0x00000001 << 2)  /*!<R/W 'b0 reset all digital circuits, except 3-point calibration, dqsen calibration, and PLL SSC block, low active*/
#define DDRPHY_BIT_CRT_RST_CTL_PLL_SSC_DIG_RST_N  ((u32)0x00000001 << 1)  /*!<R/W 'b0 reset PLL SSC circuit, low active*/
#define DDRPHY_BIT_CRT_RST_CTL_PLL_LDO_RST_N      ((u32)0x00000001 << 0)  /*!<R/W 'b0 DDRPLL LDO reset, low active*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_AFIFO_STR_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_AFIFO_STR_SEL_CMD_RD_STR_NUM_SEL  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 CMD TX FIFO threshold value select 1'b0 : cmd_rd_str_num 1'b1 : cmd_ex_rd_str_num [31] : PAD alert [30] : PAD ddr4_cs_n [29] : PAD cs_n_1 [28] : PAD cs_n [27] : PAD odt [26] : PAD cke [25] : PAD cas_n [24] : PAD ras_n [23] : PAD we_n [22] : PAD reset_n, [21:19] : PAD Bank[2:0] [18:17] : PAD Adr_ext[5:4] [16: 0] : PAD Address[16:0]*/
#define DDRPHY_AFIFO_STR_SEL_CMD_RD_STR_NUM_SEL(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_PI2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PLL_PI2_DPI_POST_PI_SEL10  ((u32)0x0000001F << 24)            /*!<R/W 'h00 PLL phase select for mck_cs*/
#define DDRPHY_PLL_PI2_DPI_POST_PI_SEL10(x)    ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_PLL_PI2_DPI_POST_PI_SEL9   ((u32)0x0000001F << 16)            /*!<R/W 'h00 PLL phase select for mck_dq_3*/
#define DDRPHY_PLL_PI2_DPI_POST_PI_SEL9(x)     ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_PLL_PI2_DPI_POST_PI_SEL8   ((u32)0x0000001F << 8)             /*!<R/W 'h00 PLL phase select for mck_dq_2*/
#define DDRPHY_PLL_PI2_DPI_POST_PI_SEL8(x)     ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_PLL_PI2_DPI_POST_PI_SEL7   ((u32)0x0000001F << 0)             /*!<R/W 'h00 PLL phase select for mck_dq_1*/
#define DDRPHY_PLL_PI2_DPI_POST_PI_SEL7(x)     ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_CTL4
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PLL_CTL4_CLK_PI_CTRL       ((u32)0x00000001 << 31)            /*!<R/W 'h0 0: PI of CLK1X/CLK1XB is controlled by dpi_post_pi_sel* 1: PI of CLK1X/CLK1XB is controlled by dpi_post_pi_sel* and fw_set_wr_dly*/
#define DDRPHY_MASK_PLL_CTL4_DPI_DLY_SEL      ((u32)0x0000001F << 24)            /*!<R/W 'h10 Delay chain selection for CLK1X_0 and CLK1XB_0*/
#define DDRPHY_PLL_CTL4_DPI_DLY_SEL(x)        ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_PLL_CTL4_DPI_DLY_SEL_CK1  ((u32)0x0000001F << 16)            /*!<R/W 'h10 Delay chain selection for CLK1X_12 and CLK1XB_12*/
#define DDRPHY_PLL_CTL4_DPI_DLY_SEL_CK1(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_PLL_CTL4_DPI_EN_POST_PI   ((u32)0x00003FFF << 0)             /*!<R/W 'h3FFF Enable PLL phase selector post_pi[0]: mck_ck post_pi[1]: mck_cmd post_pi[2]: mck_dqs_0 post_pi[3]: mck_dqs_1 post_pi[4]: mck_dqs_2 post_pi[5]: mck_dqs_3 post_pi[6]: mck_dq_0 post_pi[7]: mck_dq_1 post_pi[8]: mck_dq_2 post_pi[9]: mck_dq_3 post_pi[10]: mck_cs post_pi[11]: mck_cs_1 post_pi[12]: mck_ck_1 post_pi[13]: PLL internal clock*/
#define DDRPHY_PLL_CTL4_DPI_EN_POST_PI(x)     ((u32)(((x) & 0x00003FFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_PI3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PLL_PI3_DPI_POST_PI_SEL12  ((u32)0x0000001F << 8)            /*!<R/W 'h00 PLL phase select for mck_ck_1*/
#define DDRPHY_PLL_PI3_DPI_POST_PI_SEL12(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_PLL_PI3_DPI_POST_PI_SEL11  ((u32)0x0000001F << 0)            /*!<R/W 'h00 PLL phase select for mck_cs_1*/
#define DDRPHY_PLL_PI3_DPI_POST_PI_SEL11(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DCC_CS
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DCC_CS_DPI_DCC_CS_DSP_DBG_SEL    ((u32)0x00000001 << 31)            /*!<R/W 'b0 mck_cs duty control debug output enable*/
#define DDRPHY_BIT_DCC_CS_DPI_DCC_CS_DUTY_SEL       ((u32)0x00000001 << 29)            /*!<R/W 'b0 mck_cs duty control reg_dpi_dcc_cs_duty_sel=0: output from close loop; reg_dpi_dcc_cs_duty_sel=1: output from reg_dpi_dcc_cs_duty[6:0]*/
#define DDRPHY_BIT_DCC_CS_DPI_DCC_CS_DSP_STOP       ((u32)0x00000001 << 28)            /*!<R/W 'b1 mck_cs duty control dsp function enable*/
#define DDRPHY_MASK_DCC_CS_DPI_DCC_CS_DIV           ((u32)0x00000007 << 24)            /*!<R/W 'h5 mck_cs duty control dsp clock frequency setting: 000~111: /2^4~/2^11*/
#define DDRPHY_DCC_CS_DPI_DCC_CS_DIV(x)             ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DCC_CS_DPI_DCC_CS_INT_GAIN      ((u32)0x0000001F << 16)            /*!<R/W 'h05 mck_cs duty control close loop gain setting: 00000~11111: /2^6~/2*/
#define DDRPHY_DCC_CS_DPI_DCC_CS_INT_GAIN(x)        ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DCC_CS_DPI_DCC_CS_DUTY          ((u32)0x0000007F << 8)             /*!<R/W 'h38 mck_cs duty control when reg_dpi_dcc_cs_duty_sel=1'b,output value from this register*/
#define DDRPHY_DCC_CS_DPI_DCC_CS_DUTY(x)            ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DCC_CS_DPI_DCC_CS_DUTY_PRESETB  ((u32)0x00000007 << 4)             /*!<R/W 'h3 mck_cs duty control DCD negitive duty preset setting*/
#define DDRPHY_DCC_CS_DPI_DCC_CS_DUTY_PRESETB(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DCC_CS_DPI_DCC_CS_DUTY_PRESETA  ((u32)0x00000007 << 0)             /*!<R/W 'h3 mck_cs duty control DCD positive duty preset setting*/
#define DDRPHY_DCC_CS_DPI_DCC_CS_DUTY_PRESETA(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DCC_CS1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DCC_CS1_DPI_DCC_CS_1_DSP_DBG_SEL    ((u32)0x00000001 << 31)            /*!<R/W 'b0 mck_cs_1 duty control debug output enable*/
#define DDRPHY_BIT_DCC_CS1_DPI_DCC_CS_1_DUTY_SEL       ((u32)0x00000001 << 29)            /*!<R/W 'b0 mck_cs_1 duty control reg_dpi_dcc_cs_duty_sel=0: output from close loop; reg_dpi_dcc_cs_duty_sel=1: output from reg_dpi_dcc_cs_duty[6:0]*/
#define DDRPHY_BIT_DCC_CS1_DPI_DCC_CS_1_DSP_STOP       ((u32)0x00000001 << 28)            /*!<R/W 'b1 mck_cs_1 duty control dsp function enable*/
#define DDRPHY_MASK_DCC_CS1_DPI_DCC_CS_1_DIV           ((u32)0x00000007 << 24)            /*!<R/W 'h5 mck_cs_1 duty control dsp clock frequency setting: 000~111: /2^4~/2^11*/
#define DDRPHY_DCC_CS1_DPI_DCC_CS_1_DIV(x)             ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DCC_CS1_DPI_DCC_CS_1_INT_GAIN      ((u32)0x0000001F << 16)            /*!<R/W 'h05 mck_cs_1 duty control close loop gain setting: 00000~11111: /2^6~/2*/
#define DDRPHY_DCC_CS1_DPI_DCC_CS_1_INT_GAIN(x)        ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DCC_CS1_DPI_DCC_CS_1_DUTY          ((u32)0x0000007F << 8)             /*!<R/W 'h38 mck_cs_1 duty control when reg_dpi_dcc_cs_duty_sel=1'b,output value from this register*/
#define DDRPHY_DCC_CS1_DPI_DCC_CS_1_DUTY(x)            ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DCC_CS1_DPI_DCC_CS_1_DUTY_PRESETB  ((u32)0x00000007 << 4)             /*!<R/W 'h3 mck_cs_1 duty control DCD negitive duty preset setting*/
#define DDRPHY_DCC_CS1_DPI_DCC_CS_1_DUTY_PRESETB(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DCC_CS1_DPI_DCC_CS_1_DUTY_PRESETA  ((u32)0x00000007 << 0)             /*!<R/W 'h3 mck_cs_1 duty control DCD positive duty preset setting*/
#define DDRPHY_DCC_CS1_DPI_DCC_CS_1_DUTY_PRESETA(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PLL_CTL5
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PLL_CTL5_DPI_POST_PI_BIAS            ((u32)0x00000001 << 31)            /*!<R/W 'h Post PI bias current control*/
#define DDRPHY_BIT_PLL_CTL5_DPI_POSTDIV_EN              ((u32)0x00000001 << 27)            /*!<R/W 'h1 DDDRPLL_PH_CTRL input clock control*/
#define DDRPHY_MASK_PLL_CTL5_DPI_PWROFF_PWR_WAIT_CYCLE  ((u32)0x00000007 << 12)            /*!<R/W 'h clk on wait cycle when dfi_pwr is 0 0: wait 0 cycle 1: wait 8 mck_mc cycles 2: wait 8x2 mck_mc cycles 3: wait 8x3 mck_mc cycles,etc.*/
#define DDRPHY_PLL_CTL5_DPI_PWROFF_PWR_WAIT_CYCLE(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_PLL_CTL5_DPI_PWROFF_RD_WAIT_CYCLE   ((u32)0x00000007 << 9)             /*!<R/W 'h clk on wait cycle when RD is end, 0: wait 1 mck_mc cycle 1: wait 8 mck_mc cycles 2: wait 8x2 mck_mc cycles 3: wait 8x3 mck_mc cycles,etc.*/
#define DDRPHY_PLL_CTL5_DPI_PWROFF_RD_WAIT_CYCLE(x)     ((u32)(((x) & 0x00000007) << 9))
#define DDRPHY_MASK_PLL_CTL5_DPI_PWROFF_WR_WAIT_CYCLE   ((u32)0x00000007 << 6)             /*!<R/W 'h clk on wait cycle when WR is end 0: wait 1 mck_mc cycle 1: wait 8 mck_mc cycles 2: wait 8x2 mck_mc cycles 3: wait 8x3 mck_mc cycles,etc.*/
#define DDRPHY_PLL_CTL5_DPI_PWROFF_WR_WAIT_CYCLE(x)     ((u32)(((x) & 0x00000007) << 6))
#define DDRPHY_MASK_PLL_CTL5_DPI_RW_PWROFF_MODE         ((u32)0x00000003 << 4)             /*!<R/W 'h0 rw blk clocks gating mode: 0: no gating 1: no REF and dfi_pwr = 0, 2: no REF/WR/RD 3: no REF and (no WR/RD or dfi_pwr = 0)*/
#define DDRPHY_PLL_CTL5_DPI_RW_PWROFF_MODE(x)           ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_MASK_PLL_CTL5_DPI_CA_PWROFF_MODE         ((u32)0x00000003 << 2)             /*!<R/W 'h0 CMD/ADDR clocks gating mode: 0: no gating 1: gating DPI clock when dfi_pwr = 0 2: reservedgating DPI clock and DCK when dfi_pwr = 0 3: reserved*/
#define DDRPHY_PLL_CTL5_DPI_CA_PWROFF_MODE(x)           ((u32)(((x) & 0x00000003) << 2))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_DLY_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_DLY_0_ADR07_DLY_SEL  ((u32)0x0000000F << 28)            /*!<R/W 'h0 Address[7] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR07_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_ADR_DLY_0_ADR06_DLY_SEL  ((u32)0x0000000F << 24)            /*!<R/W 'h0 Address[6] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR06_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_ADR_DLY_0_ADR05_DLY_SEL  ((u32)0x0000000F << 20)            /*!<R/W 'h0 Address[5] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR05_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_ADR_DLY_0_ADR04_DLY_SEL  ((u32)0x0000000F << 16)            /*!<R/W 'h0 Address[4] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR04_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_ADR_DLY_0_ADR03_DLY_SEL  ((u32)0x0000000F << 12)            /*!<R/W 'h0 Address[3] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR03_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_ADR_DLY_0_ADR02_DLY_SEL  ((u32)0x0000000F << 8)             /*!<R/W 'h0 Address[2] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR02_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_ADR_DLY_0_ADR01_DLY_SEL  ((u32)0x0000000F << 4)             /*!<R/W 'h0 Address[1] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR01_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_ADR_DLY_0_ADR00_DLY_SEL  ((u32)0x0000000F << 0)             /*!<R/W 'h0 Address[0] delay chain control*/
#define DDRPHY_ADR_DLY_0_ADR00_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_DLY_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_DLY_1_ADR15_DLY_SEL  ((u32)0x0000000F << 28)            /*!<R/W 'h0 Address[15] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR15_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_ADR_DLY_1_ADR14_DLY_SEL  ((u32)0x0000000F << 24)            /*!<R/W 'h0 Address[14] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR14_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_ADR_DLY_1_ADR13_DLY_SEL  ((u32)0x0000000F << 20)            /*!<R/W 'h0 Address[13] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR13_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_ADR_DLY_1_ADR12_DLY_SEL  ((u32)0x0000000F << 16)            /*!<R/W 'h0 Address[12] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR12_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_ADR_DLY_1_ADR11_DLY_SEL  ((u32)0x0000000F << 12)            /*!<R/W 'h0 Address[11] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR11_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_ADR_DLY_1_ADR10_DLY_SEL  ((u32)0x0000000F << 8)             /*!<R/W 'h0 Address[10] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR10_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_ADR_DLY_1_ADR09_DLY_SEL  ((u32)0x0000000F << 4)             /*!<R/W 'h0 Address[9] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR09_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_ADR_DLY_1_ADR08_DLY_SEL  ((u32)0x0000000F << 0)             /*!<R/W 'h0 Address[8] delay chain control*/
#define DDRPHY_ADR_DLY_1_ADR08_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_DLY_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_DLY_2_ALERT_DLY_SEL     ((u32)0x0000000F << 28)            /*!<R/W 'h0 Alert delay chain control*/
#define DDRPHY_ADR_DLY_2_ALERT_DLY_SEL(x)       ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_ADR_DLY_2_TEN_DLY_SEL       ((u32)0x0000000F << 24)            /*!<R/W 'h0 Ten delay chain control ( for DDR4 BG0 use)*/
#define DDRPHY_ADR_DLY_2_TEN_DLY_SEL(x)         ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_ADR_DLY_2_ADR16_DLY_SEL     ((u32)0x0000000F << 20)            /*!<R/W 'h0 Address[16] delay chain control (for DDR4 A1 used)*/
#define DDRPHY_ADR_DLY_2_ADR16_DLY_SEL(x)       ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_ADR_DLY_2_BA2_DLY_SEL       ((u32)0x0000000F << 16)            /*!<R/W 'h0 BANK[2] delay chain control*/
#define DDRPHY_ADR_DLY_2_BA2_DLY_SEL(x)         ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_ADR_DLY_2_BA1_DLY_SEL       ((u32)0x0000000F << 12)            /*!<R/W 'h0 BANK[1] delay chain control*/
#define DDRPHY_ADR_DLY_2_BA1_DLY_SEL(x)         ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_ADR_DLY_2_BA0_DLY_SEL       ((u32)0x0000000F << 8)             /*!<R/W 'h0 BANK[0] delay chain control*/
#define DDRPHY_ADR_DLY_2_BA0_DLY_SEL(x)         ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_ADR_DLY_2_ADR5_EXT_DLY_SEL  ((u32)0x0000000F << 4)             /*!<R/W 'h0 Extent address[5] delay chain control*/
#define DDRPHY_ADR_DLY_2_ADR5_EXT_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_ADR_DLY_2_ADR4_EXT_DLY_SEL  ((u32)0x0000000F << 0)             /*!<R/W 'h0 Extent address[4] delay chain control*/
#define DDRPHY_ADR_DLY_2_ADR4_EXT_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CMD_DLY_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CMD_DLY_0_CS_N_1_DLY_SEL  ((u32)0x0000000F << 26)            /*!<R/W 'h0 CS_N_1 delay chain control*/
#define DDRPHY_CMD_DLY_0_CS_N_1_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 26))
#define DDRPHY_MASK_CMD_DLY_0_CS_N_DLY_SEL    ((u32)0x0000000F << 20)            /*!<R/W 'h0 CS_N delay chain control*/
#define DDRPHY_CMD_DLY_0_CS_N_DLY_SEL(x)      ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_CMD_DLY_0_ODT_DLY_SEL     ((u32)0x0000000F << 16)            /*!<R/W 'h0 ODT delay chain control*/
#define DDRPHY_CMD_DLY_0_ODT_DLY_SEL(x)       ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_CMD_DLY_0_WE_N_DLY_SEL    ((u32)0x0000000F << 12)            /*!<R/W 'h0 WE_N delay chain control*/
#define DDRPHY_CMD_DLY_0_WE_N_DLY_SEL(x)      ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_CMD_DLY_0_CAS_N_DLY_SEL   ((u32)0x0000000F << 8)             /*!<R/W 'h0 CAS_N delay chain control*/
#define DDRPHY_CMD_DLY_0_CAS_N_DLY_SEL(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_CMD_DLY_0_RAS_N_DLY_SEL   ((u32)0x0000000F << 4)             /*!<R/W 'h0 RAS_N delay chain control*/
#define DDRPHY_CMD_DLY_0_RAS_N_DLY_SEL(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_CMD_DLY_0_CKE_DLY_SEL     ((u32)0x0000000F << 0)             /*!<R/W 'h0 CKE delay chain control*/
#define DDRPHY_CMD_DLY_0_CKE_DLY_SEL(x)       ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CMD_DLY_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CMD_DLY_1_CKE_1_DLY_SEL      ((u32)0x0000000F << 28)            /*!<R/W 'h0 CKE_1 delay chain control*/
#define DDRPHY_CMD_DLY_1_CKE_1_DLY_SEL(x)        ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_CMD_DLY_1_DDR4_CS_N_DLY_SEL  ((u32)0x0000000F << 24)            /*!<R/W 'h0 DDR4_CS_N delay chain control*/
#define DDRPHY_CMD_DLY_1_DDR4_CS_N_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_CMD_DLY_1_ALERT_1_DLY_SEL    ((u32)0x0000000F << 20)            /*!<R/W 'h0 ALERT_1 delay chain control*/
#define DDRPHY_CMD_DLY_1_ALERT_1_DLY_SEL(x)      ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_CMD_DLY_1_PARITY_1_DLY_SEL   ((u32)0x0000000F << 16)            /*!<R/W 'h0 PARITY_1 delay chain control*/
#define DDRPHY_CMD_DLY_1_PARITY_1_DLY_SEL(x)     ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_CMD_DLY_1_PARITY_DLY_SEL     ((u32)0x0000000F << 12)            /*!<R/W 'h0 PARITY delay chain control*/
#define DDRPHY_CMD_DLY_1_PARITY_DLY_SEL(x)       ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_CMD_DLY_1_DDR4_BG0_DLY_SEL   ((u32)0x0000000F << 8)             /*!<R/W 'h0 DDR4_BG0 delay chain control*/
#define DDRPHY_CMD_DLY_1_DDR4_BG0_DLY_SEL(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_CMD_DLY_1_DDR4_CKE_DLY_SEL   ((u32)0x0000000F << 4)             /*!<R/W 'h0 DDR4_CKE delay chain control*/
#define DDRPHY_CMD_DLY_1_DDR4_CKE_DLY_SEL(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_CMD_DLY_1_ACT_N_DLY_SEL      ((u32)0x0000000F << 0)             /*!<R/W 'h0 ACT_N delay chain control*/
#define DDRPHY_CMD_DLY_1_ACT_N_DLY_SEL(x)        ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_OE_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PAD_OE_CTL_ADR54_1TO1   ((u32)0x00000001 << 31)  /*!<R/W 'b1 Address[5:4] OCD control: 1: controlled by ocd_set_5 (1 -> 1) 0: controlled by ocd_set_4 (1 -> 2)*/
#define DDRPHY_BIT_PAD_OE_CTL_DCK_1_OE     ((u32)0x00000001 << 28)  /*!<R/W 'b1 Output enable of DCK_1*/
#define DDRPHY_BIT_PAD_OE_CTL_DCK_OE       ((u32)0x00000001 << 27)  /*!<R/W 'b1 Output enable of DCK*/
#define DDRPHY_BIT_PAD_OE_CTL_CKE_OE       ((u32)0x00000001 << 26)  /*!<R/W 'b1 Output enable of CKE*/
#define DDRPHY_BIT_PAD_OE_CTL_RAS_N_OE     ((u32)0x00000001 << 25)  /*!<R/W 'b1 Output enable of RAS_N*/
#define DDRPHY_BIT_PAD_OE_CTL_CAS_N_OE     ((u32)0x00000001 << 24)  /*!<R/W 'b1 Output enable of CAS_N*/
#define DDRPHY_BIT_PAD_OE_CTL_WE_N_OE      ((u32)0x00000001 << 23)  /*!<R/W 'b1 Output enable of WE_N*/
#define DDRPHY_BIT_PAD_OE_CTL_ODT_OE       ((u32)0x00000001 << 22)  /*!<R/W 'b1 Output enable of ODT*/
#define DDRPHY_BIT_PAD_OE_CTL_RESET_N_OE   ((u32)0x00000001 << 21)  /*!<R/W 'b1 Output enable of Reset_N*/
#define DDRPHY_BIT_PAD_OE_CTL_BK2_OE       ((u32)0x00000001 << 20)  /*!<R/W 'b1 Output enable of BANK[2]*/
#define DDRPHY_BIT_PAD_OE_CTL_BK1_OE       ((u32)0x00000001 << 19)  /*!<R/W 'b1 Output enable of BANK[1]*/
#define DDRPHY_BIT_PAD_OE_CTL_BK0_OE       ((u32)0x00000001 << 18)  /*!<R/W 'b1 Output enable of BANK[0]*/
#define DDRPHY_BIT_PAD_OE_CTL_EXT_ADR5_OE  ((u32)0x00000001 << 17)  /*!<R/W 'b1 Output enable of Extent address[5]*/
#define DDRPHY_BIT_PAD_OE_CTL_EXT_ADR4_OE  ((u32)0x00000001 << 16)  /*!<R/W 'b1 Output enable of Extent address[4]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR15_OE     ((u32)0x00000001 << 15)  /*!<R/W 'b1 Output enable of address[15]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR14_OE     ((u32)0x00000001 << 14)  /*!<R/W 'b1 Output enable of address[14]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR13_OE     ((u32)0x00000001 << 13)  /*!<R/W 'b1 Output enable of address[13]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR12_OE     ((u32)0x00000001 << 12)  /*!<R/W 'b1 Output enable of address[12]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR11_OE     ((u32)0x00000001 << 11)  /*!<R/W 'b1 Output enable of address[11]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR10_OE     ((u32)0x00000001 << 10)  /*!<R/W 'b1 Output enable of address[10]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR09_OE     ((u32)0x00000001 << 9)   /*!<R/W 'b1 Output enable of address[09]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR08_OE     ((u32)0x00000001 << 8)   /*!<R/W 'b1 Output enable of address[08]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR07_OE     ((u32)0x00000001 << 7)   /*!<R/W 'b1 Output enable of address[07]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR06_OE     ((u32)0x00000001 << 6)   /*!<R/W 'b1 Output enable of address[06]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR05_OE     ((u32)0x00000001 << 5)   /*!<R/W 'b1 Output enable of address[05]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR04_OE     ((u32)0x00000001 << 4)   /*!<R/W 'b1 Output enable of address[04]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR03_OE     ((u32)0x00000001 << 3)   /*!<R/W 'b1 Output enable of address[03]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR02_OE     ((u32)0x00000001 << 2)   /*!<R/W 'b1 Output enable of address[02]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR01_OE     ((u32)0x00000001 << 1)   /*!<R/W 'b1 Output enable of address[01]*/
#define DDRPHY_BIT_PAD_OE_CTL_ADR00_OE     ((u32)0x00000001 << 0)   /*!<R/W 'b1 Output enable of address[00]*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_OE_CTL_EX
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PAD_OE_CTL_EX_ODT_FORCE       ((u32)0x00000001 << 31)  /*!<R/W 'h0 1 for forcing ODT to high when do write leveling*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_ACT_N_OE        ((u32)0x00000001 << 17)  /*!<R/W 'b1 Output enable of ACTN*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_TEN_OE     ((u32)0x00000001 << 16)  /*!<R/W 'b1 Output enable of DDR4 TEN*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_ALERT0_OE  ((u32)0x00000001 << 15)  /*!<R/W 'b1 Output enable of DDR4 ALERT_0*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_ALERT1_OE  ((u32)0x00000001 << 14)  /*!<R/W 'b1 Output enable of DDR4 ALERT_1*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_CKE_OE     ((u32)0x00000001 << 13)  /*!<R/W 'b1 Output enable of DDR4 CKE*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_BG0_OE     ((u32)0x00000001 << 12)  /*!<R/W 'b1 Ouput enable of DDR4 BG0*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_PARITY_OE       ((u32)0x00000001 << 11)  /*!<R/W 'b1 Ouput enable of PARITY*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_PARITY_1_OE     ((u32)0x00000001 << 10)  /*!<R/W 'b1 Ouput enable of PARITY_1*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_ADR16_OE        ((u32)0x00000001 << 9)   /*!<R/W 'b1 Ouput enable of A16*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_DDR4_CS_N_OE    ((u32)0x00000001 << 8)   /*!<R/W 'b1 Output enable of DDR4 DCS_N*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_CKE_1_OE        ((u32)0x00000001 << 2)   /*!<R/W 'b1 Output enable of CKE_1*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_CS_N_1_OE       ((u32)0x00000001 << 1)   /*!<R/W 'b1 Output enable of DCS_N_1*/
#define DDRPHY_BIT_PAD_OE_CTL_EX_CS_N_OE         ((u32)0x00000001 << 0)   /*!<R/W 'b1 Output enable of DCS_N*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ALERT_SEL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ALERT_SEL_0_ALERT_SEL_0  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 ALERT enable of { DDR4 ALERT_1, DDR4 CKE, DDR4 BG0, PARITY, PARITY_1, CKE, RAS_N, CAS_N, WE_N, ODT, BANK[2], BANK[1], BANK[0], Extent address[5], Extent address[4], Address[16:0]}*/
#define DDRPHY_PAD_ALERT_SEL_0_ALERT_SEL_0(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ALERT_SEL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ALERT_SEL_1_ALERT_SEL_1  ((u32)0x0000000F << 0)            /*!<R/W 'h1 ALERT enable of { CKE_1, ACTN, DDR4 TEN, DDR4 ALERT_0}*/
#define DDRPHY_PAD_ALERT_SEL_1_ALERT_SEL_1(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ALERT_1_SEL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ALERT_1_SEL_0_ALERT_1_SEL_0  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h80000000 ALERT_1 enable of { DDR4 ALERT_1, DDR4 CKE, DDR4 BG0, PARITY, PARITY_1, CKE, RAS_N, CAS_N, WE_N, ODT, BANK[2], BANK[1], BANK[0], Extent address[5], Extent address[4], Address[16:0]}*/
#define DDRPHY_PAD_ALERT_1_SEL_0_ALERT_1_SEL_0(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ALERT_1_SEL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ALERT_1_SEL_1_ALERT_1_SEL_1  ((u32)0x0000000F << 0)            /*!<R/W 'h0 ALERT_1 enable of { CKE_1, ACTN, DDR4 TEN, DDR4 ALERT_0}*/
#define DDRPHY_PAD_ALERT_1_SEL_1_ALERT_1_SEL_1(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_IDDQ
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_IDDQ_PAD_OE_GATE_DIS_EXT  ((u32)0x000003FF << 16)            /*!<R/W 'h3FF 1: Disable corresponding pad's output enable gating function 0: Enable function [25]: CKE_1, [24]: ACTN, [23]: DDR4 TEN, [22]: DDR4 ALERT_0 [21]: DDR4 ALERT_1, [20]: DDR4 CKE, [19]: DDR4 BG0, [18]: PARITY, [17]: PARITY_1, [16]: Address[16]*/
#define DDRPHY_PAD_IDDQ_PAD_OE_GATE_DIS_EXT(x)    ((u32)(((x) & 0x000003FF) << 16))
#define DDRPHY_MASK_PAD_IDDQ_IDDQ                 ((u32)0x00000FFF << 0)             /*!<R/W 'h000 For power-down input comparator bit0: dqs[0] bit1: dqs_n[0] ...... bit6: dqs[3] bit7: dqs_n[3] bit8: dq[7:0] ...... bit11: dq[31:24]*/
#define DDRPHY_PAD_IDDQ_IDDQ(x)                   ((u32)(((x) & 0x00000FFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_CTRL_PROG
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_FILTER_EN  ((u32)0x00000001 << 31)            /*!<R/W 'h0 ZQ Calibration filter enable*/
#define DDRPHY_MASK_PAD_CTRL_PROG_DZQ_UP_SEL      ((u32)0x00000007 << 28)            /*!<R/W 'h0 Enable auto update OCD / ODT set0~set7 & set8~set11 with dzq_up_sel_ex*/
#define DDRPHY_PAD_CTRL_PROG_DZQ_UP_SEL(x)        ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_BIT_PAD_CTRL_PROG_DZQ_AUTO_UP      ((u32)0x00000001 << 27)            /*!<R/W 'h0 Auto update TTFP, TTCP, TTFN, TTCN, PT, NT, E2 of dq31~dq0 and dqs3~dqs0 when calibration done.*/
#define DDRPHY_MASK_PAD_CTRL_PROG_ZCTRL_CLK_SEL   ((u32)0x00000003 << 25)            /*!<R/W 'h2 Zctrl clock select 00: zclk/8 01: zclk/16 10: zclk/32 11: zclk/64*/
#define DDRPHY_PAD_CTRL_PROG_ZCTRL_CLK_SEL(x)     ((u32)(((x) & 0x00000003) << 25))
#define DDRPHY_BIT_PAD_CTRL_PROG_ZCTRL_START      ((u32)0x00000001 << 24)            /*!<R/W 'h0 Calibration start*/
#define DDRPHY_BIT_PAD_CTRL_PROG_ZCOMP_MODE       ((u32)0x00000001 << 23)            /*!<R/W 'b0 1'b0 : catch first result 1'b1 : catch last result*/
#define DDRPHY_MASK_PAD_CTRL_PROG_ZCTRL_MODE      ((u32)0x00000007 << 20)            /*!<R/W 'h0 3'h0 : NOCD(CAL00)+POCD(CAL01)+ NODT(CAL10)+PODT(CAL11) 3'h1 : NOCD(CAL00) only 3'h2 : POCD(CAL01) only 3'h3 : NODT(CAL10) only 3'h4 : PODT(CAL11) only*/
#define DDRPHY_PAD_CTRL_PROG_ZCTRL_MODE(x)        ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_BIT_PAD_CTRL_PROG_RZQ_EXT_R240     ((u32)0x00000001 << 19)            /*!<R/W 'h1 1: select external calibration resistor 0: select internal calibration resistor*/
#define DDRPHY_BIT_PAD_CTRL_PROG_DZQ_REF_UP       ((u32)0x00000001 << 18)            /*!<R/W 'h0 Auto update TTFP, TTCP, TTFN, TTCN, PT, NT, E2 of dq31~dq0 and dqs3~dqs0 during refresh*/
#define DDRPHY_BIT_PAD_CTRL_PROG_RZQ_COMP_INV     ((u32)0x00000001 << 17)            /*!<R/W 'h0 Invert comp polarity*/
#define DDRPHY_BIT_PAD_CTRL_PROG_RZQ_CAL_EN       ((u32)0x00000001 << 16)            /*!<R/W 'h0 Enable resistor 480 ohm calibration*/
#define DDRPHY_MASK_PAD_CTRL_PROG_ZCTR_NCYC       ((u32)0x0000007F << 8)             /*!<R/W 'd12 ZQ calibration PRE POCD/NODT/PODT null cycle (unit : zclk_p)*/
#define DDRPHY_PAD_CTRL_PROG_ZCTR_NCYC(x)         ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_BIT_PAD_CTRL_PROG_ZQ_CAL_CMD       ((u32)0x00000001 << 7)             /*!<R/W 'h0*/
#define DDRPHY_MASK_PAD_CTRL_PROG_ZCTR_NCYC_LRG   ((u32)0x0000007F << 0)             /*!<R/W 'd25 ZQ calibration PRE NOCD null cycle (unit : zclk_p)*/
#define DDRPHY_PAD_CTRL_PROG_ZCTR_NCYC_LRG(x)     ((u32)(((x) & 0x0000007F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_PAT_H_L_DET
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_PAT_H_L_DET_DQ_PAT_HIGH  ((u32)0x0000FFFF << 16)            /*!<R 'h0000 Read 1 if DQ bit below is ever 1 when dq_latch_en is high (dq_pat_high = 0 when dq_latch_en is low) Bit representing rule is the same as dq_pat_low.*/
#define DDRPHY_DQ_PAT_H_L_DET_DQ_PAT_HIGH(x)    ((u32)(((x) & 0x0000FFFF) << 16))
#define DDRPHY_MASK_DQ_PAT_H_L_DET_DQ_PAT_LOW   ((u32)0x0000FFFF << 0)             /*!<R 'h0000 Read 1 if DQ bit below is ever 0 when dq_latch_en is high (dq_pat_high = 0 when dq_latch_en is low) Bit [3/7/11/15] represent dq6 of slice-0/1/2/3 Bit [2/6/10/14] represent dq4 of slice-0/1/2/3 Bit [1/5/9/13] represent dq2 of slice-0/1/2/3 Bit [0/4/8/12] represent dq0 of slice-0/1/2/3*/
#define DDRPHY_DQ_PAT_H_L_DET_DQ_PAT_LOW(x)     ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_OE_GATE_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_OE_GATE_CTL_PAD_OE_GATE_DIS  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'hE7FFFFFF 1: Disable corresponding pad's output enable gating function 0: Enable function [31]: DDR4_CSN, [30]: CSN_1, [29]: CSN, [28]: CLK_1, [27]: CLK, [26]: CKE, [25]: RAS_N, [24]: CAS_N, [23]: WE_N, [22]: ODT, [21]: RSTN [20:18]: BANK[2:0], [17]: Extent address[5], [16]: Extent address[4], [15:0]: Address[15:0]*/
#define DDRPHY_PAD_OE_GATE_CTL_PAD_OE_GATE_DIS(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ZQ_NOCD2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ZQ_NOCD2_ZQ_OVWR_NOCD_DATA  ((u32)0x0000003F << 8)            /*!<R/W 'h00 when zctrl_mode = 2 or 4 (POCD/ PDOT only) ZQ _PAD NOCD = zq_ovwr_nocd_data*/
#define DDRPHY_ZQ_NOCD2_ZQ_OVWR_NOCD_DATA(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_BIT_ZQ_NOCD2_ZQ_ENA_NOCD2        ((u32)0x00000001 << 0)            /*!<R/W 'b0 directly control ZQ module NOCD2 enable*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ZCTRL_STATUS
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ZCTRL_STATUS_ZCTRL_STATUS  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 Zctrl execution status xxx_sts : 2'b00 : no error 2'b01 : overflow error 2'b10 : underflow error 2'b11 : calibration in progress [31] : zctrl_done [30] : rzq480_done [29:24] : Reserved [23:22] : Reserved [21:20] : rzq480_sts [19:18] : Reserved [17:16] : nocd2_sts [15:14] : Reserved [13:12] : podt_sts [11:10] : Reserved [ 9: 8] : nodt_sts [ 7: 6] : Reserved [ 5: 4] : pocd_sts [ 3: 2] : Reserved [ 1: 0] : nocd_sts*/
#define DDRPHY_PAD_ZCTRL_STATUS_ZCTRL_STATUS(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ZCTRL_RESULT
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ZCTRL_RESULT_ZCTRL_RESULT  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Zctrl execution result [31:30] : Reserved [29:24] : PODT [23] : Reserved [22:16] : NODT [15:14] : Reserved [13: 8] : POCD [ 7] : Reserved [ 6: 0] : NOCD*/
#define DDRPHY_PAD_ZCTRL_RESULT_ZCTRL_RESULT(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_ZCTRL_RESULT_D
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_ZCTRL_RESULT_D_ZCTRL_RESULT_DBG  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Zctrl execution result [31: 8] : Reserved [ 7] : Reserved [ 6: 0] : NOCD2*/
#define DDRPHY_PAD_ZCTRL_RESULT_D_ZCTRL_RESULT_DBG(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_RZCTRL_STATUS
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_RZCTRL_STATUS_RZQ_480CODE  ((u32)0x0000001F << 1)            /*!<R/W 'h0F Control resistor value*/
#define DDRPHY_PAD_RZCTRL_STATUS_RZQ_480CODE(x)    ((u32)(((x) & 0x0000001F) << 1))
#define DDRPHY_BIT_PAD_RZCTRL_STATUS_RZQ_CAL_DONE  ((u32)0x00000001 << 0)            /*!<R 'h0 Resistor 480 ohm calibration done*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_DQS_CTRL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_DQS_CTRL_DQS_TRIG_CNT  ((u32)0x00000003 << 29)            /*!<R/W 'h0 When write leveling/command bus training,DQS pulse number: 0: 1 pulse 1: 2 pulses 2: 4 pulses 3: 8 pulses*/
#define DDRPHY_DQ_DQS_CTRL_DQS_TRIG_CNT(x)    ((u32)(((x) & 0x00000003) << 29))
#define DDRPHY_BIT_DQ_DQS_CTRL_TEN_EN         ((u32)0x00000001 << 24)            /*!<R/W 'h0 1 for ten enable*/
#define DDRPHY_BIT_DQ_DQS_CTRL_DQS_OE_SIG     ((u32)0x00000001 << 3)             /*!<R/W 'h1 DQS OE*/
#define DDRPHY_BIT_DQ_DQS_CTRL_DQ_LATCH_EN    ((u32)0x00000001 << 2)             /*!<R/W 'h0 Capture DQ bus enable*/
#define DDRPHY_BIT_DQ_DQS_CTRL_TRIG_DQS       ((u32)0x00000001 << 1)             /*!<W 'h0 dqs 1T pulse trigger (one cycle pulse, HW auto clear).*/
#define DDRPHY_BIT_DQ_DQS_CTRL_DQS_OE_FRC_EN  ((u32)0x00000001 << 0)             /*!<R/W 'h0 when set to 1, oe of dqs pad state will depend on the vaule of bit[3] (dqs_oe_sig)*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CMD_ADR_PH
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_CMD_ADR_PH_CKE_1_PH_EN     ((u32)0x00000001 << 30)            /*!<R/W 'h0 CKE_1 output delay enable*/
#define DDRPHY_BIT_CMD_ADR_PH_CKE_PH_EN       ((u32)0x00000001 << 29)            /*!<R/W 'h0 CKE output delay enable*/
#define DDRPHY_BIT_CMD_ADR_PH_CS_1_PH_EN      ((u32)0x00000001 << 28)            /*!<R/W 'h0 CS_1 output delay enable*/
#define DDRPHY_BIT_CMD_ADR_PH_CS_PH_EN        ((u32)0x00000001 << 27)            /*!<R/W 'h0 CS output delay enable*/
#define DDRPHY_BIT_CMD_ADR_PH_CMD_PH_EN       ((u32)0x00000001 << 26)            /*!<R/W 'h0 Command output delay enable*/
#define DDRPHY_BIT_CMD_ADR_PH_CKE_1_GEARDOWN  ((u32)0x00000001 << 24)            /*!<R/W 'h0 0 : normal; 1 is gear down*/
#define DDRPHY_BIT_CMD_ADR_PH_CKE_GEARDOWN    ((u32)0x00000001 << 23)            /*!<R/W 'h0 0 : normal; 1 is gear down*/
#define DDRPHY_BIT_CMD_ADR_PH_CS_1_GEARDOWN   ((u32)0x00000001 << 22)            /*!<R/W 'h0 0 : normal; 1 is gear down*/
#define DDRPHY_BIT_CMD_ADR_PH_CS_GEARDOWN     ((u32)0x00000001 << 21)            /*!<R/W 'h0 0 : normal; 1 is gear down*/
#define DDRPHY_BIT_CMD_ADR_PH_CMD_GEARDOWN    ((u32)0x00000001 << 20)            /*!<R/W 'h0 0 : normal; 1 is gear down*/
#define DDRPHY_MASK_CMD_ADR_PH_CKE_1_PH_SEL   ((u32)0x00000007 << 16)            /*!<R/W 'h0 CKE_1 output delay : 3'b000 : delay 1 cycle 3'b001 : delay 2 cycles 3'b010 : delay 3 cycles 3'b011 : delay 4 cycles ............... 3'b111 : delay 8 cycles*/
#define DDRPHY_CMD_ADR_PH_CKE_1_PH_SEL(x)     ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_CMD_ADR_PH_CKE_PH_SEL     ((u32)0x00000007 << 12)            /*!<R/W 'h0 CKE output delay : 3'b000 : delay 1 cycle 3'b001 : delay 2 cycles 3'b010 : delay 3 cycles 3'b011 : delay 4 cycles ............... 3'b111 : delay 8 cycles*/
#define DDRPHY_CMD_ADR_PH_CKE_PH_SEL(x)       ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_CMD_ADR_PH_CS_1_PH_SEL    ((u32)0x00000007 << 8)             /*!<R/W 'h0 CS_1 output delay : 3'b000 : delay 1 cycle 3'b001 : delay 2 cycles 3'b010 : delay 3 cycles 3'b011 : delay 4 cycles ............... 3'b111 : delay 8 cycles*/
#define DDRPHY_CMD_ADR_PH_CS_1_PH_SEL(x)      ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_CMD_ADR_PH_CS_PH_SEL      ((u32)0x00000007 << 4)             /*!<R/W 'h0 CS output delay : 3'b000 : delay 1 cycle 3'b001 : delay 2 cycles 3'b010 : delay 3 cycles 3'b011 : delay 4 cycles ............... 3'b111 : delay 8 cycles*/
#define DDRPHY_CMD_ADR_PH_CS_PH_SEL(x)        ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_CMD_ADR_PH_CMD_PH_SEL     ((u32)0x00000007 << 0)             /*!<R/W 'h0 Command output delay : 3'b000 : delay 1 cycle 3'b001 : delay 2 cycles 3'b010 : delay 3 cycles 3'b011 : delay 4 cycles ............... 3'b111 : delay 8 cycles*/
#define DDRPHY_CMD_ADR_PH_CMD_PH_SEL(x)       ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_PAT_IN0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_PAT_IN0_DQ_PAT_IN  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Read data from DDR when dq_latch_en is high (dq_pat_in = 0 when dq_latch_en is low) [31] dq31 .............. [1] dq01 [0] dq00*/
#define DDRPHY_DQ_PAT_IN0_DQ_PAT_IN(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_PAT_IN1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_PAT_IN1_DQ_PAT_IN  ((u32)0x0000000F << 0)            /*!<R 'h0 Read data from DDR when dq_latch_en is high (dq_pat_in = 0 when dq_latch_en is low) [3] dbi3 [2] dbi2 [1] dbi1 [0] dbi0*/
#define DDRPHY_DQ_PAT_IN1_DQ_PAT_IN(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_WRLVL_DQS_DATA
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_WRLVL_DQS_DATA_WRLVL_DQSDATA  ((u32)0x000000FF << 0)            /*!<R 'h00 Read data from DDR when ten_en is high (wrlvl_dqsdata = 0 when ten_en is low) [7] dqsb_3 [6] dqs_3 [5] dqsb_2 [4] dqs_2 [3] dqsb_1 [2] dqs_1 [1] dqsb_0 [0] dqs_0*/
#define DDRPHY_WRLVL_DQS_DATA_WRLVL_DQSDATA(x)    ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_TEST_CTRL0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_TEST_CTRL0_LBK_MODE     ((u32)0x00000003 << 30)            /*!<R/W 'h0 Self-loopback mode [00] self-loopback mode disable [01] self-loopback bypass pad [10] self-loopback through pad [11] self-loopback dfi signals*/
#define DDRPHY_TEST_CTRL0_LBK_MODE(x)       ((u32)(((x) & 0x00000003) << 30))
#define DDRPHY_BIT_TEST_CTRL0_LBK_SET       ((u32)0x00000001 << 29)            /*!<R/W 'h0 Set below registers for loopback test automatically: rd_dly_pos_sel_* = 5'h0, rd_dly_neg_sel_* = 5'h0, cal_set_mode = 2'b11, rst_fifo_mode = 2'b10, data_pre = 0*/
#define DDRPHY_BIT_TEST_CTRL0_DATA_EN_CHK   ((u32)0x00000001 << 28)            /*!<R/W 'h0 1: Check burst length of dfi_wr/rddata_en, and return check results to data_en_err_type.*/
#define DDRPHY_MASK_TEST_CTRL0_TOP_DBG_SEL  ((u32)0x00000007 << 25)            /*!<R/W 'h0 Debug selection for top*/
#define DDRPHY_TEST_CTRL0_TOP_DBG_SEL(x)    ((u32)(((x) & 0x00000007) << 25))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_TEST_CTRL1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_TEST_CTRL1_DCC_DSP_DBG_SEL  ((u32)0x00000001 << 26)  /*!<R/W 'h0 Debug selection for all DCC DSP*/
#define DDRPHY_BIT_TEST_CTRL1_DPI_DBG_DLL_SEL  ((u32)0x00000001 << 24)  /*!<R/W 'h0 Debug selection for dpi_debug_dll[63:0] to read-only register*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_HALF_CYC_DET
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_HALF_CYC_DET_DET_CLK_ENABLE  ((u32)0x00000001 << 15)           /*!<R/W 'h0 Half cycle delay tap detector clk enable. Need first enable before det_enable.*/
#define DDRPHY_BIT_HALF_CYC_DET_DET_ENABLE      ((u32)0x00000001 << 14)           /*!<R/W 'h0 Half cycle delay tap detector enable*/
#define DDRPHY_BIT_HALF_CYC_DET_DET_DONE        ((u32)0x00000001 << 13)           /*!<R/W 'h0 Half cycle delay tap detector done*/
#define DDRPHY_MASK_HALF_CYC_DET_DET_RESULT     ((u32)0x00001FFF << 0)            /*!<R 'h0000 the number of delay tap*/
#define DDRPHY_HALF_CYC_DET_DET_RESULT(x)       ((u32)(((x) & 0x00001FFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_READ_CTRL_1_RX_CAL_DIS           ((u32)0x00000001 << 31)            /*!<R/W 'h0 1 :RX calibration disable Should set fw_set_rd_dly to active the function due to double-buffer design In order to prevent the te/delta/vref setting from restoring to FW default value, please make sure the following registers should be set to 0 before triggering fw_set_rd_dly fw_dqsen_ftun_upd =0 (in rw_spec) fw_rd_delta_upd =0 (in rw_spec) fw_rd_te_upd =0 (in rw_spec) vr_upd =0 (in dll_spec)*/
#define DDRPHY_MASK_READ_CTRL_1_RDDATA_EN_DLY_SEL   ((u32)0x00000003 << 28)            /*!<R/W 'h0 Select dfi_rddata_en_p* delay cycles in DPI. 00: delay 0T 01: delay 4T 10: delay 6T 11: delay 8T*/
#define DDRPHY_READ_CTRL_1_RDDATA_EN_DLY_SEL(x)     ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_BIT_READ_CTRL_1_RD_DLY_FOLLOW_DQ0    ((u32)0x00000001 << 10)            /*!<R/W 'h0 When this field is set to 1, FW can program read delay chain (dq31 ~ dq0) setting by dq0 (1800_006C ~ 1800_00C8)*/
#define DDRPHY_BIT_READ_CTRL_1_DQ_LAT_EN_FORCE_SEL  ((u32)0x00000001 << 9)             /*!<R/W 'h0 dq latch control (in receive_1st & n/p_FIFO) 0 : controlled by read command 1: controlled by dq_lat_en_force_sig*/
#define DDRPHY_BIT_READ_CTRL_1_DQ_LAT_EN_FORCE_SIG  ((u32)0x00000001 << 8)             /*!<R/W 'h0 If set dq_lat_en_force_sel, dq latch enable is controlled by dq_lat_en_force_sig.*/
#define DDRPHY_MASK_READ_CTRL_1_TM_RD_FIFO          ((u32)0x0000003F << 0)             /*!<R/W 'h02 read FIFO latency from read command 2T DCK unit for 2to1 mode 1T DCK unit for 1to1 mode*/
#define DDRPHY_READ_CTRL_1_TM_RD_FIFO(x)            ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DT_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DT_SET0_DTN_CLK  ((u32)0x00000003 << 20)            /*!<R/W 'h3 [0]: DTN of CLK, [1]: DTN of CLK_1*/
#define DDRPHY_DT_SET0_DTN_CLK(x)    ((u32)(((x) & 0x00000003) << 20))
#define DDRPHY_MASK_DT_SET0_DTP_CLK  ((u32)0x00000003 << 16)            /*!<R/W 'h0 [0]: DTP of CLK, [1]: DTP of CLK_1*/
#define DDRPHY_DT_SET0_DTP_CLK(x)    ((u32)(((x) & 0x00000003) << 16))
#define DDRPHY_MASK_DT_SET0_DTN_DQS  ((u32)0x0000000F << 12)            /*!<R/W 'hF [0]: DTN of DQS0, [1]: DTN of DQS1, [2]: DTN of DQS2, [3]: DTN of DQS3*/
#define DDRPHY_DT_SET0_DTN_DQS(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DT_SET0_DTP_DQS  ((u32)0x0000000F << 8)             /*!<R/W 'h0 [0]: DTP of DQS0, [1]: DTP of DQS1, [2]: DTP of DQS2, [3]: DTP of DQS3*/
#define DDRPHY_DT_SET0_DTP_DQS(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DT_SET0_DTN_DQ   ((u32)0x0000000F << 4)             /*!<R/W 'hF [0]: DTN of DQ[7:0], [1]: DTN of DQ[15:8], [2]: DTN of DQ[23:16], [3]: DTN of DQ[31:24]*/
#define DDRPHY_DT_SET0_DTN_DQ(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DT_SET0_DTP_DQ   ((u32)0x0000000F << 0)             /*!<R/W 'h0 [0]: DTP of DQ[7:0], [1]: DTP of DQ[15:8], [2]: DTP of DQ[23:16], [3]: DTP of DQ[31:24] [Note1] DTP: high-period input duty trimming [Note2] DTN: low-period input duty trimming*/
#define DDRPHY_DT_SET0_DTP_DQ(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCP0_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCP0_SET0_ODT_TTCP_SET_3_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h00 P-ODT coarse trimming set_3.*/
#define DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_3_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCP0_SET0_ODT_TTCP_SET_2_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h00 P-ODT coarse trimming set_2.*/
#define DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_2_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCP0_SET0_ODT_TTCP_SET_1_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h00 P-ODT coarse trimming set_1.*/
#define DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_1_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCP0_SET0_ODT_TTCP_SET_0_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h00 P-ODT coarse trimming set_0.*/
#define DDRPHY_ODT_TTCP0_SET0_ODT_TTCP_SET_0_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCP1_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCP1_SET0_ODT_TTCP_SET_7_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h00 P-ODT coarse trimming set_7.*/
#define DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_7_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCP1_SET0_ODT_TTCP_SET_6_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h00 P-ODT coarse trimming set_6.*/
#define DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_6_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCP1_SET0_ODT_TTCP_SET_5_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h00 P-ODT coarse trimming set_5.*/
#define DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_5_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCP1_SET0_ODT_TTCP_SET_4_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h00 P-ODT coarse trimming set_4.*/
#define DDRPHY_ODT_TTCP1_SET0_ODT_TTCP_SET_4_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCN0_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCN0_SET0_ODT_TTCN_SET_3_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h00 N-ODT coarse trimming set_3.*/
#define DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_3_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCN0_SET0_ODT_TTCN_SET_2_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h00 N-ODT coarse trimming set_2.*/
#define DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_2_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCN0_SET0_ODT_TTCN_SET_1_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h00 N-ODT coarse trimming set_1.*/
#define DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_1_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCN0_SET0_ODT_TTCN_SET_0_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h00 N-ODT coarse trimming set_0.*/
#define DDRPHY_ODT_TTCN0_SET0_ODT_TTCN_SET_0_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCN1_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCN1_SET0_ODT_TTCN_SET_7_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h00 N-ODT coarse trimming set_7.*/
#define DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_7_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCN1_SET0_ODT_TTCN_SET_6_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h00 N-ODT coarse trimming set_6.*/
#define DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_6_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCN1_SET0_ODT_TTCN_SET_5_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h00 N-ODT coarse trimming set_5.*/
#define DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_5_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCN1_SET0_ODT_TTCN_SET_4_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h00 N-ODT coarse trimming set_4.*/
#define DDRPHY_ODT_TTCN1_SET0_ODT_TTCN_SET_4_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDP0_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDP0_SET0_OCDP_SET_3_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h26 P-OCD trimming set_3.*/
#define DDRPHY_OCDP0_SET0_OCDP_SET_3_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDP0_SET0_OCDP_SET_2_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h26 P-OCD trimming set_2.*/
#define DDRPHY_OCDP0_SET0_OCDP_SET_2_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDP0_SET0_OCDP_SET_1_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h26 P-OCD trimming set_1.*/
#define DDRPHY_OCDP0_SET0_OCDP_SET_1_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDP0_SET0_OCDP_SET_0_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h26 P-OCD trimming set_0.*/
#define DDRPHY_OCDP0_SET0_OCDP_SET_0_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDP1_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDP1_SET0_OCDP_SET_7_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h26 P-OCD trimming set_7.*/
#define DDRPHY_OCDP1_SET0_OCDP_SET_7_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDP1_SET0_OCDP_SET_6_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h26 P-OCD trimming set_6.*/
#define DDRPHY_OCDP1_SET0_OCDP_SET_6_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDP1_SET0_OCDP_SET_5_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h26 P-OCD trimming set_5.*/
#define DDRPHY_OCDP1_SET0_OCDP_SET_5_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDP1_SET0_OCDP_SET_4_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h26 P-OCD trimming set_4.*/
#define DDRPHY_OCDP1_SET0_OCDP_SET_4_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDN0_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDN0_SET0_OCDN_SET_3_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h26 N-OCD trimming set_3.*/
#define DDRPHY_OCDN0_SET0_OCDN_SET_3_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDN0_SET0_OCDN_SET_2_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h26 N-OCD trimming set_2.*/
#define DDRPHY_OCDN0_SET0_OCDN_SET_2_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDN0_SET0_OCDN_SET_1_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h26 N-OCD trimming set_1.*/
#define DDRPHY_OCDN0_SET0_OCDN_SET_1_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDN0_SET0_OCDN_SET_0_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h26 N-OCD trimming set_0.*/
#define DDRPHY_OCDN0_SET0_OCDN_SET_0_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDN1_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDN1_SET0_OCDN_SET_7_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h26 N-OCD trimming set_7.*/
#define DDRPHY_OCDN1_SET0_OCDN_SET_7_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDN1_SET0_OCDN_SET_6_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h26 N-OCD trimming set_6.*/
#define DDRPHY_OCDN1_SET0_OCDN_SET_6_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDN1_SET0_OCDN_SET_5_PRE  ((u32)0x0000003F << 8)             /*!<R/W 'h26 N-OCD trimming set_5.*/
#define DDRPHY_OCDN1_SET0_OCDN_SET_5_PRE(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDN1_SET0_OCDN_SET_4_PRE  ((u32)0x0000003F << 0)             /*!<R/W 'h26 N-OCD trimming set_4.*/
#define DDRPHY_OCDN1_SET0_OCDN_SET_4_PRE(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCP0_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCP0_SET1_ODT_TTCP_SET_3  ((u32)0x0000003F << 24)            /*!<R/W 'h00 P-ODT coarse trimming set_3.*/
#define DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_3(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCP0_SET1_ODT_TTCP_SET_2  ((u32)0x0000003F << 16)            /*!<R/W 'h00 P-ODT coarse trimming set_2.*/
#define DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_2(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCP0_SET1_ODT_TTCP_SET_1  ((u32)0x0000003F << 8)             /*!<R/W 'h00 P-ODT coarse trimming set_1.*/
#define DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_1(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCP0_SET1_ODT_TTCP_SET_0  ((u32)0x0000003F << 0)             /*!<R/W 'h00 P-ODT coarse trimming set_0.*/
#define DDRPHY_ODT_TTCP0_SET1_ODT_TTCP_SET_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCP1_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCP1_SET1_ODT_TTCP_SET_7  ((u32)0x0000003F << 24)            /*!<R/W 'h00 P-ODT coarse trimming set_7.*/
#define DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_7(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCP1_SET1_ODT_TTCP_SET_6  ((u32)0x0000003F << 16)            /*!<R/W 'h00 P-ODT coarse trimming set_6.*/
#define DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_6(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCP1_SET1_ODT_TTCP_SET_5  ((u32)0x0000003F << 8)             /*!<R/W 'h00 P-ODT coarse trimming set_5.*/
#define DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_5(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCP1_SET1_ODT_TTCP_SET_4  ((u32)0x0000003F << 0)             /*!<R/W 'h00 P-ODT coarse trimming set_4.*/
#define DDRPHY_ODT_TTCP1_SET1_ODT_TTCP_SET_4(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCN0_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCN0_SET1_ODT_TTCN_SET_3  ((u32)0x0000003F << 24)            /*!<R/W 'h00 N-ODT coarse trimming set_3.*/
#define DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_3(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCN0_SET1_ODT_TTCN_SET_2  ((u32)0x0000003F << 16)            /*!<R/W 'h00 N-ODT coarse trimming set_2.*/
#define DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_2(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCN0_SET1_ODT_TTCN_SET_1  ((u32)0x0000003F << 8)             /*!<R/W 'h00 N-ODT coarse trimming set_1.*/
#define DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_1(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCN0_SET1_ODT_TTCN_SET_0  ((u32)0x0000003F << 0)             /*!<R/W 'h00 N-ODT coarse trimming set_0.*/
#define DDRPHY_ODT_TTCN0_SET1_ODT_TTCN_SET_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCN1_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCN1_SET1_ODT_TTCN_SET_7  ((u32)0x0000003F << 24)            /*!<R/W 'h00 N-ODT coarse trimming set_7.*/
#define DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_7(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCN1_SET1_ODT_TTCN_SET_6  ((u32)0x0000003F << 16)            /*!<R/W 'h00 N-ODT coarse trimming set_6.*/
#define DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_6(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCN1_SET1_ODT_TTCN_SET_5  ((u32)0x0000003F << 8)             /*!<R/W 'h00 N-ODT coarse trimming set_5.*/
#define DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_5(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCN1_SET1_ODT_TTCN_SET_4  ((u32)0x0000003F << 0)             /*!<R/W 'h00 N-ODT coarse trimming set_4.*/
#define DDRPHY_ODT_TTCN1_SET1_ODT_TTCN_SET_4(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDP0_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDP0_SET1_OCDP_SET_3  ((u32)0x0000003F << 24)            /*!<R/W 'h26 P-OCD trimming set_3.*/
#define DDRPHY_OCDP0_SET1_OCDP_SET_3(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDP0_SET1_OCDP_SET_2  ((u32)0x0000003F << 16)            /*!<R/W 'h26 P-OCD trimming set_2.*/
#define DDRPHY_OCDP0_SET1_OCDP_SET_2(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDP0_SET1_OCDP_SET_1  ((u32)0x0000003F << 8)             /*!<R/W 'h26 P-OCD trimming set_1.*/
#define DDRPHY_OCDP0_SET1_OCDP_SET_1(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDP0_SET1_OCDP_SET_0  ((u32)0x0000003F << 0)             /*!<R/W 'h26 P-OCD trimming set_0.*/
#define DDRPHY_OCDP0_SET1_OCDP_SET_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDP1_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDP1_SET1_OCDP_SET_7  ((u32)0x0000003F << 24)            /*!<R/W 'h26 P-OCD trimming set_7.*/
#define DDRPHY_OCDP1_SET1_OCDP_SET_7(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDP1_SET1_OCDP_SET_6  ((u32)0x0000003F << 16)            /*!<R/W 'h26 P-OCD trimming set_6.*/
#define DDRPHY_OCDP1_SET1_OCDP_SET_6(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDP1_SET1_OCDP_SET_5  ((u32)0x0000003F << 8)             /*!<R/W 'h26 P-OCD trimming set_5.*/
#define DDRPHY_OCDP1_SET1_OCDP_SET_5(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDP1_SET1_OCDP_SET_4  ((u32)0x0000003F << 0)             /*!<R/W 'h26 P-OCD trimming set_4.*/
#define DDRPHY_OCDP1_SET1_OCDP_SET_4(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDN0_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDN0_SET1_OCDN_SET_3  ((u32)0x0000003F << 24)            /*!<R/W 'h26 N-OCD trimming set_3.*/
#define DDRPHY_OCDN0_SET1_OCDN_SET_3(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDN0_SET1_OCDN_SET_2  ((u32)0x0000003F << 16)            /*!<R/W 'h26 N-OCD trimming set_2.*/
#define DDRPHY_OCDN0_SET1_OCDN_SET_2(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDN0_SET1_OCDN_SET_1  ((u32)0x0000003F << 8)             /*!<R/W 'h26 N-OCD trimming set_1.*/
#define DDRPHY_OCDN0_SET1_OCDN_SET_1(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDN0_SET1_OCDN_SET_0  ((u32)0x0000003F << 0)             /*!<R/W 'h26 N-OCD trimming set_0.*/
#define DDRPHY_OCDN0_SET1_OCDN_SET_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDN1_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDN1_SET1_OCDN_SET_7  ((u32)0x0000003F << 24)            /*!<R/W 'h26 N-OCD trimming set_7.*/
#define DDRPHY_OCDN1_SET1_OCDN_SET_7(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDN1_SET1_OCDN_SET_6  ((u32)0x0000003F << 16)            /*!<R/W 'h26 N-OCD trimming set_6.*/
#define DDRPHY_OCDN1_SET1_OCDN_SET_6(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDN1_SET1_OCDN_SET_5  ((u32)0x0000003F << 8)             /*!<R/W 'h26 N-OCD trimming set_5.*/
#define DDRPHY_OCDN1_SET1_OCDN_SET_5(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDN1_SET1_OCDN_SET_4  ((u32)0x0000003F << 0)             /*!<R/W 'h26 N-OCD trimming set_4.*/
#define DDRPHY_OCDN1_SET1_OCDN_SET_4(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_ODT_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_ODT_SEL_DQ_1_TTCN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 N-ODT coarse tune selection for slice1 / slice3 DQ 0000: odt_ttcn_set_0 ... 1011: odt_ttcn_set_11 bit3: *_ttc*_sel_ex*/
#define DDRPHY_DQ_ODT_SEL_DQ_1_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQ_ODT_SEL_DQ_0_TTCN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 N-ODT coarse tune selection for slice0 / slice2 DQ 0000: odt_ttcn_set_0 ... 1011: odt_ttcn_set_11*/
#define DDRPHY_DQ_ODT_SEL_DQ_0_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQ_ODT_SEL_DQ_1_TTCP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 P-ODT coarse tune selection for slice1 / slice3 DQ 0000: odt_ttcp_set_0 ... 1011: odt_ttcp_set_11*/
#define DDRPHY_DQ_ODT_SEL_DQ_1_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQ_ODT_SEL_DQ_0_TTCP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 P-ODT coarse tune selection for slice0 / slice2 DQ 0000: odt_ttcp_set_0 ... 1011: odt_ttcp_set_11*/
#define DDRPHY_DQ_ODT_SEL_DQ_0_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_OCD_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_OCD_SEL_DQ_1_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 N-OCD selection for slice1 / slice3 DQ 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_DQ_OCD_SEL_DQ_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQ_OCD_SEL_DQ_0_OCDN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 N-OCD selection for slice0 / slice2 DQ 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQ_OCD_SEL_DQ_0_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQ_OCD_SEL_DQ_1_OCDP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 P-OCD selection for slice1 / slice3 DQ 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQ_OCD_SEL_DQ_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQ_OCD_SEL_DQ_0_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 P-OCD selection for slice0 / slice2 DQ 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQ_OCD_SEL_DQ_0_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_P_ODT_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_P_ODT_SEL_DQS_P_1_TTCN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 N-ODT coarse tune selection for DQS[1] / DQS[3] 0000: odt_ttcn_set_0 ... 1011: odt_ttcn_set_11 bit3: *_ttc*_sel_ex*/
#define DDRPHY_DQS_P_ODT_SEL_DQS_P_1_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQS_P_ODT_SEL_DQS_P_0_TTCN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 N-ODT coarse tune selection for DQS[0] / DQS[2] 0000: odt_ttcn_set_0 ... 1011: odt_ttcn_set_11*/
#define DDRPHY_DQS_P_ODT_SEL_DQS_P_0_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQS_P_ODT_SEL_DQS_P_1_TTCP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 P-ODT coarse tune selection for DQS[1] / DQS[3] 0000: odt_ttcp_set_0 ... 1011: odt_ttcp_set_11*/
#define DDRPHY_DQS_P_ODT_SEL_DQS_P_1_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQS_P_ODT_SEL_DQS_P_0_TTCP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 P-ODT coarse tune selection for DQS[0] / DQS[2] 0000: odt_ttcp_set_0 ... 1011: odt_ttcp_set_11*/
#define DDRPHY_DQS_P_ODT_SEL_DQS_P_0_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_N_ODT_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_N_ODT_SEL_DQS_N_1_TTCN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 N-ODT coarse tune selection for DQS#[1] / DQS#[3] 0000: odt_ttcn_set_0 ... 1011: odt_ttcn_set_11 bit3: *_ttc*_sel_ex*/
#define DDRPHY_DQS_N_ODT_SEL_DQS_N_1_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQS_N_ODT_SEL_DQS_N_0_TTCN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 N-ODT coarse tune selection for DQS#[0] / DQS#[2] 0000: odt_ttcn_set_0 ... 1011: odt_ttcn_set_11*/
#define DDRPHY_DQS_N_ODT_SEL_DQS_N_0_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQS_N_ODT_SEL_DQS_N_1_TTCP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 P-ODT coarse tune selection for DQS#[1] / DQS#[3] 0000: odt_ttcp_set_0 ... 1011: odt_ttcp_set_11*/
#define DDRPHY_DQS_N_ODT_SEL_DQS_N_1_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQS_N_ODT_SEL_DQS_N_0_TTCP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 P-ODT coarse tune selection for DQS#[0] / DQS#[2] 0000: odt_ttcp_set_0 ... 1011: odt_ttcp_set_11*/
#define DDRPHY_DQS_N_ODT_SEL_DQS_N_0_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_OCD_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_N_1_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h0 N-OCD selection for DQS#[1] / DQS#[3] 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_DQS_OCD_SEL_DQS_N_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_N_0_OCDN_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h0 N-OCD selection for DQS#[0] / DQS#[2] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_N_0_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_N_1_OCDP_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h0 P-OCD selection for DQS#[1] / DQS#[3] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_N_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_N_0_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 P-OCD selection for DQS#[0] / DQS#[2] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_N_0_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_P_1_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 N-OCD selection for DQS[1] / DQS[3] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_P_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_P_0_OCDN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 N-OCD selection for DQS[0] / DQS[2] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_P_0_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_P_1_OCDP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 P-OCD selection for DQS[1] / DQS[3] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_P_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQS_OCD_SEL_DQS_P_0_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 P-OCD selection for DQS[0] / DQS[2] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_DQS_P_0_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CKE_OCD_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CKE_OCD_SEL_CKE_1_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for CKE_1 (no use) 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_CKE_OCD_SEL_CKE_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_CKE_OCD_SEL_CKE_1_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for CKE_1 (no use) 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CKE_OCD_SEL_CKE_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_CKE_OCD_SEL_CKE_OCDN_SEL    ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for CKE 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_CKE_OCD_SEL_CKE_OCDN_SEL(x)      ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_CKE_OCD_SEL_CKE_OCDP_SEL    ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for CKE 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CKE_OCD_SEL_CKE_OCDP_SEL(x)      ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_OCD_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_OCD_SEL_CS_1_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for CS_1 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_ADR_OCD_SEL_CS_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_ADR_OCD_SEL_CS_1_OCDP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for CS_1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_ADR_OCD_SEL_CS_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_ADR_OCD_SEL_CS_OCDN_SEL    ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for CS 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_ADR_OCD_SEL_CS_OCDN_SEL(x)      ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_ADR_OCD_SEL_CS_OCDP_SEL    ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for CS 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_ADR_OCD_SEL_CS_OCDP_SEL(x)      ((u32)(((x) & 0x00000007) << 8))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CK_OCD_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CK_OCD_SEL_CK_N_1_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h2 N-OCD selection for DCK#[1] 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_CK_OCD_SEL_CK_N_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_CK_OCD_SEL_CK_N_0_OCDN_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h2 N-OCD selection for DCK#[0] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_N_0_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_CK_OCD_SEL_CK_N_1_OCDP_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h2 P-OCD selection for DCK#[1] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_N_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_CK_OCD_SEL_CK_N_0_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h2 P-OCD selection for DCK#[0] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_N_0_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_CK_OCD_SEL_CK_P_1_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h2 N-OCD selection for DCK[1] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_P_1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_CK_OCD_SEL_CK_P_0_OCDN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h2 N-OCD selection for DCK[0] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_P_0_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_CK_OCD_SEL_CK_P_1_OCDP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h2 P-OCD selection for DCK[1] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_P_1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_CK_OCD_SEL_CK_P_0_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h2 P-OCD selection for DCK[0] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CK_OCD_SEL_CK_P_0_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_BUS_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PAD_BUS_0_EN_DCC            ((u32)0x00000001 << 31)            /*!<R/W 'h1 enable O_DCC output of CLK pad*/
#define DDRPHY_BIT_PAD_BUS_0_PD_CK             ((u32)0x00000001 << 30)            /*!<R/W 'h0 Rx power down of CLK pad*/
#define DDRPHY_BIT_PAD_BUS_0_EN_DMYI           ((u32)0x00000001 << 29)            /*!<R/W 'h0*/
#define DDRPHY_BIT_PAD_BUS_0_VREF_SEL          ((u32)0x00000001 << 28)            /*!<R/W 'h0*/
#define DDRPHY_BIT_PAD_BUS_0_VREF_RANGE        ((u32)0x00000001 << 27)            /*!<R/W 'h1 VREF VREF_RANGE for VREFZQ pad 1'b1: 0.5% - 64.5% 1'b0: 28% - 92%*/
#define DDRPHY_MASK_PAD_BUS_0_VREF_S           ((u32)0x0000003F << 21)            /*!<R/W 'h0A VREF S for VREFZQ pad Control Vref voltage with vref_range, and add 1% voltage when add 1 level*/
#define DDRPHY_PAD_BUS_0_VREF_S(x)             ((u32)(((x) & 0x0000003F) << 21))
#define DDRPHY_MASK_PAD_BUS_0_VREF_CURRENTSEL  ((u32)0x00000003 << 19)            /*!<R/W 'h3*/
#define DDRPHY_PAD_BUS_0_VREF_CURRENTSEL(x)    ((u32)(((x) & 0x00000003) << 19))
#define DDRPHY_BIT_PAD_BUS_0_VREF_PD           ((u32)0x00000001 << 18)            /*!<R/W 'h0 VREF pd for VREFZQ pad Power down Vref. zq_pd = 1 when use external Vref, zq_pad = 0 when use internal Vref*/
#define DDRPHY_MASK_PAD_BUS_0_PAD_BUS_0        ((u32)0x0003FFFF << 0)             /*!<R/W 'h00000*/
#define DDRPHY_PAD_BUS_0_PAD_BUS_0(x)          ((u32)(((x) & 0x0003FFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_BUS_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PAD_BUS_1_PD_CK_1       ((u32)0x00000001 << 31)           /*!<R/W 'h0 Rx power down of CLK_1 pad*/
#define DDRPHY_BIT_PAD_BUS_1_VREF_RANGE_4  ((u32)0x00000001 << 15)           /*!<R/W 'h1 VREF VREF_RANGE for normal Command/Address(CA) VREF pad 1'b1: 0.5% - 64.5% 1'b0: 28% - 92%*/
#define DDRPHY_MASK_PAD_BUS_1_VREF_S_4     ((u32)0x0000003F << 9)            /*!<R/W 'h0A VREF S for normal CA VREF pad Control Vref voltage with vref_range, and add 1% voltage when add 1 level*/
#define DDRPHY_PAD_BUS_1_VREF_S_4(x)       ((u32)(((x) & 0x0000003F) << 9))
#define DDRPHY_BIT_PAD_BUS_1_VREF_PD_4     ((u32)0x00000001 << 8)            /*!<R/W 'h0 VREF pd for normal CA VREF pad*/
#define DDRPHY_MASK_PAD_BUS_1_PAD_BUS_1    ((u32)0x0000007F << 0)            /*!<R/W 'h00 [3:0]: No use [4]: No Use [5]: No Use [6]: ZQ power down 1 : ZQ PAD power down 0 : ZQ PAD enable*/
#define DDRPHY_PAD_BUS_1_PAD_BUS_1(x)      ((u32)(((x) & 0x0000007F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_BUS_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PAD_BUS_2_VREF_RANGE_3  ((u32)0x00000001 << 31)            /*!<R/W 'h1 VREF VREF_RANGE for normal byte-3 VREF pad 1'b1: 0.5% - 64.5% 1'b0: 28% - 92%*/
#define DDRPHY_MASK_PAD_BUS_2_VREF_S_3     ((u32)0x0000003F << 25)            /*!<R/W 'h0A VREF S for normal byte-3 VREF pad Control Vref voltage with vref_range, and add 1% voltage when add 1 level*/
#define DDRPHY_PAD_BUS_2_VREF_S_3(x)       ((u32)(((x) & 0x0000003F) << 25))
#define DDRPHY_BIT_PAD_BUS_2_VREF_PD_3     ((u32)0x00000001 << 24)            /*!<R/W 'h0 VREF pd for normal byte-3 VREF pad*/
#define DDRPHY_BIT_PAD_BUS_2_VREF_RANGE_2  ((u32)0x00000001 << 23)            /*!<R/W 'h1 VREF VREF_RANGE for normal byte-2 VREF pad 1'b1: 0.5% - 64.5% 1'b0: 28% - 92%*/
#define DDRPHY_MASK_PAD_BUS_2_VREF_S_2     ((u32)0x0000003F << 17)            /*!<R/W 'h0A VREF S for normal byte-2 VREF pad Control Vref voltage with vref_range, and add 1% voltage when add 1 level*/
#define DDRPHY_PAD_BUS_2_VREF_S_2(x)       ((u32)(((x) & 0x0000003F) << 17))
#define DDRPHY_BIT_PAD_BUS_2_VREF_PD_2     ((u32)0x00000001 << 16)            /*!<R/W 'h0 VREF pd for normal byte-2 VREF pad*/
#define DDRPHY_BIT_PAD_BUS_2_VREF_RANGE_1  ((u32)0x00000001 << 15)            /*!<R/W 'h1 VREF VREF_RANGE for normal byte-1 VREF pad 1'b1: 0.5% - 64.5% 1'b0: 28% - 92%*/
#define DDRPHY_MASK_PAD_BUS_2_VREF_S_1     ((u32)0x0000003F << 9)             /*!<R/W 'h0A VREF S for normal byte-1 VREF pad Control Vref voltage with vref_range, and add 1% voltage when add 1 level*/
#define DDRPHY_PAD_BUS_2_VREF_S_1(x)       ((u32)(((x) & 0x0000003F) << 9))
#define DDRPHY_BIT_PAD_BUS_2_VREF_PD_1     ((u32)0x00000001 << 8)             /*!<R/W 'h0 VREF pd for normal byte-1 VREF pad*/
#define DDRPHY_BIT_PAD_BUS_2_VREF_RANGE_0  ((u32)0x00000001 << 7)             /*!<R/W 'h1 VREF VREF_RANGE for normal byte-0 VREF pad 1'b1: 0.5% - 64.5% 1'b0: 28% - 92%*/
#define DDRPHY_MASK_PAD_BUS_2_VREF_S_0     ((u32)0x0000003F << 1)             /*!<R/W 'h0A VREF S for normal byte-0 VREF pad Control Vref voltage with vref_range, and add 1% voltage when add 1 level*/
#define DDRPHY_PAD_BUS_2_VREF_S_0(x)       ((u32)(((x) & 0x0000003F) << 1))
#define DDRPHY_BIT_PAD_BUS_2_VREF_PD_0     ((u32)0x00000001 << 0)             /*!<R/W 'h0 VREF pd for normal byte-0 VREF pad*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ZQ_TRIM
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_ZQ_TRIM_DZQ_UP_SEL_EX  ((u32)0x00000001 << 1)  /*!<R/W 'h0 Enable auto update OCD / ODT set8~set11*/
#define DDRPHY_BIT_ZQ_TRIM_R480_TRIM      ((u32)0x00000001 << 0)  /*!<R/W 'h0 control R480 calibration trim 1'b1 : r480 result -1*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_0_ADR00_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for A0 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_0_ADR00_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_0_ADR00_OCDP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for A0 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_0_ADR00_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_0_ADR01_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for A1 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_0_ADR01_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_0_ADR01_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for A1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_0_ADR01_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_0_ADR02_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for A2 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_0_ADR02_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_0_ADR02_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for A2 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_0_ADR02_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_0_ADR03_OCDN_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for A3 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_0_ADR03_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_0_ADR03_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for A3 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_0_ADR03_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_1_ADR04_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for A4 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_1_ADR04_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_1_ADR04_OCDP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for A4 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_1_ADR04_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_1_ADR05_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for A5 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_1_ADR05_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_1_ADR05_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for A5 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_1_ADR05_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_1_ADR06_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for A6 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_1_ADR06_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_1_ADR06_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for A6 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_1_ADR06_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_1_ADR07_OCDN_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for A7 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_1_ADR07_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_1_ADR07_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for A7 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_1_ADR07_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_2_ADR08_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for A8 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_2_ADR08_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_2_ADR08_OCDP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for A8 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_2_ADR08_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_2_ADR09_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for A9 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_2_ADR09_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_2_ADR09_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for A9 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_2_ADR09_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_2_ADR10_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for A10 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_2_ADR10_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_2_ADR10_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for A10 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_2_ADR10_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_2_ADR11_OCDN_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for A11 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_2_ADR11_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_2_ADR11_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for A11 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_2_ADR11_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_3_ADR12_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for A12 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_3_ADR12_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_3_ADR12_OCDP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for A12 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_3_ADR12_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_3_ADR13_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for A13 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_3_ADR13_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_3_ADR13_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for A13 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_3_ADR13_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_3_ADR14_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for A14 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_3_ADR14_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_3_ADR14_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for A14 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_3_ADR14_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_3_ADR15_OCDN_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for A15 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_3_ADR15_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_3_ADR15_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for A15 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_3_ADR15_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_4
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_4_ADR16_OCDN_SEL      ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for A16 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_4_ADR16_OCDN_SEL(x)        ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_4_ADR16_OCDP_SEL      ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for A16 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_4_ADR16_OCDP_SEL(x)        ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_4_ADR_EXT_4_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for AD1_4 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_4_ADR_EXT_4_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_4_ADR_EXT_4_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for AD1_4 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_4_ADR_EXT_4_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_4_ADR_EXT_5_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for AD1_5 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_4_ADR_EXT_5_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_4_ADR_EXT_5_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for AD1_5 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_4_ADR_EXT_5_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_4_BA0_OCDN_SEL        ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for BA0 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_4_BA0_OCDN_SEL(x)          ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_4_BA0_OCDP_SEL        ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for BA0 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_4_BA0_OCDP_SEL(x)          ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_5
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_5_BA1_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for BA1 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_5_BA1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_5_BA1_OCDP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for BA1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_5_BA1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_5_BA2_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for BA2 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_5_BA2_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_5_BA2_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for BA2 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_5_BA2_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_5_RAS_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for RASN 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_5_RAS_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_5_RAS_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for RASN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_5_RAS_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_5_CAS_OCDN_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for CASN 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_5_CAS_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_5_CAS_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for CASN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_5_CAS_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_6
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_6_WE_OCDN_SEL   ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for WEN 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_6_WE_OCDN_SEL(x)     ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_6_WE_OCDP_SEL   ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for WEN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_6_WE_OCDP_SEL(x)     ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_6_ODT_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for ODT 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_6_ODT_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_6_ODT_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for ODT 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_6_ODT_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_6_RST_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for RSTN 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_6_RST_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_6_RST_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for RSTN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_6_RST_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_6_PAR_OCDN_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for PAR 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_6_PAR_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_6_PAR_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for PAR 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_6_PAR_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_7
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_7_PAR1_OCDN_SEL      ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for PAR_1 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_7_PAR1_OCDN_SEL(x)        ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_7_PAR1_OCDP_SEL      ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for PAR_1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_7_PAR1_OCDP_SEL(x)        ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_7_ACT_OCDN_SEL       ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for ACTN 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_7_ACT_OCDN_SEL(x)         ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_7_ACT_OCDP_SEL       ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for ACTN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_7_ACT_OCDP_SEL(x)         ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_OCD_SEL_7_DDR4_BG0_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 N-OCD selection for DDR4_BG0 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_7_DDR4_BG0_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_OCD_SEL_7_DDR4_BG0_OCDP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 P-OCD selection for DDR4_BG0 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_7_DDR4_BG0_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_OCD_SEL_7_TEN_OCDN_SEL       ((u32)0x00000007 << 4)             /*!<R/W 'h1 N-OCD selection for DDR4_TEN 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_7_TEN_OCDN_SEL(x)         ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_OCD_SEL_7_TEN_OCDP_SEL       ((u32)0x00000007 << 0)             /*!<R/W 'h1 P-OCD selection for DDR4_TEN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_7_TEN_OCDP_SEL(x)         ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_8
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCD_SEL_8_ALERT_OCDN_SEL   ((u32)0x00000007 << 28)            /*!<R/W 'h1 N-OCD selection for ALERT 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_OCD_SEL_8_ALERT_OCDN_SEL(x)     ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_OCD_SEL_8_ALERT_OCDP_SEL   ((u32)0x00000007 << 24)            /*!<R/W 'h1 P-OCD selection for ALERT 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_8_ALERT_OCDP_SEL(x)     ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_OCD_SEL_8_ALERT1_OCDN_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h1 N-OCD selection for ALERT_1 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_OCD_SEL_8_ALERT1_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_OCD_SEL_8_ALERT1_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h1 P-OCD selection for ALERT_1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_OCD_SEL_8_ALERT1_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_SEL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_SEL_0_CA_TTCN_SEL  ((u32)0x00000007 << 4)            /*!<R/W 'h1 N-OCD selection for all address/command pad, except CKE and CSN 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_ODT_SEL_0_CA_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_ODT_SEL_0_CA_TTCP_SEL  ((u32)0x00000007 << 0)            /*!<R/W 'h1 P-OCD selection for all address/command pad, except CKE and CSN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_ODT_SEL_0_CA_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CKE_ODT_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CKE_ODT_SEL_CKE_1_TTCN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h1 TTCN selection for CKE_1 (no use) 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_CKE_ODT_SEL_CKE_1_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_CKE_ODT_SEL_CKE_1_TTCP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h1 TTCP selection for CKE_1 (no use) 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CKE_ODT_SEL_CKE_1_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_CKE_ODT_SEL_CKE_TTCN_SEL    ((u32)0x00000007 << 4)             /*!<R/W 'h1 TTCN selection for CKE 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_CKE_ODT_SEL_CKE_TTCN_SEL(x)      ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_CKE_ODT_SEL_CKE_TTCP_SEL    ((u32)0x00000007 << 0)             /*!<R/W 'h1 TTCP selection for CKE 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CKE_ODT_SEL_CKE_TTCP_SEL(x)      ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_ODT_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_ODT_SEL_CS_1_TTCN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h1 TTCN selection for CS_1 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_ADR_ODT_SEL_CS_1_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_ADR_ODT_SEL_CS_1_TTCP_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h1 TTCP selection for CS_1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_ADR_ODT_SEL_CS_1_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_ADR_ODT_SEL_CS_TTCN_SEL    ((u32)0x00000007 << 12)            /*!<R/W 'h1 TTCN selection for CS 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_ADR_ODT_SEL_CS_TTCN_SEL(x)      ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_ADR_ODT_SEL_CS_TTCP_SEL    ((u32)0x00000007 << 8)             /*!<R/W 'h1 TTCP selection for CS 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_ADR_ODT_SEL_CS_TTCP_SEL(x)      ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_ADR_ODT_SEL_RST_TTCN_SEL   ((u32)0x00000007 << 4)             /*!<R/W 'h1 TTCN selection for RSTN 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_ADR_ODT_SEL_RST_TTCN_SEL(x)     ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_ADR_ODT_SEL_RST_TTCP_SEL   ((u32)0x00000007 << 0)             /*!<R/W 'h1 TTCP selection for RSTN 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_ADR_ODT_SEL_RST_TTCP_SEL(x)     ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CLK_ODT_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CLK_ODT_SEL_CK_1_TTCN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 TTCN selection for CLK_1 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_CLK_ODT_SEL_CK_1_TTCN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_CLK_ODT_SEL_CK_1_TTCP_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 TTCP selection for CLK_1 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CLK_ODT_SEL_CK_1_TTCP_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_CLK_ODT_SEL_CK_TTCN_SEL    ((u32)0x00000007 << 4)             /*!<R/W 'h0 TTCN selection for CLK 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_CLK_ODT_SEL_CK_TTCN_SEL(x)      ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_CLK_ODT_SEL_CK_TTCP_SEL    ((u32)0x00000007 << 0)             /*!<R/W 'h0 TTCP selection for CLK 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_CLK_ODT_SEL_CK_TTCP_SEL(x)      ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTRL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DPI_CTRL_0_WR_PRE_2T             ((u32)0x00000001 << 28)            /*!<R/W 'h0 [1] 2T preamble for DDR4 only(Write) [0] 1T preamble*/
#define DDRPHY_BIT_DPI_CTRL_0_DATA_PRE              ((u32)0x00000001 << 21)            /*!<R/W 'b1 1: Enable Tx DQ preamble, enable this function will add 2T write delay in 2to1 mode, or 1T write delay in 1to1 mode*/
#define DDRPHY_BIT_DPI_CTRL_0_DATA_INIT             ((u32)0x00000001 << 20)            /*!<R/W 'b1 Set the Tx initial value of DQ/DM*/
#define DDRPHY_BIT_DPI_CTRL_0_CAL_DQSEN_UPDATE_ENA  ((u32)0x00000001 << 19)            /*!<R/W 'b1 Enable HW auto-update tm_dqsen_ftun calibration value*/
#define DDRPHY_MASK_DPI_CTRL_0_CAL_DQSEN_MODE       ((u32)0x00000003 << 17)            /*!<R/W 'h3 tm_dqs_en calibration set mode [00] during no read operation [01] during refresh [10] [11] disable auto calibration*/
#define DDRPHY_DPI_CTRL_0_CAL_DQSEN_MODE(x)         ((u32)(((x) & 0x00000003) << 17))
#define DDRPHY_BIT_DPI_CTRL_0_RD_PRE_2T             ((u32)0x00000001 << 15)            /*!<R/W 'h0 [1] 2T preamble for DDR4 only (READ) [0] 1T preamble*/
#define DDRPHY_MASK_DPI_CTRL_0_REF_UPDATE_DLY       ((u32)0x0000003F << 8)             /*!<R/W 'h20 Update delay chain setting after receiving refresh command*/
#define DDRPHY_DPI_CTRL_0_REF_UPDATE_DLY(x)         ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_DPI_CTRL_0_RSVD_6_7             ((u32)0x00000003 << 6)             /*!< Reserved/undocumented bits cleared during RX-cal disable */
#define DDRPHY_MASK_DPI_CTRL_0_CAL_SET_MODE         ((u32)0x00000003 << 4)             /*!<R/W 'h0 3-point calibration set mode [00] during no read operation [01] during refresh [10] immediately [11] disable auto calibration*/
#define DDRPHY_DPI_CTRL_0_CAL_SET_MODE(x)           ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_MASK_DPI_CTRL_0_RST_FIFO_MODE        ((u32)0x00000003 << 2)             /*!<R/W 'h1 Reset read FIFO pointer mode [00] during no read operation [01] during refresh [10] disable reset FIFO pointer [11] force reset FIFO pointer*/
#define DDRPHY_DPI_CTRL_0_RST_FIFO_MODE(x)          ((u32)(((x) & 0x00000003) << 2))
#define DDRPHY_MASK_DPI_CTRL_0_FW_SET_MODE          ((u32)0x00000003 << 0)             /*!<R/W 'h0 FW set delay chain of data slice mode [00] READ:during no read; WRITE: during refresh [01] during refresh [10] immediately [11] reserved*/
#define DDRPHY_DPI_CTRL_0_FW_SET_MODE(x)            ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTRL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_4     ((u32)0x00000001 << 31)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_DPI_CTRL_1_SET_REG_VALID  ((u32)0x00000001 << 30)  /*!<R/W 'h0 Set dll/rw register flow is as 1st: set_reg_valid = 0 2nd: ser any other dll/rw register 3rd : set_reg_valid = 1*/
#define DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_1     ((u32)0x00000001 << 3)   /*!<W Enable of the following row*/
#define DDRPHY_BIT_DPI_CTRL_1_FW_SET_RD_DLY  ((u32)0x00000001 << 2)   /*!<W 'h0 FW set read delay chain TE and Delta of data slice (one cycle pulse, HW auto clear)*/
#define DDRPHY_BIT_DPI_CTRL_1_WRITE_EN_0     ((u32)0x00000001 << 1)   /*!<W Enable of the following row*/
#define DDRPHY_BIT_DPI_CTRL_1_FW_SET_WR_DLY  ((u32)0x00000001 << 0)   /*!<R/W 'h0 FW set write delay chain of data slice (one cycle pulse, HW auto clear)*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DPI_CTRL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DPI_CTRL_2_RST_DQSEN_CAL       ((u32)0x00000001 << 31)            /*!<R/W 'h1 1'b0: disable reset fw_tm_dqs_en_ftun when reset dqsen_gen 1'b1: enable reset fw_tm_dqs_en_ftun when reset dqsen_gen*/
#define DDRPHY_BIT_DPI_CTRL_2_SET_REG_VALID_MODE  ((u32)0x00000001 << 28)            /*!<R/W 'h1 [0] trigger set_reg_valid manually [1] trigger set_reg_valid automatically*/
#define DDRPHY_MASK_DPI_CTRL_2_RST_3POINT_MODE    ((u32)0x00000003 << 26)            /*!<R/W 'h0 Reset dqsen_gen mode [00] disable reset 3 point [01] force reset 3 point [10] reserved [11] during refresh*/
#define DDRPHY_DPI_CTRL_2_RST_3POINT_MODE(x)      ((u32)(((x) & 0x00000003) << 26))
#define DDRPHY_MASK_DPI_CTRL_2_RST_DQSEN_MODE     ((u32)0x00000003 << 24)            /*!<R/W 'h0 Reset dqsen_gen mode [00] disable reset RX dqsen_gen [01] force reset RX dqsen_gen [10] reserved [11] during refresh*/
#define DDRPHY_DPI_CTRL_2_RST_DQSEN_MODE(x)       ((u32)(((x) & 0x00000003) << 24))
#define DDRPHY_BIT_DPI_CTRL_2_ALERT_ENA           ((u32)0x00000001 << 23)            /*!<R/W 'b0 DFI alert output enable*/
#define DDRPHY_BIT_DPI_CTRL_2_ALERT_1_ENA         ((u32)0x00000001 << 22)            /*!<R/W 'b0 DFI alert_1 output enable*/
#define DDRPHY_MASK_DPI_CTRL_2_RD_UPDATE_DLY      ((u32)0x0000003F << 16)            /*!<R/W 'h1A Update delay chain setting after read data enable Cmd domain 2to1 mode : > 1/2 rd_update_dly_rx +N 1to1 mode : > rd_update_dly_rx +N*/
#define DDRPHY_DPI_CTRL_2_RD_UPDATE_DLY(x)        ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_DPI_CTRL_2_RD_UPDATE_DLY_RX   ((u32)0x0000007F << 8)             /*!<R/W 'h30 Update delay chain setting after read data enable Rx domain*/
#define DDRPHY_DPI_CTRL_2_RD_UPDATE_DLY_RX(x)     ((u32)(((x) & 0x0000007F) << 8))
#define DDRPHY_MASK_DPI_CTRL_2_PWROFF_CMD_GATE    ((u32)0x0000000F << 4)             /*!<R/W 'h0 1: Let the following signals always be 1. [0]: let dfi_wr_cmd_01 always be 1. [1]: let dfi_wr_cmd_23 always be 1. [2]: let dfi_rd_cmd_01 always be 1. [3]: let dfi_rd_cmd_23 always be 1.*/
#define DDRPHY_DPI_CTRL_2_PWROFF_CMD_GATE(x)      ((u32)(((x) & 0x0000000F) << 4))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_BIST_2TO1_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_BIST_2TO1_0_BIST_CA_XOR      ((u32)0x00000001 << 24)            /*!<R/W 'h0 XOR dfi_wrdata_p* with dfi C/A signals for controller loopback test dfi_wrdata_xor_p0[63:0] = dfi_wrdata_p0[63:0] ^ { 8'h0, dfi_ten_in_p0, dfi_parity_in_p0, dfi_act_n_p0, dfi_bg0_p0, ^dfi_odt_p0[1:0], ^dfi_cke_1_p0[1:0], ^dfi_cke_p0[1:0], ^dfi_cs_n_1_p0[1:0], ^dfi_cs_n_p0[1:0], ^dfi_we_n_p0[1:0], ^dfi_ras_n_p0[1:0], ^dfi_cas_n_p0[1:0], dfi_bank_p0[5:0], dfi_address_ext_p0[3:0], dfi_address_p0[33:0]}; dfi_wrdata_xor_p1[63:0] = dfi_wrdata_p1[63:0] ^ { 8'h0, dfi_ten_in_p1, dfi_parity_in_p1, dfi_act_n_p1, dfi_bg0_p1, ^dfi_odt_p1[1:0], ^dfi_cke_1_p1[1:0], ^dfi_cke_p1[1:0], ^dfi_cs_n_1_p1[1:0], ^dfi_cs_n_p1[1:0], ^dfi_we_n_p1[1:0], ^dfi_ras_n_p1[1:0], ^dfi_cas_n_p1[1:0], dfi_bank_p1[5:0], dfi_address_ext_p1[3:0], dfi_address_p1[33:0]};*/
#define DDRPHY_BIT_BIST_2TO1_0_BIST_EN          ((u32)0x00000001 << 23)            /*!<R/W 'h0 1Enable bist circuit in PHY 0Disable bist circuit & reset all flag of bist*/
#define DDRPHY_MASK_BIST_2TO1_0_BIST_MODE       ((u32)0x00000007 << 20)            /*!<R/W 'h0 Bist mode: 000 PRBS11 001 PRBS13 010 PRBS31*/
#define DDRPHY_BIST_2TO1_0_BIST_MODE(x)         ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_BIST_2TO1_0_BIST_DEBUG_SEL  ((u32)0x0000001F << 0)             /*!<R/W 'h00 debug selection*/
#define DDRPHY_BIST_2TO1_0_BIST_DEBUG_SEL(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_BIST_2TO1_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_BIST_2TO1_1_BIST_RDY         ((u32)0x00000001 << 5)            /*!<R 'h0 When bist is finished, then bist_rdy = 1 until bist_en = 0*/
#define DDRPHY_BIT_BIST_2TO1_1_BIST_FAIL_CMD    ((u32)0x00000001 << 4)            /*!<R 'h0 Reserved*/
#define DDRPHY_MASK_BIST_2TO1_1_BIST_FAIL_DATA  ((u32)0x0000000F << 0)            /*!<R 'h0 Fail flag for DDR PHY's data slice. If bist fail, then fail flag = 1 until bist_en = 0*/
#define DDRPHY_BIST_2TO1_1_BIST_FAIL_DATA(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_BIST_PT
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_BIST_PT_BIST_PATTERN  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 BIST initial pattern*/
#define DDRPHY_BIST_PT_BIST_PATTERN(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_BIST_DEBUG
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_BIST_DEBUG_BIST_DEBUG  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 BIST debug out*/
#define DDRPHY_BIST_DEBUG_BIST_DEBUG(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_INT_CTRL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_15      ((u32)0x00000001 << 31)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_VREF_DBG_EN  ((u32)0x00000001 << 30)  /*!<W 'h0 Vref debug INT enable*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_11      ((u32)0x00000001 << 23)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_ZQ_EN        ((u32)0x00000001 << 22)  /*!<R/W 'h0 Enable ZQ / RZQ done interrupt*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_8       ((u32)0x00000001 << 17)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_ALERT_EN     ((u32)0x00000001 << 16)  /*!<R/W 'h0 Enable alert_out interrupt*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_7       ((u32)0x00000001 << 15)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_PLL_EN       ((u32)0x00000001 << 14)  /*!<R/W 'h0 Enable PLL stable interrupt*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_6       ((u32)0x00000001 << 13)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_CPU_3_EN     ((u32)0x00000001 << 12)  /*!<R/W 'h0 To CPU 3 interrupt enable*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_5       ((u32)0x00000001 << 11)  /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_CPU_2_EN     ((u32)0x00000001 << 10)  /*!<R/W 'h0 To CPU 2 interrupt enable*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_4       ((u32)0x00000001 << 9)   /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_CPU_1_EN     ((u32)0x00000001 << 8)   /*!<R/W 'h0 To CPU 1 interrupt enable*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_3       ((u32)0x00000001 << 7)   /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_CPU_0_EN     ((u32)0x00000001 << 6)   /*!<R/W 'h0 To CPU 0 interrupt enable*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_2       ((u32)0x00000001 << 5)   /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_FW_CLR_DQS_INT   ((u32)0x00000001 << 4)   /*!<W 'h0 FW clear dqs_int & tm_dqsen_late_int & tm_dqsen_early_int (one cycle pulse, HW auto clear)*/
#define DDRPHY_BIT_INT_CTRL_WRITE_EN_0       ((u32)0x00000001 << 1)   /*!<W Enable of the following row*/
#define DDRPHY_BIT_INT_CTRL_INT_DQS_EN       ((u32)0x00000001 << 0)   /*!<R/W 'h0 dqs_int enable*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_INT_STATUS_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_INT_STATUS_0_VR_HIBDY_INT_3  ((u32)0x00000001 << 19)  /*!<R/W 'h0 Byte-3 Vref hit high boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_LOBDY_INT_3  ((u32)0x00000001 << 18)  /*!<R/W 'h0 Byte-3 Vref hit low boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_HIBDY_INT_2  ((u32)0x00000001 << 17)  /*!<R/W 'h0 Byte-2 Vref hit high boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_LOBDY_INT_2  ((u32)0x00000001 << 16)  /*!<R/W 'h0 Byte-2 Vref hit low boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_HIBDY_INT_1  ((u32)0x00000001 << 15)  /*!<R/W 'h0 Byte-1 Vref hit high boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_LOBDY_INT_1  ((u32)0x00000001 << 14)  /*!<R/W 'h0 Byte-1 Vref hit low boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_HIBDY_INT_0  ((u32)0x00000001 << 13)  /*!<R/W 'h0 Byte-0 Vref hit high boundary*/
#define DDRPHY_BIT_INT_STATUS_0_VR_LOBDY_INT_0  ((u32)0x00000001 << 12)  /*!<R/W 'h0 Byte-0 Vref hit low boundary*/
#define DDRPHY_BIT_INT_STATUS_0_DQS_INT_3       ((u32)0x00000001 << 3)   /*!<R/W 'h0 dqs_int of data slice3*/
#define DDRPHY_BIT_INT_STATUS_0_DQS_INT_2       ((u32)0x00000001 << 2)   /*!<R/W 'h0 dqs_int of data slice2*/
#define DDRPHY_BIT_INT_STATUS_0_DQS_INT_1       ((u32)0x00000001 << 1)   /*!<R/W 'h0 dqs_int of data slice1*/
#define DDRPHY_BIT_INT_STATUS_0_DQS_INT_0       ((u32)0x00000001 << 0)   /*!<R/W 'h0 dqs_int of data slice0*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_INT_STATUS_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_INT_STATUS_2_ALERT_TIE_HI  ((u32)0x00000001 << 13)  /*!<R/W 'h0 1'b1 means alert pad was tied to high*/
#define DDRPHY_BIT_INT_STATUS_2_ALERT_OUT_1   ((u32)0x00000001 << 12)  /*!<R/W 'h0 1'b1 means alert_out_1 goes low*/
#define DDRPHY_BIT_INT_STATUS_2_ALERT_OUT     ((u32)0x00000001 << 11)  /*!<R/W 'h0 1'b1 means alert_out_0 goes low*/
#define DDRPHY_BIT_INT_STATUS_2_RZQ_DONE      ((u32)0x00000001 << 10)  /*!<R/W 'h0 RZQ calibration done*/
#define DDRPHY_BIT_INT_STATUS_2_ZQ_DONE       ((u32)0x00000001 << 9)   /*!<R/W 'h0 ZQ calibration done*/
#define DDRPHY_BIT_INT_STATUS_2_PLL_STABLE    ((u32)0x00000001 << 0)   /*!<R/W 'h0 PLL stable*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_SSC_STATUS
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_SSC_STATUS_SSC_NCODE_DEBUG  ((u32)0x000000FF << 16)            /*!<R 'h00 Ncode of PLL using now, for debug*/
#define DDRPHY_SSC_STATUS_SSC_NCODE_DEBUG(x)    ((u32)(((x) & 0x000000FF) << 16))
#define DDRPHY_BIT_SSC_STATUS_SSC_FMOD          ((u32)0x00000001 << 13)            /*!<R 'h0 1 : under-clock 0 : over-clock*/
#define DDRPHY_BIT_SSC_STATUS_SSC_OC_DONE       ((u32)0x00000001 << 12)            /*!<R 'h0 Indicate over-clock or under-clock done*/
#define DDRPHY_MASK_SSC_STATUS_SSC_FCODE_DEBUG  ((u32)0x000007FF << 0)             /*!<R 'h000 Fcode of PLL using now, for debug*/
#define DDRPHY_SSC_STATUS_SSC_FCODE_DEBUG(x)    ((u32)(((x) & 0x000007FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PRBS_TMUX0_PRBS11_TEN      ((u32)0x00000001 << 31)            /*!<R/W 'h0 PRBS11 test enable*/
#define DDRPHY_MASK_PRBS_TMUX0_ODT_SEL        ((u32)0x00000007 << 28)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX0_ODT_SEL(x)          ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_BIT_PRBS_TMUX0_PRBS11_DUAL_EN  ((u32)0x00000001 << 27)            /*!<R/W 'h0 PRBS11 Dual edge enable*/
#define DDRPHY_MASK_PRBS_TMUX0_CAS_N_SEL      ((u32)0x00000007 << 24)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX0_CAS_N_SEL(x)        ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_BIT_PRBS_TMUX0_CKE_TEST        ((u32)0x00000001 << 23)            /*!<R/W 'h0 1: force 1 ,0: force 0*/
#define DDRPHY_MASK_PRBS_TMUX0_RAS_N_SEL      ((u32)0x00000007 << 20)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX0_RAS_N_SEL(x)        ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_BIT_PRBS_TMUX0_PRBS11_2T_EN    ((u32)0x00000001 << 19)            /*!<R/W 'h0 1: CMD 2T ,0: CMD 1T*/
#define DDRPHY_MASK_PRBS_TMUX0_WE_N_SEL       ((u32)0x00000007 << 16)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX0_WE_N_SEL(x)         ((u32)(((x) & 0x00000007) << 16))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX1_ADR07_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR07_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_PRBS_TMUX1_ADR06_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR06_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_PRBS_TMUX1_ADR05_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR05_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_PRBS_TMUX1_ADR04_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR04_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_PRBS_TMUX1_ADR03_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR03_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_PRBS_TMUX1_ADR02_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR02_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_PRBS_TMUX1_ADR01_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR01_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_PRBS_TMUX1_ADR00_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX1_ADR00_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX2_ADR15_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR15_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_PRBS_TMUX2_ADR14_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR14_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_PRBS_TMUX2_ADR13_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR13_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_PRBS_TMUX2_ADR12_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR12_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_PRBS_TMUX2_ADR11_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR11_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_PRBS_TMUX2_ADR10_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR10_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_PRBS_TMUX2_ADR09_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR09_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_PRBS_TMUX2_ADR08_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX2_ADR08_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX3_BA02_SEL       ((u32)0x00000007 << 28)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_BA02_SEL(x)         ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_PRBS_TMUX3_BA01_SEL       ((u32)0x00000007 << 24)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_BA01_SEL(x)         ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_PRBS_TMUX3_BA00_SEL       ((u32)0x00000007 << 20)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_BA00_SEL(x)         ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_PRBS_TMUX3_ADR05_EXT_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_ADR05_EXT_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_PRBS_TMUX3_ADR04_EXT_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_ADR04_EXT_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_PRBS_TMUX3_CS_N_SEL       ((u32)0x00000007 << 4)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_CS_N_SEL(x)         ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_PRBS_TMUX3_CS_N_1_SEL     ((u32)0x00000007 << 0)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX3_CS_N_1_SEL(x)       ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX6
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX6_DDR4_CS_N    ((u32)0x00000007 << 24)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX6_DDR4_CS_N(x)      ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_PRBS_TMUX6_ALERT_IN_1   ((u32)0x00000007 << 20)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX6_ALERT_IN_1(x)     ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_PRBS_TMUX6_PARITY_IN_1  ((u32)0x00000007 << 16)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX6_PARITY_IN_1(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_PRBS_TMUX6_PARITY_IN    ((u32)0x00000007 << 12)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX6_PARITY_IN(x)      ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_PRBS_TMUX6_TEN_IN       ((u32)0x00000007 << 4)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX6_TEN_IN(x)         ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_PRBS_TMUX6_ALERT_IN     ((u32)0x00000007 << 0)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX6_ALERT_IN(x)       ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX7
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX7_ADR16_SEL  ((u32)0x00000007 << 8)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX7_ADR16_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_PRBS_TMUX7_ACT_N      ((u32)0x00000007 << 4)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX7_ACT_N(x)        ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_PRBS_TMUX7_BG0        ((u32)0x00000007 << 0)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX7_BG0(x)          ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX8
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX8_TEST_PATTERN  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 32-bit test pattern for prbs11 or ten: [16: 0] : Addr[16:0] [18:17] : Ext_Addr[5:4] [21:19] : BA [22] : CS_n [23] : CS_n_1 [24] : RESET_n [25] : CKE [26] : ODT [27] : WE_n [28] : CAS_n [29] : RAS_n [30] : CKE_1*/
#define DDRPHY_PRBS_TMUX8_TEST_PATTERN(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX9
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PRBS_TMUX9_TEST4_PATTERN  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 32-bit test pattern for prbs11 or ten: [31:8] : reserved [07] : DDR4_CS_n [06] : ACT_n [05] : DDR4_CKE [04] : DDR4_BG0 [03] : PARITY_in [02] : PARITY_in_1 [01] : ALERT_in [00] : ALERT_in_1*/
#define DDRPHY_PRBS_TMUX9_TEST4_PATTERN(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX10
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PRBS_TMUX10_CS_PRBS11_INVS    ((u32)0x00000001 << 31)            /*!<R/W 'h0 inverted PRBS11 for CS*/
#define DDRPHY_MASK_PRBS_TMUX10_CS_PRBS11_SEED   ((u32)0x000007FF << 16)            /*!<R/W 'h001 PRBS11 seed for CS*/
#define DDRPHY_PRBS_TMUX10_CS_PRBS11_SEED(x)     ((u32)(((x) & 0x000007FF) << 16))
#define DDRPHY_BIT_PRBS_TMUX10_CMD_PRBS11_INVS   ((u32)0x00000001 << 15)            /*!<R/W 'h0 inverted PRBS11 for CMD/ADR*/
#define DDRPHY_MASK_PRBS_TMUX10_CMD_PRBS11_SEED  ((u32)0x000007FF << 0)             /*!<R/W 'h001 PRBS11 seed for CMD / ADR*/
#define DDRPHY_PRBS_TMUX10_CMD_PRBS11_SEED(x)    ((u32)(((x) & 0x000007FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DUMMY_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DUMMY_0_CK_SR_P    ((u32)0x00000007 << 20)          /*!<R/W 'h0 CK SR_P [2:0]*/
#define DDRPHY_DUMMY_0_CK_SR_P(x)      ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_DUMMY_0_CK_SR_N    ((u32)0x00000007 << 16)          /*!<R/W 'h0 CK SR_N [2:0]*/
#define DDRPHY_DUMMY_0_CK_SR_N(x)      ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_DUMMY_0_CS_SR_P    ((u32)0x00000007 << 12)          /*!<R/W 'h0 CS SR_P [2:0]*/
#define DDRPHY_DUMMY_0_CS_SR_P(x)      ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DUMMY_0_CS_SR_N    ((u32)0x00000007 << 8)           /*!<R/W 'h0 CS SR_N [2:0]*/
#define DDRPHY_DUMMY_0_CS_SR_N(x)      ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DUMMY_0_CA_SR_P    ((u32)0x00000007 << 4)           /*!<R/W 'h0 CA SR_P [2:0]*/
#define DDRPHY_DUMMY_0_CA_SR_P(x)      ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DUMMY_0_CA_SR_N    ((u32)0x00000007 << 0)           /*!<R/W 'h0 CA SR_N [2:0]*/
#define DDRPHY_DUMMY_0_CA_SR_N(x)      ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DUMMY_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DUMMY_1_DUMMY_1  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 dummy register*/
#define DDRPHY_DUMMY_1_DUMMY_1(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DUMMY_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DUMMY_2_DUMMY_2  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 dummy register*/
#define DDRPHY_DUMMY_2_DUMMY_2(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DUMMY_3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DUMMY_3_DUMMY_3    ((u32)0x0FFFFFFF << 4)            /*!<R/W 'h0000000 dummy register*/
#define DDRPHY_DUMMY_3_DUMMY_3(x)      ((u32)(((x) & 0x0FFFFFFF) << 4))
#define DDRPHY_MASK_DUMMY_3_SCAN_WRAP  ((u32)0x0000000F << 0)            /*!<R/W 'h0 dummy register for scan test coverage*/
#define DDRPHY_DUMMY_3_SCAN_WRAP(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CS_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CS_EMPH_CTL_CS1_SLEWRATE  ((u32)0x00000003 << 28)            /*!<R/W 'h0 CS1_n slew rate control pin*/
#define DDRPHY_CS_EMPH_CTL_CS1_SLEWRATE(x)    ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_CS_EMPH_CTL_CS1_DLY_EMPH  ((u32)0x0000000F << 24)            /*!<R/W 'h0 CS1_n Delay tap control pin (no use)*/
#define DDRPHY_CS_EMPH_CTL_CS1_DLY_EMPH(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_CS_EMPH_CTL_CS1_EN_EMPHN  ((u32)0x0000000F << 20)            /*!<R/W 'h7 CS1_n Emphasis control pin (no use)*/
#define DDRPHY_CS_EMPH_CTL_CS1_EN_EMPHN(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_CS_EMPH_CTL_CS1_EN_EMPHP  ((u32)0x0000000F << 16)            /*!<R/W 'h7 CS1_n Emphasis control pin (no use)*/
#define DDRPHY_CS_EMPH_CTL_CS1_EN_EMPHP(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_CS_EMPH_CTL_CS_SLEWRATE   ((u32)0x00000003 << 12)            /*!<R/W 'h0 CS_n slew rate control pin*/
#define DDRPHY_CS_EMPH_CTL_CS_SLEWRATE(x)     ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_CS_EMPH_CTL_CS_DLY_EMPH   ((u32)0x0000000F << 8)             /*!<R/W 'h0 CS_n Delay tap control pin (no use)*/
#define DDRPHY_CS_EMPH_CTL_CS_DLY_EMPH(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_CS_EMPH_CTL_CS_EN_EMPHN   ((u32)0x0000000F << 4)             /*!<R/W 'h7 CS_n Emphasis control pin (no use)*/
#define DDRPHY_CS_EMPH_CTL_CS_EN_EMPHN(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_CS_EMPH_CTL_CS_EN_EMPHP   ((u32)0x0000000F << 0)             /*!<R/W 'h7 CS_n Emphasis control pin (no use)*/
#define DDRPHY_CS_EMPH_CTL_CS_EN_EMPHP(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CS_SEL_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CS_SEL_EMPH_CTL_CS1_SEL_EMPHN  ((u32)0x0000000F << 20)            /*!<R/W 'h0 CS1_n SEL Emphasis control pin*/
#define DDRPHY_CS_SEL_EMPH_CTL_CS1_SEL_EMPHN(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_CS_SEL_EMPH_CTL_CS1_SEL_EMPHP  ((u32)0x0000000F << 16)            /*!<R/W 'h0 CS1_n SEL Emphasis control pin*/
#define DDRPHY_CS_SEL_EMPH_CTL_CS1_SEL_EMPHP(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_CS_SEL_EMPH_CTL_CS_SEL_EMPHN   ((u32)0x0000000F << 4)             /*!<R/W 'h0 CS_n SEL Emphasis control pin*/
#define DDRPHY_CS_SEL_EMPH_CTL_CS_SEL_EMPHN(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_CS_SEL_EMPH_CTL_CS_SEL_EMPHP   ((u32)0x0000000F << 0)             /*!<R/W 'h0 CS_n SEL Emphasis control pin*/
#define DDRPHY_CS_SEL_EMPH_CTL_CS_SEL_EMPHP(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DDR4_CS_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DDR4_CS_EMPH_CTL_CS_SEL_EMPHN  ((u32)0x0000000F << 20)            /*!<R/W 'h0 CS_n SEL Emphasis control pin*/
#define DDRPHY_DDR4_CS_EMPH_CTL_CS_SEL_EMPHN(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_DDR4_CS_EMPH_CTL_CS_SEL_EMPHP  ((u32)0x0000000F << 16)            /*!<R/W 'h0 CS_n SEL Emphasis control pin*/
#define DDRPHY_DDR4_CS_EMPH_CTL_CS_SEL_EMPHP(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_DDR4_CS_EMPH_CTL_CS_SLEWRATE   ((u32)0x00000003 << 12)            /*!<R/W 'h0 DDR4 CS_n slew rate control pin*/
#define DDRPHY_DDR4_CS_EMPH_CTL_CS_SLEWRATE(x)     ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_DDR4_CS_EMPH_CTL_CS_DLY_EMPH   ((u32)0x0000000F << 8)             /*!<R/W 'h0 DDR4 CS_n Delay tap control pin (no use)*/
#define DDRPHY_DDR4_CS_EMPH_CTL_CS_DLY_EMPH(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DDR4_CS_EMPH_CTL_CS_EN_EMPHN   ((u32)0x0000000F << 4)             /*!<R/W 'h7 DDR4 CS_n Emphasis control pin (no use)*/
#define DDRPHY_DDR4_CS_EMPH_CTL_CS_EN_EMPHN(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DDR4_CS_EMPH_CTL_CS_EN_EMPHP   ((u32)0x0000000F << 0)             /*!<R/W 'h7 DDR4 CS_n Emphasis control pin (no use)*/
#define DDRPHY_DDR4_CS_EMPH_CTL_CS_EN_EMPHP(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CK_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CK_EMPH_CTL_CK1_SLEWRATE  ((u32)0x00000003 << 28)            /*!<R/W 'h0 CK1 slew rate control pin*/
#define DDRPHY_CK_EMPH_CTL_CK1_SLEWRATE(x)    ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_CK_EMPH_CTL_CK1_DLY_EMPH  ((u32)0x0000000F << 24)            /*!<R/W 'h0 CK1 Delay tap control pin (no use)*/
#define DDRPHY_CK_EMPH_CTL_CK1_DLY_EMPH(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_CK_EMPH_CTL_CK1_EN_EMPHN  ((u32)0x0000000F << 20)            /*!<R/W 'h7 CK1 Emphasis control pin (no use)*/
#define DDRPHY_CK_EMPH_CTL_CK1_EN_EMPHN(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_CK_EMPH_CTL_CK1_EN_EMPHP  ((u32)0x0000000F << 16)            /*!<R/W 'h7 CK1 Emphasis control pin (no use)*/
#define DDRPHY_CK_EMPH_CTL_CK1_EN_EMPHP(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_CK_EMPH_CTL_CK_SLEWRATE   ((u32)0x00000003 << 12)            /*!<R/W 'h0 CK slew rate control pin*/
#define DDRPHY_CK_EMPH_CTL_CK_SLEWRATE(x)     ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_CK_EMPH_CTL_CK_DLY_EMPH   ((u32)0x0000000F << 8)             /*!<R/W 'h0 CK Delay tap control pin (no use)*/
#define DDRPHY_CK_EMPH_CTL_CK_DLY_EMPH(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_CK_EMPH_CTL_CK_EN_EMPHN   ((u32)0x0000000F << 4)             /*!<R/W 'h7 CK Emphasis control pin (no use)*/
#define DDRPHY_CK_EMPH_CTL_CK_EN_EMPHN(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_CK_EMPH_CTL_CK_EN_EMPHP   ((u32)0x0000000F << 0)             /*!<R/W 'h7 CK Emphasis control pin (no use)*/
#define DDRPHY_CK_EMPH_CTL_CK_EN_EMPHP(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CK_SEL_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CK_SEL_EMPH_CTL_CK1_SEL_EMPHN  ((u32)0x0000000F << 20)            /*!<R/W 'h0 CK1 SEL Emphasis control pin*/
#define DDRPHY_CK_SEL_EMPH_CTL_CK1_SEL_EMPHN(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_CK_SEL_EMPH_CTL_CK1_SEL_EMPHP  ((u32)0x0000000F << 16)            /*!<R/W 'h0 CK1 SEL Emphasis control pin*/
#define DDRPHY_CK_SEL_EMPH_CTL_CK1_SEL_EMPHP(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_CK_SEL_EMPH_CTL_CK_SEL_EMPHN   ((u32)0x0000000F << 4)             /*!<R/W 'h0 CK sel Emphasis control pin*/
#define DDRPHY_CK_SEL_EMPH_CTL_CK_SEL_EMPHN(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_CK_SEL_EMPH_CTL_CK_SEL_EMPHP   ((u32)0x0000000F << 0)             /*!<R/W 'h0 CK sel Emphasis control pin*/
#define DDRPHY_CK_SEL_EMPH_CTL_CK_SEL_EMPHP(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_SLEWRATE_1  ((u32)0x00000003 << 28)            /*!<R/W 'h0 ADR/CMD slew rate control pin*/
#define DDRPHY_ADR_EMPH_CTL_ADR_SLEWRATE_1(x)    ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_DLY_EMPH_1  ((u32)0x0000000F << 24)            /*!<R/W 'h0 ADR/CMD Delay tap control pin 1 (no use)*/
#define DDRPHY_ADR_EMPH_CTL_ADR_DLY_EMPH_1(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_EN_EMPHN_1  ((u32)0x0000000F << 20)            /*!<R/W 'h7 ADR/CMD Emphasis control pin 1 (no use)*/
#define DDRPHY_ADR_EMPH_CTL_ADR_EN_EMPHN_1(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_EN_EMPHP_1  ((u32)0x0000000F << 16)            /*!<R/W 'h7 ADR/CMD Emphasis control pin 1 (no use)*/
#define DDRPHY_ADR_EMPH_CTL_ADR_EN_EMPHP_1(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_SLEWRATE_0  ((u32)0x00000003 << 12)            /*!<R/W 'h0 ADR/CMD slew rate control pin 0*/
#define DDRPHY_ADR_EMPH_CTL_ADR_SLEWRATE_0(x)    ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_DLY_EMPH_0  ((u32)0x0000000F << 8)             /*!<R/W 'h0 ADR/CMD Delay tap control pin 0 (no use)*/
#define DDRPHY_ADR_EMPH_CTL_ADR_DLY_EMPH_0(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_EN_EMPHN_0  ((u32)0x0000000F << 4)             /*!<R/W 'h7 ADR/CMD Emphasis control pin 0 (no use)*/
#define DDRPHY_ADR_EMPH_CTL_ADR_EN_EMPHN_0(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_ADR_EMPH_CTL_ADR_EN_EMPHP_0  ((u32)0x0000000F << 0)             /*!<R/W 'h7 ADR/CMD Emphasis control pin 0 (no use)*/
#define DDRPHY_ADR_EMPH_CTL_ADR_EN_EMPHP_0(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_SEL_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHN_1  ((u32)0x0000000F << 20)            /*!<R/W 'h0 ADR/CMD Emphasis control pin 1*/
#define DDRPHY_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHN_1(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHP_1  ((u32)0x0000000F << 16)            /*!<R/W 'h0 ADR/CMD Emphasis control pin 1*/
#define DDRPHY_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHP_1(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHN_0  ((u32)0x0000000F << 4)             /*!<R/W 'h0 ADR/CMD Emphasis control pin 0*/
#define DDRPHY_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHN_0(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHP_0  ((u32)0x0000000F << 0)             /*!<R/W 'h0 ADR/CMD Emphasis control pin 0*/
#define DDRPHY_ADR_SEL_EMPH_CTL_ADR_SEL_EMPHP_0(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_EMPH_SEL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A15_EMPH_SEL  ((u32)0x00000003 << 30)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A15 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A15_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 30))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A14_EMPH_SEL  ((u32)0x00000003 << 28)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A14 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A14_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A13_EMPH_SEL  ((u32)0x00000003 << 26)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A13 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A13_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 26))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A12_EMPH_SEL  ((u32)0x00000003 << 24)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A12 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A12_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 24))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A11_EMPH_SEL  ((u32)0x00000003 << 22)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A11 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A11_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 22))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A10_EMPH_SEL  ((u32)0x00000003 << 20)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A10 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A10_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 20))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A9_EMPH_SEL   ((u32)0x00000003 << 18)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A9 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A9_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 18))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A8_EMPH_SEL   ((u32)0x00000003 << 16)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A8 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A8_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 16))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A7_EMPH_SEL   ((u32)0x00000003 << 14)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A7 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A7_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 14))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A6_EMPH_SEL   ((u32)0x00000003 << 12)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A6 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A6_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A5_EMPH_SEL   ((u32)0x00000003 << 10)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A5 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A5_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 10))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A4_EMPH_SEL   ((u32)0x00000003 << 8)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A4 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A4_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A3_EMPH_SEL   ((u32)0x00000003 << 6)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A3 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A3_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 6))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A2_EMPH_SEL   ((u32)0x00000003 << 4)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A2 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A2_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A1_EMPH_SEL   ((u32)0x00000003 << 2)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A1 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A1_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 2))
#define DDRPHY_MASK_ADR_EMPH_SEL_0_A0_EMPH_SEL   ((u32)0x00000003 << 0)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A0 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_0_A0_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_EMPH_SEL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_EMPH_SEL_1_ACT_EMPH_SEL    ((u32)0x00000003 << 30)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_ACTN 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_ACT_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 30))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_RST_EMPH_SEL    ((u32)0x00000003 << 28)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_RSTN 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_RST_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_WE_EMPH_SEL     ((u32)0x00000003 << 26)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_WEN 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_WE_EMPH_SEL(x)       ((u32)(((x) & 0x00000003) << 26))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_RAS_EMPH_SEL    ((u32)0x00000003 << 24)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_RASN 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_RAS_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 24))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_PAR1_EMPH_SEL   ((u32)0x00000003 << 22)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR4_PAR_1 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_PAR1_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 22))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_PAR_EMPH_SEL    ((u32)0x00000003 << 20)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR4_PAR 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_PAR_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 20))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_ODT_EMPH_SEL    ((u32)0x00000003 << 18)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_ODT 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_ODT_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 18))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_CKE1_EMPH_SEL   ((u32)0x00000003 << 16)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_CKE_1 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_CKE1_EMPH_SEL(x)     ((u32)(((x) & 0x00000003) << 16))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_CKE_EMPH_SEL    ((u32)0x00000003 << 14)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_CKE 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_CKE_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 14))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_CAS_EMPH_SEL    ((u32)0x00000003 << 12)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_CASN 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_CAS_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_BA2_EMPH_SEL    ((u32)0x00000003 << 10)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_BA2 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_BA2_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 10))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_BA1_EMPH_SEL    ((u32)0x00000003 << 8)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_BA1 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_BA1_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_BA0_EMPH_SEL    ((u32)0x00000003 << 6)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_BA0 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_BA0_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 6))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_AD1_5_EMPH_SEL  ((u32)0x00000003 << 4)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_AD1_5 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_AD1_5_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_AD1_4_EMPH_SEL  ((u32)0x00000003 << 2)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_AD1_4 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_AD1_4_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 2))
#define DDRPHY_MASK_ADR_EMPH_SEL_1_A16_EMPH_SEL    ((u32)0x00000003 << 0)             /*!<R/W 'h0 Tx emphasis control of PAD_DDR_A16 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_1_A16_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ADR_EMPH_SEL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ADR_EMPH_SEL_2_ALERT1_EMPH_SEL    ((u32)0x00000003 << 8)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_ALERT_1 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_2_ALERT1_EMPH_SEL(x)      ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_ADR_EMPH_SEL_2_ALERT_EMPH_SEL     ((u32)0x00000003 << 6)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR_ALERT 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_2_ALERT_EMPH_SEL(x)       ((u32)(((x) & 0x00000003) << 6))
#define DDRPHY_MASK_ADR_EMPH_SEL_2_DDR4_TEN_EMPH_SEL  ((u32)0x00000003 << 4)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR4_TEN 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_2_DDR4_TEN_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 4))
#define DDRPHY_MASK_ADR_EMPH_SEL_2_DDR4_CKE_EMPH_SEL  ((u32)0x00000003 << 2)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR4_CKE 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_2_DDR4_CKE_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 2))
#define DDRPHY_MASK_ADR_EMPH_SEL_2_DDR4_BG0_EMPH_SEL  ((u32)0x00000003 << 0)            /*!<R/W 'h0 Tx emphasis control of PAD_DDR4_BG0 00: use ADR/CMD emphasis control set 0 01: use ADR/CMD emphasis control set 1 10: use ADR/CMD emphasis control set 2 11: use ADR/CMD emphasis control set 3*/
#define DDRPHY_ADR_EMPH_SEL_2_DDR4_BG0_EMPH_SEL(x)    ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DBG_READ_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DBG_READ_1_DPI_DBG_DLL  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Use dpi_dbg_dll_sel to select dpi_debug_dll[31:0] or dpi_debug_dll[63:32]*/
#define DDRPHY_DBG_READ_1_DPI_DBG_DLL(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RX_CAL_CTRL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RX_CAL_CTRL_0_VR_TARGET_3  ((u32)0x0000000F << 28)            /*!<R/W 'h5 Vref calibration process will stop if the minimum delta width is greater than or equal to this value. This is for byte-3 Vref.*/
#define DDRPHY_RX_CAL_CTRL_0_VR_TARGET_3(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_RX_CAL_CTRL_0_VR_TARGET_2  ((u32)0x0000000F << 24)            /*!<R/W 'h5 Vref calibration process will stop if the minimum delta width is greater than or equal to this value. This is for byte-2 Vref.*/
#define DDRPHY_RX_CAL_CTRL_0_VR_TARGET_2(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_RX_CAL_CTRL_0_VR_TARGET_1  ((u32)0x0000000F << 20)            /*!<R/W 'h5 Vref calibration process will stop if the minimum delta width is greater than or equal to this value. This is for byte-1 Vref.*/
#define DDRPHY_RX_CAL_CTRL_0_VR_TARGET_1(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_RX_CAL_CTRL_0_VR_TARGET_0  ((u32)0x0000000F << 16)            /*!<R/W 'h5 Vref calibration process will stop if the minimum delta width is greater than or equal to this value. This is for byte-0 Vref.*/
#define DDRPHY_RX_CAL_CTRL_0_VR_TARGET_0(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_RX_CAL_CTRL_0_VR_SRC_SEL   ((u32)0x00000007 << 12)            /*!<R/W 'h0 Select data source of Vref update criteria*/
#define DDRPHY_RX_CAL_CTRL_0_VR_SRC_SEL(x)     ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_RX_CAL_CTRL_0_CAL_OFFLINE  ((u32)0x00000003 << 8)             /*!<R/W 'h0 2'b00: Disable Rx off-line calibration. 2'b01: HW moves TE when pre-hit or post-hit occurs, and Delta is decreased only . This mode should only be used in offline or fore-ground calibration to find the minimum usable Rx delay taps. 2'b11: HW automatically moves TE to the middle of delay taps, and adjusts Delta value when left and right Delta value are not equivalent. If set this to 2'b01 or 2'b11, pre-delta values are controlled by DEL_POS_SEL, post-delta values are controlled by DEL_POS_SEL_RIGHT registers.*/
#define DDRPHY_RX_CAL_CTRL_0_CAL_OFFLINE(x)    ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_BIT_RX_CAL_CTRL_0_DELTA_FORCE   ((u32)0x00000001 << 5)             /*!<R/W 'h0 force delta value without double-buffer*/
#define DDRPHY_BIT_RX_CAL_CTRL_0_VR_UPD_3      ((u32)0x00000001 << 3)             /*!<R/W 'h1 1: vref_s_3 can be applied to Vref pad by fw_set_rd_dly*/
#define DDRPHY_BIT_RX_CAL_CTRL_0_VR_UPD_2      ((u32)0x00000001 << 2)             /*!<R/W 'h1 1: vref_s_2 can be applied to Vref pad by fw_set_rd_dly*/
#define DDRPHY_BIT_RX_CAL_CTRL_0_VR_UPD_1      ((u32)0x00000001 << 1)             /*!<R/W 'h1 1: vref_s_1 can be applied to Vref pad by fw_set_rd_dly*/
#define DDRPHY_BIT_RX_CAL_CTRL_0_VR_UPD_0      ((u32)0x00000001 << 0)             /*!<R/W 'h1 1: vref_s_0 can be applied to Vref pad by fw_set_rd_dly*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RX_CAL_CTRL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RX_CAL_CTRL_1_VR_CNT_TARGET  ((u32)0x0000003F << 0)            /*!<R/W 'h00 update Vref when target count is reached 0 : disable Vref calibration*/
#define DDRPHY_RX_CAL_CTRL_1_VR_CNT_TARGET(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_VREF_BDY_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_VREF_BDY_0_VR_HI_BDY_1  ((u32)0x0000003F << 24)            /*!<R/W 'h26 Vref1 high boundary for rw_blk_01*/
#define DDRPHY_VREF_BDY_0_VR_HI_BDY_1(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_VREF_BDY_0_VR_LO_BDY_1  ((u32)0x0000003F << 16)            /*!<R/W 'h04 Vref1 low boundary for rw_blk_01*/
#define DDRPHY_VREF_BDY_0_VR_LO_BDY_1(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_VREF_BDY_0_VR_HI_BDY_0  ((u32)0x0000003F << 8)             /*!<R/W 'h26 Vref0 high boundary for rw_blk_01*/
#define DDRPHY_VREF_BDY_0_VR_HI_BDY_0(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_VREF_BDY_0_VR_LO_BDY_0  ((u32)0x0000003F << 0)             /*!<R/W 'h04 Vref0 low boundary for rw_blk_01*/
#define DDRPHY_VREF_BDY_0_VR_LO_BDY_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_VREF_BDY_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_VREF_BDY_1_VR_HI_BDY_3  ((u32)0x0000003F << 24)            /*!<R/W 'h26 Vref3 high boundary for rw_blk_23*/
#define DDRPHY_VREF_BDY_1_VR_HI_BDY_3(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_VREF_BDY_1_VR_LO_BDY_3  ((u32)0x0000003F << 16)            /*!<R/W 'h04 Vref3 low boundary for rw_blk_23*/
#define DDRPHY_VREF_BDY_1_VR_LO_BDY_3(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_VREF_BDY_1_VR_HI_BDY_2  ((u32)0x0000003F << 8)             /*!<R/W 'h26 Vref2 high boundary for rw_blk_23*/
#define DDRPHY_VREF_BDY_1_VR_HI_BDY_2(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_VREF_BDY_1_VR_LO_BDY_2  ((u32)0x0000003F << 0)             /*!<R/W 'h04 Vref2 low boundary for rw_blk_23*/
#define DDRPHY_VREF_BDY_1_VR_LO_BDY_2(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RX_CAL_MON
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RX_CAL_MON_VREF_S_3  ((u32)0x0000003F << 24)            /*!<R 'h0A calibrated Vref value of byte-3*/
#define DDRPHY_RX_CAL_MON_VREF_S_3(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_RX_CAL_MON_VREF_S_2  ((u32)0x0000003F << 16)            /*!<R 'h0A calibrated Vref value of byte-2*/
#define DDRPHY_RX_CAL_MON_VREF_S_2(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_RX_CAL_MON_VREF_S_1  ((u32)0x0000003F << 8)             /*!<R 'h0A calibrated Vref value of byte-1*/
#define DDRPHY_RX_CAL_MON_VREF_S_1(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_RX_CAL_MON_VREF_S_0  ((u32)0x0000003F << 0)             /*!<R 'h0A calibrated Vref value of byte-0*/
#define DDRPHY_RX_CAL_MON_VREF_S_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_CTRL_ZPROG
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_CTRL_ZPROG_ZPROG_3  ((u32)0x00000007 << 24)            /*!<R/W 'h0 NODT vpcal_sel*/
#define DDRPHY_PAD_CTRL_ZPROG_ZPROG_3(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_PAD_CTRL_ZPROG_ZPROG_2  ((u32)0x0000003F << 16)            /*!<R/W 'h00 NODT pad pocd value*/
#define DDRPHY_PAD_CTRL_ZPROG_ZPROG_2(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_PAD_CTRL_ZPROG_ZPROG_1  ((u32)0x00000007 << 8)             /*!<R/W 'h0 NOCD vpcal_sel*/
#define DDRPHY_PAD_CTRL_ZPROG_ZPROG_1(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_PAD_CTRL_ZPROG_ZPROG_0  ((u32)0x0000003F << 0)             /*!<R/W 'h00 NOCD pad pocd value*/
#define DDRPHY_PAD_CTRL_ZPROG_ZPROG_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PAD_NOCD2_ZPROG
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1  ((u32)0x00000007 << 8)            /*!<R/W 'h0 NOCD2 vpcal_sel*/
#define DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_1(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0  ((u32)0x0000003F << 0)            /*!<R/W 'h00 NOCD2 pad pocd value*/
#define DDRPHY_PAD_NOCD2_ZPROG_NOCD2_ZPROG_0(x)    ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_OCD_SEL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_7_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h0 Register Set 0: NT2 selection for slice3 DQ Register Set 1: EN_EMPHN selection for slice3 DQ 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_7_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_6_OCDN_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h0 Register Set 0: NT2 selection for slice2 DQ Register Set 1: EN_EMPHN selection for slice2 DQ 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_6_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_7_OCDP_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h0 Register Set 0: PT2 selection for slice3 DQ Register Set 1: EN_EMPHP selection for slice3 DQ 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_7_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_6_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 Register Set 0: PT2 selection for slice2 DQ Register Set 1: EN_EMPHP selection for slice2 DQ 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_6_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_5_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 Register Set 0: NT2 selection for slice1 DQ Register Set 1: EN_EMPHN selection for slice1 DQ 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_5_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_4_OCDN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 Register Set 0: NT2 selection for slice0 DQ Register Set 1: EN_EMPHN selection for slice0 DQ 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_4_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_5_OCDP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 Register Set 0: PT2 selection for slice1 DQ Register Set 1: EN_EMPHP selection for slice1 DQ 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_5_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQ_OCD_SEL_2_DQ_4_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 Register Set 0: PT2 selection for slice0 DQ Register Set 1: EN_EMPHP selection for slice0 DQ 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQ_OCD_SEL_2_DQ_4_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_OCD_SEL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_7_OCDN_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h0 Set 0: NT2 selection for DQS[3] Set 1: EN_EMPHN selection for DQS[3] 0000: ocdn_set_0 ... 1011: ocdn_set_11 bit3: *_ocd*_sel_ex*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_7_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_6_OCDN_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h0 Set 0: NT2 selection for DQS[2] Set 1: EN_EMPHN selection for DQS[2] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_6_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_7_OCDP_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h0 Set 0: PT2 selection for DQS[3] Set 1: EN_EMPHP selection for DQS[3] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_7_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_6_OCDP_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 Set 0: PT2 selection for DQS[2] Set 1: EN_EMPHP selection for DQS[2] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_6_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_5_OCDN_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 Set 0: NT2 selection for DQS[1] Set 1: EN_EMPHN selection for DQS[1] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_5_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_4_OCDN_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 Set 0: NT2 selection for DQS[0] Set 1: EN_EMPHN selection for DQS[0] 0000: ocdn_set_0 ... 1011: ocdn_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_4_OCDN_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_5_OCDP_SEL  ((u32)0x00000007 << 4)             /*!<R/W 'h0 Set 0: PT2 selection for DQS[1] Set 1: EN_EMPHP selection for DQS[1] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_5_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_DQS_OCD_SEL_2_DQS_4_OCDP_SEL  ((u32)0x00000007 << 0)             /*!<R/W 'h0 Set 0: PT2 selection for DQS[0] Set 1: EN_EMPHP selection for DQS[0] 0000: ocdp_set_0 ... 1011: ocdp_set_11*/
#define DDRPHY_DQS_OCD_SEL_2_DQS_4_OCDP_SEL(x)    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DBG_READ_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DBG_READ_2_DATA_EN_ERR_TYPE  ((u32)0x0000FFFF << 0)            /*!<R 'h0000 [1:0] check result of dfi_wrdata_en_p0/p1[0] [3:2] check result of dfi_wrdata_en_p0/p1[1] [5:4] check result of dfi_wrdata_en_p0/p1[2] [7:6] check result of dfi_wrdata_en_p0/p1[3] [9:8] check result of dfi_rddata_en_p0/p1[0] [11:10] check result of dfi_rddata_en_p0/p1[1] [13:12] check result of dfi_rddata_en_p0/p1[2] [15:14] check result of dfi_rddata_en_p0/p1[3] 2'b00: No error; 2'b01, 2'b10: Error happened in dfi_rddata_en/dfi_wrdata_en.*/
#define DDRPHY_DBG_READ_2_DATA_EN_ERR_TYPE(x)    ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DATA_VAL_CTRL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DATA_VAL_CTRL_0_RDDATA_DBI_VAL   ((u32)0x0000000F << 8)            /*!<R/W 'h0 Only available if rddata_force = 1. [3:0]: Control rddata_dbi value of read slice 3/2/1/0.*/
#define DDRPHY_DATA_VAL_CTRL_0_RDDATA_DBI_VAL(x)     ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DATA_VAL_CTRL_0_WRDATA_MASK_VAL  ((u32)0x0000000F << 4)            /*!<R/W 'h0 Only available if wrdata_force = 1. [3:0]: Control wrdata_mask value of write slice 3/2/1/0.*/
#define DDRPHY_DATA_VAL_CTRL_0_WRDATA_MASK_VAL(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_BIT_DATA_VAL_CTRL_0_RDDATA_FORCE      ((u32)0x00000001 << 1)            /*!<R/W 'b0 1: Force the value of dfi_rddata_wN and dfi_rddata_dbi_wN to rddata_val and rddata_dbi_val.*/
#define DDRPHY_BIT_DATA_VAL_CTRL_0_WRDATA_FORCE      ((u32)0x00000001 << 0)            /*!<R/W 'b0 1: Force the value of dfi_wrdata_pN and dfi_wrdata_mask_pN to wrdata_val and wrdata_mask_val.*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DATA_VAL_CTRL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DATA_VAL_CTRL_1_WRDATA_VAL  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 Only available if wrdata_force = 1. [ 7: 0]: Control wrdata of write slice 0 [15:8]: Control wrdata of write slice 1 [23:16]: Control wrdata of write slice 2 [31:24]: Control wrdata of write slice 3*/
#define DDRPHY_DATA_VAL_CTRL_1_WRDATA_VAL(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DATA_VAL_CTRL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DATA_VAL_CTRL_2_RDDATA_VAL  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 Only available if rddata_force = 1. [ 7: 0]: Control rddata of read slice 0 [15:8]: Control rddata of read slice 1 [23:16]: Control rddata of read slice 2 [31:24]: Control rddata of read slice 3*/
#define DDRPHY_DATA_VAL_CTRL_2_RDDATA_VAL(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CA_CRC_CTRL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_CA_CRC_CTRL_CLR_CRC_OUT   ((u32)0x00000001 << 3)  /*!<R/W 'h0 1: clear ca_crc_out.*/
#define DDRPHY_BIT_CA_CRC_CTRL_REF_CMD_FILT  ((u32)0x00000001 << 2)  /*!<R/W 'h0 1: filter out REFERESH DFI signals after dfi_ref_cmd goes high*/
#define DDRPHY_BIT_CA_CRC_CTRL_CS_POLARITY   ((u32)0x00000001 << 1)  /*!<R/W 'h0 0: dfi_cs_n is low active; 1: dfi_cs_n is high active for CA CRC checking*/
#define DDRPHY_BIT_CA_CRC_CTRL_CA_CRC_EN     ((u32)0x00000001 << 0)  /*!<R/W 'h0 1: Enable DFI CA signals CRC checking function*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CA_CRC_OUT
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CA_CRC_OUT_CA_CRC_OUT  ((u32)0xFFFFFFFF << 0)            /*!<R 'hFFFFFFFF [8:0] CRC result of lower half of DFI CA p0 [15:9] CRC result of higher half of DFI CA p0 [23:16] CRC result of lower half of DFI CA p1 [31:24] CRC result of higher half of DFI CA p1*/
#define DDRPHY_CA_CRC_OUT_CA_CRC_OUT(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCP2_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCP2_SET0_ODT_TTCP_SET_11_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h00 P-ODT coarse trimming set_11.*/
#define DDRPHY_ODT_TTCP2_SET0_ODT_TTCP_SET_11_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCP2_SET0_ODT_TTCP_SET_10_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h00 P-ODT coarse trimming set_10.*/
#define DDRPHY_ODT_TTCP2_SET0_ODT_TTCP_SET_10_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCP2_SET0_ODT_TTCP_SET_9_PRE   ((u32)0x0000003F << 8)             /*!<R/W 'h00 P-ODT coarse trimming set_9.*/
#define DDRPHY_ODT_TTCP2_SET0_ODT_TTCP_SET_9_PRE(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCP2_SET0_ODT_TTCP_SET_8_PRE   ((u32)0x0000003F << 0)             /*!<R/W 'h00 P-ODT coarse trimming set_8.*/
#define DDRPHY_ODT_TTCP2_SET0_ODT_TTCP_SET_8_PRE(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCN2_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCN2_SET0_ODT_TTCN_SET_11_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h00 N-ODT coarse trimming set_7*/
#define DDRPHY_ODT_TTCN2_SET0_ODT_TTCN_SET_11_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCN2_SET0_ODT_TTCN_SET_10_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h00 N-ODT coarse trimming set_6*/
#define DDRPHY_ODT_TTCN2_SET0_ODT_TTCN_SET_10_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCN2_SET0_ODT_TTCN_SET_9_PRE   ((u32)0x0000003F << 8)             /*!<R/W 'h00 N-ODT coarse trimming set_5*/
#define DDRPHY_ODT_TTCN2_SET0_ODT_TTCN_SET_9_PRE(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCN2_SET0_ODT_TTCN_SET_8_PRE   ((u32)0x0000003F << 0)             /*!<R/W 'h00 N-ODT coarse trimming set_4*/
#define DDRPHY_ODT_TTCN2_SET0_ODT_TTCN_SET_8_PRE(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDP2_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDP2_SET0_OCDP_SET_11_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h26 P-OCD trimming set_11*/
#define DDRPHY_OCDP2_SET0_OCDP_SET_11_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDP2_SET0_OCDP_SET_10_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h26 P-OCD trimming set_10*/
#define DDRPHY_OCDP2_SET0_OCDP_SET_10_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDP2_SET0_OCDP_SET_9_PRE   ((u32)0x0000003F << 8)             /*!<R/W 'h26 P-OCD trimming set_9*/
#define DDRPHY_OCDP2_SET0_OCDP_SET_9_PRE(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDP2_SET0_OCDP_SET_8_PRE   ((u32)0x0000003F << 0)             /*!<R/W 'h26 P-OCD trimming set_8*/
#define DDRPHY_OCDP2_SET0_OCDP_SET_8_PRE(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDN2_SET0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDN2_SET0_OCDN_SET_11_PRE  ((u32)0x0000003F << 24)            /*!<R/W 'h26 N-OCD trimming set_11*/
#define DDRPHY_OCDN2_SET0_OCDN_SET_11_PRE(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDN2_SET0_OCDN_SET_10_PRE  ((u32)0x0000003F << 16)            /*!<R/W 'h26 N-OCD trimming set_10*/
#define DDRPHY_OCDN2_SET0_OCDN_SET_10_PRE(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDN2_SET0_OCDN_SET_9_PRE   ((u32)0x0000003F << 8)             /*!<R/W 'h26 N-OCD trimming set_9*/
#define DDRPHY_OCDN2_SET0_OCDN_SET_9_PRE(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDN2_SET0_OCDN_SET_8_PRE   ((u32)0x0000003F << 0)             /*!<R/W 'h26 N-OCD trimming set_8*/
#define DDRPHY_OCDN2_SET0_OCDN_SET_8_PRE(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCP2_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCP2_SET1_ODT_TTCP_SET_11  ((u32)0x0000003F << 24)            /*!<R/W 'h00 P-ODT coarse trimming set_11*/
#define DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_11(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCP2_SET1_ODT_TTCP_SET_10  ((u32)0x0000003F << 16)            /*!<R/W 'h00 P-ODT coarse trimming set_10*/
#define DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_10(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCP2_SET1_ODT_TTCP_SET_9   ((u32)0x0000003F << 8)             /*!<R/W 'h00 P-ODT coarse trimming set_9*/
#define DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_9(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCP2_SET1_ODT_TTCP_SET_8   ((u32)0x0000003F << 0)             /*!<R/W 'h00 P-ODT coarse trimming set_8*/
#define DDRPHY_ODT_TTCP2_SET1_ODT_TTCP_SET_8(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_TTCN2_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_ODT_TTCN2_SET1_ODT_TTCN_SET_11  ((u32)0x0000003F << 24)            /*!<R/W 'h00 N-ODT coarse trimming set_11*/
#define DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_11(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_ODT_TTCN2_SET1_ODT_TTCN_SET_10  ((u32)0x0000003F << 16)            /*!<R/W 'h00 N-ODT coarse trimming set_10*/
#define DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_10(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_ODT_TTCN2_SET1_ODT_TTCN_SET_9   ((u32)0x0000003F << 8)             /*!<R/W 'h00 N-ODT coarse trimming set_9*/
#define DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_9(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_ODT_TTCN2_SET1_ODT_TTCN_SET_8   ((u32)0x0000003F << 0)             /*!<R/W 'h00 N-ODT coarse trimming set_8*/
#define DDRPHY_ODT_TTCN2_SET1_ODT_TTCN_SET_8(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDP2_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDP2_SET1_OCDP_SET_11  ((u32)0x0000003F << 24)            /*!<R/W 'h26 P-OCD trimming set_11*/
#define DDRPHY_OCDP2_SET1_OCDP_SET_11(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDP2_SET1_OCDP_SET_10  ((u32)0x0000003F << 16)            /*!<R/W 'h26 P-OCD trimming set_10*/
#define DDRPHY_OCDP2_SET1_OCDP_SET_10(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDP2_SET1_OCDP_SET_9   ((u32)0x0000003F << 8)             /*!<R/W 'h26 P-OCD trimming set_9*/
#define DDRPHY_OCDP2_SET1_OCDP_SET_9(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDP2_SET1_OCDP_SET_8   ((u32)0x0000003F << 0)             /*!<R/W 'h26 P-OCD trimming set_8*/
#define DDRPHY_OCDP2_SET1_OCDP_SET_8(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCDN2_SET1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_OCDN2_SET1_OCDN_SET_11  ((u32)0x0000003F << 24)            /*!<R/W 'h26 N-OCD trimming set_11*/
#define DDRPHY_OCDN2_SET1_OCDN_SET_11(x)    ((u32)(((x) & 0x0000003F) << 24))
#define DDRPHY_MASK_OCDN2_SET1_OCDN_SET_10  ((u32)0x0000003F << 16)            /*!<R/W 'h26 N-OCD trimming set_10*/
#define DDRPHY_OCDN2_SET1_OCDN_SET_10(x)    ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_OCDN2_SET1_OCDN_SET_9   ((u32)0x0000003F << 8)             /*!<R/W 'h26 N-OCD trimming set_9*/
#define DDRPHY_OCDN2_SET1_OCDN_SET_9(x)     ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_OCDN2_SET1_OCDN_SET_8   ((u32)0x0000003F << 0)             /*!<R/W 'h26 N-OCD trimming set_8*/
#define DDRPHY_OCDN2_SET1_OCDN_SET_8(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_SEL_EX_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_3_TTCN_SEL_EX  ((u32)0x00000001 << 23)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_3_TTCP_SEL_EX  ((u32)0x00000001 << 22)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_2_TTCN_SEL_EX  ((u32)0x00000001 << 21)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_2_TTCP_SEL_EX  ((u32)0x00000001 << 20)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_1_TTCN_SEL_EX  ((u32)0x00000001 << 19)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_1_TTCP_SEL_EX  ((u32)0x00000001 << 18)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_0_TTCN_SEL_EX  ((u32)0x00000001 << 17)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_N_0_TTCP_SEL_EX  ((u32)0x00000001 << 16)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_3_TTCN_SEL_EX  ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_3_TTCP_SEL_EX  ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_2_TTCN_SEL_EX  ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_2_TTCP_SEL_EX  ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_1_TTCN_SEL_EX  ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_1_TTCP_SEL_EX  ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_0_TTCN_SEL_EX  ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQS_P_0_TTCP_SEL_EX  ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_3_TTCN_SEL_EX     ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_3_TTCP_SEL_EX     ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_2_TTCN_SEL_EX     ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_2_TTCP_SEL_EX     ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_1_TTCN_SEL_EX     ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_1_TTCP_SEL_EX     ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_0_TTCN_SEL_EX     ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_0_DQ_0_TTCP_SEL_EX     ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_ODT_SEL_EX_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_ODT_SEL_EX_1_CK_1_TTCN_SEL_EX   ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CK_1_TTCP_SEL_EX   ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CK_TTCN_SEL_EX     ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CK_TTCP_SEL_EX     ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CS_1_TTCN_SEL_EX   ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CS_1_TTCP_SEL_EX   ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CS_TTCN_SEL_EX     ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CS_TTCP_SEL_EX     ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_RST_TTCN_SEL_EX    ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_RST_TTCP_SEL_EX    ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CA_TTCN_SEL_EX     ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CA_TTCP_SEL_EX     ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CKE_1_TTCN_SEL_EX  ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CKE_1_TTCP_SEL_EX  ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CKE_TTCN_SEL_EX    ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_ODT_SEL_EX_1_CKE_TTCP_SEL_EX    ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_EX_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_3_OCDN_SEL_EX  ((u32)0x00000001 << 23)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_3_OCDP_SEL_EX  ((u32)0x00000001 << 22)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_2_OCDN_SEL_EX  ((u32)0x00000001 << 21)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_2_OCDP_SEL_EX  ((u32)0x00000001 << 20)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_1_OCDN_SEL_EX  ((u32)0x00000001 << 19)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_1_OCDP_SEL_EX  ((u32)0x00000001 << 18)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_0_OCDN_SEL_EX  ((u32)0x00000001 << 17)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_N_0_OCDP_SEL_EX  ((u32)0x00000001 << 16)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_3_OCDN_SEL_EX  ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_3_OCDP_SEL_EX  ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_2_OCDN_SEL_EX  ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_2_OCDP_SEL_EX  ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_1_OCDN_SEL_EX  ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_1_OCDP_SEL_EX  ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_0_OCDN_SEL_EX  ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQS_P_0_OCDP_SEL_EX  ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_3_OCDN_SEL_EX     ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_3_OCDP_SEL_EX     ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_2_OCDN_SEL_EX     ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_2_OCDP_SEL_EX     ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_1_OCDN_SEL_EX     ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_1_OCDP_SEL_EX     ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_0_OCDN_SEL_EX     ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_0_DQ_0_OCDP_SEL_EX     ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_EX_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_11_OCDN_SEL_EX  ((u32)0x00000001 << 31)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_11_OCDP_SEL_EX  ((u32)0x00000001 << 30)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_10_OCDN_SEL_EX  ((u32)0x00000001 << 29)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_10_OCDP_SEL_EX  ((u32)0x00000001 << 28)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_9_OCDN_SEL_EX   ((u32)0x00000001 << 27)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_9_OCDP_SEL_EX   ((u32)0x00000001 << 26)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_8_OCDN_SEL_EX   ((u32)0x00000001 << 25)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_8_OCDP_SEL_EX   ((u32)0x00000001 << 24)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_7_OCDN_SEL_EX   ((u32)0x00000001 << 23)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_7_OCDP_SEL_EX   ((u32)0x00000001 << 22)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_6_OCDN_SEL_EX   ((u32)0x00000001 << 21)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_6_OCDP_SEL_EX   ((u32)0x00000001 << 20)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_5_OCDN_SEL_EX   ((u32)0x00000001 << 19)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_5_OCDP_SEL_EX   ((u32)0x00000001 << 18)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_4_OCDN_SEL_EX   ((u32)0x00000001 << 17)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQS_4_OCDP_SEL_EX   ((u32)0x00000001 << 16)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_11_OCDN_SEL_EX   ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_11_OCDP_SEL_EX   ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_10_OCDN_SEL_EX   ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_10_OCDP_SEL_EX   ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_9_OCDN_SEL_EX    ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_9_OCDP_SEL_EX    ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_8_OCDN_SEL_EX    ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_8_OCDP_SEL_EX    ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_7_OCDN_SEL_EX    ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_7_OCDP_SEL_EX    ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_6_OCDN_SEL_EX    ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_6_OCDP_SEL_EX    ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_5_OCDN_SEL_EX    ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_5_OCDP_SEL_EX    ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_4_OCDN_SEL_EX    ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_1_DQ_4_OCDP_SEL_EX    ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_EX_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR15_OCDN_SEL_EX  ((u32)0x00000001 << 31)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR15_OCDP_SEL_EX  ((u32)0x00000001 << 30)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR14_OCDN_SEL_EX  ((u32)0x00000001 << 29)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR14_OCDP_SEL_EX  ((u32)0x00000001 << 28)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR13_OCDN_SEL_EX  ((u32)0x00000001 << 27)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR13_OCDP_SEL_EX  ((u32)0x00000001 << 26)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR12_OCDN_SEL_EX  ((u32)0x00000001 << 25)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR12_OCDP_SEL_EX  ((u32)0x00000001 << 24)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR11_OCDN_SEL_EX  ((u32)0x00000001 << 23)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR11_OCDP_SEL_EX  ((u32)0x00000001 << 22)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR10_OCDN_SEL_EX  ((u32)0x00000001 << 21)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR10_OCDP_SEL_EX  ((u32)0x00000001 << 20)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR09_OCDN_SEL_EX  ((u32)0x00000001 << 19)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR09_OCDP_SEL_EX  ((u32)0x00000001 << 18)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR08_OCDN_SEL_EX  ((u32)0x00000001 << 17)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR08_OCDP_SEL_EX  ((u32)0x00000001 << 16)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR07_OCDN_SEL_EX  ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR07_OCDP_SEL_EX  ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR06_OCDN_SEL_EX  ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR06_OCDP_SEL_EX  ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR05_OCDN_SEL_EX  ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR05_OCDP_SEL_EX  ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR04_OCDN_SEL_EX  ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR04_OCDP_SEL_EX  ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR03_OCDN_SEL_EX  ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR03_OCDP_SEL_EX  ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR02_OCDN_SEL_EX  ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR02_OCDP_SEL_EX  ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR01_OCDN_SEL_EX  ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR01_OCDP_SEL_EX  ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR00_OCDN_SEL_EX  ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_2_ADR00_OCDP_SEL_EX  ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_EX_3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_OCD_SEL_EX_3_ACT_OCDN_SEL_EX        ((u32)0x00000001 << 31)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ACT_OCDP_SEL_EX        ((u32)0x00000001 << 30)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_PAR1_OCDN_SEL_EX       ((u32)0x00000001 << 29)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_PAR1_OCDP_SEL_EX       ((u32)0x00000001 << 28)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_PAR_OCDN_SEL_EX        ((u32)0x00000001 << 27)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_PAR_OCDP_SEL_EX        ((u32)0x00000001 << 26)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_RST_OCDN_SEL_EX        ((u32)0x00000001 << 25)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_RST_OCDP_SEL_EX        ((u32)0x00000001 << 24)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ODT_OCDN_SEL_EX        ((u32)0x00000001 << 23)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ODT_OCDP_SEL_EX        ((u32)0x00000001 << 22)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_WE_OCDN_SEL_EX         ((u32)0x00000001 << 21)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_WE_OCDP_SEL_EX         ((u32)0x00000001 << 20)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_CAS_OCDN_SEL_EX        ((u32)0x00000001 << 19)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_CAS_OCDP_SEL_EX        ((u32)0x00000001 << 18)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_RAS_OCDN_SEL_EX        ((u32)0x00000001 << 17)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_RAS_OCDP_SEL_EX        ((u32)0x00000001 << 16)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_BA2_OCDN_SEL_EX        ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_BA2_OCDP_SEL_EX        ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_BA1_OCDN_SEL_EX        ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_BA1_OCDP_SEL_EX        ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_BA0_OCDN_SEL_EX        ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_BA0_OCDP_SEL_EX        ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_CKE_1_OCDN_SEL_EX      ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_CKE_1_OCDP_SEL_EX      ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_CKE_OCDN_SEL_EX        ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_CKE_OCDP_SEL_EX        ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ADR_EXT_5_OCDN_SEL_EX  ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ADR_EXT_5_OCDP_SEL_EX  ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ADR_EXT_4_OCDN_SEL_EX  ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ADR_EXT_4_OCDP_SEL_EX  ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ADR16_OCDN_SEL_EX      ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_3_ADR16_OCDP_SEL_EX      ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_OCD_SEL_EX_4
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_N_1_OCDN_SEL_EX    ((u32)0x00000001 << 19)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_N_1_OCDP_SEL_EX    ((u32)0x00000001 << 18)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_N_0_OCDN_SEL_EX    ((u32)0x00000001 << 17)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_N_0_OCDP_SEL_EX    ((u32)0x00000001 << 16)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_P_1_OCDN_SEL_EX    ((u32)0x00000001 << 15)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_P_1_OCDP_SEL_EX    ((u32)0x00000001 << 14)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_P_0_OCDN_SEL_EX    ((u32)0x00000001 << 13)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CK_P_0_OCDP_SEL_EX    ((u32)0x00000001 << 12)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CS_1_OCDN_SEL_EX      ((u32)0x00000001 << 11)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CS_1_OCDP_SEL_EX      ((u32)0x00000001 << 10)  /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CS_OCDN_SEL_EX        ((u32)0x00000001 << 9)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_CS_OCDP_SEL_EX        ((u32)0x00000001 << 8)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_ALERT1_OCDN_SEL_EX    ((u32)0x00000001 << 7)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_ALERT1_OCDP_SEL_EX    ((u32)0x00000001 << 6)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_ALERT_OCDN_SEL_EX     ((u32)0x00000001 << 5)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_ALERT_OCDP_SEL_EX     ((u32)0x00000001 << 4)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_TEN_OCDN_SEL_EX       ((u32)0x00000001 << 3)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_TEN_OCDP_SEL_EX       ((u32)0x00000001 << 2)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_DDR4_BG0_OCDN_SEL_EX  ((u32)0x00000001 << 1)   /*!<R/W 'b0 extended select bit*/
#define DDRPHY_BIT_OCD_SEL_EX_4_DDR4_BG0_OCDP_SEL_EX  ((u32)0x00000001 << 0)   /*!<R/W 'b0 extended select bit*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RW_DMY_A
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RW_DMY_A_DQS_1_SR_P    ((u32)0x00000007 << 28)          /*!<R/W 'h0 DQS_1 SR_P [2:0]*/
#define DDRPHY_RW_DMY_A_DQS_1_SR_P(x)      ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_RW_DMY_A_DQS_1_SR_N    ((u32)0x00000007 << 24)          /*!<R/W 'h0 DQS_1 SR_N [2:0]*/
#define DDRPHY_RW_DMY_A_DQS_1_SR_N(x)      ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_RW_DMY_A_DQ_1_SR_P     ((u32)0x00000007 << 20)          /*!<R/W 'h0 DQ_1 SR_P [2:0]*/
#define DDRPHY_RW_DMY_A_DQ_1_SR_P(x)       ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_RW_DMY_A_DQ_1_SR_N     ((u32)0x00000007 << 16)          /*!<R/W 'h0 DQ_1 SR_N [2:0]*/
#define DDRPHY_RW_DMY_A_DQ_1_SR_N(x)       ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_RW_DMY_A_DQS_0_SR_P    ((u32)0x00000007 << 12)          /*!<R/W 'h0 DQS_0 SR_P [2:0]*/
#define DDRPHY_RW_DMY_A_DQS_0_SR_P(x)      ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_RW_DMY_A_DQS_0_SR_N    ((u32)0x00000007 << 8)           /*!<R/W 'h0 DQS_0 SR_N [2:0]*/
#define DDRPHY_RW_DMY_A_DQS_0_SR_N(x)      ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_RW_DMY_A_DQ_0_SR_P     ((u32)0x00000007 << 4)           /*!<R/W 'h0 DQ_0 SR_P [2:0]*/
#define DDRPHY_RW_DMY_A_DQ_0_SR_P(x)       ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_RW_DMY_A_DQ_0_SR_N     ((u32)0x00000007 << 0)           /*!<R/W 'h0 DQ_0 SR_N [2:0]*/
#define DDRPHY_RW_DMY_A_DQ_0_SR_N(x)       ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RW_DMY_B
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RW_DMY_B_DUMMY_1  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 dummy register*/
#define DDRPHY_RW_DMY_B_DUMMY_1(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_2_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_READ_CTRL_2_0_EN_ODT_3_STG_0       ((u32)0x00000001 << 24)           /*!<R/W 'h0 turn on/off DQ/DM/DQS ODT by 3 stages. When turn on ODT, stage 1: {4'h0, ODT_value[5:4]} stage 2: {2'h0, ODT_value[5:2]} stage 3: {ODT_value[5:0]} When turn off ODT, stage 1: {ODT_value[5:0]} stage 2: {2'h0, ODT_value[5:2]} stage 3: {4'h0, ODT_value[5:4]} Enable this function will extend odt_en/IE by 2 tCK in the end. If want to enable this function, the duration of odt_en/IE should be at least 6 tCK.*/
#define DDRPHY_BIT_READ_CTRL_2_0_DQS_ODT_FORCE_SEL_0  ((u32)0x00000001 << 23)           /*!<R/W 'h0 DQS Rx ODT pad control. 0 : controlled by read command 1: controlled by dqs_odt_force_sig*/
#define DDRPHY_BIT_READ_CTRL_2_0_DQS_ODT_FORCE_SIG_0  ((u32)0x00000001 << 22)           /*!<R/W 'h0 If set dqs_odt_force_sel, DQS TE is controlled by dqs_odt_force_sig.*/
#define DDRPHY_BIT_READ_CTRL_2_0_DQ_ODT_FORCE_SEL_0   ((u32)0x00000001 << 21)           /*!<R/W 'h0 DQ Rx ODT control. 0 : controlled by read command 1: controlled by dq_odt_force_sig*/
#define DDRPHY_BIT_READ_CTRL_2_0_DQ_ODT_FORCE_SIG_0   ((u32)0x00000001 << 20)           /*!<R/W 'h0 If set dq_odt_force_sel, DQ TE is controlled by dq_odt_force_sig.*/
#define DDRPHY_BIT_READ_CTRL_2_0_TM_ODT_EN_ODD_0      ((u32)0x00000001 << 3)            /*!<R/W 'h1 for 2to1 mode 1'b1 : odd length for tm_odt_en 1'b0 : even length for tm_odt_en for 1to1 mode 1'b1 : length add 1T*/
#define DDRPHY_MASK_READ_CTRL_2_0_TM_ODT_EN_SEL_0     ((u32)0x00000003 << 1)            /*!<R/W 'h0 2'b00: follow (odt_en + odt_en_dly) setting. 2'b01: delay (odt_en + odt_en_dly) assert 0.5 T. 2'b10, 2'b11: delay (odt_en + odt_en_dly) desert 0.5T.*/
#define DDRPHY_READ_CTRL_2_0_TM_ODT_EN_SEL_0(x)       ((u32)(((x) & 0x00000003) << 1))
#define DDRPHY_BIT_READ_CTRL_2_0_TM_ODT_EN_DLY_0      ((u32)0x00000001 << 0)            /*!<R/W 'h0 1'b1 : odt_en delay 1DCK*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_2_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_READ_CTRL_2_1_EN_ODT_3_STG_1       ((u32)0x00000001 << 24)           /*!<R/W 'h0 turn on/off DQ/DM/DQS ODT by 3 stages. When turn on ODT, stage 1: {4'h0, ODT_value[5:4]} stage 2: {2'h0, ODT_value[5:2]} stage 3: {ODT_value[5:0]} When turn off ODT, stage 1: {ODT_value[5:0]} stage 2: {2'h0, ODT_value[5:2]} stage 3: {4'h0, ODT_value[5:4]} Enable this function will extend odt_en/IE by 2 tCK in the end. If want to enable this function, the duration of odt_en/IE should be at least 6 tCK.*/
#define DDRPHY_BIT_READ_CTRL_2_1_DQS_ODT_FORCE_SEL_1  ((u32)0x00000001 << 23)           /*!<R/W 'h0 DQS Rx ODT pad control. 0 : controlled by read command 1: controlled by dqs_odt_force_sig*/
#define DDRPHY_BIT_READ_CTRL_2_1_DQS_ODT_FORCE_SIG_1  ((u32)0x00000001 << 22)           /*!<R/W 'h0 If set dqs_odt_force_sel, DQS TE is controlled by dqs_odt_force_sig.*/
#define DDRPHY_BIT_READ_CTRL_2_1_DQ_ODT_FORCE_SEL_1   ((u32)0x00000001 << 21)           /*!<R/W 'h0 DQ Rx ODT control. 0 : controlled by read command 1: controlled by dq_odt_force_sig*/
#define DDRPHY_BIT_READ_CTRL_2_1_DQ_ODT_FORCE_SIG_1   ((u32)0x00000001 << 20)           /*!<R/W 'h0 If set dq_odt_force_sel, DQ TE is controlled by dq_odt_force_sig.*/
#define DDRPHY_BIT_READ_CTRL_2_1_TM_ODT_EN_ODD_1      ((u32)0x00000001 << 3)            /*!<R/W 'h1 for 2to1 mode 1'b1 : odd length for tm_odt_en 1'b0 : even length for tm_odt_en for 1to1 mode 1'b1 : length add 1T*/
#define DDRPHY_MASK_READ_CTRL_2_1_TM_ODT_EN_SEL_1     ((u32)0x00000003 << 1)            /*!<R/W 'h2 2'b00: follow (odt_en + odt_en_dly) setting. 2'b01: delay (odt_en + odt_en_dly) assert 0.5 T. 2'b10, 2'b11: delay (odt_en + odt_en_dly) desert 0.5T.*/
#define DDRPHY_READ_CTRL_2_1_TM_ODT_EN_SEL_1(x)       ((u32)(((x) & 0x00000003) << 1))
#define DDRPHY_BIT_READ_CTRL_2_1_TM_ODT_EN_DLY_1      ((u32)0x00000001 << 0)            /*!<R/W 'h1 1'b1 : odt_en delay 1DCK*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_READ_CTRL_3_DQSEN_CAL_OFFSET4   ((u32)0x00000001 << 25)            /*!<R/W 'h0 0: DQSEN Calibration offset + 2 1: DQSEN Calibration offset + 4*/
#define DDRPHY_BIT_READ_CTRL_3_DQSEN_DQS_INV       ((u32)0x00000001 << 24)            /*!<R/W 'h0 0: use dqs for dqsen calibration 1: use inverted dqs for dqsen calibration (in ddr4)*/
#define DDRPHY_BIT_READ_CTRL_3_MCK_RX_INV_SEL      ((u32)0x00000001 << 23)            /*!<R/W 'h0 0: delay dqs_en1_set by 0.5T mck_rx 1: delay dqs_en1_set by 1T mck_rx (No use)*/
#define DDRPHY_BIT_READ_CTRL_3_DQSEN_CAL_SEL       ((u32)0x00000001 << 22)            /*!<R/W 'h0 Select dqs enable calibration algorithm.*/
#define DDRPHY_BIT_READ_CTRL_3_DQS_EARLY_SEL       ((u32)0x00000001 << 21)            /*!<R/W 'h0 Select source signal of dqs early detection 0: dqs_en2 1: dqs*/
#define DDRPHY_MASK_READ_CTRL_3_TM_DQS_CAL_PRE     ((u32)0x0000001F << 16)            /*!<R/W 'h00 for tm_dqs_en_ftun calibration 2to1 : tm_dqs_cal_pre = tm_dqs_en - 1*/
#define DDRPHY_READ_CTRL_3_TM_DQS_CAL_PRE(x)       ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_READ_CTRL_3_TM_DQSEN_EARLY_PH  ((u32)0x0000003F << 8)             /*!<R/W 'h0B tm_dqs_en calibration early delay tap When dqsen_cal_sel = 1, [1:0] are used to control latency between dqs_en1_set and dqs_en1_early.*/
#define DDRPHY_READ_CTRL_3_TM_DQSEN_EARLY_PH(x)    ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_READ_CTRL_3_TM_DQSEN_LATE_PH   ((u32)0x0000003F << 0)             /*!<R/W 'h0B tm_dqs_en calibration late delay tap When dqsen_cal_sel = 1, [1:0] are used to control latency between dqs_en1_early and dqs_en1_late.*/
#define DDRPHY_READ_CTRL_3_TM_DQSEN_LATE_PH(x)     ((u32)(((x) & 0x0000003F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_6_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_READ_CTRL_6_0_TM_ODT_EN_0  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h000000FE odt_en length setting 2T DCK unit for 2to1 mode 1T DCK unit for 1to1 mode*/
#define DDRPHY_READ_CTRL_6_0_TM_ODT_EN_0(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_6_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_READ_CTRL_6_1_TM_ODT_EN_1  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h000000FF odt_en length setting 2T DCK unit for 2to1 mode 1T DCK unit for 1to1 mode*/
#define DDRPHY_READ_CTRL_6_1_TM_ODT_EN_1(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_POS_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_7  ((u32)0x0000000F << 28)            /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[7].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_7(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_6  ((u32)0x0000000F << 24)            /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[6].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_6(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_5  ((u32)0x0000000F << 20)            /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[5].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_5(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_4  ((u32)0x0000000F << 16)            /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[4].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_4(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_3  ((u32)0x0000000F << 12)            /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[3].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_3(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_2  ((u32)0x0000000F << 8)             /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[2].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_2(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_1  ((u32)0x0000000F << 4)             /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[1].*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_1(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DEL_POS_SEL_DELTA_LEFT_POS_SEL_0  ((u32)0x0000000F << 0)             /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dq[0]. DEL_POS_SEL only controls left sampling point in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_POS_SEL_DELTA_LEFT_POS_SEL_0(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_POS_SEL_DBI
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_POS_SEL_DBI_DELTA_LEFT_POS_SEL_8  ((u32)0x0000000F << 0)            /*!<R/W 'h2 Left/right sampling point of positive edge 3-point calibration for dbi. Only controls left sampling point in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_POS_SEL_DBI_DELTA_LEFT_POS_SEL_8(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_NEG_SEL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_7  ((u32)0x0000000F << 28)            /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[7].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_7(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_6  ((u32)0x0000000F << 24)            /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[6].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_6(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_5  ((u32)0x0000000F << 20)            /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[5].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_5(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_4  ((u32)0x0000000F << 16)            /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[4].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_4(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_3  ((u32)0x0000000F << 12)            /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[3].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_3(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_2  ((u32)0x0000000F << 8)             /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[2].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_2(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_1  ((u32)0x0000000F << 4)             /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[1].*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_1(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_0  ((u32)0x0000000F << 0)             /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dq[0]. DEL_NEG_SEL only controls left sampling point in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_NEG_SEL_DELTA_LEFT_NEG_SEL_0(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_NEG_SEL_DBI
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_NEG_SEL_DBI_DELTA_LEFT_NEG_SEL_8  ((u32)0x0000000F << 0)            /*!<R/W 'h2 Left/right sampling point of negative edge 3-point calibration for dbi. Only controls left sampling point in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_NEG_SEL_DBI_DELTA_LEFT_NEG_SEL_8(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_IN_DLY_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_3  ((u32)0x0000001F << 24)            /*!<R/W 'h0A DQS_IN[3] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_3(x)    ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_2  ((u32)0x0000001F << 16)            /*!<R/W 'h0A DQS_IN[2] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_2(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_1  ((u32)0x0000001F << 8)             /*!<R/W 'h0A DQS_IN[1] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_1(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_0  ((u32)0x0000001F << 0)             /*!<R/W 'h0A DQS_IN[0] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_0_FW_RD_DLY_POS_SEL_0(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_IN_DLY_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_7  ((u32)0x0000001F << 24)            /*!<R/W 'h0A DQS_IN[7] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_7(x)    ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_6  ((u32)0x0000001F << 16)            /*!<R/W 'h0A DQS_IN[6] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_6(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_5  ((u32)0x0000001F << 8)             /*!<R/W 'h0A DQS_IN[5] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_5(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_4  ((u32)0x0000001F << 0)             /*!<R/W 'h0A DQS_IN[4] raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_1_FW_RD_DLY_POS_SEL_4(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_IN_DLY_1_DBI
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_IN_DLY_1_DBI_FW_RD_DLY_POS_SEL_8  ((u32)0x0000001F << 0)            /*!<R/W 'h0A DBI raising edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_1_DBI_FW_RD_DLY_POS_SEL_8(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_IN_DLY_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_3  ((u32)0x0000001F << 24)            /*!<R/W 'h0A DQS_IN[3] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_3(x)    ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_2  ((u32)0x0000001F << 16)            /*!<R/W 'h0A DQS_IN[2] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_2(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_1  ((u32)0x0000001F << 8)             /*!<R/W 'h0A DQS_IN[1] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_1(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_0  ((u32)0x0000001F << 0)             /*!<R/W 'h0A DQS_IN[0] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_2_FW_RD_DLY_NEG_SEL_0(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_IN_DLY_3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_7  ((u32)0x0000001F << 24)            /*!<R/W 'h0A DQS_IN[7] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_7(x)    ((u32)(((x) & 0x0000001F) << 24))
#define DDRPHY_MASK_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_6  ((u32)0x0000001F << 16)            /*!<R/W 'h0A DQS_IN[6] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_6(x)    ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_5  ((u32)0x0000001F << 8)             /*!<R/W 'h0A DQS_IN[5] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_5(x)    ((u32)(((x) & 0x0000001F) << 8))
#define DDRPHY_MASK_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_4  ((u32)0x0000001F << 0)             /*!<R/W 'h0A DQS_IN[4] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_3_FW_RD_DLY_NEG_SEL_4(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQS_IN_DLY_3_DBI
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQS_IN_DLY_3_DBI_FW_RD_DLY_NEG_SEL_8  ((u32)0x0000001F << 0)            /*!<R/W 'h0A DQS_IN[4] falling edge delay chain control*/
#define DDRPHY_DQS_IN_DLY_3_DBI_FW_RD_DLY_NEG_SEL_8(x)    ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_VALID_WIN_DET_PFIFO
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_VALID_WIN_DET_PFIFO_RX_LAG_DET_PFIFO   ((u32)0x000001FF << 9)            /*!<R/W 'h000 Lag flag of pfifo valid window detection in data slice0*/
#define DDRPHY_VALID_WIN_DET_PFIFO_RX_LAG_DET_PFIFO(x)     ((u32)(((x) & 0x000001FF) << 9))
#define DDRPHY_MASK_VALID_WIN_DET_PFIFO_RX_LEAD_DET_PFIFO  ((u32)0x000001FF << 0)            /*!<R/W 'h000 Lead flag of pfifo valid window detection in data slice0*/
#define DDRPHY_VALID_WIN_DET_PFIFO_RX_LEAD_DET_PFIFO(x)    ((u32)(((x) & 0x000001FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_VALID_WIN_DET_NFIFO
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_VALID_WIN_DET_NFIFO_RX_LAG_DET_NFIFO   ((u32)0x000001FF << 9)            /*!<R/W 'h000 Lag flag of nfifo valid window detection in data slice0*/
#define DDRPHY_VALID_WIN_DET_NFIFO_RX_LAG_DET_NFIFO(x)     ((u32)(((x) & 0x000001FF) << 9))
#define DDRPHY_MASK_VALID_WIN_DET_NFIFO_RX_LEAD_DET_NFIFO  ((u32)0x000001FF << 0)            /*!<R/W 'h000 Lead flag of nfifo valid window detection in data slice0*/
#define DDRPHY_VALID_WIN_DET_NFIFO_RX_LEAD_DET_NFIFO(x)    ((u32)(((x) & 0x000001FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_DLY_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_DLY_0_DQ7_DLY_SEL  ((u32)0x0000000F << 28)            /*!<R/W 'h0 DQ[7] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ7_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_DQ_DLY_0_DQ6_DLY_SEL  ((u32)0x0000000F << 24)            /*!<R/W 'h0 DQ[6] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ6_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_DQ_DLY_0_DQ5_DLY_SEL  ((u32)0x0000000F << 20)            /*!<R/W 'h0 DQ[5] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ5_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_DQ_DLY_0_DQ4_DLY_SEL  ((u32)0x0000000F << 16)            /*!<R/W 'h0 DQ[4] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ4_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_DQ_DLY_0_DQ3_DLY_SEL  ((u32)0x0000000F << 12)            /*!<R/W 'h0 DQ[3] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ3_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DQ_DLY_0_DQ2_DLY_SEL  ((u32)0x0000000F << 8)             /*!<R/W 'h0 DQ[2] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ2_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DQ_DLY_0_DQ1_DLY_SEL  ((u32)0x0000000F << 4)             /*!<R/W 'h0 DQ[1] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ1_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DQ_DLY_0_DQ0_DLY_SEL  ((u32)0x0000000F << 0)             /*!<R/W 'h0 DQ[0] delay chain control*/
#define DDRPHY_DQ_DLY_0_DQ0_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_DLY_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_DLY_1_DQS_OE_DLY_SEL  ((u32)0x0000000F << 8)            /*!<R/W 'h0 Slice DQS output enable delay chain control*/
#define DDRPHY_DQ_DLY_1_DQS_OE_DLY_SEL(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DQ_DLY_1_DM_DLY_SEL      ((u32)0x0000000F << 4)            /*!<R/W 'h0 Slice DM delay chain control*/
#define DDRPHY_DQ_DLY_1_DM_DLY_SEL(x)        ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DQ_DLY_1_DQS_DLY_SEL     ((u32)0x0000000F << 0)            /*!<R/W 'h0 Slice DQS delay chain control*/
#define DDRPHY_DQ_DLY_1_DQS_DLY_SEL(x)       ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CAL_OUT_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CAL_OUT_0_CAL_RD_DLY_OUT  ((u32)0xFFFFFFFF << 0)            /*!<R 'h0A0A0A0A 3-point sampling result from data slice0 ~ 3, selected by cal_rd_dly_out_**/
#define DDRPHY_CAL_OUT_0_CAL_RD_DLY_OUT(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_CAL_OUT_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_CAL_OUT_1_CAL_DELTA_OUT  ((u32)0xFFFFFFFF << 0)            /*!<R 'h22222222 3-point sampling delta width result from data slice0 ~ 3, selected by cal_rd_dly_out_**/
#define DDRPHY_CAL_OUT_1_CAL_DELTA_OUT(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_WR_FIFO_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_WR_FIFO_0_WR_ADR_POS  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Write FIFO pointer of raising edge dqs { wr_adr_pos_7[3:0], wr_adr_pos_6[3:0], wr_adr_pos_5[3:0], wr_adr_pos_4[3:0], wr_adr_pos_3[3:0], wr_adr_pos_2[3:0], wr_adr_pos_1[3:0], wr_adr_pos_0[3:0]}*/
#define DDRPHY_WR_FIFO_0_WR_ADR_POS(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_WR_FIFO_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_WR_FIFO_1_WR_ADR_NEG  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Write FIFO pointer of falling edge dqs {wr_adr_neg_7[3:0], wr_adr_neg_6[3:0], wr_adr_neg_5[3:0], wr_adr_neg_4[3:0], wr_adr_neg_3[3:0], wr_adr_neg_2[3:0], wr_adr_neg_1[3:0], wr_adr_neg_0[3:0]}*/
#define DDRPHY_WR_FIFO_1_WR_ADR_NEG(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RD_FIFO_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RD_FIFO_0_RD_ADR_POS  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Read FIFO pointer of raising edge dqs {rd_adr_pos_7[3:0], rd_adr_pos_6[3:0], rd_adr_pos_5[3:0], rd_adr_pos_4[3:0], rd_adr_pos_3[3:0], rd_adr_pos_2[3:0], rd_adr_pos_1[3:0], rd_adr_pos_0[3:0]}*/
#define DDRPHY_RD_FIFO_0_RD_ADR_POS(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RD_FIFO_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RD_FIFO_1_RD_ADR_NEG  ((u32)0xFFFFFFFF << 0)            /*!<R 'h00000000 Read FIFO pointer of falling edge dqs {rd_adr_neg_7[3:0], rd_adr_neg_6[3:0], rd_adr_neg_5[3:0], rd_adr_neg_4[3:0], rd_adr_neg_3[3:0], rd_adr_neg_2[3:0], rd_adr_neg_1[3:0], rd_adr_neg_0[3:0]}*/
#define DDRPHY_RD_FIFO_1_RD_ADR_NEG(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DBI_FIFO
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DBI_FIFO_RW_ADR_DBI  ((u32)0x0000FFFF << 0)            /*!<R 'h0000 {wr_adr_dbi_pos[3:0], wr_adr_dbi_neg[3:0], rd_adr_dbi_pos[3:0], rd_adr_dbi_neg[3:0]}*/
#define DDRPHY_DBI_FIFO_RW_ADR_DBI(x)    ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_INT_STATUS_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_INT_STATUS_1_CAL_RBDY_INT_NEG  ((u32)0x000000FF << 24)            /*!<R/W 'h00 interrupt of DQS falling edge hit delay tap right boundary in 3-point calibration*/
#define DDRPHY_INT_STATUS_1_CAL_RBDY_INT_NEG(x)    ((u32)(((x) & 0x000000FF) << 24))
#define DDRPHY_MASK_INT_STATUS_1_CAL_LBDY_INT_NEG  ((u32)0x000000FF << 16)            /*!<R/W 'h00 interrupt of DQS falling edge hit delay tap left boundary in 3-point calibration*/
#define DDRPHY_INT_STATUS_1_CAL_LBDY_INT_NEG(x)    ((u32)(((x) & 0x000000FF) << 16))
#define DDRPHY_MASK_INT_STATUS_1_CAL_RBDY_INT_POS  ((u32)0x000000FF << 8)             /*!<R/W 'h00 interrupt of DQS raising edge hit delay tap right boundary in 3-point calibration*/
#define DDRPHY_INT_STATUS_1_CAL_RBDY_INT_POS(x)    ((u32)(((x) & 0x000000FF) << 8))
#define DDRPHY_MASK_INT_STATUS_1_CAL_LBDY_INT_POS  ((u32)0x000000FF << 0)             /*!<R/W 'h00 interrupt of DQS raising edge hit delay tap left boundary in 3-point calibration*/
#define DDRPHY_INT_STATUS_1_CAL_LBDY_INT_POS(x)    ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_INT_STATUS_3
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_INT_STATUS_3_CAL_RBDY_INT_NEG   ((u32)0x00000001 << 21)           /*!<R/W 'h0 interrupt of DQS falling edge hit delay tap right boundary in DBI 3-point calibration*/
#define DDRPHY_BIT_INT_STATUS_3_CAL_LBDY_INT_NEG   ((u32)0x00000001 << 20)           /*!<R/W 'h0 interrupt of DQS falling edge hit delay tap left boundary in DBI 3-point calibration*/
#define DDRPHY_BIT_INT_STATUS_3_CAL_RBDY_INT_POS   ((u32)0x00000001 << 19)           /*!<R/W 'h0 interrupt of DQS raising edge hit delay tap right boundary in DBI 3-point calibration*/
#define DDRPHY_BIT_INT_STATUS_3_CAL_LBDY_INT_POS   ((u32)0x00000001 << 18)           /*!<R/W 'h0 interrupt of DQS raising edge hit delay tap left boundary in DBI 3-point calibration*/
#define DDRPHY_MASK_INT_STATUS_3_FIFO_PTR_INT_NEG  ((u32)0x000001FF << 9)            /*!<R/W 'h000 Rx NFIFO pointer error interrupt*/
#define DDRPHY_INT_STATUS_3_FIFO_PTR_INT_NEG(x)    ((u32)(((x) & 0x000001FF) << 9))
#define DDRPHY_MASK_INT_STATUS_3_FIFO_PTR_INT_POS  ((u32)0x000001FF << 0)            /*!<R/W 'h000 Rx PFIFO pointer error interrupt*/
#define DDRPHY_INT_STATUS_3_FIFO_PTR_INT_POS(x)    ((u32)(((x) & 0x000001FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX4
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PRBS_TMUX4_DQS_TEST  ((u32)0x00000001 << 31)            /*!<R/W 'h0 1: dqs on ,0: dqs off*/
#define DDRPHY_MASK_PRBS_TMUX4_DQ5_SEL  ((u32)0x00000007 << 28)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX4_DQ5_SEL(x)    ((u32)(((x) & 0x00000007) << 28))
#define DDRPHY_MASK_PRBS_TMUX4_DQ4_SEL  ((u32)0x00000007 << 24)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX4_DQ4_SEL(x)    ((u32)(((x) & 0x00000007) << 24))
#define DDRPHY_MASK_PRBS_TMUX4_DQ3_SEL  ((u32)0x00000007 << 20)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX4_DQ3_SEL(x)    ((u32)(((x) & 0x00000007) << 20))
#define DDRPHY_MASK_PRBS_TMUX4_DQ2_SEL  ((u32)0x00000007 << 16)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX4_DQ2_SEL(x)    ((u32)(((x) & 0x00000007) << 16))
#define DDRPHY_MASK_PRBS_TMUX4_DQ1_SEL  ((u32)0x00000007 << 12)            /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX4_DQ1_SEL(x)    ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_PRBS_TMUX4_DQ0_SEL  ((u32)0x00000007 << 8)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX4_DQ0_SEL(x)    ((u32)(((x) & 0x00000007) << 8))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_PRBS_TMUX5
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_PRBS_TMUX5_DATA_PRBS11_INVS   ((u32)0x00000001 << 31)            /*!<R/W 'h0 inverted PRBS11 for data slice*/
#define DDRPHY_MASK_PRBS_TMUX5_DATA_PRBS11_SEED  ((u32)0x000007FF << 16)            /*!<R/W 'h001 PRBS11 seed for data slice*/
#define DDRPHY_PRBS_TMUX5_DATA_PRBS11_SEED(x)    ((u32)(((x) & 0x000007FF) << 16))
#define DDRPHY_MASK_PRBS_TMUX5_DM_SEL            ((u32)0x00000007 << 8)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX5_DM_SEL(x)              ((u32)(((x) & 0x00000007) << 8))
#define DDRPHY_MASK_PRBS_TMUX5_DQ7_SEL           ((u32)0x00000007 << 4)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX5_DQ7_SEL(x)             ((u32)(((x) & 0x00000007) << 4))
#define DDRPHY_MASK_PRBS_TMUX5_DQ6_SEL           ((u32)0x00000007 << 0)             /*!<R/W 'h0 000: force 0 001: force 1 010: PRBS11 011: PRBS11 invert 100: PRBS11 delay one clock cycle 101: from test_pattern register 110: test_pattern invert*/
#define DDRPHY_PRBS_TMUX5_DQ6_SEL(x)             ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RW_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RW_EMPH_CTL_DQS_SLEWRATE  ((u32)0x00000003 << 28)            /*!<R/W 'h0 DQS slew rate control pin*/
#define DDRPHY_RW_EMPH_CTL_DQS_SLEWRATE(x)    ((u32)(((x) & 0x00000003) << 28))
#define DDRPHY_MASK_RW_EMPH_CTL_DQS_DLY_EMPH  ((u32)0x0000000F << 24)            /*!<R/W 'h0 DQS Delay tap control pin*/
#define DDRPHY_RW_EMPH_CTL_DQS_DLY_EMPH(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_RW_EMPH_CTL_DQ_SLEWRATE   ((u32)0x00000003 << 12)            /*!<R/W 'h0 DQ/DM slew rate control pin*/
#define DDRPHY_RW_EMPH_CTL_DQ_SLEWRATE(x)     ((u32)(((x) & 0x00000003) << 12))
#define DDRPHY_MASK_RW_EMPH_CTL_DQ_DLY_EMPH   ((u32)0x0000000F << 8)             /*!<R/W 'h0 DQ/DM Delay tap control pin*/
#define DDRPHY_RW_EMPH_CTL_DQ_DLY_EMPH(x)     ((u32)(((x) & 0x0000000F) << 8))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RW_SEL_EMPH_CTL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_RW_SEL_EMPH_CTL_DQS_SEL_EMPHN  ((u32)0x0000000F << 20)            /*!<R/W 'h7 DQS SEL Emphasis pin*/
#define DDRPHY_RW_SEL_EMPH_CTL_DQS_SEL_EMPHN(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_RW_SEL_EMPH_CTL_DQS_SEL_EMPHP  ((u32)0x0000000F << 16)            /*!<R/W 'h7 DQS SEL Emphasis pin*/
#define DDRPHY_RW_SEL_EMPH_CTL_DQS_SEL_EMPHP(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_RW_SEL_EMPH_CTL_DQ_SEL_EMPHN   ((u32)0x0000000F << 4)             /*!<R/W 'h7 DQ/DM SEL Emphasis pin*/
#define DDRPHY_RW_SEL_EMPH_CTL_DQ_SEL_EMPHN(x)     ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_RW_SEL_EMPH_CTL_DQ_SEL_EMPHP   ((u32)0x0000000F << 0)             /*!<R/W 'h7 DQ/DM SEL Emphasis pin*/
#define DDRPHY_RW_SEL_EMPH_CTL_DQ_SEL_EMPHP(x)     ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_IO_CTL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_IO_CTL_0_DQ_OE_SIG     ((u32)0x000000FF << 24)            /*!<R/W 'h00 FW can config it to control OE of DQ pad 7 : DQ7 of slice : 0 : DQ0 of slice*/
#define DDRPHY_DQ_IO_CTL_0_DQ_OE_SIG(x)       ((u32)(((x) & 0x000000FF) << 24))
#define DDRPHY_MASK_DQ_IO_CTL_0_DQ_OE_FRC_EN  ((u32)0x000000FF << 16)            /*!<R/W 'h00 FW can set 1 to control OE of DQ pad 7 : DQ7 of slice : 0 : DQ0 of slice*/
#define DDRPHY_DQ_IO_CTL_0_DQ_OE_FRC_EN(x)    ((u32)(((x) & 0x000000FF) << 16))
#define DDRPHY_MASK_DQ_IO_CTL_0_DQ_IE_SIG     ((u32)0x000000FF << 8)             /*!<R/W 'h00 FW can config it to control IE of DQ pad 7 : DQ7 of slice : 0 : DQ0 of slice*/
#define DDRPHY_DQ_IO_CTL_0_DQ_IE_SIG(x)       ((u32)(((x) & 0x000000FF) << 8))
#define DDRPHY_MASK_DQ_IO_CTL_0_DQ_IE_FRC_EN  ((u32)0x000000FF << 0)             /*!<R/W 'h00 FW can set 1 to control IE of DQ pad 7 : DQ7 of slice : 0 : DQ0 of slice*/
#define DDRPHY_DQ_IO_CTL_0_DQ_IE_FRC_EN(x)    ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_IO_CTL_1
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_IO_CTL_1_DQ_PAT_OUT   ((u32)0x000000FF << 24)            /*!<R/W 'h00 dedicated pattern for DQ out 7 : DQ7 of slice : 0 : DQ0 of slice*/
#define DDRPHY_DQ_IO_CTL_1_DQ_PAT_OUT(x)     ((u32)(((x) & 0x000000FF) << 24))
#define DDRPHY_BIT_DQ_IO_CTL_1_DM_PAT_OUT    ((u32)0x00000001 << 23)            /*!<R/W 'h0 dedicated pattern for DM out bit0 : DM of slice*/
#define DDRPHY_BIT_DQ_IO_CTL_1_DM_OE_SIG     ((u32)0x00000001 << 3)             /*!<R/W 'h0 FW can config it to control OE of DM pad bit0 : DM of slice*/
#define DDRPHY_BIT_DQ_IO_CTL_1_DM_OE_FRC_EN  ((u32)0x00000001 << 2)             /*!<R/W 'h0 FW can set 1 to control OE of DM pad bit0 : DM of slice*/
#define DDRPHY_BIT_DQ_IO_CTL_1_DM_IE_SIG     ((u32)0x00000001 << 1)             /*!<R/W 'h0 FW can config it to control IE of DM pad bit0 : DM of slice*/
#define DDRPHY_BIT_DQ_IO_CTL_1_DM_IE_FRC_EN  ((u32)0x00000001 << 0)             /*!<R/W 'h1 FW can set 1 to control IE of DM pad bit0 : DM of slice*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_DBG_CTRL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_READ_DBG_CTRL_DQS_BURST_MON_WAIT_TIME  ((u32)0x0000000F << 26)            /*!<R/W 'h7 Waiting time for monitoring Rx DQS burst length.*/
#define DDRPHY_READ_DBG_CTRL_DQS_BURST_MON_WAIT_TIME(x)    ((u32)(((x) & 0x0000000F) << 26))
#define DDRPHY_MASK_READ_DBG_CTRL_RD_DBG_SEL1              ((u32)0x0000001F << 21)            /*!<R/W 'h00 Debug selection for data slice*/
#define DDRPHY_READ_DBG_CTRL_RD_DBG_SEL1(x)                ((u32)(((x) & 0x0000001F) << 21))
#define DDRPHY_MASK_READ_DBG_CTRL_RD_DBG_SEL0              ((u32)0x0000001F << 16)            /*!<R/W 'h00 Debug selection for data slice*/
#define DDRPHY_READ_DBG_CTRL_RD_DBG_SEL0(x)                ((u32)(((x) & 0x0000001F) << 16))
#define DDRPHY_MASK_READ_DBG_CTRL_CAL_RD_DLY_OUT_SEL       ((u32)0x00000007 << 12)            /*!<R/W 'h0 3-point sampling result selection for data slice 000: dq[1:0] of data slice 001: dq[3:2] of data slice 010: dq[5:4] of data slice 011: dq[7:6] of data slice 100: dq[8] of data slice*/
#define DDRPHY_READ_DBG_CTRL_CAL_RD_DLY_OUT_SEL(x)         ((u32)(((x) & 0x00000007) << 12))
#define DDRPHY_MASK_READ_DBG_CTRL_TX_WRMASK_MASK           ((u32)0x00000003 << 10)            /*!<R/W 'b0 1'b1 : mask per bit TX wrmask to 1'b0.*/
#define DDRPHY_READ_DBG_CTRL_TX_WRMASK_MASK(x)             ((u32)(((x) & 0x00000003) << 10))
#define DDRPHY_MASK_READ_DBG_CTRL_DQS_DM_PD_EN             ((u32)0x00000003 << 8)             /*!<R/W 'h0 slice [DQS, DM] PAD power-down b0 : normal, b1: power-down*/
#define DDRPHY_READ_DBG_CTRL_DQS_DM_PD_EN(x)               ((u32)(((x) & 0x00000003) << 8))
#define DDRPHY_MASK_READ_DBG_CTRL_DQ_PD_EN                 ((u32)0x000000FF << 0)             /*!<R/W 'h00 slice DQ[7:0] PAD power-down b0 : normal, b1: power-down*/
#define DDRPHY_READ_DBG_CTRL_DQ_PD_EN(x)                   ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_MON
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_MON_CAL_FTUN_UNDER_FLG     ((u32)0x00000001 << 30)            /*!<R 'h0 dqs_en calibration under flow, don't move ftun value. Clear by fw_clr_dqs_int.*/
#define DDRPHY_BIT_MON_CAL_FTUN_OVER_FLG      ((u32)0x00000001 << 29)            /*!<R 'h0 dqs_en calibration over flow, don't move ftun value. Clear by fw_clr_dqs_int.*/
#define DDRPHY_BIT_MON_CAL_FTUN_BOTH_HIT_FLG  ((u32)0x00000001 << 28)            /*!<R 'h0 dqs_en calibration bot hit eraly and late, don't move ftun value. Clear by fw_clr_dqs_int. This flag only change at dqsen_cal_sel = 0.*/
#define DDRPHY_MASK_MON_FW_TM_DQS_EN_FTUN     ((u32)0x0000000F << 24)            /*!<R 'h3 Slice, Internal double buffer value of tm_dqs_en_ftun.*/
#define DDRPHY_MON_FW_TM_DQS_EN_FTUN(x)       ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_MON_DQS_EN_DLY_SEL_PH     ((u32)0x0000003F << 16)            /*!<R 'h00 Central sampling point setting of slice0 3-point calibration for DQS enable.*/
#define DDRPHY_MON_DQS_EN_DLY_SEL_PH(x)       ((u32)(((x) & 0x0000003F) << 16))
#define DDRPHY_MASK_MON_MON_BURST_CNT         ((u32)0x0000000F << 4)             /*!<R 'h0 debug monitor : slice burst_cnt*/
#define DDRPHY_MON_MON_BURST_CNT(x)           ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_MON_MON_DQS_CNT           ((u32)0x0000000F << 0)             /*!<R 'h0 debug monitor : slice dqs_cnt*/
#define DDRPHY_MON_MON_DQS_CNT(x)             ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_IO_CTL_2
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_DQ_IO_CTL_2_RDDATA_DEBUG_EN  ((u32)0x00000001 << 16)            /*!<R/W 'b0 1: use dq_pat_out as dfi_rddata*/
#define DDRPHY_MASK_DQ_IO_CTL_2_DQM_OE_TIME     ((u32)0x0000000F << 12)            /*!<R/W 'h0 [1:0] Control dq/dm oe enable timing. 00: original 01: pre 0.5T enable 10: pre 1T enable 11: pre 1.5T enable [3:2] Control dq/dm oe disable timing 00: original 01: post 1T disable 11: pre 1T (0.5T if en_crc = 1) disable*/
#define DDRPHY_DQ_IO_CTL_2_DQM_OE_TIME(x)       ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DQ_IO_CTL_2_DATA_INV        ((u32)0x000001FF << 0)             /*!<R/W 'h000 Invert output value for Tx/Rx DQ/DM 8 : DM of slice 7 : DQ7 of slice : 0 : DQ0 of slice*/
#define DDRPHY_DQ_IO_CTL_2_DATA_INV(x)          ((u32)(((x) & 0x000001FF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_4
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_READ_CTRL_4_BURST_OBVS_NUM    ((u32)0x0000003F << 5)            /*!<R/W 'h02 Control the observed continuous cal_chg_rd_dly signal number before increase Delta value.*/
#define DDRPHY_READ_CTRL_4_BURST_OBVS_NUM(x)      ((u32)(((x) & 0x0000003F) << 5))
#define DDRPHY_BIT_READ_CTRL_4_DELTA_CAL_MODE     ((u32)0x00000001 << 4)            /*!<R/W 'h0 1: Increase Delta value after observe (burst_obvs_num x32) continuous cal_chg_rd_dly signals with increasing Delta.*/
#define DDRPHY_BIT_READ_CTRL_4_FW_DQSEN_FTUN_UPD  ((u32)0x00000001 << 2)            /*!<R/W 'h1 1: tm_dqs_en_ftun is applicable*/
#define DDRPHY_BIT_READ_CTRL_4_FW_RD_DELTA_UPD    ((u32)0x00000001 << 1)            /*!<R/W 'h1 1: delta_wid_*_sel are applicable*/
#define DDRPHY_BIT_READ_CTRL_4_FW_RD_TE_UPD       ((u32)0x00000001 << 0)            /*!<R/W 'h1 1: fw_rd_dly_*_sel are applicable*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_POS_SEL_RIGH
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_7  ((u32)0x0000000F << 28)            /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[7] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_7(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_6  ((u32)0x0000000F << 24)            /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[6] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_6(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_5  ((u32)0x0000000F << 20)            /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[5] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_5(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_4  ((u32)0x0000000F << 16)            /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[4] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_4(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_3  ((u32)0x0000000F << 12)            /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[3] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_3(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_2  ((u32)0x0000000F << 8)             /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[2] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_2(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_1  ((u32)0x0000000F << 4)             /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[1] in off-line calibration.*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_1(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_0  ((u32)0x0000000F << 0)             /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dq[0] in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_POS_SEL_RIGH_DELTA_RIGH_POS_SEL_0(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_POS_SEL_RIGH_DBI
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_POS_SEL_RIGH_DBI_DELTA_RIGH_POS_SEL_8  ((u32)0x0000000F << 0)            /*!<R/W 'h2 Right sampling point of positive edge 3-point calibration for dbi in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_POS_SEL_RIGH_DBI_DELTA_RIGH_POS_SEL_8(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_NEG_SEL_RIGH
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_7  ((u32)0x0000000F << 28)            /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[7] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_7(x)    ((u32)(((x) & 0x0000000F) << 28))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_6  ((u32)0x0000000F << 24)            /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[6] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_6(x)    ((u32)(((x) & 0x0000000F) << 24))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_5  ((u32)0x0000000F << 20)            /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[5] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_5(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_4  ((u32)0x0000000F << 16)            /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[4] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_4(x)    ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_3  ((u32)0x0000000F << 12)            /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[3] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_3(x)    ((u32)(((x) & 0x0000000F) << 12))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_2  ((u32)0x0000000F << 8)             /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[2] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_2(x)    ((u32)(((x) & 0x0000000F) << 8))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_1  ((u32)0x0000000F << 4)             /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[1] in off-line calibration.*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_1(x)    ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_0  ((u32)0x0000000F << 0)             /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dq[0] in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DELTA_RIGH_NEG_SEL_0(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DEL_NEG_SEL_RIGH_DBI
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DEL_NEG_SEL_RIGH_DBI_DELTA_RIGH_NEG_SEL_8  ((u32)0x0000000F << 0)            /*!<R/W 'h2 Right sampling point of negative edge 3-point calibration for dbi in off-line calibration. 0: shift 1 taps from central point 1: shift 2 taps from central point ............... 15: shift 16 taps from central point*/
#define DDRPHY_DEL_NEG_SEL_RIGH_DBI_DELTA_RIGH_NEG_SEL_8(x)    ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQSEN_DBG_READ
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQSEN_DBG_READ_DQSEN_CNT    ((u32)0x0000000F << 4)            /*!<R 'h0 dqsen cnt[7:4]*/
#define DDRPHY_DQSEN_DBG_READ_DQSEN_CNT(x)      ((u32)(((x) & 0x0000000F) << 4))
#define DDRPHY_GET_DQSEN_DBG_READ_DQSEN_CNT(x)  ((u32)(((x >> 4) & 0x0000000F)))
#define DDRPHY_MASK_DQSEN_DBG_READ_READ_EN_CNT  ((u32)0x0000000F << 0)            /*!<R 'h0 read en cnt[3:0]*/
#define DDRPHY_DQSEN_DBG_READ_READ_EN_CNT(x)    ((u32)(((x) & 0x0000000F) << 0))
#define DDRPHY_GET_DQSEN_DBG_READ_READ_EN_CNT(x)((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_DQ_PRBS_TMUX
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_DQ_PRBS_TMUX_RW_TEST_PATTERN  ((u32)0xFFFFFFFF << 0)            /*!<R/W 'h00000000 32-bit test pattern for prbs11 or ten*/
#define DDRPHY_DQ_PRBS_TMUX_RW_TEST_PATTERN(x)    ((u32)(((x) & 0xFFFFFFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RW_CTRL
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_RW_CTRL_NCON_3T_MODE  ((u32)0x00000001 << 31)  /*!<R/W 'b0 for PUPD3 non-con 3T mode (ie, 4T margin for tm_dqs_en)*/
#define DDRPHY_BIT_RW_CTRL_NCON_2T_MODE  ((u32)0x00000001 << 30)  /*!<R/W 'b0 for PUPD3 non-con 2T mode (ie, 3T margin for tm_dqs_en)*/
#define DDRPHY_BIT_RW_CTRL_EN_CRC        ((u32)0x00000001 << 27)  /*!<R/W 'h0 [1] CRC enable (post one T more) for DDR4 only [0] no CRC*/
#define DDRPHY_BIT_RW_CTRL_RW_TEN_EN     ((u32)0x00000001 << 15)  /*!<R/W 'h0 1 for ten enable*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_TX_WRDATA_MASK
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_TX_WRDATA_MASK_TX_WRDATA_MASK_S1  ((u32)0x0000FFFF << 16)            /*!<R/W 'h0000 1'b1 : mask per bit TX wrdata to 1'b0*/
#define DDRPHY_TX_WRDATA_MASK_TX_WRDATA_MASK_S1(x)    ((u32)(((x) & 0x0000FFFF) << 16))
#define DDRPHY_MASK_TX_WRDATA_MASK_TX_WRDATA_MASK_S0  ((u32)0x0000FFFF << 0)             /*!<R/W 'h0000 1'b1 : mask per bit TX wrdata to 1'b0*/
#define DDRPHY_TX_WRDATA_MASK_TX_WRDATA_MASK_S0(x)    ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_RW_INT_STATUS
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_BIT_RW_INT_STATUS_TM_DQSEN_EARLY_INT  ((u32)0x00000001 << 31)  /*!<R/W 'h0 tm_dqs_en calibration early int*/
#define DDRPHY_BIT_RW_INT_STATUS_TM_DQSEN_LATE_INT   ((u32)0x00000001 << 30)  /*!<R/W 'h0 tm_dqs_en calibration late int*/
#define DDRPHY_BIT_RW_INT_STATUS_DQS_BURST_ERR_INT   ((u32)0x00000001 << 29)  /*!<R/W 'h0 dqs_burst_err_int*/
/** @} */

/**************************************************************************//**
 * @defgroup DDRPHY_READ_CTRL_0
 * @brief
 * @{
 *****************************************************************************/
#define DDRPHY_MASK_READ_CTRL_0_DQS_RC1IN_DLY_SEL  ((u32)0x0000001F << 26)            /*!<R/W 'h0 dqs clock to receive_1st delay control*/
#define DDRPHY_READ_CTRL_0_DQS_RC1IN_DLY_SEL(x)    ((u32)(((x) & 0x0000001F) << 26))
#define DDRPHY_MASK_READ_CTRL_0_DQS_GLITCH_DLY_PH  ((u32)0x0000000F << 20)            /*!<R/W 'h0 Control delay taps to detect DQS glitch.*/
#define DDRPHY_READ_CTRL_0_DQS_GLITCH_DLY_PH(x)    ((u32)(((x) & 0x0000000F) << 20))
#define DDRPHY_MASK_READ_CTRL_0_TM_DQ_LAT_DLY      ((u32)0x0000000F << 16)            /*!<R/W 'h0 programmable dq_lat_en_cmd start time 2T DCK unit for 2to1 mode 1T DCK unit for 1to1 mode*/
#define DDRPHY_READ_CTRL_0_TM_DQ_LAT_DLY(x)        ((u32)(((x) & 0x0000000F) << 16))
#define DDRPHY_MASK_READ_CTRL_0_DQS_FIFO_DLY_SEL   ((u32)0x00000003 << 14)            /*!<R/W 'h0 dqs clock to read FIFO delay control*/
#define DDRPHY_READ_CTRL_0_DQS_FIFO_DLY_SEL(x)     ((u32)(((x) & 0x00000003) << 14))
#define DDRPHY_MASK_READ_CTRL_0_DQS_EN_DLY_SEL     ((u32)0x0000003F << 8)             /*!<R/W 'h00 dqs_en delay chain control (No use)*/
#define DDRPHY_READ_CTRL_0_DQS_EN_DLY_SEL(x)       ((u32)(((x) & 0x0000003F) << 8))
#define DDRPHY_MASK_READ_CTRL_0_TM_DQS_EN          ((u32)0x0000001F << 2)             /*!<R/W 'h00 dqs_en latency from read command 2T DCK unit for 2to1 mode 1T DCK unit for 1to1 mode*/
#define DDRPHY_READ_CTRL_0_TM_DQS_EN(x)            ((u32)(((x) & 0x0000001F) << 2))
#define DDRPHY_MASK_READ_CTRL_0_TM_DQS_EN_FTUN     ((u32)0x00000003 << 0)             /*!<R/W 'h3 dqs_en_ftun latency from read command 0.5T DCK unit*/
#define DDRPHY_READ_CTRL_0_TM_DQS_EN_FTUN(x)       ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAPRO3_ASIC_DDRPHY
 * @{
 * @brief AMEBAPRO3_ASIC_DDRPHY Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t CRT_CTL;                 /*!<  Register,  Address offset: 0x000 */
	__IO uint32_t PLL_CTL0;                /*!<  Register,  Address offset: 0x004 */
	__IO uint32_t PLL_CTL1;                /*!<  Register,  Address offset: 0x008 */
	__IO uint32_t PLL_CTL2;                /*!<  Register,  Address offset: 0x00C */
	__IO uint32_t PLL_PI0;                 /*!<  Register,  Address offset: 0x010 */
	__IO uint32_t PLL_PI1;                 /*!<  Register,  Address offset: 0x014 */
	__IO uint32_t PLL_CTL3;                /*!<  Register,  Address offset: 0x018 */
	__IO uint32_t SSC0;                    /*!<  Register,  Address offset: 0x01C */
	__IO uint32_t SSC1;                    /*!<  Register,  Address offset: 0x020 */
	__IO uint32_t SSC2;                    /*!<  Register,  Address offset: 0x024 */
	__IO uint32_t SSC3;                    /*!<  Register,  Address offset: 0x028 */
	__IO uint32_t WDOG;                    /*!<  Register,  Address offset: 0x02C */
	__IO uint32_t DCC_CMD;                 /*!<  Register,  Address offset: 0x030 */
	__IO uint32_t DCC_DQS[4];              /*!<  Register array,  Address offset: 0x034 - 0x040 */
	__IO uint32_t DCC_DQ[4];               /*!<  Register array,  Address offset: 0x044 - 0x050 */
	__IO uint32_t DCC_CK[2];               /*!<  Register array,  Address offset: 0x054 - 0x058 */
	__IO uint32_t RESERVED_0[1];           /*!< 0x05C - 0x05C */
	__IO uint32_t CRT_CTL_2;               /*!<  Register,  Address offset: 0x060 */
	__IO uint32_t RESERVED_1[5];           /*!< 0x064 - 0x074 */
	__IO uint32_t CRT_DMY;                 /*!<  Register,  Address offset: 0x078 */
	__IO uint32_t CRT_DMY1;                /*!<  Register,  Address offset: 0x07C */
	__IO uint32_t AFIFO_STR_0;             /*!<  Register,  Address offset: 0x080 */
	__IO uint32_t AFIFO_STR_1;             /*!<  Register,  Address offset: 0x084 */
	__IO uint32_t AFIFO_STR_2;             /*!<  Register,  Address offset: 0x088 */
	__IO uint32_t CRT_RST_CTL;             /*!<  Register,  Address offset: 0x08C */
	__IO uint32_t RESERVED_2[1];           /*!< 0x090 - 0x090 */
	__IO uint32_t AFIFO_STR_SEL;           /*!<  Register,  Address offset: 0x094 */
	__IO uint32_t PLL_PI2;                 /*!<  Register,  Address offset: 0x098 */
	__IO uint32_t PLL_CTL4;                /*!<  Register,  Address offset: 0x09C */
	__IO uint32_t PLL_PI3;                 /*!<  Register,  Address offset: 0x0A0 */
	__IO uint32_t DCC_CS;                  /*!<  Register,  Address offset: 0x0A4 */
	__IO uint32_t DCC_CS1;                 /*!<  Register,  Address offset: 0x0A8 */
	__IO uint32_t PLL_CTL5;                /*!<  Register,  Address offset: 0x0AC */
	__IO uint32_t RESERVED_3[20];          /*!< 0x0B0 - 0x0FC */
	__IO uint32_t ADR_DLY_0;               /*!<  Register,  Address offset: 0x100 */
	__IO uint32_t ADR_DLY_1;               /*!<  Register,  Address offset: 0x104 */
	__IO uint32_t ADR_DLY_2;               /*!<  Register,  Address offset: 0x108 */
	__IO uint32_t CMD_DLY_0;               /*!<  Register,  Address offset: 0x10C */
	__IO uint32_t CMD_DLY_1;               /*!<  Register,  Address offset: 0x110 */
	__IO uint32_t PAD_OE_CTL;              /*!<  Register,  Address offset: 0x114 */
	__IO uint32_t PAD_OE_CTL_EX;           /*!<  Register,  Address offset: 0x118 */
	__IO uint32_t PAD_ALERT_SEL_0;         /*!<  Register,  Address offset: 0x11C */
	__IO uint32_t PAD_ALERT_SEL_1;         /*!<  Register,  Address offset: 0x120 */
	__IO uint32_t PAD_ALERT_1_SEL_0;       /*!<  Register,  Address offset: 0x124 */
	__IO uint32_t PAD_ALERT_1_SEL_1;       /*!<  Register,  Address offset: 0x128 */
	__IO uint32_t PAD_IDDQ;                /*!<  Register,  Address offset: 0x12C */
	__IO uint32_t PAD_CTRL_PROG;           /*!<  Register,  Address offset: 0x130 */
	__IO uint32_t DQ_PAT_H_L_DET;          /*!<  Register,  Address offset: 0x134 */
	__IO uint32_t PAD_OE_GATE_CTL;         /*!<  Register,  Address offset: 0x138 */
	__IO uint32_t ZQ_NOCD2;                /*!<  Register,  Address offset: 0x13C */
	__IO uint32_t PAD_ZCTRL_STATUS;        /*!<  Register,  Address offset: 0x140 */
	__IO uint32_t PAD_ZCTRL_RESULT;        /*!<  Register,  Address offset: 0x144 */
	__IO uint32_t PAD_ZCTRL_RESULT_D;      /*!<  Register,  Address offset: 0x148 */
	__IO uint32_t PAD_RZCTRL_STATUS;       /*!<  Register,  Address offset: 0x14C */
	__IO uint32_t DQ_DQS_CTRL;             /*!<  Register,  Address offset: 0x150 */
	__IO uint32_t CMD_ADR_PH;              /*!<  Register,  Address offset: 0x154 */
	__IO uint32_t DQ_PAT_IN0;              /*!<  Register,  Address offset: 0x158 */
	__IO uint32_t DQ_PAT_IN1;              /*!<  Register,  Address offset: 0x15C */
	__IO uint32_t WRLVL_DQS_DATA;          /*!<  Register,  Address offset: 0x160 */
	__IO uint32_t TEST_CTRL0;              /*!<  Register,  Address offset: 0x164 */
	__IO uint32_t TEST_CTRL1;              /*!<  Register,  Address offset: 0x168 */
	__IO uint32_t HALF_CYC_DET;            /*!<  Register,  Address offset: 0x16C */
	__IO uint32_t READ_CTRL_1;             /*!<  Register,  Address offset: 0x170 */
	__IO uint32_t RESERVED_4[2];           /*!< 0x174 - 0x178 */
	__IO uint32_t DT_SET0;                 /*!<  Register,  Address offset: 0x17C */
	__IO uint32_t ODT_TTCP0_SET0;          /*!<  Register,  Address offset: 0x180 */
	__IO uint32_t ODT_TTCP1_SET0;          /*!<  Register,  Address offset: 0x184 */
	__IO uint32_t ODT_TTCN0_SET0;          /*!<  Register,  Address offset: 0x188 */
	__IO uint32_t ODT_TTCN1_SET0;          /*!<  Register,  Address offset: 0x18C */
	__IO uint32_t OCDP0_SET0;              /*!<  Register,  Address offset: 0x190 */
	__IO uint32_t OCDP1_SET0;              /*!<  Register,  Address offset: 0x194 */
	__IO uint32_t OCDN0_SET0;              /*!<  Register,  Address offset: 0x198 */
	__IO uint32_t OCDN1_SET0;              /*!<  Register,  Address offset: 0x19C */
	__IO uint32_t ODT_TTCP0_SET1;          /*!<  Register,  Address offset: 0x1A0 */
	__IO uint32_t ODT_TTCP1_SET1;          /*!<  Register,  Address offset: 0x1A4 */
	__IO uint32_t ODT_TTCN0_SET1;          /*!<  Register,  Address offset: 0x1A8 */
	__IO uint32_t ODT_TTCN1_SET1;          /*!<  Register,  Address offset: 0x1AC */
	__IO uint32_t OCDP0_SET1;              /*!<  Register,  Address offset: 0x1B0 */
	__IO uint32_t OCDP1_SET1;              /*!<  Register,  Address offset: 0x1B4 */
	__IO uint32_t OCDN0_SET1;              /*!<  Register,  Address offset: 0x1B8 */
	__IO uint32_t OCDN1_SET1;              /*!<  Register,  Address offset: 0x1BC */
	__IO uint32_t DQ_ODT_SEL[2];           /*!<  Register array,  Address offset: 0x1C0 - 0x1C4 */
	__IO uint32_t DQ_OCD_SEL[2];           /*!<  Register array,  Address offset: 0x1C8 - 0x1CC */
	__IO uint32_t DQS_P_ODT_SEL[2];        /*!<  Register array,  Address offset: 0x1D0 - 0x1D4 */
	__IO uint32_t DQS_N_ODT_SEL[2];        /*!<  Register array,  Address offset: 0x1D8 - 0x1DC */
	__IO uint32_t DQS_OCD_SEL[2];          /*!<  Register array,  Address offset: 0x1E0 - 0x1E4 */
	__IO uint32_t CKE_OCD_SEL;             /*!<  Register,  Address offset: 0x1E8 */
	__IO uint32_t ADR_OCD_SEL;             /*!<  Register,  Address offset: 0x1EC */
	__IO uint32_t CK_OCD_SEL;              /*!<  Register,  Address offset: 0x1F0 */
	__IO uint32_t PAD_BUS_0;               /*!<  Register,  Address offset: 0x1F4 */
	__IO uint32_t PAD_BUS_1;               /*!<  Register,  Address offset: 0x1F8 */
	__IO uint32_t PAD_BUS_2;               /*!<  Register,  Address offset: 0x1FC */
	__IO uint32_t ZQ_TRIM;                 /*!<  Register,  Address offset: 0x200 */
	__IO uint32_t OCD_SEL_0;               /*!<  Register,  Address offset: 0x204 */
	__IO uint32_t OCD_SEL_1;               /*!<  Register,  Address offset: 0x208 */
	__IO uint32_t OCD_SEL_2;               /*!<  Register,  Address offset: 0x20C */
	__IO uint32_t OCD_SEL_3;               /*!<  Register,  Address offset: 0x210 */
	__IO uint32_t OCD_SEL_4;               /*!<  Register,  Address offset: 0x214 */
	__IO uint32_t OCD_SEL_5;               /*!<  Register,  Address offset: 0x218 */
	__IO uint32_t OCD_SEL_6;               /*!<  Register,  Address offset: 0x21C */
	__IO uint32_t OCD_SEL_7;               /*!<  Register,  Address offset: 0x220 */
	__IO uint32_t OCD_SEL_8;               /*!<  Register,  Address offset: 0x224 */
	__IO uint32_t ODT_SEL_0;               /*!<  Register,  Address offset: 0x228 */
	__IO uint32_t CKE_ODT_SEL;             /*!<  Register,  Address offset: 0x22C */
	__IO uint32_t ADR_ODT_SEL;             /*!<  Register,  Address offset: 0x230 */
	__IO uint32_t CLK_ODT_SEL;             /*!<  Register,  Address offset: 0x234 */
	__IO uint32_t DPI_CTRL_0;              /*!<  Register,  Address offset: 0x238 */
	__IO uint32_t DPI_CTRL_1;              /*!<  Register,  Address offset: 0x23C */
	__IO uint32_t DPI_CTRL_2;              /*!<  Register,  Address offset: 0x240 */
	__IO uint32_t BIST_2TO1_0;             /*!<  Register,  Address offset: 0x244 */
	__IO uint32_t BIST_2TO1_1;             /*!<  Register,  Address offset: 0x248 */
	__IO uint32_t BIST_PT;                 /*!<  Register,  Address offset: 0x24C */
	__IO uint32_t BIST_DEBUG;              /*!<  Register,  Address offset: 0x250 */
	__IO uint32_t INT_CTRL;                /*!<  Register,  Address offset: 0x254 */
	__IO uint32_t INT_STATUS_0;            /*!<  Register,  Address offset: 0x258 */
	__IO uint32_t INT_STATUS_2;            /*!<  Register,  Address offset: 0x25C */
	__IO uint32_t SSC_STATUS;              /*!<  Register,  Address offset: 0x260 */
	__IO uint32_t PRBS_TMUX0;              /*!<  Register,  Address offset: 0x264 */
	__IO uint32_t PRBS_TMUX1;              /*!<  Register,  Address offset: 0x268 */
	__IO uint32_t PRBS_TMUX2;              /*!<  Register,  Address offset: 0x26C */
	__IO uint32_t PRBS_TMUX3;              /*!<  Register,  Address offset: 0x270 */
	__IO uint32_t PRBS_TMUX6;              /*!<  Register,  Address offset: 0x274 */
	__IO uint32_t PRBS_TMUX7;              /*!<  Register,  Address offset: 0x278 */
	__IO uint32_t PRBS_TMUX8;              /*!<  Register,  Address offset: 0x27C */
	__IO uint32_t PRBS_TMUX9;              /*!<  Register,  Address offset: 0x280 */
	__IO uint32_t PRBS_TMUX10;             /*!<  Register,  Address offset: 0x284 */
	__IO uint32_t DUMMY_0;                 /*!<  Register,  Address offset: 0x288 */
	__IO uint32_t DUMMY_1;                 /*!<  Register,  Address offset: 0x28C */
	__IO uint32_t DUMMY_2;                 /*!<  Register,  Address offset: 0x290 */
	__IO uint32_t DUMMY_3;                 /*!<  Register,  Address offset: 0x294 */
	__IO uint32_t CS_EMPH_CTL;             /*!<  Register,  Address offset: 0x298 */
	__IO uint32_t CS_SEL_EMPH_CTL;         /*!<  Register,  Address offset: 0x29C */
	__IO uint32_t DDR4_CS_EMPH_CTL;        /*!<  Register,  Address offset: 0x2A0 */
	__IO uint32_t CK_EMPH_CTL;             /*!<  Register,  Address offset: 0x2A4 */
	__IO uint32_t CK_SEL_EMPH_CTL;         /*!<  Register,  Address offset: 0x2A8 */
	__IO uint32_t ADR_EMPH_CTL[2];         /*!<  Register array,  Address offset: 0x2AC - 0x2B0 */
	__IO uint32_t ADR_SEL_EMPH_CTL[2];     /*!<  Register array,  Address offset: 0x2B4 - 0x2B8 */
	__IO uint32_t ADR_EMPH_SEL_0;          /*!<  Register,  Address offset: 0x2BC */
	__IO uint32_t ADR_EMPH_SEL_1;          /*!<  Register,  Address offset: 0x2C0 */
	__IO uint32_t ADR_EMPH_SEL_2;          /*!<  Register,  Address offset: 0x2C4 */
	__IO uint32_t RESERVED_5[1];           /*!< 0x2C8 - 0x2C8 */
	__IO uint32_t DBG_READ_1;              /*!<  Register,  Address offset: 0x2CC */
	__IO uint32_t RX_CAL_CTRL_0;           /*!<  Register,  Address offset: 0x2D0 */
	__IO uint32_t RX_CAL_CTRL_1;           /*!<  Register,  Address offset: 0x2D4 */
	__IO uint32_t VREF_BDY_0;              /*!<  Register,  Address offset: 0x2D8 */
	__IO uint32_t VREF_BDY_1;              /*!<  Register,  Address offset: 0x2DC */
	__IO uint32_t RESERVED_6[1];           /*!< 0x2E0 - 0x2E0 */
	__IO uint32_t RX_CAL_MON;              /*!<  Register,  Address offset: 0x2E4 */
	__IO uint32_t PAD_CTRL_ZPROG[8];       /*!<  Register array,  Address offset: 0x2E8 - 0x304 */
	__IO uint32_t PAD_NOCD2_ZPROG[8];      /*!<  Register array,  Address offset: 0x308 - 0x324 */
	__IO uint32_t DQ_OCD_SEL_2[2];         /*!<  Register array,  Address offset: 0x328 - 0x32C */
	__IO uint32_t DQS_OCD_SEL_2[2];        /*!<  Register array,  Address offset: 0x330 - 0x334 */
	__IO uint32_t DBG_READ_2;              /*!<  Register,  Address offset: 0x338 */
	__IO uint32_t DATA_VAL_CTRL_0;         /*!<  Register,  Address offset: 0x33C */
	__IO uint32_t DATA_VAL_CTRL_1;         /*!<  Register,  Address offset: 0x340 */
	__IO uint32_t DATA_VAL_CTRL_2;         /*!<  Register,  Address offset: 0x344 */
	__IO uint32_t CA_CRC_CTRL;             /*!<  Register,  Address offset: 0x348 */
	__IO uint32_t CA_CRC_OUT;              /*!<  Register,  Address offset: 0x34C */
	__IO uint32_t ODT_TTCP2_SET0;          /*!<  Register,  Address offset: 0x350 */
	__IO uint32_t ODT_TTCN2_SET0;          /*!<  Register,  Address offset: 0x354 */
	__IO uint32_t OCDP2_SET0;              /*!<  Register,  Address offset: 0x358 */
	__IO uint32_t OCDN2_SET0;              /*!<  Register,  Address offset: 0x35C */
	__IO uint32_t ODT_TTCP2_SET1;          /*!<  Register,  Address offset: 0x360 */
	__IO uint32_t ODT_TTCN2_SET1;          /*!<  Register,  Address offset: 0x364 */
	__IO uint32_t OCDP2_SET1;              /*!<  Register,  Address offset: 0x368 */
	__IO uint32_t OCDN2_SET1;              /*!<  Register,  Address offset: 0x36C */
	__IO uint32_t ODT_SEL_EX_0;            /*!<  Register,  Address offset: 0x370 */
	__IO uint32_t ODT_SEL_EX_1;            /*!<  Register,  Address offset: 0x374 */
	__IO uint32_t OCD_SEL_EX_0;            /*!<  Register,  Address offset: 0x378 */
	__IO uint32_t OCD_SEL_EX_1;            /*!<  Register,  Address offset: 0x37C */
	__IO uint32_t OCD_SEL_EX_2;            /*!<  Register,  Address offset: 0x380 */
	__IO uint32_t OCD_SEL_EX_3;            /*!<  Register,  Address offset: 0x384 */
	__IO uint32_t OCD_SEL_EX_4;            /*!<  Register,  Address offset: 0x388 */
	__IO uint32_t RESERVED_7[29];          /*!< 0x38C - 0x3FC */
	__IO uint32_t RW_DMY_A;                /*!<  Register,  Address offset: 0x400 */
	__IO uint32_t RW_DMY_B;                /*!<  Register,  Address offset: 0x404 */
	__IO uint32_t READ_CTRL_2_0;           /*!<  Register,  Address offset: 0x408 */
	__IO uint32_t READ_CTRL_2_1;           /*!<  Register,  Address offset: 0x40C */
	__IO uint32_t READ_CTRL_3[2];          /*!<  Register array,  Address offset: 0x410 - 0x414 */
	__IO uint32_t READ_CTRL_6_0;           /*!<  Register,  Address offset: 0x418 */
	__IO uint32_t READ_CTRL_6_1;           /*!<  Register,  Address offset: 0x41C */
	__IO uint32_t DEL_POS_SEL[2];          /*!<  Register array,  Address offset: 0x420 - 0x424 */
	__IO uint32_t DEL_POS_SEL_DBI[2];      /*!<  Register array,  Address offset: 0x428 - 0x42C */
	__IO uint32_t DEL_NEG_SEL[2];          /*!<  Register array,  Address offset: 0x430 - 0x434 */
	__IO uint32_t DEL_NEG_SEL_DBI[2];      /*!<  Register array,  Address offset: 0x438 - 0x43C */
	__IO uint32_t DQS_IN_DLY_0[2];         /*!<  Register array,  Address offset: 0x440 - 0x444 */
	__IO uint32_t DQS_IN_DLY_1[2];         /*!<  Register array,  Address offset: 0x448 - 0x44C */
	__IO uint32_t DQS_IN_DLY_1_DBI[2];     /*!<  Register array,  Address offset: 0x450 - 0x454 */
	__IO uint32_t DQS_IN_DLY_2[2];         /*!<  Register array,  Address offset: 0x458 - 0x45C */
	__IO uint32_t DQS_IN_DLY_3[2];         /*!<  Register array,  Address offset: 0x460 - 0x464 */
	__IO uint32_t DQS_IN_DLY_3_DBI[2];     /*!<  Register array,  Address offset: 0x468 - 0x46C */
	__IO uint32_t VALID_WIN_DET_PFIFO[2];  /*!<  Register array,  Address offset: 0x470 - 0x474 */
	__IO uint32_t VALID_WIN_DET_NFIFO[2];  /*!<  Register array,  Address offset: 0x478 - 0x47C */
	__IO uint32_t DQ_DLY_0[2];             /*!<  Register array,  Address offset: 0x480 - 0x484 */
	__IO uint32_t DQ_DLY_1[2];             /*!<  Register array,  Address offset: 0x488 - 0x48C */
	__IO uint32_t CAL_OUT_0[2];            /*!<  Register array,  Address offset: 0x490 - 0x494 */
	__IO uint32_t CAL_OUT_1[2];            /*!<  Register array,  Address offset: 0x498 - 0x49C */
	__IO uint32_t WR_FIFO_0[2];            /*!<  Register array,  Address offset: 0x4A0 - 0x4A4 */
	__IO uint32_t WR_FIFO_1[2];            /*!<  Register array,  Address offset: 0x4A8 - 0x4AC */
	__IO uint32_t RD_FIFO_0[2];            /*!<  Register array,  Address offset: 0x4B0 - 0x4B4 */
	__IO uint32_t RD_FIFO_1[2];            /*!<  Register array,  Address offset: 0x4B8 - 0x4BC */
	__IO uint32_t DBI_FIFO[2];             /*!<  Register array,  Address offset: 0x4C0 - 0x4C4 */
	__IO uint32_t INT_STATUS_1[2];         /*!<  Register array,  Address offset: 0x4C8 - 0x4CC */
	__IO uint32_t INT_STATUS_3[2];         /*!<  Register array,  Address offset: 0x4D0 - 0x4D4 */
	__IO uint32_t PRBS_TMUX4[2];           /*!<  Register array,  Address offset: 0x4D8 - 0x4DC */
	__IO uint32_t PRBS_TMUX5[2];           /*!<  Register array,  Address offset: 0x4E0 - 0x4E4 */
	__IO uint32_t RW_EMPH_CTL[2];          /*!<  Register array,  Address offset: 0x4E8 - 0x4EC */
	__IO uint32_t RW_SEL_EMPH_CTL[2];      /*!<  Register array,  Address offset: 0x4F0 - 0x4F4 */
	__IO uint32_t DQ_IO_CTL_0[2];          /*!<  Register array,  Address offset: 0x4F8 - 0x4FC */
	__IO uint32_t DQ_IO_CTL_1[2];          /*!<  Register array,  Address offset: 0x500 - 0x504 */
	__IO uint32_t READ_DBG_CTRL[2];        /*!<  Register array,  Address offset: 0x508 - 0x50C */
	__IO uint32_t MON[2];                  /*!<  Register array,  Address offset: 0x510 - 0x514 */
	__IO uint32_t DQ_IO_CTL_2[2];          /*!<  Register array,  Address offset: 0x518 - 0x51C */
	__IO uint32_t READ_CTRL_4[2];          /*!<  Register array,  Address offset: 0x520 - 0x524 */
	__IO uint32_t DEL_POS_SEL_RIGH[2];     /*!<  Register array,  Address offset: 0x528 - 0x52C */
	__IO uint32_t DEL_POS_SEL_RIGH_DBI[2]; /*!<  Register array,  Address offset: 0x530 - 0x534 */
	__IO uint32_t DEL_NEG_SEL_RIGH[2];     /*!<  Register array,  Address offset: 0x538 - 0x53C */
	__IO uint32_t DEL_NEG_SEL_RIGH_DBI[2]; /*!<  Register array,  Address offset: 0x540 - 0x544 */
	__IO uint32_t DQSEN_DBG_READ[2];       /*!<  Register array,  Address offset: 0x548 - 0x54C */
	__IO uint32_t RD_DBG_0_READ[2];        /*!<  Register array,  Address offset: 0x550 - 0x554 */
	__IO uint32_t RD_DBG_1_READ[2];        /*!<  Register array,  Address offset: 0x558 - 0x55C */
	__IO uint32_t DQ_PRBS_TMUX;            /*!<  Register,  Address offset: 0x560 */
	__IO uint32_t RW_CTRL;                 /*!<  Register,  Address offset: 0x564 */
	__IO uint32_t RESERVED_8[1];           /*!< 0x568 - 0x568 */
	__IO uint32_t TX_WRDATA_MASK;          /*!<  Register,  Address offset: 0x56C */
	__IO uint32_t RW_INT_STATUS[2];        /*!<  Register array,  Address offset: 0x570 - 0x574 */
	__IO uint32_t RESERVED_9[32];          /*!< 0x578 - 0x5F4 */
	__IO uint32_t READ_CTRL_0[2];          /*!<  Register array,  Address offset: 0x5F8 - 0x5FC */
} DDRPHY_ASIC_TypeDef;

/* PLL SSC helper — used by DDR_PHY_SetPLLFreq to solve N_CODE_T / F_CODE_T
 * from the target frequency and the on-chip XTAL. */
#define DDR_PHY_XTAL_MHZ        40U     /* On-chip reference crystal */
#define DDR_PHY_FRAC_DIVISOR    2048U   /* F_CODE_T denominator      */
#define DDR_PHY_N_CODE_OFFSET   3U      /* N_CODE_T = multiplier - 3 */

/* Supported operating range. Enforced with assert_param so a bad call is
 * caught during bring-up rather than silently landing on a hazardous PLL
 * code combination. */
#define DDR_PHY_MIN_FREQ_MHZ    533U
#define DDR_PHY_MAX_FREQ_MHZ    933U

/* DCC/DLL common preset: div=5, int-gain=5, duty=0x38, preset_a=3,
 * preset_b=3, DSP stop asserted. Used by every DCC register so the
 * shared constants appear once. */
#define DDR_PHY_DCC_DIV_COMMON        0x5
#define DDR_PHY_DCC_INT_GAIN_COMMON   0x5
#define DDR_PHY_DCC_DUTY_COMMON       0x38
#define DDR_PHY_DCC_PRESET_A_COMMON   0x3
#define DDR_PHY_DCC_PRESET_B_COMMON   0x3

/* Clock / reset control. Driver-internal: sequenced by the DDR_* API in ameba_ddr.c. */
_LONG_CALL_ void DDR_PHY_Clock(u32 NewState);
_LONG_CALL_ void DDR_PHY_Reset(void);
_LONG_CALL_ void DDR_PHY_PLLOff(void);
_LONG_CALL_ u32 DDR_PHY_GetPLLFreq(void);
_LONG_CALL_ void DDR_PHY_ASIC_Init(bool odt_calibration);
_LONG_CALL_ void DDR_PHY_FPGA_Init(bool odt_calibration);
_LONG_CALL_ void DDR_PHY_SetPLLFreq(u32 freq_mhz);

/**
  * @brief  DDR PHY operations vtable.
  *         Abstracts the DDR PHY IP from the controller layer (ameba_ddrc.c)
  *         and the top-level driver (ameba_ddr.c). Each target provides its
  *         own instance, e.g. DDR_PHY_ASIC_Ops (silicon) or
  *         DDR_PHY_FPGA_Ops (emulator).
  */
typedef struct {
	void (*Init)(bool odt_calibration);
} DDR_PHY_OPS_TypeDef;

extern const DDR_PHY_OPS_TypeDef DDR_PHY_ASIC_Ops;
extern const DDR_PHY_OPS_TypeDef DDR_PHY_FPGA_Ops;

#endif
