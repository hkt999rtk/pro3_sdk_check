##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin
file(GLOB globbed_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*.c)

foreach(filepath IN LISTS globbed_files)
    get_filename_component(filename_only "${filepath}" NAME)
    list(APPEND all_folder_srcs "${filename_only}")
endforeach()

ameba_list_append(private_sources
    ${all_folder_srcs}
)

ameba_list_append(private_includes
	${c_CMPT_SOC_DIR}/video/nn/app/test_model
	${c_CMPT_SOC_DIR}/video/nn/app/nn_utils
	${c_CMPT_SOC_DIR}/video/nn/app/img_process
	${c_CMPT_SOC_DIR}/video/nn/app
	${c_CMPT_SOC_DIR}/video/ts/verification
	${c_CMPT_SOC_DIR}/video/ge/hal
    
    ${c_CMPT_SOC_DIR}/sw/media/mmfv2
    ${c_CMPT_SOC_DIR}/sw/media/mmfv2/module
    
    ${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include/nbg_linker
	${c_CMPT_SOC_DIR}/video/nn/driver/VIPLiteDrv_${v_NN_VER}/include

	${c_COMPONENT_DIR}/soc/common/cmsis-dsp/Include
)

ameba_list_append(private_compile_options -mfp16-format=ieee)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
ameba_add_internal_library(test_model
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
