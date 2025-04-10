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
#include <QElapsedTimer>
#include "logmanager.h"
#include "QBluetoothServer"


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
    void clientConnected();
    void onRemoteDisconnected();
    void onSocketStateChanged(QBluetoothSocket::SocketState state);

    static QString getBluetoothVersionFromHciconfig();
    QBluetoothSocket *socket;

    ~ConnectionScreen();

signals:
    void deviceConnected(QString deviceName, QString macAddress, int rssi);
    void deviceDisconnected();
    void dataReceivedFromDevice(const QString &message);
    void deviceNameReceived(const QString &deviceName);
    void connectionDurationUpdated(const QString &duration);


private slots:
    void updateBluetoothDevices();
    void clearLog();
    double calculateDistance(int measuredPower, int rssi, double N = 2.0);
    double guessNFromRSSI(int rssi);
    //void handleDisconnection();
public slots:
    void updateConnectionLabel(const QString& text, const QString& color);
    void appendToLog(const QString& message);

private:
    Ui::ConnectionScreen *ui;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    void discoverDeviceServices(const QBluetoothAddress &address);
    QMap<QString, QString> devicess;
    QMap<QString, int> rssiValues;
    QTimer *connectionCheckTimer;
    //QTimer* rssiMonitorTimer;
    bool isDisconnectedByUser;
    bool prepareForConnection(QString &macAddress, QString &deviceName);
    QElapsedTimer connectionTimer;
    QTimer *connectionDisplayTimer;
    LogManager *logger;
    QBluetoothServiceInfo serviceInfo;
    QBluetoothServer *rfcommServer;
    bool disconnectInitiatedByUs = false;


};

#endif
