if (WIN32)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake/ameba.cmake)

###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
set(v_ASDK_VER 12.3.1)
set(v_FATFS_VER r0.14b)
set(v_FREERTOS_VER v11.1.0)
set(v_LITTLEFS_VER r2.50)
set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.5)
set(v_ENCODER_VER 2.4.114)
set(v_NN_VER 2.0.0)
set(v_GIC_VER 2)
set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
set(c_VERBOSE_MAKEFILE FALSE)
set(boot_name amebapro3_boot.bin)

###############################################################################
if( "${EXAMPLE}" MATCHES "(video_|audio_)" )
	ameba_unset(c_CMPT_EXAMPLE_DIR)
	ameba_set(c_CMPT_EXAMPLE_DIR ${c_COMPONENT_DIR}/soc/amebapro3/sw/media_example)
	ameba_set(RUN_MEDIA_EXAMPLE TRUE)
elseif( "${EXAMPLE}" MATCHES "^8735c_" )
	ameba_unset(c_CMPT_EXAMPLE_DIR)
	ameba_set(c_CMPT_EXAMPLE_DIR ${c_COMPONENT_DIR}/soc/amebapro3/sw/example)
endif()
###############################################################################
ameba_soc_project_create(amebapro3 p_OUTPUT_MCU_PROJECT np)

#WARNING: project MUST be called after ameba_soc_project_create()
project(${c_SOC_TYPE} LANGUAGES C CXX ASM)

#ameba_soc_project_check()
###############################################################################
#OPTIMIZE: Used in bin-merge process, remove theme if no need
set(NP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_np)
set(VP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_vp)
set(FP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_fp)
set(AP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_ap)
###############################################################################

#NOTE: import_kconfig is used for ameba_set_if... later
configure_file(${c_MENUCONFIG_DIR}/.config ${CMAKE_CURRENT_BINARY_DIR}/.config COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config)

###############################################################################
# Add sub projects
ameba_add_subdirectory(project_np)
ameba_add_subdirectory_if(CONFIG_LOADFP project_fp)
ameba_add_subdirectory_if(CONFIG_LOADMP project_vp)
ameba_add_subdirectory_if(CONFIG_LOADAP project_ap)

###############################################################################

###############################################################################
# Package final firmware

# ameba_set_if(CONFIG_MP_INCLUDED c_SDK_IMAGE_FOLDER_NAME image_mp p_ELSE image)
ameba_set_if(CONFIG_MP_INCLUDED app_name amebapro3_app_mp.bin p_ELSE amebapro3_app.bin)

ameba_firmware_package(
	${app_name}     #Output app name, referred by c_APP_BINARY_NAME in postbuild.cmake
	p_MCU_PROJECTS  #MCU project list, notice the ORDER is important which determines the image2/image3 order in final binary file
		vp
		fp
		np
		ap
)

###############################################################################

###############################################################################
# Exit project
ameba_soc_project_exit()
###############################################################################
