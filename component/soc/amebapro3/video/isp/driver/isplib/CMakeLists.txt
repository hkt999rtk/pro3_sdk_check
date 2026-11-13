option(MCU_PROJECT "build mcu firmware library" ON)

set(CHIP_ID "RTS3921" CACHE STRING "select a chip id")
set(TARGET_OS "FreeRTOS" CACHE STRING "set build target OS Linux/FreeRTOS")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(isptopdir ${CMAKE_CURRENT_SOURCE_DIR})

set(target isp_common_interface)
add_library(${target} INTERFACE)
target_include_directories(${target} INTERFACE common/api)
target_include_directories(${target} INTERFACE common/common)
target_include_directories(${target} INTERFACE common/oslayer)
target_compile_features(${target} INTERFACE c_std_99)
target_compile_options(${target} INTERFACE -Wall -Werror)
target_compile_options(${target} INTERFACE -ffile-prefix-map=${isptopdir}=/isplib)

if(MCU_PROJECT)
    include(mcu_lib.cmake)
else()
    include(cpu_lib.cmake)
    file(GLOB headers "common/api/*.h")
    install(FILES ${headers} DESTINATION include)
endif()
