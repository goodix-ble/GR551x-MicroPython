/**
 *****************************************************************************************
 *
 * @file user_gap_callback.c
 *
 * @brief  BLE GAP Callback Function Implementation.
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
 *****************************************************************************************
 */
#ifdef BLE_ADVERTISING_MODULE_ENABLE
#include "ble_advertising.h"
#endif

#ifdef BLE_SCAN_MODULE_ENABLE
#include "ble_scan.h"
#endif

#include "gr55xx_sys.h"
#include "common.h"

#ifdef BLE_ADVERTISING_MODULE_ENABLE
typedef void (*gap_cmd_cpl_handle_fuc_t)(uint8_t conidx, uint16_t status);

typedef struct
{
    gap_op_id_t op_id;
    gap_cmd_cpl_handle_fuc_t fun;
} gap_cmd_cpl_handle_des_t;

extern void app_gap_cmp_adv_started_handle(uint8_t conidx, uint16_t status);
extern void app_gap_cmp_adv_stopped_handle(uint8_t conidx, uint16_t status);
//extern void app_scan_started_handle(uint8_t conidx, uint16_t status);
//extern void app_stop_scan_activity_handle(uint8_t conidx, uint16_t status);
//extern void app_disconnect_handle(uint8_t conidx, uint16_t status);
//extern void app_update_connection_param_handle(uint8_t conidx, uint16_t status);
//extern void app_set_pack_size_handle(uint8_t conidx, uint16_t status);

extern void ble_advertising_on_connected(gap_on_connected_t *on_connected);
extern void ble_advertising_on_disconnected(gap_on_disconnected_t *on_disconnected);
#endif
//static gap_stopped_reason_t m_adv_stopped_reason;

/*
* LOCAL FUNCTION DECLARATION
****************************************************************************************
*/
static void app_gap_op_cmp_evt_cb(uint8_t conn_idx, const gap_op_cmp_evt_t *p_gap_op_info);
static void app_gap_get_dev_params_cb(const gap_get_dev_params_t *p_dev_info);
static void app_gap_scan_req_ind_cb(uint8_t inst_idx, const gap_bdaddr_t *p_scanner_addr);
static void app_gap_adv_stopped_cb(uint8_t inst_idx, const gap_stopped_reason_t adv_stop_reason);
static void app_gap_adv_report_ind_cb(const gap_ext_adv_report_ind_t *p_adv_report);
static void app_gap_scan_stopped_cb(const gap_scan_stopped_ind_t *p_scan_stop_reason);
static void app_gap_sync_established_cb(const gap_sync_established_ind_t *p_sync_established_info);
static void app_gap_connection_complete_cb(uint8_t conn_idx, const gap_conn_cmp_t *p_conn_param);
static void app_gap_peer_name_ind_cb(uint8_t conn_idx, const gap_peer_name_ind_t *p_peer_name);
static void app_gap_disconnect_complete_cb(uint8_t conn_idx, const uint8_t disconnect_reason);
static void app_gap_connection_update_complete_cb(uint8_t conn_idx, const gap_conn_update_cmp_t *p_conn_param_update_info);
static void app_gap_connection_update_req_cb(uint8_t conn_idx, const gap_conn_param_t *p_conn_param_update_req);
static void app_gap_get_connection_info_cb(uint8_t conn_idx, const gap_conn_info_param_t *p_conn_info);
static void app_gap_get_peer_info_cb(uint8_t conn_idx, const gap_peer_info_param_t *p_peer_dev_info);
static void app_gap_le_pkt_size_info_cb(uint8_t conn_idx, const gap_le_pkt_size_ind_t *p_supported_data_length_size);

