/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 nix.long
 *
 */

#include "py/obj.h"
#include "py/runtime.h"
#include "mp_defs.h"

#if MICROPY_PY_XBLEPY

#include "modxblepy.h"

/****************************************
 * Constants of ADV Types
 ****************************************/
STATIC const mp_rom_map_elem_t xblepy_constants_ad_types_locals_dict_table[] = {
    // GAP AD Types
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_FLAGS),                              MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE),  MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE),        MP_ROM_INT(0x03) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE),  MP_ROM_INT(0x04) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE),        MP_ROM_INT(0x05) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE), MP_ROM_INT(0x06) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE),       MP_ROM_INT(0x07) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SHORT_LOCAL_NAME),                   MP_ROM_INT(0x08) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_COMPLETE_LOCAL_NAME),                MP_ROM_INT(0x09) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_TX_POWER_LEVEL),                     MP_ROM_INT(0x0A) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_CLASS_OF_DEVICE),                    MP_ROM_INT(0x0D) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_HASH_C),              MP_ROM_INT(0x0E) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R),        MP_ROM_INT(0x0F) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SECURITY_MANAGER_TK_VALUE),          MP_ROM_INT(0x10) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SECURITY_MANAGER_OOB_FLAGS),         MP_ROM_INT(0x11) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE),    MP_ROM_INT(0x12) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SOLICITED_SERVICE_UUIDS_16BIT),      MP_ROM_INT(0x14) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SOLICITED_SERVICE_UUIDS_128BIT),     MP_ROM_INT(0x15) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SERVICE_DATA),                       MP_ROM_INT(0x16) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_PUBLIC_TARGET_ADDRESS),              MP_ROM_INT(0x17) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_RANDOM_TARGET_ADDRESS),              MP_ROM_INT(0x18) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_APPEARANCE),                         MP_ROM_INT(0x19) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_ADVERTISING_INTERVAL),               MP_ROM_INT(0x1A) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS),        MP_ROM_INT(0x1B) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_LE_ROLE),                            MP_ROM_INT(0x1C) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_HASH_C256),           MP_ROM_INT(0x1D) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R256),     MP_ROM_INT(0x1E) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SERVICE_DATA_32BIT_UUID),            MP_ROM_INT(0x20) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SERVICE_DATA_128BIT_UUID),           MP_ROM_INT(0x21) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_URI),                                MP_ROM_INT(0x24) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_3D_INFORMATION_DATA),                MP_ROM_INT(0x3D) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_MANUFACTURER_SPECIFIC_DATA),         MP_ROM_INT(0xFF) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_constants_ad_types_locals_dict, xblepy_constants_ad_types_locals_dict_table);

const mp_obj_type_t xblepy_constants_ad_types_type = {
    { &mp_type_type },
    .name = MP_QSTR_AdTypes,
    .locals_dict = (mp_obj_dict_t*)&xblepy_constants_ad_types_locals_dict
};

/***************************************************************
 * Constants of Props used by Characteristic & Descriptor
 *
 * @brief GATT Characteristic property.
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G, 3.3.1.1 Characteristic Properties
 ***************************************************************/
