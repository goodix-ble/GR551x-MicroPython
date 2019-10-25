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

#if MICROPY_PY_XBLEPY

STATIC void xblepy_peripheral_check_ble_stack_status(void) {
    if(!s_gr_ble_common_params_ins.is_ble_initialized) {
        mp_raise_msg(&mp_type_Exception, "ble stack not enable, call ble.enable() first!");
    }
    return;
}

STATIC void xblepy_peripheral_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_peripheral_obj_t * self = (xblepy_peripheral_obj_t *)o;
    (void)self;
    mp_printf(print, "Peripheral(conn_handle: " HEX2_FMT ")",
              self->conn_handle);
}

STATIC void gap_event_handler(mp_obj_t self_in, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (event_id == 16) {                // connect event
        self->conn_handle = conn_handle;
    } else if (event_id == 17) {         // disconnect event
        self->conn_handle = 0xFFFF;      // invalid connection handle
    }

    if (self->conn_handler != mp_const_none) {
        mp_obj_t args[3];
        mp_uint_t num_of_args = 3;
        args[0] = MP_OBJ_NEW_SMALL_INT(event_id);
        args[1] = MP_OBJ_NEW_SMALL_INT(conn_handle);
        if (data != NULL) {
            args[2] = mp_obj_new_bytearray_by_ref(length, data);
        } else {
            args[2] = mp_const_none;
        }

        // for now hard-code all events to conn_handler
        mp_call_function_n_kw(self->conn_handler, num_of_args, 0, args);
    }

    (void)self;
}

STATIC void gatts_event_handler(mp_obj_t self_in, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->conn_handler != mp_const_none) {
        mp_obj_t args[3];
        mp_uint_t num_of_args = 3;
        args[0] = MP_OBJ_NEW_SMALL_INT(event_id);
        args[1] = MP_OBJ_NEW_SMALL_INT(attr_handle);
        if (data != NULL) {
            args[2] = mp_obj_new_bytearray_by_ref(length, data);
        } else {
            args[2] = mp_const_none;
        }

        // for now hard-code all events to conn_handler
        mp_call_function_n_kw(self->conn_handler, num_of_args, 0, args);
    }

}

#if MICROPY_PY_XBLEPY_CENTRAL

static volatile bool m_disc_evt_received;

STATIC void gattc_event_handler(mp_obj_t self_in, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self;
    m_disc_evt_received = true;
}
#endif

STATIC mp_obj_t xblepy_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_NEW_DEVICE_ADDR,
        ARG_NEW_ADDR_TYPE
    };

    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_DEVICE_ADDR, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { ARG_NEW_ADDR_TYPE,   MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    xblepy_peripheral_obj_t *s = m_new_obj(xblepy_peripheral_obj_t);
    s->base.type = type;

    s->delegate      = mp_const_none;
    s->conn_handler  = mp_const_none;
    s->notif_handler = mp_const_none;
    s->conn_handle   = 0xFFFF;

    s->service_list = mp_obj_new_list(0, NULL);

    return MP_OBJ_FROM_PTR(s);
}

/// \method withDelegate(DefaultDelegate)
/// Set delegate instance for handling Bluetooth LE events.
///
STATIC mp_obj_t peripheral_with_delegate(mp_obj_t self_in, mp_obj_t delegate) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->delegate = delegate;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_peripheral_with_delegate_obj, peripheral_with_delegate);

/// \method setNotificationHandler(func)
/// Set handler for Bluetooth LE notification events.
///
STATIC mp_obj_t peripheral_set_notif_handler(mp_obj_t self_in, mp_obj_t func) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->notif_handler = func;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_peripheral_set_notif_handler_obj, peripheral_set_notif_handler);

/// \method setConnectionHandler(func)
/// Set handler for Bluetooth LE connection events.
///
STATIC mp_obj_t peripheral_set_conn_handler(mp_obj_t self_in, mp_obj_t func) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->conn_handler = func;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_peripheral_set_conn_handler_obj, peripheral_set_conn_handler);

#if MICROPY_PY_XBLEPY_PERIPHERAL

