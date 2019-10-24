/**
 *****************************************************************************************
 *
 * @file wss.c
 *
 * @brief The implementation of Weight Scale Service.
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
#include "wss.h"
#include "ble_prf_types.h"
#include "ble_prf_utils.h"
#include "utility.h"

/*
 * DEFINES
 *****************************************************************************************
 */
/** Reference Value Fields of Weight Measurement in SIG Assigned Numbers */
#define WSS_MEAS_VAL_MAX_LEN 15

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/** Attributes index in ATT DB */
enum wss_attr_idx_tag
{
    WSS_IDX_SVC,

    WSS_IDX_WEIGHT_SCALE_FEATURE_CHAR,
    WSS_IDX_WEIGHT_SCALE_FEATURE_VAL,

    WSS_IDX_WEIGHT_MEASUREMENT_CHAR,
    WSS_IDX_WEIGHT_MEASUREMENT_VAL,
    WSS_IDX_WEIGHT_MEASUREMENT_CCCD,

    WSS_IDX_NB,
};

enum wss_weight_measurement_flags
{
    WSS_WM_FLAG_UNIT_SI            = 0x00,
    WSS_WM_FLAG_UNIT_IMPERIAL      = 0x01,
    WSS_WM_FLAG_DATE_TIME_PRESENT  = 0x02,
    WSS_WM_FLAG_USER_ID_PRESENT    = 0x04,
    WSS_WM_FLAG_BMI_HEIGHT_PRESENT = 0x08,
};

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
/** Pointer to Weight Scale Service environment variable. */
static wss_env_t *s_p_env;

static uint8_t attrs_mask = 0x37;
                                /**< Attributes to be added into ATT database.
                                    * bit0 - Weight Scale Service Declaration.
                                    * bit1 - Weight Scale Feature declaration.
                                    * bit2 - Weight Scale Feature value.
                                    * bit3 - Weight Measurement declaration.
                                    * bit4 - Weight Measurement value.
                                    * bit5 - Weight Measurement CCCD
                                    */

/**@brief Full WSS attributes descriptor which is used to add attributes into
 *        the ATT database.
 */ 
static const attm_desc_t wss_attr_tab[WSS_IDX_NB] =
{
    // Weight Scale Service Declaration
    [WSS_IDX_SVC] = {BLE_ATT_DECL_PRIMARY_SERVICE, READ_PERM_UNSEC, 0, 0},

    // Weight Scale Feature Characteristic Declaration
    [WSS_IDX_WEIGHT_SCALE_FEATURE_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                           READ_PERM_UNSEC, 0, 0},
    // Weight Scale Feature Characteristic Value
    [WSS_IDX_WEIGHT_SCALE_FEATURE_VAL]  = {BLE_ATT_CHAR_WEIGHT_SCALE_FEATURE,
                                           READ_PERM(AUTH),
                                           0, sizeof(uint32_t)},

    [WSS_IDX_WEIGHT_MEASUREMENT_CHAR] = {BLE_ATT_DECL_CHARACTERISTIC,
                                         READ_PERM_UNSEC, 0, 0},
    [WSS_IDX_WEIGHT_MEASUREMENT_VAL]  = {BLE_ATT_CHAR_WEIGHT_MEASUREMENT,
                                         INDICATE_PERM(AUTH),
                                         0, WSS_MEAS_VAL_MAX_LEN},
    [WSS_IDX_WEIGHT_MEASUREMENT_CCCD] = {BLE_ATT_DESC_CLIENT_CHAR_CFG,
                                         READ_PERM(AUTH) | WRITE_REQ_PERM(AUTH),
                                         0, 0},
};

/*
 * LOCAL FUNCTION DECLARATIONS
 *****************************************************************************************
 */
static sdk_err_t   init_wss(void);
static void on_connect_wss(uint8_t conidx);
static sdk_err_t   write_att_cb_wss(uint8_t conidx,
                                    const gatts_write_req_cb_t *p_param);
static sdk_err_t   read_att_cb_wss(uint8_t conidx,
                                   const gatts_read_req_cb_t *p_param);
static uint16_t measurement_encode_wss(const wss_meas_t *p_meas,
                                       uint8_t *p_encoded_buf);

/**@brief Callbacks required by profile manager. */
static ble_prf_manager_cbs_t wss_mgr_cbs = {
    init_wss,
    on_connect_wss,
    NULL
};

