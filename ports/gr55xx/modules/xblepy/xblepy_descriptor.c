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

STATIC void xblepy_descriptor_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_descriptor_obj_t * self = (xblepy_descriptor_obj_t *)o;

    if(self->p_uuid == NULL) {
        mp_printf(print, "Descriptor(uuid: none)");
    } else {
        if(self->p_uuid->type == XBLEPY_UUID_128_BIT) {
            mp_printf(print, "Descriptor(uuid: %s)",gr_ble_format_uuid128b_to_string(&self->p_uuid->value_128b[0], 16));
        } else {
            mp_printf(print, "Descriptor(uuid: 0x" HEX2_FMT HEX2_FMT ")",
              self->p_uuid->value[1], self->p_uuid->value[0]);
        }
    }
    
}

STATIC mp_obj_t xblepy_descriptor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_NEW_UUID };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_perms, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = XBLEPY_PERM_READ_FREE | XBLEPY_PERM_WRITE_FREE} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    xblepy_descriptor_obj_t * s = m_new_obj(xblepy_descriptor_obj_t);
    s->base.type = type;

    mp_obj_t uuid_obj = args[0].u_obj;

    if (uuid_obj == mp_const_none) {
        mp_raise_ValueError("must provide UUID parameter");
    }
    
    if (mp_obj_is_type(uuid_obj, &xblepy_uuid_type)) {
        s->p_uuid = MP_OBJ_TO_PTR(uuid_obj);
    } else {
        mp_raise_ValueError("Invalid UUID parameter");
    }

    if (args[1].u_int > 0) {
        s->perms = (uint8_t)args[1].u_int;
    }

    s->handle                   = XBLEPY_UNASSIGNED_HANDLE;
    s->service_handle           = XBLEPY_UNASSIGNED_HANDLE;
    s->characteristic_handle    = XBLEPY_UNASSIGNED_HANDLE;

    return MP_OBJ_FROM_PTR(s);
}

STATIC const mp_rom_map_elem_t xblepy_descriptor_locals_dict_table[] = {
#if 0
    { MP_ROM_QSTR(MP_QSTR_binVal), MP_ROM_PTR(&xblepy_descriptor_bin_val_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(xblepy_descriptor_locals_dict, xblepy_descriptor_locals_dict_table);

const mp_obj_type_t xblepy_descriptor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Descriptor,
    .print = xblepy_descriptor_print,
    .make_new = xblepy_descriptor_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_descriptor_locals_dict
};

#endif // MICROPY_PY_XBLEPY
