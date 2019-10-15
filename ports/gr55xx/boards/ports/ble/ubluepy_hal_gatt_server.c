#include "mp_defs.h"
#include "stdint.h"
#include "modubluepy.h"
#include "ubluepy_hal.h"
#include "gr_config.h"
#include "gr_porting.h"

#include "ble.h"




uint8_t                     xGattTableSize = 0;
BTGattServiceList_t         xGattSrvList[GR_BLE_MAX_SERVICES];
BTGattEntity_t              xGattTable[GR_BLE_GATT_MAX_ENTITIES];



static bool prvGetServiceAttmTable(uint16_t usServiceHandle, bool * isUUID128, void ** ptable, uint32_t * att_num);













static bool gr_transfer_mpy_props_to_goodix_props(const ubluepy_prop_t xProperties, const ubluepy_permission_t xPermissions, uint16_t *perm){

    *perm = 0;
    
    if(xProperties & UBLUEPY_PROP_BROADCAST){
        *perm |= BROADCAST_ENABLE;
    }
    
    if(xProperties & UBLUEPY_PROP_READ){
        if(xPermissions & UBLUEPY_PERM_READ){
            *perm |= READ_PERM(NOAUTH);
        } else if(xPermissions & UBLUEPY_PERM_READ_ENCRYPTED){
            *perm |= READ_PERM(UNAUTH);
        } else if(xPermissions & UBLUEPY_PERM_READ_ENCRYPTED_MITM){
            *perm |= READ_PERM(AUTH);
        }        
    }
    
    if(xProperties & UBLUEPY_PROP_WRITE_WO_RESP){
        if(xPermissions & UBLUEPY_PERM_WRITE){
            *perm |= WRITE_CMD_PERM(NOAUTH);
        } else if(xPermissions & UBLUEPY_PERM_WRITE_ENCRYPTED){
            *perm |= WRITE_CMD_PERM(UNAUTH);
        } else if(xPermissions & UBLUEPY_PERM_WRITE_ENCRYPTED_MITM){
            *perm |= WRITE_CMD_PERM(AUTH);
        }
    }
    
    if(xProperties & UBLUEPY_PROP_WRITE){
        if(xPermissions & UBLUEPY_PERM_WRITE){
            *perm |= WRITE_REQ_PERM(NOAUTH);
        } else if(xPermissions & UBLUEPY_PERM_WRITE_ENCRYPTED){
            *perm |= WRITE_REQ_PERM(UNAUTH);
        } else if(xPermissions & UBLUEPY_PERM_WRITE_ENCRYPTED_MITM){
            *perm |= WRITE_REQ_PERM(AUTH);
        }
    }
    
    if(xProperties & UBLUEPY_PROP_NOTIFY){
        if(xPermissions & (UBLUEPY_PERM_WRITE | UBLUEPY_PERM_READ)){
            *perm |= NOTIFY_PERM(NOAUTH);
        } else if(xPermissions & (UBLUEPY_PERM_WRITE_ENCRYPTED | UBLUEPY_PERM_READ_ENCRYPTED)){
            *perm |= NOTIFY_PERM(UNAUTH);
        } else if(xPermissions & (UBLUEPY_PERM_WRITE_ENCRYPTED_MITM | UBLUEPY_PERM_READ_ENCRYPTED_MITM)){
            *perm |= NOTIFY_PERM(AUTH);
        }        
    }
    
    if(xProperties & UBLUEPY_PROP_INDICATE){
        if(xPermissions & (UBLUEPY_PERM_WRITE | UBLUEPY_PERM_READ)){
            *perm |= INDICATE_PERM(NOAUTH);
        } else if(xPermissions & (UBLUEPY_PERM_WRITE_ENCRYPTED | UBLUEPY_PERM_READ_ENCRYPTED)){
            *perm |= INDICATE_PERM(UNAUTH);
        } else if(xPermissions & (UBLUEPY_PERM_WRITE_ENCRYPTED_MITM | UBLUEPY_PERM_READ_ENCRYPTED_MITM)){
            *perm |= INDICATE_PERM(AUTH);
        }         
    }
    
    if(xProperties & UBLUEPY_PROP_AUTH_SIGNED_WR){
        if(xPermissions & UBLUEPY_PERM_WRITE_SIGNED){
            *perm |= WRITE_SIGNED_PERM(UNAUTH);
        } else if(xPermissions & UBLUEPY_PERM_WRITE_SIGNED_MITM){
            *perm |= WRITE_SIGNED_PERM(AUTH);
        } else {
            *perm |= WRITE_SIGNED_PERM(NOAUTH);
        }
    }
    
    if(xProperties & UBLUEPY_PROP_EXTENDED_PROP){
        *perm |= EXT_PROP_ENABLE;
    }
    
    return true;
}





