/**
 ****************************************************************************************
 *
 * @file user_l2cap_callback.c
 *
 * @brief  BLE L2CAP Callback Function Implementation.
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
 *****************************************************************************************
 */
static void app_l2cap_lecb_conn_req_cb(uint8_t conn_idx, lecb_conn_req_ind_t *p_conn_req);
static void app_l2cap_lecb_conn_ind_cb(uint8_t conn_idx, lecb_conn_ind_t *p_conn_ind);
static void app_l2cap_lecb_add_credits_ind_cb(uint8_t conn_idx, lecb_add_credits_ind_t *p_add_credits_ind);
static void app_l2cap_lecb_disconn_ind_cb(uint8_t conn_idx, lecb_disconn_ind_t *p_disconn_ind);
static void app_l2cap_lecb_sdu_ind_cb(uint8_t conn_idx, lecb_sdu_t *p_sdu);
static void app_l2cap_op_cmp_evt_cb (uint8_t conn_idx, lecb_op_cmp_evt_t *p_evt);

/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
const l2cap_lecb_cb_fun_t ble_l2cap_callback =
{
    .app_l2cap_lecb_conn_req_cb        = app_l2cap_lecb_conn_req_cb,
    .app_l2cap_lecb_conn_ind_cb        = app_l2cap_lecb_conn_ind_cb,
    .app_l2cap_lecb_add_credits_ind_cb = app_l2cap_lecb_add_credits_ind_cb,
    .app_l2cap_lecb_disconn_ind_cb     = app_l2cap_lecb_disconn_ind_cb,
    .app_l2cap_lecb_sdu_ind_cb         = app_l2cap_lecb_sdu_ind_cb,
    .app_l2cap_op_cmp_evt_cb           = app_l2cap_op_cmp_evt_cb
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving lecb connection request.
 *
 * @param[in] conn_idx:   The connection index.
 * @param[in] p_conn_req: The information of LE credit based connection request. See @ref lecb_conn_req_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_conn_req_cb(uint8_t conn_idx, lecb_conn_req_ind_t *p_conn_req)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving lecb created indication.
 *
 * @param[in] conn_idx:          The connection index.
 * @param[in] p_conn_ind: The information LE credit based connection created indication. See @ref lecb_conn_ind_t.
 *****************************************************************************************
 */
 static void app_l2cap_lecb_conn_ind_cb(uint8_t conn_idx, lecb_conn_ind_t *p_conn_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving lecb connection addition indication.
 *
 * @param[in] conn_idx:          The connection index.
 * @param[in] p_add_credits_ind: The information of LE credit based connection addition indication. See @ref lecb_add_credits_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_add_credits_ind_cb(uint8_t conn_idx, lecb_add_credits_ind_t *p_add_credits_ind)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving LE Credit Based disconnection indication.
 *
 * @param[in] conn_idx:      The connection index.
 * @param[in] p_disconn_ind: The information of LE credit based disconnect indication. See @ref lecb_disconn_ind_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_disconn_ind_cb(uint8_t conn_idx, lecb_disconn_ind_t *p_disconn_ind)
{
    
}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving SDU packet.
 *
 * @param[in] conn_idx: The connection index.
 * @param[in] p_sdu:    SDU packet parameter. See @ref lecb_sdu_t.
 *****************************************************************************************
 */
static void app_l2cap_lecb_sdu_ind_cb(uint8_t conn_idx, lecb_sdu_t *p_sdu)
{

}

/**
 *****************************************************************************************
 * @brief This callback function will be called when receiving l2cap operation complete event.
 *
 * @param[in] conn_idx:        The connection index.
 * @param[in] p_l2cap_op_info: The information of the l2cap operation. @see gap_opera_cmp_evt_t.
 *****************************************************************************************
 */
static void app_l2cap_op_cmp_evt_cb(uint8_t conn_idx, lecb_op_cmp_evt_t *p_l2cap_op_info)
{

}
