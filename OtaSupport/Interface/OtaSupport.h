/*! *********************************************************************************
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * \file
 *
 * This is the header file for the OTA Programming Support.
 *
 ** SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */

#ifndef _OTA_SUPPORT_H_
#define _OTA_SUPPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fsl_adapter_flash.h"
#include "fwk_platform.h"
#include "fsl_component_generic_list.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/

#define gOtaVersion_c (0x01)

/*!< gOtaErasePolicyOnTheFly_c sector erasure interleaved with programming operations */
#define gOtaEraseOnTheFly_c 0
/*!< gOtaEraseBeforeImageBlockReq_c erase enough headroom for the BlockImage size requested */
#define gOtaEraseBeforeImageBlockReq_c 1

/*!< Select gOtaErasePolicy_c for efficiency */
#define gOtaErasePolicy_c gOtaEraseBeforeImageBlockReq_c
#define gOtaVerifyWrite_d 1

/*  Transaction size in posted_ops_storage set to sizeof(FLASH_TransactionOpNode_t)*/
#define gOtaTransactionSz_d (sizeof(FLASH_TransactionOpNode_t))

/* The internal flash has a program page of 128 whereas the external flash has 256 byte pages
 * 256 is a good compromise.
 */
#define PROGRAM_PAGE_SZ 256U

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef enum
{
    gOtaSuccess_c = 0,
    gOtaNoImage_c,
    gOtaUpdated_c,
    gOtaError_c,
    gOtaCrcError_c,
    gOtaInvalidParam_c,
    gOtaInvalidOperation_c,
    gOtaExternalFlashError_c,
    gOtaInternalFlashError_c,
    gOtaImageTooLarge_c
} otaResult_t;

typedef struct
{
    /* Only resume posted operations in Idle Task */
    bool PostedOpInIdleTask;
    /* Number of transactions processed when calling OTA_TransactionResume */
    int maxConsecutiveTransactions;
} ota_config_t;

typedef union ota_op_completion_cb
{
    /*! Prototype of ota_completion callback */
    void (*func)(uint32_t param);
    uint32_t pf;
} ota_op_completion_cb_t;

typedef enum
{
    FLASH_OP_WRITE,
    FLASH_OP_ERASE_THEN_WRITE,
    FLASH_OP_READ,
    FLASH_OP_ERASE_SECTOR,
    FLASH_OP_ERASE_BLOCK,
    FLASH_OP_ERASE_AREA,
    FLASH_OP_ERASE_NEXT_BLOCK,
    FLASH_OP_ERASE_NEXT_BLOCK_COMPLETE,
    FLASH_OP_CONSUMED = 0xff
} FLASH_op_type;

typedef struct
{
    FLASH_op_type op_type;
    uint32_t      flash_addr;
    int32_t       sz;
    uint8_t       buf[PROGRAM_PAGE_SZ];
} FLASH_TransactionOp_t;

typedef struct
{
    list_element_t        list_node;
    FLASH_TransactionOp_t flash_transac;
} FLASH_TransactionOpNode_t;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
 * \brief  Starts the process of writing a new image to the selected OTA storage.
 *
 * \param[in] length: the length of the image to be written.
 *
 * \return
 *  - gOtaInvalidParam_c: the intended length is bigger than the FLASH capacity
 *  - gOtaInvalidOperation_c: the process is already started (can be cancelled)
 *  - gOtaEepromError_c: can not detect external EEPROM
 *
 ********************************************************************************** */
otaResult_t OTA_StartImage(uint32_t length);

/*! *********************************************************************************
 * \brief  Places the next image chunk into the selected OTA storage. The CRC will not be computed.
 *
 * \param[in] pData          pointer to the data chunk
 * \param[in] length         the length of the data chunk
 * \param[in] pImageLength   if it is not null and the function call is successful,
 *                           it will be filled with the current length of the image
 * \param[in] pImageOffset   if it is not null contains the current offset of the image
 *
 * \return
 *  - gOtaInvalidParam_c: pData is NULL or the resulting image would be bigger than the
 *       final image length specified with OTA_StartImage()
 *  - gOtaInvalidOperation_c: the process is not started
 *
 ********************************************************************************** */
otaResult_t OTA_PushImageChunk(uint8_t *pData, uint16_t length, uint32_t *pImageLength, uint32_t *pImageOffset);

/*! *********************************************************************************
 * \brief  Read and copy from previous pushed chunks (Flash or RAM) to RAM pointed by pData
 *
 * \param[in] pData          pointer to the data chunk, to be allocated by caller
 * \param[in] length         the length of the data chunk
 * \param[in] pImageOffset   if it is not null contains the current offset of the image
 *
 * \return
 *  - gOtaInvalidParam_c: pData is NULL or the resulting image would be bigger than the
 *       final image length specified with OTA_StartImage()
 *  - gOtaInvalidOperation_c: the process is not started
 *
 ********************************************************************************** */
otaResult_t OTA_PullImageChunk(uint8_t *pData, uint16_t length, uint32_t *pImageOffset);

