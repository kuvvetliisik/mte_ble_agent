#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QWidget>
#include <QBluetoothSocket>

namespace Ui {
class FileTransfer;
}

class FileTransfer : public QWidget
{
    Q_OBJECT

public:
    explicit FileTransfer(QWidget *parent = nullptr);
    void setSocket(QBluetoothSocket* btSocket);

    ~FileTransfer();
private slots :
    void on_btnSelectFile_clicked();
    void on_btnSendFile_clicked();
private:
    QString selectedFilePath;
    QBluetoothSocket* socket =nullptr;
    QBluetoothSocket* btSocket;
    void clearLog_2();
    void sendFile();

    Ui::FileTransfer *ui;
};

#endif // FILETRANSFER_H
