/*! *********************************************************************************
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * \file
 *
 * This is the Source file for the EEPROM emulated inside the MCU's FLASH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ********************************************************************************** */
#include "FunctionLib.h"
#include "OtaPrivate.h"
#include "mflash_drv.h"

#include "fsl_common.h"
#include "fsl_device_registers.h"

/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
#define KB(x)  (uint32_t)((x)*1024)
#define MB(x)  (uint32_t)(KB(x) * 1024)
#define MHz(x) ((uint32_t)(x)*1000000)

#define IS25WP064A
#ifdef IS25WP064A
#define gExtFlash_TotalSize_c      MB(8)
#define gExtFlash_SectorSize_c     KB(4) /* termed block not sector */
#define gExtFlash_PageSectorSize_c 256
#endif

#define IS_SECTOR_ALIGNED(addr) (((addr) & (gExtFlash_SectorSize_c - 1)) == 0)
#define IS_PAGE_ALIGNED(addr)   (((addr) & (gExtFlash_PageSectorSize_c - 1)) == 0)

/* Partition Info */
extern uint32_t OTA_STORAGE_START_ADDRESS[];
extern uint32_t OTA_STORAGE_SIZE[];
#define PARTITION_START ((uint32_t)OTA_STORAGE_START_ADDRESS)
#define PARTITION_SIZE  ((uint32_t)OTA_STORAGE_SIZE)

#define gExtFlash_StartOffset_c  (PARTITION_START)
#define gExtFlash_EndPartition_c (PARTITION_START + PARTITION_SIZE)

#define RAISE_ERROR(x, val) \
    {                       \
        x = (val);          \
        break;              \
    }

/* The dimension of the Erase bitmap must be sufficient to provide one bit per
 *  flash sector.
 */
#define StorageBitmapSize  ((gExtFlash_TotalSize_c / gExtFlash_SectorSize_c) / 32)
#define SET_BIT(bitmap, i) bitmap[((i) >> 5)] |= (1U << ((i)&0x1f));
#define GET_BIT(bitmap, i) ((bitmap[(i) >> 5] >> ((i)&0x1f)) != 0)

#define kStatus_Busy (-1)

#define gExtEepromParams_WriteAlignment_c 0

typedef enum
{
    kSize_ErasePage = 0x1, /*!< A page data of eeprom will be erased.*/
    kSize_Erase4K   = 0x2, /*!< 4*1024 bytes data of eeprom will be erased.*/
    kSize_Erase32K  = 0x3,
    kSize_Erase64K  = 0x4,
    kSize_EraseAll  = 0x5,
} eraseOptions_t;

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
static ota_flash_status_t ExternalFlash_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr);
static ota_flash_status_t ExternalFlash_Init(void);
static ota_flash_status_t ExternalFlash_ChipErase(void);
static ota_flash_status_t ExternalFlash_EraseBlock(uint32_t Addr, uint32_t size);
static ota_flash_status_t ExternalFlash_WriteData(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf);
static ota_flash_status_t ExternalFlash_FlushWriteBuffer(void);
static ota_flash_status_t ExternalFlash_ReadData(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf);
static uint8_t            ExternalFlash_isBusy(void);
static ota_flash_status_t ExternalFlash_EraseArea(uint32_t *pAddr, int32_t *pSize, bool non_blocking);
static ota_flash_status_t ExternalVerifyFlashProgram(uint8_t *pData, uint32_t Addr, uint16_t length);
static ota_flash_status_t ExternalFlash_EraseBlockBySectorNumber(uint32_t blk_index, eraseOptions_t eraseOpt);

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

static uint32_t mEepromEraseBitmap[StorageBitmapSize];

#if (gExtEepromParams_WriteAlignment_c > 1)
static uint8_t  mWriteBuff[gExtEepromParams_WriteAlignment_c];
static uint8_t  mWriteBuffLevel = 0;
static uint32_t mWriteBuffAddr  = 0;
#endif

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

const OtaFlash_t ext_storage = {.base_offset = gExtFlash_StartOffset_c,
                                .total_size  = gExtFlash_TotalSize_c,
                                .sector_size = gExtFlash_SectorSize_c,
                                .baudrate    = MHz(24),

                                .ops = {
                                    .init           = ExternalFlash_Init,
                                    .format_storage = ExternalFlash_ChipErase,
                                    .eraseBlock     = ExternalFlash_EraseBlock,
                                    .writeData      = ExternalFlash_WriteData,
                                    .readData       = ExternalFlash_ReadData,
                                    .isBusy         = ExternalFlash_isBusy,
                                    .eraseArea      = ExternalFlash_EraseArea,
                                    .verify         = ExternalVerifyFlashProgram,
                                    .flushWriteBuf  = ExternalFlash_FlushWriteBuffer,

                                }};

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
            /*  Ensure that Storage BitMap size is sufficient to deal with the entire
                External storage area : here the sector size is 4kB and the storage size
                is ~ 8MB */
            /* Wipe clean the EraseBitMap */
            FLib_MemSet(mEepromEraseBitmap, 0x00, StorageBitmapSize * 4);
            st = mflash_drv_init();
            if (kStatus_Success != st)
            {
                OTA_DEBUG_TRACE("Init fail %x\r\n", st);
                break;
            }
            init_done = true;
        }
        status = kStatus_OTA_Flash_Success;
    } while (0);
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
    return kStatus_OTA_Flash_Fail;
}

