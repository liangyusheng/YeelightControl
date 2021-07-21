#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mNetworkScaner(new NetworkScan)
    , mDeviceHelper(new DeviceProtocol)
    , mTcpSocket(new QTcpSocket)
    , mUdpSocket(new QUdpSocket)
{
    ui->setupUi(this);

    this->initUi();
    this->initSignalsAndSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi()
{
    this->mUdpSocket->close();
    this->mTcpSocket->close();
    //  初始化本地网络
    auto ipList = this->mNetworkScaner->start();
    for (const auto &ip : ipList)
    {
        ui->comboBox_local_ip->addItem(ip.toString());
    }
    ui->statusbar->showMessage(QString("找到 %1 个网络地址").arg(ipList.size()));

    ui->pushButton_on_off->setDisabled(true);

    ui->horizontalSlider_light->setRange(1, 100);
    ui->horizontalSlider_light->setDisabled(true);

    this->setFixedSize(this->width(), this->height());
    this->initStyle();
}

void MainWindow::initStyle()
{
    auto key = QStyleFactory::keys();
    auto style = QStyleFactory::create(key.at(2));

    this->setStyle(style);
    ui->pushButton_on_off->setStyle(style);
    ui->pushButton_connect_device->setStyle(style);
    ui->pushButton_refresh_device->setStyle(style);
    ui->pushButton_refresh_network->setStyle(style);
    ui->horizontalSlider_light->setStyle(style);
    ui->comboBox_local_ip->setStyle(style);
    ui->comboBox_device_list->setStyle(style);
}

void MainWindow::initSignalsAndSlots()
{
    // 连接按钮
    QObject::connect(ui->comboBox_device_list, &QComboBox::currentTextChanged, \
                     this, [this]()
    {
        if (ui->comboBox_device_list->currentText() == "")
        {
            ui->pushButton_connect_device->setDisabled(true);
            return ;
        }
        ui->pushButton_connect_device->setEnabled(true);
    });

    // 刷新网络
    QObject::connect(ui->pushButton_refresh_network, &QPushButton::clicked, \
                     this, [this]()
    {
        ui->pushButton_refresh_network->setDisabled(true);
        ui->comboBox_local_ip->clear();
        auto ipList = this->mNetworkScaner->start();
        for (const auto &ip : ipList)
        {
            ui->comboBox_local_ip->addItem(ip.toString());
        }
        ui->pushButton_refresh_network->setEnabled(true);
        ui->statusbar->showMessage(QString("找到 %1 个网络地址").arg(ipList.size()));
    });

    // 点击 “刷新设备”
    QObject::connect(ui->pushButton_refresh_device, &QPushButton::clicked, \
                     this, [this]()
    {
        // 刷新前先关闭连接
        this->mTcpSocket->close();
        // 清除当前设备列表
        ui->comboBox_device_list->clear();

        // UDP 加入广播网络
        this->mUdpSocket->bind(QHostAddress(ui->comboBox_local_ip->currentText()), \
                               0, QUdpSocket::ShareAddress);
        this->mUdpSocket->joinMulticastGroup(this->mDeviceHelper->getCastAddress());

        auto size = this->mUdpSocket->writeDatagram(this->mDeviceHelper->getDatagramByteArray().data(), \
                                                    this->mDeviceHelper->getDatagramByteArraySize(), \
                                                    this->mDeviceHelper->getCastAddress(), \
                                                    this->mDeviceHelper->getUdpPort());
        qDebug() << "UDP socket write " << size << " byte(s)";

        // 读取设备返回数据，获取设备列表
        QObject::connect(this->mUdpSocket, &QUdpSocket::readyRead, \
                         this, &MainWindow::getDeviceList);
    });

    // 点击 “连接设备”
    // 绑定设备
    QObject::connect(ui->pushButton_connect_device, &QPushButton::clicked, \
                     this, &MainWindow::connectToDevice);

    // 点击 “开/关”
    QObject::connect(ui->pushButton_on_off, &QPushButton::clicked, \
                     this, [this]()
    {
        QByteArray *switchCmd = new QByteArray;
        switchCmd->clear();
        auto deviceId = this->mDeviceHelper->getDeviceId(this->mReceivedData);
        switchCmd->append(this->mDeviceHelper->getCloseDeviceCmd(deviceId));
        // qDebug() << switchCmd->data();
        this->mTcpSocket->write(switchCmd->data());
    });

    // 调节亮度
    QObject::connect(ui->horizontalSlider_light, &QSlider::sliderReleased, \
                     this, [this]()
    {
        auto deviceId = this->mDeviceHelper->getDeviceId(this->mReceivedData);
        auto cmdStr = this->mDeviceHelper
                    ->getChangeBrightnessCmd(deviceId, \
                                             ui->horizontalSlider_light->value());
        // qDebug() << cmdStr;
        this->mTcpSocket->write(cmdStr);
    });
    QObject::connect(ui->horizontalSlider_light, &QSlider::valueChanged, \
                     this, [this](int value)
    {
        ui->statusbar->showMessage(QString("当前亮度 %1%").arg(value));
    });
}

bool MainWindow::connectToDevice()
{
    if (ui->comboBox_device_list->currentText() == "")
    {
        QMessageBox::critical(this, "错误", "请选择设备后进行连接！");
        return false;
    }

    if (ui->pushButton_connect_device->text() == "断开连接")
    {
        this->mTcpSocket->close();
        ui->pushButton_connect_device->setText("连接");
        ui->statusbar->showMessage("设备已断开连接！");
        ui->pushButton_on_off->setDisabled(true);
        ui->horizontalSlider_light->setDisabled(true);
        return false;
    }
    this->mTcpSocket->close();

    auto deviceInfo = ui->comboBox_device_list->currentText();
    auto deviceInfoList = deviceInfo.split(':');
    auto deviceIp = deviceInfoList.at(0);
    auto devicePort = deviceInfoList.at(1);

    // qDebug() << "IP: " << deviceIp << "\nPort: " << devicePort;
    //! TCP 连接到设备
    this->mTcpSocket->connectToHost(QHostAddress(deviceIp), static_cast<quint16>(devicePort.toUInt()));
    ui->statusbar->showMessage("设备连接成功！");
    ui->pushButton_connect_device->setText("断开连接");
    ui->horizontalSlider_light->setEnabled(true);
    ui->pushButton_on_off->setEnabled(true);

    return true;
}

void MainWindow::getDeviceList()
{
    QList<QHostAddress> deviceIpList;
    QList<qint64> devicePortList{};
    QHostAddress tmpIp;
    while (this->mUdpSocket->hasPendingDatagrams())
    {
        // 读取数据报
        this->mReceivedData.resize(static_cast<int>(this->mUdpSocket->pendingDatagramSize()));
        this->mUdpSocket->readDatagram(this->mReceivedData.data(), this->mReceivedData.size());
        // 接收设备返回信息
        // 获取 设备 IP，port
        // Location: yeelight://1.1.1.4:55443
        // qDebug() << "================ readDatagram ================\n"
        //          << this->mReceivedData.data();
        qDebug() << "received " << this->mReceivedData.size() << " byte(s)";

        // 去除重复项
        if (tmpIp.toString() == this->mDeviceHelper->getDeviceAddress(this->mReceivedData).toString())
        {
            continue;
        }
        deviceIpList.push_back(this->mDeviceHelper->getDeviceAddress(this->mReceivedData));
        devicePortList.push_back(this->mDeviceHelper->getDevicePort(this->mReceivedData));
        tmpIp = this->mDeviceHelper->getDeviceAddress(this->mReceivedData);
    }

    for (auto i = 0; i < deviceIpList.size(); i++)
    {
        ui->comboBox_device_list->addItem(deviceIpList.at(i).toString() + ':' + QString::number(devicePortList.at(i)));
    }
    ui->statusbar->showMessage(QString("获取到 %1 个设备").arg(deviceIpList.size()));
}


