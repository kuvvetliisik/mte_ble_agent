#include "connectionscreen.h"
#include "ui_connectionscreen.h"
#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QProcess>
#include <QListView>
#include "filetransfer.h"

ConnectionScreen::ConnectionScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionScreen),
    discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)),
    socket(nullptr),
    connectionCheckTimer(new QTimer(this))
{
    ui->setupUi(this);
    logger = new LogManager(this);

    ui->lblConnection->setText("🔴 Not Connected");

    connect(ui->btnScan, &QPushButton::clicked, this, &ConnectionScreen::updateBluetoothDevices);
    connect(ui->btnConnect, &QPushButton::clicked, this, &ConnectionScreen::connectToDevice);
    connect(ui->btnDisconnect, &QPushButton::clicked, this, &ConnectionScreen::disconnectDevice);
    connect(ui->btnClear, &QPushButton::clicked, this, &ConnectionScreen::clearLog);
    //connect(ui->btnRefresh, &QPushButton::clicked, this, &ConnectionScreen::refreshConnection);
    //connect(ConnectionScreen, &ConnectionScreen::bluetoothConnected, FileTransfer, &FileTransfer::setSocket);

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [this](const QBluetoothDeviceInfo &device) {
        QString deviceName = device.name().trimmed();
        QString macAddress = device.address().toString();
        int rssi = device.rssi();
        qDebug() << "📶 Cihaz RSSI:" << rssi;

        quint8 major = device.majorDeviceClass();
        quint8 minor = device.minorDeviceClass();


        qDebug() << "📦 Major Device Class:" << major;
        qDebug() << "🔸 Minor Device Class:" << minor;

        QString majorStr;
        switch (major) {
        case QBluetoothDeviceInfo::ComputerDevice:
            majorStr = "Computer"; break;
        case QBluetoothDeviceInfo::PhoneDevice:
            majorStr = "Phone"; break;
        case QBluetoothDeviceInfo::AudioVideoDevice:
            majorStr = "Sound / Video"; break;
        case QBluetoothDeviceInfo::PeripheralDevice:
            majorStr = "Peripheral (Keyboard, Mouse etc.)"; break;
        case QBluetoothDeviceInfo::HealthDevice:
            majorStr = "Health Device"; break;
        default:
            majorStr = "Unknown / Uncategorized";
        }

        QString minorStr;
        if (major == QBluetoothDeviceInfo::PhoneDevice) {
            switch (minor) {
            case 0:
                minorStr = "Unclassified Phone";
                break;
            case 1:
                minorStr = "Mobil Phone (Cellular)";
                break;
            case 2:
                minorStr = "Wireless Phone (Cordless)";
                break;
            case 3:
                minorStr = "Smartphone";
                break;
            case 4:
                minorStr = "Modem / Sound Gateway";
                break;
            case 5:
                minorStr = "ISDN Access";
                break;
            default:
                minorStr = "Unknown Subspecies";
            }
        } else {
            minorStr = "Minor is not interpreted for this major class";
        }

        if (major == QBluetoothDeviceInfo::ComputerDevice) {
            switch (minor) {
            case 0:
                minorStr = "Unclassified Computer";
                break;
            case 1:
                minorStr = "Desktop Computer";
                break;
            case 2:
                minorStr = "Server Computer";
                break;
            case 3:
                minorStr = "Laptop";
                break;
            case 4:
                minorStr = "Handheld PC / PDA";
                break;
            case 5:
                minorStr = " PC (Palm)";
                break;
            case 6:
                minorStr = "Wearable Computer";
                break;
            case 7:
                minorStr = "Tablet";
                break;
            default:
                minorStr = "Unknown Computer Subtype";
            }
        }

        if (major == QBluetoothDeviceInfo::PhoneDevice) {
        } else if (major == QBluetoothDeviceInfo::ComputerDevice) {
        } else {
            minorStr = "Minor is not interpreted for this major class";
        }


        if (deviceName.isEmpty()) {
            deviceName = macAddress;
        }

        /*if (rssi == 0) {
            qDebug() << "⚠️ Geçersiz RSSI, cihaz atlandı:" << device.address().toString();
            return; //
        }*/

        QString icon;
        if (major == QBluetoothDeviceInfo::PhoneDevice) icon = "📱 ";
        else if (major == QBluetoothDeviceInfo::ComputerDevice) icon = "💻 ";
        else if (major == QBluetoothDeviceInfo::AudioVideoDevice) icon = "🎧 ";
        else if (major == QBluetoothDeviceInfo::PeripheralDevice) icon = "🖱️ ";
        else icon = "📦 ";

        QString deviceStr = QString("%1: %2 = %3").arg(icon, deviceName, macAddress);
        //const auto deviceStr = QString("%1=%2").arg(deviceName).arg(macAddress);

        qDebug() << "Found Device:" << deviceName << "| MAC:" << macAddress;

        if (!devicess.contains(deviceStr)) {
            devicess[deviceStr] = macAddress;
            rssiValues[macAddress] = rssi;
            ui->comboBox->addItem(deviceStr);
        }
    });

    connect(connectionCheckTimer, &QTimer::timeout, this, [=]() {
        /*qDebug() << "Timer tetiklendi";
        if (socket)
            qDebug() << "Socket status : " << socket;
        if (socket)
            qDebug() << "Socket is open : " << socket->isOpen();*/
    });
    connectionCheckTimer->start(3000);

    QListView *comboListView = new QListView(ui->comboBox);
    QApplication::setStyle("Windows");
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

    connectionDisplayTimer = new QTimer(this);
    connectionDisplayTimer->setInterval(1000);

    connect(connectionDisplayTimer, &QTimer::timeout, this, [=]() {
        if (socket && socket->isOpen() && connectionTimer.isValid()) {
            qint64 elapsedMs = connectionTimer.elapsed();
            int seconds = elapsedMs / 1000;
            int minutes = seconds / 60;
            seconds %= 60;

            QString durationStr = QString("⏱️ %1:%2")
                                      .arg(minutes, 2, 10, QLatin1Char('0'))
                                      .arg(seconds, 2, 10, QLatin1Char('0'));

            emit connectionDurationUpdated(durationStr);
        }
    });
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
    bool connectedSuccessfully = false;


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
    int rssi = rssiValues.value(macAddress, -99);

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
    qDebug()<< "connecttoservice çağrıldı";
    auto finalizeConnection = [this, selectedDevice, macAddress,rssi]() {
        if (socket && socket->isOpen()) {
            qDebug() << "✅ Bağlantı başarılı, emit deviceConnected";
            emit deviceConnected(selectedDevice, macAddress, rssiValues.value(macAddress, -99));
            qDebug() << "💥 Emit bluetoothConnected(socket); gönderiliyor";
            emit bluetoothConnected(socket);
            logger->logConnectionStatus(selectedDevice, macAddress, true, rssi);
            ui->lblConnection->setText("✅ Connected: " + selectedDevice);
            ui->lblConnection->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
            ui->txtLog->append("✅ Connected to device: " + selectedDevice);

            int rssi = rssiValues.value(macAddress, -99);
            double estimatedN = guessNFromRSSI(rssi);
            double distance = calculateDistance(-59, rssi, estimatedN);
            qDebug() << "📏 Estimated distance: " << distance << " metre";

            ui->txtLog->append("📡 RSSI: " + QString::number(rssi) + " dBm ");

            ui->txtLog->append("📏 Estimated distance: " + QString::number(distance, 'f', 2) + " m");
            //ui->txtLog->append("📏 Mesafe: " + QString::number(distance, 'f', 2) + " m");
            //updateConnectionStatusLabel(true);
            connectionTimer.start();
            connectionDisplayTimer->start();
            ui->txtLog->append("⏱️ Connection time counter initialised.");

        }

        else {
            qDebug() << "❌ finalizeConnection: socket is not open!";
        }

    };

    //Timerın eklenme sebebi signal slotun yakalanmamasından dolayı eklenmiştir.
    /* QTimer::singleShot(4000, this, [=]() {
        if (socket && socket->isOpen()) {
            qDebug() << "✅ GEÇ gelen bağlantı algılandı.";
            emit deviceConnected(selectedDevice, macAddress, rssiValues.value(macAddress, -99), bluetoothVersion);
        } else {
            qDebug() << "❌ 4 saniye sonunda hala bağlantı yok.";
        }
    });

    connect(socket, &QBluetoothSocket::connected, this, [=]() {
        qDebug() << "🔹 [TEST] ";
        ui->lblConnection->setText("✅ Connected: " + selectedDevice);
        ui->lblConnection->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        ui->txtLog->append("✅ Successfully connected to device: " + selectedDevice);

        /*int rssi = -99;
        if (rssiValues.contains(macAddress)) {
            rssi = rssiValues[macAddress];
        }
        qDebug() << "⚡ Emit çağrılıyor: deviceConnected";

    emit deviceConnected(selectedDevice, macAddress, rssi, bluetoothVersion);

   });

    QTimer::singleShot(3000, this, [=]() {
        if (socket->isOpen()) {
            qDebug() << "🔹 [TEST] Socket is still open, ensuring connection label update!";
            ui->lblConnection->setText("✅ Connected: " + selectedDevice);
            ui->lblConnection->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        }

    });
*/

    connect(socket, &QBluetoothSocket::connected, this, finalizeConnection);
    QTimer::singleShot(4000, this, finalizeConnection);
    connect(socket, &QBluetoothSocket::errorOccurred, this, [=](QBluetoothSocket::SocketError error) {
        ui->lblConnection->setText("⚠️ Connection Failed!");
        ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
        ui->txtLog->append("⚠️ Connection Error: " + socket->errorString());
        qDebug() << "⚠️ Error Code: " << error;
        qDebug() << "⚠️ Connection Error: " << socket->errorString();

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
    if (connectionTimer.isValid()) {
        qint64 elapsedMs = connectionTimer.elapsed();
        int seconds = elapsedMs / 1000;
        int minutes = seconds / 60;
        seconds %= 60;
        QString durationStr = QString("%1:%2")
                                  .arg(minutes, 2, 10, QChar('0'))
                                  .arg(seconds, 2, 10, QChar('0'));

        logger->logConnectionStatus(
            ui->comboBox->currentText(),
            devicess[ui->comboBox->currentText()],
            false,
            rssiValues.value(devicess[ui->comboBox->currentText()], -99),
            durationStr);
        QString durationText = QString("⏱️ Connection Duration: %1 dakika %2 seconds")
                                   .arg(minutes).arg(seconds);
        ui->txtLog->append(durationText);
        qDebug() << durationText;
    }


    if (socket) {
        socket->deleteLater();
        socket = nullptr;
    }
//h .i .s
    ui->lblConnection->setText("❌ Disconnected");
    ui->txtLog->append("Bluetooth connection has been fully terminated.");
    ui->btnConnect->setEnabled(true);
    connectionDisplayTimer->stop();
    emit connectionDurationUpdated("⏱️ --:--");

}
double ConnectionScreen::calculateDistance(int measuredPower, int rssi, double N)
{
    // Distance = 10 ^ ((Measured Power - RSSI) / (10 * N))
    double exponent = (measuredPower - rssi) / (10.0 * N);
    double distance = pow(10.0, exponent);
    return distance;
}

double ConnectionScreen::guessNFromRSSI(int rssi) {
    if (rssi >= -50) return 1.5;
    if (rssi >= -60) return 2.0;
    if (rssi >= -70) return 2.5;
    if (rssi >= -80) return 3.0;
    return 3.5;
}

void ConnectionScreen::clearLog() {
    ui->txtLog->clear();
}

void ConnectionScreen::setConnectionLabelText(const QString& text, const QString& color) {
    ui->lblConnection->setText(text);
    ui->lblConnection->setStyleSheet("color: " + color + "; font-weight: bold;");
}


