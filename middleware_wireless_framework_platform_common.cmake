#Description: wireless framework Platform Common; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_platform_common component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_hdlc.c
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_coex.c
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_ot.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/platform/include
)


