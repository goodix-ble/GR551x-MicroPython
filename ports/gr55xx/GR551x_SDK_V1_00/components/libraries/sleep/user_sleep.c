#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "gr55xx_hal.h"
#include "gr55xx_sys.h"
#include "user_sleep.h"
#include "custom_config.h"
#include "system_gr55xx.h"
/****************************************************************************************/
//the boot time is 2000us
#define TIMER_WARM_BOOT_TIME_MAX  4000
#define BLE_WARM_BOOT_TIME_MAX  12000
#define RW_WAKE_UP_ONGOING  0x0001

bool timer_warm_boot_flag = false;
bool wkup_source_set_flag = false;
uint32_t timer_wkup_count =0;

uint32_t nvic_enable_flag0 =0;
uint32_t nvic_enable_flag1 =0;

inline void disable_nvic_irq(void)
{
    nvic_enable_flag0 = NVIC->ISER[0];
    nvic_enable_flag1 = NVIC->ISER[1];
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
}
inline void enable_nvic_irq(void)
{
    NVIC->ISER[0] = nvic_enable_flag0;
    NVIC->ISER[1] = nvic_enable_flag1;
}

bool check_pend_irq(void)
{
    uint32_t nvic_pending_flag0 =0;
    uint32_t nvic_pending_flag1 =0;
    nvic_pending_flag0 = NVIC->ISPR[0];
    nvic_pending_flag1 = NVIC->ISPR[1];
    return (nvic_enable_flag0 & nvic_pending_flag0)||(nvic_enable_flag1 & nvic_pending_flag1);
}
sleep_state_t sleep_mode_switch_cb(void);

//default configuration for the sleep;
sleep_env_t g_sleep_env ={
    .cur_sleep_mode = mode_chip_active,
    .sleep_state = ARCH_STATE_IDLE,
    .timer_value =0,
    .timer_mode = PWR_SLP_TIMER_MODE_RELOAD,
    .pin = PWR_EXTWKUP_PIN4,
    .ext_wkup_type = PWR_EXTWKUP_TYPE_LOW,
    .wakeup_source = PWR_WKUP_COND_BLE,
    .sleep_mem_config ={
                        .mem_region = PWR_MEM_ALL,
                        .mem_pwr_level= PWR_MEM_POWER_FULL,
                    },
    .wkup_mem_config ={
                        .mem_region = PWR_MEM_ALL,
                        .mem_pwr_level= PWR_MEM_POWER_FULL,
                    },
    .gpio_wkup_cb = NULL,
    .timer_wkup_cb = NULL,
    .sleep_mode_switch_cb = NULL,
};

uint32_t get_wakeup_flag()
{
    uint32_t software_reg =*((uint32_t*)(SOFTWARE_REG));
    return  (software_reg&(1<<SOFTWARE_REG_WAKEUP_FLAG_POS));
}
void set_wakeup_flag(uint32_t flag)
{
    uint32_t software_reg =*((uint32_t*)(SOFTWARE_REG));
    software_reg = (software_reg&(~(1<<SOFTWARE_REG_WAKEUP_FLAG_POS)));
    if(flag)
    {
        software_reg = software_reg|(1<<SOFTWARE_REG_WAKEUP_FLAG_POS);
    }
    *((uint32_t*)(SOFTWARE_REG))=software_reg;
}

void set_wakeup_sources(void)
{

    if(!wkup_source_set_flag)
    {
        hal_pwr_set_wakeup_condition(g_sleep_env.wakeup_source);
        if(g_sleep_env.wakeup_source&PWR_WKUP_COND_EXT)
        {
            hal_pwr_config_ext_wakeup(g_sleep_env.pin, g_sleep_env.ext_wkup_type);
            NVIC_EnableIRQ(EXTWKUP_IRQn);
        }
        if(g_sleep_env.wakeup_source&PWR_WKUP_COND_TIMER)
        {
            if(g_sleep_env.timer_value!=0)
            {
                hal_pwr_config_timer_wakeup(g_sleep_env.timer_mode, sys_us_2_lpcycles(g_sleep_env.timer_value));
                NVIC_EnableIRQ(SLPTIMER_IRQn);
            }
        }
        wkup_source_set_flag = true;
    }
}

extern void enable_nvic_irq(void);
static void arch_goto_sleep (sleep_mode_t current_sleep_mode)
{

    SCB->SCR |= (1<<4);
    //add the other action
    NVIC_EnableIRQ(BLE_SDK_IRQn);
    //timer_warm_boot_flag=false;
    enable_nvic_irq();
    return;
}

