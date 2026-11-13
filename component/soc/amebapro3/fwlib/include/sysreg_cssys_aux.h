/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SYSREG_CSSYS_AUX_H
#define SYSREG_CSSYS_AUX_H

/* AUTO_GEN_START */

/* CSSYS_AUX is a part of CSSYS_AUX_REG */


/** @addtogroup Ameba_Periph_Driver
* @{
*/
/** @defgroup CSSYS_AUX
* @brief CSSYS_AUX driver modules
* @{
*/


/** @defgroup REG_CSSYS_AUX_CFG0
 * @brief
 * @{
 */
#define CSSYS_AUX_BIT_DAP_FEN                       ((u32)0x00000001 << 31)                  /*!< R/W   1  
                                                                                              * Enable JTAG/SWD function - default to enable.
                                                                                              * This controlled bit is used as another solution for SW to reset JTAG/SWD
                                                                                              */
#define CSSYS_AUX_MASK_CSSYS_JEP106_TDESIGNER       ((u32)0x000007FF << 20)                  /*!< R/W   0x0 None */
#define CSSYS_AUX_CSSYS_JEP106_TDESIGNER(x)         ((u32)(((x) & 0x000007FF) << 20))
#define CSSYS_AUX_GET_CSSYS_JEP106_TDESIGNER(x)     ((u32)(((x >> 20) & 0x000007FF)))
#define CSSYS_AUX_MASK_CSSYS_INSTANCEID             ((u32)0x0000000F << 16)                  /*!< R/W   0x0 None */
#define CSSYS_AUX_CSSYS_INSTANCEID(x)               ((u32)(((x) & 0x0000000F) << 16))
#define CSSYS_AUX_GET_CSSYS_INSTANCEID(x)           ((u32)(((x >> 16) & 0x0000000F)))
#define CSSYS_AUX_MASK_CSSYS_REVISION               ((u32)0x0000000F << 12)                  /*!< R/W   0x0 None */
#define CSSYS_AUX_CSSYS_REVISION(x)                 ((u32)(((x) & 0x0000000F) << 12))
#define CSSYS_AUX_GET_CSSYS_REVISION(x)             ((u32)(((x >> 12) & 0x0000000F)))
#define CSSYS_AUX_MASK_CSSYS_PARTNUMBER             ((u32)0x00000FFF << 0)                   /*!< R/W   0x0 None */
#define CSSYS_AUX_CSSYS_PARTNUMBER(x)               ((u32)(((x) & 0x00000FFF) << 0))
#define CSSYS_AUX_GET_CSSYS_PARTNUMBER(x)           ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */


