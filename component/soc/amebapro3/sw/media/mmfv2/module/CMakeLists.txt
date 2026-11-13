##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

if(CONFIG_ARM_CORE_CA32)
ameba_list_append(public_libraries
	${c_CMPT_SOC_DIR}/audio/3rdparty/lib/libfdk-aac.a
)
endif()

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(p_NO_WHOLE_ARCHIVE ${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin
if(CONFIG_ARM_CORE_CA32)

ameba_list_append(private_sources
    module_video.c
    module_filesaver.c
    module_vipnn.c
    module_facerecog.c
    module_array.c
    module_mp4.c
	memory_encoder.c
	module_audio.c
	module_aac.c
	module_aad.c
	module_rtp.c
	module_g711.c
)
ameba_list_append_if(CONFIG_LWIP_LAYER private_sources
    module_rtsp2.c
)
ameba_list_append_if(CONFIG_USBD_UVC private_sources
    module_uvcd.c
)
elseif(CONFIG_ARM_CORE_CM4_VP)

if(CONFIG_4K_DEV)
    ameba_list_append(private_sources module_isp_4k.c)
    ameba_list_append(private_includes
        ${c_CMPT_SOC_DIR}/video/isp/driver/drivers/video
        ${c_CMPT_SOC_DIR}/video/isp/driver/drivers/isp
        ${c_CMPT_SOC_DIR}/video/isp/driver/drivers/reset
        ${c_CMPT_SOC_DIR}/video/isp/driver/drivers/common
        ${c_CMPT_SOC_DIR}/video/isp/driver/drivers/include
    )
else()
    ameba_list_append(private_sources module_isp.c)
endif()

ameba_list_append(private_sources
    memory_encoder.c
    module_encoder.c
    module_array.c
    module_filesaver.c
    module_vipnn.c
    module_vp2ap.c
)
ameba_list_append_if(CONFIG_SUPPORT_UVC_TUNING private_sources
    module_uvcd.c
)
elseif(CONFIG_PXP)

ameba_list_append(private_sources
    memory_encoder.c
    module_isp.c
    module_encoder.c
    module_array.c
    module_filesaver.c
    module_vipnn.c
	module_vp2ap.c
)
ameba_list_append_if(CONFIG_SUPPORT_UVC_TUNING private_sources
    module_uvcd.c
)

endif()

ameba_list_append(private_includes
	${c_CMPT_SOC_DIR}/sw/media/mmfv2
	${c_CMPT_SOC_DIR}/sw/media/mmfv2/module
	${c_CMPT_SOC_DIR}/sw/media/samples
	${c_CMPT_SOC_DIR}/sw/media/rtp_codec
	${c_CMPT_SOC_DIR}/sw/media/muxer

	${c_CMPT_SOC_DIR}/sw/video_sw/video_com_inc 
	${c_CMPT_SOC_DIR}/sw/video_sw/video_ipc
	${c_CMPT_SOC_DIR}/sw/video_sw/video_api_ap

	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include/nbg_linker
	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include
	${c_CMPT_SOC_DIR}/video/nn/driver
	${c_CMPT_SOC_DIR}/video/nn/app
	${c_CMPT_SOC_DIR}/video/nn/app/img_process
	${c_CMPT_SOC_DIR}/video/nn/app/nn_sys_api
	${c_CMPT_SOC_DIR}/video/nn/app/nn_file_system

	${c_CMPT_SOC_DIR}/video/isp/driver/isplib/common/api
	${c_CMPT_SOC_DIR}/video/isp/hal
	#${c_CMPT_SOC_DIR}/video/isp/verification

	${c_CMPT_SOC_DIR}/video/encoder/driver/VC9000E_CtrlSW_${v_ENCODER_VER}/software/inc
	${c_CMPT_SOC_DIR}/video/encoder/driver/VC9000E_CtrlSW_${v_ENCODER_VER}/software/source/hevc
	${c_CMPT_SOC_DIR}/video/encoder/driver
	${c_CMPT_SOC_DIR}/video/encoder/hal

	${c_CMPT_SOC_DIR}/video/addremap/hal

	${c_CMPT_SOC_DIR}/video/ainr/driver
	${c_CMPT_SOC_DIR}/video/ainr/model

	${c_CMPT_SOC_DIR}/video/ts/verification
	${c_CMPT_SOC_DIR}/video/ts/hal

	${c_CMPT_SOC_DIR}/fwlib/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/AEC
	${c_CMPT_SOC_DIR}/audio/3rdparty/faac/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/faac/libfaac
	${c_CMPT_SOC_DIR}/audio/3rdparty/fdk-aac-2.0.2/libSYS/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/fdk-aac-2.0.2/libAACdec/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/fdk-aac-2.0.2/libAACenc/include
	${c_COMPONENT_DIR}/soc/common/cmsis-dsp/Include
	${c_COMPONENT_DIR}/file_system/vfs
	${c_CMPT_SOC_DIR}/sw/audio_sw/audio_api

	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable/realtek/amebapro3
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/include
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/include/private
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/include/FreeRTOS_POSIX

	${c_CMPT_SOC_DIR}/hal/include
	${c_CMPT_SOC_DIR}/hal/src

	${c_CMPT_USB_DIR}/common
	${c_CMPT_USB_DIR}/device/core	
	${c_CMPT_USB_DIR}/host/core
	${c_CMPT_USB_DIR}/device/uvc

	${c_CMPT_NETWORK_DIR}

	${c_CMPT_WIFI_DIR}/common
	${c_CMPT_WIFI_DIR}/api

	${c_CMPT_LWIP_DIR}/api
	${c_LWIP_DIR}/src/include
	${c_LWIP_DIR}/port/realtek
	${c_LWIP_DIR}/port/realtek/freertos
)

ameba_list_append(private_compile_options -mfp16-format=ieee)
ameba_list_append(private_definitions REALSENSOR=1)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(mmf_module
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
if(CONFIG_ARM_CORE_CM4_VP)
add_custom_command(
	TARGET ${c_CURRENT_TARGET_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} --prefix-alloc-sections .bf $<TARGET_FILE:${c_CURRENT_TARGET_NAME}>
	COMMENT "add prefix to sections of ${c_CURRENT_TARGET_NAME} with prefix .bf"
)
endif()
##########################################################################################
