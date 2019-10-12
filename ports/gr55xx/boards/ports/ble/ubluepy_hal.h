#ifndef __UBLUEPY_HAL_H__
#define __UBLUEPY_HAL_H__

#include "mp_defs.h"
#include "modubluepy.h"
#include "gr_porting.h"

bool gr_ubluepy_add_service(ubluepy_service_obj_t * service);
bool gr_ubluepy_add_characteristic(ubluepy_characteristic_obj_t * charac);


typedef void (*ble_drv_gap_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gatts_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);

void gr_ubluepy_set_gap_event_handler(mp_obj_t obj, ble_drv_gap_evt_callback_t evt_handler);
void gr_ubluepy_set_gatts_event_handler(mp_obj_t obj, ble_drv_gatts_evt_callback_t evt_handler);


bool gr_ubluepy_gap_advertise_start(ubluepy_advertise_data_t * p_adv_params);

bool gr_ubluepy_gap_advertise_stop(void);

#endif /*__UBLUEPY_HAL_H__*/
