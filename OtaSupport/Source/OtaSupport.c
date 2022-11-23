/*! *********************************************************************************
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * \file
 *
 * This source file contains the code that enables the OTA Programming protocol
 * to load an image received over the air into an external memory, using
 * the format that the Bootloader will understand
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */
#include <stddef.h>

#include "EmbeddedTypes.h"
#include "OtaSupport.h"
#include "fsl_adapter_flash.h"
#include "fsl_component_messaging.h"

#include "FunctionLib.h"

#include "fwk_platform_ota.h"
#include "fwk_platform_flash.h"
#include "OtaPrivate.h"
#include "fsl_os_abstraction.h"

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/

#define gOtaVerifyWriteBufferSize_d (16) /* [bytes] */

#define RAISE_ERROR(x, val) \
    {                       \
        x = (val);          \
        break;              \
    }

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/* TODO : resolve [MATTER-422] to uncomment the static_assert*/
/* Check if the defined transaction size matches the transaction structure */
/*static_assert(sizeof(FLASH_TransactionOpNode_t) == gOtaTransactionSz_d,
              "gOtaTransactionSz_d does not match the actual size of FLASH_TransactionOpNode_t sructure");*/

typedef struct
{
    /*! Flag storing whether we are already in the process of writing an image received OTA in the OTA storage or not */
    bool LoadOtaImageInProgress;
    /*! Total length of the OTA image that is currently being written in OTA storage */
    uint32_t OtaImageTotalLength;
    /*! The length of the OTA image that has being written in OTA storage so far */
    uint32_t OtaImageCurrentLength;
    /*! Current write address in the OTA storage */
    uint32_t CurrentStorageAddress;
    /*! Limit up to which erase was completed */
    uint32_t ErasedUntilAddress;
    /*! When a new image is ready the flash flags will be written in idle task */
    bool NewImageReady;
    /*! Select where the image is to be written to internal storage or external one*/
    const OtaFlash_t *SelectedFlash;
    /*! Select offset at which the image is expected to start - leave space for a possible BootInfo section */
    uint32_t ImageOffset;
    /*! Define the maximum image size */
    uint32_t MaxImageLength;

    /*! Queue of flash operations */
    messaging_t op_queue;
    /*! Element in which accumulation of PROGRAM_PAGE_SZ bytes is performed  */
    FLASH_TransactionOp_t *cur_transaction;
    /*! Address of actual EEPROM address, must remain less than CurrentEepromAddress */
    uint32_t StorageAddressWritten;
    /*! Size actually written into OTA storage must be less than OtaImageCurrentLength */
    uint32_t OtaImageLengthWritten;
    /*! Number of polling: OTA_TransactionResume calls  */
    int           cnt_idle_op;
    int           max_cnt_idle;
    int           q_sz;
    int           q_max;
    void *        PostedQueue_storage;
    uint8_t       PostedQueue_capacity;
    uint8_t       PostedQueue_nb_in_queue;
    list_label_t  transaction_free_list;
    bool          isInitialized;
    ota_config_t *config;
    /* Mutex used for locking OTA Transactions */
    OSA_MUTEX_HANDLE_DEFINE(msgQueueMutex);

} OtaStateCtx_t;

union ota_op_completion_cb
{
    /*! Prototype of ota_completion callback */
    ota_op_completion_cb_t func;
    uint32_t               pf;
};

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/

static void                   OTA_WritePendingData(void);
static int                    OTA_TransactionQueuePurge(void);
static void                   OTA_MsgQueue(FLASH_TransactionOp_t *pMsg);
static void                   OTA_MsgDequeue(void);
static bool                   OTA_IsTransactionPending(void);
static otaResult_t            OTA_PostWriteToFlash(uint16_t NoOfBytes, uint32_t Addr, uint8_t *pData);
static bool                   OTA_UsePostedOperation(void);
static void                   OTA_FlashTransactionFree(FLASH_TransactionOp_t *pTr);
static FLASH_TransactionOp_t *OTA_FlashTransactionAlloc(void);
static otaResult_t            OTA_CheckVerifyFlash(uint8_t *pData, uint32_t flash_addr, uint16_t length);
static otaResult_t            OTA_WriteToFlash(uint16_t NoOfBytes, uint32_t Addr, uint8_t *outbuf);

static ota_flash_status_t OTA_TreatFlashOpWrite(FLASH_TransactionOp_t *pMsg);
static ota_flash_status_t OTA_TreatFlashOpEraseArea(FLASH_TransactionOp_t *pMsg);
static ota_flash_status_t OTA_TreatFlashOpEraseNextBlock(FLASH_TransactionOp_t *pMsg);
static ota_flash_status_t OTA_TreatFlashOpEraseNextBlockComplete(FLASH_TransactionOp_t *pMsg);
static ota_flash_status_t OTA_TreatFlashOpEraseSector(FLASH_TransactionOp_t *pMsg);

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

static ota_config_t configuration = {
    .PostedOpInIdleTask         = false,
    .maxConsecutiveTransactions = 3,
};

static OtaStateCtx_t mHdl = {
    .LoadOtaImageInProgress = false,
    .OtaImageTotalLength    = 0,
    .OtaImageCurrentLength  = 0,
    .CurrentStorageAddress  = 0,
    .ErasedUntilAddress     = 0,
    .NewImageReady          = false,
    .SelectedFlash          = NULL,
    .ImageOffset            = 0,
    .MaxImageLength         = 0,

    .q_sz                  = 0,
    .q_max                 = 0,
    .StorageAddressWritten = 0,
    .OtaImageLengthWritten = 0,
    .PostedQueue_storage   = NULL,
    .PostedQueue_capacity  = 0,
    .isInitialized         = false,
    .config                = &configuration,
};

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

