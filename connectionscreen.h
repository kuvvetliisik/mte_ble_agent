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
    void connectToDevice();
    void disconnectDevice();
    static QString getBluetoothVersionFromHciconfig();
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
    //void handleDisconnected();
    //void emitDisconnectionSignal();
    //void resetUIAfterDisconnection();
    //void terminateSocketConnection();
    //void readSocketData();
    //void updateLiveRSSI();
    //void onSocketConnected();
    //void startLiveRSSIMonitor(const QString& mac, const QString& name, const QString& btver);
    double calculateDistance(int measuredPower, int rssi, double N = 2.0);
    double guessNFromRSSI(int rssi);
    //void startRSSIMonitoring(const QString& macAddress);
    //void refreshConnection();

private:
    Ui::ConnectionScreen *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    void discoverDeviceServices(const QBluetoothAddress &address);
    QBluetoothSocket *socket;
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
