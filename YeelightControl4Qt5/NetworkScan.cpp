#include "NetworkScan.h"

NetworkScan::NetworkScan(QObject *parent)
    : QObject(parent)
{

}

QList<QHostAddress> NetworkScan::start() const
{
    auto localIpAddress = QNetworkInterface::allAddresses();
    QList<QHostAddress> localIPv4 {};

    for (auto &ip : localIpAddress)
    {
        if (ip.protocol() == QAbstractSocket::IPv4Protocol)
            localIPv4.append(ip);
    }

    return localIPv4;
}

QList<QHostAddress> NetworkScan::refresh() const
{
    return this->start();
}
