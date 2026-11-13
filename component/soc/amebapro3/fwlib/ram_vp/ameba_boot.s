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
	.global MemManage_Handler
	.global BusFault_Handler
	.global UsageFault_Handler
	.global SecureFault_Handler
	.global SVC_ROM_Handler
	.global PendSV_ROM_Handler
	.global SYSTick_ROM_Handler
		
	.syntax unified				/* for UAL syntax */

	.section .isr_vector , "ax"
vectors_table:	
    .long    __StackTop         /* Initial Top of Stack */
    .long    Reset_Handler      /* Reset Handler */
	.long    NMI_Handler
	.long    HardFault_Handler
	.long    MemManage_Handler
	.long    BusFault_Handler
	.long    UsageFault_Handler
	.long    SecureFault_Handler	
	.long    HardFault_Handler
	.long    HardFault_Handler
	.long    HardFault_Handler
	.long    SVC_ROM_Handler
	.long    HardFault_Handler
	.long    HardFault_Handler
	.long    PendSV_ROM_Handler
	.long    SYSTick_ROM_Handler
   

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
	/* The FPU enable bits are in the CPACR. */
	ldr.w r0, =0xE000ED88
	ldr	r1, [r0]

	/* Enable CP10 and CP11 coprocessors, then save back. */
	orr	r1, r1, ( 0xf << 20 ) 
	str r1, [r0]

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
	
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    MemManage_Handler
	.type    MemManage_Handler, function
MemManage_Handler: 
    ldr     r0, = INT_MemManage
    bx      r0
	
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    BusFault_Handler
	.type    BusFault_Handler, function
BusFault_Handler: 
    ldr     r0, = INT_BusFault
    bx      r0
	
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    UsageFault_Handler
	.type    UsageFault_Handler, function
UsageFault_Handler:
    ldr     r0, = INT_UsageFault
    bx      r0
	
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    SecureFault_Handler
	.type    SecureFault_Handler, function
SecureFault_Handler:
    ldr     r0, = INT_SecureFault
    bx      r0
	

	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    SVC_ROM_Handler
	.type    SVC_ROM_Handler, function
SVC_ROM_Handler:
    bx      lr
	
	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    PendSV_ROM_Handler
	.type    PendSV_ROM_Handler, function
PendSV_ROM_Handler:
    bx      lr		

	.text
	.thumb
	.thumb_func
	.section .vectors_table_text , "ax"
	.align 2
	.globl    SYSTick_ROM_Handler
	.type    SYSTick_ROM_Handler, function
SYSTick_ROM_Handler:
    bx      lr
	
.end
