/**
 ****************************************************************************************
 *
 * @file    gr55xx_ll_iso7816.h
 * @author  BLE Driver Team
 * @brief   Header file containing functions prototypes of ISO7816 LL library.
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

/** @addtogroup PERIPHERAL Peripheral Driver
  * @{
  */

/** @addtogroup LL_DRIVER LL Driver
  * @{
  */

/** @defgroup LL_ISO7816 ISO7816
  * @brief ISO7816 LL module driver.
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GR55xx_LL_ISO7816_H__
#define __GR55xx_LL_ISO7816_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gr55xx.h"

#if defined (ISO7816)
/** @defgroup ISO7816_LL_STRUCTURES Structures
  * @{
  */
/* Exported types ------------------------------------------------------------*/
/** @defgroup ISO7816_LL_ES_INIT ISO7816 Exported init structure
  * @{
  */

/**
  * @brief LL ISO7816 init Structure definition
  */
typedef struct _ll_iso7816_init
{
    uint32_t init;     /**< variable.                        */
} ll_iso7816_init_t;

/** @} */

/** @} */

/**
  * @defgroup  ISO7816_LL_MACRO Defines
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ISO7816_LL_Exported_Constants ISO7816 Exported Constants
  * @{
  */

/** @defgroup ISO7816_LL_EC_ACTION Action state.
  * @{
  */
#define LL_ISO7816_ACTION_NONE              0x00000000U             /**< Do Nothing.                        */
#define LL_ISO7816_ACTION_OFF               0x00000001U             /**< Switch Off.                        */
#define LL_ISO7816_ACTION_STOPCLK           0x00000002U             /**< Stop the clock.                    */
#define LL_ISO7816_ACTION_ON                0x00000003U             /**< Switch on and receive ATR.         */
#define LL_ISO7816_ACTION_WARMRST           0x00000004U             /**< Trigger warm reset and receive ATR.*/                       */
#define LL_ISO7816_ACTION_RX                0x00000005U             /**< Receive.                           */
#define LL_ISO7816_ACTION_TX                0x00000006U             /**< Transmit.                          */
#define LL_ISO7816_ACTION_TXRX              0x00000007U             /**< Transmit, followed by RX.          */
/** @} */

/** @defgroup ISO7816_LL_EC_IT IT Defines
  * @brief    Interrupt definitions which can be used with LL_ISO7816_ReadReg and  LL_ISO7816_WriteReg functions
  * @{
  */
#define LL_ISO7816_INTR_TEST                ISO7816_INTR_TEST       /**< Test interrupt                     */
#define LL_ISO7816_INTR_PRESENCE            ISO7816_INTR_PRESENCE   /**< Source presence interrupt          */
#define LL_ISO7816_INTR_STATE_ERR           ISO7816_INTR_STATE_ERR  /**< Source state error interrupt       */
#define LL_ISO7816_INTR_DMA_ERR             ISO7816_INTR_DMA_ERR    /**< Source dma error interrupt         */
#define LL_ISO7816_INTR_RETRY_ERR           ISO7816_INTR_RETRY_ERR  /**< Source retry error interrupt       */
#define LL_ISO7816_INTR_RX_ERR              ISO7816_INTR_RX_ERR     /**< Source rx error interrupt          */
#define LL_ISO7816_INTR_DONE                ISO7816_INTR_DONE       /**< Source done error interrupt        */

#define LL_ISO7816_INTR_MASK_ALL            ISO7816_INTR_ALL        /**< All interrupt */
/** @} */

/** @defgroup ISO7816_LL_EC_PRESENCE Card Presence Defines
  * @{
  */
#define LL_ISO7816_CARD_ABSENT              0x00000000U             /**< SIM Card is absent.   */
#define LL_ISO7816_CARD_PRESENT             0x00000001U             /**< SIM Card is present.  */
/** @} */

/** @defgroup ISO7816_LL_EC_IO_STATES IO States Defines
  * @{
  */
