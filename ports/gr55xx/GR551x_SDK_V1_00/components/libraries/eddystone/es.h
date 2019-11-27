/**
 *****************************************************************************************
 *
 * @file es.h
 *
 * @brief Definitions specific to Eddystone frame types and data formats
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
#ifndef ES_H__
#define ES_H__
#include <stdint.h>
#include <stdbool.h>

#define ES_UUID                          0xFEAA     /**< UUID for Eddystone beacons according to specification */
#define ES_UID_FRAME_TYPE                0x00       /**< UID frame type (fixed at 0x00) */
#define ES_UID_RFU                       0x00, 0x00 /**< Reserved for future use according to specification */
#define ES_URL_FRAME_TYPE                0x10       /**< URL frame type (fixed at 0x10) */
#define ES_URL_SCHEME                    0x00       /**< URL prefix scheme according to specification (0x00 = "http://www") */
#define ES_TLM_FRAME_TYPE                0x20       /**< TLM frame type (fixed at 0x20) */
#define ES_EID_FRAME_TYPE                0x30       /**< EID frame type (fixed at 0x30) */
#define ES_FRAME_TYPE_LENGTH             (1)         /**< Length of a frame type field */
#define ES_UID_LENGTH                    (20)        /**< Length of a UID frame */
#define ES_UID_NAMESPACE_LENGTH          (10)        /**< Length of a UID frame namespace field */
#define ES_UID_INSTANCE_LENGTH           (6)         /**< Length of a UID frame instance field */
#define ES_UID_RFU_LENGTH                (2)         /**< Length of a UID frame RFU field */
#define ES_URL_LENGTH                    (20)        /**< Length of a URL frame */
#define ES_URL_URL_SCHEME_LENGTH         (1)         /**< Length of a URL frame URL scheme field */
#define ES_URL_ENCODED_URL_LENGTH        (17)        /**< Maximum length of a URL frame encoded URL field */
#define ES_TLM_LENGTH                    (14)        /**< Length of a TLM frame */
#define ES_TLM_VBATT_LENGTH              (2)         /**< Length of a TLM frame VBATT field */
#define ES_TLM_TEMP_LENGTH               (2)         /**< Length of a TLM frame TEMP field */
#define ES_TLM_ADV_CNT_LENGTH            (4)         /**< Length of a TLM frame ADV count field */
#define ES_TLM_SEC_CNT_LENGTH            (4)         /**< Length of a TLM frame seconds field */
#define ES_EID_LENGTH                    (10)        /**< Length of an EID frame */
#define ES_EID_ID_LENGTH                 (8)         /**< Length of an EID frame ephemeral ID field */
#define ES_EID_GATTS_READ_LENGTH         (14)
#define ES_EID_GATTS_READ_FRAME_TYPE_IDX (0)
#define ES_EID_GATTS_READ_EXPONENT_IDX   (1)
#define ES_EID_GATTS_READ_CLCK_VALUE_IDX (2)
#define ES_EID_GATTS_READ_EID_IDX        (6)
#define ES_ETLM_LENGTH                   (18)        /**< Length of an eTLM frame */
#define ES_ETLM_ECRYPTED_LENGTH          (ES_TLM_VBATT_LENGTH +   \
                                          ES_TLM_TEMP_LENGTH +    \
                                          ES_TLM_ADV_CNT_LENGTH + \
                                          ES_TLM_SEC_CNT_LENGTH)  /**< Length of an eTLM frame encrypted TLM field */
#define ES_ACTIVE_SLOT_LENGTH             1
#define ESCS_LOCK_CODE_WRITE_LENGTH       17
#define ESCS_SLOT_WRITE_FRAME_TYPE_LENGTH (1)
#define ESCS_TLM_READ_LENGTH              (ESCS_TLM_READ_LENGTH)
#define ESCS_TLM_WRITE_LENGTH             (ES_FRAME_TYPE_LENGTH)
#define ESCS_EID_READ_LENGTH              (14)
#define ESCS_EID_WRITE_ECDH_LENGTH        (34)
#define ESCS_EID_WRITE_PUB_KEY_INDEX      (1)
#define ESCS_EID_WRITE_ENC_ID_KEY_INDEX   (1)
#define ESCS_EID_WRITE_IDK_LENGTH         (18)
#define ESCS_LOCK_STATE_READ_LENGTH       (1)
#define ESCS_URL_MIN_WRITE_LENGTH         (4)
#define ESCS_URL_WRITE_LENGTH             (19)

