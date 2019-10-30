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

#if MICROPY_PY_XBLEPY && ( MICROPY_PY_XBLEPY_PERIPHERAL || MICROPY_PY_XBLEPY_CENTRAL )

#include "modxblepy.h"

STATIC void xblepy_characteristic_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_characteristic_obj_t * self = (xblepy_characteristic_obj_t *)o;

    if(self->p_uuid == NULL) {
        mp_printf(print, "Characteristic(uuid: none, handle: %d)", self->attr_idx);
    } else {
        if(self->p_uuid->type == XBLEPY_UUID_128_BIT) {
            mp_printf(print, "Characteristic(uuid: %s, handle: %d)",gr_ble_format_uuid128b_to_string(&self->p_uuid->value_128b[0], 16), self->attr_idx);
        } else {
            mp_printf(print, "Characteristic(uuid: 0x%02x%02x, handle: %d)",
              self->p_uuid->value[1], self->p_uuid->value[0], self->attr_idx);
        }
    }    
}

STATIC mp_obj_t xblepy_characteristic_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_attr_idx, MP_ARG_REQUIRED| MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_uuid,     MP_ARG_REQUIRED| MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_props,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = XBLEPY_PROP_READ | XBLEPY_PROP_WRITE} },        
        { MP_QSTR_perms,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = XBLEPY_PERM_READ_FREE | XBLEPY_PERM_WRITE_FREE} },
        { MP_QSTR_attrs,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    xblepy_characteristic_obj_t *s = m_new_obj(xblepy_characteristic_obj_t);
    s->base.type = type;

    s->attr_idx         = args[0].u_int;

    mp_obj_t uuid_obj   = args[1].u_obj;

    if (uuid_obj == mp_const_none) {
        return MP_OBJ_FROM_PTR(s);
    }

    if (mp_obj_is_type(uuid_obj, &xblepy_uuid_type)) {
        s->p_uuid = MP_OBJ_TO_PTR(uuid_obj);
        // (void)sd_characterstic_add(s);
    } else {
        mp_raise_ValueError("Invalid UUID parameter");
    }

    if (args[2].u_int > 0) {
        s->props = (uint8_t)args[2].u_int;
    }

    if (args[3].u_int > 0) {
        s->perms = (uint8_t)args[3].u_int;
    }

    if (args[4].u_int > 0) {
        s->attrs = (uint8_t)args[4].u_int;
    }

    s->handle                   = XBLEPY_UNASSIGNED_HANDLE;
    s->service_handle           = XBLEPY_UNASSIGNED_HANDLE;

    // clear pointer to service
    s->p_service = NULL;

    // clear pointer to char value data
    s->value_data = NULL;

    s->desc_list = mp_obj_new_list(0, NULL);

    return MP_OBJ_FROM_PTR(s);
}

void char_data_callback(mp_obj_t self_in, uint16_t length, uint8_t * p_data) {
    xblepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);
    self->value_data = mp_obj_new_bytearray(length, p_data);
}

/// \method read()
/// Read Characteristic value.
///
STATIC mp_obj_t char_read(mp_obj_t self_in) {
    xblepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);

#if MICROPY_PY_XBLEPY_CENTRAL
    // TODO: free any previous allocation of value_data

    //TODO: read value data

    return self->value_data;
#else
    (void)self;
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_characteristic_read_obj, char_read);

