set(SOURCES
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