otaResult_t OTA_ServiceInit(void *posted_ops_storage, size_t posted_ops_sz)
{
    otaResult_t           st = gOtaSuccess_c;
    list_status_t         status;
    list_element_handle_t list_handle;

    /* FLASH_TransactionOpNode_t size shall be multiple of 4 bytes. The reason is to avoid
        doing unaligned access when going through the transaction operation queue, this could lead to
        crash on some toolchain (gcc) when using some instructions not supporting unaligned access*/
    assert((sizeof(FLASH_TransactionOpNode_t) & 0x3U) == 0U);

    do
    {
        int                        res            = -1;
        uint8_t                    nbTransactions = (uint8_t)(posted_ops_sz / sizeof(FLASH_TransactionOpNode_t));
        FLASH_TransactionOpNode_t *pOpNode        = (FLASH_TransactionOpNode_t *)posted_ops_storage;
        uint32_t                   posted_ops_storage_32bits;

        FLib_MemCpyWord(&posted_ops_storage_32bits, &posted_ops_storage);
        /* Check arguments */
        if (posted_ops_storage == NULL)
        {
            RAISE_ERROR(st, gOtaInvalidParam_c);
        }

        if ((posted_ops_storage_32bits & 0x3U) != 0U)
        {
            /* Avoid unaligned access on operation storage buffer, posted_ops_storage shall be word aligned */
            RAISE_ERROR(st, gOtaInvalidParam_c);
        }
        if ((posted_ops_sz % sizeof(FLASH_TransactionOpNode_t)) != 0U)
        {
            /* ops buffer size must be a multiple of transaction node */
            RAISE_ERROR(st, gOtaInvalidParam_c);
        }
        if (nbTransactions < 2U)
        {
            /* at least 2 transactions are needed to make use of posted ops */
            RAISE_ERROR(st, gOtaInvalidParam_c);
        }

        /* Check state */
        if (mHdl.LoadOtaImageInProgress || (mHdl.PostedQueue_nb_in_queue != 0U))
        {
            RAISE_ERROR(st, gOtaInvalidOperation_c);
        }

        mHdl.PostedQueue_storage = posted_ops_storage;

        LIST_Init(&mHdl.transaction_free_list, 0);

        for (uint8_t i = 0U; i < nbTransactions; i++)
        {
            void *ptr;
            ptr         = &pOpNode[i];
            list_handle = (list_element_handle_t)ptr;
            status      = LIST_AddTail(&mHdl.transaction_free_list, list_handle);
            assert(status == kLIST_Ok);
            (void)status;
        }

        mHdl.PostedQueue_capacity = nbTransactions;
        res                       = PLATFORM_OtaClearBootFlags();
        if (res < 0)
        {
            RAISE_ERROR(st, gOtaError_c);
        }

        if (OSA_MutexCreate((osa_mutex_handle_t)mHdl.msgQueueMutex) != KOSA_StatusSuccess)
        {
            RAISE_ERROR(st, gOtaError_c);
        }

        mHdl.isInitialized = true;
    } while (false);

    return st;
}

otaResult_t OTA_ServiceDeInit(void)
{
    otaResult_t st = gOtaSuccess_c;
    do
    {
        mHdl.PostedQueue_capacity    = 0;
        mHdl.PostedQueue_nb_in_queue = 0;

        if (OSA_MutexDestroy((osa_mutex_handle_t)mHdl.msgQueueMutex) != KOSA_StatusSuccess)
        {
            RAISE_ERROR(st, gOtaError_c);
        }
        mHdl.isInitialized = false;
    } while (false);

    return st;
}

void OTA_GetDefaultConfig(ota_config_t *userConfig)
{
    assert(userConfig != NULL);
    (void)memcpy(userConfig, mHdl.config, sizeof(ota_config_t));
}

void OTA_SetConfig(ota_config_t *userConfig)
{
    assert(userConfig != NULL);
    (void)memcpy(mHdl.config, userConfig, sizeof(ota_config_t));
}

/*! *********************************************************************************
 * \brief  Starts the process of writing a new image to the OTA storage.
 *
 * \param[in] length: the length of the image to be written in the OTA storage
 *
 * \return
 *  - gOtaInvalidParam_c: the intended length is bigger than the FLASH capacity
 *  - gOtaInvalidOperation_c: the process is already started (can be cancelled)
 *  - gOtaEepromError_c: can not detect external OTA storage
 *
 ********************************************************************************** */
otaResult_t OTA_StartImage(uint32_t length)
{
    otaResult_t status = gOtaSuccess_c;
    do
    {
        if (NULL == mHdl.SelectedFlash)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        /* Check if we already have an operation of writing an OTA image in the OTA Storage
        in progress and if yes, deny the current request */
        if (mHdl.LoadOtaImageInProgress)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }

        /* Check if the internal FLASH and the OTA storage have enough room to store
           the image */
        if ((length > mHdl.MaxImageLength) || (length > (mHdl.SelectedFlash->total_size - mHdl.ImageOffset)))
        {
            RAISE_ERROR(status, gOtaImageTooLarge_c);
        }

        /* Save the total length of the OTA image */
        mHdl.OtaImageTotalLength = length;
        /* Init the length of the OTA image currently written */
        mHdl.OtaImageCurrentLength = 0;
        /* Init the current OTA Storage write address */
        mHdl.CurrentStorageAddress = mHdl.ImageOffset;
        /* Mark that we have started loading an OTA image in OTA Storage */
        mHdl.LoadOtaImageInProgress = TRUE;
        mHdl.OtaImageLengthWritten  = 0;
        mHdl.StorageAddressWritten  = mHdl.ImageOffset;

    } while (false);
    return status;
}