#define LL_ISO7816_IO_STATE_OFF             (0x0UL << ISO7816_STAT_IO_STATE_Pos)  /**< Off                    */
#define LL_ISO7816_IO_STATE_IDLE            (0x1UL << ISO7816_STAT_IO_STATE_Pos)  /**< Idle                   */
#define LL_ISO7816_IO_STATE_RX_WAIT         (0x4UL << ISO7816_STAT_IO_STATE_Pos)  /**< Receive Wait           */
#define LL_ISO7816_IO_STATE_RX              (0x5UL << ISO7816_STAT_IO_STATE_Pos)  /**< Receive                */
#define LL_ISO7816_IO_STATE_TX              (0x6UL << ISO7816_STAT_IO_STATE_Pos)  /**< Transmit               */
#define LL_ISO7816_IO_STATE_TX_GUARD        (0x7UL << ISO7816_STAT_IO_STATE_Pos)  /**< Transmit Guard         */
/** @} */

/** @defgroup ISO7816_LL_EC_PWR_STATES Power States Defines
  * @{
  */
#define LL_ISO7816_PWR_STATE_OFF            (0x0UL << ISO7816_STAT_PWR_STATE_Pos) /**< Off                    */
#define LL_ISO7816_PWR_STATE_PWRUP_VCC      (0x1UL << ISO7816_STAT_PWR_STATE_Pos) /**< Power up VCC           */
#define LL_ISO7816_PWR_STATE_PWRUP_RST      (0x2UL << ISO7816_STAT_PWR_STATE_Pos) /**< Power up reset         */
#define LL_ISO7816_PWR_STATE_PWRDN_RST      (0x3UL << ISO7816_STAT_PWR_STATE_Pos) /**< Power Down reset       */
#define LL_ISO7816_PWR_STATE_PWRDN_VCC      (0x4UL << ISO7816_STAT_PWR_STATE_Pos) /**< Power Down VCC         */
#define LL_ISO7816_PWR_STATE_STOP_PRE       (0x5UL << ISO7816_STAT_PWR_STATE_Pos) /**< Preparing Clock Stop   */
#define LL_ISO7816_PWR_STATE_STOP           (0x6UL << ISO7816_STAT_PWR_STATE_Pos) /**< Clock Stopped          */
#define LL_ISO7816_PWR_STATE_STOP_POST      (0x7UL << ISO7816_STAT_PWR_STATE_Pos) /**< Exiting Clock Stop     */
#define LL_ISO7816_PWR_STATE_IDLE           (0x8UL << ISO7816_STAT_PWR_STATE_Pos) /**< Idle                   */
#define LL_ISO7816_PWR_STATE_RX_TS0         (0x9UL << ISO7816_STAT_PWR_STATE_Pos) /**< RX TS Character        */
#define LL_ISO7816_PWR_STATE_RX_TS1         (0xAUL << ISO7816_STAT_PWR_STATE_Pos) /**< RX TS Character        */
#define LL_ISO7816_PWR_STATE_RX             (0xBUL << ISO7816_STAT_PWR_STATE_Pos) /**< Receive                */
#define LL_ISO7816_PWR_STATE_TX             (0xCUL << ISO7816_STAT_PWR_STATE_Pos) /**< Transmit               */
#define LL_ISO7816_PWR_STATE_TX_RX          (0xDUL << ISO7816_STAT_PWR_STATE_Pos) /**< Transmit and Receive   */
/** @} */

/** @defgroup ISO7816_LL_EC_CLKSTOP Clock Stop Select Defines
  * @{
  */
#define LL_ISO7816_CLKSTOP_LOW              (0x00000000UL)     /**< Stop the clock at low level.  */
#define LL_ISO7816_CLKSTOP_HIGH             (0x80000000UL)     /**< Stop the clock at high level. */
/** @} */

/** @defgroup ISO7816_LL_EC_VCCSEL VCC Select Defines
  * @{
  */
