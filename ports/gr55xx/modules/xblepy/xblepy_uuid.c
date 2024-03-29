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
#include "string.h"
#include "assert.h"
#include "gr_porting.h"

#if MICROPY_PY_XBLEPY

#include "modxblepy.h"

STATIC void xblepy_uuid_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_uuid_obj_t * self = (xblepy_uuid_obj_t *)o;
    if (self->type == XBLEPY_UUID_16_BIT) {
        mp_printf(print, "UUID(uuid: 0x" HEX2_FMT HEX2_FMT ")",
                  self->value[1], self->value[0]);
    } else {
        mp_printf(print, "UUID(uuid: %s)",gr_ble_format_uuid128b_to_string(&self->value_128b[0], 16));
    }
}

STATIC mp_obj_t xblepy_uuid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_NEW_UUID };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_UUID, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    xblepy_uuid_obj_t *s = m_new_obj(xblepy_uuid_obj_t);
    s->base.type = type;

    mp_obj_t uuid_obj = args[ARG_NEW_UUID].u_obj;

    if (uuid_obj == MP_OBJ_NULL) {
        return MP_OBJ_FROM_PTR(s);
    }

    if (mp_obj_is_int(uuid_obj)) {
        s->type = XBLEPY_UUID_16_BIT;
        s->value[1] = (((uint16_t)mp_obj_get_int(uuid_obj)) >> 8) & 0xFF;
        s->value[0] = ((uint8_t)mp_obj_get_int(uuid_obj)) & 0xFF;
    } else if (mp_obj_is_str(uuid_obj)) {
        GET_STR_DATA_LEN(uuid_obj, str_data, str_len);
        if (str_len == 6) { // Assume hex digit prefixed with 0x
            s->type = XBLEPY_UUID_16_BIT;
            s->value[0]  = unichar_xdigit_value(str_data[5]);
            s->value[0] += unichar_xdigit_value(str_data[4]) << 4;
            s->value[1]  = unichar_xdigit_value(str_data[3]);
            s->value[1] += unichar_xdigit_value(str_data[2]) << 4;
        } else if (str_len == 36) {
            s->type = XBLEPY_UUID_128_BIT;
            uint8_t buffer[16];
            buffer[0]  = unichar_xdigit_value(str_data[35]);
            buffer[0] += unichar_xdigit_value(str_data[34]) << 4;
            buffer[1]  = unichar_xdigit_value(str_data[33]);
            buffer[1] += unichar_xdigit_value(str_data[32]) << 4;
            buffer[2]  = unichar_xdigit_value(str_data[31]);
            buffer[2] += unichar_xdigit_value(str_data[30]) << 4;
            buffer[3]  = unichar_xdigit_value(str_data[29]);
            buffer[3] += unichar_xdigit_value(str_data[28]) << 4;
            buffer[4]  = unichar_xdigit_value(str_data[27]);
            buffer[4] += unichar_xdigit_value(str_data[26]) << 4;
            buffer[5]  = unichar_xdigit_value(str_data[25]);
            buffer[5] += unichar_xdigit_value(str_data[24]) << 4;
            // 23 '-'
            buffer[6]  = unichar_xdigit_value(str_data[22]);
            buffer[6] += unichar_xdigit_value(str_data[21]) << 4;
            buffer[7]  = unichar_xdigit_value(str_data[20]);
            buffer[7] += unichar_xdigit_value(str_data[19]) << 4;
            // 18 '-'
            buffer[8]  = unichar_xdigit_value(str_data[17]);
            buffer[8] += unichar_xdigit_value(str_data[16]) << 4;
            buffer[9]  = unichar_xdigit_value(str_data[15]);
            buffer[9] += unichar_xdigit_value(str_data[14]) << 4;
            // 13 '-'
            buffer[10]  = unichar_xdigit_value(str_data[12]);
            buffer[10] += unichar_xdigit_value(str_data[11]) << 4;
            buffer[11]  = unichar_xdigit_value(str_data[10]);
            buffer[11] += unichar_xdigit_value(str_data[9]) << 4;
            // 8 '-'
            // 16-bit field
            s->value[0]  = unichar_xdigit_value(str_data[7]);
            s->value[0] += unichar_xdigit_value(str_data[6]) << 4;
            s->value[1]  = unichar_xdigit_value(str_data[5]);
            s->value[1] += unichar_xdigit_value(str_data[4]) << 4;
            
            buffer[12] = s->value[0];
            buffer[13] = s->value[1];

            buffer[14]  = unichar_xdigit_value(str_data[3]);
            buffer[14] += unichar_xdigit_value(str_data[2]) << 4;
            buffer[15]  = unichar_xdigit_value(str_data[1]);
            buffer[15] += unichar_xdigit_value(str_data[0]) << 4;

            memcpy(&s->value_128b[0], &buffer[0], 16);
            //ble_drv_uuid_add_vs(buffer, &s->uuid_vs_idx);
        } else {
            mp_raise_ValueError("Invalid UUID string length");
        }
    } else if (mp_obj_is_type(uuid_obj, &xblepy_uuid_type)) {
        // deep copy instance
        xblepy_uuid_obj_t * p_old = MP_OBJ_TO_PTR(uuid_obj);
        s->type     = p_old->type;
        s->value[0] = p_old->value[0];
        s->value[1] = p_old->value[1];
    } else {
        mp_raise_ValueError("Invalid UUID parameter");
    }

    return MP_OBJ_FROM_PTR(s);
}

/// \method binVal()
/// Get binary value of the 16 or 128 bit UUID. Returned as bytearray type.
///
STATIC mp_obj_t uuid_bin_val(mp_obj_t self_in) {
    xblepy_uuid_obj_t * self = MP_OBJ_TO_PTR(self_in);

    // TODO: Extend the uint16 byte value to 16 byte if 128-bit,
    //       also encapsulate it in a bytearray. For now, return
    //       the uint16_t field of the UUID.
    if (self->type == XBLEPY_UUID_128_BIT) {
        mp_obj_t uuid_str = mp_obj_new_str(gr_ble_format_uuid128b_to_string(&self->value_128b[0], 16), 36);        
        return uuid_str;//MP_OBJ_NEW_SMALL_INT(self->value_128b[0] | self->value_128b[1] << 8);
    } else {        
        return MP_OBJ_NEW_SMALL_INT(self->value[0] | self->value[1] << 8);
    }
    
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_uuid_bin_val_obj, uuid_bin_val);

STATIC const mp_rom_map_elem_t xblepy_uuid_locals_dict_table[] = {
#if 0
    { MP_ROM_QSTR(MP_QSTR_getCommonName), MP_ROM_PTR(&xblepy_uuid_get_common_name_obj) },
#endif
    // Properties
    { MP_ROM_QSTR(MP_QSTR_binVal), MP_ROM_PTR(&xblepy_uuid_bin_val_obj) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_uuid_locals_dict, xblepy_uuid_locals_dict_table);

const mp_obj_type_t xblepy_uuid_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUID,
    .print = xblepy_uuid_print,
    .make_new = xblepy_uuid_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_uuid_locals_dict
};

#endif // MICROPY_PY_XBLEPY
