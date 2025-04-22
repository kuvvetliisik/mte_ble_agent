#include "filetransfer.h"
#include "ui_filetransfer.h"
#include <QFileDialog>
#include <QFile>
#include <QBluetoothSocket>
#include <QFileInfo>
#include <QTextCursor>
#include <QTimer>


FileTransfer::FileTransfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileTransfer)
{
    ui->setupUi(this);
}

void FileTransfer::setSocket(QBluetoothSocket* btSocket) {
    qDebug() << "📲 FileTransfer::setSocket çalıştı. Gelen socket: " << btSocket;
    this->socket = btSocket;
    qDebug() << "📡 Artık kullanılacak socket: " << this->socket;

}
void FileTransfer::on_btnSelectFile_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Dosya Seç", QDir::homePath());

    if (!filePath.isEmpty()) {
        selectedFilePath = filePath;
        ui->lblFileName->setText("Seçilen Dosya: " + QFileInfo(filePath).fileName());
        ui->plainTextEdit->appendPlainText("📄 Dosya seçildi: " + filePath);
    }

}
void FileTransfer::on_btnSendFile_clicked()
{
    if (!socket) {
        ui->plainTextEdit->appendPlainText("❌ Bluetooth bağlantısı yok.");
        return;
    }

    if (socket->state() == QBluetoothSocket::SocketState::ConnectedState) {
        sendFile();
        return;
    }

    ui->plainTextEdit->appendPlainText("⌛ Bağlantının tamamlanması bekleniyor...");

    auto finalizeSend = [=]() {
        if (socket && socket->state() == QBluetoothSocket::SocketState::ConnectedState) {
            ui->plainTextEdit->appendPlainText("✅ Bağlantı hazır, dosya gönderiliyor...");
            sendFile();
        } else {
            ui->plainTextEdit->appendPlainText("❌ Bağlantı hala hazır değil, gönderim iptal.");
        }
    };

    static bool alreadyConnected = false;
    if (!alreadyConnected) {
        connect(socket, &QBluetoothSocket::connected, this, finalizeSend);
        alreadyConnected = true;
    }
}


void FileTransfer::sendFile()
{
    if (!socket || socket->state() != QBluetoothSocket::SocketState::ConnectedState) {
        ui->plainTextEdit->appendPlainText("❌ Bluetooth bağlı değil, gönderim iptal.");
        return;
    }

    if (selectedFilePath.isEmpty()) {
        ui->plainTextEdit->appendPlainText("⚠️ Lütfen önce bir dosya seçin.");
        return;
    }

    QFile file(selectedFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->plainTextEdit->appendPlainText("❌ Dosya açılamadı.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    socket->write(fileData);
    ui->plainTextEdit->appendPlainText("📤 Dosya gönderildi: " + QFileInfo(selectedFilePath).fileName());
    ui->progressBar->setValue(100);
}

FileTransfer::~FileTransfer()
{
    delete ui;
}
