set(source
    pinctrl.c
)

set(target driver_pinctrl)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE drivers_common_interface)

set_property(GLOBAL APPEND PROPERTY driver_pinctrl_objs $<TARGET_OBJECTS:${target}>)
