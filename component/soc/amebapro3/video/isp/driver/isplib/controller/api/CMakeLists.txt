set(source
    rts_isp_ae.c
    rts_isp_af.c
    rts_isp_awb.c
    rts_isp_ctrl.c
    rts_isp_mask.c
    rts_isp_sensor.c
    rts_isp_tuning.c
    rts_isp_utils.c
)

if(ISP_CPU_MODE)
    set(source ${source}
        rts_isp_cpu.c
    )
else()
    set(source ${source}
        rts_isp_legacy.c
    )
endif()

set(target isp_controller_api)
add_library(${target} OBJECT ${source})
target_include_directories(${target} PRIVATE ${isptopdir}/common/core)
target_include_directories(${target} PRIVATE ${isptopdir}/server/modules)

if(ISP_CPU_MODE)
    target_include_directories(${target} PRIVATE ${ispcontrollerdir}/core)
    target_link_libraries(${target} PRIVATE isp_controller_interface)
    target_compile_definitions(${target} PRIVATE ISP_CPU_MODE)
else()
    target_include_directories(${target} PRIVATE ${isptopdir}/server/core)
    target_include_directories(${target} PRIVATE ${isptopdir}/server/oslayer)
endif()

target_link_libraries(${target} PRIVATE isp_common_interface)

set_property(GLOBAL APPEND PROPERTY isp_controller_objs $<TARGET_OBJECTS:${target}>)
