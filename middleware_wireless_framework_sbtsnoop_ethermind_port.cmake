#Description: wireless framework sbtsnoop_ethermind_port; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_sbtsnoop_ethermind_port component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/DBG/sbtsnoop
)


