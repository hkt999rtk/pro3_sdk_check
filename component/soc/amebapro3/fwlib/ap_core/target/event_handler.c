/*
 * event_handler.c
 */
#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// Define register structure, matching the push order in Assembly
typedef struct {
	uint32_t usr_sp;
	uint32_t usr_lr;
	uint32_t spsr;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t pc; // This is the LR pushed in Assembly, effectively the corrected PC
} FaultStackFrame;

// Fault Type Definitions
#define FAULT_TYPE_DATA_ABORT       1
#define FAULT_TYPE_PREFETCH_ABORT   2
#define FAULT_TYPE_UNDEF_INSTR      3

// CP15 Register Reading Helpers (GCC Inline Assembly)
static inline uint32_t read_dfsr(void)
{
	uint32_t val;
	__asm volatile("MRC p15, 0, %0, c5, c0, 0" : "=r"(val));
	return val;
}

static inline uint32_t read_dfar(void)
{
	uint32_t val;
	__asm volatile("MRC p15, 0, %0, c6, c0, 0" : "=r"(val));
	return val;
}

static inline uint32_t read_ifsr(void)
{
	uint32_t val;
	__asm volatile("MRC p15, 0, %0, c5, c0, 1" : "=r"(val));
	return val;
}

static inline uint32_t read_ifar(void)
{
	uint32_t val;
	__asm volatile("MRC p15, 0, %0, c6, c0, 2" : "=r"(val));
	return val;
}

// Simple Stack Dump Function
void dump_stack_memory(uint32_t *stackPtr, int count)
{
	printf("\r\n Stack Dump (Current SP: 0x%08lX):\r\n", (uint32_t)stackPtr);
	for (int i = 0; i < count; i += 4) {
		printf("0x%08lX: %08lX %08lX %08lX %08lX\r\n",
			   (uint32_t)(stackPtr + i),
			   stackPtr[i], stackPtr[i + 1], stackPtr[i + 2], stackPtr[i + 3]);
	}
}


extern uint8_t __text_start__[];
extern uint8_t __text_end__[];
#define CODE_REGION_START  (uint32_t)__text_start__
#define CODE_REGION_END    (uint32_t)__text_end__

// Set maximum backtrace depth to prevent infinite loops
#define MAX_BACKTRACE_DEPTH 20

/**
 * @brief Check if an address is a valid instruction address (PC/LR)
 */
static int is_valid_code_address(uint32_t addr)
{
	// 1. Must be within the code region
	if (addr < CODE_REGION_START || addr > CODE_REGION_END) {
		return 0;
	}

	// 2. In ARMv8 AArch32, instructions must be 2-byte (Thumb) or 4-byte (ARM) aligned.
	// Usually, if a return address (LR) is in Thumb mode, the lowest bit (bit 0) will be 1.
	// Here, we simply filter out completely unaligned weird addresses.
	// (You can add stricter alignment checks if needed)
	return 1;
}

/**
 * @brief Execute User Domain Backtrace
 * @param sp The Stack Pointer of the currently crashed task
 * @param lr The Link Register of the currently crashed task (may contain the caller's return address)
 * @param pc The Program Counter where the crash occurred
 */
void dump_user_backtrace(uint32_t sp, uint32_t lr, uint32_t pc)
{
	uint32_t *stack_ptr = (uint32_t *)sp;
	int depth = 0;

	printf("\r\n🔍 --- User Domain Call Stack Backtrace --- 🔍\r\n");

	// 1. Print the current crash point (Level 0)
	printf("  [bt] #%d: 0x%08lX (Crash PC)\r\n", depth++, pc);

	// 2. Check if LR is valid (it might be the immediate caller)
	if (is_valid_code_address(lr)) {
		// Usually, LR points to the return address. The actual calling instruction is at LR - 4 or LR - 2.
		printf("  [bt] #%d: 0x%08lX (Saved LR)\r\n", depth++, lr);
	}

	// 3. Scan the stack (Stack Scan)
	// Note: Assuming a Full Descending stack, we scan towards higher addresses.
	// In practice, you can use FreeRTOS's pxTopOfStack and stack sizes to get task stack boundaries to prevent out-of-bounds access.

	printf("  Scanning stack memory starting from SP: 0x%08lX...\r\n", sp);

	// Scan the next 256 words (1024 bytes) of stack space
	for (int i = 0; i < 256; i++) {
		uint32_t val = stack_ptr[i];

		// If this value looks like an address in the code section
		if (is_valid_code_address(val)) {

			// In Thumb mode, addresses might have bit 0 = 1. We can optionally clear bit 0 for printing.
			uint32_t clean_addr = val & ~0x01;

			printf("  [bt] #%d: 0x%08lX (Found on stack at SP+0x%03X)\r\n",
				   depth++, clean_addr, i * 4);

			if (depth >= MAX_BACKTRACE_DEPTH) {
				printf("  [bt] Reached max backtrace depth.\r\n");
				break;
			}
		}
	}
	printf("--------------------------------------------\r\n");
}

