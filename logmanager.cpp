#include "logmanager.h"
#include <QJsonArray>


LogManager::LogManager(QObject *parent) : QObject(parent) {}

void LogManager::logConnectionStatus(const QString &deviceName,
                                     const QString &mac,
                                     bool connected,
                                     int rssi,
                                     const QString &duration)
{
    QJsonObject obj;
    obj["type"] = "connection";
    obj["device"] = deviceName;
    obj["mac"] = mac;
    obj["status"] = connected ? "connected" : "disconnected";
    obj["rssi"] = rssi;
    if (!duration.isEmpty())
        obj["duration"] = duration;
    obj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    writeJson(obj);
}

void LogManager::writeJson(const QJsonObject &obj)
{
    QFile file(logFilePath);
    QJsonArray logArray;

    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray allData = file.readAll();
        file.close();

        QJsonDocument existingDoc = QJsonDocument::fromJson(allData);
        if (existingDoc.isArray()) {
            logArray = existingDoc.array();
        }
    }

    logArray.append(obj);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument newDoc(logArray);
        file.write(newDoc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

/*QList<QJsonObject> LogManager::readAllLogs()
{
    QList<QJsonObject> logs;
    QFile file(logFilePath); // default: "bt_log.json"

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ bt_log.json dosyası açılamadı!";
        return logs;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine().trimmed();
        if (line.isEmpty()) continue;

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(line, &error);

        if (error.error != QJsonParseError::NoError) {
            qWarning() << "⚠️ JSON Parse Hatası:" << error.errorString();
            continue;
        }

        logs.append(doc.object()); // her satır bir QJsonObject
    }

    file.close();
    return logs;
}
*/
