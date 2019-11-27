#ifndef _BLE_ADVERTISING_H_
#define _BLE_ADVERTISING_H_

#include "gr55xx_sys.h"
#include "common.h"


typedef struct
{
	uint8_t const * 	p_data;
	uint8_t 	        dlen;
	uint8_t const * 	p_sr_data;
	uint8_t 	        srdlen;
	uint8_t const * 	p_per_data;
	uint8_t 	        perdlen;		
} adv_data_t;

typedef struct
{
	gap_own_addr_t own_addr_type;
	bool leagcy_adv_flag;
	union
	{
		gap_adv_param_t* adv_param;
		gap_ext_adv_param_t* ext_adv_param;
	}info;
	gap_adv_time_param_t* time_param;
} adv_param_t;

typedef struct
{
	uint16_t renew_dur;
	bool enable_flag;
} privacy_param_t;

typedef enum
{
    BLE_ADV_EVT_STARTED,                
    BLE_ADV_EVT_STOPPED,            
    BLE_ADV_EVT_SCAN_REQ,
	BLE_ADV_EVT_ON_CONNECTED,
	BLE_ADV_EVT_ON_DISCONNECTED,
} ble_adv_evt_t;

typedef struct
{
    uint8_t inst_idx;    /*Advertising index.*/
	uint16_t status;     /*Error code @see ble_error_t. */
} gap_adv_started_t;

typedef struct
{
    uint8_t inst_idx;    /*Advertising index.*/
	bool time_out_flag;  /*Advertising timeout. If this flag is true, the next member status is invalid.*/
	uint16_t status;     /*Error code @see ble_error_t. */
} gap_adv_stopped_t;

typedef struct
{
    uint8_t inst_idx;                   /*Advertising index.*/
    const gap_bdaddr_t *scanner_addr;   /*Transmitter device address.*/
} gap_scan_request_ind_t;

//BLE advertising event handler type.
typedef void (*ble_adv_evt_handler_t) (ble_adv_evt_t const adv_evt, void *param);

typedef struct
{
	//Restart advertising flag.
	bool                        restart_flag;        
	
	//Saved advertising configuration.
	struct
	{
		//adv_param_saved use
		union
		{
			gap_adv_param_t      adv_param;
			gap_ext_adv_param_t  ext_adv_param;
		} info;
		gap_adv_time_param_t     time_param;
		
		//adv_data_saved use
		struct
		{
			uint8_t  *p_data;
			uint8_t   dlen;	
			uint8_t  *p_sr_data;
			uint8_t   srdlen;
			uint8_t  *p_per_data;
			uint8_t   perdlen;			
		}adv_data;
		
		//privacy_param_saved use
		privacy_param_t         privacy_param;
	} info_saved;
	adv_param_t                 adv_param_saved;
	adv_data_t *                adv_data_saved;
	privacy_param_t *           privacy_param_saved;
	
	//Current existent advertisings. Bit0-4 means index0-4. 
	uint8_t						current_adv_idx_array;
	
	//BLE advertising event handler.
	ble_adv_evt_handler_t       evt_handler;             
} ble_advertising_t;

/*This function to start an advertising. 
  If the advertising activity(adv_idx related to) is existent: stop the advertising activity, and then start the advertising.
  If the advertising activity(adv_idx related to) is inexistent: start the advertising.
*/
uint16_t ble_advertising_start(uint8_t adv_idx, adv_param_t *adv_param, adv_data_t *adv_data, privacy_param_t *privacy_param);

/*This function to stop an advertising. 
  If the advertising activity(adv_idx related to) is existent: stop the advertising activity.
  If the advertising activity(adv_idx related to) is inexistent: return BLE_ERROR_INVALID_IDX.
*/
uint16_t ble_advertising_stop(uint8_t adv_idx);

/*Register BLE advertising event handler.*/
void     ble_advertising_evt_handler(ble_adv_evt_handler_t evt_handler);

void ble_advertising_scan_req_ind_cb(uint8_t inst_idx, const gap_bdaddr_t *scanner_addr);

void ble_advertising_adv_stopped_cb(uint8_t inst_idx, const gap_stopped_reason_t *param);

#endif
