/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_HSSDM_H
#define SYSREG_HSSDM_H

/* AUTO_GEN_START */

/* XTAL_DIVIDER is a part of XTAL-DIVIDER */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup XTAL_DIVIDER
* @brief XTAL_DIVIDER driver modules
* @{
*/


/** @defgroup REG_SDM_FEN
 * @brief
 * @{
 */
#define XTAL_DIVIDER_BIT_SDM_EN                     ((u32)0x00000001 << 31)                  /*!< R/W   0.0 1: Enable SDM */
#define XTAL_DIVIDER_BIT_SDM_RST                    ((u32)0x00000001 << 30)                  /*!< R/W   0.0 0: RESET */
#define XTAL_DIVIDER_BIT_SDM_MOD_SEL                ((u32)0x00000001 << 29)                  /*!< R/W   0.0
                                                                                              * 0: defsult parameter
                                                                                              * 1: input parameter
                                                                                              */
#define XTAL_DIVIDER_BIT_DBG_EN_HS_SDM              ((u32)0x00000001 << 28)                  /*!< R/W   0.0 1:Enable dbg */
#define XTAL_DIVIDER_MASK_DBG_SEL_HS_SDM            ((u32)0x0000000F << 24)                  /*!< R/W   0.0
                                                                                              * 0: 20'b0,write[3:0],clk,rstn,addr[5:0]
                                                                                              * 1: data_in
                                                                                              * 2: data_out
                                                                                              * 3: 8'd0,Total_cnt[23:0]
                                                                                              * 4: 8'd0,Adjust_duty[7:0],Adjust_cnt[15:0]
                                                                                              * 5: 8'd0,Normal_duty[7:0],Normal_cnt[15:0]
                                                                                              * 6: 4'd0,CPS_init_C[3:0],4'd0,CPS_init_A,6'd0,CPS_float[9:0]
                                                                                              * 7: loop_cnt[15:0],cycle_cnt[15:0]
                                                                                              * 8: 26'd0,adjust_loop,CPS_cycle,CPS_cycle_nxt,loop_end,cycle_end,osc_total_end
                                                                                              */
#define XTAL_DIVIDER_DBG_SEL_HS_SDM(x)              ((u32)(((x) & 0x0000000F) << 24))
#define XTAL_DIVIDER_GET_DBG_SEL_HS_SDM(x)          ((u32)(((x >> 24) & 0x0000000F)))
#define XTAL_DIVIDER_MASK_SDM_RPT                   ((u32)0x00FFFFFF << 0)                   /*!< R/W   0.0 Total calculation cycle */
#define XTAL_DIVIDER_SDM_RPT(x)                     ((u32)(((x) & 0x00FFFFFF) << 0))
#define XTAL_DIVIDER_GET_SDM_RPT(x)                 ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */


/** @defgroup REG_SDM_ADJ_CTRL
 * @brief
 * @{
 */
#define XTAL_DIVIDER_MASK_SDM_ADJ_DUTY              ((u32)0x000000FF << 16)                  /*!< R/W   0.0 Set adjust cycle duty high count */
#define XTAL_DIVIDER_SDM_ADJ_DUTY(x)                ((u32)(((x) & 0x000000FF) << 16))
#define XTAL_DIVIDER_GET_SDM_ADJ_DUTY(x)            ((u32)(((x >> 16) & 0x000000FF)))
#define XTAL_DIVIDER_MASK_SDM_ADJ_CNT               ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Set adjust cycle count */
#define XTAL_DIVIDER_SDM_ADJ_CNT(x)                 ((u32)(((x) & 0x0000FFFF) << 0))
#define XTAL_DIVIDER_GET_SDM_ADJ_CNT(x)             ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_SDM_NOR_CTRL
 * @brief
 * @{
 */
#define XTAL_DIVIDER_MASK_SDM_NOR_DUTY              ((u32)0x000000FF << 16)                  /*!< R/W   0.0 Set normal cycle dury high count */
#define XTAL_DIVIDER_SDM_NOR_DUTY(x)                ((u32)(((x) & 0x000000FF) << 16))
#define XTAL_DIVIDER_GET_SDM_NOR_DUTY(x)            ((u32)(((x >> 16) & 0x000000FF)))
#define XTAL_DIVIDER_MASK_SDM_NOR_CNT               ((u32)0x0000FFFF << 0)                   /*!< R/W   0.0 Set normal cycle count */
#define XTAL_DIVIDER_SDM_NOR_CNT(x)                 ((u32)(((x) & 0x0000FFFF) << 0))
#define XTAL_DIVIDER_GET_SDM_NOR_CNT(x)             ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */


/** @defgroup REG_SDM_COM_CTRL
 * @brief
 * @{
 */
#define XTAL_DIVIDER_MASK_SDM_CPS_INT_C             ((u32)0x0000000F << 24)                  /*!< R/W   0.0 CPS int part count */
#define XTAL_DIVIDER_SDM_CPS_INT_C(x)               ((u32)(((x) & 0x0000000F) << 24))
#define XTAL_DIVIDER_GET_SDM_CPS_INT_C(x)           ((u32)(((x >> 24) & 0x0000000F)))
#define XTAL_DIVIDER_MASK_SDM_CPS_INT_A             ((u32)0x0000000F << 16)                  /*!< R/W   0.0 Set normal cycle count */
#define XTAL_DIVIDER_SDM_CPS_INT_A(x)               ((u32)(((x) & 0x0000000F) << 16))
#define XTAL_DIVIDER_GET_SDM_CPS_INT_A(x)           ((u32)(((x >> 16) & 0x0000000F)))
#define XTAL_DIVIDER_MASK_SDM_CPS_FLOAT_SET         ((u32)0x000003FF << 0)                   /*!< R/W   0.0 Set adjust cycle count */
#define XTAL_DIVIDER_SDM_CPS_FLOAT_SET(x)           ((u32)(((x) & 0x000003FF) << 0))
#define XTAL_DIVIDER_GET_SDM_CPS_FLOAT_SET(x)       ((u32)(((x >> 0) & 0x000003FF)))
/** @} */




/** @defgroup XTAL_DIVIDER_Exported_Types XTAL_DIVIDER Exported Types
* @{
*/
/**
* @brief XTAL_DIVIDER Register Declaration
*/

typedef struct {
	__IO uint32_t SDM_FEN;                                      /*!< Register,  Address offset:0x0000 */
	__IO uint32_t SDM_ADJ_CTRL;                                 /*!< Register,  Address offset:0x0001 */
	__IO uint32_t SDM_NOR_CTRL;                                 /*!< Register,  Address offset:0x0002 */
	__IO uint32_t SDM_COM_CTRL;                                 /*!< Register,  Address offset:0x0003 */
} XTAL_DIVIDER_TypeDef;


/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here
#define HSSDM_LSF_SEL     0x2

enum hssdm_reg_access_addr_e {
	Xtal_Div_Sdm_Fen            = 0x00,
	Xtal_Div_Sdm_Adj_Ctrl       = 0x01,
	Xtal_Div_Sdm_Nor_Ctrl       = 0x02,
	Xtal_Div_Sdm_Com_Ctrl       = 0x03
};


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // SYSREG_HSSDM_H
