/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 nix.long
 *
 */

#include "py/obj.h"
#include "mp_defs.h"

#if MICROPY_PY_XBLEPY

extern const mp_obj_type_t xblepy_uuid_type;
extern const mp_obj_type_t xblepy_constants_type;
extern const mp_obj_type_t xblepy_delegate_type;
extern const mp_obj_type_t xblepy_descriptor_type;
extern const mp_obj_type_t xblepy_characteristic_type;
extern const mp_obj_type_t xblepy_service_type;
extern const mp_obj_type_t xblepy_device_type;
extern const mp_obj_type_t xblepy_peripheral_type;

/***




extern const mp_obj_type_t xblepy_scanner_type;
extern const mp_obj_type_t xblepy_scan_entry_type;
***/

STATIC const mp_rom_map_elem_t mp_module_xblepy_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_xblepy) },
    { MP_ROM_QSTR(MP_QSTR_UUID),            MP_ROM_PTR(&xblepy_uuid_type) },
    { MP_ROM_QSTR(MP_QSTR_Constants),       MP_ROM_PTR(&xblepy_constants_type) },
    { MP_ROM_QSTR(MP_QSTR_DefaultDelegate), MP_ROM_PTR(&xblepy_delegate_type) },
#if MICROPY_PY_XBLEPY_DESCRIPTOR
    { MP_ROM_QSTR(MP_QSTR_Descriptor),      MP_ROM_PTR(&xblepy_descriptor_type) },
#endif
    { MP_ROM_QSTR(MP_QSTR_Characteristic),  MP_ROM_PTR(&xblepy_characteristic_type) },
    { MP_ROM_QSTR(MP_QSTR_Service),         MP_ROM_PTR(&xblepy_service_type) },
    { MP_ROM_QSTR(MP_QSTR_Device),         MP_ROM_PTR(&xblepy_device_type) },
    { MP_ROM_QSTR(MP_QSTR_Peripheral),      MP_ROM_PTR(&xblepy_peripheral_type) },
    
/***    
    
#if MICROPY_PY_XBLEPY_PERIPHERAL
    
#endif
#if 0 // MICROPY_PY_XBLEPY_CENTRAL
    { MP_ROM_QSTR(MP_QSTR_Central),         MP_ROM_PTR(&xblepy_central_type) },
#endif
#if MICROPY_PY_XBLEPY_CENTRAL
    { MP_ROM_QSTR(MP_QSTR_Scanner),         MP_ROM_PTR(&xblepy_scanner_type) },
    { MP_ROM_QSTR(MP_QSTR_ScanEntry),       MP_ROM_PTR(&xblepy_scan_entry_type) },
#endif
    
    
    
    
    

***/
};


STATIC MP_DEFINE_CONST_DICT(mp_module_xblepy_globals, mp_module_xblepy_globals_table);

const mp_obj_module_t mp_module_xblepy = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_xblepy_globals,
};

#endif // MICROPY_PY_XBLEPY
