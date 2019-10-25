#include "py/nlr.h"
#include "mp_defs.h"
#include "modxblepy.h"
#include "xblepy_hal.h"
#include "gr_config.h"
#include "gr_porting.h"

#define     GR_BLE_ADV_LENGTH_FIELD_SIZE            1
#define     GR_BLE_ADV_AD_TYPE_FIELD_SIZE           1
#define     GR_BLE_AD_TYPE_FLAGS_DATA_SIZE          1
#define     GR_BLE_DEFAULT_ADV_NAME                 "gr_mpy"
#define     GR_BLE_DEFAULT_ADV_NAME_LEN             (sizeof(GR_BLE_DEFAULT_ADV_NAME) - 1)

#define     GR_BLE_ADVERTISING_INTERVAL             300
#define     GR_BLE_ADVERTISING_TIMEOUT              3000

gr_ble_gap_params_t s_gr_ble_gap_params_ins = {
    .is_connected       = false,
    .is_adv_started     = false,
    .is_mtu_exchanged   = false,
    .cur_connect_id     = 0,
    .is_need_sec_cfm    = false,
};

bool gr_xblepy_gap_start_adv(xblepy_advertise_data_t * p_adv_params) {
    static uint8_t adv_data[GR_BLE_GAP_ADV_DEFAULT_SIZE];    
    const  uint8_t max_size     = GR_BLE_ADV_DATA_LEN_MAX < GR_BLE_SCAN_RSP_DATA_LEN_MAX ? GR_BLE_ADV_DATA_LEN_MAX: GR_BLE_SCAN_RSP_DATA_LEN_MAX ;
    uint8_t     byte_pos        = 0;    
    uint8_t *   adv_name        = NULL;
    uint8_t     adv_name_len    = 0;    
    sdk_err_t   s_err           = 0;

    /*************
     * print 
     *************/
    gr_trace("+++ adv param: device - %s, len = %d \r\n", p_adv_params->p_device_name, p_adv_params->device_name_len);
    gr_trace("+++ adv param: service num- %d \r\n", p_adv_params->num_of_services);
    gr_trace("+++ adv param: data len - %d \r\n", p_adv_params->data_len);
    gr_trace("+++ adv param: connectable - %d \r\n", p_adv_params->connectable);
    
    memset(&adv_data[0], 0, GR_BLE_GAP_ADV_DEFAULT_SIZE);
    
    byte_pos = 0;    
    //set name
    {
        if (p_adv_params->device_name_len > 0) {
            adv_name        = p_adv_params->p_device_name;
            adv_name_len    = p_adv_params->device_name_len;        
        } else {
            adv_name        = GR_BLE_DEFAULT_ADV_NAME;
            adv_name_len    = GR_BLE_DEFAULT_ADV_NAME_LEN;        
        }
        
        if(adv_name_len + GR_BLE_ADV_LENGTH_FIELD_SIZE + GR_BLE_ADV_AD_TYPE_FIELD_SIZE > max_size) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "length of adv name is too long."));
            return false;
        }
        
        adv_data[byte_pos]  = (GR_BLE_ADV_AD_TYPE_FIELD_SIZE + adv_name_len);
        byte_pos           += GR_BLE_ADV_LENGTH_FIELD_SIZE;
        adv_data[byte_pos]  = BLE_GAP_AD_TYPE_COMPLETE_NAME;
        byte_pos           += GR_BLE_ADV_AD_TYPE_FIELD_SIZE;
        memcpy(&adv_data[byte_pos], adv_name, adv_name_len);
        byte_pos           += adv_name_len;        
    }
    
    //fill the adv data
    {
        if ((p_adv_params->data_len > 0) && (p_adv_params->p_data != NULL)) {
            if (p_adv_params->data_len + byte_pos > max_size) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can not fit data into the advertisment packet."));
                return false;
            }

            memcpy(&adv_data[byte_pos], p_adv_params->p_data, p_adv_params->data_len);
            byte_pos += p_adv_params->data_len;
        }
    }
    
    //set adv params
    {
        s_gr_ble_gap_params_ins.gap_adv_param.chnl_map     = GAP_ADV_CHANNEL_37_38_39;
        s_gr_ble_gap_params_ins.gap_adv_param.filter_pol   = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;          
        s_gr_ble_gap_params_ins.gap_adv_param.adv_intv_max = GR_BLE_ADVERTISING_INTERVAL * 2;
        s_gr_ble_gap_params_ins.gap_adv_param.adv_intv_min = GR_BLE_ADVERTISING_INTERVAL;
        s_gr_ble_gap_params_ins.gap_adv_param.disc_mode    = GAP_DISC_MODE_GEN_DISCOVERABLE;

        if( p_adv_params->connectable == true ) {
            s_gr_ble_gap_params_ins.gap_adv_param.adv_mode = GAP_ADV_TYPE_ADV_IND;
        } else {        
            s_gr_ble_gap_params_ins.gap_adv_param.adv_mode = GAP_ADV_TYPE_ADV_NONCONN_IND;
        }
        
        s_err = ble_gap_adv_param_set(0, BLE_GAP_OWN_ADDR_STATIC, &s_gr_ble_gap_params_ins.gap_adv_param);
        if(SDK_SUCCESS != s_err){
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can not set adv param,err(%d).", s_err));
            return false;
        }                
    }
    
    //set adv & resp data
    {
        
        {
            memcpy(&s_gr_ble_gap_params_ins.scan_rsp_data[0], &adv_data[0], byte_pos);
            s_gr_ble_gap_params_ins.scan_rsp_data_len = byte_pos;
#if 1            
            s_err = ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_SCAN_RSP, &adv_data[0], byte_pos);
            if(SDK_SUCCESS != s_err )
            {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can not set scan rsp data,err(%d)", s_err));
                return false;
            }
#endif
        }
        
        {
            memcpy(&s_gr_ble_gap_params_ins.adv_data[0], &adv_data[0], byte_pos);
            s_gr_ble_gap_params_ins.adv_data_len = byte_pos;
            
            s_err = ble_gap_adv_data_set(0, BLE_GAP_ADV_DATA_TYPE_DATA, &adv_data[0], byte_pos);
        	if(SDK_SUCCESS != s_err) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can not set adv data,err(%d).", s_err));
                return false;
            }
        }
    }
    
    //start adv
    {
        s_gr_ble_gap_params_ins.gap_adv_time_param.duration     = 0;//GR_BLE_ADVERTISING_TIMEOUT;
        s_gr_ble_gap_params_ins.gap_adv_time_param.max_adv_evt  = 0;
    
        s_err = ble_gap_adv_start(0, &s_gr_ble_gap_params_ins.gap_adv_time_param);
        if(SDK_SUCCESS != s_err) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can not start the adv,err(%d).", s_err));
            return false;
        }
    }
    
    gr_trace("+++ start adv sucess\r\n");
    return true;
}

