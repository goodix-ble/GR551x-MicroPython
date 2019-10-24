/**
 *****************************************************************************************
 *
 * @file uds.h
 *
 * @brief User Data Service API.
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

/**
 * @addtogroup BLE_SRV BLE Services
 * @{
 * @brief Definitions and prototypes for the BLE Service interface.
 */

/**
 * @defgroup BLE_SDK_UDS User Data Service (UDS)
 * @{
 * @brief Definitions and prototypes for the UDS interface.
 *
 * @details (TODO)
 */

#ifndef _UDS_H_
#define _UDS_H_

#include "gr55xx_sys.h"
#include "custom_config.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup UDS_MACRO Defines
 * @{
 */
#define UDS_CONNECTION_MAX      (10 < CFG_MAX_CONNECTIONS ?\
                                 10 : CFG_MAX_CONNECTIONS)   /**< Maximum number of UDS connections. */

/* @brief If the Server supports the update of one or more UDS characteristic
 * values (e.g. through its User Interface or any other out-of-band mechanism),
 * define the macro to enable Notify property of Database Change Increment
 * characteristic. */
//#define SUPPORT_UPDATE_OF_UDS_CHAR_VALUES

#define UDS_FIRST_NAME_MAX_LEN  50
#define UDS_LAST_NAME_MAX_LEN   50
#define UDS_EMAIL_ADDR_MAX_LEN  50

/** Unknown user id */
#define UDS_USER_ID_UNKNOWN     0xFF
/** UDS specific error: Access not permitted */
#define UDS_ERROR_ACCESS_NOT_PERMITED   BLE_ATT_ERR_APP_ERROR
/** @} */

/*
 * @defgroup UDS_ENUM Enumerations
 * @{
 */
/**@brief UDS characteristics defined by User Data Service specification in
 * Bluetooth SIG Assigned Numbers. At least one UDS characteristic shall be
 * exposed. */
typedef enum
{
    UDS_CHAR_FIRST_NAME                     = (0x0000000000000300 << 0),
    UDS_CHAR_LAST_NAME                      = (0x0000000000000300 << 2),
    UDS_CHAR_EMAIL_ADDR                     = (0x0000000000000300 << 4),
    UDS_CHAR_AGE                            = (0x0000000000000300 << 6),

    UDS_CHAR_DATE_OF_BIRTH                  = (0x0000000000000300 << 8),
    UDS_CHAR_GENDER                         = (0x0000000000000300 << 10),
    UDS_CHAR_WEIGHT                         = (0x0000000000000300 << 12),
    UDS_CHAR_HEIGHT                         = (0x0000000000000300 << 14),

    UDS_CHAR_VO2_MAX                        = (0x0000000000000300 << 16),
    UDS_CHAR_HEART_RATE_MAX                 = (0x0000000000000300 << 18),
    UDS_CHAR_RESTING_HEART_RATE             = (0x0000000000000300 << 20),
    UDS_CHAR_MAX_RECOMMENDED_HEART_RATE     = (0x0000000000000300 << 22),

    UDS_CHAR_AEROBIC_THRESHOLD              = (0x0000000000000300 << 24),
    UDS_CHAR_ANAEROBIC_THRESHOLD            = (0x0000000000000300 << 26),
    UDS_CHAR_SPORT_TYPE                     = (0x0000000000000300 << 28),
    UDS_CHAR_DATE_OF_THRESHOLD_ASSESSMENT   = (0x0000000000000300 << 30),

    UDS_CHAR_WAIST_CIRCUMFERENCE            = (0x0000000000000300 << 32),
    UDS_CHAR_HIP_CIRCUMFERENCE              = (0x0000000000000300 << 34),
    UDS_CHAR_FAT_BURN_HEART_RATE_LOW_LIMIT  = (0x0000000000000300 << 36),
    UDS_CHAR_FAT_BURN_HEART_RATE_UP_LIMIT   = (0x0000000000000300 << 38),

    UDS_CHAR_AEROBIC_HEART_RATE_LOW_LIMIT   = (0x0000000000000300 << 40),
    UDS_CHAR_AEROBIC_HEART_RATE_UP_LIMIT    = (0x0000000000000300 << 42),
    UDS_CHAR_ANAEROBIC_HEART_RATE_LOW_LIMIT = (0x0000000000000300 << 44),
    UDS_CHAR_ANAEROBIC_HEART_RATE_UP_LIMIT  = (0x0000000000000300 << 46),

    UDS_CHAR_FIVE_ZONE_HEART_RATE_LIMITS    = (0x0000000000000300 << 48),
    UDS_CHAR_THREE_ZONE_HEART_RATE_LIMITS   = (0x0000000000000300 << 50),
    UDS_CHAR_TWO_ZONE_HEART_RATE_LIMIT      = (0x0000000000000300 << 52),
    UDS_CHAR_LANGUAGE                       = (0x0000000000000300 << 54),
} uds_char_mask_t;

