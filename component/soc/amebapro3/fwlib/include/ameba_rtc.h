/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RTC_H_
#define _AMEBA_RTC_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
* @{
*/

/** @defgroup RTC
* @brief RTC driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup RTC_Register_Definitions RTC Register Definitions
 * @{
*/
/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_FEN
 * @brief
 * @{
 *****************************************************************************/
#define RTC_BIT_CKDIV_MOD_SEL                            ((u32)0x00000001 << 4)                              /*!<R/W 0  0: Normal mode 1024 for 1s; 1: test mode 64 for 1s */
#define RTC_BIT_CKDIV_EN                                 ((u32)0x00000001 << 1)                              /*!<R/W 0  1: Enable RTC CLK divider */
#define RTC_BIT_RTC_EN                                   ((u32)0x00000001 << 0)                              /*!<R/W 0  1: Enable RTC  */
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_COMP
 * @brief
 * @{
 *****************************************************************************/
#define RTC_BIT_FUNCTION_EN                     ((u32)0x00000001 << 31)                             /*!<R/W 0   */
#define RTC_BIT_ADJUST_DIRECTION                ((u32)0x00000001 << 18)                             /*!<R/W 0  0: plus, 1 mimus */
#define RTC_MASK_RTC_ADJUST_PERIOD_SEL          ((u32)0x0003FFFF << 0)                              /*!<R/W 0x00000  unit: s */
#define RTC_RTC_ADJUST_PERIOD_SEL(x)            (((u32)((x) & 0x0003FFFF) << 0))
#define RTC_GET_RTC_ADJUST_PERIOD_SEL(x)        ((u32)(((x >> 0) & 0x0003FFFF)))
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_IMR
 * @brief
 * @{
 *****************************************************************************/
#define RTC_BIT_ALRM_IMR                             ((u32)0x00000001 << 16)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_DOY_IMR                         ((u32)0x00000001 << 15)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_YRS_IMR                         ((u32)0x00000001 << 14)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_MTH_IMR                         ((u32)0x00000001 << 13)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_DOM_IMR                         ((u32)0x00000001 << 12)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_DOW_IMR                         ((u32)0x00000001 << 11)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_HRS_IMR                         ((u32)0x00000001 << 10)                             /*!<R/W 0   */
#define RTC_BIT_ALRM_MIN_IMR                         ((u32)0x00000001 << 9)                              /*!<R/W 0   */
#define RTC_BIT_ALRM_SEC_IMR                         ((u32)0x00000001 << 8)                              /*!<R/W 0   */
#define RTC_BIT_CNT_DOY_IMR                          ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define RTC_BIT_CNT_YRS_IMR                          ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define RTC_BIT_CNT_MTH_IMR                          ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define RTC_BIT_CNT_DOM_IMR                          ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define RTC_BIT_CNT_DOW_IMR                          ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define RTC_BIT_CNT_HRS_IMR                          ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define RTC_BIT_CNT_MIN_IMR                          ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define RTC_BIT_CNT_SEC_IMR                          ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_ISR
 * @brief
 * @{
 *****************************************************************************/
#define RTC_BIT_ALRM_ISR                             ((u32)0x00000001 << 16)                             /*!<R/W 0  Indicate ALL timer value match ALRM value */
#define RTC_BIT_ALRM_DOY_ISR                         ((u32)0x00000001 << 15)                             /*!<R/W 0  Indicate DOY timer value match ALRM_DOY */
#define RTC_BIT_ALRM_YRS_ISR                         ((u32)0x00000001 << 14)                             /*!<R/W 0  Indicate Year timer value match ALRM_YRS */
#define RTC_BIT_ALRM_MTH_ISR                         ((u32)0x00000001 << 13)                             /*!<R/W 0  Indicate MTH timer value match ALRM_MTH */
#define RTC_BIT_ALRM_DOM_ISR                         ((u32)0x00000001 << 12)                             /*!<R/W 0  Indicate DOM timer value match ALRM_DOM */
#define RTC_BIT_ALRM_DOW_ISR                         ((u32)0x00000001 << 11)                             /*!<R/W 0  Indicate DOW timer value match ALRM_DOW */
#define RTC_BIT_ALRM_HRS_ISR                         ((u32)0x00000001 << 10)                             /*!<R/W 0  Indicate Hour timer value match ALRM_HRS */
#define RTC_BIT_ALRM_MIN_ISR                         ((u32)0x00000001 << 9)                              /*!<R/W 0  Indicate Minute timer value match ALRM_MIN */
#define RTC_BIT_ALRM_SEC_ISR                         ((u32)0x00000001 << 8)                              /*!<R/W 0  Indicate Second timer value match ALRM_SEC */
#define RTC_BIT_CNT_DOY_ISR                          ((u32)0x00000001 << 7)                              /*!<R/W 0  Indicate increment of the Day-of-year timer */
#define RTC_BIT_CNT_YRS_ISR                          ((u32)0x00000001 << 6)                              /*!<R/W 0  Indicate increment of the year timer */
#define RTC_BIT_CNT_MTH_ISR                          ((u32)0x00000001 << 5)                              /*!<R/W 0  Indicate increment of the month timer */
#define RTC_BIT_CNT_DOM_ISR                          ((u32)0x00000001 << 4)                              /*!<R/W 0  Indicate increment of the day-of-month timer */
#define RTC_BIT_CNT_DOW_ISR                          ((u32)0x00000001 << 3)                              /*!<R/W 0  Indicate increment of the day-of-week timer */
#define RTC_BIT_CNT_HRS_ISR                          ((u32)0x00000001 << 2)                              /*!<R/W 0  Indicate increment of the hour timer */
#define RTC_BIT_CNT_MIN_ISR                          ((u32)0x00000001 << 1)                              /*!<R/W 0  Indicate increment of the minute timer */
#define RTC_BIT_CNT_SEC_ISR                          ((u32)0x00000001 << 0)                              /*!<R/W 0  Indicate increment of the second timer */
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_TIM0
 * @brief
 * @{
 *****************************************************************************/
#define RTC_MASK_RTC_DOW                              ((u32)0x00000007 << 24)                             /*!<R/W 000  Day of week value in the range of 0 to 6 */
#define RTC_RTC_DOW(x)                                (((u32)((x) & 0x00000007) << 24))
#define RTC_GET_RTC_DOW(x)                            ((u32)(((x >> 24) & 0x00000007)))
#define RTC_MASK_RTC_HRS                              ((u32)0x0000001F << 16)                             /*!<R/W 00000  Hours value in the range of 0 to 23 */
#define RTC_RTC_HRS(x)                                (((u32)((x) & 0x0000001F) << 16))
#define RTC_GET_RTC_HRS(x)                            ((u32)(((x >> 16) & 0x0000001F)))
#define RTC_MASK_RTC_MIN                              ((u32)0x0000003F << 8)                              /*!<R/W 000000  Minutes value in the range of 0 to 59 */
#define RTC_RTC_MIN(x)                                (((u32)((x) & 0x0000003F) << 8))
#define RTC_GET_RTC_MIN(x)                            ((u32)(((x >> 8) & 0x0000003F)))
#define RTC_MASK_RTC_SEC                              ((u32)0x0000003F << 0)                              /*!<R/W 000000  Seconds value in the range of 0 to 59 */
#define RTC_RTC_SEC(x)                                (((u32)((x) & 0x0000003F) << 0))
#define RTC_GET_RTC_SEC(x)                            ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_TIM1
 * @brief
 * @{
 *****************************************************************************/
#define RTC_MASK_RTC_YEAR                             ((u32)0x00000FFF << 16)                             /*!<R/W 0x000  Year value in the range of 0 to 4095. */
#define RTC_RTC_YEAR(x)                               (((u32)((x) & 0x00000FFF) << 16))
#define RTC_GET_RTC_YEAR(x)                           ((u32)(((x >> 16) & 0x00000FFF)))
#define RTC_MASK_RTC_MON                              ((u32)0x0000000F << 8)                              /*!<R/W 1  Month value in the range of 1 to 12 */
#define RTC_RTC_MON(x)                                (((u32)((x) & 0x0000000F) << 8))
#define RTC_GET_RTC_MON(x)                            ((u32)(((x >> 8) & 0x0000000F)))
#define RTC_MASK_RTC_DOM                              ((u32)0x0000001F << 0)                              /*!<R/W 1  Day of month value in the range of 1 to 28, 29, 30, or 31 */
#define RTC_RTC_DOM(x)                                (((u32)((x) & 0x0000001F) << 0))
#define RTC_GET_RTC_DOM(x)                            ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_TIM2
 * @brief
 * @{
 *****************************************************************************/
#define RTC_BIT_RTC_M4ISCYR                              ((u32)0x00000001 << 16)                             /*!<R/W 0  Year is quadruple of four, but it is common year */
#define RTC_MASK_RTC_DOY                                 ((u32)0x000001FF << 0)                              /*!<R/W 1  Day of year, value in the range of 1 to 365 */
#define RTC_RTC_DOY(x)                                   (((u32)((x) & 0x000001FF) << 0))
#define RTC_GET_RTC_DOY(x)                               ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_ALRM_CTRL0
 * @brief
 * @{
 *****************************************************************************/
#define RTC_MASK_RTC_ALRM_DOW                             ((u32)0x00000007 << 24)                             /*!<R/W 0  Day of week value in the range of 0 to 6 */
#define RTC_RTC_ALRM_DOW(x)                               (((u32)((x) & 0x00000007) << 24))
#define RTC_GET_RTC_ALRM_DOW(x)                           ((u32)(((x >> 24) & 0x00000007)))
#define RTC_MASK_RTC_ALRM_HRS                             ((u32)0x0000001F << 16)                             /*!<R/W 0  Hours value in the range of 0 to 23 */
#define RTC_RTC_ALRM_HRS(x)                               (((u32)((x) & 0x0000001F) << 16))
#define RTC_GET_RTC_ALRM_HRS(x)                           ((u32)(((x >> 16) & 0x0000001F)))
#define RTC_MASK_RTC_ALRM_MIN                             ((u32)0x0000003F << 8)                              /*!<R/W 0  Minutes value in the range of 0 to 59 */
#define RTC_RTC_ALRM_MIN(x)                               (((u32)((x) & 0x0000003F) << 8))
#define RTC_GET_RTC_ALRM_MIN(x)                           ((u32)(((x >> 8) & 0x0000003F)))
#define RTC_MASK_RTC_ALRM_SEC                             ((u32)0x0000003F << 0)                              /*!<R/W 0  Seconds value in the range of 0 to 59 */
#define RTC_RTC_ALRM_SEC(x)                               (((u32)((x) & 0x0000003F) << 0))
#define RTC_GET_RTC_ALRM_SEC(x)                           ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_ALRM_CTRL1
 * @brief
 * @{
 *****************************************************************************/
#define RTC_MASK_RTC_ALRM_YEAR                          ((u32)0x00000FFF << 16)                             /*!<R/W 0  Year value in the range of 0 to 4095. */
#define RTC_RTC_ALRM_YEAR(x)                            (((u32)((x) & 0x00000FFF) << 16))
#define RTC_GET_RTC_ALRM_YEAR(x)                        ((u32)(((x >> 16) & 0x00000FFF)))
#define RTC_MASK_RTC_ALRM_MON                           ((u32)0x0000000F << 8)                              /*!<R/W 0  Month value in the range of 1 to 12 */
#define RTC_RTC_ALRM_MON(x)                             (((u32)((x) & 0x0000000F) << 8))
#define RTC_GET_RTC_ALRM_MON(x)                         ((u32)(((x >> 8) & 0x0000000F)))
#define RTC_MASK_RTC_ALRM_DOM                           ((u32)0x0000001F << 0)                              /*!<R/W 0  Day of month value in the range of 1 to 28, 29, 30, or 31 */
#define RTC_RTC_ALRM_DOM(x)                             (((u32)((x) & 0x0000001F) << 0))
#define RTC_GET_RTC_ALRM_DOM(x)                         ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/**************************************************************************//**
 * @defgroup RTC_TYPE_REG_RTC_ALRM_CTRL2
 * @brief
 * @{
 *****************************************************************************/
#define RTC_BIT_RTC_ALRM_DOY_EN                          ((u32)0x00000001 << 23)                             /*!<R/W 0  Enable DOY timer value match ALRM_DOY */
#define RTC_BIT_RTC_ALRM_YRS_EN                          ((u32)0x00000001 << 22)                             /*!<R/W 0  Enable Year timer value match ALRM_YRS */
#define RTC_BIT_RTC_ALRM_MTH_EN                          ((u32)0x00000001 << 21)                             /*!<R/W 0  Enable MTH timer value match ALRM_MTH */
#define RTC_BIT_RTC_ALRM_DOM_EN                          ((u32)0x00000001 << 20)                             /*!<R/W 0  Enable DOM timer value match ALRM_DOM */
#define RTC_BIT_RTC_ALRM_DOW_EN                          ((u32)0x00000001 << 19)                             /*!<R/W 0  Enable DOW timer value match ALRM_DOW */
#define RTC_BIT_RTC_ALRM_HRS_EN                          ((u32)0x00000001 << 18)                             /*!<R/W 0  Enable Hour timer value match ALRM_HRS */
#define RTC_BIT_RTC_ALRM_MIN_EN                          ((u32)0x00000001 << 17)                             /*!<R/W 0  Enable Minute timer value match ALRM_MIN */
#define RTC_BIT_RTC_ALRM_SEC_EN                          ((u32)0x00000001 << 16)                             /*!<R/W 0  Enable Second timer value match ALRM_SEC */
#define RTC_MASK_RTC_ALRM_DOY                            ((u32)0x000001FF << 0)                              /*!<R/W 0  Day of year, value in the range of 1 to 365 */
#define RTC_RTC_ALRM_DOY(x)                              (((u32)((x) & 0x000001FF) << 0))
#define RTC_GET_RTC_ALRM_DOY(x)                          ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/**************************************************************************//**
 * @defgroup rtl8735b_RTC_TYPE
 * @{
 * @brief rtl8735b_RTC_TYPE Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t RTC_REG_RTC_FEN ;                   /*!<   register,  Address offset: 0x000 */
	__O  uint32_t RTC_REG_DUMMY1 ;                    /*!<   register,  Address offset: 0x004 */
	__IO uint32_t RTC_REG_RTC_COMP ;                  /*!<   register,  Address offset: 0x008 */
	__IO uint32_t RTC_REG_DUMMY2 ;                    /*!<   register,  Address offset: 0x00C */
	__IO uint32_t RTC_REG_RTC_IMR ;                   /*!<   register,  Address offset: 0x010 */
	__IO uint32_t RTC_REG_RTC_ISR ;                   /*!<   register,  Address offset: 0x014 */
	__IO uint32_t RTC_REG_DUMMY3 ;                    /*!<   register,  Address offset: 0x018 */
	__IO uint32_t RTC_REG_DUMMY4 ;                    /*!<   register,  Address offset: 0x01C */
	__IO uint32_t RTC_REG_RTC_TIM0 ;                  /*!<   register,  Address offset: 0x020 */
	__IO uint32_t RTC_REG_RTC_TIM1 ;                  /*!<   register,  Address offset: 0x024 */
	__IO uint32_t RTC_REG_RTC_TIM2 ;                  /*!<   register,  Address offset: 0x028 */
	__IO uint32_t RTC_REG_DUMMY5 ;                    /*!<   register,  Address offset: 0x02C */
	__IO uint32_t RTC_REG_RTC_ALRM_CTRL0 ;            /*!<   register,  Address offset: 0x030 */
	__IO uint32_t RTC_REG_RTC_ALRM_CTRL1 ;            /*!<   register,  Address offset: 0x034 */
	__IO uint32_t RTC_REG_RTC_ALRM_CTRL2 ;            /*!<   register,  Address offset: 0x038 */
} RTC_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @addtogroup RTC_Exported_Types RTC Exported Types
  * @{
  */

/**
  * @brief  RTC Init Structure Definition
  */
typedef struct {
	u32 RTC_HourFormat;	/*!< Specifies the RTC Hour Format.
					This parameter can be a value of @ref RTC_Hour_Format */

	u32 RTC_AsynchPrediv;	/*!< Specifies the RTC Asynchronous Predivider value.
								This parameter must be a value of @ref RTC_Asynchronous_Predivider */

	u32 RTC_SynchPrediv;	/*!< Specifies the RTC Synchronous Predivider value.
								This parameter must be a value of @ref RTC_Synchronous_Predivider */

	u32 RTC_DayThreshold;	/*!< Specifies the RTC Day Threshold value.
								This parameter must be a value of @ref RTC_Day_Threshold */
} RTC_InitTypeDef;

/**
  * @brief  RTC Time Structure Definition
  */
typedef struct {
	u16 RTC_Year;	/*!< Year in binary format 16bits 1900~2155 */

	u8 RTC_Hours;	/*!< Specifies the RTC Time Hour.
						This parameter must be set to a value in the 1-12 range
						if the RTC_HourFormat_12 is selected or 0-23 range if
						the RTC_HourFormat_24 is selected. */

	u8 RTC_Minutes;	/*!< Specifies the RTC Time Minutes.
						This parameter must be set to a value in the 0-59 range. */

	u8 RTC_Seconds;	/*!< Specifies the RTC Time Seconds.
						This parameter must be set to a value in the 0-59 range. */

	u8 RTC_H12_PMAM;	/*!< Specifies the RTC AM/PM Time. [not used in pro3]
						This parameter can be a value of @ref RTC_AM_PM */

	// added
	u8 RTC_DayOfWeek;   // [0,6]
	u8 RTC_DayOfMonth;  // [1,31]
	u16 RTC_Days;   	// [1,366]
	u8 RTC_Month;       // [1,12]
	u8 RTC_IsLeapYear;

} RTC_TimeTypeDef;

/**
  * @brief  RTC Alarm Structure Definition
  */
typedef struct {
	RTC_TimeTypeDef RTC_AlarmTime;	/*!< Specifies the RTC Alarm Time members. */

	u32 RTC_AlarmMask;	/*!< Specifies the RTC Alarm1 Masks(H:M:S).
					This parameter can be a value of @ref RTC_AlarmMask1 */
	u32 RTC_Alarm2Mask;	/*!< Specifies the RTC Alarm2 Masks Day).
					This parameter can be a value of @ref RTC_AlarmMask2 */
} RTC_AlarmTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup RTC_Exported_Constants RTC Exported Constants
  * @{
  */

/** @defgroup RTC_Input_Parameter_Format
  * @{
  */
#define RTC_Format_BIN			((u32)0x000000000)
#define RTC_Format_BCD			((u32)0x000000001)
#define IS_RTC_FORMAT(FORMAT)	(((FORMAT) == RTC_Format_BIN) || ((FORMAT) == RTC_Format_BCD))
/**
  * @}
  */

/** @defgroup RTC_Hour_Format
  * @{
  */
#define RTC_HourFormat_24					((u32)0x00000000)
#define RTC_HourFormat_12					((u32)0x00000080)
#define IS_RTC_HOUR_FORMAT(FORMAT)			(((FORMAT) == RTC_HourFormat_12) || \
											((FORMAT) == RTC_HourFormat_24))
#define IS_RTC_TIME_DAY_THRES(TIME_DAYS)	((TIME_DAYS) <= 0x1FF)

/**
  * @}
  */

/** @defgroup RTC_Asynchronous_Predivider
  * @{
  */
#define IS_RTC_ASYNCH_PREDIV(PREDIV)	((PREDIV) <= 0x1FF)
/**
  * @}
  */

/** @defgroup RTC_Synchronous_Predivider
  * @{
  */
#define IS_RTC_SYNCH_PREDIV(PREDIV)		((PREDIV) <= 0x1FF)
/**
  * @}
  */

/** @defgroup RTC_Day_Threshold
  * @{
  */
#define RTC_DAYTHRES_MSK			((u32)0xFF800000)
#define IS_RTC_DAY_THRES(DAYS)		((DAYS) <= 0x1FF)
/**
  * @}
  */

/** @defgroup RTC_Base_Year
  * @{
  */
#define RTC_BASE_YEAR		((u16)1900)
/**
  * @}
  */

/** @defgroup RTC_Year_Threshold
  * @{
  */
#define IS_RTC_YEAR_THRES(YEAR)		((YEAR >= RTC_BASE_YEAR) && \
									((YEAR - RTC_BASE_YEAR) <= 0xFF))
/**
  * @}
  */

/** @defgroup RTC_Time_Definitions
  * @{
  */
#define IS_RTC_HOUR12(HOUR)		(((HOUR) > 0) && ((HOUR) <= 12))
#define IS_RTC_HOUR24(HOUR)		((HOUR) <= 23)
#define IS_RTC_MINUTES(MINUTES)	((MINUTES) <= 59)
#define IS_RTC_SECONDS(SECONDS)	((SECONDS) <= 59)

#define IS_RTC_YEAR(YEAR)		(((YEAR) >= RTC_BASE_YEAR) && ((YEAR - RTC_BASE_YEAR) <= 0xFFF))
#define IS_RTC_MONTH(MONTH)		(((MONTH) >= 1) && ((MONTH) <= 12))
#define IS_RTC_DOY(DOY)			(((DOY) >= 1) && ((DOY) <= 366))
#define IS_RTC_DOM(DOM)			(((DOM) >= 1) && ((DOM) <= 31))
#define IS_RTC_DOW(DOW)			(((DOW) <= 6))
/**
  * @}
  */

/** @defgroup RTC_AM_PM
  * @{
  */
#define RTC_H12_AM				((u8)0x00) //AM or 24-hour format
#define RTC_H12_PM				((u8)0x01) //PM
#define IS_RTC_H12_AMPM(PM)		(((PM) == RTC_H12_AM))  // pro3 only support 24-hour format
/**
  * @}
  */

/** @defgroup RTC_AlarmMask1
  * @{
  */
#define RTC_AlarmMask_None		((u32)0x00000000)
#define RTC_AlarmMask_Hours		((u32)0x00800000)
#define RTC_AlarmMask_Minutes	((u32)0x00008000)
#define RTC_AlarmMask_Seconds	((u32)0x00000080)
#define RTC_AlarmMask_All		((u32)0x00808080)
#define IS_ALARM_MASK(MASK)		(((MASK) & 0x7F7F7F) == 0)
/**
  * @}
  */

/** @defgroup RTC_AlarmMask2
  * @{
  */
#define RTC_Alarm2Mask_None		((u32)0x00000000)
#define RTC_Alarm2Mask_Days		((u32)0x00000200)
#define IS_ALARM2_MASK(MASK)	(((MASK) & ~RTC_Alarm2Mask_Days) == 0)
/**
  * @}
  */

/** @defgroup RTC_DayLightSaving_Control
  * @{
  */
#define RTC_DayLightSaving_SUB1H		((u32)0x00000002)
#define RTC_DayLightSaving_ADD1H		((u32)0x00000001)
#define IS_RTC_DAYLIGHT_SAVING(SAVE)	(((SAVE) == RTC_DayLightSaving_SUB1H) || \
										((SAVE) == RTC_DayLightSaving_ADD1H))

#define RTC_StoreOperation_Reset			((u32)0x00000000)
#define RTC_StoreOperation_Set				((u32)0x00000004)
#define IS_RTC_STORE_OPERATION(OPERATION)	(((OPERATION) == RTC_StoreOperation_Reset) || \
											((OPERATION) == RTC_StoreOperation_Set))
/**
  * @}
  */

/** @defgroup RTC_Output_Control
  * @{
  */
#define RTC_Output_Disable		((u32)0x00000000)
#define RTC_Output_Alarm		((u32)0x00000020) ////wakeup
#define RTC_Output_clkspre		((u32)0x00000040) ////1Hz
#define RTC_Output_clkapre		((u32)0x00000060) ////256Hz

#define IS_RTC_OUTPUT(OUTPUT)	(((OUTPUT) == RTC_Output_Disable) || \
								((OUTPUT) == RTC_Output_Alarm) || \
								((OUTPUT) == RTC_Output_clkspre) || \
								((OUTPUT) == RTC_Output_clkapre))
/**
  * @}
  */

/** @defgroup RTC_Smooth_Calibration_Control
  * @{
  */
#define RTC_CalibPeriod_1MIN		((u32)0x00000000)
#define RTC_CalibPeriod_2MIN		((u32)0x00010000)
#define RTC_CalibPeriod_3MIN		((u32)0x00020000)
#define RTC_CalibPeriod_4MIN		((u32)0x00030000)
#define RTC_CalibPeriod_5MIN		((u32)0x00040000)
#define RTC_CalibPeriod_6MIN		((u32)0x00050000)
#define RTC_CalibPeriod_7MIN		((u32)0x00060000)
#define RTC_CalibPeriod_8MIN		((u32)0x00070000)

#define IS_RTC_CALIB_PERIOD(DCP)	(((DCP) == RTC_CalibPeriod_1MIN) || \
									((DCP) == RTC_CalibPeriod_2MIN) || \
									((DCP) == RTC_CalibPeriod_3MIN) || \
									((DCP) == RTC_CalibPeriod_4MIN) || \
									((DCP) == RTC_CalibPeriod_5MIN) || \
									((DCP) == RTC_CalibPeriod_6MIN) || \
									((DCP) == RTC_CalibPeriod_7MIN) || \
									((DCP) == RTC_CalibPeriod_8MIN))

#define RTC_Calib_Disable			((u32)0x00000000)
#define RTC_Calib_Enable			((u32)0x00008000)

#define IS_RTC_CALIB_ENABLE(DCE)	(((DCE) == RTC_Calib_Disable) || \
									((DCE) == RTC_Calib_Enable))

#define RTC_CalibSign_Positive		((u32)0x00000000)
#define RTC_CalibSign_Negative		((u32)0x00004000)

#define IS_RTC_CALIB_SIGN(SIGN)		(((SIGN) == RTC_CalibSign_Positive) || \
									((SIGN) == RTC_CalibSign_Negative))

#define IS_RTC_CALIB_VALUE(VALUE)	((VALUE) <= 0x7F)
/**
  * @}
  */

/** @defgroup RTC_TR_Mask
  * @{
  */
#define RTC_TR_RESERVED_MASK	((u32)0xFFFF7F7F)
/**
  * @}
  */

/** @defgroup RTC_Timeout_Control
  * @{
  */
#define INITMODE_TIMEOUT	((u32) 0x00010000)
#define SYNCHRO_TIMEOUT		((u32) 0x00020000)
#define RECALPF_TIMEOUT		((u32) 0x00020000)
#define ALARMDIS_TIMEOUT	((u32) 0x00020000)
#define WUTDIS_TIMEOUT		((u32) 0x00020000)
/**
  * @}
  */

/** @defgroup RTC_Shift_Control
  * @{
  */
#define RTC_SHIFT_ALR_HU	16
#define RTC_SHIFT_ALR_MNU	8
#define RTC_SHIFT_ALR_PM	22
#define RTC_SHIFT_HU		16
#define RTC_SHIFT_MNU		8
#define RTC_SHIFT_PM		22
/**
  * @}
  */

/** @defgroup Leap year check
  * @{
  */
#define IS_LEAP_YEAR_CHECK(YEAR, RET) do {\
		if(((!(YEAR % 4)) && (YEAR % 100)) || (!(YEAR % 400))) { \
			RET = 1; \
		} else { \
			RET = 0; \
		} \
} while (0)

/**
  * @}
  */

#define RTC_CMP_ADJUST_DIR_PLUS   		((u32)0x00000000)
#define RTC_CMP_ADJUST_DIR_MINUS  		((u32)0x00000001)
#define IS_RTC_CMP_ADJUST_DIR(DIR)		(((DIR) == RTC_CMP_ADJUST_DIR_PLUS) || \
										((DIR) == RTC_CMP_ADJUST_DIR_MINUS))

#define RTC_BIT_IT_ALARM_ALL_STS 		(RTC_BIT_ALRM_ISR | \
										RTC_BIT_ALRM_DOY_ISR | \
										RTC_BIT_ALRM_YRS_ISR | \
										RTC_BIT_ALRM_MTH_ISR | \
										RTC_BIT_ALRM_DOM_ISR | \
										RTC_BIT_ALRM_DOW_ISR | \
										RTC_BIT_ALRM_HRS_ISR | \
										RTC_BIT_ALRM_MIN_ISR | \
										RTC_BIT_ALRM_SEC_ISR)

#define RTC_BIT_IT_CNT_ALARM_ALL_STS 	(RTC_BIT_CNT_DOY_ISR | \
										RTC_BIT_CNT_YRS_ISR | \
										RTC_BIT_CNT_MTH_ISR | \
										RTC_BIT_CNT_DOM_ISR | \
										RTC_BIT_CNT_DOW_ISR | \
										RTC_BIT_CNT_HRS_ISR | \
										RTC_BIT_CNT_MIN_ISR | \
										RTC_BIT_CNT_SEC_ISR)


enum  rtc_reg_access_addr_e {
	Rtc_Fen       = 0x00,   //!< REG_RTC_FEN
	Rtc_Cmp       = 0x08,   //!< REG_RTC_CMP
	Rtc_Imr       = 0x10,   //!< REG_RTC_IMR
	Rtc_Isr       = 0x14,   //!< REG_RTC_ISR
	Rtc_Tim0      = 0x20,   //!< REG_RTC_TIM0
	Rtc_Tim1      = 0x24,   //!< REG_RTC_TIM1
	Rtc_Tim2      = 0x28,   //!< REG_RTC_TIM2
	Rtc_Ctrl0     = 0x30,   //!< REG_RTC_ALRM_CTRL0
	Rtc_Ctrl1     = 0x34,   //!< REG_RTC_ALRM_CTRL1
	Rtc_Ctrl2     = 0x38    //!< REG_RTC_ALRM_CTRL2

};


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup RTC_Exported_Functions RTC Exported Functions
  * @{
  */
_LONG_CALL_ void RTC_StructInit(RTC_InitTypeDef *RTC_InitStruct);
_LONG_CALL_ void RTC_AlarmStructInit(RTC_AlarmTypeDef *RTC_AlarmStruct);
_LONG_CALL_ void RTC_Init(RTC_InitTypeDef *RTC_InitStruct);
_LONG_CALL_ void RTC_TimeStructInit(RTC_TimeTypeDef *RTC_TimeStruct);
_LONG_CALL_ void RTC_AlarmCmd(u32 NewState);
_LONG_CALL_ void RTC_AlarmClear(void);
_LONG_CALL_ void RTC_CntAlarmClear(void);

_LONG_CALL_ void RTC_Enable(u8 NewState);
_LONG_CALL_ u32 RTC_GetINTStatus(void);
_LONG_CALL_ void RTC_INTClear(void);
_LONG_CALL_ void RTC_SetAlarm(u32 RTC_Format, RTC_AlarmTypeDef *RTC_AlarmStruct);
_LONG_CALL_ void RTC_GetAlarm(u32 RTC_Format, RTC_AlarmTypeDef *RTC_AlarmStruct);
_LONG_CALL_ void RTC_SetCntAlarm(u8 cnt_alarm);
_LONG_CALL_ void RTC_SetCompensation(u32 dir, u32 time_in_sec, u32 NewState);
_LONG_CALL_ void RTC_IRQInit(IRQ_FUN irq_func, void *irq_data);
_LONG_CALL_ void RTC_IRQDeInit(void);
_LONG_CALL_ void RTC_SetTime(u32 RTC_Format, RTC_TimeTypeDef *RTC_TimeStruct);
_LONG_CALL_ void RTC_GetTime(u32 RTC_Format, RTC_TimeTypeDef *RTC_TimeStruct);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