#define LL_ISO7816_VCC_OFF                  (0x00000000UL)     /**< Power Off.      */
#define LL_ISO7816_VCC_5_0                  (0x00000001UL)     /**< Power on 5.0V.  */
#define LL_ISO7816_VCC_3_0                  (0x00000002UL)     /**< Power on 3.0V.  */
#define LL_ISO7816_VCC_1_8                  (0x00000003UL)     /**< Power on 1.8V.  */
/** @} */

/** @defgroup ISO7816_LL_EC_CODING Coding Convention Defines
  * @{
  */
#define LL_ISO7816_CODING_DEFUALT           (0x00000000UL)     /**< High=1, LSB first. */
#define LL_ISO7816_CODING_INVERSE           (0x00000001UL)     /**< High=0, MSB first. */
/** @} */

/** @} */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup ISO7816_LL_Exported_Macros ISO7816 Exported Macros
  * @{
  */

/** @defgroup ISO7816_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in ISO7816 register
  * @param  __instance__ ISO7816 instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None.
  */
#define LL_ISO7816_WriteReg(__instance__, __REG__, __VALUE__) WRITE_REG(__instance__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in ISO7816 register
  * @param  __instance__ ISO7816 instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_ISO7816_ReadReg(__instance__, __REG__) READ_REG(__instance__->__REG__)

/** @} */

/** @} */

/** @} */


/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup ISO7816_LL_DRIVER_FUNCTIONS Functions
  * @{
  */

/** @defgroup ISO7816_LL_EF_Configuration Configuration
  * @{
  */
/**
  * @brief  Request ISO7816 to go to the next action.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | ACTION
  *
  * @param  ISO7816x ISO7816 instance.
  * @param  action This parameter can be one of the following values:
  *         @arg @ref LL_ISO7816_ACTION_NONE
  *         @arg @ref LL_ISO7816_ACTION_OFF
  *         @arg @ref LL_ISO7816_ACTION_STOPCLK
  *         @arg @ref LL_ISO7816_ACTION_ON
  *         @arg @ref LL_ISO7816_ACTION_WARMRST
  *         @arg @ref LL_ISO7816_ACTION_RX
  *         @arg @ref LL_ISO7816_ACTION_TX
  *         @arg @ref LL_ISO7816_ACTION_TXRX
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_action(iso7816_regs_t *ISO7816x, uint32_t action)
{
    WRITE_REG(ISO7816x->CTRL, action);
}

/**
  * @brief  Clear Transmit Retries Maximum.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | TX_RETRY_MAX_CLR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_tx_retry_max(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_CTRL_TX_RETRY_MAX_CLR);
}

/**
  * @brief  Clear Receive Retries Maximum.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | RX_RETRY_MAX_CLR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_rx_retry_max(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_CTRL_RX_RETRY_MAX_CLR);
}

/**
  * @brief  Check Card presence.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | PRESENCE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ISO7816_CARD_ABSENT
  *         @arg @ref LL_ISO7816_CARD_PRESENT
  */
__STATIC_INLINE uint32_t ll_iso7816_check_card_presence(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STAT, ISO7816_STAT_PRESENCE) >> ISO7816_STAT_PRESENCE_Pos);
}

/**
  * @brief  Get Maximum number of seen transmit retries after error signaling by ISO7816.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | TX_RETRY_MAX
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Value range between 0x1 and 0x7.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_tx_retry_max(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STAT, ISO7816_STAT_TX_RETRY_MAX) >> ISO7816_STAT_TX_RETRY_MAX_Pos);
}

/**
  * @brief  Get Maximum number of seen receive retries after error signaling by ISO7816.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | RX_RETRY_MAX
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Value range between 0x1 and 0x7.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_rx_retry_max(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STAT, ISO7816_STAT_RX_RETRY_MAX) >> ISO7816_STAT_RX_RETRY_MAX_Pos);
}

/**
  * @brief  Get ISO7816 IO States.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | IO_STATE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Returned value can be one or combination of the following values:
  *         @arg @ref LL_ISO7816_IO_STATE_OFF
  *         @arg @ref LL_ISO7816_IO_STATE_IDLE
  *         @arg @ref LL_ISO7816_IO_STATE_RX_WAIT
  *         @arg @ref LL_ISO7816_IO_STATE_RX
  *         @arg @ref LL_ISO7816_IO_STATE_TX
  *         @arg @ref LL_ISO7816_IO_STATE_TX_GUARD
  */
