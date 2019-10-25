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

#if defined(MICROPY_PY_XBLEPY) && defined(MICROPY_PY_XBLEPY_PERIPHERAL)

STATIC void xblepy_peripheral_check_ble_stack_status(void) {
    if(!s_gr_ble_common_params_ins.is_ble_initialized) {
        mp_raise_msg(&mp_type_Exception, "ble stack not enable, call ble.enable() first!");
    }
    return;
}

STATIC void xblepy_peripheral_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_peripheral_obj_t * self = (xblepy_peripheral_obj_t *)o;
    
    mp_printf(print, "Peripheral(conn id:%d,role:%d,addr:[%02x:%02x:%02x:%02x:%02x:%02x])", 
                                            self->conn_id, self->role, 
                                            self->addr.addr[0],self->addr.addr[1],self->addr.addr[2],
                                            self->addr.addr[3],self->addr.addr[4],self->addr.addr[5]
                );
}

STATIC void gap_event_handler(mp_obj_t self_in, uint16_t event_id, uint16_t conn_id, uint16_t length, uint8_t * data) {
    xblepy_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (event_id == 16) {                // connect event
        self->conn_id = conn_id;
    } else if (event_id == 17) {         // disconnect event
        self->conn_id = 0xFFFF;      // invalid connection handle
    }

    if (self->conn_handler != mp_const_none) {
        mp_obj_t args[3];
        mp_uint_t num_of_args = 3;
        args[0] = MP_OBJ_NEW_SMALL_INT(event_id);
        args[1] = MP_OBJ_NEW_SMALL_INT(conn_id);
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


#if 0   /*use device's constructor method */
STATIC mp_obj_t xblepy_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    gr_trace("+++ new peripheral() \r\n");
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
    s->device.base.type = type;
    s->role             = XBLEPY_ROLE_PERIPHERAL;

    s->delegate      = mp_const_none;
    s->conn_handler  = mp_const_none;
    s->notif_handler = mp_const_none;
    s->conn_id   = 0xFFFF;

    s->service_list = mp_obj_new_list(0, NULL);

    return MP_OBJ_FROM_PTR(s);
}
#endif

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

    //self->notif_handler = func;

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


/*
 * declare super class' methods, and register them in this class
 */
MP_DECLARE_CONST_FUN_OBJ_1(xblepy_device_whoami_obj);

STATIC const mp_rom_map_elem_t xblepy_peripheral_locals_dict_table[] = {
    /* super class method start */
    { MP_ROM_QSTR(MP_QSTR_whoami),  MP_ROM_PTR(&xblepy_device_whoami_obj) },
    /* super class method end */
    
    /* advertise & conn/disconn*/
    { MP_ROM_QSTR(MP_QSTR_startAdvertise),              MP_ROM_PTR(&xblepy_peripheral_advertise_obj) },
    { MP_ROM_QSTR(MP_QSTR_stopAdvertise),               MP_ROM_PTR(&xblepy_peripheral_advertise_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),             MP_ROM_PTR(&xblepy_peripheral_disconnect_obj) },
    
    /* Service actions */
    { MP_ROM_QSTR(MP_QSTR_addService),             MP_ROM_PTR(&xblepy_peripheral_add_service_obj) },
    { MP_ROM_QSTR(MP_QSTR_removeService),          MP_ROM_PTR(&xblepy_peripheral_remove_service_obj) },    
    { MP_ROM_QSTR(MP_QSTR_getServices),            MP_ROM_PTR(&xblepy_peripheral_get_services_obj) },
    { MP_ROM_QSTR(MP_QSTR_startServices),          MP_ROM_PTR(&xblepy_peripheral_start_services_obj) },
    
    /* event handler & delegate */
    { MP_ROM_QSTR(MP_QSTR_withDelegate),           MP_ROM_PTR(&xblepy_peripheral_with_delegate_obj) },
    { MP_ROM_QSTR(MP_QSTR_setNotificationHandler), MP_ROM_PTR(&xblepy_peripheral_set_notif_handler_obj) },
    { MP_ROM_QSTR(MP_QSTR_setConnectionHandler),   MP_ROM_PTR(&xblepy_peripheral_set_conn_handler_obj) },    

};


STATIC MP_DEFINE_CONST_DICT(xblepy_peripheral_locals_dict, xblepy_peripheral_locals_dict_table);

extern mp_obj_t xblepy_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

const mp_obj_type_t xblepy_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
    .print = xblepy_peripheral_print,
    .make_new = xblepy_device_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_peripheral_locals_dict,
    .parent = &xblepy_device_type
};

#endif /* defined(MICROPY_PY_XBLEPY) && defined(MICROPY_PY_XBLEPY_PERIPHERAL) */
