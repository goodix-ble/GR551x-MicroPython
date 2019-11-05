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
#include "gr_porting.h"
#include "xblepy_hal.h"
#include "ble_gatts.h"

#if MICROPY_PY_XBLEPY_PERIPHERAL

#include "modxblepy.h"

STATIC void xblepy_default_gatts_delegate_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_gatts_delegate_obj_t * self = (xblepy_gatts_delegate_obj_t *)o;
    (void)self;
    mp_printf(print, "GattsDelegate()");
}


STATIC mp_obj_t xblepy_default_gatts_delegate_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    xblepy_gatts_delegate_obj_t *s = m_new_obj(xblepy_gatts_delegate_obj_t);
    s->base.type = type;

    return MP_OBJ_FROM_PTR(s);
}

/// \StaticMethod sendNotification()
/// send notification message
/// param - attr_idx : integer, the attribute index of service, characteristic or descriptor, which to be read
///         data     : bytearray, data which is responsed to gatt client
STATIC mp_obj_t default_gatts_delegate_send_notification(mp_obj_t attr_idx, mp_obj_t data) {
    uint32_t                      attr_index    = mp_obj_get_int(attr_idx);
    mp_buffer_info_t              bufinfo;
    gatts_noti_ind_t              n_data;
    
    memset(&n_data, 0 , sizeof(gatts_noti_ind_t));
    n_data.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(attr_index);
    
    if (data != mp_const_none) {
        mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    }
    
    if (bufinfo.len > 0) {
        n_data.length = bufinfo.len;
        n_data.value  = bufinfo.buf; 
    } else {
        mp_raise_ValueError("empty notification data");
    }
    
    n_data.type       = BLE_GATT_NOTIFICATION;
    
    ble_gatts_noti_ind(s_gr_ble_gap_params_ins.cur_connect_id, &n_data);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gatts_delegate_send_notification_obj, default_gatts_delegate_send_notification);

/// \StaticMethod sendIndication()
/// send indication message
/// param - attr_idx : integer, the attribute index of service, characteristic or descriptor, which to be read
///         data     : bytearray, data which is responsed to gatt client
STATIC mp_obj_t default_gatts_delegate_send_indication(mp_obj_t attr_idx, mp_obj_t data) {
    uint32_t                      attr_index    = mp_obj_get_int(attr_idx);
    mp_buffer_info_t              bufinfo;
    gatts_noti_ind_t              n_data;
    
    memset(&n_data, 0 , sizeof(gatts_noti_ind_t));
    n_data.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(attr_index);
    
    if (data != mp_const_none) {
        mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    }
    
    if (bufinfo.len > 0) {
        n_data.length = bufinfo.len;
        n_data.value  = bufinfo.buf; 
    } else {
        mp_raise_ValueError("empty indication data");
    }
    
    n_data.type       = BLE_GATT_INDICATION;
    
    ble_gatts_noti_ind(s_gr_ble_gap_params_ins.cur_connect_id, &n_data);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gatts_delegate_send_indication_obj, default_gatts_delegate_send_indication);


/// \StaticMethod responseRead()
/// response to read events.
/// param - attr_idx : integer, the attribute index of service, characteristic or descriptor, which to be read
///         data     : bytearray, data which is responsed to gatt client
STATIC mp_obj_t default_gatts_delegate_response_read(mp_obj_t attr_idx, mp_obj_t data) {    
    uint32_t                      attr_index    = mp_obj_get_int(attr_idx);
    gatts_read_cfm_t              cfm;    
    mp_buffer_info_t              bufinfo;
    
    memset(&bufinfo, 0, sizeof(mp_buffer_info_t));
    
    if (data != mp_const_none) {
        mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    }
    
    if (bufinfo.len > 0) {
        cfm.length = bufinfo.len;
        cfm.value  = bufinfo.buf; 
    } else {
        char * default_val = "NO-SET";
        cfm.length = strlen(default_val);
        cfm.value  = (uint8_t*)default_val;
    }

    cfm.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(attr_index);
    cfm.status = BLE_SUCCESS;
    ble_gatts_read_cfm(s_gr_ble_gap_params_ins.cur_connect_id, &cfm);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gatts_delegate_response_read_obj, default_gatts_delegate_response_read);


/// \StaticMethod responseWrite()
/// response to write events.
/// param - attr_idx : integer, the attribute index of service, characteristic or descriptor, which to be read
///         result   : true / false, means write success or write fail
STATIC mp_obj_t default_gatts_delegate_response_write(mp_obj_t attr_idx, mp_obj_t result) {    
    uint32_t                      attr_index    = mp_obj_get_int(attr_idx);
    gatts_write_cfm_t             cfm;    

    cfm.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(attr_index);

    if(mp_obj_is_true(result)) {
        cfm.status = BLE_SUCCESS;
    } else {
        cfm.status = BLE_ATT_ERR_INSUFF_RESOURCE;
    }

    ble_gatts_write_cfm(s_gr_ble_gap_params_ins.cur_connect_id, &cfm);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gatts_delegate_response_write_obj, default_gatts_delegate_response_write);


