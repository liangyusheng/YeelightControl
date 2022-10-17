#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NetworkScan.h"
#include "DeviceProtocol.h"
#include <QUdpSocket>
#include <QMessageBox>
#include <algorithm>
#include <QSlider>
#include <QStyle>
#include <QStyleFactory>

/*
 * UDP 获取设备列表
 * TCP 进行设备控制
 */

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow
        : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    NetworkScan *mNetworkScaner = nullptr;
    DeviceProtocol *mDeviceHelper = nullptr;
    QTcpSocket *mTcpSocket = nullptr;
    QUdpSocket *mUdpSocket = nullptr;
    QByteArray mReceivedData;

    void initUi();
    void initStyle();
    void initSignalsAndSlots();
    bool connectToDevice();
    void getDeviceList();
};
#endif // MAINWINDOW_H
