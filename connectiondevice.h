#ifndef CONNECTIONDEVICE_H
#define CONNECTIONDEVICE_H

#include <QWidget>

namespace Ui {
class connectiondevice;
}

class connectiondevice : public QWidget
{
    Q_OBJECT

public:
    explicit connectiondevice(QWidget *parent = nullptr);
    ~connectiondevice();

private:
    Ui::connectiondevice *ui;
};

#endif // CONNECTIONDEVICE_H
