#ifndef DEVICEPROTOCOL_H
#define DEVICEPROTOCOL_H

#include <QHostAddress>
#include <QObject>

class DeviceProtocol
{
public:
    DeviceProtocol();
    virtual ~DeviceProtocol() = default;

    QByteArray getDatagramByteArray() const;
    int getDatagramByteArraySize() const;
    QHostAddress getCastAddress() const;
    quint16 getUdpPort() const;
    const char *getCloseDeviceCmd(const char *deviceId) const;
    const char *getChangeBrightnessCmd(const char *deviceId, int level) const;
    const QHostAddress getDeviceAddress(QByteArray receivedData) const;
    qint64 getDevicePort(QByteArray receivedData) const;
    const char *getDeviceId(QByteArray receivedData) const;
};

#endif // DEVICEPROTOCOL_H
