
#ifndef __GR55XX_LED_H__
#define __GR55XX_LED_H__

//#include "py/obj.h"
#include "gr55xx_hal.h"

typedef enum {
    GR55XX_LED_1 = 0,
    GR55XX_LED_2 = 1
} gr55xx_led_e;

#define LED_ON      (1)
#define LED_OFF     (0)

void mp_led_init(void);

extern const mp_obj_type_t board_led_type;

#endif // __GR55XX_LED_H__
