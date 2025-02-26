#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include <QWidget>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {class ConnectionScreen;}
QT_END_NAMESPACE

class ConnectionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionScreen(QWidget *parent = nullptr);
    ~ConnectionScreen();

signals:
    void deviceConnected(QString deviceName, QString macAddress, int rssi, QString bluetoothversion);

private slots:
    void updateBluetoothDevices();
    void connectToDevice();
    void disconnectDevice();
    void clearLog();

private:
    Ui::ConnectionScreen *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    //void discoverDeviceServices(const QBluetoothAddress &address);
    QBluetoothSocket *socket;
    QMap<QString, QString> devicess;
    QMap<QString, int> rssiValues;

};

#endif
