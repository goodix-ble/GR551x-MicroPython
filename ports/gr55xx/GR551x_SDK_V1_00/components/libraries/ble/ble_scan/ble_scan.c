/**
 *****************************************************************************************
 *
 * @file ble_scan.c
 *
 * @brief BLE Scan Module implementation.
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

/*
* INCLUDE FILES
******************************************************************************************
*/
#include "ble_scan.h"

/*
 * DEFINES
 *****************************************************************************************
 */


/*
 * STRUCTURES
 *****************************************************************************************
 */
/**@brief BLE Scan Module environment variable. */
struct ble_scan_env_t
{
    ble_scan_init_t              scan_init;              /**< BLE Scan Module initialization variables. */
    ble_scan_parse_report_t      data_parse_record;      /**< Advertising report parsed result. */
    uint16_t                     filter_mode_mask;       /**< Mask of filter mode. */
    ble_scan_filter_data_t       target_data;            /**< Target data provided to filter. */
    bool                         is_filter_match;        /**< Confirm filter is or not matched. */
};

/*
 * LOCAL VARIABLE DECLARATION
 *****************************************************************************************
 */
static struct ble_scan_env_t s_ble_scan_env;
static ble_scan_evt_t        s_ble_scan_evt;
static bool                  s_application_handle;

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Filter device based on filter mode and filter data.
 *
 * @param[in] p_data:     Pointer to data.
 * @param[in] length:     Length of data.
 * @param[in] p_src_addr: Pointer to source address.
 *****************************************************************************************
 */
static void ble_scanner_on_filter(const uint8_t *p_data, uint16_t length, gap_bdaddr_t *p_src_addr)
{
    s_ble_scan_env.is_filter_match = false;

    if (s_ble_scan_env.filter_mode_mask & BLE_SCAN_NAME_FILTER)
    {
        if (ble_scan_name_filter(p_data, length, &s_ble_scan_env.target_data.dev_name))
        {
            s_ble_scan_env.is_filter_match = true;
            s_ble_scan_evt.match_type.dev_name_match = true;
        }
    }

    if (s_ble_scan_env.filter_mode_mask & BLE_SCAN_APPEARANCE_FILTER)
    {
        if (ble_scan_appearance_filter(p_data, length, s_ble_scan_env.target_data.appearance))
        {
            s_ble_scan_env.is_filter_match = true;
            s_ble_scan_evt.match_type.appearance_match = true;
        }
    }

    if (s_ble_scan_env.filter_mode_mask & BLE_SCAN_UUID_FILTER)
    {
        if (ble_scan_uuid_filter(p_data, length, &s_ble_scan_env.target_data.svr_uuid))
        {
            s_ble_scan_env.is_filter_match = true;
            s_ble_scan_evt.match_type.uuid_match = true;
        }
    }

    if (s_ble_scan_env.filter_mode_mask & BLE_SCAN_ADDR_FILTER)
    {
        if (ble_scan_addr_filter(p_src_addr, &s_ble_scan_env.target_data.target_addr))
        {
            s_ble_scan_env.is_filter_match = true;
            s_ble_scan_evt.match_type.addr_match = true;
        }
    }

    if (s_ble_scan_env.is_filter_match)
    {
        s_ble_scan_evt.evt_type = BLE_SCAN_EVT_FILTER_MATCH;
        s_ble_scan_env.scan_init.evt_handler(&s_ble_scan_evt);
    }
}

/**
 *****************************************************************************************
 * @brief Parse advertising report.
 *
 * @param[in] Pointer to BLE Scanner event.
 *****************************************************************************************
 */
