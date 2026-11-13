/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRAFFIC_QOS_H_
#define _AMEBA_TRAFFIC_QOS_H_

/** @defgroup TRAFFIC_QOS
  * @brief TRAFFIC_QOS driver modules
  * @{
  */

/** @defgroup QOS_CTRL
 * @{
 **/
#define BIT_SW_AW_TO_CFP            ((u32)0x00000001 << 25)
#define BIT_SW_AW_TO_PEAK           ((u32)0x00000001 << 24)
#define BIT_SW_AW_TO_AVG            ((u32)0x00000001 << 23)
#define BIT_BLK_AW                  ((u32)0x00000001 << 22)
#define BIT_EN_AW_TFRAME_CTRL       ((u32)0x00000001 << 21)
#define BIT_EN_AW_CFP_BLK           ((u32)0x00000001 << 20)
#define BIT_EN_AW_HCI_CTRL          ((u32)0x00000001 << 19)
#define BIT_EN_AW_BURST_CHOP        ((u32)0x00000001 << 18)
#define BIT_EN_AW_OST_CTRL          ((u32)0x00000001 << 17)
#define BIT_EN_AW_RATE_CTRL         ((u32)0x00000001 << 16)
#define BIT_SW_AR_TO_CFP            ((u32)0x00000001 << 9)
#define BIT_SW_AR_TO_PEAK           ((u32)0x00000001 << 8)
#define BIT_SW_AR_TO_AVG            ((u32)0x00000001 << 7)
#define BIT_BLK_AR                  ((u32)0x00000001 << 6)
#define BIT_EN_AR_TFRAME_CTRL       ((u32)0x00000001 << 5)
#define BIT_EN_AR_CFP_BLK           ((u32)0x00000001 << 4)
#define BIT_EN_AR_HCI_CTRL          ((u32)0x00000001 << 3)
#define BIT_EN_AR_BURST_CHOP        ((u32)0x00000001 << 2)
#define BIT_EN_AR_OST_CTRL          ((u32)0x00000001 << 1)
#define BIT_EN_AR_RATE_CTRL         ((u32)0x00000001 << 0)
/** @} */


/** @defgroup MAX_OST
 * @{
 **/
#define MASK_MAX_AW_OST              ((u32)0x000000FF << 8)
#define MAX_AW_OST(x)                ((u32)(((x) & 0x000000FF) << 8))
#define MASK_MAX_AR_OST              ((u32)0x000000FF << 0)
#define MAX_AR_OST(x)                ((u32)(((x) & 0x000000FF) << 0))
/** @} */


/** @defgroup BURST_CTRL
 * @{
 **/
#define MASK_DRAM_ALIGN_POW2         ((u32)0x00000007 << 12)
#define MASK_BURST_CHOP_POW2         ((u32)0x00000007 << 8)
#define BIT_EN_DRAM_ALIGN            ((u32)0x00000001 << 0)
/** @} */


/** @defgroup QOS_STATUS
 * @{
 **/
#define BIT_AW_IN_CFP                ((u32)0x00000001 << 31)
#define BIT_AW_IN_PEAK               ((u32)0x00000001 << 30)
#define BIT_AW_IN_AVG                ((u32)0x00000001 << 29)
#define MASK_AW_DS_STALL             ((u32)0x00000003 << 26)
#define BIT_AW_BOUND_BY_RATE         ((u32)0x00000001 << 25)
#define BIT_AW_BOUND_BY_OSR          ((u32)0x00000001 << 24)
#define BIT_AW_CMD_FULL              ((u32)0x00000001 << 23)
#define BIT_AW_CMD_LE_HALF           ((u32)0x00000001 << 22)
#define BIT_WD_DATA_FIFO_FULL        ((u32)0x00000001 << 21)
#define BIT_AW_ACTIVE                ((u32)0x00000001 << 20)
#define MASK_AW_QOS_STATE            ((u32)0x0000000F << 16)

#define BIT_AR_IN_CFP                ((u32)0x00000001 << 14)
#define BIT_AR_IN_PEAK               ((u32)0x00000001 << 13)
#define BIT_AR_IN_AVG                ((u32)0x00000001 << 12)
#define MASK_AR_DS_STALL             ((u32)0x00000003 << 10)
#define BIT_AR_BOUND_BY_RATE         ((u32)0x00000001 << 9)
#define BIT_AR_BOUND_BY_OSR          ((u32)0x00000001 << 8)
#define BIT_AR_CMD_FULL              ((u32)0x00000001 << 7)
#define BIT_AR_CMD_LE_HALF           ((u32)0x00000001 << 6)
#define BIT_RD_DATA_FIFO_FULL        ((u32)0x00000001 << 5)
#define BIT_AR_ACTIVE                ((u32)0x00000001 << 4)
#define MASK_AR_QOS_STATE            ((u32)0x0000000F << 0)
/** @} */

/** @defgroup AW_RATE
 * @{
 **/
#define MASK_AW_PEAK_RATE_N          ((u32)0x000000FF << 24)
#define AW_PEAK_RATE_N(x)            ((u32)(((x) & 0x000000FF) << 24))
#define MASK_AW_PEAK_RATE_D          ((u32)0x000000FF << 16)
#define AW_PEAK_RATE_D(x)            ((u32)(((x) & 0x000000FF) << 16))
#define MASK_AW_AVG_RATE_N           ((u32)0x000000FF << 8)
#define AW_AVG_RATE_N(x)             ((u32)(((x) & 0x000000FF) << 8))
#define MASK_AW_AVG_RATE_D           ((u32)0x000000FF << 0)
#define AW_AVG_RATE_D(x)             ((u32)(((x) & 0x000000FF) << 0))
/** @} */


