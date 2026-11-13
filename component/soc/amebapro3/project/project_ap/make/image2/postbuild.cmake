#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(v_USER_CUSTOM_LOG_PREFIX "AP_POSTBUILD")

find_program(MAKE_EXE NAMES gmake nmake make)
find_package (Python COMPONENTS Interpreter)

ameba_execute_process(
	COMMAND ${CMAKE_OBJCOPY} -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
)

if(CONFIG_BT)
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace
    )
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ca32_image2_all.bin
)

message( "========== Image manipulating start ==========")

# Call make -C atf image to generate fip.bin after ca32_image2_all.bin is generated
# TODO: need to catch the error
ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf img"
    COMMAND ${CMAKE_COMMAND} -E env ${MAKE_EXE} -j CROSS_COMPILE=${CROSS_COMPILE} PROJECT_DIR=${c_SOC_PROJECT_DIR}/project_ap BUILD_BASE=${BUILD_BASE} BL33=${c_SDK_IMAGE_TARGET_DIR}/ca32_image2_all.bin -C ${c_BASEDIR}/component/soc/${c_SOC_TYPE}/atf fip image
    WORKING_DIRECTORY ${c_BASEDIR}/component/soc/${c_SOC_TYPE}/atf
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl1.bin ${c_SDK_IMAGE_TARGET_DIR}/bl1.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/fip.bin ${c_SDK_IMAGE_TARGET_DIR}/fip.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl1/bl1.dump ${c_SDK_IMAGE_TARGET_DIR}/bl1.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl2/bl2.dump ${c_SDK_IMAGE_TARGET_DIR}/bl2.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl32/bl32.dump ${c_SDK_IMAGE_TARGET_DIR}/bl32.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl1/bl1.map ${c_SDK_IMAGE_TARGET_DIR}/bl1.map
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl2/bl2.map ${c_SDK_IMAGE_TARGET_DIR}/bl2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/make/atf/bl32/bl32.map ${c_SDK_IMAGE_TARGET_DIR}/bl32.map
)

ameba_execute_process( p_SHOW_OUTPUT
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/prepend_header.py ${CMAKE_CURRENT_BINARY_DIR}/layout.ld  AP_PSRAM_BL1 ${c_SDK_IMAGE_TARGET_DIR}/bl1.bin ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin 0x04040404
)

ameba_execute_process( p_SHOW_OUTPUT
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/prepend_header.py ${CMAKE_CURRENT_BINARY_DIR}/layout.ld  AP_DDR_FIP ${c_SDK_IMAGE_TARGET_DIR}/fip.bin ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin	0x0404ffff
)

if(CONFIG_LOAD_CHECKSUM)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin -m sum
)
ameba_execute_process(		
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin -m sum
)
endif()

if(CONFIG_LOAD_CRC32)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin -m crc32
)
ameba_execute_process(		
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/calchecksum.py -f ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin -m crc32
)
endif()

#ameba_execute_process(
#    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin
#    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin
#)
ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin 
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image2_fip.bin
)


#if(NOT CONFIG_AMEBA_RLS AND NOT CONFIG_CP_TEST_CA32)
if(OFF)
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
