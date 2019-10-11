#include "app_error.h"
#include "gr551xx.h"
#include "user_app.h"
#include "scatter_common.h"
#include "flash_scatter_config.h"
#include "gr_porting.h"
#include "patch.h"
#include "assert.h"


/*
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */

extern  gap_cb_fun_t            app_gap_callbacks;
extern  gatt_common_cb_fun_t    app_gatt_common_callback;
extern  gattc_cb_fun_t          app_gattc_callback;
extern  l2cap_lecb_cb_fun_t     app_l2cap_callback;
extern  sec_cb_fun_t            app_sec_callback;

gr_ble_common_params_t          s_gr_ble_common_params_ins = {
    .is_ble_initialized     = false,
};

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
/**@brief Stack global variables for Bluetooth protocol stack. */
STACK_HEAP_INIT(heaps_table);

static void ble_init_complete_callback(void);

static app_callback_t s_app_ble_callback = 
{
    .app_ble_init_cmp_callback = ble_init_complete_callback,
    .app_gap_callbacks         = NULL,//&app_gap_callbacks,
    .app_gatt_common_callback  = NULL,//&app_gatt_common_callback,
    .app_gattc_callback        = NULL,//&app_gattc_callback,
    .app_sec_callback          = NULL,//&app_sec_callback,
};

static void ble_init_complete_callback(void){
    gap_bdaddr_t bd_addr;
    sdk_err_t    error_code;

    error_code = ble_gap_addr_get(&bd_addr);    
    
    //gr_trace(">>> code: %d  \r\n", error_code);
    gr_trace(">>> ble stack init completed [%02X:%02X:%02X:%02X:%02X:%02X] \r\n",
                                         bd_addr.gap_addr.addr[5],
                                         bd_addr.gap_addr.addr[4],
                                         bd_addr.gap_addr.addr[3],
                                         bd_addr.gap_addr.addr[2],
                                         bd_addr.gap_addr.addr[1],
                                         bd_addr.gap_addr.addr[0]);   
    s_gr_ble_common_params_ins.is_ble_initialized = true;
    memcpy(&s_gr_ble_common_params_ins.local_bd_addr, &bd_addr, sizeof(gap_bdaddr_t));
}


void gr_ble_stack_init(void){
    s_gr_ble_common_params_ins.is_ble_initialized = false;
    ble_stack_init(&s_app_ble_callback, &heaps_table);
}

char * gr_ble_format_uuid128b_to_string(uint8_t * uuid128b, uint8_t len){    
    //"6e400001-b5a3-f393-e0a9-e50e24dcca9e"
    static char u128str[37];
    
    assert(len == 16);
    memset(&u128str[0], 0 ,37);    
    sprintf(&u128str[0], "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
                                        uuid128b[15], uuid128b[14], uuid128b[13], uuid128b[12], 
                                        uuid128b[11], uuid128b[10], uuid128b[9],  uuid128b[8], 
                                        uuid128b[7],  uuid128b[6],  uuid128b[5],  uuid128b[4], 
                                        uuid128b[3],  uuid128b[2],  uuid128b[1],  uuid128b[0]);
    
    return &u128str[0];
}

#if 0
/*
 * transfer porting layer handle to gatt handle in ble stack
 * JUST Be called when connected
 */
uint16_t gr_gatt_transto_ble_stack_handle(uint16_t porting_handle){
    uint16_t stack_handle;

    if((porting_handle < GR_BLE_GATT_PORTING_LAYER_START_HANDLE) || (porting_handle == GR_BLE_GATT_INVALID_HANDLE)){
        stack_handle = GR_BLE_GATT_INVALID_HANDLE;
    } else {
        stack_handle = porting_handle - GR_BLE_GATT_PORTING_LAYER_START_HANDLE + s_gattsp_instance.start_handle;
    }    
    
    return stack_handle;
}

/*
 * transfer gatt handle in ble stack to porting layer handle
 * JUST Be called when connected
 */
uint16_t gr_gatt_transto_porting_layer_handle(uint16_t stack_handle){
    uint16_t porting_handle = 0;    
    if(stack_handle < s_gattsp_instance.start_handle){
        porting_handle = GR_BLE_GATT_INVALID_HANDLE;
    } else {
        porting_handle = (stack_handle - s_gattsp_instance.start_handle) + GR_BLE_GATT_PORTING_LAYER_START_HANDLE;
    }    
    gr_trace(">>> Stack Handle %d transferred to Porting Handle: %d \r\n", stack_handle, porting_handle);
    return porting_handle;
}

#endif



void HardFault_Handler_C(unsigned int * hardfault_args)
{
    unsigned int stacked_r0;
    unsigned int stacked_r1;
    unsigned int stacked_r2;
    unsigned int stacked_r3;
    unsigned int stacked_r12;
    unsigned int stacked_lr;
    unsigned int stacked_pc;
    unsigned int stacked_psr;
    
    __BKPT(0);
    
    stacked_r0 = ((unsigned long) hardfault_args[0]);
    stacked_r1 = ((unsigned long) hardfault_args[1]);
    stacked_r2 = ((unsigned long) hardfault_args[2]);
    stacked_r3 = ((unsigned long) hardfault_args[3]);

    stacked_r12 = ((unsigned long) hardfault_args[4]);
    stacked_lr = ((unsigned long) hardfault_args[5]);
    stacked_pc = ((unsigned long) hardfault_args[6]);
    stacked_psr = ((unsigned long) hardfault_args[7]);
    gr_trace("\n\n[Hard fault handler - all numbers in hex]\n");
    gr_trace("R0 = %x\n", stacked_r0);
    gr_trace("R1 = %x\n", stacked_r1);
    gr_trace("R2 = %x\n", stacked_r2);
    gr_trace("R3 = %x\n", stacked_r3);
    gr_trace("R12 = %x\n", stacked_r12);
    gr_trace("LR [R14] = %x  subroutine call return address\n", stacked_lr);
    gr_trace("PC [R15] = %x  program counter\n", stacked_pc);
    gr_trace("PSR = %x\n", stacked_psr);
    gr_trace("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
    gr_trace("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
    gr_trace("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
    gr_trace("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
    gr_trace("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
    gr_trace("SCB_SHCSR = %x\n", SCB->SHCSR);    
    while (1);
}
