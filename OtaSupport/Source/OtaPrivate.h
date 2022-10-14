
/*! *********************************************************************************
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * \file
 *
 * This is the header file for the OTA Internal Flash adaption.
 * It is to remain a private header - not a public interface.
 *
 ** SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */
#ifndef __OTA_PRIVATE_H__
#define __OTA_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "fsl_adapter_flash.h"

/*! @brief Hal flash status. */
typedef enum _ota_flash_status
{
    kStatus_OTA_Flash_Success,         /*!< flash operation is successful*/
    kStatus_OTA_Flash_Fail,            /*!< flash operation is not successful*/
    kStatus_OTA_Flash_InvalidArgument, /*!< Invalid argument */
    kStatus_OTA_Flash_AlignmentError,  /*!< Alignment Error */
    kStatus_OTA_Flash_EccError,        /*!< ECC error detected */
    kStatus_OTA_Flash_Error,           /*!< Illegal command */
    kStatus_OTA_Flash_NotSupport,      /*!< Not support */
} ota_flash_status_t;

typedef struct
{
    ota_flash_status_t (*init)(void);
    ota_flash_status_t (*format_storage)(void);
    ota_flash_status_t (*eraseBlock)(uint32_t Addr, uint32_t size);
    ota_flash_status_t (*writeData)(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf);
    ota_flash_status_t (*readData)(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf);
    uint8_t (*isBusy)(void);
    ota_flash_status_t (*eraseArea)(uint32_t *pAddr, int32_t *pSize, bool non_blocking);
    ota_flash_status_t (*verify)(uint8_t *pData, uint32_t Addr, uint16_t length);
    ota_flash_status_t (*flushWriteBuf)(void);

} OtaFlashOps_t;

typedef struct
{
    uint32_t base_offset;
    uint32_t total_size;
    uint32_t sector_size;
    uint8_t  use_internal_storage;
    uint32_t baudrate;

    OtaFlashOps_t ops;
} OtaFlash_t;

extern const OtaFlash_t ext_storage;
extern const OtaFlash_t int_storage;

#define DBG_LEVEL_NONE    0
#define DBG_LEVEL_WARNING 1
#define DBG_LEVEL_INFO    2
#define DBG_LEVEL_DEBUG   3
#define DBG_LEVEL_VERBOSE 4

#define OTA_DBG_LVL DBG_LEVEL_NONE

#if (OTA_DBG_LVL > DBG_LEVEL_NONE)
#include "fsl_debug_console.h"
#endif

#if (OTA_DBG_LVL >= DBG_LEVEL_VERBOSE)
#define OTA_VERBOSE_TRACE PRINTF
#else
#define OTA_VERBOSE_TRACE(...)
#endif
#if (OTA_DBG_LVL >= DBG_LEVEL_DEBUG)
#define OTA_DEBUG_TRACE PRINTF
#else
#define OTA_DEBUG_TRACE(...)
#endif

#if (OTA_DBG_LVL >= DBG_LEVEL_INFO)
#define OTA_INFO_TRACE PRINTF
#else
#define OTA_INFO_TRACE(...)
#endif

#if (OTA_DBG_LVL >= DBG_LEVEL_WARNING)
#define OTA_WARNING_TRACE PRINTF
#else
#define OTA_WARNING_TRACE(...)
#endif

#ifdef __cplusplus
}
#endif
#endif /* __OTA_PRIVATE_H__ */
