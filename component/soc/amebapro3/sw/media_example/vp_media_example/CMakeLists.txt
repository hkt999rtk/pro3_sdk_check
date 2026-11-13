##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

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
ameba_list_append(private_sources
	mmf2_video_test_startup.c
	mmf2_video_example_array_encoder_init.c
	mmf2_video_example_test_vin.c
	mmf2_video_example_test_vin_input.c
	mmf2_video_example_test_vin_large_image.c
	mmf2_video_example_large_image_pipeline.c
	mmf2_video_example_osdi_test_init.c
	mmf2_video_example_dbg_raw.c
)

if(CONFIG_4K_DEV AND CONFIG_ARM_CORE_CM4_VP)
	ameba_list_append(private_sources mmf2_video_example_4k_test_init.c)
else()
	ameba_list_append(private_sources
		mmf2_video_example_joint_test_init.c
		mmf2_video_example_vipnn_init.c
		mmf2_video_example_isp_init.c
	)
	ameba_list_append_if(CONFIG_USBD_UVC private_sources
		mmf2_video_example_uvcd_video_init.c
		mmf2_video_example_uvcd_video_tuning.c
	)
endif()

ameba_list_append(private_includes

	${c_CMPT_SOC_DIR}/sw/media/mmfv2
	${c_CMPT_SOC_DIR}/sw/media/mmfv2/module
	${c_CMPT_SOC_DIR}/sw/media/rtp_codec
	${c_CMPT_SOC_DIR}/sw/media/samples

	${c_CMPT_SOC_DIR}/sw/video_sw/video_ipc
	${c_CMPT_SOC_DIR}/sw/video_sw/video_app_vp
	${c_CMPT_SOC_DIR}/sw/video_sw/video_com_inc

	${c_CMPT_SOC_DIR}/video/encoder/driver/VC9000E_CtrlSW_${v_ENCODER_VER}/software/inc
	${c_CMPT_SOC_DIR}/video/encoder/driver/VC9000E_CtrlSW_${v_ENCODER_VER}/software/source/hevc

	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include/nbg_linker
	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include
	${c_CMPT_SOC_DIR}/video/nn/app/img_process
	${c_CMPT_SOC_DIR}/video/nn/app/nn_model
	${c_CMPT_SOC_DIR}/video/nn/app

	${c_CMPT_SOC_DIR}/video/ts/verification

	${c_CMPT_SOC_DIR}/video/ainr/driver
	${c_CMPT_SOC_DIR}/video/ainr/model

	${c_CMPT_SOC_DIR}/video/isp/driver/drivers/video
	${c_CMPT_SOC_DIR}/video/isp/driver/drivers/isp
	${c_CMPT_SOC_DIR}/video/isp/driver/drivers/reset
	${c_CMPT_SOC_DIR}/video/isp/driver/drivers/common
	${c_CMPT_SOC_DIR}/video/isp/driver/drivers/include
	${c_CMPT_SOC_DIR}/video/isp/driver/isplib/common/api
	${c_CMPT_SOC_DIR}/video/isp/hal
	#${c_CMPT_SOC_DIR}/video/isp/verification

	${c_CMPT_SOC_DIR}/video/ge/app
	${c_CMPT_SOC_DIR}/video/ge/driver
	${c_CMPT_SOC_DIR}/video/ge/hal

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

	${c_CMPT_FILE_SYSTEM_DIR}/vfs
)

ameba_list_append(private_compile_options -w)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(vp_media_example
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
