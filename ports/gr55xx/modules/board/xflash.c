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


#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "extmod/vfs_fat.h"
#include "mp_defs.h"

#include "xflash.h"
#include "spi_flash.h"

#if MICROPY_PY_MACHINE_XFLASH > 0u

static bool b_xflash_is_initialised = false;

void xflash_init(void) {
    if(!b_xflash_is_initialised) {
        
        if (!SPI_FLASH_init()){
            //init fail
            return;
        }
        
        b_xflash_is_initialised = TRUE;
    }
    
    //uint32_t id = SPI_FLASH_Read_Device_ID();
    //printf("xflash id: %d \r\n");
}

uint32_t xflash_get_block_size(void) {
    return XFLASH_BLOCK_SIZE;
}

uint32_t xflash_get_block_count(void) {
    return XFLASH_SIZE/XFLASH_BLOCK_SIZE;
}

void xflash_flush(void) {
    //nothing to do
}

/*
 * dest : buffer ptr to save read data
 * block_num : the start number of block to read ,scope :[0 ~ 1023]
 */
bool xflash_read_block(uint8_t *dest, uint32_t block_num) {
    uint32_t read_addr = XFLASH_START_ADDRESS + block_num * XFLASH_BLOCK_SIZE;
    uint32_t i = 0;
    
    if(!b_xflash_is_initialised){
        return FALSE;
    }
    
    for(i = 0; i < XFLASH_PAGE_COUNT_PER_BLOCK; i++) {
        SPI_FLASH_Read(read_addr + i*XFLASH_PAGE_SIZE, (uint8_t *)dest + i*XFLASH_PAGE_SIZE, XFLASH_PAGE_SIZE);
    }
    
    return TRUE;
}

/*
 * dest : buffer ptr to save read data
 * block_num : the start number of block to read ,scope :[0 ~ 1023]
 * num_blocks : how many blocks to read
 */
mp_uint_t xflash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    uint32_t i = 0;
    mp_uint_t ret = 0;
    
    SPI_FLASH_Disable_Quad();
    for (i = 0; i < num_blocks; i++) {
        if (!xflash_read_block(dest + i * XFLASH_BLOCK_SIZE, block_num + i)) {
            ret = 1; // error
            break;
        }
    }
    //SPI_FLASH_Disable_Quad();
    
    return ret; // success
}

/*
 * src : buffer ptr to program data
 * block_num : the start number of block to write ,scope :[0 ~ 1023]
 */
bool xflash_write_block(const uint8_t *src, uint32_t block_num) {
    uint32_t write_addr = XFLASH_START_ADDRESS + block_num * XFLASH_BLOCK_SIZE;
    
    if(!b_xflash_is_initialised){
        return FALSE;
    }
    
    uint32_t i = 0;
    
    //erase first
    SPI_FLASH_Sector_Erase(write_addr);
    
    for(i = 0; i < XFLASH_PAGE_COUNT_PER_BLOCK; i++) {
        SPI_FLASH_Page_Program(write_addr + i*XFLASH_PAGE_SIZE, (uint8_t *)src + i*XFLASH_PAGE_SIZE);
    }
    
    return TRUE;
}

/*
 * src : buffer ptr to program data
 * block_num : the start number of block to write ,scope :[0 ~ 1023]
 * num_blocks : how many blocks to write
 */
mp_uint_t xflash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {

    for (size_t i = 0; i < num_blocks; i++) {
        if (!xflash_write_block(src + i * XFLASH_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}


/******************************************************************************/
// MicroPython bindings
//
// Expose the flash as an object with the block protocol.
// grb_ means goodix board
// there is a singleton Flash object
STATIC const mp_obj_base_t grb_flash_obj = {&grb_flash_type};

STATIC mp_obj_t grb_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return MP_OBJ_FROM_PTR(&grb_flash_obj);
}

STATIC mp_obj_t grb_flash_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = xflash_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / XFLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(grb_flash_readblocks_obj, grb_flash_readblocks);

STATIC mp_obj_t grb_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = xflash_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / XFLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(grb_flash_writeblocks_obj, grb_flash_writeblocks);

STATIC mp_obj_t grb_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: 
            xflash_init();
            if(b_xflash_is_initialised){
                return MP_OBJ_NEW_SMALL_INT(0);
            }
            return MP_OBJ_NEW_SMALL_INT(1);
        
        case BP_IOCTL_DEINIT: 
            xflash_flush(); 
            return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        
        case BP_IOCTL_SYNC: 
            xflash_flush(); 
            return MP_OBJ_NEW_SMALL_INT(0);
        
        case BP_IOCTL_SEC_COUNT: 
            return MP_OBJ_NEW_SMALL_INT(xflash_get_block_count());
            
        case BP_IOCTL_SEC_SIZE: 
            return MP_OBJ_NEW_SMALL_INT(xflash_get_block_size());
            
        default: 
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(grb_flash_ioctl_obj, grb_flash_ioctl);

STATIC const mp_rom_map_elem_t grb_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks),      MP_ROM_PTR(&grb_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks),     MP_ROM_PTR(&grb_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),           MP_ROM_PTR(&grb_flash_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(grb_flash_locals_dict, grb_flash_locals_dict_table);

const mp_obj_type_t grb_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = grb_flash_make_new,
    .locals_dict = (mp_obj_dict_t*)&grb_flash_locals_dict,
};

void xflash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    //vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->readblocks[0] = MP_OBJ_FROM_PTR(&grb_flash_readblocks_obj);
    vfs->readblocks[1] = MP_OBJ_FROM_PTR(&grb_flash_obj);
    vfs->readblocks[2] = MP_OBJ_FROM_PTR(xflash_read_blocks); // native version
    vfs->writeblocks[0] = MP_OBJ_FROM_PTR(&grb_flash_writeblocks_obj);
    vfs->writeblocks[1] = MP_OBJ_FROM_PTR(&grb_flash_obj);
    vfs->writeblocks[2] = MP_OBJ_FROM_PTR(xflash_write_blocks); // native version
    vfs->u.ioctl[0] = MP_OBJ_FROM_PTR(&grb_flash_ioctl_obj);
    vfs->u.ioctl[1] = MP_OBJ_FROM_PTR(&grb_flash_obj);
}

#endif /*MICROPY_PY_MACHINE_XFLASH*/
