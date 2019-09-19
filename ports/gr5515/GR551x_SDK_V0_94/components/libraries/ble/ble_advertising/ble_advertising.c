#include "stdlib.h"
#include "ble_advertising.h"

static ble_advertising_t s_advertising;

static uint16_t ble_advertising_start_really(uint8_t adv_idx, adv_param_t *adv_param, adv_data_t *adv_data, privacy_param_t *privacy_param);

//This callback function will be called when has received the scan request.
void ble_advertising_scan_req_ind_cb(uint8_t inst_idx, const gap_bdaddr_t *scanner_addr)
{
    ble_adv_evt_t adv_evt = BLE_ADV_EVT_SCAN_REQ;
    gap_scan_request_ind_t scan_request =
    {
        .inst_idx = inst_idx,
        .scanner_addr = scanner_addr,
    };

    if (NULL != s_advertising.evt_handler)
    {
        s_advertising.evt_handler(adv_evt, &scan_request);
    }
}

//This callback function will be called when the adv has stopped, but the activity is existent.
void ble_advertising_adv_stopped_cb(uint8_t inst_idx, const gap_stopped_reason_t *param)
{
    ble_adv_evt_t adv_evt = BLE_ADV_EVT_STOPPED;
    gap_adv_stopped_t adv_stopped =
    {
        .inst_idx = inst_idx,
        .time_out_flag = false,
        .status = 0,
    };

    // If advertising timeout, notify user.
    if (GAP_STOPPED_REASON_TIMEOUT == param->reason && NULL != s_advertising.evt_handler)
    {
        adv_stopped.time_out_flag = true;
        s_advertising.evt_handler(adv_evt, &adv_stopped);
    }
}

//This callback function will be called when the adv operation has been completed.
// If status is BLE_GAP_ERR_NO_ERROR, the activity is created, otherwise the activity is not created.
void app_gap_cmp_adv_started_handle(uint8_t conidx, uint16_t status)
{
    ble_adv_evt_t adv_evt = BLE_ADV_EVT_STARTED;
    gap_adv_started_t adv_started =
    {
        .inst_idx = conidx,
        .status = status,
    };

    //Advertising started, clear restart_flag.
    s_advertising.restart_flag = false;

    //Advertising started, set current existent advertisings.
    if (BLE_SUCCESS == status)
    {
        s_advertising.current_adv_idx_array |= (1 << conidx);
    }

    //Notify user.
    if (NULL != s_advertising.evt_handler)
    {
        s_advertising.evt_handler(adv_evt, &adv_started);
    }
}

//This callback function will be called when the adv has stopped, the activity is inexistent.
void app_gap_cmp_adv_stopped_handle(uint8_t conidx, uint16_t status)
{
    ble_adv_evt_t adv_evt = BLE_ADV_EVT_STOPPED;
    gap_adv_stopped_t adv_stopped =
    {
        .inst_idx = conidx,
        .time_out_flag = false,
        .status = status,
    };

    //Advertising stopped, set current existent advertisings.
    if (BLE_SUCCESS == status)
    {
        s_advertising.current_adv_idx_array &= ~(1 << conidx);
    }

    //If something wrong, notify user(regardless of the value of m_advertising.restart_flag);
    //If no wrong, and will not restart advertising,notify user.
    if (BLE_SUCCESS != status || false == s_advertising.restart_flag)
    {
        //clear restart_flag.
        s_advertising.restart_flag = false;
        if (NULL != s_advertising.evt_handler)
        {
            s_advertising.evt_handler(adv_evt, &adv_stopped);
        }
    }

    //If no wrong, and will restart advertising.
    if (BLE_SUCCESS == status && true == s_advertising.restart_flag)
    {
        //Function ble_advertising_start doesn't ensures the return of function ble_advertising_start_really is NO ERROE.
        if (BLE_SUCCESS != ble_advertising_start_really(conidx, &s_advertising.adv_param_saved, s_advertising.adv_data_saved, s_advertising.privacy_param_saved))
        {
            APP_PRINTF("Failed to restart advertising!!!\r\n");
        }

        free(s_advertising.info_saved.adv_data.p_data);
        free(s_advertising.info_saved.adv_data.p_sr_data);
        free(s_advertising.info_saved.adv_data.p_per_data);
    }
}

