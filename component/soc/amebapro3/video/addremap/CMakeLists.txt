cmake_minimum_required(VERSION 3.19)

enable_language(C CXX ASM)

# add module name here
project(addremap)

#####################################

# source code
# source code
set (isp_root ../isp)
set (enc_path ../encoder)
set (vc9000e VC9000E_CtrlSW_2.4.114)
set (enc_root ${enc_path}/driver/${vc9000e})
include(${enc_root}/videoenc.cmake)

# file( GLOB hal_sources CONFIGURE_DEPENDS ./hal/*.c )
# isp/drivers
file(GLOB ISPDRIVERS_SRC CONFIGURE_DEPENDS 
	${isp_root}/driver/drivers/clock/*.c
	${isp_root}/driver/drivers/common/*.c
	${isp_root}/driver/drivers/gpio/*.c
	${isp_root}/driver/drivers/i2c/*.c
	${isp_root}/driver/drivers/isp/isp_driver.c
	${isp_root}/driver/drivers/isp/pipeline/mcu/rtscam_isp_iomem.c
	${isp_root}/driver/drivers/isp/pipeline/mcu/rtscam_isp_mcu.c
	${isp_root}/driver/drivers/isp/pipeline/mcu/rtscam_isp_statis.c
	${isp_root}/driver/drivers/isp/pipeline/mcu/rtscam_isp_sync.c
	${isp_root}/driver/drivers/isp/driver_mcu.c
	${isp_root}/driver/drivers/isp/rtscam_isp_pipeline.c
	${isp_root}/driver/drivers/isp/pipeline/rtscam_isp_cru.c
	${isp_root}/driver/drivers/isp/pipeline/rtscam_isp_i2c.c
	${isp_root}/driver/drivers/isp/pipeline/rtscam_isp_power.c
	${isp_root}/driver/drivers/memory/memory.c
	${isp_root}/driver/drivers/pinctrl/*.c
	${isp_root}/driver/drivers/reset/*.c
)

# isp/isplib
file(GLOB ISPLIB_SRC CONFIGURE_DEPENDS 
	${isp_root}/driver/isplib/common/core/*.c
	${isp_root}/driver/isplib/common/oslayer/freertos/*.c
	${isp_root}/driver/isplib/controller/api/*.c
	${isp_root}/driver/isplib/server/api/*.c
	${isp_root}/driver/isplib/server/common/*.c
	${isp_root}/driver/isplib/server/core/*.c
	${isp_root}/driver/isplib/server/hardware/*.c
	${isp_root}/driver/isplib/server/modules/*.c
	${isp_root}/driver/isplib/server/oslayer/freertos/*.c
	${isp_root}/driver/isplib/server/proto/*.c
	${isp_root}/driver/isplib/server/proto/nanopb/*.c
	${isp_root}/driver/isplib/server/modules/isp_modules.c
)


# mehtod 1 manual list 
list(
    APPEND ${PROJECT_NAME}_sources
	# ${hal_sources}
	./driver/addremap_runtime.c
)
if (CONFIG_VERIFICATION AND NOT CONFIG_VMODULE_VERIFICATION_ISP)
list(
	APPEND ${PROJECT_NAME}_sources
	${ISPDRIVERS_SRC} 
	${ISPLIB_SRC}	
)
endif()

# method 2 glob all *.c
file( GLOB hal_sources CONFIGURE_DEPENDS ./hal/*.c )
list(
    APPEND ${PROJECT_NAME}_sources
	${hal_sources}
)

# enc
if (CONFIG_VERIFICATION AND NOT CONFIG_VMODULE_ENCODER)
list( APPEND ${PROJECT_NAME}_sources
	${venc_src}
)
endif()


if ( CONFIG_VERIFICATION )
list(
    APPEND ${PROJECT_NAME}_sources
	./verification/addremap_verify.c  
	
)

# isp
# for debug
if (NOT CONFIG_VMODULE_VERIFICATION_ISP)
list(
    APPEND ${PROJECT_NAME}_sources
	# ${isp_root}/verification/isp_verify.c  
	${isp_root}/verification/task_mcu.c  
	${isp_root}/verification/reg_api.c  
	${isp_root}/verification/reg_log.c  
	${isp_root}/verification/config_regs.c 
	${isp_root}/verification/task_isp_io.c  
	${isp_root}/verification/crc32.c  	 
	${isp_root}/verification/config_regs_vhdr.c
	${isp_root}/verification/config_regs_pipe.c
	${isp_root}/verification/multistream/main.c
	${isp_root}/verification/multistream/isp_main.c
	${isp_root}/verification/multistream/producer_thread_ISP.c
)
endif()

# enc
if (NOT CONFIG_VMODULE_VERIFICATION_ENCODER)
list(
    APPEND ${PROJECT_NAME}_sources
	#${enc_path}/verification/enc_simple_demo.c 
	${enc_path}/verification/enc_simple_utils.c
	${enc_path}/verification/enc_simple_configs.c	
)
endif()


endif()

# include path
list(
	APPEND ${PROJECT_NAME}_include

	./driver
	./hal
	../isp/verification
	../ts/verification
	../ts/hal		
)

# isp
list(
	APPEND ${PROJECT_NAME}_include

	${isp_root}/driver
	${isp_root}/hal
	# isp/drivers
	${isp_root}/driver/drivers/clock
	${isp_root}/driver/drivers/common
	${isp_root}/driver/drivers/gpio
	${isp_root}/driver/drivers/i2c
	${isp_root}/driver/drivers/isp
	${isp_root}/driver/drivers/memory
	${isp_root}/driver/drivers/pinctrl
	${isp_root}/driver/drivers/reset
	${isp_root}/driver/drivers/include

	# isp/kernel
	${isp_root}/driver/kernel/portable   # cache.h

	# isp/isplib
	${isp_root}/driver/isplib/server/core
	${isp_root}/driver/isplib/server/common
	${isp_root}/driver/isplib/server/oslayer
	${isp_root}/driver/isplib/server/modules
	${isp_root}/driver/isplib/server/hardware
	${isp_root}/driver/isplib/server/hardware/regs
	${isp_root}/driver/isplib/server/hardware/regs/rts3921
	${isp_root}/driver/isplib/server/proto
	${isp_root}/driver/isplib/server/proto/nanopb
	${isp_root}/driver/isplib/common/core
	${isp_root}/driver/isplib/common/oslayer
	${isp_root}/driver/isplib/common/common
	${isp_root}/driver/isplib/common/api
	${isp_root}/driver/isplib/server/hardware/regs
	${isp_root}/driver/isplib/common/api
	${isp_root}/verification  
	${isp_root}/verification/multistream
	${isp_root}/application/rts_camera
	${isp_root}/application/rts_camera/linux
	${isp_root}/application/rts_camera/media

	# sensor peripheral
	${soc_root}/video/sensor/hal
)

# include path 
list(
	APPEND ${PROJECT_NAME}_include_voe
	${isp_root}/application/voe_related/cmsis/voe/include
	${isp_root}/application/voe_related/lib_isp
	${isp_root}/application/voe_related/lib_isp/drv/
	${isp_root}/application/voe_related/lib_isp/drv/clk
	${isp_root}/application/voe_related/lib_isp/drv/gpio
	${isp_root}/application/voe_related/lib_isp/drv/i2c
	${isp_root}/application/voe_related/lib_isp/drv/isp/3915/kernel/include
	${isp_root}/application/voe_related/lib_isp/drv/reset
	${isp_root}/application/voe_related/lib_isp/drv/rlx
	${isp_root}/application/voe_related/lib_isp/drv/rlx/reg
	${isp_root}/application/voe_related/lib_isp/lib/common
	${isp_root}/application/voe_related/lib_isp/lib/video/3915/include
	${isp_root}/application/voe_related/lib_isp/rtstream/hardware/isp/include/common
	${isp_root}/application/voe_related/lib_isp/rtstream/utils/librtscamkit/include
	${isp_root}/application/voe_related/lib_isp/share/common
	${isp_root}/application/voe_related/lib_isp/share/config
	${isp_root}/application/voe_related/lib_isp/sys
	${isp_root}/application/voe_related/os/freertos_v2020
	${isp_root}/application/voe_related/inc/include
	${isp_root}/application/voe_related/inc/linux
	${isp_root}/application/voe_related/inc/linux2
)

# enc
list( APPEND ${PROJECT_NAME}_include
	${venc_inc_path}
	${enc_path}/verification
)


# marco define

# isp
list(
	APPEND ${PROJECT_NAME}_define
	
	USER_DEFINE_WITH_VALUE=1
	USER_DEFINE
)

if(DEFINED VHDR AND VHDR)
list(
	APPEND ${PROJECT_NAME}_define
	VHDR
)
endif()


# enc
list( APPEND ${PROJECT_NAME}_define
	${venc_flags}
)

if(DEFINED CONFIG_VMODULE_ENCODER AND CONFIG_VMODULE_ENCODER)
list(
	APPEND ${PROJECT_NAME}_define
	VMODULE_ENCODER=1
)
endif()



#####################################
####       KEEP BELOW PART       ####
#####################################

# include(${CMAKE_SOURCE_DIR}/cmake/libversion_file.cmake)

add_library(
    ${PROJECT_NAME} STATIC
	
    ${${PROJECT_NAME}_sources}
)

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} --prefix-alloc-sections .bf $<TARGET_FILE:${PROJECT_NAME}>
	COMMENT "add prefix to sections of ${PROJECT_NAME} with prefix .bf"
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
	${${PROJECT_NAME}_include_voe}
	
	${soc_root}/fwlib/include
	${soc_root}/fwlib/include/cmsis
	${soc_root}/fwlib/include/rom	
)

# include(${CMAKE_SOURCE_DIR}/cmake/updatebsp.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/libversion_exec.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
