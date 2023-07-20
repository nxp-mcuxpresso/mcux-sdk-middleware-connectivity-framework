set(SOURCES
    fwk_platform.c
    fwk_platform_flash.c
)

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
    configs
)

add_library(connfwk-platform-${CONNFWK_PLATFORM} ${SOURCES})

# Making those includes PUBLIC will share them to the other targets linking this lib
target_include_directories(connfwk-platform-${CONNFWK_PLATFORM} PUBLIC ${INCLUDES})

# Get common configs from the connfwk-config interface
target_link_libraries(connfwk-platform-${CONNFWK_PLATFORM} PRIVATE connfwk-config connfwk-platform-${CONNFWK_PLATFORM_FAMILY})

if(PROJECT_IS_TOP_LEVEL)
    connfwk_target_set_linker_script(connfwk-platform-${CONNFWK_PLATFORM} PUBLIC ${CONNFWK_PLATFORM_LINKER_FILE})
    # Rebuild executables if the linker script is changed
    set_target_properties(connfwk-config PROPERTIES INTERFACE_LINK_DEPENDS ${CONNFWK_PLATFORM_LINKER_FILE})
    connfwk_target_generate_map_file(connfwk-platform-${CONNFWK_PLATFORM} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_PROPERTY:NAME>.map)
endif()
