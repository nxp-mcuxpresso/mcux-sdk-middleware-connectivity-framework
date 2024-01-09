#Description: wireless framework platform_lowpower_timer; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_platform_lowpower_timer component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)


include(middleware_wireless_framework_platform)
