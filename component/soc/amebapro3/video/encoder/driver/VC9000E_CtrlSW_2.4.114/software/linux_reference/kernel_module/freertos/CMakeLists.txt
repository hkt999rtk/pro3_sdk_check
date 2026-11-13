#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            VeriSilicon Inc.                                --
#-                                                                            --
#-                   (C) COPYRIGHT 2019 VeriSilicon Inc                       --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(enc_freertos_driver)

# pre global definitions
add_definitions(-D__FREERTOS__)
add_definitions(-DENC_MODULE_PATH="/tmp/dev/vsi_vcx" -DMEMALLOC_MODULE_PATH="/tmp/dev/memalloc")

string(REGEX REPLACE "(.+)\\linux_reference/kernel_module/freertos.*" "\\1" PROJECT_SW_PATH ${PROJECT_SOURCE_DIR})
set(PROJECT_RTOS_PATH   "${FREERTOS_DIR_PATH}")
set(FREERTOS_KERNEL_DIR "${PROJECT_RTOS_PATH}/FreeRTOS_Kernel/Source")
set(FREERTOS_POSIX_DIR  "${PROJECT_RTOS_PATH}/lib")
set(PROJECT_PCIE_PATH   "${PROJECT_SW_PATH}/linux_reference/kernel_module/linux")

message("software directory="        ${PROJECT_SW_PATH})
message("FreeRTOS kernel directory=" ${FREERTOS_KERNEL_DIR})
message("FreeRTOS posix directory="  ${FREERTOS_POSIX_DIR})
message("PCIE directory="            ${PROJECT_PCIE_PATH})

option(BUILD_SHARED_LIBS "Build a shared lib" OFF)

set (BUILD_MMU NO CACHE PATH "Build MMU")
set (BUILD_AXIFE NO CACHE PATH "Build AXIFE")
set (BUILD_SLICEINFO YES CACHE PATH "Build SLICE INFO")
set (BUILD_UFBC YES CACHE PATH "Build UFBC")

if (${BUILD_MMU} MATCHES "YES")
  add_definitions(-DHANTROMMU_SUPPORT)
endif()
if (${BUILD_AXIFE} MATCHES "YES")
  add_definitions(-DHANTROAXIFE_SUPPORT)
endif()

if (${BUILD_SLICEINFO} MATCHES "YES")
  add_definitions(-DLOW_LATENCY_SLICEINFO_SUPPORT)
endif()
if (${BUILD_UFBC} MATCHES "YES")
  add_definitions(-DSUPPORT_UFBC)
endif()

set(SOURCES
    frameworks.c
    io_tools.c
    memalloc_freertos.c
    user_freertos.c
    vcx_normal_driver_freertos.c
    vcx_vcmd_driver_freertos.c
    vcx_abnormal_irq_freertos.c
    vcmdswhwregisters.c
    bidirect_list_freertos.c
    $<$<BOOL:${BUILD_MMU}>:vcx_mmu_freertos.c>
    $<$<BOOL:${BUILD_AXIFE}>:${PROJECT_PCIE_PATH}/vcx_axife.c>
    )

#message("source=" ${SOURCES})

# common header files
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SW_PATH}/inc)
include_directories(${PROJECT_SW_PATH}/linux_reference/ewl)
#include_directories(${PROJECT_PCIE_PATH})

# list of used header files for FreeRTOS
# should add the related portable for your platform
include_directories(${FREERTOS_KERNEL_DIR}/include)
include_directories(${PROJECT_RTOS_PATH}/osal/portable/GCC/Posix)
#include_directories(${FREERTOS_KERNEL_DIR}/portable/ThirdParty/XCC/Xtensa)

# If you use the freertos thread based on POSIX, should be revised according to the related platform
include_directories(${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include/portable/pc/linux)
include_directories(${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include/portable)
include_directories(${FREERTOS_POSIX_DIR}/FreeRTOS-Plus-POSIX/include)
include_directories(${FREERTOS_POSIX_DIR}/include/private)
include_directories(${FREERTOS_POSIX_DIR}/include)

add_library(${PROJECT_NAME}_objlib OBJECT ${SOURCES})
set_target_properties(${PROJECT_NAME}_objlib
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(${PROJECT_NAME}_objlib PRIVATE -DSTDC_HEADERS)

target_compile_options(${PROJECT_NAME}_objlib
                       PRIVATE -Wno-unknown-pragmas -Wno-unused-parameter)
if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_C_COMPILER_VERSION VERSION_GREATER 7)
  target_compile_options(${PROJECT_NAME}_objlib
                         PRIVATE -Wno-implicit-fallthrough)
endif()
target_compile_options(${PROJECT_NAME}_objlib
                       PRIVATE -Wall -Wextra -Wsign-compare -Wformat-security)
target_compile_options(${PROJECT_NAME}_objlib
                       PRIVATE  -Wstack-protector -Winit-self)
target_compile_options(${PROJECT_NAME}_objlib
                       PRIVATE -D_FORTIFY_SOURCE=2 -O0)
target_compile_options(${PROJECT_NAME}_objlib
                       PRIVATE -fstack-protector-all)
target_compile_options(${PROJECT_NAME}_objlib
                       PRIVATE --param ssp-buffer-size=4 -ftrapv)
target_compile_options(${PROJECT_NAME}_objlib PRIVATE -fPIE -fPIC)

if(CMAKE_COMPILER_IS_GNUCC AND CMAKE_C_COMPILER_VERSION VERSION_GREATER 6)
    target_compile_options(${PROJECT_NAME}_objlib PRIVATE -mmitigate-rop)
endif()
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack -z relro -z now")

option(BUILD_ERROR_ON_WARNING "Fail compilation on warning" OFF)

if(BUILD_ERROR_ON_WARNING)
    target_compile_options(${PROJECT_NAME}_objlib PRIVATE -Werror)
endif()

target_compile_options(${PROJECT_NAME}_objlib PRIVATE $<$<CONFIG:RELEASE>:-s>)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME}_shared SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
  target_include_directories(${PROJECT_NAME}_shared PUBLIC include)
endif()
add_library(${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
target_include_directories(${PROJECT_NAME}_static PUBLIC include)
