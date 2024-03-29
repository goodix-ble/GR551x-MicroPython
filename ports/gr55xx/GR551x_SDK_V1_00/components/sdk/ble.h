/**
 ****************************************************************************************
 *
 * @file ble.h
 *
 * @brief include all ble sdk header files
 *
 * Copyright(C) 2016-2018, Shenzhen Goodix Technology Co., Ltd
 * All Rights Reserved
 *
 ****************************************************************************************
 */

 /**
 * @addtogroup BLE
 * @{
 */

/**
 @addtogroup BLE_COMMEN BLE Common
 @{
 @brief BLE Common interface.
 */

#ifndef __BLE_H__
#define __BLE_H__

#include "ble_att.h"
#include "ble_error.h"
#include "ble_gapc.h"
#include "ble_gapm.h"
#include "ble_gatt.h"
#include "ble_gattc.h"
#include "ble_gatts.h"
#include "ble_l2cap.h"
#include "ble_prf.h"
#include "ble_sec.h"

#include <stdio.h> 

/** @addtogroup BLE_COMMEN_ENUM Enumerations
 * @{
 */

/**
 * @brief RF TX mode. 
 */
typedef enum
{
    BLE_RF_TX_MODE_INVALID = 0,
    BLE_RF_TX_MODE_LP_MODE = 1,
    BLE_RF_TX_MODE_ULP_MODE = 2,
} ble_rf_tx_mode_t;

/**
 * @brief The resistance value (ohm) of the RF match circuit. 
 */
typedef enum
{
    BLE_RF_MATCH_CIRCUIT_25OHM = 25,
    BLE_RF_MATCH_CIRCUIT_100OHM = 100,
} ble_rf_match_circuit_t;
/** @} */

/** @addtogroup BLE_COMMEN_STRUCTURES Structures
 * @{
 */
/**@brief The app callbacks for GAP, GATT, SM and L2CAP. */
typedef struct
{
    app_ble_init_cmp_cb_t        app_ble_init_cmp_callback;         /**< Callback function for BLE initialization completed */
    gap_cb_fun_t                *app_gap_callbacks;                 /**< Callback function for GAP */
    const gatt_common_cb_fun_t  *app_gatt_common_callback;          /**< Callback function for GATT common */
    const gattc_cb_fun_t        *app_gattc_callback;                /**< Callback function for GATT Client */
    sec_cb_fun_t                *app_sec_callback;                  /**< Callback function for SM*/
}app_callback_t;

/**@brief The table contains the pointers to four arrays which are used
 * as heap memory by BLE stack in ROM. The size of four arrays depends on
 * the number of connections and the number of attributes of profiles. */
typedef struct 
{
    uint32_t  *env_ret;         /**< Pointer to the array for environment heap */
    uint32_t  *db_ret;          /**< Pointer to the array for ATT DB heap */
    uint32_t  *msg_ret;         /**< Pointer to the array for message heap */
    uint32_t  *non_ret;         /**< Pointer to the array for non-retention heap */
    uint16_t   env_ret_size;    /**< The size of the array for environment heap */
    uint16_t   db_ret_size;     /**< The size of the array for ATT DB heap */
    uint16_t   msg_ret_size;    /**< The size of the array for message heap */
    uint16_t   non_ret_size;    /**< The size of the array for non-retention heap */
    uint8_t   *prf_buf;         /**< Pointer to the array for profile heap */
    uint32_t   buf_size;        /**< The size of the array for profile heap */
    uint8_t   *bm_buf;          /**< Pointer to the array for bond manager heap */
    uint32_t   bm_size;         /**< The size of the array for bond manager heap */
    uint8_t   *conn_buf;        /**< Pointer to the array for connection heap */
    uint32_t   conn_size;       /**< The size of the array for connection heap */
}stack_heaps_table_t;

/**@brief The function pointers for HCI UART. */
typedef struct
{
    void (*init)(void);                                                                             /**< Initialize UART. */
    void (*flow_on)(void);                                                                          /**< Flow control on. */
    bool (*flow_off)(void);                                                                         /**< Flow control off. */
    void (*finish_transfers)(void);                                                                 /**< Finish the current transferring. */
    void (*read)(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);   /**< Read data. */
    void (*write)(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);  /**< Write data. */
} hci_uart_call_t;
/** @} */

/** @addtogroup BLE_COMMEN_FUNCTIONS Functions
 * @{ */