__STATIC_INLINE uint32_t ll_iso7816_get_io_states(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STAT, ISO7816_STAT_IO_STATE) >> ISO7816_STAT_IO_STATE_Pos);
}

/**
  * @brief  Get ISO7816 Power States.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | PWR_STATE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ISO7816_PWR_STATE_OFF
  *         @arg @ref LL_ISO7816_PWR_STATE_PWRUP_VCC
  *         @arg @ref LL_ISO7816_PWR_STATE_PWRUP_RST
  *         @arg @ref LL_ISO7816_PWR_STATE_PWRDN_RST
  *         @arg @ref LL_ISO7816_PWR_STATE_PWRDN_VCC
  *         @arg @ref LL_ISO7816_PWR_STATE_STOP_PRE
  *         @arg @ref LL_ISO7816_PWR_STATE_STOP
  *         @arg @ref LL_ISO7816_PWR_STATE_STOP_POST
  *         @arg @ref LL_ISO7816_PWR_STATE_IDLE
  *         @arg @ref LL_ISO7816_PWR_STATE_RX_TS0
  *         @arg @ref LL_ISO7816_PWR_STATE_RX_TS1
  *         @arg @ref LL_ISO7816_PWR_STATE_RX
  *         @arg @ref LL_ISO7816_PWR_STATE_TX
  *         @arg @ref LL_ISO7816_PWR_STATE_TX_RX
  */
__STATIC_INLINE uint32_t ll_iso7816_get_power_states(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STAT, ISO7816_STAT_PWR_STATE));
}

/**
  * @brief  Set value of the clock output during stopped Clock.
  *
  *  Register|BitsName
  *  --------|--------
  *  CLK_CFG | CLK_STOP_SEL
  *
  * @param  ISO7816x ISO7816 instance.
  * @param  level This parameter can be one of the following values:
  *         @arg @ref LL_ISO7816_CLKSTOP_LOW
  *         @arg @ref LL_ISO7816_CLKSTOP_HIGH
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_clkstop_level(iso7816_regs_t *ISO7816x, uint32_t level)
{
    MODIFY_REG(ISO7816x->CLK_CFG, ISO7816_CLK_CFG_CLK_STOP_SEL, level);
}

/**
  * @brief  Get value of the clock output during stopped Clock.
  *
  *  Register|BitsName
  *  --------|--------
  *  CLK_CFG | CLK_STOP_SEL
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ISO7816_CLKSTOP_LOW
  *         @arg @ref LL_ISO7816_CLKSTOP_HIGH
  */
__STATIC_INLINE uint32_t ll_iso7816_get_clkstop_level(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->CLK_CFG, ISO7816_CLK_CFG_CLK_STOP_SEL));
}

