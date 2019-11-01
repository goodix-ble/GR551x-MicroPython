##########################################################################
##
##                    set User's event Delegate / Handler
##
##
###########################################################################

import xblepy,ble
class MyGapHandler(xblepy.DefaultGapDelegate):
    def __init__(self):
        pass
    def transefr_hex(bytes):
        l = [hex(int(i)) for i in bytes]
        return (" ".join(l))
    def handleAdvStartEvent(self, status):
        print('+++ adv start: called from MyGapHandler %d ' % status)
    def handleAdvStopEvent(self, status):
        print('+++ adv stop: called from MyGapHandler %d ' % status)
    def handleConnectEvent(self, status, peer_addr):
        l = [hex(int(i)) for i in peer_addr]
        l.reverse()
        addr = ":".join(l)
        print('+++ gap connect: called from MyGapHandler %d, addr: %s' % (status, addr))
    def handleDisconnectEvent(self, status):
        print('+++ gap disconnect: called from MyGapHandler %d ' % status)

class MyGattsHandler(xblepy.DefaultGattsDelegate):
    def handleReadEvent(self, idx):
        print('+++ MyGattsHandler:read idx %d' % idx)
        MyGattsHandler.responseRead(idx, 'hello')
    def handleWriteEvent(self, idx, offset, data):
        print('+++ MyGattsHandler:write idx %d ,offset %d' % (idx, offset))
        MyGattsHandler.responseWrite(idx, True)

if __name__ == '__main__':
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
    periph.setGapDelegate(MyGapHandler())
    periph.setGattsDelegate(MyGattsHandler())
    periph.addService(s1)
    periph.startServices()
    periph.startAdvertise(device_name="mpy_test")
