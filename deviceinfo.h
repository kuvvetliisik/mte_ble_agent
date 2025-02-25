#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceInfo; }
QT_END_NAMESPACE

class DeviceInfo : public QWidget {
    Q_OBJECT

public:
    explicit DeviceInfo(QWidget *parent = nullptr);
    ~DeviceInfo();

    void updateDeviceInfo(QString deviceName, QString macAddress, int rssi, QString bluetoothVersion);

private:
    Ui::DeviceInfo *ui;
};

#endif
