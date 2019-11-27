/**
 *****************************************************************************************
 *
 * @file wss.h
 *
 * @brief Weight Scale API.
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

#ifndef _WSS_H_
#define _WSS_H_

#include "gr55xx_sys.h"
#include "custom_config.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * DEFINES
 *****************************************************************************************
 */
#define WSS_CONNECTION_MAX      (10 < CFG_MAX_CONNECTIONS ?\
                                 10 : CFG_MAX_CONNECTIONS)   /**< Maximum number of WSS connections.

/*
 * ENUMERATIONS
 *****************************************************************************************
 */
/**@brief Weight Feature characteristic bit values. The size of this
 * characteristic is 32bits. */
typedef enum
{
    WSS_FEAT_TIME_STAMP_SUPPORTED = 0x0001, /**< Time Stamp supported */
    WSS_FEAT_MULTI_USER_SUPPORTED = 0x0002, /**< Multiple Users supported */
    WSS_FEAT_BMI_SUPPORTED        = 0x0004, /**< BMI supported */

    /* Weight Resolution */
    WSS_FEAT_WT_RES_500G    = 0x0008, /**< Resolution of 0.5kg or 1lb */
    WSS_FEAT_WT_RES_200G    = 0x0010, /**< Resolution of 0.2kg or 0.5lb */
    WSS_FEAT_WT_RES_100G    = 0x0018, /**< Resolution of 0.1kg or 0.2lb */
    WSS_FEAT_WT_RES_50G     = 0x0020, /**< Resolution of 0.05kg or 0.1lb */
    WSS_FEAT_WT_RES_20G     = 0x0028, /**< Resolution of 0.02kg or 0.05lb */
    WSS_FEAT_WT_RES_10G     = 0x0030, /**< Resolution of 0.01kg or 0.02lb */
    WSS_FEAT_WT_RES_5G      = 0x0038, /**< Resolution of 0.005kg or 0.01lb */

    /* Height Resolution */
    WSS_FEAT_HT_RES_10MM    = 0x0080, /**< Resolution of 0.01m or 1in */
    WSS_FEAT_HT_RES_5MM     = 0x0100, /**< Resolution of 0.005m or 0.5in */
    WSS_FEAT_HT_RES_1MM     = 0x0180, /**< Resolution of 0.001m or 0.1in */
    
    WSS_FEAT_FORCE_SIZE = 0x7FFFFFFF,
} wss_feature_t;

/**@brief WSS unit types */
typedef enum
{
    WSS_UNIT_SI,        /**< Weight in kilograms and height in meters */
    WSS_UNIT_IMPERIAL,  /**< Weight in pounds and height in inches */
} wss_unit_t;

/**@brief Weight Scale Service event type. */
typedef enum
{
    WSS_EVT_INDICATION_SENT,     /**< Response for sent Indicate */
    WSS_EVT_INDICATION_ENABLED,  /**< Indication status of Weight Measurement */
} wss_evt_type_t;

/*
 * TYPE DEFINITIONS
 *****************************************************************************************
 */
/**@brief WSS Weight Measurement data. */
typedef struct
{
    /** User id of 0xFF for guest user. */
    uint8_t         user_id;
    /** Measurement unit */
    wss_unit_t      unit;
    /** Weight value defined in WSS spec. */
    uint16_t        weight;
    /** Height value defined in WSS spec. */
    uint16_t        height;
    /** Body Mass Index (BMI) defined in WSS spec.
     * If height=0, it will not be reported. */
    uint16_t        bmi;
    /** Flag which indicates if time-stamp is present. */
    bool            timestamp_present;
    /** Time of measurement, NULL if timestamp_present is false. */
    prf_date_time_t timestamp;
} wss_meas_t;

/**@brief Weight Scale Service event. */
typedef struct
{
    wss_evt_type_t evt_type;    /**< Type of event. */
    union
    {
        /** status for sent indication, true for success, false for failure. */
        bool status;
        /** true for indication enabled, false for indication disabled. */
        bool enabled;
    } params;
} wss_evt_t;

/**@brief Weight Scale Service event handler type. */
typedef void (*wss_evt_handler_t)(wss_evt_t *p_evt);

/**@brief Weight Scale Service environment variable. */
typedef struct 
{
    /** Weight Scale Service event handler. */
    wss_evt_handler_t evt_handler;
    /** Weight Scale Service start handle. */
    uint16_t          start_hdl;
    /** Bit mask of supported features. */
    wss_feature_t     wss_features;
    /** Client characteristic configuration descriptor for each connection. */
    uint16_t          meas_cccd[WSS_CONNECTION_MAX];
} wss_env_t;

/*
 * FUNCTION DECLARATIONS
 *****************************************************************************************
 */
/**@brief Add a Weight Scale Service instance in ATT DB
 *
 * @param[in] Pointer to a Weight Scale Service environment variable
 *
 * @return Result of service initialization.
 *****************************************************************************************
 */
sdk_err_t wss_add_service(wss_env_t *p_wss_env);

/**@brief Send weight measurement indication.
 *
 * @param[in] conidx Connection index to sent indication to.
 * @param[in] p_meas Pointer to weight measurement data.
 *
 * @return     BLE_SDK_SUCCESS on success, otherwise an error code
 */
sdk_err_t wss_measurement_send(uint8_t conidx,
                                 const wss_meas_t *p_meas);

/**@brief Check if indication is enabled.
 *
 * @param[in] conidx Connection index to sent indication to.
 *
 * @return true if indication is enabled, false otherwise.
 */
bool wss_is_indication_enabled(uint8_t conidx);

#endif