static void ble_scan_on_adv_report(ble_scan_evt_t *p_evt)
{
    uint8_t         adv_report_type;
    const uint8_t  *p_data;
    uint16_t        length;

    adv_report_type = p_evt->p_adv_report->adv_type;
    p_data          = p_evt->p_adv_report->data;
    length          = p_evt->p_adv_report->length;

    memset(&s_ble_scan_env.data_parse_record, 0, sizeof(ble_scan_parse_report_t));
    s_ble_scan_env.data_parse_record.target_addr = p_evt->p_adv_report->broadcaster_addr;
    ble_scan_data_parse(adv_report_type, p_data, length, &s_ble_scan_env.data_parse_record);

    s_ble_scan_evt.evt_type          = BLE_SACN_EVT_DATA_PARSE_CPLT;
    s_ble_scan_evt.data_parse_record = s_ble_scan_env.data_parse_record;
    ble_scan_evt_handler_call(&s_ble_scan_evt);

    if (s_ble_scan_env.filter_mode_mask)
    {
        ble_scanner_on_filter(p_data, length, &s_ble_scan_env.data_parse_record.target_addr);
    }
}

/**
 *****************************************************************************************
 * @brief Handler for filter device matched.
 *****************************************************************************************
 */
static sdk_err_t   ble_scan_on_filter_match(void)
{
    if (s_ble_scan_env.scan_init.connect_auto)
    {
        return ble_gap_scan_stop();
    }
    else
    {
        s_application_handle = true;
    }

    return BLE_SUCCESS;
}

/**
 *****************************************************************************************
 * @brief Handler for scan stop.
 *****************************************************************************************
 */
