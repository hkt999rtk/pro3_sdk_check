cmake_minimum_required(VERSION 3.19)

enable_language(C CXX ASM)

# add module name here
if(CONFIG_ARM_CORE_CA32)
project(nn_ap)
elseif(CONFIG_ARM_CORE_CM4_VP OR CONFIG_ARM_CORE_CM4_NP)
project(nn)
else()
message(FATAL_ERROR "nn: unsupported core architecture")
endif()

#####################################

# source code
set(viplite VIPLiteDrv_2.0.0)
include(./driver/${viplite}/viplite.cmake OPTIONAL)

list(
	APPEND ${PROJECT_NAME}_sources
	${viplite_src}  # VIPLite Driver
)

file(GLOB hal_sources CONFIGURE_DEPENDS ./hal/*.c )
list(
	APPEND ${PROJECT_NAME}_sources
	${hal_sources}
)

if ( CONFIG_VERIFICATION )
list(
	APPEND ${PROJECT_NAME}_sources
	./verification/nn_verify_startup.c  
	./verification/integration_soc_tests.c
)
endif()

# include path
list(
	APPEND ${PROJECT_NAME}_include

	${viplite_inc_path}
	./app/nn_file_system
	./hal
)

# marco define
list(
	APPEND ${PROJECT_NAME}_define
	${viplite_flags}
)

# nn video mem non-cache option
if(DEFINED NNVMEMNC AND NNVMEMNC)
list(
	APPEND ${PROJECT_NAME}_define
	NNVMEMNC
)
endif()

#####################################
####       KEEP BELOW PART       ####
#####################################

# include(${CMAKE_SOURCE_DIR}/cmake/libversion_file.cmake)

add_library(
	${PROJECT_NAME} STATIC

	${${PROJECT_NAME}_sources}
)

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} --prefix-alloc-sections .bf $<TARGET_FILE:${PROJECT_NAME}>
	COMMENT "add prefix to sections of ${PROJECT_NAME} with prefix .bf"
)

list(
	APPEND ${PROJECT_NAME}_flags
	
	${common_define}

	${${PROJECT_NAME}_define}	
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_flags} )
target_compile_options(${PROJECT_NAME} PRIVATE ${LIBS_WARN_ERR_FLAGS} ${common_compile_options})

target_include_directories(
	${PROJECT_NAME}
	PRIVATE

	${common_inc_path}
	
	${${PROJECT_NAME}_include}
	
	${soc_root}/fwlib/include
	${soc_root}/fwlib/include/cmsis
	${soc_root}/fwlib/include/rom	
)

target_link_options(
	${PROJECT_NAME}
	PRIVATE

	${common_link_wrap}
)

# include(${CMAKE_SOURCE_DIR}/cmake/updatebsp.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/libversion_exec.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${vmodule_lib_path})