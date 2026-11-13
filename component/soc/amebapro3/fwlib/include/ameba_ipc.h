/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMEBA_IPC_H
#define AMEBA_IPC_H

/* AUTO_GEN_START */


/** @addtogroup Ameba_Periph_Driver
* @{
*/

/** @defgroup IPC
* @brief IPC driver modules
* @{
*/


/** @defgroup IPC_Exported_Struct_Access_Types IPC Exported Struct Access Types
* @{
*/

/**
* @brief IPC Struct Access Register Declaration
*/

typedef struct {                                                         /*!< (@ 0x40211000) IPC Structure                                                                   */
	union {
		uint32_t tx_data;                                                /*!< (@ 0x00000000)                                                                                 */
		struct {
			uint32_t tx0_data: 8;                                         /*!< [7..0]
																		  *    After preparing descriptor, data and corresponding memory,
																		  * 1: Writting 1 to the Tx data register (tx0_data) channel x bit,
																		  *    and the data will be mapped to the CPU0's RX data register (rx*_data) channel x bit.
																		  *    And the corresponding Rx full status bit in REG_IPC_ISR CPU0 will be set.
																		  * 0: Writting 0 has no effect.
																		  *    If the corresponding Rx full status bit in REG_IPC_ISR CPU0 (eg.isr_rx0_full_sts7) is cleared,
																		  *    the corresponding bit int tx0_data will be cleaned automatically.
																		  */
			uint32_t tx1_data: 8;                                         /*!< [15..8]    After preparing descriptor, data and corresponding memory,
																		  * 1: Writting 1 to the Tx data register (tx1_data) channel x bit,
																		  *    and the data will be mapped to the CPU1's RX data register (rx*_data) channel x bit.
																		  *    And the corresponding Rx full status bit in REG_IPC_ISR CPU1 will be set.
																		  * 0: Writting 0 has no effect.
																		  *    If the corresponding Rx full status bit in REG_IPC_ISR CPU1 (eg.isr_rx1_full_sts7) is cleared,
																		  *    the corresponding bit int tx1_data will be cleaned automatically.
																		  * */
			uint32_t tx2_data: 8;                                         /*!< [23..16]   After preparing descriptor, data and corresponding memory,
																		  * 1: Writting 1 to the Tx data register (tx2_data) channel x bit,
																		  *    and the data will be mapped to the CPU2's RX data register (rx*_data) channel x bit.
																		  *    And the corresponding Rx full status bit in REG_IPC_ISR CPU2 will be set.
																		  * 0: Writting 0 has no effect.
																		  *    If the corresponding Rx full status bit in REG_IPC_ISR CPU2 (eg.isr_rx2_full_sts7) is cleared,
																		  *    the corresponding bit int tx2_data will be cleaned automatically.
																		  */
			uint32_t rsvd0: 8;                                           /*!< [31..24]   RSVD                                                                            */
		} tx_data_b;
	} ;
	union {
		uint32_t rx_data;                                                /*!< (@ 0x00000004)                                                                             */
		struct {
			uint32_t rx0_data: 8;                                        /*!< [7..0]     RX_DATA automatically maps data from tx*_data of CPU0                           */
			uint32_t rx1_data: 8;                                        /*!< [15..8]    RX_DATA automatically maps data from tx*_data of CPU1                           */
			uint32_t rx2_data: 8;                                        /*!< [23..16]   RX_DATA automatically maps data from tx*_data of CPU2                           */
			uint32_t rsvd1: 8;                                           /*!< [31..24]   RSVD                                                                            */
		} rx_data_b;
	} ;
	union {
		uint32_t isr0_sts;                                               /*!< (@ 0x00000008)                                                                             */
		struct {
			uint32_t isr_tx0_empty_sts0: 1;                              /*!< [0]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts1: 1;                              /*!< [1]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts2: 1;                              /*!< [2]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts3: 1;                              /*!< [3]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts4: 1;                              /*!< [4]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts5: 1;                              /*!< [5]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts6: 1;                              /*!< [6]        Same as above                                                                   */
			uint32_t isr_tx0_empty_sts7: 1;                              /*!< [7]        TX channel 7 empty interrupt status of CPU0.
																		  *              If the corresponding rx full status bit in REG_IPC_ISR CPU0  is cleaned,
																		  *	             the corresponding TX empty status is set automatically. It will be cleaned by SW writting 1
																		  */
			uint32_t isr_rx0_full_sts0: 1;                               /*!< [8]        Same as above                                                                   */
			uint32_t isr_rx0_full_sts1: 1;                               /*!< [9]        Same as above                                                                   */
			uint32_t isr_rx0_full_sts2: 1;                               /*!< [10]       Same as above                                                                   */
			uint32_t isr_rx0_full_sts3: 1;                               /*!< [11]       Same as above                                                                   */
			uint32_t isr_rx0_full_sts4: 1;                               /*!< [12]       Same as above                                                                   */
			uint32_t isr_rx0_full_sts5: 1;                               /*!< [13]       Same as above                                                                   */
			uint32_t isr_rx0_full_sts6: 1;                               /*!< [14]       Same as above                                                                   */
			uint32_t isr_rx0_full_sts7: 1;                               /*!< [15]       RX channel 7 full interrupt status of CPU0.
																		  *              The corresponding RX full status bit will be set by CPU0's TX data register (tx*_Data) channel 7.
																		  *              It will be cleaned by SW writting 1.
																		  */
			uint32_t rsvd2: 16;                                          /*!< [31..16]   RSVD                                                                            */
		} isr0_sts_b;
	} ;
	union {
		uint32_t isr1_sts;                                               /*!< (@ 0x0000000C)                                                                             */
		struct {
			uint32_t isr_tx1_empty_sts0: 1;                              /*!< [0]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts1: 1;                              /*!< [1]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts2: 1;                              /*!< [2]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts3: 1;                              /*!< [3]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts4: 1;                              /*!< [4]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts5: 1;                              /*!< [5]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts6: 1;                              /*!< [6]        Same as above                                                                   */
			uint32_t isr_tx1_empty_sts7: 1;                          	 /*!< [7]        TX channel 7 empty interrupt status of CPU1.
																		  *				 If the corresponding rx full status bit in REG_IPC_ISR CPU1  is cleaned, the corresponding TX empty status is set automatically.
																		  *				 It will be cleaned by SW writting 1
																		  */
			uint32_t isr_rx1_full_sts0: 1;                               /*!< [8]        Same as above                                                                   */
			uint32_t isr_rx1_full_sts1: 1;                               /*!< [9]        Same as above                                                                   */
			uint32_t isr_rx1_full_sts2: 1;                               /*!< [10]       Same as above                                                                   */
			uint32_t isr_rx1_full_sts3: 1;                               /*!< [11]       Same as above                                                                   */
			uint32_t isr_rx1_full_sts4: 1;                               /*!< [12]       Same as above                                                                   */
			uint32_t isr_rx1_full_sts5: 1;                               /*!< [13]       Same as above                                                                   */
			uint32_t isr_rx1_full_sts6: 1;                               /*!< [14]       Same as above                                                                   */
			uint32_t isr_rx1_full_sts7: 1;                               /*!< [15]       RX channel 7 full interrupt status of CPU1.
																		  *				 The corresponding RX full status bit will be set by CPU1's TX data register (tx*_Data) channel 7.
																		  *				 It will be cleaned by SW writting 1.
																		  */
			uint32_t rsvd3: 16;                                          /*!< [31..16]   RSVD                                                                            */
		} isr1_sts_b;
	} ;
	union {
		uint32_t isr2_sts;                                               /*!< (@ 0x00000010)                                                                             */
		struct {
			uint32_t isr_tx2_empty_sts0: 1;                              /*!< [0]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts1: 1;                              /*!< [1]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts2: 1;                              /*!< [2]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts3: 1;                              /*!< [3]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts4: 1;                              /*!< [4]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts5: 1;                              /*!< [5]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts6: 1;                              /*!< [6]        Same as above                                                                   */
			uint32_t isr_tx2_empty_sts7: 1;                              /*!< [7]        TX channel 7 empty interrupt status of CPU2.
																		  * 			 If the corresponding rx full status bit in REG_IPC_ISR CPU2  is cleaned, the corresponding TX empty status is set automatically.
																		  *				 It will be cleaned by SW writting 1
																		  */
			uint32_t isr_rx2_full_sts0: 1;                               /*!< [8]        Same as above                                                                   */
			uint32_t isr_rx2_full_sts1: 1;                               /*!< [9]        Same as above                                                                   */
			uint32_t isr_rx2_full_sts2: 1;                               /*!< [10]       Same as above                                                                   */
			uint32_t isr_rx2_full_sts3: 1;                               /*!< [11]       Same as above                                                                   */
			uint32_t isr_rx2_full_sts4: 1;                               /*!< [12]       Same as above                                                                   */
			uint32_t isr_rx2_full_sts5: 1;                               /*!< [13]       Same as above                                                                   */
			uint32_t isr_rx2_full_sts6: 1;                               /*!< [14]       Same as above                                                                   */
			uint32_t isr_rx2_full_sts7: 1;                               /*!< [15]       RX channel 7 full interrupt status of CPU2.
																		  *				 The corresponding RX full status bit will be set by CPU2's TX data register (tx*_Data) channel 7.
																		  *				 It will be cleaned by SW writting 1.
																		  */
			uint32_t rsvd4: 16;                                          /*!< [31..16]   RSVD                                                                            */
		} isr2_sts_b;
	} ;
	union {
		uint32_t imr0;                                                   /*!< (@ 0x00000014)                                                                             */
		struct {
			uint32_t imr_tx0_empty_mask_dis: 8;                          /*!< [7..0]     0: Enable TX channel x empty interrupt mask of CPU0
																		  *			 1: Disable TX channel x empty interrupt mask of CPU0
																		  */
			uint32_t imr_rx0_full_mask_dis: 8;                            /*!< [15..8]    0: Enable RX channel x full interrupt mask of CPU0
																		  * 			 1: Disable RX channel x full interrupt mask of CPU0
																		  */
			uint32_t rsvd5: 16;                                          /*!< [31..16]   RSVD                                                                            */
		} imr0_b;
	} ;
	union {
		uint32_t imr1;                                                   /*!< (@ 0x00000018)                                                                             */
		struct {
			uint32_t imr_tx1_empty_mask_dis: 8;                          /*!< [7..0]     0: Enable TX channel x empty interrupt mask of CPU1
																		  * 			 1: Disable TX channel x empty interrupt mask of CPU1
																		  */
			uint32_t imr_rx1_full_mask_dis: 8;                           /*!< [15..8]    0: Enable RX channel x full interrupt mask of CPU1
																		  *				 1: Disable RX channel x full interrupt mask of CPU1
																		  */
			uint32_t rsvd6: 16;                                          /*!< [31..16]   RSVD                                                                            */
		} imr1_b;
	} ;
	union {
		uint32_t imr2;                                                   /*!< (@ 0x0000001C)                                                                             */
		struct {
			uint32_t imr_tx2_empty_mask_dis: 8;                          /*!< [7..0]     0: Enable TX channel x empty interrupt mask of CPU2
																		  *				 1: Disable TX channel x empty interrupt mask of CPU2
																		  */
			uint32_t imr_rx2_full_mask_dis: 8;                           /*!< [15..8]    0: Enable RX channel x full interrupt mask of CPU2
																		  *				 1: Disable RX channel x full interrupt mask of CPU2
																		  */
			uint32_t rsvd7: 16;                                          /*!< [31..16]   RSVD                                                                            */
		} imr2_b;
	} ;
	union {
		uint32_t icr;                                                    /*!< (@ 0x00000020)                                                                             */
		struct {
			uint32_t icr_tx0_data_clr0: 1;                               /*!< [0]        0: no effect 1: Clear the tx data register (tx0_data) channel 0                 */
			uint32_t icr_tx0_data_clr1: 1;                               /*!< [1]        0: no effect 1: Clear the tx data register (tx0_data) channel 1                 */
			uint32_t icr_tx0_data_clr2: 1;                               /*!< [2]        0: no effect 1: Clear the tx data register (tx0_data) channel 2                 */
			uint32_t icr_tx0_data_clr3: 1;                               /*!< [3]        0: no effect 1: Clear the tx data register (tx0_data) channel 3                 */
			uint32_t icr_tx0_data_clr4: 1;                               /*!< [4]        0: no effect 1: Clear the tx data register (tx0_data) channel 4                 */
			uint32_t icr_tx0_data_clr5: 1;                               /*!< [5]        0: no effect 1: Clear the tx data register (tx0_data) channel 5                 */
			uint32_t icr_tx0_data_clr6: 1;                               /*!< [6]        0: no effect 1: Clear the tx data register (tx0_data) channel 6                 */
			uint32_t icr_tx0_data_clr7: 1;                               /*!< [7]        0: no effect 1: Clear the tx data register (tx0_data) channel 7                 */
			uint32_t icr_tx1_data_clr0: 1;                               /*!< [8]        0: no effect 1: Clear the tx data register (tx1_data) channel 0                 */
			uint32_t icr_tx1_data_clr1: 1;                               /*!< [9]        0: no effect 1: Clear the tx data register (tx1_data) channel 1                 */
			uint32_t icr_tx1_data_clr2: 1;                               /*!< [10]       0: no effect 1: Clear the tx data register (tx1_data) channel 2                 */
			uint32_t icr_tx1_data_clr3: 1;                               /*!< [11]       0: no effect 1: Clear the tx data register (tx1_data) channel 3                 */
			uint32_t icr_tx1_data_clr4: 1;                               /*!< [12]       0: no effect 1: Clear the tx data register (tx1_data) channel 4                 */
			uint32_t icr_tx1_data_clr5: 1;                               /*!< [13]       0: no effect 1: Clear the tx data register (tx1_data) channel 5                 */
			uint32_t icr_tx1_data_clr6: 1;                               /*!< [14]       0: no effect 1: Clear the tx data register (tx1_data) channel 6                 */
			uint32_t icr_tx1_data_clr7: 1;                               /*!< [15]       0: no effect 1: Clear the tx data register (tx1_data) channel 7                 */
			uint32_t icr_tx2_data_clr0: 1;                               /*!< [16]       0: no effect 1: Clear the tx data register (tx2_data) channel 0                 */
			uint32_t icr_tx2_data_clr1: 1;                               /*!< [17]       0: no effect 1: Clear the tx data register (tx2_data) channel 1                 */
			uint32_t icr_tx2_data_clr2: 1;                               /*!< [18]       0: no effect 1: Clear the tx data register (tx2_data) channel 2                 */
			uint32_t icr_tx2_data_clr3: 1;                               /*!< [19]       0: no effect 1: Clear the tx data register (tx2_data) channel 3                 */
			uint32_t icr_tx2_data_clr4: 1;                               /*!< [20]       0: no effect 1: Clear the tx data register (tx2_data) channel 4                 */
			uint32_t icr_tx2_data_clr5: 1;                               /*!< [21]       0: no effect 1: Clear the tx data register (tx2_data) channel 5                 */
			uint32_t icr_tx2_data_clr6: 1;                               /*!< [22]       0: no effect 1: Clear the tx data register (tx2_data) channel 6                 */
			uint32_t icr_tx2_data_clr7: 1;                               /*!< [23]       0: no effect 1: Clear the tx data register (tx2_data) channel 7                 */
			uint32_t rsvd8: 8;                                           /*!< [31..24]   RSVD                                                                            */
		} icr_b;
	} ;
	union {
		uint32_t sem0;                                                   /*!< (@ 0x00000024)                                                                             */
		struct {
			uint32_t sema_hw_sts: 32;                                    /*!< [31..0]    0: The semaphore is free 1: The semaphore is occupied                           */
		} sem0_b;
	} ;
	union {
		uint32_t sem_id_0;                                               /*!< (@ 0x00000028)                                                                             */
		struct {
			uint32_t sema_hw_id: 32;                                     /*!< [31..0]    The CPUID which current occupy the semaphore                                    */
		} sem_id_0_b;
	} ;
	union {
		uint32_t sem_id_1;                                               /*!< (@ 0x0000002C)                                                                             */
		struct {
			uint32_t sema_hw_id: 32;                                     /*!< [31..0]    The CPUID which current occupy the semaphore                                    */
		} sem_id_1_b;
	} ;
	union {
		uint32_t sem_id_2;                                               /*!< (@ 0x00000030)                                                                             */
		struct {
			uint32_t sema_hw_id: 32;                                     /*!< [31..0]    The CPUID which current occupy the semaphore                                    */
		} sem_id_2_b;
	} ;
	union {
		uint32_t sem_id_3;                                               /*!< (@ 0x00000034)                                                                             */
		struct {
			uint32_t sema_hw_id: 32;                                     /*!< [31..0]    The CPUID which current occupy the semaphore                                    */
		} sem_id_3_b;
	} ;
	union {
		uint32_t spare0;                                                 /*!< (@ 0x00000038)                                                                             */
		struct {
			uint32_t spare0: 32;                                         /*!< [31..0]    nan                                                                             */
		} spare0_b;
	} ;
	union {
		uint32_t spare1;                                                 /*!< (@ 0x0000003C)                                                                             */
		struct {
			uint32_t spare1: 32;                                         /*!< [31..0]    nan                                                                             */
		} spare1_b;
	} ;
} IPC_Type;
#define IPC_DEV ((IPC_Type *) (0x40211000))
/** @} */


