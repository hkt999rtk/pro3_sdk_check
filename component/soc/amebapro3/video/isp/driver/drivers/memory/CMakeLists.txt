if(ISP_CPU_MODE)
    set(source
        memory_cpu.c
    )
else()
    set(source
        memory.c
    )
endif()

set(target driver_memory)
add_library(${target} OBJECT ${source})
target_include_directories(${target} PRIVATE ${drivertopdir}/connector)
target_link_libraries(${target} PRIVATE drivers_common_interface)

set_property(GLOBAL APPEND PROPERTY driver_memory_objs $<TARGET_OBJECTS:${target}>)
