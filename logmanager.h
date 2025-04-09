#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDateTime>


class LogManager : public QObject
{
    Q_OBJECT
public:
    explicit LogManager(QObject *parent = nullptr);

    void logConnectionStatus(const QString &deviceName,
                             const QString &mac,
                             bool connected,
                             int rssi = 0,
                             const QString &duration = "");

private:
    QString logFilePath = "bt_log.json";
    void writeJson(const QJsonObject &obj);
    QList<QJsonObject> readAllLogs();

};

#endif // LOGMANAGER_H
