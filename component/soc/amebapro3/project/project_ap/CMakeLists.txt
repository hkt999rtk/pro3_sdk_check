###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(ap ca32)
#ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

###############################################################################
set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)
# note!!! Keep the latest version(ap:A) at the top for release reason.
ameba_set(c_LINK_ROM_LIB ${c_SDK_LIB_SOC_DIR}/lib_rom.a)
ameba_set(c_LINK_RTOS_LIB ${c_SDK_LIB_SOC_DIR}/lib_freertos.a)

ameba_add_subdirectory(make/image2)