/**
  * @brief  Set clock division.
  * @note   Divide system clock by this value+1.
  *
  *  Register|BitsName
  *  --------|--------
  *  CLK_CFG | CLK_DIV
  *
  * @param  ISO7816x ISO7816 instance
  * @param  value This parameter should range between 0x0 and 0xFF.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_clkdiv(iso7816_regs_t *ISO7816x, uint32_t value)
{
    MODIFY_REG(ISO7816x->CLK_CFG, ISO7816_CLK_CFG_CLK_DIV, value << ISO7816_CLK_CFG_CLK_DIV_Pos);
}

/**
  * @brief  Get clock division.
  *
  *  Register|BitsName
  *  --------|--------
  *  CLK_CFG | CLK_DIV
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value should range between 0x0 and 0xFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_clkdiv(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->CLK_CFG, ISO7816_CLK_CFG_CLK_DIV));
}

/**
  * @brief  Set divide ISO7816 clock.
  * @note   Divide SIM clock by this value+1 to define ETU length. The reset  value
  *         is the one, needed for theATR.
  *
  *  Register|BitsName
  *  --------|--------
  *  CLK_CFG | ETU_DIV
  *
  * @param  ISO7816x ISO7816 instance
  * @param  divide This parameter should range between 0x0 and 0x3FF.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_etudiv(iso7816_regs_t *ISO7816x, uint32_t divide)
{
    MODIFY_REG(ISO7816x->CLK_CFG, ISO7816_CLK_CFG_ETU_DIV, divide);
}

/**
  * @brief  Get divide ISO7816 clock.
  *
  *  Register|BitsName
  *  --------|--------
  *  CLK_CFG | ETU_DIV
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value should range between 0x0 and 0x3FF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_etudiv(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->CLK_CFG, ISO7816_CLK_CFG_ETU_DIV));
}

/**
  * @brief  Set ISO7816 VCC voltage level.
  *
  *  Register|BitsName
  *  --------|--------
  *  VCC_CFG | VCC_SEL
  *
  * @param  ISO7816x ISO7816 instance.
  * @param  power This parameter can be one of the following values:
  *         @arg @ref LL_ISO7816_VCC_OFF
  *         @arg @ref LL_ISO7816_VCC_5_0
  *         @arg @ref LL_ISO7816_VCC_3_0
  *         @arg @ref LL_ISO7816_VCC_1_8
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_vcc_sel(iso7816_regs_t *ISO7816x, uint32_t power)
{
    WRITE_REG(ISO7816x->VCC_CFG, power);
}

/**
  * @brief  Get ISO7816 VCC voltage level.
  *
  *  Register|BitsName
  *  --------|--------
  *  VCC_CFG | VCC_SEL
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ISO7816_VCC_OFF
  *         @arg @ref LL_ISO7816_VCC_5_0
  *         @arg @ref LL_ISO7816_VCC_3_0
  *         @arg @ref LL_ISO7816_VCC_1_8
  */
__STATIC_INLINE uint32_t ll_iso7816_get_vcc_sel(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_REG(ISO7816x->VCC_CFG));
}

