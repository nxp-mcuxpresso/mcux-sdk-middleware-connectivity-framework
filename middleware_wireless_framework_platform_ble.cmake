#Description: wireless framework platform_ble; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_platform_ble component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_ble.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)


include(middleware_wireless_framework_platform)
include(middleware_wireless_framework_platform_coex)
