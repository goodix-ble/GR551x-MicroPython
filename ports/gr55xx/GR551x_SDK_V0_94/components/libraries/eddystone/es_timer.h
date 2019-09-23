/**
****************************************************************************************
*
* @file es_timer.h
*
* @brief Defines for timer and timer controlled state machine function
*
****************************************************************************************
*/
#ifndef ES_TIMER_H__
#define ES_TIMER_H__
#include "es_app_config.h"
#include "es_utility.h"
#include "es_adv.h"
#include "gr55xx_delay.h"
#include "es_nvds.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/**@brief state machine type for timer controlling */
typedef enum
{
    RADIO_POWERON_ADV = 1, 
    RADIO_NORMAL_ADV,
    RADIO_CONNECTED,
} es_radio_status_t;

/**
 *****************************************************************************************
 * @brief get counter number for tlm
 *
 * @return      counter number
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
uint32_t es_tlm_sec_cnt_get(void);

/**
 *****************************************************************************************
 * @brief just for debugging,will be removed later
 *
 *****************************************************************************************
 */
void additional_poweon_delay_for_debugging(void);

/**
 *****************************************************************************************
 * @brief start advertising timer
 *
 * @param[in]   delay: timer period(mS)
 *
 * @return      BLE_SDK_SUCCESS if successful
 *
 *****************************************************************************************
 */
sdk_err_t   es_adv_timing_timers_start(uint32_t delay);

/**
 *****************************************************************************************
 * @brief stop connectable control timer
 *
 *****************************************************************************************
 */
void stop_timer_for_connectable_state(void);

/**
 *****************************************************************************************
 * @brief stop connectable control timer
 *
 *****************************************************************************************
 */
void start_timer_for_connectable_state(void);

/**
 *****************************************************************************************
 * @brief set timer controlled machine state
 *
 * @param[in]   new_status: new state
 *
 *****************************************************************************************
 */
void es_radio_adv_status_set(es_radio_status_t new_status);

/**
 *****************************************************************************************
 * @brief get timer controlled machine state
 *
 * @return      curent state
 *
 *****************************************************************************************
 */
es_radio_status_t es_radio_adv_status_get(void);
#endif // ES_TIMER_H__
