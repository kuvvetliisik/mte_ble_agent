#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include <QWidget>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QMap>
#include <QTimer>
#include <functional>

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
    void connectToDevice();
    void disconnectDevice();
    void setConnectionLabelText(const QString& text, const QString& color = "orange");
    void updateConnectionStatusLabel(bool connected);



    static QString getBluetoothVersionFromHciconfig();
    QBluetoothSocket *socket;
    ~ConnectionScreen();

signals:
    void deviceConnected(QString deviceName, QString macAddress, int rssi, QString bluetoothversion);
    void deviceDisconnected();
    void dataReceivedFromDevice(const QString &message);
    void deviceNameReceived(const QString &deviceName);
//    void bluetoothDisconnected();

private slots:
    void updateBluetoothDevices();
    void clearLog();
    double calculateDistance(int measuredPower, int rssi, double N = 2.0);
    double guessNFromRSSI(int rssi);
    //void startRSSIMonitoring(const QString& macAddress);
    //void refreshConnection();
    //void  refreshConnection();
private:
    Ui::ConnectionScreen *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    void discoverDeviceServices(const QBluetoothAddress &address);
    QMap<QString, QString> devicess;
    QMap<QString, int> rssiValues;
    QTimer *connectionCheckTimer;
    //QTimer* rssiMonitorTimer;
    //QString currentConnectedMac;
    //QString currentConnectedName;
    //QString currentBluetoothVersion;
    bool isDisconnectedByUser;
    bool prepareForConnection(QString &macAddress, QString &deviceName);
    //void initSocket(const QBluetoothAddress &bluetoothAddress, const QBluetoothUuid &uuid);
    //void setupSocketSignals(const QString &deviceName, const QString &macAddress, const QString &bluetoothVersion);
    //void updateUIOnConnectionSuccess(const QString &deviceName);
    //void sendDisconnectCommandToSystem(const QString &macAddress);
    //QTimer *rssiMonitorTimer = nullptr;
    //QProcess *rssiProcess = nullptr;

};

#endif
