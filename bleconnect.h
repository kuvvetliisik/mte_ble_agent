#ifndef BLECONNECT_H
#define BLECONNECT_H

#include <QWidget>

namespace Ui {
class BleConnect;
}

class BleConnect : public QWidget
{
    Q_OBJECT

public:
    explicit BleConnect(QWidget *parent = nullptr);
    ~BleConnect();

private:
    Ui::BleConnect *ui;
};

#endif // BLECONNECT_H
