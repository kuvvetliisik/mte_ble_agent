#include "connectionscreen.h"
#include "ui_connectionscreen.h"
#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QProcess>
#include <QListView>
#include <QAbstractSocket>
#include <QBluetoothServer>
#include <QBluetoothServiceInfo>
#include <QBluetoothLocalDevice>


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
            majorStr = "Bilgisayar"; break;
        case QBluetoothDeviceInfo::PhoneDevice:
            majorStr = "Telefon"; break;
        case QBluetoothDeviceInfo::AudioVideoDevice:
            majorStr = "Ses / Video"; break;
        case QBluetoothDeviceInfo::PeripheralDevice:
            majorStr = "Çevre Birimi (Klavye, Mouse vs.)"; break;
        case QBluetoothDeviceInfo::HealthDevice:
            majorStr = "Sağlık Cihazı"; break;
        default:
            majorStr = "Bilinmeyen / Uncategorized";
        }

        qDebug() << "🧠 Cihaz Türü Yorumlanmış:" << majorStr;

        QString minorStr;
        if (major == QBluetoothDeviceInfo::PhoneDevice) {
            switch (minor) {
            case 0:
                minorStr = "Sınıflandırılmamış Telefon";
                break;
            case 1:
                minorStr = "Cep Telefonu (Cellular)";
                break;
            case 2:
                minorStr = "Kablosuz Telefon (Cordless)";
                break;
            case 3:
                minorStr = "Smartphone";
                break;
            case 4:
                minorStr = "Modem / Ses Geçidi";
                break;
            case 5:
                minorStr = "ISDN Erişimi";
                break;
            default:
                minorStr = "Bilinmeyen Alt Tür";
            }
        } else {
            minorStr = "Bu major class için minor yorumlanmadı";
        }

        qDebug() << "🧩 Cihaz Alt Türü (Minor):" << minorStr;

        if (major == QBluetoothDeviceInfo::ComputerDevice) {
            switch (minor) {
            case 0:
                minorStr = "Sınıflandırılmamış Bilgisayar";
                break;
            case 1:
                minorStr = "Masaüstü Bilgisayar";
                break;
            case 2:
                minorStr = "Sunucu Bilgisayar";
                break;
            case 3:
                minorStr = "Dizüstü Bilgisayar (Laptop)";
                break;
            case 4:
                minorStr = "Elde Taşınabilir PC / PDA";
                break;
            case 5:
                minorStr = "Avuç içi PC (Palm)";
                break;
            case 6:
                minorStr = "Giyilebilir Bilgisayar";
                break;
            case 7:
                minorStr = "Tablet";
                break;
            default:
                minorStr = "Bilinmeyen Bilgisayar Alt Türü";
            }
        }

        if (major == QBluetoothDeviceInfo::PhoneDevice) {
        } else if (major == QBluetoothDeviceInfo::ComputerDevice) {
        } else {
            minorStr = "Bu major class için minor yorumlanmadı";
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
        qDebug() << "Timer tetiklendi";
        if (socket)
            qDebug() << "Socket status : " << socket;
        if (socket)
            qDebug() << "Socket is open : " << socket->isOpen();
    });
    connectionCheckTimer->start(3000);
  /*  connect(connectionCheckTimer, &QTimer::timeout, this, [=]() {
        qDebug() << "🕒 [TIMER] Tick";

        if (!socket) {
            qDebug() << "⚠️ [TIMER] socket null, kontrol yok.";
            return;
        }

        auto currentState = socket->state();
        bool isOpen = socket->isOpen();

        qDebug() << "🔍 [TIMER] socket->isOpen():" << isOpen;
        qDebug() << "🔍 [TIMER] socket->state():" << currentState;

        if (!isOpen || currentState != QBluetoothSocket::SocketState::ConnectedState) {

            qDebug() << "🚫 [TIMER] Bağlantı henüz oturmamış. Ping atılmıyor.";
            return;
        }

        QByteArray testData = "ping";
        socket->write(testData);
        bool success = socket->waitForBytesWritten(1000);

        if (!success) {
            qDebug() << "🧨 Ping başarısız → bağlantı kopmuş olabilir!";
            ui->lblConnection->setText("🔴 Disconnected (Ping Timeout)");
            ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
            ui->txtLog->append("🛑 Timer: ping başarısız, bağlantı kopmuş olabilir.");

            emit connectionDurationUpdated("⏱️ --:--");
            connectionDisplayTimer->stop();

            socket->deleteLater();
            socket = nullptr;

            ui->btnConnect->setEnabled(true);
        } else {
            qDebug() << "📤 Ping başarılı.";
        }
    });

    connectionCheckTimer->start(3000);*/

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
    connectionDisplayTimer->setInterval(1000); // her saniye çalışır

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
    // Bluetooth Server Başlat
    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, &QBluetoothServer::newConnection, this, &ConnectionScreen::clientConnected);
    qDebug() << "📱 Yeni bir uzak bağlantı algılandı!";
    this->clientConnected();

    QBluetoothAddress adapterAddress = QBluetoothLocalDevice().address();
    qDebug() << "Adapter address:" << adapterAddress.toString();

    bool success = rfcommServer->listen(adapterAddress);
    qDebug() << (success ? "✅ Dinleme başladı" : "❌ Dinleme BAŞARISIZ");

    QBluetoothUuid fixedUuid("00001101-0000-1000-8000-00805F9B34FB");

    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(fixedUuid);
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, "MyBluetoothServer");
    serviceInfo.setServiceUuid(fixedUuid);
    serviceInfo.setDevice(QBluetoothDeviceInfo(QBluetoothLocalDevice().address(), "LocalDevice", 0));

    // 🔥 UUID çakışmalarını engelle
    if (serviceInfo.isRegistered()) {
        serviceInfo.unregisterService();
    }
    serviceInfo.registerService(adapterAddress);



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
    //connect(socket, &QBluetoothSocket::disconnected, this, &ConnectionScreen::handleDisconnection);
    connect(socket, &QBluetoothSocket::disconnected, this, &ConnectionScreen::onRemoteDisconnected);
    connect(socket, &QBluetoothSocket::stateChanged, this, &ConnectionScreen::onSocketStateChanged);


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
        connectionTimer.start();// Bağlantı süresi ölçümü başlar
        connectionDisplayTimer->start();
        ui->txtLog->append("⏱️ Bağlantı süresi sayacı başlatıldı.");

        }

        else {
            qDebug() << "❌ finalizeConnection: socket açık değil!";
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
        }connect(socket, &QBluetoothSocket::connected, this, finalizeConnection);
        qDebug() << "⚡ Emit çağrılıyor: deviceConnected";

    emit deviceConnected(selectedDevice, macAddress, rssi, bluetoothVersion);

   });

    QTimer::singleShot(3000, this, [=]() {
        if (socket->isOpen()) {
            qDebug() << "🔹 [TEST] Socket is still open, ensuring connection label update!";
            ui->lblConnection->setText("✅ Connected: " + selectedDevice);
            ui->lblConnection->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        }connect(socket, &QBluetoothSocket::connected, this, finalizeConnection);

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
       /* if (error == QBluetoothSocket::RemoteHostClosedError ||
            error == QBluetoothSocket::NetworkError ||
            error == QBluetoothSocket::OperationError) {
            // Bu hatalar bağlantının koptuğunu gösterebilir
            handleDisconnection();
        }*/
    });
}

