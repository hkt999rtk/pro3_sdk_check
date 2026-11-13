/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPCCFG_H_
#define _AMEBA_IPCCFG_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup IPC
  * @brief IPC driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_NP_Channel
  * @{
  */
#define IPC_N2F_SHELL_COMMAND	            0
#define IPC_N2F_TICKLESS_INDICATION		    1	/*!<  NP -->  FP Tickless indicate */
#define IPC_N2F_WIFI_FW_INFO		        2
// #define IPC_N2F_XXX		                    3
// #define IPC_N2F_XXX		           		    4
// #define IPC_N2F_XXX		           		    5
// #define IPC_N2F_XXX		           		    6
// #define IPC_N2F_XXX		           		    7
// #define IPC_N2F_XXX		           		    8
// #define IPC_N2F_XXX		           		    9
// #define IPC_N2F_XXX		           		    10
// #define IPC_N2F_XXX		           		    11
// #define IPC_N2F_XXX		           		    12
// #define IPC_N2F_XXX		           		    13
// #define IPC_N2F_XXX		           		    14
// #define IPC_N2F_XXX		           		    15

#define IPC_N2V_SHELL_COMMAND               0
#define IPC_N2V_TICKLESS_INDICATION		    1	/*!<  NP -->  VP Tickless indicate */
// #define IPC_N2V_XXX		                    2
// #define IPC_N2V_XXX		                    3
// #define IPC_N2V_XXX		           		    4
// #define IPC_N2V_XXX		           		    5
// #define IPC_N2V_XXX		           		    6
// #define IPC_N2V_XXX		           		    7
// #define IPC_N2V_XXX		           		    8
// #define IPC_N2V_XXX		           		    9
// #define IPC_N2V_XXX		           		    10
// #define IPC_N2V_XXX		           		    11
// #define IPC_N2V_XXX		           		    12
// #define IPC_N2V_XXX		           		    13
// #define IPC_N2V_XXX		           		    14
// #define IPC_N2V_XXX		           		    15

#define IPC_N2A_WIFI_TRX_TRAN				0	/*!<  NP -->  AP WIFI Message Exchange */
#define IPC_N2A_WIFI_API_TRAN				1	/*!<  NP -->  AP API WIFI Message Exchange */
#define IPC_N2A_WIFI_FW_INFO				2	/*!<  NP -->  AP FW Info*/
#define IPC_N2A_FLASHPG_REQ				3	/*!<  NP -->  AP Flash Program REQUEST*/
#define IPC_N2A_LOGUART_RX_SWITCH		4	/*!<  NP -->  AP Loguart Rx Switch*/
#define IPC_N2A_COEX_API_TRAN				5	/*!<  NP -->  AP COEX API Exchange */
// #define IPC_N2A_BT_DATA_TRAN			5	/*!<  NP -->  AP BT DATA Exchange */
#define IPC_N2A_TICKLESS_INDICATION		6	/*!<  NP -->  AP Tickless indicate */
#define IPC_N2A_WAKE_AP					7	/*!<  NP -->  AP Wakeup*/
#define IPC_N2A_SHELL_COMMAND	            9
#define IPC_N2A_WOWLAN                    10
#define IPC_N2A_PMC_RESULT                11	/*!<  NP -->  AP PMC wake result reply */
// #define IPC_N2A_XXX		                    3
// #define IPC_N2A_XXX		           		    4
// #define IPC_N2A_XXX		           		    5
// #define IPC_N2A_XXX		           		    6
// #define IPC_N2A_XXX		           		    7
// #define IPC_N2A_XXX		           		    8
// #define IPC_N2A_XXX		           		    9
// #define IPC_N2A_XXX		           		    10
// #define IPC_N2A_XXX		           		    11
// #define IPC_N2A_XXX		           		    12
// #define IPC_N2A_XXX		           		    13
// #define IPC_N2A_XXX		           		    14
// #define IPC_N2A_XXX		           		    15



/** @defgroup IPC_VP_Channel
  * @{
  */
#define IPC_V2A_TICKLESS_INDICATION         0	/*!<  VP -->  AP Tickless indicate */
#define IPC_V2A_VCMD_RESP_CH0               1
#define IPC_V2A_VCMD_RESP_CH1               2
#define IPC_V2A_VCMD_RESP_CH2               3
#define IPC_V2A_VCMD_RESP_CH3               4
#define IPC_V2A_VCMD_RESP_CH4               5
// #define IPC_V2A_XXX		           		    6
// #define IPC_V2A_XXX		           		    7
// #define IPC_V2A_XXX		           		    8
// #define IPC_V2A_XXX		           		    9
// #define IPC_V2A_XXX		           		    10
// #define IPC_V2A_XXX		           		    11
// #define IPC_V2A_XXX		           		    12
// #define IPC_V2A_XXX		           		    13
// #define IPC_V2A_XXX		           		    14
// #define IPC_V2A_XXX		           		    15

#define IPC_V2N_SHELL_COMMAND               0
#define IPC_V2N_TICKLESS_INDICATION         1	/*!<  VP -->  NP Tickless indicate */
// #define IPC_V2N_XXX		                    2
// #define IPC_V2N_XXX		                    3
// #define IPC_V2N_XXX		           		    4
// #define IPC_V2N_XXX		           		    5
// #define IPC_V2N_XXX		           		    6
// #define IPC_V2N_XXX		           		    7
// #define IPC_V2N_XXX		           		    8
// #define IPC_V2N_XXX		           		    9
// #define IPC_V2N_XXX		           		    10
// #define IPC_V2N_XXX		           		    11
// #define IPC_V2N_XXX		           		    12
// #define IPC_V2N_XXX		           		    13
// #define IPC_V2N_XXX		           		    14
// #define IPC_V2N_XXX		           		    15


/** @defgroup IPC_FP_Channel
  * @{
  */
#define IPC_F2N_TICKLESS_INDICATION         0	/*!<  FP -->  NP Tickless indicate */
#if defined(CONFIG_BT_PG_VERIFY) && CONFIG_BT_PG_VERIFY
#define IPC_F2N_TICKLESS_INDICATION_BT      1	/*!<  FP -->  NP BT Tickless indicate */
#endif
// #define IPC_F2N_XXX		                    2
// #define IPC_F2N_XXX		                    3
// #define IPC_F2N_XXX		           		    4
// #define IPC_F2N_XXX		           		    5
// #define IPC_F2N_XXX		           		    6
// #define IPC_F2N_XXX		           		    7
// #define IPC_F2N_XXX		           		    8
// #define IPC_F2N_XXX		           		    9
// #define IPC_F2N_XXX		           		    10
// #define IPC_F2N_XXX		           		    11
// #define IPC_F2N_XXX		           		    12
// #define IPC_F2N_XXX		           		    13
// #define IPC_F2N_XXX		           		    14
// #define IPC_F2N_XXX		           		    15

#define IPC_F2A_TICKLESS_INDICATION         0	/*!<  FP -->  AP Tickless indicate */
// #define IPC_F2A_XXX		                    1
// #define IPC_F2A_XXX		                    2
// #define IPC_F2A_XXX		                    3
// #define IPC_F2A_XXX		           		    4
// #define IPC_F2A_XXX		           		    5
// #define IPC_F2A_XXX		           		    6
// #define IPC_F2A_XXX		           		    7
// #define IPC_F2A_XXX		           		    8
// #define IPC_F2A_XXX		           		    9
// #define IPC_F2A_XXX		           		    10
// #define IPC_F2A_XXX		           		    11
// #define IPC_F2A_XXX		           		    12
// #define IPC_F2A_XXX		           		    13
// #define IPC_F2A_XXX		           		    14
// #define IPC_F2A_XXX		           		    15


/** @defgroup IPC_AP_Channel
  * @{
  */
#define IPC_A2V_TICKLESS_INDICATION         0	/*!<  AP -->  VP Tickless indicate */
#define IPC_A2V_VCMD_SEND_CH0               1
#define IPC_A2V_VCMD_SEND_CH1               2
#define IPC_A2V_VCMD_SEND_CH2               3
#define IPC_A2V_VCMD_SEND_CH3               4
#define IPC_A2V_VCMD_SEND_CH4               5
// #define IPC_A2V_XXX		           		    6
// #define IPC_A2V_XXX		           		    7
// #define IPC_A2V_XXX		           		    8
// #define IPC_A2V_XXX		           		    9
// #define IPC_A2V_XXX		           		    10
// #define IPC_A2V_XXX		           		    11
// #define IPC_A2V_XXX		           		    12
// #define IPC_A2V_XXX		           		    13
// #define IPC_A2V_XXX		           		    14
// #define IPC_A2V_XXX		           		    15

#define IPC_A2N_WIFI_TRX_TRAN						0	/*!<  AP -->  NP WIFI Message Exchange */
#define IPC_A2N_WIFI_API_TRAN						1	/*!<  AP -->  NP WIFI API Message Exchange */
#define IPC_A2N_WAKE_AP							2	/*!<  AP -->  NP Wakeup*/
#define IPC_A2N_LOGUART_RX_SWITCH				3	/*!<  AP -->  NP Loguart Rx Switch*/
#define IPC_A2N_FLASHPG_REQ						4	/*!<  AP -->  NP IFlash Program Request*/
#define IPC_A2N_COEX_API_TRAN						5	/*!<  AP -->  NP COEX API Exchange */
// #define IPC_A2N_BT_DATA_TRAN					5	/*!<  AP -->  NP BT DATA Exchange */
#define IPC_A2N_TICKLESS_INDICATION				6	/*!<  AP -->  NP Tickless indicate */
#define IPC_A2N_WIFI_FW_INFO						7	/*!<  AP -->  NP FW Info*/
#define IPC_A2N_DIAGNOSE								8	/*!<  AP -->  NP Diagnose API Message Exchange*/
#define IPC_A2N_WOWLAN                      9
#define IPC_A2N_UARTBRIDGE                  10	/*!<  AP -->  NP UART bridge channel. */
#define IPC_A2N_PMC_QUERY                   11	/*!<  AP -->  NP PMC wake result query */
// #define IPC_A2N_XXX		                    1
// #define IPC_A2N_XXX		                    2
// #define IPC_A2N_XXX		                    3
// #define IPC_A2N_XXX		           		    4
// #define IPC_A2N_XXX		           		    5
// #define IPC_A2N_XXX		           		    6
// #define IPC_A2N_XXX		           		    7
// #define IPC_A2N_XXX		           		    8
// #define IPC_A2N_XXX		           		    9
// #define IPC_A2N_XXX		           		    10
// #define IPC_A2N_XXX		           		    11
// #define IPC_A2N_XXX		           		    12
// #define IPC_A2N_XXX		           		    13
// #define IPC_A2N_XXX		           		    14
// #define IPC_A2N_XXX		           		    15

#define IPC_A2F_TICKLESS_INDICATION         0	/*!<  AP -->  FP Tickless indicate */
// #define IPC_A2F_XXX		                    1
// #define IPC_A2F_XXX		                    2
// #define IPC_A2F_XXX		                    3
// #define IPC_A2F_XXX		           		    4
// #define IPC_A2F_XXX		           		    5
// #define IPC_A2F_XXX		           		    6
// #define IPC_A2F_XXX		           		    7
// #define IPC_A2F_XXX		           		    8
// #define IPC_A2F_XXX		           		    9
// #define IPC_A2F_XXX		           		    10
// #define IPC_A2F_XXX		           		    11
// #define IPC_A2F_XXX		           		    12
// #define IPC_A2F_XXX		           		    13
// #define IPC_A2F_XXX		           		    14
// #define IPC_A2F_XXX		           		    15

#endif

