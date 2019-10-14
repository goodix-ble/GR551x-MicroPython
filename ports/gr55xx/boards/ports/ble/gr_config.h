#ifndef __GR_CONFIG_H__
#define __GR_CONFIG_H__

#include "custom_config.h"

#define GR_BLE_128BIT_UUID_LEN                              (16)

#define GR_BLE_GATT_MAX_ENTITIES                            (40)
#define GR_BLE_ATTR_MASK_LEN                                (GR_BLE_GATT_MAX_ENTITIES/8+1)
#define GR_BLE_GATTS_VAR_ATTR_LEN_DEFAULT                   (64)
#define GR_BLE_GATTS_VAR_ATTR_LEN_MAX                       (512)   /**< Maximum length for variable length Attribute Values. */
#define GR_BLE_SRV_CONNECT_MAX                              (10 < CFG_MAX_CONNECTIONS ? 10 : CFG_MAX_CONNECTIONS)    /**< Maximum number of connections. */
#define GR_BLE_MAX_SERVICES                                 (10)
#define GR_BLE_GATT_PORTING_LAYER_START_HANDLE              (1)
#define GR_BLE_GATT_INVALID_HANDLE                          0xFFFF
//#define GR_BLE_ADV_DATA_LEN_MAX                             (28)        /// Advertising data maximum length
//#define GR_BLE_SCAN_RSP_DATA_LEN_MAX                        (31)        /// Scan response data maximum length
#define GR_BLE_GAP_ADV_DEFAULT_SIZE                         (31)
#endif /* __GR_CONFIG_H__ */