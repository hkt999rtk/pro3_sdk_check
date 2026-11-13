cmake_minimum_required(VERSION 3.19)

enable_language(C CXX ASM)

# add module name here
project(isp)

#####################################

# source code

# mehtod 1 manual list
list(
    APPEND ${PROJECT_NAME}_sources

	# ./driver/template_driver.c
)

# method 2 glob all *.c
file( GLOB hal_sources CONFIGURE_DEPENDS ./hal/*.c ./hal/rts_raw/*.c ./hal/tuningtool/*.c )
# isp/drivers
file(GLOB ISPDRIVERS_SRC CONFIGURE_DEPENDS
	./driver/drivers/clock/*.c
	./driver/drivers/common/*.c
	./driver/drivers/irq/*.c
	./driver/drivers/gpio/*.c
	./driver/drivers/i2c/*.c
	./driver/drivers/isp/isp_driver.c
	./driver/drivers/isp/pipeline/mcu/rtscam_isp_iomem.c
	./driver/drivers/isp/pipeline/mcu/rtscam_isp_mcu.c
	./driver/drivers/isp/pipeline/mcu/rtscam_isp_statis.c
	./driver/drivers/isp/pipeline/mcu/rtscam_isp_sync.c
	./driver/drivers/isp/driver_mcu.c
	./driver/drivers/isp/rtscam_isp_pipeline.c
	./driver/drivers/isp/rtscam_isp_input.c
	./driver/drivers/isp/pipeline/rtscam_isp_cru.c
	./driver/drivers/isp/pipeline/rtscam_isp_i2c.c
	./driver/drivers/isp/pipeline/rtscam_isp_power.c
	./driver/drivers/memory/memory.c
	./driver/drivers/pinctrl/*.c
	./driver/drivers/reset/*.c
)

# isp/isplib
file(GLOB ISPLIB_SRC CONFIGURE_DEPENDS
	./driver/isplib/common/core/*.c
	./driver/isplib/common/oslayer/freertos/*.c
	./driver/isplib/controller/api/*.c
	./driver/isplib/server/api/*.c
	./driver/isplib/server/common/*.c
	./driver/isplib/server/core/*.c
	./driver/isplib/server/hardware/*.c
	./driver/isplib/server/modules/*.c
	./driver/isplib/server/oslayer/freertos/*.c
	./driver/isplib/server/proto/*.c
	./driver/isplib/server/proto/nanopb/*.c
	./driver/isplib/server/modules/isp_modules.c
)
list( FILTER ISPLIB_SRC EXCLUDE REGEX "_cpu")

#set(isp_modules
#	./driver/isplib/server/modules/isp_modules.c
#)
#
#add_custom_command( OUTPUT ${isp_modules}
#    COMMAND ${CMAKE_COMMAND} -Dprj_root=${prj_root} -P ./driver/isplib/server/modules/gen_modules_file.cmake
#)
list(
    APPEND ${PROJECT_NAME}_sources
	${hal_sources}
	${ISPDRIVERS_SRC}
	${ISPLIB_SRC}
#	${isp_modules}
	./test/test_md.c
	./test/test_osdi.c
)
# os04a10.c is already compiled into video/lib/libsensor.a — do not add it here

if ( CONFIG_VERIFICATION )
#file(GLOB isp_ver_multi_stream_src CONFIGURE_DEPENDS
#    ./verification/multistream/*.c
#)
# list(
    # APPEND ${PROJECT_NAME}_sources
#	./verification/isp_verify.c
#	./verification/reg_api.c
#	./verification/reg_log.c
#	./verification/config_regs.c
#	./verification/config_regs_pipe.c
#	./verification/config_regs_vhdr.c
#	./verification/task_isp_io.c
#	./verification/crc32.c
# )
endif()

file(GLOB RTS_CAMERA_SRC CONFIGURE_DEPENDS
	./driver/drivers/video/*.c
)
list(
    APPEND ${PROJECT_NAME}_sources
	${RTS_CAMERA_SRC}
	${isp_ver_multi_stream_src}
)

# driver/others/ does not exist in this SDK — sources_voe is empty
# list(APPEND ${PROJECT_NAME}_sources_voe)

# include path
list(
	APPEND ${PROJECT_NAME}_include

	./driver
	./hal
	# isp/drivers
	./driver/drivers/clock
	./driver/drivers/common
	./driver/drivers/irq
	./driver/drivers/gpio
	./driver/drivers/i2c
	./driver/drivers/isp
	./driver/drivers/memory
	./driver/drivers/pinctrl
	./driver/drivers/reset
	./driver/drivers/include

	# driver/kernel is no longer needed — hal/freertos/ bridges all <freertos/X.h> to platform FreeRTOS

	# isp/isplib
	./driver/isplib/server/core
	./driver/isplib/server/common
	./driver/isplib/server/oslayer
	./driver/isplib/server/modules
	./driver/isplib/server/hardware
	./driver/isplib/server/hardware/regs
	./driver/isplib/server/hardware/regs/rts3921
	./driver/isplib/server/proto
	./driver/isplib/server/proto/nanopb
	./driver/isplib/common/core
	./driver/isplib/common/oslayer
	./driver/isplib/common/common
	./driver/isplib/common/api
	./driver/isplib/server/hardware/regs
	./driver/isplib/common/api
	./verification
#	./verification/isp_io_sample
	./driver/drivers/video

	# sensor peripheral
	${soc_root}/video/sensor/hal
	${soc_root}/video/sensor/ameba_cmos_sensor/omniversion/os04a10
	${soc_root}/../../usb/device/core
	${soc_root}/../../usb/device/uvc
	${soc_root}/../../usb/common
	${soc_root}/../../file_system/vfs
	${soc_root}/video/isp/hal/rts_raw/

	# VFS (needed by test/test_osdi.c for SD card frame saves)
	../../../../file_system/vfs

	# RCC init helper (needed by test/test_osdi.c for ISP hardware init)
	../../sw/video_sw/video_app_vp
	./iq
)

# driver/others/ does not exist in this SDK — include_voe is empty
# list(APPEND ${PROJECT_NAME}_include_voe)

# marco define
list(
	APPEND ${PROJECT_NAME}_define

	USER_DEFINE_WITH_VALUE=1
	USER_DEFINE
)

# real sensor option
list(
	APPEND ${PROJECT_NAME}_define
	REALSENSOR=1
	# Driver uses configIRQ_THREAD_TASK_PRIORITY which is not in platform FreeRTOSConfig
	"configIRQ_THREAD_TASK_PRIORITY=(configMAX_PRIORITIES-2)"
)

if(DEFINED VHDR AND VHDR)
list(
	APPEND ${PROJECT_NAME}_define
	VHDR
)
endif()

#####################################
####       KEEP BELOW PART       ####
#####################################

# include(${CMAKE_SOURCE_DIR}/cmake/libversion_file.cmake)

add_library(
    ${PROJECT_NAME} STATIC

    ${${PROJECT_NAME}_sources}
    ${${PROJECT_NAME}_sources_voe}
)

list(
	APPEND ${PROJECT_NAME}_flags

	${common_define}

	${${PROJECT_NAME}_define}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_flags} )
target_compile_options(${PROJECT_NAME} PRIVATE ${LIBS_WARN_ERR_FLAGS} ${common_compile_options} -g3)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE

	# Driver-specific paths first so driver's list.h takes precedence over FreeRTOS list.h
	${${PROJECT_NAME}_include}
	${${PROJECT_NAME}_include_voe}

	${common_inc_path}

	${soc_root}/fwlib/include
	${soc_root}/fwlib/include/cmsis
	${soc_root}/fwlib/include/rom
	${soc_root}/hal/include
	${soc_root}/hal/src
)
list(APPEND isp_algo_libs
	${soc_root}/video/isp/driver/lib/librts_algo_ae.a
	${soc_root}/video/isp/driver/lib/librts_algo_af.a
	${soc_root}/video/isp/driver/lib/librts_algo_awb.a
	${soc_root}/video/isp/driver/lib/librts_algo_other.a
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${isp_algo_libs} m)

# include(${CMAKE_SOURCE_DIR}/cmake/updatebsp.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/libversion_exec.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${vmodule_lib_path})