target_compile_options(isp_common_interface INTERFACE -ffreestanding)

add_subdirectory(common)
add_subdirectory(server)
get_property(common_objs GLOBAL PROPERTY isp_common_objs)
get_property(server_objs GLOBAL PROPERTY isp_server_objs)
set(objs ${common_objs} ${server_objs})

if(NOT ISP_CPU_MODE)
	add_subdirectory(controller)
	get_property(controller_objs GLOBAL PROPERTY isp_controller_objs)
	set(objs ${objs} ${controller_objs})
endif()

set(target isp)
add_library(${target} STATIC ${objs})
install(TARGETS ${target} ARCHIVE DESTINATION ${ISP_MCU_STAGING_DIR}/lib)
