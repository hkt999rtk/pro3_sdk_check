set(soc_root ${c_CMPT_SOC_DIR})
set(vmodule_lib_path ${soc_root}/video/lib)
set(common_inc_path "")

list(APPEND common_inc_path
	${soc_root}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable/realtek/amebapro3
	${soc_root}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include/portable
	${soc_root}/sw/freertos_posix/lib/FreeRTOS-Plus-POSIX/include
	${soc_root}/sw/freertos_posix/lib/include/private
	${soc_root}/sw/freertos_posix/lib/include/FreeRTOS_POSIX
	${soc_root}/sw/freertos_posix/lib/include
)
get_target_property(global_inc_paths ${c_MCU_PROJ_CONFIG} INTERFACE_INCLUDE_DIRECTORIES)
list(APPEND common_inc_path ${global_inc_paths})

list(APPEND common_compile_options
    -w
)


if ((CONFIG_ARM_CORE_CM4_VP AND NOT CONFIG_OLT_VERIFY) OR
    (CONFIG_ARM_CORE_CM4_NP AND
        (CONFIG_FPGA OR
         CONFIG_PXP OR
         CONFIG_FT2_VERIFY OR
         CONFIG_OLT_VERIFY)))

include(${CMAKE_CURRENT_LIST_DIR}/video.cmake OPTIONAL)

set(public_includes)

ameba_list_append(public_includes
    ${video_include}
)

ameba_global_library(${video_soc_libs})

ameba_global_include(${public_includes})


elseif (CONFIG_ARM_CORE_CA32)

ameba_add_subdirectory(nn)
ameba_global_library(nn_ap)

endif()

if (CONFIG_ARM_CORE_CM4_VP OR CONFIG_ARM_CORE_CA32)
ameba_add_subdirectory(nn/app)
endif()
