/**
*****************************************************************************************
*
* @file es_gatts_read_write.c
*
* @brief Characteristic read and write operatations for eddystone configuration service
*
*****************************************************************************************
*/
#include "es_gatts_read_write.h"
#include "gr55xx_sys.h"

/**
 * Private functions
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
static void on_time_challenge_token_generate (uint8_t* p_token_buf)
{
    uint32_t sim_data;
    
    sim_data = (uint32_t) p_token_buf;
    memcpy (p_token_buf, &sim_data, ESCS_AES_KEY_SIZE);
}

static void eid_identity_key_get (uint8_t* p_eid_identity_key_buf)
{
    uint32_t sim_data;
    
    sim_data = (uint32_t) p_eid_identity_key_buf;
    memcpy (p_eid_identity_key_buf, &sim_data, ESCS_AES_KEY_SIZE);
}

static void es_slot_on_write (uint8_t length, uint8_t* p_frame_data)
{
    /* Cleared */
    if (length == 0 || (length == 1 && p_frame_data[0] == 0))
    {
        /* clear none eid slot data */
        /* destroy state for eid slot */
        es_nvds_clear_slot();
    }
    
    /* EID slot being configured */
    else if (p_frame_data[0] == ES_FRAME_TYPE_EID)
    {
        if (is_active_slot_eid()) {
            if (ESCS_EID_WRITE_ECDH_LENGTH == length)
            {
                es_public_ecdh_key_set (p_frame_data);
            }
            if (ESCS_EID_WRITE_IDK_LENGTH == length)
            {
                es_security_key_set (p_frame_data + 1, true);
            }
        }
    }
    
    /* Non-EID slot configured */
    else
    {
        es_nvds_clear_slot();
        es_nvds_slot_data_set (length, p_frame_data);
    }
}

/**
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
att_error_t escs_attribute_value_set (uint8_t att_indx, gatts_read_cfm_t* cfm, uint8_t lock_status)
{
    uint16_t supp_frame_types;
    uint16_t adv_interval;
    int8_t supported_tx[ESCS_NUM_OF_SUPPORTED_TX_POWER] = ESCS_SUPPORTED_TX_POWER;
    
    if ( (ESCS_LOCK_STATE_UNLOCKED != lock_status) && (ESCS_LOCK_STATE_RW_VALUE != att_indx) && (ESCS_UNLOCK_RW_VALUE != att_indx) && (ESCS_REMAIN_CONNECTABLE_RW_VALUE != att_indx))
    {
        cfm->length = 0;
        cfm->status = BLE_ATT_READ_NOT_PERMITTED;
        return BLE_ATT_READ_NOT_PERMITTED;
    }
    
    switch (att_indx)
    {
    case ESCS_BROADCAST_CAP_RD_VALUE:
        /* version */
        cfm->value[0] = ES_SPEC_VERSION_BYTE;
        /* max_supported_total_slots */
        cfm->value[1] = APP_MAX_ADV_SLOTS;
        /* max_supported_eid_slots */
        cfm->value[2] = APP_MAX_EID_SLOTS;
        /* capabilities_bit_field */
        cfm->value[3] = ( (APP_IS_VARIABLE_ADV_SUPPORTED << ESCS_BROADCAST_VAR_ADV_SUPPORTED_Pos)
                          | (APP_IS_VARIABLE_TX_POWER_SUPPORTED << ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Pos))
                        & (ESCS_BROADCAST_VAR_RFU_MASK);;
        /* supported frame types */
        supp_frame_types = ( (APP_IS_URL_SUPPORTED << ESCS_FRAME_TYPE_URL_SUPPORTED_Pos)
                             | (APP_IS_UID_SUPPORTED << ESCS_FRAME_TYPE_UID_SUPPORTED_Pos)
                             | (APP_IS_TLM_SUPPORTED << ESCS_FRAME_TYPE_TLM_SUPPORTED_Pos)
                             | (APP_IS_EID_SUPPORTED << ESCS_FRAME_TYPE_EID_SUPPORTED_Pos))
                           & (ESCS_FRAME_TYPE_RFU_MASK);
        cfm->value[4] = supp_frame_types >> 8;
        cfm->value[5] = supp_frame_types;
        /* supported radio Tx power absolute (dBm) values */
        memcpy (cfm->value + ESEC_CAP_DEFAULT_LEN, supported_tx, ESCS_NUM_OF_SUPPORTED_TX_POWER);
        cfm->length = ESEC_CAP_DEFAULT_LEN + ESCS_NUM_OF_SUPPORTED_TX_POWER;
        break;
        
    case ESCS_ACTIVE_SLOT_RW_VALUE:
        cfm->value[0] = es_active_slot_number_get();
        cfm->length = ES_ACTIVE_SLOT_LENGTH;
        break;
    
    case ESCS_ADV_INTERVAL_RW_VALUE:
        adv_interval = es_adv_interval_get();
        cfm->value[0] = adv_interval >> 8;
        cfm->value[1] = adv_interval;
        cfm->length = 2;
        break;
    
    case ESCS_RADIO_TX_PWR_RW_VALUE:
        cfm->value[0] = es_slot_tx_power_get();
        cfm->length = 1;
        break;
    
    case ESCS_ADV_TX_PWR_RW_VALUE:
        cfm->value[0] = es_adv_tx_power_get();
        cfm->length = 1;
        break;
    
    case ESCS_LOCK_STATE_RW_VALUE:
        cfm->value[0] = lock_status;
        cfm->length = 1;
        break;
    
    case ESCS_UNLOCK_RW_VALUE:
        if (ESCS_LOCK_STATE_UNLOCKED == lock_status)
        {
            on_time_challenge_token_generate (cfm->value);
            cfm->length = ESCS_AES_KEY_SIZE;
        }
        else
        {
            cfm->length = 0;
            cfm->status = BLE_ATT_READ_NOT_PERMITTED;
        }
        break;
        
    case ESCS_PUBLIC_ECDH_KEY_RD_VALUE:
        if (is_active_slot_eid())
        {
            es_public_ecdh_key_get (cfm->value);
            cfm->length = ESCS_ECDH_KEY_SIZE;
        }
        else
        {
            cfm->length = 0;
            cfm->status = BLE_ATT_READ_NOT_PERMITTED;
        }
        break;
        
    case ESCS_EID_ID_KEY_RD_VALUE:
        if (is_active_slot_eid())
        {
            eid_identity_key_get (cfm->value);
            cfm->length = ESCS_AES_KEY_SIZE;
        }
        else
        {
            cfm->length = 0;
            cfm->status = BLE_ATT_UNLIKELY_ERR;
        }
        break;
        
    case ESCS_REMAIN_CONNECTABLE_RW_VALUE:
        cfm->value[0] = APP_IS_REMAIN_CONNECTABLE_SUPPORTED;
        cfm->length = 1;
        break;
    
    case ESCS_RW_ADV_SLOT_RW_VALUE:
        if (is_active_slot_advertising())
        {
            if (is_active_slot_eid())
            {
                /* 1 byte frame type, 1 byte exponent, 4-byte clock value, 8-byte EID */
                cfm->value[0] = ES_FRAME_TYPE_EID;
                cfm->value[1] = 0x00;
                memset (cfm->value + 2, 0x03, 4);
                memset (cfm->value + 6, 0x04, 8);
                cfm->length = ES_EID_GATTS_READ_LENGTH;
            }
            else
            {
                es_nvds_slot_data_get (es_active_slot_number_get(), & (cfm->length), cfm->value);
            }
        }
        else
        {
            cfm->value[0] = 0;
            cfm->length = 1;
        }
        break;
        
    default:
        break;
    }
    return BLE_SUCCESS;
}

