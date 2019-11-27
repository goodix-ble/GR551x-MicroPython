#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "custom_config.h"
#include "gr55xx_sys.h"
#include "ble_task.h"

/*
 * LOCAL VARIABLE DECLARATION
 *****************************************************************************************
 */
static SemaphoreHandle_t g_xSeamp;
/**
 *****************************************************************************************
 * @brief The ble stack event notification callback function is called back by ROM code
 *        when any ble event needs to be processed
 *****************************************************************************************
 */
static void ble_stack_signal_callback(int type)
{
    BaseType_t var = pdFALSE;
    xSemaphoreGiveFromISR(g_xSeamp, &var);
}

/**
 *****************************************************************************************
 * @brief Create a task dedicated to running ble protocol stack under rtos. 
 *        all configurations of the task are fixed. Users should try not to modify it to 
 *        avoid problems.
 *
 * @param[in] arg: Pointer to args
 *****************************************************************************************
 */
void ble_schedule_task(void *arg)
{
    vSemaphoreCreateBinary(g_xSeamp);
    sys_signal_handler_register(ble_stack_signal_callback);
    while (1)
    {
        if (ble_is_powered())
        {           
              ble_sdk_schedule();            
              __disable_irq();
              ble_core_sleep();
              __enable_irq();
        }
        xSemaphoreTake(g_xSeamp, portMAX_DELAY);        
    }
}
