set(source
    isp_rtos_atomic.c
    isp_rtos_completion.c
    isp_rtos_dev.c
    isp_rtos_mutex.c
    isp_rtos_spinlock.c
    isp_rtos_wait_queue.c
)

set(target driver_common)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE drivers_common_interface)
target_link_libraries(${target} PRIVATE kernel)

set_property(GLOBAL APPEND PROPERTY driver_common_objs $<TARGET_OBJECTS:${target}>)
