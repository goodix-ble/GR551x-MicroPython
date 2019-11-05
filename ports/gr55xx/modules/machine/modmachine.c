#include <stdio.h>

#include "modmachine.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "timer.h"


void machine_init(void) {
    
}

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
STATIC mp_obj_t machine_info(mp_uint_t n_args, const mp_obj_t *args) {  
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);


STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),           MP_ROM_QSTR(MP_QSTR_machine) },
    { MP_ROM_QSTR(MP_QSTR_info),               MP_ROM_PTR(&machine_info_obj) },
#if 1//MICROPY_PY_MACHINE_TIMER
    { MP_ROM_QSTR(MP_QSTR_Timer),              MP_ROM_PTR(&machine_timer_type) },
#endif    
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