/*! *********************************************************************************
 * \brief  Places the next image chunk into the external FLASH. The CRC will not be computed.
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
otaResult_t OTA_PushImageChunk(uint8_t *pData, uint16_t length, uint32_t *pImageLength, uint32_t *pImageOffset)
{
    otaResult_t status = gOtaSuccess_c;
    do
    {
        bool posted_pos = OTA_UsePostedOperation();
        if (mHdl.SelectedFlash == NULL)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        /* Cannot add a chunk without a prior call to OTA_StartImage() */
        if (!mHdl.LoadOtaImageInProgress)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        /* Validate parameters */
        if ((length == 0U) || (pData == NULL))
        {
            RAISE_ERROR(status, gOtaInvalidParam_c);
        }
        /* Check if the chunk does not extend over the boundaries of the image */
        if (mHdl.OtaImageCurrentLength + length > mHdl.OtaImageTotalLength)
        {
            RAISE_ERROR(status, gOtaInvalidParam_c);
        }

        /* Received a chunk with offset */
        if (NULL != pImageOffset)
        {
            mHdl.CurrentStorageAddress = mHdl.ImageOffset + *pImageOffset;
        }
        if (posted_pos)
        {
            OTA_DEBUG_TRACE("storage addr=%x length=%d\r\n", mHdl.CurrentStorageAddress, length);
            status = OTA_PostWriteToFlash(length, mHdl.CurrentStorageAddress, pData);
        }
        else
        {
            /* Try to write the data chunk into the image storage */
            status = OTA_WriteToFlash(length, mHdl.CurrentStorageAddress, pData);
        }

        if (status != gOtaSuccess_c)
        {
            break;
        }
        /* Data chunk successfully written into OTA Storage
        Update operation parameters */
        mHdl.CurrentStorageAddress += length;
        mHdl.OtaImageCurrentLength += length;

        /* Return the currently written length of the OTA image to the caller */
        if (pImageLength != NULL)
        {
            *pImageLength = mHdl.OtaImageCurrentLength;
        }
    } while (false);
    return status;
}

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
otaResult_t OTA_PullImageChunk(uint8_t *pData, uint16_t length, uint32_t *pImageOffset)
{
    otaResult_t        status = gOtaSuccess_c;
    ota_flash_status_t st;

    do
    {
        bool     posted_ops;
        uint32_t mAbsoluteOffset;
        /* Validate parameters */
        if ((length == 0U) || (pData == NULL) || (pImageOffset == NULL))
        {
            RAISE_ERROR(status, gOtaInvalidParam_c);
        }

        if (mHdl.SelectedFlash == NULL)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        posted_ops      = OTA_UsePostedOperation();
        mAbsoluteOffset = mHdl.ImageOffset + *pImageOffset;
        if (posted_ops)
        {
            /* When posted operations are used, a requested chunk may be partially written to flash
             * and the remainder staged in RAM buffer.
             */
            uint32_t end_of_queried_data;
            end_of_queried_data = mAbsoluteOffset + length;
            if (mAbsoluteOffset > mHdl.StorageAddressWritten)
            {
                if (mAbsoluteOffset <= (mHdl.StorageAddressWritten + PROGRAM_PAGE_SZ) &&
                    (end_of_queried_data <= (mHdl.StorageAddressWritten + PROGRAM_PAGE_SZ + 1U)))
                {
                    /* The asked buffer is still in RAM */
                    FLib_MemCpy(pData, mHdl.cur_transaction->buf + (mAbsoluteOffset - PROGRAM_PAGE_SZ), length);
                    status = gOtaSuccess_c;
                    break;
                }
            }
            else
            {
                /* so (mAbsoluteOffset <= mHdl.StorageAddressWritten)
                 * end_of_queried_data should be positioned inside the selected image partition
                 * taking into consideration the offset at which the image starts in flash */
                if ((end_of_queried_data > (mHdl.OtaImageLengthWritten + mHdl.ImageOffset)) &&
                    (end_of_queried_data < (mHdl.StorageAddressWritten + PROGRAM_PAGE_SZ)))
                {
                    uint16_t lenInFlash = (length - (uint16_t)mHdl.OtaImageLengthWritten);
                    uint16_t lenInRam   = (length - lenInFlash);
                    /* The asked buffer is in Flash and in RAM */
                    st = mHdl.SelectedFlash->ops.readData(lenInFlash, mAbsoluteOffset, pData);
                    if (st != kStatus_OTA_Flash_Success)
                    {
                        RAISE_ERROR(status, gOtaExternalFlashError_c);
                    }
                    pData += lenInFlash;
                    FLib_MemCpy(pData, mHdl.cur_transaction->buf, lenInRam);
                    status = gOtaSuccess_c;
                    break;
                }
            }
        }
        /* The asked buffer is in Flash */
        st = mHdl.SelectedFlash->ops.readData(length, mAbsoluteOffset, pData);
        if (st != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }
        status = gOtaSuccess_c;
    } while (false);
    return status;
}

