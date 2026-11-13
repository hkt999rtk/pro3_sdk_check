### add .cmkae need if neeeded ###
message(STATUS "build Encoder Driver")

### add source file ###
# encoder codec src
file(GLOB ENC_COMM_SRC CONFIGURE_DEPENDS ${enc_root}/software/source/common/*.c)
file(GLOB ENC_HEVC_SRC CONFIGURE_DEPENDS ${enc_root}/software/source/hevc/*.c)
file(GLOB ENC_JPEG_SRC CONFIGURE_DEPENDS ${enc_root}/software/source/jpeg/*.c)
# encoder wrapper layer src
list(
	APPEND ENC_EWL_SRC
	${enc_root}/software/linux_reference/ewl/ewl.c
	${enc_root}/software/linux_reference/ewl/ewl_common.c
	${enc_root}/software/linux_reference/ewl/ewl_multicore.c
)

# encoder user src
list(
	APPEND ENC_USER_SRC
	${ENC_COMM_SRC}
	${ENC_HEVC_SRC}
	${ENC_JPEG_SRC}
	${ENC_EWL_SRC}
)

# encoder kernel src
list(
	APPEND ENC_KERNEL_SRC
	${enc_root}/software/linux_reference/kernel_module/freertos/frameworks.c
	${enc_root}/software/linux_reference/kernel_module/freertos/io_tools.c
	${enc_root}/software/linux_reference/kernel_module/freertos/memalloc_freertos.c
	${enc_root}/software/linux_reference/kernel_module/freertos/user_freertos.c
	${enc_root}/software/linux_reference/kernel_module/freertos/vcx_abnormal_irq_freertos.c
	${enc_root}/software/linux_reference/kernel_module/freertos/vcx_vcmd_driver_freertos.c
	${enc_root}/software/linux_reference/kernel_module/freertos/vcmdswhwregisters.c
	# ${enc_root}/software/linux_reference/kernel_module/freertos/vcx_normal_driver.c

	${enc_root}/software/linux_reference/kernel_module/linux/bidirect_list.c
)

#video encoder test bench
list(
	APPEND JPEG_TEST_BENCH	
	${enc_root}/software/linux_reference/test/jpeg/JpegTestBench.c
	${enc_root}/software/linux_reference/test/jpeg/EncGetOption.c
)
list(
	APPEND HEVC_H264_TEST_BENCH	
	${enc_root}/software/linux_reference/test/hevc/test_bench.c 
	${enc_root}/software/linux_reference/test/hevc/test_bench_utils.c 
	${enc_root}/software/linux_reference/test/hevc/test_bench_statistic.c 
	${enc_root}/software/linux_reference/test/hevc/get_option.c 
	${enc_root}/software/linux_reference/test/hevc/test_bench_pic_config.c 
	${enc_root}/software/linux_reference/test/hevc/tb_flex_ref.c 
	${enc_root}/software/linux_reference/test/hevc/tb_input_file_list.c 
	${enc_root}/software/linux_reference/test/hevc/tb_parse_roi.c
)

list(
	APPEND venc_src
	${ENC_USER_SRC}
	${ENC_KERNEL_SRC}
)

### default option ###
option(BUILD_ENC_HEVC_TEST "BUILD_ENC_HEVC_TEST" OFF)
if(DEFINED BUILD_ENC_HEVC_TEST AND BUILD_ENC_HEVC_TEST)
list(
	APPEND venc_src
	${HEVC_H264_TEST_BENCH}
)
list(
	APPEND venc_flags
	CONFIG_BUILD_ENC_HEVC_TB=1
)
else()
list(
	APPEND venc_src
	${JPEG_TEST_BENCH}
)
endif()



### add flags ###
list(
	APPEND venc_flags

	#-----------------------------------#
	# OS definition                     #
	#-----------------------------------#
	__FREERTOS__
	_HAVE_PTHREAD_H

	#-----------------------------------#
	# OS environment                    #
	#-----------------------------------#
	# POLLING_ISR
	# ENCH2_IRQ_DISABLE=1

	#-----------------------------------#
	# software space codec definition   #
	#-----------------------------------#
	MULTI_FRAME_SUPPORT
	CTBRC_STRENGTH

	# SUPPORT_AXIFE             #1.axife
	CHECKSUM_CRC_BUILD_SUPPORT  #2.checksum & crc
	CUTREE_BUILD_SUPPORT        #3.cutree
	VCMD_BUILD_SUPPORT          #4.vcmd
	# LOGMSG                    #5.log
	# SUPPORT_DEC400            #6.dec400
	LOW_LATENCY_BUILD_SUPPORT   #7.low latency
	ERROR_BUILD_SUPPORT         #8.error log
	# LINUX_LOCK_BUILD_SUPPORT  #9.linux lock
	# SUPPORT_MEM_SYNC          #10.memory synchronization
	FIFO_BUILD_SUPPORT          #11.fifo
	# SUPPORT_CACHE             #12.cache
	SEI_BUILD_SUPPORT           #13.sei
	RATE_CONTROL_BUILD_SUPPORT  #14.rate control
	RCP_FIXED_POINT_OPT         #14.rate control
	INTERNAL_TEST             #15.test id
	CUINFO_BUILD_SUPPORT        #16.cuinfo
	ROI_BUILD_SUPPORT           #17.roi
	STREAM_CTRL_BUILD_SUPPORT   #18.stream ctrl
	# MULTICORE_SUPPORT           #19.multi core
	OVERLAY_BUILD_SUPPORT       #20.overlay support
	VCECFG_MAX_CORE_NUM=1       #21.EWL MAX CORE NUMBER
	# HDR_HELPER_SUPPORT        #22.HDR10

	#-----------------------------------#
	# VCMD                              #
	#-----------------------------------#
	# USE_END_CMD                 # option to choose using JMP cmd or END cmd

	#-----------------------------------#
	# CODEC                             #
	#-----------------------------------#
	SUPPORT_HEVC
	SUPPORT_H264
	SUPPORT_VIDEO
	SUPPORT_JPEG
	OVERLAY_BUILD_SUPPORT
	SUPPORT_UFBC

	#-----------------------------------#
	# Kernel space module definition    #
	#-----------------------------------#
	ENC_MODULE_PATH="/tmp/dev/vsi_vcx"
	MEMALLOC_MODULE_PATH="/tmp/dev/memalloc"

	#-----------------------------------#
	# Debug                             #
	#-----------------------------------#
	# HANTRO_DRIVER_DEBUG
	# REGISTER_DEBUG
	# MEMALLOC_DEBUG
	# BIDIRECTION_LIST_DEBUG

	#-----------------------------------#
	# Platform                          #
	#-----------------------------------#
	ARM_ARCH_SWAP

	#-----------------------------------#
	# Test bench                        #
	#-----------------------------------#
	TB_DEFINED_COMMENT
)

### add header files ###
list (
    APPEND venc_inc_path

	${enc_root}/software/inc
	${enc_root}/software/source/common
	${enc_root}/software/source/hevc
	${enc_root}/software/source/jpeg
	${enc_root}/software/linux_reference/debug_trace
	${enc_root}/software/linux_reference/kernel_module/freertos
	${enc_root}/software/linux_reference/kernel_module/linux
)
