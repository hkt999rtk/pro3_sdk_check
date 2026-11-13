/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_AUD_H_
#define _SYSREG_AUD_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AUD
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup AUD_Register_Definitions AUD Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup AUD_ADDA_CTL
 * @brief
 * @{
 **/
#define AUD_BIT_POWADDACK                   ((u32)0x00000001 << 0)          /*!<R/W 1'b0  AD/DA clock power down control (0: power down, 1: power on) */
#define AUD_BIT_DAC_CKXEN                   ((u32)0x00000001 << 1)          /*!<R/W 1'b1  DAC chopper clock enable control (0: disable, 1: enable) */
#define AUD_BIT_DAC_CKXSEL                  ((u32)0x00000001 << 2)          /*!<R/W 1'b0  DAC chopper clock selection (0: ckx2/4, 1: ckx2/8) */
#define AUD_BIT_DAC_POW                     ((u32)0x00000001 << 3)          /*!<R/W 1'b0  DAC power down control (0: power down, 1: power on) */
#define AUD_MASK_DPRAMP_CSEL                ((u32)0x00000003 << 4)          /*!<R/W 2'b11  Depop C size selection (00: 1x, 01: 2x, 10: 3x, 11: 4x) */
#define AUD_DPRAMP_CSEL(x)                  ((u32)(((x) & 0x00000003) << 4))
#define AUD_GET_DPRAMP_CSEL(x)              ((u32)(((x >> 4) & 0x00000003)))
#define AUD_BIT_DPRAMP_ENRAMP               ((u32)0x00000001 << 6)          /*!<R/W 1'b0  DPRAMP enable ramp control (0: disable, 1: enable) */
#define AUD_BIT_DPRAMP_POW                  ((u32)0x00000001 << 7)          /*!<R/W 1'b0  DPRAMP power down control (0: power down, 1: power on) */
#define AUD_BIT_DTSDM_CKXEN                 ((u32)0x00000001 << 8)          /*!<R/W 1'b1  ADC integrater 1 OP chopper enable: 0 : disable 1 : enable (default) */
#define AUD_BIT_DTSDM_POW                 	((u32)0x00000001 << 9)          /*!<R/W 1'b0  ADC power on control: 0: power down 1: power on */
/** @} */

/** @defgroup AUD_LO_CTL
 * @brief
 * @{
 **/
#define AUD_BIT_LO_CAL                      ((u32)0x00000001 << 0)          /*!<R/W 1'b0  Lineout output offset calibration mode selection (1: calibration mode, 0: normal mode) */
#define AUD_BIT_LO_ENDP                     ((u32)0x00000001 << 1)          /*!<R/W 1'b1  Lineout enable depop control (0: disable, 1: enable) */
#define AUD_BIT_LO_POW                      ((u32)0x00000001 << 2)          /*!<R/W 1'b0  Lineout power down control (0: power down, 1: power on) */
#define AUD_BIT_LO_MDP                      ((u32)0x00000001 << 3)          /*!<R/W 1'b0  Lineout mute depop mode control (0: no depop, 1: depop) */
#define AUD_BIT_LO_SE                       ((u32)0x00000001 << 4)          /*!<R/W 1'b0  Lineout single-end mode control (0: differential, 1: single-end) */
#define AUD_MASK_LO_M                       ((u32)0x00000003 << 5)          /*!<R/W 2'b11  Lineout mute control (0: un-mute, 1: mute), <0>: DAC, <1>: Analog in */
#define AUD_LO_M(x)                         ((u32)(((x) & 0x00000003) << 5))
#define AUD_GET_LO_M(x)                     ((u32)(((x >> 5) & 0x00000003)))
#define AUD_BIT_LO_OPNDP                    ((u32)0x00000001 << 7)          /*!<R/W 1'b0  Lineout op negative depop mode control (0: no depop, 1: depop) */
#define AUD_BIT_LO_OPPDP                    ((u32)0x00000001 << 8)          /*!<R/W 1'b0  Lineout op positive depop mode control (0: no depop, 1: depop) */
#define AUD_MASK_LO_GSEL                    ((u32)0x00000003 << 9)          /*!<R/W 2'b00  Lineout gain selection (00: 0dB, 01: -3dB, 10: -6dB, 11: -9dB) */
#define AUD_LO_GSEL(x)                      ((u32)(((x) & 0x00000003) << 9))
#define AUD_GET_LO_GSEL(x)                  ((u32)(((x >> 9) & 0x00000003)))
#define AUD_MASK_LO_BIAS                    ((u32)0x00000003 << 11)          /*!<R/W 2'b00  Lineout mode output MOS quiescent curremt tuning (00: 120uA) */
#define AUD_LO_BIAS(x)                      ((u32)(((x) & 0x00000003) << 11))
#define AUD_GET_LO_BIAS(x)                  ((u32)(((x >> 11) & 0x00000003)))
#define AUD_MASK_LO_CCSEL                   ((u32)0x00000003 << 13)          /*!<R/W 2'b11  Lineout op 1st stage miller cap size (00: 20pF, 01: 24pF, 10: 28pF, 11: 32pF) */
#define AUD_LO_CCSEL(x)                     ((u32)(((x) & 0x00000003) << 13))
#define AUD_GET_LO_CCSEL(x)                 ((u32)(((x >> 13) & 0x00000003)))
#define AUD_MASK_LO_STBLE_CTRL				((u32)0x00000003 << 15)		 	/*!<R/W 2'b00: LO_STBLE_CTRL<0> (0: use large switch to get better THD performance of Lineout, 1: use small switch) LO_STBLE_CTRL<1> (0: enlarge 2nd stage gm of LO opamp to reduce voltage stress, 1: 2nd stage gm of LO opamp use default small gm) */
#define AUD_LO_STBLE_CTRL(x)				((u32)(((x) & 0x00000003) << 15))
#define AUD_GET_LO_STBLE_CTRL(x)			((u32)(((x >> 15) & 0x00000003)))
/** @} */

/** @defgroup AUD_MICBIAS_CTL
 * @brief
 * @{
 **/
#define AUD_BIT_MICBIAS_ENCHX              ((u32)0x00000001 << 0)          /*!<R/W 1'b1  MICBIAS enable chopper clock 0:disable 1:enable */
#define AUD_BIT_MICBIAS_POW                ((u32)0x00000001 << 1)          /*!<R/W 1'b0  MICBIAS power control 0:power down 1:power on */
#define AUD_MASK_MICBIAS_VSET              ((u32)0x0000000F << 2)          /*!<R/W 4'b0011  MICBIAS select output voltage level 0.1V per step 0000:1.15V 0001:1.25V 0010:1.35V 0011:1.45V 0100:1.55V 0101:1.65V 0110:1.75V 0111:1.8V */
#define AUD_MICBIAS_VSET(x)                ((u32)(((x) & 0x0000000F) << 3))
#define AUD_GET_MICBIAS_VSET(x)            ((u32)(((x >> 3) & 0x0000000F)))
#define AUD_MASK_MICBIAS_OCSEL             ((u32)0x00000003 << 6)          /*!<R/W 2'b01  OCP current selection 00: 2.5mA 01: 5mA 10: 7.5mA 11: 10mA */
#define AUD_MICBIAS_OCSEL(x)               ((u32)(((x) & 0x00000003) << 6))
#define AUD_GET_MICBIAS_OCSEL(x)           ((u32)(((x >> 6) & 0x00000003)))
#define AUD_MASK_MICBIAS_COUNT             ((u32)0x00000003 << 8)          /*!<R/W 2'b01  when OCP happen disable time x312.5kHz 00: 819.2us 01: 1638.4us 10: 3276.8us 11: 6553.6us */
#define AUD_MICBIAS_COUNT(x)               ((u32)(((x) & 0x00000003) << 8))
#define AUD_GET_MICBIAS_COUNT(x)           ((u32)(((x >> 8) & 0x00000003)))
#define AUD_BIT_MICBIAS_POWSHDT            ((u32)0x00000001 << 10)          /*!<R/W 1'b0  MICBIAS OCP power control 0: disable OCP 1: enable OCP */
#define AUD_BIT_MICBIAS_COMP               ((u32)0x0000000F << 11)          /*!<R/W 1'b0  MICBIAS_COMP<0> 0: disable internal miller cap 1: enable internal miller cap */
#define AUD_BIT_MICBIAS_OCP                ((u32)0x00000001 << 12)          /*!<R   OCP happen output signal */
/** @} */

/** @defgroup AUD_MICBST_CTL
 * @brief
 * @{
 **/
#define AUD_BIT_MICBST_POW                 ((u32)0x00000001 << 0)          /*!<R/W 1'b0  MICBST power control left channel 0:power down 1:power on */
#define AUD_BIT_MICBST_ENDF                ((u32)0x00000001 << 1)          /*!<R/W 1'b0  MICBST left channel enable differential 0:single to differential 1:differential to differential */
#define AUD_BIT_MICBST_ENCAL               ((u32)0x00000001 << 2)          /*!<R/W 1'b0  MICBST left channel enable calibration path 0:disable 1:enable */
#define AUD_BIT_MICBST_ENCAL_SWAP          ((u32)0x00000001 << 3)          /*!<R/W 1'b0  MICBST left channel swap calibration path 0:disable 1:enable */
#define AUD_MASK_MICBST_GSEL               ((u32)0x0000000F << 4)          /*!<R/W 4'b0000  MICBST left channel gain select 0000: 0dB 0001:5dB 0010:10dB 0011:15dB 0100:20dB 0101:25dB 0110:30dB 0111:35dB 1XXX:40dB */
#define AUD_MICBST_GSEL(x)                 ((u32)(((x) & 0x0000000F) << 4))
#define AUD_GET_MICBST_GSEL(x)             ((u32)(((x >> 4) & 0x0000000F)))
#define AUD_BIT_MICBST_MUTE                ((u32)0x00000001 << 8)          /*!<R/W 1'b1  MICBST left channel mute control 0:unmute 1:mute */
/** @} */

/** @defgroup AUD_LDO_CTL
 * @brief
 * @{
 **/
#define AUD_BIT_LDO_POW						((u32)0x00000001 << 0)          /*!<R/W 1'b0  LDO power control 0:disable LDO 1:enable LDO */
#define AUD_MASK_LDO_TUNE					((u32)0x0000001F << 1)          /*!<R/W 5'b10000  LDO voltage control 10000 Vref 0.9V LDO 1.8V */
#define AUD_LDO_TUNE(x)						((u32)(((x) & 0x0000001F) << 1))
#define AUD_GET_LDO_TUNE(x)					((u32)(((x >> 1) & 0x0000001F)))
#define AUD_BIT_LDO_COMP_INT				((u32)0x00000001 << 6)          /*!<R/W 1'b0  LDO Miller Compensation 0:close 1:short */
#define AUD_BIT_LDO_POW_0P9V				((u32)0x00000001 << 7)          /*!<R/W 1'b0  LDO Precharge 0: LDO 1: Unity Gain buffer */
#define AUD_BIT_LDO_PREC					((u32)0x00000001 << 8)          /*!<R/W 1'b0  REG_LOD_PREC */
#define AUD_BIT_LDO_EM						((u32)0x00000001 << 9)          /*!<R/W 1'b1  selection for driving  path of LDO_CODEC 0:select strong driving path, PMOS w=240um, L=0.44um 1:select 1.8kohm driving path(default) */
#define AUD_BIT_LDO_DISCHARGE				((u32)0x00000001 << 10)          /*!<R/W 1'b0  selection for discharge  path of LDO_CODEC 0:discharge  path is turned off (default) 1:1.8kohm discharge  path is turned on */

/** @} */

/** @defgroup AUD_MBIAS_CTL
 * @brief
 * @{
 **/
#define AUD_BIT_MBIAS_POW					((u32)0x00000001 << 0)          /*!<R/W 1'b0  MBIAS power control 0:power down 1:power on */
#define AUD_BIT_LPMODE_EN					((u32)0x00000001 << 1)          /*!<R/W 1'b0  low power mode en 0: disable , current use 12.5uA 1: enable , current use 1uA */
#define AUD_MASK_MBIAS_ISEL_DAC             ((u32)0x00000007 << 2)          /*!<R/W 3'b010  DAC current control: default 4uA, 0.5uA per step */
#define AUD_MBIAS_ISEL_DAC(x)               ((u32)(((x) & 0x00000007) << 2))
#define AUD_GET_MBIAS_ISEL_DAC(x)           ((u32)(((x >> 2) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_DAC_VCM			((u32)0x00000007 << 5)          /*!<R/W 3'b010  DAC vcm buffer current control: default 4uA, 0.5uA per step */
#define AUD_MBIAS_ISEL_DAC_VCM(x)           ((u32)(((x) & 0x00000007) << 5))
#define AUD_GET_MBIAS_ISEL_DAC_VCM(x)       ((u32)(((x >> 5) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_DPRAMP			((u32)0x00000007 << 8)          /*!<R/W 3'b010  DPRAMP current control: default 4uA, 0.5uA per step */
#define AUD_MBIAS_ISEL_DPRAMP(x)           	((u32)(((x) & 0x00000007) << 8))
#define AUD_GET_MBIAS_ISEL_DPRAMP(x)       	((u32)(((x >> 8) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_LO              ((u32)0x00000007 << 11)          /*!<R/W 3'b110  Lineout current control: if LPMODE_EN=1 000:1uA, 001:1.5uA, 010:2uA, 011:2.5uA, 100:3uA, 101:3.5uA, 110:4uA, 111:4.5uA if LPMODE_EN=0 000:2uA, 001:3uA, 010:4uA, 011:5uA, 100:6uA, 101:7uA, 110:8uA, 111:9uA */
#define AUD_MBIAS_ISEL_LO(x)                ((u32)(((x) & 0x00000007) << 11))
#define AUD_GET_MBIAS_ISEL_LO(x)            ((u32)(((x >> 11) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_MICBIAS          ((u32)0x00000007 << 14)          /*!<R/W 3'b010  MIVBIAS current control: default 4uA, 0.5uA per step */
#define AUD_MBIAS_ISEL_MICBIAS(x)            ((u32)(((x) & 0x00000007) << 14))
#define AUD_GET_MBIAS_ISEL_MICBIAS(x)        ((u32)(((x >> 14) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_DTSDM          ((u32)0x00000007 << 17)          /*!<R/W 3'b110  DTSDM current control: if LPMODE_EN=1 000:1uA, 001:1.5uA, 010:2uA, 011:2.5uA, 100:3uA, 101:3.5uA, 110:4uA, 111:4.5uA if LPMODE_EN=0 000:2uA, 001:3uA, 010:4uA, 011:5uA, 100:6uA, 101:7uA, 110:8uA, 111:9uA */
#define AUD_MBIAS_ISEL_DTSDM(x)            ((u32)(((x) & 0x00000007) << 17))
#define AUD_GET_MBIAS_ISEL_DTSDM(x)        ((u32)(((x >> 17) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_DTSDM_INT1     ((u32)0x00000007 << 20)          /*!<R/W 3'b110  DTSDM integrator current control: if LPMODE_EN=1 000:2uA, 001:3uA, 010:4uA, 011:5uA, 100:6uA, 101:7uA, 110:8uA, 111:9uA if LPMODE_EN=0 000:4uA, 001:6uA, 010:8uA, 011:10uA, 100:12uA, 101:14uA, 110:16uA, 111:18uA */
#define AUD_MBIAS_ISEL_DTSDM_INT1(x)       ((u32)(((x) & 0x00000007) << 20))
#define AUD_GET_MBIAS_ISEL_DTSDM_INT1(x)   ((u32)(((x >> 20) & 0x00000007)))
#define AUD_MASK_MBIAS_ISEL_MICBST     ((u32)0x00000007 << 23)          /*!<R/W 3'b010  MICBST current control:: default 4uA, 0.5uA per step */
#define AUD_MBIAS_ISEL_MICBST(x)       ((u32)(((x) & 0x00000007) << 23))
#define AUD_GET_MBIAS_ISEL_MICBST(x)   ((u32)(((x >> 23) & 0x00000007)))
/** @} */

/* Exported types --------------------------------------------------------*/

/** @defgroup AUD_Exported_Types AUD Exported Types
  * @{
*****************************************************************************/


/**
 * @brief AUD Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t AUD_ADDA_CTL;                           	/*!<  Register,  Address offset: 0x00 */
	__IO uint32_t AUD_LO_CTL;                             	/*!<  Register,  Address offset: 0x04 */
	__IO uint32_t AUD_MICBIAS_CTL;                       	/*!<  Register,  Address offset: 0x08 */
	__IO uint32_t AUD_MICBST_CTL;                        	/*!<  Register,  Address offset: 0x00C */
	__IO uint32_t AUD_LDO_CTL;                        		/*!<  Register,  Address offset: 0x010 */
	__IO uint32_t RSVD0;                                  	/*!<  Reserved,  Address offset: 0x014 */
	__IO uint32_t AUD_MBIAS_CTL;                         	/*!<  Register,  Address offset: 0x018 */
} AUD_TypeDef;

// #define AUD_SYS_BASE ((AUD_TypeDef *) (0x41022100))
// #define AUD_SYS_BASE_S ((AUD_TypeDef *) (0x51022100))

/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here
/** @defgroup AUD_Power_Mode
  * @{
  */
#define AUD_POWER_ON			  ((u32)0x00000001)
#define AUD_POWER_DOWN			((u32)0x00000000)
#define IS_CODEC_POWER_SEL(SEL) (((SEL) == AUD_POWER_ON) || ((SEL) == AUD_POWER_DOWN))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_LintOut_Mode
 * @{
 */
#define AUD_SINGLE			((u32)0x00000000)
#define AUD_DIFF			  ((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUD_DAC_MUTE_Control
* @{
*/
#define AUD_ANALOGIN			((u32)0x00000001)
#define AUD_DACIN				  ((u32)0x00000002)
/**
  * @}
  */
/** @defgroup AUD_LDO_Power
  * @{
  */
#define IS_LDO_POWER_SEL(SEL) 	((SEL == AUD_POWER_ON) || (SEL == AUD_POWER_DOWN))
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
