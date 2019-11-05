##########################################################################
##
##                           Ble Heart Rate Service Demo
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

import ble, xblepy, utime
from machine import Timer

class HRS_SENSOR_LOC():
    OTHER = 0
    CHEST = 1
    WRIST = 2
    FINGER = 3
    HAND  = 4
    EARLOBE = 5
    FOOT = 6

class ATTR_IDX():
    # HRS
    HRS_SRV = 1  # Service: HeartRate Service
    HRS_CHR_HRM = 2  # Characteristic: HeartRate Measurement
    HRS_CHR_BSL = 3  # Characteristic: Body Sensor Location
    HRS_CHR_HRCP = 4  # Characteristic: HeartRate Control Point
    HRS_CCCD_HRM = 5  # Descriptor: CCCD for HeartRate Measurement
    # DIS
    DIS_SRV = 6  # Service: Device Information Service
    DIS_CHR_SYS_ID = 7  # Characteristic: System ID
    DIS_CHR_MODEL_NB = 8  # Characteristic: Model Number String
    DIS_CHR_SERIAL = 9  # Characteristic: Serial Number
    DIS_CHR_FW_VER = 10  # Characteristic: Fireware Version
    DIS_CHR_HW_VER = 11  # Characteristic: Hardware Version
    DIS_CHR_SW_VER = 12  # Characteristic: Software Version
    DIS_CHR_MANU_NAME = 13  # Characteristic: Manufacture Name
    DIS_CHR_IEEE_CERTIF = 14  # Characteristic: IEEE Certification Data Listc
    DIS_CHR_PNP_ID = 15  # Characteristic: PnP ID

def simulateHR():
    # simulate heart rate, scope [60, 100]
    max = 300
    min = 180
    random = utime.ticks_ms() % (max - min)
    hr_val = (min + random)/3    
    encode = bytearray([])
    encode.append(0x06)         # first byte: flag
    encode.append(int(hr_val))  # second byte: heart rate value
    return encode.decode('utf-8')

class MyGattsHandler(xblepy.DefaultGattsDelegate):
    def handleReadEvent(self, idx):
        res = 'unknown'
        print('+++ MyGattsHandler:read idx %d' % idx)
        if idx == ATTR_IDX.HRS_CHR_HRM or idx == ATTR_IDX.HRS_CCCD_HRM :
            res = simulateHR()
        elif idx == ATTR_IDX.HRS_CHR_BSL :
            # simulate sensor location
            # int(HRS_SENSOR_LOC.FOOT - HRS_SENSOR_LOC.OTHER + 1)
            loc = utime.ticks_ms() % 7
            print('loc: %d' % loc)
            res = str(loc)
        elif idx == ATTR_IDX.DIS_CHR_SYS_ID :
            res = str(b'\x12\x34\x56\x78\x90\x64\x5d\xd7', 'utf-8')
        elif idx == ATTR_IDX.DIS_CHR_MODEL_NB :
            res = 'hr-sensor-01'
        elif idx == ATTR_IDX.DIS_CHR_SERIAL :
            res = '0001'
        elif idx == ATTR_IDX.DIS_CHR_FW_VER :
            res = '1.0.1'
        elif idx == ATTR_IDX.DIS_CHR_HW_VER :
            res = '1.1.0'
        elif idx == ATTR_IDX.DIS_CHR_SW_VER :
            res = '2.3.1'
        elif idx == ATTR_IDX.DIS_CHR_MANU_NAME :
            res = 'goodix'
        elif idx == ATTR_IDX.DIS_CHR_IEEE_CERTIF :
            res = str(b'\xfe\x00', 'utf-8')
            res = res + 'experimental'
        elif idx == ATTR_IDX.DIS_CHR_PNP_ID :
            res = str(b'\x01\xF7\x04\x34\x12\x10\x01', 'utf-8')
        else :
            res = 'unknown'
        MyGattsHandler.responseRead(idx, res)
    def handleWriteEvent(self, idx, offset, data):
        print('+++ MyGattsHandler:write idx %d ,offset %d' % (idx, offset))
        if idx == ATTR_IDX.HRS_CHR_HRCP :
            print("+++ reset energy extend")
        elif idx == ATTR_IDX.HRS_CCCD_HRM :
            val = int.from_bytes(data, 'little')
            print(val)
            if val == 0 :
                periodicNotifyTimer.stop()
                print("+++ Disable HRM Notify ")
            else :
                periodicNotifyTimer.start()
                print("+++ Enable HRM Notify ")
        else :
            pass
        MyGattsHandler.responseWrite(idx, True)

def periodicNotifyHeartRate(timer_id):
    res = simulateHR()
    print('+++ periodicNotifyHeartRate: %d ' % (timer_id))
    MyGattsHandler.sendNotification(ATTR_IDX.HRS_CHR_HRM, res)

if __name__ == '__main__':
    global periodicNotifyTimer
    global gattsHandler
    periodicNotifyTimer = Timer(1, period=2000, mode=Timer.PERIODIC, callback=periodicNotifyHeartRate)
    gattsHandler = MyGattsHandler()
    ble.enable()    
    # HRS
    hrs = xblepy.Service(1, xblepy.UUID(0x2800))
    hrs_c1 = xblepy.Characteristic(2, xblepy.UUID(0x2A37), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_NOTIFY)
    hrs_c2 = xblepy.Characteristic(3, xblepy.UUID(0x2A38), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    hrs_c3 = xblepy.Characteristic(4, xblepy.UUID(0x2A39), perms=xblepy.Constants.AttrPerm.PERM_WRITE_FREE, props=xblepy.Constants.CharacProp.PROP_WRITE)
    hrs_d1 = xblepy.Descriptor(5, xblepy.UUID(0x2902), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE | xblepy.Constants.AttrPerm.PERM_WRITE_FREE)
    hrs_c1.addDescriptor(hrs_d1)
    hrs.addCharacteristic(hrs_c1)
    hrs.addCharacteristic(hrs_c2)
    hrs.addCharacteristic(hrs_c3)
    # DIS
    dis = xblepy.Service(6, xblepy.UUID(0x2800))
    dis_c1 = xblepy.Characteristic(7, xblepy.UUID(0x2A23), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c2 = xblepy.Characteristic(8, xblepy.UUID(0x2A24), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c3 = xblepy.Characteristic(9, xblepy.UUID(0x2A25), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c4 = xblepy.Characteristic(10, xblepy.UUID(0x2A26), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c5 = xblepy.Characteristic(11, xblepy.UUID(0x2A27), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c6 = xblepy.Characteristic(12, xblepy.UUID(0x2A28), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c7 = xblepy.Characteristic(13, xblepy.UUID(0x2A29), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c8 = xblepy.Characteristic(14, xblepy.UUID(0x2A2A), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis_c9 = xblepy.Characteristic(15, xblepy.UUID(0x2A50), perms=xblepy.Constants.AttrPerm.PERM_READ_FREE, props=xblepy.Constants.CharacProp.PROP_READ)
    dis.addCharacteristic(dis_c1)
    dis.addCharacteristic(dis_c2)
    dis.addCharacteristic(dis_c3)
    dis.addCharacteristic(dis_c4)
    dis.addCharacteristic(dis_c5)
    dis.addCharacteristic(dis_c6)
    dis.addCharacteristic(dis_c7)
    dis.addCharacteristic(dis_c8)
    dis.addCharacteristic(dis_c9)
    # Peripheral
    periph = xblepy.Peripheral()    
    periph.setGattsDelegate(gattsHandler)
    periph.addService(hrs)
    periph.addService(dis)
    periph.startServices()
    periph.startAdvertise(device_name="mpy_test")