/// \method startAdvertise(device_name, [data=bytearray], [connectable=True])
/// Start the advertising. Connectable advertisment type by default.
///
STATIC mp_obj_t peripheral_advertise(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device_name, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        //{ MP_QSTR_services,    MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_data,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_connectable, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    self->role = XBLEPY_ROLE_PERIPHERAL;

    //check ble stack
    xblepy_peripheral_check_ble_stack_status();

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_obj_t device_name_obj = args[0].u_obj;
    //mp_obj_t service_obj     = args[1].u_obj;
    mp_obj_t data_obj        = args[1].u_obj;
    mp_obj_t connectable_obj = args[2].u_obj;

    xblepy_advertise_data_t adv_data;
    memset(&adv_data, 0, sizeof(xblepy_advertise_data_t));

    if (device_name_obj != mp_const_none && mp_obj_is_str(device_name_obj)) {
        GET_STR_DATA_LEN(device_name_obj, str_data, str_len);

        adv_data.p_device_name = (uint8_t *)str_data;
        adv_data.device_name_len = str_len;
    }

    /*
     * use services registered in advance by call addService(...)
     * not use the service obj delivered by function-in params
     */
    {
#if 0    
        if (service_obj != mp_const_none) {
            mp_obj_t * services = NULL;
            mp_uint_t  num_services;
            mp_obj_get_array(service_obj, &num_services, &services);

            if (num_services > 0) {
                adv_data.p_services      = services;
                adv_data.num_of_services = num_services;
            }
        }
#else
        // For each service perform a characteristic discovery
        mp_obj_t * services = NULL;
        mp_uint_t  num_services;
        mp_obj_get_array(self->service_list, &num_services, &services);
        if (num_services > 0) {
            adv_data.p_services      = services;
            adv_data.num_of_services = num_services;
        }
#endif
    }

    if (data_obj != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len > 0) {
            adv_data.p_data   = bufinfo.buf;
            adv_data.data_len = bufinfo.len;
        }
    }

    adv_data.connectable = true;
    if (connectable_obj != mp_const_none && !(mp_obj_is_true(connectable_obj))) {
        adv_data.connectable = false;
    } else {
        gr_xblepy_set_gap_event_handler(MP_OBJ_FROM_PTR(self), gap_event_handler);
        gr_xblepy_set_gatts_event_handler(MP_OBJ_FROM_PTR(self), gatts_event_handler);
    }

    if(!gr_xblepy_gap_start_advertise(&adv_data)){
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                "Can not start adv."));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(xblepy_peripheral_advertise_obj, 0, peripheral_advertise);

/// \method stopAdvertise()
/// Stop advertisment if any onging advertisment.
///
STATIC mp_obj_t peripheral_advertise_stop(mp_obj_t self_in) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    gr_xblepy_gap_stop_advertise();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_peripheral_advertise_stop_obj, peripheral_advertise_stop);

#endif // MICROPY_PY_XBLEPY_PERIPHERAL

/// \method disconnect()
/// disconnect connection.
///
STATIC mp_obj_t peripheral_disconnect(mp_obj_t self_in) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    (void)self;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_peripheral_disconnect_obj, peripheral_disconnect);

/// \method addService(Service)
/// Add service to the Peripheral.
///
STATIC mp_obj_t peripheral_add_service(mp_obj_t self_in, mp_obj_t service) {
    xblepy_peripheral_obj_t * self = MP_OBJ_TO_PTR(self_in);
    xblepy_service_obj_t    * p_service = MP_OBJ_TO_PTR(service);

    p_service->p_periph = self;

    mp_obj_list_append(self->service_list, service);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_peripheral_add_service_obj, peripheral_add_service);

