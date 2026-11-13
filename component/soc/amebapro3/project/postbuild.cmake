#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
find_package (Python COMPONENTS Interpreter)

message( "========== Image app generate start ==========")

set(app_full_path ${c_IMAGE_OUTPUT_DIR}/amebapro3_app_nofip.bin)
set(app_final_path ${c_IMAGE_OUTPUT_DIR}/${c_APP_BINARY_NAME})

ameba_modify_file_path(${app_full_path} app_ns_full_path p_SUFFIX _ns)

# remove ap_fip.bin from c_IMAGE2_ALL_FILES list
if(CONFIG_LOADAP)
set(c_FIP_IMG "")
foreach(item IN LISTS c_IMAGE2_ALL_FILES)
    if(item MATCHES "ap_image2_fip.bin")
        list(REMOVE_ITEM  c_IMAGE2_ALL_FILES "${item}")
		set(c_FIP_IMG "${item}")
    endif()
endforeach()
endif()

if (c_IMAGE3_ALL_FILES)
    ameba_axf2bin_fw_pack(
        ${app_full_path}
        p_IMAGE2 ${c_IMAGE2_ALL_FILES}
        p_IMAGE3 ${c_IMAGE3_ALL_FILES}
    )
else()
    ameba_axf2bin_fw_pack(
        ${app_full_path}
        p_IMAGE2 ${c_IMAGE2_ALL_FILES}
    )
endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS {c_SOC_PROJECT_DIR}/fatfs.bin)
        ameba_axf2bin_pad(${app_full_path} 4096)
        ameba_axf2bin_pad(${app_ns_full_path} 4096)
        ameba_axf2bin_prepend_head(
            {c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            {c_SOC_PROJECT_DIR}/fatfs.bin
            VFS1_FLASH_BASE_ADDR
            ${c_IMAGE_OUTPUT_DIR}/target_img2.map
        )
        ameba_file_append(${app_full_path} {c_SOC_PROJECT_DIR}/fatfs_prepend.bin)
        ameba_file_append(${app_ns_full_path} {c_SOC_PROJECT_DIR}/fatfs_prepend.bin)
    endif()
endif()

# Append FIP to produce the full application binary BEFORE generating
# ota_all.bin so that ota_all.bin contains the AP FIP (TF-A).
# Without the FIP, booting from OTA2 fails because BOOT_CheckFIPImgAddr
# finds no valid IMAGE_HEADER magic after the NP image.
if(CONFIG_LOADAP AND NOT CONFIG_LINUX_FW_EN)
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_AXF2BIN_SCRIPT} --post-build-dir ${c_SOC_PROJECT_DIR} encrypt manifest -i ${c_FIP_IMG} -o ${c_IMAGE_OUTPUT_DIR}/fip_manifest.bin
)

# APPEND application.bin --- align4K---| fip_manifest.bin | app_image2_fip.bin
ameba_axf2bin_pad(${app_full_path} 4096)
ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${app_full_path} ${c_IMAGE_OUTPUT_DIR}/fip_manifest.bin ${c_FIP_IMG}
    OUTPUT_FILE ${app_final_path}
)
else()
if(CONFIG_LINUX_FW_EN)
# padding 4096 for Linux build FIP
ameba_axf2bin_pad(${app_full_path} 4096)
endif()
ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${app_full_path}
    OUTPUT_FILE ${app_final_path}
)
endif()

# Use the full application binary (including FIP) as the OTA payload.
if(CONFIG_COMPRESS_OTA_IMG)
    ameba_modify_file_path(${app_final_path} app_compress p_SUFFIX _compress)
    ameba_modify_file_path(${app_final_path} app_ns_compress p_SUFFIX _compress_ns)

    ameba_axf2bin_compress(${app_compress} ${app_final_path})
    ameba_axf2bin_compress(${app_ns_compress} ${app_final_path})
    ameba_copy_file(${app_compress} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_copy_file(${app_ns_compress} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
else()
    ameba_copy_file(${app_final_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_copy_file(${app_final_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    ameba_modify_file_path(${c_IMAGE1_ALL_FILES} boot_ns_full_path p_SUFFIX _ns)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE1_ALL_FILES})
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${boot_ns_full_path})
else()
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

ameba_remove_file(${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${c_IMAGE_OUTPUT_DIR}/*prepend.bin)

# generate freertos memory report
if(NOT CONFIG_LINUX_FW_EN AND NOT CONFIG_FT2_VERIFY)
set(mem_report_arg)
set(layout_path ${c_IMAGE_OUTPUT_DIR}/../make/image2/layout.ld)
set(np_linker_path ${c_IMAGE_OUTPUT_DIR}/../make/image2/amebapro3.ld)
set(ap_linker_path ${c_IMAGE_OUTPUT_DIR}/../../project_ap/make/image2/amebapro3.ld)
set(vp_linker_path ${c_IMAGE_OUTPUT_DIR}/../../project_vp/make/image2/amebapro3.ld)
get_filename_component(image_folder_name ${c_IMAGE_OUTPUT_DIR} NAME)
set(np_map_path ${c_IMAGE_OUTPUT_DIR}/text_image2_ns.map)
set(ap_map_path ${c_IMAGE_OUTPUT_DIR}/../../project_ap/${image_folder_name}/text_image2.map)
set(vp_map_path ${c_IMAGE_OUTPUT_DIR}/../../project_vp/${image_folder_name}/text_image2_ns.map)
ameba_list_append(mem_report_arg --layout ${layout_path} --np-linker ${np_linker_path} --np-map ${np_map_path})
ameba_list_append_if(CONFIG_LOADAP mem_report_arg --ap-linker ${ap_linker_path} --ap-map ${ap_map_path})
ameba_list_append_if(CONFIG_LOADMP mem_report_arg --vp-linker ${vp_linker_path} --vp-map ${vp_map_path})
ameba_execute_process(
	COMMAND ${Python_EXECUTABLE} ${c_SOC_PROJECT_DIR}/utils/generate_memory_report.py ${mem_report_arg}
)
endif()

ameba_copy_file(${app_final_path} ${FINAL_IMAGE_DIR})
ameba_copy_file(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${FINAL_IMAGE_DIR})
message("========== Image manipulating end ==========")
