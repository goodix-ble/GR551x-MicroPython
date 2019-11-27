#ifndef __USER_SLEEP_H_
#define __USER_SLEEP_H_
#include "gr55xx_ll_pwr.h"
#include "custom_config.h"
#include <stdbool.h>
/// Macro to read a platform register
#define REG_PL_RD(addr)              (*(volatile uint32_t *)(addr))

/// Macro to write a platform register
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)
    
#define SOFTWARE_REG   (0XA000C578UL)
#define SOFTWARE_REG_WAKEUP_FLAG_POS   (8)
#define SOFTWARE_REG_FLASH_COPY_FLAG_POS   (9)


typedef struct
{
    /// Sleep Duration Value in periodic wake-up mode in Half slots
    uint32_t max_dur_periodic_wakeup;

    /// Sleep Duration Value in external wake-up mode
    uint32_t max_dur_external_wakeup;
    
    /// Sleep Duration Threshold (in: us)
    uint32_t max_slp_dur_thr;
    
    /// Hardware latch the CLKN delay (in: us)
    uint32_t min_clkn_latch_delay;
} ble_sleep_param_config_t;

typedef enum {
    GOTO_SLEEP = 0,
    KEEP_POWERED
} arch_main_loop_callback_ret_t;

typedef enum
{
    mode_chip_active = 0,
    mode_chip_idle,
    mode_chip_sleep,
} sleep_mode_t;

extern bool ble_is_powered(void);
extern uint32_t get_remain_sleep_dur(void);
extern uint8_t ble_core_sleep(void );
extern uint32_t sys_us_2_lpcycles(uint32_t us);
extern uint32_t sys_lpcycles_2_hus(uint32_t lpcycles, uint32_t *error_corr);
/*
 * Main loop callback functions' structure. Developer must set application's callback functions in
 * the initialization of the structure in user_callback_config.h 
 ****************************************************
 */
struct arch_main_loop_callbacks {

    arch_main_loop_callback_ret_t (*app_on_ble_powered)(void);
    arch_main_loop_callback_ret_t (*app_on_system_powered)(void);
 
};

typedef enum
{
    ARCH_STATE_ACTIVE,
    ARCH_STATE_IDLE,
    ARCH_STATE_SLEEP,
} sleep_state_t;

enum ble_sleep_state
{
    /// Some activity pending, can not enter in sleep state
    CPU_ACTIVE    = 0,
    /// CPU can be put in idle state
    CPU_IDLE,
    /// CPU can be put in sleep state
    CPU_SLEEP,
};

/// RWIP Sleep Environment structure
struct ble_sleep_env_tag
{
    /// Contains sleep duration accumulated timing error (in half us)
    uint32_t sleep_acc_error;
    /// Power_up delay (in LP clock cycle unit, depends on Low power clock frequency)
    uint32_t lp_cycle_wakeup_delay;
    /// Duration of sleep and wake-up algorithm (depends on CPU speed) expressed in half us.
    uint16_t sleep_algo_dur;
    /// Prevent sleep bit field
    volatile uint16_t prevent_sleep;
    /// External wake-up support
    bool     ext_wakeup_enable;
};
/**@brief AON memoty config definition. */
typedef struct 
{
    uint32_t mem_region;            /**< Memory region      */
    uint32_t mem_pwr_level;   /**< Memory power state  */
} mem_config_t;

typedef void(*gpio_wkup_cb_t)(uint8_t pin);
typedef void(*timer_wkup_cb_t)(void);
typedef sleep_state_t (*sleep_mode_switch_cb_t)(void);
typedef struct
{
    uint8_t pin;
	uint8_t cur_sleep_mode;
    sleep_state_t sleep_state;
    uint32_t timer_value;
    uint8_t  timer_mode;
    uint32_t ext_wkup_type;
    uint32_t ext_wakeup_pinx;
    uint32_t wakeup_source;
    mem_config_t sleep_mem_config;
    mem_config_t wkup_mem_config;
    gpio_wkup_cb_t gpio_wkup_cb;
    timer_wkup_cb_t timer_wkup_cb;
	sleep_mode_switch_cb_t sleep_mode_switch_cb;
}sleep_env_t;

#ifdef CFG_GPIO_DEBUG
#define GPIO_DEBUG(PIN,NUM)\
        {\
            uint8_t i=0;\
            for(i=0;i<NUM*2;i++)\
            {\
                ll_aon_gpio_set_pin_mode(PIN, LL_AON_GPIO_MODE_OUTPUT); \
                ll_aon_gpio_disable_it(PIN);\
                ll_aon_gpio_toggle_pin(PIN);\
            }\
        }
#else
#define GPIO_DEBUG(PIN,NUM)        
#endif




/**
 ****************************************************************************************
 * @brief Used for application specific tasks immediately after exiting the low power mode.
 * @param[in] sleep_mode     Sleep Mode
 * @return void
 ****************************************************************************************
 */
void app_sleep_exit_proc(sleep_mode_t sleep_mode);

void arch_goto_sleep (sleep_mode_t current_sleep_mode);


/**
 ****************************************************************************************
 * @brief  An interrupt came, resume from sleep.
 * @return void
 ****************************************************************************************
 */
void arch_resume_from_sleep(sleep_mode_t sleep_mode);

void arch_sleep_state_set(sleep_state_t state);

extern sleep_mode_t sys_sleep_mode(void);

/**
 ****************************************************************************************
 * @brief Power down the BLE Radio and whatever is allowed according to the sleep mode and
 *        the state of the system and application
 * @param[in] current_sleep_mode The current sleep mode proposed by the application.
 * @return void
 ****************************************************************************************
 */
void pmu_sleep(void);

/**
 ****************************************************************************************
 * @brief       Wake the BLE core via an external request. 
 * @param       void
 * @return      status
 * @retval      The status of the requested operation.
 *              <ul>
 *                  <li> false, if the BLE core is not sleeping
 *                  <li> true, if the BLE core was woken-up successfully
 *              </ul>
 ****************************************************************************************
 */
bool arch_ble_force_wakeup(void);

extern sleep_env_t sleep_env;
void wakeup_init(void);
void low_power_setting(void);
void system_sleep(void);
uint32_t get_wakeup_flag(void);
void set_wakeup_flag(uint32_t flag);

/**
 ****************************************************************************************
 * @brief  add a hal interface to get the status of sleep mode;
 *        
 * @retval : 
 ****************************************************************************************
 */
int hal_check_sleep_state(void);

void hal_pm_init(void);
#endif
