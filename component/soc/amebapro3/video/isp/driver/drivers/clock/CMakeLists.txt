set(source
    clock.c
)

set(target driver_clock)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE drivers_common_interface)

set_property(GLOBAL APPEND PROPERTY driver_clock_objs $<TARGET_OBJECTS:${target}>)
