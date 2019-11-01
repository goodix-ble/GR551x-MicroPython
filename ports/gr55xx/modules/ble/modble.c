/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <stdio.h>
#include <stdint.h>
#include "py/runtime.h"
#include "py/misc.h"
#include "mp_defs.h"
#include "gr_porting.h"

#if MICROPY_PY_BLE > 0u

/// \method enable()
/// Enable BLE softdevice.
mp_obj_t ble_obj_enable(void) {
    
    if(!s_gr_ble_common_params_ins.is_ble_initialized){
        gr_ble_stack_init();
        gr_xblepy_init();
        gr_xblepy_gap_hal_init();
        gr_trace("enable the ble stack...\r\n");
    } else {
        gr_trace("ble stack enabled already...\r\n");
    }
    
    return mp_const_none;
}

/// \method disable()
/// Disable BLE softdevice.
mp_obj_t ble_obj_disable(void) {
    if(s_gr_ble_common_params_ins.is_ble_initialized) {
        gr_trace("not support disable ble stack ...\r\n");
    } else {
        gr_trace("ble stack not enabled...\r\n");
    }
    
    return mp_const_none;
}

/// \method enabled()
/// Get state of whether the softdevice is enabled or not.
mp_obj_t ble_obj_enabled(void) {
    uint8_t is_enabled = (uint8_t)s_gr_ble_common_params_ins.is_ble_initialized;
    mp_int_t enabled = is_enabled;
    return MP_OBJ_NEW_SMALL_INT(enabled);
}

/// \method startadv()
/// test function, start the adv test
mp_obj_t ble_obj_start_adv(void) {
    if(s_gr_ble_common_params_ins.is_ble_initialized) {
        gr_xblepy_gap_start_adv_test();
    }

    return mp_const_none;
}


/// \method stopadv()
/// test function, stop the adv test
mp_obj_t ble_obj_stop_adv(void) {
    uint32_t x_err = 0;
    if(s_gr_ble_gap_params_ins.is_adv_started) {        
        x_err = ble_gap_adv_stop(0);
        gr_trace( "+++ ble_obj_stop_adv  ret: %d  ",  x_err);
    }
    
    return mp_const_none;
}

/// \method address()
/// Return device address as text string.
mp_obj_t ble_obj_address(void) {
    gap_bdaddr_t    local_bd_addr;
    
    ble_gap_addr_get(&local_bd_addr);

    vstr_t vstr;
    vstr_init(&vstr, 17);

    vstr_printf(&vstr, ""HEX2_FMT":"HEX2_FMT":"HEX2_FMT":" \
                         HEX2_FMT":"HEX2_FMT":"HEX2_FMT"",
                local_bd_addr.gap_addr.addr[5], local_bd_addr.gap_addr.addr[4], local_bd_addr.gap_addr.addr[3],
                local_bd_addr.gap_addr.addr[2], local_bd_addr.gap_addr.addr[1], local_bd_addr.gap_addr.addr[0]);

    mp_obj_t mac_str = mp_obj_new_str(vstr.buf, vstr.len);

    vstr_clear(&vstr);

    return mac_str;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_enable_obj, ble_obj_enable);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_disable_obj, ble_obj_disable);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_enabled_obj, ble_obj_enabled);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_address_obj, ble_obj_address);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_start_adv_obj, ble_obj_start_adv);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_stop_adv_obj, ble_obj_stop_adv);

STATIC const mp_rom_map_elem_t ble_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ble) },
    { MP_ROM_QSTR(MP_QSTR_enable),   MP_ROM_PTR(&ble_obj_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable),  MP_ROM_PTR(&ble_obj_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enabled),  MP_ROM_PTR(&ble_obj_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_address),  MP_ROM_PTR(&ble_obj_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_startadv),  MP_ROM_PTR(&ble_obj_start_adv_obj) },
    { MP_ROM_QSTR(MP_QSTR_stopadv),  MP_ROM_PTR(&ble_obj_stop_adv_obj) },
};


STATIC MP_DEFINE_CONST_DICT(ble_module_globals, ble_module_globals_table);

const mp_obj_module_t mp_module_ble = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ble_module_globals,
};

#endif // MICROPY_PY_BLE
