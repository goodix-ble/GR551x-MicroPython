/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "py/mperrno.h"

#include "mp_defs.h"
#include "ubluepy_hal.h"

#if MICROPY_PY_UBLUEPY && ( MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL )

#include "modubluepy.h"

STATIC void ubluepy_service_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_service_obj_t * self = (ubluepy_service_obj_t *)o;
    
    if(self->p_uuid == NULL) {
        mp_printf(print, "Service(uuid: none, handle: 0x%02x)", self->handle);
    } else {
        if(self->p_uuid->type == UBLUEPY_UUID_128_BIT) {
            mp_printf(print, "Service(uuid: %s, handle: %d)",gr_ble_format_uuid128b_to_string(&self->p_uuid->value_128b[0], 16), self->handle);
        } else {
            mp_printf(print, "Service(uuid: 0x%02x%02x, handle: %d)",
              self->p_uuid->value[1], self->p_uuid->value[0], self->handle);
        }
    }
}

STATIC mp_obj_t ubluepy_service_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_NEW_UUID, ARG_NEW_TYPE };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_UUID, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { ARG_NEW_TYPE, MP_ARG_INT, {.u_int = UBLUEPY_SERVICE_PRIMARY} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_service_obj_t *s = m_new_obj(ubluepy_service_obj_t);
    s->base.type    = type;
    s->handle       = gr_ble_get_mpy_handle();
    
    mp_obj_t uuid_obj = args[ARG_NEW_UUID].u_obj;

    if (uuid_obj == MP_OBJ_NULL) {
        return MP_OBJ_FROM_PTR(s);
    }

    if (mp_obj_is_type(uuid_obj, &ubluepy_uuid_type)) {
        s->p_uuid = MP_OBJ_TO_PTR(uuid_obj);

        uint8_t type = args[ARG_NEW_TYPE].u_int;
        if (type > 0 &&  type <= UBLUEPY_SERVICE_PRIMARY) {
            s->type = type;
        } else {
            mp_raise_ValueError("Invalid Service type");
        }
/*
        if(!gr_ubluepy_gatt_add_service(s)) {
            mp_raise_OSError(MP_ENOMEM);
        } else {
            //update handle
            s->start_handle = s->handle;
        }
*/
    } else {
        mp_raise_ValueError("Invalid UUID parameter");
    }

    // clear reference to peripheral
    s->p_periph = NULL;
    s->char_list = mp_obj_new_list(0, NULL);

    return MP_OBJ_FROM_PTR(s);
}

/// \method addCharacteristic(Characteristic)
/// Add Characteristic to the Service.
///
STATIC mp_obj_t service_add_characteristic(mp_obj_t self_in, mp_obj_t characteristic) {
    ubluepy_service_obj_t        * self   = MP_OBJ_TO_PTR(self_in);
    ubluepy_characteristic_obj_t * p_char = MP_OBJ_TO_PTR(characteristic);

    p_char->service_handle  = self->handle;
    p_char->p_service       = self;
#if 0
    bool retval = gr_ubluepy_gatt_add_characteristic(p_char);

    if (retval) {
        mp_obj_list_append(self->char_list, characteristic);
    } else {
        p_char->p_service = NULL;
    }
#endif
    mp_obj_list_append(self->char_list, characteristic);

    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_service_add_char_obj, service_add_characteristic);

/// \method getCharacteristics()
/// Return list with all characteristics registered in the Service.
///
STATIC mp_obj_t service_get_chars(mp_obj_t self_in) {
    ubluepy_service_obj_t * self = MP_OBJ_TO_PTR(self_in);

    return self->char_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_service_get_chars_obj, service_get_chars);

