/**
****************************************************************************************
*
* @file es_adv.h
*
* @brief Advertising start and stop api declare.
*
****************************************************************************************
*/
#ifndef ES_ADV_H__
#define ES_ADV_H__
#include "es_app_config.h"
#include "es_utility.h"
#include "user_app.h"
#include "es.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 *****************************************************************************************
 * @brief find valid slot data in nvds and and advertise use data found
 *
 * @param[in]   is_connectable: true if device is connectable
 *
 * @retval ::BLE_SDK_SUCCESS: Operation is Success.
 * @retval ::BLE_SDK_ERR_BAD_PARAM: The parameter is invalid.
 * @retval ::BLE_SDK_ERR_BUSY: SDK is busy internally as last operation has not completed yet.
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
sdk_err_t   es_next_adv_start (bool is_connectable);

/**
 *****************************************************************************************
 * @brief stop ble advertising
 *
 * @retval ::BLE_SDK_SUCCESS: Operation is Success.
 * @retval ::BLE_SDK_ERR_BAD_PARAM: The parameter is invalid.
 *
 *****************************************************************************************
 */
sdk_err_t   es_adv_stop(void);
#endif // ES_ADV_H__
