#include "mp_defs.h"
#include "mphalport.h"
#include "boards.h"
#include "gr55xx_hal.h"
#include "gr55xx_sys.h"

#include "app_log.h"
#include "app_assert.h"
#include "app_uart.h"
#include "hal_flash.h"
#include "patch.h"


#ifndef APP_UART_ID
#define APP_UART_ID     APP_UART_ID_0
#endif

void mp_hal_delay_ms(mp_uint_t ms) {
    sys_delay_ms(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    sys_delay_us(us);    
}

mp_uint_t mp_hal_ticks_ms(void) { 
    return hal_get_tick(); 
}

mp_uint_t mp_hal_ticks_us(void) {    
    return 1000 * mp_hal_ticks_ms();
}

void mp_hal_set_interrupt_char(char c) {
    
}


/**@brief Bluetooth device address. */
static const uint8_t    s_bd_addr[6] = {0xea, 0x99, 0xcf, 0x3e, 0xcb, 0x15};
 

/*
 * LOCAL  FUNCTION DEFINITIONS
 *****************************************************************************************
 */

#define UART_RX_BUFF_SIZE				1u
#define UART_TX_BUFF_SIZE				2048u

static uint8_t s_uart_rx_buffer[UART_RX_BUFF_SIZE];
static uint8_t s_uart_tx_buffer[UART_TX_BUFF_SIZE];

extern void app_log_transfer_rx_char(char data);

static void gr5515_bsp_uart_send(uint8_t *p_data, uint16_t length)
{
    app_uart_transmit_async(APP_UART_ID, p_data, length);
}

static void gr5515_bsp_uart_flush(void)
{
    app_uart_flush(APP_UART_ID);
}

/*
 * Handle Rx Of UART0
 * __WEAK version in bsp.c
 */
void gr5515_app_uart_evt_handler(app_uart_evt_t *p_evt)
{
    if(p_evt->type == APP_UART_EVT_RX_DATA) {
        if(p_evt->data.size == UART_RX_BUFF_SIZE) {
            app_log_transfer_rx_char(s_uart_rx_buffer[0]);
        }
        memset(&s_uart_rx_buffer[0], 0 , UART_RX_BUFF_SIZE);
        app_uart_receive_async(APP_UART_ID, &s_uart_rx_buffer[0], UART_RX_BUFF_SIZE);
    }
}

void gr5515_bsp_uart_init(void)
{
    app_uart_tx_buf_t uart_buffer;
    app_uart_params_t uart_param;

    uart_buffer.tx_buf       = s_uart_tx_buffer;
    uart_buffer.tx_buf_size  = UART_TX_BUFF_SIZE;

    uart_param.id                   = APP_UART_ID;
    uart_param.init.baud_rate       = APP_UART_BAUDRATE;
    uart_param.init.data_bits       = UART_DATABITS_8;
    uart_param.init.stop_bits       = UART_STOPBITS_1;
    uart_param.init.parity          = UART_PARITY_NONE;
    uart_param.init.hw_flow_ctrl    = UART_HWCONTROL_NONE;
    uart_param.init.rx_timeout_mode = UART_RECEIVER_TIMEOUT_ENABLE;
    uart_param.pin_cfg.rx.type      = APP_UART_RX_IO_TYPE;
    uart_param.pin_cfg.rx.pin       = APP_UART_RX_PIN;
    uart_param.pin_cfg.rx.mux       = APP_UART_RX_PINMUX;
    uart_param.pin_cfg.tx.type      = APP_UART_TX_IO_TYPE;
    uart_param.pin_cfg.tx.pin       = APP_UART_TX_PIN;
    uart_param.pin_cfg.tx.mux       = APP_UART_TX_PINMUX;
    uart_param.use_mode.type        = APP_UART_TYPE_INTERRUPT;

    app_uart_init(&uart_param, gr5515_app_uart_evt_handler, &uart_buffer);
    app_uart_receive_async(APP_UART_ID, s_uart_rx_buffer, UART_RX_BUFF_SIZE);
}

static void mp_gr5515_log_assert_init(void)
{
    static app_log_init_t   s_app_log_init;
    
    s_app_log_init.filter.level                 = APP_LOG_LVL_DEBUG;
    s_app_log_init.fmt_set[APP_LOG_LVL_ERROR]   = APP_LOG_FMT_ALL & (~APP_LOG_FMT_TAG);
    s_app_log_init.fmt_set[APP_LOG_LVL_WARNING] = APP_LOG_FMT_LVL;
    s_app_log_init.fmt_set[APP_LOG_LVL_INFO]    = APP_LOG_FMT_LVL;
    s_app_log_init.fmt_set[APP_LOG_LVL_DEBUG]   = APP_LOG_FMT_LVL;

    app_log_init(&s_app_log_init, gr5515_bsp_uart_send, gr5515_bsp_uart_flush);
    app_assert_default_cb_register();
}

static gr5515_update_xo_offset(void) {
    #warning "verify xo-offset value here!"
    
    uint32_t xo_value = 0x25b260c6;
    //[bit18, bit10]
    uint32_t rf_adjust_value = (xo_value >> 10) & 0x1FF;

    uint32_t xo_reg_value = (*((uint32_t *)(0xA000C538)) & (~0x0007FC00)) | (rf_adjust_value << 10);
    (*(volatile uint32_t *)(0xa000c538)) = xo_reg_value;
}

void mp_gr5515_init(void)
{
    // update xo_offset
    gr5515_update_xo_offset();
    hal_init();
    hal_flash_init();
    gr5515_bsp_uart_init();
    mp_hal_log_uart_init();
    mp_gr5515_log_assert_init();
    nvds_init(NVDS_START_ADDR, NVDS_NUM_SECTOR);
    SYS_SET_BD_ADDR(s_bd_addr);
    
    pwr_mgmt_init(pwr_table, CPLL_S64M_CLK);
    pwr_mgmt_mode_set(PMR_MGMT_ACTIVE_MODE);
    
    /* enable sdk log*/
#if 1
    ble_stack_debug_setup(0x7FFFFFFF, 0x7FFFFFFF, vprintf);
#endif
}