STATIC const mp_rom_map_elem_t xblepy_constants_charac_prop_locals_dict_table[] = {
    //map to enum _xblepy_prop_t
    { MP_ROM_QSTR(MP_QSTR_PROP_NONE),           MP_ROM_INT(XBLEPY_PROP_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PROP_BROADCAST),      MP_ROM_INT(XBLEPY_PROP_BROADCAST) },
    { MP_ROM_QSTR(MP_QSTR_PROP_READ),           MP_ROM_INT(XBLEPY_PROP_READ) },
    { MP_ROM_QSTR(MP_QSTR_PROP_WRITE_WO_RESP),  MP_ROM_INT(XBLEPY_PROP_WRITE_WO_RESP) },
    { MP_ROM_QSTR(MP_QSTR_PROP_WRITE),          MP_ROM_INT(XBLEPY_PROP_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_PROP_NOTIFY),         MP_ROM_INT(XBLEPY_PROP_NOTIFY) },
    { MP_ROM_QSTR(MP_QSTR_PROP_INDICATE),       MP_ROM_INT(XBLEPY_PROP_INDICATE) },
    { MP_ROM_QSTR(MP_QSTR_PROP_AUTH_SIGNED_WR), MP_ROM_INT(XBLEPY_PROP_AUTH_SIGNED_WR) },
    { MP_ROM_QSTR(MP_QSTR_PROP_EXTENDED_PROP),  MP_ROM_INT(XBLEPY_PROP_EXTENDED_PROP) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_constants_charac_prop_locals_dict, xblepy_constants_charac_prop_locals_dict_table);

const mp_obj_type_t xblepy_constants_charac_prop_type = {
    { &mp_type_type },
    .name = MP_QSTR_CharacProp,
    .locals_dict = (mp_obj_dict_t*)&xblepy_constants_charac_prop_locals_dict
};

/***************************************************************
 * Constants of Perms used by Characteristic & Descriptor
 *
 * @brief GATT permissions.
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F, 3.2.5 Attribute Permissions
 ***************************************************************/
STATIC const mp_rom_map_elem_t xblepy_constants_attr_perm_locals_dict_table[] = {
    //map to enum _xblepy_permission_t
    { MP_ROM_QSTR(MP_QSTR_PERM_NONE),                   MP_ROM_INT(XBLEPY_PERM_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PERM_READ),                   MP_ROM_INT(XBLEPY_PERM_READ) },
    { MP_ROM_QSTR(MP_QSTR_PERM_READ_ENCRYPTED),         MP_ROM_INT(XBLEPY_PERM_READ_ENCRYPTED) },
    { MP_ROM_QSTR(MP_QSTR_PERM_READ_ENCRYPTED_MITM),    MP_ROM_INT(XBLEPY_PERM_READ_ENCRYPTED_MITM) },
    { MP_ROM_QSTR(MP_QSTR_PERM_WRITE),                  MP_ROM_INT(XBLEPY_PERM_WRITE) },
    { MP_ROM_QSTR(MP_QSTR_PERM_WRITE_ENCRYPTED),        MP_ROM_INT(XBLEPY_PERM_WRITE_ENCRYPTED) },
    { MP_ROM_QSTR(MP_QSTR_PERM_WRITE_ENCRYPTED_MITM),   MP_ROM_INT(XBLEPY_PERM_WRITE_ENCRYPTED_MITM) },
    { MP_ROM_QSTR(MP_QSTR_PERM_WRITE_SIGNED),           MP_ROM_INT(XBLEPY_PERM_WRITE_SIGNED) },
    { MP_ROM_QSTR(MP_QSTR_PERM_WRITE_SIGNED_MITM),      MP_ROM_INT(XBLEPY_PERM_WRITE_SIGNED_MITM) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_constants_attr_perm_locals_dict, xblepy_constants_attr_perm_locals_dict_table);

const mp_obj_type_t xblepy_constants_attr_perm_type = {
    { &mp_type_type },
    .name = MP_QSTR_AttrPerm,
    .locals_dict = (mp_obj_dict_t*)&xblepy_constants_attr_perm_locals_dict
};

/***************************************************************
 * Constants of CC Descriptor
 ***************************************************************/
STATIC const mp_rom_map_elem_t xblepy_constants_ccd_locals_dict_table[] = {
    //map to enum _xblepy_attr_t
#if MICROPY_PY_XBLEPY_PERIPHERAL
    { MP_ROM_QSTR(MP_QSTR_ATTR_CCCD),           MP_ROM_INT(XBLEPY_ATTR_CCCD) },
#endif
#if MICROPY_PY_XBLEPY_CENTRAL
    { MP_ROM_QSTR(MP_QSTR_ATTR_SCCD),           MP_ROM_INT(XBLEPY_ATTR_SCCD) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(xblepy_constants_ccd_locals_dict, xblepy_constants_ccd_locals_dict_table);

const mp_obj_type_t xblepy_constants_ccd_type = {
    { &mp_type_type },
    .name = MP_QSTR_Ccd,
    .locals_dict = (mp_obj_dict_t*)&xblepy_constants_ccd_locals_dict
};

/****************************************
 * Constants of All
 ****************************************/
STATIC const mp_rom_map_elem_t xblepy_constants_locals_dict_table[] = {
    // GAP events
    { MP_ROM_QSTR(MP_QSTR_EVT_GAP_CONNECTED),       MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_EVT_GAP_DISCONNECTED),    MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_EVT_GATTS_WRITE),         MP_ROM_INT(80) },
    { MP_ROM_QSTR(MP_QSTR_UUID_CCCD),               MP_ROM_INT(0x2902) },
    { MP_ROM_QSTR(MP_QSTR_UNASSIGNED_HANDLE),       MP_ROM_INT(XBLEPY_UNASSIGNED_HANDLE) },
    { MP_ROM_QSTR(MP_QSTR_INVALID_HANDLE),          MP_ROM_INT(XBLEPY_INVALID_HANDLE) },

    { MP_ROM_QSTR(MP_QSTR_ADDR_TYPE_PUBLIC),        MP_ROM_INT(XBLEPY_ADDR_TYPE_PUBLIC) },
    { MP_ROM_QSTR(MP_QSTR_ADDR_TYPE_RANDOM_STATIC), MP_ROM_INT(XBLEPY_ADDR_TYPE_RANDOM_STATIC) },

    { MP_ROM_QSTR(MP_QSTR_AdTypes),                 MP_ROM_PTR(&xblepy_constants_ad_types_type) },
    { MP_ROM_QSTR(MP_QSTR_CharacProp),              MP_ROM_PTR(&xblepy_constants_charac_prop_type) },
    { MP_ROM_QSTR(MP_QSTR_AttrPerm),                MP_ROM_PTR(&xblepy_constants_attr_perm_type) },
    { MP_ROM_QSTR(MP_QSTR_Ccd),                     MP_ROM_PTR(&xblepy_constants_ccd_type) },
};

STATIC MP_DEFINE_CONST_DICT(xblepy_constants_locals_dict, xblepy_constants_locals_dict_table);

const mp_obj_type_t xblepy_constants_type = {
    { &mp_type_type },
    .name = MP_QSTR_Constants,
    .locals_dict = (mp_obj_dict_t*)&xblepy_constants_locals_dict
};

#endif // MICROPY_PY_XBLEPY

