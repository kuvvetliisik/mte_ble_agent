#include "bleconnect.h"
#include "ui_bleconnect.h"

BleConnect::BleConnect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BleConnect)
{
    ui->setupUi(this);
}

BleConnect::~BleConnect()
{
    delete ui;
}
