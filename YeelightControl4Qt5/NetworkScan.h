#ifndef NETWORKSCAN_H
#define NETWORKSCAN_H

#include <QObject>
#include <QWidget>
#include <QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork>

class NetworkScan : public QObject
{
    Q_OBJECT
public:
    explicit NetworkScan(QObject *parent = nullptr);
    virtual ~NetworkScan() = default;

    QList<QHostAddress> start() const;
    QList<QHostAddress> refresh() const;

signals:

public slots:
};

#endif // NETWORKSCAN_H