/*! *********************************************************************************
 * \brief  Called internally to erase block - keep track of erased blocks in bitmap
 *
 * \param[in] blk_index index of block to erase (offset divided by 2^12)
 * \param[in] eraseOpt erase option to erase 4k, 32k or 64k at a time
 * \return    kStatus_HAL_Flash_Success if successful, other values in case of error
 *
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_EraseBlockBySectorNumber(uint32_t blk_index, eraseOptions_t eraseOpt)
{
    return kStatus_OTA_Flash_Fail;
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
    return kStatus_OTA_Flash_Fail;
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
    ota_flash_status_t status = kStatus_OTA_Flash_Fail;
    status_t           st;
    do
    {
        if (Addr > (gExtFlash_StartOffset_c + PARTITION_SIZE))
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }
        if (0 == NoOfBytes)
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }
        if (NoOfBytes >= (uint32_t)(gExtFlash_TotalSize_c - Addr))
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }
        /* Address should be page aligned */
        if (!IS_PAGE_ALIGNED(Addr))
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_InvalidArgument);
        }

        status = ExternalFlash_PrepareForWrite(NoOfBytes, Addr);
        if (kStatus_OTA_Flash_Success != status)
        {
            break;
        }

        /* Write data to FLASH */
        if (NoOfBytes == gExtFlash_PageSectorSize_c)
        {
            /* Get the physical address before writing data to flash */
            uint32_t phys_addr = mflash_drv_log2phys((void *)Addr, NoOfBytes);
            st                 = mflash_drv_page_program(phys_addr, (void *)Outbuf);
        }
        if (kStatus_Success == st)
            status = kStatus_OTA_Flash_Success;
    } while (0);
    return status;
}

/*! *********************************************************************************
 * \brief  Writes remainder of 16 byte buffer to flash when terminating FW update
 *
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_FlushWriteBuffer(void)
{
    return kStatus_OTA_Flash_Fail;
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
    uint32_t           phys_addr = mflash_drv_log2phys((void *)Addr, NoOfBytes);
    st                           = mflash_drv_read(phys_addr, (void *)inbuf, NoOfBytes);
    if (st == kStatus_Success)
    {
        status = kStatus_OTA_Flash_Success;
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
    /* Should send JEDEC command 05h : if returned value has bit (0) set the flash is in busy mode.
      Existing API allows to detect Busy situation by issuing a command and comparing answer to kStatus_Busy
    */
    uint8_t  dummy;
    status_t st;
    st = mflash_drv_read(0, (void *)&dummy, 1);
    return (kStatus_Busy == st) ? 1U : 0U;
}

/*! *********************************************************************************
 * \brief  Erase number of sectors required for an upcomingprogram operation
 *
 * \param[in] NoOfBytes   Number of bytes of area to be cleared
 *            will be rounded to a multiple of flash sectors
 * \param[in] Addr        offset address relative to start of External Storage from which
 *            erase operation is required
 * \return    kStatus_OTA_Flash_Success if successful, other values in case of error
 ***********************************************************************************/
static ota_flash_status_t ExternalFlash_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr)
{
    ota_flash_status_t status = kStatus_OTA_Flash_Success;

    uint32_t i;
    uint32_t startBlk, endBlk;
    /* Obtain the first and last block that need to be erased */
    uint32_t end = (Addr + NoOfBytes);
    if (end & (gExtFlash_SectorSize_c - 1))
        end = ((end / gExtFlash_SectorSize_c) + 1) * gExtFlash_SectorSize_c;

    /* Obtain the first and last block that need to be erased */
    startBlk = Addr / gExtFlash_SectorSize_c;

    endBlk = end / gExtFlash_SectorSize_c;

    for (i = startBlk; i < endBlk;)
    {
        /* get index for the bitmap as i is expected in access-address format */
        uint32_t index = i - (gExtFlash_StartOffset_c / gExtFlash_SectorSize_c);
        if (GET_BIT(mEepromEraseBitmap, index))
        {
            /* already erased so skip */
            i++;
        }
        else
        {
            /* Get the physical address of flash area to erase */
            uint32_t phys_addr = mflash_drv_log2phys((void *)(i * gExtFlash_SectorSize_c), gExtFlash_SectorSize_c);
            status             = mflash_drv_sector_erase(phys_addr);
            SET_BIT(mEepromEraseBitmap, index);
            if (kStatus_OTA_Flash_Success != status)
            {
                break;
            }
            i += gExtFlash_SectorSize_c;
        }
    }
    return status;
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
ota_flash_status_t ExternalFlash_EraseArea(uint32_t *pAddr, int32_t *pSize, bool non_blocking)
{
    ota_flash_status_t status;
    int32_t            remain_sz  = (int32_t)*pSize;
    uint32_t           erase_addr = *pAddr;

    // NOT_USED(non_blocking);

    do
    {
        if (!IS_SECTOR_ALIGNED(erase_addr))
        {
            RAISE_ERROR(status, kStatus_OTA_Flash_AlignmentError);
        }
        /* Erase address shouldn't go past the end of the flash partition */
        if (erase_addr < gExtFlash_EndPartition_c)
        {
            if (remain_sz != 0)
            {
                status = ExternalFlash_PrepareForWrite(remain_sz, erase_addr);
                if (status != kStatus_OTA_Flash_Success)
                {
                    break;
                }
                /* we know beforehand that we erase by sector */
                if (remain_sz < gExtFlash_SectorSize_c)
                {
                    erase_addr += gExtFlash_SectorSize_c;
                }
                else
                {
                    erase_addr += remain_sz;
                }
                remain_sz = 0;
            }
        }
        status = kStatus_OTA_Flash_Success;
    } while (0);

    *pAddr = erase_addr;
    *pSize = remain_sz;

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
    ota_flash_status_t status = kStatus_OTA_Flash_Success;

    uint32_t phys_addr = mflash_drv_log2phys((void *)Addr, length);

    if (!FLib_MemCmp(pData, (void const *)phys_addr, length))
    {
        status = kStatus_OTA_Flash_Fail;
    }
    return status;
}
