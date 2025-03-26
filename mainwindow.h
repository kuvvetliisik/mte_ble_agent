#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionscreen.h"
#include "deviceinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showDeviceInfo();
    void showConnectionScreen();
    void toggleBluetooth();
    void checkBluetoothStatus();
    void handleDeviceConnected(QString deviceName, QString macAddress, int rssi, QString bluetoothversion);
    void handleDeviceData(const QString &message);
    void handleDeviceName(const QString &deviceName);

private:
    Ui::MainWindow *ui;
    DeviceInfo *deviceInfo;
    ConnectionScreen *connectionScreen;
};

#endif
