/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef UBLUEPY_H__
#define UBLUEPY_H__

/* Examples:

Advertisment:

from ubluepy import Peripheral
p = Peripheral()
p.advertise(device_name="MicroPython")

DB setup:

from ubluepy import Service, Characteristic, UUID, Peripheral, constants
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
c0 = Characteristic(u1, props = Characteristic.PROP_WRITE | Characteristic.PROP_WRITE_WO_RESP)
c1 = Characteristic(u2, props = Characteristic.PROP_NOTIFY, attrs = Characteristic.ATTR_CCCD)
s.addCharacteristic(c0)
s.addCharacteristic(c1)
p = Peripheral()
p.addService(s)
p.setConnectionHandler(event_handler)
p.advertise(device_name="micr", services=[s])

*/

#include "py/obj.h"

extern const mp_obj_type_t ubluepy_uuid_type;
extern const mp_obj_type_t ubluepy_service_type;
extern const mp_obj_type_t ubluepy_characteristic_type;
extern const mp_obj_type_t ubluepy_peripheral_type;
extern const mp_obj_type_t ubluepy_scanner_type;
extern const mp_obj_type_t ubluepy_scan_entry_type;
extern const mp_obj_type_t ubluepy_constants_type;
extern const mp_obj_type_t ubluepy_constants_ad_types_type;

#define UBLUEPY_UNASSIGNED_HANDLE       (0)
#define UBLUEPY_INVALID_HANDLE          (0xffff)

typedef enum {
    UBLUEPY_UUID_16_BIT = 1,
    UBLUEPY_UUID_128_BIT
} ubluepy_uuid_type_t;

typedef enum {
    UBLUEPY_SERVICE_PRIMARY = 1,
    UBLUEPY_SERVICE_SECONDARY = 2
} ubluepy_service_type_t;

typedef enum {
    UBLUEPY_ADDR_TYPE_PUBLIC = 0,
    UBLUEPY_ADDR_TYPE_RANDOM_STATIC = 1,
#if 0
    UBLUEPY_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE = 2,
    UBLUEPY_ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE = 3,
#endif
} ubluepy_addr_type_t;

typedef enum {
    UBLUEPY_ROLE_PERIPHERAL,
    UBLUEPY_ROLE_CENTRAL
} ubluepy_role_type_t;


typedef enum _ubluepy_prop_t {
    UBLUEPY_PROP_NONE           = 0x00,
    UBLUEPY_PROP_BROADCAST      = 0x01,
    UBLUEPY_PROP_READ           = 0x02,
    UBLUEPY_PROP_WRITE_WO_RESP  = 0x04,
    UBLUEPY_PROP_WRITE          = 0x08,
    UBLUEPY_PROP_NOTIFY         = 0x10,
    UBLUEPY_PROP_INDICATE       = 0x20,
    UBLUEPY_PROP_AUTH_SIGNED_WR = 0x40,
    UBLUEPY_PROP_EXTENDED_PROP  = 0x80,
} ubluepy_prop_t;

typedef enum _ubluepy_permission_t
{
    UBLUEPY_PERM_NONE                  = 0x0000,
    UBLUEPY_PERM_READ                  = 0x0001,       /**< Readable, Encryption not required, No Authentication Required. */
    UBLUEPY_PERM_READ_ENCRYPTED        = 0x0002,       /**< Readable, Encryption required, No Authentication Required. */
    UBLUEPY_PERM_READ_ENCRYPTED_MITM   = 0x0004,       /**< Readable, Encryption required, Authentication Required. */
    UBLUEPY_PERM_WRITE                 = 0x0010,       /**< Writable, Encryption not required, No Authentication Required. */
    UBLUEPY_PERM_WRITE_ENCRYPTED       = 0x0020,       /**< Writable, Encryption required, No Authentication Required. */
    UBLUEPY_PERM_WRITE_ENCRYPTED_MITM  = 0x0040,       /**< Writable, Encryption required, Authentication Required. */
    UBLUEPY_PERM_WRITE_SIGNED          = 0x0080,       /**< Writable, Signed required, No Authentication Required. */
    UBLUEPY_PERM_WRITE_SIGNED_MITM     = 0x0100,       /**< Writable, Signed required, Authentication Required. */    
}ubluepy_permission_t;

