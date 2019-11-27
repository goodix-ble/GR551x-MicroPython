/**
 *****************************************************************************************
 *
 * @file ble_task.h
 *
 * @brief BLE task Module API
 *
 *****************************************************************************************
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
 *****************************************************************************************
 */

/**
 * @defgroup BLE task module
 * @{
 * @brief Definitions and prototypes for the BLE Module interface.
 */
 
#ifndef __BLE_TASK_HEADER_FILE_
#define __BLE_TASK_HEADER_FILE_

/**
 *****************************************************************************************
 * @brief This function includes the scheduler of the whole ble protocol stack, and also 
 *        contains the core sleep entry function of ble. User-level code should try to use 
 *        this function, and try not to use sdk_schedule directly.
 *
 * @param[in] void
 *****************************************************************************************
 */
extern void ble_task_main(void);

/**
 *****************************************************************************************
 * @brief Create a task dedicated to running ble protocol stack under rtos. 
 *        all configurations of the task are fixed. Users should try not to modify it to 
 *        avoid problems.
 *
 * @param[in] arg: Pointer to args
 *****************************************************************************************
 */
extern void ble_schedule_task(void *arg);

#endif