/// \method removeService(Service)
/// Remove service which is added by addService method from the Peripheral.
///
STATIC mp_obj_t peripheral_remove_service(mp_obj_t self_in, mp_obj_t service) {
    xblepy_peripheral_obj_t * self = MP_OBJ_TO_PTR(self_in);
    xblepy_service_obj_t    * p_service = MP_OBJ_TO_PTR(service);

    // 1. check, if service is running or adv is going, cannot remove

    // 2. remove
    if(p_service != mp_const_none) {
        //free the service resources in porting layer
        gr_xblepy_delete_service(p_service);

        p_service->p_periph = NULL;
        mp_obj_list_remove(self->service_list, service);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_peripheral_remove_service_obj, peripheral_remove_service);

/// \method getServices()
/// Return list with all service registered in the Peripheral.
///
STATIC mp_obj_t peripheral_get_services(mp_obj_t self_in) {
    xblepy_peripheral_obj_t * self = MP_OBJ_TO_PTR(self_in);

    return self->service_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_peripheral_get_services_obj, peripheral_get_services);


/// \method startServices()
/// start all services registered in the Peripheral.
///
STATIC mp_obj_t peripheral_start_services(mp_obj_t self_in) {
    xblepy_peripheral_obj_t *  self    = MP_OBJ_TO_PTR(self_in);
    uint16_t        service_handle      = 0;
    uint16_t        charac_handle       = 0;
    bool            retval              = false;
    mp_obj_t *      services            = NULL;
    mp_uint_t       num_services        = 0;
    mp_obj_t *      characs             = NULL;
    mp_uint_t       num_chars           = 0;
    mp_obj_t *      descs               = NULL;
    mp_uint_t       num_descs           = 0;

    //check ble stack
    xblepy_peripheral_check_ble_stack_status();

    /**** 1 : add services to native porting *******/

    num_services    = 0;
    services        = NULL;
    mp_obj_get_array(self->service_list, &num_services, &services);

    for (uint8_t i = 0; i < num_services; i++) {
        xblepy_service_obj_t * s = (xblepy_service_obj_t *)services[i];

        // 1. register service
        retval = gr_xblepy_gatt_add_service(s);
        if(!retval) {
            mp_raise_OSError(MP_ENOMEM);
        } else {
            //update handle
            s->start_handle = s->handle;
        }

        service_handle = s->handle;

        // 2. register characteristics belongs to service       
        num_chars   = 0; 
        characs     = NULL;
        mp_obj_get_array(s->char_list, &num_chars, &characs);
        for(uint8_t j =0; j < num_chars; j++) {
            xblepy_characteristic_obj_t * c = (xblepy_characteristic_obj_t *)characs[j];

            c->service_handle = service_handle;
            retval = gr_xblepy_gatt_add_characteristic(c);

            if (!retval) {
                mp_raise_OSError(MP_ENOMEM);
            }

            charac_handle = c->handle;

            // 3. register descriptors belongs to characteristic
            descs     = NULL;
            num_descs = 0;
            mp_obj_get_array(c->desc_list, &num_descs, &descs);
            for(uint8_t k =0; k < num_descs; k++) {
                xblepy_descriptor_obj_t * d = (xblepy_descriptor_obj_t *)descs[k];
                
                d->service_handle           = service_handle;
                d->characteristic_handle    = charac_handle;

                retval = gr_xblepy_gatt_add_descriptor(d);

                if (!retval) {
                    mp_raise_OSError(MP_ENOMEM);
                }
            }
        }
    }

    /**** 2 : start the services in stack *******/
    if((num_services > 0) &&  (services != NULL)) {
        retval = gr_xblepy_gap_start_services(services, num_services);

        if(!retval) {
            mp_raise_ValueError("start services failed");
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(xblepy_peripheral_start_services_obj, peripheral_start_services);


#if MICROPY_PY_XBLEPY_CENTRAL

    void static disc_add_service(mp_obj_t self, ble_drv_service_data_t * p_service_data) {
        xblepy_service_obj_t * p_service = m_new_obj(xblepy_service_obj_t);
        p_service->base.type = &xblepy_service_type;

        xblepy_uuid_obj_t * p_uuid = m_new_obj(xblepy_uuid_obj_t);
        p_uuid->base.type = &xblepy_uuid_type;

        p_service->p_uuid = p_uuid;

        p_uuid->type = p_service_data->uuid_type;
        p_uuid->value[0] = p_service_data->uuid & 0xFF;
        p_uuid->value[1] = p_service_data->uuid >> 8;

        p_service->handle       = p_service_data->start_handle;
        p_service->start_handle = p_service_data->start_handle;
        p_service->end_handle   = p_service_data->end_handle;

        p_service->char_list = mp_obj_new_list(0, NULL);

        peripheral_add_service(self, MP_OBJ_FROM_PTR(p_service));
    }

    void static disc_add_char(mp_obj_t service_in, ble_drv_char_data_t * p_desc_data) {
        xblepy_service_obj_t        * p_service   = MP_OBJ_TO_PTR(service_in);
        xblepy_characteristic_obj_t * p_char = m_new_obj(xblepy_characteristic_obj_t);
        p_char->base.type = &xblepy_characteristic_type;

        xblepy_uuid_obj_t * p_uuid = m_new_obj(xblepy_uuid_obj_t);
        p_uuid->base.type = &xblepy_uuid_type;

        p_char->p_uuid = p_uuid;

        p_uuid->type = p_desc_data->uuid_type;
        p_uuid->value[0] = p_desc_data->uuid & 0xFF;
        p_uuid->value[1] = p_desc_data->uuid >> 8;

        // add characteristic specific data from discovery
        p_char->props  = p_desc_data->props;
        p_char->handle = p_desc_data->value_handle;

        // equivalent to xblepy_service.c - service_add_characteristic()
        // except the registration of the characteristic towards the bluetooth stack
        p_char->service_handle = p_service->handle;
        p_char->p_service      = p_service;

        mp_obj_list_append(p_service->char_list, MP_OBJ_FROM_PTR(p_char));
    }

    /// \method connect(device_address [, addr_type=ADDR_TYPE_PUBLIC])
    /// Connect to device peripheral with the given device address.
    /// addr_type can be either ADDR_TYPE_PUBLIC (default) or
    /// ADDR_TYPE_RANDOM_STATIC.
    ///
    STATIC mp_obj_t peripheral_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
        xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
        mp_obj_t dev_addr              = pos_args[1];

        self->role = XBLEPY_ROLE_CENTRAL;

        static const mp_arg_t allowed_args[] = {
            { MP_QSTR_addr_type, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = XBLEPY_ADDR_TYPE_PUBLIC } },
        };

        // parse args
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

        uint8_t addr_type = args[0].u_int;

        ble_drv_gap_event_handler_set(MP_OBJ_FROM_PTR(self), gap_event_handler);

        if (mp_obj_is_str(dev_addr)) {
            GET_STR_DATA_LEN(dev_addr, str_data, str_len);
            if (str_len == 17) { // Example "11:22:33:aa:bb:cc"

                uint8_t * p_addr = m_new(uint8_t, 6);

                p_addr[0]  = unichar_xdigit_value(str_data[16]);
                p_addr[0] += unichar_xdigit_value(str_data[15]) << 4;
                p_addr[1]  = unichar_xdigit_value(str_data[13]);
                p_addr[1] += unichar_xdigit_value(str_data[12]) << 4;
                p_addr[2]  = unichar_xdigit_value(str_data[10]);
                p_addr[2] += unichar_xdigit_value(str_data[9]) << 4;
                p_addr[3]  = unichar_xdigit_value(str_data[7]);
                p_addr[3] += unichar_xdigit_value(str_data[6]) << 4;
                p_addr[4]  = unichar_xdigit_value(str_data[4]);
                p_addr[4] += unichar_xdigit_value(str_data[3]) << 4;
                p_addr[5]  = unichar_xdigit_value(str_data[1]);
                p_addr[5] += unichar_xdigit_value(str_data[0]) << 4;

                ble_drv_connect(p_addr, addr_type);

                m_del(uint8_t, p_addr, 6);
            }
        }

        // block until connected
        while (self->conn_handle == 0xFFFF) {
            ;
        }

        ble_drv_gattc_event_handler_set(MP_OBJ_FROM_PTR(self), gattc_event_handler);

        bool service_disc_retval = ble_drv_discover_services(self, self->conn_handle, 0x0001, disc_add_service);

        // continue discovery of primary services ...
        while (service_disc_retval) {
            // locate the last added service
            mp_obj_t * services = NULL;
            mp_uint_t  num_services;
            mp_obj_get_array(self->service_list, &num_services, &services);

            xblepy_service_obj_t * p_service = (xblepy_service_obj_t *)services[num_services - 1];

            service_disc_retval = ble_drv_discover_services(self,
                                                            self->conn_handle,
                                                            p_service->end_handle + 1,
                                                            disc_add_service);
        }

        // For each service perform a characteristic discovery
        mp_obj_t * services = NULL;
        mp_uint_t  num_services;
        mp_obj_get_array(self->service_list, &num_services, &services);

        for (uint16_t s = 0; s < num_services; s++) {
            xblepy_service_obj_t * p_service = (xblepy_service_obj_t *)services[s];
            bool char_disc_retval = ble_drv_discover_characteristic(p_service,
                                                                    self->conn_handle,
                                                                    p_service->start_handle,
                                                                    p_service->end_handle,
                                                                    disc_add_char);
            // continue discovery of characteristics ...
            while (char_disc_retval) {
                mp_obj_t * characteristics = NULL;
                mp_uint_t  num_chars;
                mp_obj_get_array(p_service->char_list, &num_chars, &characteristics);

                xblepy_characteristic_obj_t * p_char = (xblepy_characteristic_obj_t *)characteristics[num_chars - 1];
                uint16_t next_handle = p_char->handle + 1;
                if ((next_handle) < p_service->end_handle) {
                    char_disc_retval = ble_drv_discover_characteristic(p_service,
                                                                       self->conn_handle,
                                                                       next_handle,
                                                                       p_service->end_handle,
                                                                       disc_add_char);
                } else {
                    break;
                }
            }
        }

        return mp_const_none;
    }
    STATIC MP_DEFINE_CONST_FUN_OBJ_KW(xblepy_peripheral_connect_obj, 2, peripheral_connect);

#endif

STATIC const mp_rom_map_elem_t xblepy_peripheral_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_withDelegate),           MP_ROM_PTR(&xblepy_peripheral_with_delegate_obj) },
    { MP_ROM_QSTR(MP_QSTR_setNotificationHandler), MP_ROM_PTR(&xblepy_peripheral_set_notif_handler_obj) },
    { MP_ROM_QSTR(MP_QSTR_setConnectionHandler),   MP_ROM_PTR(&xblepy_peripheral_set_conn_handler_obj) },    