#ifdef BLE_ADVERTISING_MODULE_ENABLE
static gap_cmd_cpl_handle_des_t gap_cmd_cpl_hadlel_tab[] =
{
    {GAP_NO_OP,                          NULL},
    {GAP_OPCODE_SET_CHNL_MAP,            NULL},
    {GAP_OPCODE_SET_WHITELIST,           NULL},
    {GAP_OPCODE_SET_PER_ADV_LIST,        NULL},
    {GAP_OPCODE_GET_DEVICE_PARAMS_REQ,   NULL},
    {GAP_OPCODE_START_ADV,               app_gap_cmp_adv_started_handle},
    {GAP_OPCODE_STOP_ADV,                app_gap_cmp_adv_stopped_handle},
    {GAP_OPCODE_UPDATE_ADV_DATA,         NULL},
    {GAP_OPCODE_START_SCAN,              NULL/*app_scan_started_handle*/},
    {GAP_OPCODE_STOP_SCAN,               NULL/*app_stop_scan_activity_handle*/},
    {GAP_OPCODE_START_PER_SYNC,          NULL},
    {GAP_OPCODE_STOP_PER_SYNC,           NULL},
    {GAP_OPCODE_CONNECT,                 NULL/*app_init_created_handle*/},
    {GAP_OPCODE_CANCEL_CONNECTION,       NULL/*app_delete_init_activity_handle*/},
    {GAP_OPCODE_DISCONNECT,              NULL/*app_disconnect_handle*/},
    {GAP_OPCODE_REGISTER_LEPSM,          NULL},
    {GAP_OPCODE_UNREGISTER_LEPSM,        NULL},
    {GAP_OPCODE_UPDATE_CONNECTION_PARAM, NULL/*app_update_connection_param_handle*/},
    {GAP_OPCODE_CONN_PARAM_UPDATE_REPLY, NULL},
    {GAP_OPCODE_DATA_LENGTH_UPDATE,      NULL/*app_set_pack_size_handle*/},
    {GAP_OPCODE_PHY_UPDATE,              NULL},
    {GAP_OPCODE_GET_PEER_INFO_REQ,       NULL},
    {GAP_OPCODE_GET_CONN_INFO_REQ,       NULL},
    {GAP_OPCODE_MAX,                     NULL},
};
#endif
/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const gap_cb_fun_t ble_gap_callbacks =
{
    // -------------------------  Common Callbacks       ---------------------------------
    .app_gap_op_cmp_evt_cb                 = app_gap_op_cmp_evt_cb,
    .app_gap_get_dev_params_cb             = app_gap_get_dev_params_cb,

    // -------------------------  Advertising Callbacks       ----------------------------
    .app_gap_scan_req_ind_cb               = app_gap_scan_req_ind_cb,
    .app_gap_adv_stopped_cb                = app_gap_adv_stopped_cb,

    // --------------------  Scanning/Periodic Synchronization Callbacks  ----------------
    .app_gap_adv_report_ind_cb             = app_gap_adv_report_ind_cb,
    .app_gap_scan_stopped_cb               = app_gap_scan_stopped_cb,
    .app_gap_sync_established_cb           = app_gap_sync_established_cb,

    // -------------------------   Initiating Callbacks   --------------------------------
    .app_gap_connection_complete_cb        = app_gap_connection_complete_cb,
    .app_gap_peer_name_ind_cb              = app_gap_peer_name_ind_cb,

    // -------------------------   Connection Control Callbacks  -------------------------
    .app_gap_disconnect_complete_cb        = app_gap_disconnect_complete_cb,
    .app_gap_connection_update_complete_cb = app_gap_connection_update_complete_cb,
    .app_gap_connection_update_req_cb      = app_gap_connection_update_req_cb,
    .app_gap_get_connection_info_cb        = app_gap_get_connection_info_cb,
    .app_gap_get_peer_info_cb              = app_gap_get_peer_info_cb,
    .app_gap_le_pkt_size_info_cb           = app_gap_le_pkt_size_info_cb,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving gap operation complete event.
 *
 * @param[in] conn_idx:      The connection index for connection control operations or fix number zero for non-connection operations.
 * @param[in] p_gap_op_info: The information of the gap operation. See @ref gap_opera_cmp_evt_t.
 *****************************************************************************************
 */
static void app_gap_op_cmp_evt_cb(uint8_t conn_idx, const gap_op_cmp_evt_t *p_gap_op_info)
{
//    if (GAP_OPCODE_STOP_ADV == param->op_id &&
//        GAP_STOPPED_REASON_TIMEOUT == m_adv_stopped_reason) {
//        APP_PRINTF("Complete opration of Stopping adv for timeout\r\n");
//        slow_advertising_start();
//    }
#ifdef BLE_ADVERTISING_MODULE_ENABLE
    if (NULL != gap_cmd_cpl_hadlel_tab[p_gap_op_info->op_id].fun)
    {
        gap_cmd_cpl_hadlel_tab[p_gap_op_info->op_id].fun(conn_idx, p_gap_op_info->error_code);
    }
#endif

    if (GAP_OPCODE_STOP_SCAN == p_gap_op_info->op_id)
    {
#ifdef BLE_SCAN_MODULE_ENABLE
        ble_scan_evt_t ble_scan_evt =
        {
            .conn_idx   = conn_idx,
            .evt_type   = BLE_SCAN_EVT_SCAN_STOP,
            .error_code = p_gap_op_info->error_code,
        };

        ble_scan_evt_handler_call(&ble_scan_evt);
#endif
    }
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when requested parameters has been got.
 *
 * @param[in] p_dev_info: The device information. See @ref gap_get_dev_params_t.
 *****************************************************************************************
 */
static void app_gap_get_dev_params_cb(const gap_get_dev_params_t *p_dev_info)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving the scan request.
 *
 * @param[in] inst_idx:       The advertising index. valid range is: 0 - 4. See @ref GAP_INVALID_ADV_IDX.
 * @param[in] p_scanner_addr: The BD address of scanner. See @ref gap_bdaddr_t.
 *****************************************************************************************
 */
static void app_gap_scan_req_ind_cb(uint8_t inst_idx, const gap_bdaddr_t *p_scanner_addr)
{
#ifdef BLE_ADVERTISING_MODULE_ENABLE
    ble_advertising_scan_req_ind_cb(inst_idx, p_scanner_addr);
#endif
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when the adv has been stopped.
 *
 * @param[in] inst_idx:          The advertising index. valid range is: 0 - 4. See @ref GAP_INVALID_ADV_IDX.
 * @param[in] p_adv_stop_reason: The reason of advertising stop. See @ref gap_adv_stopped_ind_t.
 *****************************************************************************************
 */
static void app_gap_adv_stopped_cb(uint8_t inst_idx, const gap_stopped_reason_t adv_stop_reason)
{
#ifdef BLE_ADVERTISING_MODULE_ENABLE
    ble_advertising_adv_stopped_cb(inst_idx, p_adv_stop_reason);
#endif
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving advertising report.
 * 
 * @param[in] p_adv_report: The extended advertising report. See @ref gap_ext_adv_report_ind_t.
 *****************************************************************************************
 */
static void app_gap_adv_report_ind_cb(const gap_ext_adv_report_ind_t *p_adv_report)
{
#ifdef BLE_SCAN_MODULE_ENABLE
    ble_scan_evt_t ble_scan_evt =
    { 
        .evt_type     = BLE_SCAN_EVT_ADV_REPORT,
        .p_adv_report = p_adv_report,
    };
    ble_scan_evt_handler_call(&ble_scan_evt);
#endif
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when the scanning activity has been stopped.
 *
 * @param[in] conn_idx:           The connection index.
 * @param[in] p_scan_stop_reason: The reason of scan stop. See @ref gap_scan_stopped_ind_t.
 *****************************************************************************************
 */
static void app_gap_scan_stopped_cb(const gap_scan_stopped_ind_t *p_scan_stop_reason)
{
#ifdef BLE_SCAN_MODULE_ENABLE
    if (GAP_STOPPED_REASON_TIMEOUT == p_scan_stop_reason->reason)
    {
        ble_scan_evt_t ble_scan_evt =
        { 
            .evt_type = BLE_SCAN_EVT_SCAN_TIME_OUT,
        };
        ble_scan_evt_handler_call(&ble_scan_evt);
    }
#endif
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when the periodic advertising synchronization has been established.
 *
 * @param[in] p_sync_established_info: The information of established indication. See @ref gap_sync_established_ind_t.
 *****************************************************************************************
 */
static void app_gap_sync_established_cb(const gap_sync_established_ind_t *p_sync_established_info)
{

}


/**
 *****************************************************************************************
 * @brief This callback function will be called when connection completed.
 *
 * @param[in] conn_idx:     The connection index.
 * @param[in] p_conn_param: The connection parameters. See @ref gap_conn_cmp_t.
 *****************************************************************************************
 */
static void app_gap_connection_complete_cb(uint8_t conn_idx, const gap_conn_cmp_t *p_conn_param)
{
#ifdef BLE_ADVERTISING_MODULE_ENABLE
    gap_on_connected_t on_connected =
    {
        .conidx = conn_idx,
        .param  = p_conn_param,
    };

    ble_advertising_on_connected(&on_connected);
#endif

#ifdef BLE_SCAN_MODULE_ENABLE
    ble_scan_evt_t ble_scan_evt =
    {
        .conn_idx   = conn_idx,
        .evt_type   = BLE_SCAN_EVT_CONNECTED,
    };

    ble_scan_evt_handler_call(&ble_scan_evt);
#endif
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when the peer name info has been got.
 *
 * @param[in] conn_idx:    The connection index.
 * @param[in] p_peer_name: The peer device name indication information. See @ref gap_peer_name_ind_t.
 *****************************************************************************************
 */
static void app_gap_peer_name_ind_cb(uint8_t conn_idx, const gap_peer_name_ind_t *p_peer_name)
{

}


/**
 *****************************************************************************************
 * @brief This callback function will be called when disconnect completed.
 *
 * @param[in] conn_idx:            The connection index.
 * @param[in] p_disconnect_reason: The reason of disconnect. See @ref ble_hci_error_t.
 *****************************************************************************************
 */
static void app_gap_disconnect_complete_cb(uint8_t  conn_idx,  const uint8_t disconnect_reason)
{
#ifdef BLE_ADVERTISING_MODULE_ENABLE
    gap_on_disconnected_t on_disconnected =
    {
        .conidx = conn_idx,
        .param  = p_disconnect_reason,
    };

    ble_advertising_on_disconnected(&on_disconnected);
#endif
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when connection update completed.
 *
 * @param[in] conn_idx:          The connection index.
 * @param[in] p_update_cmp_info: The connection update complete param. See @ref gap_conn_update_cmp_t.
 *****************************************************************************************
 */
static void app_gap_connection_update_complete_cb(uint8_t conn_idx, const gap_conn_update_cmp_t *p_update_cmp_info)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving update connection request from peer device.
 *
 * @param[in] conn_idx:            The connection index.
 * @param[in] p_update_conn_param: The connection update parameters. See @ref gap_conn_param_t.
 *****************************************************************************************
 */
static void app_gap_connection_update_req_cb(uint8_t conn_idx, const gap_conn_param_t *p_update_conn_param)
{
    gap_param_update_reply_t cfm;

    cfm.accept     = true;
    cfm.ce_len_max = 0;
    cfm.ce_len_min = 0;

    ble_gap_conn_param_update_reply(conn_idx, &cfm);
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when the connection information has been got.
 *
 * @param[in] conn_idx:    The connection index.
 * @param[in] p_conn_info: The connection information. See @ref gap_conn_info_param_t.
 *****************************************************************************************
 */
static void app_gap_get_connection_info_cb(uint8_t conn_idx, const gap_conn_info_param_t *p_conn_info)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when the peer information.
 * @param[in] conn_idx:        The connection index.
 * @param[in] p_peer_dev_info: The peer device information. See @ref gap_peer_info_param_t.
 *****************************************************************************************
 */
static void app_gap_get_peer_info_cb(uint8_t conn_idx, const gap_peer_info_param_t *p_peer_dev_info)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when set supported data length.
 * @param[in] conn_idx:       The connection index.
 * @param[in] p_data_sup_ind: Supported data length indication. See @ref gap_le_pkt_size_ind_t.
 *****************************************************************************************
 */
static void app_gap_le_pkt_size_info_cb(uint8_t conn_idx, const gap_le_pkt_size_ind_t *p_data_sup_ind)
{

}
