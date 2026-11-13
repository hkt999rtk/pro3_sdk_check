option(ISP_CPU_MODE "ISP is used by CPU" OFF)

if(${TARGET_OS} STREQUAL FreeRTOS)
    set(source
        freertos/isp_os_driver.c
        freertos/isp_os_io.c
    )

    if(ISP_CPU_MODE)
        set(source ${source}
            freertos/isp_os_log_cpu.c
        )
    else()
        set(source ${source}
            freertos/isp_os_log.c
        )
    endif()
else()
    message(FATAL_ERROR "Only supportFreeRTOS")
endif()

set(target isp_server_oslayer)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE isp_server_interface)
target_link_libraries(${target} PRIVATE drivers_interface)

set_property(GLOBAL APPEND PROPERTY isp_server_objs $<TARGET_OBJECTS:${target}>)
