##########################################################################
##
##                    set User's event Delegate / Handler
##
##
###########################################################################

import xblepy,ble
class MyGapHandler(xblepy.DefaultGAPDelegate):
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

ble.enable()
periph = xblepy.Peripheral()
handler = MyGapHandler()
periph.setGapDelegate(handler)
periph.startAdvertise(device_name="mpy_test")
