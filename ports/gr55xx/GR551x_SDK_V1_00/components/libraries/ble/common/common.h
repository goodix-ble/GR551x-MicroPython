#ifndef _COMMOM_H_
#define _COMMOM_H_

#include "gr55xx_sys.h"

typedef enum
{
	BLE_SUCCESS = 0x0000,
	BLE_ERROR_NULL = 0x0100,
	BLE_ERROR_INVALID_IDX = 0x0101,
	
} ble_module_error_t;

typedef struct
{
	uint8_t conidx; 
	const gap_conn_cmp_t *param;
} gap_on_connected_t;

typedef struct
{
	uint8_t  conidx;
	const uint8_t *param;
} gap_on_disconnected_t;


#endif
