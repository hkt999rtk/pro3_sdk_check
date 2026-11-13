cmake_minimum_required(VERSION 3.19)

macro(add_vmodule mo config_ena module_name_for_verify)
	if(${${config_ena}})
		# message(STATUS "ADD VMODULE ${mo}")
		# Set CONFIG_VERIFICATION for this module's CMakeLists.txt
		set(CONFIG_VERIFICATION OFF)
		if(CONFIG_VMODULE_VERIFY AND DEFINED CONFIG_VMODULE_VERIFICATION_${module_name_for_verify})
			set(CONFIG_VERIFICATION ${CONFIG_VMODULE_VERIFICATION_${module_name_for_verify}})
		endif()

		# ADD new module subdirectory
		# add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/<module name>  video_<module name>.out
		add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/${mo} video_${mo}.out )
		# Add link libraries
		list(
			APPEND video_soc_libs
			${mo}
		)
	endif()
endmacro() 

macro(add_vmodule_prebuilt mo config_ena lib_path)
	if(${${config_ena}})
	# message(STATUS "ADD PREBUILT VMODULE ${mo} from ${lib_path}")
	# Add the pre-built library to link libraries
	list(
		APPEND video_soc_libs
		${lib_path}
	)
	endif()
endmacro()

macro(vmodule_kconfig)
	if(DEFINED ${ARGV0})
		message(STATUS "${ARGV0}=${${ARGV0}} [KCONFIG]")
	else()
		set(${ARGV0} OFF)
		message(STATUS "${ARGV0}=OFF [NOT DEFINED, FORCE OFF]")
	endif()
endmacro()

#### Keep Above unchanged #####
###############################

################################################################################
# Define video modules list
set(VIDEO_MODULE_LIST
    ADDREMAP
    AINR
    ENCODER
    GE
    ISP
    SWISP2ENC
    NN
    SENSOR
    TEMPLATE
    TS
)

# Define which modules use prebuilt libraries (ON = prebuilt, OFF = build from source)
set(VIDEO_MODULE_PREBUILT_MAP
    "ADDREMAP:ON"
    "AINR:ON"
    "ENCODER:ON"
    "GE:OFF"
    "ISP:OFF"
    "SWISP2ENC:ON"
    "NN:OFF"
    "SENSOR:OFF"
    "TEMPLATE:OFF"
    "TS:ON"
)
################################################################################

# Parse which modules use prebuilt from map
foreach(prebuilt_pair ${VIDEO_MODULE_PREBUILT_MAP})
    string(REPLACE ":" ";" prebuilt_parts ${prebuilt_pair})
    list(GET prebuilt_parts 0 module_name)
    list(GET prebuilt_parts 1 prebuilt_default)

    if(NOT DEFINED VMODULE_${module_name}_PREBUILT)
        set(VMODULE_${module_name}_PREBUILT ${prebuilt_default})
    endif()
endforeach()

# Configure video modules from kconfig
foreach(module_name ${VIDEO_MODULE_LIST})
    string(TOLOWER ${module_name} module_dir)

    vmodule_kconfig(CONFIG_VMODULE_${module_name})

    # Check if module use prebuilt library or build from source
    if(CONFIG_VMODULE_${module_name})
        if(VMODULE_${module_name}_PREBUILT)
            # Use prebuilt library
            set(lib_path ${CMAKE_CURRENT_LIST_DIR}/lib/lib${module_dir}.a)
            add_vmodule_prebuilt(${module_dir} CONFIG_VMODULE_${module_name} ${lib_path})
        else()
            # Build from source
            add_vmodule(${module_dir} CONFIG_VMODULE_${module_name} ${module_name})
        endif()

        # Store mapping for linker test command use
        set(video_module_name_${module_dir} ${module_name})
    endif()
endforeach()

# Configure video module verification flag
foreach(module_name ${VIDEO_MODULE_LIST})
    string(TOLOWER ${module_name} module_dir)

    if(CONFIG_VMODULE_VERIFY)
        if(DEFINED VMODULE_VERIFICATION_${module_name})
            set(CONFIG_VMODULE_VERIFICATION_${module_name} ${VMODULE_VERIFICATION_${module_name}})
            unset(VMODULE_VERIFICATION_${module_name} CACHE)
            message(STATUS "CONFIG_VMODULE_VERIFICATION_${module_name}=${CONFIG_VMODULE_VERIFICATION_${module_name}} [COMMAND LINE]")
        else()
            if(DEFINED CONFIG_VMODULE_VERIFICATION_${module_name})
                message(STATUS "CONFIG_VMODULE_VERIFICATION_${module_name}=${CONFIG_VMODULE_VERIFICATION_${module_name}} [KCONFIG]")
            else()
                set(CONFIG_VMODULE_VERIFICATION_${module_name} OFF)
                message(STATUS "CONFIG_VMODULE_VERIFICATION_${module_name}=OFF [NOT DEFINED, FORCE OFF]")
            endif()
        endif()
    endif()
endforeach()

##############################
#### Keep Below unchanged ####
# Generate linker test command flags for modules with verification enabled
set(link_video_test_cmd "")
if(CONFIG_VMODULE_VERIFY)
    foreach(module_dir IN LISTS video_soc_libs)
        set(module_name ${video_module_name_${module_dir}})
        if(CONFIG_VMODULE_VERIFICATION_${module_name})
            list(APPEND link_video_test_cmd "LINKER:SHELL:-u video_${module_dir}_test_cmd")
        endif()
    endforeach()
endif()

if( DEFINED JKSVERIFY )
	unset(JKSVERIFY CACHE)
endif()
