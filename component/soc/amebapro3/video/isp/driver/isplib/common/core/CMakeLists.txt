set(source
    isp_msg_buf_common.c
)
set(target isp_common_core)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE isp_common_interface)

set_property(GLOBAL APPEND PROPERTY isp_common_objs $<TARGET_OBJECTS:${target}>)
