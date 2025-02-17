#ifndef CİHAZBİLGİ_H
#define CİHAZBİLGİ_H

#include <QWidget>

namespace Ui {
class CihazBilgi;
}

class CihazBilgi : public QWidget
{
    Q_OBJECT

public:
    explicit CihazBilgi(QWidget *parent = nullptr);
    ~CihazBilgi();

    void updateDeviceInfo(const QString &name, const QString &address);
    void appendToLog(const QString &message);

private:
    Ui::CihazBilgi *ui;
};

#endif // CİHAZBİLGİ_H