/// \method getCharacteristic(UUID)
/// Return Characteristic with the given UUID.
///
STATIC mp_obj_t service_get_characteristic(mp_obj_t self_in, mp_obj_t uuid) {
    ubluepy_service_obj_t * self   = MP_OBJ_TO_PTR(self_in);
    ubluepy_uuid_obj_t    * p_uuid = MP_OBJ_TO_PTR(uuid);

    // validate that there is an UUID object passed in as parameter
    if (!(mp_obj_is_type(uuid, &ubluepy_uuid_type))) {
        mp_raise_ValueError("Invalid UUID parameter");
    }

    mp_obj_t * chars     = NULL;
    mp_uint_t  num_chars = 0;
    mp_obj_get_array(self->char_list, &num_chars, &chars);

    for (uint8_t i = 0; i < num_chars; i++) {
        ubluepy_characteristic_obj_t * p_char = (ubluepy_characteristic_obj_t *)chars[i];

        bool type_match = p_char->p_uuid->type == p_uuid->type;
        bool uuid_match = ((uint16_t)(*(uint16_t *)&p_char->p_uuid->value[0]) ==
                           (uint16_t)(*(uint16_t *)&p_uuid->value[0]));

        if (type_match && uuid_match) {
            return MP_OBJ_FROM_PTR(p_char);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_service_get_char_obj, service_get_characteristic);

/// \method removeCharacteristic(Characteristic)
/// Remove Characteristic from the Service.
///
STATIC mp_obj_t service_remove_characteristic(mp_obj_t self_in, mp_obj_t characteristic) {
    ubluepy_service_obj_t           * self      = MP_OBJ_TO_PTR(self_in);
    ubluepy_characteristic_obj_t    * p_char    = MP_OBJ_TO_PTR(characteristic);

    if(p_char != mp_const_none) {
        p_char->p_service = NULL;
        mp_obj_list_remove(self->char_list, characteristic);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ubluepy_service_remove_characteristic_obj, service_remove_characteristic);


/// \method desc()
/// use to describe the Service Member structure
///
STATIC mp_obj_t service_desc(mp_obj_t self_in) {
    ubluepy_service_obj_t * self = MP_OBJ_TO_PTR(self_in);
    
    if(self->p_uuid == NULL) {
        printf("+++ Service(uuid: none, handle: 0x%02x)\r\n", self->handle);
    } else {
        if(self->p_uuid->type == UBLUEPY_UUID_128_BIT) {
            printf("+++ Service(uuid: %s, handle: %d)\r\n",gr_ble_format_uuid128b_to_string(&self->p_uuid->value_128b[0], 16), self->handle);
        } else {
            printf("+++ Service(uuid: 0x%02x%02x, handle: %d)\r\n",
              self->p_uuid->value[1], self->p_uuid->value[0], self->handle);
        }
    }
        
    
    mp_obj_t * chars     = NULL;
    mp_uint_t  num_chars = 0;
    mp_obj_get_array(self->char_list, &num_chars, &chars);

    for (uint8_t i = 0; i < num_chars; i++) {
        ubluepy_characteristic_obj_t * p_char = (ubluepy_characteristic_obj_t *)chars[i];
        
        if(p_char->p_uuid == NULL) {
            printf("+++++ Characteristic(uuid: none, handle: %d)\r\n", p_char->handle);
        } else {
            if(p_char->p_uuid->type == UBLUEPY_UUID_128_BIT) {
                printf("+++++ Characteristic(uuid: %s, handle: %d)\r\n",gr_ble_format_uuid128b_to_string(&p_char->p_uuid->value_128b[0], 16), p_char->handle);
            } else {
                printf("+++++ Characteristic(uuid: 0x%02x%02x, handle: %d)\r\n",
                  p_char->p_uuid->value[1], p_char->p_uuid->value[0], p_char->handle);
            }
        } 
    }
        
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_service_desc_obj, service_desc);


/// \method uuid()
/// Get UUID instance of the Service.
///
STATIC mp_obj_t service_uuid(mp_obj_t self_in) {
    ubluepy_service_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->p_uuid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_service_get_uuid_obj, service_uuid);

STATIC const mp_rom_map_elem_t ubluepy_service_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_getCharacteristic),       MP_ROM_PTR(&ubluepy_service_get_char_obj) },
    { MP_ROM_QSTR(MP_QSTR_addCharacteristic),       MP_ROM_PTR(&ubluepy_service_add_char_obj) },
    { MP_ROM_QSTR(MP_QSTR_getCharacteristics),      MP_ROM_PTR(&ubluepy_service_get_chars_obj) },
    { MP_ROM_QSTR(MP_QSTR_removeCharacteristic),    MP_ROM_PTR(&ubluepy_service_remove_characteristic_obj) },
    
#if 0
    // Properties
    { MP_ROM_QSTR(MP_QSTR_peripheral), MP_ROM_PTR(&ubluepy_service_get_peripheral_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_desc),       MP_ROM_PTR(&ubluepy_service_desc_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),       MP_ROM_PTR(&ubluepy_service_get_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_PRIMARY),    MP_ROM_INT(UBLUEPY_SERVICE_PRIMARY) },
    { MP_ROM_QSTR(MP_QSTR_SECONDARY),  MP_ROM_INT(UBLUEPY_SERVICE_SECONDARY) },
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_service_locals_dict, ubluepy_service_locals_dict_table);

const mp_obj_type_t ubluepy_service_type = {
    { &mp_type_type },
    .name = MP_QSTR_Service,
    .print = ubluepy_service_print,
    .make_new = ubluepy_service_make_new,
    .locals_dict = (mp_obj_dict_t*)&ubluepy_service_locals_dict
};

#endif // MICROPY_PY_UBLUEPY_PERIPHERAL || MICROPY_PY_UBLUEPY_CENTRAL
