set(source
    rts_isp.c
    rts_isp_patch.c
    rts_isp_sensor.c
    rts_isp_algo.c
)
set(target isp_server_api)
add_library(${target} OBJECT ${source})
target_include_directories(${target} PRIVATE ${ispserverdir}/core)
target_include_directories(${target} PRIVATE ${ispserverdir}/modules)
target_include_directories(${target} PRIVATE ${isptopdir}/common/core)
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE isp_server_interface)
target_link_libraries(${target} PRIVATE kernel)

set_property(GLOBAL APPEND PROPERTY isp_server_objs $<TARGET_OBJECTS:${target}>)
