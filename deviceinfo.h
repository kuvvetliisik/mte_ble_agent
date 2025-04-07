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
    Ui::DeviceInfo *ui;
    ~DeviceInfo();
    void updateDeviceInfo(QString deviceName, QString macAddress, int rssi, QString bluetoothVersion);

signals:
        void refreshConnectionRequested();
private:
     //Ui::DeviceInfo *ui;

};

#endif
