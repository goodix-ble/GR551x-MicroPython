/**
*****************************************************************************************
*
* @file es_slot.h
*
* @brief Slot data packeting api declare
*
*****************************************************************************************
*/
#ifndef ES_TLM_H__
#define ES_TLM_H__
#include "es_app_config.h"
#include "es_utility.h"
#include "es_adv.h"
#include "es.h"
#include "utility.h"
#include "es_timer.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/**
 *****************************************************************************************
 * @brief increase counter timers for adverstising
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
void es_tlm_adv_cnt_inc (void);

/**
 *****************************************************************************************
 * @brief packet slot data to url broadcast frame
 *
 * @param[in]   p_slot_data: pointer to slot data
 * @param[out]  p_adv_data: poter to buffer for data after packetized
 *
 *****************************************************************************************
 */
void es_packet_url (es_slot_param_t* p_slot_data, es_adv_data_t* p_adv_data);

/**
 *****************************************************************************************
 * @brief packet slot data to uid broadcast frame
 *
 * @param[in]   p_slot_data: pointer to slot data
 * @param[out]  p_adv_data: poter to buffer for data after packetized
 *
 *****************************************************************************************
 */
void es_packet_uid (es_slot_param_t* p_slot_data, es_adv_data_t* p_adv_data);

/**
 *****************************************************************************************
 * @brief packet slot data to tlm broadcast frame
 *
 * @param[in]   p_slot_data: pointer to slot data
 * @param[out]  p_adv_data: poter to buffer for data after packetized
 *
 *****************************************************************************************
 */
void es_packet_tlm (es_slot_param_t * p_slot_data,es_adv_data_t * p_adv_data);
#endif // ES_NVDS_H__
