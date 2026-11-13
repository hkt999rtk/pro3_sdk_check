cmake_minimum_required(VERSION 3.12)

option(ISP_CPU_MODE "ISP is used by CPU" OFF)
project(isp_mcu_fw C)

add_subdirectory(kernel)
add_subdirectory(drivers)
add_subdirectory(isplib)
add_subdirectory(apps)
