#include <unistd.h>
#include "string.h"
#include "py/mpconfig.h"
#include "mphalport.h"


static mp_uart_buff_t s_mp_uart_buff;

static void mp_hal_log_uart_rx_char_produce(char ch){
    mp_uart_buff_t * p = &s_mp_uart_buff;

    p->RingBufRxCtr++;                                  /* No, increment character count            */  
    *p->RingBufRxInPtr++ = ch;                           /* Put character into buffer                */  
    if(p->RingBufRxInPtr == &p->RingBufRx[GR_UART_RX_BUFF_LEN])  
    {   /* Wrap IN pointer           */  
        p->RingBufRxInPtr = &p->RingBufRx[0];  
    }  
}

static int mp_hal_log_uart_rx_char_consume(char * ch){
    mp_uart_buff_t * p = &s_mp_uart_buff;
    char c;
    
    if(p->RingBufRxCtr > 0)  
    {   /* See if buffer is empty                   */  
        p->RingBufRxCtr--;                              /* No, decrement character count            */  
        c = *p->RingBufRxOutPtr++;                      /* Get character from buffer                */  
        if(p->RingBufRxOutPtr == &p->RingBufRx[GR_UART_RX_BUFF_LEN])  
        {   /* Wrap OUT pointer     */  
            p->RingBufRxOutPtr = &p->RingBufRx[0];  
        }  
        
        *ch = c;  

        return TRUE;                                        /* Characters are still available           */  
    }  

    return FALSE;
}

void mp_hal_log_uart_init(void) {    
    memset(&s_mp_uart_buff.RingBufRx[0], 0, GR_UART_RX_BUFF_LEN);
    s_mp_uart_buff.RingBufRxInPtr   = &s_mp_uart_buff.RingBufRx[0];
    s_mp_uart_buff.RingBufRxOutPtr  = &s_mp_uart_buff.RingBufRx[0];
    s_mp_uart_buff.RingBufRxCtr     = 0;    
}


/* called in USRT0 ISR */
void app_log_transfer_rx_char(char ch) {
    mp_hal_log_uart_rx_char_produce(ch);
}


// wait forever till Receive single character 
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;

    for (;;) 
    {
        if(TRUE == mp_hal_log_uart_rx_char_consume(&c)){
            return c;
        }

        mp_hal_delay_us(1);
    }

    return -1;
}

// Receive single character without wait, if no rx char, return -1
int mp_hal_stdin_rx_chr_nowait(void) {
    unsigned char c = 0;

    if(TRUE == mp_hal_log_uart_rx_char_consume(&c)){
        return c;
    }

    return -1;
}

extern int _write(int file, const char *buf, int len);

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    _write(0, str, len);
}