/**@brief Response status for UDS control point operation */
typedef enum
{
    UDS_CP_RESPONSE_SUCCESS                 = 0x01,
    UDS_CP_RESPONSE_OPCODE_NOT_SUPPORTED    = 0x02,
    UDS_CP_RESPONSE_INVALID_RARAM           = 0x03,
    UDS_CP_RESPONSE_FAILED                  = 0x04,
    UDS_CP_RESPONSE_NOT_AUTHORIZED          = 0x05,
} uds_cp_response_t;

/**@brief User Data Service event type. */
typedef enum
{
    UDS_EVT_INDICATION_SENT,     /**< Response for sent Indicate */
    UDS_EVT_INDICATION_ENABLED,  /**< Indication status of User Control Point */
    UDS_EVT_INCREMENT_CHANGED,
    UDS_EVT_REGISTER_NEW_USER,
    UDS_EVT_CP_CONSENT,
    UDS_EVT_CP_DELETE_USER_DATA,
    UDS_EVT_CP_READ,
    UDS_EVT_CP_WIRTTE,
    UDS_EVT_UDS_CHAR_WRITE,
    UDS_EVT_UDS_CHAR_READ,
} uds_evt_type_t;
/** @} */

/**
 * @defgroup UDS_TYPEDEF Typedefs
 * @{
 */
/**@brief User Data Service event. */
typedef struct
{
    uds_evt_type_t  evt_type;    /**< Type of event. */
    uint8_t         conn_idx;
    uds_char_mask_t uds_char;
    uint16_t        offset;
    uint16_t        length;
    const void     *p_value;
} uds_evt_t;

/**@brief User Data Service event handler type. */
typedef sdk_err_t (*uds_evt_handler_t)(uds_evt_t *p_evt);

/**@brief User Data Service initialization variable. */
typedef struct 
{
    /** User Data Service event handler. */
    uds_evt_handler_t evt_handler;
    /** Bit mask of supported UDS characteristics. */
    uint64_t          uds_chars;
} uds_init_t;
/** @} */

/*
 * @defgroup UDS_FUNCTION Functions
 * @{
 */
/**
 *****************************************************************************************
 * @brief Initialize an User Data Service instance in ATT DB.
 *
 * @param[in] p_uds_init: Pointer to a User Data Service initialization variable.
 *
 * @return Result of service initialization.
 *****************************************************************************************
 */
sdk_err_t uds_service_init(uds_init_t *p_uds_init);

/**
 *****************************************************************************************
 * @brief Send control point response indication.
 *
 * @param[in] conn_idx:  Connection index to sent indication to.
 * @param[in] p_cp_resp: Pointer to the repose of control point operation.
 *
 * @return BLE_SDK_SUCCESS on success, otherwise an error code
 *****************************************************************************************
 */
sdk_err_t uds_cp_response_send(uint8_t conn_idx,
                               const uds_cp_response_t *p_resp);

/**
 *****************************************************************************************
 * @brief Check if indication is enabled.
 *
 * @param[in] conn_idx: Connection index to sent indication to.
 *
 * @return true if indication is enabled, false otherwise.
 *****************************************************************************************
 */
bool uds_is_cp_indication_enabled(uint8_t conn_idx);

/**
 *****************************************************************************************
 * @brief Get user ID from User Index characteristic.
 *
 * @param[in] conn_idx: Connection index.
 *
 * @return user ID
 *****************************************************************************************
 */
uint8_t uds_user_id_get(uint8_t conn_idx);

/**
 *****************************************************************************************
 * @brief Confirm the value of UDS characteristic has been written. The function
 *        shall be called in \ref uds_evt_handler_t function to handle \ref
 *        UDS_EVT_UDS_CHAR_WRITE. Its return value shall be returned by
 *        \ref uds_evt_handler_t function.
 *
 * @note The function shall be called before the time consuming operation, like
 *       storing data to Flash, in \ref uds_evt_handler_t function.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] uds_char: The UDS characteristic which has been written.
 * @param[in] err_code: Error code.
 *
 * @return \ref BLE_SDK_SUCCESS on success, otherwise an error code.
 *****************************************************************************************
 */
sdk_err_t uds_char_write_cfm(uint8_t conn_idx,uds_char_mask_t uds_char, sdk_err_t err_code);

/**
 *****************************************************************************************
 * @brief Confirm the request of reading UDS characteristic. The function shall
 *        be called in \ref uds_evt_handler_t function to handle \ref
 *        UDS_EVT_UDS_CHAR_READ. Its return value shall be returned by \ref
 *        uds_evt_handler_t function.
 *
 * @param[in] conn_idx: Connection index.
 * @param[in] uds_char: The UDS characteristic which is read.
 * @param[in] err_code: Error code.
 * @param[in] length:   The length of value.
 * @param[in] p_value:  Pointer to the value.
 *
 * @return \ref BLE_SDK_SUCCESS on success, otherwise an error code.
 * */
sdk_err_t uds_char_read_cfm(uint8_t conn_idx, uds_char_mask_t uds_char,  sdk_err_t err_code, uint16_t length, const void *p_value);
/** @} */

#endif
/** @} */
/** @} */