/**
 ****************************************************************************************
 * @brief  An interrupt came, resume from sleep.
 * @return void
 ****************************************************************************************
 */
static void arch_resume_from_sleep(sleep_mode_t sleep_mode)
{

}

 /**
 ****************************************************************************************
 * @brief  the interface enable the user to modify the allowed sleep mode.
 *
 * @return void
 ****************************************************************************************
 */
void arch_sleep_state_set(sleep_state_t state)
{
    g_sleep_env.sleep_state =state;
}

///Sleep Environment structure
struct sleep_env_tag
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

extern void sys_sleep_env_get(struct sleep_env_tag **sleep_env);

void sleep_mode_switch(void)
{
    if(g_sleep_env.sleep_mode_switch_cb!=NULL)
    {
        g_sleep_env.sleep_state = g_sleep_env.sleep_mode_switch_cb();
    }

}
/**
 ****************************************************************************************
 * @brief enable the IRQ when the cpu is wakeuped by the wakeup sources ;
 * @param[in] [us]    duration in us;
 *                    remain_timer_sleep_dur : the remain sleep duration until the next timer interrupt (IRQ#26);
 *                    remain_ble_timer_sleep_dur : the remain sleep duration until the next BLE timer interrupt(IRQ#25);
 * @return the sleep mode;
 ****************************************************************************************
 */

sleep_mode_t sys_sleep_mode(void)
{
    uint32_t error =0;
    uint32_t remain_ble_timer_sleep_dur = 0xFFFFFFFF; //us
    uint32_t remain_timer_sleep_dur = 0xFFFFFFFF;
    //read the remain sleep timer depend on the timer or the ble_timer ;
    g_sleep_env.wakeup_source = ll_pwr_get_wakeup_condition();
    sleep_mode_switch();

    sleep_mode_t ret_mode = mode_chip_active;

    uint8_t sleep_mode =0;
    if(g_sleep_env.sleep_state==ARCH_STATE_ACTIVE)
        return ret_mode;

    sleep_mode = ble_core_sleep();

    //when the next wkup is coming very fast,not need to deep sleep;
    if((sleep_mode == CPU_SLEEP))
    {
        if(g_sleep_env.wakeup_source&PWR_WKUP_COND_TIMER)
        {
            hal_pwr_get_timer_current_value(PWR_TIMER_TYPE_SLP_TIMER,&remain_timer_sleep_dur); //us
            remain_timer_sleep_dur =sys_lpcycles_2_hus(remain_timer_sleep_dur,&error);
        }

        if(g_sleep_env.wakeup_source!=PWR_WKUP_COND_BLE)
        {
            remain_ble_timer_sleep_dur = get_remain_sleep_dur(); //us
        }

        if(remain_timer_sleep_dur < TIMER_WARM_BOOT_TIME_MAX||remain_ble_timer_sleep_dur < BLE_WARM_BOOT_TIME_MAX)
        {
            sleep_mode  = CPU_IDLE;
        }
    }
    switch (sleep_mode)
    {
        case CPU_ACTIVE:
             break;
        case CPU_IDLE:
            {
                ret_mode= mode_chip_idle;

            }break;
        case CPU_SLEEP:
            {
                ret_mode= mode_chip_sleep;
                if(g_sleep_env.sleep_state==ARCH_STATE_IDLE)
                {
                    ret_mode= mode_chip_idle;
                }
            }break;
    }

    g_sleep_env.cur_sleep_mode = sleep_mode;

    return ret_mode;
}

/**
 ****************************************************************************************
 * @brief Power down the BLE Radio and whatever is allowed according to the sleep mode and
 *        the state of the system and application
 * @return void
 ****************************************************************************************
 */
#pragma arm section code = "RAM_CODE"

/* ================================================================================================================= */
/* ================                               Low Power Sleep Settings                          ================ */
/* ================================================================================================================= */
#define LOW_IO_VOLTAGE_OUT 0x21  //1.8 volts out
#define CTRL_TEMPCO        0x0E
#define COUNTER_A          0x03
#define COUNTER_B          0x00
#define DIG_LDO_D          0x03
#define TUNE_C             0x03
#define CLK_PERIOD         0x05


