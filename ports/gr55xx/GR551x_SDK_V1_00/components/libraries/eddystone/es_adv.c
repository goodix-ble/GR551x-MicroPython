/**
 *****************************************************************************************
 *
 * @file es_adv.c
 *
 * @brief Advertising start and stop api.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */
#include "es_adv.h"
#include "gr55xx_sys.h"

/**
 * Private variables
 *****************************************************************************************
 */    
static const uint8_t default_adv_data[] =                                  /**< Advertising data. */
{
    APP_EDDYSTONE_URL_SCHEME,
    APP_EDDYSTONE_URL_URL,
};

static const uint8_t default_adv_rsp_data_set[] =                          /**< Scan responce data. */
{
    0x11,
    BLE_GAP_AD_TYPE_COMPLETE_NAME,
    'G', 'o', 'o', 'd', 'i', 'x', '_', 'E', 'd', 'd', 'y', 's', 't', 'o', 'n', 'e',
};

/**
 * Private functions
 *****************************************************************************************
 */
static void copy_default_url_data(es_slot_param_t* p_slot_data)
{
    p_slot_data->len = sizeof (default_adv_data);
    memcpy(p_slot_data->data,(uint8_t*) default_adv_data,p_slot_data->len);
    p_slot_data->radio_tx_power = es_adv_tx_power_get();
    p_slot_data->frame_type = ES_FRAME_TYPE_URL;
}

static bool packet_advertising_data (es_slot_param_t* p_slot_data, es_adv_data_t* p_adv_data)
{
    if ( (NULL == p_slot_data) || (NULL == p_adv_data))
    {
        return false;
    }

    CURRENT_FUNC_INFO_PRINT();
    
    switch (p_slot_data->frame_type) {
    case ES_FRAME_TYPE_TLM:
        es_packet_tlm (p_slot_data, p_adv_data);
        break;
    
    case ES_FRAME_TYPE_URL:
        es_packet_url (p_slot_data, p_adv_data);
        break;
    
    case ES_FRAME_TYPE_UID:
        es_packet_uid (p_slot_data, p_adv_data);
        break;
    
    default:
        copy_default_url_data(p_slot_data);
        es_packet_url (p_slot_data, p_adv_data);
        break;
    }
    
    return true;
}

static sdk_err_t   es_adv_start (es_slot_param_t* p_slot_data)
{
    sdk_err_t   error_code;
    gap_adv_time_param_t gap_adv_time_param;
    gap_adv_param_t      gap_adv_param;
    es_adv_data_t       adv_data;
    
    gap_adv_param.adv_intv_max = APP_ADV_FAST_MAX_INTERVAL;
    gap_adv_param.adv_intv_min = APP_ADV_FAST_MIN_INTERVAL;
    gap_adv_param.adv_mode     = ( (p_slot_data->connectable) ? GAP_ADV_TYPE_ADV_IND : GAP_ADV_TYPE_ADV_NONCONN_IND);
    gap_adv_param.chnl_map = GAP_ADV_CHANNEL_37_38_39;
    gap_adv_param.disc_mode    = GAP_DISC_MODE_GEN_DISCOVERABLE;
    gap_adv_param.filter_pol   = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    
    error_code = ble_gap_adv_param_set (DEFAULE_ADV_INDEX, BLE_GAP_OWN_ADDR_STATIC, &gap_adv_param);
    RETURN_IF_ERROR (error_code, BLE_SUCCESS);
    
    packet_advertising_data (p_slot_data, &adv_data);
    
    error_code = ble_gap_adv_data_set (DEFAULE_ADV_INDEX, BLE_GAP_ADV_DATA_TYPE_DATA,adv_data.data,adv_data.len);
    RETURN_IF_ERROR (error_code, BLE_SUCCESS);
    
    error_code = ble_gap_adv_data_set (DEFAULE_ADV_INDEX, BLE_GAP_ADV_DATA_TYPE_SCAN_RSP, default_adv_rsp_data_set, sizeof (default_adv_rsp_data_set));
    RETURN_IF_ERROR (error_code, BLE_SUCCESS);
    
    gap_adv_time_param.duration    = 0;
    gap_adv_time_param.max_adv_evt = 0;
    
    error_code = ble_gap_adv_start (DEFAULE_ADV_INDEX, &gap_adv_time_param);
    RETURN_IF_ERROR (error_code, BLE_SUCCESS);
    
    es_tlm_adv_cnt_inc();
    
    return error_code;
}

/**
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
sdk_err_t   es_next_adv_start (bool is_connectable)
{
    sdk_err_t   error_code;
    
    es_slot_param_t slot_data;
    
    nvds_get_next_adv_frame (&slot_data);
    
    if (is_connectable)
    {
        slot_data.connectable = true;
    }
    
    error_code =es_adv_start (&slot_data);
    ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
    
    return error_code;
}

sdk_err_t   es_adv_stop (void)
{
    sdk_err_t   error_code;
    
    error_code = ble_gap_adv_stop (DEFAULE_ADV_INDEX);
    RETURN_IF_ERROR (error_code, BLE_SUCCESS);
    
    return error_code;
}