/// \method handleReadEvent()
/// Handle read attr callback events.
/// param - attr_idx : the attribute index of service, characteristic or descriptor, which to be read
///
STATIC mp_obj_t default_gatts_delegate_handle_read_event(mp_obj_t self_in, mp_obj_t attr_idx) {
    xblepy_gatts_delegate_obj_t * self          = MP_OBJ_TO_PTR(self_in);    
    xblepy_gatts_value_t *        value         = NULL;    
    uint32_t                      attr_index    = mp_obj_get_int(attr_idx);    
    mp_obj_t                      response_data;

    gr_trace("xblepy: DefaultGattsDelegate:handleReadEvent called, attr idx: %d \r\n", attr_index);
    
    value = xblepy_gatts_delegate_read(attr_index);
    
    if(value == NULL) {
        char * default_val = "NO-SET";
        response_data =  mp_obj_new_bytes(default_val, strlen(default_val));
    } else {
        response_data = mp_obj_new_bytes(value->data, value->offset);
    }

#if 1
    default_gatts_delegate_response_read(attr_idx, response_data);
#else
    gatts_read_cfm_t              cfm;
    if(value == NULL) {
        cfm.length = strlen("not-set");
        cfm.value  = (uint8_t*)"not-set";
    } else {
        cfm.length = value->offset;
        cfm.value  = value->data;        
    }
    
    cfm.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(attr_index);
    cfm.status = BLE_SUCCESS;
    ble_gatts_read_cfm(s_gr_ble_gap_params_ins.cur_connect_id, &cfm);

#endif

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gatts_delegate_handle_read_event_obj, default_gatts_delegate_handle_read_event);


/// \method handleWriteEvent()
/// Handle read attr callback events.
/// param - self    : self
///         attr_idx: the attribute index of service, characteristic or descriptor, which to be written
///         offset  : Offset at which the data has to be written.
///         data    : Data to be written to characteristic value
///
STATIC mp_obj_t default_gatts_delegate_handle_write_event(size_t n_args, const mp_obj_t *all_args )
{
    gr_trace("xblepy: DefaultGattsDelegate:handleWriteEvent called, args %d \r\n", n_args);
    
    mp_obj_t self_in    = all_args[0];
    mp_obj_t attr_idx   = all_args[1];
    mp_obj_t offset     = all_args[2];
    mp_obj_t data       = all_args[3];
    
    xblepy_gatts_delegate_obj_t *   self      = MP_OBJ_TO_PTR(self_in);    
    bool                            wr_ret    = FALSE;
    mp_buffer_info_t                bufinfo;
            
    uint32_t idx        = mp_obj_get_int(attr_idx);
    uint32_t off_set    = mp_obj_get_int(offset);
    
    gr_trace("xblepy: DefaultGattsDelegate:handleWriteEvent called, attr idx: %d; offset:%d \r\n", idx, off_set);
    
    if (data != mp_const_none) {
        mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len > 0) {
            wr_ret = xblepy_gatts_delegate_write(idx, off_set, bufinfo.len, bufinfo.buf);
        }
    }

#if 1
    default_gatts_delegate_response_write(attr_idx, wr_ret ? mp_const_true : mp_const_false);
#else
    gatts_write_cfm_t               cfm;

    cfm.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(idx);
    cfm.status = BLE_ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
    
    if(wr_ret) {
        cfm.status = BLE_SUCCESS;
    }
    
    ble_gatts_write_cfm(s_gr_ble_gap_params_ins.cur_connect_id, &cfm);
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(xblepy_default_gatts_delegate_handle_write_event_obj, 4, 4, default_gatts_delegate_handle_write_event);



STATIC const mp_rom_map_elem_t xblepy_default_gatts_delegate_locals_dict_table[] = {
    //StaticMethod    
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GATTS_SEND_NOTIFICATION)),         MP_ROM_PTR(&xblepy_default_gatts_delegate_send_notification_obj) },
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GATTS_SEND_INDICATION)),           MP_ROM_PTR(&xblepy_default_gatts_delegate_send_indication_obj) },    
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GATTS_RESPONSE_READ)),             MP_ROM_PTR(&xblepy_default_gatts_delegate_response_read_obj) },
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GATTS_RESPONSE_WRITE)),            MP_ROM_PTR(&xblepy_default_gatts_delegate_response_write_obj) },
    
    //Object Method
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GATTS_HANDLE_READ_EVENT)),         MP_ROM_PTR(&xblepy_default_gatts_delegate_handle_read_event_obj) },
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GATTS_HANDLE_WRITE_EVENT)),        MP_ROM_PTR(&xblepy_default_gatts_delegate_handle_write_event_obj) }, 
};


STATIC MP_DEFINE_CONST_DICT(xblepy_default_gatts_delegate_locals_dict, xblepy_default_gatts_delegate_locals_dict_table);

const mp_obj_type_t xblepy_default_gatts_delegate_type = {
    { &mp_type_type },
    .name = MP_QSTR_DefaultGattsDelegate,
    .print = xblepy_default_gatts_delegate_print,
    .make_new = xblepy_default_gatts_delegate_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_default_gatts_delegate_locals_dict
};

#endif // MICROPY_PY_XBLEPY_PERIPHERAL
