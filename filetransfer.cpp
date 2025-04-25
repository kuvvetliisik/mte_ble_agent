#include "filetransfer.h"
#include "ui_filetransfer.h"
#include <QFileDialog>
#include <QFile>
#include <QBluetoothSocket>
#include <QFileInfo>
#include <QTextCursor>
#include <QTimer>


FileTransfer::FileTransfer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::FileTransfer)
{
    ui->setupUi(this);

    connect(ui->btnSelectFile, &QPushButton::clicked,
            this, &FileTransfer::on_btnSelectFile_clicked);
    connect(ui->btnSendFile, &QPushButton::clicked,
            this, &FileTransfer::on_btnSendFile_clicked);
    connect(ui->btnClearLog, &QPushButton::clicked,
            this, &FileTransfer::clearLog_2);

    ui->plainTextEdit->appendPlainText("🔧 FileTransfer initialized.");
}

FileTransfer::~FileTransfer() {
    delete ui;
}

void FileTransfer::on_btnSelectFile_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Dosya Seç", QDir::homePath());
    if (path.isEmpty()) return;
    selectedFilePath = path;
    ui->lblFileName->setText(QFileInfo(path).fileName());
    ui->plainTextEdit->appendPlainText("📄 Dosya seçildi: " + path);
}

void FileTransfer::on_btnSendFile_clicked() {
    if (selectedFilePath.isEmpty()) {
        ui->plainTextEdit->appendPlainText("⚠️ Lütfen önce dosya seçin!");
        return;
    }

    ui->plainTextEdit->appendPlainText("🚀 Gönderim başlatılıyor...");
    if (!socket) {
        ui->plainTextEdit->appendPlainText("❌ Bluetooth soketi yok.");
        return;
    }
    if (socket->state() != QBluetoothSocket::SocketState::ConnectedState) {
        ui->plainTextEdit->appendPlainText("⌛ Bağlantı bekleniyor...");
        connect(socket, &QBluetoothSocket::connected,
                this, &FileTransfer::sendFile, Qt::UniqueConnection);
        return;
    }
    qDebug() << "qDebug çağrıldı";
        sendFile();
}

void FileTransfer::sendFile() {

    ui->plainTextEdit->appendPlainText("📤 Gönderiliyor: " + QFileInfo(selectedFilePath).fileName());
    QFile file(selectedFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->plainTextEdit->appendPlainText("❌ Dosya açılamadı.");
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    socket->write(data);
    if (!socket->waitForBytesWritten(5000)) {
        ui->plainTextEdit->appendPlainText("⚠️ Zaman aşımı: " + socket->errorString());
        return;
    }
    ui->plainTextEdit->appendPlainText("✅ Dosya başarıyla gönderildi.");
}

void FileTransfer::clearLog_2() {
    ui->plainTextEdit->clear();
}

void FileTransfer::setSocket(QBluetoothSocket* btSocket) {
    if (socket && socket != btSocket) {
        socket->disconnect(this); // Eski bağlantıları kopart
    }
    socket = btSocket;
    if (socket)
        ui->plainTextEdit->appendPlainText("🔌 Yeni soket atandı.");
}


