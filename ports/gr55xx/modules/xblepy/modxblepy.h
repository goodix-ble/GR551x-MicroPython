/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 nix.long
 *
 */

#ifndef XBLEPY_H__
#define XBLEPY_H__

/* Examples:

Advertisment:

from xblepy import Peripheral
p = Peripheral()
p.advertise(device_name="MicroPython")

DB setup:

from xblepy import Service, Characteristic, UUID, Peripheral, constants
from board import LED

def event_handler(id, handle, data):
    print("BLE event:", id, "handle:", handle)
    print(data)

    if id == constants.EVT_GAP_CONNECTED:
        # connected
        LED(2).on()
    elif id == constants.EVT_GAP_DISCONNECTED:
        # disconnect
        LED(2).off()
    elif id == 80:
        print("id 80, data:", data)

# u0 = UUID("0x180D") # HRM service
# u1 = UUID("0x2A37") # HRM measurement

u0 = UUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e")
u1 = UUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e")
u2 = UUID("6e400003-b5a3-f393-e0a9-e50e24dcca9e")
s = Service(u0)
c0 = Characteristic(u1, props = Characteristic.PROP_WRITE | Characteristic.PROP_WRITE_NO_RESP)
c1 = Characteristic(u2, props = Characteristic.PROP_NOTIFY, attrs = Characteristic.ATTR_CCCD)
s.addCharacteristic(c0)
s.addCharacteristic(c1)
p = Peripheral()
p.addService(s)
p.setConnectionHandler(event_handler)
p.advertise(device_name="micr", services=[s])

*/

#include "py/obj.h"

extern const mp_obj_type_t xblepy_device_type;
extern const mp_obj_type_t xblepy_uuid_type;
extern const mp_obj_type_t xblepy_service_type;
extern const mp_obj_type_t xblepy_characteristic_type;
extern const mp_obj_type_t xblepy_peripheral_type;
extern const mp_obj_type_t xblepy_scanner_type;
extern const mp_obj_type_t xblepy_scan_entry_type;
extern const mp_obj_type_t xblepy_constants_type;
extern const mp_obj_type_t xblepy_constants_ad_types_type;

extern const mp_obj_type_t xblepy_default_gap_delegate_type;
extern const mp_obj_type_t xblepy_default_gatts_delegate_type;

#define XBLEPY_UNASSIGNED_HANDLE       (0)
#define XBLEPY_INVALID_HANDLE          (0xffff)
#define XBLEPY_BD_ADDR_LEN             (6)
#define XBLEPY_BD_ADDR_DEFAULT         {0x11,0x22,0x33,0xdd,0xee,0xff}

typedef enum {
    XBLEPY_UUID_16_BIT = 1,
    XBLEPY_UUID_128_BIT
} xblepy_uuid_type_t;

typedef enum {
    XBLEPY_SERVICE_PRIMARY = 1,
    XBLEPY_SERVICE_SECONDARY = 2
} xblepy_service_type_t;

typedef enum {
    XBLEPY_ADDR_TYPE_PUBLIC = 0,
    XBLEPY_ADDR_TYPE_RANDOM_STATIC = 1,
#if 0
    XBLEPY_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE = 2,
    XBLEPY_ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE = 3,
#endif
} xblepy_addr_type_t;


typedef enum {
    XBLEPY_ROLE_UNKNOWN = 0,
    XBLEPY_ROLE_PERIPHERAL,
    XBLEPY_ROLE_CENTRAL
} xblepy_role_type_t;


typedef enum _xblepy_prop_t {
    XBLEPY_PROP_NONE                    = 0x00,
    XBLEPY_PROP_BROADCAST               = 0x01,
    XBLEPY_PROP_READ                    = 0x02,
    XBLEPY_PROP_WRITE_NO_RESP           = 0x04,
    XBLEPY_PROP_WRITE                   = 0x08,
    XBLEPY_PROP_NOTIFY                  = 0x10,
    XBLEPY_PROP_INDICATE                = 0x20,
    XBLEPY_PROP_AUTH_SIGNED_WR          = 0x40,
    XBLEPY_PROP_EXTENDED_PROP           = 0x80,
} xblepy_prop_t;

typedef enum _xblepy_permission_t
{
    XBLEPY_PERM_NONE                    = 0x0000,       /**< no permission to read ro write */
    XBLEPY_PERM_READ_FREE               = 0x0001,       /**< Readable, read freely, both Encryption and Authentication are not Required. */
    XBLEPY_PERM_READ_ENCRYPTED          = 0x0002,       /**< Readable, Encryption required, No Authentication Required. */
    XBLEPY_PERM_READ_ENCRYPTED_MITM     = 0x0004,       /**< Readable, Encryption required, Authentication Required. */
    XBLEPY_PERM_WRITE_FREE              = 0x0010,       /**< Writable, write freely, both Encryption and Authentication are not Required. */
    XBLEPY_PERM_WRITE_ENCRYPTED         = 0x0020,       /**< Writable, Encryption required, No Authentication Required. */
    XBLEPY_PERM_WRITE_ENCRYPTED_MITM    = 0x0040,       /**< Writable, Encryption required, Authentication Required. */
    XBLEPY_PERM_WRITE_SIGNED            = 0x0080,       /**< Writable, Signed required, No Authentication Required. */
    XBLEPY_PERM_WRITE_SIGNED_MITM       = 0x0100,       /**< Writable, Signed required, Authentication Required. */    
}xblepy_permission_t;