#if MICROPY_PY_XBLEPY_CENTRAL
    { MP_ROM_QSTR(MP_QSTR_connect),                MP_ROM_PTR(&xblepy_peripheral_connect_obj) },
    #if 0
        { MP_ROM_QSTR(MP_QSTR_disconnect),             MP_ROM_PTR(&xblepy_peripheral_disconnect_obj) },
        { MP_ROM_QSTR(MP_QSTR_getServiceByUUID),       MP_ROM_PTR(&xblepy_peripheral_get_service_by_uuid_obj) },
        { MP_ROM_QSTR(MP_QSTR_getCharacteristics),     MP_ROM_PTR(&xblepy_peripheral_get_chars_obj) },
        { MP_ROM_QSTR(MP_QSTR_getDescriptors),         MP_ROM_PTR(&xblepy_peripheral_get_descs_obj) },
        { MP_ROM_QSTR(MP_QSTR_waitForNotifications),   MP_ROM_PTR(&xblepy_peripheral_wait_for_notif_obj) },
        { MP_ROM_QSTR(MP_QSTR_writeCharacteristic),    MP_ROM_PTR(&xblepy_peripheral_write_char_obj) },
        { MP_ROM_QSTR(MP_QSTR_readCharacteristic),     MP_ROM_PTR(&xblepy_peripheral_read_char_obj) },
    #endif // 0
