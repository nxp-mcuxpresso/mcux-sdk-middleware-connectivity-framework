#Description: wireless framework lfs_config; user_visible: True
include_guard(GLOBAL)
message("middleware_wireless_framework_lfs_config component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs/fwk_lfs_config.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs
)


