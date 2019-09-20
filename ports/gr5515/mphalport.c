#include "mphalport.h"
#include "gr55xx_sys.h"

void mp_hal_delay_ms(mp_uint_t ms) {
    sys_delay_ms(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    sys_delay_us(us);    
}

mp_uint_t mp_hal_ticks_ms(void) { 
    return 0; 
}

void mp_hal_set_interrupt_char(char c) {
    
}