#ifndef __GR55xx_MP_HAL_PORT_H__
#define __GR55xx_MP_HAL_PORT_H__

#include "mp_defs.h"

#define GR_UART_RX_BUFF_LEN         (256u)

typedef struct 
{
    unsigned char     *RingBufRxInPtr;                      /* Pointer to where next character will be inserted        */  
    unsigned char     *RingBufRxOutPtr;                     /* Pointer from where next character will be extracted     */  
    unsigned char      RingBufRx[GR_UART_RX_BUFF_LEN];      /* Ring buffer character storage (Rx)                      */  
    unsigned short     RingBufRxCtr;                        /* Number of characters in the Tx ring buffer              */  
} mp_uart_buff_t;

/********************************************************************
 *                            Sys Tick Porting
 ********************************************************************/

void        mp_hal_delay_ms(mp_uint_t ms);
void        mp_hal_delay_us(mp_uint_t us);

mp_uint_t   mp_hal_ticks_ms(void);
void        mp_hal_set_interrupt_char(char c);


/********************************************************************
 *                            Uart for Trace Porting
 ********************************************************************/

void        mp_hal_log_uart_init(void) ;
int         mp_hal_stdin_rx_chr(void);                                  // wait forever till Receive single character 
int         mp_hal_stdin_rx_chr_nowait(void);                           // Receive single character without wait, if no rx char, return -1
void        mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);     // Send string of given length

#endif /*__GR55xx_MP_HAL_PORT_H__*/
