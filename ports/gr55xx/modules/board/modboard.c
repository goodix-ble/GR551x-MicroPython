
#include "py/builtin.h"
#include "lib/utils/pyexec.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "led.h"
#include "mp_defs.h"


STATIC const mp_rom_map_elem_t board_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gr5515_sk) },             //set board name as 'goodix'
    { MP_ROM_QSTR(MP_QSTR_repl_info), MP_ROM_PTR(&pyb_set_repl_info_obj) },
#if MICROPY_PY_MACHINE_LED > 0u
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&board_led_type) },
#endif
};


STATIC MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);

const mp_obj_module_t mp_module_board = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&board_module_globals,
};
