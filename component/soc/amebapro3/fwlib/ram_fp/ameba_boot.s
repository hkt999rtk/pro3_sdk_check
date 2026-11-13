/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
 

	.global Reset_Handler
	.global NMI_Handler
	.global HardFault_Handler

		
	.syntax unified				/* for UAL syntax */

	.section .isr_vector , "ax"
vectors_table:	
    .long    __StackTop         /* Initial Top of Stack */
    .long    Reset_Handler      /* Reset Handler */
	.long    NMI_Handler
	.long    HardFault_Handler
  

.text
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 8 
	.align 2
	.globl    Reset_Handler
	.type    Reset_Handler, function
Reset_Handler:  

    ldr     R0, = startup
    bx      R0
         
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    NMI_Handler
	.type    NMI_Handler, function
NMI_Handler:
    ldr     r0, = INT_NMI
    bx      r0
	
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    HardFault_Handler
	.type    HardFault_Handler, function
HardFault_Handler:
    ldr     r0, = INT_HardFault
    bx      r0
	
.end
