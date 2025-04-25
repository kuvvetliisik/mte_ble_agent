#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionscreen.h"
#include "deviceinfo.h"
#include "filetransfer.h"


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
    void handleDeviceConnected(QString deviceName, QString macAddress, int rssi);
    //void handleDeviceData(const QString &message);
    //void handleDeviceName(const QString &deviceName);
    // void refreshConnectionFromDeviceInfo();
    void refreshConnectionFromDeviceInfo();
    //void setDeviceInfoConnectionStatus(bool connected);
    void showFileTransfer();
    void handleBluetoothConnected(QBluetoothSocket* socket);


private:
    Ui::MainWindow *ui;
    DeviceInfo *deviceInfo;
    ConnectionScreen *connectionScreen;
    FileTransfer *fileTransfer;
};

#endif
