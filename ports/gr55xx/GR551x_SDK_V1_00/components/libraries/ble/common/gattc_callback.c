/**
 *****************************************************************************************
 *
 * @file user_gattc_callback.c
 *
 * @brief  BLE GATTC Callback Function Implementation.
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
#include "gr55xx_sys.h"

/*
* LOCAL FUNCTION DECLARATION
******************************************************************************************
*/
static void app_gattc_srvc_disc_cb(uint8_t conn_idx, const gattc_disc_srvc_ind_t *p_disc_srvc_ind);
static void app_gattc_inc_srvc_disc_cb(uint8_t conn_idx, const gattc_disc_inc_srvc_ind_t *p_disc_inc_srvc_ind);
static void app_gattc_char_disc_cb(uint8_t conn_idx, const gattc_disc_char_ind_t *p_disc_char_ind);
static void app_gattc_char_desc_disc_cb(uint8_t conn_idx, const gattc_disc_char_desc_ind_t *p_disc_char_desc_ind);
static void app_gattc_read_cb(uint8_t conn_idx, const gattc_read_ind_t *p_read_ind);
static void app_gattc_noti_ind_cb(uint8_t conn_idx, const gattc_noti_ind_t *p_notify_indicate_ind);
static void app_gattc_srvc_browse_cb(uint8_t conn_idx, const gattc_browse_srvc_ind_t *p_browse_srvc_ind);

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const gattc_cb_fun_t ble_gattc_callback =
{
    .app_gattc_srvc_disc_cb      = app_gattc_srvc_disc_cb,
    .app_gattc_inc_srvc_disc_cb  = app_gattc_inc_srvc_disc_cb,
    .app_gattc_char_disc_cb      = app_gattc_char_disc_cb,
    .app_gattc_char_desc_disc_cb = app_gattc_char_desc_disc_cb,
    .app_gattc_read_cb           = app_gattc_read_cb,
    .app_gattc_noti_ind_cb       = app_gattc_noti_ind_cb,
    .app_gattc_srvc_browse_cb    = app_gattc_srvc_browse_cb,
};


/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when primary service has been discovered.
 *
 * @param[in] conn_idx:        The connection index.
 * @param[in] p_disc_srvc_ind: The information of primary service. See @ref gattc_disc_srvc_ind_t.
 *****************************************************************************************
 */
static void app_gattc_srvc_disc_cb(uint8_t conn_idx, const gattc_disc_srvc_ind_t *p_disc_srvc_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when service relationship has been discovered.
 *
 * @param[in] conn_idx:            The connection index.
 * @param[in] p_disc_inc_srvc_ind: The information of service relationship. See @ref gattc_disc_char_ind_t.
 *****************************************************************************************
 */
static void app_gattc_inc_srvc_disc_cb(uint8_t conn_idx, const gattc_disc_inc_srvc_ind_t *p_disc_inc_srvc_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when descriptor(s) has been discovered.
 *
 * @param[in] conn_idx:        The connection index.
 * @param[in] p_disc_char_ind: The information of descriptor(s). See @ref gattc_disc_char_ind_t.
 *****************************************************************************************
 */
static void app_gattc_char_disc_cb(uint8_t conn_idx, const gattc_disc_char_ind_t *p_disc_char_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when characteristic(s) has been discovered.
 *
 * @param[in] conn_idx:             The connection index.
 * @param[in] p_disc_char_desc_ind: The information of characteristic(s). See @ref gattc_disc_char_desc_ind_t.
 *****************************************************************************************
 */
static void app_gattc_char_desc_disc_cb(uint8_t conn_idx, const gattc_disc_char_desc_ind_t *p_disc_char_desc_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving read response.
 *
 * @param[in] conn_idx:   The connection index.
 * @param[in] p_read_ind: The information of read response. See @ref gattc_read_ind_t.
 *****************************************************************************************
 */
static void app_gattc_read_cb(uint8_t conn_idx, const gattc_read_ind_t *p_read_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving notification or indication.
 *
 * @param[in] conn_idx:              The connection index.
 * @param[in] p_notify_indicate_ind: The information of notification or indication. See @ref gattc_noti_ind_t.
 *****************************************************************************************
 */
static void app_gattc_noti_ind_cb(uint8_t conn_idx, const gattc_noti_ind_t *p_notify_indicate_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving browse service indication.
 *
 * @param[in] conn_idx:          The connection index.
 * @param[in] p_browse_srvc_ind: The information of service browse. See @ref gattc_browse_srvc_ind_t.
 *****************************************************************************************
 */
static void app_gattc_srvc_browse_cb(uint8_t conn_idx, const gattc_browse_srvc_ind_t *p_browse_srvc_ind)
{

}
