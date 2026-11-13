set(source
    gpio.c
)

set(target driver_gpio)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE drivers_common_interface)

set_property(GLOBAL APPEND PROPERTY driver_gpio_objs $<TARGET_OBJECTS:${target}>)
