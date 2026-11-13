###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(fp km0)
#ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

###############################################################################
set(CMAKE_EXECUTABLE_SUFFIX .axf)


set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)

ameba_set(c_LINK_ROM_LIB ${c_SDK_LIB_SOC_DIR}/lib_rom.a)
#NOTE: These compile options are not required for rom's component
add_compile_options(-fdata-sections -ffunction-sections)

ameba_add_subdirectory(make/image2)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)
