#ifndef DEVICEINFO_H
#define DEVICEINFO_H


#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceInfo; }
QT_END_NAMESPACE

class DeviceInfo : public QWidget {
    Q_OBJECT

public:
    explicit DeviceInfo(QWidget *parent = nullptr);
    Ui::DeviceInfo *ui;
      void setConnectionStatusLabel(bool connected);
    ~DeviceInfo();
    void updateDeviceInfo(QString deviceName, QString macAddress, int rssi);
public slots:
    void updateLiveConnectionDuration(const QString &duration);

signals:
        void refreshConnectionRequested();
private:
     //Ui::DeviceInfo *ui;

};

#endif
