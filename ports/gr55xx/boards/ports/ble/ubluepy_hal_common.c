#include "mp_defs.h"
#include "modubluepy.h"
#include "ubluepy_hal.h"
#include "gr_porting.h"

typedef void (*ble_drv_gap_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gatts_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);

static ble_drv_gap_evt_callback_t           gap_event_handler;
static ble_drv_gatts_evt_callback_t         gatts_event_handler;

static mp_obj_t                             mp_gap_observer;
static mp_obj_t                             mp_gatts_observer;


void gr_ubluepy_set_gap_event_handler(mp_obj_t obj, ble_drv_gap_evt_callback_t evt_handler) {
    printf("+++ gr_ubluepy_set_gap_event_handler called \r\n");
    mp_gap_observer = obj;
    gap_event_handler = evt_handler;
}

void gr_ubluepy_set_gatts_event_handler(mp_obj_t obj, ble_drv_gatts_evt_callback_t evt_handler) {
    printf("+++ gr_ubluepy_set_gatts_event_handler called \r\n");
    mp_gatts_observer = obj;
    gatts_event_handler = evt_handler;
}