/** @defgroup AW_CONT_CTRL
 * @{
 **/
#define MASK_AW_MAX_SUCC           ((u32)0x0000FFFF << 16)
#define AW_MAX_SUCC(x)             ((u32)(((x) & 0x0000FFFF) << 16))
#define MASK_AW_MIN_SUCC           ((u32)0x0000FFFF << 0)
#define AW_MIN_SUCC(x)             ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */


/** @defgroup AR_RATE
 * @{
 **/
#define MASK_AR_PEAK_RATE_N          ((u32)0x000000FF << 24)
#define AR_PEAK_RATE_N(x)            ((u32)(((x) & 0x000000FF) << 24))
#define MASK_AR_PEAK_RATE_D          ((u32)0x000000FF << 16)
#define AR_PEAK_RATE_D(x)            ((u32)(((x) & 0x000000FF) << 16))
#define MASK_AR_AVG_RATE_N           ((u32)0x000000FF << 8)
#define AR_AVG_RATE_N(x)             ((u32)(((x) & 0x000000FF) << 8))
#define MASK_AR_AVG_RATE_D           ((u32)0x000000FF << 0)
#define AR_AVG_RATE_D(x)             ((u32)(((x) & 0x000000FF) << 0))
/** @} */

/** @defgroup AR_CONT_CTRL
 * @{
 **/
#define MASK_AR_MAX_SUCC           ((u32)0x0000FFFF << 16)
#define AR_MAX_SUCC(x)             ((u32)(((x) & 0x0000FFFF) << 16))
#define MASK_AR_MIN_SUCC           ((u32)0x0000FFFF << 0)
#define AR_MIN_SUCC(x)             ((u32)(((x) & 0x0000FFFF) << 0))
/** @} */


/**************************************************************************//**
 * @defgroup AMEBAPRO3_TRAFFIC_QOS
 * @{
 * @brief AMEBAPRO3_TRAFFIC_QOS Register Declaration
 *****************************************************************************/

typedef struct {
	__IO uint32_t QOS_CTRL;                            /*!<  Register,  Address offset: 0x00 */
	__IO uint32_t MAX_OST;                             /*!<  Register,  Address offset: 0x04 */
	__IO uint32_t BURST_CTRL;                          /*!<  Register,  Address offset: 0x08 */
	__I  uint32_t QOS_STATUS;                          /*!<  Register,  Address offset: 0x0c */
	__IO uint32_t AW_RATE;                             /*!<  Register,  Address offset: 0x10 */
	__IO uint32_t AW_PEAK_RATE_DATA;                   /*!<  Register,  Address offset: 0x14 */
	__IO uint32_t AW_TFRAME_LEN;                       /*!<  Register,  Address offset: 0x18 */
	__IO uint32_t AW_TFRAME_P1_LEN;                    /*!<  Register,  Address offset: 0x1c */
	__IO uint32_t AW_CONT_CTRL;                        /*!<  Register,  Address offset: 0x20 */
	__IO uint32_t AR_RATE;                             /*!<  Register,  Address offset: 0x24 */
	__IO uint32_t AR_PEAK_RATE_DATA;                   /*!<  Register,  Address offset: 0x28 */
	__IO uint32_t AR_TFRAME_LEN;                       /*!<  Register,  Address offset: 0x2c */
	__IO uint32_t AR_TFRAME_P1_LEN;                    /*!<  Register,  Address offset: 0x30 */
	__IO uint32_t AR_CONT_CTRL;                        /*!<  Register,  Address offset: 0x34 */
} TRAFFIC_QOS_TypeDef;

/** @defgroup QOS_Exported_Functions DDRC Exported Functions
  * @{
  */
_LONG_CALL_ void QOS_SetAWOutstanding(TRAFFIC_QOS_TypeDef *qos, int outstanding);
_LONG_CALL_ void QOS_SetAROutstanding(TRAFFIC_QOS_TypeDef *qos, int outstanding);
_LONG_CALL_ void QOS_EnableAWOutstandingRegulation(TRAFFIC_QOS_TypeDef *qos, int enable);
_LONG_CALL_ void QOS_EnableAROutstandingRegulation(TRAFFIC_QOS_TypeDef *qos, int enable);
_LONG_CALL_ void QOS_SetAWPeakRateData(TRAFFIC_QOS_TypeDef *qos, int peak_rate_data);
_LONG_CALL_ void QOS_SetARPeakRateData(TRAFFIC_QOS_TypeDef *qos, int peak_rate_data);
_LONG_CALL_ void QOS_SetAWTFrameLen(TRAFFIC_QOS_TypeDef *qos, int total_len, int p1_len);
_LONG_CALL_ void QOS_SetARTFrameLen(TRAFFIC_QOS_TypeDef *qos, int total_len, int p1_len);
_LONG_CALL_ void QOS_SetAWSucc(TRAFFIC_QOS_TypeDef *qos, int max_succ, int min_succ);
_LONG_CALL_ void QOS_SetARSucc(TRAFFIC_QOS_TypeDef *qos, int max_succ, int min_succ);
_LONG_CALL_ void QOS_SetAWBandwidthRate(TRAFFIC_QOS_TypeDef *qos, int peak_rate_n, int peak_rate_d, int avg_rate_n, int avg_rate_d);
_LONG_CALL_ void QOS_SetARBandwidthRate(TRAFFIC_QOS_TypeDef *qos, int peak_rate_n, int peak_rate_d, int avg_rate_n, int avg_rate_d);
_LONG_CALL_ void QOS_EnableAWBandwidthRegulation(TRAFFIC_QOS_TypeDef *qos, int enable);
_LONG_CALL_ void QOS_EnableARBandwidthRegulation(TRAFFIC_QOS_TypeDef *qos, int enable);
#endif
