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

    //connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    connect(ui->btnConnectionScreen, &QPushButton::clicked, this, &MainWindow::showConnectionScreen);
    connect(ui->btnDeviceInfo, &QPushButton::clicked, this, &MainWindow::showDeviceInfo);
    //connect(connectionScreen, &ConnectionScreen::dataReceivedFromDevice, this, &MainWindow::handleDeviceData);
    //connect(deviceInfo->ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshConnectionFromDeviceInfo);


    //connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    qDebug() << "connectionScreen nesnesi: " << connectionScreen;
    qDebug() << "deviceInfo nesnesi: " << deviceInfo;
    bool success = connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    qDebug() << (success ? "✅ Signal-Slot bağlantısı başarılı!" : "❌ Signal-Slot bağlantısı başarısız!");
    //checkBluetoothStatus();
    /*connect(connectionScreen, &ConnectionScreen::bluetoothDisconnected, this, [=]() {
        qDebug() << "🔁 bluetoothDisconnected sinyali alındı, yeniden bağlanılıyor...";
        QTimer::singleShot(1000, this, [=]() {
            connectionScreen->connectToDevice();
        });
    });*/

}

MainWindow::~MainWindow() {
    delete connectionScreen;
    delete deviceInfo;
    delete ui;
}

void MainWindow::showConnectionScreen() {
    ui->stackedWidget->setCurrentWidget(connectionScreen);
    qDebug() << "🔹 showConnectionScreen() çağrıldı, ekran değiştiriliyor...";

}
void MainWindow::showDeviceInfo() {
    qDebug() << "🔹 showDeviceInfo() çağrıldı, ekran değiştiriliyor...";
    ui->stackedWidget->setCurrentWidget(deviceInfo);
}

void MainWindow::handleDeviceConnected(QString deviceName, QString macAddress, int rssi, QString bluetoothversion )
{

     //qDebug() << "✅ handleDeviceConnected() CALLED";
     qDebug() << "Device Name: " << deviceName;
     qDebug() << "MAC Address: " << macAddress;
     qDebug() << "RSSI: " << rssi;
     qDebug() << "Bluetooth Level: " << bluetoothversion;
     //deviceInfo->updateDeviceInfo(deviceName, macAddress, rssi, bluetoothversion);

    /*deviceInfo->ui->lblDeviceName->setText(deviceName);
    deviceInfo->ui->lblMacAddress->setText(macAddress);
    deviceInfo->ui->lblSignalStrength->setText(QString::number(rssi) + " dBm");
    deviceInfo->ui->lblBluetoothVersion->setText(bluetoothversion);*/
    deviceInfo->updateDeviceInfo(deviceName, macAddress, rssi, bluetoothversion);

    showDeviceInfo();
}

/*void MainWindow::refreshConnectionFromDeviceInfo() {
    qDebug() << "🔄 Refresh button clicked from DeviceInfo screen.";

    if (connectionScreen) {
        qDebug() << "🔄 connectionScreen->disconnectDevice(); çağrılıyor...";
        connectionScreen->disconnectDevice();

        QTimer::singleShot(2000, this, [=]() {
            qDebug() << "🔁 Timeout sonrası bağlantı tekrar deneniyor.";
            connectionScreen->connectToDevice();
        });

        showConnectionScreen();
    }
}
*/


