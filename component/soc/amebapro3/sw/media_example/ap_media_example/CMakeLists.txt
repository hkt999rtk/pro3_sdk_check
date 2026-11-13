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
	# video example entry
	example_video_ap.c

	# audio example entry
	example_audio_ap.c
)
if(NOT CONFIG_4K_DEV AND NOT CONFIG_FAST_CAMERA_START)
	ameba_list_append(private_sources
		# video example
		video_example_src/mmf2_video_example_joint_test_init.c
		video_example_src/mmf2_video_example_v1_rtsp_init.c
		video_example_src/mmf2_video_example_v1v2_mp4_rtsp_init.c
		video_example_src/mmf2_video_example_av_rtsp_mp4_nn_init.c
		video_example_src/mmf2_video_example_v1_param_change_init.c
		video_example_src/mmf2_video_example_v1v2_a2way_rtsp_mp4_init.c
		video_example_src/mmf2_video_example_uvc_tuning_init.c
		video_example_src/mmf2_video_example_v1_osd_init.c
		video_example_src/mmf2_video_example_v1_streaming_with_snapshot_init.c
		# nn video example
		video_example_src/mmf2_video_example_nn_object_detection_init.c
		video_example_src/mmf2_video_example_nn_face_detection_init.c
		video_example_src/mmf2_video_example_nn_face_recognition_init.c
	)
	ameba_list_append_if(CONFIG_USBH_CDC_ECM private_sources
		# video usb ethernet example
		video_example_src/mmf2_video_example_v1_rtsp_usb_ethernet_init.c
		${c_COMPONENT_DIR}/../example/usb/usbh_cdc_ecm/example_usbh_cdc_ecm.c
	)
endif()
ameba_list_append_if(CONFIG_4K_DEV private_sources
	# video 4K example
	video_example_src/mmf2_video_example_v1_4k_init.c
)
ameba_list_append_if(CONFIG_FAST_CAMERA_START private_sources
	# video FCS example
	video_example_src/fast_camera_start/mmf2_video_example_fast_camera_start_init.c
	video_example_src/fast_camera_start/video_fcs_params.c
)

ameba_list_append_if(CONFIG_BUILD_VIDEO_TEST_TOOL private_sources
	video_example_src/video_test_tool/mmf2_video_cmd_test_tool.c
)

ameba_list_append(private_sources
	# audio example
	audio_example_src/mmf2_example_aacloop_init.c
	audio_example_src/mmf2_example_audioloop_init.c
	audio_example_src/mmf2_example_2way_audio_init.c
	audio_example_src/mmf2_example_g711loop_init.c
)
ameba_list_append_if(CONFIG_NEWAEC private_sources
	audio_example_src/audio_vqe_test.c
)

if(CONFIG_BUILD_AUDIO_TEST_TOOL)
	ameba_list_append(private_sources
		audio_example_src/audio_test_tool/module_null.c
		audio_example_src/audio_test_tool/module_afft.c
		audio_example_src/audio_test_tool/module_tone.c
		audio_example_src/audio_test_tool/audio_tool_filterbank.c
		audio_example_src/audio_test_tool/audio_cjson_generater.c
		audio_example_src/audio_test_tool/audio_http_server.c
		audio_example_src/audio_test_tool/audio_tool_init.c
		audio_example_src/audio_test_tool/audio_tool_command.c
		audio_example_src/audio_test_tool/pcm8K_music.c
		audio_example_src/audio_test_tool/pcm16K_music.c
		${c_CMPT_NETWORK_DIR}/tftp/tftp_client.c
		${c_CMPT_NETWORK_DIR}/tftp/tftp_server.c
	)

	ameba_list_append(private_includes
		${c_CMPT_SOC_DIR}/sw/media_example/ap_media_example/audio_example_src/audio_test_tool
		${c_CMPT_NETWORK_DIR}/tftp
	)

	ameba_list_append(private_definitions SAVE_AUDIO_DATA=1)
endif()

ameba_list_append(private_includes
	${c_CMPT_SOC_DIR}/sw/media/mmfv2
	${c_CMPT_SOC_DIR}/sw/media/mmfv2/module
	${c_CMPT_SOC_DIR}/sw/media/rtp_codec
	${c_CMPT_SOC_DIR}/sw/video_sw/video_ipc
	${c_CMPT_SOC_DIR}/sw/video_sw/video_app_vp
	${c_CMPT_SOC_DIR}/sw/video_sw/video_com_inc
	${c_CMPT_SOC_DIR}/sw/video_sw/video_api_ap
	${c_CMPT_SOC_DIR}/sw/video_sw/video_osd
	${c_CMPT_SOC_DIR}/sw/video_sw/video_nn_utils
	${c_CMPT_SOC_DIR}/sw/media/muxer

	${c_CMPT_SOC_DIR}/video/encoder/driver/VC9000E_CtrlSW_${v_ENCODER_VER}/software/inc
	${c_CMPT_SOC_DIR}/video/encoder/driver/VC9000E_CtrlSW_${v_ENCODER_VER}/software/source/hevc

	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include/nbg_linker
	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include
	${c_CMPT_SOC_DIR}/video/nn/app/img_process
	${c_CMPT_SOC_DIR}/video/nn/app/nn_model
	${c_CMPT_SOC_DIR}/video/nn/app

	${c_CMPT_SOC_DIR}/video/isp/hal

    ${c_CMPT_SOC_DIR}/sw/audio_sw/audio_api
    
    ${c_CMPT_SOC_DIR}/audio/3rdparty/AEC
	${c_CMPT_SOC_DIR}/audio/3rdparty/faac/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/faac/libfaac
	${c_CMPT_SOC_DIR}/audio/3rdparty/fdk-aac-2.0.2/libSYS/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/fdk-aac-2.0.2/libAACdec/include
	${c_CMPT_SOC_DIR}/audio/3rdparty/fdk-aac-2.0.2/libAACenc/include
	${c_COMPONENT_DIR}/soc/common/cmsis-dsp/Include

	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable/realtek/amebapro3
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/include
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/include/private
	${c_CMPT_SOC_DIR}/sw/freertos_posix/lib/include/FreeRTOS_POSIX

	${c_CMPT_NETWORK_DIR}

	${c_CMPT_WIFI_DIR}/common

	# USB CDC-ECM headers
	${c_COMPONENT_DIR}/usb/common
	${c_COMPONENT_DIR}/usb/host/core
	${c_COMPONENT_DIR}/usb/host/cdc_ecm
	
	# USB UVC device headers
	${c_COMPONENT_DIR}/usb/device/core
	${c_COMPONENT_DIR}/usb/device/uvc
)
ameba_list_append_if(CONFIG_FAST_CAMERA_START private_includes
	# video FCS example
	${c_CMPT_SOC_DIR}/sw/media_example/ap_media_example/video_example_src/fast_camera_start
)
	
ameba_list_append(private_compile_options -w)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
if(CONFIG_BUILD_ISP_SRAM_TEST)
	# Only the UART comparison entry points; other examples require optional sinks.
	set(private_sources
		example_video_ap.c
		video_example_src/video_test_tool/mmf2_video_cmd_test_tool.c
	)
endif()

ameba_add_internal_library(ap_media_example
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
