#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(t_USER_CUSTOM_LOG_PREFIX "VP_IMG2_POSTBUILD")

find_package (Python COMPONENTS Interpreter)

message( "========== Image manipulating start ==========")

ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/elfconv.py -m app -e ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf -j ${CMAKE_CURRENT_LIST_DIR}/vp_app_conv.json -o ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_stage1.bin
)

ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/elfconv.py -m app -e ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf -j ${CMAKE_CURRENT_LIST_DIR}/vp_app_bf_conv.json -o ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_bf.bin
)

if(CONFIG_LOAD_CHECKSUM)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_stage1.bin -m sum
)
ameba_execute_process(	
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_bf.bin -m sum
)
endif()

if(CONFIG_LOAD_CRC32)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_stage1.bin -m crc32
)
ameba_execute_process(	
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_bf.bin -m crc32
)
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_stage1.bin ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_bf.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/km4vp_image2_all.bin
)

message("========== Image manipulating end ==========")

#if(NOT CONFIG_AMEBA_RLS)
if(False)
    message("========== Image analyze start ==========")
    ameba_execute_process(p_SHOW_OUTPUT
        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_SDK_IMAGE_TARGET_DIR}/../
    )
    ameba_execute_process(p_SHOW_OUTPUT
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD} ${c_MCU_PROJECT_DIR}
        WORKING_DIRECTORY ${c_SDK_IMAGE_TARGET_DIR}/../
    )
    message("========== Image analyze end ==========")
endif()
