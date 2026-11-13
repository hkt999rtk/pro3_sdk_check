set_source_files_properties(isp_v4l2_ctrl.c PROPERTIES
    COMPILE_FLAGS -Wno-address-of-packed-member)

set(source
    isp_algo.c
    isp_avl.c
    isp_client.c
    isp_client_message.c
    isp_msg_buf.c
    isp_core.c
    isp_geom.c
    isp_iq_table.c
    isp_iq.c
    isp_main.c
    isp_mod.c
    isp_notify.c
    isp_plugin.c
    isp_poll.c
    isp_sensor.c
    isp_statis.c
    isp_top.c
    isp_tracker.c
    isp_v4l2_ctrl.c
)

set(target isp_server_core)
add_library(${target} OBJECT ${source})
target_include_directories(${target} PRIVATE ${isptopdir}/common/core)
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE isp_server_interface)
target_link_libraries(${target} PRIVATE kernel)

set_property(GLOBAL APPEND PROPERTY isp_server_objs $<TARGET_OBJECTS:${target}>)
