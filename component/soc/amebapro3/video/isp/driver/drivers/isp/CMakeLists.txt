set(source
    isp_driver.c
    pipeline/mcu/rtscam_isp_iomem.c
    pipeline/mcu/rtscam_isp_mcu.c
    pipeline/mcu/rtscam_isp_statis.c
    pipeline/mcu/rtscam_isp_sync.c
)

if(ISP_CPU_MODE)
    set(source ${source}
        driver_cpu.c
        rtscam_isp_pipeline_cpu.c
    )
else()
    set(source ${source}
        driver_mcu.c
        rtscam_isp_pipeline.c
        rtscam_isp_input.c
        pipeline/rtscam_isp_cru.c
        pipeline/rtscam_isp_i2c.c
        pipeline/rtscam_isp_power.c
    )
endif()

set(target driver_isp)
add_library(${target} OBJECT ${source})
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PRIVATE drivers_common_interface)
target_link_libraries(${target} PRIVATE kernel)

set_property(GLOBAL APPEND PROPERTY driver_isp_objs $<TARGET_OBJECTS:${target}>)
