/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#ifndef __MOD_XFLASH_H__
#define __MOD_XFLASH_H__

#include "extmod/vfs_fat.h"
#include "spi_flash.h"

/*
 * sector size is 4096, use a sector as one block
 */
#define XFLASH_BLOCK_SIZE               SPI_FLASH_SECTOR_SIZE
#define XFLASH_PAGE_SIZE                SPI_FLASH_PAGE_SIZE
#define XFLASH_START_ADDRESS            0x00000000
#define XFLASH_SIZE                     0x00400000              //4MBytes

#define XFLASH_PAGE_COUNT_PER_BLOCK     (XFLASH_BLOCK_SIZE/XFLASH_PAGE_SIZE)
#define XFLASH_BLOCK_COUNT              (XFLASH_SIZE/XFLASH_BLOCK_SIZE)

extern const mp_obj_type_t grb_flash_type;

void xflash_init(void);

void xflash_init_vfs(fs_user_mount_t *vfs);

#endif /*__MOD_XFLASH_H__*/