att_error_t escs_attribute_value_get (uint8_t att_indx, gatts_write_req_cb_t *p_param, uint8_t lock_status)
{
    uint16_t att_recv_data_len;
    uint8_t* p_att_recv_data;
    uint16_t adv_interval;
    
    att_recv_data_len = p_param->length;
    p_att_recv_data = p_param->value;
    
    if (ESCS_LOCK_STATE_UNLOCKED != lock_status)
    {
        return BLE_ATT_WRITE_NOT_PERMITTED;
    }
    
    switch (att_indx)
    {   
    case ESCS_ACTIVE_SLOT_RW_VALUE:
        if ( (ESEC_ACTIVE_SLOT_LEN != att_recv_data_len) || (p_att_recv_data[0] > (APP_MAX_ADV_SLOTS - 1)))
        {
            return BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN;
        }
        else
        {
            es_active_slot_number_set (p_att_recv_data[0]);
        }
        break;
        
    case ESCS_ADV_INTERVAL_RW_VALUE:
        adv_interval = p_att_recv_data[1];
        adv_interval += ( (p_att_recv_data[0]) << 8);
        es_adv_interval_set (adv_interval);
        break;
    
    case ESCS_RADIO_TX_PWR_RW_VALUE:
        es_slot_tx_power_set (p_att_recv_data[0]);
        break;
    
    case ESCS_ADV_TX_PWR_RW_VALUE:
        es_adv_tx_power_set (p_att_recv_data[0]);
        break;
    
    case ESCS_LOCK_STATE_RW_VALUE:
        if ( (1 == att_recv_data_len) && (ESCS_LOCK_BYTE_LOCK == p_att_recv_data[0]))
        {
            set_beacon_locked();
        }
        else if ( (ESCS_LOCK_CODE_WRITE_LENGTH == att_recv_data_len) && (ESCS_LOCK_BYTE_LOCK == p_att_recv_data[0]))
        {
            set_beacon_locked();
            es_security_key_set (p_att_recv_data + 1, false);
        }
        if ( (1 == att_recv_data_len) && (ESCS_LOCK_BYTE_DISABLE_AUTO_RELOCK == p_att_recv_data[0]))
        {
            /* Do nothing special, allow the write */
        }
        break;
        
    case ESCS_UNLOCK_RW_VALUE:
    /* check data with aes with previous challenge token */
    /* need a version update to support AES function */
    /* here is a function unreally */
    {
        if (true)
        {
            set_beacon_unlocked();
        }
        else
        {
            return BLE_ATT_WRITE_NOT_PERMITTED;
        }
    }
    break;
    
    case ESCS_FACTORY_RESET_SET_VALUE:
        if ( (1 == att_recv_data_len) && (0x0b == p_att_recv_data[0]))
        {
            es_nvds_factory_reset();
        }
        else
        {
            return BLE_ATT_WRITE_NOT_PERMITTED;
        }
        break;
        
    case ESCS_REMAIN_CONNECTABLE_RW_VALUE:
#if APP_IS_REMAIN_CONNECTABLE_SUPPORTED == ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_Yes
        if (1 == att_recv_data_len)
        {
            if (p_att_recv_data[0] != 0)
            {
                es_adv_remain_connectable_set (true);
            }
            else
            {
                es_adv_remain_connectable_set (false);
            }
        }
#endif
        break;
        
    case ESCS_RW_ADV_SLOT_RW_VALUE:
        es_slot_on_write (att_recv_data_len, p_att_recv_data);
        break;
    
    default:
        break;
    }
    return BLE_SUCCESS;
}
