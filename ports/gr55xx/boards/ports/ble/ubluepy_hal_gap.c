#include "mp_defs.h"
#include "modubluepy.h"
#include "ubluepy_hal.h"
#include "gr_porting.h"

bool gr_ubluepy_gap_start_services_and_advertise(ubluepy_advertise_data_t * p_adv_params) {
    gr_trace("+++ gr_ubluepy_gap_start_services_and_advertise called \r\n");
    
    //1. start service 
    if (p_adv_params->num_of_services > 0) {

        bool type_16bit_present  = false;
        bool type_128bit_present = false;

        for (uint8_t i = 0; i < p_adv_params->num_of_services; i++) {
            ubluepy_service_obj_t * p_service = (ubluepy_service_obj_t *)p_adv_params->p_services[i];
            
            gr_ubluepy_gatt_start_service(p_service);
        }
    }
    
    //2. start adv
    
    
    return true;
}

bool gr_ubluepy_gap_stop_advertise(void) {
    gr_trace("+++ gr_ubluepy_gap_stop_advertise called \r\n");
    
    return true;
}