/*! *********************************************************************************
 * \brief  Finishes the writing of a new image to the permanent storage.
 *         It will write the image header (signature and length) and footer (sector copy bitmap).
 *
 * \param[in] bitmap   pointer to a  byte array indicating the sector erase pattern for the
 *                     internal FLASH before the image update.
 *
 * \return
 *  - gOtaInvalidOperation_c: the process is not started,
 *  - gOtaEepromError_c: error while trying to write the OTA Storage
 *
 ********************************************************************************** */
otaResult_t OTA_CommitImage(uint8_t *pBitmap)
{
    NOT_USED(pBitmap);
    otaResult_t status = gOtaSuccess_c;
    do
    {
        OtaLoaderInfo_t ota_load_info;

        /* Cannot commit a image without a prior call to OTA_StartImage() */
        if (!mHdl.LoadOtaImageInProgress)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        /* Cannot commit if the image hasn't been completely received */
        if (mHdl.OtaImageCurrentLength != mHdl.OtaImageTotalLength)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        /* Writes the pending data to flash */
        OTA_WritePendingData();
        /* After flushing the remainder the written length must match the queued length */
        if (mHdl.OtaImageLengthWritten != mHdl.OtaImageTotalLength)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        ota_load_info.image_sz   = mHdl.OtaImageTotalLength;
        ota_load_info.image_addr = mHdl.CurrentStorageAddress;
        ota_load_info.pBitMap    = pBitmap;
        int res;
        res = PLATFORM_OtaBootDataUpdateOnCommit(&ota_load_info);
        if (0 != res)
        {
            break;
        }
        /* Flash flags will be write in next instance of idle task */
        mHdl.NewImageReady = TRUE;
        /* End the load of OTA image in OTA storage process */
        mHdl.LoadOtaImageInProgress = FALSE;
    } while (false);
    return status;
}

/*! *********************************************************************************
 * \brief  Set the boot flags, to trigger the Bootloader at the next CPU reset.
 *
 ********************************************************************************** */
void OTA_SetNewImageFlag(void)
{
    /* OTA image successfully written into the non-volatile storage.
       Set the boot flag to trigger the Bootloader at the next CPU Reset. */

    int st;

    if (mHdl.NewImageReady)
    {
        OtaLoaderInfo_t loader_info;
        loader_info.image_addr = mHdl.SelectedFlash->base_offset + mHdl.ImageOffset;
        loader_info.image_sz   = mHdl.OtaImageTotalLength;
        loader_info.pBitMap    = NULL;
        if (mHdl.SelectedFlash->use_internal_storage != 0U)
        {
            loader_info.sb_arch_in_ext_flash = false;
            loader_info.spi_baudrate         = 0;
        }
        else
        {
            loader_info.sb_arch_in_ext_flash = true;
            loader_info.spi_baudrate         = 4000000U; // TODO pass the right value
        }
        st = PLATFORM_OtaUpdateBootFlags(&loader_info);
        if (st >= 0)
        {
            mHdl.NewImageReady = FALSE;
        }
    }
}

/*! *********************************************************************************
 * \brief  Cancels the process of writing a new image to the OTA storage.
 *
 ********************************************************************************** */
void OTA_CancelImage(void)
{
    mHdl.LoadOtaImageInProgress = FALSE;
}

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
uint16_t OTA_CrcCompute(uint8_t *pData, uint16_t lenData, uint16_t crcValueOld)
{
    uint8_t i;

    while (0U != (lenData--))
    {
        crcValueOld ^= (uint16_t)((uint16_t)*pData++ << 8);
        for (i = 0; i < 8U; ++i)
        {
            if (0U != (crcValueOld & 0x8000U))
            {
                crcValueOld = (crcValueOld << 1) ^ 0x1021U;
            }
            else
            {
                crcValueOld = crcValueOld << 1;
            }
        }
    }
    return crcValueOld;
}

/*! *********************************************************************************
 * \brief  This function is called in order to erase the entire image storage
 *         (external memory or internal flash)
 *
 * \return  error code.
 *
 ********************************************************************************** */
otaResult_t OTA_EraseStorageMemory(void)
{
    otaResult_t status;
    do
    {
        ota_flash_status_t st;

        if (NULL == mHdl.SelectedFlash)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        st = mHdl.SelectedFlash->ops.format_storage();
        if (st != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }
        status = gOtaSuccess_c;
    } while (false);
    return status;
}

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
otaResult_t OTA_ReadStorageMemory(uint8_t *pData, uint16_t length, uint32_t address)
{
    otaResult_t status;
    do
    {
        ota_flash_status_t st;
        if (NULL == mHdl.SelectedFlash)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }

        st = mHdl.SelectedFlash->ops.readData(length, address, pData);
        if (st != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }

        status = gOtaSuccess_c;
    } while (false);

    return status;
}

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
otaResult_t OTA_WriteStorageMemory(uint8_t *pData, uint16_t length, uint32_t address)
{
    otaResult_t status;
    do
    {
        ota_flash_status_t st;

        if (NULL == mHdl.SelectedFlash)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }
        st = mHdl.SelectedFlash->ops.writeData(length, address, pData);
        if (st != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }

#if gOtaVerifyWrite_d
        status = OTA_CheckVerifyFlash(pData, address, length);
#else
        status = gOtaSuccess_c;
#endif

    } while (false);
    return status;
}

/*! *********************************************************************************
 * \brief  Called in background to poll whether current flash transactions completed
 *         and process the next one from the queue.
 *
 * \return  number of transactions treated.
 *
 ********************************************************************************** */
