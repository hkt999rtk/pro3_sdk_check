
#ifndef _PINNAMES_H_
#define _PINNAMES_H_

#include "cmsis.h"
#include "PortNames.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_DATA(PUPD, FUNC)	(((PUPD) << 6) | (FUNC))
#define PIN_PIN_PUPD(X)			((X) & 0x03) /* PullNone/PullUp/PullDown */
#define PIN_FUNC(X)				((X) & 0x7F) /* PINMUX_FUNCTION_XXXX */

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GPIO
 *  @brief    MBED_GPIO driver modules.
 *  @{
 */

/** @addtogroup MBED_GPIO_Exported_Types MBED_GPIO Exported Types
  * @{
  */
/** @defgroup MBED_GPIO_Enumeration_Type Enumeration Type
  * @{
  */

typedef enum {
	PIN_INPUT = 0,
	PIN_OUTPUT
} PinDirection;

/* (((port)<<5)|(pin)) */
typedef enum {
	PA_0  = (PORT_A << 5 | 0),
	PA_1  = (PORT_A << 5 | 1),
	PA_2  = (PORT_A << 5 | 2),
	PA_3  = (PORT_A << 5 | 3),
	PA_4  = (PORT_A << 5 | 4),
	PA_5  = (PORT_A << 5 | 5),

	PB_0  = (PORT_B << 5 | 0),
	PB_1  = (PORT_B << 5 | 1),
	PB_2  = (PORT_B << 5 | 2),
	PB_3  = (PORT_B << 5 | 3),
	PB_4  = (PORT_B << 5 | 4),

	PC_0  = (PORT_C << 5 | 0),
	PC_1  = (PORT_C << 5 | 1),
	PC_2  = (PORT_C << 5 | 2),
	PC_3  = (PORT_C << 5 | 3),
	PC_4  = (PORT_C << 5 | 4),
	PC_5  = (PORT_C << 5 | 5),
	PC_6  = (PORT_C << 5 | 6),
	PC_7  = (PORT_C << 5 | 7),
	PC_8  = (PORT_C << 5 | 8),
	PC_9  = (PORT_C << 5 | 9),
	PC_10  = (PORT_C << 5 | 10),

	PD_0  = (PORT_D << 5 | 0),
	PD_1  = (PORT_D << 5 | 1),
	PD_2  = (PORT_D << 5 | 2),
	PD_3  = (PORT_D << 5 | 3),
	PD_4  = (PORT_D << 5 | 4),
	PD_5  = (PORT_D << 5 | 5),
	PD_6  = (PORT_D << 5 | 6),
	PD_7  = (PORT_D << 5 | 7),
	PD_8  = (PORT_D << 5 | 8),
	PD_9  = (PORT_D << 5 | 9),

	PE_0  = (PORT_E << 5 | 0),
	PE_1  = (PORT_E << 5 | 1),
	PE_2  = (PORT_E << 5 | 2),
	PE_3  = (PORT_E << 5 | 3),
	PE_4  = (PORT_E << 5 | 4),
	PE_5  = (PORT_E << 5 | 5),
	PE_6  = (PORT_E << 5 | 6),
	PE_7  = (PORT_E << 5 | 7),
	PE_8  = (PORT_E << 5 | 8),
	PE_9  = (PORT_E << 5 | 9),
	PE_10  = (PORT_E << 5 | 10),
	PE_11  = (PORT_E << 5 | 11),
	PE_12  = (PORT_E << 5 | 12),
	PE_13  = (PORT_E << 5 | 13),
	PE_14  = (PORT_E << 5 | 14),
	PE_15  = (PORT_E << 5 | 15),
	PE_16  = (PORT_E << 5 | 16),

	PF_0  = (PORT_F << 5 | 0),
	PF_1  = (PORT_F << 5 | 1),
	PF_2  = (PORT_F << 5 | 2),
	PF_3  = (PORT_F << 5 | 3),
	PF_4  = (PORT_F << 5 | 4),
	PF_5  = (PORT_F << 5 | 5),
	PF_6  = (PORT_F << 5 | 6),
	PF_7  = (PORT_F << 5 | 7),
	PF_8  = (PORT_F << 5 | 8),
	PF_9  = (PORT_F << 5 | 9),
	PF_10  = (PORT_F << 5 | 10),
	PF_11  = (PORT_F << 5 | 11),
	PF_12  = (PORT_F << 5 | 12),
	PF_13  = (PORT_F << 5 | 13),
	PF_14  = (PORT_F << 5 | 14),
	PF_15  = (PORT_F << 5 | 15),

	PS_0  = (PORT_S << 5 | 0),
	PS_1  = (PORT_S << 5 | 1),
	PS_2  = (PORT_S << 5 | 2),
	PS_3  = (PORT_S << 5 | 3),
	PS_4  = (PORT_S << 5 | 4),
	PS_5  = (PORT_S << 5 | 5),
	PS_6  = (PORT_S << 5 | 6),

	PP_0  = (PORT_P << 5 | 0),
	PP_1  = (PORT_P << 5 | 1),
	PP_2  = (PORT_P << 5 | 2),
	PP_3  = (PORT_P << 5 | 3),
	PP_4  = (PORT_P << 5 | 4),
	PP_5  = (PORT_P << 5 | 5),
	PP_6  = (PORT_P << 5 | 6),
	PP_7  = (PORT_P << 5 | 7),
	PP_8  = (PORT_P << 5 | 8),
	PP_9  = (PORT_P << 5 | 9),
	PP_10  = (PORT_P << 5 | 10),
	PP_11  = (PORT_P << 5 | 11),

	AD_0 = PF_12, //CH0
	AD_1 = PF_3, //CH1
	AD_2 = PF_2, //CH2
	AD_3 = PF_1, //CH3
	AD_4 = PA_0, //CH4
	AD_5 = PA_1, //CH5
	AD_6 = PA_2, //CH6
	AD_7 = PA_5, //CH7

	// Not connected
	NC = (uint32_t)0xFFFFFFFF
} PinName;


typedef enum {
	PullNone  = 0, //IN HIGHZ
	PullDown    = 1,
	PullUp  = 2,
	PullNoneSlp = 3,
	PullUpSlp = 4,
	PullDownSlp = 5,
	PullDefault = PullNone
} PinMode;
/** @}*/
/** @}*/

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

#endif
