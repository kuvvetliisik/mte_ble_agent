#include "deviceinfo.h"
#include "ui_deviceinfo.h"

DeviceInfo::DeviceInfo(QWidget *parent)
    : QWidget(parent), ui(new Ui::DeviceInfo) {
    ui->setupUi(this);
    connect(ui->btnRefresh, &QPushButton::clicked, this, [=]() {
        emit refreshConnectionRequested();
    });
}

DeviceInfo::~DeviceInfo() {
    delete ui;
}

void DeviceInfo::updateDeviceInfo(QString deviceName, QString macAddress, int rssi, QString bluetoothVersion) {
    qDebug() << "✅ updateDeviceInfo() CALLED";

    if (deviceName.isEmpty() || macAddress.isEmpty()) {
        qDebug() << "❌ ERROR: Device Info is EMPTY!";
        return;
    }

    ui->lblDeviceName->setText("Device Name: " + deviceName);
    ui->lblMacAddress->setText("MAC Address: " + macAddress);
    ui->lblSignalStrength->setText("Signal Strength Level(RSSI) :"+  QString::number(rssi) + "  dBm");
    ui->lblConnectionStatus_2->setText("Connection Status :");
    ui->lblBluetoothVersion->setText("Bluetooth Level:");
    qDebug() << "🔹 QLabel'ler güncellendi!";
}