/** @defgroup REG_IPC_TX_DATA
 * @brief
 * @{
 */
#define IPC_MASK_RSVD0                              ((u32)0x000000FF << 24)                  /*!< R     0.0 RSVD */
#define IPC_RSVD0(x)                                ((u32)(((x) & 0x000000FF << 24))
#define IPC_GET_RSVD0(x)                            ((u32)(((x >> 24) & 0x000000FF)))
#define IPC_MASK_TX2_DATA                           ((u32)0x000000FF << 16)                  /*!< R/W   0.0 After preparing descriptor, data and corresponding memory,1: Writting 1 to the Tx data register (tx2_data) channel x bit, and the data will be mapped to the CPU2's RX data register (rx*_data) channel x bit. And the corresponding Rx full status bit in REG_IPC_ISR CPU2 will be set.0: Writting 0 has no effect.If the corresponding Rx full status bit in REG_IPC_ISR CPU2 (eg.isr_rx2_full_sts7) is cleared, the corresponding bit int tx2_data will be cleaned automatically. */
#define IPC_TX2_DATA(x)                             ((u32)(((x) & 0x000000FF << 16))
#define IPC_GET_TX2_DATA(x)                         ((u32)(((x >> 16) & 0x000000FF)))
#define IPC_MASK_TX1_DATA                           ((u32)0x000000FF << 8)                   /*!< R/W   0.0 After preparing descriptor, data and corresponding memory,1: Writting 1 to the Tx data register (tx1_data) channel x bit, and the data will be mapped to the CPU1's RX data register (rx*_data) channel x bit. And the corresponding Rx full status bit in REG_IPC_ISR CPU1 will be set.0: Writting 0 has no effect.If the corresponding Rx full status bit in REG_IPC_ISR CPU1 (eg.isr_rx1_full_sts7) is cleared, the corresponding bit int tx1_data will be cleaned automatically. */
#define IPC_TX1_DATA(x)                             ((u32)(((x) & 0x000000FF << 8))
#define IPC_GET_TX1_DATA(x)                         ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_TX0_DATA                           ((u32)0x000000FF << 0)                   /*!< R/W   0.0 After preparing descriptor, data and corresponding memory,1: Writting 1 to the Tx data register (tx0_data) channel x bit, and the data will be mapped to the CPU0's RX data register (rx*_data) channel x bit. And the corresponding Rx full status bit in REG_IPC_ISR CPU0 will be set.0: Writting 0 has no effect.If the corresponding Rx full status bit in REG_IPC_ISR CPU0 (eg.isr_rx0_full_sts7) is cleared, the corresponding bit int tx0_data will be cleaned automatically. */
#define IPC_TX0_DATA(x)                             ((u32)(((x) & 0x000000FF << 0))
#define IPC_GET_TX0_DATA(x)                         ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_IPC_RX_DATA
 * @brief
 * @{
 */
#define IPC_MASK_RSVD1                              ((u32)0x000000FF << 24)                  /*!< R     0.0 RSVD */
#define IPC_RSVD1(x)                                ((u32)(((x) & 0x000000FF << 24))
#define IPC_GET_RSVD1(x)                            ((u32)(((x >> 24) & 0x000000FF)))
#define IPC_MASK_RX2_DATA                           ((u32)0x000000FF << 16)                  /*!< R     0.0 RX_DATA automatically maps data from tx*_data of CPU2 */
#define IPC_RX2_DATA(x)                             ((u32)(((x) & 0x000000FF << 16))
#define IPC_GET_RX2_DATA(x)                         ((u32)(((x >> 16) & 0x000000FF)))
#define IPC_MASK_RX1_DATA                           ((u32)0x000000FF << 8)                   /*!< R     0.0 RX_DATA automatically maps data from tx*_data of CPU1 */
#define IPC_RX1_DATA(x)                             ((u32)(((x) & 0x000000FF << 8))
#define IPC_GET_RX1_DATA(x)                         ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_RX0_DATA                           ((u32)0x000000FF << 0)                   /*!< R     0.0 RX_DATA automatically maps data from tx*_data of CPU0 */
#define IPC_RX0_DATA(x)                             ((u32)(((x) & 0x000000FF << 0))
#define IPC_GET_RX0_DATA(x)                         ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_IPC_ISR0_STS
 * @brief
 * @{
 */
#define IPC_MASK_RSVD2                              ((u32)0x0000FFFF << 16)                  /*!< R     0.0 RSVD */
#define IPC_RSVD2(x)                                ((u32)(((x) & 0x0000FFFF << 16))
#define IPC_GET_RSVD2(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_BIT_ISR_RX0_FULL_STS7                   ((u32)0x00000001 << 15)                  /*!< R/W1C 0.0 RX channel 7 full interrupt status of CPU0. The corresponding RX full status bit will be set by CPU0's TX data register (tx*_Data) channel 7. It will be cleaned by SW writting 1. */
#define IPC_BIT_ISR_RX0_FULL_STS6                   ((u32)0x00000001 << 14)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX0_FULL_STS5                   ((u32)0x00000001 << 13)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX0_FULL_STS4                   ((u32)0x00000001 << 12)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX0_FULL_STS3                   ((u32)0x00000001 << 11)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX0_FULL_STS2                   ((u32)0x00000001 << 10)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX0_FULL_STS1                   ((u32)0x00000001 << 9)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX0_FULL_STS0                   ((u32)0x00000001 << 8)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS7                  ((u32)0x00000001 << 7)                   /*!< R/W1C 0.0 TX channel 7 empty interrupt status of CPU0. If the corresponding rx full status bit in REG_IPC_ISR CPU0  is cleaned, the corresponding TX empty status is set automatically. It will be cleaned by SW writting 1 */
#define IPC_BIT_ISR_TX0_EMPTY_STS6                  ((u32)0x00000001 << 6)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS5                  ((u32)0x00000001 << 5)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS4                  ((u32)0x00000001 << 4)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS3                  ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS2                  ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS1                  ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX0_EMPTY_STS0                  ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0 Same as above */
/** @} */


/** @defgroup REG_IPC_ISR1_STS
 * @brief
 * @{
 */
#define IPC_MASK_RSVD3                              ((u32)0x0000FFFF << 16)                  /*!< R     0.0 RSVD */
#define IPC_RSVD3(x)                                ((u32)(((x) & 0x0000FFFF << 16))
#define IPC_GET_RSVD3(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_BIT_ISR_RX1_FULL_STS7                   ((u32)0x00000001 << 15)                  /*!< R/W1C 0.0 RX channel 7 full interrupt status of CPU1. The corresponding RX full status bit will be set by CPU1's TX data register (tx*_Data) channel 7. It will be cleaned by SW writting 1. */
#define IPC_BIT_ISR_RX1_FULL_STS6                   ((u32)0x00000001 << 14)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX1_FULL_STS5                   ((u32)0x00000001 << 13)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX1_FULL_STS4                   ((u32)0x00000001 << 12)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX1_FULL_STS3                   ((u32)0x00000001 << 11)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX1_FULL_STS2                   ((u32)0x00000001 << 10)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX1_FULL_STS1                   ((u32)0x00000001 << 9)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX1_FULL_STS0                   ((u32)0x00000001 << 8)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS7                  ((u32)0x00000001 << 7)                   /*!< R/W1C 0.0 TX channel 7 empty interrupt status of CPU1. If the corresponding rx full status bit in REG_IPC_ISR CPU1  is cleaned, the corresponding TX empty status is set automatically. It will be cleaned by SW writting 1 */
#define IPC_BIT_ISR_TX1_EMPTY_STS6                  ((u32)0x00000001 << 6)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS5                  ((u32)0x00000001 << 5)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS4                  ((u32)0x00000001 << 4)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS3                  ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS2                  ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS1                  ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX1_EMPTY_STS0                  ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0 Same as above */
/** @} */


/** @defgroup REG_IPC_ISR2_STS
 * @brief
 * @{
 */
#define IPC_MASK_RSVD4                              ((u32)0x0000FFFF << 16)                  /*!< R     0.0 RSVD */
#define IPC_RSVD4(x)                                ((u32)(((x) & 0x0000FFFF << 16))
#define IPC_GET_RSVD4(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_BIT_ISR_RX2_FULL_STS7                   ((u32)0x00000001 << 15)                  /*!< R/W1C 0.0 RX channel 7 full interrupt status of CPU2. The corresponding RX full status bit will be set by CPU2's TX data register (tx*_Data) channel 7. It will be cleaned by SW writting 1. */
#define IPC_BIT_ISR_RX2_FULL_STS6                   ((u32)0x00000001 << 14)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX2_FULL_STS5                   ((u32)0x00000001 << 13)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX2_FULL_STS4                   ((u32)0x00000001 << 12)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX2_FULL_STS3                   ((u32)0x00000001 << 11)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX2_FULL_STS2                   ((u32)0x00000001 << 10)                  /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX2_FULL_STS1                   ((u32)0x00000001 << 9)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_RX2_FULL_STS0                   ((u32)0x00000001 << 8)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS7                  ((u32)0x00000001 << 7)                   /*!< R/W1C 0.0 TX channel 7 empty interrupt status of CPU2. If the corresponding rx full status bit in REG_IPC_ISR CPU2  is cleaned, the corresponding TX empty status is set automatically. It will be cleaned by SW writting 1 */
#define IPC_BIT_ISR_TX2_EMPTY_STS6                  ((u32)0x00000001 << 6)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS5                  ((u32)0x00000001 << 5)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS4                  ((u32)0x00000001 << 4)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS3                  ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS2                  ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS1                  ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0 Same as above */
#define IPC_BIT_ISR_TX2_EMPTY_STS0                  ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0 Same as above */
/** @} */


/** @defgroup REG_IPC_IMR0
 * @brief
 * @{
 */
#define IPC_MASK_RSVD5                              ((u32)0x0000FFFF << 16)                  /*!< R     0.0 RSVD */
#define IPC_RSVD5(x)                                ((u32)(((x) & 0x0000FFFF << 16))
#define IPC_GET_RSVD5(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_MASK_IMR_RX0_FULL_MASK_DIS              ((u32)0x000000FF << 8)                   /*!< R/W   0.0 0: Enable RX channel x full interrupt mask of CPU01: Disable RX channel x full interrupt mask of CPU0 */
#define IPC_IMR_RX0_FULL_MASK_DIS(x)                ((u32)(((x) & 0x000000FF << 8))
#define IPC_GET_IMR_RX0_FULL_MASK_DIS(x)            ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_IMR_TX0_EMPTY_MASK_DIS             ((u32)0x000000FF << 0)                   /*!< R/W   0.0 0: Enable TX channel x empty interrupt mask of CPU01: Disable TX channel x empty interrupt mask of CPU0 */
#define IPC_IMR_TX0_EMPTY_MASK_DIS(x)               ((u32)(((x) & 0x000000FF << 0))
#define IPC_GET_IMR_TX0_EMPTY_MASK_DIS(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_IPC_IMR1
 * @brief
 * @{
 */
#define IPC_MASK_RSVD6                              ((u32)0x0000FFFF << 16)                  /*!< R     0.0 RSVD */
#define IPC_RSVD6(x)                                ((u32)(((x) & 0x0000FFFF << 16))
#define IPC_GET_RSVD6(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_MASK_IMR_RX1_FULL_MASK_DIS              ((u32)0x000000FF << 8)                   /*!< R/W   0.0 0: Enable RX channel x full interrupt mask of CPU11: Disable RX channel x full interrupt mask of CPU1 */
#define IPC_IMR_RX1_FULL_MASK_DIS(x)                ((u32)(((x) & 0x000000FF << 8))
#define IPC_GET_IMR_RX1_FULL_MASK_DIS(x)            ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_IMR_TX1_EMPTY_MASK_DIS             ((u32)0x000000FF << 0)                   /*!< R/W   0.0 0: Enable TX channel x empty interrupt mask of CPU11: Disable TX channel x empty interrupt mask of CPU1 */
#define IPC_IMR_TX1_EMPTY_MASK_DIS(x)               ((u32)(((x) & 0x000000FF << 0))
#define IPC_GET_IMR_TX1_EMPTY_MASK_DIS(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_IPC_IMR2
 * @brief
 * @{
 */
#define IPC_MASK_RSVD7                              ((u32)0x0000FFFF << 16)                  /*!< R     0.0 RSVD */
#define IPC_RSVD7(x)                                ((u32)(((x) & 0x0000FFFF << 16))
#define IPC_GET_RSVD7(x)                            ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_MASK_IMR_RX2_FULL_MASK_DIS              ((u32)0x000000FF << 8)                   /*!< R/W   0.0 0: Enable RX channel x full interrupt mask of CPU21: Disable RX channel x full interrupt mask of CPU2 */
#define IPC_IMR_RX2_FULL_MASK_DIS(x)                ((u32)(((x) & 0x000000FF << 8))
#define IPC_GET_IMR_RX2_FULL_MASK_DIS(x)            ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_IMR_TX2_EMPTY_MASK_DIS             ((u32)0x000000FF << 0)                   /*!< R/W   0.0 0: Enable TX channel x empty interrupt mask of CPU21: Disable TX channel x empty interrupt mask of CPU2 */
#define IPC_IMR_TX2_EMPTY_MASK_DIS(x)               ((u32)(((x) & 0x000000FF << 0))
#define IPC_GET_IMR_TX2_EMPTY_MASK_DIS(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/** @defgroup REG_IPC_ICR
 * @brief
 * @{
 */
#define IPC_MASK_RSVD8                              ((u32)0x000000FF << 24)                  /*!< R     0.0 RSVD */
#define IPC_RSVD8(x)                                ((u32)(((x) & 0x000000FF << 24))
#define IPC_GET_RSVD8(x)                            ((u32)(((x >> 24) & 0x000000FF)))
#define IPC_BIT_ICR_TX2_DATA_CLR7                   ((u32)0x00000001 << 23)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 7 */
#define IPC_BIT_ICR_TX2_DATA_CLR6                   ((u32)0x00000001 << 22)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 6 */
#define IPC_BIT_ICR_TX2_DATA_CLR5                   ((u32)0x00000001 << 21)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 5 */
#define IPC_BIT_ICR_TX2_DATA_CLR4                   ((u32)0x00000001 << 20)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 4 */
#define IPC_BIT_ICR_TX2_DATA_CLR3                   ((u32)0x00000001 << 19)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 3 */
#define IPC_BIT_ICR_TX2_DATA_CLR2                   ((u32)0x00000001 << 18)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 2 */
#define IPC_BIT_ICR_TX2_DATA_CLR1                   ((u32)0x00000001 << 17)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 1 */
#define IPC_BIT_ICR_TX2_DATA_CLR0                   ((u32)0x00000001 << 16)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx2_data) channel 0 */
#define IPC_BIT_ICR_TX1_DATA_CLR7                   ((u32)0x00000001 << 15)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 7 */
#define IPC_BIT_ICR_TX1_DATA_CLR6                   ((u32)0x00000001 << 14)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 6 */
#define IPC_BIT_ICR_TX1_DATA_CLR5                   ((u32)0x00000001 << 13)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 5 */
#define IPC_BIT_ICR_TX1_DATA_CLR4                   ((u32)0x00000001 << 12)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 4 */
#define IPC_BIT_ICR_TX1_DATA_CLR3                   ((u32)0x00000001 << 11)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 3 */
#define IPC_BIT_ICR_TX1_DATA_CLR2                   ((u32)0x00000001 << 10)                  /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 2 */
#define IPC_BIT_ICR_TX1_DATA_CLR1                   ((u32)0x00000001 << 9)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 1 */
#define IPC_BIT_ICR_TX1_DATA_CLR0                   ((u32)0x00000001 << 8)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx1_data) channel 0 */
#define IPC_BIT_ICR_TX0_DATA_CLR7                   ((u32)0x00000001 << 7)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 7 */
#define IPC_BIT_ICR_TX0_DATA_CLR6                   ((u32)0x00000001 << 6)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 6 */
#define IPC_BIT_ICR_TX0_DATA_CLR5                   ((u32)0x00000001 << 5)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 5 */
#define IPC_BIT_ICR_TX0_DATA_CLR4                   ((u32)0x00000001 << 4)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 4 */
#define IPC_BIT_ICR_TX0_DATA_CLR3                   ((u32)0x00000001 << 3)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 3 */
#define IPC_BIT_ICR_TX0_DATA_CLR2                   ((u32)0x00000001 << 2)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 2 */
#define IPC_BIT_ICR_TX0_DATA_CLR1                   ((u32)0x00000001 << 1)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 1 */
#define IPC_BIT_ICR_TX0_DATA_CLR0                   ((u32)0x00000001 << 0)                   /*!< R/W1C 0.0 0: no effect1: Clear the tx data register (tx0_data) channel 0 */
/** @} */


/** @defgroup REG_IPC_SEM0
 * @brief
 * @{
 */
#define IPC_MASK_SEMA_HW_STS                        ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 0: The semaphore is free1: The semaphore is occupied */
#define IPC_SEMA_HW_STS(x)                          ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SEMA_HW_STS(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_IPC_SEM_ID_0
 * @brief
 * @{
 */
#define IPC_MASK_SEMA_HW_ID                         ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 The CPUID which current occupy the semaphore */
#define IPC_SEMA_HW_ID(x)                           ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SEMA_HW_ID(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_IPC_SEM_ID_1
 * @brief
 * @{
 */
#define IPC_MASK_SEMA_HW_ID                         ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 The CPUID which current occupy the semaphore */
#define IPC_SEMA_HW_ID(x)                           ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SEMA_HW_ID(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_IPC_SEM_ID_2
 * @brief
 * @{
 */
#define IPC_MASK_SEMA_HW_ID                         ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 The CPUID which current occupy the semaphore */
#define IPC_SEMA_HW_ID(x)                           ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SEMA_HW_ID(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_IPC_SEM_ID_3
 * @brief
 * @{
 */
#define IPC_MASK_SEMA_HW_ID                         ((u32)0xFFFFFFFF << 0)                   /*!< R     0.0 The CPUID which current occupy the semaphore */
#define IPC_SEMA_HW_ID(x)                           ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SEMA_HW_ID(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_IPC_SPARE0
 * @brief
 * @{
 */
#define IPC_MASK_SPARE0                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 nan */
#define IPC_SPARE0(x)                               ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SPARE0(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */


/** @defgroup REG_IPC_SPARE1
 * @brief
 * @{
 */
#define IPC_MASK_SPARE1                             ((u32)0xFFFFFFFF << 0)                   /*!< R/W   0.0 nan */
#define IPC_SPARE1(x)                               ((u32)(((x) & 0xFFFFFFFF << 0))
#define IPC_GET_SPARE1(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */



/*==========IPC Register Address Definition==========*/
#define REG_IPC_TX_DATA                             0x0000
#define REG_IPC_RX_DATA                             0x0004
#define REG_IPC_ISR0_STS                            0x0008
#define REG_IPC_ISR1_STS                            0x000C
#define REG_IPC_ISR2_STS                            0x0010
#define REG_IPC_IMR0                                0x0014
#define REG_IPC_IMR1                                0x0018
#define REG_IPC_IMR2                                0x001C
#define REG_IPC_ICR                                 0x0020
#define REG_IPC_SEM0                                0x0024
#define REG_IPC_SEM_ID_0                            0x0028
#define REG_IPC_SEM_ID_1                            0x002C
#define REG_IPC_SEM_ID_2                            0x0030
#define REG_IPC_SEM_ID_3                            0x0034
#define REG_IPC_SPARE0                              0x0038
#define REG_IPC_SPARE1                              0x003C




/** @defgroup IPC_Exported_Types IPC Exported Types
* @{
*/

/**
* @brief IPC Register Declaration
*/
typedef struct {
	__IO uint32_t IPC_TX_DATA;                            			/*!< Register,  Address offset:0x0000 */
	__IO uint32_t IPC_RX_DATA;                            			/*!< Register,  Address offset:0x0004 */
	__IO uint32_t IPC_ISR[3];                             			/*!< Register,  Address offset:0x0008 ~ 0x0014*/
	__IO uint32_t IPC_IMR[3];                             			/*!< Register,  Address offset:0x0014 ~ 0x0020*/
	__IO uint32_t IPC_ICR;                                			/*!< Register,  Address offset:0x0020 */
	__IO uint32_t IPC_SEMSTS;                             			/*!< Register,  Address offset:0x0024 */
	__IO uint32_t IPC_SEMx[4];										/*!< Register,  Address offset:0x0024~ 0x0038*/
	__IO uint32_t IPC_SPARE[2];                              		/*!< Register,  Address offset:0x0038~ 0x003c */
} IPC_TypeDef;

typedef struct {
	__IO uint32_t IPC_SEMA_INDEX[32];								/*!< Register,  Address offset:0x0000~ 0x0080 */
} IPC_SEMA_TypeDef;

#define IPC_BASE ((IPC_TypeDef *) (0x40211000))
/** @} */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/**
  * @brief IPC IRQ Function Definition
  */
typedef void (*IPC_IRQ_FUN)(void *Data, u32 IrqStatus, u32 ChanNum);

/**
  * @brief IPC User Message Type Definition
  */
typedef enum {
	IPC_USER_POINT = 0,
	IPC_USER_DATA	= 1
} USER_MSG_TYP_DEF;

/**
  * @brief IPC Init Table Definition
  */
typedef struct _IPC_INIT_TABLE_ {
	u32 USER_MSG_TYPE;
	void (*Rxfunc)(void *Data, u32 IrqStatus, u32 ChanNum);
	void *RxIrqData;
	void (*Txfunc)(void *Data, u32 IrqStatus, u32 ChanNum);
	void *TxIrqData;
	u32 IPC_Direction;	/* direction of ipc, this parameter is from @IPC_Direction_Mode*/
	u32 IPC_Channel;	/* ipc channel */
} IPC_INIT_TABLE, *PIPC_INIT_TABLE;

/**
  * @brief IPC SEM IDX
  */
typedef enum {
	IPC_SEM_ID0 = 0,
	IPC_SEM_ID1,
	IPC_SEM_ID2,
	IPC_SEM_ID3,
	IPC_SEM_ID4,
	IPC_SEM_ID5,
	IPC_SEM_ID6,
	IPC_SEM_ID7,
	IPC_SEM_ID8,
	IPC_SEM_ID9,
	IPC_SEM_ID10,
	IPC_SEM_ID11,
	IPC_SEM_ID12,
	IPC_SEM_ID13,
	IPC_SEM_ID14,
	IPC_SEM_ID15,
	IPC_SEM_ID16,
	IPC_SEM_ID17,
	IPC_SEM_ID18,
	IPC_SEM_ID19,
	IPC_SEM_ID20,
	IPC_SEM_ID21,
	IPC_SEM_ID22,
	IPC_SEM_ID23,
	IPC_SEM_ID24,
	IPC_SEM_ID25,
	IPC_SEM_ID26,
	IPC_SEM_ID27,
	IPC_SEM_ID28,
	IPC_SEM_ID29,
	IPC_SEM_ID30,
	IPC_SEM_ID31,
	IPC_SEM_MAX,			/* can't be this value, total 4 ipc semaphores*/
} IPC_SEM_IDX;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_Peripheral_Definition
  * @{
  */
#define IS_IPC_ALL_PERIPH(PERIPH) (((PERIPH) == IPCNP_DEV) || \
										((PERIPH) == IPCVP_DEV)) || \
										((PERIPH) == IPCAP_DEV)) || \
										((PERIPH) == IPCAP_DEV))
/**
  * @}
  */

/** @defgroup IPC_Direction_Mode
  * @{
  */
#define IPC_NP_TO_FP			((u32)0x00000000)
#define IPC_NP_TO_AP			((u32)0x00000001)
#define IPC_NP_TO_VP			((u32)0x00000002)
#define IPC_VP_TO_NP			((u32)0x00000003)
#define IPC_VP_TO_AP			((u32)0x00000004)
#define IPC_VP_TO_FP			((u32)0x00000005)
#define IPC_AP_TO_NP			((u32)0x00000006)
#define IPC_AP_TO_VP			((u32)0x00000007)
#define IPC_AP_TO_FP			((u32)0x00000008)
#define IPC_FP_TO_NP			((u32)0x00000009)
#define IPC_FP_TO_VP			((u32)0x0000000A)
#define IPC_FP_TO_AP			((u32)0x0000000B)

#define IS_IPC_DIR_MODE(MODE) (((MODE) == IPC_NP_TO_VP) || \
									((MODE) == IPC_NP_TO_AP) || \
									((MODE) == IPC_NP_TO_FP) || \
									((MODE) == IPC_VP_TO_NP) || \
									((MODE) == IPC_VP_TO_AP) || \
									((MODE) == IPC_VP_TO_FP) || \
									((MODE) == IPC_AP_TO_NP) || \
									((MODE) == IPC_AP_TO_VP) || \
									((MODE) == IPC_AP_TO_FP) || \
									((MODE) == IPC_FP_TO_NP) || \
									((MODE) == IPC_FP_TO_VP) || \
									((MODE) == IPC_FP_TO_AP))

#define IS_SEND_TO_NP(MODE)		(((MODE) == IPC_VP_TO_NP) || \
								(((MODE) == IPC_AP_TO_NP) || \
								((MODE) == IPC_FP_TO_NP)))

#define IS_SEND_TO_VP(MODE)		(((MODE) == IPC_NP_TO_VP) || \
								(((MODE) == IPC_AP_TO_VP) || \
								((MODE) == IPC_FP_TO_VP)))

#define IS_SEND_TO_AP(MODE)		(((MODE) == IPC_NP_TO_AP) || \
								(((MODE) == IPC_VP_TO_AP) || \
								((MODE) == IPC_FP_TO_AP)))

#define IS_SEND_TO_FP(MODE)		(((MODE) == IPC_NP_TO_FP) || \
								(((MODE) == IPC_VP_TO_FP) || \
								((MODE) == IPC_AP_TO_FP)))
/**
  * @}
  */

/** @defgroup IPC_INTR_Mode
  * @{
  */
#define IPC_TX_EMPTY			((u32)0x00000001)
#define IPC_RX_FULL 			((u32)0x00000002)
#define IS_IPC_INTR_MODE(MODE) (((MODE) == IPC_TX_EMPTY) || \
                                   ((MODE) == IPC_RX_FULL))
/**
  * @}
  */

/** @defgroup IPC_Valid_PXID
  * @{
  */
#define IS_IPC_VALID_PXID(PXID) ((PXID) >= 0)
#define IPC_MAX_VALID_PXID		(255)
/**
  * @}
  */

/** @defgroup IPC_Valid_CPUID
  * @{
  */

#define IS_IPC_Valid_CPUID(cpuid)		((cpuid)<=3)

/**
  * @}
  */


/** @defgroup IPC_CHANNEL
  * @{
  */
#define IPC_TX_CHANNEL_NUM						48//24
#define IPC_TX_CHANNEL_SWITCH(x)				((u32)(((x >> 4) & 0x0000000F)))
#define IPC_TX0_CHANNEL_NUM						8
#define IPC_TX0_CHANNEL_SWITCH(x)				((u32)((x) & 0x0000000F))
#define IS_IPC_RX_CHNUM(NUM) 					((NUM) >= 0x08)
#define IPC_CHANNEL_NUM 						96//48

/**
  * @}
  */

/** @defgroup IPC_Valid_CHNUM
  * @{
  */
#define IS_IPC_VALID_CHNUM(NUM) ((NUM) < 16)
/**
  * @}
  */

/** @defgroup IPC_SEM_PXID
  * @{
  */
#define NP_IPC_PXID 0x00
#define VP_IPC_PXID 0x0A
#define FP_IPC_PXID 0x03
#define AP_IPC_PXID 0x09


#if defined(ARM_CORE_CM4_NP)
#define IPC_FLASH_LOCK  NP_IPC_PXID
#elif defined(ARM_CORE_CM4_VP)
#define IPC_FLASH_LOCK  VP_IPC_PXID
#elif defined(ARM_CORE_CA32)
#define IPC_FLASH_LOCK  AP_IPC_PXID
#else
#define IPC_FLASH_LOCK  F7_IPC_PXID
#endif

/**
  * @}
  */

/**
  * @}
  */

/** @} */
/** @} */

void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u8 IPC_Shiftbit, u32 NewState);
void IPC_IERSet(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u32 IPC_Chs);
u32 IPC_IERGet(IPC_TypeDef *IPCx, u8 IPC_GroupShift);
u32 IPC_INTRequest(IPC_TypeDef *IPCx, u32 IPC_Dir, u8 IPC_ChNum);
u32 IPC_INTGet(IPC_TypeDef *IPCx, u8 IPC_GroupShift);
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u8 IPC_Shiftbit);
u32 IPC_INTHandler(void *Data);
void IPC_INTUserHandler(IPC_TypeDef *IPCx, u8 IPC_GroupShift, u8 IPC_Shiftbit, void *IrqHandler, void *IrqData);
u32 IPC_SEMTake(IPC_SEM_IDX SEM_Idx, u32 timeout);
u32 IPC_SEMFree(IPC_SEM_IDX SEM_Idx);
void IPC_SEMDelayStub(void (*pfunc)(uint32_t));
IPC_TypeDef *IPC_GetDevById(u32 cpu_id);

/* Other definitions --------------------------------------------------------*/
extern const IPC_INIT_TABLE  ipc_init_config[];
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */
#endif  // AMEBA_IPC_H
