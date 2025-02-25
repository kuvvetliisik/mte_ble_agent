#include "deviceinfo.h"
#include "ui_deviceinfo.h"

DeviceInfo::DeviceInfo(QWidget *parent)
    : QWidget(parent), ui(new Ui::DeviceInfo) {
    ui->setupUi(this);
}

DeviceInfo::~DeviceInfo() {
    delete ui;
}

void DeviceInfo::updateDeviceInfo(QString deviceName, QString macAddress, int rssi, QString bluetoothVersion) {
    qDebug() << "✅ updateDeviceInfo() CALLED";
    qDebug() << "Device Name: " << deviceName;
    qDebug() << "MAC Address: " << macAddress;
    qDebug() << "RSSI: " << rssi;
    qDebug() << "Bluetooth Level: " << bluetoothVersion;

    ui->lblDeviceName->setText("Device Name: " + deviceName);
    ui->lblMacAddress->setText("MAC Address: " + macAddress);
    ui->lblSignalStrength->setText(QString::number(rssi) + " dBm");
    ui->lblConnectionStatus_2->setText("Status: Connected");

    if (rssi != -99) {
        ui->lblSignalStrength->setText("Signal Strength (RSSI): " + QString::number(rssi) + " dBm");
    } else {
        ui->lblSignalStrength->setText("Signal Strength: Unknown");
    }

    ui->lblBluetoothVersion->setText("Bluetooth Level: " + bluetoothVersion);

    if (deviceName.isEmpty() || macAddress.isEmpty()) {
        qDebug() << "❌ ERROR: Device Info is EMPTY!";
        return;
    }


}