/** Contains definitions specific to the Eddystone Configuration Service */
#define ESCS_UID_READ_LENGTH          (ES_UID_LENGTH)
#define ESCS_UID_WRITE_LENGTH         (ES_UID_NAMESPACE_LENGTH + \
                                       ES_UID_INSTANCE_LENGTH + ES_FRAME_TYPE_LENGTH)

/** Defined in Eddystone Specifications */
#define ESCS_AES_KEY_SIZE               (16)
#define ESCS_ECDH_KEY_SIZE              (32)
#define ESCS_ADV_SLOT_CHAR_LENGTH_MAX   (34) /** Corresponds to when the slots is configured as an EID slot */
#define ESEC_CAP_DEFAULT_LEN                      6               /** Capabilities default length */
#define ESEC_ACTIVE_SLOT_LEN                                            1
#define ESEC_MAX_READ_LEN                 64

/** The UUID of Eddystone Service for setting advertising data */
#define EDDYSTONE_CONFIGURATION_SERVICE_UUID             0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,\
                                                         0xdf, 0x4b, 0xd3, 0x8e, 0x00, 0x75, 0xc8, 0xa3
/** ESCS UUIDs */
#define BLE_UUID_ESCS_BROADCAST_CAP_CHAR      {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x01, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_ACTIVE_SLOT_CHAR        {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x02, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_ADV_INTERVAL_CHAR       {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x03, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_RADIO_TX_PWR_CHAR       {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x04, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_ADV_TX_PWR_CHAR         {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x05, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_LOCK_STATE_CHAR         {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x06, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_UNLOCK_CHAR             {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x07, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_PUBLIC_ECDH_KEY_CHAR    {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x08, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_EID_ID_KEY_CHAR         {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x09, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_RW_ADV_SLOT_CHAR        {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x0a, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_FACTORY_RESET_CHAR      {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x0b, 0x75, 0xc8, 0xa3}
#define BLE_UUID_ESCS_REMAIN_CONNECTABLE_CHAR {0x95, 0xe2, 0xed, 0xeb, 0x1b, 0xa0, 0x39, 0x8a,0xdf, 0x4b, 0xd3, 0x8e, 0x0c, 0x75, 0xc8, 0xa3}

/**  Characteristic: Broadcast Capabilities */
#define ESCS_BROADCAST_VAR_ADV_SUPPORTED_Yes          (1) /**  Set if the beacon supports individual per-slot adv intervals */
#define ESCS_BROADCAST_VAR_ADV_SUPPORTED_No           (0)
#define ESCS_BROADCAST_VAR_ADV_SUPPORTED_Pos          (0)
#define ESCS_BROADCAST_VAR_ADV_SUPPORTED_Msk          (1 << ESCS_BROADCAST_VAR_ADV_SUPPORTED_Pos)
#define ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Yes     (1) /** Set if the beacon supports individual per-slot TX intervals */
#define ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_No      (0)
#define ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Pos     (1)
#define ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Msk     (1 << ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Pos)
#define ESCS_BROADCAST_VAR_RFU_MASK                   (0x03) /** AND Mask to guarantee that bits 0x04 to 0x80 (RFU) are cleared */