/**@brief Callbacks for GATT server. */
static gatts_prf_cbs_t wss_cb_func = {
    read_att_cb_wss,
    write_att_cb_wss,
    NULL,
    NULL
};

/**@brief Information for registering WS service. */
static const prf_server_info_t wss_prf_info = {
    /* There shall be only one instance on a device */
    .max_connection_nb = 1,
    .manager_cbs       = &wss_mgr_cbs,
    .gatts_prf_cbs      = &wss_cb_func
};

/**@brief Init WS service and create db in att.
 *
 * @return BLE_ATT_ERR_NO_ERROR on success, otherwise error code.
 */
static sdk_err_t   init_wss(void)
{
    sdk_err_t   status = SDK_SUCCESS;
    const uint8_t wss_svc_uuid[] = BLE_ATT_16_TO_16_ARRAY(BLE_ATT_SVC_WEIGHT_SCALE);
    gatts_create_db_t gatts_db;
    uint16_t start_hdl = PRF_INVALID_HANDLE; /* The start hanlde is an in/out
                                              * parameter of ble_gatts_srvc_db_create().
                                              * It must be set with PRF_INVALID_HANDLE
                                              * to be allocated automatically by BLE Stack.*/
    
    memset(&gatts_db, 0, sizeof(gatts_db));

    gatts_db.shdl                 = &start_hdl;
    gatts_db.uuid                 = (uint8_t*)wss_svc_uuid;
    gatts_db.attr_tab_cfg         = &attrs_mask;
    gatts_db.max_nb_attr          = WSS_IDX_NB;
    gatts_db.srvc_perm            = 0;
    gatts_db.attr_tab_type        = SERVICE_TABLE_TYPE_16;
    gatts_db.attr_tab.attr_tab_16 = wss_attr_tab;
    
    status = ble_gatts_srvc_db_create(&gatts_db);
    if (SDK_SUCCESS == status) {
        s_p_env->start_hdl = *gatts_db.shdl;

        uint16_t handle = prf_find_handle_by_idx(WSS_IDX_WEIGHT_SCALE_FEATURE_VAL,
                                                s_p_env->start_hdl,
                                                &attrs_mask);
        APP_PRINTF("Handle of WSS Feature Value: %d\n", handle);
        /* make sure the size of features is 32bits. */
        ble_gatts_value_set(handle, sizeof(uint32_t), 0,
                            (const uint8_t *)&s_p_env->wss_features);
    }

    return status;
}

/**@brief Handle the connect event.
 *
 * @param[in] conidx Connection index to send indication to.
 */
static void on_connect_wss(uint8_t conidx)
{
    if (s_p_env->evt_handler) {
        uint16_t cccd = s_p_env->meas_cccd[conidx];
        wss_evt_t evt;

        evt.evt_type       = WSS_EVT_INDICATION_ENABLED;
        evt.params.enabled = prf_is_indication_enabled(cccd);
        /* Inform Application the status of indication once connected */
        s_p_env->evt_handler(&evt);
    }
}

/**@brief Handles reception of the write request.
 *
 * @param[in] conidx  Connection index to send indication to.
 * @param[in] p_param Pointer to the parameters of the write request.
 *
 * @return If the request was consumed or not.
 */
static sdk_err_t   write_att_cb_wss(uint8_t conidx,
                                    const gatts_write_req_cb_t *p_param)
{
    uint16_t handle = prf_find_handle_by_idx(WSS_IDX_WEIGHT_MEASUREMENT_CCCD,
                                             s_p_env->start_hdl, &attrs_mask);
    gatts_write_cfm_t cfm;

    cfm.handle = p_param->handle;

    if (handle != p_param->handle) {
        cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
    } else {
        if (p_param->length != sizeof(uint16_t)) {
            cfm.status = BLE_ATT_INVALID_ATTRIBUTE_VAL_LEN;
        } else {
            uint16_t cccd = le16toh(&p_param->value[0]);

            s_p_env->meas_cccd[conidx] = cccd;

            if (s_p_env->evt_handler != NULL) {
                wss_evt_t evt;

                evt.evt_type       = WSS_EVT_INDICATION_ENABLED;
                evt.params.enabled = prf_is_indication_enabled(cccd);
                /* Inform Application the status of indication */
                s_p_env->evt_handler(&evt);
            }

            cfm.status = BLE_SUCCESS;
        }
    }

    return ble_gatts_write_cfm(conidx, &cfm);
}

