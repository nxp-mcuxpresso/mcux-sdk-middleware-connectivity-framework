/*! *********************************************************************************
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * \file
 *
 * This is the Source file for the EEPROM emulated inside the MCU's FLASH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */

#include "fwk_platform_flash.h"
#include "fwk_platform_ota.h"
#include "FunctionLib.h"
#include "OtaPrivate.h"
#include "OtaSupport.h"

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
#define KB(x)  ((x)*1024U)
#define MB(x)  (KB(x) * 1024U)
#define MHz(x) ((x)*1000000U)

#define OTA_WRITE_BUFFER_SIZE (1U * PLATFORM_EXTFLASH_PAGE_SIZE)

#define IS_SECTOR_ALIGNED(addr)   (((addr) & (PLATFORM_EXTFLASH_SECTOR_SIZE - 1U)) == 0U)
#define CURRENT_SECTOR_ADDR(addr) (((addr) & ~(PLATFORM_EXTFLASH_SECTOR_SIZE - 1U)))
#define NEXT_SECTOR_ADDR(addr)    (CURRENT_SECTOR_ADDR(addr) + PLATFORM_EXTFLASH_SECTOR_SIZE)

#define RAISE_ERROR(x, val) \
    {                       \
        x = (val);          \
        break;              \
    }

/* If gOtaEraseBeforeImageBlockReq_c is defined and enabled, then it means the OTA module will
 * erase the space needed for the next block before writing anything to the flash (OTA_MakeHeadRoomForNextBlock)
 * In such case, we don't need to use an erase bitmap as the erase will always be done before writing.
 */
#if !defined(gOtaEraseBeforeImageBlockReq_c) || (gOtaEraseBeforeImageBlockReq_c == 0)
/* The dimension of the Erase bitmap must be sufficient to provide one bit per flash sector. */
#define ERASE_BITMAP_SIZE  ((PLATFORM_OTA_EXTFLASH_TOTAL_SIZE / PLATFORM_EXTFLASH_SECTOR_SIZE) / 32U)
#define SET_BIT(bitmap, i) bitmap[((i) >> 5)] |= (((uint32_t)1U << ((i)&0x1fU)))
#define CLR_BIT(bitmap, i) bitmap[((i) >> 5)] &= ~(((uint32_t)1U << ((i)&0x1fU)))
#define GET_BIT(bitmap, i) (((bitmap[(i) >> 5] & (((uint32_t)1U << ((i)&0x1fU)))) >> ((i)&0x1fU)) != 0U)
#endif

/*!< Macro to convert offset relative to start of external storage */
#define PHYS_ADDR(x) ((uint32_t)(x) + ext_storage.base_offset)

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
static ota_flash_status_t ExternalFlash_Init(void);
static ota_flash_status_t ExternalFlash_ChipErase(void);
static ota_flash_status_t ExternalFlash_EraseBlock(uint32_t Addr, uint32_t size);
static ota_flash_status_t ExternalFlash_WriteData(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf);
static ota_flash_status_t ExternalFlash_FlushWriteBuffer(void);
static ota_flash_status_t ExternalFlash_ReadData(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf);
static uint8_t            ExternalFlash_isBusy(void);
static ota_flash_status_t ExternalFlash_EraseArea(uint32_t *pAddr, int32_t *pSize, bool non_blocking);
static ota_flash_status_t ExternalVerifyFlashProgram(uint8_t *pData, uint32_t Addr, uint16_t length);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/

static uint8_t  mWriteBuffer[OTA_WRITE_BUFFER_SIZE];
static uint32_t mWriteBufferAddr  = 0U;
static uint32_t mWriteBufferIndex = 0U;

#if !defined(gOtaEraseBeforeImageBlockReq_c) || (gOtaEraseBeforeImageBlockReq_c == 0)
static uint32_t eraseBitmap[ERASE_BITMAP_SIZE] = {0U};
#endif

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

