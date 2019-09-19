/**
 *****************************************************************************************
 *
 * @file es_nvds.c
 *
 * @brief NVDS apis provided for EddyStone profile
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
#include "es_nvds.h"
#include "gr55xx_sys.h"

/**
 * Private variables
 *****************************************************************************************
 */    
static const NvdsTag_t s_slot_data_tag_array[APP_MAX_ADV_SLOTS] = {                                                             \
                                                                NV_TAG_APP (ES_NVDS_TAG_SLOT_DATA_IDX_BASE + 0), \
                                                                NV_TAG_APP (ES_NVDS_TAG_SLOT_DATA_IDX_BASE + 1), \
                                                                NV_TAG_APP (ES_NVDS_TAG_SLOT_DATA_IDX_BASE + 2), \
                                                                NV_TAG_APP (ES_NVDS_TAG_SLOT_DATA_IDX_BASE + 3), \
                                                                NV_TAG_APP (ES_NVDS_TAG_SLOT_DATA_IDX_BASE + 4)  \
                                                            };
static const NvdsTag_t s_unlock_key_tag = NV_TAG_APP (ES_NVDS_TAG_ULOCK_KEY_IDX);

/**
 * Private functions
 *****************************************************************************************
 */                                                            
static uint8_t store_slot_data (uint8_t slot_no, es_slot_param_t* p_slot_data)
{
    uint8_t error_code;

    CURRENT_FUNC_INFO_PRINT();
    if (slot_no > (APP_MAX_ADV_SLOTS - 1))
    {
        slot_no = (APP_MAX_ADV_SLOTS - 1);
    }

    CURRENT_FUNC_INFO_PRINT();

    p_slot_data->strorage_flag = NVDS_STORAGE_FLAG;

    error_code = nvds_put (s_slot_data_tag_array[slot_no], sizeof (es_slot_param_t), (uint8_t*) p_slot_data);
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    return error_code;
}

static uint8_t get_slot_setting (uint8_t slot_no, es_slot_param_t* p_slot_data)
{
    uint8_t error_code;
    uint16_t len;

    if (slot_no > (APP_MAX_ADV_SLOTS - 1))
    {
        slot_no = (APP_MAX_ADV_SLOTS - 1);
    }

    len = sizeof (es_slot_param_t);

    error_code = nvds_get (s_slot_data_tag_array[slot_no], &len, (uint8_t*) p_slot_data);
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    return error_code;
}

static uint8_t nvds_lock_key_clear (void)
{
    uint8_t error_code;
    uint8_t key_data[SIZE_OF_LOCK_KEY];

    memset (key_data, 0x0ff, SIZE_OF_LOCK_KEY);

    error_code = es_nvds_lock_key_set (key_data);

    return error_code;
}

static void modify_or_add_tlm_slot_with_telemetry (void)
{
#if(APP_IS_EID_SUPPORTED)
    /* 1.if beacon has tlm,delete it(them) */
    /* 2.add one or more tlm(s) to adaptive number exit eid */
#endif //APP_IS_EID_SUPPORTED
}

static void set_plain_tlm_slot (void)
{
    uint8_t slot_no;
    es_slot_param_t slot_nvds_data;
    slot_no = es_active_slot_number_get();

    slot_nvds_data.frame_type = ES_FRAME_TYPE_TLM;
    slot_nvds_data.tlm_version = ES_TLM_VERSION_TLM;
    slot_nvds_data.radio_tx_power = es_adv_tx_power_get();
    slot_nvds_data.adv_tx_setting = es_slot_tx_power_get();
    slot_nvds_data.adv_interval = es_adv_interval_get();
    slot_nvds_data.len = ESCS_TLM_WRITE_LENGTH - ES_FRAME_TYPE_LENGTH;

    store_slot_data (slot_no, &slot_nvds_data);
}

