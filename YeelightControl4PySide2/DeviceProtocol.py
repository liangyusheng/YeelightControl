from Modules import *


class DeviceProtocol(QObject):
    def __init__(self, parent:QObject):
        QObject.__init__(self)
        self.setParent(parent)
        
    def getDatagramByteArray(self)->QByteArray:
        datagram = QByteArray(b"M-SEARCH * HTTP/1.1\r\n\
        HOST: 239.255.255.250:1982\r\n\
        MAN: \"ssdp:discover\"\r\n\
        ST: wifi_bulb")
        return datagram;
    
    def datagramByteArraySize(self)->int:
        datagram = QByteArray(b"M-SEARCH * HTTP/1.1\r\n\
        HOST: 239.255.255.250:1982\r\n\
        MAN: \"ssdp:discover\"\r\n\
        ST: wifi_bulb")
        return datagram.size();
    
    def getCastAddress(self)->QHostAddress:
        return QHostAddress("239.255.255.250")
    
    def getUdpPort(self)->int:
        return 1982
    
    def getCloseDeviceCmd(self, deviceId:bytes)->bytes:
        cmd = bytes("{\"id\":", encoding='utf8')
        cmd += deviceId
        cmd += bytes(b",\"method\":\"toggle\",\"params\":[]}\r\n")
    
        return cmd
    
    def getChangeBrightnessCmd(self, deviceId:bytearray, level:int)->bytes:
        cmd = str()
        cmd += "{\"id\":"
        cmd += deviceId.decode() # to string
        cmd += ",\"method\":\"set_bright\",\"params\":["
        cmd += str(level)
        cmd += ", \"smooth\", 500]}\r\n"
        
        return bytes(cmd, encoding='utf8')
    
    def getDeviceAddress(self, receivedData:QByteArray)->QHostAddress:
        datagramString = str(receivedData.data())
        infoList = datagramString.split(":")
        ipTemp = infoList[5]
        ip = str()
        for i in ipTemp:
            if (i != '/'):
                ip += i

        return QHostAddress(ip)
    
    def getDevicePort(self, receivedData:QByteArray)->int:
        datagramString = str(receivedData)
        infoList = datagramString.split(":")
    
        portTemp = infoList[6]
        port = str()
    
        for i in portTemp:
            if (i == '\\' or i =='\n'):
                break
            port += i
    
        return int(port)
    
    def getDeviceId(self, receivedData:QByteArray)->bytes:
        deviceInfoList = receivedData.split(':')
        deviceIdTmp = bytes(deviceInfoList[8])
    
        deviceId = bytearray()
        for i in deviceIdTmp:
            if (chr(i) == '\r' or chr(i) == '\n'):
                break
            deviceId.append(i)
        return bytes(deviceId)