/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_RXI_CORE_H
#define SYSREG_RXI_CORE_H

/* AUTO_GEN_START */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup RXI_CORE
* @brief RXI_CORE driver modules
* @{
*/


/** @defgroup NAME
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_NAME                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x52300 "RXI300" */
#define RXI_CORE_DESIGN_NAME(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_NAME(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup VER
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_VER                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x300 "RXI300" */
#define RXI_CORE_DESIGN_VER(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_VER(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REV
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_REV                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x00000000 "Reversion, it is different for every release" */
#define RXI_CORE_DESIGN_REV(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_REV(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup INST
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_INST                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x4150 "Instance" */
#define RXI_CORE_DESIGN_INST(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_INST(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup IMPL_Y
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_IMPL_YEAR              ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x2024 "Implation year" */
#define RXI_CORE_DESIGN_IMPL_YEAR(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_IMPL_YEAR(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup IMPL_D
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_IMPL_DATE              ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x11281032 "Implation date" */
#define RXI_CORE_DESIGN_IMPL_DATE(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_IMPL_DATE(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup DEV
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_DEV                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x525751 "Designer S2527" */
#define RXI_CORE_DESIGN_DEV(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_DEV(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup PRO_NUM
 * @brief
 * @{
 */
#define RXI_CORE_MASK_DESIGN_PRO_NUM                ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x7013 "Project name" */
#define RXI_CORE_DESIGN_PRO_NUM(x)                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_DESIGN_PRO_NUM(x)              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_PLD0_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (addr_hole)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_ADDR_HOLE         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (addr_hole)" */
#define RXI_CORE_ERR_BST_LEN_ADDR_HOLE(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_ADDR_HOLE(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_ADDR_HOLE         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (addr_hole)" */
#define RXI_CORE_ERR_BSTTYPE_ADDR_HOLE(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_ADDR_HOLE(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_ADDR_HOLE             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (addr_hole)" */
#define RXI_CORE_ERR_CMD_ADDR_HOLE(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_ADDR_HOLE(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_ADDR_HOLE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (addr_hole)
                                                                                              * Error source of s0_da = 8'h00
                                                                                              * Error source of s1_da = 8'h01
                                                                                              * Error source of s4_da = 8'h02
                                                                                              * Error source of s5_da = 8'h03
                                                                                              * Error source of S2_1_da = 8'h04
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_ADDR_HOLE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_ADDR_HOLE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (addr_hole)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_ADDR_HOLE            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (addr_hole)" */
#define RXI_CORE_ERR_SIZE_ADDR_HOLE(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_ADDR_HOLE(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_ADDR_HOLE          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (addr_hole)" */
#define RXI_CORE_ERR_BYTEEN_ADDR_HOLE(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_ADDR_HOLE(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (addr_hole)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (addr_hole)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (addr_hole)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h01 is for address-hole (addr_hole)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (addr_hole)" */
/** @} */


/** @defgroup ERR_PLD0_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_TIMEOUT_0         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_BST_LEN_TIMEOUT_0(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_TIMEOUT_0(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_TIMEOUT_0         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_BSTTYPE_TIMEOUT_0(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_TIMEOUT_0(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_TIMEOUT_0             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_CMD_TIMEOUT_0(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_TIMEOUT_0(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_TIMEOUT_0             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (s0, s3 timeout)
                                                                                              * Error source of S0 = 8'h3f 
                                                                                              * Error source of S3-1 = 8'h31
                                                                                              * Error source of S3-2 = 8'h20
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_TIMEOUT_0(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_TIMEOUT_0(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_TIMEOUT_0            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_SIZE_TIMEOUT_0(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_TIMEOUT_0(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_TIMEOUT_0          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (s0, s3 timeout)" */
#define RXI_CORE_ERR_BYTEEN_TIMEOUT_0(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_TIMEOUT_0(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (s0, s3 timeout)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (s0, s3 timeout)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (s0, s3 timeout)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h38 is for axi slave arready timeout, 8'h39 is for axi slave rvalid timeout, 8'h3a is for axi slave awready timeout, 8'h3b is for axi slave wready timeout,  8'h3c is for axi slave bvalid timeout (s0, s3 timeout) " */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TIMEOUT_0
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (s0, s3 timeout)" */
/** @} */


/** @defgroup ERR_PLD0_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_TIMEOUT_1         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_BST_LEN_TIMEOUT_1(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_TIMEOUT_1(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_TIMEOUT_1         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_BSTTYPE_TIMEOUT_1(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_TIMEOUT_1(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_TIMEOUT_1             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_CMD_TIMEOUT_1(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_TIMEOUT_1(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_TIMEOUT_1             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (s1, s5, s6, s7 timeout)
                                                                                              * Error source of S1-1 = 8'h11
                                                                                              * Error source of S1-2 = 8'h30
                                                                                              * Error source of S5 = 8'h05
                                                                                              * Error source of S6 = 8'h06
                                                                                              * Error source of S7 = 8'h07
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_TIMEOUT_1(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_TIMEOUT_1(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_TIMEOUT_1            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_SIZE_TIMEOUT_1(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_TIMEOUT_1(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_TIMEOUT_1          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_BYTEEN_TIMEOUT_1(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_TIMEOUT_1(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (s1, s5, s6, s7 timeout)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h38 is for axi slave arready timeout, 8'h39 is for axi slave rvalid timeout, 8'h3a is for axi slave awready timeout, 8'h3b is for axi slave wready timeout,  8'h3c is for axi slave bvalid timeout (s1, s5, s6, s7 timeout)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TIMEOUT_1
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (s1, s5, s6, s7 timeout)" */
/** @} */


/** @defgroup ERR_PLD0_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_TIMEOUT_2         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_BST_LEN_TIMEOUT_2(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_TIMEOUT_2(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_TIMEOUT_2         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_BSTTYPE_TIMEOUT_2(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_TIMEOUT_2(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_TIMEOUT_2             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_CMD_TIMEOUT_2(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_TIMEOUT_2(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_TIMEOUT_2             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (s2 group timeout)
                                                                                              * Error source of S2-1 = 8'h21
                                                                                              * Error source of S2-2 = 8'h22
                                                                                              * Error source of S2-3 = 8'h23
                                                                                              * Error source of S2-4 = 8'h24
                                                                                              * Error source of S2-5 = 8'h25
                                                                                              * Error source of S2-6 = 8'h26
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_TIMEOUT_2(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_TIMEOUT_2(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_TIMEOUT_2            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_SIZE_TIMEOUT_2(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_TIMEOUT_2(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_TIMEOUT_2          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (s2 group timeout)" */
#define RXI_CORE_ERR_BYTEEN_TIMEOUT_2(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_TIMEOUT_2(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (s2 group timeout)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (s2 group timeout)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (s2 group timeout)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h38 is for axi slave arready timeout, 8'h39 is for axi slave rvalid timeout, 8'h3a is for axi slave awready timeout, 8'h3b is for axi slave wready timeout,  8'h3c is for axi slave bvalid timeout (s2 group timeout)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TIMEOUT_2
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (s2 group timeout)" */
/** @} */


/** @defgroup ERR_PLD0_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_TIMEOUT_3         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_BST_LEN_TIMEOUT_3(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_TIMEOUT_3(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_TIMEOUT_3         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_BSTTYPE_TIMEOUT_3(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_TIMEOUT_3(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_TIMEOUT_3             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_CMD_TIMEOUT_3(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_TIMEOUT_3(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_TIMEOUT_3             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (s4 group timeout)
                                                                                              * Error source of S4-1 = 8'h41
                                                                                              * Error source of S4-2 = 8'h42
                                                                                              * Error source of S4-3 = 8'h43
                                                                                              * Error source of S4-4 = 8'h44
                                                                                              * Error source of S4-5 = 8'h45
                                                                                              * Error source of S4-6 = 8'h46
                                                                                              * Error source of S4-7 = 8'h47
                                                                                              * Error source of S4-8 = 8'h48
                                                                                              * Error source of S4-10 = 8'h4a
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_TIMEOUT_3(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_TIMEOUT_3(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_TIMEOUT_3            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_SIZE_TIMEOUT_3(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_TIMEOUT_3(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_TIMEOUT_3          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (s4 group timeout)" */
#define RXI_CORE_ERR_BYTEEN_TIMEOUT_3(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_TIMEOUT_3(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (s4 group timeout)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (s4 group timeout)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (s4 group timeout)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h38 is for axi slave arready timeout, 8'h39 is for axi slave rvalid timeout, 8'h3a is for axi slave awready timeout, 8'h3b is for axi slave wready timeout,  8'h3c is for axi slave bvalid timeout (s4 group timeout)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TIMEOUT_3
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (s4 group timeout)" */
/** @} */


/** @defgroup ERR_PLD0_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_TIMEOUT_4         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_BST_LEN_TIMEOUT_4(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_TIMEOUT_4(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_TIMEOUT_4         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_BSTTYPE_TIMEOUT_4(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_TIMEOUT_4(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_TIMEOUT_4             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_CMD_TIMEOUT_4(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_TIMEOUT_4(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_TIMEOUT_4             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (s8-1 to s8-11 group timeout)
                                                                                              * Error source of S8-1 = 8'h81
                                                                                              * Error source of S8-2 = 8'h82
                                                                                              * Error source of S8-3 = 8'h83
                                                                                              * Error source of S8-4 = 8'h84
                                                                                              * Error source of S8-5 = 8'h85
                                                                                              * Error source of S8-6 = 8'h86
                                                                                              * Error source of S8-7 = 8'h87
                                                                                              * Error source of S8-8 = 8'h88
                                                                                              * Error source of S8-9 = 8'h89
                                                                                              * Error source of S8-10 = 8'h8a
                                                                                              * Error source of S8-11 = 8'h8b
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_TIMEOUT_4(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_TIMEOUT_4(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_TIMEOUT_4            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_SIZE_TIMEOUT_4(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_TIMEOUT_4(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_TIMEOUT_4          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_BYTEEN_TIMEOUT_4(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_TIMEOUT_4(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (s8-1 to s8-11 group timeout)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h38 is for axi slave arready timeout, 8'h39 is for axi slave rvalid timeout, 8'h3a is for axi slave awready timeout, 8'h3b is for axi slave wready timeout,  8'h3c is for axi slave bvalid timeout (s8-1 to s8-11 group timeout)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TIMEOUT_4
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (s8-1 to s8-11 group timeout)" */
/** @} */


/** @defgroup ERR_PLD0_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_TIMEOUT_5         ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_BST_LEN_TIMEOUT_5(x)           ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_TIMEOUT_5(x)       ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_TIMEOUT_5         ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_BSTTYPE_TIMEOUT_5(x)           ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_TIMEOUT_5(x)       ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_TIMEOUT_5             ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_CMD_TIMEOUT_5(x)               ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_TIMEOUT_5(x)           ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_TIMEOUT_5             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request(s8-12 to s8-15 group timeout)
                                                                                              * Error source of S8-12 = 8'h8c
                                                                                              * Error source of S8-13 = 8'h8d
                                                                                              * Error source of S8-14 = 8'h8e
                                                                                              * Error source of S8-15 = 8'h8f
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_TIMEOUT_5(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_TIMEOUT_5(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_TIMEOUT_5            ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_SIZE_TIMEOUT_5(x)              ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_TIMEOUT_5(x)          ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_TIMEOUT_5          ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_BYTEEN_TIMEOUT_5(x)            ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_TIMEOUT_5(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid(s8-12 to s8-15 group timeout)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h38 is for axi slave arready timeout, 8'h39 is for axi slave rvalid timeout, 8'h3a is for axi slave awready timeout, 8'h3b is for axi slave wready timeout,  8'h3c is for axi slave bvalid timeout(s8-12 to s8-15 group timeout)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_TIMEOUT_5
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt(s8-12 to s8-15 group timeout)" */
/** @} */


/** @defgroup ERR_PLD0_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_BSTINDEX                  ((u32)0x000000FF << 24)                  /*!< READ_ONLY 0x00 "Burst index of the error request (security addr hole)" */
#define RXI_CORE_ERR_BSTINDEX(x)                    ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ERR_BSTINDEX(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BST_LEN_SEC_ADDR_HOLE     ((u32)0x000000FF << 16)                  /*!< READ_ONLY 0x00 "Burst length of the error request (security addr hole)" */
#define RXI_CORE_ERR_BST_LEN_SEC_ADDR_HOLE(x)       ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ERR_BST_LEN_SEC_ADDR_HOLE(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ERR_BSTTYPE_SEC_ADDR_HOLE     ((u32)0x00000007 << 11)                  /*!< READ_ONLY 0x0 "BurstSeq of the error request (security addr hole)" */
#define RXI_CORE_ERR_BSTTYPE_SEC_ADDR_HOLE(x)       ((u32)(((x) & 0x00000007) << 11))
#define RXI_CORE_GET_ERR_BSTTYPE_SEC_ADDR_HOLE(x)   ((u32)(((x >> 11) & 0x00000007)))
#define RXI_CORE_MASK_ERR_CMD_SEC_ADDR_HOLE         ((u32)0x00000007 << 8)                   /*!< READ_ONLY 0x0 "Read/write cmd of the error request (security addr hole)" */
#define RXI_CORE_ERR_CMD_SEC_ADDR_HOLE(x)           ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ERR_CMD_SEC_ADDR_HOLE(x)       ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ERR_SRC_SEC_ADDR_HOLE         ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "Error source of the error request (security addr hole)
                                                                                              * Error source of s0_sec_da = 8'ha0
                                                                                              * Error source of s1_sec_da = 8'ha1
                                                                                              * Error source of s4_sec_da = 8'ha4
                                                                                              * Error source of s5_sec_da = 8'ha5
                                                                                              * Error source of s8_sec_da = 8'ha5
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_SRC_SEC_ADDR_HOLE(x)           ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_SRC_SEC_ADDR_HOLE(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_PLD1_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_MREQINFO                  ((u32)0x000001FF << 23)                  /*!< READ_ONLY 0x0 "Req_info of the error request (security addr hole)" */
#define RXI_CORE_ERR_MREQINFO(x)                    ((u32)(((x) & 0x000001FF) << 23))
#define RXI_CORE_GET_ERR_MREQINFO(x)                ((u32)(((x >> 23) & 0x000001FF)))
#define RXI_CORE_MASK_ERR_SIZE_SEC_ADDR_HOLE        ((u32)0x00000007 << 16)                  /*!< READ_ONLY 0x0 "Burst size of the error request (security addr hole)" */
#define RXI_CORE_ERR_SIZE_SEC_ADDR_HOLE(x)          ((u32)(((x) & 0x00000007) << 16))
#define RXI_CORE_GET_ERR_SIZE_SEC_ADDR_HOLE(x)      ((u32)(((x >> 16) & 0x00000007)))
#define RXI_CORE_MASK_ERR_BYTEEN_SEC_ADDR_HOLE      ((u32)0x0000FFFF << 0)                   /*!< READ_ONLY 0x0 "Byteen of the error request (security addr hole)" */
#define RXI_CORE_ERR_BYTEEN_SEC_ADDR_HOLE(x)        ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_ERR_BYTEEN_SEC_ADDR_HOLE(x)    ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup ERR_ID_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_TRANS_ID                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "ID of the error request which include master id and tagid (security addr hole)
                                                                                              * master id of M0   = 4'b0000
                                                                                              * master id of M1   = 4'b0001
                                                                                              * master id of M2   = 4'b0010
                                                                                              * master id of M3   = 4'b0011
                                                                                              * master id of M4-1 = 4'b0100
                                                                                              * master id of M4-2 = 4'b0101
                                                                                              * master id of M5-1 = 4'b0110
                                                                                              * master id of M5-2 = 4'b0111
                                                                                              * master id of M6   = 4'b1000
                                                                                              * master id of M7   = 4'b1001
                                                                                              * master id of M8   = 4'b1010
                                                                                              * master id of M9-1 = 4'b1011
                                                                                              * master id of M9-2 = 4'b1100
                                                                                              * master id of M9-3 = 4'b1101
                                                                                              * master id of M10  = 4'b1110
                                                                                              * "
                                                                                              */
#define RXI_CORE_ERR_TRANS_ID(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_TRANS_ID(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR0_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_LOW                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [31:0] of request (security addr hole)" */
#define RXI_CORE_ERR_ADDR_LOW(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_LOW(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_ADR1_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_ADDR_HIGH                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0 "Address [63:32] of request (security addr hole)" */
#define RXI_CORE_ERR_ADDR_HIGH(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ERR_ADDR_HIGH(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ERR_CODE_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ERR_CODE_ERR_TYPE             ((u32)0x000000FF << 0)                   /*!< READ_ONLY 0x0 "Error code of error request , 8'h02 is for address-hole (security addr hole)" */
#define RXI_CORE_ERR_CODE_ERR_TYPE(x)               ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_ERR_CODE_ERR_TYPE(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup ERR_INTR_CLR_SEC_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_BIT_ERR_INTR_CLR_ERR_INFO          ((u32)0x00000001 << 0)                   /*!< WRITE_ONLY 0x0 "Error log interrupt clear, writing 1'b1 to clear interrupt (security addr hole)" */
/** @} */


/** @defgroup ACP_BASE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ACP_BASE                      ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x6e001000 "ACP base address" */
#define RXI_CORE_ACP_BASE(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ACP_BASE(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ACP_LIMIT
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ACP_LIMIT                     ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x6e002000 "ACP limit address" */
#define RXI_CORE_ACP_LIMIT(x)                       ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ACP_LIMIT(x)                   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ACP_OFFSET
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ACP_OFFSET                    ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x00001000 "ACP offset" */
#define RXI_CORE_ACP_OFFSET(x)                      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ACP_OFFSET(x)                  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup GPR_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_GPR_GROUP_0                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0ff0012c
                                                                                              * "bit[1:0] = traffic gen for ddrc selection, 00 for ddrc_p3, 01 for ddrc_p4, 10 for ddrc_p5
                                                                                              * bit[3:2]   = ca32_acp_awcache ctrl bit [3:2], bit[1:0] is fixed to 2'b11
                                                                                              * bit[5:4]   = ca32_acp_arcache ctrl bit [3:2], bit[1:0] is fixed to 2'b11
                                                                                              * bit[16:8]  = sram ctrl rd weighted
                                                                                              * bit[28]    = m9-3 secure switch, 0 for secure, 1 for non-secure 
                                                                                              * bit[27]    = m9-2 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[26]    = m9-1 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[25]    = m5-2 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[24]    = m5-1 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[23]    = m4-2 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[22]    = m4-1 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[21]    = m3 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * bit[20]    = m2 secure switch, 0 for secure, 1 for non-secure  
                                                                                              * "
                                                                                              */
#define RXI_CORE_GPR_GROUP_0(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_GPR_GROUP_0(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup GPR_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_GPR_GROUP_1                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x3fffffff
                                                                                              * "
                                                                                              * bit[29:14] = s8-16 to s8-1 secure switch, 1 for secure, 0 for non-secure 
                                                                                              * bit[13]    = not used, use idau
                                                                                              * bit[12]    = not used, s4-10 is S_NS
                                                                                              * bit[11]    = s4-9 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[10]    = s4-7 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[9]     = s4-6 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[8]     = s4-5 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[7]     = s4-4 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[6]     = s4-3 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[5]     = s4-2 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[4]     = s4-1 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[3]     = not used, use idau 
                                                                                              * bit[2]     = s3-1 secure switch, 1 for secure, 0 for non-secure  
                                                                                              * bit[1]     = not used, use idau
                                                                                              * bit[0]     = not used, use idau
                                                                                              * "
                                                                                              */
#define RXI_CORE_GPR_GROUP_1(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_GPR_GROUP_1(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GPR_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_GPR_GROUP_2                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x00888888
                                                                                              * "
                                                                                              * bit[23:20] = ddrc_p5 arqos vaule
                                                                                              * bit[19:16] = ddrc_p4 arqos vaule
																							  * bit[15:12] = ddrc_p3 arqos vaule
																							  * bit[11: 8] = ddrc_p2 arqos vaule
																							  * bit[ 7: 4] = ddrc_p1 arqos vaule
																							  * bit[ 3: 0] = ddrc_p0 arqos vaule
                                                                                              * "
                                                                                              */
#define RXI_CORE_GPR_GROUP_2(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_GPR_GROUP_2(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
#define DDRC_P5_ARQOS(x)                            ((u32)(((x) & 0x0000000F) << 20))
#define DDRC_P4_ARQOS(x)                            ((u32)(((x) & 0x0000000F) << 16))
#define DDRC_P3_ARQOS(x)                            ((u32)(((x) & 0x0000000F) << 12))
#define DDRC_P2_ARQOS(x)                            ((u32)(((x) & 0x0000000F) << 8))
#define DDRC_P1_ARQOS(x)                            ((u32)(((x) & 0x0000000F) << 4))
#define DDRC_P0_ARQOS(x)                            ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/** @defgroup GPR_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_GPR_GROUP_3                   ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x00888888
                                                                                              * "
                                                                                              * bit[23:20] = ddrc_p5 awqos vaule
                                                                                              * bit[19:16] = ddrc_p4 awqos vaule
																							  * bit[15:12] = ddrc_p3 awqos vaule
																							  * bit[11: 8] = ddrc_p2 awqos vaule
																							  * bit[ 7: 4] = ddrc_p1 awqos vaule
																							  * bit[ 3: 0] = ddrc_p0 awqos vaule
                                                                                              * "
                                                                                              */
#define RXI_CORE_GPR_GROUP_3(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_GPR_GROUP_3(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
#define DDRC_P5_AWQOS(x)                            ((u32)(((x) & 0x0000000F) << 20))
#define DDRC_P4_AWQOS(x)                            ((u32)(((x) & 0x0000000F) << 16))
#define DDRC_P3_AWQOS(x)                            ((u32)(((x) & 0x0000000F) << 12))
#define DDRC_P2_AWQOS(x)                            ((u32)(((x) & 0x0000000F) << 8))
#define DDRC_P1_AWQOS(x)                            ((u32)(((x) & 0x0000000F) << 4))
#define DDRC_P0_AWQOS(x)                            ((u32)(((x) & 0x0000000F) << 0))
/** @} */

/** @defgroup GPR_IN_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_GPR_IN_GROUP_0                ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x00000000 "Error id from core_peri slave, bit[31:16] = rid, bit[15:0] = bid " */
#define RXI_CORE_GPR_IN_GROUP_0(x)                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_GPR_IN_GROUP_0(x)              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup GPR_IN_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_GPR_IN_GROUP_1                ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x00000000 "Error id from core_mem slave, bit[31:16] = rid, bit[15:0] = bid " */
#define RXI_CORE_GPR_IN_GROUP_1(x)                  ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_GPR_IN_GROUP_1(x)              ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_GUARDIAN_EN_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_GUARDIAN_EN_0         ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Enable timeout check function respectively for slave
                                                                                              * bit[31] = s8_10
                                                                                              * bit[30] = s8_9
                                                                                              * bit[29] = s8_8
                                                                                              * bit[28] = s8_7
                                                                                              * bit[27] = s8_6
                                                                                              * bit[26] = s8_5
                                                                                              * bit[25] = s8_4
                                                                                              * bit[24] = s8_3
                                                                                              * bit[23] = s8_2
                                                                                              * bit[22] = s8_1
                                                                                              * bit[21] = s4_10
                                                                                              * bit[20] = s4_8
                                                                                              * bit[19] = s4_7
                                                                                              * bit[18] = s4_6
                                                                                              * bit[17] = s4_5
                                                                                              * bit[16] = s4_4
                                                                                              * bit[15] = s4_3
                                                                                              * bit[14] = s4_2
                                                                                              * bit[13] = s4_1
                                                                                              * bit[12] = s2_6
                                                                                              * bit[11] = s2_5
                                                                                              * bit[10] = s2_4
                                                                                              * bit[9] = s2_3
                                                                                              * bit[8] = s2_2
                                                                                              * bit[7] = s2_1
                                                                                              * bit[6] = s7
                                                                                              * bit[5] = s6
                                                                                              * bit[4] = s5
                                                                                              * bit[3] = s1
                                                                                              * bit[2] = s3_2
                                                                                              * bit[1] = s3_1
                                                                                              * bit[0] = s0
                                                                                              * "
                                                                                              */
#define RXI_CORE_TIMEOUT_GUARDIAN_EN_0(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_TIMEOUT_GUARDIAN_EN_0(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_GUARDIAN_EN_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_GUARDIAN_EN_1         ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Enable timeout check function respectively for slave
                                                                                              * bit[7] = s1_2
                                                                                              * bit[4] = s8_15
                                                                                              * bit[3] = s8_14
                                                                                              * bit[2] = s8_13
                                                                                              * bit[1] = s8_12
                                                                                              * bit[0] = s8_11
                                                                                              * "
                                                                                              */
#define RXI_CORE_TIMEOUT_GUARDIAN_EN_1(x)           ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_TIMEOUT_GUARDIAN_EN_1(x)       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_CLEAR_N_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_CLEAR_N_0             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0xffffffff
                                                                                              * "The timeout clear register, this register can only be written to 0 when a timeout occurs, and automatically clear this register by hardware
                                                                                              * bit[31] = s8_10
                                                                                              * bit[30] = s8_9
                                                                                              * bit[29] = s8_8
                                                                                              * bit[28] = s8_7
                                                                                              * bit[27] = s8_6
                                                                                              * bit[26] = s8_5
                                                                                              * bit[25] = s8_4
                                                                                              * bit[24] = s8_3
                                                                                              * bit[23] = s8_2
                                                                                              * bit[22] = s8_1
                                                                                              * bit[21] = s4_10
                                                                                              * bit[20] = s4_8
                                                                                              * bit[19] = s4_7
                                                                                              * bit[18] = s4_6
                                                                                              * bit[17] = s4_5
                                                                                              * bit[16] = s4_4
                                                                                              * bit[15] = s4_3
                                                                                              * bit[14] = s4_2
                                                                                              * bit[13] = s4_1
                                                                                              * bit[12] = s2_6
                                                                                              * bit[11] = s2_5
                                                                                              * bit[10] = s2_4
                                                                                              * bit[9] = s2_3
                                                                                              * bit[8] = s2_2
                                                                                              * bit[7] = s2_1
                                                                                              * bit[6] = s7
                                                                                              * bit[5] = s6
                                                                                              * bit[4] = s5
                                                                                              * bit[3] = s1
                                                                                              * bit[1] = s3_1
																							  * bit[0] = reserved
                                                                                              * "
                                                                                              */
#define RXI_CORE_TIMEOUT_CLEAR_N_0(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_TIMEOUT_CLEAR_N_0(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_CLEAR_N_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_CLEAR_N_1             ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0xffffffff
                                                                                              * "The timeout clear register, this register can only be written to 0 when a timeout occurs, and automatically clear this register by hardware
                                                                                              * bit[4] = s8_15
                                                                                              * bit[3] = s8_14
                                                                                              * bit[2] = s8_13
                                                                                              * bit[1] = s8_12
                                                                                              * bit[0] = s8_11
                                                                                              * "
                                                                                              */
#define RXI_CORE_TIMEOUT_CLEAR_N_1(x)               ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_TIMEOUT_CLEAR_N_1(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_0_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S0, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_0_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_0_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_0             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_0(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_0(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_1_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S3-1, S3-2, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_1_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_1_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_1             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_1(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_1(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_2_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S1-1, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_2_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_2_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_2             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_2(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_2(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_3_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S1-2, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_3_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_3_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_3             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_3(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_3(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_4_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S5, S4-8, S4-9 , contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_4_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_4_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_4             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_4(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_4(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_5_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S6, S7, S4-10, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_5_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_5_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_5             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_5(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_5(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_6
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_6_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S2-1 to S2-6, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_6_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_6_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_6             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_6(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_6(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_7
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_7_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S4-1, S4-2, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_7_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_7_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_7             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_7(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_7(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_8
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_8_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S4-3, S4-5, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_8_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_8_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_8             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_8(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_8(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_9
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_9_THRESHOLD   ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S4-4, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_9_THRESHOLD(x)     ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_9_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_9             ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_9(x)               ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_9(x)           ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_10
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_10_THRESHOLD  ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S4-7, S4-7, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_10_THRESHOLD(x)    ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_10_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_10            ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_10(x)              ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_10(x)          ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_11
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_11_THRESHOLD  ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S8-1 to S8-11, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_11_THRESHOLD(x)    ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_11_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_11            ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_11(x)              ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_11(x)          ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup TIMEOUT_MON_CFG_12
 * @brief
 * @{
 */
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_12_THRESHOLD  ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0xff
                                                                                              * "The timeout monitor config register for S8-12 to S8-15, contains one threshold and one grade_select
                                                                                              * threshold = TIMEOUT_MON_CFG[7:0]"
                                                                                              */
#define RXI_CORE_TIMEOUT_MON_CFG_12_THRESHOLD(x)    ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_12_THRESHOLD(x) ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_TIMEOUT_MON_CFG_12            ((u32)0x0000000F << 16)                  /*!< READ_WRITE 0xf "grade_select = TIMEOUT_MON_CFG[19:16]" */
#define RXI_CORE_TIMEOUT_MON_CFG_12(x)              ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_TIMEOUT_MON_CFG_12(x)          ((u32)(((x >> 16) & 0x0000000F)))
/** @} */


/** @defgroup RBARA0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA0_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f000 "MPC Base address 0, it is 4KB align, the default value is for video_mem_0" */
#define RXI_CORE_IDAU_RBARA0_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA0_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA0
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA0_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f002 "MPC Limit address 0, it is 4KB align, the default value is for video_mem_0" */
#define RXI_CORE_IDAU_RLARA0_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA0_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA1_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f005 "MPC Base address 1, it is 4KB align, the default value is for video_mem_1" */
#define RXI_CORE_IDAU_RBARA1_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA1_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA1
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA1_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f006 "MPC Limit address 1, it is 4KB align, the default value is for video_mem_1" */
#define RXI_CORE_IDAU_RLARA1_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA1_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA2_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f009 "MPC Base address 2, it is 4KB align, the default value is for video_mem_2" */
#define RXI_CORE_IDAU_RBARA2_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA2_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA2_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f00a "MPC Limit address 2, it is 4KB align, the default value is for video_mem_2" */
#define RXI_CORE_IDAU_RLARA2_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA2_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA3_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f00b "MPC Base address 2, it is 4KB align, the default value is for video_mem_3" */
#define RXI_CORE_IDAU_RBARA3_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA3_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA3_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f00f "MPC Limit address 2, it is 4KB align, the default value is for video_mem_3" */
#define RXI_CORE_IDAU_RLARA3_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA3_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA4_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f010 "MPC Base address 2, it is 4KB align, the default value is for video_mem_4" */
#define RXI_CORE_IDAU_RBARA4_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA4_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA4
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA4_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f020 "MPC Limit address 2, it is 4KB align, the default value is for video_mem_4" */
#define RXI_CORE_IDAU_RLARA4_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA4_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA5_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f030 "MPC Base address 2, it is 4KB align, the default value is for video_mem_5" */
#define RXI_CORE_IDAU_RBARA5_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA5_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA5
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA5_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f031 "MPC Limit address 2, it is 4KB align, the default value is for video_mem_5" */
#define RXI_CORE_IDAU_RLARA5_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA5_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA6
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA6_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f032 "MPC Base address 2, it is 4KB align, the default value is for video_mem_6" */
#define RXI_CORE_IDAU_RBARA6_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA6_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA6
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA6_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f033 "MPC Limit address 2, it is 4KB align, the default value is for video_mem_6" */
#define RXI_CORE_IDAU_RLARA6_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA6_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RBARA7
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RBARA7_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f050 "MPC Base address 2, it is 4KB align, the default value is for video_mem_7" */
#define RXI_CORE_IDAU_RBARA7_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RBARA7_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup RLARA7
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_RLARA7_4KB_ALIGN         ((u32)0x000FFFFF << 12)                  /*!< READ_WRITE 0x6f070 "MPC Limit address 2, it is 4KB align, the default value is for video_mem_7" */
#define RXI_CORE_IDAU_RLARA7_4KB_ALIGN(x)           ((u32)(((x) & 0x000FFFFF) << 12))
#define RXI_CORE_GET_IDAU_RLARA7_4KB_ALIGN(x)       ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */


/** @defgroup IDAU_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_CTRL_ENABLE              ((u32)0x0000FFFF << 0)                   /*!< READ_WRITE 0x0000
                                                                                              * "The control register is use to config address range as secure, it enable region 10 to 0 respectively
                                                                                              * bit[7:0] for S5 8 regions, 1 for nonsecure, 0 for secure
                                                                                              * bit[8] for ca32 acp region, 1 for nonsecure, 0 for secure
                                                                                              * bit[9] for media_km4 region, 1 for nonsecure, 0 for secure
                                                                                              * bit[10] for gic region, 1 for nonsecure, 0 for secure
                                                                                              * bit[11] for ai_dns region, 1 for nonsecure, 0 for secure
                                                                                              * bit[12:15] not used"
                                                                                              */
#define RXI_CORE_IDAU_CTRL_ENABLE(x)                ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_IDAU_CTRL_ENABLE(x)            ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup LOCK
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_LOCK_READ_AS_ZERO        ((u32)0x7FFFFFFF << 1)                   /*!< READ_ONLY 0x00000000 "" */
#define RXI_CORE_IDAU_LOCK_READ_AS_ZERO(x)          ((u32)(((x) & 0x7FFFFFFF) << 1))
#define RXI_CORE_GET_IDAU_LOCK_READ_AS_ZERO(x)      ((u32)(((x >> 1) & 0x7FFFFFFF)))
#define RXI_CORE_BIT_IDAU_LOCK                      ((u32)0x00000001 << 0)                   /*!< READ_WRITE 0x0 "The Lock register. Set this bit to 1 will disable write attribute of all IDAU registers,  and it can't be wrote to 0 again until system reset." */
/** @} */


/** @defgroup IDAU_DIR
 * @brief
 * @{
 */
#define RXI_CORE_MASK_IDAU_DIR                      ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x00100c00 "IDAU design information register" */
#define RXI_CORE_IDAU_DIR(x)                        ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_IDAU_DIR(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup GLOBAL_INTR_RAW_STATUS
 * @brief
 * @{
 */
#define RXI_CORE_MASK_ELR_INTR_RECORD               ((u32)0xFFFFFFFF << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "It recorde elr_intr which include address hole, secure fault, timeout
                                                                                              * bit[0] = address hole
                                                                                              * bit[1] = timeout s0,s3
                                                                                              * bit[2] = timeout s1,s5,s6,s7
                                                                                              * bit[3] = timeout s2
                                                                                              * bit[4] = timeout s4
                                                                                              * bit[5] = timeout s8-1 to s8-11 group
                                                                                              * bit[6] = timeout s8-12 to s8-15 group
                                                                                              * bit[7] = security error
                                                                                              * "
                                                                                              */
#define RXI_CORE_ELR_INTR_RECORD(x)                 ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_ELR_INTR_RECORD(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_CONTROL
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_CONTROL                  ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "The mask control register
                                                                                              * The ELR0_0 is controlled by MASK_CONTROL[1:0], ELR0_1 is controlled by MASK_CONTROL[3:2], and so on.
                                                                                              * MASK_CONTROL[N+1:N] = 2'b00 -> interrupt will not be masked
                                                                                              * MASK_CONTROL[N+1:N] = 2'b01 -> interrupt will be masked depends on ERR_CODE"
                                                                                              */
#define RXI_CORE_MASK_CONTROL(x)                    ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_CONTROL(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_ADDR_HOLE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches.The error_code will be re-encoded to one-hot signals.
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S0_S3
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S1_S5_S6_S7
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S2
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S8_1_to_S8_11
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_S8_12_to_S8_15
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup MASK_ERR_CODE_TMO_SECURITY_FAULT
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MASK_ERR_CODE                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0
                                                                                              * "Mask interrupt when ERR_CODE matches
                                                                                              * If `ERR_CODE is 8'h1(address_hole) -> re-encode MASK_ERR_CODE as 32'h1;
                                                                                              * If `ERR_CODE is 8'h2(secure_fault) -> re-encode MASK_ERR_CODE as 32'h2;
                                                                                              * If `ERR_CODE is 8'h38(arready timeout) -> re-encode MASK_ERR_CODE as 32'h10;
                                                                                              * If `ERR_CODE is 8'h39(rvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h20;
                                                                                              * If `ERR_CODE is 8'h3a(awready_timeout) -> re-encode MASK_ERR_CODE as 32'h40;
                                                                                              * If `ERR_CODE is 8'h3b(wready_timeout) -> re-encode MASK_ERR_CODE as 32'h80;
                                                                                              * If `ERR_CODE is 8'h3c(bvalid_timeout) -> re-encode MASK_ERR_CODE as 32'h100;"
                                                                                              */
#define RXI_CORE_MASK_ERR_CODE(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_MASK_ERR_CODE(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_QOS_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_BIT_EN_AR_RATE_CTRL                ((u32)0x00000001 << 0)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) request issue control for AR channel. Functions of bits [25:19] are only valid if this bit is 1" */
#define RXI_CORE_BIT_NN_EN_AR_OST_CTRL              ((u32)0x00000001 << 1)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) outstanding transaction regulation for AR channel." */
#define RXI_CORE_BIT_NN_EN_AR_BURST_CHOP            ((u32)0x00000001 << 2)                   /*!< READ_WRITE 0x0 Enable (1) or disable (0) burst chop for AR channel."""" */
#define RXI_CORE_BIT_NN_EN_AR_HCI_CTRL              ((u32)0x00000001 << 3)                   /*!< READ_WRITE 0x0 Enable (1) or disable (0) hardware control interface. The regulator is controlled by the union of software control registers and hardware control interface."""" */
#define RXI_CORE_BIT_NN_EN_AR_CFP_BLK               ((u32)0x00000001 << 4)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) CFP control. Note that it has to enable this function not only for the regulator in CFP but also the regulators which are deferred." */
#define RXI_CORE_BIT_NN_EN_AR_TFAME_CTRL            ((u32)0x00000001 << 5)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) the time-frame control mechanism which let the regulator automatically switches between peak rate and average rate modes." */
#define RXI_CORE_BIT_NN_BLK_AR                      ((u32)0x00000001 << 6)                   /*!< READ_WRITE 0x0 "Contrary to SW_AR_TO_CFP, this register is used to defer the ARchannel itself because other reuqest channel demands CFP." */
#define RXI_CORE_BIT_NN_SW_AR_TO_AVG                ((u32)0x00000001 << 7)                   /*!< WRITE_ONLY 0x0 "Switch AR channel to average rate mode." */
#define RXI_CORE_BIT_NN_SW_AR_TO_PEAK               ((u32)0x00000001 << 8)                   /*!< WRITE_ONLY 0x0 "Switch AR channel to peak rate mode." */
#define RXI_CORE_BIT_NN_SW_AR_TO_CFP                ((u32)0x00000001 << 9)                   /*!< WRITE_ONLY 0x0 "Switch AR channel to CFP. User has to ensure no more than one request channel is in CFP at the same time." */
#define RXI_CORE_BIT_NN_EN_AW_RATE_CTRL             ((u32)0x00000001 << 16)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) request issue control for AW channel. Functions of bits [25:19] are only valid if this bit is 1" */
#define RXI_CORE_BIT_NN_EN_AW_OST_CTRL              ((u32)0x00000001 << 17)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) outstanding transaction regulation for AW channel." */
#define RXI_CORE_BIT_NN_EN_AW_BURST_CHOP            ((u32)0x00000001 << 18)                  /*!< READ_WRITE 0x0 Enable (1) or disable (0) burst chop for AW channel."""" */
#define RXI_CORE_BIT_NN_EN_AW_HCI_CTRL              ((u32)0x00000001 << 19)                  /*!< READ_WRITE 0x0 Enable (1) or disable (0) hardware control interface. The regulator is controlled by the union of software control registers and hardware control interface."""" */
#define RXI_CORE_BIT_NN_EN_AW_CFP_BLK               ((u32)0x00000001 << 20)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) CFP control. Note that it has to enable this function not only for the regulator in CFP but also the regulators which are deferred." */
#define RXI_CORE_BIT_NN_EN_AW_TFAME_CTRL            ((u32)0x00000001 << 21)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) the time-frame control mechanism which let the regulator automatically switches between peak rate and average rate modes." */
#define RXI_CORE_BIT_NN_BLK_AW                      ((u32)0x00000001 << 22)                  /*!< READ_WRITE 0x0 "Contrary to SW_AW_TO_CFP, this register is used to defer the AWchannel itself because other reuqest channel demands CFP." */
#define RXI_CORE_BIT_NN_SW_AW_TO_AVG                ((u32)0x00000001 << 23)                  /*!< WRITE_ONLY 0x0 "Switch AW channel to average rate mode." */
#define RXI_CORE_BIT_NN_SW_AW_TO_PEAK               ((u32)0x00000001 << 24)                  /*!< WRITE_ONLY 0x0 "Switch AW channel to peak rate mode." */
#define RXI_CORE_BIT_NN_SW_AW_TO_CFP                ((u32)0x00000001 << 25)                  /*!< WRITE_ONLY 0x0 "Switch AW channel to CFP. User has to ensure no more than one request channel is in CFP at the same time." */
/** @} */


/** @defgroup NN_MAX_OST
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MAX_AR_OST                    ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0x0 "The maximum number of outstanding write transactions issued to downstream components. This register is only valid if EN_AR_OST_CTRL in QOS_CTRL is enabled." */
#define RXI_CORE_MAX_AR_OST(x)                      ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_MAX_AR_OST(x)                  ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_NN_MAX_AW_OST                 ((u32)0x000000FF << 8)                   /*!< READ_WRITE 0x0 "The maximum number of outstanding write transactions issued to downstream components. This register is only valid if EN_AW_OST_CTRL in QOS_CTRL is enabled." */
#define RXI_CORE_NN_MAX_AW_OST(x)                   ((u32)(((x) & 0x000000FF) << 8))
#define RXI_CORE_GET_NN_MAX_AW_OST(x)               ((u32)(((x >> 8) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AR_QOS                     ((u32)0x0000000F << 24)                  /*!< READ_WRITE 0x0 "Programmable QoS value of AR channel, this register is notvalid is this version." */
#define RXI_CORE_NN_AR_QOS(x)                       ((u32)(((x) & 0x0000000F) << 24))
#define RXI_CORE_GET_NN_AR_QOS(x)                   ((u32)(((x >> 24) & 0x0000000F)))
#define RXI_CORE_MASK_NN_AW_QOS                     ((u32)0x0000000F << 28)                  /*!< READ_WRITE 0x0 "Programmable QoS value of AW channel, this register is notvalid is this version." */
#define RXI_CORE_NN_AW_QOS(x)                       ((u32)(((x) & 0x0000000F) << 28))
#define RXI_CORE_GET_NN_AW_QOS(x)                   ((u32)(((x >> 28) & 0x0000000F)))
/** @} */


/** @defgroup NN_BURST_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_BIT_EN_DRAM_ALIGN                  ((u32)0x00000001 << 0)                   /*!< READ_WRITE 0x0 "Enables (1) or disables (0) a customized burst length alignment for every first downstream command to favor the DRAM controller for both read and write transactions." */
#define RXI_CORE_MASK_NN_BURST_CHOP_POW2            ((u32)0x00000007 << 8)                   /*!< READ_WRITE 0x0 "The burst length constraint for the downstream components. The constraint presents the number of bytes in power of 2. For example, suppose BURST_CHOP_POW2 is 5 (i.e., 32 bytes), then the maximum burst length after burst chop translation is 4 if the data width is 8 bytes wide." */
#define RXI_CORE_NN_BURST_CHOP_POW2(x)              ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_NN_BURST_CHOP_POW2(x)          ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_NN_DRAM_ALIGN_POW2            ((u32)0x00000007 << 12)                  /*!< READ_WRITE 0x0 "The additional constraint for every first downstream command, which is represented as the number of bytes in power of 2." */
#define RXI_CORE_NN_DRAM_ALIGN_POW2(x)              ((u32)(((x) & 0x00000007) << 12))
#define RXI_CORE_GET_NN_DRAM_ALIGN_POW2(x)          ((u32)(((x >> 12) & 0x00000007)))
/** @} */


/** @defgroup NN_QOS_STATUS
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_QOS_STATE                  ((u32)0x0000000F << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "This signal represents combination of status of burst chop and request issue control (i.e., {burst chop status, request issue control status}).
                                                                                              * 4'b0000: {IDLE, IDLE}
                                                                                              * 4'b0001: {IDLE, Avg. Rate}
                                                                                              * 4'b0010: {IDLE, Peak Rate}
                                                                                              * 4'b0011: {IDLE, CFP}
                                                                                              * 4'b0100: {IDLE, BLOCK}
                                                                                              * 4'b0101: {First Cmd., IDLE}
                                                                                              * 4'b0110: {First Cmd., Avg. Rate}
                                                                                              * 4'b0111: {First Cmd., Peak Rate}
                                                                                              * 4'b1000: {First Cmd., CFP}
                                                                                              * 4'b1001: {First Cmd., BLOCK}
                                                                                              * 4'b1010: {Other Cmd., IDLE}
                                                                                              * 4'b1011: {Other Cmd., Avg. Rate}
                                                                                              * 4'b1100: {Other Cmd., Peak Rate}
                                                                                              * 4'b1101: {Other Cmd., CFP}
                                                                                              * 4'b1110: {Other Cmd., BLOCK}"
                                                                                              */
#define RXI_CORE_AR_QOS_STATE(x)                    ((u32)(((x) & 0x0000000F) << 0))
#define RXI_CORE_GET_AR_QOS_STATE(x)                ((u32)(((x >> 0) & 0x0000000F)))
#define RXI_CORE_BIT_NN_AR_ACTIVE                   ((u32)0x00000001 << 4)                   /*!< READ_ONLY 0x0 "This signal is asserted when there is at least one pending write transaction." */
#define RXI_CORE_BIT_NN_RD_DATA_FIFO_FULL           ((u32)0x00000001 << 5)                   /*!< READ_ONLY 0x0 "This signal is asserted when read data FIFO is full." */
#define RXI_CORE_BIT_NN_AR_CMD_LE_HALF              ((u32)0x00000001 << 6)                   /*!< READ_ONLY 0x1 "This signal is asserted when the number of valid entries in AR command FIFO is less than half of total number of entries." */
#define RXI_CORE_BIT_NN_AR_CMD_FULL                 ((u32)0x00000001 << 7)                   /*!< READ_ONLY 0x1 "This signal is asserted when the AR command FIFO is full." */
#define RXI_CORE_BIT_NN_AR_BOUND_BY_OSR             ((u32)0x00000001 << 8)                   /*!< READ_ONLY 0x0 "This signal is similar to [9], but the write requests are blocked because of the maximum number of outstanding write transactions." */
#define RXI_CORE_BIT_NN_AR_BOUND_BY_RATE            ((u32)0x00000001 << 9)                   /*!< READ_ONLY 0x0 "This signal is asserted when the regulator blocks one or more read requests because the effective bandwidth reaches the maximum bandwidth constraint" */
#define RXI_CORE_BIT_NN_AR_DS_STALL                 ((u32)0x00000001 << 10)                  /*!< READ_ONLY 0x0 "This signal is asserted when the regulator issues a read request to downstream and the request is not accepted by the downstream component."" */
#define RXI_CORE_BIT_NN_AR_IN_AVG                   ((u32)0x00000001 << 12)                  /*!< READ_ONLY 0x0 "1 = the AR channel is in average rate mode." */
#define RXI_CORE_BIT_NN_AR_IN_PEAK                  ((u32)0x00000001 << 13)                  /*!< READ_ONLY 0x0 "1 = the AR channel is in peak rate mode." */
#define RXI_CORE_BIT_NN_AR_IN_CFP                   ((u32)0x00000001 << 14)                  /*!< READ_ONLY 0x0 "1 = the AR channel is in CFP state." */
#define RXI_CORE_MASK_NN_AW_QOS_STATE               ((u32)0x0000000F << 16)                  /*!< READ_ONLY 0x0
                                                                                              * "This signal represents combination of status of burst chop and request issue control (i.e., {burst chop status, request issue control status}).
                                                                                              * 4'b0000: {IDLE, IDLE}
                                                                                              * 4'b0001: {IDLE, Avg. Rate}
                                                                                              * 4'b0010: {IDLE, Peak Rate}
                                                                                              * 4'b0011: {IDLE, CFP}
                                                                                              * 4'b0100: {IDLE, BLOCK}
                                                                                              * 4'b0101: {First Cmd., IDLE}
                                                                                              * 4'b0110: {First Cmd., Avg. Rate}
                                                                                              * 4'b0111: {First Cmd., Peak Rate}
                                                                                              * 4'b1000: {First Cmd., CFP}
                                                                                              * 4'b1001: {First Cmd., BLOCK}
                                                                                              * 4'b1010: {Other Cmd., IDLE}
                                                                                              * 4'b1011: {Other Cmd., Avg. Rate}
                                                                                              * 4'b1100: {Other Cmd., Peak Rate}
                                                                                              * 4'b1101: {Other Cmd., CFP}
                                                                                              * 4'b1110: {Other Cmd., BLOCK}"
                                                                                              */
#define RXI_CORE_NN_AW_QOS_STATE(x)                 ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_NN_AW_QOS_STATE(x)             ((u32)(((x >> 16) & 0x0000000F)))
#define RXI_CORE_BIT_NN_AW_ACTIVE                   ((u32)0x00000001 << 20)                  /*!< READ_ONLY 0x0 "This signal is asserted when there is at least one pending write transaction." */
#define RXI_CORE_BIT_NN_WD_DATA_FIFO_FULL           ((u32)0x00000001 << 21)                  /*!< READ_ONLY 0x0 "This signal is asserted when write data FIFO is full." */
#define RXI_CORE_BIT_NN_AW_CMD_LE_HALF              ((u32)0x00000001 << 22)                  /*!< READ_ONLY 0x1 "This signal is asserted when the number of valid entries in AW command FIFO is less than half of total number of entries." */
#define RXI_CORE_BIT_NN_AW_CMD_FULL                 ((u32)0x00000001 << 23)                  /*!< READ_ONLY 0x1 "This signal is asserted when the AW command FIFO is full." */
#define RXI_CORE_BIT_NN_AW_BOUND_BY_OSR             ((u32)0x00000001 << 24)                  /*!< READ_ONLY 0x0 "This signal is similar to [25], but the write requests are blocked because of the maximum number of outstanding write transactions." */
#define RXI_CORE_BIT_NN_AW_BOUND_BY_RATE            ((u32)0x00000001 << 25)                  /*!< READ_ONLY 0x0 This signal is asserted when the regulator blocks one or more write requests because the effective bandwidth reaches the maximum bandwidth constraint."""" */
#define RXI_CORE_BIT_NN_AW_DS_STALL                 ((u32)0x00000001 << 26)                  /*!< READ_ONLY 0x0 "This signal is asserted when the regulator issues a write request to downstream and the request is not accepted by the downstream component." */
#define RXI_CORE_BIT_NN_AW_IN_AVG                   ((u32)0x00000001 << 29)                  /*!< READ_ONLY 0x0 "1 = the AW channel is in average rate mode." */
#define RXI_CORE_BIT_NN_AW_IN_PEAK                  ((u32)0x00000001 << 30)                  /*!< READ_ONLY 0x0 "1 = the AW channel is in peak rate mode." */
#define RXI_CORE_BIT_NN_AW_IN_CFP                   ((u32)0x00000001 << 31)                  /*!< READ_ONLY 0x0 "1 = the AW channel is in CFP state." */
/** @} */


/** @defgroup NN_AW_RATE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_AVG_RATE_D                 ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_AW_AVG_RATE_D(x)                   ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_AW_AVG_RATE_D(x)               ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AW_AVG_RATE_N              ((u32)0x000000FF << 8)                   /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_NN_AW_AVG_RATE_N(x)                ((u32)(((x) & 0x000000FF) << 8))
#define RXI_CORE_GET_NN_AW_AVG_RATE_N(x)            ((u32)(((x >> 8) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AW_PEAK_RATE_D             ((u32)0x000000FF << 16)                  /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_NN_AW_PEAK_RATE_D(x)               ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_NN_AW_PEAK_RATE_D(x)           ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AW_PEAK_RATE_N             ((u32)0x000000FF << 24)                  /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_NN_AW_PEAK_RATE_N(x)               ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_NN_AW_PEAK_RATE_N(x)           ((u32)(((x >> 24) & 0x000000FF)))
/** @} */


/** @defgroup NN_AW_PEAK_RATE_DATA
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_PRATE_DATA                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The maximum number of bytes transferred in peak rate mode or CFP." */
#define RXI_CORE_AW_PRATE_DATA(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AW_PRATE_DATA(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_AW_TFRAME_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_TFRAME_LEN                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of a time frame." */
#define RXI_CORE_AW_TFRAME_LEN(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AW_TFRAME_LEN(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_AW_TFRAME_P1_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_TFRAME_P1_LEN              ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of phase 1 in a time frame" */
#define RXI_CORE_AW_TFRAME_P1_LEN(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AW_TFRAME_P1_LEN(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_AW_CONT_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_MIN_SUCC                   ((u32)0x0000FFFF << 0)                   /*!< READ_WRITE 0x0 "The regulator defers requests until this number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_AW_MIN_SUCC(x)                     ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_AW_MIN_SUCC(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
#define RXI_CORE_MASK_NN_AW_MAX_SUCC                ((u32)0x0000FFFF << 16)                  /*!< READ_WRITE 0x0 "The maximum number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_NN_AW_MAX_SUCC(x)                  ((u32)(((x) & 0x0000FFFF) << 16))
#define RXI_CORE_GET_NN_AW_MAX_SUCC(x)              ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */


/** @defgroup NN_AR_RATE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_AVG_RATE_D                 ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_AR_AVG_RATE_D(x)                   ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_AR_AVG_RATE_D(x)               ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AR_AVG_RATE_N              ((u32)0x000000FF << 8)                   /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_NN_AR_AVG_RATE_N(x)                ((u32)(((x) & 0x000000FF) << 8))
#define RXI_CORE_GET_NN_AR_AVG_RATE_N(x)            ((u32)(((x >> 8) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AR_PEAK_RATE_D             ((u32)0x000000FF << 16)                  /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_NN_AR_PEAK_RATE_D(x)               ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_NN_AR_PEAK_RATE_D(x)           ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_NN_AR_PEAK_RATE_N             ((u32)0x000000FF << 24)                  /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_NN_AR_PEAK_RATE_N(x)               ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_NN_AR_PEAK_RATE_N(x)           ((u32)(((x >> 24) & 0x000000FF)))
/** @} */


/** @defgroup NN_AR_PEAK_RATE_DATA
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_PRATE_DATA                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The maximum number of bytes transferred in peak rate mode or CFP." */
#define RXI_CORE_AR_PRATE_DATA(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AR_PRATE_DATA(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_AR_TFRAME_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_TFRAME_LEN                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of a time frame." */
#define RXI_CORE_AR_TFRAME_LEN(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AR_TFRAME_LEN(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_AR_TFRAME_P1_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_TFRAME_P1_LEN              ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of phase 1 in a time frame" */
#define RXI_CORE_AR_TFRAME_P1_LEN(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AR_TFRAME_P1_LEN(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup NN_AR_CONT_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_MIN_SUCC                   ((u32)0x0000FFFF << 0)                   /*!< READ_WRITE 0x0 "The regulator defers requests until this number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_AR_MIN_SUCC(x)                     ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_AR_MIN_SUCC(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
#define RXI_CORE_MASK_NN_AR_MAX_SUCC                ((u32)0x0000FFFF << 16)                  /*!< READ_WRITE 0x0 "The maximum number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_NN_AR_MAX_SUCC(x)                  ((u32)(((x) & 0x0000FFFF) << 16))
#define RXI_CORE_GET_NN_AR_MAX_SUCC(x)              ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */


/** @defgroup ISP_QOS_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_BIT_EN_AR_RATE_CTRL                ((u32)0x00000001 << 0)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) request issue control for AR channel. Functions of bits [25:19] are only valid if this bit is 1" */
#define RXI_CORE_BIT_ISP_EN_AR_OST_CTRL             ((u32)0x00000001 << 1)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) outstanding transaction regulation for AR channel." */
#define RXI_CORE_BIT_ISP_EN_AR_BURST_CHOP           ((u32)0x00000001 << 2)                   /*!< READ_WRITE 0x0 Enable (1) or disable (0) burst chop for AR channel."""" */
#define RXI_CORE_BIT_ISP_EN_AR_HCI_CTRL             ((u32)0x00000001 << 3)                   /*!< READ_WRITE 0x0 Enable (1) or disable (0) hardware control interface. The regulator is controlled by the union of software control registers and hardware control interface."""" */
#define RXI_CORE_BIT_ISP_EN_AR_CFP_BLK              ((u32)0x00000001 << 4)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) CFP control. Note that it has to enable this function not only for the regulator in CFP but also the regulators which are deferred." */
#define RXI_CORE_BIT_ISP_EN_AR_TFAME_CTRL           ((u32)0x00000001 << 5)                   /*!< READ_WRITE 0x0 "Enable (1) or disable (0) the time-frame control mechanism which let the regulator automatically switches between peak rate and average rate modes." */
#define RXI_CORE_BIT_ISP_BLK_AR                     ((u32)0x00000001 << 6)                   /*!< READ_WRITE 0x0 "Contrary to SW_AR_TO_CFP, this register is used to defer the ARchannel itself because other reuqest channel demands CFP." */
#define RXI_CORE_BIT_ISP_SW_AR_TO_AVG               ((u32)0x00000001 << 7)                   /*!< WRITE_ONLY 0x0 "Switch AR channel to average rate mode." */
#define RXI_CORE_BIT_ISP_SW_AR_TO_PEAK              ((u32)0x00000001 << 8)                   /*!< WRITE_ONLY 0x0 "Switch AR channel to peak rate mode." */
#define RXI_CORE_BIT_ISP_SW_AR_TO_CFP               ((u32)0x00000001 << 9)                   /*!< WRITE_ONLY 0x0 "Switch AR channel to CFP. User has to ensure no more than one request channel is in CFP at the same time." */
#define RXI_CORE_BIT_ISP_EN_AW_RATE_CTRL            ((u32)0x00000001 << 16)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) request issue control for AW channel. Functions of bits [25:19] are only valid if this bit is 1" */
#define RXI_CORE_BIT_ISP_EN_AW_OST_CTRL             ((u32)0x00000001 << 17)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) outstanding transaction regulation for AW channel." */
#define RXI_CORE_BIT_ISP_EN_AW_BURST_CHOP           ((u32)0x00000001 << 18)                  /*!< READ_WRITE 0x0 Enable (1) or disable (0) burst chop for AW channel."""" */
#define RXI_CORE_BIT_ISP_EN_AW_HCI_CTRL             ((u32)0x00000001 << 19)                  /*!< READ_WRITE 0x0 Enable (1) or disable (0) hardware control interface. The regulator is controlled by the union of software control registers and hardware control interface."""" */
#define RXI_CORE_BIT_ISP_EN_AW_CFP_BLK              ((u32)0x00000001 << 20)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) CFP control. Note that it has to enable this function not only for the regulator in CFP but also the regulators which are deferred." */
#define RXI_CORE_BIT_ISP_EN_AW_TFAME_CTRL           ((u32)0x00000001 << 21)                  /*!< READ_WRITE 0x0 "Enable (1) or disable (0) the time-frame control mechanism which let the regulator automatically switches between peak rate and average rate modes." */
#define RXI_CORE_BIT_ISP_BLK_AW                     ((u32)0x00000001 << 22)                  /*!< READ_WRITE 0x0 "Contrary to SW_AW_TO_CFP, this register is used to defer the AWchannel itself because other reuqest channel demands CFP." */
#define RXI_CORE_BIT_ISP_SW_AW_TO_AVG               ((u32)0x00000001 << 23)                  /*!< WRITE_ONLY 0x0 "Switch AW channel to average rate mode." */
#define RXI_CORE_BIT_ISP_SW_AW_TO_PEAK              ((u32)0x00000001 << 24)                  /*!< WRITE_ONLY 0x0 "Switch AW channel to peak rate mode." */
#define RXI_CORE_BIT_ISP_SW_AW_TO_CFP               ((u32)0x00000001 << 25)                  /*!< WRITE_ONLY 0x0 "Switch AW channel to CFP. User has to ensure no more than one request channel is in CFP at the same time." */
/** @} */


/** @defgroup ISP_MAX_OST
 * @brief
 * @{
 */
#define RXI_CORE_MASK_MAX_AR_OST                    ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0x0 "The maximum number of outstanding write transactions issued to downstream components. This register is only valid if EN_AR_OST_CTRL in QOS_CTRL is enabled." */
#define RXI_CORE_MAX_AR_OST(x)                      ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_MAX_AR_OST(x)                  ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_MAX_AW_OST                ((u32)0x000000FF << 8)                   /*!< READ_WRITE 0x0 "The maximum number of outstanding write transactions issued to downstream components. This register is only valid if EN_AW_OST_CTRL in QOS_CTRL is enabled." */
#define RXI_CORE_ISP_MAX_AW_OST(x)                  ((u32)(((x) & 0x000000FF) << 8))
#define RXI_CORE_GET_ISP_MAX_AW_OST(x)              ((u32)(((x >> 8) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AR_QOS                    ((u32)0x0000000F << 24)                  /*!< READ_WRITE 0x0 "Programmable QoS value of AR channel, this register is notvalid is this version." */
#define RXI_CORE_ISP_AR_QOS(x)                      ((u32)(((x) & 0x0000000F) << 24))
#define RXI_CORE_GET_ISP_AR_QOS(x)                  ((u32)(((x >> 24) & 0x0000000F)))
#define RXI_CORE_MASK_ISP_AW_QOS                    ((u32)0x0000000F << 28)                  /*!< READ_WRITE 0x0 "Programmable QoS value of AW channel, this register is notvalid is this version." */
#define RXI_CORE_ISP_AW_QOS(x)                      ((u32)(((x) & 0x0000000F) << 28))
#define RXI_CORE_GET_ISP_AW_QOS(x)                  ((u32)(((x >> 28) & 0x0000000F)))
/** @} */


/** @defgroup ISP_BURST_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_BIT_EN_DRAM_ALIGN                  ((u32)0x00000001 << 0)                   /*!< READ_WRITE 0x0 "Enables (1) or disables (0) a customized burst length alignment for every first downstream command to favor the DRAM controller for both read and write transactions." */
#define RXI_CORE_MASK_ISP_BURST_CHOP_POW2           ((u32)0x00000007 << 8)                   /*!< READ_WRITE 0x0 "The burst length constraint for the downstream components. The constraint presents the number of bytes in power of 2. For example, suppose BURST_CHOP_POW2 is 5 (i.e., 32 bytes), then the maximum burst length after burst chop translation is 4 if the data width is 8 bytes wide." */
#define RXI_CORE_ISP_BURST_CHOP_POW2(x)             ((u32)(((x) & 0x00000007) << 8))
#define RXI_CORE_GET_ISP_BURST_CHOP_POW2(x)         ((u32)(((x >> 8) & 0x00000007)))
#define RXI_CORE_MASK_ISP_DRAM_ALIGN_POW2           ((u32)0x00000007 << 12)                  /*!< READ_WRITE 0x0 "The additional constraint for every first downstream command, which is represented as the number of bytes in power of 2." */
#define RXI_CORE_ISP_DRAM_ALIGN_POW2(x)             ((u32)(((x) & 0x00000007) << 12))
#define RXI_CORE_GET_ISP_DRAM_ALIGN_POW2(x)         ((u32)(((x >> 12) & 0x00000007)))
/** @} */


/** @defgroup ISP_QOS_STATUS
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_QOS_STATE                  ((u32)0x0000000F << 0)                   /*!< READ_ONLY 0x0
                                                                                              * "This signal represents combination of status of burst chop and request issue control (i.e., {burst chop status, request issue control status}).
                                                                                              * 4'b0000: {IDLE, IDLE}
                                                                                              * 4'b0001: {IDLE, Avg. Rate}
                                                                                              * 4'b0010: {IDLE, Peak Rate}
                                                                                              * 4'b0011: {IDLE, CFP}
                                                                                              * 4'b0100: {IDLE, BLOCK}
                                                                                              * 4'b0101: {First Cmd., IDLE}
                                                                                              * 4'b0110: {First Cmd., Avg. Rate}
                                                                                              * 4'b0111: {First Cmd., Peak Rate}
                                                                                              * 4'b1000: {First Cmd., CFP}
                                                                                              * 4'b1001: {First Cmd., BLOCK}
                                                                                              * 4'b1010: {Other Cmd., IDLE}
                                                                                              * 4'b1011: {Other Cmd., Avg. Rate}
                                                                                              * 4'b1100: {Other Cmd., Peak Rate}
                                                                                              * 4'b1101: {Other Cmd., CFP}
                                                                                              * 4'b1110: {Other Cmd., BLOCK}"
                                                                                              */
#define RXI_CORE_AR_QOS_STATE(x)                    ((u32)(((x) & 0x0000000F) << 0))
#define RXI_CORE_GET_AR_QOS_STATE(x)                ((u32)(((x >> 0) & 0x0000000F)))
#define RXI_CORE_BIT_ISP_AR_ACTIVE                  ((u32)0x00000001 << 4)                   /*!< READ_ONLY 0x0 "This signal is asserted when there is at least one pending write transaction." */
#define RXI_CORE_BIT_ISP_RD_DATA_FIFO_FULL          ((u32)0x00000001 << 5)                   /*!< READ_ONLY 0x0 "This signal is asserted when read data FIFO is full." */
#define RXI_CORE_BIT_ISP_AR_CMD_LE_HALF             ((u32)0x00000001 << 6)                   /*!< READ_ONLY 0x1 "This signal is asserted when the number of valid entries in AR command FIFO is less than half of total number of entries." */
#define RXI_CORE_BIT_ISP_AR_CMD_FULL                ((u32)0x00000001 << 7)                   /*!< READ_ONLY 0x1 "This signal is asserted when the AR command FIFO is full." */
#define RXI_CORE_BIT_ISP_AR_BOUND_BY_OSR            ((u32)0x00000001 << 8)                   /*!< READ_ONLY 0x0 "This signal is similar to [9], but the write requests are blocked because of the maximum number of outstanding write transactions." */
#define RXI_CORE_BIT_ISP_AR_BOUND_BY_RATE           ((u32)0x00000001 << 9)                   /*!< READ_ONLY 0x0 "This signal is asserted when the regulator blocks one or more read requests because the effective bandwidth reaches the maximum bandwidth constraint" */
#define RXI_CORE_BIT_ISP_AR_DS_STALL                ((u32)0x00000001 << 10)                  /*!< READ_ONLY 0x0 "This signal is asserted when the regulator issues a read request to downstream and the request is not accepted by the downstream component."" */
#define RXI_CORE_BIT_ISP_AR_IN_AVG                  ((u32)0x00000001 << 12)                  /*!< READ_ONLY 0x0 "1 = the AR channel is in average rate mode." */
#define RXI_CORE_BIT_ISP_AR_IN_PEAK                 ((u32)0x00000001 << 13)                  /*!< READ_ONLY 0x0 "1 = the AR channel is in peak rate mode." */
#define RXI_CORE_BIT_ISP_AR_IN_CFP                  ((u32)0x00000001 << 14)                  /*!< READ_ONLY 0x0 "1 = the AR channel is in CFP state." */
#define RXI_CORE_MASK_ISP_AW_QOS_STATE              ((u32)0x0000000F << 16)                  /*!< READ_ONLY 0x0
                                                                                              * "This signal represents combination of status of burst chop and request issue control (i.e., {burst chop status, request issue control status}).
                                                                                              * 4'b0000: {IDLE, IDLE}
                                                                                              * 4'b0001: {IDLE, Avg. Rate}
                                                                                              * 4'b0010: {IDLE, Peak Rate}
                                                                                              * 4'b0011: {IDLE, CFP}
                                                                                              * 4'b0100: {IDLE, BLOCK}
                                                                                              * 4'b0101: {First Cmd., IDLE}
                                                                                              * 4'b0110: {First Cmd., Avg. Rate}
                                                                                              * 4'b0111: {First Cmd., Peak Rate}
                                                                                              * 4'b1000: {First Cmd., CFP}
                                                                                              * 4'b1001: {First Cmd., BLOCK}
                                                                                              * 4'b1010: {Other Cmd., IDLE}
                                                                                              * 4'b1011: {Other Cmd., Avg. Rate}
                                                                                              * 4'b1100: {Other Cmd., Peak Rate}
                                                                                              * 4'b1101: {Other Cmd., CFP}
                                                                                              * 4'b1110: {Other Cmd., BLOCK}"
                                                                                              */
#define RXI_CORE_ISP_AW_QOS_STATE(x)                ((u32)(((x) & 0x0000000F) << 16))
#define RXI_CORE_GET_ISP_AW_QOS_STATE(x)            ((u32)(((x >> 16) & 0x0000000F)))
#define RXI_CORE_BIT_ISP_AW_ACTIVE                  ((u32)0x00000001 << 20)                  /*!< READ_ONLY 0x0 "This signal is asserted when there is at least one pending write transaction." */
#define RXI_CORE_BIT_ISP_WD_DATA_FIFO_FULL          ((u32)0x00000001 << 21)                  /*!< READ_ONLY 0x0 "This signal is asserted when write data FIFO is full." */
#define RXI_CORE_BIT_ISP_AW_CMD_LE_HALF             ((u32)0x00000001 << 22)                  /*!< READ_ONLY 0x1 "This signal is asserted when the number of valid entries in AW command FIFO is less than half of total number of entries." */
#define RXI_CORE_BIT_ISP_AW_CMD_FULL                ((u32)0x00000001 << 23)                  /*!< READ_ONLY 0x1 "This signal is asserted when the AW command FIFO is full." */
#define RXI_CORE_BIT_ISP_AW_BOUND_BY_OSR            ((u32)0x00000001 << 24)                  /*!< READ_ONLY 0x0 "This signal is similar to [25], but the write requests are blocked because of the maximum number of outstanding write transactions." */
#define RXI_CORE_BIT_ISP_AW_BOUND_BY_RATE           ((u32)0x00000001 << 25)                  /*!< READ_ONLY 0x0 This signal is asserted when the regulator blocks one or more write requests because the effective bandwidth reaches the maximum bandwidth constraint."""" */
#define RXI_CORE_BIT_ISP_AW_DS_STALL                ((u32)0x00000001 << 26)                  /*!< READ_ONLY 0x0 "This signal is asserted when the regulator issues a write request to downstream and the request is not accepted by the downstream component." */
#define RXI_CORE_BIT_ISP_AW_IN_AVG                  ((u32)0x00000001 << 29)                  /*!< READ_ONLY 0x0 "1 = the AW channel is in average rate mode." */
#define RXI_CORE_BIT_ISP_AW_IN_PEAK                 ((u32)0x00000001 << 30)                  /*!< READ_ONLY 0x0 "1 = the AW channel is in peak rate mode." */
#define RXI_CORE_BIT_ISP_AW_IN_CFP                  ((u32)0x00000001 << 31)                  /*!< READ_ONLY 0x0 "1 = the AW channel is in CFP state." */
/** @} */


/** @defgroup ISP_AW_RATE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_AVG_RATE_D                 ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_AW_AVG_RATE_D(x)                   ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_AW_AVG_RATE_D(x)               ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AW_AVG_RATE_N             ((u32)0x000000FF << 8)                   /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_ISP_AW_AVG_RATE_N(x)               ((u32)(((x) & 0x000000FF) << 8))
#define RXI_CORE_GET_ISP_AW_AVG_RATE_N(x)           ((u32)(((x >> 8) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AW_PEAK_RATE_D            ((u32)0x000000FF << 16)                  /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_ISP_AW_PEAK_RATE_D(x)              ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ISP_AW_PEAK_RATE_D(x)          ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AW_PEAK_RATE_N            ((u32)0x000000FF << 24)                  /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_ISP_AW_PEAK_RATE_N(x)              ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ISP_AW_PEAK_RATE_N(x)          ((u32)(((x >> 24) & 0x000000FF)))
/** @} */


/** @defgroup ISP_AW_PEAK_RATE_DATA
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_PRATE_DATA                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The maximum number of bytes transferred in peak rate mode or CFP." */
#define RXI_CORE_AW_PRATE_DATA(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AW_PRATE_DATA(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ISP_AW_TFRAME_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_TFRAME_LEN                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of a time frame." */
#define RXI_CORE_AW_TFRAME_LEN(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AW_TFRAME_LEN(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ISP_AW_TFRAME_P1_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_TFRAME_P1_LEN              ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of phase 1 in a time frame" */
#define RXI_CORE_AW_TFRAME_P1_LEN(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AW_TFRAME_P1_LEN(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ISP_AW_CONT_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AW_MIN_SUCC                   ((u32)0x0000FFFF << 0)                   /*!< READ_WRITE 0x0 "The regulator defers requests until this number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_AW_MIN_SUCC(x)                     ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_AW_MIN_SUCC(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
#define RXI_CORE_MASK_ISP_AW_MAX_SUCC               ((u32)0x0000FFFF << 16)                  /*!< READ_WRITE 0x0 "The maximum number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_ISP_AW_MAX_SUCC(x)                 ((u32)(((x) & 0x0000FFFF) << 16))
#define RXI_CORE_GET_ISP_AW_MAX_SUCC(x)             ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */


/** @defgroup ISP_AR_RATE
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_AVG_RATE_D                 ((u32)0x000000FF << 0)                   /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_AR_AVG_RATE_D(x)                   ((u32)(((x) & 0x000000FF) << 0))
#define RXI_CORE_GET_AR_AVG_RATE_D(x)               ((u32)(((x >> 0) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AR_AVG_RATE_N             ((u32)0x000000FF << 8)                   /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in average rate mode for AW channel." */
#define RXI_CORE_ISP_AR_AVG_RATE_N(x)               ((u32)(((x) & 0x000000FF) << 8))
#define RXI_CORE_GET_ISP_AR_AVG_RATE_N(x)           ((u32)(((x >> 8) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AR_PEAK_RATE_D            ((u32)0x000000FF << 16)                  /*!< READ_WRITE 0x0 "The denominator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_ISP_AR_PEAK_RATE_D(x)              ((u32)(((x) & 0x000000FF) << 16))
#define RXI_CORE_GET_ISP_AR_PEAK_RATE_D(x)          ((u32)(((x >> 16) & 0x000000FF)))
#define RXI_CORE_MASK_ISP_AR_PEAK_RATE_N            ((u32)0x000000FF << 24)                  /*!< READ_WRITE 0x0 "The numerator part of the practical maximum bandwidth constraint in peak rate mode for AW channel." */
#define RXI_CORE_ISP_AR_PEAK_RATE_N(x)              ((u32)(((x) & 0x000000FF) << 24))
#define RXI_CORE_GET_ISP_AR_PEAK_RATE_N(x)          ((u32)(((x >> 24) & 0x000000FF)))
/** @} */


/** @defgroup ISP_AR_PEAK_RATE_DATA
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_PRATE_DATA                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The maximum number of bytes transferred in peak rate mode or CFP." */
#define RXI_CORE_AR_PRATE_DATA(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AR_PRATE_DATA(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ISP_AR_TFRAME_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_TFRAME_LEN                 ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of a time frame." */
#define RXI_CORE_AR_TFRAME_LEN(x)                   ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AR_TFRAME_LEN(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ISP_AR_TFRAME_P1_LEN
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_TFRAME_P1_LEN              ((u32)0xFFFFFFFF << 0)                   /*!< READ_WRITE 0x0 "The number of cycles of phase 1 in a time frame" */
#define RXI_CORE_AR_TFRAME_P1_LEN(x)                ((u32)(((x) & 0xFFFFFFFF) << 0))
#define RXI_CORE_GET_AR_TFRAME_P1_LEN(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup ISP_AR_CONT_CTRL
 * @brief
 * @{
 */
#define RXI_CORE_MASK_AR_MIN_SUCC                   ((u32)0x0000FFFF << 0)                   /*!< READ_WRITE 0x0 "The regulator defers requests until this number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_AR_MIN_SUCC(x)                     ((u32)(((x) & 0x0000FFFF) << 0))
#define RXI_CORE_GET_AR_MIN_SUCC(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
#define RXI_CORE_MASK_ISP_AR_MAX_SUCC               ((u32)0x0000FFFF << 16)                  /*!< READ_WRITE 0x0 "The maximum number of requests (represented in number of beats in burst transactions) can be issued successively." */
#define RXI_CORE_ISP_AR_MAX_SUCC(x)                 ((u32)(((x) & 0x0000FFFF) << 16))
#define RXI_CORE_GET_ISP_AR_MAX_SUCC(x)             ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */




/** @defgroup RXI_CORE_Exported_Types RXI_CORE Exported Types
* @{
*/
/**
* @brief RXI_CORE Register Declaration
*/

typedef struct {
	__I  uint32_t NAME;                                         /*!< Register,  Address offset:0x0000 */
	__I  uint32_t VER;                                          /*!< Register,  Address offset:0x0004 */
	__I  uint32_t REV;                                          /*!< Register,  Address offset:0x0008 */
	__I  uint32_t INST;                                         /*!< Register,  Address offset:0x000C */
	__I  uint32_t IMPL_Y;                                       /*!< Register,  Address offset:0x0010 */
	__I  uint32_t IMPL_D;                                       /*!< Register,  Address offset:0x0014 */
	__I  uint32_t DEV;                                          /*!< Register,  Address offset:0x0018 */
	__I  uint32_t PRO_NUM;                                      /*!< Register,  Address offset:0x001C */
	uint32_t RSVD_NO_FUNCTION0[56];
	__I  uint32_t ERR_PLD0_ADDR_HOLE;                           /*!< Register,  Address offset:0x0100 */
	__I  uint32_t ERR_PLD1_ADDR_HOLE;                           /*!< Register,  Address offset:0x0104 */
	__I  uint32_t ERR_ID_ADDR_HOLE;                             /*!< Register,  Address offset:0x0108 */
	__I  uint32_t ERR_ADR0_ADDR_HOLE;                           /*!< Register,  Address offset:0x010C */
	__I  uint32_t ERR_ADR1_ADDR_HOLE;                           /*!< Register,  Address offset:0x0110 */
	uint32_t RSVD_NO_FUNCTION1[7];
	__I  uint32_t ERR_CODE_ADDR_HOLE;                           /*!< Register,  Address offset:0x0130 */
	uint32_t RSVD_NO_FUNCTION2[2];
	__O  uint32_t ERR_INTR_CLR_ADDR_HOLE;                       /*!< Register,  Address offset:0x013C */
	__I  uint32_t ERR_PLD0_TIMEOUT_0;                           /*!< Register,  Address offset:0x0140 */
	__I  uint32_t ERR_PLD1_TIMEOUT_0;                           /*!< Register,  Address offset:0x0144 */
	__I  uint32_t ERR_ID_TIMEOUT_0;                             /*!< Register,  Address offset:0x0148 */
	__I  uint32_t ERR_ADR0_TIMEOUT_0;                           /*!< Register,  Address offset:0x014C */
	__I  uint32_t ERR_ADR1_TIMEOUT_0;                           /*!< Register,  Address offset:0x0150 */
	uint32_t RSVD_NO_FUNCTION3[7];
	__I  uint32_t ERR_CODE_TIMEOUT_0;                           /*!< Register,  Address offset:0x0170 */
	uint32_t RSVD_NO_FUNCTION4[2];
	__O  uint32_t ERR_INTR_CLR_TIMEOUT_0;                       /*!< Register,  Address offset:0x017C */
	__I  uint32_t ERR_PLD0_TIMEOUT_1;                           /*!< Register,  Address offset:0x0180 */
	__I  uint32_t ERR_PLD1_TIMEOUT_1;                           /*!< Register,  Address offset:0x0184 */
	__I  uint32_t ERR_ID_TIMEOUT_1;                             /*!< Register,  Address offset:0x0188 */
	__I  uint32_t ERR_ADR0_TIMEOUT_1;                           /*!< Register,  Address offset:0x018C */
	__I  uint32_t ERR_ADR1_TIMEOUT_1;                           /*!< Register,  Address offset:0x0190 */
	uint32_t RSVD_NO_FUNCTION5[7];
	__I  uint32_t ERR_CODE_TIMEOUT_1;                           /*!< Register,  Address offset:0x01B0 */
	uint32_t RSVD_NO_FUNCTION6[2];
	__O  uint32_t ERR_INTR_CLR_TIMEOUT_1;                       /*!< Register,  Address offset:0x01BC */
	__I  uint32_t ERR_PLD0_TIMEOUT_2;                           /*!< Register,  Address offset:0x01C0 */
	__I  uint32_t ERR_PLD1_TIMEOUT_2;                           /*!< Register,  Address offset:0x01C4 */
	__I  uint32_t ERR_ID_TIMEOUT_2;                             /*!< Register,  Address offset:0x01C8 */
	__I  uint32_t ERR_ADR0_TIMEOUT_2;                           /*!< Register,  Address offset:0x01CC */
	__I  uint32_t ERR_ADR1_TIMEOUT_2;                           /*!< Register,  Address offset:0x01D0 */
	uint32_t RSVD_NO_FUNCTION7[7];
	__I  uint32_t ERR_CODE_TIMEOUT_2;                           /*!< Register,  Address offset:0x01F0 */
	uint32_t RSVD_NO_FUNCTION8[2];
	__O  uint32_t ERR_INTR_CLR_TIMEOUT_2;                       /*!< Register,  Address offset:0x01FC */
	__I  uint32_t ERR_PLD0_TIMEOUT_3;                           /*!< Register,  Address offset:0x0200 */
	__I  uint32_t ERR_PLD1_TIMEOUT_3;                           /*!< Register,  Address offset:0x0204 */
	__I  uint32_t ERR_ID_TIMEOUT_3;                             /*!< Register,  Address offset:0x0208 */
	__I  uint32_t ERR_ADR0_TIMEOUT_3;                           /*!< Register,  Address offset:0x020C */
	__I  uint32_t ERR_ADR1_TIMEOUT_3;                           /*!< Register,  Address offset:0x0210 */
	uint32_t RSVD_NO_FUNCTION9[7];
	__I  uint32_t ERR_CODE_TIMEOUT_3;                           /*!< Register,  Address offset:0x0230 */
	uint32_t RSVD_NO_FUNCTION10[2];
	__O  uint32_t ERR_INTR_CLR_TIMEOUT_3;                       /*!< Register,  Address offset:0x023C */
	__I  uint32_t ERR_PLD0_TIMEOUT_4;                           /*!< Register,  Address offset:0x0240 */
	__I  uint32_t ERR_PLD1_TIMEOUT_4;                           /*!< Register,  Address offset:0x0244 */
	__I  uint32_t ERR_ID_TIMEOUT_4;                             /*!< Register,  Address offset:0x0248 */
	__I  uint32_t ERR_ADR0_TIMEOUT_4;                           /*!< Register,  Address offset:0x024C */
	__I  uint32_t ERR_ADR1_TIMEOUT_4;                           /*!< Register,  Address offset:0x0250 */
	uint32_t RSVD_NO_FUNCTION11[7];
	__I  uint32_t ERR_CODE_TIMEOUT_4;                           /*!< Register,  Address offset:0x0270 */
	uint32_t RSVD_NO_FUNCTION12[2];
	__O  uint32_t ERR_INTR_CLR_TIMEOUT_4;                       /*!< Register,  Address offset:0x027C */
	__I  uint32_t ERR_PLD0_TIMEOUT_5;                           /*!< Register,  Address offset:0x0280 */
	__I  uint32_t ERR_PLD1_TIMEOUT_5;                           /*!< Register,  Address offset:0x0284 */
	__I  uint32_t ERR_ID_TIMEOUT_5;                             /*!< Register,  Address offset:0x0288 */
	__I  uint32_t ERR_ADR0_TIMEOUT_5;                           /*!< Register,  Address offset:0x028C */
	__I  uint32_t ERR_ADR1_TIMEOUT_5;                           /*!< Register,  Address offset:0x0290 */
	uint32_t RSVD_NO_FUNCTION13[7];
	__I  uint32_t ERR_CODE_TIMEOUT_5;                           /*!< Register,  Address offset:0x02B0 */
	uint32_t RSVD_NO_FUNCTION14[2];
	__O  uint32_t ERR_INTR_CLR_TIMEOUT_5;                       /*!< Register,  Address offset:0x02BC */
	__I  uint32_t ERR_PLD0_SEC_ADDR_HOLE;                       /*!< Register,  Address offset:0x02C0 */
	__I  uint32_t ERR_PLD1_SEC_ADDR_HOLE;                       /*!< Register,  Address offset:0x02C4 */
	__I  uint32_t ERR_ID_SEC_ADDR_HOLE;                         /*!< Register,  Address offset:0x02C8 */
	__I  uint32_t ERR_ADR0_SEC_ADDR_HOLE;                       /*!< Register,  Address offset:0x02CC */
	__I  uint32_t ERR_ADR1_SEC_ADDR_HOLE;                       /*!< Register,  Address offset:0x02D0 */
	uint32_t RSVD_NO_FUNCTION15[7];
	__I  uint32_t ERR_CODE_SEC_ADDR_HOLE;                       /*!< Register,  Address offset:0x02F0 */
	uint32_t RSVD_NO_FUNCTION16[2];
	__O  uint32_t ERR_INTR_CLR_SEC_ADDR_HOLE;                   /*!< Register,  Address offset:0x02FC */
	__IO uint32_t ACP_BASE;                                     /*!< Register,  Address offset:0x0300 */
	__IO uint32_t ACP_LIMIT;                                    /*!< Register,  Address offset:0x0304 */
	__IO uint32_t ACP_OFFSET;                                   /*!< Register,  Address offset:0x0308 */
	__IO uint32_t GPR_0;                                        /*!< Register,  Address offset:0x030C */
	__IO uint32_t GPR_1;                                        /*!< Register,  Address offset:0x0310 */
	__IO uint32_t GPR_2;                                        /*!< Register,  Address offset:0x0314 */
	__IO uint32_t GPR_3;                                        /*!< Register,  Address offset:0x0318 */
	uint32_t RSVD_NO_FUNCTION17[1];
	__I  uint32_t GPR_IN_0;                                     /*!< Register,  Address offset:0x0320 */
	__I  uint32_t GPR_IN_1;                                     /*!< Register,  Address offset:0x0324 */
	uint32_t RSVD_NO_FUNCTION18[54];
	__IO uint32_t TIMEOUT_GUARDIAN_EN_0;                        /*!< Register,  Address offset:0x0400 */
	__IO uint32_t TIMEOUT_GUARDIAN_EN_1;                        /*!< Register,  Address offset:0x0404 */
	uint32_t RSVD_NO_FUNCTION19[14];
	__IO uint32_t TIMEOUT_CLEAR_N_0;                            /*!< Register,  Address offset:0x0440 */
	__IO uint32_t TIMEOUT_CLEAR_N_1;                            /*!< Register,  Address offset:0x0444 */
	uint32_t RSVD_NO_FUNCTION20[14];
	__IO uint32_t TIMEOUT_MON_CFG_0;                            /*!< Register,  Address offset:0x0480 */
	__IO uint32_t TIMEOUT_MON_CFG_1;                            /*!< Register,  Address offset:0x0484 */
	__IO uint32_t TIMEOUT_MON_CFG_2;                            /*!< Register,  Address offset:0x0488 */
	__IO uint32_t TIMEOUT_MON_CFG_3;                            /*!< Register,  Address offset:0x048C */
	__IO uint32_t TIMEOUT_MON_CFG_4;                            /*!< Register,  Address offset:0x0490 */
	__IO uint32_t TIMEOUT_MON_CFG_5;                            /*!< Register,  Address offset:0x0494 */
	__IO uint32_t TIMEOUT_MON_CFG_6;                            /*!< Register,  Address offset:0x0498 */
	__IO uint32_t TIMEOUT_MON_CFG_7;                            /*!< Register,  Address offset:0x049C */
	__IO uint32_t TIMEOUT_MON_CFG_8;                            /*!< Register,  Address offset:0x04A0 */
	__IO uint32_t TIMEOUT_MON_CFG_9;                            /*!< Register,  Address offset:0x04A4 */
	__IO uint32_t TIMEOUT_MON_CFG_10;                           /*!< Register,  Address offset:0x04A8 */
	__IO uint32_t TIMEOUT_MON_CFG_11;                           /*!< Register,  Address offset:0x04AC */
	__IO uint32_t TIMEOUT_MON_CFG_12;                           /*!< Register,  Address offset:0x04B0 */
	uint32_t RSVD_NO_FUNCTION21[19];
	__IO uint32_t RBARA0;                                       /*!< Register,  Address offset:0x0500 */
	__IO uint32_t RLARA0;                                       /*!< Register,  Address offset:0x0504 */
	__IO uint32_t RBARA1;                                       /*!< Register,  Address offset:0x0508 */
	__IO uint32_t RLARA1;                                       /*!< Register,  Address offset:0x050C */
	__IO uint32_t RBARA2;                                       /*!< Register,  Address offset:0x0510 */
	__IO uint32_t RLARA2;                                       /*!< Register,  Address offset:0x0514 */
	__IO uint32_t RBARA3;                                       /*!< Register,  Address offset:0x0518 */
	__IO uint32_t RLARA3;                                       /*!< Register,  Address offset:0x051C */
	__IO uint32_t RBARA4;                                       /*!< Register,  Address offset:0x0520 */
	__IO uint32_t RLARA4;                                       /*!< Register,  Address offset:0x0524 */
	__IO uint32_t RBARA5;                                       /*!< Register,  Address offset:0x0528 */
	__IO uint32_t RLARA5;                                       /*!< Register,  Address offset:0x052C */
	__IO uint32_t RBARA6;                                       /*!< Register,  Address offset:0x0530 */
	__IO uint32_t RLARA6;                                       /*!< Register,  Address offset:0x0534 */
	__IO uint32_t RBARA7;                                       /*!< Register,  Address offset:0x0538 */
	__IO uint32_t RLARA7;                                       /*!< Register,  Address offset:0x053C */
	__IO uint32_t IDAU_CTRL;                                    /*!< Register,  Address offset:0x0540 */
	__IO uint32_t LOCK;                                         /*!< Register,  Address offset:0x0544 */
	uint32_t RSVD_NO_FUNCTION22[45];
	__I  uint32_t IDAU_DIR;                                     /*!< Register,  Address offset:0x05FC */
	__I  uint32_t GLOBAL_INTR_RAW_STATUS;                       /*!< Register,  Address offset:0x0600 */
	uint32_t RSVD_NO_FUNCTION23[3];
	__IO uint32_t MASK_CONTROL;                                 /*!< Register,  Address offset:0x0610 */
	uint32_t RSVD_NO_FUNCTION24[3];
	__IO uint32_t MASK_ERR_CODE_ADDR_HOLE;                      /*!< Register,  Address offset:0x0620 */
	__IO uint32_t MASK_ERR_CODE_TMO_S0_S3;                      /*!< Register,  Address offset:0x0624 */
	__IO uint32_t MASK_ERR_CODE_TMO_S1_S5_S6_S7;                /*!< Register,  Address offset:0x0628 */
	__IO uint32_t MASK_ERR_CODE_TMO_S2;                         /*!< Register,  Address offset:0x062C */
	__IO uint32_t MASK_ERR_CODE_TMO_S8_1_TO_S8_11;              /*!< Register,  Address offset:0x0630 */
	__IO uint32_t MASK_ERR_CODE_TMO_S8_12_TO_S8_15;             /*!< Register,  Address offset:0x0634 */
	__IO uint32_t MASK_ERR_CODE_TMO_SECURITY_FAULT;             /*!< Register,  Address offset:0x0638 */
	uint32_t RSVD_NO_FUNCTION25[49];
	__IO uint32_t NN_QOS_CTRL;                                  /*!< Register,  Address offset:0x0700 */
	__IO uint32_t NN_MAX_OST;                                   /*!< Register,  Address offset:0x0704 */
	__IO uint32_t NN_BURST_CTRL;                                /*!< Register,  Address offset:0x0708 */
	__I  uint32_t NN_QOS_STATUS;                                /*!< Register,  Address offset:0x070C */
	__IO uint32_t NN_AW_RATE;                                   /*!< Register,  Address offset:0x0710 */
	__IO uint32_t NN_AW_PEAK_RATE_DATA;                         /*!< Register,  Address offset:0x0714 */
	__IO uint32_t NN_AW_TFRAME_LEN;                             /*!< Register,  Address offset:0x0718 */
	__IO uint32_t NN_AW_TFRAME_P1_LEN;                          /*!< Register,  Address offset:0x071C */
	__IO uint32_t NN_AW_CONT_CTRL;                              /*!< Register,  Address offset:0x0720 */
	__IO uint32_t NN_AR_RATE;                                   /*!< Register,  Address offset:0x0724 */
	__IO uint32_t NN_AR_PEAK_RATE_DATA;                         /*!< Register,  Address offset:0x0728 */
	__IO uint32_t NN_AR_TFRAME_LEN;                             /*!< Register,  Address offset:0x072C */
	__IO uint32_t NN_AR_TFRAME_P1_LEN;                          /*!< Register,  Address offset:0x0730 */
	__IO uint32_t NN_AR_CONT_CTRL;                              /*!< Register,  Address offset:0x0734 */
	uint32_t RSVD_NO_FUNCTION26[2];
	__IO uint32_t ISP_QOS_CTRL;                                 /*!< Register,  Address offset:0x0740 */
	__IO uint32_t ISP_MAX_OST;                                  /*!< Register,  Address offset:0x0744 */
	__IO uint32_t ISP_BURST_CTRL;                               /*!< Register,  Address offset:0x0748 */
	__I  uint32_t ISP_QOS_STATUS;                               /*!< Register,  Address offset:0x074C */
	__IO uint32_t ISP_AW_RATE;                                  /*!< Register,  Address offset:0x0750 */
	__IO uint32_t ISP_AW_PEAK_RATE_DATA;                        /*!< Register,  Address offset:0x0754 */
	__IO uint32_t ISP_AW_TFRAME_LEN;                            /*!< Register,  Address offset:0x0758 */
	__IO uint32_t ISP_AW_TFRAME_P1_LEN;                         /*!< Register,  Address offset:0x075C */
	__IO uint32_t ISP_AW_CONT_CTRL;                             /*!< Register,  Address offset:0x0760 */
	__IO uint32_t ISP_AR_RATE;                                  /*!< Register,  Address offset:0x0764 */
	__IO uint32_t ISP_AR_PEAK_RATE_DATA;                        /*!< Register,  Address offset:0x0768 */
	__IO uint32_t ISP_AR_TFRAME_LEN;                            /*!< Register,  Address offset:0x076C */
	__IO uint32_t ISP_AR_TFRAME_P1_LEN;                         /*!< Register,  Address offset:0x0770 */
	__IO uint32_t ISP_AR_CONT_CTRL;                             /*!< Register,  Address offset:0x0774 */
} RXI_CORE_TypeDef;

#define RXI_CORE_BASE ((RXI_CORE_TypeDef *) (0x6010C000))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // SYSREG_RXI_CORE_H
