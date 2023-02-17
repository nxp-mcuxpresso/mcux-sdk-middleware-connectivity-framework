# Give the possibility to the user to define his own path to board files
if(CONNFWK_BOARD_PATH)
    message(STATUS  "Using user defined board folder:\n"
        "${CONNFWK_BOARD_PATH}"
        "${CONNFWK_FREERTOS_CONFIG_PATH}"
    )
    if(NOT CONNFWK_BOARD_LIB)
        message(WARNING  "The name of the board lib is not specified. It may not be linked properly by framework targets\n"
            "You can add the link dependency in your projet:\n"
            "target_link_libraries(<app-target> <your board target>)\n"
            "Or you can define the name of your board library:\n"
            "-DCONNFWK_BOARD_LIB=<name of the board lib>\n"
            "or set it in your project before including framework:\n"
            "set(CONNFWK_BOARD_LIB <name of the board lib>)"
        )
    endif()
    add_subdirectory(${CONNFWK_BOARD_PATH} ${PROJECT_BINARY_DIR}/board)
    target_link_libraries(connfwk-config
        INTERFACE
        ${CONNFWK_BOARD_LIB}
    )
elseif(CONNFWK_BOARD)
    message(STATUS "CONNFWK_BOARD: " ${CONNFWK_BOARD})
    if(NOT CONNFWK_FREERTOS_CONFIG_PATH)
        set(CONNFWK_FREERTOS_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CONNFWK_BOARD}/freertos)
    endif()
    set(CONNFWK_BOARD_LIB connfwk-board-${CONNFWK_BOARD})
    add_subdirectory(${CONNFWK_BOARD})
    target_link_libraries(connfwk-config
        INTERFACE
            ${CONNFWK_BOARD_LIB}
    )
endif()
