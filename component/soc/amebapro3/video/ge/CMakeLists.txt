cmake_minimum_required(VERSION 3.19)

enable_language(C CXX ASM)

# add module name here
project(ge)

#####################################

# source code

# mehtod 1 manual list 

# method 2 glob all *.c
file( GLOB hal_sources CONFIGURE_DEPENDS ./hal/*.c )
list(
    APPEND ${PROJECT_NAME}_sources
	${hal_sources}
)
file( GLOB drv_sources CONFIGURE_DEPENDS ./driver/*.c )
list(
	APPEND ${PROJECT_NAME}_sources
	${drv_sources}
)
file( GLOB app_sources CONFIGURE_DEPENDS ./app/*.c )
list(
	APPEND ${PROJECT_NAME}_sources
	${app_sources}
)

if ( CONFIG_VERIFICATION )
list(
    APPEND ${PROJECT_NAME}_sources
	./verification/ge_verify.c  
	./verification/dbg_blit.c  
	./verification/dbg_zoom.c  
	./verification/dbg_ge.c  
	./verification/crc.c  
	
)
endif()

# include path
list(
	APPEND ${PROJECT_NAME}_include

	./driver
	./hal
	./app
	../../../../file_system/vfs
)

# marco define
list(
	APPEND ${PROJECT_NAME}_define
	
	USER_DEFINE_WITH_VALUE=1
	USER_DEFINE
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

target_compile_definitions(${PROJECT_NAME} PRIVATE ${${CMAKE_PROJECT_NAME}_flags} )
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

# include(${CMAKE_SOURCE_DIR}/cmake/updatebsp.cmake)
# include(${CMAKE_SOURCE_DIR}/cmake/libversion_exec.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${vmodule_lib_path})