const OtaFlash_t ext_storage = {
    .base_offset = PLATFORM_OTA_EXTFLASH_START_OFFSET,
    .total_size  = PLATFORM_OTA_EXTFLASH_TOTAL_SIZE,
    .sector_size = PLATFORM_EXTFLASH_SECTOR_SIZE,
    .baudrate    = MHz(24),
    .ops =
        {
            .init           = ExternalFlash_Init,
            .format_storage = ExternalFlash_ChipErase,
            .eraseBlock     = ExternalFlash_EraseBlock,
            .writeData      = ExternalFlash_WriteData,
            .readData       = ExternalFlash_ReadData,
            .isBusy         = ExternalFlash_isBusy,
            .eraseArea      = ExternalFlash_EraseArea,
            .verify         = ExternalVerifyFlashProgram,
            .flushWriteBuf  = ExternalFlash_FlushWriteBuffer,
        },
};

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

/*! *********************************************************************************
 * \brief  Initialize External storage for OTA.
 *
 * \return    kStatus_HAL_Flash_Success if successful, other values in case of error
 *
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_Init(void)
{
    ota_flash_status_t status = kStatus_OTA_Flash_Success;

    static bool init_done = false;
    do
    {
        if (!init_done)
        {
            status = kStatus_OTA_Flash_Error;
            status_t st;

            st = PLATFORM_InitExternalFlash(ext_storage.baudrate);

#if !defined(gOtaEraseBeforeImageBlockReq_c) || (gOtaEraseBeforeImageBlockReq_c == 0)
            FLib_MemSet(eraseBitmap, 0x0U, ERASE_BITMAP_SIZE);
#endif

            if (kStatus_Success != st)
            {
                OTA_DEBUG_TRACE("Init fail %x\r\n", st);
                break;
            }
            init_done = true;
        }
        status = kStatus_OTA_Flash_Success;
    } while (false);
    return status;
}

/*! *********************************************************************************
 * \brief  Clean External storage partition by erasing all sectors
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 *
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_ChipErase(void)
{
    ota_flash_status_t status;

    status = ExternalFlash_EraseBlock(PLATFORM_OTA_EXTFLASH_START_OFFSET, PLATFORM_OTA_EXTFLASH_TOTAL_SIZE);

    return status;
}

/*! *********************************************************************************
 * \brief  Erase a sector size worth of data in External storage.
 *
 * \param[in] Addr     offset address from which erase operation is required
 * \param[in] size     must be gIntFlash_SectorSize_c for External flash
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 *
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_EraseBlock(uint32_t Addr, uint32_t size)
{
    ota_flash_status_t status  = kStatus_OTA_Flash_Success;
    uint32_t           endAddr = Addr + size;
    uint32_t           startBlock, endBlock;
    uint32_t           index;

    if ((endAddr & (PLATFORM_EXTFLASH_SECTOR_SIZE - 1U)) != 0U)
    {
        /* If the address is in the middle of a block, round up to the next block
         * This gives the upper block limit, every blocks before this one will be erased but not this block
         * as it is outside the erase range */
        endAddr = ((endAddr / PLATFORM_EXTFLASH_SECTOR_SIZE) + 1U) * PLATFORM_EXTFLASH_SECTOR_SIZE;
    }

    startBlock = Addr / PLATFORM_EXTFLASH_SECTOR_SIZE;
    endBlock   = endAddr / PLATFORM_EXTFLASH_SECTOR_SIZE;
    index      = startBlock;

    while (index < endBlock)
    {
#if !defined(gOtaEraseBeforeImageBlockReq_c) || (gOtaEraseBeforeImageBlockReq_c == 0)
        if (GET_BIT(eraseBitmap, index))
        {
            /* If the block is already erased, skip */
            index++;
        }
        else
#endif
        {
            uint32_t blockAddr  = index * PLATFORM_EXTFLASH_SECTOR_SIZE;
            uint32_t blockCount = endBlock - index;
            uint32_t eraseSize  = blockCount * PLATFORM_EXTFLASH_SECTOR_SIZE;

            if (PLATFORM_EraseExternalFlash(PHYS_ADDR(blockAddr), eraseSize) != kStatus_Success)
            {
                status = kStatus_OTA_Flash_Fail;
                break;
            }

#if !defined(gOtaEraseBeforeImageBlockReq_c) || (gOtaEraseBeforeImageBlockReq_c == 0)
            for (uint32_t i = 0U; i < blockCount; i++)
            {
                /* Tag all the blocks as erased */
                SET_BIT(eraseBitmap, index + i);
            }
#endif

            index += blockCount;
        }
    }

    return status;
}