//This callback function will be called when connection completed
void ble_advertising_on_connected(gap_on_connected_t *on_connected)
{
    ble_adv_evt_t adv_evt = BLE_ADV_EVT_ON_CONNECTED;

    //Notify user.
    if (NULL != s_advertising.evt_handler)
    {
        s_advertising.evt_handler(adv_evt, on_connected);
    }
}

//This callback function will be called when disconnection completed
void ble_advertising_on_disconnected(gap_on_disconnected_t *on_disconnected)
{
    ble_adv_evt_t adv_evt = BLE_ADV_EVT_ON_DISCONNECTED;

    //Notify user.
    if (NULL != s_advertising.evt_handler)
    {
        s_advertising.evt_handler(adv_evt, on_disconnected);
    }
}

//Start an advertising really.
static uint16_t ble_advertising_start_really(uint8_t adv_idx, adv_param_t *adv_param, adv_data_t *adv_data, privacy_param_t *privacy_param)
{
    uint16_t status = BLE_SUCCESS;
    if (NULL == adv_param || NULL == adv_param->info.adv_param || NULL == adv_param->time_param)
    {
        return BLE_ERROR_NULL;
    }
    else
    {
        //Start leagcy/extend/periodic advertising.
        if (true == adv_param->leagcy_adv_flag)
        {
            status = ble_gap_adv_param_set(adv_idx, adv_param->own_addr_type, adv_param->info.adv_param);
        }
        else
        {
            status = ble_gap_ext_adv_param_set(adv_idx, adv_param->own_addr_type, adv_param->info.ext_adv_param);
        }

        if (BLE_SUCCESS != status)
        {
            return status;
        }
    }

    //Set advertising data.
    if (NULL != adv_data && NULL != adv_data->p_data && 0 < adv_data->dlen)
    {
        status = ble_gap_adv_data_set(adv_idx, BLE_GAP_ADV_DATA_TYPE_DATA, adv_data->p_data, adv_data->dlen);
        if (BLE_SUCCESS != status)
        {
            return status;
        }
    }

    //Set scan response data.
    if (NULL != adv_data && NULL != adv_data->p_sr_data  && 0 < adv_data->srdlen)
    {
        status = ble_gap_adv_data_set(adv_idx, BLE_GAP_ADV_DATA_TYPE_SCAN_RSP, adv_data->p_sr_data, adv_data->srdlen);
        if (BLE_SUCCESS != status)
        {
            return status;
        }
    }

    //Set periodic advertising data.
    if (NULL != adv_data && NULL != adv_data->p_per_data  && 0 < adv_data->perdlen)
    {
        status = ble_gap_adv_data_set(adv_idx, BLE_GAP_ADV_DATA_TYPE_PER_DATA, adv_data->p_per_data, adv_data->perdlen);
        if (BLE_SUCCESS != status)
        {
            return status;
        }
    }

    //Set privacy advertising.
    if (NULL != privacy_param)
    {
        status = ble_gap_privacy_params_set(privacy_param->renew_dur, privacy_param->enable_flag);
        if (BLE_SUCCESS != status)
        {
            return status;
        }
    }

    status = ble_gap_adv_start(adv_idx, adv_param->time_param);

    return status;
}

/*This function to stop an advertising.
  If the advertising activity(adv_idx related to) is existent: stop the advertising activity.
  If the advertising activity(adv_idx related to) is inexistent: return BLE_ERROR_INVALID_IDX.
*/
uint16_t ble_advertising_stop(uint8_t adv_idx)
{
    uint16_t status = BLE_SUCCESS;
    bool adv_idx_valid = (s_advertising.current_adv_idx_array & (1 << adv_idx)) ? true : false;

    if (false == adv_idx_valid)
    {
        return BLE_ERROR_INVALID_IDX;
    }
    status = ble_gap_adv_stop(adv_idx);

    return status;
}

