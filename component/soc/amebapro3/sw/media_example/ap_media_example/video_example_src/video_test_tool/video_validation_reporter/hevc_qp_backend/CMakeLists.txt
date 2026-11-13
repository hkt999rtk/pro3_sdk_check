cmake_minimum_required(VERSION 3.20)
project(video_qp_backend LANGUAGES C CXX)
include(FetchContent)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ENABLE_SDL OFF CACHE BOOL "" FORCE)
set(ENABLE_DECODER OFF CACHE BOOL "" FORCE)
set(ENABLE_ENCODER OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(libde265
    URL https://github.com/strukturag/libde265/archive/refs/tags/v1.0.16.tar.gz
    URL_HASH SHA256=ed12c931759c1575848832f70db5071a001ac813db4e4f568ee08aef6e234d4e
)
FetchContent_MakeAvailable(libde265)
target_sources(de265 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/qp_export.cc")
target_compile_features(de265 PRIVATE cxx_std_11)
set_target_properties(de265 PROPERTIES OUTPUT_NAME de265_qp)

set_target_properties(de265 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_include_directories(de265 PRIVATE "${libde265_SOURCE_DIR}")
