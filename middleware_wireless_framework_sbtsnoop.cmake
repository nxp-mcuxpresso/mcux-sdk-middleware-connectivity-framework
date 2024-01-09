#Description: wireless framework sbtsnoop; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_sbtsnoop component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/DBG/sbtsnoop/sbtsnoop.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/DBG/sbtsnoop
)

#OR Logic component
if(CONFIG_USE_middleware_wireless_framework_sbtsnoop_ethermind_port) 
    include(middleware_wireless_framework_sbtsnoop_ethermind_port)
endif()