/*This function to start an advertising.
  If the advertising activity(adv_idx related to) is existent: 1) stop the advertising activity, 2) start the advertising.
    (note: if the advertising configuration is wrong, the second step will make an error(in function app_gap_cmp_adv_stopped_handle).)
  If the advertising activity(adv_idx related to) is inexistent: start the advertising.
*/
uint16_t ble_advertising_start(uint8_t adv_idx, adv_param_t *adv_param, adv_data_t *adv_data, privacy_param_t *privacy_param)
{
    uint16_t status = BLE_SUCCESS;
    bool adv_idx_valid = (s_advertising.current_adv_idx_array & (1 << adv_idx)) ? true : false;

    if (false == adv_idx_valid)
    {
        return ble_advertising_start_really(adv_idx, adv_param, adv_data, privacy_param);
    }
    else
    {
        if (NULL == adv_param || NULL == adv_param->info.adv_param || NULL == adv_param->time_param)
        {
            return BLE_ERROR_NULL;
        }

        //save adv_param
        s_advertising.adv_param_saved = *adv_param;
        if (true == adv_param->leagcy_adv_flag)
        {
            s_advertising.info_saved.info.adv_param = *(adv_param->info.adv_param);
            s_advertising.adv_param_saved.info.adv_param = &s_advertising.info_saved.info.adv_param;
        }
        else
        {
            s_advertising.info_saved.info.ext_adv_param = *(adv_param->info.ext_adv_param);
            s_advertising.adv_param_saved.info.ext_adv_param = &s_advertising.info_saved.info.ext_adv_param;
        }
        s_advertising.info_saved.time_param = *(adv_param->time_param);
        s_advertising.adv_param_saved.time_param = &s_advertising.info_saved.time_param;

        //save adv_data
        s_advertising.info_saved.adv_data.p_data = NULL;
        s_advertising.info_saved.adv_data.p_sr_data = NULL;
        s_advertising.info_saved.adv_data.p_per_data = NULL;
        s_advertising.adv_data_saved = NULL;
        if (NULL != adv_data )
        {
            s_advertising.adv_data_saved = (adv_data_t *)&s_advertising.info_saved.adv_data;

            if (NULL != adv_data->p_data  && 0 < adv_data->dlen)
            {
                s_advertising.info_saved.adv_data.dlen = adv_data->dlen;
                s_advertising.info_saved.adv_data.p_data = (uint8_t *)malloc(adv_data->dlen * sizeof(uint8_t));
                memcpy(s_advertising.info_saved.adv_data.p_data, adv_data->p_data, adv_data->dlen);
            }

            if (NULL != adv_data->p_sr_data && 0 < adv_data->srdlen)
            {
                s_advertising.info_saved.adv_data.srdlen = adv_data->srdlen;
                s_advertising.info_saved.adv_data.p_sr_data = (uint8_t *)malloc(adv_data->srdlen * sizeof(uint8_t));
                memcpy(s_advertising.info_saved.adv_data.p_sr_data, adv_data->p_sr_data, adv_data->srdlen);
            }

            if (NULL != adv_data->p_per_data && 0 < adv_data->perdlen)
            {
                s_advertising.info_saved.adv_data.perdlen = adv_data->perdlen;
                s_advertising.info_saved.adv_data.p_per_data = (uint8_t *)malloc(adv_data->perdlen * sizeof(uint8_t));
                memcpy(s_advertising.info_saved.adv_data.p_per_data, adv_data->p_per_data, adv_data->perdlen);
            }
        }

        //save privacy_param
        s_advertising.privacy_param_saved = NULL;
        if (NULL != privacy_param)
        {
            s_advertising.privacy_param_saved = &s_advertising.info_saved.privacy_param;
            s_advertising.info_saved.privacy_param = *privacy_param;
        }

        status = ble_gap_adv_stop(adv_idx);
        if (BLE_SUCCESS == status)
        {
            s_advertising.restart_flag = true;
        }
    }
    return status;
}

/*Register BLE advertising event handler.*/
void ble_advertising_evt_handler(ble_adv_evt_handler_t evt_handler)
{
    s_advertising.evt_handler = evt_handler;
}