bool gr_ubluepy_gatt_add_service(ubluepy_service_obj_t * service){
    bool ret = true;
    attm_desc_t     attm;
    attm_desc_128_t attm128;    

    if( xGattTableSize == GR_BLE_GATT_MAX_ENTITIES - 1 )
    {
        gr_trace("+++ no memory for service\r\n");
        ret = false;
    }
    else
    {            
        if( UBLUEPY_SERVICE_PRIMARY == service->type ){
            xGattTable[ xGattTableSize ].type = UBLUEPY_ATTR_TYPE_PRIMARY_SERVICE;
        } else {
            xGattTable[ xGattTableSize ].type = UBLUEPY_ATTR_TYPE_SECONDARY_SERVICE;
        }
        
        if(service->p_uuid->type == UBLUEPY_UUID_16_BIT){

            attm.perm           = 0;
            attm.ext_perm       = 0;
            attm.max_size       = 0;
            attm.uuid           = (service->p_uuid->value[1] << 8) | service->p_uuid->value[0];
            
            xGattTable[ xGattTableSize ].uuid_type          = UBLUEPY_UUID_16_BIT;
            xGattTable[ xGattTableSize ].properties.attm    = attm;
        } else if(service->p_uuid->type == UBLUEPY_UUID_128_BIT){                
            attm128.perm        = 0;
            attm128.ext_perm    = 0;
            attm128.max_size    = 0;                
            memcpy(&attm128.uuid[0], &service->p_uuid->value_128b[0], GR_BLE_128BIT_UUID_LEN);
            
            xGattTable[ xGattTableSize ].uuid_type          = UBLUEPY_UUID_128_BIT;
            xGattTable[ xGattTableSize ].properties.attm128 = attm128;
        }
        
        xGattTable[ xGattTableSize ].parent_handle  = 0;
        xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? GR_BLE_GATT_PORTING_LAYER_START_HANDLE : xGattTable[ xGattTableSize - 1 ].handle + 1;            
        xGattTable[ xGattTableSize ].service_handle = xGattTable[ xGattTableSize ].handle;
        xGattTable[ xGattTableSize ].raw_properties = 0;
        xGattTable[ xGattTableSize ].raw_permissions= 0;
        
        //update mpy service handle
        service->handle = xGattTable[ xGattTableSize ].handle;
        
        //prvBTGattValueHandlePush(xGattTable[ xGattTableSize ].handle, GR_BLE_GATTS_VAR_ATTR_LEN_DEFAULT);
        xGattTableSize += 1;
        
        ret = true;
        gr_trace( "+++ Service Added to HAL table with handle %x \r\n", xGattTable[ xGattTableSize - 1 ].handle );
    }
    
    return ret;
}

