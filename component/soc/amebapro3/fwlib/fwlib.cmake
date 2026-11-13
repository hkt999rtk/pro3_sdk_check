
set(sub_component_name fwlib)	# <- setup component name

############################## SOURCE LIST/COMPILE OPTION/DEFINE/INCLUDE PATH ##############################

file(GLOB fwlib_sources CONFIGURE_DEPENDS ${soc_root}/fwlib/*.c )

list(
	APPEND ${sub_component_name}_src
	
	${fwlib_sources}
)	

list(
	APPEND ${sub_component_name}_inc_path
	
	${soc_root}/fwlib/include
)

list(
	APPEND ${sub_component_name}_define
	
)

list(
	APPEND ${sub_component_name}_option

) 

############################## DO NOT EDIT BELOW SECTION ##############################
set_property( SOURCE ${${sub_component_name}_src} APPEND PROPERTY COMPILE_DEFINITIONS ${${sub_component_name}_define})
set_property( SOURCE ${${sub_component_name}_src} APPEND PROPERTY COMPILE_OPTIONS ${${sub_component_name}_option})

list(
	APPEND ${component_name}_src
	
	${${sub_component_name}_src}
)

list(
	APPEND ${component_name}_inc_path
	
	${${sub_component_name}_inc_path}
)
unset(sub_component_name)