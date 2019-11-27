/**
 *****************************************************************************************
 *
 * @file ble_scan.h
 *
 * @brief BLE Scan Module API
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
 * @defgroup BLE_MDL BLE Module
 * @{
 * @brief Definitions and prototypes for the BLE Module interface.
 */

/**
 * @defgroup BLE_MDL_SCAN BLE Scan Module
 * @{
 * @brief Definitions and prototypes for the Scan Module interface.
 *
 * @details The Scan Module provides scanning device, parsing advertising report and filtering device functions for application.
 *
 *          After receive advertising report from other devices, Scan Module will parse these data and save the result of
 *          parsing for application. If application enable filter in the module, which will find target device base on filter mode.
 */

#ifndef _BLE_SCAN_
#define _BLE_SCAN_

#include "common.h"
#include "gr55xx_sys.h"

/**
 * @defgroup BLE_SCAN_MAROC Defines
 * @{
 */
#define RAW_DATA_LEN_MAX                    31             /**< Maximum length of one advertising report packet. */
#define AD_TYPE_NUM_MAX                     10             /**< Maximum number of ad type can be parsed in one advertising report packet. */

#define UUID_16_BIT_NUM_MAX                 14             /**< Maximum number of 16 bit uuid service can be parsed in one advertising report packet. */
#define UUID_32_BIT_NUM_MAX                 7              /**< Maximum number of 32 bit uuid service can be parsed in one advertising report packet. */
#define UUID_128_BIT_NUM_MAX                1              /**< Maximum number of 128 bit uuid service can be parsed in one advertising report packet. */

#define UUID_16_BIT_BYTES                   2              /**< Length of bytes for 16 bit UUID. */
#define UUID_32_BIT_BYTES                   4              /**< Length of bytes for 32 bit UUID. */
#define UUID_128_BIT_BYTES                  16             /**< Length of bytes for 128 bit UUID. */

#define DEV_NAME_LEN_MAX                    18             /**< Maximum length of device name. */
#define UUID_LEN_MAX                        16             /**< Maximum length of UUID. */
/** @} */

/**
 * @defgroup BLE_SCAN_ENUM Enumerations
 * @{
 */
/**@brief BLE Scan filter mode. */
typedef enum
{
    BLE_SCAN_NAME_FILTER = 0x01,         /**< Filter device base on target device name. */
    BLE_SCAN_APPEARANCE_FILTER,          /**< Filter device base on target appearance. */
    BLE_SCAN_UUID_FILTER,                /**< Filter device base on tagert service UUID. */
    BLE_SCAN_ADDR_FILTER,                /**< Filter device base on target address. */
} ble_scan_filter_mode_t;

/**@brief BLE Scan event type. */
typedef enum
{
    BLE_SCAN_EVT_INVALID,                /**< Invalid event. */
    BLE_SCAN_EVT_WHITELIST_REQUEST,      /**< Request whitelist load. */
    BLE_SCAN_EVT_ADV_REPORT,             /**< Received advertising report. */
    BLE_SCAN_EVT_WHITELIST_DEV_FOUND,    /**< Found device from whitelist. */
    BLE_SACN_EVT_DATA_PARSE_CPLT,        /**< Advertising or scan response data parse completely. */
    BLE_SCAN_EVT_FILTER_MATCH,           /**< Filter match from adv report. */
    BLE_SCAN_EVT_FILTER_NO_MATCH,        /**< Filter no match from adv report. */
    BLE_SCAN_EVT_SCAN_TIME_OUT,          /**< Scan timeout. */
    BLE_SCAN_EVT_SCAN_STOP,              /**< Scan activity stopped. */
    BLE_SCAN_EVT_CONNECTED,              /**< Host connected. */
} ble_scan_evt_type_t;
/** @} */

/**
 * @defgroup BLE_SCAN_STRUCT Structures
 * @{
 */
/**@brief Local name parsed from advertising report. */
typedef struct
{
    uint8_t  *p_data;      /**< Pointer to local name data in raw data  @See ble_scan_parse_record_t.raw_data.. */
    uint8_t   length;      /**< Length of local name. */
} ble_scan_local_name_t;

/**@brief Manufacturer Specific data parsed from advertising report. */
typedef struct
{
    uint16_t  company_id;            /**< Company identifier code. */
    uint8_t  *p_data;                /**< Pointer to manufacturer specific data in raw data @See ble_scan_parse_record_t.raw_data. */
    uint8_t   length;                /**< Length of additional manufacturer specific data. */
} ble_scan_manu_data_t;

