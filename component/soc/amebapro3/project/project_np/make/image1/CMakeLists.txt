ameba_add_image(target_loader p_IMAGE_ALL km4_boot_all.bin)
# Use variable c_CURRENT_IMAGE to represent full image target name

message(STATUS "<TARGET> PRO3 bootloader  : ${c_CURRENT_IMAGE}")
###############################  ADD COMPONENT  ###################################

add_compile_definitions(
	CONFIG_LINK_ROM_SYMB
	CONFIG_BOOTLOADER
)
set(CONFIG_BOOTLOADER TRUE)

ameba_add_subdirectory(${c_CMPT_SOC_DIR}/loader/bootloader)
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
#ameba_add_subdirectory(${c_CMPT_LZMA_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/lib/bootloader lib_bootloader)
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR})

###############################  GENERATE IMAGE  ###################################

#---------------- PRE LINK -------------------#
if(CONFIG_DISABLE_PSRAM)
message("[WARNING] [WARNING] select DISALBE PSRAM only for Verification")
endif()
set(ameba_img1_ld ${c_SDK_LD_DIR}/ameba_img1_all.ld)

add_custom_command(
    TARGET ${c_CURRENT_IMAGE} PRE_LINK
	#COMMAND ${CMAKE_COMMAND} -E copy ${ameba_img1_ld} ${CMAKE_CURRENT_BINARY_DIR}/amebapro3.ld
	COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${ameba_img1_ld} -o ${CMAKE_CURRENT_BINARY_DIR}/amebapro3.ld -I ${c_CMPT_FWLIB_DIR}/include -I ${c_MENUCONFIG_DIR}/project_np
	COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_LD_DIR}/rom_symbol_acut.ld ${CMAKE_CURRENT_BINARY_DIR}/rom_symbol.ld
	#COMMAND ${CMAKE_COMMAND} -E copy ${ameba_layout_ld} ${CMAKE_CURRENT_BINARY_DIR}/layout.ld
	COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_LAYOUT_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/layout.ld -I ${c_CMPT_FWLIB_DIR}/include -I ${c_MENUCONFIG_DIR}/project_np
    COMMENT "build bootloader linkerscript amebapro3.ld"
)


#----------------- LINKING -------------------#
target_link_options(${c_CURRENT_IMAGE}
	PRIVATE
	"LINKER:SHELL:-L ${CMAKE_CURRENT_BINARY_DIR}"
	"LINKER:SHELL:-T${CMAKE_CURRENT_BINARY_DIR}/amebapro3.ld"
	"LINKER:SHELL:-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map"
	"LINKER:SHELL:-u RamStartTable"
	"LINKER:SHELL:-u RAM_IMG1_VALID_PATTEN"
)

target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
	
	
    #-T${CMAKE_CURRENT_BINARY_DIR}/amebapro3.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    #-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    -Wl,--no-whole-archive

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>
)

#--------------- POST BUILD ------------------#
ameba_get_image_target_name(gdb_floader floader_target)
if(TARGET ${floader_target}_postbuild)
    set(DEP_GDB_FLOADER ${floader_target}_postbuild)
endif()

add_custom_command(OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/${boot_name}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${c_SDK_IMAGE_TARGET_DIR}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy ${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      ${boot_name}"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_PROJECT_NAME=${c_MCU_PROJECT_NAME} # name of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE} # file of current project's kconfig file
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output
		-Dc_BOOT_BINARY_NAME=${boot_name} # name of boot image
		
        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}
        -DEXTERN_DIR=${EXTERN_DIR}

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/${c_POST_BUILD_SCRIPT}
    DEPENDS ${c_CURRENT_IMAGE} ${DEP_GDB_FLOADER}
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/${boot_name}
)


################################  EXTRA PROCESS  ###################################

set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/${boot_name}
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_SDK_LD_DIR}/ameba_img1_all.ld
    ${c_LINK_ROM_SYMBOL_S}
    ${c_LINK_ROM_LIB}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)

################################  CUSTOM TARGET  ###################################

add_custom_target(
    boot
    COMMENT "build bootloader only"
    DEPENDS ${c_CURRENT_IMAGE}
)
