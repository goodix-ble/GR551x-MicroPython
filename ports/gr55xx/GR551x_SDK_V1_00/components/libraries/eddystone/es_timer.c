/**
*****************************************************************************************
*
* @file es_timer.c
*
* @brief Timer and timer controlled state machine api for Eddystone profile
*
*****************************************************************************************
*/
#include "es_timer.h"
#include "gr55xx_sys.h"

/**
 * Private variables
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
static uint8_t                  s_es_slot_timer_id;
static uint8_t                  s_es_tlm_sec_cnt_timer_id;
static uint8_t                  s_connectable_state_control_timer_id;

static uint32_t                 s_es_poweron_cnt;   /**< counter for tlm time service*/

static volatile es_radio_status_t s_radio_status=RADIO_POWERON_ADV; 

/**
 * Private functions
 *****************************************************************************************
 */   
/**@brief Timeout handler for the advertisement slot timer. */
static void adv_slot_timeout (uint8_t timer_id)
{
    sdk_err_t   error_code;
    
    if(timer_id !=s_es_slot_timer_id)
    {
        return;
    }
    
    if(RADIO_CONNECTED !=(es_radio_adv_status_get()))
    {
        error_code = es_adv_stop();
        ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
        
        CURRENT_FUNC_INFO_PRINT();
    }
}

/**@brief Timeout handler for connectable control timer. */
static void connectable_state_control_timeout_handler (uint8_t timer_id)
{
    sdk_err_t   error_code;
    
    if(timer_id !=s_connectable_state_control_timer_id)
    {
        return;
    }
    
    if(RADIO_CONNECTED !=(es_radio_adv_status_get()))
    {
        error_code = es_adv_stop();
        ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
        
        es_radio_adv_status_set(RADIO_NORMAL_ADV);
    }
    
    error_code =es_adv_timing_timers_start(es_adv_interval_get());
    ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
}

/**@brief Timeout handler for counter timer. */
static void es_tlm_sec_cnt_timeout (uint8_t timer_id)
{
    if(timer_id !=s_es_tlm_sec_cnt_timer_id)
    {
        return;
    }
    
    s_es_poweron_cnt++;
}

/**
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
uint32_t es_tlm_sec_cnt_get (void)
{
    return s_es_poweron_cnt;
}

void additional_poweon_delay_for_debugging (void)
{
    /* delay when power on to avoid unexpected jtag io configuration */
    /* delay when power on to avoid unexpected low power state */
    delay_ms (2000);
}

sdk_err_t   es_adv_timing_timers_start (uint32_t delay_ms)
{
    sdk_err_t   error_code;
    
    s_es_poweron_cnt = 0;
    
    /* wait for 30s for user config when system power on */
    error_code = sys_timer_set (delay_ms/10, true, adv_slot_timeout,
                                &s_es_slot_timer_id);
    RETURN_IF_ERROR (error_code, BLE_SUCCESS);
    
    return error_code;
}

void stop_timer_for_connectable_state (void)
{
    sdk_err_t   error_code;
    
    error_code = sys_timer_clear (s_connectable_state_control_timer_id);
    ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
}

void start_timer_for_connectable_state (void)
{
    sdk_err_t   error_code;
    
    /* wait for 30s for user config when system power on */
    error_code = sys_timer_set (3000, false, connectable_state_control_timeout_handler,
                                &s_connectable_state_control_timer_id);
    ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
    
    /* also start counter timer */
    error_code = sys_timer_set (100, true, es_tlm_sec_cnt_timeout,
                                &s_es_tlm_sec_cnt_timer_id);
    ERROR_CODE_PRINT (error_code, BLE_SUCCESS);
}

void es_radio_adv_status_set(es_radio_status_t new_status)
{
    s_radio_status =new_status;
}

es_radio_status_t es_radio_adv_status_get(void)
{
    return s_radio_status;
}