static void set_uid_slot (uint8_t* p_uid_data)
{
    uint8_t slot_no;
    es_slot_param_t slot_nvds_data;

    slot_no = es_active_slot_number_get();

    slot_nvds_data.frame_type = ES_FRAME_TYPE_UID;
    slot_nvds_data.radio_tx_power = es_adv_tx_power_get();
    slot_nvds_data.adv_tx_setting = es_slot_tx_power_get();
    slot_nvds_data.adv_interval = es_adv_interval_get();
    slot_nvds_data.len = ES_UID_NAMESPACE_LENGTH + ES_UID_INSTANCE_LENGTH;

    memcpy (slot_nvds_data.data, p_uid_data, slot_nvds_data.len);
    store_slot_data (slot_no, &slot_nvds_data);
}

static void set_url_slot (uint8_t* p_url_data, uint8_t len)
{
    uint8_t slot_no;
    es_slot_param_t slot_nvds_data;

    slot_no = es_active_slot_number_get();

    slot_nvds_data.frame_type = ES_URL_FRAME_TYPE;
    slot_nvds_data.radio_tx_power = es_adv_tx_power_get();
    slot_nvds_data.adv_tx_setting = es_slot_tx_power_get();
    slot_nvds_data.adv_interval = es_adv_interval_get();
    slot_nvds_data.len = len;

    CURRENT_FUNC_INFO_PRINT();

    memcpy (slot_nvds_data.data, p_url_data, slot_nvds_data.len);
    store_slot_data (slot_no, &slot_nvds_data);
}

