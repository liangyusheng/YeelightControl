from Modules import *


class NetworkScan(QObject):
    def __init__(self):
        QObject.__init__(self)
        
    def start(self):
        localIpAddress = QNetworkInterface.allAddresses()
        ips = list()
        ips.clear()
        for ip in localIpAddress:
            if ip.protocol() is QAbstractSocket.IPv4Protocol:
                ips.append(ip.toString())
        return ips
    
    def refresh(self):
        return self.start()