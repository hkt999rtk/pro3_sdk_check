set(protos
    isp_iq_table.proto
    isp_statis.proto
    isp_algo_param.proto
)
find_program(PYTHON3 python3)

if(NOT PYTHON3)
    message(FATAL_ERROR "python3 not found!")
endif()

foreach(proto ${protos})
    get_filename_component(name ${proto} NAME)
    execute_process(COMMAND ${PYTHON3} nanopb/generator/nanopb_generator.py ${name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE ret_val)

    if(NOT ret_val EQUAL 0)
        message(FATAL_ERROR "==== generate c code for ${name} fail ====")
    endif()
endforeach(proto)

set(source
    isp_algo_param.pb.c
    isp_iq_table.pb.c
    isp_statis.pb.c
    nanopb/pb_common.c
    nanopb/pb_decode.c
    nanopb/pb_encode.c
)
set(target isp_server_proto)
add_library(${target} OBJECT ${source})
target_link_libraries(${target} PRIVATE isp_common_interface)
target_link_libraries(${target} PRIVATE isp_server_interface)

set_property(GLOBAL APPEND PROPERTY isp_server_objs $<TARGET_OBJECTS:${target}>)

install(FILES nanopb/pb.h DESTINATION ${ISP_MCU_STAGING_DIR}/include)
install(FILES isp_iq_table.pb.h DESTINATION ${ISP_MCU_STAGING_DIR}/include)
install(FILES isp_iq_table.proto DESTINATION ${ISP_MCU_STAGING_DIR}/include/isp-proto/)
install(FILES isp_statis.proto DESTINATION ${ISP_MCU_STAGING_DIR}/include/isp-proto/)
install(FILES isp_algo_param.pb.h DESTINATION ${ISP_MCU_STAGING_DIR}/include)
install(FILES isp_algo_param.proto DESTINATION ${ISP_MCU_STAGING_DIR}/include/isp-proto/)
