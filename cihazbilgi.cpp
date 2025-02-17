#include "cihazbilgi.h"
#include "ui_cihazbilgi.h"

CihazBilgi::CihazBilgi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CihazBilgi)
{
    ui->setupUi(this);
}

CihazBilgi::~CihazBilgi()
{
    delete ui;
}

void CihazBilgi::updateDeviceInfo(const QString &name, const QString &address) {
    ui->labelDeviceName->setText("Cihaz Adı: " + name);
    ui->labelDeviceAddress->setText("MAC Adresi: " + address);
}

void CihazBilgi::appendToLog(const QString &message) {
    ui->textEditLogs->append(QTime::currentTime().toString("[HH:mm:ss] ") + message);
}