typedef enum _xblepy_attr_t {
    XBLEPY_ATTR_CCCD                    = 0x01,     //Client CCD
    XBLEPY_ATTR_SCCD                    = 0x02,     //Server CCD
} xblepy_attr_t;


/**
 * @brief Attribute types.
 */
typedef enum
{
    XBLEPY_ATTR_TYPE_PRIMARY_SERVICE = 0,      /**< Primary service. */
    XBLEPY_ATTR_TYPE_SECONDARY_SERVICE,        /**< Secondary service. */
    XBLEPY_ATTR_TYPE_INCLUDED_SERVICE,         /**< Included service. */
    XBLEPY_ATTR_TYPE_CHARACTERISTIC_DECL,      /**< Characteristic declaration. */
    XBLEPY_ATTR_TYPE_CHARACTERISTIC_VAL,       /**< Characteristic. */
    XBLEPY_ATTR_TYPE_DESCRIPTOR,               /**< Characteristic descriptor. */    
} xblepy_attr_type_t;

/********************************************************************************************
 *   Basic Element Object struct for Ble : UUID, Descriptor, Characteristic, Service
 ********************************************************************************************/
typedef struct _xblepy_bd_addr_obj_t {
    uint8_t                         addr[XBLEPY_BD_ADDR_LEN];
} xblepy_bd_addr_t;

typedef struct _xblepy_event_handler_t {
    uint8_t                         e_type;
    mp_obj_t                        e_handler;
} xblepy_event_handler_t;

typedef struct _xblepy_gap_delegate_obj_t {
    mp_obj_base_t                   base;
} xblepy_delegate_obj_t, 
  xblepy_gap_delegate_obj_t,
  xblepy_gatts_delegate_obj_t;

/*
 * Device is super class of Peripheral and Central
 */
typedef struct _xblepy_device_obj_t {
    mp_obj_base_t                   base;
    xblepy_role_type_t              role;
    xblepy_addr_type_t              addr_type;
    xblepy_bd_addr_t                addr;
    volatile uint16_t               conn_id;
    mp_obj_t                        delegate;
    mp_obj_t                        service_list;
    xblepy_event_handler_t *        handler_list;
    
    xblepy_gap_delegate_obj_t *     gap_delegate;
    xblepy_gatts_delegate_obj_t *   gatts_delegate;

    mp_obj_t                        conn_handler;       //delete later, avoid compile error for now
} xblepy_device_obj_t,
  xblepy_peripheral_obj_t,
  xblepy_central_obj_t;

typedef struct _xblepy_uuid_obj_t {
    mp_obj_base_t                   base;
    xblepy_uuid_type_t              type;
    uint8_t                         value[2];
    uint8_t                         uuid_vs_idx;
    uint8_t                         value_128b[16];
} xblepy_uuid_obj_t;

typedef struct _xblepy_service_obj_t {
    mp_obj_base_t                   base;
    uint16_t                        attr_idx;               //attribute index, mpy layer's handle
    uint16_t                        handle;              //myself handle
    xblepy_service_type_t           type;
    xblepy_uuid_obj_t       *       p_uuid;
    xblepy_peripheral_obj_t *       p_periph;
    mp_obj_t                        char_list;           //save characteristics belongs to this service
    uint16_t                        start_handle;
    uint16_t                        end_handle;
} xblepy_service_obj_t;

typedef struct _xblepy_characteristic_obj_t {
    mp_obj_base_t                   base;
    uint16_t                        attr_idx;               //attribute index, mpy layer's handle
    uint16_t                        handle;                 //myself handle
    xblepy_uuid_obj_t *             p_uuid;
    uint16_t                        service_handle;
    mp_obj_t                        desc_list;          //save descriptors belongs to this charac
    xblepy_attr_t                   attrs;
    xblepy_prop_t                   props;
    xblepy_permission_t             perms;
    xblepy_service_obj_t *          p_service;          //belong to which service
    mp_obj_t                        value_data;
} xblepy_characteristic_obj_t;

typedef struct _xblepy_descriptor_obj_t {
    mp_obj_base_t                   base;
    uint16_t                        attr_idx;               //attribute index, mpy layer's handle
    uint16_t                        handle;                 //myself handle
    xblepy_uuid_obj_t    *          p_uuid;    
    xblepy_permission_t             perms;
    uint16_t                        service_handle;         //belongs to which service
    uint16_t                        characteristic_handle;  //belongs to which characteristic handle
    xblepy_characteristic_obj_t *   p_charac;        //belongs to which characteristic ptr
} xblepy_descriptor_obj_t;















typedef struct _xblepy_advertise_data_t {
    uint8_t *                       p_device_name;
    uint8_t                         device_name_len;
    mp_obj_t *                      p_services;
    uint8_t                         num_of_services;
    uint8_t *                       p_data;
    uint8_t                         data_len;
    bool                            connectable;
} xblepy_advertise_data_t;

typedef struct _xblepy_scanner_obj_t {
    mp_obj_base_t                   base;
    mp_obj_t                        adv_reports;
    
} xblepy_scanner_obj_t;

typedef struct _xblepy_scan_entry_obj_t {
    mp_obj_base_t                   base;
    mp_obj_t                        addr;
    uint8_t                         addr_type;
    bool                            connectable;
    int8_t                          rssi;
    mp_obj_t                        data;
} xblepy_scan_entry_obj_t;


#endif // XBLEPY_H__
