/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SD_HOST_H
#define _AMEBA_SD_HOST_H

#include "hal_platform.h"

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SD HOST
  * @brief SD HOST driver modules
  * @{
  */

/* Registers Definitions --------------------------------------------------------*/
/**
 * @defgroup SD HOST Register_Definitions SD HOST Register Definitions
 * @{
 **/

/**
 * @defgroup SRAM_CTL
 * @{
 **/
#define SD_HOST_BIT_LX_SWAP                 ((u32)0x00000001 << 8)
#define SD_HOST_LX_SWAP(x)                  ((u32)(((x) & 0x00000001) << 8))
#define SD_HOST_MASK_LX_BURST_SIZE          ((u32)0x00000003 << 6)
#define SD_HOST_LX_BURST_SIZE(x)            ((u32)(((x) & 0x00000003) << 6))
#define SD_HOST_BIT_MAP_SEL                 ((u32)0x00000001 << 5)
#define SD_HOST_MAP_SEL(x)                  ((u32)(((x) & 0x00000001) << 5))
#define SD_HOST_BIT_ACCESS_EN               ((u32)0x00000001 << 4)
#define SD_HOST_ACCESS_EN(x)                ((u32)(((x) & 0x00000001) << 4))
#define SD_HOST_MASK_MEM_REGION             ((u32)0x0000000f << 0)
#define SD_HOST_MEM_REGION(x)               ((u32)(((x) & 0x0000000f) << 0))
/** @} */

/**
 * @defgroup DMA_CTL1
 * @{
 **/
#define SD_HOST_MASK_DRAM_SA                ((u32)0x1FFFFFFF << 0)
#define SD_HOST_DRAM_SA(x)                  ((u32)(((x) & 0x1FFFFFFF) << 0))
/** @} */

/**
 * @defgroup DMA_CTL2
 * @{
 **/
#define SD_HOST_MASK_DMA_LEN                ((u32)0x0000FFFF << 0)
#define SD_HOST_DMA_LEN(x)                  ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */

/**
 * @defgroup DMA_CTL3
 * @{
 **/
#define SD_HOST_BIT_DATA64_SEL              ((u32)0x00000001 << 5)
#define SD_HOST_DATA64_SEL(x)               ((u32)(((x) & 0x00000001) << 5))
#define SD_HOST_BIT_RSP17_SEL               ((u32)0x00000001 << 4)
#define SD_HOST_RSP17_SEL(x)                ((u32)(((x) & 0x00000001) << 4))
#define SD_HOST_BIT_DDR_WR                  ((u32)0x00000001 << 1)
#define SD_HOST_DDR_WR(x)                   ((u32)(((x) & 0x00000001) << 1))
#define SD_HOST_BIT_DMA_XFER                ((u32)0x00000001 << 0)
#define SD_HOST_DMA_XFER(x)                 ((u32)(((x) & 0x00000001) << 0))
/** @} */

/**
 * @defgroup SYS_LOW_PWR
 * @{
 **/
#define SD_HOST_BIT_MCU_PP_SRAM_LP_ENA      ((u32)0x00000001 << 7)
#define SD_HOST_MCU_PP_SRAM_LP_ENA(x)       ((u32)(((x) & 0x00000001) << 7))
#define SD_HOST_BIT_SYS_CLK_GATE_ENA        ((u32)0x00000001 << 6)
#define SD_HOST_SYS_CLK_GATE_ENA(x)         ((u32)(((x) & 0x00000001) << 6))
#define SD_HOST_BIT_DMA_SRAM_LP_ENA         ((u32)0x00000001 << 4)
#define SD_HOST_DMA_SRAM_LP_ENA(x)          ((u32)(((x) & 0x00000001) << 4))
#define SD_HOST_MASK_DMA_SRAM_RDY_NUM       ((u32)0x0000000F << 0)
#define SD_HOST_DMA_SRAM_RDY_NUM(x)         ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**
 * @defgroup DMA_CTL4
 * @{
 **/
#define SD_HOST_MASK_DMA_INFO               ((u32)0x000000FF << 16)
#define SD_HOST_DMA_INFO(x)                 ((u32)(((x) & 0x000000FF) << 16))
#define SD_HOST_BIT_DMA_SOFT_RSTN           ((u32)0x00000001 << 0)
#define SD_HOST_DMA_SOFT_RSTN(x)            ((u32)(((x) & 0x00000001) << 0))
/** @} */

/**
 * @defgroup ISR
 * @{
 **/
#define SD_HOST_BIT_INT4                    ((u32)0x00000001 << 4)
#define SD_HOST_BIT_INT2                    ((u32)0x00000001 << 2)
#define SD_HOST_BIT_INT1                    ((u32)0x00000001 << 1)
#define SD_HOST_BIT_WRITE_DATA              ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup ISREN
 * @{
 **/
#define SD_HOST_BIT_INT4_EN                 ((u32)0x00000001 << 4)
#define SD_HOST_BIT_INT2_EN                 ((u32)0x00000001 << 2)
#define SD_HOST_BIT_INT1_EN                 ((u32)0x00000001 << 1)
#define SD_HOST_BIT_WRITE_DATA_EN           ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup DBG
 * @{
 **/
#define SD_HOST_BIT_WRITE_ENABLE4           ((u32)0x00000001 << 12)
#define SD_HOST_MASK_CR_DBG_SEL             ((u32)0x00000003 << 10)
#define SD_HOST_CR_DBG_SEL(x)               ((u32)(((x) & 0x00000003) << 10))
#define SD_HOST_BIT_WRITE_ENABLE3           ((u32)0x00000001 << 9)
#define SD_HOST_MASK_SEL1                   ((u32)0x00000007 << 6)
#define SD_HOST_SEL1(x)                     ((u32)(((x) & 0x00000007) << 6))
#define SD_HOST_BIT_WRITE_ENABLE2           ((u32)0x00000001 << 5)
#define SD_HOST_MASK_SEL0                   ((u32)0x00000007 << 2)
#define SD_HOST_SEL0(x)                     ((u32)(((x) & 0x00000007) << 2))
#define SD_HOST_BIT_WRITE_ENABLE1           ((u32)0x00000001 << 1)
#define SD_HOST_BIT_ENABLE                  ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup IP_CTL
 * @{
 **/
#define SD_HOST_MASK_ASIC_CRC_DBGO_SEL       ((u32)0x000000FF << 8)
#define SD_HOST_ASIC_CRC_DBGO_SEL(x)         ((u32)(((x) & 0x000000FF) << 8))
#define SD_HOST_MASK_CRC_DBGO_SEL            ((u32)0x0000000F << 3)
#define SD_HOST_CRC_DBGO_SEL(x)              ((u32)(((x) & 0x0000000F) << 3))
#define SD_HOST_BIT_IP_EA_FLASH              ((u32)0x00000001 << 2)
#define SD_HOST_BIT_CRC_CLK_DISABLE_TRIG     ((u32)0x00000001 << 1)
#define SD_HOST_BIT_MCU_TIME_1_US            ((u32)0x00000001 << 0)
/** @} */


/**
 * @defgroup PAD_CTL
 * @{
 **/
#define SD_HOST_BIT_PAD_TUNE3318            ((u32)0x00000001 << 0)  ///< 1: 3.3V pad, 0: 1.8V pad
/** @} */

/**
 * @defgroup CKGEN_CTL
 * @{
 **/
#define SD_HOST_BIT_SD30_SAMPLE_CHANGE      ((u32)0x00000001 << 18)
#define SD_HOST_BIT_SD30_PUSH_CHANGE        ((u32)0x00000001 << 17)
#define SD_HOST_BIT_CRC_CLK_CHANGE          ((u32)0x00000001 << 16)
#define SD_HOST_MASK_SD30_SAMPLE_CLK_SRC    ((u32)0x00000003 << 12)
#define SD_HOST_SD30_SAMPLE_CLK_SRC(x)      ((u32)(((x) & 0x00000003) << 12))
#define SD_HOST_MASK_SD30_PUSH_CLK_SRC      ((u32)0x00000003 << 8)
#define SD_HOST_SD30_PUSH_CLK_SRC(x)        ((u32)(((x) & 0x00000003) << 8))
#define SD_HOST_MASK_CRC_CLK_SRC            ((u32)0x00000003 << 4)
#define SD_HOST_CRC_CLK_SRC(x)              ((u32)(((x) & 0x00000003) << 4))
#define SD_HOST_MASK_CLK_DIV                ((u32)0x00000007 << 0)
#define SD_HOST_CLK_DIV(x)                  ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**
 * @defgroup CARD_STOP
 * @{
 **/
#define SD_HOST_BIT_TARGET_MODULE_IS_SDMMC        ((u32)0x00000001 << 2)
/** @} */

/**
 * @defgroup MUX_ADDR_SEL
 * @{
 **/
#define SD_HOST_BIT_MS_MUX_ADDR_SEL                ((u32)0x00000001 << 0)
#define SD_HOST_BIT_SD_MUX_ADDR_SEL                ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup CARD_DMA1_CTL
 * @{
 **/
#define SD_HOST_BIT_DMA1_BUS_WIDTH                ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup CARD_DATA_SOURCE
 * @{
 **/
#define SD_HOST_BIT_DATA_SOURCE                   ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup CARD_SELECT
 * @{
 **/
#define SD_HOST_MASK_CARD_SELECT                  ((u32)0x00000007 << 0)
#define SD_HOST_CARD_SELECT(x)                    ((u32)(((x) & 0x00000007) << 0))
/** @} */

/**
 * @defgroup CARD_EXIST
 * @{
 **/
#define SD_HOST_BIT_XD_WP_INPUT                   ((u32)0x00000001 << 6)
#define SD_HOST_BIT_SD_WRITE_PROTECT              ((u32)0x00000001 << 5)
#define SD_HOST_BIT_SD_EXISTENCE                  ((u32)0x00000001 << 2)
/** @} */

/**
 * @defgroup CARD_INT_EN
 * @{
 **/
#define SD_HOST_BIT_SD_MMC_INT_EN                 ((u32)0x00000001 << 2)
/** @} */

/**
 * @defgroup CARD_INT_PEND
 * @{
 **/
#define SD_HOST_BIT_SD_MMC_INT_PENDING            ((u32)0x00000001 << 2)
/** @} */

/**
 * @defgroup CARD_INT_PEND_2
 * @{
 **/
#define SD_HOST_BIT_CDINT_ERROR                   ((u32)0x00000001 << 7)
#define SD_HOST_BIT_HW_AUTO_STOP_EN               ((u32)0x00000001 << 1)
/** @} */

/**
 * @defgroup CLK_EN_SETTING_STAT
 * @{
 **/
#define SD_HOST_BIT_CRC_CLK_AUTO_DISABLE_EN           ((u32)0x00000001 << 7)
#define SD_HOST_BIT_HW_AUTO_SWITCH_TO_DMA_BUFFER_EN   ((u32)0x00000001 << 6)
#define SD_HOST_BIT_FINAL_SD_CARD_MODULE_BY_FW_OR_HW  ((u32)0x00000001 << 2)
/** @} */

/**
 * @defgroup CARD_EXIST_2
 * @{
 **/
#define SD_HOST_MASK_FINAL_CARD_SELECT            ((u32)0x00000007 << 5)
/** @} */

/**
 * @defgroup CARD_CLOCK_EN_CTL
 * @{
 **/
#define SD_HOST_BIT_AUTO_CBW_MODE                 ((u32)0x00000001 << 7)
#define SD_HOST_BIT_SWITCH_CLK_MODE               ((u32)0x00000001 << 6)
#define SD_HOST_BIT_SD_CARD_MODULE_CLK_EN         ((u32)0x00000001 << 2)
/** @} */

/**
 * @defgroup SD_CONFIGURE1
 * @{
 **/
#define SD_HOST_BIT_INITIAL_MODE                  ((u32)0x00000001 << 7)
#define SD_HOST_BIT_CLK_DIVDER                    ((u32)0x00000001 << 6)
#define SD_HOST_BIT_SD30_ASYNC_FIFO_RST_N         ((u32)0x00000001 << 4)
#define SD_HOST_MASK_MODE_SELECTION               ((u32)0x00000003 << 2)
#define SD_HOST_MODE_SELECTION(x)                 ((u32)(((x) & 0x00000003) << 2))
#define SD_HOST_MASK_BUS_WIDTH                    ((u32)0x00000003 << 0)
#define SD_HOST_BUS_WIDTH(x)                      ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**
 * @defgroup SD_CONFIGURE2
 * @{
 **/
#define SD_HOST_BIT_CRC7_CALCULATION_EN                     ((u32)0x00000001 << 7)
#define SD_HOST_CRC7_CALCULATION_EN(x)                      ((u32)(((x) & 0x00000001) << 7))
#define SD_HOST_BIT_CRC16_CHECK_EN                          ((u32)0x00000001 << 6)
#define SD_HOST_CRC16_CHECK_EN(x)                           ((u32)(((x) & 0x00000001) << 6))
#define SD_HOST_BIT_WAIT_WRITE_CRC_STATUS_TIMEOUT_EN        ((u32)0x00000001 << 5)
#define SD_HOST_WAIT_WRITE_CRC_STATUS_TIMEOUT_EN(x)         ((u32)(((x) & 0x00000001) << 5))
#define SD_HOST_BIT_IGNORE_WRITE_CRC_ERROR_EN               ((u32)0x00000001 << 4)
#define SD_HOST_IGNORE_WRITE_CRC_ERROR_EN(x)                ((u32)(((x) & 0x00000001) << 4))
#define SD_HOST_BIT_WAIT_BUSY_END_EN                        ((u32)0x00000001 << 3)
#define SD_HOST_WAIT_BUSY_END_EN(x)                         ((u32)(((x) & 0x00000001) << 3))
#define SD_HOST_BIT_CRC7_CHECK_EN                           ((u32)0x00000001 << 2)
#define SD_HOST_CRC7_CHECK_EN(x)                            ((u32)(((x) & 0x00000001) << 2))
#define SD_HOST_MASK_RESPONSE_TYPE_CONFIG                   ((u32)0x00000003 << 0)
#define SD_HOST_RESPONSE_TYPE_CONFIG(x)                     ((u32)(((x) & 0x00000003) << 0))
/** @} */

/**
 * @defgroup SD_CONFIGURE3
 * @{
 **/
#define SD_HOST_BIT_STOP_CMD_START_NO_WAIT_BUSY                   ((u32)0x00000001 << 7)
#define SD_HOST_STOP_CMD_START_NO_WAIT_BUSY(x)                    ((u32)(((x) & 0x00000001) << 7))
#define SD_HOST_BIT_CMD_START_WAIT_NO_CARD_BUSY                   ((u32)0x00000001 << 6)
#define SD_HOST_CMD_START_WAIT_NO_CARD_BUSY(x)                    ((u32)(((x) & 0x00000001) << 6))
#define SD_HOST_BIT_DATA_PHASE_WAIT_CARD_BUSY_EN                  ((u32)0x00000001 << 5)
#define SD_HOST_DATA_PHASE_WAIT_CARD_BUSY_EN(x)                   ((u32)(((x) & 0x00000001) << 5))
#define SD_HOST_BIT_SD30_CLK_STOP_AFTER_DATA_TRANSFER_OVER_EN     ((u32)0x00000001 << 4)
#define SD_HOST_SD30_CLK_STOP_AFTER_DATA_TRANSFER_OVER_EN(x)      ((u32)(((x) & 0x00000001) << 4))
#define SD_HOST_BIT_SD20_CLK_STOP_AFTER_DATA_TRANSFER_OVER_EN     ((u32)0x00000001 << 3)
#define SD_HOST_SD20_CLK_STOP_AFTER_DATA_TRANSFER_OVER_EN(x)      ((u32)(((x) & 0x00000001) << 3))
#define SD_HOST_BIT_SD_CMD_RESPONSE_CHECK_EN                      ((u32)0x00000001 << 2)
#define SD_HOST_SD_CMD_RESPONSE_CHECK_EN(x)                       ((u32)(((x) & 0x00000001) << 2))
#define SD_HOST_BIT_ADDRESS_MODE                                  ((u32)0x00000001 << 1)
#define SD_HOST_ADDRESS_MODE(x)                                   ((u32)(((x) & 0x00000001) << 1))
#define SD_HOST_BIT_SD_CMD_RESPONSE_TIMEOUT_EN                    ((u32)0x00000001 << 0)
#define SD_HOST_SD_CMD_RESPONSE_TIMEOUT_EN(x)                     ((u32)(((x) & 0x00000001) << 0))
/** @} */

/**
 * @defgroup SD_STATUS1
 * @{
 **/
#define SD_HOST_BIT_CRC7_ERROR                            ((u32)0x00000001 << 7)
#define SD_HOST_BIT_CRC16_ERROR                           ((u32)0x00000001 << 6)
#define SD_HOST_BIT_WRITE_CRC_ERROR                       ((u32)0x00000001 << 5)
#define SD_HOST_MASK_STATUS_OF_WRITE_CRC_STATUS           ((u32)0x00000007 << 2)
#define SD_HOST_BIT_GET_WRITE_CRC_STATUS_TIMEOUT_ERROR    ((u32)0x00000001 << 1)
#define SD_HOST_BIT_SD_TUNING_PATTERN_COMPARE_ERROR       ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_STATUS2
 * @{
 **/
#define SD_HOST_MASK_CBW_STATE_MACHINE                    ((u32)0x0000000F << 2)
#define SD_HOST_BIT_SD_CMD_RESPONSE_INVALID               ((u32)0x00000001 << 1)
#define SD_HOST_BIT_SD_CMD_RESPONSE_TIMEOUT_ERROR         ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_BUS_STATUS
 * @{
 **/
#define SD_HOST_BIT_SD_CLK_TOGGLE_EN                                  ((u32)0x00000001 << 7)
#define SD_HOST_SD_CLK_TOGGLE_EN(x)                                   ((u32)(((x) & 0x00000001) << 7))
#define SD_HOST_BIT_STOP_SD_CLK_TOGGLING_WHEN_NO_CMD_DATA_TRANSFER    ((u32)0x00000001 << 6)
#define SD_HOST_MASK_DATA3_TO_DATA0                                   ((u32)0x0000000F << 1)
#define SD_HOST_BIT_CMD                                               ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_CMD_CODE
 * @{
 **/
#define SD_HOST_MASK_WRITE_CMD_CODE                       ((u32)0x0000000F << 4)
#define SD_HOST_WRITE_CMD_CODE(x)                         ((u32)(((x) & 0x0000000F) << 4))
#define SD_HOST_MASK_READ_CMD_CODE                        ((u32)0x0000000F << 0)
#define SD_HOST_READ_CMD_CODE(x)                          ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**
 * @defgroup SD_SAMPLE_POINT_CTL
 * @{
 **/
#define SD_HOST_BIT_SAMPLE_POINT_TYPE_FOR_DDR_DAT             ((u32)0x00000001 << 7)
#define SD_HOST_BIT_SAMPLE_POINT_SELECTION_FOR_DDR_DAT        ((u32)0x00000001 << 6)
#define SD_HOST_BIT_SAMPLE_POINT_TYPE_FOR_DDR_CMD             ((u32)0x00000001 << 5)
#define SD_HOST_BIT_SAMPLE_POINT_SELECTION_FOR_DDR_CMD        ((u32)0x00000001 << 4)
#define SD_HOST_BIT_SAMPLE_POINT_SELECTION_FOR_SD20_CMD_DATA  ((u32)0x00000001 << 3)
#define SD_HOST_DDR50_SAMPLE_POINT  (SD_HOST_BIT_SAMPLE_POINT_SELECTION_FOR_DDR_DAT | \
                                     SD_HOST_BIT_SAMPLE_POINT_SELECTION_FOR_DDR_CMD)  /* 0x50 */
/** @} */

/**
 * @defgroup SD_SAMPLE_POINT_CTL
 * @{
 **/
#define SD_HOST_BIT_PUSH_POINT_SELECTION_TYPE_FOR_DDR_CMD_DAT     ((u32)0x00000001 << 7)
#define SD_HOST_BIT_PUSH_POINT_SELECTION_FOR_DDR_MODE_DAT         ((u32)0x00000001 << 6)
#define SD_HOST_BIT_PUSH_POINT_SELECTION_FOR_DDR_MODE_CMD         ((u32)0x00000001 << 5)
#define SD_HOST_BIT_PUSH_POINT_SELECTION_FOR_SD20_MODE_CMD_DATA   ((u32)0x00000001 << 4)
#define SD_HOST_BIT_SAMPLE_EDGE_OF_PCLK_TO_SAMPLE_DPCLK           ((u32)0x00000001 << 3)
#define SD_HOST_MASK_SDCLK_POLARITY_SOLUTION_FOR_DDR              ((u32)0x00000003 << 1)
#define SD_HOST_BIT_SDCLK_POLARITY_SWAP                           ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_BYTE_CNT_L
 * @{
 **/
#define SD_HOST_MASK_BYTE_COUNT_L     ((u32)0x000000FF << 0)
#define SD_HOST_BYTE_COUNT_L(x)       ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**
 * @defgroup SD_BYTE_CNT_H
 * @{
 **/
#define SD_HOST_MASK_BYTE_COUNT_H     ((u32)0x000000FF << 0)
#define SD_HOST_BYTE_COUNT_H(x)       ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**
 * @defgroup SD_BLOCK_CNT_L
 * @{
 **/
#define SD_HOST_MASK_BLOCK_COUNT_L     ((u32)0x000000FF << 0)
#define SD_HOST_BLOCK_COUNT_H(x)       ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**
 * @defgroup SD_BLOCK_CNT_H
 * @{
 **/
#define SD_HOST_MASK_BLOCK_COUNT_H     ((u32)0x000000FF << 0)
#define SD_HOST_BLOCK_COUNT_H(x)       ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/**
 * @defgroup SD_TRANSFER
 * @{
 **/
#define SD_HOST_BIT_START             ((u32)0x00000001 << 7)
#define SD_HOST_BIT_END               ((u32)0x00000001 << 6)
#define SD_HOST_BIT_IDLE              ((u32)0x00000001 << 5)
#define SD_HOST_BIT_ERROR_BIT         ((u32)0x00000001 << 4)
#define SD_HOST_MASK_COMMAND_CODE     ((u32)0x0000000F << 0)
#define SD_HOST_COMMAND_CODE(x)       ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**
 * @defgroup SD_CMD_STATE
 * @{
 **/
#define SD_HOST_MASK_CMD_STATE_MACHINE_IS_IDLE        ((u32)0x00000001 << 7)
#define SD_HOST_MASK_CMD_STATE_MACHINE                ((u32)0x0000000F << 0)
/** @} */

/**
 * @defgroup SD_DATA_STATE
 * @{
 **/
#define SD_HOST_MASK_DATA_STATE_MACHINE_IS_IDLE       ((u32)0x00000001 << 7)
#define SD_HOST_MASK_DATA_STATE_MACHINE               ((u32)0x0000000F << 0)
/** @} */

/**
 * @defgroup SD_BUS_TA_TIME
 * @{
 **/
#define SD_HOST_MASK_TURN_AROUND_TIME_EN              ((u32)0x00000001 << 3)
#define SD_HOST_MASK_BUS_TA_TIME                      ((u32)0x00000007 << 0)
/** @} */

/**
 * @defgroup SD_STOP_SDCLK_CFG
 * @{
 **/
#define SD_HOST_BIT_STOP_SD20CLK_CONFIG_EN                    ((u32)0x00000001 << 5)
#define SD_HOST_BIT_STOP_SD20CLK_CFG1                         ((u32)0x00000001 << 4)
#define SD_HOST_BIT_STOP_SD20CLK_CFG0                         ((u32)0x00000001 << 3)
#define SD_HOST_BIT_STOP_SD30CLK_CONFIG_EN                    ((u32)0x00000001 << 2)
#define SD_HOST_BIT_STOP_SD30CLK_CFG1                         ((u32)0x00000001 << 1)
#define SD_HOST_BIT_STOP_SD30CLK_CFG0                         ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_AUTO_RST_FIFO
 * @{
 **/
#define SD_HOST_BIT_DELAY_CHAIN_CLK_SOURCE                                    ((u32)0x00000001 << 5)
#define SD_HOST_BIT_SD_ASYNC_FIFO_SAMPLE_CLK_SELECTION_FOR_THE_SDR50_CMD_DAT  ((u32)0x00000001 << 4)
#define SD_HOST_BIT_AUTO_RESET_FIFO_EN                                        ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_DUTY_CTL
 * @{
 **/
#define SD_HOST_BIT_SD30_DATO_DUTY_ENABLE         ((u32)0x00000001 << 7)
#define SD_HOST_MASK_SD30_DATO_DELAY_DISABLE      ((u32)0x00000007 << 4)
#define SD_HOST_BIT_SD30_CMDO_DUTY_EN             ((u32)0x00000001 << 3)
#define SD_HOST_MASK_SD30_CMDO_DELAY_DISABLE      ((u32)0x00000007 << 0)
/** @} */

/**
 * @defgroup SD_SEQ_RW_CTL
 * @{
 **/
#define SD_HOST_BIT_SD_CONTROLLER_BUSY        ((u32)0x00000001 << 1)
#define SD_HOST_BIT_SD_CONTROLLER_STOP        ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_CONFIGURE4
 * @{
 **/
#define SD_HOST_BIT_IN_RANDOM_AUTO_MODE_NO_PENDING_CMD_WHEN_CARD_BUSY     ((u32)0x00000001 << 1)
#define SD_HOST_BIT_IN_RANDOM_AUTO_MODE                                   ((u32)0x00000001 << 0)
/** @} */

/**
 * @defgroup SD_WRITE_DELAY
 * @{
 **/
#define SD_HOST_MASK_SD_WRITE_DELAY       ((u32)0x0000000F << 0)
#define SD_HOST_SD_WRITE_DELAY(x)         ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**
 * @defgroup SD_READ_DELAY
 * @{
 **/
#define SD_HOST_MASK_SD_READ_DELAY       ((u32)0x0000000F << 0)
#define SD_HOST_SD_READ_DELAY(x)         ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/**
 * @defgroup SDCLK_DELAY_CHAIN_CFG
 * @{
 **/
#define SD_HOST_MASK_SDCLK_DELAY_CHAIN_CFG       ((u32)0x0000001F << 0)
#define SD_HOST_SDCLK_DELAY_CHAIN_CFG(x)         ((u32)(((x) & 0x0000001F) << 0))
/** @} */

/* Exported Types --------------------------------------------------------*/
/** @defgroup Exported_Types SDMMC Exported Types
  * @{
  */

/**
 * @brief SD_HOST_HOST Register Declaration
 */
typedef struct {
	__IO uint32_t   RSVD1[256];             /*!< , Address offset: 0x00000000 - 0x000003FC*/
	__IO uint32_t   SRAM_CTL;               /*!< SRAM Control Register, Address offset: 0x00000400*/
	__IO uint32_t   DMA_CTL1;               /*!< DMA Control Register 1, Address offset: 0x00000404*/
	__IO uint32_t   DMA_CTL2;               /*!< DMA Control Register 2, Address offset: 0x00000408*/
	__IO uint32_t   DMA_CTL3;               /*!< DMA Control Register 3, Address offset: 0x0000040C*/
	__IO uint32_t   SYS_LOW_PWR;            /*!< Low Power Control Register, Address offset: 0x00000410*/
	__IO uint32_t   RSVD2[3];               /*!< , Address offset: 0x00000414 - 0x0000041C*/
	__IO uint32_t   DMA_CTL4;               /*!< , Address offset: 0x00000420*/
	__IO uint32_t   ISR;                    /*!< SD Interrupt Status Register, Address offset: 0x00000424*/
	__IO uint32_t   ISREN;                  /*!<, Address offset: 0x00000428*/
	__IO uint32_t   DUMMY_SYS;              /*!<, Address offset: 0x0000042C*/
	__IO uint32_t   RSVD3[5];               /*!< , Address offset: 0x00000430 - 0x00000440*/
	__IO uint32_t   DBG;                    /*!<, Address offset: 0x00000444*/
	__IO uint32_t   RSVD4[10];              /*!< , Address offset: 0x00000448 - 0x0000046C*/
	__IO uint32_t   IP_CTL;                 /*!<, Address offset: 0x00000470*/
	__IO uint32_t   PAD_CTL;                /*!<, Address offset: 0x00000474*/
	__IO uint32_t   CKGEN_CTL;              /*!<, Address offset: 0x00000478*/
	__IO uint32_t   RSVD5[33];              /*!< , Address offset: 0x0000047C - 0x00000500*/
	__IO uint8_t    RSVD6[2];                /*!< , Address offset: 0x00000500 - 0x00000501*/
	__IO uint8_t    CARD_DRIVE_SEL;          /*!<, Address offset: 0x00000502*/
	__IO uint8_t    CARD_STOP;               /*!<, Address offset: 0x00000503*/
	__IO uint8_t    RSVD7[7];                /*!< , Address offset: 0x00000504 - 0x0000050A*/
	__IO uint8_t    MUX_ADDR_SEL;            /*!<, Address offset: 0x0000050B*/
	__IO uint8_t    CARD_DMA1_CTL;           /*!<, Address offset: 0x0000050C*/
	__IO uint8_t    CARD_DATA_SOURCE;        /*!<, Address offset: 0x0000050D*/
	__IO uint8_t    CARD_SELECT;             /*!<, Address offset: 0x0000050E*/
	__IO uint8_t    RSVD8[16];               /*!< , Address offset: 0x0000050F - 0x0000051E*/
	__I  uint8_t    CARD_EXIST;              /*!<, Address offset: 0x0000051F*/
	__IO uint8_t    CARD_INT_EN;             /*!<, Address offset: 0x00000520*/
	__IO uint8_t    CARD_INT_PEND;           /*!<, Address offset: 0x00000521*/
	__IO uint8_t    RSVD9;                   /*!< , Address offset: 0x00000522*/
	__IO uint8_t    CARD_INT_PEND2;          /*!<, Address offset: 0x00000523*/
	__IO uint8_t    RSVD10[3];               /*!< , Address offset: 0x00000524 - 0x00000526*/
	__IO uint8_t    CLK_EN_SETTING_STAT;     /*!<, Address offset: 0x00000527*/
	__I  uint8_t    CARD_EXIST_2;            /*!<, Address offset: 0x00000528*/
	__IO uint8_t    CARD_CLOCK_EN_CTL;       /*!<, Address offset: 0x00000529*/
	__IO uint8_t    RSVD11[86];              /*!< , Address offset: 0x0000052A - 0x0000057F*/
	__IO uint8_t    SD_CONFIGURE1;           /*!<, Address offset: 0x00000580*/
	__IO uint8_t    SD_CONFIGURE2;           /*!<, Address offset: 0x00000581*/
	__IO uint8_t    SD_CONFIGURE3;           /*!<, Address offset: 0x00000582*/
	__I  uint8_t    SD_STATUS1;              /*!<, Address offset: 0x00000583*/
	__I  uint8_t    SD_STATUS2;              /*!<, Address offset: 0x00000584*/
	__IO uint8_t    SD_BUS_STATUS;           /*!<, Address offset: 0x00000585*/
	__IO uint8_t    SD_CMD_CODE;             /*!<, Address offset: 0x00000586*/
	__IO uint8_t    SD_SAMPLE_POINT_CTL;     /*!<, Address offset: 0x00000587*/
	__IO uint8_t    SD_PUSH_POINT_CTL;       /*!<, Address offset: 0x00000588*/
	__IO uint8_t    SD_CMD[6];               /*!<, Address offset: 0x00000589 - 0x0000058E*/
	__IO uint8_t    SD_BYTE_CNT_L;           /*!<, Address offset: 0x0000058F*/
	__IO uint8_t    SD_BYTE_CNT_H;           /*!<, Address offset: 0x00000590*/
	__IO uint8_t    SD_BLOCK_CNT_L;          /*!<, Address offset: 0x00000591*/
	__IO uint8_t    SD_BLOCK_CNT_H;          /*!<, Address offset: 0x00000592*/
	__IO uint8_t    SD_TRANSFER;             /*!<, Address offset: 0x00000593*/
	__IO uint8_t    SD_DDR_DETECT_START;     /*!<, Address offset: 0x00000594*/
	__I  uint8_t    SD_CMD_STATE;            /*!<, Address offset: 0x00000595*/
	__I  uint8_t    SD_DATA_STATE;           /*!<, Address offset: 0x00000596*/
	__IO uint8_t    SD_BUS_TA_TIME;          /*!<, Address offset: 0x00000597*/
	__IO uint8_t    SD_STOP_SDCLK_CFG;       /*!<, Address offset: 0x00000598*/
	__IO uint8_t    SD_AUTO_RST_FIFO;        /*!<, Address offset: 0x00000599*/
	__I  uint8_t    SD_DAT_PAD;              /*!<, Address offset: 0x0000059A*/
	__IO uint8_t    SD_DUMMY_4;              /*!<, Address offset: 0x0000059B*/
	__IO uint8_t    SD_DUMMY_5;              /*!<, Address offset: 0x0000059C*/
	__IO uint8_t    SD_DUTY_CTL;             /*!<, Address offset: 0x0000059D*/
	__IO uint8_t    SD_SEQ_RW_CTL;           /*!<, Address offset: 0x0000059E*/
	__IO uint8_t    SD_CONFIGURE4;           /*!<, Address offset: 0x0000059F*/
	__IO uint8_t    SD_ADDR_L;               /*!<, Address offset: 0x000005A0*/
	__IO uint8_t    SD_ADDR_H;               /*!<, Address offset: 0x000005A1*/
	__IO uint8_t    SD_START_ADDR_0;         /*!<, Address offset: 0x000005A2*/
	__IO uint8_t    SD_START_ADDR_1;         /*!<, Address offset: 0x000005A3*/
	__IO uint8_t    SD_START_ADDR_2;         /*!<, Address offset: 0x000005A4*/
	__IO uint8_t    SD_START_ADDR_3;         /*!<, Address offset: 0x000005A5*/
	__IO uint8_t    SD_RSP_MASK_1;           /*!<, Address offset: 0x000005A6*/
	__IO uint8_t    SD_RSP_MASK_2;           /*!<, Address offset: 0x000005A7*/
	__IO uint8_t    SD_RSP_MASK_3;           /*!<, Address offset: 0x000005A8*/
	__IO uint8_t    SD_RSP_MASK_4;           /*!<, Address offset: 0x000005A9*/
	__IO uint8_t    SD_RSP_DATA_1;           /*!<, Address offset: 0x000005AA*/
	__IO uint8_t    SD_RSP_DATA_2;           /*!<, Address offset: 0x000005AB*/
	__IO uint8_t    SD_RSP_DATA_3;           /*!<, Address offset: 0x000005AC*/
	__IO uint8_t    SD_RSP_DATA_4;           /*!<, Address offset: 0x000005AD*/
	__IO uint8_t    SD_WRITE_DELAY;          /*!<, Address offset: 0x000005AE*/
	__IO uint8_t    SD_READ_DELAY;           /*!<, Address offset: 0x000005AF*/
	__IO uint8_t    SDCLK_DELAY_CHAIN_CFG;   /*!<, Address offset: 0x000005B0*/
} SD_HOST_TypeDef;


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup Exported_Constants SDMMC Exported Constants
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup Exported_Functions SDMMC Exported Functions
  * @{
  */

#define SD_HOST_CMD_CPLT_TIMEOUT      5000
#define SD_HOST_TRANSFER_CPLT_TIMEOUT 1000000
#define SD_HOST_READ_TIMEOUT          100000
#define SD_HOST_WRITE_TIMEOUT         250000
#define SD_HOST_ERASE_TIMEOUT         2000000

#define SD_HOST_BUS_IDLE_TIMEOUT      100000      // Defines the timeout for waiting for bus idle -> 100ms
#define SD_HOST_BUS_IDLE_POLL_INTER   10000       // Defines the polling time of waiting for bus idel -> 10ms

#define SD_HOST_HOST_COMMAND          BIT(6)
#define SD_HOST_CMD_IDX_MASK          0x3F
#define SD_HOST_CMD8_VHS              1           // Define value of VHS field in CMD8 -> 2.7-3.6V
#define SD_HOST_CMD8_CHK_PATN         0xAA        // Define value of Check pattern field in CMD8

#define SD_HOST_ACMD41_TIMEOUT        1000000     // Define max timeout to wait for ACMD41 busy - 1 sec
#define SD_HOST_ACMD41_POLL_INTERAL   10000       // Define time interval to wait for ACMD41 busy - 10ms

#define SD_HOST_OCR_VDD_WIN           0xFF8000    // Define the value of OCR field in ACMD41
#define SD_HOST_RSP_HEADER_LENGTH     1

#define SD_HOST_DMA_TRANSFER_DONE     BIT(4)
#define SD_HOST_CARD_ERROR            BIT(2)
#define SD_HOST_CARD_END              BIT(1)
#define SD_HOST_CARD_DETECT           BIT(7)  /* routed to CARD_INT_EN/PEND, not ISREN */
#define SD_HOST_ISR_ALL               (SD_HOST_DMA_TRANSFER_DONE | SD_HOST_CARD_ERROR | SD_HOST_CARD_END)

#define SD_HOST_CSD_LENGTH            16
#define SD_HOST_BUFFER_LENGTH         64

/** @defgroup Card_Response_Classification
  * @{
  */
#define SD_HOST_NO_RESP					0
#define SD_HOST_RSP_6B					1
#define SD_HOST_RSP_17B					2
/**
  * @}
  */

/** @defgroup SDHOST_DMA_Transfer_Type
  * @{
  */
#define SD_HOST_DMA_NORMAL				0
#define SD_HOST_DMA_64B					1
#define SD_HOST_DMA_R2					2
/**
  * @}
  */

/** @defgroup SD_HOST_Data_Present
  * @{
  */
#define SD_HOST_NO_DATA					0
#define SD_HOST_DATA_EXIST				1
/**
  * @}
  */

/**
  * @brief SDHOST Init Structure Definition
  */
typedef struct {
	u8 SDIOH_idle_level;	/*!> Indicate the idle pin level mask. When operate in 1-Bit bus width, this value is 0x03.
								When operate in 4-Bit bus width, this value is 0x1F. */
} SD_HOST_InitTypeDef;

/** @brief SD Host Configuration (bus speed, width, pins)
  */
typedef struct {
	u8 sdioh_bus_speed;		/*!< Specify SD Host bus speed, should be SD_SPEED_DS or SD_SPEED_HS */
	u8 sdioh_bus_width;		/*!< Specify SD Host bus width, should be a value of @ref SDIOH_Bus_Width */
	u32 sdioh_cd_pin;		/*!< Specify Card Detect pin */
	u32 sdioh_wp_pin;		/*!< Specify Write Protection pin */
} SD_HostCfg_TypeDef;

typedef SD_HostCfg_TypeDef SDIOHCFG_TypeDef;

/**
  * @brief SDMMC Host Sending Command Information
  */
typedef struct {
	u32 arg;                        ///< Command argument
	u8 idx;                         ///< Command index: ::SD_HOST__cmd_idx_e
	u8 rsp_type;                    ///< Respons type: ::SD_HOST__rsp_type_e
	u8 rsp_crc_chk;                 ///< 1: check response crc
	u8 data_present;                ///< If there is trasfer in DAT lines: ::SD_HOST__data_present_e
} SD_HOST_CmdTypeDef;

/**
  * @brief Defines SD Host DMA operation.
  */
enum dma_op {
	DMA_Write   = 0,           ///< DMA write
	DMA_Read    = 1            ///< DMA read
};

/**
  * @brief Defines SD Host DMA transfer type.
  */
enum dma_transfer_type {
	DMA_Normal  = 0,           ///< Normal mode
	DMA_64B     = 1,           ///< For CMD6, read data length is 64 bytes
	DMA_R2      = 2,           ///< For R2, response will be stored in memory instead of register
};

/** @brief SD_HOST_Command_CODE
  */
typedef enum {
	SD_HOST_CMD_NormalWr         = 0,
	SD_HOST_CMD_AutoWr3          = 1,
	SD_HOST_CMD_AutoWr4          = 2,
	SD_HOST_CMD_AutoRd3          = 5,
	SD_HOST_CMD_AutoRd4          = 6,
	SD_HOST_CMD_SendCmdGetRsp    = 8,
	SD_HOST_CMD_AutoWr1          = 9,
	SD_HOST_CMD_AutoWr2          = 10,
	SD_HOST_CMD_NormalRd         = 12,
	SD_HOST_CMD_AutoRd1          = 13,
	SD_HOST_CMD_AutoRd2          = 14,
	SD_HOST_CMD_Tuning           = 15
} SD_HOST_COMMAND_CODE;

/** @brief SDMMC Host Mode
  */
typedef enum {
	SD_HOST_modesd20   = 0,
	SD_HOST_modeddr    = 1,
	SD_HOST_modesd30   = 2
} SD_HOST_MODE;

/** @brief SDMMC Host Bus Width
  */
typedef enum {
	SDIOH_BUS_WIDTH_1BIT    = 0,
	SDIOH_BUS_WIDTH_4BIT    = 1
} SD_HOST_BUS_WIDTH;

/**
  * @brief SDHOST DMA Control Structure Definition
  */
typedef struct {
	u32 start_addr;		/*!< Specify the DMA start address. Unit: 8 Bytes. */
	u16 blk_cnt;		/*!< Specify the DMA transfer length.  Unit: 512 Bytes). */
	u8 op;				/*!< Specify the data move direction. Should be a value of @ref SDIOH_DMA_Operation. */
	u8 type;			/*!< Specify the transfer type. Shold be a value of @ref SDIOH_DMA_Transfer_Type. */
} SD_HOST_DMA_CTL;

/** @brief SD_HOST_BUS_SIGNALING_LEVEL
 */
typedef enum {
	SD_HOST_BUS_SIGNALING_LEVEL_3p3V = 0x00,
	SD_HOST_BUS_SIGNALING_LEVEL_1p8V = 0x01
} SD_SIG_LVL;

/** @defgroup SDHOST_Card_Response_Byte_Index
  * @{
  */
#define SDIO_RESP0						0
#define SDIO_RESP1						1
#define SDIO_RESP2						2
#define SDIO_RESP3						3
#define SDIO_RESP4						4
#define SDIO_RESP5						5
/**
  * @}
  */

/** @defgroup SDHOST_Card_Response_Type
  * @{
  */
#define SDIOH_RESP_R1					1
#define SDIOH_RESP_R2					2
#define SDIOH_RESP_R3					3
#define SDIOH_RESP_R6					4
#define SDIOH_RESP_R7					5
/**
  * @}
  */


/**
 * @brief SD host controller operations vtable.
 *
 * Abstracts the host IP from the card protocol layer (ameba_sd_card.c).
 * The card layer accesses the host exclusively through this vtable,
 * allowing different host IPs to be swapped without modifying card code.
 *
 * Instance defined in ameba_sd.c, assigned during SD_Init().
 */
typedef struct {
	/* Command / Response */
	u32(*SendCommand)(SD_HOST_CmdTypeDef *cmd_attrib, u32 timeout_us);
	u8(*GetResponse)(u8 byte_index);
	u32(*CheckTxError)(u16 *status);

	/* DMA */
	void (*DMAConfig)(SD_HOST_DMA_CTL *dma_ctl);
	u32(*WaitDMADone)(u32 timeout_us);
	void (*DMAReset)(void);
	u8(*CheckTransferDone)(void);

	/* Error */
	u8(*HasError)(void);
	void (*ClearErrorBit)(void);

	/* Bus Configuration */
	void (*SetBusWidth)(u8 width);
	u8(*GetBusWidth)(void);

	/* Interrupt */
	void (*INTConfig)(u8 SDIO_IT, u32 newState);
	void (*INTClearPendingBit)(u8 SDIO_IT);

	/* Card Detect / Stop */
	u8(*CardExist)(void);
	void (*CardStop)(void);
} SD_HOST_OPS_TypeDef;

extern const SD_HOST_OPS_TypeDef SDHost_Ops;

/** @defgroup SDIO_HOST_Functions SDIO Host Functions
  * @{
  */
_LONG_CALL_ u32 SDHost_Busy(void);
_LONG_CALL_ u32 SDHost_CheckTxError(u16 *status);
_LONG_CALL_ u32 SDHost_WaitTxDone(u32 timeout_us);
_LONG_CALL_ u32 SDHost_WaitDMADone(u32 timeout_us);
_LONG_CALL_ u32 SDHost_GetISR(void);
_LONG_CALL_ void SDHost_INTConfig(u8 SDIO_IT, u32 newState);
_LONG_CALL_ void SDHost_INTClearPendingBit(u8 SDIO_IT);
_LONG_CALL_ u32 SDHost_CheckBusState(u8 status, u32 timeout_us);
_LONG_CALL_ u8 SDHost_GetBusWidth(void);
_LONG_CALL_ void SDHost_SetBusWidth(u8 width);
_LONG_CALL_ void SDHost_DMAConfig(SD_HOST_DMA_CTL *dma_ctl);
_LONG_CALL_ void SDHost_DMAReset(void);
_LONG_CALL_ u32 SDHost_SendCommand(SD_HOST_CmdTypeDef *cmd_attrib, u32 timeout_us);
_LONG_CALL_ u32 SDHost_SendCommandEx(SD_HOST_CmdTypeDef *cmd_attrib, u8 cmd_code, u32 timeout_us);
_LONG_CALL_ u8 SDHost_GetResponse(u8 byte_index);
_LONG_CALL_ void SDHost_SwitchSpeed(u8 clk_div, u8 mode);
_LONG_CALL_ u32 SDHost_InitialModeCmd(u8 NewState, u8 Level);
_LONG_CALL_ void SDHost_EnableClk(void);
_LONG_CALL_ void SDHost_PowerDown(void);
_LONG_CALL_ void SDHost_PowerUp(void);
_LONG_CALL_ void SDHost_PinmuxInit(void);
_LONG_CALL_ u32 SDHost_Init(u8 BusBitMode);
_LONG_CALL_ void SDHost_DeInit(void);


_LONG_CALL_ void write_8_bit(u32 addr, u8 data);
_LONG_CALL_ u32 SDHost_CheckBusState(u8 status, u32 timeout_us);
_LONG_CALL_ u32 SDHost_SetPhase(u8 vp_sel, u8 phase);
_LONG_CALL_ u32 SDHost_SendTuningBlock(void);
_LONG_CALL_ u32 SDHost_SendTuningBlockEx(u8 cmd_idx);
_LONG_CALL_ u32 SDHost_ExecTuning(void);
_LONG_CALL_ u32 SDHost_ExecTuningEx(u8 tuning_cmd_idx, u32 cmd13_arg,
									u32(*data_test)(void *ctx), void *data_test_ctx);
_LONG_CALL_ u32  SDHost_ForceIdle(void);
_LONG_CALL_ void SDHost_ClearErrorBit(void);
_LONG_CALL_ u8 SDHost_CardExist(void);
_LONG_CALL_ u8 SDHost_CheckTransferDone(void);
_LONG_CALL_ u8 SDHost_HasError(void);
_LONG_CALL_ u16 SDHost_GetStatus(void);
_LONG_CALL_ void SDHost_SetClkToggle(u8 enable);
_LONG_CALL_ void SDHost_SetPadVoltage(u8 level);
_LONG_CALL_ void SDHost_CardStop(void);
_LONG_CALL_ void SDHost_SetSamplePoint(u8 val);

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
