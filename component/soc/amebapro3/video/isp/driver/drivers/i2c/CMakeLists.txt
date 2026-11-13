set(source
    i2c.c
)

set(target driver_i2c)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE drivers_common_interface)
target_link_libraries(${target} PRIVATE kernel)

set_property(GLOBAL APPEND PROPERTY driver_i2c_objs $<TARGET_OBJECTS:${target}>)
