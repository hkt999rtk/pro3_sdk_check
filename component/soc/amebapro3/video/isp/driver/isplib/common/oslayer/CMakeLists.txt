if(${TARGET_OS} STREQUAL Linux)
    set(source
        linux/isp_os_errno.c
        linux/isp_os_mem.c
        linux/isp_os_time.c
    )
elseif(${TARGET_OS} STREQUAL FreeRTOS)
    set(source
        freertos/isp_os_errno.c
        freertos/isp_os_mem.c
        freertos/isp_os_thread.c
        freertos/isp_os_time.c
    )
else()
    message(FATAL_ERROR "Only support Linux and FreeRTOS")
endif()

set(target isp_common_oslayer)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE kernel)

set_property(GLOBAL APPEND PROPERTY isp_common_objs $<TARGET_OBJECTS:${target}>)
