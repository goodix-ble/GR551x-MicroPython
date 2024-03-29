/**
 ****************************************************************************************
 *
 * @file    app_gpiote.h
 * @author  BLE Driver Team
 * @brief   Header file containing functions prototypes of GPIO Interrupt app library.
 *
 ****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************
 */

/**
 @addtogroup PERIPHERAL APP DRIVER
 @{
*/

/**
  @addtogroup PERIPHERAL_API_HAL_APP_GPIO_INTERRUPT_DRIVER HAL APP GPIO Interrupt Interface
  @{
  @brief Definitions and prototypes for HAL APP DRIVER Interface.
 */

#ifndef _APP_GPIOTE_H_
#define _APP_GPIOTE_H_

#include "app_io.h"
#include "app_drv_error.h"

/** @addtogroup HAL_APP_GPIO_INTERRUPT_STRUCTURES Structures
  * @{
  */

/**
  * @brief GPIO Interrupt event Structure definition
  */
typedef struct
{
    app_io_type_t           type;       /**< Type of event. */
    uint32_t                pin;
    app_ctx_type_t          ctx_type;
} app_gpiote_evt_t;

/**
  * @brief GPIOTE input event handler type.
  */
typedef void (*app_gpiote_event_handler_t)(app_gpiote_evt_t *p_evt);

typedef void (*app_io_callback_t)(app_gpiote_evt_t *p_evt);

/**
  * @brief GPIO Interrupt parameters structure definition
  */
typedef struct
{
    app_io_type_t           type;           /** Pin number to observe */
    uint32_t                pin;
    app_io_mode_t           mode;
    app_io_pull_t           pull; 
    app_handle_mode_t       handle_mode;
    app_io_callback_t       io_evt_cb;
} app_gpiote_param_t;

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup HAL_APP_GPIOTE_DRIVER_FUNCTIONS Functions
  * @{
  */

/**
 ****************************************************************************************
 * @brief  Initialize the APP GPIO Interrupt DRIVER according to the specified parameters
 *         in the app_gpiote_param_t and app_gpiote_event_handler_t.
 *
 * @param[in]  p_params: Pointer to app_gpiote_param_t parameter which contains the
 *                       configuration information for the specified GPIO.
 * @param[in]  evt_handler: GPIO Interrupt user callback function.
 *
 * @return Result of initialization.
 ****************************************************************************************
 */
uint16_t app_gpiote_init(const app_gpiote_param_t *p_params, uint8_t table_cnt);

/**
 ****************************************************************************************
 * @brief  De-initialize the APP GPIO Interrupt DRIVER peripheral.
 *
 ****************************************************************************************
 */
void app_gpiote_deinit(void);
/** @} */

#endif

/** @} */
/** @} */
