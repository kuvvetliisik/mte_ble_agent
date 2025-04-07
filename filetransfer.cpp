#include "filetransfer.h"
#include "ui_filetransfer.h"

FileTransfer::FileTransfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileTransfer)
{
    ui->setupUi(this);
}

FileTransfer::~FileTransfer()
{
    delete ui;
}