static sdk_err_t   ble_scan_on_scan_stop(ble_scan_evt_t *p_evt)
{
    if (BLE_SUCCESS == p_evt->error_code && s_ble_scan_env.scan_init.connect_auto)
    {
        return ble_gap_connect(s_ble_scan_env.scan_init.own_addr_type, &s_ble_scan_env.scan_init.conn_param);
    }
    else
    {
        s_application_handle = true;
    }

    return BLE_SUCCESS;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
sdk_err_t   ble_scan_init(ble_scan_init_t *p_scan_init)
{
    if (NULL == p_scan_init)
    {
        return BLE_SDK_ERR_BAD_PARAM;
    }

    memcpy(&s_ble_scan_env.scan_init, p_scan_init, sizeof(ble_scan_init_t));
    memset(&s_ble_scan_env.target_data, 0, sizeof(ble_scan_filter_data_t));

    return ble_gap_scan_param_set(s_ble_scan_env.scan_init.own_addr_type, &s_ble_scan_env.scan_init.scan_param);
}

sdk_err_t   ble_scan_start(void)
{
    if (s_ble_scan_env.scan_init.scan_param.use_whitelist)
    {
        s_ble_scan_evt.evt_type = BLE_SCAN_EVT_WHITELIST_REQUEST;
        ble_scan_evt_handler_call(&s_ble_scan_evt);
    }

    return ble_gap_scan_start();
}

sdk_err_t   ble_scan_filter_set(ble_scan_filter_mode_t filter_mode, ble_scan_filter_data_t *p_filter_data)
{
    sdk_err_t   error_code = BLE_SUCCESS;

    if (NULL == p_filter_data)
    {
        return BLE_SDK_ERR_BAD_PARAM;
    }

    switch (filter_mode)
    {
        case BLE_SCAN_NAME_FILTER:
            s_ble_scan_env.filter_mode_mask |= BLE_SCAN_NAME_FILTER;
            memcpy(&s_ble_scan_env.target_data.dev_name, &p_filter_data->dev_name, sizeof(ble_scan_name_filter_t));
            break;

        case BLE_SCAN_APPEARANCE_FILTER:
            s_ble_scan_env.filter_mode_mask |= BLE_SCAN_APPEARANCE_FILTER;
            s_ble_scan_env.target_data.appearance = p_filter_data->appearance;
            break;

        case BLE_SCAN_UUID_FILTER:
            s_ble_scan_env.filter_mode_mask |= BLE_SCAN_UUID_FILTER;
            memcpy(&s_ble_scan_env.target_data.svr_uuid, &p_filter_data->svr_uuid, sizeof(ble_scan_uuid_filter_t));
            break;

        case BLE_SCAN_ADDR_FILTER:
            s_ble_scan_env.filter_mode_mask |= BLE_SCAN_ADDR_FILTER;
            memcpy(&s_ble_scan_env.target_data.target_addr, &p_filter_data->target_addr, sizeof(gap_bdaddr_t));
            break;

        default:
            error_code = BLE_SDK_ERR_BAD_PARAM;
            break;
    }

    return error_code;
}

void ble_scan_filter_disable(void)
{
    s_ble_scan_env.filter_mode_mask = 0;
    memset(&s_ble_scan_env.target_data, 0, sizeof(ble_scan_filter_data_t));
}

void ble_scan_data_parse(gap_adv_report_type_t adv_report_type, const uint8_t *p_data, uint16_t length, ble_scan_parse_report_t *p_parse_encode)
{
    if (NULL == p_data)
    {
        return;
    }

    uint16_t current_offset       = 0;
    uint8_t  fragment_length      = 0;
    uint8_t  fragment_ad_type     = 0;
    uint8_t  data_length          = 0;
    uint16_t parse_index          = 0;
    uint8_t  ad_type_count        = 1;
    uint8_t  ad_type_index        = 0;

    p_parse_encode->adv_report_type = adv_report_type;

    while (current_offset < length)
    {
        fragment_length = p_data[current_offset++];

        if (0 == fragment_length)
        {
            break;
        }
        else
        {
            data_length      = fragment_length - 1;
            fragment_ad_type = p_data[current_offset++];
            ad_type_index    = ad_type_count - 1;
        }

        memcpy(&p_parse_encode->all_parse_record.raw_data[parse_index], &p_data[current_offset], data_length);
        p_parse_encode->all_parse_record.single_record[ad_type_index].adv_type = (gap_ad_type_t)fragment_ad_type;
        p_parse_encode->all_parse_record.single_record[ad_type_index].p_data   = &p_parse_encode->all_parse_record.raw_data[parse_index];
        p_parse_encode->all_parse_record.single_record[ad_type_index].length   = data_length;

        switch (fragment_ad_type)
        {
            case BLE_GAP_AD_TYPE_FLAGS:
                p_parse_encode->flag = p_data[current_offset];
                break;

            case BLE_GAP_AD_TYPE_TRANSMIT_POWER:
                p_parse_encode->tx_power = p_data[current_offset];
                break;

            case BLE_GAP_AD_TYPE_APPEARANCE:
                p_parse_encode->appearance = p_data[current_offset] |  p_data[current_offset + 1] << 8;
                break;

            case BLE_GAP_AD_TYPE_SHORTENED_NAME:
            case BLE_GAP_AD_TYPE_COMPLETE_NAME:
                p_parse_encode->local_name.p_data = &p_parse_encode->all_parse_record.raw_data[parse_index];
                p_parse_encode->local_name.length = data_length;
                break;

            case BLE_GAP_AD_TYPE_MANU_SPECIFIC_DATA:
                p_parse_encode->manufacture_data.company_id = p_data[current_offset] | p_data[current_offset + 1] << 8;
                p_parse_encode->manufacture_data.p_data     = &p_parse_encode->all_parse_record.raw_data[parse_index + 2];
                p_parse_encode->manufacture_data.length     = data_length - 2;
                break;

            case BLE_GAP_AD_TYPE_MORE_16_BIT_UUID:
            case BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID:
                ble_scan_uuid_encode(&p_data[current_offset], data_length, UUID_16_BIT_BYTES, &p_parse_encode->srv_uuid_list);
                break;

            case BLE_GAP_AD_TYPE_MORE_32_BIT_UUID:
            case BLE_GAP_AD_TYPE_COMPLETE_LIST_32_BIT_UUID:
                ble_scan_uuid_encode(&p_data[current_offset], data_length, UUID_32_BIT_BYTES, &p_parse_encode->srv_uuid_list);
                break;

            case BLE_GAP_AD_TYPE_MORE_128_BIT_UUID:
            case BLE_GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID:
                ble_scan_uuid_encode(&p_data[current_offset], data_length, UUID_128_BIT_BYTES, &p_parse_encode->srv_uuid_list);
                break;

            default:
                break;
        }
        p_parse_encode->all_parse_record.data_type_count = ad_type_count;
        current_offset += data_length;
        parse_index    += data_length;
        ad_type_count++;
    }
}

void ble_scan_uuid_encode(const uint8_t *p_uuid_data, uint16_t length, uint8_t uuid_type, ble_scan_srv_uuid_t *p_uuid_buff)
{
    if (NULL == p_uuid_data || NULL == p_uuid_buff)
    {
        return;
    }

    uint8_t current_offset = 0;

    while (current_offset < length)
    {
        switch (uuid_type)
        {
            case UUID_16_BIT_BYTES:
                if (p_uuid_buff->uuid_16_bit_count < UUID_16_BIT_NUM_MAX)
                {
                    memcpy(&p_uuid_buff->uuid_16_bit[p_uuid_buff->uuid_16_bit_count++], &p_uuid_data[current_offset], UUID_16_BIT_BYTES);
                    current_offset += UUID_16_BIT_BYTES;
                }
                break;

            case UUID_32_BIT_BYTES:
                if (p_uuid_buff->uuid_32_bit_count < UUID_32_BIT_NUM_MAX)
                {
                    memcpy(&p_uuid_buff->uuid_16_bit[p_uuid_buff->uuid_32_bit_count++], &p_uuid_data[current_offset], UUID_32_BIT_BYTES);
                    current_offset += UUID_32_BIT_BYTES;
                }
                break;

            case UUID_128_BIT_BYTES:
                if (p_uuid_buff->uuid_128_bit_count < UUID_128_BIT_NUM_MAX)
                {
                    memcpy(&p_uuid_buff->uuid_128_bit[p_uuid_buff->uuid_128_bit_count++][0], &p_uuid_data[current_offset], UUID_128_BIT_BYTES);
                    current_offset += UUID_128_BIT_BYTES;
                }
                break;

            default:
                return;
        }
    }
}


bool ble_scan_name_filter(const uint8_t *p_data, uint16_t length, ble_scan_name_filter_t *p_target_name)
{
    if (NULL == p_data)
    {
        return false;
    }

    uint16_t current_offset       = 0;
    uint8_t  fragment_length      = 0;
    uint8_t  fragment_ad_type     = 0;
    uint8_t  data_length = 0;

    while (current_offset < length)
    {
        fragment_length = p_data[current_offset++];

        if (0 == fragment_length)
        {
            break;
        }

        data_length      = fragment_length - 1;
        fragment_ad_type = p_data[current_offset++];

        if (data_length == p_target_name->length &&
                (BLE_GAP_AD_TYPE_SHORTENED_NAME == fragment_ad_type ||
                 BLE_GAP_AD_TYPE_COMPLETE_NAME == fragment_ad_type))
        {
            if (0 == memcmp(p_target_name->name, &p_data[current_offset], p_target_name->length))
            {
                return true;
            }

            return false;
        }

        current_offset += data_length;
    }

    return false;
}

bool ble_scan_appearance_filter(const uint8_t *p_data, uint16_t length, uint16_t target_appearance)
{
    if (NULL == p_data)
    {
        return false;
    }

    uint16_t current_offset       = 0;
    uint8_t  fragment_length      = 0;
    uint8_t  fragment_ad_type     = 0;
    uint8_t  data_length = 0;

    while (current_offset < length)
    {
        fragment_length = p_data[current_offset++];

        if (0 == fragment_length)
        {
            break;
        }

        data_length      = fragment_length - 1;
        fragment_ad_type = p_data[current_offset++];

        if (BLE_GAP_AD_TYPE_APPEARANCE == fragment_ad_type)
        {
            if (p_data[current_offset] == target_appearance)
            {
                return true;
            }

            return false;
        }

        current_offset += data_length;
    }

    return false;
}

bool ble_scan_uuid_filter(const uint8_t *p_data, uint16_t length, ble_scan_uuid_filter_t *p_target_srv_uuid)
{
    if (NULL == p_data)
    {
        return false;
    }

    gap_ad_type_t target_ad_type[2];

    switch (p_target_srv_uuid->length)
    {
        case UUID_16_BIT_BYTES:
            target_ad_type[0] = BLE_GAP_AD_TYPE_MORE_16_BIT_UUID;
            target_ad_type[1] = BLE_GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID;
            break;

        case UUID_32_BIT_BYTES:
            target_ad_type[0] = BLE_GAP_AD_TYPE_MORE_32_BIT_UUID;
            target_ad_type[1] = BLE_GAP_AD_TYPE_COMPLETE_LIST_32_BIT_UUID;
            break;

        case UUID_128_BIT_BYTES:
            target_ad_type[0] = BLE_GAP_AD_TYPE_MORE_128_BIT_UUID;
            target_ad_type[1] = BLE_GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID;
            break;

        default:
            return false;
    }

    uint16_t current_offset       = 0;
    uint8_t  fragment_length      = 0;
    uint8_t  fragment_ad_type     = 0;
    uint8_t  data_length = 0;

    while (current_offset < length)
    {
        fragment_length = p_data[current_offset++];

        if (0 == fragment_length)
        {
            break;
        }

        data_length      = fragment_length - 1;
        fragment_ad_type = p_data[current_offset++];

        if (fragment_ad_type == target_ad_type[0] || fragment_ad_type == target_ad_type[1])
        {
            uint8_t count_16_bit_uuid = data_length / p_target_srv_uuid->length;

            for (uint8_t i = 0; i < count_16_bit_uuid; i++)
            {
                if (0 == memcmp(p_target_srv_uuid->uuid, &p_data[current_offset + 16 * i], p_target_srv_uuid->length))
                {
                    return true;
                }
            }

            return false;
        }

        current_offset += data_length;
    }

    return false;
}

bool ble_scan_addr_filter(gap_bdaddr_t *p_src_addr, gap_bdaddr_t *p_target_addr)
{
    if (p_src_addr->addr_type == p_target_addr->addr_type)
    {
        if (0 == memcmp(p_src_addr->gap_addr.addr, p_target_addr->gap_addr.addr, 6))
        {
            return true;
        }
    }

    return false;
}


sdk_err_t   ble_scan_evt_handler_call(ble_scan_evt_t *p_evt)
{
    sdk_err_t   error_code = BLE_SUCCESS;

    switch (p_evt->evt_type)
    {
        case BLE_SCAN_EVT_WHITELIST_REQUEST:
            s_application_handle = true;
            break;

        case BLE_SCAN_EVT_ADV_REPORT:
            if (s_ble_scan_env.scan_init.scan_param.use_whitelist)
            {
                p_evt->evt_type      = BLE_SCAN_EVT_WHITELIST_DEV_FOUND;
                s_application_handle = true;
            }
            else
            {
                ble_scan_on_adv_report(p_evt);
            }
            break;

        case BLE_SACN_EVT_DATA_PARSE_CPLT:
            s_application_handle = true;
            break;

        case BLE_SCAN_EVT_FILTER_MATCH:
            ble_scan_on_filter_match();
            break;

        case BLE_SCAN_EVT_SCAN_TIME_OUT:
            if (!s_ble_scan_env.is_filter_match)
            {
                p_evt->evt_type = BLE_SCAN_EVT_FILTER_NO_MATCH;
            }
            s_application_handle = true;
            break;

        case BLE_SCAN_EVT_SCAN_STOP:
            ble_scan_on_scan_stop(p_evt);
            break;

        case BLE_SCAN_EVT_CONNECTED:
            s_application_handle = true;
            break;

        default:
            break;
    }

    if (s_ble_scan_env.scan_init.evt_handler && s_application_handle)
    {
        s_ble_scan_env.scan_init.evt_handler(p_evt);
        s_application_handle = false;
    }

    memset(&s_ble_scan_evt, 0, sizeof(ble_scan_evt_t));

    return error_code;
}



