set(source
    isp_utils.c
)
set(target isp_server_common)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE isp_server_interface)

set_property(GLOBAL APPEND PROPERTY isp_server_objs $<TARGET_OBJECTS:${target}>)
