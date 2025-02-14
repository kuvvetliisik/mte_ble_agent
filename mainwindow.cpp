#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QBluetoothSocket>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothAddress>
#include <QMap>
#include <QBluetoothServiceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QListWidgetItem>

QMap<QString, QString> cihazlar;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    ui->lblConnectionStatus->setText("🔴 Bağlı Değil");//

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    socket = nullptr;
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [this](const QBluetoothDeviceInfo &device) {
        QString cihazAdi = device.name();
        QString cihazAdresi = device.address().toString();

        const auto deviceStr = QString("%1=%2").arg(device.name()).arg(device.address().toString());

        if (!cihazAdi.isEmpty()) {
            ui->comboBluetoothDevices->addItem(deviceStr);
            cihazlar[deviceStr] = cihazAdresi;
        }

        qDebug() << "Bulunan cihaz: " << cihazAdi << " - " << cihazAdresi;
    });


    connect(ui->btnScan, &QPushButton::clicked, this, &MainWindow::updateBluetoothDevices);
    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::connectToDevice);
    connect(ui->btnDisconnect, &QPushButton::clicked, this, &MainWindow::disconnectDevice);
    connect(ui->ClearLogButton, &QPushButton::clicked, this, &MainWindow::clearLog);
}



MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::discoverServices(const QBluetoothAddress &address) {
    auto serviceAgent = new QBluetoothServiceDiscoveryAgent(this);

    connect(serviceAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this, [](const QBluetoothServiceInfo &serviceInfo) {
        qDebug() << serviceInfo.serviceName() << "UUID: " << serviceInfo.serviceUuid().toString();
    });

    connect(serviceAgent, &QBluetoothServiceDiscoveryAgent::finished, this, []() {
    });

    serviceAgent->start();
}

void MainWindow::updateBluetoothDevices() {
    ui->comboBluetoothDevices->clear();
    cihazlar.clear();

    ui->lblConnectionStatus->setText("🔍 Cihazlar Taranıyor...");
    ui->txtLog->append("🔍 Cihazlar taranıyor...");

    discoveryAgent->start();

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [this](const QBluetoothDeviceInfo &device) {
        QString cihazAdi = device.name().trimmed();
        QString macAdresi = device.address().toString().trimmed();

        if (cihazAdi.isEmpty()) {
            if (cihazlar.contains(macAdresi)) {
                cihazAdi = cihazlar[macAdresi];
            } else {
                cihazAdi = "Bilinmeyen Cihaz";
            }
        }

        QString cihazGosterim = QString("%1 [%2]").arg(cihazAdi, macAdresi);

        if (!cihazlar.contains(macAdresi)) {
            ui->comboBluetoothDevices->addItem(cihazGosterim);
            cihazlar[macAdresi] = cihazGosterim;
        }
    });

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, [this]() {
        ui->lblConnectionStatus->setText("✅ Cihaz Taraması Tamamlandı");
        ui->txtLog->append("✅ Cihaz taraması tamamlandı.");
    });
}

void MainWindow::connectToDevice() {
    QString secilenCihaz = ui->comboBluetoothDevices->currentText();

    if (socket && socket->isOpen()) {
        ui->txtLog->append("⚠️ Cihaz zaten bağlı: " + secilenCihaz);
        ui->lblConnectionStatus->setText("✅ Zaten Bağlı: " + secilenCihaz);
        ui->lblConnectionStatus->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        return;
    }

    if (secilenCihaz.isEmpty() || !cihazlar.contains(secilenCihaz)) {
        ui->txtLog->append("Lütfen bir cihaz seçin!");
        return;
    }

    QString cihazAdresi = cihazlar[secilenCihaz];
    QBluetoothAddress bluetoothAdresi(cihazAdresi);
    QBluetoothUuid serviceUuid("0000110a-0000-1000-8000-00805f9b34fb");

    if (socket) {
        if (socket->isOpen()) {
            socket->disconnectFromService();

            QEventLoop loop;
            connect(socket, &QBluetoothSocket::disconnected, &loop, &QEventLoop::quit);
            QTimer::singleShot(3000, &loop, &QEventLoop::quit);
            loop.exec();
        }
        delete socket;
        socket = nullptr;
    }

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    ui->lblConnectionStatus->setText("⏳ Bağlanıyor...");
    ui->lblConnectionStatus->setStyleSheet("color: orange; font-weight: bold; font-size: 18px;");
    ui->txtLog->append("Cihaza bağlanılıyor: " + secilenCihaz);

    connect(socket, &QBluetoothSocket::connected, this, [=]() {
        ui->lblConnectionStatus->setText("✅ Bağlandı: " + secilenCihaz);
        ui->lblConnectionStatus->setStyleSheet("color: green; font-weight: bold; font-size: 18px; ");
        ui->txtLog->append("Cihaza başarıyla bağlanıldı: " + secilenCihaz);

    });

    QTimer::singleShot(5000, this, [=]() {
        if (socket && socket->isOpen()) {
            ui->lblConnectionStatus->setText("✅ Bağlandı: " + secilenCihaz);
            ui->lblConnectionStatus->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
            ui->txtLog->append("Cihaza başarıyla bağlanıldı: " + secilenCihaz);
        }
    });

    connect(socket, &QBluetoothSocket::disconnected, this, [=]() {
        ui->lblConnectionStatus->setText("❌ Bağlantı Kesildi");
        ui->lblConnectionStatus->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
        ui->txtLog->append("Cihaz bağlantısı kesildi.");
    });

    connect(socket, &QBluetoothSocket::errorOccurred, this, [=](QBluetoothSocket::SocketError error) {
        ui->lblConnectionStatus->setText("⚠️ Bağlantı Başarısız");
        ui->lblConnectionStatus->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
        ui->txtLog->append("Bağlantı hatası: " + socket->errorString());
    });

    socket->connectToService(bluetoothAdresi, serviceUuid);
}


void MainWindow::disconnectDevice() {
    if (!socket || !socket->isOpen()) {
        ui->txtLog->append("⚠️ Bağlantı zaten kapalı.");
        return;
    }

    ui->lblConnectionStatus->setText("⏳ Bağlantı Kesiliyor...");
    ui->txtLog->append("Bağlantı kesiliyor...");

    if (socket->isOpen()) {
        socket->disconnectFromService();
        socket->abort();

        QEventLoop loop;
        connect(socket, &QBluetoothSocket::disconnected, &loop, &QEventLoop::quit);
        QTimer::singleShot(3000, &loop, &QEventLoop::quit);
        loop.exec();
    }

    if (cihazlar.contains(ui->comboBluetoothDevices->currentText())) {
        QString macAdres = cihazlar[ui->comboBluetoothDevices->currentText()];
        QString command = "bluetoothctl disconnect " + macAdres;

        QProcess *process = new QProcess(this);
        process->start("bash", QStringList() << "-c" << command);
        process->waitForFinished();
    }

    delete socket;
    socket = nullptr;

    ui->lblConnectionStatus->setText("❌ Bağlantı Kesildi");
    ui->txtLog->append("Bluetooth bağlantısı tamamen kapatıldı.");
}

void MainWindow::clearLog() {
    ui->txtLog->clear();
}



void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if (item->text() == "Bağlantı Ekranı") {
        ui->stackedWidget->setCurrentWidget(ui->page_Baglanti);
    } else if (item->text() == "Cihaz Bilgisi") {
        ui->stackedWidget->setCurrentWidget(ui->page_cihaz);
    }
}