void low_power_setting(void) {
    AON->MEM_N_SLP_CTL = ((AON->MEM_N_SLP_CTL) & ~AON_MEM_CTL_SLP_Msk) |\
                         ((AON_MEM_CTL_SLP_TRN_OFF_DCDC   | AON_MEM_CTL_SLP_TRN_OFF_XO |\
                           AON_MEM_CTL_SLP_TRN_OFF_PLL_EN | AON_MEM_CTL_SLP_TRN_OFF_PLL_TUNE |\
                           AON_MEM_CTL_SLP_TRN_OFF_LDO_EN | AON_MEM_CTL_SLP_TRN_OFF_PLL_RST /* |\
                           AON_MEM_CTL_SLP_TRN_OFF_IO_LDO_EN*/ ));
      
    AON->RF_REG_0 = ((AON->RF_REG_0) & ~(AON_RF_REG_0_LPD_REG1_Msk | AON_RF_REG_0_LPD_REG2_Msk)) |\
                                      ( (0xE0 << AON_RF_REG_0_LPD_REG1_Pos) |\
                                        (0x03 << AON_RF_REG_0_LPD_REG2_Pos) );
    AON->RF_REG_4 = ((AON->RF_REG_4) & ~AON_RF_REG_4_CLK_PERIOD_Msk) | (CLK_PERIOD << AON_RF_REG_4_CLK_PERIOD_Pos);  
    AON->RF_REG_3 = ((AON->RF_REG_3) & ~((AON_RF_REG_3_BOD_EN_Msk)) );
    AON->RF_REG_1 = ((AON->RF_REG_1) & ~AON_RF_REG_1_EN_INJ_Msk ) | AON_RF_REG_1_EN_INJ_ON;  
    AON->WARM_BOOT_TIME = ((TUNE_C    << AON_WARM_BOOT_TIME_TUNE_C_Pos)    |\
                           (DIG_LDO_D << AON_WARM_BOOT_TIME_DIG_LDO_D_Pos) |\
                           (COUNTER_B << AON_WARM_BOOT_TIME_COUNTER_B_Pos) |\
                           (COUNTER_A << AON_WARM_BOOT_TIME_COUNTER_A_Pos) );
    
    MCU_SUB->DPAD_RE_N_BUS = ((MCU_SUB->DPAD_RE_N_BUS) & ~MCU_SUB_DPAD_RE_N_BUS_Msk) | 0x0366f388; //Enable resistors
    MCU_SUB->DPAD_RTYP_BUS = (MCU_SUB->DPAD_RTYP_BUS) & ~MCU_SUB_DPAD_RTYP_BUS_Msk;  //Pull down resistors
}
void pmu_sleep() 
{
    set_wakeup_flag(true);
    sys_context_save();
    hal_pwr_enter_deepsleep_mode(g_sleep_env.sleep_mem_config.mem_region,g_sleep_env.wkup_mem_config.mem_region);
}
#pragma arm section code
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

/*****************************************************************************************
 * @brief Function to wait until ble core is ready to work.
 ****************************************************************************************/
void arch_ble_wait_wakeup_done(void)
{
    while (ll_pwr_is_enabled_smc_wakeup_req());
}
bool arch_ble_force_wakeup(void)
{
    bool retval = false;

    // If BLE is sleeping, wake it up!
    GLOBAL_INT_DISABLE();
    if (!ble_is_powered()) { // BLE clock is off
        ll_pwr_enable_smc_wakeup_req();
        retval = true;
    }
    GLOBAL_INT_RESTORE();
    arch_ble_wait_wakeup_done();
    return retval;
}



/**
 ****************************************************************************************
 * @brief enable the IRQ when the cpu is wakeuped by the wakeup sources ;
 *
 * @return void
 ****************************************************************************************
 */
extern void hal_flash_init(void);
void wakeup_init()
{
   hal_flash_init();
   NVIC_EnableIRQ(BLESLP_IRQn);
   NVIC_EnableIRQ(BLE_IRQn);
   NVIC_EnableIRQ(BLE_SDK_IRQn);
   NVIC_EnableIRQ(SLPTIMER_IRQn);
   NVIC_EnableIRQ(EXTWKUP_IRQn);
}

/**
 ****************************************************************************************
 * @brief the interface for the chip goto sleep while the cpu is idle ;
 *
 * @return void
 ****************************************************************************************
 */
#pragma arm section code = "RAM_CODE"
void wfe_sleep(bool clock_flag)
{
    __disable_irq();

    SCB->SCR |=SCB_SCR_SEVONPEND_Msk;

    REG_PL_WR(0xA000E2B0,0X00000001); //Turn off the 192 Mhz clock tree.  This should be done when xo has been set as clock source.

    AON->RF_REG_8 = ((AON->RF_REG_8) &~ (0x1f << AON_RF_REG_8_XO_REG1_Pos)) | (0x08 << AON_RF_REG_8_XO_REG1_Pos); //Quarter Bias
    
      __WFI();
    REG_PL_WR(0xA000E2B0,0X00000000);

    __enable_irq();

}
#pragma arm section code