/**
 *****************************************************************************************
 * @brief Initialize BEL Stack.
 *
 * @param[in] p_app_callback: Pointer to the structure of app callbacks.
 * @param[in] p_heaps_table:  Pointer to the BLE stack heaps table.
 *****************************************************************************************
 */
void ble_stack_init(app_callback_t *p_app_callback, stack_heaps_table_t *p_heaps_table);

/**
 *****************************************************************************************
 * @brief Register UART instance for HCI.
 *
 * @param[in] id:  Instance index.
 * @param[in] api: Pointer to the struct of function pointers for HCI UART.
 *****************************************************************************************
 */
void ble_hci_uart_register(uint8_t id, hci_uart_call_t *api);

/**
 *****************************************************************************************
 * @brief Register BLE idle time notification callback function.
 *
 * @param[in] callback:  function pointer of BLE idle time notification function.
 * @note        param[in] of callback: hs - the idle time of BLE in half slot (312.5μs).
 *                  Callback will be called by BLE ISR to notify the rest idle time if there are some BLE activities.
 *                  It should be realized as simlpe as you can.
 *                  It's not suitable for ISO activities.
 *****************************************************************************************
 */
void ble_idle_time_notify_cb_register(void (*callback)(uint32_t hs));

/**
 *****************************************************************************************
 * @brief Register BLE activity start time notification callback function.
 *
 * @param[in] callback:  function pointer of BLE activity start time notification function.
 * @note            param[in] of callback: e_role - the role of activity, @ref gap_activity_role_t for possible roles.
 *                  param[in] of callback: index: The index parameter is interpreted by role.
 *                  If role is @ref GAP_ACTIVITY_ROLE_ADV, it's the index of Advertising.
 *                  If role is @ref GAP_ACTIVITY_ROLE_CON, it's the index of Connection.
 *                  For all other roles, it should be ignored.
 *                  Callback will be called by BLE ISR when the BLE activity starts very time.
 *                  It should be realized as simlpe as you can.
 *                  It's not suitable for ISO activities.
 *****************************************************************************************
 */
void ble_activity_start_notify_cb_register(void (*callback)(gap_activity_role_t e_role,  uint8_t index));

/**
 *****************************************************************************************
 * @brief Change the RF TX mode of LP or ULP.

 * @param[in] e_rf_tx_mode:    Refer to @ref ble_rf_tx_mode_t.
 *                                        BLE_RF_TX_MODE_LP_MODE: LP mode.
 *                                        BLE_RF_TX_MODE_ULP_MODE: ULP mode.
 *                                        Others: invalid mode.
 *                               
 * @note  This function should be called before BLE stack init.
 *
 * @return        SDK_SUCCESS: Successfully set Tx mode.   
 *                SDK_ERR_DISALLOWED: Failed to set Tx mode.
 *****************************************************************************************
 */
uint8_t ble_rf_tx_mode_set(ble_rf_tx_mode_t e_rf_tx_mode);

/**
 *****************************************************************************************
 * @brief  Get the RF TX mode of LP or ULP.
 *
 * @return  BLE_RF_TX_MODE_LP_MODE: LP Mode.
 *              BLE_RF_TX_MODE_ULP_MODE: ULP Mode.
 *              Others: Fail.
 *****************************************************************************************
 */
ble_rf_tx_mode_t ble_rf_tx_mode_get(void);
 
/**
 *****************************************************************************************
 * @brief Set the resistance value of the RF match circuit (unit: ohm).
 *
 * @note  This function should be called before BLE stack init.
 *
 * @param[in] e_ohm: The resistance value (ohm) of the RF match circuit according to the board.
 *                                    BLE_RF_MATCH_CIRCUIT_25OHM: 25 ohm.
 *                                    BLE_RF_MATCH_CIRCUIT_100OHM: 100 ohm.
 *                                    Others: invalid.
 *****************************************************************************************
 */
void ble_rf_match_circuit_set(ble_rf_match_circuit_t e_ohm);

/**
 *****************************************************************************************
 * @brief  Get the resistance value of the RF match circuit (unit: ohm).
 *
 * @return  The resistance value (ohm) of the RF match circuit according to the board (ohm).
 *                    BLE_RF_MATCH_CIRCUIT_25OHM: 25 ohm.
 *                    BLE_RF_MATCH_CIRCUIT_100OHM: 100 ohm.
 *                    Others: invalid.
 *****************************************************************************************
 */
ble_rf_match_circuit_t ble_rf_match_circuit_get(void);

/** @} */
#endif
/** @} */
/** @} */
