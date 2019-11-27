/**
*****************************************************************************************
*
* @file es_slot.c
*
* @brief Packet data of slot for advertising 
*
*****************************************************************************************
*/
#include "es_slot.h"
#include "gr55xx_sys.h"

/**
 * Private variables
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
static uint16_t s_es_vbatt;
static int8_t   s_es_temp;
static uint32_t s_le_adv_cnt;

/**
 * Private functions
 *****************************************************************************************
 */   
/**@brief Function for updating the TEMP field of TLM*/
static int16_t get_temp (void)
{
    s_es_temp ++;
    
    /* simulate temperature,-10 to 60 */
    if ( (s_es_temp < -10) || (s_es_temp > 60))
    {
        s_es_temp = -10;
    }
    
    
    return ((s_es_temp<<8) +1);
}

/**@brief Function for updating the VBATT field of TLM*/
static uint16_t get_vbatt (void)
{
    s_es_vbatt --;
    
    /* simulate bat vol,3.70v-4.12v */
    if ( (s_es_vbatt < 3700) || (s_es_vbatt > 4120))
    {
        s_es_vbatt = 4120;
    }
    return s_es_vbatt;
}

/**
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void es_tlm_adv_cnt_inc (void)
{
    s_le_adv_cnt++;
}

void es_packet_tlm (es_slot_param_t* p_slot_data, es_adv_data_t* p_adv_data)
{
    uint8_t fragment_subscript;
    uint16_t bat_val;
    uint16_t temp_val;
    uint32_t tlm_sec_cnt;
    
    bat_val = get_vbatt();
    temp_val = get_temp();
    tlm_sec_cnt = es_tlm_sec_cnt_get();
    
    /* Device appearance */
    fragment_subscript = 1;
    p_adv_data->data[fragment_subscript++] = BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID;
    p_adv_data->data[fragment_subscript++] = LO_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = HI_U16 (ES_UUID);
    p_adv_data->data[0] = 3;
    
    /* service data */
    fragment_subscript = 5;
    p_adv_data->data[fragment_subscript++] = BLE_GAP_AD_TYPE_SERVICE_16_BIT_DATA;
    p_adv_data->data[fragment_subscript++] = LO_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = HI_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = ES_FRAME_TYPE_TLM;
    p_adv_data->data[fragment_subscript++] = ES_TLM_VERSION_TLM;
    p_adv_data->data[fragment_subscript++] = HI_U16 (bat_val);
    p_adv_data->data[fragment_subscript++] = LO_U16 (bat_val);
    p_adv_data->data[fragment_subscript++] = HI_U16 (temp_val);
    p_adv_data->data[fragment_subscript++] = LO_U16 (temp_val);
    p_adv_data->data[fragment_subscript++] = HI_UINT32_T (s_le_adv_cnt);
    p_adv_data->data[fragment_subscript++] = L3_UINT32_T (s_le_adv_cnt);
    p_adv_data->data[fragment_subscript++] = L2_UINT32_T (s_le_adv_cnt);
    p_adv_data->data[fragment_subscript++] = LO_UINT32_T (s_le_adv_cnt);
    p_adv_data->data[fragment_subscript++] = HI_UINT32_T (tlm_sec_cnt);
    p_adv_data->data[fragment_subscript++] = L3_UINT32_T (tlm_sec_cnt);
    p_adv_data->data[fragment_subscript++] = L2_UINT32_T (tlm_sec_cnt);
    p_adv_data->data[fragment_subscript++] = LO_UINT32_T (tlm_sec_cnt);
    p_adv_data->data[4] = 17;
    p_adv_data->len = p_adv_data->data[0] +p_adv_data->data[4]+2;
    
    CURRENT_FUNC_INFO_PRINT();
}

void es_packet_url (es_slot_param_t* p_slot_data, es_adv_data_t* p_adv_data)
{
    uint8_t fragment_subscript;

    /* Device appearance */
    fragment_subscript = 1;
    p_adv_data->data[fragment_subscript++] = BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID;
    p_adv_data->data[fragment_subscript++] = LO_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = HI_U16 (ES_UUID);
    p_adv_data->data[0] = 3;
    
    /* service data */
    fragment_subscript = 5;
    p_adv_data->data[fragment_subscript++] = BLE_GAP_AD_TYPE_SERVICE_16_BIT_DATA;
    p_adv_data->data[fragment_subscript++] = LO_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = HI_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = ES_FRAME_TYPE_URL;
    p_adv_data->data[fragment_subscript++] = APP_EDDYSTONE_RSSI;
    memcpy(p_adv_data->data+fragment_subscript,p_slot_data->data,p_slot_data->len);
    p_adv_data->data[4] = 5+p_slot_data->len;
    p_adv_data->len = p_adv_data->data[0] +p_adv_data->data[4]+2;
}

void es_packet_uid (es_slot_param_t* p_slot_data, es_adv_data_t* p_adv_data)
{
    uint8_t fragment_subscript;
    int8_t rfu[ES_UID_RFU_LENGTH] ={ES_UID_RFU};  
    
    /* Device appearance */
    fragment_subscript = 1;
    p_adv_data->data[fragment_subscript++] = BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID;
    p_adv_data->data[fragment_subscript++] = LO_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = HI_U16 (ES_UUID);
    p_adv_data->data[0] = 3;
    
    /* service data */
    fragment_subscript = 5;
    p_adv_data->data[fragment_subscript++] = BLE_GAP_AD_TYPE_SERVICE_16_BIT_DATA;
    p_adv_data->data[fragment_subscript++] = LO_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = HI_U16 (ES_UUID);
    p_adv_data->data[fragment_subscript++] = ES_FRAME_TYPE_UID;
    p_adv_data->data[fragment_subscript++] = APP_EDDYSTONE_RSSI;
    memcpy(p_adv_data->data+fragment_subscript,p_slot_data->data,p_slot_data->len);
    fragment_subscript +=p_slot_data->len;
    memcpy(p_adv_data->data+fragment_subscript,rfu,ES_UID_RFU_LENGTH);
    
    p_adv_data->data[4] = 5+p_slot_data->len+ES_UID_RFU_LENGTH;
    p_adv_data->len = p_adv_data->data[0] +p_adv_data->data[4]+2;
}