extern void  ble_task_main(void );
void system_sleep(void)
{
    sleep_mode_t sleep_mode = mode_chip_active ;
    ble_task_main();
    NVIC_EnableIRQ(BLE_SDK_IRQn);
    disable_nvic_irq();

    sleep_mode = sys_sleep_mode();

    if(!check_pend_irq())
    {
        switch (sleep_mode) {

            case mode_chip_active:
            case mode_chip_idle: {
                        #ifdef CFG_GPIO_DEBUG
                        ll_aon_gpio_set_pin_mode(LL_AON_GPIO_PIN_4, LL_AON_GPIO_MODE_OUTPUT);
                        ll_aon_gpio_disable_it(LL_AON_GPIO_PIN_4);
                        ll_aon_gpio_toggle_pin(LL_AON_GPIO_PIN_4);
                        #endif
                
                        arch_goto_sleep(sleep_mode);
                        //wait for an interrupt to resume operation
                #if 0
                        //__SEV();
                        //__WFE();
                        //__WFE();
                #else
                        wfe_sleep(false);
                #endif
                        arch_resume_from_sleep(sleep_mode);
//                
                        #ifdef CFG_GPIO_DEBUG
                        ll_aon_gpio_toggle_pin(LL_AON_GPIO_PIN_4);
                        #endif
            }break;
            case mode_chip_sleep:
            {
                #ifdef CFG_GPIO_DEBUG
                ll_aon_gpio_set_pin_mode(LL_AON_GPIO_PIN_4, LL_AON_GPIO_MODE_OUTPUT);
                ll_aon_gpio_disable_it(LL_AON_GPIO_PIN_4);
                ll_aon_gpio_toggle_pin(LL_AON_GPIO_PIN_4);
                ll_aon_gpio_toggle_pin(LL_AON_GPIO_PIN_4);
                #endif
                pmu_sleep();
            }break;

            default:break;
        }
    }
    enable_nvic_irq();

}

void SLPTIMER_IRQHandler(void)
{
    hal_pwr_sleep_timer_irq_handler();
    if(g_sleep_env.timer_wkup_cb!=NULL)
        g_sleep_env.timer_wkup_cb();
    timer_wkup_count++;

}
void EXTWKUP_IRQHandler(void)
{
    uint32_t pin = ll_pwr_get_ext_wakeup_status();
    hal_pwr_ext_wakeup_irq_handler();
    if(g_sleep_env.gpio_wkup_cb!=NULL)
        g_sleep_env.gpio_wkup_cb(pin);

}
void gpio_sleep_mode_switch_init(void)
{
    #ifdef SLEEP_TEST_ON_CHIP
    aon_gpio_init_t aon_gpio_config = AON_GPIO_DEFAULT_CONFIG;
    aon_gpio_config.pin = AON_GPIO_PIN_5;
    aon_gpio_config.pull = AON_GPIO_PULLUP;
    hal_aon_gpio_init(&aon_gpio_config);
    #endif
}
sleep_state_t sleep_mode_switch_cb(void)
{
    sleep_state_t  ret_sleep_state = ARCH_STATE_SLEEP;
    #ifdef SLEEP_TEST_ON_CHIP
    gpio_sleep_mode_switch_init();
    if(hal_aon_gpio_read_pin(AON_GPIO_PIN_5)) 
        {
        ret_sleep_state = ARCH_STATE_IDLE;
        }
    else 
        {
        ret_sleep_state = ARCH_STATE_SLEEP;
        }
    #endif
    
    return  ret_sleep_state;
}


extern ble_sleep_param_config_t* get_ble_slp_param_cfg(void);

