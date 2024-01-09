#Description: wireless framework LowPower CLI; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_LPM_cli component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR_cli.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/LowPower
)


include(middleware_wireless_framework_LPM)