void ConnectionScreen::disconnectDevice() {
      disconnectInitiatedByUs = true;
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
        QString durationText = QString("⏱️ Bağlantı süresi: %1 dakika %2 saniye")
                                   .arg(minutes).arg(seconds);
        ui->txtLog->append(durationText);
        qDebug() << durationText;
    }


    if (socket) {
        socket->deleteLater();
        socket = nullptr;
    }

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

/*void ConnectionScreen::handleDisconnection()
{
    if (!socket) return;

    qDebug() << "🔌 Connection lost, reason:" << socket->errorString();
    if (connectionTimer.isValid()) {
        qint64 elapsedMs = connectionTimer.elapsed();
        int seconds = elapsedMs / 1000;
        int minutes = seconds / 60;
        seconds %= 60;
        QString durationStr = QString("%1:%2")
                                  .arg(minutes, 2, 10, QChar('0'))
                                  .arg(seconds, 2, 10, QChar('0'));

        QString deviceName = ui->comboBox->currentText();
        QString macAddress = devicess.value(deviceName, "unknown");

        logger->logConnectionStatus(
            deviceName,
            macAddress,
            false,
            rssiValues.value(macAddress, -99),
            durationStr);

        QString durationText = QString("⏱️ Connection duration: %1 minutes %2 seconds")
                                   .arg(minutes).arg(seconds);
        ui->txtLog->append(durationText);
        ui->txtLog->append("❌ Connection lost: " + socket->errorString());
    }

    ui->lblConnection->setText("❌ Disconnected");
    ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
    ui->btnConnect->setEnabled(true);

    connectionDisplayTimer->stop();
    emit connectionDurationUpdated("⏱️ --:--");

    socket->deleteLater();
    socket = nullptr;
}
*/
void ConnectionScreen::clientConnected()
{
    qDebug() << "📡 clientConnected() çağrıldı!";

    // Mevcut socket temizle
    if (socket) {
        disconnect(socket, nullptr, this, nullptr);
        socket->deleteLater();
        socket = nullptr;
    }

    // Yeni bağlantıyı al
    socket = rfcommServer->nextPendingConnection();

    if (!socket) {
        qDebug() << "❌ Bağlantı alınamadı!";
        return;
    }
    QMetaObject::invokeMethod(this, [this]() {
        ui->lblConnection->setText("✅ Connected (Remote)");
        ui->lblConnection->setStyleSheet("color: green; font-weight: bold;");
        ui->txtLog->append("✅ Uzak cihaz bağlandı!");
});

    // Debug bilgisi
    QBluetoothAddress remoteAddress = socket->peerAddress();
    QString macAddress = remoteAddress.toString();
    QString deviceName = "Uzak Cihaz (" + macAddress + ")";

    qDebug() << "✅ Uzak cihaz bağlandı: " << macAddress;

    // UI güncellemesi için direkt fonksiyon çağrısı yap
    updateConnectionLabel("✅ Connected: " + deviceName, "green");
    appendToLog("✅ Uzak cihazdan gelen bağlantı kabul edildi!");

    // Bağlantı süresini başlat
    connectionTimer.start();
    connectionDisplayTimer->start();

    // Sinyalleri bağla
    connect(socket, &QBluetoothSocket::disconnected, this, &ConnectionScreen::onRemoteDisconnected);
    connect(socket, &QBluetoothSocket::stateChanged, this, &ConnectionScreen::onSocketStateChanged);

    // Veri geldiğinde işle
    connect(socket, &QBluetoothSocket::readyRead, this, [this]() {
        QByteArray data = socket->readAll();
        qDebug() << "📥 Veri alındı:" << data;
        appendToLog("📥 Uzak cihazdan veri alındı: " + QString(data));
    });

    // Cihaz bağlantı sinyalini gönder
    emit deviceConnected(deviceName, macAddress, -99);
}
void ConnectionScreen::onRemoteDisconnected()
{
    qDebug() << "🔌 onRemoteDisconnected() çağrıldı!";

    // Bağlantı süresi hesapla
    QString durationStr = "--:--";
    if (connectionTimer.isValid()) {
        qint64 elapsedMs = connectionTimer.elapsed();
        int seconds = elapsedMs / 1000;
        int minutes = seconds / 60;
        seconds %= 60;
        durationStr = QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0'));

        QString durationText = QString("⏱️ Bağlantı süresi: %1 dakika %2 saniye")
                                   .arg(minutes).arg(seconds);
        qDebug() << durationText;
        appendToLog(durationText);
    }

    // Soketi kontrol et ve log kaydet
    if (socket) {
        QString deviceName = "Uzak Cihaz";
        QString macAddress = socket->peerAddress().toString();
        logger->logConnectionStatus(deviceName, macAddress, false, -99, durationStr);
    }
    if (disconnectInitiatedByUs) {
        qDebug() << "ℹ️ Bağlantı bizim tarafımızdan sonlandırılmış. Remote disconnect değil.";
        disconnectInitiatedByUs = false; // ✅ Resetle
        return;
    }
    // UI güncelle
    updateConnectionLabel("🔴 Disconnected (Remote)", "red");
    appendToLog("🔌 Uzak cihaz bağlantıyı kesti.");
    ui->btnConnect->setEnabled(true);

    QMetaObject::invokeMethod(this, [this]() {
        ui->lblConnection->setText("🔴 Disconnected (Remote)");
        ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
        ui->txtLog->append("🔌 Uzak cihaz bağlantıyı kesti!");
    });

    // Timer'ı durdur
    connectionDisplayTimer->stop();
    emit connectionDurationUpdated("⏱️ --:--");

    // Socket temizle
    if (socket) {
        socket->close();
        socket->deleteLater();
        socket = nullptr;
    }
}
void ConnectionScreen::onSocketStateChanged(QBluetoothSocket::SocketState state)
{
    qDebug() << "🔄 Socket durumu değişti: " << state;

    switch (state) {
    case QBluetoothSocket::SocketState::ConnectedState:
        qDebug() << "🔗 Socket bağlı durumda.";
        break;

    case QBluetoothSocket::SocketState::UnconnectedState:
        qDebug() << "🔌 Socket bağlantısı kesildi.";

        // UI güncelle - ancak onRemoteDisconnected() zaten çağrılmış olabilir
        if (socket) { // Hala socket varsa UI güncelle
            QMetaObject::invokeMethod(this, [this](){
                ui->lblConnection->setText("🔴 Disconnected (stateChanged)");
                ui->lblConnection->setStyleSheet("color: red; font-weight: bold;");
                ui->txtLog->append("🔌 Bağlantı kesildi (durum değişikliği ile).");
                ui->btnConnect->setEnabled(true);
            }, Qt::QueuedConnection);

            connectionDisplayTimer->stop();
            emit connectionDurationUpdated("⏱️ --:--");

            // Socket temizle
            socket->deleteLater();
            socket = nullptr;
        }
        break;

    default:
        qDebug() << "ℹ️ Socket durumu: " << state;
        break;
    }
}
// connectionscreen.cpp dosyasında bu fonksiyonları tanımlayın:

void ConnectionScreen::updateConnectionLabel(const QString& text, const QString& color)
{
    ui->lblConnection->setText(text);
    ui->lblConnection->setStyleSheet("color: " + color + "; font-weight: bold;");
    qDebug() << "Label güncellendi: " << text;
}

void ConnectionScreen::appendToLog(const QString& message)
{
    ui->txtLog->append(message);
    qDebug() << "Log'a eklendi: " << message;
}
