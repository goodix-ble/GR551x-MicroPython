#include "mp_defs.h"
#include "modubluepy.h"
#include "ubluepy_hal.h"
#include "gr_porting.h"

bool gr_ubluepy_gap_advertise_start(ubluepy_advertise_data_t * p_adv_params) {
    gr_trace("+++ gr_ubluepy_gap_advertise_start called \r\n");
    
    return true;
}

bool gr_ubluepy_gap_advertise_stop(void) {
    gr_trace("+++ gr_ubluepy_gap_advertise_stop called \r\n");
    
    return true;
}