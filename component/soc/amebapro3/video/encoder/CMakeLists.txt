cmake_minimum_required(VERSION 3.19)

enable_language(C CXX ASM)

# add module name here
project(encoder)

#####################################

# source code
set (vc9000e VC9000E_CtrlSW_2.4.114)
set (enc_root ./driver/${vc9000e})
include(${enc_root}/videoenc.cmake)

list(
	APPEND ${PROJECT_NAME}_sources
	${venc_src}
	./driver/enc_sys_api.c
)

file(GLOB hal_sources CONFIGURE_DEPENDS ./hal/*.c )
list(
	APPEND ${PROJECT_NAME}_sources
	${hal_sources}
)

if ( CONFIG_VERIFICATION )
list(
	APPEND ${PROJECT_NAME}_sources
	./verification/enc_verify_startup.c  
	./verification/enc_verify_startup2.c  
	./verification/enc_simple_demo.c
	./verification/enc_simple_utils.c
	./verification/enc_simple_configs.c
)
endif()

# include path
list(
	APPEND ${PROJECT_NAME}_include
	${venc_inc_path}
	./driver
	./hal
)

# marco define
list(
	APPEND ${PROJECT_NAME}_define
	${venc_flags}
)

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