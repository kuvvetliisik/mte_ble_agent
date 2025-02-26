#include "connectionscreen.h"
#include "ui_connectionscreen.h"
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QProcess>

ConnectionScreen::ConnectionScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionScreen),
    discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)),
    socket(nullptr)
{
    ui->setupUi(this);
    ui->lblConnection->setText("🔴 Not Connected");

    connect(ui->btnScan, &QPushButton::clicked, this, &ConnectionScreen::updateBluetoothDevices);
    connect(ui->btnConnect, &QPushButton::clicked, this, &ConnectionScreen::connectToDevice);
    connect(ui->btnDisconnect, &QPushButton::clicked, this, &ConnectionScreen::disconnectDevice);
    connect(ui->btnClear, &QPushButton::clicked, this, &ConnectionScreen::clearLog);

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [this](const QBluetoothDeviceInfo &device) {
        QString deviceName = device.name().trimmed();
        QString macAddress = device.address().toString();
        int rssi = device.rssi();

        if (deviceName.isEmpty()) {
            deviceName = macAddress;
        }

        const auto deviceStr = QString("%1=%2").arg(deviceName).arg(macAddress);

        qDebug() << "Found Device:" << deviceName << "| MAC:" << macAddress;

        if (!devicess.contains(deviceStr)) {
            devicess[deviceStr] = macAddress;
            rssiValues[macAddress] = rssi;
            ui->comboBox->addItem(deviceStr);
        }
    });
}

ConnectionScreen::~ConnectionScreen() {
    delete ui;
}

void ConnectionScreen::updateBluetoothDevices() {
    ui->comboBox->clear();
    devicess.clear();
    ui->lblConnection->setText("🔍 Scanning for devices...");
    ui->txtLog->append("🔍 Scanning for devices...");
    discoveryAgent->start();
}

void ConnectionScreen::connectToDevice() {
    QString selectedDevice = ui->comboBox->currentText();
    QString bluetoothVersion = "Unknown";

    if (socket && socket->isOpen()) {
        ui->txtLog->append("⚠️ Device is already connected: " + selectedDevice);
        ui->btnConnect->setEnabled(false);
        return;
    }

    if (selectedDevice.isEmpty() || !devicess.contains(selectedDevice)) {
        ui->txtLog->append("⚠️ Please select a device!");
        return;
    }

    QString macAddress = devicess[selectedDevice];
    QBluetoothAddress bluetoothAddress(macAddress);
   // QBluetoothUuid serviceUuid("00001108-0000-1000-8000-00805f9b34fb");
    QBluetoothUuid serviceUuid("0000110d-0000-1000-8000-00805f9b34fb"); // A2DP UUID

    //QBluetoothUuid serviceUuid("0000110a-0000-1000-8000-00805f9b34fb");

    if (socket) {
        // if (socket->isOpen()) {
        //     socket->disconnectFromService();
        // }
        // delete socket;
        socket = nullptr;
    }

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    ui->lblConnection->setText("⏳ Connecting...");
    ui->txtLog->append("Connecting to device: " + selectedDevice);


    qDebug() << "🔹 [TEST] Trying to connect to Bluetooth device...";
    qDebug() << "   Selected Device: " << selectedDevice;
    qDebug() << "   MAC Address: " << macAddress;
    qDebug() << "   UUID: " << serviceUuid.toString();


    connect(socket, &QBluetoothSocket::connected, this, [=]() {
        ui->lblConnection->setText("✅ Connected: " + selectedDevice);
        ui->txtLog->append("Successfully connected to the device: " + selectedDevice);

        int rssi = -99;
        if (rssiValues.contains(macAddress)) {
            rssi = rssiValues[macAddress];
        }

        qDebug() << "🔹 CONNECTED SUCCESSFULLY!";
        qDebug() << "🔹 Emitting deviceConnected SIGNAL!";
        qDebug() << "Device Name: " << selectedDevice;
        qDebug() << "MAC Address: " << macAddress;
        qDebug() << "RSSI (Initial): " << rssi << " dBm";
        qDebug() << "Bluetooth Level: " << bluetoothVersion;
        emit deviceConnected(selectedDevice, macAddress, rssi, bluetoothVersion);

        QTimer::singleShot(2000, this, [=]() {
            if (rssiValues.contains(macAddress)) {
                int updatedRssi = rssiValues[macAddress];
                qDebug() << "🔹 Updated RSSI after connection: " << updatedRssi << " dBm";
                //emit deviceConnected(selectedDevice, macAddress, updatedRssi, bluetoothVersion);
            }
        });
    });

    connect(socket, &QBluetoothSocket::errorOccurred, this, [=](QBluetoothSocket::SocketError error) {
        ui->lblConnection->setText("⚠️ Connection error!");
        qDebug() << "⚠️ Error Code: " << error;
        //ui->txtLog->append("Connection error: " + socket->errorString());
        qDebug() << "⚠️ Connection Error: " << socket->errorString();
    });

    socket->connectToService(bluetoothAddress, serviceUuid);
}

void ConnectionScreen::disconnectDevice() {
    if (!socket || !socket->isOpen()) {
        ui->txtLog->append("⚠️ No active connection.");
        ui->lblConnection->setText("🔴 Not Connected");
        return;
    }

    ui->lblConnection->setText("⏳ Disconnecting...");
    ui->txtLog->append("Disconnecting from device...");

    emit deviceConnected("-", "-", -99, "Unknown");

    if (socket->isOpen()) {
        qDebug() << "Disconnecting from Bluetooth service...";
        socket->disconnectFromService();
        socket->close();
        qDebug() << "Bluetooth connection closed.";
    }

    if (devicess.contains(ui->comboBox->currentText())) {
        QString macAddress = devicess[ui->comboBox->currentText()];
        QString command = "bluetoothctl disconnect " + macAddress;

        QProcess process;
        process.start("bash", QStringList() << "-c" << command);
        process.waitForFinished();
        qDebug() << "Bluetoothctl disconnect command executed for: " << macAddress;
    }

    delete socket;
    socket = nullptr;

    ui->lblConnection->setText("❌ Disconnected");
    ui->txtLog->append("Bluetooth connection has been fully terminated.");
    ui->btnConnect->setEnabled(true);
}

void ConnectionScreen::clearLog() {
    ui->txtLog->clear();
}
