#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include <QWidget>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QMap>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {class ConnectionScreen;}
QT_END_NAMESPACE

class ConnectionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionScreen(QWidget *parent = nullptr);
    void setScanButtonEnabled(bool enabled);
    void clearDeviceList();
    void setConnectionLabel(const QString &text);
    ~ConnectionScreen();

signals:
    void deviceConnected(QString deviceName, QString macAddress, int rssi, QString bluetoothversion);

private slots:
    void updateBluetoothDevices();
    void connectToDevice();
    void disconnectDevice();
    void clearLog();
    void handleDisconnected();



private:
    Ui::ConnectionScreen *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    void discoverDeviceServices(const QBluetoothAddress &address);
    QBluetoothSocket *socket;
    QMap<QString, QString> devicess;
    QMap<QString, int> rssiValues;
    QTimer *connectionCheckTimer;

};

#endif
