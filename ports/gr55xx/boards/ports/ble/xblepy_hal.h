#ifndef __XBLEPY_HAL_H__
#define __XBLEPY_HAL_H__

#include "mp_defs.h"
#include "modxblepy.h"
#include "gr_porting.h"

bool gr_xblepy_gatt_add_service(xblepy_service_obj_t * service);
bool gr_xblepy_gatt_add_characteristic(xblepy_characteristic_obj_t * charac);
bool gr_xblepy_gatt_add_descriptor(xblepy_descriptor_obj_t * desc);


typedef void (*ble_drv_gap_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gatts_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);

void gr_xblepy_set_gap_event_handler(mp_obj_t obj, ble_drv_gap_evt_callback_t evt_handler);
void gr_xblepy_set_gatts_event_handler(mp_obj_t obj, ble_drv_gatts_evt_callback_t evt_handler);


bool gr_xblepy_start_service(xblepy_service_obj_t * service);
bool gr_xblepy_stop_service(xblepy_service_obj_t * service);
bool gr_xblepy_delete_service(xblepy_service_obj_t * service);
bool gr_xblepy_gap_start_services(mp_obj_t * p_services, uint8_t num_of_services);
bool gr_xblepy_gap_start_advertise(xblepy_advertise_data_t * p_adv_params);
bool gr_xblepy_gap_stop_advertise(void);

#endif /*__XBLEPY_HAL_H__*/
