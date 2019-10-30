#ifndef __XBLEPY_HAL_H__
#define __XBLEPY_HAL_H__

#include "mp_defs.h"
#include "modxblepy.h"
#include "gr_porting.h"


/***************************************************************************
 *          define native interfaces between mpy and c
 ***************************************************************************/

/* PNI means micropython native interface */
//methods for DefaultGAPDelegate
#define PNI_GAP_HANDLE_ADV_START_EVENT              handleAdvStartEvent
#define PNI_GAP_HANDLE_ADV_STOP_EVENT               handleAdvStopEvent
#define PNI_GAP_HANDLE_CONNECT_EVENT                handleConnectEvent
#define PNI_GAP_HANDLE_DISCONNECT_EVENT             handleDisconnectEvent

#define _METHOD_QSTR_(method)                       MP_QSTR_##method
#define XBLEPY_METHOD_QSTR(method)                  _METHOD_QSTR_(method)
#define XBLEPY_METHOD_STR(method)                   #method
#define XBLEPY_METHOD_TO_QSTR(method)               qstr_from_str(XBLEPY_METHOD_STR(method))







/***************************************************************************
 *          define hal methods
 ***************************************************************************/

bool gr_xblepy_gatt_add_service(xblepy_service_obj_t * service);
bool gr_xblepy_gatt_add_characteristic(xblepy_characteristic_obj_t * charac);
bool gr_xblepy_gatt_add_descriptor(xblepy_descriptor_obj_t * desc);


extern mp_obj_t     mp_ble_gap_delagate_obj;

typedef void (*ble_drv_gatts_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);

void gr_xblepy_set_gap_delegate_event_handler(mp_obj_t obj);
void gr_xblepy_set_gatts_event_handler(mp_obj_t obj, ble_drv_gatts_evt_callback_t evt_handler);


bool gr_xblepy_start_service(xblepy_service_obj_t * service);
bool gr_xblepy_stop_service(xblepy_service_obj_t * service);
bool gr_xblepy_delete_service(xblepy_service_obj_t * service);
bool gr_xblepy_gap_start_services(mp_obj_t * p_services, uint8_t num_of_services);
bool gr_xblepy_gap_start_advertise(xblepy_advertise_data_t * p_adv_params);
bool gr_xblepy_gap_stop_advertise(void);

#endif /*__XBLEPY_HAL_H__*/