bool gr_ubluepy_gatt_add_characteristic(ubluepy_characteristic_obj_t * charac) {
    bool ret = true;
    attm_desc_t     attm;
    attm_desc_128_t attm128;    
    uint16_t        perm = 0, 
                    ext_perm = 0;

    if( xGattTableSize == GR_BLE_GATT_MAX_ENTITIES - 1 )
    {
        gr_trace("+++ no memory for characteristic\r\n");
        ret = false;
    }
    else
    {
        gr_transfer_mpy_props_to_goodix_props(charac->props, charac->perms, &perm);
    
        ext_perm |= ATT_VAL_LOC_USER;
        
        if(charac->p_uuid->type == UBLUEPY_UUID_16_BIT) {
            ext_perm |= ATT_UUID_TYPE_SET(UUID_TYPE_16);            

            attm.perm           = perm;
            attm.ext_perm       = ext_perm;
            attm.max_size       = GR_BLE_GATTS_VAR_ATTR_LEN_MAX;
            attm.uuid           = (charac->p_uuid->value[1] << 8) | charac->p_uuid->value[0];
            
            xGattTable[ xGattTableSize ].uuid_type          = UBLUEPY_UUID_16_BIT;
            xGattTable[ xGattTableSize ].properties.attm    = attm;
        } else if(charac->p_uuid->type == UBLUEPY_UUID_128_BIT) {
            ext_perm |= ATT_UUID_TYPE_SET(UUID_TYPE_128);
                            
            attm128.perm        = perm;
            attm128.ext_perm    = ext_perm;
            attm128.max_size    = GR_BLE_GATTS_VAR_ATTR_LEN_MAX;                
            memcpy(&attm128.uuid[0], &charac->p_uuid->value_128b[0], GR_BLE_128BIT_UUID_LEN);
            
            xGattTable[ xGattTableSize ].uuid_type          = UBLUEPY_UUID_128_BIT;
            xGattTable[ xGattTableSize ].properties.attm128 = attm128;
        }
        
        xGattTable[ xGattTableSize ].type = UBLUEPY_ATTR_TYPE_CHARACTERISTIC_VAL;
        xGattTable[ xGattTableSize ].parent_handle  = UINT16_MAX;
        xGattTable[ xGattTableSize ].service_handle = charac->p_service->handle;
        xGattTable[ xGattTableSize ].raw_properties = charac->props;
        xGattTable[ xGattTableSize ].raw_permissions= charac->perms;

        for( int i = 0; i < xGattTableSize; ++i )
        {
            if( ( (xGattTable[i].type == UBLUEPY_ATTR_TYPE_PRIMARY_SERVICE) || (xGattTable[i].type == UBLUEPY_ATTR_TYPE_SECONDARY_SERVICE)) && ( xGattTable[ i ].handle == charac->p_service->handle ) )
            {
                xGattTable[ xGattTableSize ].parent_handle = xGattTable[ i ].handle;
                break;
            }
        }

        if( xGattTable[ xGattTableSize ].parent_handle == UINT16_MAX )
        {
            gr_trace("+++ invalid parent handle for characteristic\r\n");
            ret = false;
        }
        else
        {
            xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? GR_BLE_GATT_PORTING_LAYER_START_HANDLE : xGattTable[ xGattTableSize - 1 ].handle + 2;   //handle + 1 left for char decl
            //prvBTGattValueHandlePush(xGattTable[ xGattTableSize ].handle - 1, GR_BLE_GATTS_VAR_ATTR_LEN_DEFAULT);  //for char decl
            //prvBTGattValueHandlePush(xGattTable[ xGattTableSize ].handle, GR_BLE_GATTS_VAR_ATTR_LEN_DEFAULT);      //for char value
            
            //update mpy handle
            charac->handle = xGattTable[ xGattTableSize ].handle;
            charac->service_handle = xGattTable[ xGattTableSize ].service_handle;
            
            xGattTableSize += 1;
            
            ret = true;
            gr_trace( "Charachtersitics Added to HAL table with handle %d   \r\n", xGattTable[ xGattTableSize - 1 ].handle );
        }
    }

    return ret;
}




bool gr_ubluepy_gatt_add_descriptor(ubluepy_descriptor_obj_t * desc) {

}




bool gr_ubluepy_gatt_start_service(ubluepy_service_obj_t * service) {
    if(service == NULL)
        return false;
    
    uint16_t service_handle = service->handle;
    
    bool        ret = true;
    uint32_t    att_num = 0;
    void *      ptable = NULL;
    bool        isUuid128 = false;
    
    BTGattServiceList_t srvlist;
    
    ret = prvGetServiceAttmTable(service_handle, &isUuid128, (void*) &ptable, &att_num);

    if(ret == true){
        
        srvlist.mGattNum        = att_num;        
        srvlist.mServiceHandle  = service_handle;
        srvlist.mUuidType       = isUuid128 ? UBLUEPY_UUID_128_BIT : UBLUEPY_UUID_16_BIT;
        srvlist.pAttTable       = ptable;
        
        ret = prvBTGattServiceListPut(srvlist);
        
        if(ret == true){
            ret = gr_gatt_service_register(service_handle);
        }
    }
    gr_trace("+++ gr_ubluepy_gatt_start_service : %d  \r\n", ret); 
    
    return ret;    
}

bool gr_ubluepy_gatt_stop_service(ubluepy_service_obj_t * service) {
    service = service;
    //nothing to to
    
    return true;
}