/**
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
bool is_active_slot_advertising (void)
{
    es_slot_param_t slot_nvds_data;

    get_slot_setting (es_active_slot_number_get(), &slot_nvds_data);

#if(APP_IS_EID_SUPPORTED)
    if ( (ES_FRAME_TYPE_UID == slot_nvds_data.frame_type) ||     \
            (ES_FRAME_TYPE_URL == slot_nvds_data.frame_type) ||  \
            (ES_FRAME_TYPE_TLM == slot_nvds_data.frame_type) ||  \
            (ES_FRAME_TYPE_EID == slot_nvds_data.frame_type))
#else //APP_IS_EID_SUPPORTED
    if ( (ES_FRAME_TYPE_UID == slot_nvds_data.frame_type) ||     \
            (ES_FRAME_TYPE_URL == slot_nvds_data.frame_type) ||  \
            (ES_FRAME_TYPE_TLM == slot_nvds_data.frame_type))
#endif //APP_IS_EID_SUPPORTED
    {
        if (slot_nvds_data.len <= ESCS_EID_WRITE_ECDH_LENGTH)
        {
            return true;
        }
    }

    return false;
}

uint8_t es_nvds_clear_slot (void)
{
    uint8_t error_code;
    uint8_t slot_no;
    es_slot_param_t slot_nvds_data;

    CURRENT_FUNC_INFO_PRINT();
    slot_no = es_active_slot_number_get();
    memset ( (uint8_t*) (&slot_nvds_data), 0x0ff, sizeof (es_slot_param_t));

    error_code = store_slot_data (slot_no, &slot_nvds_data);
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    return error_code;
}

uint8_t es_nvds_slot_data_get (uint8_t slot_no, uint16_t* p_length, uint8_t* p_frame_data)
{
    uint8_t error_code;
    es_slot_param_t slot_nvds_data;

    error_code = get_slot_setting (slot_no, &slot_nvds_data);
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    *p_length = slot_nvds_data.len;
    memcpy (p_frame_data, slot_nvds_data.data, slot_nvds_data.len);

    CURRENT_FUNC_INFO_PRINT();

    return error_code;
}

void es_nvds_slot_data_set (uint8_t length, uint8_t* p_frame_data)
{
    uint8_t frame_type;

    frame_type = p_frame_data[0];

    switch (frame_type)
    {
    case ES_FRAME_TYPE_UID:
        if (ESCS_UID_WRITE_LENGTH == length)
        {
            set_uid_slot (p_frame_data + ES_FRAME_TYPE_LENGTH);
        }
        break;

    case ES_FRAME_TYPE_URL:
        if ( (length >= ESCS_URL_MIN_WRITE_LENGTH) && (length <= ESCS_URL_WRITE_LENGTH))
        {
            set_url_slot (p_frame_data + ES_FRAME_TYPE_LENGTH, length - ES_FRAME_TYPE_LENGTH);
        }
        break;

    case ES_FRAME_TYPE_TLM:
        if (ESCS_TLM_WRITE_LENGTH == length)
        {
            if (es_beacon_has_eid_adv())
            {
                modify_or_add_tlm_slot_with_telemetry();
            }
            else
            {
                set_plain_tlm_slot();
            }
        }
        break;

    default:
        break;
    }
}

uint8_t es_nvds_lock_key_set (uint8_t* p_key_data)
{
    uint8_t error_code;

    error_code = nvds_put (s_unlock_key_tag, SIZE_OF_LOCK_KEY, p_key_data);
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    return error_code;
}

uint8_t es_nvds_lock_key_get (uint8_t* p_key_data)
{
    uint8_t error_code;
    uint16_t data_len;

    data_len = SIZE_OF_LOCK_KEY;

    error_code = nvds_get (s_unlock_key_tag, &data_len, p_key_data);
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    return error_code;
}

uint8_t es_nvds_factory_reset (void)
{
    uint8_t error_code;
    uint8_t i;

    for (i = 0; i < APP_MAX_ADV_SLOTS; i++)
    {
        es_active_slot_number_set (i);
        error_code = es_nvds_clear_slot();
        ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);
    }

    error_code = nvds_lock_key_clear();
    ERROR_CODE_PRINT (error_code, NVDS_SUCCESS);

    return error_code;
}

void nvds_get_next_adv_frame (es_slot_param_t* p_slot_data)
{
    uint8_t slot_no;
    es_slot_param_t slot_nvds_data;
    uint8_t slot_number;
    uint8_t i;

    slot_no = es_active_slot_number_get();
    
    for (i = 0; i < APP_MAX_ADV_SLOTS; i++)
    {
        slot_number = (i + slot_no + 1) % (APP_MAX_ADV_SLOTS);
        get_slot_setting (slot_number, &slot_nvds_data);

        if (NVDS_STORAGE_FLAG == slot_nvds_data.strorage_flag) {
#if (APP_IS_EID_SUPPORTED)
            if ( (ES_FRAME_TYPE_UID == slot_nvds_data.frame_type) ||     \
                    (ES_FRAME_TYPE_TLM == slot_nvds_data.frame_type) ||  \
                    (ES_FRAME_TYPE_URL == slot_nvds_data.frame_type) ||  \
                    (ES_FRAME_TYPE_EID == slot_nvds_data.frame_type))
#else //APP_IS_EID_SUPPORTED
            if ( (ES_FRAME_TYPE_UID == slot_nvds_data.frame_type) ||     \
                   (ES_FRAME_TYPE_TLM == slot_nvds_data.frame_type) ||  \
                    (ES_FRAME_TYPE_URL == slot_nvds_data.frame_type))
#endif //APP_IS_EID_SUPPORTED
            {
                if (slot_nvds_data.len <= ESCS_EID_WRITE_ECDH_LENGTH)
                {
                    /* update slot number */
                    es_active_slot_number_set (slot_number);

                    p_slot_data->frame_type = slot_nvds_data.frame_type;
                    p_slot_data->len = slot_nvds_data.len;
                    memcpy(p_slot_data->data,slot_nvds_data.data,p_slot_data->len);

                    p_slot_data->radio_tx_power = slot_nvds_data.radio_tx_power;
                    p_slot_data->adv_tx_setting = slot_nvds_data.adv_tx_setting;
                    p_slot_data->adv_interval = slot_nvds_data.adv_interval;

                    p_slot_data->connectable = es_adv_remain_connectable_get();

                    CURRENT_FUNC_INFO_PRINT();
                    
                    break;
                }
            }
        }
    }

    CURRENT_FUNC_INFO_PRINT();
}