/**@brief Handles reception of the read request.
 *
 * @param[in] conidx  Connection index to send indication to.
 * @param[in] p_param Pointer to the parameters of the read request.
 *
 * @return If the request was consumed or not.
 */
static sdk_err_t   read_att_cb_wss(uint8_t conidx,
                                   const gatts_read_req_cb_t *p_param)
{
    uint16_t cccd_hdl = prf_find_handle_by_idx(WSS_IDX_WEIGHT_MEASUREMENT_CCCD,
                                              s_p_env->start_hdl, &attrs_mask);
    gatts_read_cfm_t cfm;

    cfm.handle = p_param->handle;

    if (cccd_hdl != p_param->handle) {
        cfm.length = 0;
        cfm.status = BLE_ATT_ERR_INVALID_HANDLE;
    } else {
        cfm.length = sizeof(uint16_t);
        cfm.value  = (uint8_t *)(&s_p_env->meas_cccd[conidx]);
        cfm.status = BLE_SUCCESS;
    }

    return ble_gatts_read_cfm(conidx, &cfm);
}

/**@brief Encode a Weight Measurement.
 *
 * @param[in]  p_meas        Pointer to the measurement to be encoded.
 * @param[out] p_encoded_buf Pointer to the buffer where the encoded data will
 *                           be written.
 *
 * @return Length of encoded data.
 */
static uint16_t measurement_encode_wss(const wss_meas_t *p_meas,
                                       uint8_t *p_encoded_buf)
{
    uint8_t *p_flags = p_encoded_buf;
    /** The first octet is reserved for flags. */
    uint8_t *p_field = p_encoded_buf + 1;

    *p_flags = (p_meas->unit == WSS_UNIT_SI) ?
               WSS_WM_FLAG_UNIT_SI : WSS_WM_FLAG_UNIT_IMPERIAL;

    *p_field++ = LO_U16(p_meas->weight);
    *p_field++ = HI_U16(p_meas->weight);

    /** The following are optional fields. */
    if ((s_p_env->wss_features & WSS_FEAT_TIME_STAMP_SUPPORTED) &&
        p_meas->timestamp_present) {
        *p_flags |= WSS_WM_FLAG_DATE_TIME_PRESENT;
        p_field += prf_pack_date_time(p_field, &p_meas->timestamp);
    }

    if (s_p_env->wss_features & WSS_FEAT_MULTI_USER_SUPPORTED) {
        *p_flags |= WSS_WM_FLAG_USER_ID_PRESENT;
        *p_field++ = p_meas->user_id;
    }

    if ((s_p_env->wss_features & WSS_FEAT_BMI_SUPPORTED) &&
        p_meas->bmi > 0 && p_meas->height > 0) {
        *p_flags |= WSS_WM_FLAG_BMI_HEIGHT_PRESENT;
        *p_field++ = LO_U16(p_meas->bmi);
        *p_field++ = HI_U16(p_meas->bmi);

        *p_field++ = LO_U16(p_meas->height);
        *p_field++ = HI_U16(p_meas->height);

    }

    return (p_field - p_encoded_buf);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
void wss_add_service(wss_env_t *p_wss_env)
{
    s_p_env = p_wss_env;
    ble_server_prf_add(&wss_prf_info);
}

sdk_err_t   wss_measurement_send(uint8_t conidx,
                                 const wss_meas_t *p_meas)
{
    if (NULL == s_p_env) {
        return SDK_ERR_INVALID_PARAM;
    }

    if (!prf_is_indication_enabled(s_p_env->meas_cccd[conidx])) {
        return BLE_PRF_ERR_IND_DISABLED;
    }

    uint8_t value[WSS_MEAS_VAL_MAX_LEN];
    uint16_t len = measurement_encode_wss(p_meas, value);
    gatts_noti_ind_t indi;

    indi.type   = GATTS_INDICATION;
    indi.handle = prf_find_handle_by_idx(WSS_IDX_WEIGHT_MEASUREMENT_VAL,
                                         s_p_env->start_hdl,
                                         (uint8_t*)&attrs_mask);
    indi.length = len;
    indi.value  = value;

    return ble_gatts_noti_ind(conidx, &indi);
}

bool wss_is_indication_enabled(uint8_t conidx)
{
    if (NULL == s_p_env) {
        return false;
    }

    return prf_is_indication_enabled(s_p_env->meas_cccd[conidx]);
}

