/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 nix.long
 *
 */

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/misc.h"
#include "mp_defs.h"
#include "gr_porting.h"

#if MICROPY_PY_XBLEPY

#include "modxblepy.h"

STATIC void xblepy_device_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_device_obj_t * self = (xblepy_device_obj_t *)o;

    mp_printf(print, "Device(conn id:%d,role:%d,addr:[%02x:%02x:%02x:%02x:%02x:%02x])", 
                                            self->conn_id, self->role, 
                                            self->addr.addr[0],self->addr.addr[1],self->addr.addr[2],
                                            self->addr.addr[3],self->addr.addr[4],self->addr.addr[5]
                );
}

/*********************************************************************
 * args:
 *      none
 * kw_args:
 *      role = [Peripheral | Central | Unknown (Default)]
 *      addr_type = [PUBLIC(Default) | RANDOM_STATIC]
 *      addr = [bytearray[6]]
 *********************************************************************/
mp_obj_t xblepy_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    gr_trace("+++ new Device() \r\n");

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_role,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = XBLEPY_ROLE_UNKNOWN} },
        { MP_QSTR_addr_type, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = XBLEPY_ADDR_TYPE_PUBLIC} },
        { MP_QSTR_addr,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    xblepy_role_type_t role         = args[0].u_int;
    xblepy_addr_type_t addr_type    = args[1].u_int;
    mp_obj_t addr_obj               = args[2].u_obj;
    uint8_t  addr_buf[XBLEPY_BD_ADDR_LEN]   = XBLEPY_BD_ADDR_DEFAULT;
    
    if (addr_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(addr_obj, &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len != XBLEPY_BD_ADDR_LEN) {
            mp_raise_ValueError("length of ble addr must be 6!");
        } else {            
            memcpy(&addr_buf, bufinfo.buf, XBLEPY_BD_ADDR_LEN);
        }
    }
    
    xblepy_device_obj_t * s = m_new_obj(xblepy_device_obj_t);
    
    memcpy(&s->addr.addr[0], &addr_buf[0], XBLEPY_BD_ADDR_LEN);
    s->base.type        = type;
    s->role             = role;
    s->addr_type        = addr_type;
    s->conn_id          = 0xFFFF;    
    s->delegate         = mp_const_none;
    s->service_list     = mp_obj_new_list(0, NULL);
    s->handler_list     = mp_obj_new_list(0, NULL);
    
    s->conn_handler     = mp_const_none;
    
    return MP_OBJ_FROM_PTR(s);
}


/// \method whoami()
/// print who am i
mp_obj_t xblepy_device_whoami(mp_obj_t self_in) {
    gr_trace( "+++ Device : who am i\r\n");
    
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(xblepy_device_whoami_obj, xblepy_device_whoami);

STATIC const mp_rom_map_elem_t xblepy_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_whoami),  MP_ROM_PTR(&xblepy_device_whoami_obj) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_device_locals_dict, xblepy_device_locals_dict_table);

const mp_obj_type_t xblepy_device_type = {
    { &mp_type_type },
    .name = MP_QSTR_Device,
    .print = xblepy_device_print,
    .make_new = xblepy_device_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_device_locals_dict
};

#endif // MICROPY_PY_XBLEPY
