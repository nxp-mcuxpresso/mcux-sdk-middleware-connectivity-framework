#Description: wireless framework Platform sensors; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_platform_sensors component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_sensors.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)


include(middleware_wireless_framework_platform)