int OTA_TransactionResume(void)
{
    int                nb_treated = 0;
    ota_flash_status_t st         = kStatus_OTA_Flash_Success;

    if (mHdl.isInitialized)
    {
        /* Mutex to lock transaction processing */
        osa_status_t status = OSA_MutexLock(mHdl.msgQueueMutex, osaWaitForever_c);
        assert(status == KOSA_StatusSuccess);

        while (OTA_UsePostedOperation() &&
               (kStatus_OTA_Flash_Success == st) /* Stop as soon as there is an error */
               /* && mHdl.LoadOtaImageInProgress */
               && OTA_IsTransactionPending() /* There are queued flash operations pending in queue */
               && (nb_treated <
                   mHdl.config->maxConsecutiveTransactions)) /* ... but do not schedule too many in a same pass */
        {
            if (mHdl.SelectedFlash->ops.isBusy() != 0U)
            {
                /* There were transactions pending but we consumed none */
                mHdl.cnt_idle_op++;
                if (mHdl.cnt_idle_op > mHdl.max_cnt_idle)
                {
                    mHdl.max_cnt_idle = mHdl.cnt_idle_op;
                }
                break;
            }
            nb_treated++;
            /* Use MSG_GetHead so as to leave Msg in queue so that op_type or sz can be transformed when operation
             * completes (in particular for block erasure) */
            FLASH_TransactionOp_t *pMsg = MSG_QueueGetHead(&mHdl.op_queue);
            if (pMsg == NULL)
            {
                break;
            }
            switch (pMsg->op_type)
            {
                case FLASH_OP_WRITE:
                {
                    st = OTA_TreatFlashOpWrite(pMsg);
                }
                break;

                case FLASH_OP_ERASE_AREA:
                {
                    st = OTA_TreatFlashOpEraseArea(pMsg);
                }
                break;
                case FLASH_OP_ERASE_NEXT_BLOCK:
                {
                    st = OTA_TreatFlashOpEraseNextBlock(pMsg);
                }
                break;
                case FLASH_OP_ERASE_NEXT_BLOCK_COMPLETE:
                {
                    st = OTA_TreatFlashOpEraseNextBlockComplete(pMsg);
                }
                break;
                case FLASH_OP_ERASE_BLOCK:
                case FLASH_OP_ERASE_SECTOR:
                {
                    st = OTA_TreatFlashOpEraseSector(pMsg);
                }
                break;
                default:
                {
                    /*MISRA rule 16.4*/
                    assert(0);
                    break;
                }
            };
        } /* while */
        /* There were transactions pending but we consumed some */
        mHdl.cnt_idle_op = 0;
        if (st != kStatus_OTA_Flash_Success)
        {
            OTA_CancelImage();
            (void)OTA_TransactionQueuePurge();
        }
        /* Unlock Mutex to be accessed by other tasks */
        status = OSA_MutexUnlock(mHdl.msgQueueMutex);
        assert(status == KOSA_StatusSuccess);
    }
    return nb_treated;
}

/*****************************************************************************
 *   OTA_MakeHeadRoomForNextBlock
 *
 *  This function is called in order to erase enough blocks to receive next OTA window
 *
 *****************************************************************************/
otaResult_t OTA_MakeHeadRoomForNextBlock(uint32_t size, ota_op_completion_cb_t cb, uint32_t param)
{
    otaResult_t                status = gOtaSuccess_c;
    union ota_op_completion_cb callback;
    callback.func = cb;

    FLASH_TransactionOp_t *pMsg;

    do
    {
        if (NULL == mHdl.SelectedFlash)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }

        if (size == 0U)
        {
            RAISE_ERROR(status, gOtaInvalidParam_c);
        }
        if (OTA_UsePostedOperation())
        {
            pMsg = OTA_FlashTransactionAlloc();
            if (pMsg == NULL)
            {
                assert(pMsg == NULL);
                RAISE_ERROR(status, gOtaError_c);
            }

            pMsg->flash_addr             = mHdl.ErasedUntilAddress;
            pMsg->sz                     = (int32_t)size;
            pMsg->op_type                = FLASH_OP_ERASE_NEXT_BLOCK;
            *(uint32_t *)(&pMsg->buf[0]) = callback.pf;
            *(uint32_t *)(&pMsg->buf[4]) = param;

            OTA_MsgQueue(pMsg);

            if (!mHdl.config->PostedOpInIdleTask)
            {
                /* Always take head of queue */
                (void)OTA_TransactionResume();
            }
        }
        else
        {
            /* Make Headroom for the synchronous execution case */
            ota_flash_status_t st;
            uint32_t           erase_addr = mHdl.ErasedUntilAddress;
            int32_t            remain_sz  = (int32_t)size;
            st                            = mHdl.SelectedFlash->ops.eraseArea(&erase_addr, &remain_sz, false);
            if (kStatus_OTA_Flash_Success == st)
            {
                mHdl.ErasedUntilAddress = erase_addr;
                if (callback.func != NULL)
                {
                    callback.func(param);
                }
            }
            else
            {
                status = gOtaError_c;
            }
        }
    } while (false);

    return status;
}

/*****************************************************************************
 *  OTA_GetSelectedFlashAvailableSpace
 *
 *  return SelectedFlash->total_size if selected 0 otherwise.
 *
 *****************************************************************************/
