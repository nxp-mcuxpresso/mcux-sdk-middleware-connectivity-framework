#Description: wireless framework fwk_lfs_mflash; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_fsabstraction_littlefs component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction/fwk_fs_abstraction.c
    ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction/fwk_lfs_mflash.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction
)


