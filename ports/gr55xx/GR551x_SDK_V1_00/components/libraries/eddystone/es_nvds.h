/**
****************************************************************************************
*
* @file es_nvds.h
*
* @brief Defines for nvds function
*
****************************************************************************************
*/
#ifndef ES_NVDS_H__
#define ES_NVDS_H__
#include "es_app_config.h"
#include "es.h"
#include "es_utility.h"
#include "es_adv.h"
#include "utility.h"
#include "es_slot.h"
#include "escs.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define ES_NVDS_TAG_SLOT_DATA_IDX_BASE 5
#define ES_NVDS_TAG_ULOCK_KEY_IDX     (ES_NVDS_TAG_SLOT_DATA_IDX_BASE+APP_MAX_ADV_SLOTS)

#define SIZE_OF_LOCK_KEY  ESCS_AES_KEY_SIZE /**< Size of lock key */
#define NVDS_STORAGE_FLAG 0x55AA

/**
 *****************************************************************************************
 * @brief check if active slot is advertising
 *
 * @return  true if is advertising
 *
 *****************************************************************************************
*/
bool is_active_slot_advertising(void);

/**
 *****************************************************************************************
 * @brief get slot data
 *
 * @param[in]   slot_no: number of the slot to get data
 * @param[out]   p_length: pointer for storing slot length
 * @param[out]   p_frame_data: pointer for storing slot data
 * @return      NVDS_SUCCESS if successful
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
uint8_t es_nvds_slot_data_get (uint8_t slot_no, uint16_t* p_length, uint8_t* p_frame_data);

/**
 *****************************************************************************************
 * @brief set slot data
 *
 * @param[in]   length: new slot data length
 * @param[in]   p_frame_data: pointer for data buffer
 *
 *****************************************************************************************
 */
void es_nvds_slot_data_set (uint8_t length, uint8_t* p_frame_data);

/**
 *****************************************************************************************
 * @brief set lock key
 *
 * @param[in]   p_key_data: key data pointer
 *
 * @return      NVDS_SUCCESS if successful
 *
 *****************************************************************************************
 */
uint8_t es_nvds_lock_key_set (uint8_t* p_key_data);

/**
 *****************************************************************************************
 * @brief get lock key
 *
 * @param[out]  p_key_buffer: given data pointer to store key
 *
 * @return      NVDS_SUCCESS if successful
 *
 *****************************************************************************************
 */
uint8_t es_nvds_lock_key_get (uint8_t* p_key_data);

/**
 *****************************************************************************************
 * @brief clear active slot data
 *
 * @return      NVDS_SUCCESS if successful
 *
 *****************************************************************************************
 */
uint8_t es_nvds_clear_slot (void);

/**
 *****************************************************************************************
 * @brief clear all slot data
 *
 * @return      NVDS_SUCCESS if successful
 *
 *****************************************************************************************
 */
uint8_t es_nvds_factory_reset (void);

/**
 *****************************************************************************************
 * @brief get a packet of valid slot data to buffer
 *
 * @param[out]  p_slot_data    poter to buf for storing slot data
 *
 *****************************************************************************************
 */
void nvds_get_next_adv_frame(es_slot_param_t* p_slot_data);

#endif // ES_NVDS_H__