/**
  * @brief  Get maximum card response time(leading edge to leading edge)..
  *
  *  Register|BitsName
  *  --------|--------
  *  TIMES   | WAITTIME
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value should range between 0x0 and 0x3FFFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_waittime(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->TIMES, ISO7816_TIMES_WAITTIME) >> ISO7816_TIMES_WAITTIME_Pos);
}

/**
  * @brief  Get time between the leading edges of two consecutive characters.
  *
  *  Register|BitsName
  *  --------|--------
  *  TIMES   | GDUARDTIME
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value should range between 0x0 and 0x3FF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_guardtime(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->TIMES, ISO7816_TIMES_DUARDTIME));
}

/**
  * @brief  Set maximum number of issued retries before giving up.
  *
  *  Register|BitsName
  *  --------|--------
  *  DATA_CFG| RETRY_LIMIT
  *
  * @param  ISO7816x ISO7816 instance
  * @param  number This parameter should range between 0x0 and 0x7.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_retry_limit(iso7816_regs_t *ISO7816x, uint32_t number)
{
    MODIFY_REG(ISO7816x->DATA_CFG, ISO7816_DATA_CFG_RETRY_LIMIT, number << ISO7816_DATA_CFG_RETRY_LIMIT_Pos);
}

/**
  * @brief  Get maximum number of issued retries before giving up.
  *
  *  Register|BitsName
  *  --------|--------
  *  DATA_CFG| RETRY_LIMIT
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value should range between 0x0 and 0x7.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_retry_limit(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->DATA_CFG, ISO7816_DATA_CFG_RETRY_LIMIT) >> ISO7816_DATA_CFG_RETRY_LIMIT_Pos);
}

/**
  * @brief  Enable coding detection.
  *
  *  Register|BitsName
  *  --------|--------
  *  DATA_CFG| DETECT_CODING
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_enable_coding_detection(iso7816_regs_t *ISO7816x)
{
    SET_BITS(ISO7816x->DATA_CFG, ISO7816_DATA_CFG_DETECT_CODING);
}

/**
  * @brief  Disable coding detection.
  *
  *  Register|BitsName
  *  --------|--------
  *  DATA_CFG| DETECT_CODING
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_disable_coding_detection(iso7816_regs_t *ISO7816x)
{
    CLEAR_BITS(ISO7816x->DATA_CFG, ISO7816_DATA_CFG_DETECT_CODING);
}

/**
  * @brief  Set coding convention.
  *
  *  Register|BitsName
  *  --------|--------
  *  DATA_CFG| CODING
  *
  * @param  ISO7816x ISO7816 instance.
  * @param  convention This parameter can be one of the following values:
  *         @arg @ref LL_ISO7816_CODING_DEFUALT
  *         @arg @ref LL_ISO7816_CODING_INVERSE
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_coding_convention(iso7816_regs_t *ISO7816x, uint32_t convention)
{
    MODIFY_REG(ISO7816x->DATA_CFG, ISO7816_DATA_CFG_CODING, convention);
}

/**
  * @brief  Get coding convention.
  *
  *  Register|BitsName
  *  --------|--------
  *  DATA_CFG| CODING
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ISO7816_CODING_DEFUALT
  *         @arg @ref LL_ISO7816_CODING_INVERSE
  */
__STATIC_INLINE uint32_t ll_iso7816_get_coding_convention(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->DATA_CFG, ISO7816_DATA_CFG_CODING));
}

