
/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

/**
 * @file asm_vectors.s
 *
 * This file contains the initial vector table for the Cortex A35 processor
 *
 */

	.org 0
	.text

	.globl	_vector_table
	
	.extern vHandleMemoryFault

	.section .vectors
_vector_table:
	B	_boot
	B	FreeRTOS_UndefinedHandler
	B	FreeRTOS_SWI_Handler
	B	FreeRTOS_PrefetchAbortHandler
	B	FreeRTOS_DataAbortHandler
	B	.
	B	FreeRTOS_IRQ_Handler
	B	FreeRTOS_FIQHandler


	.align	4
FreeRTOS_FIQHandler:				@ FIQ vector handler */
	STMDB	SP!, {R0-R3, R12, LR}	@ state save from compiled code

	BL	FIQHandler					@ FIQ vector

	LDMIA	SP!, {R0-R3, R12, LR}	@ state restore from compiled code
	SUBS	PC, LR, #4				@ adjust return


#if 0
	.align	4
FreeRTOS_UndefinedHandler:			@ Undefined handler
	STMDB	SP!, {R0-R3, R12, LR}	@ state save from compiled code
	LDR		R0, =UndefinedExceptionAddr
	SUB		R1, LR, #4
	STR		R1, [R0]            	@ store address of instruction causing exception

	BL	UndefinedExceptionHandler	@ call C function here

	LDMIA	SP!, {R0-R3, R12, LR}	@ state restore from compiled code
	MOVS	PC, LR

	.align	4
FreeRTOS_DataAbortHandler:			@ Data Abort handler
	STMDB	SP!, {R0-R3, R12, LR}	@ state save from compiled code
	LDR		R0, =DataAbortAddr
	SUB		R1, LR, #8
	STR		R1, [R0]            	@ stores instruction causing data abort

	bl		DataAbortHandler		@ call C function

	LDMIA	SP!, {R0-R3, R12, LR}	@ state restore from compiled code
	SUBS	PC, LR, #8				@ points to the instruction that caused the exception

	.align	4
FreeRTOS_PrefetchAbortHandler:		@ Prefetch Abort handler
	STMDB	SP!, {R0-R3, R12, LR}	@ state save from compiled code
	LDR		R0, =PrefetchAbortAddr
	SUB		R1, LR, #4
	STR		R1, [R0]	 			@ stores instruction causing prefetch abort

	BL		PrefetchAbortHandler	@ call C function

	LDMIA	SP!, {R0-R3, R12, LR}	@ state restore from compiled code
	SUBS	PC, LR, #4				@ points to the instruction that caused the exception
#else
	.align	4
FreeRTOS_UndefinedHandler:			@ Undefined handler
    STMFD   SP!, {R0-R12, LR}       @ LR usually points to next instruction, adjust based on architecture if needed
    MRS     R0, SPSR
    STMFD   SP!, {R0}

    CPS 	#0x1F           		@ switch to System Mode (share with User Mode)
    MOV 	R0, SP          
    MOV 	R1, LR          
    CPS 	#0x17           		@ switch to Abort Mode (0x17)
	STMFD   SP!, {R1}				@ put user LR to frame
	STMFD   SP!, {R0}				@ put user SP to frame
	
    MOV     R0, SP
    MOV     R1, #3                  @ R1 = Fault Type (3 = Undefined Instruction) 
    B       Fault_Common_Entry

	LDMIA   SP!, {R0-R2}			@ state restore
	LDMIA	SP!, {R0-R12, LR}		@ state restore from compiled code
	MOVS	PC, LR

	.align	4
FreeRTOS_DataAbortHandler:			@ Data Abort handler
    SUB     LR, LR, #8              @ Adjust LR to point to the instruction that caused the Abort 
    STMFD   SP!, {R0-R12, LR}       @ Save general registers R0-R12 and LR (Original PC) to stack 
    MRS     R0, SPSR                @ Read SPSR (CPSR before the exception) 
    STMFD   SP!, {R0}               @ Save SPSR 

    CPS 	#0x1F           		@ switch to System Mode (share with User Mode)
    MOV 	R0, SP          
    MOV 	R1, LR          
    CPS 	#0x17           		@ switch to Abort Mode (0x17)
	STMFD   SP!, {R1}				@ put user LR to frame
	STMFD   SP!, {R0}				@ put user SP to frame	
	
    MOV     R0, SP                  @ R0 = Pointer to the stack top (Passed as argument to C function) 
    MOV     R1, #1                  @ R1 = Fault Type (1 = Data Abort) 
    B       Fault_Common_Entry

	LDMIA   SP!, {R0-R2}			@ state restore
	LDMIA	SP!, {R0-R12, LR}		@ state restore from compiled code
	SUBS	PC, LR, #8				@ points to the instruction that caused the exception

	.align	4
FreeRTOS_PrefetchAbortHandler:		@ Prefetch Abort handler
    SUB     LR, LR, #4              @ Adjust LR 
    STMFD   SP!, {R0-R12, LR}
    MRS     R0, SPSR
    STMFD   SP!, {R0}
	
    CPS 	#0x1F           		@ switch to System Mode (share with User Mode)
    MOV 	R0, SP          
    MOV 	R1, LR          
    CPS 	#0x17           		@ switch to Abort Mode (0x17)
	STMFD   SP!, {R1}				@ put user LR to frame
	STMFD   SP!, {R0}				@ put user SP to frame	
	
    MOV     R0, SP
    MOV     R1, #2                  @ R1 = Fault Type (2 = Prefetch Abort) 
    B       Fault_Common_Entry

	LDMIA   SP!, {R0-R2}			@ state restore
	LDMIA	SP!, {R0-R12, LR}		@ state restore from compiled code
	SUBS	PC, LR, #4				@ points to the instruction that caused the exception

	.align	4
Fault_Common_Entry:
    /* 
     * At this point, R0 points to the Stack Frame structure:
     * [SPSR, R0, R1, R2, ..., R12, LR(PC)] 
     * Call the C function to print details
     */
    BL      vHandleMemoryFault
    
    /* Enter infinite loop to prevent erratic behavior */
Hang_Loop:
    B       Hang_Loop	
#endif
.end
