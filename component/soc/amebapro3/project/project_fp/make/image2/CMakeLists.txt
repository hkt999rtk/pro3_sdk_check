ameba_add_image(target_img2 p_IMAGE_ALL km0_image2_all.bin)
# Use variable c_CURRENT_IMAGE to represent full image target name

message(STATUS "<TARGET> PRO3 FP Image2   :${c_CURRENT_IMAGE}")
#use ROM code as lib
set(BAREMETAL ON)
set(CONFIG_SUPPORT_ATCMD OFF)
###############################  ADD COMPONENT  ###################################
add_compile_definitions(
	CONFIG_CPUS_NUM=1
)

ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_SOC_DIR}/rom)
#ameba_add_subdirectory(${c_CMPT_SOC_DIR}/loader/bootloader)
#ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/sw/os)
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/osportable)
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
#ameba_add_subdirectory(${c_CMPT_PMC_DIR})
ameba_add_subdirectory(${c_CMPT_MONITOR_DIR})
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR} usrcfg)
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_SWLIB_DIR})
#ameba_add_subdirectory(${c_CMPT_MISC_DIR})
#ameba_add_subdirectory(${c_CMPT_DIAGNOSE_DIR})

#ameba_add_subdirectory(${c_MCU_PROJECT_DIR}/src)
ameba_add_subdirectory(${c_MCU_SRC_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/init/fp)
ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_fw)
ameba_add_subdirectory(${c_CMPT_WIFI_DIR})

ameba_add_subdirectory(${c_CMPT_SOC_DIR}/lib/ram_fp)
#ameba_add_subdirectory_if_exist(${c_CMPT_SOC_DIR}/rom)
###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#

if(CONFIG_DISABLE_PSRAM)
message("[WARNING] [WARNING] select DISALBE PSRAM only for Verification")
endif()

add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  
  #COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_SDK_LD_DIR}/ameba_img2_sram.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  #COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  #COMMAND ${CMAKE_COMMAND} -E copy ${ameba_layout_ld} ${CMAKE_CURRENT_BINARY_DIR}/layout.ld 
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_LAYOUT_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/layout.ld -I ${c_CMPT_FWLIB_DIR}/include -I ${c_MENUCONFIG_DIR}/project_fp
  COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_LD_DIR}/ameba_img2_sram.ld ${CMAKE_CURRENT_BINARY_DIR}/amebapro3.ld
  COMMENT "build img2 linkerscript amebapro3.ld"
)
#----------------- LINKING -------------------#
target_link_options(${c_CURRENT_IMAGE}
	PRIVATE

	"LINKER:SHELL:-L ${CMAKE_CURRENT_BINARY_DIR}"
	"LINKER:SHELL:-T${CMAKE_CURRENT_BINARY_DIR}/amebapro3.ld"
	"LINKER:SHELL:-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map"
)
target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    #${SDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.base/libm.a
    -Wl,--no-whole-archive
	
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>
    -lm -lstdc++
    ${c_LINK_ROM_LIB} #WARNING: keep this at last
)

#--------------- POST BUILD ------------------#
add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_DIR}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm
    # COMMAND ${CMAKE_OBJDUMP} -h -S --disassemble-all ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.map

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km0_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE} # file of current project's kconfig file
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output

        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}
        -DEXTERN_DIR=${EXTERN_DIR}

        # special variables for current image

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/${c_POST_BUILD_SCRIPT}

    DEPENDS ${c_CURRENT_IMAGE}
    COMMENT "${c_CURRENT_IMAGE} post build"
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
)

################################  EXTRA PROCESS  ###################################
set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_LINK_ROM_SYMBOL}
    ${c_SDK_LD_DIR}/ameba_img2_sram.ld
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)
