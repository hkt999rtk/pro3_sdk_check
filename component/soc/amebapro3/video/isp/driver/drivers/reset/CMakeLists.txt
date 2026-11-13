set(source
    reset.c
)

set(target driver_reset)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE drivers_common_interface)

set_property(GLOBAL APPEND PROPERTY driver_reset_objs $<TARGET_OBJECTS:${target}>)