/*! *********************************************************************************
 * \brief  Writes a data buffer into External storage, at a given address
 *
 * \param[in] NoOfBytes   Number of bytes to be written
 * \param[in] Addr        offset address relative to start of External Storage
 * \param[in] Outbuf     pointer on buffer to be written
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_WriteData(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf)
{
    ota_flash_status_t status = kStatus_OTA_Flash_Success;

    do
    {
        if (Addr > PLATFORM_OTA_EXTFLASH_TOTAL_SIZE)
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }
        if (0U == NoOfBytes)
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }
        if (NoOfBytes >= (uint32_t)(PLATFORM_OTA_EXTFLASH_TOTAL_SIZE - Addr))
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }

        if (NoOfBytes == PLATFORM_EXTFLASH_PAGE_SIZE)
        {
            /* Write the current write buffer to flash */
            if (PLATFORM_WriteExternalFlash(Outbuf, NoOfBytes, PHYS_ADDR(Addr)) != kStatus_Success)
            {
                status = kStatus_OTA_Flash_Fail;
                break;
            }
        }
        else
        {
            while (NoOfBytes > 0U)
            {
                uint32_t sizeToCopy;
                uint32_t pageId               = Addr / OTA_WRITE_BUFFER_SIZE;
                uint32_t pageAddr             = pageId * OTA_WRITE_BUFFER_SIZE;
                uint32_t pageEndAddr          = (pageId + 1U) * OTA_WRITE_BUFFER_SIZE - 1U;
                uint32_t remainingBytesInPage = pageEndAddr - Addr + 1U;
                uint32_t offset               = Addr - pageAddr;
                bool     isWriteBufferFull    = false;

                mWriteBufferAddr = pageAddr;

                if (NoOfBytes >= remainingBytesInPage)
                {
                    sizeToCopy        = remainingBytesInPage;
                    isWriteBufferFull = true;
                }
                else
                {
                    sizeToCopy = NoOfBytes;
                }

                /* Copy the data to write in the buffer */
                FLib_MemCpy(&mWriteBuffer[offset], Outbuf, sizeToCopy);
                mWriteBufferIndex += sizeToCopy;

                if (isWriteBufferFull == true)
                {
                    /* The write buffer is full, we can store all the data in flash */
                    status = ExternalFlash_FlushWriteBuffer();
                    if (status != kStatus_OTA_Flash_Success)
                    {
                        break;
                    }
                }

                /* Substract copied bytes from the data to be copied, and increase address offset */
                NoOfBytes -= sizeToCopy;
                Addr += sizeToCopy;
                Outbuf += sizeToCopy;
            }
        }

    } while (false);
    return status;
}

/*! *********************************************************************************
 * \brief  Writes remainder of 16 byte buffer to flash when terminating FW update
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_FlushWriteBuffer(void)
{
    ota_flash_status_t status;
    status_t           st;

    status = kStatus_OTA_Flash_Success;

    do
    {
#if !defined(gOtaEraseBeforeImageBlockReq_c) || (gOtaEraseBeforeImageBlockReq_c == 0)
        /* Erase the sector if needed */
        status = ExternalFlash_EraseBlock(mWriteBufferAddr, PLATFORM_EXTFLASH_SECTOR_SIZE);
        if (kStatus_OTA_Flash_Success != status)
        {
            break;
        }
#endif

        /* Write the current write buffer to flash */
        st = PLATFORM_WriteExternalFlash(mWriteBuffer, OTA_WRITE_BUFFER_SIZE, PHYS_ADDR(mWriteBufferAddr));
        if (st != kStatus_Success)
        {
            status = kStatus_OTA_Flash_Fail;
            break;
        }

#if 0
        /* debug - verify the data stored */
        static uint8_t verifyBuffer[PLATFORM_EXTFLASH_SECTOR_SIZE];

        status = ExternalFlash_ReadData(PLATFORM_EXTFLASH_SECTOR_SIZE, mWriteBufferAddr, verifyBuffer);
        if(memcmp(mWriteBuffer, verifyBuffer, PLATFORM_EXTFLASH_SECTOR_SIZE) != 0)
        {
            status = kStatus_OTA_Flash_Fail;
            break;
        }