__WEAK void app_ble_sleep_param_config(uint16_t *twosc, uint16_t *twext, uint16_t *slp_dur)
{
    #ifdef LOW_POWER_MEM_CTRL
    if(APP_CODE_RUN_ADDR == APP_CODE_LOAD_ADDR) //XIP Mode
    {
        //64k
        g_sleep_env.sleep_mem_config.mem_region = (LL_PWR_MEM_SRAM_8K_0 |\
                                                   LL_PWR_MEM_SRAM_8K_1 |\
                                                   LL_PWR_MEM_SRAM_8K_2 |\
                                                   LL_PWR_MEM_SRAM_8K_3 |\
                                                   LL_PWR_MEM_SRAM_32K_0|\
                                                   LL_PWR_MEM_HOPPING_TABLE|\
                                                   LL_PWR_MEM_PACKET_MEM);
                                                                                  
        g_sleep_env.wkup_mem_config.mem_region = ( LL_PWR_MEM_SRAM_8K_0 |\
                                                   LL_PWR_MEM_SRAM_8K_1 |\
                                                   LL_PWR_MEM_SRAM_8K_2 |\
                                                   LL_PWR_MEM_SRAM_8K_3 |\
                                                   LL_PWR_MEM_SRAM_32K_0|\
                                                   LL_PWR_MEM_SRAM_32K_1|\
                                                   LL_PWR_MEM_SRAM_32K_2|\
                                                   LL_PWR_MEM_SRAM_32K_5|\
                                                   LL_PWR_MEM_HOPPING_TABLE|\
                                                   LL_PWR_MEM_ICACHE|\
                                                   LL_PWR_MEM_PACKET_MEM);
    }
    else  //MIRROR MODE
    {
        //128k
      g_sleep_env.sleep_mem_config.mem_region = (LL_PWR_MEM_SRAM_8K_0 |\
                                                   LL_PWR_MEM_SRAM_8K_1 |\
                                                   LL_PWR_MEM_SRAM_8K_2 |\
                                                   LL_PWR_MEM_SRAM_8K_3 |\
                                                   LL_PWR_MEM_SRAM_32K_0|\
                                                   LL_PWR_MEM_SRAM_32K_1|\
                                                   LL_PWR_MEM_SRAM_32K_2|\
                                                   LL_PWR_MEM_HOPPING_TABLE|\
                                                   LL_PWR_MEM_PACKET_MEM|\
                                                   AON_MEM_PWR_SLP_PD_MCU_KEYRAM);
                                                                                  
      g_sleep_env.wkup_mem_config.mem_region = ( LL_PWR_MEM_SRAM_8K_0 |\
                                                   LL_PWR_MEM_SRAM_8K_1 |\
                                                   LL_PWR_MEM_SRAM_8K_2 |\
                                                   LL_PWR_MEM_SRAM_8K_3 |\
                                                   LL_PWR_MEM_SRAM_32K_0|\
                                                   LL_PWR_MEM_SRAM_32K_1|\
                                                   LL_PWR_MEM_SRAM_32K_2|\
                                                   LL_PWR_MEM_SRAM_32K_5|\
                                                   LL_PWR_MEM_HOPPING_TABLE|\
                                                   LL_PWR_MEM_PACKET_MEM|\
                                                   AON_MEM_PWR_SLP_PD_MCU_KEYRAM);
    }
    #endif
    //Check running mode
    if(APP_CODE_RUN_ADDR != APP_CODE_LOAD_ADDR)
    { // Mirror mode
        switch(SystemCoreClock)
        {
            case CLK_64M:
                *slp_dur = 550;
                *twext = 830;
                *twosc = 830;
                break;
            case CLK_48M:
                *slp_dur = 580;
                *twext = 880;
                *twosc = 880;
                break;
            case CLK_32M:
                *slp_dur = 620;
                *twext = 930;
                *twosc = 930;
                break;
            case CLK_24M:
                *slp_dur = 700;
                *twext = 1050;
                *twosc = 1050;
                break;
            case CLK_16M:
                *slp_dur =800;
                *twext = 1300;
                *twosc = 1300;
                break;
            default:
                *slp_dur = 1900;
                *twext = 10000;
                *twosc = 10000;
                break;
        }
    }
    else
    { //XIP
        switch(SystemCoreClock)
        {
            case CLK_64M:
                *slp_dur = 550;
                *twext = 840;
                *twosc = 840;
                break;
            case CLK_48M:
                *slp_dur = 600;
                *twext = 880;
                *twosc = 880;
                break;
            case CLK_32M:
                *slp_dur = 700;
                *twext = 1050;
                *twosc = 1050;
                break;
            case CLK_24M:
                *slp_dur = 780;
                *twext = 1200;
                *twosc = 1200;
                break;
            case CLK_16M:
                *slp_dur = 850;
                *twext = 1300;
                *twosc = 1300;
                break;
            default:
                *slp_dur = 1900;
                *twext = 10000;
                *twosc = 10000;
                break;
        }
    }
}

