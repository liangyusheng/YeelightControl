#include "DeviceProtocol.h"

DeviceProtocol::DeviceProtocol()
{
}

QByteArray DeviceProtocol::getDatagramByteArray() const
{
    QByteArray datagram = "M-SEARCH * HTTP/1.1\r\n\
            HOST: 239.255.255.250:1982\r\n\
            MAN: \"ssdp:discover\"\r\n\
            ST: wifi_bulb";
            return datagram;
}

int DeviceProtocol::getDatagramByteArraySize() const
{
    QByteArray datagram = "M-SEARCH * HTTP/1.1\r\n\
            HOST: 239.255.255.250:1982\r\n\
            MAN: \"ssdp:discover\"\r\n\
            ST: wifi_bulb";
            return datagram.size();
}

QHostAddress DeviceProtocol::getCastAddress() const
{
    return QHostAddress("239.255.255.250");
}

quint16 DeviceProtocol::getUdpPort() const
{
    return 1982;
}

const char *DeviceProtocol::getCloseDeviceCmd(const char *deviceId) const
{
    QByteArray cmdStr = "{\"id\":";
    cmdStr.append(deviceId);
    cmdStr.append(",\"method\":\"toggle\",\"params\":[]}\r\n");

    return cmdStr.data();
}

const char *DeviceProtocol::getChangeBrightnessCmd(const char *deviceId, int level) const
{
    QByteArray cmdStr = "{\"id\":";
    cmdStr.append(deviceId);
    cmdStr.append(",\"method\":\"set_bright\",\"params\":[");
    cmdStr.append(QString::number(level));
    cmdStr.append(", \"smooth\", 500]}\r\n");

    return cmdStr.data();
}
/*
 * HTTP/1.1 200 OK
 * Cache-Control: max-age=3600
 * Date:
 * Ext:
 * Location: yeelight://1.1.1.4:55443
 * Server: POSIX UPnP/1.0 YGLC/1
 * id: 0x000000000734b805
 * model: desklamp
 * fw_ver: 62
 * support: get_prop set_default set_power toggle set_ct_abx set_bright start_cf stop_cf set_scene cron_add cron_get cron_del set_adjust adjust_bright adjust_ct set_name
 * power: on
 * bright: 100
 * color_mode: 2
 * ct: 6132
 * rgb: 0
 * hue: 0
 * sat: 0
 * name:
 */
const QHostAddress DeviceProtocol::getDeviceAddress(QByteArray receivedData) const
{
    QString datagramString(receivedData);
    auto infoList = datagramString.split(":");

    auto ipTemp = infoList.at(5);
    QString ip{};
    for (const auto &i : ipTemp)
    {
        if (i != '/')
        {
            ip.append(i);
        }
    }
    // qDebug() << "ip is : " << ip;

    return QHostAddress(ip);
}

qint64 DeviceProtocol::getDevicePort(QByteArray receivedData) const
{
    QString datagramString(receivedData);
    auto infoList = datagramString.split(":");

    auto portTemp = infoList.at(6);
    QString port{};

    for (const auto &i : portTemp)
    {
        if (i == '\r' || i =='\n')
            break;
        port.append(i);
    }

    // qDebug() << "port is " << port.toInt();
    return port.toInt();
}

const char *DeviceProtocol::getDeviceId(QByteArray receivedData) const
{
    auto deviceInfoList = receivedData.split(':');
    auto idTemp = deviceInfoList.at(8);
    QString deviceId{};

    for (const auto &i : idTemp)
    {
        if (i == '\r' || i =='\n')
            break;
        deviceId.append(i);
    }

    return deviceId.toStdString().c_str();
}
