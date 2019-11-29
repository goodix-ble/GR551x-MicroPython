/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "gr55xx_sys.h"
#include "user_app.h"
#include "gr_config.h"
#include "gr_porting.h"
#include "xblepy_hal.h"



gr_srv_env_t s_gattsp_instance = {
    .cur_start_srv_index    = 0,
    .register_srv_num       = 0,
    .register_srv_handle    = {0xffff},
    .start_handle           = 0,
};

/*
 * LOCAL FUNCTION DECLARATION
 *****************************************************************************************
 */
static ble_err_t gatt_prf_db_init(void);
static void      srv_gatts_read_cb(uint8_t conidx, const gatts_read_req_cb_t *p_read_req);
static void      srv_gatts_write_cb(uint8_t conidx, const gatts_write_req_cb_t *p_write_req);
static void      srv_gatts_prep_write_cb(uint8_t conidx, const gatts_prep_write_req_cb_t *p_prep_req);
static void      srv_gatts_ntf_ind_cb(uint8_t conidx, uint8_t status, const ble_gatts_ntf_ind_t *p_ntf_ind);


static ble_prf_manager_cbs_t srv_mgr_cbs =
{
    gatt_prf_db_init,
    NULL,
    NULL
};

static gatts_prf_cbs_t srv_gatts_cbs =
{
    srv_gatts_read_cb,
    srv_gatts_write_cb,
    srv_gatts_prep_write_cb,
    srv_gatts_ntf_ind_cb,
};

static const prf_server_info_t srv_prf_info =
{
    .max_connection_nb = GR_BLE_SRV_CONNECT_MAX,
    .manager_cbs       = &srv_mgr_cbs,
    .gatts_prf_cbs     = &srv_gatts_cbs
};

static bool     is_first_srv = true;

static uint8_t  s_attr_mask[GR_BLE_ATTR_MASK_LEN];

static ble_err_t gatt_prf_db_init(void)
{
    uint16_t           start_hdl  = GR_PRF_INVALID_HANDLE;
    sdk_err_t          error_code = SDK_SUCCESS;
    gatts_create_db_t  gatts_db;
    BTGattServiceList_t * psrv;

    memset(&gatts_db, 0, sizeof(gatts_db));
    
    if(s_gattsp_instance.cur_start_srv_index >= s_gattsp_instance.register_srv_num){
        gr_trace("gatt_prf_db_init, index exception... ");
        return BLE_ATT_ERR_INVALID_HANDLE;
    }
    
    uint16_t new_idx = s_gattsp_instance.cur_start_srv_index % GR_BLE_MAX_SERVICES;
    uint16_t this_handle = s_gattsp_instance.register_srv_handle[new_idx];    
    
    psrv = prvBTGattServiceListGet(this_handle);
    s_gattsp_instance.cur_start_srv_index ++;
    
    if(psrv == NULL){
        gr_trace("gatt_prf_db_init, invalid service handle... \r\n");
        return SDK_ERR_INVALID_HANDLE;
    }
    for(int i = 0; i < GR_BLE_ATTR_MASK_LEN; i++){
        s_attr_mask[i] = 0xFF;
    }
    
    gatts_db.shdl                  = &start_hdl;
    gatts_db.attr_tab_cfg          = &s_attr_mask[0];
    gatts_db.max_nb_attr           = psrv->mGattNum;
    
    if (psrv->mUuidType == XBLEPY_UUID_128_BIT)
    {
        gatts_db.uuid                  = ((attm_desc_128_t *)psrv->pAttTable)->uuid;
        gatts_db.srvc_perm             = SRVC_UUID_TYPE_SET(UUID_TYPE_128);
        gatts_db.attr_tab_type         = SERVICE_TABLE_TYPE_128;
        gatts_db.attr_tab.attr_tab_128 = (attm_desc_128_t *)psrv->pAttTable;
    }
    else
    {
        uint8_t svc_uuid[2]            = BLE_ATT_16_TO_16_ARRAY(((attm_desc_t *)psrv->pAttTable)->uuid);
        
        gatts_db.uuid                  = &svc_uuid[0];
        gatts_db.srvc_perm             = 0;
        gatts_db.attr_tab_type         = SERVICE_TABLE_TYPE_16;
        gatts_db.attr_tab.attr_tab_16  = (attm_desc_t *)psrv->pAttTable;
    }

    error_code = ble_gatts_srvc_db_create(&gatts_db);

    if (SDK_SUCCESS == error_code && is_first_srv)
    {
        s_gattsp_instance.start_handle = *gatts_db.shdl;
        is_first_srv = false;
    }
    
    gr_trace("gatt_prf_db_init, code: %d, start handle: %d... \r\n", error_code, s_gattsp_instance.start_handle);

    return error_code;
}



void gr_gatt_service_reset(void){
    is_first_srv = true;
    
    s_gattsp_instance.cur_start_srv_index    = 0;
    s_gattsp_instance.register_srv_num       = 0;    
    s_gattsp_instance.start_handle           = 0;
    
    for(int i=0; i< GR_BLE_MAX_SERVICES;i++) {
        s_gattsp_instance.register_srv_handle[i] = GR_BLE_GATT_INVALID_HANDLE;
    }
}


