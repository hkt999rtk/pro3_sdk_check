###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(np km4)
ameba_mcu_project_check()

###############################################################################

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

###############################################################################
set(CMAKE_EXECUTABLE_SUFFIX .axf)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)

set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)


if(CONFIG_AMEBAPRO3_A_CUT)
    ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/${c_SOC_TYPE}_rom_acut)
    ameba_set(c_FLOADER_BIN_DIR ${c_SDK_IMGTOOL_FLOADER_DIR}/${c_SOC_TYPE}_acut)
elseif(CONFIG_AMEBAPRO3_B_CUT)
	ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/${c_SOC_TYPE}_rom_bcut)
    ameba_set(c_FLOADER_BIN_DIR ${c_SDK_IMGTOOL_FLOADER_DIR}/${c_SOC_TYPE}_bcut)
else()
    ameba_fatal("CONFIG_AMEBAPRO3_A_CUT not defined as TRUE")
endif()

#if(CONFIG_LINK_ROM_SYMB)
#    ameba_unset(c_LINK_ROM_LIB)
#    ameba_unset(c_LINK_ROM_LIB_NS)
#    if(CONFIG_AMEBASMART_A_CUT)
#        ameba_set(c_LINK_ROM_SYMBOL_NS    ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
#        ameba_set(c_LINK_ROM_SYMBOL_S     ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_s.ld)
#    endif()
#else()
#    ameba_set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
#    ameba_set(c_LINK_ROM_LIB_NS ${c_ROM_LIB_DIR}/rom_ns.a)
#endif()

ameba_set(c_LINK_ROM_LIB ${c_SDK_LIB_SOC_DIR}/lib_rom.a)

ameba_add_subdirectory_if_exist(make/image_rom)

#NOTE: These compile options are not required for rom's component
add_compile_options(-fdata-sections -ffunction-sections)

#ameba_add_subdirectory_if(CONFIG_FPGA make/image_gdb_floader)
ameba_add_subdirectory_ifnot(CONFIG_AMEBA_RLS make/image_imgtool_floader)
ameba_add_subdirectory(make/image1)
ameba_add_subdirectory(make/image2)

if(CONFIG_TRUSTZONE)
    ameba_add_subdirectory(make/image3)
    ameba_get_image_target_name(image2 image2_name)
    ameba_get_image_target_name(image3 image3_name)
    add_dependencies(${image2_name} ${image3_name}_postbuild)
endif()

#ameba_add_subdirectory(CONFIG_FPGA make/verify_image_sram_gdb)
if(NOT CONFIG_AMEBA_RLS)
    ameba_add_subdirectory_if(CONFIG_FPGA make/verify_image_fpga_bare)
    ameba_add_subdirectory_if(CONFIG_PXP make/verify_image_pxp_bare)
endif()

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)

add_custom_target(
    gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Note: please use command :./build.py -gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " -------------   HELP   -----------------"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -gdb for loading image with gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -debug for debugging with gdb"
)

execute_process(
    COMMAND ${c_AXF2BIN_RUN} --post-build-dir ${c_SOC_PROJECT_DIR} helper query-manifest --key sboot_enable
    OUTPUT_VARIABLE _sboot_enabled
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(_sboot_enabled STREQUAL "ON" AND EXISTS ${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin)
    execute_process(COMMAND ${op_CUT} -i ${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin -o ${CMAKE_CURRENT_BINARY_DIR}/ram_1_prepend.bin -l 4096 -B)
    execute_process(COMMAND ${op_FW_PACKAGE} --imgtool-floader ${CMAKE_CURRENT_BINARY_DIR}/ram_1_prepend.bin -o ${CMAKE_CURRENT_BINARY_DIR}/imgtool_flashloader.bin)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/imgtool_flashloader.bin ${FINAL_IMAGE_DIR}/floader_${c_SOC_TYPE}.bin)
endif()