/**
  * @brief  Get current address relative to base_addr.
  *
  *  Register|BitsName
  *  --------|--------
  *  ADDR    | ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3FFFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_current_addr(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->ADDR, ISO7816_ADDR_CUR) >> ISO7816_ADDR_CUR_Pos);
}

/**
  * @brief  Get address fraction.
  *
  *  Register|BitsName
  *  --------|--------
  *  ADDR    | ADDR_FRAC
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_current_addr_frac(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->ADDR, ISO7816_ADDR_FRAC));
}

/**
  * @brief  Set base address for RX and TX buffer.
  *
  *  Register |BitsName
  *  ---------|--------
  *  STRT_ADDR| BASE_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @param  addr This parameter should range between 0x0 and 0xFFF.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_base_addr(iso7816_regs_t *ISO7816x, uint32_t addr)
{
    MODIFY_REG(ISO7816x->STRT_ADDR, ISO7816_STRT_ADDR_BASE_ADDR, addr << ISO7816_STRT_ADDR_BASE_ADDR_Pos);
}

/**
  * @brief  Get base address for RX and TX buffer.
  *
  *  Register |BitsName
  *  ---------|--------
  *  STRT_ADDR| BASE_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0xFFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_base_addr(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STRT_ADDR, ISO7816_STRT_ADDR_BASE_ADDR) >> ISO7816_STRT_ADDR_BASE_ADDR_Pos);
}

/**
  * @brief  Set start address for RX and TX buffer, relative to base_addr.
  *
  *  Register |BitsName
  *  ---------|--------
  *  STRT_ADDR| STRT_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @param  addr This parameter should range between 0x0 and 0x3FFFF.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_start_addr(iso7816_regs_t *ISO7816x, uint32_t addr)
{
    MODIFY_REG(ISO7816x->STRT_ADDR, ISO7816_STRT_ADDR_STRT_ADDR, addr << ISO7816_STRT_ADDR_STRT_ADDR_Pos);
}

/**
  * @brief  Get start address for RX and TX buffer, relative to base_addr.
  *
  *  Register |BitsName
  *  ---------|--------
  *  STRT_ADDR| STRT_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3FFFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_start_addr(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->STRT_ADDR, ISO7816_STRT_ADDR_STRT_ADDR) >> ISO7816_STRT_ADDR_STRT_ADDR_Pos);
}

/**
  * @brief  Set end address of receive buffer, relative to base_addr.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  RX_END_ADDR| RX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @param  addr This parameter should range between 0x0 and 0x3FFFF.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_rx_end_addr(iso7816_regs_t *ISO7816x, uint32_t addr)
{
    MODIFY_REG(ISO7816x->RX_END_ADDR, ISO7816_RX_END_ADDR, addr << ISO7816_RX_END_ADDR_Pos);
}

/**
  * @brief  Get end address of receive buffer, relative to base_addr.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  RX_END_ADDR| RX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3FFFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_rx_end_addr(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->RX_END_ADDR, ISO7816_RX_END_ADDR) >> ISO7816_RX_END_ADDR_Pos);
}

/**
  * @brief  Set RX end address fraction.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  RX_END_ADDR| RX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @param  frac This parameter should range between 0x0 and 0x3.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_rx_end_addr_frac(iso7816_regs_t *ISO7816x, uint32_t frac)
{
    MODIFY_REG(ISO7816x->RX_END_ADDR, ISO7816_RX_END_ADDR_FRAC, frac);
}

/**
  * @brief  Get RX end address fraction.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  RX_END_ADDR| RX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_rx_end_addr_frac(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->RX_END_ADDR, ISO7816_RX_END_ADDR_FRAC));
}

/**
  * @brief  Set end address of transmit  buffer, relative to base_addr.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  TX_END_ADDR| TX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @param  addr This parameter should range between 0x0 and 0x3FFFF.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_tx_end_addr(iso7816_regs_t *ISO7816x, uint32_t addr)
{
    MODIFY_REG(ISO7816x->TX_END_ADDR, ISO7816_TX_END_ADDR, addr << ISO7816_TX_END_ADDR_Pos);
}

/**
  * @brief  Get end address of transmit buffer, relative to base_addr.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  TX_END_ADDR| TX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3FFFF.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_tx_end_addr(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->TX_END_ADDR, ISO7816_TX_END_ADDR) >> ISO7816_TX_END_ADDR_Pos);
}

/**
  * @brief  Set TX end address fraction.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  TX_END_ADDR| TX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @param  frac This parameter should range between 0x0 and 0x3.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_set_tx_end_addr_frac(iso7816_regs_t *ISO7816x, uint32_t frac)
{
    MODIFY_REG(ISO7816x->TX_END_ADDR, ISO7816_TX_END_ADDR_FRAC, frac);
}

/**
  * @brief  Get TX end address fraction.
  *
  *  Register   | BitsName
  *  -----------|--------
  *  TX_END_ADDR| TX_END_ADDR
  *
  * @param  ISO7816x ISO7816 instance
  * @retval Value between 0x0 and 0x3.
  */
__STATIC_INLINE uint32_t ll_iso7816_get_tx_end_addr_frac(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_BITS(ISO7816x->TX_END_ADDR, ISO7816_TX_END_ADDR_FRAC));
}

/** @} */

