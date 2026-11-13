cmake_minimum_required(VERSION 3.20)

project(cmos_sensors C)

set(CHIP_ID "" CACHE STRING "select a chip id")
set(SENSOR_LIST "" CACHE STRING "select sensor list")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -ffreestanding -nostdlib -z max-page-size=0x1000")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(sensors_list "${SENSORS_LIST}")
set(sensors_target "" PARENT_SCOPE)
string(REPLACE "," ";" sensors_list "${sensors_list}")

if (DEFINED ISP_MCU_STAGING_DIR)
    set(PROTO_DIR ${ISP_MCU_STAGING_DIR}/include/isp-proto)
endif()

# find_program(PROTOC protoc)
# if (NOT PROTOC)
    # message(FATAL_ERROR "protoc not found!")
# endif()
if (DEFINED PROTO_DIR)
    execute_process(COMMAND ${PROTOC} -I${PROTO_DIR}
                            --python_out=${CMAKE_BINARY_DIR}
                            ${PROTO_DIR}/isp_iq_table.proto
                    RESULT_VARIABLE ret_val)
    if (NOT ret_val EQUAL 0)
        message(FATAL_ERROR "==== generate isp_iq_table_pb2.py fail ====")
    endif()
    file(COPY pack_iq.py DESTINATION ${CMAKE_BINARY_DIR} USE_SOURCE_PERMISSIONS)
endif()

macro(add_all_subdirectories)
    set(cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB children RELATIVE ${cur_dir} ${cur_dir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${cur_dir}/${child} AND
                EXISTS ${cur_dir}/${child}/CMakeLists.txt)
            add_subdirectory(${child})
            set(sensors_target ${sensors_target} PARENT_SCOPE)
        endif()
    endforeach()
endmacro()

macro(add_sensor name)
    file(GLOB source ${name}.c ${name}_patch.c)
    set(target sensor_${name})
    set(sensors_target ${sensors_target} ${target} PARENT_SCOPE)
    add_library(${target} STATIC ${source})
    target_include_directories( ${target} PUBLIC ${soc_root}/video/isp/driver/isplib/common/api )
    list(FIND sensors_list ${name} index)
    if (index GREATER -1)
        install(TARGETS ${target} ARCHIVE DESTINATION /lib)
    endif()

endmacro()


find_program(PYTHON3 python3)
if (NOT PYTHON3)
    message(FATAL_ERROR "python3 not found!")
endif()
macro(add_iq name)
    string(TOLOWER "${CHIP_ID}" chip_id)
    set(iq_proto ${CMAKE_BINARY_DIR}/isp_iq_table_pb2.py)
    set(iq_json ${CMAKE_CURRENT_SOURCE_DIR}/${name}_${chip_id}.json)
    set(iq_binary ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin)
    if (EXISTS ${iq_proto} AND EXISTS ${iq_json})
        execute_process(COMMAND ${PYTHON3} ${CMAKE_BINARY_DIR}/pack_iq.py
                                ${iq_binary}
                                ${iq_json} ${iq_json} ${iq_json} ${iq_json}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                        RESULT_VARIABLE ret_val)
        if (NOT ret_val EQUAL 0)
            message(FATAL_ERROR "==== convert iq table fail ====")
        endif()

    install(FILES ${iq_binary} DESTINATION /lib
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    endif()
endmacro()

add_all_subdirectories()
