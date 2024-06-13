#include "fileprogressdialog.h"
#include <QDebug>

FileProgressDialog::FileProgressDialog(QObject *parent)
    : QObject(parent)
{
    connect(&timeCheckMemory, SIGNAL(timeout()), this, SLOT(checkMemory()));
    timeCheckMemory.start(100);
    connect(&progressDialog, SIGNAL(canceled()), this, SLOT(cancelRead()));
    progressDialog.setWindowTitle("Loading file...");
    progressDialog.show();
}

FileProgressDialog::~FileProgressDialog()
{
    progressDialog.cancel();
}

void FileProgressDialog::checkMemory()
{
    static QString oldMsg = "";
    QString msg = shareMemory.getMessadge();
    float procent = shareMemory.getProcent();
    if(procent > 100)
        procent = 100;
    progressDialog.setLabelText(msg);
    progressDialog.setValue(procent);
    if(oldMsg != msg)
        qDebug() << msg;

    oldMsg = msg;
}

void FileProgressDialog::cancelRead()
{
    shareMemory.setCancelFlag(true);
}

