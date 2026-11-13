
file(GLOB globbed_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*.c)
foreach(filepath IN LISTS globbed_files)
    get_filename_component(filename_only "${filepath}" NAME)
    list(APPEND all_folder_srcs "${filename_only}")
endforeach()

ameba_list_append(private_sources
    ${all_folder_srcs}
)

ameba_list_append(private_includes
    ${c_CMPT_SOC_DIR}/sw/media/mmfv2
)

ameba_list_append(private_compile_options)