uint32_t OTA_GetSelectedFlashAvailableSpace(void)
{
    uint32_t sz = 0;
    if (mHdl.SelectedFlash != NULL)
    {
        sz = mHdl.SelectedFlash->total_size;
    }
    return sz;
}

otaResult_t OTA_SelectInternalStoragePartition(void)
{
    otaResult_t status = gOtaInternalFlashError_c;
    do
    {
        hal_flash_status_t flashInitStatus;

        if (mHdl.LoadOtaImageInProgress)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }

        flashInitStatus = HAL_FlashInit();

        if (flashInitStatus != kStatus_HAL_Flash_Success)
        {
            RAISE_ERROR(status, gOtaError_c);
        }

        /* This might happen if target is not generated with gUseInternalStorageLink_d=1 */
        assert(0UL != int_storage.total_size && 0UL != int_storage.sector_size);
        OTA_DEBUG_TRACE("Select Internal flash\r\n");

        mHdl.SelectedFlash      = &int_storage;
        mHdl.ImageOffset        = PLATFORM_OtaGetImageOffset(true, 0);
        mHdl.MaxImageLength     = PLATFORM_OtaGetMaxImageSize(true);
        mHdl.ErasedUntilAddress = 0U;
        status                  = gOtaSuccess_c;
        /* Try to initialize the OTA Storage */
        if (mHdl.SelectedFlash->ops.init() != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaInternalFlashError_c);
        }

    } while (false);

    return status;
}

