#ifndef DEVICEINFO_H
#define DEVICEINFO_H
#include "ui_deviceinfo.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceInfo; }
QT_END_NAMESPACE

class DeviceInfo : public QWidget {
    Q_OBJECT

public:
    explicit DeviceInfo(QWidget *parent = nullptr);
    ~DeviceInfo();
    Ui::DeviceInfo *ui;

    void updateDeviceInfo(QString deviceName, QString macAddress, int rssi, QString bluetoothVersion);

private:

};

#endif
