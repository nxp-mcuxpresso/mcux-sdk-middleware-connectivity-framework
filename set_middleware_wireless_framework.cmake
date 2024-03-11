include_guard(GLOBAL)


if (CONFIG_USE_middleware_wireless_framework_CMake_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_CMake_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_CMake_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")


endif()


if (CONFIG_USE_middleware_wireless_framework_FSCI)
# Add set(CONFIG_USE_middleware_wireless_framework_FSCI true) in config.cmake to use this component

message("middleware_wireless_framework_FSCI component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FSCI/Source/FsciCommands.c
  ${CMAKE_CURRENT_LIST_DIR}/FSCI/Source/FsciCommunication.c
  ${CMAKE_CURRENT_LIST_DIR}/FSCI/Source/FsciLogging.c
  ${CMAKE_CURRENT_LIST_DIR}/FSCI/Source/FsciMain.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FSCI/Interface
  ${CMAKE_CURRENT_LIST_DIR}/FSCI/Source
)


endif()


if (CONFIG_USE_middleware_wireless_HWParameter)
# Add set(CONFIG_USE_middleware_wireless_HWParameter true) in config.cmake to use this component

message("middleware_wireless_HWParameter component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/HWParameter/HWParameter.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/HWParameter/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_Common)
# Add set(CONFIG_USE_middleware_wireless_framework_Common true) in config.cmake to use this component

message("middleware_wireless_framework_Common component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/Common/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_RNG)
# Add set(CONFIG_USE_middleware_wireless_framework_RNG true) in config.cmake to use this component

message("middleware_wireless_framework_RNG component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/RNG/RNG.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/RNG/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_RNG_mbedtls)
# Add set(CONFIG_USE_middleware_wireless_framework_RNG_mbedtls true) in config.cmake to use this component

message("middleware_wireless_framework_RNG_mbedtls component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/RNG/RNG_mbedTLS.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/RNG/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_sec_lib_cryptolib_src)
# Add set(CONFIG_USE_middleware_wireless_framework_sec_lib_cryptolib_src true) in config.cmake to use this component

message("middleware_wireless_framework_sec_lib_cryptolib_src component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/SecLib.c
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/build/CryptoLib/src/SW_AES128.c
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/build/CryptoLib/src/SW_RNG.c
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/build/CryptoLib/src/SW_SHA1.c
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/build/CryptoLib/src/SW_SHA256.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/.
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/build/CryptoLib/src
)


endif()


if (CONFIG_USE_middleware_wireless_framework_function_lib)
# Add set(CONFIG_USE_middleware_wireless_framework_function_lib true) in config.cmake to use this component

message("middleware_wireless_framework_function_lib component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FunctionLib/FunctionLib.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FunctionLib/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_module_info)
# Add set(CONFIG_USE_middleware_wireless_framework_module_info true) in config.cmake to use this component

message("middleware_wireless_framework_module_info component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/ModuleInfo/ModuleInfo.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/ModuleInfo/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_NVM)
# Add set(CONFIG_USE_middleware_wireless_framework_NVM true) in config.cmake to use this component

message("middleware_wireless_framework_NVM component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/NVM/Source/NV_Flash.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/NVM/Interface
  ${CMAKE_CURRENT_LIST_DIR}/NVM/Source
)


endif()


if (CONFIG_USE_middleware_wireless_framework_NV_FSCI)
# Add set(CONFIG_USE_middleware_wireless_framework_NV_FSCI true) in config.cmake to use this component

message("middleware_wireless_framework_NV_FSCI component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/NVM/Source/NV_FsciCommands.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/NVM/Source
)


endif()


if (CONFIG_USE_middleware_wireless_framework_OtaServerSupport)
# Add set(CONFIG_USE_middleware_wireless_framework_OtaServerSupport true) in config.cmake to use this component

message("middleware_wireless_framework_OtaServerSupport component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Source/OtaServerSupport.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Interface
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Source
)


endif()


if (CONFIG_USE_middleware_wireless_framework_sbtsnoop_ethermind_port)
# Add set(CONFIG_USE_middleware_wireless_framework_sbtsnoop_ethermind_port true) in config.cmake to use this component

message("middleware_wireless_framework_sbtsnoop_ethermind_port component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/DBG/sbtsnoop/.
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DSBTSNOOP_PORT_FILE=sbtsnoop_port_ethermind.h
    -DSERIAL_BTSNOOP
  )

endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_sbtsnoop_nxp_ble_port)
# Add set(CONFIG_USE_middleware_wireless_framework_sbtsnoop_nxp_ble_port true) in config.cmake to use this component

message("middleware_wireless_framework_sbtsnoop_nxp_ble_port component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/sbtsnoop/.
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DSBTSNOOP_PORT_FILE=sbtsnoop_port_nxp_ble.h
    -DTM_ENABLE_TIME_STAMP=1
    -DSERIAL_BTSNOOP
  )

endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_linkscripts_kw45)
# Add set(CONFIG_USE_middleware_wireless_framework_linkscripts_kw45 true) in config.cmake to use this component

message("middleware_wireless_framework_linkscripts_kw45 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript/end_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript/main_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript/symbols.ldt
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_linkscripts_mcxw34xevk)
# Add set(CONFIG_USE_middleware_wireless_framework_linkscripts_mcxw34xevk true) in config.cmake to use this component

message("middleware_wireless_framework_linkscripts_mcxw34xevk component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/MCXW345/mcux/linkscript/end_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/MCXW345/mcux/linkscript/main_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/MCXW345/mcux/linkscript/symbols.ldt
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_linkscript_bootloader_kw45)
# Add set(CONFIG_USE_middleware_wireless_framework_linkscript_bootloader_kw45 true) in config.cmake to use this component

message("middleware_wireless_framework_linkscript_bootloader_kw45 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_bootloader/end_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_bootloader/main_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_bootloader/main_text_section.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_bootloader/symbols.ldt
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_linkscript_warmboot_kw45)
# Add set(CONFIG_USE_middleware_wireless_framework_linkscript_warmboot_kw45 true) in config.cmake to use this component

message("middleware_wireless_framework_linkscript_warmboot_kw45 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_TOOLCHAIN STREQUAL mcux)
  target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_warmboot/end_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_warmboot/main_text.ldt
      ${CMAKE_CURRENT_LIST_DIR}/Common/devices/kw45_k32w1/mcux/linkscript_warmboot/symbols.ldt
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_HDI)
# Add set(CONFIG_USE_middleware_wireless_HDI true) in config.cmake to use this component

message("middleware_wireless_HDI component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/HDI/hdi.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/HDI/.
)


endif()


if (CONFIG_USE_middleware_wireless_IPC)
# Add set(CONFIG_USE_middleware_wireless_IPC true) in config.cmake to use this component

message("middleware_wireless_IPC component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/IPC/ipc.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/IPC/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_PDM)
# Add set(CONFIG_USE_middleware_wireless_framework_PDM true) in config.cmake to use this component

message("middleware_wireless_framework_PDM component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/PDM/Include
)


endif()


if (CONFIG_USE_middleware_wireless_framework_PDUM)
# Add set(CONFIG_USE_middleware_wireless_framework_PDUM true) in config.cmake to use this component

message("middleware_wireless_framework_PDUM component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/PDUM/Include
)


endif()


if (CONFIG_USE_middleware_wireless_framework_markdown)
# Add set(CONFIG_USE_middleware_wireless_framework_markdown true) in config.cmake to use this component

message("middleware_wireless_framework_markdown component is included from ${CMAKE_CURRENT_LIST_FILE}.")


endif()


if (CONFIG_USE_middleware_wireless_framework_SFC)
# Add set(CONFIG_USE_middleware_wireless_framework_SFC true) in config.cmake to use this component

message("middleware_wireless_framework_SFC component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/SFC/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_RF_SFC)
# Add set(CONFIG_USE_middleware_wireless_framework_RF_SFC true) in config.cmake to use this component

message("middleware_wireless_framework_RF_SFC component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/SFC/fwk_rf_sfc.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/SFC/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_OTW)
# Add set(CONFIG_USE_middleware_wireless_framework_OTW true) in config.cmake to use this component

message("middleware_wireless_framework_OTW component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/OTW/k32w0_transceiver/fwk_otw.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/OTW/Interface
  ${CMAKE_CURRENT_LIST_DIR}/OTW/k32w0_transceiver
)


endif()


if (CONFIG_USE_middleware_wireless_framework_FactoryDataProvider)
# Add set(CONFIG_USE_middleware_wireless_framework_FactoryDataProvider true) in config.cmake to use this component

message("middleware_wireless_framework_FactoryDataProvider component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FactoryDataProvider/fwk_factory_data_provider.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FactoryDataProvider/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_filesystem)
# Add set(CONFIG_USE_middleware_wireless_framework_filesystem true) in config.cmake to use this component

message("middleware_wireless_framework_filesystem component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FileSystem/fwk_filesystem.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FileSystem/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_fsabstraction_littlefs)
# Add set(CONFIG_USE_middleware_wireless_framework_fsabstraction_littlefs true) in config.cmake to use this component

message("middleware_wireless_framework_fsabstraction_littlefs component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction/fwk_lfs_mflash.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_swo_dbg)
# Add set(CONFIG_USE_middleware_wireless_framework_swo_dbg true) in config.cmake to use this component

message("middleware_wireless_framework_swo_dbg component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/DBG/SWO/fwk_debug_swo.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/DBG/SWO/.
)


endif()


if (CONFIG_USE_middleware_wireless_framework_settings)
# Add set(CONFIG_USE_middleware_wireless_framework_settings true) in config.cmake to use this component

message("middleware_wireless_framework_settings component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Source/settings.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Source/settings_init.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Source/settings_runtime.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Source/settings_store.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Source/settings_nvs.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Interface
  ${CMAKE_CURRENT_LIST_DIR}/../framework/Settings/Source
)


endif()


if (CONFIG_USE_middleware_wireless_framework_NVS)
# Add set(CONFIG_USE_middleware_wireless_framework_NVS true) in config.cmake to use this component

message("middleware_wireless_framework_NVS component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Source/nvs.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Source/fwk_nvs_flash.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Source/fwk_nvs_ExternalFlash.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Source/fwk_nvs_InternalFlash.c
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Source/crc8.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Interface
  ${CMAKE_CURRENT_LIST_DIR}/../framework/NVS/Source
)


endif()


if (CONFIG_USE_middleware_wireless_framework_sec_lib)
# Add set(CONFIG_USE_middleware_wireless_framework_sec_lib true) in config.cmake to use this component

message("middleware_wireless_framework_sec_lib component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/SecLib.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/.
)

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/SecLib/lib_crypto_m33.a
      -Wl,--end-group
  )
endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_platform_common_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_common_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_common_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_hdlc.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_coex.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_ot.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/include
)


endif()


if (CONFIG_USE_middleware_wireless_framework_platform_coex_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_coex_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_coex_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_coex.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/include
)


endif()


if (CONFIG_USE_middleware_wireless_framework_platform_internal_flash_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_internal_flash_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_internal_flash_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)


endif()


if (CONFIG_USE_middleware_wireless_framework_board_lp_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_board_lp_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_board_lp_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga/board_lp.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


endif()


if (CONFIG_USE_middleware_wireless_framework_board_dcdc_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_board_dcdc_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_board_dcdc_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


endif()


if (CONFIG_USE_middleware_wireless_framework_board_platform_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_board_platform_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_board_platform_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


endif()


if (CONFIG_USE_middleware_wireless_framework_board_comp_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_board_comp_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_board_comp_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


endif()


if (CONFIG_USE_middleware_wireless_framework_board_extflash_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_board_extflash_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_board_extflash_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


endif()


if (CONFIG_USE_middleware_wireless_framework_lfs_config_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_lfs_config_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_lfs_config_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs/fwk_lfs_config.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DLFS_CONFIG=fwk_lfs_config.h
  )

endif()


endif()


if (CONFIG_USE_middleware_wireless_framework_matter_config_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_matter_config_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_matter_config_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/./boards/rdrw612bga/board.c
  ${CMAKE_CURRENT_LIST_DIR}/./boards/rdrw612bga/board_lp.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/./boards/rdrw612bga
  ${CMAKE_CURRENT_LIST_DIR}/./Common/devices/rdrw612bga/gcc
)


endif()


if (CONFIG_USE_middleware_wireless_framework_init_config_rdrw612bga)
# Add set(CONFIG_USE_middleware_wireless_framework_init_config_rdrw612bga true) in config.cmake to use this component

message("middleware_wireless_framework_init_config_rdrw612bga component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga/app_services_init.c
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga/hardware_init.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/boards/rdrw612bga
)


endif()


if (CONFIG_USE_middleware_wireless_framework_rpmsg_config)
# Add set(CONFIG_USE_middleware_wireless_framework_rpmsg_config true) in config.cmake to use this component

message("middleware_wireless_framework_rpmsg_config component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs
)


endif()


if (CONFIG_USE_middleware_wireless_framework_mbedtls_config_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_mbedtls_config_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_mbedtls_config_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs
)


endif()


if (CONFIG_USE_middleware_wireless_framework_platform_rt_ot_coex)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_rt_ot_coex true) in config.cmake to use this component

message("middleware_wireless_framework_platform_rt_ot_coex component is included from ${CMAKE_CURRENT_LIST_FILE}.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/imx_rt/fwk_platform_ot.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/imx_rt/fwk_platform_hdlc.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/imx_rt/k32w0/fwk_platform_coex.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/imx_rt/iw612/fwk_platform_coex.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/imx_rt/configs/fwk_lfs_config.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/platform/imx_rt/configs
)


endif()


if (CONFIG_USE_middleware_wireless_framework_OtaSupport)
# Add set(CONFIG_USE_middleware_wireless_framework_OtaSupport true) in config.cmake to use this component

message("middleware_wireless_framework_OtaSupport component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_ota_rw61x AND CONFIG_USE_middleware_wireless_framework_platform_extflash_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Source/OtaSupport.c
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Source/OtaInternalFlash.c
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Source/OtaExternalFlash.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Interface
  ${CMAKE_CURRENT_LIST_DIR}/OtaSupport/Source
)

else()

message(SEND_ERROR "middleware_wireless_framework_OtaSupport dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_sbtsnoop)
# Add set(CONFIG_USE_middleware_wireless_framework_sbtsnoop true) in config.cmake to use this component

message("middleware_wireless_framework_sbtsnoop component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_sbtsnoop_ethermind_port OR CONFIG_USE_middleware_wireless_framework_sbtsnoop_nxp_ble_port)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/DBG/sbtsnoop/sbtsnoop.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/DBG/sbtsnoop/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_sbtsnoop dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_fwk_debug)
# Add set(CONFIG_USE_middleware_wireless_framework_fwk_debug true) in config.cmake to use this component

message("middleware_wireless_framework_fwk_debug component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_swo_dbg)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/DBG/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_fwk_debug dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_Sensors)
# Add set(CONFIG_USE_middleware_wireless_Sensors true) in config.cmake to use this component

message("middleware_wireless_Sensors component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_sensors_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Sensors/sensors.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/Sensors/.
)

else()

message(SEND_ERROR "middleware_wireless_Sensors dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_freertos_heap)
# Add set(CONFIG_USE_middleware_wireless_freertos_heap true) in config.cmake to use this component

message("middleware_wireless_freertos_heap component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_freertos-kernel)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Common/rtos/freertos/heap_mem_manager.c
)

else()

message(SEND_ERROR "middleware_wireless_freertos_heap dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_freertos_utils)
# Add set(CONFIG_USE_middleware_wireless_framework_freertos_utils true) in config.cmake to use this component

message("middleware_wireless_framework_freertos_utils component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_freertos-kernel AND CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/Common/rtos/freertos/fwk_freertos_utils.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/Common/rtos/freertos/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_freertos_utils dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_LPM_RW610)
# Add set(CONFIG_USE_middleware_wireless_framework_LPM_RW610 true) in config.cmake to use this component

message("middleware_wireless_framework_LPM_RW610 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_lowpower_rw61x AND CONFIG_USE_middleware_wireless_framework_platform_lowpower_timer_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/LowPower/.
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DHAL_UART_ADAPTER_LOWPOWER=1
  )

endif()

else()

message(SEND_ERROR "middleware_wireless_framework_LPM_RW610 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_LPM_systicks_RW610)
# Add set(CONFIG_USE_middleware_wireless_framework_LPM_systicks_RW610 true) in config.cmake to use this component

message("middleware_wireless_framework_LPM_systicks_RW610 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_lowpower_rw61x AND CONFIG_USE_middleware_wireless_framework_platform_lowpower_timer_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR.c
)

if(CONFIG_USE_middleware_freertos-kernel)
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
  ${CMAKE_CURRENT_LIST_DIR}/LowPower/.
)

if(CONFIG_USE_COMPONENT_CONFIGURATION)
  message("===>Import configuration from ${CMAKE_CURRENT_LIST_FILE}")

  target_compile_definitions(${MCUX_SDK_PROJECT_NAME} PUBLIC
    -DHAL_UART_ADAPTER_LOWPOWER=1
  )

endif()

else()

message(SEND_ERROR "middleware_wireless_framework_LPM_systicks_RW610 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_LPM_cli_RW610)
# Add set(CONFIG_USE_middleware_wireless_framework_LPM_cli_RW610 true) in config.cmake to use this component

message("middleware_wireless_framework_LPM_cli_RW610 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_LPM_RW610)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/LowPower/PWR_cli.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/LowPower/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_LPM_cli_RW610 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_fsabstraction)
# Add set(CONFIG_USE_middleware_wireless_framework_fsabstraction true) in config.cmake to use this component

message("middleware_wireless_framework_fsabstraction component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_fsabstraction_littlefs)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction/fwk_fs_abstraction.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FSAbstraction/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_fsabstraction dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_filecache)
# Add set(CONFIG_USE_middleware_wireless_framework_filecache true) in config.cmake to use this component

message("middleware_wireless_framework_filecache component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_fsabstraction)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/FileCache/fwk_file_cache.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/FileCache/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_filecache dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_keystorage)
# Add set(CONFIG_USE_middleware_wireless_framework_keystorage true) in config.cmake to use this component

message("middleware_wireless_framework_keystorage component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_filecache)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/KeyStorage/fwk_key_storage.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/KeyStorage/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_keystorage dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_fwk_debug)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/include
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/configs
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_flash_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_flash_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_flash_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_flash.c
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_extflash.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
  ${CMAKE_CURRENT_LIST_DIR}/platform/Common
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_flash_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_extflash_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_extflash_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_extflash_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_extflash.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
  ${CMAKE_CURRENT_LIST_DIR}/platform/Common
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_extflash_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_sensors_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_sensors_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_sensors_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_sensors.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_sensors_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_ota_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_ota_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_ota_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_ota.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_ota_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_ics_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_ics_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_ics_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x AND CONFIG_USE_middleware_wireless_framework_platform_sensors_rw61x AND CONFIG_USE_middleware_wireless_framework_SFC)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_ics_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_MWS)
# Add set(CONFIG_USE_middleware_wireless_framework_MWS true) in config.cmake to use this component

message("middleware_wireless_framework_MWS component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_mws_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/MWSCoexistence/MWS.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/MWSCoexistence/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_MWS dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_MWS_Intercore)
# Add set(CONFIG_USE_middleware_wireless_framework_MWS_Intercore true) in config.cmake to use this component

message("middleware_wireless_framework_MWS_Intercore component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_mws_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/MWSCoexistence/MWS_Intercore.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/MWSCoexistence/.
)

else()

message(SEND_ERROR "middleware_wireless_framework_MWS_Intercore dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_mws_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_mws_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_mws_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_mws_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_ble_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_ble_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_ble_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x AND CONFIG_USE_middleware_wireless_framework_platform_coex_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_ble.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_ble_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_genfsk_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_genfsk_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_genfsk_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_genfsk_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_ot_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_ot_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_ot_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_ot.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_ot_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_lowpower_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_lowpower_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_lowpower_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x/fwk_platform_lowpower.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_lowpower_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_lowpower_timer_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_lowpower_timer_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_lowpower_timer_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_lowpower_timer_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_platform_reset_rw61x)
# Add set(CONFIG_USE_middleware_wireless_framework_platform_reset_rw61x true) in config.cmake to use this component

message("middleware_wireless_framework_platform_reset_rw61x component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/platform/rw61x
)

else()

message(SEND_ERROR "middleware_wireless_framework_platform_reset_rw61x dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_sec_lib_mbedtls_m33)
# Add set(CONFIG_USE_middleware_wireless_framework_sec_lib_mbedtls_m33 true) in config.cmake to use this component

message("middleware_wireless_framework_sec_lib_mbedtls_m33 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x AND CONFIG_USE_middleware_wireless_framework_mbedtls_config_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/SecLib_mbedTLS.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/.
)

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/SecLib/lib_crypto_m33.a
      -Wl,--end-group
  )
endif()

else()

message(SEND_ERROR "middleware_wireless_framework_sec_lib_mbedtls_m33 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()


if (CONFIG_USE_middleware_wireless_framework_sec_lib_sss_m33)
# Add set(CONFIG_USE_middleware_wireless_framework_sec_lib_sss_m33 true) in config.cmake to use this component

message("middleware_wireless_framework_sec_lib_sss_m33 component is included from ${CMAKE_CURRENT_LIST_FILE}.")

if(CONFIG_USE_middleware_wireless_framework_platform_rw61x)

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/SecLib_sss.c
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/SecLib/.
)

if((CONFIG_TOOLCHAIN STREQUAL armgcc OR CONFIG_TOOLCHAIN STREQUAL mcux))
  target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE
    -Wl,--start-group
      ${CMAKE_CURRENT_LIST_DIR}/SecLib/lib_crypto_m33.a
      -Wl,--end-group
  )
endif()

else()

message(SEND_ERROR "middleware_wireless_framework_sec_lib_sss_m33 dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()

endif()