otaResult_t OTA_SelectExternalStoragePartition(uint32_t partition_offset, uint32_t partition_sz)
{
    otaResult_t status = gOtaExternalFlashError_c;
    do
    {
        OTA_DEBUG_TRACE("Select External flash\r\n");

        if (mHdl.LoadOtaImageInProgress)
        {
            RAISE_ERROR(status, gOtaInvalidOperation_c);
        }

        mHdl.SelectedFlash  = &ext_storage;
        mHdl.ImageOffset    = PLATFORM_OtaGetImageOffset(false, partition_offset);
        mHdl.MaxImageLength = PLATFORM_OtaGetMaxImageSize(false);

        /* Define the start of the erase process */
        mHdl.ErasedUntilAddress = mHdl.ImageOffset;

        if (mHdl.SelectedFlash->ops.init() != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }
        status = gOtaSuccess_c;
    } while (false);

    return status;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

/*****************************************************************************
 *  OTA_WritePendingData
 *
 *  Writes pending data buffer into OTA storage
 *
 *****************************************************************************/
static void OTA_WritePendingData(void)
{
    ota_flash_status_t status;
    if (OTA_UsePostedOperation())
    {
        FLASH_TransactionOp_t *pMsg = mHdl.cur_transaction;
        do
        {
            if ((pMsg != NULL) && (pMsg->sz != 0))
            {
                mHdl.cur_transaction = NULL;
                /* Submit transaction */
                OTA_MsgQueue(pMsg);
            }

            while (mHdl.SelectedFlash->ops.isBusy() != 0U)
            {
            }

            /* Make sure to flush the entire posted ops queue */
            while (OTA_IsTransactionPending())
            {
                (void)OTA_TransactionResume();
                while (mHdl.SelectedFlash->ops.isBusy() != 0U)
                {
                }
            }

        } while (false);
    }
    else
    {
        status = mHdl.SelectedFlash->ops.flushWriteBuf();
        assert(status == kStatus_OTA_Flash_Success);
        (void)status;
    }

    mHdl.OtaImageLengthWritten = mHdl.OtaImageCurrentLength;
}

/*****************************************************************************
 *  OTA_UsePostedOperation
 *
 *  Tell if erase and writes to flash are blocking.
 *
 *****************************************************************************/
static bool OTA_UsePostedOperation(void)
{
    return (mHdl.PostedQueue_capacity != 0U);
}

static otaResult_t OTA_PostWriteToFlash(uint16_t NoOfBytes, uint32_t Addr, uint8_t *pData)
{
    otaResult_t            status = gOtaSuccess_c;
    FLASH_TransactionOp_t *pMsg;
    uint8_t *              Outbuf;
    Outbuf = pData;
    do
    {
        if (mHdl.OtaImageLengthWritten > mHdl.OtaImageCurrentLength)
        {
            RAISE_ERROR(status, gOtaInvalidParam_c);
        }

        while (NoOfBytes > 0U)
        {
            uint8_t *p; /* write pointer to buffer */
            size_t   remaining_space;
            size_t   nb_bytes_copy;

            if (mHdl.cur_transaction != NULL)
            {
                pMsg = mHdl.cur_transaction;
                /* Current transaction was ongoing : continue filling it */
                remaining_space = PROGRAM_PAGE_SZ - (uint32_t)pMsg->sz;
                Addr += remaining_space;
            }
            else
            {
                pMsg = OTA_FlashTransactionAlloc();
                if (pMsg == NULL)
                {
                    assert(pMsg != NULL);
                    RAISE_ERROR(status, gOtaError_c);
                }
                pMsg->flash_addr = Addr;
                pMsg->op_type    = FLASH_OP_WRITE;
                pMsg->sz         = 0;
                remaining_space  = PROGRAM_PAGE_SZ;
            }
            p             = &pMsg->buf[pMsg->sz];
            nb_bytes_copy = MIN(remaining_space, NoOfBytes);
            FLib_MemCpy(p, Outbuf, nb_bytes_copy);
            Outbuf += nb_bytes_copy;
            pMsg->sz += (int16_t)nb_bytes_copy;
            if (pMsg->sz == (int16_t)PROGRAM_PAGE_SZ)
            {
                assert((pMsg->flash_addr % PROGRAM_PAGE_SZ) == 0);
                /* Submit transaction */
                OTA_MsgQueue(pMsg);
                if (mHdl.cur_transaction != NULL)
                {
                    mHdl.cur_transaction = NULL;
                }
                else
                {
                    Addr += PROGRAM_PAGE_SZ;
                }
            }
            else
            {
                mHdl.cur_transaction = pMsg;
            }
            NoOfBytes -= (uint16_t)nb_bytes_copy;
        }

        if ((!mHdl.config->PostedOpInIdleTask) && (OTA_IsTransactionPending()))
        {
            /* Always take head of queue */
            (void)OTA_TransactionResume();
        }
    } while (false);
    return status;
}

static FLASH_TransactionOp_t *OTA_FlashTransactionAlloc(void)
{
    FLASH_TransactionOp_t *    pTr = NULL;
    FLASH_TransactionOpNode_t *flash_transaction;
    list_element_handle_t      list_handle;
    void *                     ptr;
    OSA_DisableIRQGlobal();

    list_handle       = LIST_RemoveHead(&mHdl.transaction_free_list);
    ptr               = list_handle;
    flash_transaction = (FLASH_TransactionOpNode_t *)ptr;

    if (flash_transaction != NULL)
    {
        pTr = &flash_transaction->flash_transac;
        mHdl.PostedQueue_nb_in_queue++;
    }
    OSA_EnableIRQGlobal();

    return pTr;
}

static void OTA_FlashTransactionFree(FLASH_TransactionOp_t *pTr)
{
    list_status_t         status;
    uint8_t *             flash_transaction;
    list_element_handle_t list_handle;
    OSA_DisableIRQGlobal();
    flash_transaction = ((uint8_t *)pTr - offsetof(FLASH_TransactionOpNode_t, flash_transac));
    mHdl.PostedQueue_nb_in_queue--;
    list_handle = (list_element_handle_t)((uint32_t)flash_transaction);
    status      = LIST_AddTail(&mHdl.transaction_free_list, list_handle);
    assert(status == kLIST_Ok);
    (void)status;
    OSA_EnableIRQGlobal();
}

static bool OTA_IsTransactionPending(void)
{
    return LIST_GetSize(&mHdl.op_queue) != 0U ? true : false;
}

static void OTA_MsgQueue(FLASH_TransactionOp_t *pMsg)
{
    OSA_DisableIRQGlobal();
    (void)MSG_QueueAddTail(&mHdl.op_queue, pMsg);
    mHdl.q_sz++;
    if (mHdl.q_sz > mHdl.q_max)
    {
        mHdl.q_max = mHdl.q_sz;
    }
    OSA_EnableIRQGlobal();
}

static void OTA_MsgDequeue(void)
{
    OSA_DisableIRQGlobal();
    (void)MSG_QueueRemoveHead(&mHdl.op_queue);
    mHdl.q_sz--;
    OSA_EnableIRQGlobal();
}

/*****************************************************************************
 *  OTA_TransactionQueuePurge
 *
 *  Purge queue and abandon current posted operations
 *
 *****************************************************************************/
static int OTA_TransactionQueuePurge(void)
{
    int nb_purged = 0;
    while (OTA_IsTransactionPending())
    {
        FLASH_TransactionOp_t *pMsg = MSG_QueueGetHead(&mHdl.op_queue);
        if (pMsg == NULL)
        {
            break;
        }
        OTA_MsgDequeue();
        OTA_FlashTransactionFree(pMsg);
        nb_purged++;
    }

    if (mHdl.cur_transaction != NULL)
    {
        OTA_FlashTransactionFree(mHdl.cur_transaction);
        mHdl.cur_transaction = NULL;
    }

    return nb_purged;
}

/*****************************************************************************
 *  OTA_CheckVerifyFlash
 *
 *  Compare if flash contents matches that of RAM programmed buffer.
 *  Maybe be called when data are still held in accumulation write buffer.
 *
 *****************************************************************************/
#if (gOtaVerifyWrite_d > 0)
static otaResult_t OTA_CheckVerifyFlash(uint8_t *pData, uint32_t flash_addr, uint16_t length)
{
    otaResult_t status                                = gOtaSuccess_c;
    uint8_t     readData[gOtaVerifyWriteBufferSize_d] = {0};
    uint16_t    readLen;
    uint16_t    i = 0;
    /* Not very easy to use when writes are partial,
    the actual size written differs : works only for posted operations */
    /* We iterate so as to keep the readData buffer reasonable in size */
    while (i < length)
    {
        ota_flash_status_t st;

        readLen = length - i;

        if (readLen > sizeof(readData))
        {
            readLen = (uint16_t)sizeof(readData);
        }
        st = mHdl.SelectedFlash->ops.readData(readLen, flash_addr + i, readData);
        if (st != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }

        if (!FLib_MemCmp(&pData[i], readData, readLen))
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }

        i += readLen;
    }
    assert(status == gOtaSuccess_c);
    return status;
}
#endif /* gOtaVerifyWrite_d */

