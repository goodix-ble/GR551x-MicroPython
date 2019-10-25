/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 nix.long
 *
 */

#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/objlist.h"
#include "py/mperrno.h"

#include "mp_defs.h"
#include "xblepy_hal.h"

#if defined(MICROPY_PY_XBLEPY) // && defined(MICROPY_PY_XBLEPY_CENTRAL)

STATIC void xblepy_central_check_ble_stack_status(void) {
    if(!s_gr_ble_common_params_ins.is_ble_initialized) {
        mp_raise_msg(&mp_type_Exception, "ble stack not enable, call ble.enable() first!");
    }
    return;
}

STATIC void xblepy_central_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_central_obj_t * self = (xblepy_central_obj_t *)o;
    
    mp_printf(print, "Central(conn id:%d,role:%d,addr:[%02x:%02x:%02x:%02x:%02x:%02x])", 
                                            self->conn_id, self->role, 
                                            self->addr.addr[0],self->addr.addr[1],self->addr.addr[2],
                                            self->addr.addr[3],self->addr.addr[4],self->addr.addr[5]
                );
}



/*
 * declare super class' methods, and register them in this class
 */
MP_DECLARE_CONST_FUN_OBJ_1(xblepy_device_whoami_obj);

STATIC const mp_rom_map_elem_t xblepy_central_locals_dict_table[] = {
    /* super class method start */
    { MP_ROM_QSTR(MP_QSTR_whoami),  MP_ROM_PTR(&xblepy_device_whoami_obj) },
    /* super class method end */
    
    
};


STATIC MP_DEFINE_CONST_DICT(xblepy_central_locals_dict, xblepy_central_locals_dict_table);

extern mp_obj_t xblepy_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

const mp_obj_type_t xblepy_central_type = {
    { &mp_type_type },
    .name = MP_QSTR_Central,
    .print = xblepy_central_print,
    .make_new = xblepy_device_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_central_locals_dict,
    .parent = &xblepy_device_type
};

#endif /* defined(MICROPY_PY_XBLEPY) && defined(MICROPY_PY_XBLEPY_CENTRAL) */
