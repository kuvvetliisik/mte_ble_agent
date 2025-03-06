#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connectionScreen = new ConnectionScreen(this);
    deviceInfo = new DeviceInfo(this);

    ui->stackedWidget->addWidget(connectionScreen);
    ui->stackedWidget->addWidget(deviceInfo);

    connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    connect(ui->btnConnectionScreen, &QPushButton::clicked, this, &MainWindow::showConnectionScreen);
    connect(ui->btnDeviceInfo, &QPushButton::clicked, this, &MainWindow::showDeviceInfo);

}

MainWindow::~MainWindow() {
    delete connectionScreen;
    delete deviceInfo;
    delete ui;
}

void MainWindow::showConnectionScreen() {
    ui->stackedWidget->setCurrentWidget(connectionScreen);
}

void MainWindow::handleDeviceConnected(QString deviceName, QString macAddress, int rssi, QString bluetoothversion )
{

    qDebug() << "✅ handleDeviceConnected() CALLED";
    qDebug() << "Device Name: " << deviceName;
    qDebug() << "MAC Address: " << macAddress;
    qDebug() << "RSSI: " << rssi;
    qDebug() << "Bluetooth Level: " << bluetoothversion;
    deviceInfo->updateDeviceInfo(deviceName, macAddress, rssi, bluetoothversion);

    deviceInfo->ui->lblDeviceName->setText(deviceName);
    deviceInfo->ui->lblMacAddress->setText(macAddress);
    deviceInfo->ui->lblSignalStrength->setText(QString::number(rssi) + " dBm");
    deviceInfo->ui->lblBluetoothVersion->setText(bluetoothversion);

    //showDeviceInfo();

}

void MainWindow::showDeviceInfo() {
    ui->stackedWidget->setCurrentWidget(deviceInfo);
}