/** @defgroup ISO7816_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief  Get ISO7816 interrupt flags
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_TEST
  *  STAT    | STAT_PRESENCE
  *  STAT    | STAT_STATE_ERR
  *  STAT    | STAT_DMA_ERR
  *  STAT    | STAT_RETRY_ERR
  *  STAT    | STAT_RX_ERR
  *  STAT    | STAT_DONE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval Returned value can be one or combination of the following values:
  *         @arg @ref LL_ISO7816_INTR_TEST
  *         @arg @ref LL_ISO7816_INTR_PRESENCE
  *         @arg @ref LL_ISO7816_INTR_STATE_ERR
  *         @arg @ref LL_ISO7816_INTR_DMA_ERR
  *         @arg @ref LL_ISO7816_INTR_RETRY_ERR
  *         @arg @ref LL_ISO7816_INTR_RX_ERR
  *         @arg @ref LL_ISO7816_INTR_DONE
*/
__STATIC_INLINE uint32_t ll_iso7816_get_it_flag(iso7816_regs_t *ISO7816x)
{
    return (uint32_t)(READ_REG(ISO7816x->STAT) & ISO7816_INTR_ALL);
}

/**
  * @brief  Indicate the status of STAT_TEST flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_TEST
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_test(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_TEST) == (ISO7816_INTR_TEST));
}

/**
  * @brief  Indicate the status of STAT_PRESENCE flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_PRESENCE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_presence(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_PRESENCE) == (ISO7816_INTR_PRESENCE));
}

/**
  * @brief  Indicate the status of STAT_STATE_ERR flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_STATE_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_state_err(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_STATE_ERR) == (ISO7816_INTR_STATE_ERR));
}

/**
  * @brief  Indicate the status of STAT_DMA_ERR flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_DMA_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_dma_err(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_DMA_ERR) == (ISO7816_INTR_DMA_ERR));
}

/**
  * @brief  Indicate the status of STAT_RETRY_ERR flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_RETRY_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_retry_err(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_RETRY_ERR) == (ISO7816_INTR_RETRY_ERR));
}

/**
  * @brief  Indicate the status of STAT_RX_ERR flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_RX_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_rx_err(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_RX_ERR) == (ISO7816_INTR_RX_ERR));
}

/**
  * @brief  Indicate the status of STAT_DONE flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_DONE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t ll_iso7816_is_active_flag_done(iso7816_regs_t *ISO7816x)
{
    return (READ_BITS(ISO7816x->STAT, ISO7816_INTR_DONE) == (ISO7816_INTR_DONE));
}

/**
  * @brief  Clear the combined interrupt, all individual interrupts, and the STAT register
  *
  *  Register|BitsName
  *  --------|--------
  *  STAT    | STAT_TEST
  *  STAT    | STAT_PRESENCE
  *  STAT    | STAT_STATE_ERR
  *  STAT    | STAT_DMA_ERR
  *  STAT    | STAT_RETRY_ERR
  *  STAT    | STAT_RX_ERR
  *  STAT    | STAT_DONE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_intr(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_ALL);
}

/**
  * @brief  Clear test flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_TEST
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_test(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_TEST);
}

/**
  * @brief  Clear presence flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_PRESENCE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_presence(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_PRESENCE);
}

/**
  * @brief  Clear state error flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_STATE_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_state_err(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_STATE_ERR);
}

/**
  * @brief  Clear dma error flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_DMA_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_dma_err(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_DMA_ERR);
}

/**
  * @brief  Clear retry error flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_RETRY_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_retry_err(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_RETRY_ERR);
}

/**
  * @brief  Clear RX error flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_RX_ERR
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_rx_err(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_RX_ERR);
}

/**
  * @brief  Clear done flag.
  *
  *  Register|BitsName
  *  --------|--------
  *  CTRL    | CTRL_DONE
  *
  * @param  ISO7816x ISO7816 instance.
  * @retval None.
  */
__STATIC_INLINE void ll_iso7816_clear_flag_done(iso7816_regs_t *ISO7816x)
{
    WRITE_REG(ISO7816x->CTRL, ISO7816_INTR_DONE);
}

/** @} */

/** @} */


#endif /* ISO7816 */

#ifdef __cplusplus
}
#endif

#endif /* __GR55xx_LL_ISO7816_H__ */

/** @} */

/** @} */

/** @} */

