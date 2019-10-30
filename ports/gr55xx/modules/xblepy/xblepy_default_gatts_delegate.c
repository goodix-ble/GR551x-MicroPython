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






/// \method handleReadEvent()
/// Handle read attr callback events.
/// param - attr_idx : the attribute index of service, characteristic or descriptor, which to be read
///
STATIC mp_obj_t default_gatts_delegate_handle_read_event(mp_obj_t self_in, mp_obj_t attr_idx) {
    xblepy_gatts_delegate_obj_t * self          = MP_OBJ_TO_PTR(self_in);    
    xblepy_gatts_value_t *        value         = NULL;    
    uint32_t                      attr_index    = mp_obj_get_int(attr_idx);
    gatts_read_cfm_t              cfm;
    
    gr_trace("xblepy: DefaultGattsDelegate:handleReadEvent called, attr idx: %d \r\n", attr_index);
    
    value = xblepy_gatts_delegate_read(attr_index);
        
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

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gatts_delegate_handle_read_event_obj, default_gatts_delegate_handle_read_event);


/// \method handleWriteEvent()
/// Handle read attr callback events.
/// param - attr_idx: the attribute index of service, characteristic or descriptor, which to be written
///         offset  : Offset at which the data has to be written.
///         length  : Data length to be written
///         data    : Data to be written to characteristic value
///
//STATIC mp_obj_t default_gatts_delegate_handle_write_event(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args )
//mp_obj_t attr_idx, mp_obj_t offset, mp_obj_t length, mp_obj_t data) 
STATIC mp_obj_t default_gatts_delegate_handle_write_event(size_t n_args, const mp_obj_t *all_args )
{
    gr_trace("xblepy: DefaultGattsDelegate:handleWriteEvent called, args %d \r\n", n_args);
    
    mp_obj_t self_in    = all_args[0];
    mp_obj_t attr_idx   = all_args[1];
    mp_obj_t offset     = all_args[2];
    mp_obj_t length     = all_args[3];
    mp_obj_t data       = all_args[4];
    
    xblepy_gatts_delegate_obj_t *   self      = MP_OBJ_TO_PTR(self_in);    
    bool                            wr_ret    = FALSE;
    mp_buffer_info_t                bufinfo;
    gatts_write_cfm_t               cfm;
        
    uint32_t idx     = mp_obj_get_int(attr_idx);
    uint32_t off_set = mp_obj_get_int(offset);
    uint32_t len     = mp_obj_get_int(length);
    
    gr_trace("xblepy: DefaultGattsDelegate:handleWriteEvent called, attr idx: %d; offset:%d; length: %d \r\n", idx, off_set, len);
    
    if ( (len > 0) && (data != mp_const_none) ) {
        mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len > 0) {
            wr_ret = xblepy_gatts_delegate_write(idx, off_set, bufinfo.len, bufinfo.buf);
        }
    }
    
    cfm.handle = gr_ble_gatt_transto_stack_handle_from_mpy_layer_handle(idx);
    cfm.status = BLE_ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
    
    if(wr_ret) {
        cfm.status = BLE_SUCCESS;
    }
    
    ble_gatts_write_cfm(s_gr_ble_gap_params_ins.cur_connect_id, &cfm);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(xblepy_default_gatts_delegate_handle_write_event_obj, 5, 5, default_gatts_delegate_handle_write_event);






STATIC const mp_rom_map_elem_t xblepy_default_gatts_delegate_locals_dict_table[] = {
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
