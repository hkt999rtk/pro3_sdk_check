ameba_internal_library(example_bridge_dispatch)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    app_example.c
    example_bridge_dispatch.c
)
