

#include "py/nlr.h"
#include "py/runtime.h"
#include "timer.h"
#include "app_timer.h"

#if 1//MICROPY_PY_MACHINE_TIMER

enum {
    TIMER_0 = 0,
    TIMER_1,
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_5,
    TIMER_6,
    TIMER_7,
    
    TIMER_MAX       // must at the end
} ;

enum {
    TIMER_MODE_ONESHOT  = ATIMER_ONE_SHOT,
    TIMER_MODE_PERIODIC = ATIMER_REPEAT,
};

typedef struct _machine_timer_obj_t {
    const mp_obj_base_t base;
    const uint8_t       id;
    bool                is_used;    
    uint8_t             mode;
    uint32_t            period_ms;
    app_timer_id_t      handle;
    mp_obj_t            callback;
} machine_timer_obj_t;

extern const mp_obj_type_t machine_timer_type;

STATIC machine_timer_obj_t machine_timer_obj[TIMER_MAX] = {
    {{&machine_timer_type}, TIMER_0, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_1, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_2, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_3, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_4, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_5, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_6, false, TIMER_MODE_PERIODIC, 0, NULL, NULL},
    {{&machine_timer_type}, TIMER_7, false, TIMER_MODE_PERIODIC, 0, NULL, NULL}
};



STATIC int check_timer_id(mp_obj_t id) {
    // given an integer id
    int timer_id = mp_obj_get_int(id);
    if ( (timer_id >= TIMER_0) && (timer_id < TIMER_MAX) ) {
        if(!machine_timer_obj[timer_id].is_used){
            return timer_id;
        }
        else {
            mp_raise_ValueError("Timer ID is in used!");
        }
    }
    mp_raise_ValueError("Timer doesn't exist");
}

STATIC void reset_timer_id(uint8_t timer_id) {
    machine_timer_obj_t * timer_obj = NULL;
    
    if ( (timer_id >= TIMER_0) && (timer_id < TIMER_MAX) ) {
        timer_obj = &machine_timer_obj[timer_id];
        
        timer_obj->is_used      = false;
        timer_obj->mode         = TIMER_MODE_PERIODIC;
        timer_obj->period_ms    = 0;
        timer_obj->handle       = NULL;
        timer_obj->callback     = NULL;
    }
}

STATIC void timer_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_timer_obj_t *self = o;
    mp_printf(print, "Timer(%d | %d ms | %s)", self->id, self->period_ms, self->mode == TIMER_MODE_PERIODIC ? "PERIODIC" : "ONESHOT");
}

STATIC void timer_event_handler(void * p_obj) {
    machine_timer_obj_t *self = p_obj;
    mp_obj_t callback = machine_timer_obj[self->id].callback;
    printf("+++ Timer(%d) callback happened !\r\n", self->id);
    if (callback != NULL) {
        mp_call_function_1(callback, self->id);
    }
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id = 0, ARG_period, ARG_mode, ARG_callback };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ,                  {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} }, // 1000 ms
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int timer_id = check_timer_id(args[ARG_id].u_obj);

    machine_timer_obj_t *self = (machine_timer_obj_t*)&machine_timer_obj[timer_id];

    if (mp_obj_is_fun(args[ARG_callback].u_obj)) {
        self->callback = args[ARG_callback].u_obj;
    } else if (args[ARG_callback].u_obj == mp_const_none) {
        self->callback = NULL;
    } else {
        mp_raise_ValueError("callback must be a function");
    }
    
    self->period_ms = args[ARG_period].u_int;
    self->mode      = args[ARG_mode].u_int;

    printf("new, period: %d, mode:%d \r\n", self->period_ms, self->mode);
    
    if(SDK_SUCCESS != app_timer_create(&self->handle, self->mode, timer_event_handler)) {
        mp_raise_ValueError("unknown error happens for hardware timer!");
    }
        
    self->is_used = true;
    
    return MP_OBJ_FROM_PTR(self);
}

/// \method start()
/// Start the timer.
///
STATIC mp_obj_t machine_timer_start(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    uint32_t ret = 0;

    ret = app_timer_start(self->handle, self->period_ms, (void *) self);
    
    if(SDK_SUCCESS != ret) {
        static uint8_t buff[128];
        memset(&buff[0], 0 ,128);
        sprintf(&buff[0], "start timer failed, handle:0x%x, period:%d, reason: %d ",self->handle, self->period_ms, ret);
        mp_raise_ValueError(&buff[0]);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_start_obj, machine_timer_start);

/// \method stop()
/// Stop the timer.
///
STATIC mp_obj_t machine_timer_stop(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    app_timer_stop(self->handle);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_stop_obj, machine_timer_stop);

/// \method deinit()
/// Free resources associated with the timer.
///
STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t * self = MP_OBJ_TO_PTR(self_in);

    app_timer_delete(&self->handle);    
    reset_timer_id(self->id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start),    MP_ROM_PTR(&machine_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),     MP_ROM_PTR(&machine_timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),   MP_ROM_PTR(&machine_timer_deinit_obj) },

    // constants
    { MP_ROM_QSTR(MP_QSTR_ONESHOT),  MP_ROM_INT(TIMER_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};

STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_timer_locals_dict
};

#endif // MICROPY_PY_MACHINE_TIMER
