##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)
set(public_definitions)
set(public_libraries)

ameba_list_append(public_includes
	${CMAKE_CURRENT_LIST_DIR}
)

ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

##########################################################################################
## * This part defines private part of the component

set(private_sources)
set(private_includes)
set(private_definitions)
set(private_compile_options)

ameba_list_append(private_sources
	video_osd.c
	video_osd_font.c
	video_osd_polygon.c
	video_osd_compositor.c
	video_osd_privacy.c
	video_osd_stream.c
	video_osd_plan.c
)

ameba_list_append(private_includes
	${c_CMPT_SOC_DIR}/sw/video_sw/video_api_ap
	${c_CMPT_SOC_DIR}/sw/video_sw/video_com_inc
)

ameba_add_internal_library(video_osd
	p_SOURCES
		${private_sources}
	p_INCLUDES
		${private_includes}
	p_DEFINITIONS
		${private_definitions}
	p_COMPILE_OPTIONS
		${private_compile_options}
)
##########################################################################################
