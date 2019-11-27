/**
 *****************************************************************************************
 *
 * @file dfu_port.c
 *
 * @brief  DFU port Implementation.
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
 
 /*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "dfu_port.h"
#include "hal_flash.h"
#include "otas.h"
#ifdef ENABLE_DFU_SPI_FLASH
    #include "gr551x_spi_flash.h"
#endif



/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
static void ble_send_data(uint8_t *p_data, uint16_t length); /**< BLE send data to peer device. */

static dfu_func_t dfu_func =                                 /**< DFU used functions config definition . */
{
    .dfu_ble_send_data      = ble_send_data,
    .dfu_uart_send_data     = NULL,
    .dfu_flash_read         = hal_flash_read,
    .dfu_flash_write        = hal_flash_write,
    .dfu_flash_erase        = hal_flash_erase,
    .dfu_flash_erase_chip   = hal_flash_erase_chip,
    .dfu_flash_set_security = hal_flash_set_security,
    .dfu_flash_get_security = hal_flash_get_security,
    .dfu_flash_get_info     = hal_flash_get_info,
};

static dfu_enter_callback dfu_enter_func = NULL;             /**< DFU enter callback. */

#ifdef ENABLE_DFU_SPI_FLASH

static void dfu_spi_flash_init(uint8_t cs_pin, uint8_t cs_mux, uint8_t spi_group); /**< SPI flash init. */
static dfu_spi_flash_func_t dfu_spi_flash_func=               /**< SPI used functions config definition. */
{
    .dfu_spi_flash_init = dfu_spi_flash_init,
    .dfu_spi_flash_read = spi_flash_read,
    .dfu_spi_flash_write = spi_flash_write,
    .dfu_spi_flash_erase = spi_flash_sector_erase,
    .dfu_spi_flash_erase_chip = spi_flash_chip_erase,
    .dfu_spi_flash_get_info = spi_flash_device_info,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @brief The function is used to config flash spi.
 *
 * @param[in] cs_pin: spi flash cs pin.
 * @param[in] cs_mux: spi flash cs mux.
 * @param[in] spi_group: spi group index.
 *****************************************************************************************
 */
static void dfu_spi_flash_init(uint8_t cs_pin, uint8_t cs_mux, uint8_t spi_group)
{
    const spi_flash_io_t spi_io[4] = {DEFAULT_SPIM_GROUP0, DEFAULT_SPIM_GROUP1,\
                                      DEFAULT_SPIM_GROUP2, DEFAULT_SPIM_GROUP3};
    const uint32_t gpio_pin[] = {APP_GPIO_PIN_0,APP_GPIO_PIN_1,APP_GPIO_PIN_2,APP_GPIO_PIN_3,APP_GPIO_PIN_4,APP_GPIO_PIN_5,APP_GPIO_PIN_6,APP_GPIO_PIN_7,\
                                 APP_GPIO_PIN_8,APP_GPIO_PIN_9,APP_GPIO_PIN_10,APP_GPIO_PIN_11,APP_GPIO_PIN_12,APP_GPIO_PIN_13,APP_GPIO_PIN_14,APP_GPIO_PIN_15,\
                                 APP_GPIO_PIN_16,APP_GPIO_PIN_17,APP_GPIO_PIN_18,APP_GPIO_PIN_19,APP_GPIO_PIN_20,APP_GPIO_PIN_21,APP_GPIO_PIN_22,APP_GPIO_PIN_23,\
                                 APP_GPIO_PIN_24,APP_GPIO_PIN_25,APP_GPIO_PIN_26,APP_GPIO_PIN_27,APP_GPIO_PIN_28,APP_GPIO_PIN_29,APP_GPIO_PIN_30,APP_GPIO_PIN_31,};
    const uint32_t gpio_pin_mux[] = {APP_GPIO_MUX_0,APP_GPIO_MUX_1,APP_GPIO_MUX_2,APP_GPIO_MUX_3,APP_GPIO_MUX_4,APP_GPIO_MUX_5,APP_GPIO_MUX_6,APP_GPIO_MUX_7,\
                                     APP_GPIO_MUX_8};
    
    spi_flash_io_t spi_config_io;
    if(spi_group < 5)
    {
        memcpy(&spi_config_io, &spi_io[spi_group], sizeof(spi_flash_io_t));
    }
    
    if(cs_pin < 32)
    {
        spi_config_io.spi_cs.gpio = APP_GPIO_TYPE_NORMAL;
        spi_config_io.spi_cs.pin = gpio_pin[cs_pin];
    }
    if(cs_mux < 9)
    {
        spi_config_io.spi_cs.mux = gpio_pin_mux[cs_mux];
    }
    spi_flash_init(&spi_config_io);
}

#endif

/**
 *****************************************************************************************
 * @brief Process ota service event.
 *
 * @param[in] p_evt: Pointer to otas event.
 *****************************************************************************************
 */
static void otas_evt_process(otas_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
        case OTAS_EVT_RX_RECEIVE_DATA:
            dfu_ble_receive_data_process(p_evt->p_data, p_evt->length);
            break;

        case OTAS_EVT_NOTIFY_COMPLETE:
            dfu_ble_send_data_cmpl_process();
            break;
        
        case OTAS_EVT_DFU_MODE_ENTER:
            if(dfu_enter_func != NULL)
            {
                dfu_enter_func();
            }
            break;

        default:
            break;
    }
}

/**
 *****************************************************************************************
 * @brief Send data to peer device by BLE.
 *
 * @param[in] p_data: Pointer to send data.
 * @param[in] length: Length of send data.
 *****************************************************************************************
 */
static void ble_send_data(uint8_t *p_data, uint16_t length)
{
    otas_notify_tx_data(0, p_data, length);
}


void dfu_port_init(dfu_uart_send_data uart_send_data, dfu_pro_callback_t *p_dfu_callback)
{
    if(uart_send_data != NULL)
    {
        dfu_func.dfu_uart_send_data = uart_send_data;
    }
    dfu_init(&dfu_func, p_dfu_callback);
#ifdef ENABLE_DFU_SPI_FLASH
    dfu_spi_flash_func_config(&dfu_spi_flash_func);
#endif
}

void dfu_service_init(dfu_enter_callback dfu_enter)
{
    if(dfu_enter != NULL)
    {
        dfu_enter_func = dfu_enter;
    }
    otas_init_t otas_init;
    otas_init.evt_handler = otas_evt_process;
    otas_service_init(&otas_init);
}
