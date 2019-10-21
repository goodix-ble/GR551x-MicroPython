#include "mphalport.h"
#include "gr55xx_sys.h"
#include "gr55xx_hal.h"

#include "app_log.h"
#include "app_assert.h"
#include "hal_flash.h"
#include "patch.h"

void mp_hal_delay_ms(mp_uint_t ms) {
    sys_delay_ms(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    sys_delay_us(us);    
}

mp_uint_t mp_hal_ticks_ms(void) { 
    return hal_get_tick(); 
}

void mp_hal_set_interrupt_char(char c) {
    
}


/**@brief Bluetooth device address. */
static const uint8_t    s_bd_addr[6] = {0x15, 0x00, 0xcf, 0x3e, 0xcb, 0xea};
 

/*
 * LOCAL  FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void mp_gr5515_log_assert_init(void)
{
    static app_log_init_t   s_app_log_init;
    
    s_app_log_init.filter.level                 = APP_LOG_LVL_DEBUG;
    s_app_log_init.fmt_set[APP_LOG_LVL_ERROR]   = APP_LOG_FMT_ALL & (~APP_LOG_FMT_TAG);
    s_app_log_init.fmt_set[APP_LOG_LVL_WARNING] = APP_LOG_FMT_LVL;
    s_app_log_init.fmt_set[APP_LOG_LVL_INFO]    = APP_LOG_FMT_LVL;
    s_app_log_init.fmt_set[APP_LOG_LVL_DEBUG]   = APP_LOG_FMT_LVL;

    app_log_init(&s_app_log_init);
    app_assert_default_cb_register();
}


void mp_gr5515_init(void)
{
    set_patch_flag(MANDATORY_PATCH);
    hal_init();
    hal_flash_init();
    mp_hal_log_uart_init();
    mp_gr5515_log_assert_init();
    nvds_init(NVDS_START_ADDR, NVDS_NUM_SECTOR);
    SYS_SET_BD_ADDR(s_bd_addr);
    
    
    //pwr_mgmt_init(pwr_table);
    pwr_mgmt_mode_set(PMR_MGMT_ACTIVE_MODE);
    
    /* enable sdk log*/
#if 1
    ble_stack_debug_setup(0x7FFFFFFF, 0x7FFFFFFF, vprintf);
#endif
}