typedef enum _ubluepy_attr_t {
    UBLUEPY_ATTR_CCCD           = 0x01,     //Client CCD
    UBLUEPY_ATTR_SCCD           = 0x02,     //Server CCD
} ubluepy_attr_t;


/**
 * @brief Attribute types.
 */
typedef enum
{
    UBLUEPY_ATTR_TYPE_PRIMARY_SERVICE = 0,      /**< Primary service. */
    UBLUEPY_ATTR_TYPE_SECONDARY_SERVICE,        /**< Secondary service. */
    UBLUEPY_ATTR_TYPE_INCLUDED_SERVICE,         /**< Included service. */
    UBLUEPY_ATTR_TYPE_CHARACTERISTIC_DECL,      /**< Characteristic declaration. */
    UBLUEPY_ATTR_TYPE_CHARACTERISTIC_VAL,       /**< Characteristic. */
    UBLUEPY_ATTR_TYPE_DESCRIPTOR,               /**< Characteristic descriptor. */    
} ubluepy_attr_type_t;

typedef struct _ubluepy_uuid_obj_t {
    mp_obj_base_t       base;
    ubluepy_uuid_type_t type;
    uint8_t             value[2];
    uint8_t             uuid_vs_idx;
    uint8_t             value_128b[16];
} ubluepy_uuid_obj_t;

typedef struct _ubluepy_peripheral_obj_t {
    mp_obj_base_t       base;
    ubluepy_role_type_t role;
    volatile uint16_t   conn_handle;
    mp_obj_t            delegate;
    mp_obj_t            notif_handler;
    mp_obj_t            conn_handler;
    mp_obj_t            service_list;
} ubluepy_peripheral_obj_t;

typedef struct _ubluepy_service_obj_t {
    mp_obj_base_t              base;
    uint16_t                   handle;              //myself handle
    ubluepy_service_type_t     type;
    ubluepy_uuid_obj_t       * p_uuid;
    ubluepy_peripheral_obj_t * p_periph;
    mp_obj_t                   char_list;           //save characteristics belongs to this service
    uint16_t                   start_handle;
    uint16_t                   end_handle;
} ubluepy_service_obj_t;

typedef struct _ubluepy_characteristic_obj_t {
    mp_obj_base_t           base;
    uint16_t                handle;                 //myself handle
    ubluepy_uuid_obj_t    * p_uuid;
    uint16_t                service_handle;
    uint16_t                user_desc_handle;
    uint16_t                cccd_handle;
    uint16_t                sccd_handle;
    mp_obj_t                desc_list;          //save descriptors belongs to this charac
    ubluepy_attr_t          attrs;
    ubluepy_prop_t          props;
    ubluepy_permission_t    perms;
    ubluepy_service_obj_t * p_service;          //belong to which service
    mp_obj_t                value_data;
} ubluepy_characteristic_obj_t;

typedef struct _ubluepy_descriptor_obj_t {
    mp_obj_base_t           base;
    uint16_t                handle;                 //myself handle
    ubluepy_uuid_obj_t    * p_uuid;    
    ubluepy_permission_t    perms;
    uint16_t                service_handle;         //belongs to which service
    uint16_t                characteristic_handle;  //belongs to which characteristic handle
    ubluepy_characteristic_obj_t * p_charac;        //belongs to which characteristic ptr
} ubluepy_descriptor_obj_t;

typedef struct _ubluepy_delegate_obj_t {
    mp_obj_base_t        base;
} ubluepy_delegate_obj_t;

typedef struct _ubluepy_advertise_data_t {
    uint8_t *  p_device_name;
    uint8_t    device_name_len;
    mp_obj_t * p_services;
    uint8_t    num_of_services;
    uint8_t *  p_data;
    uint8_t    data_len;
    bool       connectable;
} ubluepy_advertise_data_t;

typedef struct _ubluepy_scanner_obj_t {
    mp_obj_base_t base;
    mp_obj_t      adv_reports;
} ubluepy_scanner_obj_t;

typedef struct _ubluepy_scan_entry_obj_t {
    mp_obj_base_t base;
    mp_obj_t      addr;
    uint8_t       addr_type;
    bool          connectable;
    int8_t        rssi;
    mp_obj_t      data;
} ubluepy_scan_entry_obj_t;




#endif // UBLUEPY_H__
