#include "mp_defs.h"
#include "modxblepy.h"
#include "xblepy_hal.h"
#include "gr_porting.h"

typedef void (*ble_drv_gap_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gatts_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);

static ble_drv_gap_evt_callback_t           gap_event_handler;
static ble_drv_gatts_evt_callback_t         gatts_event_handler;

mp_obj_t                                    mp_ble_gap_delagate_obj = NULL;
static mp_obj_t                             mp_gatts_observer;


void gr_xblepy_set_gap_delegate_event_handler(mp_obj_t obj) {
    printf("+++ gr_xblepy_set_gap_delegate_event_handler called \r\n");
    mp_ble_gap_delagate_obj = obj;
}

void gr_xblepy_set_gatts_event_handler(mp_obj_t obj, ble_drv_gatts_evt_callback_t evt_handler) {
    printf("+++ gr_xblepy_set_gatts_event_handler called \r\n");
    mp_gatts_observer = obj;
    gatts_event_handler = evt_handler;
}