#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connectionScreen = new ConnectionScreen(this);
    deviceInfo = new DeviceInfo(this);
    fileTransfer = new FileTransfer(this);

    ui->stackedWidget->addWidget(connectionScreen);
    ui->stackedWidget->addWidget(deviceInfo);
    ui->stackedWidget->addWidget(fileTransfer);

    connect(ui->btnConnectionScreen, &QPushButton::clicked, this, &MainWindow::showConnectionScreen);
    connect(ui->btnDeviceInfo, &QPushButton::clicked, this, &MainWindow::showDeviceInfo);
    connect(deviceInfo, &DeviceInfo::refreshConnectionRequested, this, &MainWindow::refreshConnectionFromDeviceInfo);
    connect(ui->btnFile, &QPushButton::clicked, this, &MainWindow::showFileTransfer);

    qDebug() << "connectionScreen nesnesi: " << connectionScreen;
    qDebug() << "deviceInfo nesnesi: " << deviceInfo;
    bool success = connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    qDebug() << (success ? "✅ Signal-Slot bağlantısı başarılı!" : "❌ Signal-Slot bağlantısı başarısız!");
    connect(connectionScreen, &ConnectionScreen::connectionDurationUpdated,
            deviceInfo, &DeviceInfo::updateLiveConnectionDuration);

}

MainWindow::~MainWindow() {
    delete connectionScreen;
    delete deviceInfo;
    delete fileTransfer;
    delete ui;
}

void MainWindow::showConnectionScreen() {
    ui->stackedWidget->setCurrentWidget(connectionScreen);
    qDebug() << "🔹 Connection Screen ekranı çağrıldı, ekran değiştiriliyor...";

}
void MainWindow::showDeviceInfo() {
    qDebug() << "🔹 Device Info ekranı çağrıldı, ekran değiştiriliyor...";
    ui->stackedWidget->setCurrentWidget(deviceInfo);
}
void MainWindow::showFileTransfer() {
    qDebug() << "🔹 File Transfer ekranı çağrıldı, ekran değiştiriliyor...";
    ui->stackedWidget->setCurrentWidget(fileTransfer);
}

void MainWindow::handleDeviceConnected(QString deviceName, QString macAddress, int rssi )
{
     qDebug() << "Device Name: " << deviceName;
     qDebug() << "MAC Address: " << macAddress;
     qDebug() << "RSSI: " << rssi;
    deviceInfo->updateDeviceInfo(deviceName, macAddress, rssi);

   // showDeviceInfo();
}

void MainWindow::refreshConnectionFromDeviceInfo() {
    qDebug() << "🔄 Refresh button clicked from DeviceInfo screen.";

    QBluetoothSocket* socket = connectionScreen->socket;

    if (socket && socket->isOpen()) {
        qDebug() << "🔌 Socket is open. Disconnecting first...";

        // Disconnected sinyali geldiğinde tekrar bağlan
        QMetaObject::Connection handler;
        handler = connect(socket, &QBluetoothSocket::disconnected, this, [=]() {
            qDebug() << "✅ disconnected signal, reconnecting...";
            QObject::disconnect(handler); // bir kez çalışsın
            connectionScreen->connectToDevice();
        });

        //Eğer sinyal gelmezse fallback olarak 3 saniye sonra bağlanmayı dene
        QTimer::singleShot(3000, this, [=]() {
            if (connectionScreen->socket == nullptr || !connectionScreen->socket->isOpen()) {
                qDebug() << "⏱️ no disconnected signal, connecting as fallback...";
                connectionScreen->connectToDevice();
            } else {
                qDebug() << "⏱️ Still connected, no reconnection.";
            }
        });
        connectionScreen->setConnectionLabelText("🔄 Refreshing connection...");

        connectionScreen->disconnectDevice();
    } else {
        qDebug() << "ℹ️ No active socket. Connecting directly...";
        connectionScreen->connectToDevice();
    }
}




