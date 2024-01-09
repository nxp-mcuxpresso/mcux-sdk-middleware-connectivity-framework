#Description: wireless framework board_lp; user_visible: False
include_guard(GLOBAL)
message("middleware_wireless_framework_board_lp component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga/board_lp.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