#endif // MICROPY_PY_XBLEPY_CENTRAL

#if MICROPY_PY_XBLEPY_PERIPHERAL || MICROPY_PY_XBLEPY_BROADCASTER
    { MP_ROM_QSTR(MP_QSTR_startAdvertise),              MP_ROM_PTR(&xblepy_peripheral_advertise_obj) },
    { MP_ROM_QSTR(MP_QSTR_stopAdvertise),               MP_ROM_PTR(&xblepy_peripheral_advertise_stop_obj) },
#endif

#if MICROPY_PY_XBLEPY_PERIPHERAL    
    { MP_ROM_QSTR(MP_QSTR_disconnect),             MP_ROM_PTR(&xblepy_peripheral_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_addService),             MP_ROM_PTR(&xblepy_peripheral_add_service_obj) },
    { MP_ROM_QSTR(MP_QSTR_removeService),          MP_ROM_PTR(&xblepy_peripheral_remove_service_obj) },    
    { MP_ROM_QSTR(MP_QSTR_getServices),            MP_ROM_PTR(&xblepy_peripheral_get_services_obj) },
    { MP_ROM_QSTR(MP_QSTR_startServices),          MP_ROM_PTR(&xblepy_peripheral_start_services_obj) },
    #if 0
        { MP_ROM_QSTR(MP_QSTR_addCharacteristic),      MP_ROM_PTR(&xblepy_peripheral_add_char_obj) },
        { MP_ROM_QSTR(MP_QSTR_addDescriptor),          MP_ROM_PTR(&xblepy_peripheral_add_desc_obj) },
        { MP_ROM_QSTR(MP_QSTR_writeCharacteristic),    MP_ROM_PTR(&xblepy_peripheral_write_char_obj) },
        { MP_ROM_QSTR(MP_QSTR_readCharacteristic),     MP_ROM_PTR(&xblepy_peripheral_read_char_obj) },
    #endif
#endif



#if MICROPY_PY_XBLEPY_OBSERVER
    // Nothing yet.
#endif
};

STATIC MP_DEFINE_CONST_DICT(xblepy_peripheral_locals_dict, xblepy_peripheral_locals_dict_table);

const mp_obj_type_t xblepy_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
    .print = xblepy_peripheral_print,
    .make_new = xblepy_peripheral_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_peripheral_locals_dict
};

#endif // MICROPY_PY_XBLEPY
