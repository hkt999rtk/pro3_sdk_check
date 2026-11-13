#ifndef __DEVICE_PLATFORM_CONFIG_
#define __DEVICE_PLATFORM_CONFIG_

/****************************************************/
/* Encoder HW configuration                         */
/****************************************************/
#include "ameba_soc.h"

/* encoder IO base address */
#define AMEBAPRO3_ENC_IO_ADDR       ((uint32_t)ENCODER_REG_BASE)

/* encoder IRQ line number
 * NP --> 40
 * VP --> 34
 * AP --> 40 */
#define AMEBAPRO3_ENC_IRQ_NUM       ENCODER_IRQ


/****************************************************/
/* memory address configuration                     */
/****************************************************/
#if 0

extern uint32_t __vsram_start__[];
extern uint32_t __vsram_end__[];

/* encoder memory pool start address */
#define AMEBAPRO3_ENC_MEM_BASE     ((uint32_t)__vsram_start__)

/* encoder memory pool size */
#define AMEBAPRO3_ENC_MEM_SIZE     ((uint32_t)__vsram_end__ - (uint32_t)__vsram_start__)

#else  //DDR

extern uint32_t __en_ddr_start__[];
extern uint32_t __en_ddr_end__[];

/* encoder memory pool start address */
#define AMEBAPRO3_ENC_MEM_BASE     ((uint32_t)__en_ddr_start__)

/* encoder memory pool size */
#define AMEBAPRO3_ENC_MEM_SIZE     ((uint32_t)__en_ddr_end__ - (uint32_t)__en_ddr_start__)

#endif

#endif /* __DEVICE_PLATFORM_CONFIG_ */
