#Description: wireless framework LowPower; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_LPM_systicks component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR.c
)

#OR Logic component
if(CONFIG_USE_middleware_freertos-kernel_RW612)
target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR_systicks.c
)
endif()

if(CONFIG_USE_middleware_baremetal)
target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR_systicks_bm.c
)
endif()

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/LowPower
)


include(middleware_wireless_framework_platform_lowpower)
include(middleware_wireless_framework_platform_lowpower_timer)