static otaResult_t OTA_WriteToFlash(uint16_t NoOfBytes, uint32_t Addr, uint8_t *outbuf)
{
    otaResult_t status = gOtaSuccess_c;
    do
    {
        /* Try to write the data chunk into the image storage */
        if (mHdl.SelectedFlash->ops.writeData(NoOfBytes, Addr, outbuf) != kStatus_OTA_Flash_Success)
        {
            RAISE_ERROR(status, gOtaExternalFlashError_c);
        }
        /* If Flash programming operation requires verification do it now
         */
#if (gOtaVerifyWrite_d > 0)
        status = OTA_CheckVerifyFlash(outbuf, Addr, NoOfBytes);
#endif /* gOtaVerifyWrite_d */
    } while (false);
    return status;
}

static ota_flash_status_t OTA_TreatFlashOpWrite(FLASH_TransactionOp_t *pMsg)
{
    ota_flash_status_t st;
    if (pMsg->sz < (int32_t)PROGRAM_PAGE_SZ) /* Should only happen at last chunk */
    {
        FLib_MemSet(&pMsg->buf[pMsg->sz], 0, PROGRAM_PAGE_SZ - ((uint32_t)pMsg->sz));
        /* Message buffer completed with 0 from pMsg-sz index to PROGRAM_PAGE_SZ
        new size is PROGRAM_PAGE_SZ */
        pMsg->sz = (int32_t)PROGRAM_PAGE_SZ;
    }
    if (OTA_WriteStorageMemory(&pMsg->buf[0], (uint16_t)pMsg->sz, pMsg->flash_addr) == gOtaSuccess_c)
    {
        mHdl.OtaImageLengthWritten += ((uint32_t)pMsg->sz);
        assert(mHdl.StorageAddressWritten == pMsg->flash_addr);
        mHdl.StorageAddressWritten += ((uint32_t)pMsg->sz);
        st = kStatus_OTA_Flash_Success;
    }
    else
    {
        OTA_WARNING_TRACE("Failed FlashOp %x @%08x sz=%d\r\n", pMsg->op_type, pMsg->flash_addr, pMsg->sz);
        st = kStatus_OTA_Flash_Error;
        assert(st == kStatus_OTA_Flash_Success);
    }
    OTA_MsgDequeue();
    OTA_FlashTransactionFree(pMsg);
    return st;
}

static ota_flash_status_t OTA_TreatFlashOpEraseArea(FLASH_TransactionOp_t *pMsg)
{
    ota_flash_status_t st;
    int32_t            remain_sz  = (int32_t)pMsg->sz;
    uint32_t           erase_addr = pMsg->flash_addr;

    st = mHdl.SelectedFlash->ops.eraseArea(&erase_addr, &remain_sz, true);
    if (kStatus_OTA_Flash_Success == st)
    {
        if (remain_sz <= 0)
        {
            OTA_MsgDequeue();
            OTA_FlashTransactionFree(pMsg);
        }
        else
        {
            /* Leave head request in queue */
            pMsg->flash_addr = erase_addr;
            pMsg->sz         = (int32_t)remain_sz;
        }
    }
    else
    {
        OTA_WARNING_TRACE("Failed FlashOp %x @%08x sz=%d\r\n", pMsg->op_type, pMsg->flash_addr, pMsg->sz);
        assert(st == kStatus_OTA_Flash_Success);
    }
    return st;
}

static ota_flash_status_t OTA_TreatFlashOpEraseNextBlock(FLASH_TransactionOp_t *pMsg)
{
    ota_flash_status_t st;
    int32_t            remain_sz = (int32_t)pMsg->sz;
    st                           = mHdl.SelectedFlash->ops.eraseArea(&pMsg->flash_addr, &remain_sz, false);
    if (kStatus_OTA_Flash_Success == st)
    {
        pMsg->op_type           = FLASH_OP_ERASE_NEXT_BLOCK_COMPLETE;
        mHdl.ErasedUntilAddress = pMsg->flash_addr;
    }
    else
    {
        OTA_WARNING_TRACE("Failed FlashOp %x @%08x sz=%d\r\n", pMsg->op_type, pMsg->flash_addr, pMsg->sz);
        assert(st == kStatus_OTA_Flash_Success);
    }
    return st;
}

static ota_flash_status_t OTA_TreatFlashOpEraseNextBlockComplete(FLASH_TransactionOp_t *pMsg)
{
    union ota_op_completion_cb cb;
    cb.func        = NULL;
    uint32_t param = 0U;

    FLib_MemCpyWord(&cb.pf, &(pMsg->buf[0]));

    if (cb.func != NULL)
    {
        FLib_MemCpyWord(&param, &(pMsg->buf[4]));
        cb.func(param);
    }
    OTA_MsgDequeue();
    OTA_FlashTransactionFree(pMsg);
    return kStatus_OTA_Flash_Success;
}

static ota_flash_status_t OTA_TreatFlashOpEraseSector(FLASH_TransactionOp_t *pMsg)
{
    ota_flash_status_t st;
    OTA_DEBUG_TRACE("Erase block @%08x sz=%d\r\n", pMsg->flash_addr, pMsg->sz);

    int32_t remain_sz = (int32_t)pMsg->sz;
    st                = mHdl.SelectedFlash->ops.eraseArea(&pMsg->flash_addr, &remain_sz, true);
    if (kStatus_OTA_Flash_Success == st)
    {
        OTA_MsgDequeue();
        OTA_FlashTransactionFree(pMsg);
    }
    else
    {
        OTA_WARNING_TRACE("Failed FlashOp %x @%08x sz=%d\r\n", pMsg->op_type, pMsg->flash_addr, pMsg->sz);
        assert(st == kStatus_OTA_Flash_Success);
    }
    return st;
}