bool gr_xblepy_gap_start_services(mp_obj_t * p_services, uint8_t num_of_services) {
    bool ret = true;
    gr_trace("+++ gr_xblepy_gap_start_services called(%d) \r\n", num_of_services);

    //1. start service 
    if ((num_of_services > 0) && (p_services != NULL)) {

        for (uint8_t i = 0; i < num_of_services; i++) {
            xblepy_service_obj_t * p_service = (xblepy_service_obj_t *)p_services[i];
            
            ret = gr_xblepy_start_service(p_service);
            if(!ret) {
                return ret;
            }
        }
    }

    return ret;
}

bool gr_xblepy_gap_delete_service(mp_obj_t * p_service) {
    bool ret = true;
    gr_trace("+++ gr_xblepy_gap_delete_service called \r\n");



}

bool gr_xblepy_gap_start_advertise(xblepy_advertise_data_t * p_adv_params) {
    bool ret;
    gr_trace("+++ gr_xblepy_gap_start_advertise called (%s) \r\n", p_adv_params->p_device_name);
    
    //1. start service 
#if 0
    if (p_adv_params->num_of_services > 0) {

        for (uint8_t i = 0; i < p_adv_params->num_of_services; i++) {
            xblepy_service_obj_t * p_service = (xblepy_service_obj_t *)p_adv_params->p_services[i];
            
            ret = gr_xblepy_start_service(p_service);
            if(!ret) {
                return ret;
            }
        }
    }
#endif
    //2. start adv
    ret = gr_xblepy_gap_start_adv(p_adv_params);    

    uint32_t patch_enable_flag_addr = 0x300041d8;
    printf("+++ patch_enable_flag: 0x%x \r\n", *((uint32_t *)0x300041d8));

    
    return ret;
}

bool gr_xblepy_gap_stop_advertise(void) {
    gr_trace("+++ gr_xblepy_gap_stop_advertise called \r\n");
    
    return true;
}


bool gr_xblepy_gap_start_adv_test(void) {
    xblepy_advertise_data_t  adv_params;

    memset(&adv_params, 0 ,sizeof(xblepy_advertise_data_t));

    adv_params.p_device_name = "micro_p";
    adv_params.device_name_len = 7;
    adv_params.p_services = NULL;
    adv_params.num_of_services = 0;
    adv_params.p_data = NULL;
    adv_params.data_len = 0;
    adv_params.connectable = true;

    return gr_xblepy_gap_start_adv(&adv_params);
}