bool gr_gatt_service_register(uint16_t service_handle) {
    bool ret = true;
    sdk_err_t  error_code;    
    BTGattServiceList_t * srvlist;
        
    srvlist = prvBTGattServiceListGet(service_handle);
    
    if(srvlist != NULL){
        s_gattsp_instance.register_srv_handle[s_gattsp_instance.register_srv_num%GR_BLE_MAX_SERVICES] = service_handle;
        s_gattsp_instance.register_srv_num++;
            
        //every service calls gatt_prf_db_init each time
        error_code = ble_server_prf_add(&srv_prf_info);  
        
        if (error_code != SDK_SUCCESS){
            gr_trace("ble_server_prf_add fail: %d  \r\n", error_code);
            //return gr_util_to_afr_status_code(error_code);
            ret = false;
        } else {
            gr_ble_gatt_handle_map_print();
        }
    } else {
        ret = false;
    }           
    
    return ret;
}

static void srv_gatts_read_cb(uint8_t conn_idx, const gatts_read_req_cb_t *p_read_req)
{
    gr_trace("srv_gatts_read_cb called...  \r\n");

    uint16_t attr_idx = gr_ble_gatt_transto_mpy_layer_handle_from_stack_handle(p_read_req->handle);

    if(mp_ble_active_peripheral_object != NULL) {
        xblepy_device_obj_t * device = MP_OBJ_TO_PTR(mp_ble_active_peripheral_object);
        if(device->gatts_delegate != mp_const_none) {
            xblepy_gatts_delegate_obj_t * gatts_dele = MP_OBJ_TO_PTR(device->gatts_delegate);
            
            mp_obj_t load_attr = mp_load_attr(gatts_dele, XBLEPY_METHOD_TO_QSTR(PNI_GATTS_HANDLE_READ_EVENT));
            mp_call_function_1(load_attr, MP_OBJ_NEW_SMALL_INT(attr_idx));   
        }
    }
}

static void srv_gatts_write_cb(uint8_t conn_idx, const gatts_write_req_cb_t *p_write_req)
{
    gr_trace("srv_gatts_write_cb called...  \r\n");

    uint16_t attr_idx   = gr_ble_gatt_transto_mpy_layer_handle_from_stack_handle(p_write_req->handle);
    mp_obj_t write_data = mp_obj_new_bytes(&p_write_req->value[0], p_write_req->length);

    if(mp_ble_active_peripheral_object != NULL) {
        xblepy_device_obj_t * device = MP_OBJ_TO_PTR(mp_ble_active_peripheral_object);
        if(device->gatts_delegate != mp_const_none) {
            xblepy_gatts_delegate_obj_t * gatts_dele = MP_OBJ_TO_PTR(device->gatts_delegate);
            
            mp_obj_t arg[3] = { 
                        MP_OBJ_NEW_SMALL_INT(attr_idx),
                        MP_OBJ_NEW_SMALL_INT(p_write_req->offset),
                        //MP_OBJ_NEW_SMALL_INT(p_write_req->length),
                        write_data};

            mp_obj_t load_attr = mp_load_attr(gatts_dele, XBLEPY_METHOD_TO_QSTR(PNI_GATTS_HANDLE_WRITE_EVENT));
            mp_call_function_n_kw(load_attr, 3, 0, arg);
        }
    }
}

static void srv_gatts_prep_write_cb(uint8_t conn_idx, const gatts_prep_write_req_cb_t *p_prep_req)
{
    gr_trace("srv_gatts_prep_write_cb called...  \r\n");
#if 0
    static GR_CB_MSG_GATTS_PREP_WRITE_T p_wr_msg;
    GR_CALLBACK_MSG_T            * msg = NULL;
    
    
    
    msg = (GR_CALLBACK_MSG_T*) gr_ble_cb_msg_alloc_mem();
    if(msg == NULL){
        return;
    }    
    p_wr_msg.gr_index           = conn_idx;    
    memcpy(&p_wr_msg.prep_write_req, p_prep_req, sizeof(gatts_prep_write_req_cb_t));
    
    msg->msg_type               = GR_CB_MSG_GATTS_PREP_WRITE;
    msg->msg                    = (void*) &p_wr_msg;
    
    gr_ble_cb_msg_send(msg, true);    
#endif
}

static void srv_gatts_ntf_ind_cb(uint8_t conn_idx, uint8_t status, const ble_gatts_ntf_ind_t *p_ntf_ind)
{
    gr_trace("srv_gatts_ntf_ind_cb called, idx:%d, status:%d...\r\n", conn_idx, status);
#if 0
    static GR_CB_MSG_NTF_IND_T n_msg;
    GR_CALLBACK_MSG_T            * msg = NULL;
    
    
    
    msg = (GR_CALLBACK_MSG_T*) gr_ble_cb_msg_alloc_mem();
    if(msg == NULL){
        return;
    }
    
    n_msg.msg_basic.gr_index    = conn_idx;
    n_msg.msg_basic.gr_status   = status;
    memcpy(&n_msg.ntf_ind, p_ntf_ind, sizeof(ble_gatts_ntf_ind_t));

    msg->msg_type               = GR_CB_MSG_GATTS_NTF_IND;
    msg->msg                    = (void*) &n_msg;
    
    gr_ble_cb_msg_send(msg, true);    
#endif
}