bool prvGetServiceAttmTable(uint16_t usServiceHandle, bool * isUUID128, void ** ptable, uint32_t * att_num){
    bool        is_128b    = false;    
    uint32_t    gatt_count = 0, i =0;
    void *      pattm = NULL;
    
    gatt_count = 0;
    for( i= 0; i < xGattTableSize; i++)
    {        
        if(xGattTable[ i ].service_handle == usServiceHandle){
            if( ( xGattTable[ i ].type == UBLUEPY_ATTR_TYPE_CHARACTERISTIC_VAL ) && ( xGattTable[ i ].parent_handle == usServiceHandle ) ){ //find characteristic value
                gatt_count += 2;  //extra 1 for characteristic decl
            } else {
                gatt_count++;
            }
            
            if(xGattTable[ i ].uuid_type == UBLUEPY_UUID_128_BIT){
                is_128b = true;
            }
        }        
    }
    
    *isUUID128 = is_128b;
    
    if(gatt_count == 0){
        gr_trace("+++ no atts in service\r\n");
        return false;
    }
    
    uint32_t temp_count = 0;
    if(is_128b){        
        attm_desc_128_t char_decl_128 = {
                    BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_CHARACTERISTIC), READ_PERM_UNSEC, 0, 0
        };
        
        pattm = gr_malloc(gatt_count * sizeof(attm_desc_128_t));
        if(pattm == NULL){
            gr_trace("+++ no mem\r\n");
            return false;
        }
        memset(pattm, 0, gatt_count * sizeof(attm_desc_128_t));
        
        temp_count = 0;
        for(i = 0; i < xGattTableSize; i++){
            if(xGattTable[ i ].service_handle == usServiceHandle){
                //add char decl
                if( ( xGattTable[ i ].type == UBLUEPY_ATTR_TYPE_CHARACTERISTIC_VAL ) && ( xGattTable[ i ].parent_handle == usServiceHandle ) ){
                    memcpy(((attm_desc_128_t*)pattm) + temp_count, &char_decl_128, sizeof(attm_desc_128_t));
                    temp_count++;
                } 
                
                if(xGattTable[ i ].uuid_type == UBLUEPY_UUID_128_BIT){
                    memcpy( (((attm_desc_128_t*)pattm) + temp_count), &(xGattTable[ i ].properties.attm128), sizeof(attm_desc_128_t));
                } else {
                    uint8_t uuid128[16] = BLE_ATT_16_TO_128_ARRAY(xGattTable[ i ].properties.attm.uuid);
                    
                    memcpy( (((attm_desc_128_t*)pattm) + temp_count)->uuid, &uuid128[0], 16);
                    
                    (((attm_desc_128_t*)pattm) + temp_count)->perm      = xGattTable[ i ].properties.attm.perm;
                    (((attm_desc_128_t*)pattm) + temp_count)->ext_perm  = xGattTable[ i ].properties.attm.ext_perm;
                    (((attm_desc_128_t*)pattm) + temp_count)->max_size  = xGattTable[ i ].properties.attm.max_size;                        
                }
                temp_count++;
            }
            
            if(temp_count == gatt_count){
                break;
            }
        }
        
    } else {
        attm_desc_t char_decl = {
                    BLE_ATT_DECL_CHARACTERISTIC, READ_PERM_UNSEC, 0, 0
        };
        
        pattm = gr_malloc(gatt_count * sizeof(attm_desc_t));
        if(pattm == NULL){
            gr_trace("+++ no mem\r\n");
            return false;
        }
        memset(pattm, 0, gatt_count * sizeof(attm_desc_t));
        
        temp_count = 0;
        for(i = 0; i < xGattTableSize; i++){
            if(xGattTable[ i ].service_handle == usServiceHandle){
                //add char decl
                if( ( xGattTable[ i ].type == UBLUEPY_ATTR_TYPE_CHARACTERISTIC_VAL ) && ( xGattTable[ i ].parent_handle == usServiceHandle ) ){
                    memcpy(((attm_desc_t*)pattm) + temp_count, &char_decl, sizeof(attm_desc_t));
                    temp_count++;
                }
                
                memcpy(((attm_desc_t*)pattm) + temp_count, &(xGattTable[ i ].properties.attm), sizeof(attm_desc_t));
                temp_count++;
            }
            
            if(temp_count == gatt_count){
                break;
            }
        }
    }
    
    * ptable = pattm;
    * att_num = gatt_count;
    
    return true;
}








void prvBTGattServiceListInit(void){
    memset(&xGattSrvList[0], 0, sizeof(BTGattServiceList_t) * GR_BLE_MAX_SERVICES);
}

bool prvBTGattServiceListPut(const BTGattServiceList_t srv){
    bool isPut = false;
    
    for(int i = 0; i< GR_BLE_MAX_SERVICES; i++){
        if(!xGattSrvList[i].isUsed){
            memcpy(&xGattSrvList[i], &srv, sizeof(BTGattServiceList_t));
            xGattSrvList[i].isUsed = true;
            isPut = true;
            break;
        }
    }
    
    return isPut;
    /*
    if(isPut){
        return eBTStatusSuccess;
    } else {
        return eBTStatusNoMem;
    }
    */
}

