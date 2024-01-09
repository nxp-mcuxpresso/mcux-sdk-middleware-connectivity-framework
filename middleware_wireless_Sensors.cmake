#Description: wireless framework Sensors; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_Sensors component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Sensors/sensors.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Sensors
)


include(middleware_wireless_framework_platform_sensors)
