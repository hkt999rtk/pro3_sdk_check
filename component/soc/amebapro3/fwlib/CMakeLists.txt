
ameba_global_include(
    include
    include/rom
    ../rom/include
    ${c_CMPT_SWLIB_DIR}
    ${c_COMPONENT_DIR}/soc/common/include
    #${c_COMPONENT_DIR}/soc/common/include/cmsis
	include/cmsis
)

ameba_add_subdirectory(ram_common)
ameba_add_subdirectory(${c_COMPONENT_DIR}/soc/common/rtk_assert)

if("${c_MCU_PROJECT_NAME}" STREQUAL "fp")
    ameba_add_subdirectory(ram_fp)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "np")
    ameba_add_subdirectory(ram_np)
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "vp")
    ameba_add_subdirectory(ram_vp)	
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
    ameba_add_subdirectory(ram_ap)
endif()