/**@brief UUID data parsed from advertising report. */
typedef struct
{
    uint8_t  uuid_16_bit_count;                                           /**< Count of 16 bit uuid parsed. */
    uint8_t  uuid_32_bit_count;                                           /**< Count of 32 bit uuid parsed. */
    uint8_t  uuid_128_bit_count;                                          /**< Count of 128 bit uuid parsed. */
    uint16_t uuid_16_bit[UUID_16_BIT_NUM_MAX];                            /**< All 16 bit uuid data parsed. */
    uint32_t uuid_32_bit[UUID_32_BIT_NUM_MAX];                            /**< All 32 bit uuid data parsed. */
    uint8_t  uuid_128_bit[UUID_128_BIT_NUM_MAX][UUID_128_BIT_BYTES];      /**< All 128 bit uuid data parsed. */
} ble_scan_srv_uuid_t;

/**@brief Single GAP advertising type parsed from advertising report. */
typedef struct
{
    gap_ad_type_t  adv_type;                /**< GAP advertising type. */
    uint8_t       *p_data;                  /**< Pointer to adv type in raw data @See ble_scanner_parse_record_t.raw_data. */
    uint8_t        length;                  /**< Length of the adv type data. */
} ble_scan_single_record_t;

/**@brief All GAP advertising type parsed from advertising report. */
typedef struct
{
    uint8_t                     data_type_count;                   /**< Count of advertising type parsed. */
    ble_scan_single_record_t    single_record[AD_TYPE_NUM_MAX];    /**< Information of parsed. */
    uint8_t                     raw_data[RAW_DATA_LEN_MAX];        /**< All parsed advertising type data. */
} ble_scan_parse_record_t;

/**@brief All parsed result from advertising report. */
typedef struct
{
    gap_adv_report_type_t      adv_report_type;   /**< Advertising type. @see gap_adv_report_type_t. */
    gap_bdaddr_t               target_addr;       /**< Address of device from which advertising. */
    uint8_t                    flag;              /**< Flag, shall not appear more than once in a block. */
    int8_t                     tx_power;          /**< TX Power Level. */
    uint16_t                   appearance;        /**< Appearance, shall not appear more than once in a block. */
    ble_scan_local_name_t      local_name;        /**< Local name, shall not appear more than once in a block. */
    ble_scan_manu_data_t       manufacture_data;  /**< Manufacturer specific data. */
    ble_scan_srv_uuid_t        srv_uuid_list;     /**< Service uuid list. */
    ble_scan_parse_record_t    all_parse_record;  /**< All data parse record. */
} ble_scan_parse_report_t;

/**@brief Target name provided to filter. */
typedef struct
{
    uint8_t  name[DEV_NAME_LEN_MAX];               /**< Target name data. */
    uint16_t length;                                /**< Length of target name. */
} ble_scan_name_filter_t;

/**@brief Target UUID provided to filter. */
typedef struct
{
    uint8_t  uuid[UUID_LEN_MAX];                  /**< Target UUID data. */
    uint16_t length;                              /**< Length of target UUID. */
} ble_scan_uuid_filter_t;
/** @} */

/**
 * @defgroup BLE_SCAN_UNION Unions
 * @{
 */
/**@brief Record all target data provided to filter. */
typedef union
{
    ble_scan_name_filter_t    dev_name;           /**< Target device name. */
    uint16_t                  appearance;         /**< Target appearance. */
    ble_scan_uuid_filter_t    svr_uuid;           /**< Target service UUID. */
    gap_bdaddr_t              target_addr;        /**< Target device address. */
} ble_scan_filter_data_t;
/** @} */

/**
 * @defgroup BLE_SCAN_STRUCT Structures
 * @{
 */
/**@brief Matched with which target data. */
typedef struct
{
    bool  dev_name_match;            /**< True if device name matched, false or not. */
    bool  appearance_match;          /**< True if appearance matched, false or not. */
    bool  uuid_match;                /**< True if service UUID matched, false or not. */
    bool  addr_match;                /**< True if address matched, false or not. */
} ble_scan_filter_match_t;

/**@brief BLE Scan Module event. */
typedef struct
{ 
    ble_scan_evt_type_t               evt_type;          /**< BLE Scanner event type. */
    sdk_err_t                         error_code;        /**< Error codes for BLE operation. */
    ble_scan_filter_match_t           match_type;        /**< Filter match type. */
    ble_scan_parse_report_t           data_parse_record; /**< Advertising report parse result record. */
    const gap_ext_adv_report_ind_t   *p_adv_report;      /**< The extended advertising report. */
} ble_scan_evt_t;
/** @} */

/**
 * @defgroup BLE_SCAN_TYPEDEF Typedefs
 * @{
 */
typedef void (*ble_scan_evt_handler_t)(ble_scan_evt_t *p_evt);
/** @} */

/**
 * @defgroup BLE_SCAN_STRUCT Structures
 * @{
 */