/*! *********************************************************************************
 * \brief  Finishes the writing of a new image to the permanent storage.
 *         It will write the image header (signature and length) and footer (sector copy bitmap).
 *
 * \param[in] bitmap   pointer to a byte array indicating the sector erase pattern for the
 *                     internal FLASH before the image update.
 *
 * \return
 *  - gOtaInvalidOperation_c: the process is not started,
 *  - gOtaEepromError_c: error while trying to write the EEPROM
 *
 ********************************************************************************** */
otaResult_t OTA_CommitImage(uint8_t *pBitmap);

/*! *********************************************************************************
 * \brief  Cancels the process of writing a new image to the selected OTA storage.
 *
 ********************************************************************************** */
void OTA_CancelImage(void);

/*! *********************************************************************************
 * \brief  Set the boot flags, to trigger the Bootloader at the next CPU reset.
 *
 ********************************************************************************** */
void OTA_SetNewImageFlag(void);

/*! *********************************************************************************
 * \brief  Read from the image storage (external memory or internal flash)
 *
 * \param[in] pData    pointer to the data chunk
 * \param[in] length   the length of the data chunk
 * \param[in] address  image storage address
 *
 * \return  error code.
 *
 ********************************************************************************** */
otaResult_t OTA_ReadStorageMemory(uint8_t *pData, uint16_t length, uint32_t address);

/*! *********************************************************************************
 * \brief  Write into the image storage (external memory or internal flash)
 *
 * \param[in] pData    pointer to the data chunk
 * \param[in] length   the length of the data chunk
 * \param[in] address  image storage address
 *
 * \return  error code.
 *
 ********************************************************************************** */
otaResult_t OTA_WriteStorageMemory(uint8_t *pData, uint16_t length, uint32_t address);

/*! *********************************************************************************
 * \brief  Compute CRC over a data chunk.
 *
 * \param[in] pData        pointer to the data chunk
 * \param[in] length       the length of the data chunk
 * \param[in] crcValueOld  current CRC value
 *
 * \return  computed CRC.
 *
 ********************************************************************************** */
uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld);

/*! *********************************************************************************
 * \brief  This function is called in order to erase the entire image storage
 *         (external memory or internal flash)
 *
 * \return  computed CRC.
 *
 ********************************************************************************** */
otaResult_t OTA_EraseStorageMemory(void);

/*! *********************************************************************************
 * \brief  Selects Internal Storage for OTA
 *
 * \return  error code.
 *
 ********************************************************************************** */
otaResult_t OTA_SelectInternalStoragePartition(void);

/*! *********************************************************************************
 * \brief  Selects External for OTA
 *
 * \param[in]  partition_offset offset at which OTA partition starts in external flash
 * \param[in]  partition_sz  size of claimed OTA partition in bytes
 * \return  error code.
 *
 ********************************************************************************** */
otaResult_t OTA_SelectExternalStoragePartition(uint32_t partition_offset, uint32_t partition_sz);

/*! *********************************************************************************
 * \brief  Attempt to resume OTA from some polling task (Idle Task preferably)
 *
 * \return  Number of transactions treated.
 *
 ********************************************************************************** */
int OTA_TransactionResume(void);

/*! *********************************************************************************
* \brief  Start OTA service Attempt to resume OTA from some polling task (Idle Task preferably)

* \param[in] posted_ops_storage    pointer to the buffer used for transaction storage.
*                                  Ideally application allocates a dynamic buffer to allow
                                   allocation of transactions. Between 4 and 8 transaction
                                   buffers are required to prevent stalling.
                                   Note : Transaction size should be set to
                                   gOtaTransactionSz_d (sizeof(FLASH_TransactionOpNode_t)).
* \param[in] posted_ops_sz         size in bytes of the transaction buffer.
* \return  error code.
*
********************************************************************************** */
otaResult_t OTA_ServiceInit(void *posted_ops_storage, size_t posted_ops_sz);

/*! *********************************************************************************
 * \brief  Suppress all transactions pending in storage
 *
 * \return  error code 0.
 *
 ********************************************************************************** */
otaResult_t OTA_ServiceDeInit(void);

/*! *********************************************************************************
* \brief  Get the default configuration of OTA setting

* \param[in] userConfig             pointer to the configuration structure
*
********************************************************************************** */
void OTA_GetDefaultConfig(ota_config_t *userConfig);

/*! *********************************************************************************
* \brief  Set the configuration of OTA setting

* \param[in] userConfig             pointer to the configuration structure
*
********************************************************************************** */
void OTA_SetConfig(ota_config_t *userConfig);

/*! *********************************************************************************
* \brief  Erase in advance enough space to receive an Image Block

* \param [in] size  block size to prepare
* \param [in] cb    callback function to call on completion of erase operation
* \param [in] param callback parameter (not really used)
*
* \return  error code 0.
*
********************************************************************************** */
otaResult_t OTA_MakeHeadRoomForNextBlock(uint32_t size, ota_op_completion_cb_t cb, uint32_t param);

/*! *********************************************************************************
 * \brief  Return available size of current OTA storage
 *
 * \return  0 if not initialized, total size otherwise.
 *
 ********************************************************************************** */
uint32_t OTA_GetSelectedFlashAvailableSpace(void);

#ifdef __cplusplus
}
#endif

#endif /* _OTA_SUPPORT_H_ */
