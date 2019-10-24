/**
 ****************************************************************************************
 *
 * @file custom_config.h
 *
 * @brief Custom configuration file for applications.
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

/*
 * DEFINES
 *****************************************************************************************
 */
#ifndef __CUSTOM_CONFIG_H__
#define __CUSTOM_CONFIG_H__

// <<< Use Configuration Wizard in Context Menu >>>

// <h>basic configuration

// <o>enable hardfault callstack info print
//          <0=> DISABLE
//          <1=> ENABLE
#define ENABLE_FAULT_TRACE      0

// <o>APP log print
//          <0=> DISABLE
//          <1=> ENABLE
#define APP_LOG_ENABLE          1

// <o>DTM test support
//          <0=> DISABLE
//          <1=> ENABLE
#define DTM_TEST_ENABLE         0

// <o>BLE DFU support
//          <0=> DISABLE
//          <1=> ENABLE
#define DFU_ENABLE              1

// <o> The start address of NVDS
//  <i>Default: 0x0107E000
#define NVDS_START_ADDR         0x010FC000

// <o> The Number of NVDS sector, each sector is 4K
//  <i>Default:  1
#define NVDS_NUM_SECTOR         1             //<The Number of NVDS block, the size of a block is 4K. */

// <o> Call Stack Size
//  <i>Default: 0x8000
#define CSTACK_HEAP_SIZE        0x8000        //<Call Stack Size.

// <o> RAM size of Application
//  <i>Default: 0x00030000
#define APP_RAM_SIZE            0x00030000    //<RAM size of Application reserved. */

// <o> Code size of Application
//  <i>Default: 0x00800000
#define APP_MAX_CODE_SIZE       0x00800000    //<MAX size of Application reserved. */
// </h>


// <h>boot info configuration
// <o> Code load address
//  <i>Default:  0x01002000
#define APP_CODE_LOAD_ADDR      0x01002000
// <o> Code run address
//  <i>Default:  0x01002000
#define APP_CODE_RUN_ADDR       0x01002000

//<ol.0..5> System clock
//          <0=> 64MHZ
//          <1=> 48MHZ
//          <2=> 16MHZ-XO
//          <3=> 24MHZ
//          <4=> 16MHZ
//          <5=> 32MHZ-CPLL
//  <i>Default:  0
//  <i>(0:64Mhz, 1:48Mhz, 2:16Mhz(xo), 3:24Mhz, 4:16Mhz, 5:32Mhz(cpll))
#define SYSTEM_CLOCK            0             /**<System clock(0:64Mhz, 1:48Mhz, 2:16Mhz(xo), 3:24Mhz, 4:16Mhz, 5:32Mhz(cpll)) */

//   <o> External clock accuracy used in the LL to compute timing  <1-500>
//      <i>CFG_LF_ACCURACY_PPM
#define CFG_LF_ACCURACY_PPM     (500)

// <o>Delay time for Boot startup
//                      <0=> Not Delay
//                      <1=> Delay 1s
#define BOOT_LONG_TIME          1


#define VERSION                 1             /**<Code version.16bits */

// <o>Dap boot mode
//                      <0=> DISABLE
//                      <1=> ENABLE
#define DAP_BOOT_ENABLE         1

// </h>


// <h> ble resource configuration
//   <o> Support maximum number of BLE profile  <1-64>
//      <i>CFG_MAX_PRF_NB
#define CFG_MAX_PRF_NB          (10)
//   <o> Support maximum number of bonded devices  <1-10>
//      <i>CFG_MAX_BOND_DEV_NUM
#define CFG_MAX_BOND_DEV_NUM    (4)
//   <o> Support maximum number of BLE link  <1-10>
//      <i>CFG_MAX_CONNECTIONS
#define CFG_MAX_CONNECTIONS     (10)
// </h>

#define GR551xx_C0

//CHIP version define start
// C1 is the same with C0 except RF
#if defined(GR551xx_C1)
#endif

#if defined(GR551xx_C0)
#define CFG_PATCH_BY_FPB
#endif

#if defined(GR551xx_C2)
#define CFG_ISO_SUPPORT   // modularize the iso in ble_tool
#define ROM_RUN_IN_FLASH
#endif

#if defined(GR551xx_D0)

#endif
//CHIP version define end

// <<< end of configuration section >>>
#endif //__CUSTOM_CONFIG_H__
