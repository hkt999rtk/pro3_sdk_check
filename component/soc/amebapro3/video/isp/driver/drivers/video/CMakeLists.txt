set(source
    rts_camera.c
    rts_camera_soc.c
    rts_camera_zoom.c
    rts_camera_fps.c
    rts_camera_mem.c
    rts_camera_video.c
    rts_camera_md.c
)

set(target driver_video)
add_library(${target} OBJECT ${source})
# uint32_t is 'long unsigned int' on this ARM32 target; silence format mismatches
# from ported Zephyr printf calls that use plain %d/%u/%x with uint32_t arguments.
target_compile_options(${target} PRIVATE -Wno-format)
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../isp)
target_link_libraries(${target} PRIVATE drivers_common_interface)

set_property(GLOBAL APPEND PROPERTY driver_video_objs $<TARGET_OBJECTS:${target}>)
