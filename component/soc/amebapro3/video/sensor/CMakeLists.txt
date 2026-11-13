cmake_minimum_required(VERSION 3.19)

enable_language(C CXX ASM)

# add module name here
project(sensor)

#####################################

# source code

if ( CONFIG_VERIFICATION )
list(
	APPEND ${PROJECT_NAME}_sources
	./verification/sensor_start.c
)
endif()

# hal
list(
	APPEND ${PROJECT_NAME}_sources
	./hal/sensor_i2c.c
)

# include path
list(
	APPEND ${PROJECT_NAME}_include

	${viplite_inc_path}
	./verification/
	../../hal/include
	../../hal/src
)

# marco define
list(
	APPEND ${PROJECT_NAME}_define
	${viplite_flags}
)

# add sensor driver from PC to build
list(
	APPEND ${PROJECT_NAME}_sources
	ameba_cmos_sensor/omniversion/os04a10/os04a10.c
	ameba_cmos_sensor/galaxycore/gc4023/gc4023.c
	ameba_cmos_sensor/smartsens/sc485sl/sc485sl.c
	ameba_cmos_sensor/galaxycore/gc8613/gc8613_25fps_mipi.c
	ameba_cmos_sensor/realtek/pseudo/pseudo_4m.c
	ameba_cmos_sensor/realtek/fixp/fixp.c
)
list(
	APPEND ${PROJECT_NAME}_include
	${soc_root}/video/isp/driver/isplib/common/api
)

#####################################
####       KEEP BELOW PART       ####
#####################################

# include(${CMAKE_SOURCE_DIR}/cmake/libversion_file.cmake)

add_library(
	${PROJECT_NAME} STATIC

	${${PROJECT_NAME}_sources}
)

list(
	APPEND ${PROJECT_NAME}_flags

	${common_define}

	${${PROJECT_NAME}_define}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_flags} )
target_compile_options(${PROJECT_NAME} PRIVATE ${LIBS_WARN_ERR_FLAGS} ${common_compile_options})

target_include_directories(
	${PROJECT_NAME}
	PRIVATE

	${common_inc_path}

	${${PROJECT_NAME}_include}

	${soc_root}/fwlib/include
	${soc_root}/fwlib/include/cmsis
	${soc_root}/fwlib/include/rom
)

target_link_options(
	${PROJECT_NAME}
	PRIVATE

	${common_link_wrap}
)

# include(${CMAKE_SOURCE_DIR}/cmake/updatebsp.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/libversion_exec.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${vmodule_lib_path})