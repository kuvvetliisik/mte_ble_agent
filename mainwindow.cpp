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

    //connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    qDebug() << "connectionScreen nesnesi: " << connectionScreen;
    qDebug() << "deviceInfo nesnesi: " << deviceInfo;
    bool success = connect(connectionScreen, &ConnectionScreen::deviceConnected, this, &MainWindow::handleDeviceConnected);
    qDebug() << (success ? "✅ Signal-Slot bağlantısı başarılı!" : "❌ Signal-Slot bağlantısı başarısız!");
    //checkBluetoothStatus();
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
/*void MainWindow::handleDeviceData(const QString &message)
{
    if (deviceInfo && deviceInfo->ui) {
        deviceInfo->ui->lblReceivedData->setText(message);
    }
}

void MainWindow::handleDeviceName(const QString &name)
{
    //ui->statusBar->showMessage("Cihaz Adı Alındı: " + name);
    deviceInfo->ui->lblDeviceName->setText("Device Name: " + name);
}
*/

/*void MainWindow::toggleBluetooth() {
    static bool isBluetoothOn = false;

    QProcess process;
    QString command;

    if (!isBluetoothOn) {
        qDebug() << "🔹 Bluetooth Açılıyor...";

        command = "sudo rfkill unblock bluetooth";
        process.start(command);
        process.waitForFinished();
        qDebug() << "rfkill unblock stdout: " << process.readAllStandardOutput();
        qDebug() << "rfkill unblock stderr: " << process.readAllStandardError();

        command = "sudo systemctl restart bluetooth.service";
        process.start(command);
        process.waitForFinished();
        qDebug() << "systemctl restart stdout: " << process.readAllStandardOutput();
        qDebug() << "systemctl restart stderr: " << process.readAllStandardError();

        command = "bluetoothctl power on";
        process.start(command);
        process.waitForFinished();
        qDebug() << "bluetoothctl power on stdout: " << process.readAllStandardOutput();
        qDebug() << "bluetoothctl power on stderr: " << process.readAllStandardError();

        command = "sudo hciconfig hci0 up";
        process.start(command);
        process.waitForFinished();
        qDebug() << "hciconfig hci0 up stdout: " << process.readAllStandardOutput();
        qDebug() << "hciconfig hci0 up stderr: " << process.readAllStandardError();

        ui->btnBluetooth->setText("Bluetooth Kapat");
    } else {
        qDebug() << "🔻 Bluetooth Kapatılıyor...";

        command = "sudo rfkill block bluetooth";
        process.start(command);
        process.waitForFinished();
        qDebug() << "rfkill block stdout: " << process.readAllStandardOutput();
        qDebug() << "rfkill block stderr: " << process.readAllStandardError();

        command = "bluetoothctl power off";
        process.start(command);
        process.waitForFinished();
        qDebug() << "bluetoothctl power off stdout: " << process.readAllStandardOutput();
        qDebug() << "bluetoothctl power off stderr: " << process.readAllStandardError();

        command = "sudo hciconfig hci0 down";
        process.start(command);
        process.waitForFinished();
        qDebug() << "hciconfig hci0 down stdout: " << process.readAllStandardOutput();
        qDebug() << "hciconfig hci0 down stderr: " << process.readAllStandardError();

        command = "sudo systemctl stop bluetooth.service";
        process.start(command);
        process.waitForFinished();
        qDebug() << "systemctl stop stdout: " << process.readAllStandardOutput();
        qDebug() << "systemctl stop stderr: " << process.readAllStandardError();

        ui->btnBluetooth->setText("Bluetooth Aç");
    }

    isBluetoothOn = !isBluetoothOn;
}

void MainWindow::checkBluetoothStatus() {
    QProcess process;
    process.start("rfkill list bluetooth");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    if (output.contains("Soft blocked: no")) {
        ui->btnBluetooth->setText("Bluetooth Kapat");
    } else {
        ui->btnBluetooth->setText("Bluetooth Aç");
    }
}
*/