#endif

        /* Reset the write buffer for the next sector */
        FLib_MemSet(mWriteBuffer, 0xFFU, OTA_WRITE_BUFFER_SIZE);
        mWriteBufferIndex = 0U;

    } while (false);

    return status;
}

/*! *********************************************************************************
 * \brief  Read data from an address pointing to External flash to a RAM buffer
 *
 * \param[in] NoOfBytes   Number of bytes to be read
 * \param[in] Addr        offset address relative to start of External Storage
 * \param[out] Outbuf      pointer on buffer to be write to
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_ReadData(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf)
{
    ota_flash_status_t status = kStatus_OTA_Flash_Error;
    status_t           st;

    st = PLATFORM_ReadExternalFlash(inbuf, NoOfBytes, PHYS_ADDR(Addr), true);
    if (st == kStatus_Success)
    {
        status = kStatus_OTA_Flash_Success;
    }

    if (status == kStatus_OTA_Flash_Success)
    {
        for (uint32_t i = 0U; i < mWriteBufferIndex; i++)
        {
            if ((mWriteBufferAddr + i) >= Addr && (mWriteBufferAddr + i) < (Addr + NoOfBytes))
            {
                inbuf[mWriteBufferAddr - Addr + i] = mWriteBuffer[i];
            }
        }
    }

    return status;
}

/*! *********************************************************************************
 * \brief  Return busy status i.e. whether engaged in Program or Erase operation
 *
 * \return    1 if busy, 0 otherwise
 ***********************************************************************************/
static uint8_t ExternalFlash_isBusy(void)
{
    status_t st;
    bool     isBusy = true;

    st = PLATFORM_IsExternalFlashBusy(&isBusy);
    assert(st == kStatus_Success);
    NOT_USED(st);

    return (uint8_t)isBusy;
}

/*! *********************************************************************************
 * \brief  Erase area comprised between address up to required size
 *
 * \param[in] pAddr on entry *pAddr contain address where to start erasing and is
 *            updated to the limit where is was actually erased on exit
 * \param[in] pSize         *pSize contains the size to erase on entry, is update to
 *            the value actually erased on exit (rounded to the multiple of sectors)
 * \param[in] non_blocking not used in this implementation
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_EraseArea(uint32_t *pAddr, int32_t *pSize, bool non_blocking)
{
    ota_flash_status_t status;
    uint32_t           remain_sz  = (uint32_t)*pSize;
    uint32_t           erase_addr = *pAddr;
    /* The erase operation in internal flash is necessarily blocking */
    NOT_USED(non_blocking);

    do
    {
        if (!IS_SECTOR_ALIGNED(erase_addr))
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_AlignmentError);
        }
        if (remain_sz != 0U)
        {
            status = ExternalFlash_EraseBlock(erase_addr, remain_sz);
            if (status != kStatus_OTA_Flash_Success)
            {
                break;
            }
            erase_addr += remain_sz;
            /* In the case of internal flash the entire erase operation must
             * complete till the end
             */
            remain_sz = 0U;
        }
        status = kStatus_OTA_Flash_Success;
    } while (false);

    *pAddr = NEXT_SECTOR_ADDR(erase_addr);
    *pSize = (int32_t)remain_sz;

    return status;
}

/*! *********************************************************************************
 * \brief  Read back flash after programming operation
 *
 * \param[in] pData pointer of programmed content buffer
 *
 * \param[in] Addr  offset from which to read back in external flash
 *
 * \param[in] length size to compare
 *
 * \return    kStatus_OTA_Flash_Success if identical, kStatus_OTA_Flash_Fail otherwise
 ***********************************************************************************/
static ota_flash_status_t ExternalVerifyFlashProgram(uint8_t *pData, uint32_t Addr, uint16_t length)
{
    // TODO ?
    NOT_USED(pData);
    NOT_USED(Addr);
    NOT_USED(length);

    return kStatus_OTA_Flash_Success;
}
