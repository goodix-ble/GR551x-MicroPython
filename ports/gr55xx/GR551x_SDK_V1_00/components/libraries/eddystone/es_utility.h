/**
 ****************************************************************************************
 *
 * @file es_utility.h
 *
 * @brief Header file - eddystone utility
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
 *****************************************************************************************
 */
#ifndef APP_ERROR_H__
#define APP_ERROR_H__
#include <stdio.h>
#include "system_sdk.h"
#include "custom_config.h"
#include "dbg_define.h"
#include "ble_sdk_error.h"

/**@brief If error occurs,print current error code,function information,and then return erroe code */  
#define RETURN_IF_ERROR(result,expected_rsp)                       \
            do                                                      \
            {                                                       \
                uint32_t _err_code = (uint32_t) (result);        \
                uint32_t _rsp_expected = (uint32_t) (expected_rsp);        \
                if (_err_code != _rsp_expected)                       \
                {                                                   \
                    DEBUG_PRINTF("%s:line %d-<%s()>:",__FILE__,__LINE__,__FUNCTION__);              \
                    DEBUG_PRINTF("error code:0x%x\r\n",_err_code);                        \
                    return result;                        \
                }                               \
            } while(0)
            
/**@brief Print current error code and function information if error */                  
#define ERROR_CODE_PRINT(result,expected_rsp)                       \
            do                                                      \
            {                                                       \
                uint32_t _err_code = (uint32_t) (result);        \
                uint32_t _rsp_expected = (uint32_t) (expected_rsp);        \
                if (_err_code != _rsp_expected)                       \
                {                                                   \
                    DEBUG_PRINTF("%s:line %d-<%s()>:",__FILE__,__LINE__,__FUNCTION__);              \
                    DEBUG_PRINTF("error code:0x%x\r\n",_err_code);                        \
                }                                                   \
            } while(0)

/**@brief Macros for printing current function information */            
#define CURRENT_FUNC_INFO_PRINT() DEBUG_PRINTF("%s:line %d-<%s()>\r\n",__FILE__,__LINE__,__FUNCTION__);
      
/**@brief Macros for conversion of 128bit to 16bit UUID */
#define ATT_128_PRIMARY_SERVICE BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_PRIMARY_SERVICE)
#define ATT_128_CHARACTERISTIC  BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DECL_CHARACTERISTIC)
#define ATT_128_CLIENT_CHAR_CFG BLE_ATT_16_TO_128_ARRAY(BLE_ATT_DESC_CLIENT_CHAR_CFG)
                
#endif // APP_ERROR_H__
