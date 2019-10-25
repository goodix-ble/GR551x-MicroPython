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

#if MICROPY_PY_XBLEPY_CENTRAL

#include "ble_drv.h"
#include "mphalport.h"

STATIC void adv_event_handler(mp_obj_t self_in, uint16_t event_id, ble_drv_adv_data_t * data) {
    xblepy_scanner_obj_t *self = MP_OBJ_TO_PTR(self_in);

    xblepy_scan_entry_obj_t * item = m_new_obj(xblepy_scan_entry_obj_t);
    item->base.type = &xblepy_scan_entry_type;

    vstr_t vstr;
    vstr_init(&vstr, 17);

    vstr_printf(&vstr, ""HEX2_FMT":"HEX2_FMT":"HEX2_FMT":" \
                         HEX2_FMT":"HEX2_FMT":"HEX2_FMT"",
                data->p_peer_addr[5], data->p_peer_addr[4], data->p_peer_addr[3],
                data->p_peer_addr[2], data->p_peer_addr[1], data->p_peer_addr[0]);

    item->addr = mp_obj_new_str(vstr.buf, vstr.len);

    vstr_clear(&vstr);

    item->addr_type = data->addr_type;
    item->rssi      = data->rssi;
    item->data      = mp_obj_new_bytearray(data->data_len, data->p_data);

    mp_obj_list_append(self->adv_reports, item);

    // Continue scanning
    ble_drv_scan_start(true);
}

STATIC void xblepy_scanner_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_scanner_obj_t * self = (xblepy_scanner_obj_t *)o;
    (void)self;
    mp_printf(print, "Scanner");
}

STATIC mp_obj_t xblepy_scanner_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {

    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    xblepy_scanner_obj_t * s = m_new_obj(xblepy_scanner_obj_t);
    s->base.type = type;

    return MP_OBJ_FROM_PTR(s);
}

/// \method scan(timeout)
/// Scan for devices. Timeout is in milliseconds and will set the duration
/// of the scanning.
///
STATIC mp_obj_t scanner_scan(mp_obj_t self_in, mp_obj_t timeout_in) {
    xblepy_scanner_obj_t * self = MP_OBJ_TO_PTR(self_in);
    mp_int_t timeout = mp_obj_get_int(timeout_in);

    self->adv_reports = mp_obj_new_list(0, NULL);

    ble_drv_adv_report_handler_set(MP_OBJ_FROM_PTR(self), adv_event_handler);

    // start
    ble_drv_scan_start(false);

    // sleep
    mp_hal_delay_ms(timeout);

    // stop
    ble_drv_scan_stop();

    return self->adv_reports;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_scanner_scan_obj, scanner_scan);

STATIC const mp_rom_map_elem_t xblepy_scanner_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&xblepy_scanner_scan_obj) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_scanner_locals_dict, xblepy_scanner_locals_dict_table);


const mp_obj_type_t xblepy_scanner_type = {
    { &mp_type_type },
    .name = MP_QSTR_Scanner,
    .print = xblepy_scanner_print,
    .make_new = xblepy_scanner_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_scanner_locals_dict
};

#endif // MICROPY_PY_XBLEPY_CENTRAL
