##########################################################################
##
##                           Ble Heart Rate Service Demo
##
## 
##
## HRS Service Profile:
##      +++   ATTR  ++++++++++  UUID ++++++++++++++++++++++++++++++
##      +++ (SERVICE    )  0x2800 
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A37       # Heart Rate Measurement
##      +++       (CCCD     )  0x2902 
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A38       # Body Sensor Location
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A39       # Heart Rate Control Point
##
## DIS(Device Information) Service Profile:
##      +++   ATTR  ++++++++++  UUID ++++++++++++++++++++++++++++++
##      +++ (SERVICE    )  0x2800 
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A23       # System ID
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A24       # Module Number String
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A25       # Serial Number String
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A26       # Fireware Revision String
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A27       # Hardware Revision String
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A28       # Software Revision String
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A29       # Manafacturer Name String
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A2A       # IEEE Regulatory Certification Data List
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  0x2A50       # PnP ID
##
###########################################################################

import ble,xblepy
ble.enable()
# HRS
hrs = xblepy.Service(xblepy.UUID(0x2800))
hrs_c1 = xblepy.Characteristic(xblepy.UUID(0x2A37), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_NOTIFY)
hrs_c2 = xblepy.Characteristic(xblepy.UUID(0x2A38), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
hrs_c3 = xblepy.Characteristic(xblepy.UUID(0x2A39), perms=xblepy.Constants.AttrPerm.PERM_WRITE_FREE, props=xblepy.Constants.CharacProp.PROP_WRITE)
hrs_d1 = xblepy.Descriptor(xblepy.UUID(0x2902), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE | xblepy.Constants.AttrPerm.PERM_WRITE_FREE)
hrs_c1.addDescriptor(hrs_d1)
hrs.addCharacteristic(hrs_c1)
hrs.addCharacteristic(hrs_c2)
hrs.addCharacteristic(hrs_c3)
# DIS
dis = xblepy.Service(xblepy.UUID(0x2800))
dis_c1 = xblepy.Characteristic(xblepy.UUID(0x2A23), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c2 = xblepy.Characteristic(xblepy.UUID(0x2A24), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c3 = xblepy.Characteristic(xblepy.UUID(0x2A25), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c4 = xblepy.Characteristic(xblepy.UUID(0x2A26), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c5 = xblepy.Characteristic(xblepy.UUID(0x2A27), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c6 = xblepy.Characteristic(xblepy.UUID(0x2A28), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c7 = xblepy.Characteristic(xblepy.UUID(0x2A29), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c8 = xblepy.Characteristic(xblepy.UUID(0x2A2A), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis_c9 = xblepy.Characteristic(xblepy.UUID(0x2A50), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
dis.addCharacteristic(dis_c1)
dis.addCharacteristic(dis_c2)
dis.addCharacteristic(dis_c3)
dis.addCharacteristic(dis_c4)
dis.addCharacteristic(dis_c5)
dis.addCharacteristic(dis_c6)
dis.addCharacteristic(dis_c7)
dis.addCharacteristic(dis_c8)
dis.addCharacteristic(dis_c9)
periph = xblepy.Peripheral()
periph.addService(hrs)
periph.addService(dis)
periph.startServices()
periph.startAdvertise(device_name="mpy_test")


###########################################################################
##
## if starting adv without any services except the basic service
## (0x1800 & 0x1801), call it like this:
##
##      import ble,xblepy
##      ble.enable()
##      periph = xblepy.Peripheral()
##      periph.startAdvertise(device_name="mpy_test")
##
###########################################################################
