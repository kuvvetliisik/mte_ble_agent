#include "connectiondevice.h"
#include "ui_connectiondevice.h"

connectiondevice::connectiondevice(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::connectiondevice)
{
    ui->setupUi(this);
}

connectiondevice::~connectiondevice()
{
    delete ui;
}
