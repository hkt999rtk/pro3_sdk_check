ameba_internal_library(example_wowlan_tickless)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    app_example.c
    example_wowlan_tickless.c
)
