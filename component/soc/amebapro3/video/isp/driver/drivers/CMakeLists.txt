set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(drivertopdir ${CMAKE_CURRENT_SOURCE_DIR})

set(target drivers_common_interface)
add_library(${target} INTERFACE)
target_include_directories(${target} INTERFACE include/)
target_include_directories(${target} INTERFACE common)
target_include_directories(${target} INTERFACE clock)
target_include_directories(${target} INTERFACE i2c)
target_include_directories(${target} INTERFACE reset)
target_include_directories(${target} INTERFACE pinctrl)
target_include_directories(${target} INTERFACE gpio)
target_compile_features(${target} INTERFACE c_std_99)
target_compile_options(${target} INTERFACE -Wall -Werror)
target_compile_options(${target} INTERFACE -fno-common)
target_compile_options(${target} INTERFACE -ffreestanding)
target_compile_options(${target} INTERFACE -ffunction-sections -fdata-sections)
target_compile_options(${target} INTERFACE -ffile-prefix-map=${drivertopdir}=/drivers)

set(target drivers_interface)
add_library(${target} INTERFACE)
target_include_directories(${target} INTERFACE include/)

add_subdirectory(common)
add_subdirectory(isp)
add_subdirectory(memory)

get_property(common_objs GLOBAL PROPERTY driver_common_objs)
get_property(isp_objs GLOBAL PROPERTY driver_isp_objs)
get_property(memory_objs GLOBAL PROPERTY driver_memory_objs)

if(ISP_CPU_MODE)
    add_subdirectory(connector)
    get_property(connector_objs GLOBAL PROPERTY driver_connector_objs)
    set(objs ${common_objs} ${connector_objs} ${memory_objs} ${isp_objs})
else()
    # video driver is MCU-only (direct hardware access, requires MCU ISP pipeline)
    add_subdirectory(video)
    get_property(video_objs GLOBAL PROPERTY driver_video_objs)
    add_subdirectory(i2c)
    add_subdirectory(gpio)
    add_subdirectory(pinctrl)
    add_subdirectory(reset)
    add_subdirectory(clock)
    get_property(i2c_objs GLOBAL PROPERTY driver_i2c_objs)
    get_property(gpio_objs GLOBAL PROPERTY driver_gpio_objs)
    get_property(pinctrl_objs GLOBAL PROPERTY driver_pinctrl_objs)
    get_property(reset_objs GLOBAL PROPERTY driver_reset_objs)
    get_property(clock_objs GLOBAL PROPERTY driver_clock_objs)
    set(objs ${common_objs} ${i2c_objs} ${gpio_objs} ${pinctrl_objs} ${reset_objs} ${clock_objs} ${memory_objs} ${isp_objs} ${video_objs})
endif()

set(target drivers)
add_library(${target} STATIC ${objs})

install(TARGETS ${target} ARCHIVE DESTINATION lib)

set(headers
    include/isp_rtos_dev.h
    include/isp_rtos_epoll.h
    include/isp_rtos_ioctl.h
    include/isp_driver.h
    include/rts_camera_isp.h
    include/memory_prv.h
    isp/rtscam_isp_pipeline.h
    video/rts_camera_hw_config.h
)

if(ISP_CPU_MODE)
    set(headers ${headers}
        connector/rts_camera_ispc.h
        connector/rtscam_ispc.h
    )
endif()

install(FILES ${headers} DESTINATION /include)
