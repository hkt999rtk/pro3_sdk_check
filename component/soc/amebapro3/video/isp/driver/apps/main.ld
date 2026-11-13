MEMORY
{
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 1024K
}

ENTRY(ResetISR)

SECTIONS
{
    .text :
    {
        __vectors_start__ = .;
        KEEP(*(.isr_vector))
        *(.after_vectors*)
        . = ALIGN(32);
        . = 0x1000;
        __isp_iq_table_start__ = .;
        . += 256K;
        __isp_iq_table_end__ = . - 1;
        __privileged_functions_start__ = .;
        *(privileged_functions)
        . = ALIGN(32);
        __privileged_functions_end__ = . - 1;
        __syscalls_flash_start__ = .;
        *(freertos_system_calls)
        . = ALIGN(32);
        __syscalls_flash_end__ = . - 1;
        __unprivileged_flash_start__ = .;
        *(.text*)
    } > RAM
    . = ALIGN(4);
    .rodata :
    {
        *(.rodata*)
        . = ALIGN(32);
        __unprivileged_flash_end__ = . - 1;
    } > RAM
    . = ALIGN(4);
    .data :
    {
        . = ALIGN(32);
        __privileged_sram_start__ = .;
        *(privileged_data)
        . = ALIGN(32);
        __privileged_sram_end__ = . - 1;
        *(.data*)
    } > RAM
    . = ALIGN(4);
    .bss (NOLOAD) :
    {
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > RAM
    . = ALIGN(16);
    __StackLimit = .;
    __StackSize = 4K;
    __StackTop = __StackLimit + __StackSize;
    ASSERT(__StackTop <= ORIGIN(RAM) + LENGTH(RAM), "main stack overlap")
}
