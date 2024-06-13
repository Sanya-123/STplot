#ifndef FILEPROGRESSDIALOG_H
#define FILEPROGRESSDIALOG_H

#include <QDialog>
#include "../fileprogresssharememory.h"
#include <QTimer>
#include <QProgressDialog>


class FileProgressDialog : public QObject
{
    Q_OBJECT

public:
    FileProgressDialog(QObject *parent = nullptr);
    ~FileProgressDialog();

private slots:
    void checkMemory();
    void cancelRead();

private:
    FileProgressShareMemory shareMemory;
    QTimer timeCheckMemory;
    QProgressDialog progressDialog;
};
#endif // FILEPROGRESSDIALOG_H