BTGattServiceList_t * prvBTGattServiceListGet(uint16_t serviceHandle){
    
    for(int i = 0; i< GR_BLE_MAX_SERVICES; i++){
        if(xGattSrvList[i].isUsed && xGattSrvList[i].mServiceHandle == serviceHandle){
            return &xGattSrvList[i];
        }
    }
    
    return NULL;
}

BTGattServiceList_t * prvBTGattServiceListGetHead(void){
    return &xGattSrvList[0];
}

void prvBTGattServiceListDelete(uint16_t serviceHandle){
    
    for(int i = 0; i< GR_BLE_MAX_SERVICES; i++){
        if(xGattSrvList[i].isUsed && xGattSrvList[i].mServiceHandle == serviceHandle){
            //must free ptable before delete
            memset(&xGattSrvList[i], 0 , sizeof(BTGattServiceList_t));
            break;
        }
    }
}


static char * prvFormatUUID(BTGattEntity_t gatt){
    static char tbuff[64];
    uint8_t len = 0;
    memset(&tbuff[0], 0 , 64);
    
    if(gatt.uuid_type == UBLUEPY_UUID_128_BIT){
        len = 0;
        for(int i = 15;i >= 0; i--){
            len += sprintf(&tbuff[len], "%02x", gatt.properties.attm128.uuid[i]);
        }
        
    } else if(gatt.uuid_type == UBLUEPY_UUID_16_BIT){
        sprintf(&tbuff[0], "%04x", gatt.properties.attm.uuid);
    }
    
    return &tbuff[0];
}

//can be called after service added finished
void gr_ble_gatt_handle_map_print(void){
#if 1//GR_BLE_HAL_TRACE_ENABLE > 0u    
    uint32_t max = xGattTableSize > GR_BLE_GATT_MAX_ENTITIES ? GR_BLE_GATT_MAX_ENTITIES : xGattTableSize;
    uint16_t stack_handle = 0;
    
    gr_trace("\r\n++++++++ Gatt Service Handle Map ++++++++\r\n");
    gr_trace("+++ Port  +++  Stack  +++  UUID +++\r\n");
    
    for (int i=0; i< max; i++){
        if((xGattTable[i].type >= UBLUEPY_ATTR_TYPE_PRIMARY_SERVICE) && (xGattTable[i].type <= UBLUEPY_ATTR_TYPE_INCLUDED_SERVICE)) {
            stack_handle = gr_gatt_transto_ble_stack_handle(xGattTable[i].handle);
            gr_trace("+++ %-4d  +++  %-6d +++ (SERVICE    )%s \r\n", xGattTable[i].handle, stack_handle, prvFormatUUID(xGattTable[i]));
        }else if(UBLUEPY_ATTR_TYPE_CHARACTERISTIC_VAL == xGattTable[i].type) {
            stack_handle = gr_gatt_transto_ble_stack_handle(xGattTable[i].handle - 1);
            gr_trace("+++ %-4d  +++  %-6d +++   (CHAR DECL)0x2803 \r\n", xGattTable[i].handle - 1, stack_handle);

            stack_handle = gr_gatt_transto_ble_stack_handle(xGattTable[i].handle);
            gr_trace("+++ %-4d  +++  %-6d +++   (CHAR VALU)%s \r\n", xGattTable[i].handle, stack_handle, prvFormatUUID(xGattTable[i]));
        } else if(UBLUEPY_ATTR_TYPE_DESCRIPTOR == xGattTable[i].type) {
            stack_handle = gr_gatt_transto_ble_stack_handle(xGattTable[i].handle);
            gr_trace("+++ %-4d  +++  %-6d +++   (DESC     )%s \r\n", xGattTable[i].handle, stack_handle, prvFormatUUID(xGattTable[i]));
        } else {
            stack_handle = gr_gatt_transto_ble_stack_handle(xGattTable[i].handle);
            gr_trace("+++ %-4d  +++  %-6d +++ %s \r\n", xGattTable[i].handle, stack_handle, prvFormatUUID(xGattTable[i]));
        }
    }
    gr_trace("++++++++++++++++++++++++++++++++++++++++++\r\n\r\n");
#endif
    return;
}


void gr_ubluepy_init(void)
{    
    xGattTableSize = 0;
    memset(&xGattTable[0], 0, sizeof(BTGattEntity_t) * GR_BLE_GATT_MAX_ENTITIES);
    gr_gatt_service_reset();
    prvBTGattServiceListInit();
    //prvBTGattValueHandleInit();
    
    memset(&s_gr_ble_gap_params_ins, 0 , sizeof(gr_ble_gap_params_t));
}



