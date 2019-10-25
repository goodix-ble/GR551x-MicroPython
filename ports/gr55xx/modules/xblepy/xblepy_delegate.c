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
#include "mp_defs.h"

#if MICROPY_PY_XBLEPY_PERIPHERAL || MICROPY_PY_XBLEPY_CENTRAL

#include "modxblepy.h"

STATIC void xblepy_delegate_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_delegate_obj_t * self = (xblepy_delegate_obj_t *)o;
    (void)self;
    mp_printf(print, "DefaultDelegate()");
}

STATIC mp_obj_t xblepy_delegate_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    xblepy_delegate_obj_t *s = m_new_obj(xblepy_delegate_obj_t);
    s->base.type = type;

    return MP_OBJ_FROM_PTR(s);
}

/// \method handleConnection()
/// Handle connection events.
///
STATIC mp_obj_t delegate_handle_conn(mp_obj_t self_in) {
    xblepy_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_delegate_handle_conn_obj, delegate_handle_conn);

/// \method handleNotification()
/// Handle notification events.
///
STATIC mp_obj_t delegate_handle_notif(mp_obj_t self_in) {
    xblepy_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_delegate_handle_notif_obj, delegate_handle_notif);

STATIC const mp_rom_map_elem_t xblepy_delegate_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_handleConnection),   MP_ROM_PTR(&xblepy_delegate_handle_conn_obj) },
    { MP_ROM_QSTR(MP_QSTR_handleNotification), MP_ROM_PTR(&xblepy_delegate_handle_notif_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_handleDiscovery),    MP_ROM_PTR(&xblepy_delegate_handle_disc_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(xblepy_delegate_locals_dict, xblepy_delegate_locals_dict_table);

const mp_obj_type_t xblepy_delegate_type = {
    { &mp_type_type },
    .name = MP_QSTR_DefaultDelegate,
    .print = xblepy_delegate_print,
    .make_new = xblepy_delegate_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_delegate_locals_dict
};

#endif // MICROPY_PY_XBLEPY_PERIPHERAL || MICROPY_PY_XBLEPY_CENTRAL
