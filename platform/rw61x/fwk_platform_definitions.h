/*
 * Copyright 2022-2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FWK_PLAT_DEFS_H_
#define _FWK_PLAT_DEFS_H_

#include "mflash_drv.h" /* TODO remove this dependency */

#define PLATFORM_EXTFLASH_SECTOR_SIZE MFLASH_SECTOR_SIZE
#define PLATFORM_EXTFLASH_PAGE_SIZE   MFLASH_PAGE_SIZE
#define PLATFORM_EXTFLASH_TOTAL_SIZE  FLASH_SIZE /* SPI NOR Flash is an MX25U51245G (512Mb) */

#endif /* _FWK_PLAT_DEFS_H_ */
