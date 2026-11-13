#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(t_USER_CUSTOM_LOG_PREFIX "NP_IMG2_POSTBUILD")

find_package (Python COMPONENTS Interpreter)

message( "========== Image manipulating start ==========")
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/elfconv.py -m app -e ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf -j ${CMAKE_CURRENT_LIST_DIR}/np_app_ssram_conv.json -o ${c_SDK_IMAGE_TARGET_DIR}/np_img2_ssram.bin
)

ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/elfconv.py -m app -e ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf -j ${CMAKE_CURRENT_LIST_DIR}/np_app_psram_conv.json -o ${c_SDK_IMAGE_TARGET_DIR}/np_img2_psram.bin
)

if(CONFIG_LOAD_CHECKSUM)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/np_img2_ssram.bin -m sum
)
ameba_execute_process(		
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/np_img2_psram.bin -m sum
)
endif()


if(CONFIG_LOAD_CRC32)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/np_img2_ssram.bin -m crc32
)
ameba_execute_process(		
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/np_img2_psram.bin -m crc32
)
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/np_img2_psram.bin ${c_SDK_IMAGE_TARGET_DIR}/np_img2_ssram.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/km4_image2_all.bin
)

if(CONFIG_BT)
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace
    )
endif()


if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace
    )
endif()


if(OFF AND NOT CONFIG_AMEBA_RLS)
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

