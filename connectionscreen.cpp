#include "connectionscreen.h"
#include "ui_connectionscreen.h"
#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QProcess>
#include <QListView>

ConnectionScreen::ConnectionScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionScreen),
    discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)),
    socket(nullptr),
    connectionCheckTimer(new QTimer(this))
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

    connect(connectionCheckTimer, &QTimer::timeout, this, [=]() {
        if (socket && !socket->isOpen()) {
            qDebug()<< "⚠️ Bağlantı fiziksel olarak kopmuş, handleDisconnected() çağırılıyor!";
            handleDisconnected();
        }
    });
    connectionCheckTimer->start(3000);

    QListView *comboListView = new QListView(ui->comboBox);
    comboListView->setMaximumHeight(500);
    comboListView->setFixedWidth(280);
    comboListView->setStyleSheet(
        "QComboBox::drop-down {"
        "   border: none; "
        "   background-color: #2E2E2E; "
        "   width: 20px; "
        "   padding: 0px; "
        "   margin: 0px; "
        "}"
        );
    ui->comboBox->setView(comboListView);
}

ConnectionScreen::~ConnectionScreen() {
    delete ui;
}

void ConnectionScreen::updateBluetoothDevices() {


    ui->comboBox->setUpdatesEnabled(false);
    ui->comboBox->clear();
    devicess.clear();
    ui->lblConnection->setText("🔍 Scanning for devices...");
    ui->txtLog->append("🔍 Scanning for devices...");

    if (discoveryAgent->isActive()) {
        discoveryAgent->stop();
    }

    discoveryAgent->start();
    ui->comboBox->setUpdatesEnabled(true);
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
    QBluetoothUuid serviceUuid("0000110a-0000-1000-8000-00805f9b34fb");

    if (socket) {
        delete socket;
        socket = nullptr;
    }

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    ui->lblConnection->setText("⏳ Connecting...");
    ui->lblConnection->setStyleSheet("color: yellow; font-weight: bold;");
    ui->txtLog->append("Connecting to device: " + selectedDevice);

    qDebug() << "🔹 [TEST] Trying to connect to Bluetooth device...";
    qDebug() << "   Selected Device: " << selectedDevice;
    qDebug() << "   MAC Address: " << macAddress;
    qDebug() << "   UUID: " << serviceUuid.toString();

    socket->connectToService(bluetoothAddress, serviceUuid);

    connect(socket, &QBluetoothSocket::connected, this, [=]() {
        qDebug() << "🔹 [TEST] Connection Established!";
        ui->lblConnection->setText("✅ Connected: " + selectedDevice);
        ui->lblConnection->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        ui->txtLog->append("✅ Successfully connected to device: " + selectedDevice);

        int rssi = -99;
        if (rssiValues.contains(macAddress)) {
            rssi = rssiValues[macAddress];
        }
        qDebug() << "⚡ Emit çağrılıyor: deviceConnected";
        emit deviceConnected(selectedDevice, macAddress, rssi, bluetoothVersion);
        bool success = connect(socket, &QBluetoothSocket::disconnected, this, &ConnectionScreen::handleDisconnected);
        qDebug() << (success ? "✅ Signal-Slot bağlantısı başarılı!" : "❌ Signal-Slot bağlantısı başarısız!");
    });

    QTimer::singleShot(3000, this, [=]() {
        if (socket->isOpen()) {
            qDebug() << "🔹 [TEST] Socket is still open, ensuring connection label update!";
            ui->lblConnection->setText("✅ Connected: " + selectedDevice);
            ui->lblConnection->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        }
    });

    connect(socket, &QBluetoothSocket::errorOccurred, this, [=](QBluetoothSocket::SocketError error) {
        ui->lblConnection->setText("⚠️ Connection Failed!");
        ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
        ui->txtLog->append("⚠️ Connection Error: " + socket->errorString());
        qDebug() << "⚠️ Error Code: " << error;
        qDebug() << "⚠️ Connection Error: " << socket->errorString();
        if (!socket || !socket->isOpen()) return;
         handleDisconnected();
    });
}

void ConnectionScreen::disconnectDevice() {
    if (!socket || !socket->isOpen()) {
        ui->txtLog->append("⚠️ No active connection.");
        ui->lblConnection->setText("🔴 Not Connected");
        return;
    }

    ui->lblConnection->setText("⏳ Disconnecting...");
    ui->txtLog->append("Disconnecting from device...");
    qDebug() << "⚡ Emit çağrılıyor: devicedConnected";

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

        QProcess *process = new QProcess(this);
        connect(process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, [=](QProcess::ProcessError error) {
            ui->txtLog->append("⚠️ Process Error: " + process->errorString());
            process->deleteLater();
        });
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                ui->txtLog->append("Bluetoothctl disconnect command executed successfully.");
            } else {
                ui->txtLog->append("⚠️ Bluetoothctl disconnect command failed.");
            }
            process->deleteLater();
        });
        process->start("bluetoothctl", QStringList() << "disconnect" << macAddress);
    }

    if (socket) {
        delete socket;
        socket = nullptr;
    }

    ui->lblConnection->setText("❌ Disconnected");
    ui->txtLog->append("Bluetooth connection has been fully terminated.");
    ui->btnConnect->setEnabled(true);
}

void ConnectionScreen::handleDisconnected() {
    qDebug() << "⚠️ Bluetooth bağlantısı kesildi!";
    ui->lblConnection->setText("🔴 Not Connected");
    ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
    ui->txtLog->append("⚠️ Device disconnected unexpectedly.");
    qDebug() << "⚡ Emit çağrılıyor: devicehnConnected";

    emit deviceConnected("-", "-", -99, "Unknown");

    if (socket) {
        socket->deleteLater();
        socket = nullptr;
    }

    ui->btnConnect->setEnabled(true);
}

void ConnectionScreen::clearLog() {
    ui->txtLog->clear();
}
