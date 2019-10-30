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

#if MICROPY_PY_XBLEPY_PERIPHERAL

#include "modxblepy.h"

STATIC void xblepy_default_gap_delegate_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    xblepy_gap_delegate_obj_t * self = (xblepy_gap_delegate_obj_t *)o;
    (void)self;
    mp_printf(print, "GapDelegate()");
}

STATIC mp_obj_t xblepy_default_gap_delegate_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    xblepy_gap_delegate_obj_t *s = m_new_obj(xblepy_gap_delegate_obj_t);
    s->base.type = type;

    return MP_OBJ_FROM_PTR(s);
}

/// \method handleAdvStartEvent()
/// Handle adv start callback events.
/// param - status : 0 - means start adv successful; other value - start adv fail
///
STATIC mp_obj_t default_gap_delegate_handle_adv_start_event(mp_obj_t self_in, mp_obj_t status) {
    xblepy_gap_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self;
    
    uint32_t s = mp_obj_get_int(status);
    
    gr_trace("xblepy: DefaultGapDelegate:handleAdvStartEvent called, status: %d \r\n", s);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gap_delegate_handle_adv_start_event_obj, default_gap_delegate_handle_adv_start_event);

/// \method handleAdvStopEvent()
/// Handle adv stop callback events.
/// param - status : 0 - means stop adv successful; other value - stop adv fail
///
STATIC mp_obj_t default_gap_delegate_handle_adv_stop_event(mp_obj_t self_in, mp_obj_t status) {
    xblepy_gap_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self;
    int s = mp_obj_get_int(status);
    gr_trace("xblepy: DefaultGapDelegate:handleAdvStopEvent called, status: %d \r\n", s);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gap_delegate_handle_adv_stop_event_obj, default_gap_delegate_handle_adv_stop_event);


/// \method handleConnectEvent()
/// Handle connect callback events.
/// param - status : 0 - connect successful; other value - connect fail
///
STATIC mp_obj_t default_gap_delegate_handle_connect_event(mp_obj_t self_in, mp_obj_t status, mp_obj_t peer_addr) {
    xblepy_gap_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self;
    int s = mp_obj_get_int(status);
    
    uint8_t bd_addr[XBLEPY_BD_ADDR_LEN] = {0x00,0x00,0x00,0x00,0x00,0x00};
    
    if (peer_addr != mp_const_none) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(peer_addr, &bufinfo, MP_BUFFER_READ);

        if (bufinfo.len == XBLEPY_BD_ADDR_LEN) {
            memcpy(&bd_addr[0], &bufinfo.buf[0], XBLEPY_BD_ADDR_LEN);                
        }
    }

    gr_trace("xblepy: DefaultGapDelegate:handleConnectEvent called, status: %d / peer addr: %02x:%02x:%02x:%02x:%02x:%02x \r\n", s,
                                                bd_addr[0],bd_addr[1],bd_addr[2],bd_addr[3],bd_addr[4],bd_addr[5]);   

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(xblepy_default_gap_delegate_handle_connect_event_obj, default_gap_delegate_handle_connect_event);

/// \method handleDisconnectEvent()
/// Handle disconnect callback events.
/// param - status : 0 - disconnect successful; other value - disconnect fail
///
STATIC mp_obj_t default_gap_delegate_handle_disconnect_event(mp_obj_t self_in, mp_obj_t status) {
    xblepy_gap_delegate_obj_t *self = MP_OBJ_TO_PTR(self_in);
    (void)self;
    int s = mp_obj_get_int(status);
    gr_trace("xblepy: DefaultGapDelegate:handleDisconnectEvent called, status: %d \r\n", s);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(xblepy_default_gap_delegate_handle_disconnect_event_obj, default_gap_delegate_handle_disconnect_event);



STATIC const mp_rom_map_elem_t xblepy_default_gap_delegate_locals_dict_table[] = {
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GAP_HANDLE_ADV_START_EVENT)),     MP_ROM_PTR(&xblepy_default_gap_delegate_handle_adv_start_event_obj) },
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GAP_HANDLE_ADV_STOP_EVENT)),      MP_ROM_PTR(&xblepy_default_gap_delegate_handle_adv_stop_event_obj) },    
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GAP_HANDLE_CONNECT_EVENT)),       MP_ROM_PTR(&xblepy_default_gap_delegate_handle_connect_event_obj) },
    { MP_ROM_QSTR(XBLEPY_METHOD_QSTR(PNI_GAP_HANDLE_DISCONNECT_EVENT)),    MP_ROM_PTR(&xblepy_default_gap_delegate_handle_disconnect_event_obj) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_default_gap_delegate_locals_dict, xblepy_default_gap_delegate_locals_dict_table);

const mp_obj_type_t xblepy_default_gap_delegate_type = {
    { &mp_type_type },
    .name = MP_QSTR_DefaultGapDelegate,
    .print = xblepy_default_gap_delegate_print,
    .make_new = xblepy_default_gap_delegate_make_new,
    .locals_dict = (mp_obj_dict_t*)&xblepy_default_gap_delegate_locals_dict
};

#endif // MICROPY_PY_XBLEPY_PERIPHERAL
