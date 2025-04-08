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

void DeviceInfo::updateDeviceInfo(QString deviceName, QString macAddress, int rssi) {
    qDebug() << "✅ updateDeviceInfo() CALLED";

    if (deviceName.isEmpty() || macAddress.isEmpty()) {
        qDebug() << "❌ ERROR: Device Info is EMPTY!";
        return;
    }

    ui->lblDeviceName->setText("Device Name: " + deviceName);
    ui->lblMacAddress->setText("MAC Address: " + macAddress);
    ui->lblSignalStrength->setText("Signal Strength Level(RSSI) :"+  QString::number(rssi) + "  dBm");
    ui->lblConnectionStatus_2->setText("Connection Status :");
    qDebug() << "🔹 QLabel'ler güncellendi!";
}

void DeviceInfo::setConnectionStatusLabel(bool connected) {
    if (connected) {
        ui->lblConnectionStatus_2->setText("✅ Connected");
        ui->lblConnectionStatus_2->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->lblConnectionStatus_2->setText("🔴 Not Connected");
        ui->lblConnectionStatus_2->setStyleSheet("color: red; font-weight: bold;");
    }
}
void DeviceInfo::updateLiveConnectionDuration(const QString &duration)
{
    //ui->lblDuration->setText(duration);    // UI'deki QLabel'a süreyi yaz
    ui->lblDuration->setText("Connection Time   : " +  duration );
}

