# Copyright 2023 NXP
# All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.21)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 99)

project(connfwk VERSION 6.1.0)

message(STATUS "************ CONNFWK CONFIGURATION ************")
message(STATUS "CONNFWK_PLATFORM: " ${CONNFWK_PLATFORM})

# The user must fill the connfwk-config with SDK infos (paths, compile options, definitions...)
# The framework will fill it with its own infos
add_library(connfwk-config INTERFACE)

# Check if the Connectivity Framework is the top level folder
if(PROJECT_IS_TOP_LEVEL)
    # CONNFWK_SDK_ROOT can be defined by the user in case the framework is not directly included in the SDK
    if(NOT CONNFWK_SDK_ROOT)
        # Get absolute path to mcu-sdk-2.0 root folder
        file(REAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../.. CONNFWK_SDK_ROOT)
    endif()
    message(STATUS "CONNFWK_SDK_ROOT: " ${CONNFWK_SDK_ROOT})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
    # Configure sdk specific flags, includes and compile options for the selected platform
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/configs/${CONNFWK_PLATFORM}.cmake)
    message(STATUS "CONNFWK_PLATFORM_LINKER_FILE: " ${CONNFWK_PLATFORM_LINKER_FILE})
else()
    message(STATUS "CONNFWK is a subdirectory ! Make sure to set SDK properties to connfwk-config")
endif()

# Set common paths for the framework
target_include_directories(connfwk-config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/DBG
    ${CMAKE_CURRENT_SOURCE_DIR}/FunctionLib
    ${CMAKE_CURRENT_SOURCE_DIR}/SecLib
    ${CMAKE_CURRENT_SOURCE_DIR}/RNG
    ${CMAKE_CURRENT_SOURCE_DIR}/HWParameter
    ${CMAKE_CURRENT_SOURCE_DIR}/Sensors
    ${CMAKE_CURRENT_SOURCE_DIR}/FileSystem
    ${CMAKE_CURRENT_SOURCE_DIR}/FSAbstraction
    ${CMAKE_CURRENT_SOURCE_DIR}/FileCache
    ${CMAKE_CURRENT_SOURCE_DIR}/LowPower
    ${CMAKE_CURRENT_SOURCE_DIR}/ModuleInfo
    ${CMAKE_CURRENT_SOURCE_DIR}/HDI
    ${CMAKE_CURRENT_SOURCE_DIR}/IPC
    ${CMAKE_CURRENT_SOURCE_DIR}/SFC
    ${CMAKE_CURRENT_SOURCE_DIR}/OtaSupport/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/FSCI/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/NVM/Interface
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/include
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${CONNFWK_PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/${CONNFWK_PLATFORM}/configs
)

# Configure the board lib
add_subdirectory(boards)

# Configure the platform lib
add_subdirectory(platform/${CONNFWK_PLATFORM})

if(CONNFWK_PLATFORM_FAMILY)
    target_include_directories(connfwk-config INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/${CONNFWK_PLATFORM_FAMILY}/${CONNFWK_PLATFORM}
    )
    add_subdirectory(platform/${CONNFWK_PLATFORM_FAMILY})
endif()

if(CONNFWK_FLIB)
    add_subdirectory(FunctionLib)
endif()

if(CONNFWK_OTA)
    add_subdirectory(OtaSupport)
endif()

if(CONNFWK_FILESYSTEM)
    add_subdirectory(FileSystem)
endif()

if(CONNFWK_FILECACHE)
    add_subdirectory(FileCache)
endif()

if(CONNFWK_FSABSTRACTION)
    add_subdirectory(FSAbstraction)
endif()

if(CONNFWK_RNG)
    add_subdirectory(RNG)
endif()

if(CONNFWK_SECLIB)
    add_subdirectory(SecLib)
endif()

if(CONNFWK_HWPARAM)
    add_subdirectory(HWParameter)
endif()

if(CONNFWK_MODINFO)
    add_subdirectory(ModuleInfo)
endif()

if(CONNFWK_SENSORS)
    add_subdirectory(Sensors)
endif()

if(CONNFWK_PWR)
    add_subdirectory(LowPower)
endif()

if(CONNFWK_NVM)
    add_subdirectory(NVM)
endif()

if(CONNFWK_SFC)
    add_subdirectory(SFC)
endif()

if(CONNFWK_OTW)
    add_subdirectory(OTW)
endif()

if(CONNFWK_TESTS)
    add_subdirectory(tests)
endif()

add_subdirectory(third_party)
