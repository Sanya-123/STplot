#include "fileprogressdialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileProgressDialog w;
    return a.exec();
}