/// \method write(data, [with_response=False])
/// Write Characteristic value.
///
STATIC mp_obj_t char_write(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    xblepy_characteristic_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_obj_t data                      = pos_args[1];

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_with_response, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    // figure out mode of the Peripheral
    xblepy_role_type_t role = self->p_service->p_periph->role;

    if (role == XBLEPY_ROLE_PERIPHERAL) {
#if MICROPY_PY_XBLEPY_PERIPHERAL
    //todo
    /*
        if (self->props & XBLEPY_PROP_NOTIFY) {
            ble_drv_attr_s_notify(self->p_service->p_periph->conn_handle,
                                  self->handle,
                                  bufinfo.len,
                                  bufinfo.buf);
        } else {
            ble_drv_attr_s_write(self->p_service->p_periph->conn_handle,
                                 self->handle,
                                 bufinfo.len,
                                 bufinfo.buf);
        }
    */
#endif
    } else {
#if MICROPY_PY_XBLEPY_CENTRAL
        bool with_response = args[0].u_bool;

        //todo: write
        /*
        ble_drv_attr_c_write(self->p_service->p_periph->conn_handle,
                             self->handle,
                             bufinfo.len,
                             bufinfo.buf,
                             with_response);
        */
#endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(xblepy_characteristic_write_obj, 2, char_write);

/// \method properties()
/// Read Characteristic value properties.
///
STATIC mp_obj_t char_properties(mp_obj_t self_in) {
    xblepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->props);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_characteristic_get_properties_obj, char_properties);

/// \method uuid()
/// Get UUID instance of the characteristic.
///
STATIC mp_obj_t char_uuid(mp_obj_t self_in) {
    xblepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->p_uuid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_characteristic_get_uuid_obj, char_uuid);


/// \method addDescriptor(Descriptor)
/// Add Descriptor to the Characteristic.
///
STATIC mp_obj_t char_add_descriptor(mp_obj_t self_in, mp_obj_t descriptor) {
    xblepy_characteristic_obj_t * self   = MP_OBJ_TO_PTR(self_in);
    xblepy_descriptor_obj_t     * p_desc = MP_OBJ_TO_PTR(descriptor);

    p_desc->p_charac                = self;
    p_desc->characteristic_handle   = self->handle;

    mp_obj_list_append(self->desc_list, descriptor);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_char_add_descriptor_obj, char_add_descriptor);

/// \method getDescriptors()
/// Return list with all descriptors registered in the Service.
///
STATIC mp_obj_t char_get_descriptors(mp_obj_t self_in) {
    xblepy_characteristic_obj_t * self = MP_OBJ_TO_PTR(self_in);

    return self->desc_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_char_get_descriptors_obj, char_get_descriptors);

/// \method getDescriptor(UUID)
/// Return descriptor with the given UUID.
///
STATIC mp_obj_t char_get_descriptor(mp_obj_t self_in, mp_obj_t uuid) {
    xblepy_characteristic_obj_t * self   = MP_OBJ_TO_PTR(self_in);
    xblepy_uuid_obj_t    * p_uuid = MP_OBJ_TO_PTR(uuid);

    // validate that there is an UUID object passed in as parameter
    if (!(mp_obj_is_type(uuid, &xblepy_uuid_type))) {
        mp_raise_ValueError("Invalid UUID parameter");
    }

    mp_obj_t * descs     = NULL;
    mp_uint_t  num_descs = 0;
    mp_obj_get_array(self->desc_list, &num_descs, &descs);

    for (uint8_t i = 0; i < num_descs; i++) {
        xblepy_descriptor_obj_t * p_desc = (xblepy_descriptor_obj_t *)descs[i];

        bool type_match = p_desc->p_uuid->type == p_uuid->type;
        bool uuid_match = ((uint16_t)(*(uint16_t *)&p_desc->p_uuid->value[0]) ==
                           (uint16_t)(*(uint16_t *)&p_uuid->value[0]));

        if (type_match && uuid_match) {
            return MP_OBJ_FROM_PTR(p_desc);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_char_get_descriptor_obj, char_get_descriptor);


/// \method removeDescriptor(Descriptor)
/// Remove Descriptor from the Characteristic.
///
STATIC mp_obj_t char_remove_descriptor(mp_obj_t self_in, mp_obj_t descriptor) {
    xblepy_characteristic_obj_t * self     = MP_OBJ_TO_PTR(self_in);
    xblepy_descriptor_obj_t    * p_desc    = MP_OBJ_TO_PTR(descriptor);

    if(p_desc != mp_const_none) {
        p_desc->p_charac = NULL;
        mp_obj_list_remove(self->desc_list, descriptor);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_char_remove_descriptor_obj, char_remove_descriptor);


STATIC const mp_rom_map_elem_t xblepy_characteristic_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_addDescriptor),       MP_ROM_PTR(&xblepy_char_add_descriptor_obj) },
    { MP_ROM_QSTR(MP_QSTR_getDescriptor),       MP_ROM_PTR(&xblepy_char_get_descriptor_obj) },
    { MP_ROM_QSTR(MP_QSTR_getDescriptors),      MP_ROM_PTR(&xblepy_char_get_descriptors_obj) },
    { MP_ROM_QSTR(MP_QSTR_removeDescriptor),    MP_ROM_PTR(&xblepy_char_remove_descriptor_obj) },

    { MP_ROM_QSTR(MP_QSTR_read),                MP_ROM_PTR(&xblepy_characteristic_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),               MP_ROM_PTR(&xblepy_characteristic_write_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_supportsRead),        MP_ROM_PTR(&xblepy_characteristic_supports_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_propertiesToString),  MP_ROM_PTR(&xblepy_characteristic_properties_to_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_getHandle),           MP_ROM_PTR(&xblepy_characteristic_get_handle_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_peripheral),          MP_ROM_PTR(&xblepy_characteristic_get_peripheral_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_uuid),                MP_ROM_PTR(&xblepy_characteristic_get_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_properties),          MP_ROM_PTR(&xblepy_characteristic_get_properties_obj) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_characteristic_locals_dict, xblepy_characteristic_locals_dict_table);

const mp_obj_type_t xblepy_characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
    .print = xblepy_characteristic_print,
    .make_new = xblepy_characteristic_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_characteristic_locals_dict
};

#endif // MICROPY_PY_XBLEPY_PERIPHERAL || MICROPY_PY_XBLEPY_CENTRAL