/** Field: .broadcast_cap.supp_frame_types */
#define ESCS_FRAME_TYPE_UID_SUPPORTED_Yes             (1)
#define ESCS_FRAME_TYPE_UID_SUPPORTED_No              (0)
#define ESCS_FRAME_TYPE_UID_SUPPORTED_Pos             (0)
#define ESCS_FRAME_TYPE_UID_SUPPORTED_Msk             (1 << ESCS_FRAME_TYPE_UID_SUPPORTED_Pos)
#define ESCS_FRAME_TYPE_URL_SUPPORTED_Yes             (1)
#define ESCS_FRAME_TYPE_URL_SUPPORTED_No              (0)
#define ESCS_FRAME_TYPE_URL_SUPPORTED_Pos             (1)
#define ESCS_FRAME_TYPE_URL_SUPPORTED_Msk             (1 << ESCS_FRAME_TYPE_URL_SUPPORTED_Pos)
#define ESCS_FRAME_TYPE_TLM_SUPPORTED_Yes             (1)
#define ESCS_FRAME_TYPE_TLM_SUPPORTED_No              (0)
#define ESCS_FRAME_TYPE_TLM_SUPPORTED_Pos             (2)
#define ESCS_FRAME_TYPE_TLM_SUPPORTED_Msk             (1 << ESCS_FRAME_TYPE_TLM_SUPPORTED_Pos)
#define ESCS_FRAME_TYPE_EID_SUPPORTED_Yes             (1)
#define ESCS_FRAME_TYPE_EID_SUPPORTED_No              (0)
#define ESCS_FRAME_TYPE_EID_SUPPORTED_Pos             (3)
#define ESCS_FRAME_TYPE_EID_SUPPORTED_Msk             (1 << ESCS_FRAME_TYPE_EID_SUPPORTED_Pos)
#define ESCS_FRAME_TYPE_RFU_MASK                      (0x000F) /** AND Mask to guarantee that bits 0x0010 to 0x8000 (RFU) are cleared */

/** Characteristic: Lock State: Lock State (READ) */
#define ESCS_LOCK_STATE_LOCKED                        (0x00)
#define ESCS_LOCK_STATE_UNLOCKED                      (0x01)
#define ESCS_LOCK_STATE_UNLOCKED_AUTO_RELOCK_DISABLED (0x02)

/** Characteristic: Lock State: Lock Byte (WRITE) */
#define ESCS_LOCK_BYTE_LOCK                           (0x00)
#define ESCS_LOCK_BYTE_DISABLE_AUTO_RELOCK            (0x02)

/**  Charcteristic: Remain Connectable */
#define ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_Yes   (0x01)
#define ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_No    (0x00)

#define ES_ETLM_RFU          (0x00) /**< eTLM frame RFU field value */
#define ES_SPEC_VERSION_BYTE (0x00) /**< eTLM frame specification version field value */

#define MAX_RADIO_DATA_LEN                32

/**  Eddystone frame type values. These values are advertised as frame types */
typedef enum
{
    ES_FRAME_TYPE_UID = ES_UID_FRAME_TYPE, /**< UID frame type */
    ES_FRAME_TYPE_URL = ES_URL_FRAME_TYPE, /**< URL frame type */
    ES_FRAME_TYPE_TLM = ES_TLM_FRAME_TYPE, /**< TLM frame type */
    ES_FRAME_TYPE_EID = ES_EID_FRAME_TYPE,  /**< EID frame type */
} es_frame_type_t;

/**  TLM version values */
typedef enum
{
    ES_TLM_VERSION_TLM = 0x00, /**< TLM */
    ES_TLM_VERSION_ETLM = 0x01 /**< Encrypted TLM (eTLM). */
} es_tlm_version_t;

/** advertising data structure */
typedef struct
{
    uint8_t data[MAX_RADIO_DATA_LEN];
    uint8_t len;                       /**< Length of the data buffer, in bytes */
} es_adv_data_t;

/** sloe data structure */
typedef struct
{
    uint8_t          frame_type;
    es_tlm_version_t tlm_version;
    
    uint16_t         len;
    uint8_t          data[MAX_RADIO_DATA_LEN];
    
    int8_t           radio_tx_power;
    int8_t           adv_tx_setting;
    uint16_t         adv_interval;
    bool             connectable;
    
    uint16_t         strorage_flag;   /**< a label in flash to recognizing valid nvds data */
} es_slot_param_t;
#endif // ES_H__
