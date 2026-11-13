set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -fno-common")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

file(GLOB link_script main.ld)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -static -T${link_script}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections,-z,max-page-size=0x1000")

set(source
    libc_stub.c
    printf.c
    start.c
    uart.c
    mbed_printf_implementation.c
    task_mcu.c
    main.c
)

if(ISP_CPU_MODE)
    set(source ${source}
        task_cpu.c
        isp_rtos_stats.c
    )
endif()

set(target apps)
add_executable(${target} ${source})

target_link_libraries(
    ${target}
    rts_algo_ae
    rts_algo_awb
    rts_algo_af
    rts_algo_other
    sensor_sc401ai_fpga
    -Wl,--start-group
    isp
    drivers
    -Wl,--end-group
    isp_common_interface
    drivers_interface
    kernel
    m
)

set_target_properties(${target} PROPERTIES LINK_DEPENDS ${link_script})

if(ISP_CPU_MODE)
    target_compile_definitions(${target} PRIVATE ISP_CPU_MODE)
endif()

add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
    ${target} isp.fw)

execute_process(
    COMMAND stat -c %s ${ISP_MCU_STAGING_DIR}/lib/default.bin
    OUTPUT_VARIABLE iq_size
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(${iq_size} GREATER 0x40000)
    message(FATAL_ERROR "The iq table size is out of range 256K!")
endif()

add_custom_command(TARGET ${target}
    COMMAND dd bs=1 if=${ISP_MCU_STAGING_DIR}/lib/default.bin
    of=isp.fw skip=0 seek=4096 conv=notrunc)
install(FILES ${CMAKE_BINARY_DIR}/apps/isp.fw DESTINATION ${ISP_MCU_STAGING_DIR}/lib/firmware
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
