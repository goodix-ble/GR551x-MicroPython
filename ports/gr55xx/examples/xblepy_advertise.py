##########################################################################
##
##                           Ble Advertise Demo
##
## start adv with a service, and adv name is mpy_test
##
## adv Service:
##      +++   ATTR  ++++++++++  UUID +++
##      +++ (SERVICE    )  6e400001b5a3f393e0a9e50e24dcca98 
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  6e400002b5a3f393e0a9e50e24dcca98 
##      +++       (DESC     )  6e400003b5a3f393e0a9e50e24dcca98 
##      +++       (DESC     )  0x2902 
##      +++   (CHAR DECL)  0x2803 
##      +++       (CHAR VALU)  6e400004b5a3f393e0a9e50e24dcca98 
##      +++       (DESC     )  0x2902 
##
##      Peripheral contains Services
##      Service contains Characteristics
##      Characteristic contains Descriptors
##
###########################################################################

import ble,xblepy
ble.enable()
s1 = xblepy.Service(1, xblepy.UUID("6e400001-b5a3-f393-e0a9-e50e24dcca98"))
c1 = xblepy.Characteristic(2, xblepy.UUID("6e400002-b5a3-f393-e0a9-e50e24dcca98"))
c2 = xblepy.Characteristic(3, xblepy.UUID("6e400004-b5a3-f393-e0a9-e50e24dcca98"))
d1 = xblepy.Descriptor(4, xblepy.UUID("6e400003-b5a3-f393-e0a9-e50e24dcca98"))
d2 = xblepy.Descriptor(5, xblepy.UUID(0x2902))
d3 = xblepy.Descriptor(6, xblepy.UUID(0x2902))
c1.addDescriptor(d1)
c1.addDescriptor(d2)
c2.addDescriptor(d3)
s1.addCharacteristic(c1)
s1.addCharacteristic(c2)
periph = xblepy.Peripheral()
periph.addService(s1)
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
