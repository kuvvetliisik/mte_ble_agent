#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QWidget>

namespace Ui {
class FileTransfer;
}

class FileTransfer : public QWidget
{
    Q_OBJECT

public:
    explicit FileTransfer(QWidget *parent = nullptr);
    ~FileTransfer();

private:
    Ui::FileTransfer *ui;
};

#endif // FILETRANSFER_H
