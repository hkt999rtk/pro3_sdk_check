#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(t_USER_CUSTOM_LOG_PREFIX "NP_POSTBUILD")

find_package (Python COMPONENTS Interpreter)

message( "========== Image converting start ==========")
ameba_execute_process( p_SHOW_OUTPUT
    COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/elfconv.py -m boot -e ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf -j ${CMAKE_CURRENT_LIST_DIR}/bootloader_conv.json -o ${c_SDK_IMAGE_TARGET_DIR}/km4_boot.bin
)

message( "========== Image packing start ==========")
ameba_axf2bin_fw_pack(
	${c_SDK_IMAGE_TARGET_DIR}/${c_BOOT_BINARY_NAME}
	p_IMAGE1
	${c_SDK_IMAGE_TARGET_DIR}/km4_boot.bin
)

message( "========== Image postprocessing end ==========")

if (FINAL_IMAGE_DIR)
	set(AP_BOOT_ALL ${c_SDK_IMAGE_TARGET_DIR}/${c_BOOT_BINARY_NAME})
    if(EXISTS ${AP_BOOT_ALL})
		ameba_execute_process(
			COMMAND ${CMAKE_COMMAND} -E copy ${AP_BOOT_ALL} ${FINAL_IMAGE_DIR}
		)
	endif()
endif()

