/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fwk_platform_ota.h"
#include "FunctionLib.h"

extern uint32_t OTA_STORAGE_SIZE[];
#define PARTITION_SIZE ((uint32_t)OTA_STORAGE_SIZE)

/* TODO : Define Platform Ota bootloader functions */

int PLATFORM_OtaUpdateBootFlags(const OtaLoaderInfo_t *ota_load_info)
{
    return 0;
}

int PLATFORM_OtaBootDataUpdateOnCommit(const OtaLoaderInfo_t *ota_loader_info)
{
    return 0;
}

int PLATFORM_OtaClearBootFlags(void)
{
    return 0;
}

uint32_t PLATFORM_OtaGetImageOffset(bool internal_storage, uint32_t partition_offset)
{
    // NOT_USED(internal_storage);

    return partition_offset;
}

uint32_t PLATFORM_OtaGetMaxImageSize(bool internal_storage)
{
    return PARTITION_SIZE;
}

void PLATFORM_InitSPIFlash(void)
{
}

//#endif /*(!gEnableOTAServer_d || (gEnableOTAServer_d && gUpgradeImageOnCurrentDevice_d)) */
