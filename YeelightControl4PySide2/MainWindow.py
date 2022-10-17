from Modules import *
from NetworkScan import *
from DeviceProtocol import *


class MainWindow(QMainWindow):
    ____widget = None
    __netBox = None
    __devBox = None
    __brightnessBox = None
    ____vLayout = None


    def __init__(self):
        QMainWindow.__init__(self)
        self.__initUi()
    
    
    def __initUi(self):
        self.__networkScan = NetworkScan()
        __netBox = QGroupBox(self)
        __netBox.setMaximumHeight(100)
        __netBox.setTitle(self.tr("Network settings"))
        __devBox = QGroupBox(self) 
        __devBox.setMaximumHeight(150)
        __devBox.setTitle(self.tr("Devices settings"))
        __brightnessBox = QGroupBox(self)
        __brightnessBox.setTitle(self.tr("Brightness settings"))
        __brightnessBox.setMaximumHeight(80)
        
        self.__tcpSocket = QTcpSocket(self)
        self.__udpSocket = QUdpSocket(self)
        self.__devHelper = DeviceProtocol(self)
        self.__receivedData = QByteArray()
        
        # doesn't work. ??????????????
        keys = QStyleFactory.keys()
        style = QStyleFactory.create(keys[2])
        self.setStyle(style)
        
        # GroupBox
        ____vLayout = QVBoxLayout()
        ____vLayout.addWidget(__netBox)
        ____vLayout.addWidget(__devBox)
        ____vLayout.addWidget(__brightnessBox)
        
        self.__widget = QWidget()
        self.__widget.setLayout(____vLayout)
        self.setCentralWidget(self.__widget)
        
        # set network box
        self.__lableIp = QLabel(self.__netBox)
        self.__lableIp.setMinimumWidth(85)
        self.__lableIp.setText(self.tr("Local IP: "))
        
        self.__hLayout4net = QHBoxLayout()
        self.__hLayout4net.addWidget(self.__lableIp)
        
        self.__combboxIp = QComboBox()
        self.__combboxIp.setMinimumSize(200, 25)
        self.__combboxIp.addItems(self.__networkScan.start())
        self.__hLayout4net.addWidget(self.__combboxIp)
        
        self.__refreshNetBtn = QPushButton(__netBox)
        self.__refreshNetBtn.setText(self.tr("Refresh network"))
        self.__hLayout4net.addWidget(self.__refreshNetBtn)
        self.__refreshNetBtn.clicked.connect(self.__onRefreshNetBtnClicked)
        
        __netBox.setLayout(self.__hLayout4net)
        
        # set devices box
        self.__vLayout = QVBoxLayout()
        self.__hLayout4Dev = QHBoxLayout()
        self.__hLayout4Btn = QHBoxLayout()
        self.__vLayout.addLayout(self.__hLayout4Dev)
        self.__vLayout.addLayout(self.__hLayout4Btn)
        
        self.__labelDev = QLabel(__devBox)
        self.__labelDev.setText(self.tr("Devices: "))
        self.__lableIp.setMinimumWidth(85)
        self.__hLayout4Dev.addWidget(self.__labelDev)
        
        self.__combboxDev = QComboBox(__devBox)
        self.__combboxDev.setMinimumSize(200, 25)
        self.__hLayout4Dev.addWidget(self.__combboxDev)
        self.__refreshDevBtn = QPushButton(__devBox)
        self.__refreshDevBtn.setText(self.tr("Refresh devices"))
        self.__hLayout4Dev.addWidget(self.__refreshDevBtn)
        self.__refreshDevBtn.clicked.connect(self.__onRefreshDevBtnClicked)
        
        self.__connectBtn = QPushButton(__devBox)
        self.__connectBtn.setText(self.tr("Connect"))
        self.__connectBtn.clicked.connect(self.__onConnectBtnClicked)
        self.__connectBtn.setDisabled(True)
        self.__switchBtn = QPushButton(__devBox)
        self.__switchBtn.setDisabled(True)
        self.__switchBtn.setText(self.tr("On/Off"))
        self.__switchBtn.clicked.connect(self.__onSwitchBtnClicked)
        self.__spacer1 = QSpacerItem(90, 20)
        self.__spacer2 = QSpacerItem(90, 20)
        self.__hLayout4Btn.addItem(self.__spacer1)
        self.__hLayout4Btn.addWidget(self.__connectBtn)
        self.__hLayout4Btn.addWidget(self.__switchBtn)
        self.__hLayout4Btn.addItem(self.__spacer2)
        __devBox.setLayout(self.__vLayout)
        
        # brightness settings
        self.__sliderBri = QSlider(__brightnessBox)
        self.__sliderBri.setOrientation(Qt.Orientation.Horizontal)
        self.__sliderBri.setRange(1, 100)
        self.__sliderBri.setDisabled(True)
        
        self.__hLayout4Bri = QHBoxLayout()
        self.__hLayout4Bri.addWidget(self.__sliderBri)
        __brightnessBox.setLayout(self.__hLayout4Bri)
        self.__sliderBri.valueChanged.connect(self.__onSliderValueChanged)
        
        # status bar
        self.__statusBar = QStatusBar()
        self.setStatusBar(self.__statusBar)
            
    
    @Slot()
    def __onSliderValueChanged(self):
        self.__statusBar.showMessage(self.tr("The current brightness is: ") \
                                   + str(self.__sliderBri.value()) + '%')
        deviceId = self.__devHelper.getDeviceId(self.__receivedData)
        cmd = self.__devHelper.getChangeBrightnessCmd(deviceId, int(self.__sliderBri.value()))
        self.__tcpSocket.write(cmd)
    
    
    @Slot()
    def __onRefreshNetBtnClicked(self):
        ips = self.__networkScan.refresh()
        self.__combboxIp.clear()
        for ip in ips:
            self.__combboxIp.addItem(ip)
        self.__statusBar.showMessage(self.tr("Local network refreshed!"))
            
            
    @Slot()
    def __onRefreshDevBtnClicked(self):
        self.__tcpSocket.close()
        # self.__udpSocket.close()
        
        self.__udpSocket.bind(QHostAddress(self.__combboxIp.currentText()), \
                              0, QUdpSocket.ShareAddress)
        self.__udpSocket.joinMulticastGroup(self.__devHelper.getCastAddress())
        size = self.__udpSocket.writeDatagram(self.__devHelper.getDatagramByteArray().data(), \
                                            # self.__devHelper.datagramByteArraySize(), \
                                            self.__devHelper.getCastAddress(), \
                                            self.__devHelper.getUdpPort())
        devIpList, devPortList = self.__getDeviceList()

        self.__combboxDev.clear()
        i = 0
        for _ in devIpList:
            self.__combboxDev.addItem(devIpList[i] + ':' + self.tr(str(devPortList[i])))        
        
        count = self.__combboxDev.count()
        if count != 0:
            self.__statusBar.showMessage(str(count) + self.tr(" device(s) found!"))
            self.__connectBtn.setEnabled(True)
        elif (count == 0):
            self.__statusBar.showMessage(self.tr("Device not found!"))
            self.__connectBtn.setDisabled(True)
    
    
    @Slot()
    def __onConnectBtnClicked(self):
        if (self.__combboxDev.currentText() == ""):
            QMessageBox.critical(self, self.tr("Error"), self.tr("Please select a device before connecting!"))
            return
        
        if self.__connectBtn.text() == self.tr("Connect"):
            self.__statusBar.showMessage(self.tr("Connected!"))
            self.__connectBtn.setText(self.tr("Disconnect"))
            self.__connectBtn.setStyleSheet("color: red;")
            self.__switchBtn.setEnabled(True)
            self.__connect2Dev()
        else:
            self.__statusBar.showMessage(self.tr("Disconnected!"))
            self.__connectBtn.setText(self.tr("Connect"))
            self.__connectBtn.setStyleSheet("color: black;")
            self.__switchBtn.setEnabled(False)
            self.__sliderBri.setEnabled(False)
            self.__tcpSocket.close()
            self.__udpSocket.close()
            return
        
        
    def __connect2Dev(self)-> bool:
        self.__tcpSocket.close()
        if (self.__connectBtn.text() == self.tr("Connect")):
            self.__connectBtn.setText(self.tr("Connect"))
            self.__statusBar.showMessage(self.tr("Device disconnected!"))
            self.__switchBtn.setDisabled(True)
            self.__sliderBri.setDisabled(True)
            return False
    
        deviceInfo = self.__combboxDev.currentText()
        deviceInfoList = deviceInfo.split(':')
        deviceIp = deviceInfoList[0]
        devicePort = deviceInfoList[1]
        
        self.__tcpSocket.connectToHost(QHostAddress(deviceIp), int(devicePort))
        self.__statusBar.showMessage("Device connection succeeded!")
        self.__connectBtn.setText(self.tr("Disconnect"))
        self.__sliderBri.setEnabled(True)
        self.__switchBtn.setEnabled(True)
        
        return True
    
    
    @Slot()
    def __onSwitchBtnClicked(self):
        switchCmd = QByteArray()
        switchCmd.clear()
        deviceId = self.__devHelper.getDeviceId(self.__receivedData)
        switchCmd.append(self.__devHelper.getCloseDeviceCmd(deviceId))
        self.__tcpSocket.write(switchCmd.data())
        
        
    def __getDeviceList(self):
        deviceIpList = list()
        devicePortList = list()
        tmpIp = QHostAddress()
        while True:
            if not (self.__udpSocket.hasPendingDatagrams()):
                break
            # the interface here is inconsistent with that of Cpp.
            data, address, port = self.__udpSocket.readDatagram(1024)
            self.__receivedData = data

            if (tmpIp.toString() == self.__devHelper.getDeviceAddress(self.__receivedData).toString()):
                continue
            deviceIpList.append(address.toString())
            devicePortList.append(self.__devHelper.getDevicePort(self.__receivedData))
            tmpIp = address
        
        return deviceIpList, devicePortList