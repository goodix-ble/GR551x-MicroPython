
#include "py/runtime.h"

#include "mp_defs.h"
#include "mphalport.h"
#include "boards.h"
#include "gr55xx_hal.h"
#include "led.h"

#if MICROPY_PY_MACHINE_LED > 0u

typedef struct _board_led_obj_t {
    const mp_obj_base_t     base;
    const mp_uint_t         led_id;
    uint8_t                 led_state;
} board_led_obj_t;

static board_led_obj_t board_led_obj[] = {
    {{&board_led_type}, GR55XX_LED_1, LED_OFF},
    {{&board_led_type}, GR55XX_LED_2, LED_OFF}
};

#define NUM_LEDS MP_ARRAY_SIZE(board_led_obj)


static inline void led_off(board_led_obj_t * const led_obj) {
    
    switch(led_obj->led_id){        
        case GR55XX_LED_1:
        {
            ll_gpio_set_output_pin(LED_NUM_0_GRP, LED_NUM_0_IO);
            board_led_obj[GR55XX_LED_1].led_state = LED_OFF;
        }
        break;
        
        case GR55XX_LED_2:
        {
            hal_msio_write_pin(LED_NUM_1_IO, MSIO_PIN_SET);
            board_led_obj[GR55XX_LED_2].led_state = LED_OFF;
        }
        break;
    }
}

static inline void led_on(board_led_obj_t * const led_obj) {
    switch(led_obj->led_id){        
        case GR55XX_LED_1:
        {
            ll_gpio_reset_output_pin(LED_NUM_0_GRP, LED_NUM_0_IO);
            board_led_obj[GR55XX_LED_1].led_state = LED_ON;
        }
        break;
        
        case GR55XX_LED_2:
        {
            hal_msio_write_pin(LED_NUM_1_IO, MSIO_PIN_RESET);
            board_led_obj[GR55XX_LED_2].led_state = LED_ON;
        }
        break;
    }
}


void mp_led_init(void) {
    //use msio
    msio_init_t MSIO_Init   = MSIO_DEFAULT_CONFIG;
    MSIO_Init.pin           = LED_NUM_1_IO;
    MSIO_Init.direction     = MSIO_DIRECTION_OUTPUT;
    MSIO_Init.mode          = MSIO_MODE_DIGITAL;
    hal_msio_init(&MSIO_Init);

    //use gpio
    gpio_init_t gpio_config = GPIO_DEFAULT_CONFIG;
    gpio_config.mode        = GPIO_MODE_OUTPUT;
    gpio_config.pin         = LED_NUM_0_IO;
    hal_gpio_init(LED_NUM_0_GRP, &gpio_config); 
    
    for(int i = 0; i < NUM_LEDS; i++) {
        led_off(&board_led_obj[i]);
    }    
}

void led_state(board_led_obj_t * led_obj, int state) {
    if (state == 1) {
        led_on(led_obj);
    } else {
        led_off(led_obj);
    }
}

void led_toggle(board_led_obj_t * led_obj) {
    if(led_obj->led_state == LED_ON) {
        led_off(led_obj);
    } else {
        led_on(led_obj);
    }    
}


/******************************************************************************/
/* MicroPython bindings                                                      */

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    board_led_obj_t *self = self_in;
    mp_printf(print, "LED(%lu)", self->led_id);
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 1-2.
STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        mp_raise_ValueError("LED doesn't exist");
    }

    // return static led object
    return (mp_obj_t)&board_led_obj[led_id - 1];
}

/// \method on()
/// Turn the LED on.
mp_obj_t led_obj_on(mp_obj_t self_in) {
    board_led_obj_t *self = self_in;
    led_state(self, 1);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
mp_obj_t led_obj_off(mp_obj_t self_in) {
    board_led_obj_t *self = self_in;
    led_state(self, 0);
    return mp_const_none;
}

/// \method toggle()
/// Toggle the LED between on and off.
mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    board_led_obj_t *self = self_in;
    led_toggle(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);

STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&led_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle),  MP_ROM_PTR(&led_obj_toggle_obj) },
};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t board_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_dict_t*)&led_locals_dict,
};

#endif /*MICROPY_PY_MACHINE_LED*/