/**
 * @brief Core Fault Handling Function
 * @param frame Pointer to registers saved on stack
 * @param faultType Fault Type ID
 */
void vHandleMemoryFault(FaultStackFrame *frame, uint32_t faultType)
{
	uint32_t dfsr = 0, dfar = 0, ifsr = 0, ifar = 0;

	// Disable interrupts to ensure output integrity
	taskDISABLE_INTERRUPTS();

	printf("\r\n=======================================================\r\n");
	printf(" FATAL SYSTEM FAULT DETECTED \r\n");
	printf("=======================================================\r\n");

	// 1. Identify fault type and read corresponding CP15 registers
	switch (faultType) {
	case FAULT_TYPE_DATA_ABORT:
		dfsr = read_dfsr();
		dfar = read_dfar();
		printf(" Exception: DATA ABORT\r\n");
		printf(" Fault Status Register (DFSR): 0x%08lX\r\n", dfsr);
		printf(" Fault Address Register (DFAR): 0x%08lX (Accessing this address caused fault)\r\n", dfar);
		break;

	case FAULT_TYPE_PREFETCH_ABORT:
		ifsr = read_ifsr();
		ifar = read_ifar(); // IFAR only available on some arch, usually PC is enough
		printf(" Exception: PREFETCH ABORT (Instruction Fetch Error)\r\n");
		printf(" Fault Status Register (IFSR): 0x%08lX\r\n", ifsr);
		printf(" Fault Address Register (IFAR): 0x%08lX\r\n", ifar);
		printf(" Fault Address: 0x%08lX\r\n", frame->pc);
		break;

	case FAULT_TYPE_UNDEF_INSTR:
		printf(" Exception: UNDEFINED INSTRUCTION\r\n");
		printf(" At Address: 0x%08lX\r\n", frame->pc);
		break;

	default:
		printf(" Exception: UNKNOWN FAULT (%lu)\r\n", faultType);
		break;
	}

	// 2. Print Core Register Context
	printf("\r\n Register Context:\r\n");
	printf("   R0  : 0x%08lX   R1  : 0x%08lX   R2  : 0x%08lX   R3  : 0x%08lX\r\n",
		   frame->r0, frame->r1, frame->r2, frame->r3);
	printf("   R4  : 0x%08lX   R5  : 0x%08lX   R6  : 0x%08lX   R7  : 0x%08lX\r\n",
		   frame->r4, frame->r5, frame->r6, frame->r7);
	printf("   R8  : 0x%08lX   R9  : 0x%08lX   R10 : 0x%08lX   R11 : 0x%08lX\r\n",
		   frame->r8, frame->r9, frame->r10, frame->r11);
	printf("   R12 : 0x%08lX   PC  : 0x%08lX   SPSR: 0x%08lX\r\n",
		   frame->r12, frame->pc, frame->spsr);

	// 3. Print Current Task Info (FreeRTOS Specific)
	// Check if Scheduler has started, otherwise accessing CurrentTCB might crash
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		const char *taskName = pcTaskGetName(NULL); // Get current task name
		TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
		printf("\r\n Current Task: %s (Handle: 0x%08lX)\r\n", taskName ? taskName : "None", (uint32_t)taskHandle);
	} else {
		printf("\r\n Current Task: [Scheduler Not Started]\r\n");
	}

	// 4. Parse DFSR (For Data Abort) - Based on ARMv8/v7 Reference Manual
	if (faultType == FAULT_TYPE_DATA_ABORT) {
		uint32_t status = ((dfsr & 0x400) >> 6) | (dfsr & 0xF);
		printf("\r\n DFSR Analysis (Status: 0x%lX):\r\n", status);
		switch (status) {
		case 0x01:
			printf("   -> Alignment fault\r\n");
			break;
		case 0x05:
			printf("   -> Translation fault (Section)\r\n");
			break;
		case 0x07:
			printf("   -> Translation fault (Page)\r\n");
			break;
		case 0x0D:
			printf("   -> Permission fault (Section) - MPU/MMU Blocked\r\n");
			break;
		case 0x0F:
			printf("   -> Permission fault (Page) - MPU/MMU Blocked\r\n");
			break;
		case 0x08:
			printf("   -> Synchronous External Abort\r\n");
			break;
		case 0x16:
			printf("   -> Asynchronous External Abort\r\n");
			break;
		default:
			printf("   -> See ARM Architecture Reference Manual\r\n");
			break;
		}
	}

	// 5. Stack Dump (Raw stack content)
	// Note: The frame pointer itself is on the stack, so we print from there onwards
	dump_stack_memory((uint32_t *)frame->usr_sp, 32);

	// 6. Backtrace
	dump_user_backtrace(frame->usr_sp, frame->usr_lr, frame->pc);

	printf("=======================================================\r\n");

	// Infinite loop, usually triggers Watchdog Reset
	while (1);
}