typedef struct
{
    bool                      connect_auto;      /**< Ture: Connect to target device when found, False: Notify to application when device when found. */
    gap_own_addr_t            own_addr_type;     /**< Own BD address source of the device. */
    gap_scan_param_t          scan_param;        /**< Scan parameters. */
    gap_init_param_t          conn_param;        /**< Connect parameters which must be initialized when @see connect_auto to be set Ture. */
    ble_scan_evt_handler_t    evt_handler;       /**< Handler to handle some Scanner events for application. */
} ble_scan_init_t;
/** @} */


/**
 * @defgroup BLE_SCAN_FUNCTION Functions
 * @{
 */
/**
 *****************************************************************************************
 * @brief BLE Scan Module initialization.
 *
 * @param[in]  p_scan_init: Pointer to BLE Scan Module initialization parameters
 *
 * @return Result of init operation.
 *****************************************************************************************
 */
sdk_err_t   ble_scan_init(ble_scan_init_t *p_scan_init);

/**
 *****************************************************************************************
 * @brief BLE Scan Module filter set.
 *
 * @param[in] filter_mode:    Filter mode.
 * @param[in] p_filter_sdata: Pointer to data for filtering.
 *
 * @return Result of set operation.
 *****************************************************************************************
 */
sdk_err_t   ble_scan_filter_set(ble_scan_filter_mode_t filter_mode, ble_scan_filter_data_t *p_filter_data);


/**
 *****************************************************************************************
 * @brief Disable BLE Scan Module filter.
 *****************************************************************************************
 */
void ble_scan_filter_disable(void);

/**
 *****************************************************************************************
 * @brief BLE Scan Module starts scanning device.
 *
 * @return Result of start scanning.
 *****************************************************************************************
 */
sdk_err_t   ble_scan_start(void);

/**
 *****************************************************************************************
 * @brief Parse advertising report.
 *
 * @param[in]  adv_report_type: Type of advertising report.
 * @param[in]  p_data:          Pointer to data.
 * @param[in]  length:          Length of data.
 * @param[out] p_parse_encode   Pointer to result of parsing.
 *****************************************************************************************
 */
void ble_scan_data_parse(gap_adv_report_type_t adv_report_type, const uint8_t *p_data, uint16_t length, ble_scan_parse_report_t *p_parse_encode);

/**
 *****************************************************************************************
 * @brief Encode uuid.
 *
 * @param[in] p_data:       Pointer to uuid data.
 * @param[in] length:       Length of uuid data.
 * @param[in] uuid_type:    Type of uuid.
 * @param[out] p_uuid_buff: Pointer to where save encode result.
 *****************************************************************************************
 */
void ble_scan_uuid_encode(const uint8_t *p_uuid_data, uint16_t length, uint8_t uuid_type, ble_scan_srv_uuid_t *p_uuid_buff);

/**
 *****************************************************************************************
 * @brief Filter device from advertising report based on tagert name data.
 *
 * @param[in] p_data:        Pointer to advertising report data.
 * @param[in] length:        Length of advertising report data.
 * @param[in] p_target_name: Pointer to target name.
 *
 * @return Operation result.
 *****************************************************************************************
 */
bool ble_scan_name_filter(const uint8_t *p_data, uint16_t length, ble_scan_name_filter_t *p_target_name);

/**
 *****************************************************************************************
 * @brief Filter device from advertising report based on tagert appearance.
 *
 * @param[in] p_data:            Pointer to advertising report data.
 * @param[in] length:            Length of advertising report data.
 * @param[in] target_appearance: Target appearance.
 *
 * @return Operation result.
 *****************************************************************************************
 */
bool ble_scan_appearance_filter(const uint8_t *p_data, uint16_t length, uint16_t target_appearance);

/**
 *****************************************************************************************
 * @brief Filter device from advertising report based on tagert service uuid.
 *
 * @param[in] p_data:            Pointer to advertising report data.
 * @param[in] length:            Length of advertising report data.
 * @param[in] p_target_srv_uuid: Pointer to target service uuid.
 *
 * @return Operation result.
 *****************************************************************************************
 */
bool ble_scan_uuid_filter(const uint8_t *p_data, uint16_t length, ble_scan_uuid_filter_t *p_target_srv_uuid);

/**
 *****************************************************************************************
 * @brief Filter device from advertising report based on tagert address.
 *
 * @param[in] p_src_addr:    Pointer to source address.
 * @param[in] p_target_addr: Pointer to target address.
 *
 * @return Operation result.
 *****************************************************************************************
 */
bool ble_scan_addr_filter(gap_bdaddr_t *p_src_addr, gap_bdaddr_t *p_target_addr);

/**
 *****************************************************************************************
 * @brief Call BLE Scan Module event handler.
 *
 * @param[in] p_evt: Pointer to BLE Scanner event structure.
 *
 * @return Operation result.
 *****************************************************************************************
 */
sdk_err_t   ble_scan_evt_handler_call(ble_scan_evt_t *p_evt);
/** @} */

#endif
/** @} */
/** @} */