/** @defgroup REG_CSSYS_AUX_CFG1
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_ROMBASEADDR_AHBAP_0          ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0xE00FF003 None */
#define CSSYS_AUX_ROMBASEADDR_AHBAP_0(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_ROMBASEADDR_AHBAP_0(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CSSYS_AUX_CFG2
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_ROMBASEADDR_AHBAP_1          ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0xE00FF003 RSVD */
#define CSSYS_AUX_ROMBASEADDR_AHBAP_1(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_ROMBASEADDR_AHBAP_1(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CSSYS_AUX_CFG3
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_ROMBASEADDR_AHBAP_2          ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0xE00FF003 RSVD */
#define CSSYS_AUX_ROMBASEADDR_AHBAP_2(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_ROMBASEADDR_AHBAP_2(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CSSYS_AUX_CFG4
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_ROMBASEADDR_AHBAP_3          ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x8000_0003 RSVD */
#define CSSYS_AUX_ROMBASEADDR_AHBAP_3(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_ROMBASEADDR_AHBAP_3(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CSSYS_AUX_CFG5
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_ROMBASEADDR_APBAP_0          ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0x8000_0003 None */
#define CSSYS_AUX_ROMBASEADDR_APBAP_0(x)            ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_ROMBASEADDR_APBAP_0(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY0
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY0        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[31:0]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY0(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY0(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY1
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY1        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[63:31]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY1(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY1(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY2
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY2        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[95:64]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY2(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY2(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY3
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY3        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[127:96]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY3(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY3(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY4
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY4        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[159:128]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY4(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY4(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY5
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY5        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[191:160]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY5(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY5(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY6
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY6        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[223:192]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY6(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY6(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_SECURE_KEY7
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_SECURE_KEY7        ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * Secure JTAG Key[255:224]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_SECURE_KEY7(x)          ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_SECURE_KEY7(x)      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY0
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY0    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[31:0]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY0(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY0(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY1
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY1    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[63:31]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY1(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY1(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY2
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY2    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[95:64]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY2(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY2(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY3
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY3    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[127:96]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY3(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY3(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY4
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY4    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[159:128]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY4(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY4(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY5
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY5    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[191:160]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY5(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY5(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY6
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY6    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[223:192]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY6(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY6(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_NON_FIXED_NON_SECURE_KEY7
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_NON_FIXED_NON_SECURE_KEY7    ((u32)0xFFFFFFFF << 0)                   /*!< WOnce   0
                                                                                              * NON-Secure JTAG Key[255:224]
                                                                                              * This register just can be written once. After that, hardware need disable the writing function.
                                                                                              */
#define CSSYS_AUX_NON_FIXED_NON_SECURE_KEY7(x)      ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_NON_FIXED_NON_SECURE_KEY7(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_SECURE_JTAG_SWD_CTRL
 * @brief
 * @{
 */
#define CSSYS_AUX_BIT_FW_READY_NONSC                ((u32)0x00000001 << 1)                   /*!< R/W1 Once   0
                                                                                              * In No-Fixed Key Mode and Non-Secure Mode JTAG, Secure JTAG/SWD wait this signal.
                                                                                              * 0: Secue JTAG/SWD cannot execute the procedure of the unlocking JTAG/SWD. (Default)
                                                                                              * 1: Secue JTAG/SWD can execute the procedure of the unlocking JTAG/SWD.
                                                                                              */
#define CSSYS_AUX_BIT_FW_READY_SC                   ((u32)0x00000001 << 0)                   /*!< R/W1 Once   0
                                                                                              * In No-Fixed Key Mode and Secure Mode JTAG, Secure JTAG/SWD wait this signal.
                                                                                              * 0: Secue JTAG/SWD cannot execute the procedure of the unlocking JTAG/SWD. (Default)
                                                                                              * 1: Secue JTAG/SWD can execute the procedure of the unlocking JTAG/SWD.
                                                                                              */
/** @} */


/** @defgroup REG_CSSYS_AUX_SPARE0
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_KM0_SPARE0                   ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   SPARE for SW */
#define CSSYS_AUX_KM0_SPARE0(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_KM0_SPARE0(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_CSSYS_AUX_SPARE1
 * @brief
 * @{
 */
#define CSSYS_AUX_MASK_KM0_SPARE1                   ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0   SPARE for SW */
#define CSSYS_AUX_KM0_SPARE1(x)                     ((u32)(((x) & 0xFFFFFFFF) << 0))
#define CSSYS_AUX_GET_KM0_SPARE1(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */




/** @defgroup CSSYS_AUX_Exported_Types CSSYS_AUX Exported Types
* @{
*/
/**
* @brief CSSYS_AUX Register Declaration
*/

typedef struct {
	__IO uint32_t CSSYS_AUX_CFG0;                               /*!< Register,  Address offset:0x0000 */
	__IO uint32_t CSSYS_AUX_CFG1;                               /*!< Register,  Address offset:0x0004 */
	__IO uint32_t CSSYS_AUX_CFG2;                               /*!< Register,  Address offset:0x0008 */
	__IO uint32_t CSSYS_AUX_CFG3;                               /*!< Register,  Address offset:0x000C */
	__IO uint32_t CSSYS_AUX_CFG4;                               /*!< Register,  Address offset:0x0010 */
	__IO uint32_t CSSYS_AUX_CFG5;                               /*!< Register,  Address offset:0x0014 */
	__O  uint32_t NON_FIXED_SECURE_KEY0;                        /*!< Register,  Address offset:0x0018 */
	__O  uint32_t NON_FIXED_SECURE_KEY1;                        /*!< Register,  Address offset:0x001C */
	__O  uint32_t NON_FIXED_SECURE_KEY2;                        /*!< Register,  Address offset:0x0020 */
	__O  uint32_t NON_FIXED_SECURE_KEY3;                        /*!< Register,  Address offset:0x0024 */
	__O  uint32_t NON_FIXED_SECURE_KEY4;                        /*!< Register,  Address offset:0x0028 */
	__O  uint32_t NON_FIXED_SECURE_KEY5;                        /*!< Register,  Address offset:0x002C */
	__O  uint32_t NON_FIXED_SECURE_KEY6;                        /*!< Register,  Address offset:0x0030 */
	__O  uint32_t NON_FIXED_SECURE_KEY7;                        /*!< Register,  Address offset:0x0034 */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY0;                    /*!< Register,  Address offset:0x0038 */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY1;                    /*!< Register,  Address offset:0x003C */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY2;                    /*!< Register,  Address offset:0x0040 */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY3;                    /*!< Register,  Address offset:0x0044 */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY4;                    /*!< Register,  Address offset:0x0048 */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY5;                    /*!< Register,  Address offset:0x004C */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY6;                    /*!< Register,  Address offset:0x0050 */
	__O  uint32_t NON_FIXED_NON_SECURE_KEY7;                    /*!< Register,  Address offset:0x0054 */
	__IO uint32_t SECURE_JTAG_SWD_CTRL;                         /*!< Register,  Address offset:0x0058 */
	__IO uint32_t CSSYS_AUX_SPARE0;                             /*!< Register,  Address offset:0x005C */
	__IO uint32_t CSSYS_AUX_SPARE1;                             /*!< Register,  Address offset:0x0060 */
} CSSYS_AUX_TypeDef;

#define CSSYS_AUX_BASE ((CSSYS_AUX_TypeDef *) (0x60470000))
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
#endif  // SYSREG_CSSYS_AUX_H
