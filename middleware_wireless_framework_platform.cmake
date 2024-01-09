#Description: wireless framework Platform; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_platform component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/platform/include
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs
)


include(middleware_wireless_framework_fwk_debug)
